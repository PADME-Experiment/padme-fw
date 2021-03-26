// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2021-03-12
//
// --------------------------------------------------------------

#include "MCTruthRootIO.hh"

#include <sstream>

#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "RootIOManager.hh"

#include "TMCTruthEvent.hh"
#include "TMCVertex.hh"
#include "TMCParticle.hh"

#include "TDetectorInfo.hh"

#include "TString.h"
#include "TVector3.h"
#include "TProcessID.h"

#include "MCTruthManager.hh"
#include "MCTruthVertex.hh"
#include "MCTruthParticle.hh"

MCTruthRootIO::MCTruthRootIO() : MCVRootIO(G4String("MCTruth"))
{

  // Connect to MCTruth manager class
  fManager = MCTruthManager::GetInstance();

  // Create event object
  fEvent = new TMCTruthEvent();

  TTree::SetBranchStyle(fBranchStyle);

  fEnabled = true;

  fVerbose = 1;

  if (fVerbose) G4cout << "MCTruthRootIO: Initialized" << G4endl;

}

MCTruthRootIO::~MCTruthRootIO()
{;}

void MCTruthRootIO::Close()
{;}

void MCTruthRootIO::NewRun(G4int nRun, TFile*, TDetectorInfo*)
{

  fRunNumber = nRun;

  if (fVerbose) {
    G4cout << "MCTruthRootIO: Initializing I/O for run " << fRunNumber << G4endl;
  }

  // Create branch to hold MCTruth particles for this run
  fEventTree = RootIOManager::GetInstance()->GetEventTree();
  fMCTruthBranch = fEventTree->Branch("MCTruth", fEvent->IsA()->GetName(), &fEvent);
  fMCTruthBranch->SetAutoDelete(kFALSE);

}

void MCTruthRootIO::EndRun()
{
  if (fVerbose) G4cout << "MCTruthRootIO: Executing End-of-Run procedure" << G4endl;
}

void MCTruthRootIO::SaveEvent(const G4Event* eventG4)
{

  if (fVerbose>=2) G4cout << "MCTruthRootIO: Preparing event structure" << G4endl;

  //Save current Object count
  Int_t savedObjNumber = TProcessID::GetObjectCount();

  // Reset and fill MCTruth event structure
  fEvent->Clear();
  fEvent->SetRunNumber(fRunNumber);
  fEvent->SetEventNumber(eventG4->GetEventID());
  if (fManager->IsEnabled()) {
    fEvent->SetEventWeight(fManager->GetEventWeight());
    for(G4int v=0; v<fManager->GetNVertices(); v++) {
      MCTruthVertex* vtx = fManager->Vertex(v);
      TMCVertex* tvtx = fEvent->AddVertex();
      tvtx->SetProcess(vtx->GetProcess());
      tvtx->SetPosition(TVector3(vtx->GetPosX(),vtx->GetPosY(),vtx->GetPosZ()));
      tvtx->SetTime(vtx->GetTime());
      for(G4int p=0; p<vtx->GetNParticlesIn(); p++) {
	MCTruthParticle* part = vtx->ParticleIn(p);
	TMCParticle* tpart = tvtx->AddParticleIn();
	tpart->SetPDGCode(part->GetPDGCode());
	tpart->SetEnergy(part->GetEnergy());
	tpart->SetMomentum(TVector3(part->GetMomX(),part->GetMomY(),part->GetMomZ()));
      }
      for(G4int p=0; p<vtx->GetNParticlesOut(); p++) {
	MCTruthParticle* part = vtx->ParticleOut(p);
	TMCParticle* tpart = tvtx->AddParticleOut();
	tpart->SetPDGCode(part->GetPDGCode());
	tpart->SetEnergy(part->GetEnergy());
	tpart->SetMomentum(TVector3(part->GetMomX(),part->GetMomY(),part->GetMomZ()));
      }
    }
  }

  TProcessID::SetObjectCount(savedObjNumber);

}
