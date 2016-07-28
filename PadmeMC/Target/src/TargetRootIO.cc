// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "TargetRootIO.hh"

#include <sstream>

#include "G4Event.hh"

#include "RootIOManager.hh"
#include "TargetGeometry.hh"
#include "TargetSD.hh"

#include "TTargetMCEvent.hh"
#include "TTargetMCHit.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

TargetRootIO::TargetRootIO() : MCVRootIO(G4String("Target"))
{

  fGeoPars = TargetGeometry::GetInstance();

  // Create event object
  fEvent = new TTargetMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;

  G4cout << "TargetRootIO: Initialized" << G4endl;

}

TargetRootIO::~TargetRootIO()
{;}

void TargetRootIO::Close()
{;}

//void TargetRootIO::NewRun(G4int nRun, TFile* hfile)
void TargetRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  //if (fVerbose)
    G4cout << "TargetRootIO: Initializing I/O for run " << nRun << G4endl;

  fRunNumber = nRun;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* targetInfo = detInfo->AddSubDetectorInfo("Target");
  targetInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    targetInfo->Print();

  // Create branch to hold Target Hits this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fTargetBranch = fEventTree->Branch("Target", fEvent->IsA()->GetName(), &fEvent);
  fTargetBranch->SetAutoDelete(kFALSE);

}

void TargetRootIO::EndRun()
{
  if (fVerbose)
    G4cout << "TargetRootIO: Executing End-of-Run procedure" << G4endl;
}

void TargetRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "TargetRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "TargetRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  // Get list of hit collections in this event
  G4HCofThisEvent* LHC = eventG4->GetHCofThisEvent();
  G4int nHC = LHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = LHC->GetHC(iHC)->GetName();
    if (HCname == "TargetCollection"){
      if (fVerbose>=2)
	G4cout << "TargetRootIO: Found hits collection " << HCname << G4endl;
      TargetHitsCollection* TargetC = (TargetHitsCollection*)(LHC->GetHC(iHC));
      int n_hit=0;
      if(TargetC) {
	n_hit = TargetC->entries();
	if(n_hit>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_hit;i++) {
	    TTargetMCHit* Hit = (TTargetMCHit*)fEvent->AddHit();
	    //Hit->SetChannelId((*TargetC)[i]->GetCryNb()); 
	    Hit->SetChannelId(0); 
	    Hit->SetPosition(TVector3((*TargetC)[i]->GetPos()[0],
				      (*TargetC)[i]->GetPos()[1],
				      (*TargetC)[i]->GetPos()[2])
			     );
	    Hit->SetEnergy((*TargetC)[i]->GetEdep());
	    e_tot += (*TargetC)[i]->GetEdep()/MeV;
	    Hit->SetTime((*TargetC)[i]->GetTime());
	  }
	  G4cout << "TargetRootIO: " << n_hit << " hits with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }
  TProcessID::SetObjectCount(savedObjNumber);

}
