// --------------------------------------------------------------
// History:
//
// Created by  Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#include "TMMRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "TMMGeometry.hh"
#include "TMMHit.hh"
#include "TMMDigi.hh"

#include "TTMMMCEvent.hh" 
#include "TTMMMCHit.hh"  
#include "TTMMMCDigi.hh" 
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

TMMRootIO::TMMRootIO() : MCVRootIO(G4String("TMM"))
{

  fGeoPars = TMMGeometry::GetInstance();
  fVerbose = fGeoPars->GetVerboseLevel();

  // Create event object
  fEvent = new TTMMMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = false;
  fDigisEnabled = true;

  if (fVerbose) G4cout << "TMMRootIO: Initialized" << G4endl;

}

TMMRootIO::~TMMRootIO()
{;}

void TMMRootIO::Close()
{;}

void TMMRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  if (fVerbose) {
    G4cout << "TMMRootIO: Initializing I/O for run " << fRunNumber;
    if (fHitsEnabled)  G4cout << " - save hits";
    if (fDigisEnabled) G4cout << " - save digis";
    G4cout << G4endl;
  }

  // Fill detector info section of run structure
  std::vector<TString> geoParR;
  std::vector<G4String> geoParG = fGeoPars->GetHashTable();
  for(unsigned int i=0; i<geoParG.size(); i++) {
    TString par = geoParG[i].data();
    geoParR.push_back(par);
  }
  TSubDetectorInfo* TMMInfo = detInfo->AddSubDetectorInfo("TMM");
  TMMInfo->SetGeometryParameters(geoParR);
  if (fVerbose) TMMInfo->Print();

  // Create branch to hold TMM Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fTMMBranch = fEventTree->Branch("TMM", fEvent->IsA()->GetName(), &fEvent);
  fTMMBranch->SetAutoDelete(kFALSE);

}

void TMMRootIO::EndRun()
{
  if (fVerbose) G4cout << "TMMRootIO: Executing End-of-Run procedure" << G4endl;
}

void TMMRootIO::SaveEvent(const G4Event* eventG4)
{

    if (fVerbose>=2) G4cout << "TMMRootIO: Preparing event structure" << G4endl;

    //Save current Object count
    Int_t savedObjNumber = TProcessID::GetObjectCount();

    // Reset event structure
    fEvent->Clear();
    //G4cout << "TMMRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
    fEvent->SetRunNumber(fRunNumber);
    fEvent->SetEventNumber(eventG4->GetEventID());

    if (fHitsEnabled) { //maybe i do not need to save hits for TMM

        // Get list of hit collections in this event
        G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
        G4int nHC = theHC->GetNumberOfCollections();

        for(G4int iHC=0; iHC<nHC; iHC++) {

            // Handle each collection type with the right method
            G4String HCname = theHC->GetHC(iHC)->GetName();
            if (HCname == "TMMCollection"){
                if (fVerbose>=2) G4cout << "TMMRootIO: Found hits collection " << HCname << G4endl;
                TMMHitsCollection* TMMHC = (TMMHitsCollection*)(theHC->GetHC(iHC));
                if(TMMHC) {
                    G4int n_hit = TMMHC->entries();
                    if(n_hit>0){
                        G4double e_tot = 0.;
                        for(G4int i=0;i<n_hit;i++) {
                            TTMMMCHit* hit = (TTMMMCHit*)fEvent->AddHit();  
                            hit->SetChannelId(0); //in TMM hits do not contain ID info!
                            hit->SetTime((*TMMHC)[i]->GetTime());
                            hit->SetPosition(TVector3((*TMMHC)[i]->GetPosX(),
                                                        (*TMMHC)[i]->GetPosY(),
                                                        (*TMMHC)[i]->GetPosZ()));
                            hit->SetEnergy((*TMMHC)[i]->GetEnergy());
                            // e_tot += hit->GetEnergy();
                        }
                        if (fVerbose>=2)
                            G4cout << "TMMRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
            if (DCname == "TMMDigiCollection"){ //change to TMM
                if (fVerbose>=2) G4cout << "TMMRootIO: Found digi collection " << DCname << G4endl;
                TMMDigiCollection* TMMDC = (TMMDigiCollection*)(theDC->GetDC(iDC));
                if(TMMDC) {
                    G4int n_digi = TMMDC->entries();
                    if(n_digi>0){
                        // G4double e_tot = 0.;
                        for(G4int i=0;i<n_digi;i++) {
                            TTMMMCDigi* digi = (TTMMMCDigi*)fEvent->AddDigi();
                            digi->SetChannelId((*TMMDC)[i]->GetID()); 
                            //digi->SetNHitxCh((*TMMDC)[i]->GetNHitxCh());
                            // DQ: for now i save earliest time and total charge
                            // NB: WE FILL THE ENERGY FIELD WITH CHARGE!
                            digi->SetEnergy((*TMMDC)[i]->GetCharge()); 
                            digi->SetTime((*TMMDC)[i]->GetTime());
                            
                            // e_tot += (*TMMDC)[i]->GetCharge();
                        }
                        // if (fVerbose>=2)
                        // G4cout << "TMMRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
                    }
                }
            }

        }
    }

    TProcessID::SetObjectCount(savedObjNumber);

}
