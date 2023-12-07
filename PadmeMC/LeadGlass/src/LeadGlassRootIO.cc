// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------

#include "LeadGlassRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "LeadGlassGeometry.hh"
#include "LeadGlassHit.hh"
#include "LeadGlassDigi.hh"

#include "TLeadGlassMCEvent.hh"
#include "TLeadGlassMCHit.hh"
#include "TLeadGlassMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

LeadGlassRootIO::LeadGlassRootIO() : MCVRootIO(G4String("LeadGlass"))
{

  fGeoPars = LeadGlassGeometry::GetInstance();
  fVerbose = fGeoPars->GetVerboseLevel();

  // Create event object
  fEvent = new TLeadGlassMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = true;
  fDigisEnabled = true;

  if (fVerbose) G4cout << "LeadGlassRootIO: Initialized" << G4endl;

}

LeadGlassRootIO::~LeadGlassRootIO()
{;}

void LeadGlassRootIO::Close()
{;}

void LeadGlassRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  if (fVerbose) {
    G4cout << "LeadGlassRootIO: Initializing I/O for run " << fRunNumber;
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
  TSubDetectorInfo* etagInfo = detInfo->AddSubDetectorInfo("LeadGlass");
  etagInfo->SetGeometryParameters(geoParR);
  if (fVerbose) etagInfo->Print();

  // Create branch to hold LeadGlass Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fLeadGlassBranch = fEventTree->Branch("LeadGlass", fEvent->IsA()->GetName(), &fEvent);
  fLeadGlassBranch->SetAutoDelete(kFALSE);

}

void LeadGlassRootIO::EndRun()
{
  if (fVerbose) G4cout << "LeadGlassRootIO: Executing End-of-Run procedure" << G4endl;
}

void LeadGlassRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "LeadGlassRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "LeadGlassRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  if (fHitsEnabled) {

    // Get list of hit collections in this event
    G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
    G4int nHC = theHC->GetNumberOfCollections();

    for(G4int iHC=0; iHC<nHC; iHC++) {

      // Handle each collection type with the right method
      G4String HCname = theHC->GetHC(iHC)->GetName();
      if (HCname == "LeadGlassCollection"){
	if (fVerbose>=2) G4cout << "LeadGlassRootIO: Found hits collection " << HCname << G4endl;
	LeadGlassHitsCollection* leadglassHC = (LeadGlassHitsCollection*)(theHC->GetHC(iHC));
	if(leadglassHC) {
	  G4int n_hit = leadglassHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      TLeadGlassMCHit* hit = (TLeadGlassMCHit*)fEvent->AddHit();
	      hit->SetTime((*leadglassHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*leadglassHC)[i]->GetPosX(),
					(*leadglassHC)[i]->GetPosY(),
					(*leadglassHC)[i]->GetPosZ()));
	      hit->SetEnergy((*leadglassHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    if (fVerbose>=2)
	      G4cout << "LeadGlassRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
      //      G4cout<<""<< theDC->GetDC(iDC)->GetName()<<G4endl;
      if (DCname == "LeadGlassDigiCollection"){
	if (fVerbose>=2) G4cout << "LeadGlassRootIO: Found digi collection " << DCname << G4endl;
	LeadGlassDigiCollection* leadglassDC = (LeadGlassDigiCollection*)(theDC->GetDC(iDC));
	if(leadglassDC) {
	  G4int n_digi = leadglassDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      TLeadGlassMCDigi* digi = (TLeadGlassMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*leadglassDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*leadglassDC)[i]->GetEnergy());
	      digi->SetTime((*leadglassDC)[i]->GetTime());
	      e_tot += (*leadglassDC)[i]->GetEnergy();
	    }
	    if (fVerbose>=2)
	      G4cout << "LeadGlassRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
