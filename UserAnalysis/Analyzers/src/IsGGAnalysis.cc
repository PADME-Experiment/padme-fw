// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 6/04/2022
#include "IsGGAnalysis.hh"
#include "TF1.h"

IsGGAnalysis::IsGGAnalysis(TString cfgFile, Int_t verbose)
{
  Bool_t fisMC = false; 
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating IsGGAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  NGG=0;
  fHS = HistoSvc::GetInstance();
  fECalCalib = ECalCalib::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fMCTruth = MCTruth::GetInstance();
  // Standard cuts list
  MinECluster=50.;
  MaxECluster=400.;

  TMin=-170.;
  TMax= 130.;
  TWin= 5.;
//  ClRadMin= 100.;
//  ClRadMax= 250.;

  ClRadMin= 90.;
  ClRadMax= 270.;
  
  COGMax  = 30.;
  DistMax = 150.;

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

  fHS->BookHisto2List("GGAnalysis","E1vsE2_BadVtx",hEBins/2,0.,hEMax/2,hEBins/2,0.,hEMax/2);

  fHS->BookHistoList("GGAnalysis","ETotECal"                         ,hEBins,0.,hEMax);
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
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime",200,20.,20.);  
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime_COG",200,20.,20.);  
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime_COG_Dist",200,20.,20.); 
  fHS->BookHistoList("GGAnalysis","TCluDiff2g_Intime_COG_Dist_Enth",200,20.,20.);  
  
  fHS->BookHistoList("GGAnalysis","COG_X",300,-150.,150.);   
  fHS->BookHistoList("GGAnalysis","COG_Y",300,-150.,150.);   
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
  fHS->BookHistoList("GGAnalysis","MGG_COG"       ,NBinM,0.,MaxM);
  fHS->BookHistoList("GGAnalysis","MGG_COG_Dist"  ,NBinM,0.,MaxM);
  fHS->BookHistoList("GGAnalysis","MGG_EnnvsTheta",NBinM,0.,MaxM);   

  fHS->BookHistoList("GGAnalysis","ClClDist_COG",300,0.,600.);   
  fHS->BookHistoList("GGAnalysis","ECalClTime_GG",500,-250.,250.);

  fHS->BookHisto2List("GGAnalysis","EnvsTheta",250,0.,500.,100,0.,0.1); 
  fHS->BookHisto2List("GGAnalysis","EnvsTheta_cut",250,0.,500.,100,0.,0.1); 

  fHS->BookHisto2List("GGAnalysis","EnvsTimeDiff",400,-200.,200.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsTime",200,-200.,200.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnvsTimeScaled",200,-200.,200.,240,0.,1.2);

  fHS->BookHisto2List("GGAnalysis","EnvsTimeDiff_2g",300,-150.,150.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","COG_Map",300,-150.,150.,300,-150.,150.);
  fHS->BookHisto2List("GGAnalysis","COG_MapCut",200,-100.,100.,200,-100.,100.);
  fHS->BookHisto2List("GGAnalysis","Chi2vsETot",100,0.,20.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap",150,-300.,300.,150,-300.,300.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap_GG",150,-300.,300.,150,-300.,300.);
  fHS->BookHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",150,-300.,300.,150,-300.,300.);

  fHS->BookHisto2List("GGAnalysis","EpairvsZv",500,-5000.,0.,125,0.,500.);
  fHS->BookHisto2List("GGAnalysis","EpairvsZv_allCuts",500,-5000.,0.,125,0.,500.);
  
  fHS->BookHistoList("GGAnalysis","NClusters_final",25,-0.5,24.5);
  fHS->BookHistoList("GGAnalysis","NGoodClusters_final",25,-0.5,24.5);

  return true;
}

Bool_t IsGGAnalysis::Process(){
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

  
  //Set Some variables at the first event.
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber();

  if(NEvent==0){
    //    cout<<" Entering IsGG "<<NEvent<<" ismC"<<fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)<<endl;
    if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
      fisMC=true;
    }
    if(fisMC) fBeamE = fMCTruth->GetBeamEnergy(); 
    if(!fisMC) fBeamE = fECalCalib->GetBeamEnergy();
    if(fBeamE==0 && !fisMC) fBeamE = 432.5;
  }

  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  if(!fisMC && NEvent%10000==0) cout<<"NEvent "<<NEvent<<endl;
  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && fisMC==false) return false;

  //Calo-Calo checks
  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();
  //***************************************
  // Cut on at least 2 clusters
  //***************************************
  if(NClusters<2) return false;
  ETotECal=0;
  if(NClusters >25) cout<<"Crazy amount of clusters "<<NClusters<<endl;
  for(int ical = 0;ical < NClusters; ical++) {
    //    if(NEvent==5) cout<<"GG Energy "<<fEvent->ECalRecoCl->Element(ical)->GetEnergy()<<endl;
    double eECal    =  fEvent->ECalRecoCl->Element(ical)->GetEnergy();
    double tECal    =  fEvent->ECalRecoCl->Element(ical)->GetTime();
    TVector3 pos    =  fEvent->ECalRecoCl->Element(ical)->GetPosition();
    double ClRadius = sqrt(pos.X()*pos.X()+pos.Y()*pos.Y());
    ETotECal+=eECal;
    fHS->FillHistoList("GGAnalysis","ETotECal",ETotECal,1);    
    fHS->FillHistoList("GGAnalysis","NClusters",NClusters,1);
    fHS->FillHistoList("GGAnalysis","ECalClEnergy",eECal,1);
    fHS->FillHistoList("GGAnalysis","ECalClTime",tECal,1);
    fHS->FillHisto2List("GGAnalysis","EnergyMap",pos.X(),pos.Y(),eECal);
    fHS->FillHistoList("GGAnalysis","ClusterRadius",ClRadius,1);
    
    //Data cut on cluster energy now it's calibrated
    if(eECal<MinECluster) continue; 
    if(eECal>MaxECluster) continue; 

    //Cut on cluster radius Min and Max
    if(ClRadius<ClRadMin) continue;
    if(ClRadius>ClRadMax) continue;

    //compute angles
    Double_t ECalTargetDist=3543;
    Double_t Angle=sqrt(pos.X()*pos.X()+pos.Y()*pos.Y())/ECalTargetDist;

    bool GGCheck=CheckThetaAngle(eECal,Angle);
    int IsGGCheckOk=-1;
    if(GGCheck) IsGGCheckOk=1;

    EGoodCluster.push_back(eECal);
    TGoodCluster.push_back(tECal);
    PosXGoodCluster.push_back(pos.X());
    PosYGoodCluster.push_back(pos.Y());
    AngleGoodCluster.push_back(Angle);
    IsGGGoodCluster.push_back(IsGGCheckOk);
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
      Double_t COGX = CompCOG(vEi,vPosX); Double_t COGY = CompCOG(vEi,vPosY);
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
      if(Dist<DistMax) continue; 
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

      //Check if it's a good GG cluster
      if(IsGGGoodCluster[jj]>0 && IsGGGoodCluster[kk]>0){ 
	fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist_Enth",EGoodCluster[kk]+EGoodCluster[jj],1);
	fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime_COG_Dist_Enth",TGoodCluster[kk]-TGoodCluster[jj],1);
	fHS->FillHistoList("GGAnalysis","ZVertex_EnnvsTheta",Zv,1);
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_cut",EGoodCluster[kk],AngleGoodCluster[kk],1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_cut",EGoodCluster[jj],AngleGoodCluster[jj],1.);  
      }else{      
	fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[jj],PosYGoodCluster[jj],EGoodCluster[jj]);
	fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[kk],PosYGoodCluster[kk],EGoodCluster[kk]);
	fHS->FillHisto2List("GGAnalysis","E1vsE2_BadVtx",EGoodCluster[jj],EGoodCluster[kk]);
      }

      Double_t Angle         = sqrt(PosXGoodCluster[jj]*PosXGoodCluster[jj]+PosYGoodCluster[jj]*PosYGoodCluster[jj])/fabs(Zv);
      Double_t Angle1        = sqrt(PosXGoodCluster[kk]*PosXGoodCluster[kk]+PosYGoodCluster[kk]*PosYGoodCluster[kk])/fabs(Zv);

      bool G1Check_Displaced = CheckThetaAngle(EGoodCluster[jj],Angle);
      bool G2Check_Displaced = CheckThetaAngle(EGoodCluster[kk],Angle1);
      //      cout<<"Angle "<<Angle<<" Angle1 "<<Angle1<<" ZV "<<Zv<<endl;
      
      if(G1Check_Displaced && G2Check_Displaced & abs(EGoodCluster[kk]+EGoodCluster[jj]-fBeamE)/14<3){ 
      	fHS->FillHistoList("GGAnalysis","ZVertex_EnnvsTheta_displaced",Zv,1);
      }
      NPairs++;      
    }
  }
  //**************************
  // Cut on at least good pair
  //**************************
  if(NPairs<1) return false;

  fHS->FillHistoList("GGAnalysis","NClusters_final",NClusters);
  fHS->FillHistoList("GGAnalysis","NGoodClusters_final",NGoodClusters);
  fHS->FillHistoList("GGAnalysis","NPairs",NPairs,1);

  double chi2 = 0;
  for(Int_t ll=0;ll<NPairs;ll++){
    if(fisMC && NPairs>1) IsMCGG(EGoodCluster[PairGClIndex1[ll]],EGoodCluster[PairGClIndex2[ll]]);
    if(NPairs==1) {
      Double_t EPair      = EGoodCluster[PairGClIndex1[ll]] + EGoodCluster[PairGClIndex2[ll]];
      Double_t DeltaTPair = TGoodCluster[PairGClIndex1[ll]] - TGoodCluster[PairGClIndex2[ll]];
      Double_t TimePair   = (TGoodCluster[PairGClIndex1[ll]] + TGoodCluster[PairGClIndex2[ll]])/2;
      fHS->FillHistoList("GGAnalysis","ECalClTime_GG",TimePair,1);
      //Event quality estimator 
      chi2 = ( fabs((EPair-fBeamE)/15) + fabs(DeltaTPair/1.2) +fabs(PairCOGX[ll]/8.) + fabs(PairCOGY[ll]/8.) )/4; 
      fHS->FillHistoList("GGAnalysis","EG1_final",EGoodCluster[PairGClIndex1[ll]],1);
      fHS->FillHistoList("GGAnalysis","EG2_final",EGoodCluster[PairGClIndex2[ll]],1);
      fHS->FillHisto2List("GGAnalysis","Chi2vsETot",chi2,EPair,1.);

      fHS->FillHisto2List("GGAnalysis","EpairvsZv_allCuts",PairVertex[ll],EPair,1.);
      if(PosXGoodCluster[PairGClIndex1[0]]>0 && PosYGoodCluster[PairGClIndex1[ll]]>0) fHS->FillHistoList("GGAnalysis","EGG_topRight",EPair,1);
      if(PosXGoodCluster[PairGClIndex1[0]]<0 && PosYGoodCluster[PairGClIndex1[ll]]>0) fHS->FillHistoList("GGAnalysis","EGG_topLeft",EPair,1);
      if(PosXGoodCluster[PairGClIndex1[0]]>0 && PosYGoodCluster[PairGClIndex1[ll]]<0) fHS->FillHistoList("GGAnalysis","EGG_botRight",EPair,1);
      if(PosXGoodCluster[PairGClIndex1[0]]<0 && PosYGoodCluster[PairGClIndex1[ll]]<0) fHS->FillHistoList("GGAnalysis","EGG_botLeft",EPair,1);
      
      //Use only after general scale calibration
      if(abs(EPair-fBeamE)/15<4){ 
	fHS->FillHistoList("GGAnalysis","ZVertex_EPair",PairVertex[ll],1);
	fHS->FillHisto2List("GGAnalysis","EnvsTime",TimePair,EPair,1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTimeScaled",TimePair,EPair/fBeamE,1.);
	//looking for bad events
	if(Zv>-2500.){
	  fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[PairGClIndex1[ll]],PosYGoodCluster[PairGClIndex1[ll]],EGoodCluster[PairGClIndex1[ll]]);
	}
      }
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

Bool_t IsGGAnalysis::Finalize()
{
 // TGraph* nPotVsTime = new TGraph((Int_t)vNPoT.size(),&vNEvt[0],&vNPoT[0]);
 // fHS->SaveTGraphList("GGAnalysis","NPotVsTime",nPotVsTime);
  std::cout<<"TotGG        = "<<NGG<<std::endl;
  std::cout<<"TotGG_MC     = "<<NGG_MC<<std::endl;
  if (fVerbose) printf("---> Finalizing IsGGAnalysis\n");
  return true;
}


double IsGGAnalysis::CompCOG(std::vector<double> Ei,std::vector<double> Posi)
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
  Zv = -sqrt(Ei[0]*Ei[1]*Rij*Rij)/sqrt(2*0.5111*fBeamE);  //Need to use the correct beam energy
  //  std::cout<<"Size of vectors "<<Ei.size()<<" "<<PosX.size()<<" "<<Ei[0]<<" "<<PosY[0]<<" "<<Zv<<" EBeam "<<fBeamE<<std::endl;
  return Zv;
}


double IsGGAnalysis::ComputeInvariantMass(std::vector<double> Ei,std::vector<double> PosX,std::vector<double> PosY)
{
  Double_t Mgg=0;
  Double_t Zv  = 3534; //use some kind of geometry plz
  Double_t Rij = sqrt( (PosX[0]-PosX[1])*(PosX[0]-PosX[1]) + (PosY[0]-PosY[1])*(PosY[0]-PosY[1]) ); 
  Mgg = sqrt(Ei[0]*Ei[1]*Rij*Rij)/Zv;  //Need to use the correct beam energy
  //  std::cout<<"Size of vectors "<<Ei.size()<<" "<<PosX.size()<<" "<<Ei[0]<<" "<<PosY[0]<<" "<<Zv<<" Mgg "<<Mgg<<std::endl;
  return Mgg;
}

// Be Carefull this only work for 2020 430 MeV data set. MR 04/2022
Bool_t IsGGAnalysis::CheckThetaAngle(double Ei, double Thetai)
{
  //Uses a pol4 parameterization for the Theta energy correlation
  Double_t Par[5];
  
  Par[0]= 0.1567;
  Par[1]=-1E-3;
  Par[2]= 3.4E-6;
  Par[3]=-5.95E-9;
  Par[4]= 3.87E-12;

  //Upper line cut
  Double_t fP4High=Par[0]+0.01+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
  if(Thetai>fP4High) return false;
  // lower line cut
  Double_t fP4Low=Par[0]-0.01+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
  //  cout<<"Ei "<<Ei<<" Fp4L "<<fP4Low->Eval(Ei)<<" Theta "<<Thetai<<endl;
  if(Thetai<fP4Low) return false;  
  return true;
}


Bool_t IsGGAnalysis::IsMCGG(double E1,double E2)
{
  Bool_t isGG_IN = false;
  Int_t NGG_MCBunch=0;
  Double_t EG1,EG2;
  // Is there a gg in the MC bunch?
  if(!fEvent->MCTruthEvent->GetNVertices()>0) return false;

  for(Int_t iV = 0; iV<fEvent->MCTruthEvent->GetNVertices(); iV++) {
    mcVtx = fEvent->MCTruthEvent->Vertex(iV);
    if(!(mcVtx->GetProcess()=="annihil")) continue;
    for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
      TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
      if(mcOPart->GetPDGCode() != 22) cout<<"e che cazzo"<<endl;
      //      std::cout<<"input data are simulatetd Annihilltion "<< mcOPart->GetEnergy()<<std::endl;
      if(iO==0) EG1=mcOPart->GetEnergy();
      if(iO==1) EG2=mcOPart->GetEnergy();
      isGG_IN = true;
      if(iO==1){
	NGG_MC++;
	NGG_MCBunch++;
//	std::cout<<"input data are simulatetd Annihilltion "<<NGG_MC<<" Iv "<<iV<<" NGG "<<NGG_MCBunch<<std::endl;
//	std::cout<<"EG reco  "<<E1<<" "<<E2<<" Eg true "<<EG1<<" "<<EG2<<std::endl;
      }
    }
  }
  std::cout<<"*** "<<std::endl;
  return isGG_IN;
}
