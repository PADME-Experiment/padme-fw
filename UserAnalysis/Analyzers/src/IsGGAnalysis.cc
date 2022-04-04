// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 12/08/2021
#include "IsGGAnalysis.hh"

IsGGAnalysis::IsGGAnalysis(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating IsGGAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  NGG=0;
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

IsGGAnalysis::~IsGGAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t IsGGAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing IsGGAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t IsGGAnalysis::InitHistos(){
  // IsGGAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("GGAnalysis");
  fHS->BookHistoList("GGAnalysis","NClusters",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","ECalClTime",500,-250.,250.);
  fHS->BookHistoList("GGAnalysis","ClusterRadius",200,0.,400.);
  fHS->BookHistoList("GGAnalysis","TClTimeDiff2g",100,25.,25.);
  fHS->BookHistoList("GGAnalysis","NClusters_AfterPresel",25,-0.5,24.5);

  Double_t hEMax=800;
  Double_t hEBins=800;
  // Energy Histos
  fHS->BookHistoList("GGAnalysis","ECalClEnergy"                     ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","EClusters_AfterPresel"            ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","EG1_final"                        ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","EG2_final"                        ,hEBins,0.,hEMax);

  fHS->BookHistoList("GGAnalysis","ETotECal"                         ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g"                   ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime"            ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG"        ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist"   ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime_BestPair_ZV",hEBins,0.,hEMax); 
  fHS->BookHistoList("GGAnalysis","ClEOutofTime"                     ,hEBins,0.,hEMax);   
  //quandrants NGG distribution
  fHS->BookHistoList("GGAnalysis","EGG_topLeft" ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","EGG_topRight",hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","EGG_botLeft" ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","EGG_botRight",hEBins,0.,hEMax);


  fHS->BookHistoList("GGAnalysis","NPairs",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime",200,20.,20.);  
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime_COG",200,20.,20.);  
  
  fHS->BookHistoList("GGAnalysis","COG_X",300,-150.,150.);   
  fHS->BookHistoList("GGAnalysis","COG_Y",300,-150.,150.);   
  fHS->BookHistoList("GGAnalysis","ClClDist",300,0.,600.);   

  fHS->BookHistoList("GGAnalysis","ZVertex",500,-5000.,0.);   
  fHS->BookHistoList("GGAnalysis","ZVertex_allCut",500,-5000.,0.);   
  fHS->BookHistoList("GGAnalysis","ClClDist_COG",300,0.,600.);   
  fHS->BookHistoList("GGAnalysis","ECalClTime_GG",500,-250.,250.);

  fHS->BookHisto2List("GGAnalysis","EnvsTheta",100,0.,0.1,250,0.,500.); 
  fHS->BookHisto2List("GGAnalysis","EnvsTimeDiff",400,-200.,200.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsTime",200,-200.,200.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsTimeScaled",200,-200.,200.,240,0.,1.2);

  fHS->BookHisto2List("GGAnalysis","EnvsTimeDiff_2g",300,-150.,150.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","COG_Map",300,-150.,150.,300,-150.,150.);
  fHS->BookHisto2List("GGAnalysis","Chi2vsETot",100,0.,20.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap",150,-300.,300.,150,-300.,300.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap_GG",150,-300.,300.,150,-300.,300.);

  fHS->BookHisto2List("GGAnalysis","EpairvsZv",500,-5000.,0.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EpairvsZv_allCuts",500,-5000.,0.,125,0.,500.);
  
  fHS->BookHistoList("GGAnalysis","NClusters_final",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","NGoodClusters_final",25,-0.5,24.5);

  return true;
}

Bool_t IsGGAnalysis::Process(){

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
  static const Double_t TWin= 4.;
  static const Double_t ClRadMin= 100.;
  static const Double_t ClRadMax= 250.;
  static const Double_t COGMax  = 45.;

    
  //Check if is MC or data
  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber(); 

  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && isMC==false) return false;

  //Calo-Calo checks
  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();

  //  if(NClusters<2) return false;
  ETotECal=0;
  for(int ical = 0;ical < NClusters; ical++) {
    if(NEvent==5) cout<<"GG Energy "<<fEvent->ECalRecoCl->Element(ical)->GetEnergy()<<endl;
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    //    if(!isMC) eECal*=  EScale;  //Data ECal energy Need the reco to be calibrated
    double tECal    =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos1   =  fEvent->ECalRecoCl->Element(ical)->GetPosition();
    double ClRadius = sqrt(pos1.X()*pos1.X()+pos1.Y()*pos1.Y());
    ETotECal+=eECal;
    fHS->FillHistoList("GGAnalysis","ETotECal",ETotECal,1);    
    fHS->FillHistoList("GGAnalysis","NClusters",NClusters,1);
    fHS->FillHistoList("GGAnalysis","ECalClEnergy",eECal,1);
    fHS->FillHistoList("GGAnalysis","ECalClTime",tECal,1);
    fHS->FillHisto2List("GGAnalysis","EnergyMap",pos1.X(),pos1.Y(),eECal);
    fHS->FillHistoList("GGAnalysis","ClusterRadius",ClRadius,1);
    
    //Data cut on cluster energy Need the reco to be calibrated
    if(eECal<MinECluster) continue; ///ohhhhhhhooooo 

    //Cut on cluster time wrt to bunch structureData only
//    if(tECal>TMax && isMC==false) continue;
//    if(tECal<TMin && isMC==false) continue;

    //Cut on cluster radius Min and Max
    if(ClRadius<ClRadMin) continue;
    if(ClRadius>ClRadMax) continue;

    EGoodCluster.push_back(eECal);
    TGoodCluster.push_back(tECal);
    PosXGoodCluster.push_back(pos1.X());
    PosYGoodCluster.push_back(pos1.Y());
    fHS->FillHistoList("GGAnalysis","EClusters_AfterPresel",eECal,1.);
  }

//  cut at at least two good clusters
  if(EGoodCluster.size()<2) return false;
  fHS->FillHistoList("GGAnalysis","NClusters_AfterPresel",NClusters,1.);

// Search for in time cluster pairs 
  Int_t NGoodClusters = (Int_t) EGoodCluster.size();
  Int_t NPairs = 0;
  Double_t Zv=0;
  Double_t Dist=0;
  //BUG on dimensions of the arrays
  for(Int_t kk=0;kk<NGoodClusters;kk++){
    for(Int_t jj=kk+1;jj<NGoodClusters;jj++){
      fHS->FillHistoList("GGAnalysis","TClTimeDiff2g",TGoodCluster[kk]-TGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g",EGoodCluster[kk]+EGoodCluster[jj],1);

      if(! (fabs(TGoodCluster[kk]-TGoodCluster[jj])<TWin) ) continue;
      // have a look at the out of time Events to cross check 
      if(! (fabs(TGoodCluster[kk]-TGoodCluster[jj])<TWin) ) fHS->FillHistoList("GGAnalysis","ClEOutofTime",EGoodCluster[kk]+EGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime",EGoodCluster[kk]+EGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime",TGoodCluster[kk]-TGoodCluster[jj],1);
      fHS->FillHisto2List("GGAnalysis","EnvsTimeDiff_2g",TGoodCluster[kk]-TGoodCluster[jj],EGoodCluster[kk]+EGoodCluster[jj],1);

      //Compute COG of the identified pair
      Ei.push_back(EGoodCluster[kk]);  PosX.push_back(PosXGoodCluster[kk]); PosY.push_back(PosYGoodCluster[kk]);
      Ei.push_back(EGoodCluster[jj]);  PosX.push_back(PosXGoodCluster[jj]); PosY.push_back(PosYGoodCluster[jj]);
      // Compute the X,Y coordinates of the center of gravity
      Double_t COGX = GetCOG(Ei,PosX); Double_t COGY = GetCOG(Ei,PosY);
      // Compute the Z coordinate of the interaction Vertex using mass assumption sqrt(2*me*430MeV)
      Zv = GetVertex(Ei,PosX,PosY);
      //distance between two clusters
      Dist = sqrt( (PosXGoodCluster[jj]-PosXGoodCluster[kk])*(PosXGoodCluster[jj]-PosXGoodCluster[kk]) + (PosYGoodCluster[jj]-PosYGoodCluster[kk])*(PosYGoodCluster[jj]-PosYGoodCluster[kk]));      
      fHS->FillHistoList("GGAnalysis","COG_X",COGX,1);
      fHS->FillHistoList("GGAnalysis","COG_Y",COGY,1);
      fHS->FillHisto2List("GGAnalysis","COG_Map",COGX,COGY,1);
      fHS->FillHistoList("GGAnalysis","ClClDist",Dist,1);
      fHS->FillHistoList("GGAnalysis","ZVertex",Zv,1);

      //Cut on pair COG sei sicuro di ABS e non FABS??
      //      if(isMC){
      //	if(abs(COGX+3.88) > COGMax || abs(COGY+3.)>COGMax) continue; //Problems in MC rilasso il CUT
      //      }else{ //COG in data is fine
      if(fabs(COGX) > COGMax || fabs(COGY)>COGMax) continue; 
      //      }

      EClusterPair.push_back(EGoodCluster[kk]);
      TClusterPair.push_back(TGoodCluster[kk]);
      PosXClusterPair.push_back(PosXGoodCluster[kk]);      
      PosYClusterPair.push_back(PosYGoodCluster[kk]);
      
      EClusterPair.push_back(EGoodCluster[jj]);
      TClusterPair.push_back(TGoodCluster[jj]);
      PosXClusterPair.push_back(PosXGoodCluster[jj]);
      PosYClusterPair.push_back(PosYGoodCluster[jj]);

      fHS->FillHisto2List("GGAnalysis","EnergyMap_GG",PosXGoodCluster[jj],PosYGoodCluster[jj],EGoodCluster[jj]);
      fHS->FillHisto2List("GGAnalysis","EnergyMap_GG",PosXGoodCluster[kk],PosYGoodCluster[kk],EGoodCluster[jj]);
      fHS->FillHistoList("GGAnalysis","ClClDist_COG",Dist,1);
      
      NPairs++;
      ETotPair.push_back(EGoodCluster[kk]+EGoodCluster[jj]);
      TDiffPair.push_back(TGoodCluster[kk]-TGoodCluster[jj]);
      //clear vectors
      Ei.clear();
      PosX.clear();
      PosY.clear();
    }
  }
  //cut on at least 1 in time pair
  if(NPairs<1) return false;
  fHS->FillHistoList("GGAnalysis","NPairs",NPairs,1);
  fHS->FillHistoList("GGAnalysis","NClusters_final",NClusters);
  fHS->FillHistoList("GGAnalysis","NGoodClusters_final",NGoodClusters);
  Double_t Angle0=sqrt(PosXGoodCluster[0]*PosXGoodCluster[0]+PosYGoodCluster[0]*PosYGoodCluster[0])/3550.;
  fHS->FillHisto2List("GGAnalysis","EnvsTheta",Angle0,EClusterPair[0],1.);

  double chi2 = 0;
  for(Int_t ll=0;ll<NPairs;ll++){
    if(NPairs==1) {
      fHS->FillHistoList("GGAnalysis","ECalClTime_GG",TClusterPair[0],1);
      fHS->FillHistoList("GGAnalysis","ECalClTime_GG",TClusterPair[1],1);
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG",ETotPair[ll],1);
      if(Dist>300.) fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist",ETotPair[ll],1);
      fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime_COG",TDiffPair[ll],1);
      chi2 = fabs( (ETotPair[ll]-430)/13 ) + fabs(TDiffPair[ll]/1.2);
      fHS->FillHistoList("GGAnalysis","EG1_final",EClusterPair[0],1);
      fHS->FillHistoList("GGAnalysis","EG2_final",EClusterPair[1],1);
      fHS->FillHisto2List("GGAnalysis","Chi2vsETot",chi2,ETotPair[ll],1.);

      fHS->FillHisto2List("GGAnalysis","EpairvsZv_allCuts",Zv,ETotPair[ll],1.);
      //Use only after general scale calibration
      if(ETotPair[ll]>400.){ 
	fHS->FillHistoList("GGAnalysis","ZVertex_allCut",Zv,1);
	fHS->FillHisto2List("GGAnalysis","EnvsTime",(TClusterPair[0]+TClusterPair[1])/2,ETotPair[ll],1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTimeScaled",(TClusterPair[0]+TClusterPair[1])/2,ETotPair[ll]/430,1.);
      }
      //      if(Zv < -2500.)  fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_BestPair_ZV",ETotPair[ll],1); 
    }
  }

//  if(PosXClusterPair[0]>0 && PosYClusterPair[0]>0) fHS->FillHistoList("GGAnalysis","EGG_topRight",ETotPair[0],1);
//  if(PosXClusterPair[0]<0 && PosYClusterPair[0]>0) fHS->FillHistoList("GGAnalysis","EGG_topLeft",ETotPair[0],1);
//
//  if(PosXClusterPair[0]>0 && PosYClusterPair[0]<0) fHS->FillHistoList("GGAnalysis","EGG_botRight",ETotPair[0],1);
//  if(PosXClusterPair[0]<0 && PosYClusterPair[0]<0) fHS->FillHistoList("GGAnalysis","EGG_botLeft",ETotPair[0],1);

  NGG++;
  //Cleaning memory
  ETotPair.clear();  
  TDiffPair.clear(); 
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

Bool_t IsGGAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
  std::cout<<"TotGG        = "<<NGG<<std::endl;
  if (fVerbose) printf("---> Finalizing IsGGAnalysis\n");
  return true;
}


double IsGGAnalysis::GetCOG(std::vector<double> Ei,std::vector<double> Posi)
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

double IsGGAnalysis::GetVertex(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY)
{
  
  Double_t Zv=0;
  Double_t Rij= sqrt(  (PosX[0]-PosX[1])*(PosX[0]-PosX[1]) + (PosY[0]-PosY[1])*(PosY[0]-PosY[1]) ); 
  Zv = -sqrt(Ei[0]*Ei[1]*Rij*Rij)/sqrt(2*0.5111*431);  //Need to use the correct beam energy
  //  std::cout<<"Size of vectors "<<Ei.size()<<" "<<PosX.size()<<" "<<Ei[0]<<" "<<PosY[0]<<" "<<Zv<<std::endl;
  return Zv;
}
