//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: EleVetoSD.cc,v 1.2 2014/06/23 13:44:16 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EleVetoSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EleVetoSD::EleVetoSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="EleVetoCollection"); //crea il collection name
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EleVetoSD::~EleVetoSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EleVetoSD::Initialize(G4HCofThisEvent* HCE)
{
  EleVetoCollection = new EleVetoHitsCollection
                          (SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,EleVetoCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EleVetoSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4ThreeVector posix = aStep->GetPostStepPoint()->GetPosition();
  //  printf("Hey I'm processing a Hit %f\n",edep);
  if(edep==0.) return false;

  edep  = aStep->GetTrack()->GetTotalEnergy();
  EleVetoHit* newHit = new EleVetoHit();
  newHit -> SetTrackID(aStep->GetTrack()->GetTrackID());
  //newHit -> SetTrackCh(aStep->GetTrack()->GetParticleDefinition()->GetPDGCharge());
  newHit -> SetEleVetoNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  newHit -> SetEdep(edep);
  newHit -> SetTime(aStep->GetTrack()->GetGlobalTime())	;
  newHit -> SetPos(aStep->GetPostStepPoint()->GetPosition());
  newHit -> SetHitE(aStep->GetTotalEnergyDeposit());
  EleVetoCollection ->insert( newHit );
  // Totedep=ToTEDep+edep;
  // printf("EDep %f %d\n",edep,aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  //  newHit->Print();
  //  newHit->Draw();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EleVetoSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
     G4int NbHits = EleVetoCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
            << " hits in the EleVeto : " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*EleVetoCollection)[i]->Print();
    } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

