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

  // The real values must be set when the run starts and the geometry is defined
  fEHistoNBins  = 0;
  fEHistoTStart = 0.*ns;
  fEHistoTStart = 0.*ns;
  fEHistoTEnd   = 0.*ns;

  fEHistoBeamBunchLengthT = 40.*ns;

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

  // Create a branch to hold SAC Hits
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fSACBranch = fEventTree->Branch("SAC", fEvent->IsA()->GetName(), &fEvent);
  fSACBranch->SetAutoDelete(kFALSE);

  // Define parameters for the energy distribution histogram
  // Currently we assume a 40ns bunch spread and use 100ps bins
  //fEHistoNBins  = 400;
  fEHistoNBins  = TSACMCHIT_N_BINS;
  fEHistoTStart = ( SACGeometry::GetInstance()->GetSACFrontFacePosZ() - fEHistoBeamStartZ )/c_light;
  fEHistoTEnd   = fEHistoTStart+fEHistoBeamBunchLengthT;
  fEHistoTStep  = fEHistoBeamBunchLengthT/fEHistoNBins;
  G4cout << "SAC Energy histo: " << fEHistoNBins << " " << fEHistoTStart << " " << fEHistoTEnd << " " << fEHistoTStep << G4endl;

}

void SACRootIO::EndRun()
{
  if (fVerbose)
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
	    // Check if an hit for this channel already exists
	    Int_t chid = (*sacHC)[i]->GetChannelId();
	    //G4cout << "SAC - Found hit for ch " << chid << G4endl;
	    //G4cout << "    Energy " << (*sacHC)[i]->GetEnergy() << G4endl;
	    //G4cout << "    Time " << (*sacHC)[i]->GetTime() << G4endl;
	    TSACMCHit* hit = fEvent->GetOrCreateHit(chid);
	    //if (!hit->GetEnergyHisto()) hit->CreateEnergyHisto(fEHistoNBins,fEHistoTStart,fEHistoTEnd);
	    //G4cout << "    Histo name " << hit->GetEnergyHisto()->GetName() << G4endl;
	    // If this is a new hit, initialize energy histogram
	    //if (strcmp(hit->GetEnergyHisto()->GetName(),"") == 0)
	    //  hit->CreateEnergyHisto(fEHistoNBins,fEHistoTStart,fEHistoTEnd);
	    // This should be done only once per hit
	    hit->SetTHistoStart(fEHistoTStart);
	    hit->SetTHistoStep(fEHistoTStep);

	    hit->AddEnergy((*sacHC)[i]->GetEnergy());
	    e_tot += (*sacHC)[i]->GetEnergy()/MeV;
	    hit->AddEnergyAtTime((*sacHC)[i]->GetEnergy(),(*sacHC)[i]->GetTime());
	    // Hit time = earliest time
	    if ( (*sacHC)[i]->GetTime() < hit->GetTime() )
	      hit->SetTime((*sacHC)[i]->GetTime());

	    //G4cout << "    THit Energy " << hit->GetEnergy() << G4endl;;
	    //G4cout << "    THit Time " << hit->GetTime() << G4endl;;
	    /*
	    TSACMCHit* hit = (TSACMCHit*)fEvent->AddHit();
	    hit->SetChannelId((*sacHC)[i]->GetChannelId()); 
	    hit->SetPosition(TVector3((*sacHC)[i]->GetX(),(*sacHC)[i]->GetY(),(*sacHC)[i]->GetZ()));
	    hit->SetEnergy((*sacHC)[i]->GetEnergy());
	    hit->SetTime((*sacHC)[i]->GetTime());
	    */
	  }
	  G4cout << "SACRootIO: " << n_hit << " hits with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }

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
	    e_tot += (*sacDC)[i]->GetEnergy()/MeV;
	  }
	  G4cout << "SACRootIO: " << n_digi << " digi with " << e_tot << " MeV total energy" << G4endl;
	}
      }
    }
  }

  TProcessID::SetObjectCount(savedObjNumber);

}
