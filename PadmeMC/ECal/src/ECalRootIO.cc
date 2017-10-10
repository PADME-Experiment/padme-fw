// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
// Modified by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//   - added digis to output structure
//
// --------------------------------------------------------------

#include "ECalRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "ECalGeometry.hh"
#include "ECalHit.hh"
#include "ECalDigi.hh"

#include "TECalMCEvent.hh"
#include "TECalMCHit.hh"
#include "TECalMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

ECalRootIO::ECalRootIO() : MCVRootIO(G4String("ECal"))
{

  fGeoPars = ECalGeometry::GetInstance();

  // Create event object
  fEvent = new TECalMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = false;
  fDigisEnabled = true;

  G4cout << "ECalRootIO: Initialized" << G4endl;

}

ECalRootIO::~ECalRootIO()
{;}

void ECalRootIO::Close()
{;}

void ECalRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  G4cout << "ECalRootIO: Initializing I/O for run " << fRunNumber;
  if (fHitsEnabled)  G4cout << " - save hits";
  if (fDigisEnabled) G4cout << " - save digis";
  G4cout << G4endl;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* ecalInfo = detInfo->AddSubDetectorInfo("ECal");
  ecalInfo->SetGeometryParameters(geoParR);
  ecalInfo->Print();

  // Create branch to hold ECal Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fECalBranch = fEventTree->Branch("ECal", fEvent->IsA()->GetName(), &fEvent);
  fECalBranch->SetAutoDelete(kFALSE);

}

void ECalRootIO::EndRun()
{
  G4cout << "ECalRootIO: Executing End-of-Run procedure" << G4endl;
}

void ECalRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "ECalRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  if (fHitsEnabled) {

    // Get list of hit collections in this event
    G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
    G4int nHC = theHC->GetNumberOfCollections();

    for(G4int iHC=0; iHC<nHC; iHC++) {

      // Handle each collection type with the right method
      G4String HCname = theHC->GetHC(iHC)->GetName();
      if (HCname == "ECalCollection"){
	if (fVerbose>=2)
	  G4cout << "ECalRootIO: Found hits collection " << HCname << G4endl;
	ECalHitsCollection* eCalHC = (ECalHitsCollection*)(theHC->GetHC(iHC));
	if(eCalHC) {
	  G4int n_hit = eCalHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      TECalMCHit* hit = (TECalMCHit*)fEvent->AddHit();
	      hit->SetChannelId((*eCalHC)[i]->GetChannelId()); 
	      hit->SetTime((*eCalHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*eCalHC)[i]->GetPosX(),
					(*eCalHC)[i]->GetPosY(),
					(*eCalHC)[i]->GetPosZ()));
	      hit->SetEnergy((*eCalHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    G4cout << "ECalRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  if (fDigisEnabled) {

    // Get list of digi collections in this event
    G4DCofThisEvent* theDC = eventG4->GetDCofThisEvent();
    G4int nDC = theDC->GetNumberOfCollections();

    for(G4int iDC=0; iDC<nDC; iDC++) {

      // Handle each collection type with the right method
      G4String DCname = theDC->GetDC(iDC)->GetName();
      if (DCname == "ECalDigiCollection"){
	if (fVerbose>=2)
	  G4cout << "ECalRootIO: Found digi collection " << DCname << G4endl;
	ECalDigiCollection* eCalDC = (ECalDigiCollection*)(theDC->GetDC(iDC));
	if(eCalDC) {
	  G4int n_digi = eCalDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      TECalMCDigi* digi = (TECalMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*eCalDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*eCalDC)[i]->GetEnergy());
	      digi->SetTime((*eCalDC)[i]->GetTime());
	      digi->SetSignal((*eCalDC)[i]->GetSignal());
	      e_tot += (*eCalDC)[i]->GetEnergy();
	    }
	    G4cout << "ECalRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
