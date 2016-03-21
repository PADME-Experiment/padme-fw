// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "ECalRootIO.hh"

#include <sstream>

#include "G4Event.hh"

#include "ECalGeometry.hh"

#include "TECalMCEvent.hh"
#include "TECalMCHit.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "ECalSD.hh"

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

//void ECalRootIO::NewRun(G4int nRun, TFile* hfile)
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
  TSubDetectorInfo* lavInfo = detInfo->AddSubDetectorInfo("ECal");
  lavInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    lavInfo->Print();

  // Create tree to hold ECal Hits this run
  fECalTree = new TTree("ECal","ECal Hits tree");
  fECalTree->SetAutoSave(10000);  // autosave when ~10 Kbyte written
  fECalTree->SetDirectory(hfile->GetDirectory("/"));

  // Create branch to hold ECal Hits
  fECalBranch = fECalTree->Branch("MCHits",&fEvent,fBufSize,5);
  fECalBranch->SetAutoDelete(kFALSE);

}

void ECalRootIO::EndRun()
{
  if (fVerbose)
    G4cout << "ECalRootIO: Executing End-of-Run procedure" << G4endl;
  // Dump tree for this run to file and erase it
  if(fECalTree){
    fECalTree->Write();
    if (fVerbose)
      fECalTree->Print();
    delete fECalTree;
  }
}

void ECalRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "ECalRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  // Get list of hit collections in this event
  G4HCofThisEvent* LHC = eventG4->GetHCofThisEvent();
  G4int nHC = LHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = LHC->GetHC(iHC)->GetName();
    if (HCname == "ECalCollection"){
      if (fVerbose>=2)
	G4cout << "ECalRootIO: Found hits collection " << HCname << G4endl;
      ECalHitsCollection* ECalC = (ECalHitsCollection*)(LHC->GetHC(iHC));
      int n_hit=0;
      if(ECalC) {
	n_hit = ECalC->entries();
	if(n_hit>0){
	  for(G4int i=0;i<n_hit;i++) {
	    TECalMCHit* Hit = (TECalMCHit*)fEvent->AddHit();
	    Hit->SetChannelId((*ECalC)[i]->GetCryNb()); 
	    Hit->SetPosition(TVector3((*ECalC)[i]->GetPos()[0],
				      (*ECalC)[i]->GetPos()[1],
				      (*ECalC)[i]->GetPos()[2])
			     );
	    Hit->SetEnergy((*ECalC)[i]->GetEdep());
	    Hit->SetTime((*ECalC)[i]->GetTime());
	  }
	}
      }
    }
  }

  fECalTree->Fill();
  TProcessID::SetObjectCount(savedObjNumber);

}
