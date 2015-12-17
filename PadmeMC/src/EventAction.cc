#include "globals.hh"
#include "Randomize.hh"
 
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"

#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "StackingAction.hh"
#include "HistoManager.hh"
#include "Constants.hh"
#include "MyEvent.hh"
#include <numeric>
#include "G4SystemOfUnits.hh"

extern double NNeutrons;
extern double Npionc;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
EventAction::EventAction(RunAction* run, HistoManager* histo)
:fRunAct(run),fHistoManager(histo)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void EventAction::BeginOfEventAction(const G4Event*)
{
  // Get current run and event numbers
  //  G4int nRun = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();
  //  G4int nEvent = evt->GetEventID();
  //  G4cout << ">>> Run " << nRun << " Event " << nEvent << " started" << G4endl;
  ETotCal  = 0;
  EtotMRod = 0;
  EtotTRod = 0;
  ECalHitT = 0;
  CalEvtT  = 0;
  ClPosX   = 0;
  ClPosY   = 0; 
  ProcID   = 0; 
  NClusters= 0;
  NTracks  = 0;
  NTarget  = 0;
  SACTracks  = 0;
  LAVTracks  = 0;
  NHEPVetoTracks = 0;
  NPVetoTracks = 0;
  NEleVetoTracks = 0;
  ETarget  = 0;
  TTarget   =0;

  memset(&(fHistoManager->myEvt),0,sizeof(NTEvent));
  myStepping->SetPhysProc(0);

  for(G4int i=0;i<ECalNCells;i++){Used[i]=0;}
  for(G4int i=0;i<TrackerNRings;i++){UsedRing[i]=0;}
  for(G4int i=0;i<ECalNCells;i++){Empty[i]=0;}
  
  //Clear completely the event:
  //  G4cout << "BeginOfEventAction:   " << G4endl; 
  
  MyEvent *TheEvent = MyEvent::GetInstance();
  TheEvent->GetSimEvent()->ClearEvent();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{
  G4int event_id = evt->GetEventID();
  // Periodic printing
  if (event_id < 1 || event_id%NPrint == 0)
    G4cout << ">>> Event " << evt->GetEventID() << G4endl;
  MyEvent *TheEvent = MyEvent::GetInstance();
  MySimEvent *simEvt = TheEvent->GetSimEvent();
  MyEventGenerator *genEvt = TheEvent->GetGenEvent();
  //  std::cout << "Before: Number of tracker clusters: " <<  simEvt->GetTrackerClusters()->size() << std::endl;

  std::vector<MyParticle>::iterator it;

 // G4cout << "================================================" << G4endl  ;
 // G4cout << "               Event                            " << G4endl  ;
 // G4cout << "Number of primaries in that event: "<< TheEvent->GetGenEvent()->getParticles()->size() << G4endl;
  it = genEvt->getParticles()->begin();
  while (it != genEvt->getParticles()->end()) {
    //    G4cout << "Particle:  " << it->getType() << G4endl;
    it++;
  }

  //G4cout << "Number of secondaries in that event: "<< simEvt->GetParticles()->size() << G4endl;
  it = simEvt->GetParticles()->begin();
  while (it != simEvt->GetParticles()->end()) {
    //   G4cout << "Particle:  " << it->getType() << G4endl;
    it++;
  }
  //  G4cout << "================================================" << G4endl  ;

  G4HCofThisEvent* LHC = evt->GetHCofThisEvent(); //list of Hit collections
  G4int nHC = LHC->GetNumberOfCollections();
  //  G4cout<<"N collections "<<nHC<<G4endl;
  for(G4int iHC=0; iHC<nHC; iHC++) {
    G4String HCname = LHC->GetHC(iHC)->GetName();  //nome della collezione
    //    G4cout << "RootIO: Found hits collection " << HCname << G4endl;
    if (HCname == "ECryCollection") {
      AddECryHits((ECalHitsCollection*) (LHC->GetHC(iHC)));
    } else if (HCname == "MRodCollection") {
      AddMRodHits((MRodHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "TRodCollection") {
      AddTRodHits((TRodHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "HEPVetoCollection") {
      AddHEPVetoHits((HEPVetoHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "PVetoCollection") {
      AddPVetoHits((PVetoHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "EleVetoCollection") {
      AddEleVetoHits((EleVetoHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "SACCollection") {
      AddSACHits((SACHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "LAVCollection") {
      AddLAVHits((LAVHitsCollection*)(LHC->GetHC(iHC)));
    }else if (HCname == "TrackerCollection") {
      AddTrackerHits((TrackerHitsCollection*)(LHC->GetHC(iHC)));
    }
  }
  int Ncells=0;
  for(G4int i=0;i<ECalNRow*ECalNCol;i++){
    if(ETotCry[i]>CellsE) Ncells++;
  }
  //Retrieve beam Infos!
  G4double BeamE  = myStepping->GetPositronE();
  G4double BeamX  = myStepping->GetPositronX();
  G4double BeamY  = myStepping->GetPositronY();
  G4double BeamZ  = myStepping->GetPositronZ();
  G4double PrimE  = myStepping->GetPrimE();

//  G4cout<<myAction->GetNGenN()<<G4endl;

  ProcID = myStepping->GetPhysProc();   
//  G4cout<<"ETOT Veto "<<EtotEVeto<<" NTRACKS "<<NVetoTracks<<G4endl;
//  G4cout<<"ETOT Tracker "<<Etrack[0]<<" NTRACKS "<<NTracks<<G4endl;

  fHistoManager->FillHisto(1,ETotCal);
  fHistoManager->FillHisto(2,BeamE);
  fHistoManager->FillHisto(3,Etrack[0]);
  fHistoManager->FillHisto(4,Ncells);
  fHistoManager->FillHisto(14,ProcID);
  fHistoManager->FillHisto(15,BeamZ);

  fHistoManager->FillHisto2(36,BeamX,BeamY,1.);
  fHistoManager->FillHisto2(30,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);

  if(ProcID==1) fHistoManager->FillHisto2(31,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
  if(ProcID==2) fHistoManager->FillHisto2(32,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
  
  if(NClusters>0){
    fHistoManager->FillHisto2(33,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
    if(ProcID==1) fHistoManager->FillHisto2(34,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
    if(ProcID==2) fHistoManager->FillHisto2(35,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
  }  
  //  fill ntuple for the event
  fHistoManager->myEvt.NTNevent         = evt->GetEventID();
  fHistoManager->myEvt.NTNCluster       = NClusters;
  fHistoManager->myEvt.NTNTracks        = NTracks;
  fHistoManager->myEvt.NTNHEPVetoTracks = NHEPVetoTracks;
  fHistoManager->myEvt.NTNPVetoTracks   = NPVetoTracks;
  fHistoManager->myEvt.NTNEleVetoTracks = NEleVetoTracks;
  fHistoManager->myEvt.NTSACNHit        = SACTracks;
  fHistoManager->myEvt.NTLAVNHit        = LAVTracks;
  fHistoManager->myEvt.NTNTarget        = NTarget;

  fHistoManager->myEvt.NTEtot     = ETotCal;
  fHistoManager->myEvt.NTIDProc   = ProcID;
  fHistoManager->myEvt.NTPBeam    = BeamE;
  fHistoManager->myEvt.NTPriBeam  = PrimE;
  fHistoManager->myEvt.NTXBeam    = BeamX;
  fHistoManager->myEvt.NTYBeam    = BeamY;
  fHistoManager->myEvt.NTNtotCell = Ncells;

  fHistoManager->myEvt.NTETarget  = ETarget; //capisci come fare per 10^4 event per burst
  fHistoManager->myEvt.NTTTarget  = TTarget;
  fHistoManager->myEvt.NTXTarget  = XTarget;
  fHistoManager->myEvt.NTYTarget  = YTarget;

  for(int i=0;i<SACTracks;i++){	  
    if(i>MaxTracks-1) break;
    //    G4cout<<"NSAC Tr "<<SACTracks<<" "<< SACEtrack[i]<<" "<<SACTrackTime[i]<<G4endl;
    fHistoManager->myEvt.NTSACE[i]  = SACEtrack[i];
    fHistoManager->myEvt.NTSACT[i]  = SACTrackTime[i];
    fHistoManager->myEvt.NTSACPType[i] = SACPType[i];
    fHistoManager->myEvt.NTSACX[i] = SACX[i];
    fHistoManager->myEvt.NTSACY[i] = SACY[i];
  }

  for(int i=0;i<LAVTracks;i++){	  
    if(i>MaxTracks-1) break;
    //    G4cout<<"NLAV Tr "<<LAVTracks<<" "<< LAVEtrack[i]<<" "<<LAVTrackTime[i]<<G4endl;
    fHistoManager->myEvt.NTLAVE[i]  = LAVEtrack[i];
    fHistoManager->myEvt.NTLAVT[i]  = LAVTrackTime[i];
    fHistoManager->myEvt.NTLAVPType[i] = LAVPType[i];
    fHistoManager->myEvt.NTLAVX[i] = LAVX[i];
    fHistoManager->myEvt.NTLAVY[i] = LAVY[i];
  }



  for(int i=0;i<NTracks;i++){	  
    if(i>(MaxTracks-1)) break;
       fHistoManager->myEvt.NTETracker[i]   = Etrack[i]*TrackCh[i];
       fHistoManager->myEvt.NTTrackerLay[i] = TrackerLay[i];
       fHistoManager->myEvt.NTTrackerTime[i]= TrackTime[i];
       fHistoManager->myEvt.NTTrackerZ[i]   = TrackZ[i]*TrackCh[i];
       fHistoManager->myEvt.NTTrackerX[i]   = TrackX[i];
       fHistoManager->myEvt.NTTrackerY[i]   = TrackY[i];
  }

  for(int i=0;i<NHEPVetoTracks;i++){  //BUG on number of channel!
	  if(i>MaxTracks-1) break;
	  fHistoManager->myEvt.NTHEPVetoTrkEne[i]    = HEPVetoEtrack[i];
	  if(HEPVetoTrackCh[i]<100 && HEPVetoTrackCh[i]>-100) fHistoManager->myEvt.NTHEPVetoTrkFinger[i] = HEPVetoTrackCh[i];
	  fHistoManager->myEvt.NTHEPVetoTrkTime[i]   = HEPVetoTrackTime[i];
	  fHistoManager->myEvt.NTHEPVetoFingerE[i]   = ETotHEPVeto[HEPVetoTrackCh[i]]; //Just one finger per track to be improved!
	  fHistoManager->myEvt.NTHEPVetoX[i]   = HEPVetoX[i]; //Just one finger per track to be improve!
	  fHistoManager->myEvt.NTHEPVetoY[i]   = HEPVetoY[i]; //Just one finger per track to be improved!

	  //if(VetoTrackCh[i]>100) G4cout<<i<<" Tracker Lay "<<VetoTrackCh[i]<<" "<<VetoEtrack[i]<<" "<<VetoTrackTime[i]<<G4endl;
	  //	  G4cout<<VetoX[i]<<" "<<myEvt.NTVetoY[i]<<G4endl;
  }

  for(int i=0;i<NPVetoTracks;i++){  //BUG on number of channel!
	  if(i>MaxTracks-1) break;
	  fHistoManager->myEvt.NTPVetoTrkEne[i]  = PVetoEtrack[i];
	  //	  if(PVetoTrackCh[i]<100 && PVetoTrackCh[i]>-100) fHistoManager->myEvt.NTVetoTrkFinger[i] = PVetoTrackCh[i];
	  fHistoManager->myEvt.NTPVetoTrkFinger[i] = PVetoTrackCh[i];
	  fHistoManager->myEvt.NTPVetoTrkTime[i] = PVetoTrackTime[i];
	  fHistoManager->myEvt.NTPVetoFingerE[i] = ETotPVeto[PVetoTrackCh[i]]; //Just one finger per track to be improved!
	  fHistoManager->myEvt.NTPVetoX[i]       = PVetoX[i]; //Just one finger per track to be improve!
	  fHistoManager->myEvt.NTPVetoY[i]       = PVetoY[i]; //Just one finger per track to be improved!

	  //if(VetoTrackCh[i]>100) G4cout<<i<<" Tracker Lay "<<VetoTrackCh[i]<<" "<<VetoEtrack[i]<<" "<<VetoTrackTime[i]<<G4endl;
	  //	  G4cout<<VetoX[i]<<" "<<myEvt.NTVetoY[i]<<G4endl;
  }

  for(int i=0;i<NEleVetoTracks;i++){  //BUG on number of channel!
	  if(i>MaxTracks-1) break;
	  fHistoManager->myEvt.NTEleVetoTrkEne[i]  = EleVetoEtrack[i];
	  //	  if(EleVetoTrackCh[i]<100 && EleVetoTrackCh[i]>-100) fHistoManager->myEvt.NTVetoTrkFinger[i] = EleVetoTrackCh[i];
	  fHistoManager->myEvt.NTEleVetoTrkFinger[i]  = EleVetoTrackCh[i];
	  fHistoManager->myEvt.NTEleVetoTrkTime[i] = EleVetoTrackTime[i];
	  fHistoManager->myEvt.NTEleVetoFingerE[i] = EleETotVeto[EleVetoTrackCh[i]]; //Just one finger per track to be improved!
	  fHistoManager->myEvt.NTEleVetoX[i]       = EleVetoX[i]; //Just one finger per track to be improve!
	  fHistoManager->myEvt.NTEleVetoY[i]       = EleVetoY[i]; //Just one finger per track to be improved!xs
	  //	  G4cout<<i<<" Tracker Lay "<<EleVetoTrackCh[i]<<" "<<EleVetoEtrack[i]<<" "<<EleVetoTrackTime[i]<<G4endl;
	  //	  G4cout<<VetoX[i]<<" "<<myEvt.NTVetoY[i]<<G4endl;
  }

  for(int i=0;i<NClusters;i++){	
    //    G4cout<<"DDD CALO"<<EneCl[i]<<" "<<NTQCl[i]<<G4endl;
    if(i>19) break;              
    fHistoManager->myEvt.NTECluster[i] = EneCl[i];
    fHistoManager->myEvt.NTQCluster[i] = QCl[i];
    fHistoManager->myEvt.NTXCluster[i] = XCl[i];   
    fHistoManager->myEvt.NTYCluster[i] = YCl[i];   
    fHistoManager->myEvt.NTThCluster[i]= ThCl[i];
    fHistoManager->myEvt.NTM2Cluster[i]= MM2[i];
    fHistoManager->myEvt.NTTCluster[i] = TimeCl[i];
  }
  for(int i=0;i< ECalNCells;i++){
    fHistoManager->myEvt.NTECell[i]=ETotCry[i];
    fHistoManager->myEvt.NTQCell[i]=QTotCry[i];
    fHistoManager->myEvt.NTTCell[i]=TimeCry[i];
  }

  //Tracker information:

  //  std::cout << "Number of tracker clusters: " <<  simEvt->GetTrackerClusters()->size() << std::endl;
  //Remapping of the variables .... :(

  if(IsTrackerRecoON==1){
    fHistoManager->myEvt.NTNTrClus  = simEvt->GetTrackerClusters()->size();
    //    fHistoManager->myEvt.NTNTrClus=0;
    for(int i=0; i< fHistoManager->myEvt.NTNTrClus && i < MaxTrHits ; i++) {
      fHistoManager->myEvt.NTTrClusX[i] = simEvt->GetTrackerClusters()->at(i).pos[0];
      fHistoManager->myEvt.NTTrClusY[i] = simEvt->GetTrackerClusters()->at(i).pos[1];
      fHistoManager->myEvt.NTTrClusZ[i] = simEvt->GetTrackerClusters()->at(i).pos[2];
      fHistoManager->myEvt.NTTrClusLayer[i] = simEvt->GetTrackerClusters()->at(i).layer;        
    }
  }
  
  if(IsTrackerRecoON==1){
    if(ETotCal>EMinSaveNT || fHistoManager->myEvt.NTNTrClus>4) fHistoManager->FillNtuple(&(fHistoManager->myEvt));
  }else{
    if(ETotCal>EMinSaveNT) fHistoManager->FillNtuple(&(fHistoManager->myEvt));
    //fHistoManager->FillNtuple(&(fHistoManager->myEvt));
    //    if(ETotCal>EMinSaveNT || NTracks>0.) fHistoManager->FillNtuple(&(fHistoManager->myEvt));
  }
  //  G4cout << "Ntuples filled, going further" << G4endl;
}

void EventAction::AddECryHits(ECalHitsCollection* hcont)
{
  G4double ETotEvt=0;
  G4int nHits = hcont->entries();
  G4int SeedInd;
  for(G4int jj=0;jj<ECalNCells;jj++){
    ETotCry[jj]=0.0;
    QTotCry[jj]=0.0;
    TimeCry[jj]=0.0;
  }
  
  // Copy all hits from G4 to ROOT event
  for (G4int h=0; h<nHits; h++) {
    ECalHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
     ETotCry[hit->GetCryNb()] += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristallo
     QTotCry[hit->GetCryNb()] += GetCharge(hit->GetEdep()); //somma le cariche su tutti gli hit di ogni cristallo
     if(TimeCry[hit->GetCryNb()]==0.) TimeCry[hit->GetCryNb()] =  hit->GetTime();//assing to each crystal the time of the first hit!
     ETotEvt += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristalli
     //     if(TimeCry[hit->GetCryNb()]==0.) G4cout<<"Hit TIme "<< hit->GetTime()<<" Nhits "<<hit->GetCryNb()<<" Cl "<<TimeCry[hit->GetCryNb()]<<" "<<QTotCry[hit->GetCryNb()]<< G4endl;
    }
  }//end of loop

  while( (SeedInd=GetSeedCell()) !=-1 ){
    GetEClus(SeedInd);
    
    // double Charge = GetCharge(EClus);
    //   G4cout<<"Charge"<<Charge<<G4endl;
    if (NClusters<19) EneCl[NClusters] = EClus;
    if (NClusters<19) QCl[NClusters]   = QClus;
    if (NClusters<19) XCl[NClusters]   = ClPosX;
    if (NClusters<19) YCl[NClusters]   = ClPosY;
    if (NClusters<19) ThCl[NClusters]  = Theta;
    if (NClusters<19) MM2[NClusters]   = Mmiss2;
    if (NClusters<19) TimeCl[NClusters]= ClTime;
    //    G4cout<<"DD "<<EClus<<" "<<EneCl[NClusters]<<G4endl;    
    //    G4cout<<"Cl PosX "<<ClPosX<<" Cl PosY "<<ClPosY<<" Nclus "<<EClus<<G4endl;
    fHistoManager->FillHisto2(1,GetCryPosX(SeedInd),GetCryPosY(SeedInd),ETotCry[SeedInd]);  
    fHistoManager->FillHisto2(2,ClPosX,ClPosY,EClus);  
    G4double ProcID = myStepping->GetPhysProc();
    
    if(ProcID==1) fHistoManager->FillHisto2(8,EClus,Theta,1.); //Nclus==1
    if(ProcID==2) fHistoManager->FillHisto2(9,EClus,Theta,1.); //Nclus==2
    if (NClusters==0) {
      fHistoManager->FillHisto(6, EClus);
      //      fHistoManager->FillHisto(7, SeedCell);
      fHistoManager->FillHisto(8, Theta);
      fHistoManager->FillHisto(9, Mmiss2);
    }
    NClusters++;
  }
  fHistoManager->FillHisto(11,NClusters);
  ETotCal = ETotEvt;
  //  G4cout<<"DDDD "<<EClus<<G4endl;
}

void EventAction::AddMRodHits(MRodHitsCollection* hcont)
{
  G4double METotRod[100];
  G4double METotRodEvt=0;

  G4int nHits = hcont->entries();
  for(G4int jj=0;jj<MaxTracks;jj++){METotRod[jj]=0.0;}

  // Copy all hits from G4 to ROOT event
  for (G4int h=0; h<nHits; h++) {
    MRodHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      //      ECryHit* rhit = event->AddProfilometerHit();
      //      rhit->SetTrackID(hit->GetTrackID());
      //      rhit->SetFiberNb(hit->GetFiberNb());
      METotRod[hit->GetMRodNb()] += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristallo
      METotRodEvt += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristallo
      //      rhit->SetTime(hit->GetTime());
      //      G4ThreeVector p = hit->GetPos();
      //      rhit->SetPos(TVector3(p.x(),p.y(),p.z()));
      //      G4ThreeVector lp = hit->GetLocPos();
      //      rhit->SetLocPos(TVector3(lp.x(),lp.y(),lp.z()));
    }
  }//end of loop
  EtotMRod = METotRodEvt;    
}

void EventAction::AddTRodHits(TRodHitsCollection* hcont)  //Target readout module
{
  G4double ETotRod[100];
  G4double ETotRodEvt=0;
  G4int nHits = hcont->entries();
  for(G4int jj=0;jj<MaxTracks;jj++){ETotRod[jj]=0.0;}

  // Copy all hits from G4 to ROOT event
  for (G4int h=0; h<nHits; h++) {
    TRodHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      //      ECryHit* rhit = event->AddProfilometerHit();
      //      rhit->SetTrackID(hit->GetTrackID());
      //      rhit->SetFiberNb(hit->GetFiberNb());
      ETotRod[hit->GetTRodNb()] += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristallo
      ETotRodEvt += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristalli
      TTarget=hit->GetTime();
      XTarget=hit->GetX();
      YTarget=hit->GetY();
      NTarget++;
 //     EtotTRod = ETotRodEvt;
      //G4cout<<" Edep "<< EtotTRod<<" "<<nHits<<" time "<<hit->GetTime()<<G4endl;
    }
  }//end of loop

//  EtotTRod = ETotRodEvt;
    ETarget = ETotRodEvt;
}

void EventAction::AddTrackerHits(TrackerHitsCollection* hcont)
{
  //MySimEvent *mySim = (MyEvent::GetInstance())->GetSimEvent();
  G4double ETotTrackerEvt=0;										
  G4int nHits = hcont->entries();
  //  std::cout << "EventAction::AddTrackerHits  nHits = " <<  nHits << std::endl;

  G4int IDHit[MaxTracks];
  G4int SeedInd=0;
  G4int LastID=0;
  for(G4int jj=0;jj<200;jj++){ETotRing[jj]=0.0;}
  for(G4int jj=0;jj<MaxTracks;jj++){
    Etrack[jj]=0.0;
    TrackTime[jj]=0.0;
    TrackZ[jj]=0.0;
    TrackX[jj]=0.0;
    TrackY[jj]=0.0;
  }
  //  G4cout<<"new event "<<G4endl;
  for (G4int h=0; h<nHits; h++) {									
    TrackerHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit					
    if ( hit != 0 ) {
      (MyEvent::GetInstance())->GetSimEvent()->AddTrHit(hit->GetTrackerNb(), hit->GetX(), hit->GetY(),hit->GetZ(), hit->GetE(),0.3*mm );
      ETotRing[hit->GetTrackerNb()] += hit->GetEdep();
      //      G4cout<<"Hit X "<<hit->GetX()<<" HIT Y "<<hit->GetY()<<" HIT Z "<<hit->GetZ()<<" "<<hit->GetEdep()<<G4endl;
      
      ETotTrackerEvt += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristalli	
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && hit->GetEdep()>2.*MeV && NTracks < MaxTracks) {
	TrackCh[NTracks]    = hit->GetTrackCh();
	IDHit[NTracks]      = hit->GetTrackID();
	Etrack[NTracks]     = hit->GetEdep();
	TrackerLay[NTracks] = hit->GetTrackerNb();
	TrackTime[NTracks]  = hit->GetTime();
	TrackZ[NTracks]     = hit->GetZ();
	TrackX[NTracks]     = hit->GetX();
	TrackY[NTracks]     = hit->GetY();
	//  G4cout<<"EtotTracker i "<<TrackZ[NTracks]<<" "<<Etrack[NTracks]<<G4endl;
	NTracks++;
	//G4cout<<"EtotTracker i "<<Etrack[NTracks]<<" "<<hit->GetTrackerNb()<<" "<<nHits<<"  "<< TrackTime[NTracks]<<G4endl;
      }
      if(NTracks>MaxTracks) break; 
      LastID = hit->GetTrackID();
    }													
  }//end of loop
  ETotTra = ETotTrackerEvt;             
}

void EventAction::AddHEPVetoHits(HEPVetoHitsCollection* hcont)
{
  G4double ETotHEPVetoEvt=0.;
  G4int LastID=-1;
  for(G4int jj=0;jj<MaxTracks;jj++){
    ETotHEPVeto[jj]=0.0;							
    HEPVetoTrackCh[jj]=0;
    HEPVetoTrackTime[jj]=0;
    HEPVetoX[jj]=0;
    HEPVetoY[jj]=0;
  }

  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++) {									
    HEPVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit					
    if ( hit != 0 ) {
      ETotHEPVeto[hit->GetHEPVetoNb()] += hit->GetHitE();  //sum single fingers energies and get total finger
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && NHEPVetoTracks < MaxTracks && hit->GetEdep()>1.*MeV) {
    	  HEPVetoTrackCh[NHEPVetoTracks]    = hit->GetHEPVetoNb();   //bugs gives crazy numbers
    	  HEPVetoEtrack[NHEPVetoTracks]     = hit->GetEdep();
    	  HEPVetoTrackTime[NHEPVetoTracks]  = hit->GetTime();
	  HEPVetoX[NHEPVetoTracks]          = hit->GetX();
	  HEPVetoY[NHEPVetoTracks]          = hit->GetY();
    	  NHEPVetoTracks++;
    	  //G4cout<<"trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" Strip Numb "<<hit->GetEVetoNb()<<G4endl;
      }
      if(NHEPVetoTracks>MaxTracks) break; 
      LastID = hit->GetTrackID();
    }
  }//end of loop											
}

void EventAction::AddPVetoHits(PVetoHitsCollection* hcont){
  G4double ETotPVetoEvt=0;
  G4int LastID=-1;
  for(G4int jj=0;jj<MaxTracks;jj++){
    ETotPVeto[jj]=0.0;
    PVetoTrackCh[jj]=0;
    PVetoTrackTime[jj]=0;
    PVetoX[jj]=0;
    PVetoY[jj]=0;
  }
  
  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++) {									
    PVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit					
    if ( hit != 0 ) {
      ETotPVeto[hit->GetPVetoNb()] += hit->GetHitE();  //sum single fingers energies and get total finger
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && NPVetoTracks < MaxTracks && hit->GetEdep()>2.*MeV) {
	PVetoTrackCh[NPVetoTracks]    = hit->GetPVetoNb();   //bugs gives crazy numbers
	PVetoEtrack[NPVetoTracks]     = hit->GetEdep();
	PVetoTrackTime[NPVetoTracks]  = hit->GetTime();
	PVetoX[NPVetoTracks]          = hit->GetX();
	PVetoY[NPVetoTracks]          = hit->GetY();
	NPVetoTracks++;
	//G4cout<<"trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" Strip Numb "<<hit->GetEVetoNb()<<G4endl;
      }
      if(NPVetoTracks>MaxTracks) break; 
      LastID = hit->GetTrackID();
    }
  }
}

void EventAction::AddEleVetoHits(EleVetoHitsCollection* hcont){
  //  G4cout<<"vaffanculo "<<G4endl;
  G4double EleETotVetoEvt=0;
  G4int LastID=-1;
  for(G4int jj=0;jj<MaxTracks;jj++){
    EleETotVeto[jj]=0.0;						  
    EleVetoTrackCh[jj]=0;
    EleVetoTrackTime[jj]=0;
    EleVetoX[jj]=0;
    EleVetoY[jj]=0;
  }
  
  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++) {									
    EleVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit					
    if ( hit != 0 ) {
      EleETotVeto[hit->GetEleVetoNb()] += hit->GetHitE();  //sum single fingers energies and get total finger
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && NEleVetoTracks < MaxTracks && hit->GetEdep()>2.*MeV) {
      //      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID) {
	EleVetoTrackCh[NEleVetoTracks]    = hit->GetEleVetoNb();   //bugs gives crazy numbers
	EleVetoEtrack[NEleVetoTracks]     = hit->GetEdep();
	EleVetoTrackTime[NEleVetoTracks]  = hit->GetTime();
	EleVetoX[NEleVetoTracks]          = hit->GetX();
	EleVetoY[NEleVetoTracks]          = hit->GetY();
	NEleVetoTracks++;
	//	G4cout<<"cazzo "<<NEleVetoTracks<<G4endl;
	//G4cout<<"trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" Strip Numb "<<hit->GetEVetoNb()<<G4endl;
      }
      if(NEleVetoTracks>MaxTracks) break; 
      LastID = hit->GetTrackID();
    }
  }

}

void EventAction::AddSACHits(SACHitsCollection* hcont)
{
  G4int LastID=-1;
  G4int nHits = hcont->entries();			
  for(G4int jj=0;jj<MaxTracks;jj++){
    ETotSAC[jj]=0.0;										     
    SACTrackTime[jj]=0.0;										     
    SACEtrack[jj]=0.0;
    SACPType[jj]=0.0;
  }							
  for (G4int h=0; h<nHits; h++) {
    SACHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && hit->GetEdep()>2.*MeV && SACTracks < MaxTracks) {
	//	ETotSAC[hit->GetSACNb()] += hit->GetEdep();  //sum single fingers energies and get total finger
	//    	  SACTrackCh[SACTracks] = hit->GetSACNb();
	SACEtrack[SACTracks]    = hit->GetEdep();
	SACTrackTime[SACTracks] = hit->GetTime();
	SACPType[SACTracks]     = hit->GetPType();
	SACX[SACTracks]         = hit->GetX();
	SACY[SACTracks]         = hit->GetY();
	//	G4cout<<"CC Nhits "<<nHits<<" trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" time "<<hit->GetTime()<<G4endl;
	//	G4cout<<"CC LastID "<<LastID<<" "<<SACY[SACTracks]<<G4endl;
	SACTracks++;
      }
      LastID = hit->GetTrackID();
    }
  }//end of loop
}

void EventAction::AddLAVHits(LAVHitsCollection* hcont)
{
  G4int LastID=-1;
  G4int nHits = hcont->entries();			
  for(G4int jj=0;jj<MaxTracks;jj++){
    ETotLAV[jj]=0.0;										     
    LAVTrackTime[jj]=0.0;										     
    LAVEtrack[jj]=0.0;
    LAVPType[jj]=0.0;
  }							
  for (G4int h=0; h<nHits; h++) {
    LAVHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && hit->GetEdep()>0.1*MeV && LAVTracks < MaxTracks) {
	//	ETotLAV[hit->GetLAVNb()] += hit->GetEdep();  //sum single fingers energies and get total finger
	//    	  LAVTrackCh[LAVTracks] = hit->GetLAVNb();
	LAVEtrack[LAVTracks]    = hit->GetEdep();
	LAVTrackTime[LAVTracks] = hit->GetTime();
	LAVPType[LAVTracks]     = hit->GetPType();
	LAVX[LAVTracks]         = hit->GetX();
	LAVY[LAVTracks]         = hit->GetY();
	G4cout<<"CC Nhits "<<nHits<<" trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" time "<<hit->GetTime()<<G4endl;
	//	G4cout<<"CC LastID "<<LastID<<" "<<LAVY[LAVTracks]<<G4endl;
	LAVTracks++;
      }
      LastID = hit->GetTrackID();
    }
  }//end of loop
}

// Return the index of crystal with maximum energy				  
G4int EventAction::GetSeedCell()						  
{										  
  G4double Emax=0.;								  
  G4int Imax=-1;								  
  for(G4int i=0;i<ECalNRow*ECalNCol;i++){					  
    if(ETotCry[i]>SeedE && ETotCry[i]>Emax && Used[i]==0) {			  
      Emax=ETotCry[i];								  
      Imax=i;									  
    }										  
  }										  
  return Imax;									  
}                                                                                 

// Return the index of crystal with maximum energy				  
G4int EventAction::GetSeedRing()						  
{										  
  G4double Emax=0.;								  
  G4int Imax=-1;
  //G4cout<<TrackerNRings<<G4endl;
  for(G4int i=0;i<TrackerNRings;i++){  				  
    if(ETotRing[i]>0 && ETotRing[i]>Emax && UsedRing[i]==0) {			  
      Emax=ETotRing[i];								  
      Imax=i;      	       	       								  
    }										  
  }										  
  return Imax;									  
}                                                                                 

G4double EventAction::GetETrack(G4int SeedTrack){
  G4double EtotTra=0;
  // G4cout<<"azz" <<SeedTrack <<G4endl;
  if(SeedTrack == -1) return -1;
  //for(G4int kk=10;kk<20;kk++){
  EtotTra+=ETotRing[SeedTrack]+ETotRing[SeedTrack+1]+ETotRing[SeedTrack-1];
  //G4cout<<"EtotTra "<<EtotTra<<" "<<SeedTrack<<G4endl;
  UsedRing[SeedTrack]=1;
  UsedRing[SeedTrack-1]=1;
  UsedRing[SeedTrack+1]=1;

    //}
  return ETotTra;
}

G4double EventAction::GetEClus(G4int SeedCell)
{
  G4int NcellsCl=0;
  G4int NNeig = GetNeigPos(SeedCell);
  EClus =0;
  QClus =0;
  ClPosX=0;
  ClPosY=0;
  ClTime=0;
  Mmiss2=0;
  Theta =0;

  for(G4int j=0;j<NNeig;j++){
    if(ETotCry[Neig[j]]>CellsE && Used[Neig[j]]!=1) {//some fraction of E(SeedCell)
      EClus+=ETotCry[Neig[j]];
      QClus+=QTotCry[Neig[j]];
      //       G4cout<<"EClus "<<EClus<<G4endl;
      ClPosX+= GetCryPosX(Neig[j])*ETotCry[Neig[j]];
      ClPosY+= GetCryPosY(Neig[j])*ETotCry[Neig[j]];
      // ClTime+=TimeCry[Neig[j]]*ETotCry[Neig[j]];
      ClTime+=TimeCry[Neig[j]];
      NcellsCl++;
      Used[Neig[j]]=1;
      //    }else if(ETotCry[Neig[j]]<CellsE){  //don't use it in the iterated neig search
      //      Empty[Neig[j]]=1;
    }
  }
  ClTime=ClTime/(NcellsCl);  //can try to use weighted average
  //G4cout<<"ClTime "<<ClTime<<G4endl;
  ClPosX=ClPosX/EClus;
  ClPosY=ClPosY/EClus;

  G4double DxDz=ClPosX/(ECalPosiZ-ECalSizeZ/2);
  G4double DyDz=ClPosY/(ECalPosiZ-ECalSizeZ/2);
  G4double norma=sqrt(DxDz*DxDz+DyDz*DyDz+1.);
  G4double GDir[3],BDir[3];
  G4double GMom[4],BeamMom[4],TargetEleMom[4],P4Miss[4];

  GDir[0]=DxDz/norma;
  GDir[1]=DyDz/norma;
  GDir[2]=1./norma;

  GMom[0]=GDir[0]*EClus;  
  GMom[1]=GDir[1]*EClus;
  GMom[2]=GDir[2]*EClus;
  GMom[3]=EClus;        

  BDir[0]=0.;
  BDir[1]=0.;
  BDir[2]=1.;
 
  BeamMom[0]=BDir[0]*BeamEnergy;
  BeamMom[1]=BDir[1]*BeamEnergy;
  BeamMom[2]=BDir[2]*BeamEnergy;
  BeamMom[3]=sqrt(BeamEnergy*BeamEnergy+0.511*0.511);         

  //The target electron is at rest
  TargetEleMom[0]=0.;
  TargetEleMom[1]=0.;
  TargetEleMom[2]=0.;
  TargetEleMom[3]=0.511;

  for (int i=0; i<4; i++){ 
    P4Miss[i]=TargetEleMom[i]+BeamMom[i]-GMom[i];
  }
  Mmiss2 = P4Miss[3]*P4Miss[3]-P4Miss[2]*P4Miss[2]-P4Miss[1]*P4Miss[1]-P4Miss[0]*P4Miss[0];
  //  G4cout<<"***************MMiss "<<Mmiss2<<G4endl;
  //  G4cout<<"***************Gmom "<<GMom[3]<<G4endl;
  //  G4cout<<"***************MMiss "<<Mmiss2<<G4endl;
  G4double product=0;
  for (int i=0; i<3; i++)  product+= GDir[i]*BDir[i];
  ClRadius=sqrt(ClPosX*ClPosX+ClPosY*ClPosY);
  Theta = acos (product) * 180.0 / 3.14159265;  //in degreees
  //  G4cout<<" Scalat product "<<product<<" norma "<<norma<<" Theta "<<Theta<<G4endl;
  // ClTime = aStep->GetTrack()->GetGlobalTime()<<G4endl;   MMM
  // G4cout<<"Eclus out "<<EClus<< " Ecry Seed "<<ETotCry[SeedCell]<<" ClPosX "<<ClPosX<<G4endl;
  //  if(Imax !=-1) G4cout<<"I max "<<Imax<<" E max "<<Emax<<" Ncells "<<Ncells<<G4endl;
  
  fHistoManager->FillHisto(5, NcellsCl);
  //  fHistoManager->FillHisto(6, EClus);
  //  fHistoManager->FillHisto(7, SeedCell);
  //  fHistoManager->FillHisto(8, Theta);
  //  fHistoManager->FillHisto(9, Mmiss2);
  if(EClus>50. && NcellsCl>1 && ClRadius>3. && EClus<400.) fHistoManager->FillHisto(10,Mmiss2);

  fHistoManager->FillHisto2(4,NcellsCl,EClus,1.);
  fHistoManager->FillHisto2(5,ClPosX,ClPosY,1.);
  fHistoManager->FillHisto2(6,EClus,Theta,1.);
  fHistoManager->FillHisto2(10,Mmiss2,ClRadius,1.);
  if(EClus>50.) fHistoManager->FillHisto2(7,EClus,Theta,1.);
  return EClus;
}

G4double EventAction::GetCryPosY(G4int CryNb)
{
  G4int Yind=int(CryNb/ECalNRow);
  G4double CryY  = ECalSizeY/ECalNCol;
  G4double Ycoord=-ECalSizeY*0.5+0.5*CryY+Yind*CryY;
  //  G4cout<<"Ncry "<<CryNb<<" Xind " << Xind <<" CryX "<<CryX<<" Xcoord "<<Xcoord<<G4endl;
  return Ycoord;
}

G4double EventAction::GetCryPosX(G4int CryNb)
{
  G4int Xind=int(CryNb%ECalNRow);
  G4double CryX  = ECalSizeX/ECalNRow;
  G4double Xcoord=-ECalSizeX*0.5+0.5*CryX+Xind*CryX;
  //  G4cout<<"Ncry "<<CryNb<<" Yind " << Yind <<" CryY "<<CryY<<" Ycoord "<<Ycoord<<G4endl;
  return Xcoord;
}

G4double EventAction::GetCharge(G4double Energia)
{
  G4double LY      =   1800.; //photons per MeV
  G4double Gain    =     1E6;
  G4double Echarge = 1.6E-19;
  G4double CollEff =     0.1; //geometrical factor due to photodetector area
  G4double QE      =    0.25; //photodetector quantum efficiency
  G4double NPh = Energia*LY;
  G4double rnd = G4RandGauss::shoot(NPh,sqrt(NPh)); //should be poisson
  G4double Charge = rnd*QE*CollEff*Gain*Echarge/1E-12;  //in pC
  //  G4cout<<"Energia "<<Energia<<" rnd " << rnd <<" nph "<<NPh<<" Charge "<<Charge<<" pC"<<G4endl;
  return Charge;
}

G4int EventAction::GetNeigPos(G4int CryNb)
{
  G4int NNeig=0;
  G4int SeedCell=CryNb;
  
  for(int jj=0;jj<ECalNRow*ECalNCol;jj++) Neig[jj]=0;
  G4double SX= GetCryPosX(SeedCell);
  G4double SY= GetCryPosY(SeedCell);
  
  for(int jj=0;jj<ECalNRow*ECalNCol;jj++){
    G4double NX= GetCryPosX(jj);
    G4double NY= GetCryPosY(jj);
    //    G4cout<<"NX Y "<<NX<<" "<<NY<<" SX Y "<<SX<<" "<<SY<<G4endl;
    //    G4cout<<fabs(NX-SX)<<" "<<RadiusCl<<G4endl;
    if( fabs(NX-SX)<RadiusCl && fabs(NY-SY)<RadiusCl){
      Neig[NNeig]=jj;
      //      G4cout<<" Dx "<< fabs(NX-SX)<<" DY "<< fabs(NY-SY)<<" "<<jj<<" "<<NNeig<<G4endl;
      NNeig++;
    }
  }
  //  G4cout<<" NNeig "<<NNeig<<" SeedCell "<<SeedCell<<G4endl;
  return NNeig;
}
