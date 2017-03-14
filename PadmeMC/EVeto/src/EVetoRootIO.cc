// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "EVetoRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "EVetoGeometry.hh"
#include "EVetoSD.hh"

#include "TEVetoMCEvent.hh"
#include "TEVetoMCHit.hh"
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
  G4HCofThisEvent* LHC = eventG4->GetHCofThisEvent();
  G4int nHC = LHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = LHC->GetHC(iHC)->GetName();
    if (HCname == "EVetoCollection"){
      if (fVerbose>=2)
	G4cout << "EVetoRootIO: Found hits collection " << HCname << G4endl;
      EVetoHitsCollection* EVetoC = (EVetoHitsCollection*)(LHC->GetHC(iHC));
      int n_hit=0;
      if(EVetoC) {
	n_hit = EVetoC->entries();
	if(n_hit>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_hit;i++) {
	    TEVetoMCHit* Hit = (TEVetoMCHit*)fEvent->AddHit();
	    Hit->SetChannelId((*EVetoC)[i]->GetChannelId()); 
	    Hit->SetTime((*EVetoC)[i]->GetTime());
	    /* Old hits counted the total track energy and used global position
	    Hit->SetPosition(TVector3((*EVetoC)[i]->GetPos()[0],
				      (*EVetoC)[i]->GetPos()[1],
				      (*EVetoC)[i]->GetPos()[2])
			     );
	    Hit->SetEnergy((*EVetoC)[i]->GetEdep());
	    e_tot += (*EVetoC)[i]->GetEdep()/MeV;
	    */
	    Hit->SetPosition(TVector3((*EVetoC)[i]->GetPosX(),
				      (*EVetoC)[i]->GetPosY(),
				      (*EVetoC)[i]->GetPosZ()));
	    Hit->SetEnergy((*EVetoC)[i]->GetEnergy());
	    e_tot += Hit->GetEnergy();
	  }
	  G4cout << "EVetoRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	}
      }
    }
  }
  TProcessID::SetObjectCount(savedObjNumber);

}
