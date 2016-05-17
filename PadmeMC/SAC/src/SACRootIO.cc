// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#include "SACRootIO.hh"

#include <sstream>

#include "G4Event.hh"

#include "RootIOManager.hh"
#include "SACGeometry.hh"
#include "SACSD.hh"

#include "TSACMCEvent.hh"
#include "TSACMCHit.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

SACRootIO::SACRootIO() : MCVRootIO(G4String("SAC"))
{

  fGeoPars = SACGeometry::GetInstance();

  // Create event object
  fEvent = new TSACMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;

  // The real values must be set when the run starts and the geometry is defined
  fEHistoNBins  = 0;
  fEHistoTStart = 0.*ns;
  fEHistoTStart = 0.*ns;
  fEHistoTEnd   = 0.*ns;

  fEHistoBeamBunchLengthT = 40.*ns;

  G4cout << "SACRootIO: Initialized" << G4endl;

}

SACRootIO::~SACRootIO()
{;}

void SACRootIO::Close()
{;}

//void SACRootIO::NewRun(G4int nRun, TFile* hfile)
void SACRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  //if (fVerbose)
    G4cout << "SACRootIO: Initializing I/O for run " << nRun << G4endl;

  fRunNumber = nRun;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* sacInfo = detInfo->AddSubDetectorInfo("SAC");
  sacInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    sacInfo->Print();

  // Create a branch to hold SAC Hits
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fSACBranch = fEventTree->Branch("SAC", fEvent->IsA()->GetName(), &fEvent);
  fSACBranch->SetAutoDelete(kFALSE);

  // Define parameters for the energy distribution histogram
  // Currently we assume a 40ns bunch spread and use 100ps bins
  //fEHistoNBins  = 400;
  fEHistoNBins  = TSACMCHIT_N_BINS;
  fEHistoTStart = ( SACGeometry::GetInstance()->GetSACFrontFacePosZ() - fEHistoBeamStartZ )/c_light;
  fEHistoTEnd   = fEHistoTStart+fEHistoBeamBunchLengthT;
  fEHistoTStep  = fEHistoBeamBunchLengthT/fEHistoNBins;
  G4cout << "SAC Energy histo: " << fEHistoNBins << " " << fEHistoTStart << " " << fEHistoTEnd << " " << fEHistoTStep << G4endl;

}

void SACRootIO::EndRun()
{
  if (fVerbose)
    G4cout << "SACRootIO: Executing End-of-Run procedure" << G4endl;

  // Dump last event to stdout (debug: remove for production)
  //for (Int_t iHit=0; iHit<fEvent->GetNHits(); iHit++) {
  //  TSACMCHit* Hit = (TSACMCHit*)fEvent->Hit(iHit);
  //  G4cout << " --- Hit " << iHit << G4endl;
  //  Hit->Print();
  //  //if (Hit->GetChannelId() == 33) Hit->GetEnergyHisto()->Print("all");
  //  if (Hit->GetChannelId() == 33) Hit->PrintTHisto();
  //}
}

void SACRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "SACRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());
  //G4cout << "SACRootIO: saving run/event " << fRunNumber << " " << eventG4->GetEventID() << G4endl;

  // Get list of hit collections in this event
  G4HCofThisEvent* LHC = eventG4->GetHCofThisEvent();
  G4int nHC = LHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = LHC->GetHC(iHC)->GetName();
    if (HCname == "SACCollection"){
      if (fVerbose>=2)
	G4cout << "SACRootIO: Found hits collection " << HCname << G4endl;
      SACHitsCollection* SACC = (SACHitsCollection*)(LHC->GetHC(iHC));
      int n_hit=0;
      if(SACC) {
	n_hit = SACC->entries();
	if(n_hit>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_hit;i++) {
	    // Check if an hit for this channel already exists
	    Int_t chid = (*SACC)[i]->GetChannelId();
	    //G4cout << "SAC - Found hit for ch " << chid << G4endl;
	    //G4cout << "    Energy " << (*SACC)[i]->GetEnergy() << G4endl;
	    //G4cout << "    Time " << (*SACC)[i]->GetTime() << G4endl;
	    TSACMCHit* Hit = fEvent->GetOrCreateHit(chid);
	    //if (!Hit->GetEnergyHisto()) Hit->CreateEnergyHisto(fEHistoNBins,fEHistoTStart,fEHistoTEnd);
	    //G4cout << "    Histo name " << Hit->GetEnergyHisto()->GetName() << G4endl;
	    // If this is a new hit, initialize energy histogram
	    //if (strcmp(Hit->GetEnergyHisto()->GetName(),"") == 0)
	    //  Hit->CreateEnergyHisto(fEHistoNBins,fEHistoTStart,fEHistoTEnd);
	    // This should be done only once per hit
	    Hit->SetTHistoStart(fEHistoTStart);
	    Hit->SetTHistoStep(fEHistoTStep);

	    Hit->AddEnergy((*SACC)[i]->GetEnergy());
	    e_tot += (*SACC)[i]->GetEnergy()/MeV;
	    Hit->AddEnergyAtTime((*SACC)[i]->GetEnergy(),(*SACC)[i]->GetTime());
	    // Hit time = earliest time
	    if ( (*SACC)[i]->GetTime() < Hit->GetTime() )
	      Hit->SetTime((*SACC)[i]->GetTime());

	    //G4cout << "    THit Energy " << Hit->GetEnergy() << G4endl;;
	    //G4cout << "    THit Time " << Hit->GetTime() << G4endl;;
	    /*
	    TSACMCHit* Hit = (TSACMCHit*)fEvent->AddHit();
	    Hit->SetChannelId((*SACC)[i]->GetChannelId()); 
	    Hit->SetPosition(TVector3((*SACC)[i]->GetX(),(*SACC)[i]->GetY(),(*SACC)[i]->GetZ()));
	    Hit->SetEnergy((*SACC)[i]->GetEnergy());
	    Hit->SetTime((*SACC)[i]->GetTime());
	    */
	  }
	  G4cout << "SACRootIO: " << n_hit << " hits with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }
  TProcessID::SetObjectCount(savedObjNumber);

}
