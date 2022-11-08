// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "ETagRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "ETagGeometry.hh"
#include "ETagHit.hh"
#include "ETagDigi.hh"

#include "TETagMCEvent.hh"
#include "TETagMCHit.hh"
#include "TETagMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

ETagRootIO::ETagRootIO() : MCVRootIO(G4String("ETag"))
{

  fGeoPars = ETagGeometry::GetInstance();
  fVerbose = fGeoPars->GetVerboseLevel();

  // Create event object
  fEvent = new TETagMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = true;
  fDigisEnabled = true;

  if (fVerbose) G4cout << "ETagRootIO: Initialized" << G4endl;

}

ETagRootIO::~ETagRootIO()
{;}

void ETagRootIO::Close()
{;}

void ETagRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  if (fVerbose) {
    G4cout << "ETagRootIO: Initializing I/O for run " << fRunNumber;
    if (fHitsEnabled)  G4cout << " - save hits";
    if (fDigisEnabled) G4cout << " - save digis";
    G4cout << G4endl;
  }

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* etagInfo = detInfo->AddSubDetectorInfo("ETag");
  etagInfo->SetGeometryParameters(geoParR);
  if (fVerbose) etagInfo->Print();

  // Create branch to hold ETag Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fETagBranch = fEventTree->Branch("ETag", fEvent->IsA()->GetName(), &fEvent);
  fETagBranch->SetAutoDelete(kFALSE);

}

void ETagRootIO::EndRun()
{
  if (fVerbose) G4cout << "ETagRootIO: Executing End-of-Run procedure" << G4endl;
}

void ETagRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "ETagRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "ETagRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  if (fHitsEnabled) {

    // Get list of hit collections in this event
    G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
    G4int nHC = theHC->GetNumberOfCollections();

    for(G4int iHC=0; iHC<nHC; iHC++) {

      // Handle each collection type with the right method
      G4String HCname = theHC->GetHC(iHC)->GetName();
      if (HCname == "ETagCollection"){
	if (fVerbose>=2) G4cout << "ETagRootIO: Found hits collection " << HCname << G4endl;
	ETagHitsCollection* etagHC = (ETagHitsCollection*)(theHC->GetHC(iHC));
	if(etagHC) {
	  G4int n_hit = etagHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      TETagMCHit* hit = (TETagMCHit*)fEvent->AddHit();
	      hit->SetChannelId((*etagHC)[i]->GetChannelId()); 
	      hit->SetTime((*etagHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*etagHC)[i]->GetPosX(),
					(*etagHC)[i]->GetPosY(),
					(*etagHC)[i]->GetPosZ()));
	      hit->SetEnergy((*etagHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    if (fVerbose>=2)
	      G4cout << "ETagRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
      if (DCname == "ETagDigiCollection"){
	if (fVerbose>=2) G4cout << "ETagRootIO: Found digi collection " << DCname << G4endl;
	ETagDigiCollection* etagDC = (ETagDigiCollection*)(theDC->GetDC(iDC));
	if(etagDC) {
	  G4int n_digi = etagDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      TETagMCDigi* digi = (TETagMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*etagDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*etagDC)[i]->GetEnergy());
	      digi->SetTime((*etagDC)[i]->GetTime());
	      e_tot += (*etagDC)[i]->GetEnergy();
	    }
	    if (fVerbose>=2)
	      G4cout << "ETagRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
