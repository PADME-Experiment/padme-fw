#include "globals.hh"
#include "Randomize.hh"
 
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "TLorentzVector.h"

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
  Sgeom = SACGeometry::GetInstance(); //M. Raggi 29/06/2018

  //M. Raggi defining default output settings 
  fEnableSaveEcal = 1;
  fEnableSaveSAC  = 0; 
  fEnableSaveVeto = 0; 

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

  SACTracks  = 0;
  SACCh.clear();
  SACEtrack.clear();  
  SACTrackTime.clear();  
  SACPType.clear();  
  SACX.clear();
  SACY.clear();
  SACQ.clear();

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
      //      AddSACHits((SACHitsCollection*)(LHC->GetHC(iHC)));
    } else if (HCname == "LAVCollection") {
      AddLAVHits((LAVHitsCollection*)(LHC->GetHC(iHC)));
    }
  }
  
  //Retrieve beam Infos!
  G4double BeamE  = myStepping->GetPositronE();
  G4double BeamX  = myStepping->GetPositronX();
  G4double BeamY  = myStepping->GetPositronY();
  //G4double BeamZ  = myStepping->GetPositronZ();
  G4double PrimE  = myStepping->GetPrimE();
  
  
  G4ThreeVector PositronMomentum = myStepping->GetPositronMomentum();
  // G4cout << " Getting the momentum to store:   " << PositronMomentum << G4endl;

  fHistoManager->myEvt.PMomX = PositronMomentum.x();
  fHistoManager->myEvt.PMomY = PositronMomentum.y();
  fHistoManager->myEvt.PMomZ = PositronMomentum.z();

  ProcID = myStepping->GetPhysProc();   
  fHistoManager->FillHisto(1,ETotCal);
  fHistoManager->FillHisto(14,ProcID);
  //fHistoManager->FillHisto(15,BeamZ);

  fHistoManager->FillHisto2(30,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);

  if(ProcID==1) fHistoManager->FillHisto2(31,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
  if(ProcID==2) fHistoManager->FillHisto2(32,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
  
  if(NClusters>0){
    fHistoManager->FillHisto2(33,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
    if(ProcID==1) fHistoManager->FillHisto2(34,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
    if(ProcID==2) fHistoManager->FillHisto2(35,myStepping->GetGammaEnergy(),myStepping->GetGammaAngle(),1.);
  }

  NTracks = NPVetoTracks+ NEVetoTracks;
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

  // Revised SAC filling now based on vectors M.Raggi 28/06/2018
  for(int i=0;i<SACTracks;i++){	  
    //    G4cout<<evt->GetEventID()<<" Event action NSAC Tr "<<SACTracks<<" "<< SACEtrack[i]<<" "<<SACTrackTime[i]<<G4endl;
    if(i>MaxTracks-1) break;
    fHistoManager->myEvt.NTSACE.push_back(SACEtrack.at(i));
    fHistoManager->myEvt.NTSACT.push_back(SACTrackTime.at(i));
    //    fHistoManager->myEvt.NTSACPType.push_back(SACPType.at(i));
    fHistoManager->myEvt.NTSACX.push_back(SACX.at(i));
    fHistoManager->myEvt.NTSACY.push_back(SACY.at(i));
    //    fHistoManager->myEvt.NTSACCh.push_back(SACCh.at(i));
    fHistoManager->myEvt.NTSACQ.push_back(SACQ.at(i));
  }
  //  for(int ii=0;ii<25;ii++) fHistoManager->myEvt.NTSACQ.push_back(ETotSACCh[i]);


  //FIX IT
  for(int i=0;i<CalNPart;i++){
    //    if(i>19) break;
    fHistoManager->myEvt.NTCalPartE.push_back(CalE[i]);
    fHistoManager->myEvt.NTCalPartT.push_back(CalTime[i]);
    fHistoManager->myEvt.NTCalPartPType.push_back(CalPType[i]);
    fHistoManager->myEvt.NTCalPartX.push_back(CalX[i]);
    fHistoManager->myEvt.NTCalPartY.push_back(CalY[i]);
  }
  
  for(int i=0;i<LAVTracks;i++){
    //    if(i>100) break;
    fHistoManager->myEvt.NTLAVE.push_back(LAVEtrack[i]);
    fHistoManager->myEvt.NTLAVT.push_back(LAVTrackTime[i]);
    fHistoManager->myEvt.NTLAVPType.push_back(LAVPType[i]);
    fHistoManager->myEvt.NTLAVX.push_back(LAVX[i]); 
    fHistoManager->myEvt.NTLAVY.push_back(LAVY[i]);
  }
  
  // HEP veto ntupla changed to vectors M. Raggi 28/06/2018
  for(int i=0;i<NHEPVetoTracks;i++){  
    fHistoManager->myEvt.NTHEPVetoTrkEne.push_back(HEPVetoEtrack[i]);
    fHistoManager->myEvt.NTHEPVetoTrkFinger.push_back(HEPVetoTrackCh[i]);
    fHistoManager->myEvt.NTHEPVetoTrkTime.push_back(HEPVetoTrackTime[i]);
    fHistoManager->myEvt.NTHEPVetoFingerE.push_back(ETotHEPVeto[HEPVetoTrackCh[i]]); //Just one finger per track to be improved!
    fHistoManager->myEvt.NTHEPVetoX.push_back(HEPVetoX[i]); //Just one finger per track to be improve!
    fHistoManager->myEvt.NTHEPVetoY.push_back(HEPVetoY[i]); //Just one finger per track to be improved!
  }

  for(int iBar=0;iBar<NHEPVetoBars;iBar++) {
    fHistoManager->myEvt.NTHEPVetoClIndex[iBar] = HEPVetoClIndex[iBar];
    for(int iCluster = 0; iCluster < HEPVetoClIndex[iBar] ; iCluster++) {
      fHistoManager->myEvt.NTHEPVetoTimeCl[iBar][iCluster] = HEPVetoTimeCl[iBar][iCluster];
      fHistoManager->myEvt.NTHEPVetoECl[iBar][iCluster] = HEPVetoECl[iBar][iCluster] ;
    }
  }
  // Positron veto ntupla changed to vectors M. Raggi 28/06/2018
  for(int i=0;i<NPVetoTracks;i++){  
    fHistoManager->myEvt.NTPVetoTrkEne.push_back(PVetoEtrack[i]);
    fHistoManager->myEvt.NTPVetoTrkFinger.push_back(PVetoTrackCh[i]);
    fHistoManager->myEvt.NTPVetoTrkTime.push_back(PVetoTrackTime[i]);
    fHistoManager->myEvt.NTPVetoFingerE.push_back(ETotPVeto[PVetoTrackCh[i]]); 
    fHistoManager->myEvt.NTPVetoX.push_back(PVetoX[i]);                        
    fHistoManager->myEvt.NTPVetoY.push_back(PVetoY[i]);                        
  }
  for(int iBar=0;iBar<NPVetoBars;iBar++) {
    fHistoManager->myEvt.NTPVetoBarEnergy[iBar]= ETotPVeto[iBar];
    fHistoManager->myEvt.NTPVetoBarTime[iBar] = TimePVeto[iBar];
    fHistoManager->myEvt.NTPVetoClIndex[iBar] = PVetoClIndex[iBar]; ///means the bar is in cluster  PVetoClIndex[iBar]
    for(int iCluster = 0; iCluster < PVetoClIndex[iBar] ; iCluster++) {
      fHistoManager->myEvt.NTPVetoTimeCl[iBar][iCluster] = PVetoTimeCl[iBar][iCluster];
      fHistoManager->myEvt.NTPVetoECl[iBar][iCluster] = PVetoECl[iBar][iCluster] ;
    }
  }
  
  // EVeto ntupla Moved to Vectors 28/06/2018
  for(int i=0;i<NEVetoTracks;i++){
    fHistoManager->myEvt.NTEVetoTrkEne.push_back(EVetoEtrack[i]);
    fHistoManager->myEvt.NTEVetoTrkFinger.push_back(EVetoTrackCh[i]);
    fHistoManager->myEvt.NTEVetoTrkTime.push_back(EVetoTrackTime[i]);
    fHistoManager->myEvt.NTEVetoFingerE.push_back(ETotEVeto[EVetoTrackCh[i]]); 
    fHistoManager->myEvt.NTEVetoX.push_back(EVetoX[i]); 
    fHistoManager->myEvt.NTEVetoY.push_back( EVetoY[i]); 
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

  //Revised ECAL ntu filling using vectors M. Raggi 27/06/2018
  //  if(NClusters>0) G4cout<<"Filling CALO vectors ECl size"<<EneCl.size()<<" NClusters "<<NClusters<<G4endl;
  for(int i=0;i<NClusters;i++){	
    fHistoManager->myEvt.NTECluster.push_back(EneCl.at(i));
    fHistoManager->myEvt.NTQCluster.push_back(QCl.at(i));
    fHistoManager->myEvt.NTXCluster.push_back(XCl.at(i));   
    fHistoManager->myEvt.NTYCluster.push_back(YCl.at(i));   
    fHistoManager->myEvt.NTThCluster.push_back(ThCl.at(i));
    fHistoManager->myEvt.NTM2Cluster.push_back(MM2.at(i));
    fHistoManager->myEvt.NTTCluster.push_back(TimeCl.at(i));
    fHistoManager->myEvt.NTNClusCells.push_back(NCellsCl.at(i));
  }

//  for(int i=0;i< ECalNCells;i++){
//    fHistoManager->myEvt.NTECell.push_back(ETotCry.at(i));
//    fHistoManager->myEvt.NTQCell.push_back(QTotCry.at(i));
//    fHistoManager->myEvt.NTTCell.push_back(TimeCry.at(i));
//  }
  
  // Set conditions to write NTU using the datacards.  M. raggi 23/06/2018
  if( (ETotCal>5. && fEnableSaveEcal) || (SACTracks>0 && fEnableSaveSAC) || (NTracks>0 &&  fEnableSaveVeto) ){ 
    fHistoManager->FillNtuple(&(fHistoManager->myEvt));
  }else{
    //    G4cout<<"No event saved in the FastMC output"<<NTracks<<" "<<fEnableSaveVeto<<G4endl;
  }
}

void EventAction::AddECryHits(ECalHitsCollection* hcont)
{
  //G4double ETotEvt=0;
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

void EventAction::FindClusters() //implement a fast square cluster algorithm revised M. Raggi 27/06/2018
{
  G4double NRows= Egeom->GetECalNRows();  // righe sulla y 
  G4double NCols= Egeom->GetECalNCols(); // colonne sulla X
  G4int   NAddCl= 2;   //number of raw and columns added into the cluster 2 means a 5x5 cells cluster
  
  NClusters=0;
  EneCl.clear();   
  QCl.clear();     
  XCl.clear();     
  YCl.clear();     
  ThCl.clear();    
  MM2.clear();     
  TimeCl.clear();  
  NCellsCl.clear();
  
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
    //    G4cout<<"Seed found "<<EMax<<"Seed Ind"<<SeedIndY<<G4endl;
    double ECL=0,NCells=0,QCL_T=0,TCL=0,X_CL=0,Y_CL=0;

    for(G4int jj=SeedIndY-NAddCl; jj<=SeedIndY+NAddCl; jj++){
      if(jj>=0 && jj<NRows){
	for(G4int kk=SeedIndX-NAddCl; kk<=SeedIndX+NAddCl; kk++){
	  if( kk>=0 && kk<NCols){
	    if(MatEtot[jj][kk]>CellsE && !MatUsed[jj][kk]) {//some fraction of E(SeedCell) // Attenzione ECells in Constants.hh
	      ECL   += MatEtot[jj][kk];
	      QCL_T += MatQtot[jj][kk];
	      TCL   += MatTstart[jj][kk]  * MatEtot[jj][kk];
	      X_CL  += Egeom->GetCrystalPosX(jj,kk)* MatEtot[jj][kk];
	      Y_CL  += Egeom->GetCrystalPosY(jj,kk)* MatEtot[jj][kk];
	      MatUsed[jj][kk]  = 1;
	      NCells++;
	    }
	  }
	}    
      }
    }
    EneCl.push_back(ECL);
    NCellsCl.push_back(NCells);
    QCl.push_back(QCL_T);	      
    XCl.push_back(X_CL/ECL);
    YCl.push_back(Y_CL/ECL);
    TimeCl.push_back(TCL/ECL);
    G4double DxDz=XCl.at(NClusters)/(Egeom->GetECalFrontFacePosZ() - Tgeom->GetTargetFrontFacePosZ() );
    G4double DyDz=YCl.at(NClusters)/(Egeom->GetECalFrontFacePosZ() - Tgeom->GetTargetFrontFacePosZ() );
    G4double norma=sqrt(DxDz*DxDz+DyDz*DyDz+1.);
//
    G4double GMom[4],BeamMom[4];//,P4_Miss[4];
    TLorentzVector P4Gamma,P4em,P4Beam,P4Miss;
    G4double GDir[3];
    GDir[0]=DxDz/norma;
    GDir[1]=DyDz/norma;
    GDir[2]=1./norma;

 //Compute gamma momentum (introduced average energy calibration constant 0.97 M. Raggi 24/04/2016)
    GMom[0]= DxDz/norma * EneCl.at(NClusters)/0.97;
    GMom[1]= DyDz/norma * EneCl.at(NClusters)/0.97;
    GMom[2]= 1./norma   * EneCl.at(NClusters)/0.97;
    GMom[3]= EneCl.at(NClusters)/0.97; 
    P4Gamma.SetPxPyPzE(GMom[0],GMom[1],GMom[2],GMom[3]);        

    //The target electron is at rest
    P4em.SetPxPyPzE(0.,0.,0.,0.511);// in MeV
    
    G4ThreeVector BDir=Bpar->GetBeamDirection().unit();
//    //Beam Momentum from Beam Paramets class 
    BeamMom[0]=BDir.x()*Bpar->GetBeamMomentum();
    BeamMom[1]=BDir.y()*Bpar->GetBeamMomentum();
    BeamMom[2]=BDir.z()*Bpar->GetBeamMomentum();
    BeamMom[3]=sqrt(Bpar->GetBeamMomentum()*Bpar->GetBeamMomentum()+0.511*0.511); 
    P4Beam.SetPxPyPzE(BeamMom[0],BeamMom[1],BeamMom[2],BeamMom[3]);// in MeV
    
//    for (int i=0; i<4; i++){ 
//      P4_Miss[i]=TargetEleMom[i]+BeamMom[i]-GMom[i];
//    }
    P4Miss=P4em+P4Beam-P4Gamma;
    //    double MM2_CL= P4_Miss[3]*P4_Miss[3]-P4_Miss[2]*P4_Miss[2]-P4_Miss[1]*P4_Miss[1]-P4_Miss[0]*P4_Miss[0];
    //    MM2.push_back(MM2_CL);
    //    G4cout<<"MM2 "<<P4Miss.M2()<<" "<<MM2_CL<<G4endl;;
    MM2.push_back(P4Miss.M2());
    G4double product=0.;
    G4double BDire[3];

    BDire[0]=BDir.x();
    BDire[1]=BDir.y();
    BDire[2]=BDir.z();

    for (int i=0; i<3; i++)  product+= GDir[i]*BDire[i];
    ThCl.push_back(acos(product));//* 180.0 / 3.14159265;
    //    printf("Th Cl %f Th Gamma %f\n",ThCl.at(NClusters),P4Gamma.Theta());

    G4double ProcID = myStepping->GetPhysProc();    
    printf("PROCID %f\n",ProcID);
    if(ProcID==1) fHistoManager->FillHisto2(8,EneCl.at(NClusters),ThCl.at(NClusters),1.); //Nclus==1
    if(ProcID==2) fHistoManager->FillHisto2(9,EneCl.at(NClusters),ThCl.at(NClusters),1.); //Nclus==2
    if (NClusters==0 && EneCl.at(NClusters)>5.) {
      fHistoManager->FillHisto(7,EneCl.at(NClusters));
      fHistoManager->FillHisto(8,ThCl.at(NClusters));
      fHistoManager->FillHisto(9,MM2.at(NClusters));
      fHistoManager->FillHisto(15,QCl.at(NClusters));
    }
    NClusters++;
    //    G4cout<<ECL<<" Cluster find CALO "<<EneCl.at(0)<<" NCL "<<NClusters<<" "<<EneCl.size()<<G4endl;
    
    if(NClusters==2 && fabs(TimeCl.at(0)-TimeCl.at(1)<2.) ){ 
      G4double mgg= GGMass();
      fHistoManager->FillHisto(16,mgg);
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
  for(Int_t ii=0;ii<32;ii++) ETotHEPVeto[ii]=0;							
  HEPVetoEtrack.clear();
  HEPVetoTrackCh.clear();
  HEPVetoTrackTime.clear();
  HEPVetoX.clear();
  HEPVetoY.clear();
  
  // Zero cluster variables
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
    }
  }//end of first loop on hits
  
  for (G4int h=0; h<nHits; h++) {
    HEPVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      if(hit->GetTrackID()!=0 && ETotHEPVeto[hit->GetHEPVetoNb()]>0.1*MeV){
	HEPVetoTrackCh.push_back(hit->GetHEPVetoNb());
	HEPVetoEtrack.push_back(hit->GetTrackEnergy());
	HEPVetoTrackTime.push_back(hit->GetTime());
	HEPVetoX.push_back(hit->GetX());
	HEPVetoY.push_back(hit->GetY());
	NHEPVetoTracks++;
	//G4cout<<"trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" Strip Numb "<<hit->GetEVetoNb()<<G4endl;
      }
      //      if(NHEPVetoTracks>MaxTracks) break; 
    }
  }//end of loop
}

void EventAction::AddPVetoHits(PVetoHitsCollection* hcont){
  G4double ETotPVetoEvt=0;  
  PVetoTrackCh.clear();
  PVetoTrackTime.clear();
  PVetoX.clear();
  PVetoY.clear();
  PVetoEtrack.clear();
  //zero clusters variables
  for(int iBar=0;iBar<NPVetoBars;iBar++){
    TimePVeto[iBar]=0.0;
    ETotPVeto[iBar]=0.0;
    PVetoClIndex[iBar] = 0;
    for(int iCluster = 0; iCluster < PVetoClIndex[iBar] ; iCluster++) {
      PVetoTimeCl[iBar][iCluster] = 0.;
      PVetoECl[iBar][iCluster]  = 0.;
    }
  }

  G4int nHits = hcont->entries();
  for (G4int h=0; h<nHits; h++){	
    PVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore
    if( hit != 0 ) {
      int iBar = hit->GetChannelId();
      if (iBar > NPVetoBars ) {
	std::cerr << "[EventAction] [PVeto]  Bar index outside range" << std::endl;
	exit(0);
      }
      int newhit = 1;
      // each bar can have signal for different clusters checks needed!!!!
      //      G4cout<<"T "<<hit->GetTime()<<" indexbar "<<PVetoClIndex[iBar]<<G4endl;
      for(int iCluster = 0; iCluster < PVetoClIndex[iBar] ; iCluster++) {
	//	G4cout<<hit->GetTime()<<" PVTC "<<PVetoTimeCl[iBar][iCluster]<<G4endl;
	// if hit exists accumulate energy and time
	if( fabs( hit->GetTime() - PVetoTimeCl[iBar][iCluster] ) < 0.15) {
	  newhit=0;
	  PVetoTimeCl[iBar][iCluster] = 
	    (hit->GetTime()*hit->GetHitE() + PVetoECl[iBar][iCluster]*PVetoTimeCl[iBar][iCluster])/
	    ( hit->GetHitE() + PVetoECl[iBar][iCluster]  ) ;
	  PVetoECl[iBar][iCluster] +=  hit->GetHitE();
	}
	if(newhit==0) break; //next hit
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
    }
  }

  // M. Raggi 30/05/2018 select only tracks when they enter the volume and deposit >0.1 MeV energy in the finger
  // M. Raggi 28/06/2018 moved to vector output
  for (G4int h=0; h<nHits; h++) {
    PVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      if(hit->GetTrackID()!=0 && ETotPVeto[hit->GetPVetoNb()] > 0.1*MeV) {
	PVetoTrackCh.push_back(hit->GetPVetoNb());   //bugs gives crazy numbers
	PVetoEtrack.push_back(hit->GetTrackEnergy());
	PVetoTrackTime.push_back(hit->GetTime());
	PVetoX.push_back(hit->GetX());
	PVetoY.push_back(hit->GetY());
	NPVetoTracks++;
	//G4cout<<"trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" Strip Numb "<<hit->GetEVetoNb()<<G4endl;
      }
    }
  }
}

void EventAction::AddEVetoHits(EVetoHitsCollection* hcont){
  for(G4int jj=0;jj<MaxTracks;jj++)  ETotEVeto[jj]=0.0;						  
  
  EVetoEtrack.clear();
  EVetoTrackCh.clear();
  EVetoTrackTime.clear();
  EVetoX.clear();
  EVetoY.clear();
  
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
    }
  }
  //M. Raggi 30/05/2018 select only tracks when they enter the volume and deposit >0.1 MeV energy in the finger
  for (G4int h=0; h<nHits; h++) {
    EVetoHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      if(hit->GetTrackID()!=0 && ETotEVeto[hit->GetEVetoNb()] > 0.1*MeV) {
	EVetoTrackCh.push_back(hit->GetEVetoNb());   //bugs gives crazy numbers
	EVetoEtrack.push_back(hit->GetTrackEnergy());
	EVetoTrackTime.push_back(hit->GetTime());
	EVetoX.push_back(hit->GetX());
	EVetoY.push_back(hit->GetY());
	NEVetoTracks++;
	//	G4cout<<"cazzo "<<NEVetoTracks<<G4endl;
	//G4cout<<"trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" Strip Numb "<<hit->GetEVetoNb()<<G4endl;
      }
      if(NEVetoTracks>MaxTracks) break; 
    }
  }
}

void EventAction::AddSACHits(SACHitsCollection* hcont)
{
  std::vector<double> ECluster;
  const int NMaxCl=100;
  const int NRows=Sgeom->GetSACNRows();
  const int NCols=Sgeom->GetSACNCols();
  const int NTotCh=NRows*NCols;
  G4double TTotSACCh[NTotCh][NMaxCl];
  G4double ETotSACCh[NTotCh][NMaxCl];

  std::vector<G4double> ETotSAC;
  std::vector<G4double> TTotSAC;
  std::vector<G4double> QTotSAC;
  std::vector<G4double> XTotSAC;
  std::vector<G4double> YTotSAC;
  std::vector<G4double> ClTime;

  G4int NCry=0;
  G4int nHits = hcont->entries();
  if (nHits == 0) return;
  //  G4cout<<"Entering sac hits analysis "<<nHits<<G4endl;
  G4int NClus=1;

  for(Int_t ll=0;ll<NMaxCl;ll++){ 
    for(Int_t kk=0;kk<NTotCh;kk++){ 
      ETotSACCh[kk][ll]=0.;
      TTotSACCh[kk][ll]=0.;
      //      XSACCh[kk][ll]=0.;
      //      YSACCh[kk][ll]=0.;
    }
  }
  ClTime.clear();
  SACTracks = 0;
  SACCh.clear();
  SACEtrack.clear();  
  SACTrackTime.clear();  
  SACPType.clear();  
  SACX.clear();
  SACY.clear();

  //  G4cout<<"SAC Begin of event "<<NClus<<G4endl;
  //Summing hits energy in each single crystal
  for (G4int h=0; h<nHits; h++) {
    G4int Used=0;
    SACHit* hit = (*hcont)[h]; //prende l'elemento h del vettore hit
    if ( hit != 0 ) {
      G4int ChPos = hit->GetChannelId();
      G4int NCh   = ChPos/10*NRows+ChPos%NCols;
      if(h==0) ClTime.push_back(hit->GetTime());    //prova ad usare il Tmin del cluster
      // loop to create clusters 
      for(int iCl=0; iCl<NClus; iCl++){
	if(iCl>NMaxCl) break;
	//	G4cout<<h<<" SAC Ic Begin of event "<<ClTime[iCl]<<" CH "<<NCh<<" icl "<<iCl<<" Nclus "<<NClus<<G4endl;
	if(fabs(hit->GetTime()-ClTime[iCl])<1) {     //selects in time cluter and sum energies
	  ETotSACCh[NCh][iCl] += hit->GetEnergy();                    //sum single crystals energies
	  if(hit->GetTime()<ClTime[iCl]) ClTime[iCl]=hit->GetTime();  //Get the first time!
	  Used=1;
	  break;  //hit assigned exit cluster loop
	}
      }//end loop on cluster

      if(Used==0){
	//	G4cout<< "MA ce passi ?"<<G4endl;
	//	G4cout<<h<<" SAC New cluster "<<ClTime[NClus]<<" hitTime "<<hit->GetTime()<<G4endl;
	//after checking for all existing clusters create a new one
	ETotSACCh[NCh][NClus] = hit->GetEnergy(); //sum single crystals energies
	TTotSACCh[NCh][NClus] = hit->GetTime(); //Get the first time!
	ClTime.push_back(hit->GetTime());
	//intialize vector location for the new cluster
	ETotSAC.push_back(0.);
	TTotSAC.push_back(0.);
	QTotSAC.push_back(0.);
	XTotSAC.push_back(0.);
	YTotSAC.push_back(0.);
	NClus++;
      }
    }
  } //end on loop on hits.
  G4cout<<"End of event "<<NClus<<G4endl;

  // compute cluster variables (non credo che la somma funzioni!!)
  for(int cc=0;cc<NClus;cc++){
    for(int ii=0;ii<NTotCh;ii++){
      if(ETotSACCh[ii][cc]>0.2){
	XTotSAC[cc]+=Sgeom->GetCrystalPosX(ii/NRows,ii%NCols)*ETotSACCh[ii][cc];
	YTotSAC[cc]+=Sgeom->GetCrystalPosY(ii/NRows,ii%NCols)*ETotSACCh[ii][cc];
	ETotSAC[cc]+=ETotSACCh[ii][cc];
	QTotSAC[cc]+=GetSACCharge(ETotSACCh[ii][cc]);
	//	TTotSAC[cc]=ClTime[cc];  // find the right 
	TTotSAC[cc]+=TTotSACCh[ii][cc]*ETotSACCh[ii][cc];  // find the right 
	NCry++;
      }
    }
    //push back variables to TTree
    SACEtrack.push_back(ETotSAC[cc]);
    SACTrackTime.push_back(TTotSAC[cc]/ETotSAC[cc]);
    //    SACTrackTime.push_back(TTotSAC[cc]);
    SACX.push_back(XTotSAC[cc]/ETotSAC[cc]);
    SACY.push_back(YTotSAC[cc]/ETotSAC[cc]);
    SACQ.push_back(QTotSAC[cc]); 
    SACTracks++;
    G4cout<<"********end of loop hits***************"<<G4endl;
    G4cout<<"QSAC "<<QTotSAC[cc]<<ETotSAC[cc]<<G4endl;
    G4cout<<"********end of loop hits***************"<<G4endl;
  }
}

void EventAction::AddSACHitsStep(G4double E,G4double T, G4int Ptype, G4double X, G4double Y, G4int NCry)
{
  //  static G4int SACTracks  = 0;
  //  if(SACTracks < MaxTracks){
    SACEtrack.push_back(E);
    SACTrackTime.push_back(T);
    SACPType.push_back(Ptype);
    SACX.push_back(X);
    SACY.push_back(Y);
    SACCh.push_back(NCry);
    SACQ.push_back(GetSACCharge(E)); 
    SACTracks++;
    //    G4cout<<SACTracks<<" E SAC step "<< E <<" T "<< T <<"Ptype "<<Ptype<<" X "<<X<<" Y "<<Y<<" Ncry "<<NCry<<G4endl;
    //    G4cout<<"Step SAC "<<SACTracks<<" T "<<T<<" E "<<E<<" Type "<<Ptype<<G4endl;
    //  }
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
	//	G4cout<<"CC Nhits "<<nHits<<" trkID "<<hit->GetTrackID()<<" edep "<<hit->GetEdep()<<" time "<<hit->GetTime()<<G4endl;
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

G4double EventAction::GetSACCharge(G4double Energia)
{
  G4double LY      =     1.7;    //p.e per MeV (measured)
  G4double Gain    =     3.2E5;  //typical gain tapered PMT
  G4double Echarge = 1.6E-19;
  //  G4double CollEff =     0.1; //geometrical factor due to photodetector area
  //  G4double QE      =    0.25; //photodetector quantum efficiency
  G4double NPh = Energia*LY;
  G4double rnd = G4Poisson(NPh);
  G4double SACCharge = rnd*Gain*Echarge/1E-12;  //in pC
  return SACCharge;
}

G4double EventAction::GGMass() 
{
  double ECalPosiZ=3300.; //
  if(NClusters!=2)                        return -1;  // Need 2 clusters
  double XDiff2 = (XCl[0]-XCl[1])*(XCl[0]-XCl[1]);
  double YDiff2 = (YCl[0]-YCl[1])*(YCl[0]-YCl[1]);
  double Massa  = sqrt(EneCl[0]*EneCl[1]*(XDiff2+YDiff2))/(ECalPosiZ);
  G4cout<<XCl[0]<<" Massa "<<Massa<<G4endl;
  return Massa;
}
