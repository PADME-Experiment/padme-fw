// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
// Modified by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-22
//   - added digis to output structure
//
// --------------------------------------------------------------

#include "TargetRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "TargetGeometry.hh"
#include "TargetHit.hh"
#include "TargetDigi.hh"

#include "TTargetMCEvent.hh"
#include "TTargetMCHit.hh"
#include "TTargetMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

TargetRootIO::TargetRootIO() : MCVRootIO(G4String("Target"))
{

  fGeoPars = TargetGeometry::GetInstance();

  // Create event object
  fEvent = new TTargetMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = false;
  fDigisEnabled = true;

  G4cout << "TargetRootIO: Initialized" << G4endl;

}

TargetRootIO::~TargetRootIO()
{;}

void TargetRootIO::Close()
{;}

void TargetRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  G4cout << "TargetRootIO: Initializing I/O for run " << fRunNumber;
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
  TSubDetectorInfo* targetInfo = detInfo->AddSubDetectorInfo("Target");
  targetInfo->SetGeometryParameters(geoParR);
  targetInfo->Print();

  // Create branch to hold Target Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fTargetBranch = fEventTree->Branch("Target", fEvent->IsA()->GetName(), &fEvent);
  fTargetBranch->SetAutoDelete(kFALSE);

}

void TargetRootIO::EndRun()
{
  G4cout << "TargetRootIO: Executing End-of-Run procedure" << G4endl;
}

void TargetRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "TargetRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "TargetRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  if (fHitsEnabled) {

    // Get list of hit collections in this event
    G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
    G4int nHC = theHC->GetNumberOfCollections();

    for(G4int iHC=0; iHC<nHC; iHC++) {

      // Handle each collection type with the right method
      G4String HCname = theHC->GetHC(iHC)->GetName();
      if (HCname == "TargetCollection"){
	if (fVerbose>=2)
	  G4cout << "TargetRootIO: Found hits collection " << HCname << G4endl;
	TargetHitsCollection* targetHC = (TargetHitsCollection*)(theHC->GetHC(iHC));
	if(targetHC) {
	  G4int n_hit = targetHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      TTargetMCHit* hit = (TTargetMCHit*)fEvent->AddHit();
	      hit->SetChannelId(0);
	      hit->SetTime((*targetHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*targetHC)[i]->GetPosX(),
					(*targetHC)[i]->GetPosY(),
					(*targetHC)[i]->GetPosZ()));
	      hit->SetEnergy((*targetHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    //	    G4cout << "TargetRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
      if (DCname == "TargetDigiCollection"){
	if (fVerbose>=2)
	  G4cout << "TargetRootIO: Found digi collection " << DCname << G4endl;
	TargetDigiCollection* targetDC = (TargetDigiCollection*)(theDC->GetDC(iDC));
	if(targetDC) {
	  G4int n_digi = targetDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      TTargetMCDigi* digi = (TTargetMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*targetDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*targetDC)[i]->GetEnergy());
	      digi->SetTime((*targetDC)[i]->GetTime());
	      e_tot += (*targetDC)[i]->GetEnergy();
	    }
	    //	    G4cout << "TargetRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
