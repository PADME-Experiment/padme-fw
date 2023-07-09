// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 6/04/2022
#include "Is22GGAnalysis.hh"
#include "TF1.h"
#include "TGraph.h"
#include <map>
#include "TLorentzVector.h"
#include <cmath>
#include "TMath.h"

//const Double_t ECalTargetDist = 3640; //new 2022 distance
const Double_t ECalTargetDist = 3700; //new 2022 distance
const Double_t me = 0.511; //electron mass
 
Is22GGAnalysis::Is22GGAnalysis(TString cfgFile, Int_t verbose)
{
  Neve=0;
  Bool_t fisMC = false; 
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating Is22GGAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  NGG=0;
  Navg=500;  //2 min a 49 Hz
  NPoTAvg=0;
  ETotECal=0;
  EToTAvg=0;
  
  fHS = HistoSvc::GetInstance();
  fECalCalib = ECalCalib::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fMCTruth = MCTruth::GetInstance();
  // Standard cuts list
  MinECluster=50.;
  MaxECluster=300.;

  //  TMin=-170.;
  //  TMax= 130.;
  TMin=-500.;
  TMax= 500.;

  TWin= 5.;
//  ClRadMin= 100.;
//  ClRadMax= 250.;

  ClRadMin= 90.;
  ClRadMax= 270.;
  
  COGMax  = 70.;
  DistMin = 240.;
}

Is22GGAnalysis::~Is22GGAnalysis(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t Is22GGAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing Is22GGAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t Is22GGAnalysis::InitHistos(){
  // Is22GGAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("GGAnalysis");
  fHS->CreateList("GG_CM_Analysis");
  fHS->BookHistoList("GGAnalysis","NClusters",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","NHits",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","ClusterSize",26,-0.5,25.5);
  fHS->BookHisto2List("GGAnalysis","ClusterSizeVsRadius",300.,0.,300.,26,-0.5,25.5);
  fHS->BookHisto2List("GGAnalysis","ClusterSizeVsEnergy",700,0.,700.,26,-0.5,25.5);

  fHS->BookHistoList("GGAnalysis","ECalClTime",500,-250.,250.);
  fHS->BookHistoList("GGAnalysis","ClusterRadius",175,0.,350.);
  fHS->BookHistoList("GGAnalysis","TClTimeDiff2g",100,-25.,25.);
  fHS->BookHistoList("GGAnalysis","NClusters_AfterPresel",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","ETotOvNPoT",100,0.,2.);
  fHS->BookHistoList("GGAnalysis","HitETotOvNPoT",100,0.,2.);

  Double_t hEMax = 800;
  Double_t hEBins= 800;
  // Energy Histos
  fHS->BookHistoList("GGAnalysis","ECalClEnergy"         ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","EClusters_AfterPresel",hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","EG1_final"            ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","EG2_final"            ,hEBins,0.,hEMax);

  fHS->BookHisto2List("GGAnalysis","E1vsE2_BadVtx",hEBins/2,0.,hEMax/2,hEBins/2,0.,hEMax/2);

  //  fHS->BookHistoList("GGAnalysis","ETotECal"                         ,hEBins*2,0.,hEMax*2);
  fHS->BookHistoList("GGAnalysis","ETotECal"                         ,hEBins,0.,1600.);
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g"                   ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime"            ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG"        ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist"   ,hEBins,0.,hEMax);
  fHS->BookHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist_Enth",hEBins,0.,hEMax); 
  fHS->BookHistoList("GGAnalysis","ClEOutofTime"                     ,hEBins,0.,hEMax);   

  //quandrants NGG distribution
  fHS->BookHistoList("GGAnalysis","EGG_topLeft" ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","EGG_topRight",hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","EGG_botLeft" ,hEBins,0.,hEMax);  
  fHS->BookHistoList("GGAnalysis","EGG_botRight",hEBins,0.,hEMax);

  fHS->BookHistoList("GGAnalysis","NPairs",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime",200,-20.,20.);  
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime_COG",200,-20.,20.);  
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime_COG_Dist",200,-20.,20.); 
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime_COG_Dist_Enth",200,-20.,20.);  

  fHS->BookHistoList("GGAnalysis","TDiff_DataMC_gg",200,25.,-25.);  
  fHS->BookHistoList("GGAnalysis","EDiff_DataMC_gg",hEBins,-hEMax/2,hEMax/2);     
  fHS->BookHistoList("GGAnalysis","EDiff_DataMC_gg_Ok",hEBins,0,hEMax);   

  fHS->BookHistoList("GGAnalysis","COG_X",300,-150.,150.);   
  fHS->BookHistoList("GGAnalysis","COG_Y",300,-150.,150.);   

  fHS->BookHistoList("GGAnalysis","COG_X_EnTh",300,-150.,150.);   
  fHS->BookHistoList("GGAnalysis","COG_Y_EnTh",300,-150.,150.);   

  fHS->BookHistoList("GGAnalysis","ClClDist",300,0.,600.);   
  
  Int_t NBinZ=400;
  Double_t MinZ=-4000.;
  fHS->BookHistoList("GGAnalysis","ZVertex"           ,NBinZ,MinZ,0.);
  fHS->BookHistoList("GGAnalysis","ZVertex_COG"       ,NBinZ,MinZ,0.);
  fHS->BookHistoList("GGAnalysis","ZVertex_COG_Dist"  ,NBinZ,MinZ,0.);
  fHS->BookHistoList("GGAnalysis","ZVertex_allCut"    ,NBinZ,MinZ,0.);   
  fHS->BookHistoList("GGAnalysis","ZVertex_EnnvsTheta",NBinZ,MinZ,0.);   
  fHS->BookHistoList("GGAnalysis","ZVertex_EnnvsTheta_displaced",NBinZ,MinZ,0.);   
  fHS->BookHistoList("GGAnalysis","ZVertex_EPair",     NBinZ,MinZ,0.);   
  
  Int_t NBinM=125;
  Double_t MaxM=25.;
  fHS->BookHistoList("GGAnalysis","MGG"           ,NBinM,0.,MaxM);
  fHS->BookHistoList("GGAnalysis","MGG_Open"      ,200,0.,200.);
  fHS->BookHistoList("GGAnalysis","MGG_COG"       ,NBinM,0.,MaxM);
  fHS->BookHistoList("GGAnalysis","MGG_COG_Dist"  ,NBinM,0.,MaxM);
  fHS->BookHistoList("GGAnalysis","MGG_EnnvsTheta",NBinM,0.,MaxM);   

  fHS->BookHistoList("GGAnalysis","ClClDist_COG",300,0.,600.);   
  fHS->BookHistoList("GGAnalysis","ECalClTime_GG",500,-250.,250.);

  fHS->BookHisto2List("GGAnalysis","EnvsTheta",250,0.,500.,100,0.,0.1); 
  fHS->BookHisto2List("GGAnalysis","EnvsTheta_cut",250,0.,500.,100,0.,0.1); 
  fHS->BookHisto2List("GGAnalysis","EnvsTheta_NoCMcut",250,0.,500.,100,0.,0.1); 

  fHS->BookHisto2List("GGAnalysis","EnvsTimeDiff"  ,400,-200.,200.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsTime"      ,200,-250.,125.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsTimeNoCuts",200,-250.,125.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsXNoCuts",200,-300.,300.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsYNoCuts",200,-300.,300.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsTimeScaled",200,-200.,200.,240,0.,1.2);

  fHS->BookHisto2List("GGAnalysis","EnvsTimeDiff_2g",300,-150.,150.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","COG_Map",300,-150.,150.,300,-150.,150.);
  fHS->BookHisto2List("GGAnalysis","COG_MapCut",200,-100.,100.,200,-100.,100.);
  fHS->BookHisto2List("GGAnalysis","COG_MapCut_EnTh",200,-100.,100.,200,-100.,100.);
  fHS->BookHisto2List("GGAnalysis","Chi2vsETot",100,0.,20.,125,0.,500.);

  fHS->BookHisto2List("GGAnalysis","EnergyMap",150,-300.,300.,150,-300.,300.);  
  fHS->BookHisto2List("GGAnalysis","EnergyMap<60MeV",150,-300.,300.,150,-300.,300.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap>220MeV",150,-300.,300.,150,-300.,300.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap2ClFinal",150,-300.,300.,150,-300.,300.);

  fHS->BookHisto2List("GGAnalysis","EnergyMapRawCol",30,0.,30.,30,0.,30.);   

  fHS->BookHisto2List("GGAnalysis","EnergyMap_GG",150,-300.,300.,150,-300.,300.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",150,-300.,300.,150,-300.,300.);

  fHS->BookHisto2List("GGAnalysis","EpairvsZv",500,-5000.,0.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EpairvsZv_allCuts",500,-5000.,0.,125,0.,500.);
  
  fHS->BookHistoList("GGAnalysis","NClusters_final",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","NGoodClusters_final",25,-0.5,24.5);

  fHS->BookHistoList("GGAnalysis","NPoT_LG",500,-500.,9500.);
  fHS->BookHistoList("GGAnalysis","NPoT_LG_Clean",500,-500.,9500.);
  fHS->BookHistoList("GGAnalysis","NPoT_Tar",500,-500.,9500.);

  ///HOLE clusters study
  fHS->BookHistoList("GGAnalysis","HoleCl_Seed",500,-500.,9500.);
  fHS->BookHistoList("GGAnalysis","HoleCl_Ene",500,0.,500.);

  //CM frame plots
  fHS->BookHistoList("GG_CM_Analysis","ECM1",500,0.,50.);
  fHS->BookHistoList("GG_CM_Analysis","ECM2",500,0.,50.);
  fHS->BookHistoList("GG_CM_Analysis","ECM_Tot",500,0.,50.);
  fHS->BookHisto2List("GG_CM_Analysis","E1vsE2_CM",100,0.,25.,100,0.,25.); 
  fHS->BookHisto2List("GG_CM_Analysis","E1vsE2_CM_cut",100,0.,25.,100,0.,25.); 
  fHS->BookHisto2List("GG_CM_Analysis","E1vsE2_CM_ThetaPhiCut",100,0.,25.,100,0.,25.); 
  fHS->BookHisto2List("GG_CM_Analysis","E1vsE2_CM_EnthetaCut",100,0.,25.,100,0.,25.); 
  fHS->BookHisto2List("GG_CM_Analysis","E1vsE2_CM_NotEth",100,0.,25.,100,0.,25.); 
  fHS->BookHistoList("GG_CM_Analysis","DTheta_CM",250,1.,3.5); 
  fHS->BookHistoList("GG_CM_Analysis","DPhi_CM",250,1.,3.5);
  fHS->BookHisto2List("GG_CM_Analysis","ThetaPhi_CM",250,1.,3.5,250,1.,3.5);
  fHS->BookHistoList("GG_CM_Analysis","M_CM",NBinM,0.,MaxM);    
  
  return true;
}

Bool_t Is22GGAnalysis::Process(){
  //Cleaning memory cluster
  EGoodCluster.clear();
  TGoodCluster.clear();
  PosXGoodCluster.clear();
  PosYGoodCluster.clear();
  AngleGoodCluster.clear();
  IsGGGoodCluster.clear();

  // Selected Cluster having correct gg kinematics
  
  PairGClIndex1.clear();  //index1 of the paired crystal in the good cluster structure  
  PairGClIndex2.clear();  //index2 of the paired crystal in the good cluster structure  
  PairCOGX.clear();
  PairCOGY.clear();
  PairDist.clear();
  PairVertex.clear();
 
  vEi.clear();
  vPosX.clear();
  vPosY.clear();
  
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber();
  //Check for physics trigger in data Only.
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) fisMC=true;
  if(!fisMC && NEvent%10000==0) cout<<"NEvent "<<NEvent<<endl;
  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && fisMC==false) return false;

  //Set Some variables at the first event.

  Int_t NEventHits = fEvent->ECalRecoEvent->GetNHits();
  Int_t ChId = 0;
  Int_t Row,Col;
  Double_t TotHitE=0;

  //  cout<<" Entering Is22GG "<<NEvent<<" ismC"<<fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<endl;
  if(Neve==0){
    fNRun = fEvent->RecoEvent->GetRunNumber(); //Occhio che 30000 vale solo per il 2022
//    if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
//      fisMC=true;
//    }
    if(fisMC)  fBeamE = fMCTruth->GetBeamEnergy(); 
    if(!fisMC) fBeamE = fECalCalib->GetBeamEnergy();
    fSqrtS=TMath::Sqrt(2*me*fBeamE);
    if(fBeamE==0 && !fisMC){
      cout<<"No Beam energy found in DB "<<endl;
      exit(1);
    } //need to be run dependent
    
    //cout<<"Run nymber "<<fNRun<<" Beam energy "<<fBeamE<<" sqrts "<<fSqrtS<<" me "<<me<<endl;
    cout<<"****************************************************************"<<endl;
    MinECluster=fBeamE*0.20;
    MaxECluster=fBeamE*0.90;
    cout<<"Run number "<<fNRun<<" Beam energy "<<fBeamE<<" Min ECluster "<<MinECluster<<" SQRT(s) "<<endl;
  }
  Neve++;
  
  double NPoTTar = fEvent->TargetRecoBeam->getnPOT();
  Double_t NPoTLG=0; 
  if(fEvent->LeadGlassRecoEvent!=0) NPoTLG = fEvent->LeadGlassRecoEvent->GetNPoTs();
  Double_t TimeStamp=fEvent->RecoEvent->GetEventTime(); 
  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  ETotECal=0;
  NPoTAvg+=NPoTLG;

  for(int ical = 0;ical < NClusters; ical++){
    double eECal = fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    ETotECal+=eECal;
  }

  for(int iHit = 0;iHit <NEventHits ; iHit++) {
    ChId = fEvent->ECalRecoEvent->Hit(iHit)->GetChannelId();
    double HitE = fEvent->ECalRecoEvent->Hit(iHit)->GetEnergy();
    fHS->FillHistoList("GGAnalysis","NHits",NEventHits);
    Row=ChId%100;   Col=ChId/100;
    // cout<<" Entering NHits "<<NEventHits<<" IHit "<<iHit<<" "<<ChId<<" Row "<<Row<<" Col "<<Col<<""<<HitE<<endl;
    //      if(fEvent->ECalRecoEvent->Hit(iHit)->GetEnergy()>20.) 
    fHS->FillHistoList("GGAnalysis","EnergyMapRawCol",Col,Row);
    TotHitE+=HitE;
  }
  
  HitEToTAvg += TotHitE;
  EToTAvg += ETotECal;
  if(Neve%Navg==0){
    //    cout<<Neve<<" NPot "<<NPoTAvg<<" NAVG "<<Navg<<" NPotLG "<<NPoTLG<<endl;
    NPoTAvg/=Navg;
    EToTAvg/=Navg;
    HitEToTAvg/=Navg;

    vNPoT.push_back(NPoTAvg);
    vNEvt.push_back(TimeStamp);
    vETotECal.push_back(EToTAvg);
    vRatio.push_back(EToTAvg/NPoTAvg);
    //    cout<<"NPot "<<NPoTAvg<<endl;
    fHS->FillHistoList("GGAnalysis","ETotOvNPoT",EToTAvg/NPoTAvg);
    fHS->FillHistoList("GGAnalysis","HitETotOvNPoT",HitEToTAvg/NPoTAvg);
    NPoTAvg=0;
    EToTAvg=0;
    HitEToTAvg=0;
  }

  //***************************************
  //Check for non empty bunch using NPOTs
  //***************************************
  if(NPoTLG<250 && !fisMC) return false;
  fHS->FillHistoList("GGAnalysis","NPoT_LG",NPoTLG);
  fHS->FillHistoList("GGAnalysis","NPoT_Tar",NPoTTar); 
  
  if(fNRun==50238 || fNRun==50240){
    if(EToTAvg/NPoTAvg>0.43) return false;
  }
  fHS->FillHistoList("GGAnalysis","NPoT_LG_Clean",NPoTLG);
  //***************************************
  // Cut on at least 2 clusters
  //***************************************
  if(NClusters >20) cout<<"Crazy amount of clusters "<<NClusters<<endl;
  if(NClusters<2 || NClusters>20) return false;
  ETotECal=0;
  
  for(int ical = 0;ical < NClusters; ical++) {
    //    if(NEvent==5) cout<<"GG Energy "<<fEvent->ECalRecoCl->Element(ical)->GetEnergy()<<endl;
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal    =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos    =  fEvent->ECalRecoCl->Element(ical)->GetPosition();
    
    //    if(eECal>35 && eECal<60 && pos.X()<-280.){
    Int_t iSeed     = fEvent->ECalRecoCl->Element(ical)->GetSeed();
    Int_t ChId      = fEvent->ECalRecoEvent->Hit(iSeed)->GetChannelId();
    Int_t ClSize    = fEvent->ECalRecoCl->Element(ical)->GetNHitsInClus();
    Int_t Raw,Col;
//    if(abs(pos.X())<5 && (abs(pos.Y())<5)) {
//      ChId = fEvent->ECalRecoEvent->Hit(iHit)->GetChannelId();
//      Row=ChId%100;
//      Col=ChId/100;
//     
//      fHS->Fill2HistoList("GGAnalysis","",eECal,1);        
//      fHS->FillHistoList("GGAnalysis","HoleCl_Ene",eECal,1);         
//      fHS->FillHistoList("GGAnalysis","HoleCl_Seed",ChiD,1);    
//    }
//      cout<<"Chi sei riga? "<<ChId%100<<" colonna "<<ChId/100<<endl;
//    }
    
    double ClRadius = sqrt(pos.X()*pos.X()+pos.Y()*pos.Y());
    ETotECal+=eECal;
    
    fHS->FillHistoList("GGAnalysis","NClusters",NClusters,1);
    fHS->FillHistoList("GGAnalysis","ClusterSize",ClSize,1);
    fHS->FillHistoList("GGAnalysis","ClusterSizeVsEnergy",eECal,ClSize);
    fHS->FillHistoList("GGAnalysis","ClusterSizeVsRadius",ClRadius,ClSize);
    
    fHS->FillHistoList("GGAnalysis","ECalClEnergy",eECal,1);
    fHS->FillHistoList("GGAnalysis","ECalClTime",tECal,1);
    fHS->FillHisto2List("GGAnalysis","EnvsTimeNoCuts",tECal,eECal,1.);
    fHS->FillHisto2List("GGAnalysis","EnvsXNoCuts",pos.X(),eECal,1.);
    fHS->FillHisto2List("GGAnalysis","EnvsYNoCuts",pos.Y(),eECal,1.);
    fHS->FillHisto2List("GGAnalysis","EnergyMap",pos.X(),pos.Y(),eECal);

    if(eECal<60)  fHS->FillHisto2List("GGAnalysis","EnergyMap<60MeV",pos.X(),pos.Y(),1);
    if(eECal>220) fHS->FillHisto2List("GGAnalysis","EnergyMap>220MeV",pos.X(),pos.Y(),1);
    fHS->FillHistoList("GGAnalysis","ClusterRadius",ClRadius,1);
    
    //Data cut on cluster energy now it's calibrated
    if(eECal<MinECluster) continue; 
    if(eECal>MaxECluster) continue; 

    //Cut on cluster radius Min and Max
    if(ClRadius<ClRadMin) continue;
    if(ClRadius>ClRadMax) continue;

    //compute angles
    Double_t Angle=sqrt(pos.X()*pos.X()+pos.Y()*pos.Y())/ECalTargetDist;
    bool GGCheck=CheckThetaAngle(eECal,Angle);
    int Is22GGCheckOk=-1;
    if(GGCheck) Is22GGCheckOk=1;

    EGoodCluster.push_back(eECal);
    TGoodCluster.push_back(tECal);
    PosXGoodCluster.push_back(pos.X());
    PosYGoodCluster.push_back(pos.Y());
    AngleGoodCluster.push_back(Angle);
    IsGGGoodCluster.push_back(Is22GGCheckOk);
    fHS->FillHistoList("GGAnalysis","EClusters_AfterPresel",eECal,1.);
  }

  // Search for in time cluster pairs 
  Int_t NGoodClusters = (Int_t) EGoodCluster.size();
  //  cut at at least two good clusters
  if(NGoodClusters<2) return false;
  if(NGoodClusters>10) cout<<"Crazy amount of Good clusters "<<NGoodClusters<<endl;
  fHS->FillHistoList("GGAnalysis","NClusters_AfterPresel",NClusters,1.);
  Int_t NPairs = 0;
  Double_t Zv=0;
  Double_t Dist=0;
 
  //Start Looking for cluster pairs
  for(Int_t kk=0;kk<NGoodClusters-1;kk++){
    for(Int_t jj=kk+1;jj<NGoodClusters;jj++){
      fHS->FillHistoList("GGAnalysis","TClTimeDiff2g",TGoodCluster[kk]-TGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g",EGoodCluster[kk]+EGoodCluster[jj],1);
      // have a look at the out of time Events to cross check cut quality 
      if(! (fabs(TGoodCluster[kk]-TGoodCluster[jj])<TWin) ) fHS->FillHistoList("GGAnalysis","ClEOutofTime",EGoodCluster[kk]+EGoodCluster[jj],1);

      //***********************
      //    CUT ON PAIR TIME
      //***********************
      if(! (fabs(TGoodCluster[kk]-TGoodCluster[jj])<TWin) ) continue;
      fHS->FillHisto2List("GGAnalysis","EnergyMap_GG",PosXGoodCluster[jj],PosYGoodCluster[jj],EGoodCluster[jj]);
      fHS->FillHisto2List("GGAnalysis","EnergyMap_GG",PosXGoodCluster[kk],PosYGoodCluster[kk],EGoodCluster[jj]);

      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime",EGoodCluster[kk]+EGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime",TGoodCluster[kk]-TGoodCluster[jj],1);
      fHS->FillHisto2List("GGAnalysis","EnvsTimeDiff_2g",TGoodCluster[kk]-TGoodCluster[jj],EGoodCluster[kk]+EGoodCluster[jj],1);

      //Compute COG of the identified pair
      vEi.push_back(EGoodCluster[kk]);  vPosX.push_back(PosXGoodCluster[kk]); vPosY.push_back(PosYGoodCluster[kk]);
      vEi.push_back(EGoodCluster[jj]);  vPosX.push_back(PosXGoodCluster[jj]); vPosY.push_back(PosYGoodCluster[jj]);
      Double_t COGX = CompCOG(vEi,vPosX); 
      Double_t COGY = CompCOG(vEi,vPosY);
      //Retrieve COG position for the current RUN
      Double_t RunCOGX = fECalCalib->GetCOGX(); 
      Double_t RunCOGY = fECalCalib->GetCOGY();
      //      cout<<"GET COG "<<RunCOGX<<" GET COG Y "<<fECalCalib->GetCOGY()<<endl;

      fHS->FillHistoList("GGAnalysis","COG_X",COGX,1);
      fHS->FillHistoList("GGAnalysis","COG_Y",COGY,1);
      fHS->FillHisto2List("GGAnalysis","COG_Map",COGX,COGY,1);

      // Compute the Z coordinate of the interaction Vertex using mass assumption sqrt(2*me*430MeV)
      Zv = GetVertex(vEi,vPosX,vPosY);  //occhio che funziona solo a due
      double Mgg = ComputeInvariantMass(vEi,vPosX,vPosY);  //Massa invariante gg
      //      cout<<"MGG "<<Mgg<<endl;
      fHS->FillHistoList("GGAnalysis","ZVertex",Zv,1);
      fHS->FillHistoList("GGAnalysis","MGG",Mgg,1);
      fHS->FillHistoList("GGAnalysis","MGG_Open",Mgg,1);
      vEi.clear();
      vPosX.clear();
      vPosY.clear();
      
      //distance between two clusters
      Dist = sqrt( (PosXGoodCluster[jj]-PosXGoodCluster[kk])*(PosXGoodCluster[jj]-PosXGoodCluster[kk]) + (PosYGoodCluster[jj]-PosYGoodCluster[kk])*(PosYGoodCluster[jj]-PosYGoodCluster[kk]));      
      fHS->FillHistoList("GGAnalysis","ClClDist",Dist,1);

      //***********************
      // CUT ON PAIR COG
      //***********************
      if(sqrt(COGY*COGY+COGX*COGX)>COGMax) continue; 
      fHS->FillHisto2List("GGAnalysis","COG_MapCut",COGX,COGY,1);
      fHS->FillHistoList("GGAnalysis","ClClDist_COG",Dist,1);
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG",EGoodCluster[kk]+EGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime_COG",TGoodCluster[kk]-TGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","ZVertex_COG",Zv,1);
      fHS->FillHistoList("GGAnalysis","MGG_COG",Mgg,1);
      //***********************
      // CUT ON PAIR Dist
      //***********************
      if(Dist<DistMin) continue; 
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist",EGoodCluster[kk]+EGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime_COG_Dist",TGoodCluster[kk]-TGoodCluster[jj],1);
      fHS->FillHisto2List("GGAnalysis","EnvsTheta",EGoodCluster[kk],AngleGoodCluster[kk],1.);
      fHS->FillHisto2List("GGAnalysis","EnvsTheta",EGoodCluster[jj],AngleGoodCluster[jj],1.); 
      fHS->FillHistoList("GGAnalysis","ZVertex_COG_Dist",Zv,1);
      fHS->FillHistoList("GGAnalysis","MGG_COG_Dist",Mgg,1);
      PairGClIndex1.push_back(kk);      
      PairGClIndex2.push_back(jj);
      PairCOGX.push_back(COGX);
      PairCOGY.push_back(COGY);
      PairDist.push_back(Dist);
      PairVertex.push_back(Zv);

      //Study of the CM variables
      TLorentzVector P4E1= FillTLorentzVector(PosXGoodCluster[jj],PosYGoodCluster[jj],EGoodCluster[jj]);      
      TLorentzVector P4E2= FillTLorentzVector(PosXGoodCluster[kk],PosYGoodCluster[kk],EGoodCluster[kk]);      
      TLorentzVector P4Beam;
      P4Beam.SetPxPyPzE(0,0,sqrt(fBeamE*fBeamE-me*me),fBeamE);
      TLorentzVector P4E1_CM=TransformToRestFrame(P4Beam,P4E1);
      TLorentzVector P4E2_CM=TransformToRestFrame(P4Beam,P4E2);
      TLorentzVector PTot = P4E1_CM+P4E2_CM;
      Double_t M2cl_CM = PTot.M();  //Massa invariante gg
      std::cout<<"E1CM "<<Mgg<<" Mgg_CM "<<M2cl_CM<<std::endl;
//      std::cout<<"E1CM "<<P4E1_CM.E()<<std::endl;
//      std::cout<<"E2CM "<<P4E2_CM.E()<<std::endl;
      fHS->FillHistoList("GG_CM_Analysis","ECM1",P4E1_CM.E(),1);
      fHS->FillHistoList("GG_CM_Analysis","ECM2",P4E2_CM.E(),1);
      fHS->FillHistoList("GG_CM_Analysis","ECM_Tot",P4E2_CM.E()+P4E1_CM.E(),1);
      fHS->FillHistoList("GG_CM_Analysis","DTheta_CM",P4E2_CM.Theta()+P4E1_CM.Theta(),1);
      fHS->FillHistoList("GG_CM_Analysis","DPhi_CM",fabs(P4E2_CM.Phi()-P4E1_CM.Phi()),1);
      fHS->FillHisto2List("GG_CM_Analysis","E1vsE2_CM",P4E1_CM.E(),P4E2_CM.E());
      fHS->FillHistoList("GG_CM_Analysis","M_CM",M2cl_CM);
      fHS->FillHisto2List("GG_CM_Analysis","ThetaPhi_CM",P4E2_CM.Theta()+P4E1_CM.Theta(),fabs(P4E2_CM.Phi()-P4E1_CM.Phi()));

      if((P4E1_CM.E()-fSqrtS*0.5)*(P4E1_CM.E()-fSqrtS*0.5) + (P4E2_CM.E()-fSqrtS*0.5)*(P4E2_CM.E()-fSqrtS*0.5)<fSqrtS*0.35){ 
	fHS->FillHisto2List("GG_CM_Analysis","E1vsE2_CM_cut",P4E1_CM.E(),P4E2_CM.E());
      }else{
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_NoCMcut",EGoodCluster[kk],AngleGoodCluster[kk],1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_NoCMcut",EGoodCluster[jj],AngleGoodCluster[jj],1.);
      }
      if((P4E2_CM.Theta()+P4E1_CM.Theta())>2.9 && fabs(P4E2_CM.Phi()-P4E1_CM.Phi())>2.9) fHS->FillHisto2List("GG_CM_Analysis","E1vsE2_CM_ThetaPhiCut",P4E1_CM.E(),P4E2_CM.E());
      //Check if it's a good GG cluster
      if(IsGGGoodCluster[jj]>0 && IsGGGoodCluster[kk]>0){ 
	fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist_Enth",EGoodCluster[kk]+EGoodCluster[jj],1);
	fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime_COG_Dist_Enth",TGoodCluster[kk]-TGoodCluster[jj],1);
	fHS->FillHistoList("GGAnalysis","ZVertex_EnnvsTheta",Zv,1);
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_cut",EGoodCluster[kk],AngleGoodCluster[kk],1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_cut",EGoodCluster[jj],AngleGoodCluster[jj],1.);  
	fHS->FillHistoList("GGAnalysis","MGG_EnnvsTheta",Mgg,1.);  
	fHS->FillHisto2List("GGAnalysis","COG_MapCut_EnTh",COGX,COGY,1);
	fHS->FillHistoList("GGAnalysis","COG_X_EnTh",COGX,1);
	fHS->FillHistoList("GGAnalysis","COG_Y_EnTh",COGY,1);
	fHS->FillHisto2List("GGAnalysis","E1vsE2_CM_EnthetaCut",P4E1_CM.E(),P4E2_CM.E());
      }else{      
	fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[jj],PosYGoodCluster[jj],EGoodCluster[jj]);
	fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[kk],PosYGoodCluster[kk],EGoodCluster[kk]);
	//	fHS->FillHisto2List("GGAnalysis","E1vsE2_BadVtx",EGoodCluster[jj],EGoodCluster[kk]);
	fHS->FillHisto2List("GGAnalysis","E1vsE2_CM_NotEth",P4E1_CM.E(),P4E2_CM.E());
      }

      Double_t Angle         = sqrt(PosXGoodCluster[jj]*PosXGoodCluster[jj]+PosYGoodCluster[jj]*PosYGoodCluster[jj])/fabs(Zv);
      Double_t Angle1        = sqrt(PosXGoodCluster[kk]*PosXGoodCluster[kk]+PosYGoodCluster[kk]*PosYGoodCluster[kk])/fabs(Zv);

      bool G1Check_Displaced = CheckThetaAngle(EGoodCluster[jj],Angle);
      bool G2Check_Displaced = CheckThetaAngle(EGoodCluster[kk],Angle1);

      if(G1Check_Displaced && G2Check_Displaced & abs(EGoodCluster[kk]+EGoodCluster[jj]-fBeamE)/14<3){ 
      	fHS->FillHistoList("GGAnalysis","ZVertex_EnnvsTheta_displaced",Zv,1);
      }
      NPairs++;      
    }
  } // end of loop on good clusters
  //**************************
  // Cut on at least good pair
  //**************************
  if(NPairs<1) return false;

  fHS->FillHistoList("GGAnalysis","NClusters_final",NClusters);
  fHS->FillHistoList("GGAnalysis","NGoodClusters_final",NGoodClusters);
  fHS->FillHistoList("GGAnalysis","NPairs",NPairs,1);

  double chi2 = 0;
  for(Int_t ll=0;ll<NPairs;ll++){
    bool isOk=false;
    if(NPairs==1) {
      if(fisMC && NPairs==1) isOk=IsMCGG(TGoodCluster[PairGClIndex1[ll]],EGoodCluster[PairGClIndex1[ll]],EGoodCluster[PairGClIndex2[ll]]);
      Double_t EPair      = EGoodCluster[PairGClIndex1[ll]] + EGoodCluster[PairGClIndex2[ll]];
      Double_t DeltaTPair = TGoodCluster[PairGClIndex1[ll]] - TGoodCluster[PairGClIndex2[ll]];
      Double_t TimePair   = (TGoodCluster[PairGClIndex1[ll]] + TGoodCluster[PairGClIndex2[ll]])/2;
      
      if(isOk) fHS->FillHistoList("GGAnalysis","EDiff_DataMC_gg_Ok",EPair);   
      
      fHS->FillHistoList("GGAnalysis","ECalClTime_GG",TimePair,1);
      //Event quality estimator 
      Int_t GClInd1=PairGClIndex1[ll];
      Int_t GClInd2=PairGClIndex2[ll];
      chi2 = ( fabs((EPair-fBeamE)/15) + fabs(DeltaTPair/1.2) +fabs(PairCOGX[ll]/8.) + fabs(PairCOGY[ll]/8.) )/4; 
      fHS->FillHistoList("GGAnalysis","EG1_final",EGoodCluster[PairGClIndex1[ll]],1);
      fHS->FillHistoList("GGAnalysis","EG2_final",EGoodCluster[PairGClIndex2[ll]],1);
      fHS->FillHisto2List("GGAnalysis","EnergyMap2ClFinal",PosXGoodCluster[GClInd1],PosYGoodCluster[GClInd1],EGoodCluster[GClInd1]);
      fHS->FillHisto2List("GGAnalysis","EnergyMap2ClFinal",PosXGoodCluster[GClInd2],PosYGoodCluster[GClInd2],EGoodCluster[GClInd2]);
      fHS->FillHisto2List("GGAnalysis","Chi2vsETot",chi2,EPair,1.);

      fHS->FillHisto2List("GGAnalysis","EpairvsZv_allCuts",PairVertex[ll],EPair,1.);
      //      if(IsGGGoodCluster[jj]>0 && IsGGGoodCluster[kk]>0){ 
      if(PosXGoodCluster[PairGClIndex1[0]]>0 && PosYGoodCluster[PairGClIndex1[ll]]>0) fHS->FillHistoList("GGAnalysis","EGG_topRight",EPair,1);
      if(PosXGoodCluster[PairGClIndex1[0]]<0 && PosYGoodCluster[PairGClIndex1[ll]]>0) fHS->FillHistoList("GGAnalysis","EGG_topLeft",EPair,1);
      if(PosXGoodCluster[PairGClIndex1[0]]>0 && PosYGoodCluster[PairGClIndex1[ll]]<0) fHS->FillHistoList("GGAnalysis","EGG_botRight",EPair,1);
      if(PosXGoodCluster[PairGClIndex1[0]]<0 && PosYGoodCluster[PairGClIndex1[ll]]<0) fHS->FillHistoList("GGAnalysis","EGG_botLeft",EPair,1);
	//      }
      //Use only after general scale calibration
      //      if(abs(EPair-fBeamE)/15<4){ 
	fHS->FillHistoList("GGAnalysis","ZVertex_EPair",PairVertex[ll],1);
	fHS->FillHisto2List("GGAnalysis","EnvsTime",TimePair,EPair,1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTimeScaled",TimePair,EPair/fBeamE,1.);
	//looking for bad events
	if(Zv>-2500.){
	  fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[PairGClIndex1[ll]],PosYGoodCluster[PairGClIndex1[ll]],EGoodCluster[PairGClIndex1[ll]]);
	}
	//      }
    }
  }
  NGG++;
  //  cout<<" NGG "<<NGG<<" NGG MC << "<<NGG_MC<<" NPairs "<<NPairs<<endl;
  //Cleaning memory cluster
  EGoodCluster.clear();
  TGoodCluster.clear();
  PosXGoodCluster.clear();
  PosYGoodCluster.clear();
  AngleGoodCluster.clear();

  PairGClIndex1.clear();  //index1 of the paired crystal in the good cluster structure  
  PairGClIndex2.clear();  //index2 of the paired crystal in the good cluster structure  
  PairCOGX.clear();
  PairCOGY.clear();
  PairDist.clear();
  PairVertex.clear();
  NPairs=0;
  return true;
}

Bool_t Is22GGAnalysis::Finalize()
{
  TGraph* NPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
  fHS->SaveTGraphList("GGAnalysis","NPotVsTime",NPotVsTime);
  NPotVsTime->SetMarkerStyle(21);

  TGraph* ETotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vETotECal[0]);
  fHS->SaveTGraphList("GGAnalysis","ETotVsTime",ETotVsTime);
  ETotVsTime->SetMarkerStyle(21);

  TGraph* ETotOvNPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vRatio[0]);
  fHS->SaveTGraphList("GGAnalysis","ETotOvNPotVsTime",ETotOvNPotVsTime);
  ETotOvNPotVsTime->SetMarkerStyle(21);

  std::cout<<"TotGG        = "<<NGG<<std::endl;
  std::cout<<"TotGG_MC     = "<<NGG_MC<<std::endl;
  if (fVerbose) printf("---> Finalizing Is22GGAnalysis\n");
  return true;
}

double Is22GGAnalysis::CompCOG(std::vector<double> Ei,std::vector<double> Posi)
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

double Is22GGAnalysis::GetVertex(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY)
{
  Double_t Zv=0;
  Double_t Rij= sqrt(  (PosX[0]-PosX[1])*(PosX[0]-PosX[1]) + (PosY[0]-PosY[1])*(PosY[0]-PosY[1]) ); 
  Zv = -sqrt(Ei[0]*Ei[1]*Rij*Rij)/sqrt(2*me*fBeamE);  //Need to use the correct beam energy
  //  std::cout<<"Size of vectors "<<Ei.size()<<" "<<PosX.size()<<" "<<Ei[0]<<" "<<PosY[0]<<" "<<Zv<<" EBeam "<<fBeamE<<std::endl;
  return Zv;
}


double Is22GGAnalysis::ComputeInvariantMass(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY)
{
  Double_t Mgg=0;
  //  Double_t Zv  = 3534+177; //use some kind of geometry plz
  //  Double_t Zv  = 3640; //use the 2022 geometry with displaced Ecal 3640+177 to be checked.
  Double_t Rij = sqrt( (PosX[0]-PosX[1])*(PosX[0]-PosX[1]) + (PosY[0]-PosY[1])*(PosY[0]-PosY[1]) ); 
  Mgg = sqrt(Ei[0]*Ei[1]*Rij*Rij)/ECalTargetDist;  //Need to use the correct beam energy
  //  std::cout<<"Size of vectors "<<Ei.size()<<" "<<PosX.size()<<" "<<Ei[0]<<" "<<PosY[0]<<" "<<Zv<<" Mgg "<<Mgg<<std::endl;
  return Mgg;
}

// Be Carefull this only work for 2020 430 MeV data set. MR 04/2022
Bool_t Is22GGAnalysis::CheckThetaAngle(double Ei, double Thetai)
{
  //Uses a pol4 parameterization for the Theta energy correlation
  Double_t Par[5];
  Double_t ERun = fBeamE;
  
  if(ERun<220.){
    if(Ei<70.) return false;
    if(Ei>140.) return false;
    if(Thetai<0.055) return false;
  }else if(ERun>250. and ERun<270.){
    if(Ei<90.) return false;
    if(Ei>190.) return false;
    if(Thetai<0.045) return false;
  }else if(ERun>410.){  
    Par[0]= 0.1567;
    Par[1]=-1E-3;
    Par[2]= 3.4E-6;
    Par[3]=-5.95E-9;
    Par[4]= 3.87E-12;
    //  //Upper line cut
    Double_t fP4High=Par[0]+0.01+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
    if(Thetai>fP4High) return false;
    // lower line cut
    Double_t fP4Low=Par[0]-0.01+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
    //    cout<<"Ei "<<Ei<<" FBema "<<fBeamE<<" Theta "<<Thetai<<endl;
    if(Thetai<fP4Low) return false;
    if(Thetai<0.032) return false;
  }else if(ERun>390. && ERun<410.){  
    Par[0]= 0.1567;
    Par[1]=-1E-3;
    Par[2]= 3.4E-6;
    Par[3]=-5.95E-9;
    Par[4]= 3.87E-12;
    //  //Upper line cut
    Double_t fP4High=Par[0]+0.0075+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
    if(Thetai>fP4High) return false;
    // lower line cut
    Double_t fP4Low=Par[0]-0.015+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
    //    cout<<"Ei "<<Ei<<" FBema "<<fBeamE<<" Theta "<<Thetai<<endl;
    if(Thetai<fP4Low) return false;
    if(Thetai<0.032) return false;
  }  
  return true;
}


TLorentzVector Is22GGAnalysis::FillTLorentzVector(double XPart,double YPart,double E)
{
    TLorentzVector P4;
    Double_t DistCal=ECalTargetDist;
    // Calculate the momentum magnitude
    Double_t P = std::sqrt(E*E - me*me);

    // Calculate the momentum components
    Double_t Px = P * (XPart / DistCal);
    Double_t Py = P * (YPart / DistCal);
    Double_t Pz = std::sqrt(P*P - Px*Px - Py*Py);

    // Fill the TLorentzVector
    P4.SetPxPyPzE(Px,Py,Pz,E);
    return P4;
}

TLorentzVector Is22GGAnalysis::TransformToRestFrame(TLorentzVector& P4beam,TLorentzVector& P4Part)
{
    // Calculate the total four-momentum in the lab frame
  TLorentzVector P4eRest; 
  P4eRest.SetPxPyPzE(0.,0.,0.,me);
  TLorentzVector labPTotal = P4beam + P4eRest;

    // Calculate the total energy and momentum magnitude in the lab frame
    Double_t labETotal = labPTotal.E();
    Double_t labPTotalMag = labPTotal.P();

    // Calculate the boost velocity components
    Double_t beta = labPTotalMag / labETotal;
    Double_t bx = beta * P4beam.Px() / P4beam.P();
    Double_t by = beta * P4beam.Py() / P4beam.P();
    Double_t bz = beta * P4beam.Pz() / P4beam.P();

    // Perform the Lorentz transformation
    TLorentzVector restP4Part = P4Part;
    restP4Part.Boost(-bx,-by,-bz);

    // Print the components of the four-momentum vector in the rest frame
//    std::cout << "Rest frame four-momentum vector: "
//              << "E = " << restP4Part.E()
//              << ", Px = " << restP4Part.Px()
//              << ", Py = " << restP4Part.Py()
//              << ", Pz = " << restP4Part.Pz() << std::endl;

    // Return the transformed four-momentum of the particle in the rest frame
    return restP4Part;
}

Bool_t Is22GGAnalysis::IsMCGG(double VTime,double E1,double E2)
{
  Bool_t isGG_IN = false;
  Int_t NGG_MCBunch=0;
  Double_t vEgMC[2];
  Double_t vEgData[2];
  Double_t VTimeMC;
  Double_t TimeOfFlight=13.2;
  Double_t DeltaTCorr =0;
  vEgData[0]=E1;
  vEgData[1]=E2;
  // Is there a gg in the MC bunch?
  if(!fEvent->MCTruthEvent->GetNVertices()>0) return false;
  
  for(Int_t iV = 0; iV<fEvent->MCTruthEvent->GetNVertices(); iV++) {
    mcVtx = fEvent->MCTruthEvent->Vertex(iV);
    VTimeMC = mcVtx->GetTime();
    //remove non gg verices
    if(!(mcVtx->GetProcess()=="annihil")) continue; //be carefull if you are not using G4 generated events
    //std::cout<<"Entering "<<mcVtx->GetProcess()<<" data time "<<VTime<<" MC Time "<<VTimeMC<<std::endl;
    //Check if you are in time with the gamma gg verices

    DeltaTCorr=VTimeMC+TimeOfFlight-VTime;
    //    if(!(fabs(DeltaTCorr)<5.)) continue;
    fHS->FillHistoList("GGAnalysis","TDiff_DataMC_gg",DeltaTCorr);      
    for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
      TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
      if(mcOPart->GetPDGCode() != 22) cout<<"not a gamma"<<endl;
      //      std::cout<<"input data are simulatetd Annihilltion "<< mcOPart->GetEnergy()<<std::endl;
      vEgMC[iO]=mcOPart->GetEnergy();
    }
    //    std::cout<<"Energies Data "<<E1<<" "<<E2<<"  MC "<<vEgMC[0]<<" "<<vEgMC[1]<<std::endl;
    Int_t ncombo=0;
    Int_t NOk=0;
    Double_t DeltaE[4];
    for(Int_t kk=0;kk<2;kk++){
      for(Int_t jj=0;jj<2;jj++){
	DeltaE[ncombo] = vEgMC[kk]-vEgData[jj];
	//	std::cout<<"combo kk "<<kk<<" jj "<<jj<<" Ediff "<<DeltaE[ncombo]<<std::endl;
	fHS->FillHistoList("GGAnalysis","EDiff_DataMC_gg",DeltaE[ncombo]);   
	if(fabs(DeltaE[ncombo]/vEgData[jj])<0.2){
	  //	  std::cout<<"Matching "<<fabs(DeltaE[ncombo]/vEgData[jj])<<" "<<vEgData[jj]<<std::endl;
	  isGG_IN=true;
	} 
	ncombo++;
      }
    }
  }
  //  std::cout<<"*** "<<std::endl;
  return isGG_IN;
}
