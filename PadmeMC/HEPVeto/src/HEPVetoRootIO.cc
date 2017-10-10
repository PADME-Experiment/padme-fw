// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "HEPVetoRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "HEPVetoGeometry.hh"
#include "HEPVetoHit.hh"
#include "HEPVetoDigi.hh"

#include "THEPVetoMCEvent.hh"
#include "THEPVetoMCHit.hh"
#include "THEPVetoMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

HEPVetoRootIO::HEPVetoRootIO() : MCVRootIO(G4String("HEPVeto"))
{

  fGeoPars = HEPVetoGeometry::GetInstance();

  // Create event object
  fEvent = new THEPVetoMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = true;
  fDigisEnabled = true;

  G4cout << "HEPVetoRootIO: Initialized" << G4endl;

}

HEPVetoRootIO::~HEPVetoRootIO()
{;}

void HEPVetoRootIO::Close()
{;}

//void HEPVetoRootIO::NewRun(G4int nRun, TFile* hfile)
void HEPVetoRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  G4cout << "HEPVetoRootIO: Initializing I/O for run " << fRunNumber;
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
  TSubDetectorInfo* hepvetoInfo = detInfo->AddSubDetectorInfo("HEPVeto");
  hepvetoInfo->SetGeometryParameters(geoParR);
  hepvetoInfo->Print();

  // Create branch to hold HEPVeto Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fHEPVetoBranch = fEventTree->Branch("HEPVeto", fEvent->IsA()->GetName(), &fEvent);
  fHEPVetoBranch->SetAutoDelete(kFALSE);

}

void HEPVetoRootIO::EndRun()
{
  G4cout << "HEPVetoRootIO: Executing End-of-Run procedure" << G4endl;
}

void HEPVetoRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "HEPVetoRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "HEPVetoRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  if (fHitsEnabled) {

    // Get list of hit collections in this event
    G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
    G4int nHC = theHC->GetNumberOfCollections();

    for(G4int iHC=0; iHC<nHC; iHC++) {

      // Handle each collection type with the right method
      G4String HCname = theHC->GetHC(iHC)->GetName();
      if (HCname == "HEPVetoCollection"){
	if (fVerbose>=2)
	  G4cout << "HEPVetoRootIO: Found hits collection " << HCname << G4endl;
	HEPVetoHitsCollection* hepVetoHC = (HEPVetoHitsCollection*)(theHC->GetHC(iHC));
	if(hepVetoHC) {
	  G4int n_hit = hepVetoHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      THEPVetoMCHit* hit = (THEPVetoMCHit*)fEvent->AddHit();
	      hit->SetChannelId((*hepVetoHC)[i]->GetChannelId());
	      hit->SetTime((*hepVetoHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*hepVetoHC)[i]->GetPosX(),
					(*hepVetoHC)[i]->GetPosY(),
					(*hepVetoHC)[i]->GetPosZ()));
	      hit->SetEnergy((*hepVetoHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    G4cout << "HEPVetoRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
      if (DCname == "HEPVetoDigiCollection"){
	if (fVerbose>=2)
	  G4cout << "HEPVetoRootIO: Found digi collection " << DCname << G4endl;
	HEPVetoDigiCollection* hepVetoDC = (HEPVetoDigiCollection*)(theDC->GetDC(iDC));
	if(hepVetoDC) {
	  G4int n_digi = hepVetoDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      THEPVetoMCDigi* digi = (THEPVetoMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*hepVetoDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*hepVetoDC)[i]->GetEnergy());
	      digi->SetTime((*hepVetoDC)[i]->GetTime());
	      e_tot += (*hepVetoDC)[i]->GetEnergy();
	    }
	    G4cout << "HEPVetoRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
