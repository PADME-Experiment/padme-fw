// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "EVetoRootIO.hh"

#include <sstream>

#include "G4Event.hh"

#include "RootIOManager.hh"
#include "EVetoGeometry.hh"
#include "EVetoHit.hh"
#include "EVetoDigi.hh"

#include "TEVetoMCEvent.hh"
#include "TEVetoMCHit.hh"
#include "TEVetoMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

EVetoRootIO::EVetoRootIO() : MCVRootIO(G4String("EVeto"))
{

  fGeoPars = EVetoGeometry::GetInstance();

  // Create event object
  fEvent = new TEVetoMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;

  G4cout << "EVetoRootIO: Initialized" << G4endl;

}

EVetoRootIO::~EVetoRootIO()
{;}

void EVetoRootIO::Close()
{;}

//void EVetoRootIO::NewRun(G4int nRun, TFile* hfile)
void EVetoRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  //if (fVerbose)
    G4cout << "EVetoRootIO: Initializing I/O for run " << nRun << G4endl;

  fRunNumber = nRun;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* evetoInfo = detInfo->AddSubDetectorInfo("EVeto");
  evetoInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    evetoInfo->Print();

  // Create branch to hold EVeto Hits this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fEVetoBranch = fEventTree->Branch("EVeto", fEvent->IsA()->GetName(), &fEvent);
  fEVetoBranch->SetAutoDelete(kFALSE);

}

void EVetoRootIO::EndRun()
{
  if (fVerbose)
    G4cout << "EVetoRootIO: Executing End-of-Run procedure" << G4endl;
}

void EVetoRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "EVetoRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "EVetoRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  // Get list of hit collections in this event
  G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
  G4int nHC = theHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = theHC->GetHC(iHC)->GetName();
    if (HCname == "EVetoCollection"){
      if (fVerbose>=2)
	G4cout << "EVetoRootIO: Found hits collection " << HCname << G4endl;
      EVetoHitsCollection* eVetoHC = (EVetoHitsCollection*)(theHC->GetHC(iHC));
      if(eVetoHC) {
	G4int n_hit = eVetoHC->entries();
	if(n_hit>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_hit;i++) {
	    TEVetoMCHit* hit = (TEVetoMCHit*)fEvent->AddHit();
	    hit->SetChannelId((*eVetoHC)[i]->GetChannelId()); 
	    hit->SetPosition(TVector3((*eVetoHC)[i]->GetPos()[0],
				      (*eVetoHC)[i]->GetPos()[1],
				      (*eVetoHC)[i]->GetPos()[2])
			     );
	    hit->SetEnergy((*eVetoHC)[i]->GetEnergy());
	    hit->SetTime((*eVetoHC)[i]->GetTime());
	    e_tot += (*eVetoHC)[i]->GetEnergy()/MeV;
	  }
	  G4cout << "EVetoRootIO: " << n_hit << " hits with " << e_tot << " MeV total energy" << G4endl;
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
    if (DCname == "EVetoDigiCollection"){
      if (fVerbose>=2)
	G4cout << "EVetoRootIO: Found digi collection " << DCname << G4endl;
      EVetoDigiCollection* eVetoDC = (EVetoDigiCollection*)(theDC->GetDC(iDC));
      if(eVetoDC) {
	G4int n_digi = eVetoDC->entries();
	if(n_digi>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_digi;i++) {
	    TEVetoMCDigi* digi = (TEVetoMCDigi*)fEvent->AddDigi();
	    digi->SetChannelId((*eVetoDC)[i]->GetChannelId()); 
	    digi->SetEnergy((*eVetoDC)[i]->GetEnergy());
	    digi->SetTime((*eVetoDC)[i]->GetTime());
	    e_tot += (*eVetoDC)[i]->GetEnergy()/MeV;
	  }
	  G4cout << "EVetoRootIO: " << n_digi << " digi with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }

  TProcessID::SetObjectCount(savedObjNumber);
}
