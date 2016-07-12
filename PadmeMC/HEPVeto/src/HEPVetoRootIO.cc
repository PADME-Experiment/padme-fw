// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "HEPVetoRootIO.hh"

#include <sstream>

#include "G4Event.hh"

#include "RootIOManager.hh"
#include "HEPVetoGeometry.hh"
#include "HEPVetoSD.hh"

#include "THEPVetoMCEvent.hh"
#include "THEPVetoMCHit.hh"
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

  G4cout << "HEPVetoRootIO: Initialized" << G4endl;

}

HEPVetoRootIO::~HEPVetoRootIO()
{;}

void HEPVetoRootIO::Close()
{;}

//void HEPVetoRootIO::NewRun(G4int nRun, TFile* hfile)
void HEPVetoRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  //if (fVerbose)
    G4cout << "HEPVetoRootIO: Initializing I/O for run " << nRun << G4endl;

  fRunNumber = nRun;

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* hepvetoInfo = detInfo->AddSubDetectorInfo("HEPVeto");
  hepvetoInfo->SetGeometryParameters(geoParR);
  //if (fVerbose>=2)
    hepvetoInfo->Print();

  // Create branch to hold HEPVeto Hits this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fHEPVetoBranch = fEventTree->Branch("HEPVeto", fEvent->IsA()->GetName(), &fEvent);
  fHEPVetoBranch->SetAutoDelete(kFALSE);

}

void HEPVetoRootIO::EndRun()
{
  if (fVerbose)
    G4cout << "HEPVetoRootIO: Executing End-of-Run procedure" << G4endl;
}

void HEPVetoRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2)
    G4cout << "HEPVetoRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset event structure
  fEvent->Clear();
  //G4cout << "HEPVetoRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());

  // Get list of hit collections in this event
  G4HCofThisEvent* LHC = eventG4->GetHCofThisEvent();
  G4int nHC = LHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = LHC->GetHC(iHC)->GetName();
    if (HCname == "HEPVetoCollection"){
      if (fVerbose>=2)
	G4cout << "HEPVetoRootIO: Found hits collection " << HCname << G4endl;
      HEPVetoHitsCollection* HEPVetoC = (HEPVetoHitsCollection*)(LHC->GetHC(iHC));
      int n_hit=0;
      if(HEPVetoC) {
	n_hit = HEPVetoC->entries();
	if(n_hit>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_hit;i++) {
	    THEPVetoMCHit* Hit = (THEPVetoMCHit*)fEvent->AddHit();
	    Hit->SetChannelId((*HEPVetoC)[i]->GetChannelId()); 
	    //Hit->SetChannelId(0); 
	    Hit->SetPosition(TVector3((*HEPVetoC)[i]->GetPos()[0],
				      (*HEPVetoC)[i]->GetPos()[1],
				      (*HEPVetoC)[i]->GetPos()[2])
			     );
	    Hit->SetEnergy((*HEPVetoC)[i]->GetEdep());
	    e_tot += (*HEPVetoC)[i]->GetEdep()/MeV;
	    Hit->SetTime((*HEPVetoC)[i]->GetTime());
	  }
	  G4cout << "HEPVetoRootIO: " << n_hit << " hits with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }
  TProcessID::SetObjectCount(savedObjNumber);

}
