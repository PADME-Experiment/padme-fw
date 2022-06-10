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

  // Standard cuts list
  MinECluster=50.;
  TMin=-170.;
  TMax= 130.;
  TWin= 4.;
  ClRadMin= 100.;
  ClRadMax= 250.;
  COGMax  = 45.;
  DistMax = 150.;
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
  static const Double_t hMaxE  = 800.;
  static const Double_t hBinE  = 400.;

  // Is3GAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("GGGAnalysis");

  fHS->BookHistoList("GGGAnalysis","NClusters",25,-0.5,24.5);
  fHS->BookHistoList("GGGAnalysis","ETotECal",500,0.,1000.);
  fHS->BookHistoList("GGGAnalysis","ECalClTime"  ,500,-250.,250.);
  fHS->BookHistoList("GGGAnalysis","ClusterRadius",200,0.,400.);
  fHS->BookHisto2List("GGGAnalysis","EnergyMap",150,-300.,300.,150,-300.,300.);
  fHS->BookHistoList("GGGAnalysis","TClTimeDiff3g",100,25.,25.);

  fHS->BookHistoList("GGGAnalysis","NClusters_AfterPresel",25,-0.5,24.5);

  fHS->BookHistoList("GGGAnalysis","ECalClEnergy3g"                ,hBinE,0.,hMaxE);
  fHS->BookHistoList("GGGAnalysis","ECalClEnergy"                  ,hBinE,0.,hMaxE);
  fHS->BookHistoList("GGGAnalysis","ECalClEnergy3g_Intime"         ,hBinE,0.,hMaxE);  
  fHS->BookHistoList("GGGAnalysis","ECalClEnergy3g_Intime_COG"     ,hBinE,0.,hMaxE);  
  fHS->BookHistoList("GGGAnalysis","ECalClEnergy3g_Intime_BestPair",hBinE,0.,hMaxE); 

  fHS->BookHisto2List("GGGAnalysis","EnvsTimeDiff",300,-150.,150.,125,0.,500.);
  fHS->BookHisto2List("GGGAnalysis","EnvsTime",100,-150.,150.,125,0.,500.);
  fHS->BookHistoList("GGGAnalysis","NPairs",25,-0.5,24.5);
  fHS->BookHistoList("GGGAnalysis","TCluDiff3g_Intime",200,20.,20.);  
  fHS->BookHistoList("GGGAnalysis","TCluDiff3g_Intime_COG",200,20.,20.);  

  fHS->BookHistoList("GGGAnalysis","COG_X",300,-150.,150.);   
  fHS->BookHistoList("GGGAnalysis","COG_Y",300,-150.,150.);   
  fHS->BookHisto2List("GGGAnalysis","COG_Map",300,-150.,150.,300,-150.,150.);   

  //3g analysis
  fHS->BookHisto2List("GGGAnalysis","EnvsTimeDiff_3g",300,-150.,150.,125,0.,500.);
  fHS->BookHisto2List("GGGAnalysis","Chi2vsETot",100,0.,20.,125,0.,500.);
  fHS->BookHistoList("GGGAnalysis","ClClDist",300,0.,600.);   
  
  return true;
}

Bool_t Is3GAnalysis::Process(){

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
  if(NClusters<3) return false;
  ETotECal=0;
  for(int ical = 0;ical < NClusters; ical++) {
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal    =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos1   =  fEvent->ECalRecoCl->Element(ical)->GetPosition();
    double ClRadius = sqrt(pos1.X()*pos1.X()+pos1.Y()*pos1.Y());
    ETotECal+=eECal;
    fHS->FillHistoList("GGGAnalysis","ETotECal",ETotECal,1);    
    fHS->FillHistoList("GGGAnalysis","NClusters",NClusters,1);
    fHS->FillHistoList("GGGAnalysis","ECalClEnergy",eECal,1);
    fHS->FillHistoList("GGGAnalysis","ECalClTime",tECal,1);
    fHS->FillHisto2List("GGGAnalysis","EnergyMap",pos1.X(),pos1.Y(),eECal);
    fHS->FillHistoList("GGGAnalysis","ClusterRadius",ClRadius,1);
    
    //Data cut on cluster energy Need the reco to be calibrated
    if(eECal<MinECluster) continue; 

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
  fHS->FillHistoList("GGGAnalysis","NClusters_AfterPresel",NClusters,1);

// Search for in time cluster pairs 
  Int_t NGoodClusters=(Int_t) EGoodCluster.size();
  Int_t NPairs = 0;

  //BUG on dimensions of the arrays
  for(Int_t kk=0;kk<NGoodClusters-2;kk++){
    for(Int_t jj=kk+1;jj<NGoodClusters-1;jj++){
      for(Int_t ll=jj+1;ll<NGoodClusters;ll++){	
	fHS->FillHistoList("GGGAnalysis","TClTimeDiff3g",TGoodCluster[kk]-TGoodCluster[jj],1.);
	fHS->FillHistoList("GGGAnalysis","TClTimeDiff3g",TGoodCluster[kk]-TGoodCluster[ll],1.);
	fHS->FillHistoList("GGGAnalysis","ECalClEnergy3g",EGoodCluster[kk]+EGoodCluster[jj]+EGoodCluster[ll],1);
	
	//	if(! (fabs(TGoodCluster[kk]-TGoodCluster[jj]-TGoodCluster[ll])<TWin) ) continue;
	if(! (fabs(TGoodCluster[kk]-TGoodCluster[jj])<TWin && 
	      fabs(TGoodCluster[kk]-TGoodCluster[ll])<TWin && 
	      fabs(TGoodCluster[jj]-TGoodCluster[ll])<TWin )) continue;
	fHS->FillHistoList("GGGAnalysis","ECalClEnergy3g_Intime",EGoodCluster[kk]+EGoodCluster[jj]+EGoodCluster[ll],1);
	fHS->FillHistoList("GGGAnalysis","TCluDiff3g_Intime",TGoodCluster[kk]-TGoodCluster[jj],1);
	fHS->FillHistoList("GGGAnalysis","TCluDiff3g_Intime",TGoodCluster[kk]-TGoodCluster[ll],1);
	fHS->FillHisto2List("GGGAnalysis","EnvsTimeDiff_3g",TGoodCluster[kk]-TGoodCluster[jj],EGoodCluster[kk]+EGoodCluster[jj],1);
	
	//Compute COG of the identified pair   BUG for more than 3 clusters!!!!
	Ei.push_back(EGoodCluster[kk]);  PosX.push_back(PosXGoodCluster[kk]); PosY.push_back(PosYGoodCluster[kk]);
	Ei.push_back(EGoodCluster[jj]);  PosX.push_back(PosXGoodCluster[jj]); PosY.push_back(PosYGoodCluster[jj]);
	Ei.push_back(EGoodCluster[ll]);  PosX.push_back(PosXGoodCluster[ll]); PosY.push_back(PosYGoodCluster[ll]);
	Double_t COGX = GetCOG(Ei,PosX); Double_t COGY = GetCOG(Ei,PosY);
	fHS->FillHistoList("GGGAnalysis","COG_X",COGX,1);
	fHS->FillHistoList("GGGAnalysis","COG_Y",COGY,1);
	fHS->FillHisto2List("GGGAnalysis","COG_Map",COGX,COGY,1);
	Double_t Dist_jk=sqrt( (PosXGoodCluster[jj]-PosXGoodCluster[kk])*(PosXGoodCluster[jj]-PosXGoodCluster[kk]) + (PosYGoodCluster[jj]-PosYGoodCluster[kk])*(PosYGoodCluster[jj]-PosYGoodCluster[kk]));      	
	Double_t Dist_kl=sqrt( (PosXGoodCluster[ll]-PosXGoodCluster[kk])*(PosXGoodCluster[ll]-PosXGoodCluster[kk]) + (PosYGoodCluster[ll]-PosYGoodCluster[kk])*(PosYGoodCluster[ll]-PosYGoodCluster[kk]));      	
	fHS->FillHistoList("GGGAnalysis","ClClDist",Dist_jk,1);
	fHS->FillHistoList("GGGAnalysis","ClClDist",Dist_kl,1);
  
	if(sqrt(COGY*COGY+COGX*COGX)>COGMax) continue; 

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
	fHS->FillHistoList("GGGAnalysis","ECalClEnergy3g_Intime_COG",EGoodCluster[kk]+EGoodCluster[jj]+EGoodCluster[ll],1);
	fHS->FillHistoList("GGGAnalysis","ECalClEnergy3g_Intime_COG",TGoodCluster[kk]-TGoodCluster[jj],1);
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
//  //cut on at least 1 pair
//  if(NPairs<1) return false;
//  fHS->FillHistoList("GGGAnalysis","NPairs",NPairs,1);
//  double chi2 = 0;
//  for(Int_t ll=0;ll<NPairs;ll++){
//    if(NPairs==1) fHS->FillHistoList("GGGAnalysis","ECalClEnergy3g_Intime_COG",ETotPair[ll],1);
//    //    chi2 = fabs( (ETotPair[ll]-430)/14 ) + fabs(TDiffPair[ll]/1.2);
//    fHS->FillHisto2List("GGGAnalysis","Chi2vsETot",chi2,ETotPair[ll],1.);
//    //    fHS->FillHisto2List("GGGAnalysis","EnvsTime",(TClusterPair[0]+TClusterPair[1])/2,ETotPair[ll],1.);
//  }
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
  std::cout<<"Tot 3G  = "<<N3G<<std::endl;
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
