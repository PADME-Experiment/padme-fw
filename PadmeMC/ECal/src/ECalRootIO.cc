// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-02-02
//
// --------------------------------------------------------------

#include "ECalRootIO.hh"

#include <sstream>

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4Trajectory.hh"
#include "G4TwoVector.hh"

#include "ECalGeometry.hh"
//#include "ECalMaterialParameters.hh"

//#include "Event.hh"
//#include "MCTruthManager.hh"
//#include "SubDetectorInfo.hh"

#include "TECalMCEvent.hh"
#include "TECalMCHit.hh"

#include "ECalSD.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

ECalRootIO::ECalRootIO() : MCVRootIO(G4String("ECal"))
{

  fGeoPars = ECalGeometry::GetInstance();
  //fMatPars = ECalMaterialParameters::GetInstance();

  // Create run and event objects
  fEvent = new TECalMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  G4cout << "ECalRootIO: Initialized" << G4endl;

}

ECalRootIO::~ECalRootIO()
{;}

void ECalRootIO::Close()
{
}

//void ECalRootIO::NewRun(G4int nRun, TFile* hfile, DetectorInfo* detInfo)
void ECalRootIO::NewRun(G4int nRun, TFile* hfile)
{

  if (fVerbose) G4cout << "ECalRootIO: Initializing I/O for run " << nRun << G4endl;

  // Fill detector info section of run structure
  //SubDetectorInfo* lavInfo = detInfo->AddSubDetectorInfo("ECal");
  //lavInfo->SetGeometryParameters(fGeoPars->GetHashTable());
  //lavInfo->SetMaterialParameters(fMatPars->GetHashTable());

  //if (fVerbose>=2) lavInfo->Print();

  // Create tree to hold ECal Hits this run
  fECalTree = new TTree("ECal","ECal Hits tree");
  fECalTree->SetAutoSave(10000);  // autosave when ~10 Kbyte written
  fECalTree->SetDirectory(hfile->GetDirectory("/"));

  // Create branch to hold ECal Hits
  fECalBranch = fECalTree->Branch("Hits", &fEvent, fBufSize,5);
  fECalBranch->SetAutoDelete(kFALSE);

}

void ECalRootIO::EndRun()
{
  if (fVerbose) G4cout << "ECalRootIO: Executing End-of-Run procedure" << G4endl;
  // Dump tree for this run to file and erase it
  if(fECalTree){
    fECalTree->Write();
    if (fVerbose) fECalTree->Print();
    delete fECalTree;
  }
}

void ECalRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "ECalRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();
  G4int nEvent = eventG4->GetEventID();
  fEvent->Clear();
  //fEvent->SetRunID(nRun);
  fEvent->SetEventID(nEvent);

  // Get list of hit collections in this event
  G4HCofThisEvent* LHC = eventG4->GetHCofThisEvent();
  G4int nHC = LHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = LHC->GetHC(iHC)->GetName();
    if(fVerbose>=2) G4cout << "ECalRootIO: Found hits collection " << HCname << G4endl;
    if (HCname == "ECalCollection"){
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
