#include "ECalAnalysis.hh"
#include "TECalRecoEvent.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include <iostream>

ECalAnalysis::ECalAnalysis(): ValidationBase()
{
  fAlgoName = "ECal";
  InitHistos();
  return;
}
ECalAnalysis::ECalAnalysis(Int_t processingMode, Int_t verbosityFlag): ValidationBase(processingMode, verbosityFlag)
{
  fAlgoName = "ECal";
  InitHistos();
  InitHistosSelfCalibration();
  return;
}
ECalAnalysis::~ECalAnalysis()
{
}

Bool_t ECalAnalysis::InitHistosSelfCalibration(){
  fOfflineServerDB = OfflineServer::GetInstance();
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  hSvcVal->BookHisto2(Form("ECal_SC_DrVsDt"), 800, -400,400, 200, 0, 600.);
  hSvcVal->BookHisto2(Form("ECal_SC_EVsT"), 800, -400,400, 200, 0, 600.);
  hSvcVal->BookHisto2(Form("ECal_SC_DE1VsDE2"), 800, -400,400, 800, -400, 400.);
  hSvcVal->BookHisto2(Form("ECal_SC_EExpVsE"), 800, 0,400, 400, 0, 400.);
  for (int q=0; q<6; q++){
    hSvcVal->BookHisto2(Form("ECal_SC_EExpVsE_q%d",q), 800, 0,400, 400, 0, 400.);
  }
  fCalibTree = hSvcVal->BookNtuple("ECal_SC_Tree");
  fCalibTree->Branch("RunID", &(fCalibTreeStruct.RunID), "RunID/I");
  fCalibTree->Branch("EventID", &(fCalibTreeStruct.EventID), "EventID/I");
  fCalibTree->Branch("EventTime", &(fCalibTreeStruct.EventTime), "EventTime/l");
  fCalibTree->Branch("Sqrts", &(fCalibTreeStruct.Sqrts), "Sqrts/D");
  fCalibTree->Branch("Temp", &(fCalibTreeStruct.Temp), "Temp/D");
  fCalibTree->Branch("BunchLength", &(fCalibTreeStruct.BunchLength), "Temp/D");
  fCalibTree->Branch("BeamStart", &(fCalibTreeStruct.BeamStart), "Temp/D");
  fCalibTree->Branch("RunTime", &(fCalibTreeStruct.RunTime), "Temp/D");
  fCalibTree->Branch("RunPOT", &(fCalibTreeStruct.RunPOT), "Temp/D");
  fCalibTree->Branch("TargX", &(fCalibTreeStruct.TargX), "TargX/D");
  fCalibTree->Branch("TargY", &(fCalibTreeStruct.TargY), "TargY/D");
  fCalibTree->Branch("TClus", &(fCalibTreeStruct.TClus), "TClus[2]/D");
  fCalibTree->Branch("EClus", &(fCalibTreeStruct.EClus), "EClus[2]/D");
  fCalibTree->Branch("XClus", &(fCalibTreeStruct.XClus), "XClus[2]/D");
  fCalibTree->Branch("YClus", &(fCalibTreeStruct.YClus), "YClus[2]/D");
  fCalibTree->Branch("EExp", &(fCalibTreeStruct.EExp), "EExp[2]/D");

  return kTRUE;
}

Bool_t ECalAnalysis::InitHistosAnalysis()
{
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  std::string hname;
  int nBin, min, max;
  nBin=250;
  min=0.;
  max=500;
  hname="ECal_NHits";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=500;
  min=0.;
  max=1000;
  hname = "ECal_HitEnergy";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=5000;
  min=0.;
  max=10000;
  hname="ECal_EHitTot";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=250;
  min=0.;
  max=250;
  hname = "ECal_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=51;
  min=-0.5;
  max=50.5;
  hname = "ECal_NHitsInClus";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=500;
  min=0.;
  max=1000;
  hname = "ECal_ClusterEnergy";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=5000;
  min=0.;
  max=10000;
  hname="ECal_EClTot";
  hSvcVal->BookHisto(hname, nBin, min, max);
  return true;
}

Bool_t ECalAnalysis::InitHistosValidation()
{
  //Validation histos
  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  std::string hname;
  int nBin, min, max;
  nBin=100;
  min=0;
  max=200;
  hname="ECal_NHits";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=50;
  min=0;
  max=50;
  hname="ECal_NCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=300;
  min=0;
  max=600;
  hname = "ECal_HitEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=350;
  min=0;
  max=700;
  hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest5ns";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=1000;
  min=0;
  max=15000;
  hname = "ECal_HitTotEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  hname = "ECal_ClusterTotEnergy";
  hSvcVal->BookHisto(hname,nBin,min, max);
  nBin=800;
  min=-400;
  max=400;
  hname = "ECal_HitTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  hname = "ECal_ClusterTime";
  hSvcVal->BookHisto(hname,nBin, min, max);
  nBin=200;
  min=-300;
  max=300;
  hname = "ECal_HitMap";
  hSvcVal->BookHisto2(hname, nBin, min, max, nBin, min, max);
  hname = "ECal_ClusterMap";
  hSvcVal->BookHisto2(hname, nBin, min, max, nBin, min, max);
  nBin=500;
  min=-300;
  max=300;
  hname="ECal_HitXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterXPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_HitYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterYPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=3;
  min=2545;
  max=2565;
  hname="ECal_HitZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterZPos";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=3000;
  min=0;
  max=3000;
  hname="ECal_HitChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  hname="ECal_ClusterSeedChannelId";
  hSvcVal->BookHisto(hname, nBin, min, max);
  nBin=16;
  min=0;
  max=15;
  hname="ECal_NHitInCluster";
  hSvcVal->BookHisto(hname, nBin, min, max);
  return true;
}


Bool_t ECalAnalysis::ProcessAnalysis()
{
  Bool_t retCode = 0;

  HistoSvc* hSvc =  HistoSvc::GetInstance();

  TRecoVHit* hit=NULL;
  TRecoVCluster* clu=NULL;
  std::string hname;
  Int_t      chId;
  Double_t energy;
  Double_t   time;
    
  Int_t fNhits = fhitEvent->GetNHits();
  Int_t fNclus = fClColl->GetNElements();
  Int_t seedId;
  Int_t clSize;

     //fillHitsFlatNTP

  (hSvc->myEvt).NTNECal_Hits=fhitEvent->GetNHits();
  
  for (Int_t i=0; i<fNhits; ++i){
    hit    = fhitEvent->Hit(i);
    chId   = hit->GetChannelId();
    energy = hit->GetEnergy();
    time   = hit->GetTime();

   (hSvc->myEvt).NTECal_Hits_ChannelId[i]=(Double_t)chId;
   (hSvc->myEvt).NTECal_Hits_Energy[i]=hit->GetEnergy();
   (hSvc->myEvt).NTECal_Hits_Time[i]=hit->GetTime();
   (hSvc->myEvt).NTECal_Hits_Xpos[i]=hit->GetPosition().X();
   (hSvc->myEvt).NTECal_Hits_Ypos[i]=hit->GetPosition().Y();
   (hSvc->myEvt).NTECal_Hits_Zpos[i]=hit->GetPosition().Z();
  }

    //fillClustersFlatNTP 

  (hSvc->myEvt).NTNECal_Clusters= fClColl->GetNElements(); 

   for (Int_t j=0; j<fNclus; ++j){
     clu    = fClColl->Element(j);
     seedId = clu->GetChannelId();
  
   (hSvc->myEvt).NTECal_Clusters_ChannelId[j]=Double_t(clu->GetChannelId());
   (hSvc->myEvt).NTECal_Clusters_Energy[j]=clu->GetEnergy();
   (hSvc->myEvt).NTECal_Clusters_Time[j]=clu->GetTime();
   (hSvc->myEvt).NTECal_Clusters_Xpos[j]=clu->GetPosition().X();
   (hSvc->myEvt).NTECal_Clusters_Ypos[j]=clu->GetPosition().Y();
   (hSvc->myEvt).NTECal_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

   hname = "ECal_NHits";
   hSvc->FillHisto(hname, fNhits,1.);
   Double_t eTot  =  0.;
   for (Int_t i=0; i<fNhits; ++i){
     hit = fhitEvent->Hit(i);
     Double_t energy = hit->GetEnergy();
     hname = "ECal_HitEnergy";
     hSvc->FillHisto(hname, energy, 1.);
     eTot += energy;
   }
   hname="ECal_EHitTot";
   hSvc->FillHisto(hname, eTot, 1.);

   
   Double_t eTotCl;
   hname = "ECal_NCluster";
   hSvc->FillHisto(hname, fNclus,1.);

   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     eTotCl += clu->GetEnergy();
     hname = "ECal_ClusterEnergy";
     hSvc->FillHisto(hname, clu->GetEnergy(), 1.);
     clSize = clu->GetNHitsInClus();
     hname = "ECal_NHitsInClus";
     hSvc->FillHisto(hname, clSize,1.);
   
   }
   hname="ECal_EClTot";
   hSvc->FillHisto(hname, eTotCl, 1.);


  // HistoSvc* hSvc =  HistoSvc::GetInstance();

  // Double_t eMax  =  0.;
  // Int_t    iLead = -1;
  // Double_t eTot  =  0.;
  // TRecoVHit* hit=NULL;
  // TRecoVHit* hitn=NULL;
  // std::string hname;
  // Int_t fNhits = fhitEvent->GetNHits();
  // std::cout<<"+++++++++++++++++++++++++++++++++++++++++ ECalAnalysis ... "<<fNhits<<std::endl;
  // for (Int_t i=0; i<fNhits; ++i){
  //   hit = fhitEvent->Hit(i);
  //   Int_t ix = hit->GetChannelId()/100;
  //   Int_t iy = hit->GetChannelId()%100;
  //   Double_t energy = hit->GetEnergy();
  //   eTot += energy;
  //   if (energy > eMax) {iLead = i; eMax=energy;}
  //   Double_t time = hit->GetTime();

  //   // ntuple variables
  //   hSvc->myEvt.NTECluster[i]=hit->GetEnergy();
  //   hSvc->myEvt.NTTCluster[i]=hit->GetTime();
  //   hSvc->myEvt.NTXCluster[i]=hit->GetChannelId()/100;
  //   hSvc->myEvt.NTYCluster[i]=hit->GetChannelId()%100;

  //  }

  //  if (fNhits<1) return true;
  //  TRecoVHit* lHit = fhitEvent->Hit(iLead);
  //  Int_t lix = lHit->GetChannelId()/100;
  //  Int_t liy = lHit->GetChannelId()%100;
  //  Double_t lenergy = lHit->GetEnergy();

  //  hname = "ECal_LeadingHitTime";
  //  hSvc->FillHisto(hname,lHit->GetTime(),1.);
  //  hname = "ECal_LeadingHitEnergy";
  //  hSvc->FillHisto(hname,lenergy,1.);
  //  hname = "ECal_LeadingHitMap";
  //  hSvc->FillHisto2(hname, lix, liy, 1.);
  //  hname = "ECal_LeadingEneMap";
  //  hSvc->FillHisto2(hname, lix, liy, lenergy);


  //  TRecoVCluster* clu=NULL;
  //  TRecoVCluster* clun=NULL;
  //  //std::cout<<"ECal sanity checks:"
  //  Int_t seed;
  //  Int_t seedId;
  //  Double_t seedE;
  //  Double_t seedT;
  //  Double_t eTotCl;
  //  Int_t clSize;
  //  Int_t fNclus = fClColl->GetNElements();
  //  for (Int_t i=0; i<fNclus; ++i){
  //    clu    = fClColl->Element(i);
  //    seed   = clu->GetSeed();
  //    seedId = clu->GetChannelId();
  //    Int_t checkseedId =  fhitEvent->Hit(seed)->GetChannelId();
  //    if ( checkseedId != seedId ) {
  //      std::cout<<"ERROR on ECal indices --- seed: clustID, hit(seed).id = "<<seed<<" "<<seedId<<" "<<checkseedId<<std::endl;
  //    }
  //    //     else
  //    //       {
  //    //	 std::cout<<"++++++++++++++++++++     Sanity check OK ... "<<std::endl;
  //    //       }
  //    seedE  = clu->GetEnergy();
  //    seedT  = clu->GetTime();
  //    clSize = clu->GetNHitsInClus();
  //    //std::cout<<"ECal:: cluster n. "<<i<<" size, seed  "<<clSize<<" "<<seed<<"  chId, E, T "<<seedId<<" "<<seedE<<" "<<seedT<<std::endl;
  //    std::vector<Int_t> vhinclu = clu->GetHitVecInClus();
  //    for (Int_t ih=0; ih<clSize; ++ih){
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<std::endl;
  //      Int_t hitInd    = vhinclu[ih];
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<std::endl;
  //      if (hitInd == seed) {std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<"- skipping seed"<<std::endl;continue;}
  //      hit = fhitEvent->Hit(hitInd);
  //      //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" hitIndex = "<<hitInd<<" chid, E, T "
  //      //	<<hit->GetChannelId()<<" "<<hit->GetEnergy()<<" "<<hit->GetTime()<<std::endl;
  //      hname = "ECal_inClus_DCHXseed";
  //      hSvc->FillHisto(hname,hit->GetChannelId()/100-seedId/100,1.);
  //      hname = "ECal_inClus_DCHYseed";
  //      hSvc->FillHisto(hname,hit->GetChannelId()%100-seedId%100,1.);
  //      hname = "ECal_inClus_DTseed";
  //      hSvc->FillHisto(hname,hit->GetTime()-seedT,1.);
  //      for (Int_t ihn=ih+1; ihn<clSize; ++ihn){
  // 	 Int_t hitIndn    = vhinclu[ihn];
  // 	 //if (hitIndn == seed) continue;
  // 	 hitn = fhitEvent->Hit(hitIndn);
  // 	 //std::cout<<"ECal:: cluster n. "<<i<<" hit comp. n "<<ih<<" vs comp in="<<ihn<<" chId, T "<<hitn->GetChannelId()<<" "<<hitn->GetTime()<<std::endl;
  // 	 hname = "ECal_inClus_DCHXhits";
  // 	 hSvc->FillHisto(hname,hit->GetChannelId()/100-hitn->GetChannelId()/100,1.);
  // 	 hname = "ECal_inClus_DCHYhits";
  // 	 hSvc->FillHisto(hname,hit->GetChannelId()%100-hitn->GetChannelId()%100,1.);
  // 	 hname = "ECal_inClus_DThits";
  // 	 hSvc->FillHisto(hname,hit->GetTime()-hitn->GetTime(),1.);
  //      }
  //    }
  //    for (Int_t in=0; in<fNclus; ++in){
  //      clun    = fClColl->Element(in);
  //      //
  //      hname = "ECal_Clus2Clus_seedDCHX";
  //      hSvc->FillHisto(hname,clun->GetChannelId()/100-clu->GetChannelId()/100,1.);
  //      hname = "ECal_Clus2Clus_seedDCHY";
  //      hSvc->FillHisto(hname,clun->GetChannelId()/100-clu->GetChannelId()%100,1.);
  //      hname = "ECal_Clus2Clus_seedDT";
  //      hSvc->FillHisto(hname,clun->GetTime()-clu->GetTime(),1.);
  //    }
  //  }


   return retCode;
}

Bool_t ECalAnalysis::SelfCalibration(TRecoEvent* recoEvent){
  int runID = recoEvent->GetRunNumber();
  int eventID = recoEvent->GetEventNumber();
  HistoSvc* hSvcVal =  HistoSvc::GetInstance(); // should be a private var
  double beamEnergy = fOfflineServerDB->getDHSTB01Energy(runID); // read from a db at least run-dependent
  double tempAvg = fOfflineServerDB->getAvgTemp(runID);

  const double me = 0.511;
  double sqrts = sqrt(2.*me*me + 2.*beamEnergy*me);
  double bg = beamEnergy/sqrts; // beta
  double gam = sqrt(bg*bg+1.);
  double beta = bg/gam;
  double xTarg;
  const ULong64_t startRunSep2020 = 1600256773;// first event of the first time 
  ULong64_t eventTime = static_cast<long int>(recoEvent->GetEventTime().GetSec());//fOfflineServerDB->getRunTime(runID)-startRunSep2020;

  ULong64_t deventTime = eventTime - startRunSep2020;

  // rough for x
  if (deventTime < 1380000) xTarg = 3.2  ; // -> should be from db
  else if (deventTime < 2500000) xTarg = 1.6  ; // -> should be from db
  else if (deventTime < 6200000) xTarg = 2.6  ; // -> should be from db
  else  xTarg = 4.0  ; 
  // even more rough for y
  const double yTarg = 1; // -> should be from db
  const double zTarg = -1030; // should be -1028 from the survey, right?
  TVector3 rTarg(xTarg,yTarg,zTarg);
 
  const double zOff = -422; //[mm] = zClu[which is 2555 now and not 2550.51 which should be from the survey] - 230./2. + 6.5*X0, X0=11.2 mm
  const double energyMin = 20; //MeV
  const double timeSafeMin = -1E10;//-110; // ns should do a time-dependent study
  const double maxTimeDistance = 5; // ns
  const double minGGDistance = 30; // mm
  
  //  std::cout << "beam pars " << fTargetRecoBeam->getXCfit() << " " << fTargetRecoBeam->getYCfit() << std::endl; //HOW TO USE THEM??

  const double deCutCenterX = -40; // MeV
  const double deCutRadiusX = 150.; // MeV
  const double deCutCenterY = 0; // MeV
  const double deCutRadiusY = 40; // MeV

  const double pi=3.14159265358979;

  TRecoVCluster* tempClu[2];
  TVector3 cluPos[2];
  double cluTime[2];
  double cluEnergy[2];

  fCalibTreeStruct.RunID = runID;
  fCalibTreeStruct.EventID = eventID;
  fCalibTreeStruct.EventTime = eventTime;
  fCalibTreeStruct.Sqrts = sqrts;
  fCalibTreeStruct.Temp = tempAvg;

  fCalibTreeStruct.TargX = xTarg;
  fCalibTreeStruct.TargY = yTarg;
  fCalibTreeStruct.BunchLength = fOfflineServerDB->getBunchLength(runID);
  fCalibTreeStruct.BeamStart = fOfflineServerDB->getBeamStart(runID);
  fCalibTreeStruct.RunTime = fOfflineServerDB->getRunTime(runID);
  fCalibTreeStruct.RunPOT = fOfflineServerDB->getRunPOT(runID);

  for (int h1=0; h1<fClColl->GetNElements(); ++h1) {
    tempClu[0] = fClColl->Element((int)h1);

    cluEnergy[0] = tempClu[0]->GetEnergy();
    if (cluEnergy[0] < energyMin) continue; // require a minimum energy

    cluTime[0] = tempClu[0]->GetTime();
    if (cluTime[0] < timeSafeMin) continue; // require time in the safe region [TBchecked]

    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),tempClu[0]->GetPosition().Z()+zOff);

    int isPaired = -1; // look for a gamma-gamma event, with loose conditions

    for (int h2=h1+1; h2<fClColl->GetNElements(); ++h2) {
      tempClu[1] = fClColl->Element((int)h2);

      cluEnergy[1] = tempClu[1]->GetEnergy();
      if (cluEnergy[1] < energyMin) continue; // require a minimum energy

      cluTime[1] = tempClu[1]->GetTime();
      if (cluTime[1] < timeSafeMin) continue;

      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),tempClu[1]->GetPosition().Z()+zOff);
      
      double dt = cluTime[0]-cluTime[1];
      double dr = (cluPos[0]-cluPos[1]).Mag();
      
      hSvcVal->FillHisto2(Form("ECal_SC_DrVsDt"), dt, dr, 1.);

      if (fabs(dt) < maxTimeDistance && dr > minGGDistance) {
	if (isPaired == -1) isPaired = h2;
	else { // more than 1 cluster found connected -> reject the event
	  isPaired = -1;
	  break;
	}
      }
    } // inner cluster loop

    if (isPaired != -1) { // fill plots
      cluEnergy[0] = tempClu[0]->GetEnergy();//*tempCorr;
      tempClu[1] = fClColl->Element((int)isPaired);
      cluEnergy[1] = tempClu[1]->GetEnergy();//*tempCorr;
      cluTime[1] = tempClu[1]->GetTime();
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),tempClu[1]->GetPosition().Z()+zOff); 

      hSvcVal->FillHisto2(Form("ECal_SC_EVsT"), cluTime[0], cluEnergy[0], 1.);
      hSvcVal->FillHisto2(Form("ECal_SC_EVsT"), cluTime[1], cluEnergy[1], 1.);
      double pg[2];
      for (int i = 0; i<2; i++){
	double cluDist = (cluPos[i]-rTarg).Mag();
	double cosq = (cluPos[i].Z() - rTarg.Z())/cluDist; //will have to correct Z of each cluster for its average cluster depth
	pg[i] = 0.5*sqrts/sqrt(1.-cosq*cosq + gam*gam*cosq*cosq - 2.*bg*gam*cosq + bg*bg);
      }      
      hSvcVal->FillHisto2(Form("ECal_SC_DE1VsDE2"), cluEnergy[0]-pg[0], cluEnergy[1]-pg[1], 1.);
      


      double elli = 
	(cluEnergy[0]-pg[0]-deCutCenterX)*(cluEnergy[0]-pg[0]-deCutCenterX)/(deCutRadiusX*deCutRadiusX) +
	(cluEnergy[1]-pg[1]-deCutCenterY)*(cluEnergy[1]-pg[1]-deCutCenterY)/(deCutRadiusY*deCutRadiusY);
      if (elli < 2) { // looser cut because of the temperature effect
	for (int i=0; i<2; i++) {
	  fCalibTreeStruct.TClus[i] = cluTime[i];
	  fCalibTreeStruct.EClus[i] = cluEnergy[i];
	  fCalibTreeStruct.XClus[i] = cluPos[i].X();
	  fCalibTreeStruct.YClus[i] = cluPos[i].Y();
	  fCalibTreeStruct.EExp[i] = pg[i];

	  hSvcVal->FillHisto2(Form("ECal_SC_EExpVsE"), cluEnergy[i], pg[i],1.);
	  double phi = cluPos[i].Phi();
	  if (phi < 0) phi += 2*pi;
	  int q = (phi*3./pi+0.5);
	  if (q > 5) q = 0;
	  hSvcVal->FillHisto2(Form("ECal_SC_EExpVsE_q%d",q), cluEnergy[i], pg[i],1.);
	}

//	std::cout << " E vs Exp h1=" << h1 << " " << cluEnergy[0] << " " << pg[0] << " h2=" << isPaired << " " << cluEnergy[1] << " " << pg[1] << std::endl;
//	std::cout << " CLUPOS0=(" << cluPos[0].X() << "," << cluPos[0].Y() << "," << cluPos[0].Z() << ") CLUPOS1=("<<cluPos[1].X() << "," << cluPos[1].Y() << ","<<cluPos[1].Z()<<")"<< " EexpTot="<< pg[0]+pg[1] << " EclTot="<<cluEnergy[0]+cluEnergy[1] << std::endl;

	hSvcVal->FillNtuple("ECal_SC_Tree");

      }
    }

  } // cluster loop
  return kTRUE;
}

Bool_t ECalAnalysis::ProcessValidation()
{  
  ValidationBase::ProcessValidation("ECal");
  
  
  Bool_t retCode = 0;

  HistoSvc* hSvcVal =  HistoSvc::GetInstance();
  TRecoVHit* hit=NULL;
  std::string hname;
  Double_t ETotHit=0.;
  Int_t fNhits = fhitEvent->GetNHits();
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    Int_t ix = hit->GetPosition().X();
    Int_t iy = hit->GetPosition().Y();
    ETotHit += hit->GetEnergy();
    //Int_t ix=position.X();
    //Int_t iy=position.Y();
  
    hname = "ECal_HitMap";
    hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);

    
   }
   hname="ECal_HitTotEnergy";
   hSvcVal->FillHisto(hname, ETotHit, 1.);

   TRecoVCluster* clu=NULL;
   TRecoVCluster* clun=NULL;
   Int_t fNclus = fClColl->GetNElements();
   Double_t ETotCl=0.;
   for (Int_t i=0; i<fNclus; ++i){
     clu    = fClColl->Element(i);
     Int_t ix = clu->GetPosition().X();
     Int_t iy = clu->GetPosition().Y();
     ETotCl+=clu->GetEnergy();
     //Int_t ix=position.X();
     //Int_t iy=position.Y();
     hname = "ECal_ClusterMap";
     hSvcVal->FillHisto2(hname, (Double_t)ix, (Double_t)iy, 1.);
     for(int j=i+1; j< fNclus; j++){
       clun   = fClColl->Element(j);
       if(fabs(clu->GetTime() - clun->GetTime())<5.)
       {
          hname="ECal_TwoPhotonEnergy_TimeCoincidenceRequest5ns";
          hSvcVal->FillHisto(hname, clu->GetEnergy()+clun->GetEnergy());
       }
     }
   }
   hname="ECal_ClusterTotEnergy";
   hSvcVal->FillHisto(hname, ETotCl, 1.);


   return retCode;
}




void ECalAnalysis::EnergyCalibration(Bool_t isMC)
{
  TRecoVHit* hit;
  Int_t fNhits =fhitEvent->GetNHits();
  Double_t eTot  =  0.;
  Double_t constantToAddForDataSeptember20=1.02242195116151890e+00;
  Double_t constantForDataJuly19=1.084; 
  for (Int_t i=0; i<fNhits; ++i){
    hit = fhitEvent->Hit(i);
    if(hit){
      if (!isMC) hit  ->SetEnergy(constantForDataJuly19*constantToAddForDataSeptember20*(hit->GetEnergy()));
    }
  }
  
  TRecoVCluster* xClu;
  for (int h=0; h<fClColl->GetNElements(); ++h)
    {
      xClu = fClColl->Element((int)h);
      if (xClu) 
	{
	  if (!isMC) xClu  ->SetEnergy(constantForDataJuly19*constantToAddForDataSeptember20*(xClu->GetEnergy()));
	}
    }
  return;

}
