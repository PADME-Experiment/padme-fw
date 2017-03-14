// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-05-18
//
// --------------------------------------------------------------

#include "PVetoRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "PVetoGeometry.hh"
#include "PVetoSD.hh"

#include "TPVetoMCEvent.hh"
#include "TPVetoMCHit.hh"
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

//void PVetoRootIO::NewRun(G4int nRun, TFile* hfile)
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

  // Create branch to hold PVeto Hits this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fPVetoBranch = fEventTree->Branch("PVeto", fEvent->IsA()->GetName(), &fEvent);
  fPVetoBranch->SetAutoDelete(kFALSE);

}

void PVetoRootIO::EndRun()
{
  if (fVerbose)
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
  G4HCofThisEvent* LHC = eventG4->GetHCofThisEvent();
  G4int nHC = LHC->GetNumberOfCollections();

  for(G4int iHC=0; iHC<nHC; iHC++) {

    // Handle each collection type with the right method
    G4String HCname = LHC->GetHC(iHC)->GetName();
    if (HCname == "PVetoCollection"){
      if (fVerbose>=2)
	G4cout << "PVetoRootIO: Found hits collection " << HCname << G4endl;
      PVetoHitsCollection* PVetoC = (PVetoHitsCollection*)(LHC->GetHC(iHC));
      int n_hit=0;
      if(PVetoC) {
	n_hit = PVetoC->entries();
	if(n_hit>0){
	  G4double e_tot = 0.;
	  for(G4int i=0;i<n_hit;i++) {
	    TPVetoMCHit* Hit = (TPVetoMCHit*)fEvent->AddHit();
	    Hit->SetChannelId((*PVetoC)[i]->GetChannelId()); 
	    Hit->SetTime((*PVetoC)[i]->GetTime());
	    /* Old hits counted the total track energy and used global position
	    Hit->SetPosition(TVector3((*PVetoC)[i]->GetPos()[0],
				      (*PVetoC)[i]->GetPos()[1],
				      (*PVetoC)[i]->GetPos()[2])
			     );
	    Hit->SetEnergy((*PVetoC)[i]->GetEdep());
	    e_tot += (*PVetoC)[i]->GetEdep()/MeV;
	    */
	    Hit->SetPosition(TVector3((*PVetoC)[i]->GetPosX(),
				      (*PVetoC)[i]->GetPosY(),
				      (*PVetoC)[i]->GetPosZ()));
	    Hit->SetEnergy((*PVetoC)[i]->GetEnergy());
	    e_tot += Hit->GetEnergy();
	  }
	  G4cout << "PVetoRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	}
      }
    }
  }
  TProcessID::SetObjectCount(savedObjNumber);

}
