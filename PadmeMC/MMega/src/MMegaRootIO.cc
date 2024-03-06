// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#include "MMegaRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "MMegaGeometry.hh"
#include "MMegaHit.hh"
#include "MMegaDigi.hh"

#include "TETagMCEvent.hh" //change to MMega
#include "TETagMCHit.hh"  //same
#include "TETagMCDigi.hh" //same
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

MMegaRootIO::MMegaRootIO() : MCVRootIO(G4String("MMega"))
{

  fGeoPars = MMegaGeometry::GetInstance();
  fVerbose = fGeoPars->GetVerboseLevel();

  // Create event object
  fEvent = new TETagMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = true;
  fDigisEnabled = true;

  if (fVerbose) G4cout << "MMegaRootIO: Initialized" << G4endl;

}

MMegaRootIO::~MMegaRootIO()
{;}

void MMegaRootIO::Close()
{;}

void MMegaRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  if (fVerbose) {
    G4cout << "MMegaRootIO: Initializing I/O for run " << fRunNumber;
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
  TSubDetectorInfo* mMegaInfo = detInfo->AddSubDetectorInfo("MMega");
  mMegaInfo->SetGeometryParameters(geoParR);
  if (fVerbose) mMegaInfo->Print();

  // Create branch to hold MMega Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fMMegaBranch = fEventTree->Branch("MMega", fEvent->IsA()->GetName(), &fEvent);
  fMMegaBranch->SetAutoDelete(kFALSE);

}

void MMegaRootIO::EndRun()
{
  if (fVerbose) G4cout << "MMegaRootIO: Executing End-of-Run procedure" << G4endl;
}

void MMegaRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "MMegaRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "MMegaRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  if (fHitsEnabled) {

    // Get list of hit collections in this event
    G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
    G4int nHC = theHC->GetNumberOfCollections();

    for(G4int iHC=0; iHC<nHC; iHC++) {

      // Handle each collection type with the right method
      G4String HCname = theHC->GetHC(iHC)->GetName();
      if (HCname == "MMegaCollection"){
	if (fVerbose>=2) G4cout << "MMegaRootIO: Found hits collection " << HCname << G4endl;
	MMegaHitsCollection* MMegaHC = (MMegaHitsCollection*)(theHC->GetHC(iHC));
	if(MMegaHC) {
	  G4int n_hit = MMegaHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      TETagMCHit* hit = (TETagMCHit*)fEvent->AddHit();  //change to MMega
	      hit->SetChannelId((*MMegaHC)[i]->GetChannelId()); 
	      hit->SetTime((*MMegaHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*MMegaHC)[i]->GetPosX(),
					(*MMegaHC)[i]->GetPosY(),
					(*MMegaHC)[i]->GetPosZ()));
	      hit->SetEnergy((*MMegaHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    if (fVerbose>=2)
	      G4cout << "MMegaRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
      if (DCname == "MMegaDigiCollection"){ //change to MMega
	if (fVerbose>=2) G4cout << "MMegaRootIO: Found digi collection " << DCname << G4endl;
	MMegaDigiCollection* mMegaDC = (MMegaDigiCollection*)(theDC->GetDC(iDC));
	if(mMegaDC) {
	  G4int n_digi = mMegaDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      TETagMCDigi* digi = (TETagMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*mMegaDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*mMegaDC)[i]->GetEnergy());
	      digi->SetTime((*mMegaDC)[i]->GetTime());
	      e_tot += (*mMegaDC)[i]->GetEnergy();
	    }
	    if (fVerbose>=2)
	      G4cout << "MMegaRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
