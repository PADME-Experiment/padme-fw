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
#include "RootIOManager.hh"
#include "HistoManager.hh"
#include "Constants.hh"
#include <numeric>
#include "G4SystemOfUnits.hh"
#include "G4Poisson.hh"

#include "G4DigiManager.hh"
#include "TargetDigitizer.hh"
#include "PVetoDigitizer.hh"
#include "EVetoDigitizer.hh"
#include "HEPVetoDigitizer.hh"
#include "ECalDigitizer.hh"
#include "SACDigitizer.hh"
#include "TPixDigitizer.hh"

extern double NNeutrons;
extern double Npionc;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
EventAction::EventAction(RunAction* run)
:fRunAct(run)
{

  fHistoManager = HistoManager::GetInstance();
  Egeom = ECalGeometry::GetInstance();
  Tgeom = TargetGeometry::GetInstance();
  Bpar  = BeamParameters::GetInstance();

  // Create and register digitizer modules for all detectors
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  TargetDigitizer* targetDM = new TargetDigitizer("TargetDigitizer");
  theDM->AddNewModule(targetDM);
  PVetoDigitizer* pVetoDM = new PVetoDigitizer("PVetoDigitizer");
  theDM->AddNewModule(pVetoDM);
  EVetoDigitizer* eVetoDM = new EVetoDigitizer("EVetoDigitizer");
  theDM->AddNewModule(eVetoDM);
  HEPVetoDigitizer* hepVetoDM = new HEPVetoDigitizer("HEPVetoDigitizer");
  theDM->AddNewModule(hepVetoDM);
  ECalDigitizer* eCalDM = new ECalDigitizer("ECalDigitizer");
  theDM->AddNewModule(eCalDM);
  SACDigitizer* sacDM = new SACDigitizer("SACDigitizer");
  theDM->AddNewModule(sacDM);
  TPixDigitizer* tPixDM = new TPixDigitizer("TPixDigitizer");
  theDM->AddNewModule(tPixDM);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void EventAction::BeginOfEventAction(const G4Event*)
{
  // Get current run and event numbers
  ETotCal  = 0;
  ECalHitT = 0;
  CalEvtT  = 0;
  ClPosX   = 0;
  ClPosY   = 0; 
  ProcID   = 0; 
  NClusters= 0;
  NTracks  = 0;

  SACTracks  = 0;
  CalNPart   = 0;
  LAVTracks  = 0;
  NHEPVetoTracks = 0;
  NPVetoTracks = 0;
  NEVetoTracks = 0;

  ETarget  = 0;
  TTarget  = 1e6;
  XTarget  = 0;
  YTarget  = 0;
  NTarget  = 0;

  memset(&(fHistoManager->myEvt),0,sizeof(NTEvent));
  myStepping->SetPhysProc(0);

  for(G4int i=0;i<ECalNCells;i++){Used[i]=0;}
  for(G4int i=0;i<TrackerNRings;i++){UsedRing[i]=0;}
  for(G4int i=0;i<ECalNCells;i++){Empty[i]=0;}
  
  //Clear completely the event:
  //G4cout << "BeginOfEventAction:   " << G4endl; 
  
  //MyEvent *TheEvent = MyEvent::GetInstance();
  //TheEvent->GetSimEvent()->ClearEvent();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{

  G4int event_id = evt->GetEventID();
  // Periodic printing
  if (event_id < 1 || event_id%NPrint == 0) G4cout << ">>> Event " << event_id << G4endl;

  // Digitize this event
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  TargetDigitizer* targetDM = (TargetDigitizer*)theDM->FindDigitizerModule("TargetDigitizer");
  targetDM->Digitize();
  PVetoDigitizer* pVetoDM = (PVetoDigitizer*)theDM->FindDigitizerModule("PVetoDigitizer");
  pVetoDM->Digitize();
  EVetoDigitizer* eVetoDM = (EVetoDigitizer*)theDM->FindDigitizerModule("EVetoDigitizer");
  eVetoDM->Digitize();
  HEPVetoDigitizer* hepVetoDM = (HEPVetoDigitizer*)theDM->FindDigitizerModule("HEPVetoDigitizer");
  hepVetoDM->Digitize();
  ECalDigitizer* eCalDM = (ECalDigitizer*)theDM->FindDigitizerModule("ECalDigitizer");
  eCalDM->Digitize();
  SACDigitizer* sacDM = (SACDigitizer*)theDM->FindDigitizerModule("SACDigitizer");
  sacDM->Digitize();
  TPixDigitizer* tPixDM = (TPixDigitizer*)theDM->FindDigitizerModule("TPixDigitizer");
  tPixDM->Digitize();

  // Save event to root file
  RootIOManager::GetInstance()->SaveEvent(evt);

  //MyEvent *TheEvent = MyEvent::GetInstance();
  //MySimEvent *simEvt = TheEvent->GetSimEvent();
  //MyEventGenerator *genEvt = TheEvent->GetGenEvent();
  //std::cout << "Before: Number of tracker clusters: " <<  simEvt->GetTrackerClusters()->size() << std::endl;

  //G4cout << "================================================" << G4endl  ;
  //G4cout << "               Event                            " << G4endl  ;
  //G4cout << "Number of primaries in that event: "<< TheEvent->GetGenEvent()->getParticles()->size() << G4endl;
  //std::vector<MyParticle>::iterator it;
  //it = genEvt->getParticles()->begin();
  //while (it != genEvt->getParticles()->end()) {
  //  G4cout << "Particle:  " << it->getType() << G4endl;
  //  it++;
  //}
  //G4cout << "Number of secondaries in that event: "<< simEvt->GetParticles()->size() << G4endl;
  //it = simEvt->GetParticles()->begin();
  //while (it != simEvt->GetParticles()->end()) {
  //  G4cout << "Particle:  " << it->getType() << G4endl;
  //  it++;
  //}
  //G4cout << "================================================" << G4endl  ;
  
  G4int nHC = 0;
  G4HCofThisEvent* LHC = evt->GetHCofThisEvent(); //list of Hit collections
  if (LHC) nHC = LHC->GetNumberOfCollections();
  //  G4cout<<"N collections "<<nHC<<G4endl;
  for(G4int iHC=0; iHC<nHC; iHC++) {
    G4String HCname = LHC->GetHC(iHC)->GetName();  //nome della collezione
    if (HCname == "ECalCollection") {
      AddECryHits((ECalHitsCollection*) (LHC->GetHC(iHC)));
      FindClusters();
    } else if (HCname == "TargetCollection") {
      AddTargetHits((TargetHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "HEPVetoCollection") {
      AddHEPVetoHits((HEPVetoHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "PVetoCollection") {
      AddPVetoHits((PVetoHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "EVetoCollection") {
      AddEVetoHits((EVetoHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "SACCollection") {
      AddSACHits((SACHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "LAVCollection") {
      AddLAVHits((LAVHitsCollection*)(LHC->GetHC(iHC)));
    }
  }
  int Ncells=0;

  //Retrieve beam Infos!
  G4double BeamE  = myStepping->GetPositronE();
  G4double BeamX  = myStepping->GetPositronX();
  G4double BeamY  = myStepping->GetPositronY();
  G4double BeamZ  = myStepping->GetPositronZ();
  G4double PrimE  = myStepping->GetPrimE();
  
  
  G4ThreeVector PositronMomentum = myStepping->GetPositronMomentum();
  // G4cout << " Getting the momentum to store:   " << PositronMomentum << G4endl;

  fHistoManager->myEvt.PMomX = PositronMomentum.x();
  fHistoManager->myEvt.PMomY = PositronMomentum.y();
  fHistoManager->myEvt.PMomZ = PositronMomentum.z();



  ProcID = myStepping->GetPhysProc();   
  fHistoManager->FillHisto(1,ETotCal);
  fHistoManager->FillHisto(14,ProcID);
  //  fHistoManager->FillHisto(15,BeamZ);

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
  fHistoManager->myEvt.NTNCal           = CalNPart;
  fHistoManager->myEvt.NTNTracks        = NTracks;
  fHistoManager->myEvt.NTNHEPVetoTracks = NHEPVetoTracks;
  fHistoManager->myEvt.NTNPVetoTracks   = NPVetoTracks;
  fHistoManager->myEvt.NTNEVetoTracks   = NEVetoTracks;
  fHistoManager->myEvt.NTSACNHit        = SACTracks;
  fHistoManager->myEvt.NTLAVNHit        = LAVTracks;
  fHistoManager->myEvt.NTNTarget        = NTarget;

  fHistoManager->myEvt.NTEtot     = ETotCal;
  fHistoManager->myEvt.NTIDProc   = ProcID;
  fHistoManager->myEvt.NTPBeam    = BeamE;
  fHistoManager->myEvt.NTPriBeam  = PrimE;
  fHistoManager->myEvt.NTXBeam    = BeamX;
  fHistoManager->myEvt.NTYBeam    = BeamY;
  
  fHistoManager->myEvt.NTETarget  = ETarget; //capisci come fare per 10^4 event per burst
  fHistoManager->myEvt.NTTTarget  = TTarget;
  fHistoManager->myEvt.NTXTarget  = XTarget;
  fHistoManager->myEvt.NTYTarget  = YTarget;

  for(int i=0;i<SACTracks;i++){	  
    if(i>MaxTracks-1) break;
    //    G4cout<<evt->GetEventID()<<" Event action NSAC Tr "<<SACTracks<<" "<< SACEtrack[i]<<" "<<SACTrackTime[i]<<G4endl;
    fHistoManager->myEvt.NTSACE[i]     = SACEtrack[i];
    fHistoManager->myEvt.NTSACT[i]     = SACTrackTime[i];
    fHistoManager->myEvt.NTSACPType[i] = SACPType[i];
    fHistoManager->myEvt.NTSACX[i]     = SACX[i];
    fHistoManager->myEvt.NTSACY[i]     = SACY[i];
    fHistoManager->myEvt.NTSACCh[i]    = SACCh[i];
  }

  for(int i=0;i<CalNPart;i++){
    if(i>19) break;
    //    G4cout<<"NLAV Tr "<<LAVTracks<<" "<< LAVEtrack[i]<<" "<<LAVTrackTime[i]<<G4endl;
    fHistoManager->myEvt.NTCalPartE[i]     =  CalE[i];
    fHistoManager->myEvt.NTCalPartT[i]     =  CalTime[i];
    fHistoManager->myEvt.NTCalPartPType[i] =  CalPType[i];
    fHistoManager->myEvt.NTCalPartX[i]     =  CalX[i];
    fHistoManager->myEvt.NTCalPartY[i]     =  CalY[i];
  }
  
  for(int i=0;i<  LAVTracks;i++){
    if(i>100) break;

    fHistoManager->myEvt.NTLAVE    [i] = LAVEtrack[i];
    fHistoManager->myEvt.NTLAVT    [i] = LAVTrackTime[i];
    fHistoManager->myEvt.NTLAVPType[i] = LAVPType[i];
    fHistoManager->myEvt.NTLAVX    [i] =LAVX[i]; 
    fHistoManager->myEvt.NTLAVY    [i] =LAVY[i] ;
  }
  

  for(int i=0;i<NHEPVetoTracks;i++){  //BUG on number of channel!
	  if(i>MaxTracks-1) break;
	  fHistoManager->myEvt.NTHEPVetoTrkEne[i]    = HEPVetoEtrack[i];
	  if(HEPVetoTrackCh[i]<100 && HEPVetoTrackCh[i]>-100) fHistoManager->myEvt.NTHEPVetoTrkFinger[i] = HEPVetoTrackCh[i];
	  fHistoManager->myEvt.NTHEPVetoTrkTime[i]   = HEPVetoTrackTime[i];
	  fHistoManager->myEvt.NTHEPVetoFingerE[i]   = ETotHEPVeto[HEPVetoTrackCh[i]]; //Just one finger per track to be improved!
	  fHistoManager->myEvt.NTHEPVetoX[i]   = HEPVetoX[i]; //Just one finger per track to be improve!
	  fHistoManager->myEvt.NTHEPVetoY[i]   = HEPVetoY[i]; //Just one finger per track to be improved!
  }
  for(int iBar=0;iBar<NHEPVetoBars;iBar++) {
    fHistoManager->myEvt.NTHEPVetoClIndex[iBar] = HEPVetoClIndex[iBar];
    for(int iCluster = 0; iCluster < HEPVetoClIndex[iBar] ; iCluster++) {
      fHistoManager->myEvt.NTHEPVetoTimeCl[iBar][iCluster] = HEPVetoTimeCl[iBar][iCluster];
      fHistoManager->myEvt.NTHEPVetoECl[iBar][iCluster] = HEPVetoECl[iBar][iCluster] ;
    }
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
  for(int iBar=0;iBar<NPVetoBars;iBar++) {
    fHistoManager->myEvt.NTPVetoBarEnergy[iBar] = ETotPVeto[iBar];
    fHistoManager->myEvt.NTPVetoBarTime[iBar] = TimePVeto[iBar];
    fHistoManager->myEvt.NTPVetoClIndex[iBar] = PVetoClIndex[iBar];
    for(int iCluster = 0; iCluster < PVetoClIndex[iBar] ; iCluster++) {
      fHistoManager->myEvt.NTPVetoTimeCl[iBar][iCluster] = PVetoTimeCl[iBar][iCluster];
      fHistoManager->myEvt.NTPVetoECl[iBar][iCluster] = PVetoECl[iBar][iCluster] ;
    }
  }

  for(int i=0;i<NEVetoTracks;i++){  //BUG on number of channel!
	  if(i>MaxTracks-1) break;
	  fHistoManager->myEvt.NTEVetoTrkEne[i]  = EVetoEtrack[i];
	  //	  if(EVetoTrackCh[i]<100 && EVetoTrackCh[i]>-100) fHistoManager->myEvt.NTVetoTrkFinger[i] = EVetoTrackCh[i];
	  fHistoManager->myEvt.NTEVetoTrkFinger[i]  = EVetoTrackCh[i];
	  fHistoManager->myEvt.NTEVetoTrkTime[i] = EVetoTrackTime[i];
	  fHistoManager->myEvt.NTEVetoFingerE[i] = ETotEVeto[EVetoTrackCh[i]]; //Just one finger per track to be improved!
	  fHistoManager->myEvt.NTEVetoX[i]       = EVetoX[i]; //Just one finger per track to be improve!
	  fHistoManager->myEvt.NTEVetoY[i]       = EVetoY[i]; //Just one finger per track to be improved!xs
	  //	  G4cout<<i<<" Tracker Lay "<<EVetoTrackCh[i]<<" "<<EVetoEtrack[i]<<" "<<EVetoTrackTime[i]<<G4endl;
	  //	  G4cout<<VetoX[i]<<" "<<myEvt.NTVetoY[i]<<G4endl;
  }
  for(int iBar=0;iBar<NEVetoBars;iBar++) {
    fHistoManager->myEvt.NTEVetoClIndex[iBar] = EVetoClIndex[iBar];
    for(int iCluster = 0; iCluster < EVetoClIndex[iBar] ; iCluster++) {
      fHistoManager->myEvt.NTEVetoTimeCl[iBar][iCluster] = EVetoTimeCl[iBar][iCluster];
      fHistoManager->myEvt.NTEVetoECl[iBar][iCluster] = EVetoECl[iBar][iCluster] ;
    }
  }

  for(int i=0;i<NClusters;i++){	
    //    G4cout<<"DDD CALO"<<EneCl[i]<<" "<<NTQCl[i]<<G4endl;
    if(i>19) break;              
    fHistoManager->myEvt.NTECluster[i]  = EneCl[i];
    fHistoManager->myEvt.NTQCluster[i]  = QCl[i];
    fHistoManager->myEvt.NTXCluster[i]  = XCl[i];   
    fHistoManager->myEvt.NTYCluster[i]  = YCl[i];   
    fHistoManager->myEvt.NTThCluster[i] = ThCl[i];
    fHistoManager->myEvt.NTM2Cluster[i] = MM2[i];
    fHistoManager->myEvt.NTTCluster[i]  = TimeCl[i];
    fHistoManager->myEvt.NTNClusCells[i]= NCellsCl[i];
  }

  //  if(SACTracks>0) for(int ll=0;ll<SACTracks;ll++) fHistoManager->FillHisto(18,SACEtrack[ll]);

//  for(int i=0;i< ECalNCells;i++){
//    fHistoManager->myEvt.NTECell[i]=ETotCry[i];
//    fHistoManager->myEvt.NTQCell[i]=QTotCry[i];
//    fHistoManager->myEvt.NTTCell[i]=TimeCry[i];
//  }

//  if(IsTrackerRecoON==1){
//    if(ETotCal>EMinSaveNT || fHistoManager->myEvt.NTNTrClus>4) fHistoManager->FillNtuple(&(fHistoManager->myEvt));
//  }else{
  if(ETotCal>EMinSaveNT || SACTracks>0) fHistoManager->FillNtuple(&(fHistoManager->myEvt));
//    fHistoManager->FillNtuple(&(fHistoManager->myEvt));
    //    if(ETotCal>EMinSaveNT || NTracks>0.) fHistoManager->FillNtuple(&(fHistoManager->myEvt));
    //  }
}

void EventAction::AddECryHits(ECalHitsCollection* hcont)
{
  G4double ETotEvt=0;
  G4int nHits = hcont->entries();

  G4double NRows= Egeom->GetECalNRows();  // righe sulla y 
  G4double NCols= Egeom->GetECalNCols(); // colonne sulla X

  for(G4int jj=0;jj<NRows;jj++){
    for(G4int kk=0;kk<NCols;kk++){
      MatEtot  [jj][kk]=0.;   
      MatQtot  [jj][kk]=0.;   
      MatTstart[jj][kk]=1.e6; 
      MatUsed  [jj][kk]=0;   
    }
  }
  
  // Copy all hits from G4 to ROOT event
  //  std::cout << "Number of ECAL hits: " <<  nHits << std::endl;
  for (G4int h=0; h<nHits; h++) {
    ECalHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      //G4int index = hit->GetCryNb();
      G4int index = hit->GetChannelId();
      G4int Xind = index%100;
      G4int Yind = index/100;
      MatEtot[Yind][Xind] += hit->GetEnergy(); //somma le energie su tutti gli hit di ogni cristallo
      ETotCal += hit->GetEnergy();
      //  std::cout << "Hit energy : " << hit->GetEnergy() << std::endl;
      if(hit->GetTime() < MatTstart[Yind][Xind]) MatTstart[Yind][Xind] =  hit->GetTime();//assing to each crystal the time of the first hit!
    }
  }//end of loop on hits
  
  for(int xx=0;xx<NCols;xx++){
    for(int yy=0;yy<NRows;yy++){
      MatQtot[yy][xx]=GetCharge(MatEtot[yy][xx]);
    }
  }
  
}
void EventAction::FindClusters()
{
  //  G4int NcellsCl=0;
  G4double NRows= Egeom->GetECalNRows();  // righe sulla y 
  G4double NCols= Egeom->GetECalNCols(); // colonne sulla X
  NClusters=0;
  while(1){
    G4int SeedIndX=-1;
    G4int SeedIndY=-1;
    G4double EMax = 0.; 

    //Seed Search
    for(G4int jj=0;jj<NRows;jj++){
      for(G4int kk=0;kk<NCols;kk++){
	if(!MatUsed[jj][kk] && MatEtot[jj][kk]>EMax){
	  SeedIndX = kk;
	  SeedIndY = jj;
	  EMax     = MatEtot[jj][kk]; 
	}
      }
    }
    if( EMax < SeedE ) break;  // Attenzione SeedE in Constants.hh
     
    EneCl[NClusters]   =0;
    QCl[NClusters]     =0;
    XCl[NClusters]     =0;
    YCl[NClusters]     =0;
    ThCl[NClusters]    =0;
    MM2[NClusters]     =0;
    TimeCl[NClusters]  =0;
    NCellsCl[NClusters]=0;
    
    G4int NAddCl=2;
    for(G4int jj=SeedIndY-NAddCl; jj<=SeedIndY+NAddCl; jj++){
      if(jj>=0 && jj<NRows){
	for(G4int kk=SeedIndX-NAddCl; kk<=SeedIndX+NAddCl; kk++){
	  if( kk>=0 && kk<NCols){
	    if(MatEtot[jj][kk]>CellsE && !MatUsed[jj][kk]) {//some fraction of E(SeedCell) // Attenzione ECells in Constants.hh
	      EneCl[NClusters] += MatEtot[jj][kk];
	      QCl[NClusters]   += MatQtot[jj][kk];
	      TimeCl[NClusters]+= MatTstart[jj][kk]  * MatEtot[jj][kk];
	      XCl[NClusters]   += Egeom->GetCrystalPosX(jj,kk)* MatEtot[jj][kk];
	      YCl[NClusters]   += Egeom->GetCrystalPosY(jj,kk)* MatEtot[jj][kk];
	      MatUsed[jj][kk]  = 1;
	      NCellsCl[NClusters]++;
	    }
	  }
	}    
      }
    }
    TimeCl[NClusters]/= EneCl[NClusters];     
    XCl[NClusters]   /= EneCl[NClusters];     
    YCl[NClusters]   /= EneCl[NClusters];     

    G4double DxDz=XCl[NClusters]/(Egeom->GetECalFrontFacePosZ() - Tgeom->GetTargetFrontFacePosZ() );
    G4double DyDz=YCl[NClusters]/(Egeom->GetECalFrontFacePosZ() - Tgeom->GetTargetFrontFacePosZ() );
    G4double norma=sqrt(DxDz*DxDz+DyDz*DyDz+1.);

    G4double GMom[4],BeamMom[4],TargetEleMom[4],P4Miss[4];
    G4double GDir[3];

    GDir[0]=DxDz/norma;
    GDir[1]=DyDz/norma;
    GDir[2]=1./norma;

    //Compute gamma momentum (introduced average energy calibration constant 0.97 M. Raggi 24/04/2016)
    GMom[0]= DxDz/norma * EneCl[NClusters]/0.97;
    GMom[1]= DyDz/norma * EneCl[NClusters]/0.97;
    GMom[2]= 1./norma   * EneCl[NClusters]/0.97;
    GMom[3]= EneCl[NClusters]/0.97;         
    
    //The target electron is at rest
    TargetEleMom[0]=0.;
    TargetEleMom[1]=0.;
    TargetEleMom[2]=0.;
    TargetEleMom[3]=0.511;
    
    G4ThreeVector BDir=Bpar->GetBeamDirection().unit();
    //Beam Momentum from Beam Paramets class 
    BeamMom[0]=BDir.x()*Bpar->GetBeamMomentum();
    BeamMom[1]=BDir.y()*Bpar->GetBeamMomentum();
    BeamMom[2]=BDir.z()*Bpar->GetBeamMomentum();
    BeamMom[3]=sqrt(Bpar->GetBeamMomentum()*Bpar->GetBeamMomentum()+0.511*0.511); 
    
    for (int i=0; i<4; i++){ 
      P4Miss[i]=TargetEleMom[i]+BeamMom[i]-GMom[i];
    }
    MM2[NClusters] = P4Miss[3]*P4Miss[3]-P4Miss[2]*P4Miss[2]-P4Miss[1]*P4Miss[1]-P4Miss[0]*P4Miss[0];
  
    G4double product=0.;
    G4double BDire[3];

    BDire[0]=BDir.x();
    BDire[1]=BDir.y();
    BDire[2]=BDir.z();

    for (int i=0; i<3; i++)  product+= GDir[i]*BDire[i];
    ThCl[NClusters] = acos (product);//* 180.0 / 3.14159265;
    //    printf("Th Cl %f\n",ThCl[NClusters]);

    G4double ProcID = myStepping->GetPhysProc();    
    printf("PROCID %f\n",ProcID);
    if(ProcID==1) fHistoManager->FillHisto2(8,EneCl[NClusters],ThCl[NClusters],1.); //Nclus==1
    if(ProcID==2) fHistoManager->FillHisto2(9,EneCl[NClusters],ThCl[NClusters],1.); //Nclus==2
    if (NClusters==0 && EneCl[NClusters]>5.) {
      fHistoManager->FillHisto(7,EneCl[NClusters]);
      fHistoManager->FillHisto(8,ThCl[NClusters]);
      fHistoManager->FillHisto(9,MM2[NClusters]);
      fHistoManager->FillHisto(15,QCl[NClusters]);
    }
    NClusters++;
    if(NClusters==2 && fabs(TimeCl[0]-TimeCl[1]<2.) ){ 
     G4double mgg= GGMass();
     fHistoManager->FillHisto(16,mgg);
    }
    if(NClusters>19){ 
      G4cout<<"too many clusters \n!!"<<G4endl;
      break;
    }
  }
  fHistoManager->FillHisto(11,NClusters);
}

void EventAction::AddTargetHits(TargetHitsCollection* hcont)  //Target readout module
{
  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++) {
    TargetHit * hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      ETarget += hit->GetEdep(); //somma le energie su tutti gli hit di ogni cristalli
      if (hit->GetTime()<TTarget) TTarget  = hit->GetTime();
      XTarget += hit->GetX();
      YTarget += hit->GetY();
      NTarget++;
    }
  }//end of loop
  XTarget/=NTarget;
  YTarget/=NTarget;
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
  for(int iBar=0;iBar<NPVetoBars;iBar++){
    HEPVetoClIndex[iBar] = 0;
    for(int iCluster = 0; iCluster < HEPVetoClIndex[iBar] ; iCluster++) {
      HEPVetoTimeCl[iBar][iCluster] = 0.;
      HEPVetoECl[iBar][iCluster]  = 0.;
    }
  }

  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++) {		
    HEPVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit

    if ( hit != 0 ) {
      int iBar = hit->GetChannelId();
      if (iBar > NHEPVetoBars ) {
	std::cerr << "[EventAction] [HEPVeto]  Bar index outside range" << std::endl;
	exit(0);
      }

      int newhit = 1;

      for(int iCluster = 0; iCluster < HEPVetoClIndex[iBar] ; iCluster++) {
	if( fabs( hit->GetTime() - HEPVetoTimeCl[iBar][iCluster] ) < 5. ) {
	  newhit=0;
	  HEPVetoTimeCl[iBar][iCluster] = 
	    (hit->GetTime()*hit->GetHitE() + HEPVetoECl[iBar][iCluster]*HEPVetoTimeCl[iBar][iCluster])/
	    ( hit->GetHitE() + HEPVetoECl[iBar][iCluster]  ) ;
	  HEPVetoECl[iBar][iCluster] +=  hit->GetHitE();
	}
	if(newhit==0) break;
      }

      if (newhit == 1 && HEPVetoClIndex[iBar]< NHEPVetoMaxNCl) {
	HEPVetoECl[iBar][HEPVetoClIndex[iBar]] = hit->GetHitE();
	HEPVetoTimeCl[iBar][HEPVetoClIndex[iBar]]  = hit->GetTime();
	HEPVetoClIndex[iBar]++;
      }


      ETotHEPVeto[hit->GetHEPVetoNb()] += hit->GetHitE();  //sum single fingers energies and get total finger
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && NHEPVetoTracks < MaxTracks && hit->GetTrackEnergy()>0.1*MeV) {
    	  HEPVetoTrackCh[NHEPVetoTracks]    = hit->GetHEPVetoNb();   //bugs gives crazy numbers
    	  HEPVetoEtrack[NHEPVetoTracks]     = hit->GetTrackEnergy();
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
    TimePVeto[jj]=0.0;
    PVetoTrackCh[jj]=0;
    PVetoTrackTime[jj]=0;
    PVetoX[jj]=0;
    PVetoY[jj]=0;

  }

  for(int iBar=0;iBar<NPVetoBars;iBar++){
    TimePVeto[iBar]=0.0;
    ETotPVeto[iBar]=0.0;
    PVetoClIndex[iBar] = 0;
    for(int iCluster = 0; iCluster < PVetoClIndex[iBar] ; iCluster++) {
      PVetoTimeCl[iBar][iCluster] = 0.;
      PVetoECl[iBar][iCluster]  = 0.;
    }
  }


  double puppo=0;
  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++) {									
    PVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit					
    if ( hit != 0 ) {
      // ETotPVeto[hit->GetPVetoNb()] += hit->GetHitE();  //sum single fingers energies and get total finger
      int iBar = hit->GetChannelId();
      if (iBar > NPVetoBars ) {
	std::cerr << "[EventAction] [PVeto]  Bar index outside range" << std::endl;
	exit(0);
      }

      int newhit = 1;

      for(int iCluster = 0; iCluster < PVetoClIndex[iBar] ; iCluster++) {
	if( fabs( hit->GetTime() - PVetoTimeCl[iBar][iCluster] ) < 0.1) {
	  newhit=0;
	  PVetoTimeCl[iBar][iCluster] = 
	    (hit->GetTime()*hit->GetHitE() + PVetoECl[iBar][iCluster]*PVetoTimeCl[iBar][iCluster])/
	    ( hit->GetHitE() + PVetoECl[iBar][iCluster]  ) ;
	  PVetoECl[iBar][iCluster] +=  hit->GetHitE();
	}
	if(newhit==0) break;
      }

      if (newhit == 1 && PVetoClIndex[iBar]< NPVetoMaxNCl) {
	PVetoECl[iBar][PVetoClIndex[iBar]] = hit->GetHitE();
	PVetoTimeCl[iBar][PVetoClIndex[iBar]]  = hit->GetTime();
	PVetoClIndex[iBar]++;
      }
      
      TimePVeto[hit->GetPVetoNb()] = 
	(TimePVeto[hit->GetPVetoNb()]*ETotPVeto[hit->GetPVetoNb()] + hit->GetTime()*hit->GetHitE())/
	(ETotPVeto[hit->GetPVetoNb()]+hit->GetHitE());

      ETotPVeto[hit->GetPVetoNb()] += hit->GetHitE();  //sum single fingers energies and get total finger
      ETotPVetoEvt += hit->GetHitE();

      //old style variable deprected.
      puppo= hit->GetEdep();
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && NPVetoTracks < MaxTracks && hit->GetTrackEnergy() > 0.1*MeV) {
	PVetoTrackCh[NPVetoTracks]    = hit->GetPVetoNb();   //bugs gives crazy numbers
	PVetoEtrack[NPVetoTracks]     = hit->GetTrackEnergy();
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

void EventAction::AddEVetoHits(EVetoHitsCollection* hcont){
  //  G4cout<<"vaffanculo "<<G4endl;
  G4double TotEVetoEvt=0;
  G4int LastID=-1;
  for(G4int jj=0;jj<MaxTracks;jj++){
    ETotEVeto[jj]=0.0;						  
    EVetoTrackCh[jj]=0;
    EVetoTrackTime[jj]=0;
    EVetoX[jj]=0;
    EVetoY[jj]=0;
  }
  for(int iBar=0;iBar<NPVetoBars;iBar++){
    EVetoClIndex[iBar] = 0;
    for(int iCluster = 0; iCluster < EVetoClIndex[iBar] ; iCluster++) {
      EVetoTimeCl[iBar][iCluster] = 0.;
      EVetoECl[iBar][iCluster]  = 0.;
    }
  }


  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++) {									
    EVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit					
    if ( hit != 0 ) {

      int iBar = hit->GetChannelId();
      if (iBar > NEVetoBars ) {
	std::cerr << "[EventAction] [EVeto]  Bar index outside range" << std::endl;
	exit(0);
      }

      int newhit = 1;

      for(int iCluster = 0; iCluster < EVetoClIndex[iBar] ; iCluster++) {
	if( fabs( hit->GetTime() - EVetoTimeCl[iBar][iCluster] ) < 5.) {
	  newhit=0;
	  EVetoTimeCl[iBar][iCluster] = 
	    (hit->GetTime()*hit->GetHitE() + EVetoECl[iBar][iCluster]*EVetoTimeCl[iBar][iCluster])/
	    ( hit->GetHitE() + EVetoECl[iBar][iCluster]  ) ;
	  EVetoECl[iBar][iCluster] +=  hit->GetHitE();
	}
	if(newhit==0) break;
      }

      if (newhit == 1 && EVetoClIndex[iBar]< NEVetoMaxNCl) {
	EVetoECl[iBar][EVetoClIndex[iBar]] = hit->GetHitE();
	EVetoTimeCl[iBar][EVetoClIndex[iBar]]  = hit->GetTime();
	EVetoClIndex[iBar]++;
      }



      ETotEVeto[hit->GetEVetoNb()] += hit->GetHitE();  //sum single fingers energies and get total finger
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && NEVetoTracks < MaxTracks && hit->GetTrackEnergy()>0.1*MeV) {
      //      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID) {
	EVetoTrackCh[NEVetoTracks]    = hit->GetEVetoNb();   //bugs gives crazy numbers
	EVetoEtrack[NEVetoTracks]     = hit->GetTrackEnergy();
	EVetoTrackTime[NEVetoTracks]  = hit->GetTime();
	EVetoX[NEVetoTracks]          = hit->GetX();
	EVetoY[NEVetoTracks]          = hit->GetY();
	NEVetoTracks++;
	//	G4cout<<"cazzo "<<NEVetoTracks<<G4endl;
	//G4cout<<"trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" Strip Numb "<<hit->GetEVetoNb()<<G4endl;
      }
      if(NEVetoTracks>MaxTracks) break; 
      LastID = hit->GetTrackID();
    }
  }

}

void EventAction::AddSACHits(SACHitsCollection* hcont)
{

  G4int LastID=-1;
  G4int nHits = hcont->entries();

  if (nHits == 0) return;

  for(G4int jj=0;jj<MaxTracks;jj++){
    ETotSAC[jj]=0.0;										     
    SACTrackTime[jj]=0.0;										     
    SACEtrack[jj]=0.0;
    SACPType[jj]=0.0;
  }
  SACTracks = 0;
  for (G4int h=0; h<nHits; h++) {
    SACHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && hit->GetEdep()>0.1*MeV && SACTracks < MaxTracks) {
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

void EventAction::AddSACHitsStep(G4double E,G4double T, G4int Ptype, G4double X, G4double Y, G4int NCry)
{
  //  static G4int SACTracks  = 0;
  if(SACTracks < MaxTracks){
    SACEtrack[SACTracks]    = E;
    SACTrackTime[SACTracks] = T;
    SACPType[SACTracks]     = Ptype;
    SACX[SACTracks]         = X;
    SACY[SACTracks]         = Y;
    SACCh[SACTracks]        = NCry;

    SACTracks++;
    //    G4cout<<SACTracks<<" E "<< E <<" T "<< T <<"Ptype "<<Ptype<<" X "<<X<<" Y "<<Y<<" Ncry "<<NCry<<G4endl;
  }
}

void EventAction::AddCalHitsStep(G4double E,G4double T, G4int Ptype, G4double X, G4double Y)
{
  //  static G4int SACTracks  = 0;
  if(CalNPart < 19){
    CalE[CalNPart]    = E;
    CalTime[CalNPart] = T;
    CalPType[CalNPart]= Ptype;
    CalX[CalNPart]    = X;
    CalY[CalNPart]    = Y;
    CalNPart++;
    //    G4cout<<CalNPart<<" E "<< E <<" T "<< T <<"Ptype "<<Ptype<<" X "<<X<<" Y "<<Y<<G4endl;
  }
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
      if(hit->GetTrackID()!=0 && hit->GetTrackID()!=LastID && hit->GetETrack()>0.01*MeV && LAVTracks < MaxTracks) {
	//	ETotLAV[hit->GetLAVNb()] += hit->GetEdep();  //sum single fingers energies and get total finger
	//    	  LAVTrackCh[LAVTracks] = hit->GetLAVNb();
	LAVEtrack[LAVTracks]    = hit->GetETrack();
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

G4double EventAction::GetCharge(G4double Energia)
{
  G4double LY      =   25000.; //photons per MeV
  G4double Gain    =     2E5;
  G4double Echarge = 1.6E-19;
  G4double CollEff =     0.1; //geometrical factor due to photodetector area
  G4double QE      =    0.25; //photodetector quantum efficiency
  G4double NPh = Energia*LY*QE*CollEff;
  //  G4double rnd = G4RandGauss::shoot(NPh,sqrt(NPh)); //should be poisson
  G4double rnd = G4Poisson(NPh);
  G4double Charge = rnd*Gain*Echarge/1E-12;  //in pC
  //  G4cout<<"Energia "<<Energia<<" rnd " << rnd <<" nph "<<NPh<<" Charge "<<Charge<<" pC"<<G4endl;
  return Charge;
}


G4double EventAction::GGMass()
{
  double ECalPosiZ=3000.; //
  if(NClusters!=2)                        return -1;  // Need 2 clusters
  double XDiff2 = (XCl[0]-XCl[1])*(XCl[0]-XCl[1]);
  double YDiff2 = (YCl[0]-YCl[1])*(YCl[0]-YCl[1]);
  double Massa  = sqrt(EneCl[0]*EneCl[1]*(XDiff2+YDiff2))/(ECalPosiZ);
  G4cout<<"Massa "<<Massa<<G4endl;
  return Massa;
}
