// Written by M. Raggi   20/07/2021 
// Last modified by M. Raggi 6/04/2022
#include "IsGGAnalysis.hh"
#include "TF1.h"

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
  
  fHS->BookHistoList("GGAnalysis","COG_X",300,-150.,150.);   
  fHS->BookHistoList("GGAnalysis","COG_Y",300,-150.,150.);   
  fHS->BookHistoList("GGAnalysis","ClClDist",300,0.,600.);   

  fHS->BookHistoList("GGAnalysis","ZVertex",500,-5000.,0.);
  fHS->BookHistoList("GGAnalysis","ZVertex_COG",500,-5000.,0.);
  fHS->BookHistoList("GGAnalysis","ZVertex_COG_Dist",500,-5000.,0.);
  fHS->BookHistoList("GGAnalysis","ZVertex_allCut",500,-5000.,0.);   
  fHS->BookHistoList("GGAnalysis","ZVertex_EnnvsTheta",500,-5000.,0.);   
  fHS->BookHistoList("GGAnalysis","ZVertex_EPair",500,-5000.,0.);   
  
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
  //  cout<<""
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
 
  vEi.clear();
  vPosX.clear();
  vPosY.clear();

  //Check if is MC or data
  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber(); 

  if(NEvent%10000==0) cout<<"NEvent "<<NEvent<<endl;
  
  //Cut on physics trigger Data Only
  if( !(trigMask & (1 << 0)) && isMC==false) return false;

  //Calo-Calo checks
  Int_t NClusters =fEvent->ECalRecoCl->GetNElements();

  //***************************************
  // Cut on at least 2 clusters
  //***************************************
  if(NClusters<2) return false;
  ETotECal=0;
  if(NClusters >25) cout<<"Crazy amount of clusters "<<NClusters<<endl;
  for(int ical = 0;ical < NClusters; ical++) {
    if(NEvent==5) cout<<"GG Energy "<<fEvent->ECalRecoCl->Element(ical)->GetEnergy()<<endl;
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

    //Cut on cluster radius Min and Max
    if(ClRadius<ClRadMin) continue;
    if(ClRadius>ClRadMax) continue;

    //compute angles
    Double_t ECalTargetDist=3543;
    Double_t Angle=sqrt(pos.X()*pos.X()+pos.Y()*pos.Y())/ECalTargetDist;

    EGoodCluster.push_back(eECal);
    TGoodCluster.push_back(tECal);
    PosXGoodCluster.push_back(pos.X());
    PosYGoodCluster.push_back(pos.Y());
    AngleGoodCluster.push_back(Angle);
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
 
  //BUG on dimensions of the arrays
  for(Int_t kk=0;kk<NGoodClusters-1;kk++){
    for(Int_t jj=kk+1;jj<NGoodClusters;jj++){
      fHS->FillHistoList("GGAnalysis","TClTimeDiff2g",TGoodCluster[kk]-TGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g",EGoodCluster[kk]+EGoodCluster[jj],1);

      // have a look at the out of time Events to cross check 
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
      Double_t COGX = GetCOG(vEi,vPosX); Double_t COGY = GetCOG(vEi,vPosY);

      fHS->FillHistoList("GGAnalysis","COG_X",COGX,1);
      fHS->FillHistoList("GGAnalysis","COG_Y",COGY,1);
      fHS->FillHisto2List("GGAnalysis","COG_Map",COGX,COGY,1);

      // Compute the Z coordinate of the interaction Vertex using mass assumption sqrt(2*me*430MeV)
      Zv = GetVertex(vEi,vPosX,vPosY);  //occhio che funziona solo a due

      fHS->FillHistoList("GGAnalysis","ZVertex",Zv,1);
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
      fHS->FillHistoList("GGAnalysis","TCluDiff2g_Intime_COG",TGoodCluster[kk]+TGoodCluster[jj],1);
      fHS->FillHistoList("GGAnalysis","ZVertex_COG",Zv,1);

      //***********************
      // CUT ON PAIR Dist
      //***********************

      if(Dist<DistMax) continue; 
      fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist",EGoodCluster[kk]+EGoodCluster[jj],1);
      PairGClIndex1.push_back(kk);      
      PairGClIndex2.push_back(jj);
      PairCOGX.push_back(COGX);
      PairCOGY.push_back(COGY);
      PairDist.push_back(Dist);
      PairVertex.push_back(Zv);

      fHS->FillHisto2List("GGAnalysis","EnvsTheta",EGoodCluster[kk],AngleGoodCluster[kk],1.);
      fHS->FillHisto2List("GGAnalysis","EnvsTheta",EGoodCluster[jj],AngleGoodCluster[jj],1.); 
      fHS->FillHistoList("GGAnalysis","ZVertex_COG_Dist",Zv,1);

      bool G1Check=CheckThetaAngle(EGoodCluster[kk],AngleGoodCluster[kk]);
      bool G2Check=CheckThetaAngle(EGoodCluster[jj],AngleGoodCluster[jj]);
      if(G1Check && G2Check){ 
	fHS->FillHistoList("GGAnalysis","ECalClEnergy2g_Intime_COG_Dist_Enth",EGoodCluster[kk]+EGoodCluster[jj],1);
	fHS->FillHistoList("GGAnalysis","ZVertex_EnnvsTheta",Zv,1);
	//
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_cut",EGoodCluster[kk],AngleGoodCluster[kk],1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTheta_cut",EGoodCluster[jj],AngleGoodCluster[jj],1.); 
	fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[jj],PosYGoodCluster[jj],EGoodCluster[jj]);
	fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[kk],PosYGoodCluster[kk],EGoodCluster[kk]);
	fHS->FillHisto2List("GGAnalysis","E1vsE2_BadVtx",EGoodCluster[jj],EGoodCluster[kk]);
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
    if(NPairs==1) {
      Double_t EPair      = EGoodCluster[PairGClIndex1[ll]] + EGoodCluster[PairGClIndex2[ll]];
      Double_t DeltaTPair = TGoodCluster[PairGClIndex1[ll]] - TGoodCluster[PairGClIndex2[ll]];
      Double_t TimePair   = (TGoodCluster[PairGClIndex1[ll]] + TGoodCluster[PairGClIndex2[ll]])/2;
      fHS->FillHistoList("GGAnalysis","ECalClTime_GG",TimePair,1);
      //Event quality estimator 
      chi2 = ( fabs((EPair-430.)/15) + fabs(DeltaTPair/1.2) +fabs(PairCOGX[ll]/8.) + fabs(PairCOGY[ll]/8.) )/4; 
      fHS->FillHistoList("GGAnalysis","EG1_final",EGoodCluster[PairGClIndex1[ll]],1);
      fHS->FillHistoList("GGAnalysis","EG2_final",EGoodCluster[PairGClIndex2[ll]],1);
      fHS->FillHisto2List("GGAnalysis","Chi2vsETot",chi2,EPair,1.);

      fHS->FillHisto2List("GGAnalysis","EpairvsZv_allCuts",PairVertex[ll],EPair,1.);
      //Use only after general scale calibration
      if(EPair>400.){ 
	fHS->FillHistoList("GGAnalysis","ZVertex_EPair",PairVertex[ll],1);
	fHS->FillHisto2List("GGAnalysis","EnvsTime",TimePair,EPair,1.);
	fHS->FillHisto2List("GGAnalysis","EnvsTimeScaled",TimePair,EPair/430,1.);
	//lloking for bad events
	if(Zv>-2800.){
	  fHS->FillHisto2List("GGAnalysis","EnergyMap_GG_BadVtx",PosXGoodCluster[PairGClIndex1[ll]],PosYGoodCluster[PairGClIndex1[ll]],EGoodCluster[PairGClIndex1[ll]]);
	}
      }
    }
  }

//  if(PosXClusterPair[0]>0 && PosYClusterPair[0]>0) fHS->FillHistoList("GGAnalysis","EGG_topRight",EPair,1);
//  if(PosXClusterPair[0]<0 && PosYClusterPair[0]>0) fHS->FillHistoList("GGAnalysis","EGG_topLeft",EPair,1);
//
//  if(PosXClusterPair[0]>0 && PosYClusterPair[0]<0) fHS->FillHistoList("GGAnalysis","EGG_botRight",EPair,1);
//  if(PosXClusterPair[0]<0 && PosYClusterPair[0]<0) fHS->FillHistoList("GGAnalysis","EGG_botLeft",EPair,1);
  NGG++;
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


// Be Carefull this only work for 2020 430 MeV data set. MR 04/2022
Bool_t IsGGAnalysis::CheckThetaAngle(double Ei, double Thetai)
{
  //Uses a pol4 parameterization for the Theta energy correlations
  Double_t Par[5];
  
  Par[0]= 0.1567;
  Par[1]=-1E-3;
  Par[2]= 3.4E-6;
  Par[3]=-5.95E-9;
  Par[4]= 3.87E-12;

  //Upper line cut
//  TF1 *fP4High= new TF1("fP4H","[0]+x*[1]+x*x*[2]+x*x*x*[3]+x*x*x*x*[4]",50,450);
//  Param[0]+= 0.01;
//  fP4High->SetParameters(Param);
//  //  cout<<"Ei "<<Ei<<" Fp4H "<<fP4High->Eval(Ei)<<" Theta "<<Thetai<<endl;
//  if(Thetai>fP4High->Eval(Ei)) return false;
  Double_t fP4High=Par[0]+0.01+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
  if(Thetai>fP4High) return false;
  // lower line cut
  //Upper line cut
  //  TF1 *fP4Low= new TF1("fP4L","[0]+x*[1]+x*x*[2]+x*x*x*[3]+x*x*x*x*[4]",50,450);
  Double_t fP4Low=Par[0]-0.01+Ei*Par[1]+Ei*Ei*Par[2]+Ei*Ei*Ei*Par[3]+Ei*Ei*Ei*Ei*Par[4];
  //  Param[0]-= 0.02;
  //  fP4Low->SetParameters(Param);
  //  cout<<"Ei "<<Ei<<" Fp4L "<<fP4Low->Eval(Ei)<<" Theta "<<Thetai<<endl;
  if(Thetai<fP4Low) return false;
  
  return true;
}
