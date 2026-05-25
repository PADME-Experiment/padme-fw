// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#include "MMRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"
#include "MMGeometry.hh"
#include "MMHit.hh"
#include "MMDigi.hh"

#include "TMMMCEvent.hh" 
#include "TMMMCHit.hh"  
#include "TMMMCDigi.hh" 
#include "TDetectorInfo.hh"
#include "TSubDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

MMRootIO::MMRootIO() : MCVRootIO(G4String("MM"))
{

  fGeoPars = MMGeometry::GetInstance();
  fVerbose = fGeoPars->GetVerboseLevel();

  // Create event object
  fEvent = new TMMMCEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;
  fHitsEnabled = false;
  fDigisEnabled = true;

  if (fVerbose) G4cout << "MMRootIO: Initialized" << G4endl;

}

MMRootIO::~MMRootIO()
{;}

void MMRootIO::Close()
{;}

void MMRootIO::NewRun(G4int nRun, TFile* hfile, TDetectorInfo* detInfo)
{

  fRunNumber = nRun;

  if (fVerbose) {
    G4cout << "MMRootIO: Initializing I/O for run " << fRunNumber;
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
  TSubDetectorInfo* MMInfo = detInfo->AddSubDetectorInfo("MM");
  MMInfo->SetGeometryParameters(geoParR);
  if (fVerbose) MMInfo->Print();

  // Create branch to hold MM Hits and Digis for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fMMBranch = fEventTree->Branch("MM", fEvent->IsA()->GetName(), &fEvent);
  fMMBranch->SetAutoDelete(kFALSE);

}

void MMRootIO::EndRun()
{
  if (fVerbose) G4cout << "MMRootIO: Executing End-of-Run procedure" << G4endl;
}

void MMRootIO::SaveEvent(const G4Event* eventG4)
{

    if (fVerbose>=2) G4cout << "MMRootIO: Preparing event structure" << G4endl;

    //Save current Object count
    Int_t savedObjNumber = TProcessID::GetObjectCount();

    // Reset event structure
    fEvent->Clear();
    //G4cout << "MMRootIO: setting run/event to " << fRunNumber << "/" << eventG4->GetEventID() << G4endl;
    fEvent->SetRunNumber(fRunNumber);
    fEvent->SetEventNumber(eventG4->GetEventID());

    if (fHitsEnabled) { //maybe i do not need to save hits for MM

        // Get list of hit collections in this event
        G4HCofThisEvent* theHC = eventG4->GetHCofThisEvent();
        G4int nHC = theHC->GetNumberOfCollections();

        for(G4int iHC=0; iHC<nHC; iHC++) {

            // Handle each collection type with the right method
            G4String HCname = theHC->GetHC(iHC)->GetName();
            if (HCname == "MMCollection"){
                if (fVerbose>=2) G4cout << "MMRootIO: Found hits collection " << HCname << G4endl;
                MMHitsCollection* MMHC = (MMHitsCollection*)(theHC->GetHC(iHC));
                if(MMHC) {
                    G4int n_hit = MMHC->entries();
                    if(n_hit>0){
                        G4double e_tot = 0.;
                        for(G4int i=0;i<n_hit;i++) {
                            TMMMCHit* hit = (TMMMCHit*)fEvent->AddHit();  
                            hit->SetChannelId(0); //in MM hits do not contain ID info!
                            hit->SetTime((*MMHC)[i]->GetTime());
                            hit->SetPosition(TVector3((*MMHC)[i]->GetPosX(),
                                                        (*MMHC)[i]->GetPosY(),
                                                        (*MMHC)[i]->GetPosZ()));
                            hit->SetEnergy((*MMHC)[i]->GetEnergy());
                            // e_tot += hit->GetEnergy();
                        }
                        if (fVerbose>=2)
                            G4cout << "MMRootIO: " << n_hit << " hits with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
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
            if (DCname == "MMDigiCollection"){ //change to MM
                if (fVerbose>=2) G4cout << "MMRootIO: Found digi collection " << DCname << G4endl;
                MMDigiCollection* MMDC = (MMDigiCollection*)(theDC->GetDC(iDC));
                if(MMDC) {
                    G4int n_digi = MMDC->entries();
                    if(n_digi>0){
                        // G4double e_tot = 0.;
                        for(G4int i=0;i<n_digi;i++) {
                            TMMMCDigi* digi = (TMMMCDigi*)fEvent->AddDigi();
                            digi->SetChannelId((*MMDC)[i]->GetID()); 
                            //digi->SetNHitxCh((*MMDC)[i]->GetNHitxCh());
                            // DQ: for now i save earliest time and total charge
                            // NB: WE FILL THE ENERGY FIELD WITH CHARGE!
                            digi->SetEnergy((*MMDC)[i]->GetCharge()); 
                            digi->SetTime((*MMDC)[i]->GetTime());
                            
                            // e_tot += (*MMDC)[i]->GetCharge();
                        }
                        // if (fVerbose>=2)
                        // G4cout << "MMRootIO: " << n_digi << " digi with " << G4BestUnit(e_tot,"Energy") << " total energy" << G4endl;
                    }
                }
            }

        }
    }

    TProcessID::SetObjectCount(savedObjNumber);

}
