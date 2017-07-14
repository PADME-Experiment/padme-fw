// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------

#include "TPixRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "TPixGeometry.hh"
#include "TPixHit.hh"
#include "TPixDigi.hh"

#include "TTPixMCEvent.hh"
#include "TTPixMCHit.hh"
#include "TTPixMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

TPixRootIO::TPixRootIO() : MCVRootIO(G4String("TPix"))
{

  fGeoPars = TPixGeometry::GetInstance();

  // Create event object
  fEvent = new TTPixMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = false;
  fDigisEnabled = true;

  G4cout << "TPixRootIO: Initialized" << G4endl;

}

TPixRootIO::~TPixRootIO()
{;}

void TPixRootIO::Close()
{;}

void TPixRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  G4cout << "TPixRootIO: Initializing I/O for run " << fRunNumber;
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
  TSubDetectorInfo* tpixInfo = detInfo->AddSubDetectorInfo("TPix");
  tpixInfo->SetGeometryParameters(geoParR);
  tpixInfo->Print();

  // Create branch to hold TPix Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fTPixBranch = fEventTree->Branch("TPix", fEvent->IsA()->GetName(), &fEvent);
  fTPixBranch->SetAutoDelete(kFALSE);

}

void TPixRootIO::EndRun()
{
  G4cout << "TPixRootIO: Executing End-of-Run procedure" << G4endl;
}

void TPixRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "TPixRootIO: Preparing event structure" << G4endl;

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
      if (HCname == "TPixCollection"){
	if (fVerbose>=2)
	  G4cout << "TPixRootIO: Found hits collection " << HCname << G4endl;
	TPixHitsCollection* tPixHC = (TPixHitsCollection*)(theHC->GetHC(iHC));
	if(tPixHC) {
	  G4int n_hit = tPixHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      TTPixMCHit* hit = (TTPixMCHit*)fEvent->AddHit();
	      hit->SetChannelId((*tPixHC)[i]->GetChannelId()); 
	      hit->SetTime((*tPixHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*tPixHC)[i]->GetPosX(),
					(*tPixHC)[i]->GetPosY(),
					(*tPixHC)[i]->GetPosZ()));
	      hit->SetEnergy((*tPixHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    G4cout << "TPixRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
      if (DCname == "TPixDigiCollection"){
	if (fVerbose>=2)
	  G4cout << "TPixRootIO: Found digi collection " << DCname << G4endl;
	TPixDigiCollection* tPixDC = (TPixDigiCollection*)(theDC->GetDC(iDC));
	if(tPixDC) {
	  G4int n_digi = tPixDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      TTPixMCDigi* digi = (TTPixMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*tPixDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*tPixDC)[i]->GetEnergy());
	      digi->SetTime((*tPixDC)[i]->GetTime());
	      e_tot += (*tPixDC)[i]->GetEnergy();
	    }
	    //	    G4cout << "TPixRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
