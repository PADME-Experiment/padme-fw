// Written by M. Raggi   20/08/2021 
// Last modified by M. Raggi 2/08/2021
#include "Is3GAnalysis.hh"

Is3GAnalysis::Is3GAnalysis(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating Is3GAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

Is3GAnalysis::~Is3GAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t Is3GAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing Is3GAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t Is3GAnalysis::InitHistos(){
  // Is3GAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("3GAnalysis");

  fHS->BookHistoList("3GAnalysis","NClusters",25,-0.5,24.5);
  fHS->BookHistoList("3GAnalysis","ETotECal",500,0.,1000.);
  fHS->BookHistoList("3GAnalysis","ECalClEnergy",500,0.,500.);
  fHS->BookHistoList("3GAnalysis","ECalClTime",500,-250.,250.);
  fHS->BookHistoList("3GAnalysis","ClusterRadius",200,0.,400.);
  fHS->BookHisto2List("3GAnalysis","EnergyMap",150,-300.,300.,150,-300.,300.);
  fHS->BookHistoList("3GAnalysis","TClTimeDiff3g",100,25.,25.);

  fHS->BookHistoList("3GAnalysis","NClusters_AfterPresel",25,-0.5,24.5);

  fHS->BookHistoList("3GAnalysis","ECalClEnergy3g",500,0.,500.);

  fHS->BookHisto2List("3GAnalysis","EnvsTimeDiff",300,-150.,150.,125,0.,500.);
  fHS->BookHisto2List("3GAnalysis","EnvsTime",100,-150.,150.,125,0.,500.);

  fHS->BookHistoList("3GAnalysis","NPairs",25,-0.5,24.5);
  fHS->BookHistoList("3GAnalysis","ECalClEnergy3g_Intime",500,0.,500.);  
  fHS->BookHistoList("3GAnalysis","TCluDiff3g_Intime",200,20.,20.);  

  fHS->BookHistoList("3GAnalysis","ECalClEnergy3g_Intime_COG",500,0.,500.);  
  fHS->BookHistoList("3GAnalysis","ECalClEnergy3g_Intime_BestPair",500,0.,500.); 

  fHS->BookHistoList("3GAnalysis","COG_X",300,-150.,150.);   
  fHS->BookHistoList("3GAnalysis","COG_Y",300,-150.,150.);   
  fHS->BookHisto2List("3GAnalysis","COG_Map",300,-150.,150.,300,-150.,150.);   

  //3g analysis
  fHS->BookHistoList("3GAnalysis","ECalClEnergy3g_Intime",500,0.,500.); 
  fHS->BookHisto2List("3GAnalysis","EnvsTimeDiff_3g",300,-150.,150.,125,0.,500.);
  fHS->BookHisto2List("3GAnalysis","Chi2vsETot",100,0.,20.,125,0.,500.);
 
  return true;
}

Bool_t Is3GAnalysis::Process(){

  std::vector<double> EClusterPair;
  std::vector<double> TClusterPair;
  std::vector<double> PosXClusterPair;
  std::vector<double> PosYClusterPair;

  std::vector<double> Ei;
  std::vector<double> PosX;
  std::vector<double> PosY;

  std::vector<double> EGoodCluster;
  std::vector<double> TGoodCluster;
  std::vector<double> PosXGoodCluster;
  std::vector<double> PosYGoodCluster;

  std::vector<double> ETotPair;  
  std::vector<double> TDiffPair; 
  std::vector<double> ETotTris;    
  
  // Standard cuts list
  static const Double_t MinECluster=50.;
  static const Double_t TMin=-170.;
  static const Double_t TMax= 130.;
  static const Double_t TWin= 5.;
  static const Double_t ClRadMin= 100.;
  static const Double_t ClRadMax= 250.;
  static const Double_t COGMax  = 25.;

  //Check if is MC or data
  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();

  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && isMC==false) return false;

  //Calo-Calo checks
  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  ETotECal=0;
  for(int ical = 0;ical < NClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal    =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos1   =  fEvent->ECalRecoCl->Element(ical)->GetPosition();
    double ClRadius = sqrt(pos1.X()*pos1.X()+pos1.Y()*pos1.Y());
    ETotECal+=eECal;
    fHS->FillHistoList("3GAnalysis","ETotECal",ETotECal,1);
    
    fHS->FillHistoList("3GAnalysis","NClusters",NClusters,1);
    fHS->FillHistoList("3GAnalysis","ECalClEnergy",eECal,1);
    fHS->FillHistoList("3GAnalysis","ECalClTime",tECal,1);
    fHS->FillHisto2List("3GAnalysis","EnergyMap",pos1.X(),pos1.Y(),eECal);
    fHS->FillHistoList("3GAnalysis","ClusterRadius",ClRadius,1);
    
    //Data cut on cluster energy Need the reco to be calibrated
    if(!isMC && eECal*1.131<MinECluster) continue; 
    //MC on cluster energy is calibrated
    if(isMC && eECal<MinECluster) continue;

    //Cut on cluster Time Data only
    if(tECal>TMax && isMC==false) continue;
    if(tECal<TMin && isMC==false) continue;

    //Cut on cluster radius Min and Max
    if(ClRadius<ClRadMin) continue;
    if(ClRadius>ClRadMax) continue;

    EGoodCluster.push_back(eECal);
    TGoodCluster.push_back(tECal);
    PosXGoodCluster.push_back(pos1.X());
    PosYGoodCluster.push_back(pos1.Y());
  }

//  //cut at at least two clusters
  if(EGoodCluster.size()<3) return false;

  fHS->FillHistoList("3GAnalysis","NClusters_AfterPresel",NClusters,1);

// Search for in time cluster pairs 
  Int_t NGoodClusters=(Int_t) EGoodCluster.size();
  Int_t NPairs = 0;

  //BUG on dimensions of the arrays
  for(Int_t kk=0;kk<NGoodClusters;kk++){
    for(Int_t jj=kk+1;jj<NGoodClusters;jj++){
      for(Int_t ll=jj+1;ll<NGoodClusters;ll++){	
	fHS->FillHistoList("3GAnalysis","TClTimeDiff3g",TGoodCluster[kk]-TGoodCluster[jj]-TGoodCluster[ll],1);
	fHS->FillHistoList("3GAnalysis","ECalClEnergy3g",EGoodCluster[kk]+EGoodCluster[jj]+EGoodCluster[ll],1);
	
	if(! (fabs(TGoodCluster[kk]-TGoodCluster[jj]-TGoodCluster[ll])<TWin) ) continue;
	fHS->FillHistoList("3GAnalysis","ECalClEnergy3g_Intime",EGoodCluster[kk]+EGoodCluster[jj]+EGoodCluster[ll],1);
	fHS->FillHistoList("3GAnalysis","TCluDiff3g_Intime",TGoodCluster[kk]-TGoodCluster[jj],1);
	fHS->FillHisto2List("3GAnalysis","EnvsTimeDiff_3g",TGoodCluster[kk]-TGoodCluster[jj],EGoodCluster[kk]+EGoodCluster[jj],1);
	
	//Compute COG of the identified pair
	Ei.push_back(EGoodCluster[kk]);  PosX.push_back(PosXGoodCluster[kk]); PosY.push_back(PosYGoodCluster[kk]);
	Ei.push_back(EGoodCluster[jj]);  PosX.push_back(PosXGoodCluster[jj]); PosY.push_back(PosYGoodCluster[jj]);
	Ei.push_back(EGoodCluster[ll]);  PosX.push_back(PosXGoodCluster[ll]); PosY.push_back(PosYGoodCluster[ll]);
	Double_t COGX = GetCOG(Ei,PosX); Double_t COGY = GetCOG(Ei,PosY);
	fHS->FillHistoList("3GAnalysis","COG_X",COGX,1);
	fHS->FillHistoList("3GAnalysis","COG_Y",COGY,1);
	fHS->FillHisto2List("3GAnalysis","COG_Map",COGX,COGY,1);
	
	//Cut on pair COG
	if(isMC){
	  if(abs(COGX+3.88) > COGMax || abs(COGY+3.)>COGMax) continue; //Problems in MC rilasso il CUT
	}else{ //COG in data is fine
	  if(abs(COGX) > COGMax || abs(COGY)>COGMax) continue; 
	}
	//      EClusterPair.push_back(EGoodCluster[kk]);
	//      TClusterPair.push_back(TGoodCluster[kk]);
	//      PosXClusterPair.push_back(PosXGoodCluster[kk]);      
	//      PosYClusterPair.push_back(PosYGoodCluster[kk]);
	//      
	//      EClusterPair.push_back(EGoodCluster[jj]);
	//      TClusterPair.push_back(TGoodCluster[jj]);
	//      PosXClusterPair.push_back(PosXGoodCluster[jj]);
	//      PosYClusterPair.push_back(PosYGoodCluster[jj]);
	
	NPairs++;
	ETotPair.push_back(EGoodCluster[kk]+EGoodCluster[jj]+EGoodCluster[ll]);
	//	TDiffPair.push_back(TGoodCluster[kk]+TGoodCluster[jj]+TGoodCluster[jj]);
	//clear vectors
	Ei.clear();
	PosX.clear();
	PosY.clear();
	break;
      }
      break;
    }
    break;
  }
  //cut on at least 1 pair
  if(NPairs<1) return false;
  fHS->FillHistoList("3GAnalysis","NPairs",NPairs,1);
  double chi2 = 0;
  for(Int_t ll=0;ll<NPairs;ll++){
    if(NPairs==1) fHS->FillHistoList("3GAnalysis","ECalClEnergy3g_Intime_COG",ETotPair[ll],1);
    //    chi2 = fabs( (ETotPair[ll]-430)/14 ) + fabs(TDiffPair[ll]/1.2);
    fHS->FillHisto2List("3GAnalysis","Chi2vsETot",chi2,ETotPair[ll],1.);
    //    fHS->FillHisto2List("3GAnalysis","EnvsTime",(TClusterPair[0]+TClusterPair[1])/2,ETotPair[ll],1.);
  }
  N3G++;

  //Cleaning memory
  ETotPair.clear();  
  TDiffPair.clear(); 
  ETotTris.clear();  

  EClusterPair.clear();
  TClusterPair.clear();
  PosXClusterPair.clear();
  PosYClusterPair.clear();

  EGoodCluster.clear();
  TGoodCluster.clear();
  PosXGoodCluster.clear();
  PosYGoodCluster.clear();

  return true;
}

Bool_t Is3GAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
  std::cout<<"TotGG        = "<<N3G<<std::endl;
  if (fVerbose) printf("---> Finalizing IsGGAnalysis\n");
  return true;
}


double Is3GAnalysis::GetCOG(std::vector<double> Ei,std::vector<double> Posi)
{
  //  std::cout<<"Size of vectors "<<Ei.size()<<" "<<Posi.size()<<" "<<Ei[0]<<" "<<Posi[0]<<std::endl;
  Double_t ETot=0;
  Double_t COG=0;
  for(Int_t i=0;i<(Int_t)Ei.size();i++){
    ETot += Ei[i];
    COG  +=  Ei[i]*Posi[i];
  }
  COG=COG/ETot;
  return COG;
}
