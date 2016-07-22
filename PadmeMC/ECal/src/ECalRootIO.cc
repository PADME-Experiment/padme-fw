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

  G4cout << "ECalRootIO: Initialized" << G4endl;

}

ECalRootIO::~ECalRootIO()
{;}

void ECalRootIO::Close()
{;}

void ECalRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  //if (fVerbose)
    G4cout << "ECalRootIO: Initializing I/O for run " << nRun << G4endl;

  fRunNumber = nRun;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* ecalInfo = detInfo->AddSubDetectorInfo("ECal");
  ecalInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    ecalInfo->Print();

  // Create tree to hold ECal Hits this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fECalBranch = fEventTree->Branch("ECal", fEvent->IsA()->GetName(), &fEvent);
  fECalBranch->SetAutoDelete(kFALSE);

}

void ECalRootIO::EndRun()
{
  //if (fVerbose)
    G4cout << "ECalRootIO: Executing End-of-Run procedure" << G4endl;
}

void ECalRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "ECalRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "ECalRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

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
	    hit->SetPosition(TVector3((*eCalHC)[i]->GetPos()[0],
				      (*eCalHC)[i]->GetPos()[1],
				      (*eCalHC)[i]->GetPos()[2])
			     );
	    hit->SetEnergy((*eCalHC)[i]->GetEnergy());
	    hit->SetTime((*eCalHC)[i]->GetTime());
	    e_tot += (*eCalHC)[i]->GetEnergy()/MeV;
	  }
	  G4cout << "ECalRootIO: " << n_hit << " hits with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }

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
	    e_tot += (*eCalDC)[i]->GetEnergy()/MeV;
	  }
	  G4cout << "ECalRootIO: " << n_digi << " digi with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }

  TProcessID::SetObjectCount(savedObjNumber);

}
