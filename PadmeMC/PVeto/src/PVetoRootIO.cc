// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "PVetoRootIO.hh"

#include <sstream>

#include "G4Event.hh"

#include "RootIOManager.hh"
#include "PVetoGeometry.hh"
//#include "PVetoSD.hh"
#include "PVetoHit.hh"
#include "PVetoDigi.hh"

#include "TPVetoMCEvent.hh"
#include "TPVetoMCHit.hh"
#include "TPVetoMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

PVetoRootIO::PVetoRootIO() : MCVRootIO(G4String("PVeto"))
{

  fGeoPars = PVetoGeometry::GetInstance();

  // Create event object
  fEvent = new TPVetoMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;

  G4cout << "PVetoRootIO: Initialized" << G4endl;

}

PVetoRootIO::~PVetoRootIO()
{;}

void PVetoRootIO::Close()
{;}

void PVetoRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  //if (fVerbose)
    G4cout << "PVetoRootIO: Initializing I/O for run " << nRun << G4endl;

  fRunNumber = nRun;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* pvetoInfo = detInfo->AddSubDetectorInfo("PVeto");
  pvetoInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    pvetoInfo->Print();

  // Create branch to hold PVeto Hits and Digi for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fPVetoBranch = fEventTree->Branch("PVeto", fEvent->IsA()->GetName(), &fEvent);
  fPVetoBranch->SetAutoDelete(kFALSE);

}

void PVetoRootIO::EndRun()
{
  //if (fVerbose)
    G4cout << "PVetoRootIO: Executing End-of-Run procedure" << G4endl;
}

void PVetoRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "PVetoRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "PVetoRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  // Get list of hit collections in this event
  G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
  G4int nHC = theHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = theHC->GetHC(iHC)->GetName();
    if (HCname == "PVetoCollection"){
      if (fVerbose>=2)
	G4cout << "PVetoRootIO: Found hits collection " << HCname << G4endl;
      PVetoHitsCollection* pVetoHC = (PVetoHitsCollection*)(theHC->GetHC(iHC));
      if(pVetoHC) {
	G4int n_hit = pVetoHC->entries();
	if(n_hit>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_hit;i++) {
	    TPVetoMCHit* hit = (TPVetoMCHit*)fEvent->AddHit();
	    hit->SetChannelId((*pVetoHC)[i]->GetChannelId()); 
	    hit->SetPosition(TVector3((*pVetoHC)[i]->GetPos()[0],
				      (*pVetoHC)[i]->GetPos()[1],
				      (*pVetoHC)[i]->GetPos()[2])
			     );
	    hit->SetEnergy((*pVetoHC)[i]->GetEnergy());
	    hit->SetTime((*pVetoHC)[i]->GetTime());
	    e_tot += (*pVetoHC)[i]->GetEnergy()/MeV;
	  }
	  G4cout << "PVetoRootIO: " << n_hit << " hits with " << e_tot << " MeV total energy" << G4endl;
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
    if (DCname == "PVetoDigiCollection"){
      if (fVerbose>=2)
	G4cout << "PVetoRootIO: Found digi collection " << DCname << G4endl;
      PVetoDigiCollection* pVetoDC = (PVetoDigiCollection*)(theDC->GetDC(iDC));
      if(pVetoDC) {
	G4int n_digi = pVetoDC->entries();
	if(n_digi>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_digi;i++) {
	    TPVetoMCDigi* digi = (TPVetoMCDigi*)fEvent->AddDigi();
	    digi->SetChannelId((*pVetoDC)[i]->GetChannelId()); 
	    digi->SetEnergy((*pVetoDC)[i]->GetEnergy());
	    digi->SetTime((*pVetoDC)[i]->GetTime());
	    e_tot += (*pVetoDC)[i]->GetEnergy()/MeV;
	  }
	  G4cout << "PVetoRootIO: " << n_digi << " digi with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }

  TProcessID::SetObjectCount(savedObjNumber);

}
