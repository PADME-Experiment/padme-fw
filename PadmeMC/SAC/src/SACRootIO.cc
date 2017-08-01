// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
// Modified by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-22
//   - added digis to output structure
//
// --------------------------------------------------------------

#include "SACRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "SACGeometry.hh"
#include "SACHit.hh"
#include "SACDigi.hh"

#include "TSACMCEvent.hh"
#include "TSACMCHit.hh"
#include "TSACMCDigi.hh"
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

using namespace CLHEP;

SACRootIO::SACRootIO() : MCVRootIO(G4String("SAC"))
{

  fGeoPars = SACGeometry::GetInstance();

  // Create event object
  fEvent = new TSACMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = false;
  fDigisEnabled = true;

  G4cout << "SACRootIO: Initialized" << G4endl;

}

SACRootIO::~SACRootIO()
{;}

void SACRootIO::Close()
{;}

//void SACRootIO::NewRun(G4int nRun, TFile* hfile)
void SACRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  G4cout << "SACRootIO: Initializing I/O for run " << fRunNumber;
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
  TSubDetectorInfo* sacInfo = detInfo->AddSubDetectorInfo("SAC");
  sacInfo->SetGeometryParameters(geoParR);
  sacInfo->Print();

  // Create branch to hold SAC Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fSACBranch = fEventTree->Branch("SAC", fEvent->IsA()->GetName(), &fEvent);
  fSACBranch->SetAutoDelete(kFALSE);

}

void SACRootIO::EndRun()
{
  G4cout << "SACRootIO: Executing End-of-Run procedure" << G4endl;

  // Dump last event to stdout (debug: remove for production)
  //for (Int_t iHit=0; iHit<fEvent->GetNHits(); iHit++) {
  //  TSACMCHit* Hit = (TSACMCHit*)fEvent->Hit(iHit);
  //  G4cout << " --- Hit " << iHit << G4endl;
  //  Hit->Print();
  //  //if (Hit->GetChannelId() == 33) Hit->GetEnergyHisto()->Print("all");
  //  if (Hit->GetChannelId() == 33) Hit->PrintTHisto();
  //}
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

  if (fHitsEnabled) {

    // Get list of hit collections in this event
    G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
    G4int nHC = theHC->GetNumberOfCollections();

    for(G4int iHC=0; iHC<nHC; iHC++) {

      // Handle each collection type with the right method
      G4String HCname = theHC->GetHC(iHC)->GetName();
      if (HCname == "SACCollection"){
	if (fVerbose>=2)
	  G4cout << "SACRootIO: Found hits collection " << HCname << G4endl;
	SACHitsCollection* sacHC = (SACHitsCollection*)(theHC->GetHC(iHC));
	int n_hit=0;
	if(sacHC) {
	  n_hit = sacHC->entries();
	  if(n_hit>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_hit;i++) {
	      TSACMCHit* hit = (TSACMCHit*)fEvent->AddHit();
	      hit->SetChannelId((*sacHC)[i]->GetChannelId()); 
	      hit->SetTime((*sacHC)[i]->GetTime());
	      hit->SetPosition(TVector3((*sacHC)[i]->GetPosX(),
					(*sacHC)[i]->GetPosY(),
					(*sacHC)[i]->GetPosZ()));
	      hit->SetEnergy((*sacHC)[i]->GetEnergy());
	      e_tot += hit->GetEnergy();
	    }
	    G4cout << "SACRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
      if (DCname == "SACDigiCollection"){
	if (fVerbose>=2)
	  G4cout << "SACRootIO: Found digi collection " << DCname << G4endl;
	SACDigiCollection* sacDC = (SACDigiCollection*)(theDC->GetDC(iDC));
	if(sacDC) {
	  G4int n_digi = sacDC->entries();
	  if(n_digi>0){
	    G4double e_tot = 0.;
	    for(G4int i=0;i<n_digi;i++) {
	      TSACMCDigi* digi = (TSACMCDigi*)fEvent->AddDigi();
	      digi->SetChannelId((*sacDC)[i]->GetChannelId()); 
	      digi->SetEnergy((*sacDC)[i]->GetEnergy());
	      digi->SetTime((*sacDC)[i]->GetTime());
	      digi->SetTHistoStart((*sacDC)[i]->GetQHistoStart());
	      digi->SetTHistoStep((*sacDC)[i]->GetQHistoStep());
	      for(G4int j=0; j<digi->GetTHistoNBins(); j++) digi->SetTHistoBin(j,(*sacDC)[i]->GetQHistoBin(j));
	      e_tot += digi->GetEnergy();
	    }
	    G4cout << "SACRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
	  }
	}
      }

    }

  }

  TProcessID::SetObjectCount(savedObjNumber);

}
