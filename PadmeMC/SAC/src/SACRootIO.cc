// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#include "SACRootIO.hh"

#include <sstream>

#include "G4Event.hh"

#include "SACGeometry.hh"

#include "TSACMCEvent.hh"
#include "TSACMCHit.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "SACSD.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

SACRootIO::SACRootIO() : MCVRootIO(G4String("SAC"))
{

  fGeoPars = SACGeometry::GetInstance();

  // Create event object
  fEvent = new TSACMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;

  G4cout << "SACRootIO: Initialized" << G4endl;

}

SACRootIO::~SACRootIO()
{;}

void SACRootIO::Close()
{;}

//void SACRootIO::NewRun(G4int nRun, TFile* hfile)
void SACRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  //if (fVerbose)
    G4cout << "SACRootIO: Initializing I/O for run " << nRun << G4endl;

  fRunNumber = nRun;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* sacInfo = detInfo->AddSubDetectorInfo("SAC");
  sacInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    sacInfo->Print();

  // Create tree to hold SAC Hits this run
  fSACTree = new TTree("SAC","SAC Hits tree");
  fSACTree->SetAutoSave(10000);  // autosave when ~10 Kbyte written
  fSACTree->SetDirectory(hfile->GetDirectory("/"));

  // Create branch to hold SAC Hits
  fSACBranch = fSACTree->Branch("MCHits", &fEvent, fBufSize,5);
  fSACBranch->SetAutoDelete(kFALSE);

}

void SACRootIO::EndRun()
{
  if (fVerbose)
    G4cout << "SACRootIO: Executing End-of-Run procedure" << G4endl;
  // Dump tree for this run to file and erase it
  if(fSACTree){
    fSACTree->Write();
    if (fVerbose)
      fSACTree->Print();
    delete fSACTree;
  }
}

void SACRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "SACRootIO: Preparing event structure" << G4endl;

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
    if (HCname == "SACCollection"){
      if (fVerbose>=2)
	G4cout << "SACRootIO: Found hits collection " << HCname << G4endl;
      SACHitsCollection* SACC = (SACHitsCollection*)(LHC->GetHC(iHC));
      int n_hit=0;
      if(SACC) {
	n_hit = SACC->entries();
	if(n_hit>0){
	  for(G4int i=0;i<n_hit;i++) {
	    TSACMCHit* Hit = (TSACMCHit*)fEvent->AddHit();
	    Hit->SetChannelId((*SACC)[i]->GetChannelId()); 
	    Hit->SetPosition(TVector3((*SACC)[i]->GetX(),(*SACC)[i]->GetY(),(*SACC)[i]->GetZ()));
	    Hit->SetEnergy((*SACC)[i]->GetEnergy());
	    Hit->SetTime((*SACC)[i]->GetTime());
	  }
	}
      }
    }
  }

  fSACTree->Fill();
  TProcessID::SetObjectCount(savedObjNumber);

}
