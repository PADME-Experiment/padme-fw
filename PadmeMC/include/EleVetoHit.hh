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
// $Id: EleVetoHit.hh,v 1.2 2014/06/23 13:44:14 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EleVetoHit_h
#define EleVetoHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EleVetoHit : public G4VHit
{
  public:

      EleVetoHit();
     ~EleVetoHit();
      EleVetoHit(const EleVetoHit&);
      const EleVetoHit& operator=(const EleVetoHit&);
      G4int operator==(const EleVetoHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID  (G4int track)      { VtrackID = track; };
      void SetEleVetoNb  (G4int cry)        { EleVetoNb = cry; };
      void SetEdep     (G4double de)      { edep = de; };
      void SetHitE     (G4double hite)    { eHit = hite; };
      void SetPos      (G4ThreeVector xyz){ pos = xyz; };
      void SetTime     (G4double time)    { VetoT = time; };
      
      G4int GetTrackID()    { return VtrackID; };
      G4int GetEleVetoNb()    { return EleVetoNb; };
      G4double GetEdep()    { return edep; };
      G4double GetHitE()    { return eHit; };
      G4double GetTime()    { return VetoT; };
      G4double GetX()       { return pos.x(); };
      G4double GetY()       { return pos.y(); };

      G4ThreeVector GetPos(){ return pos; };
      

  private:
  
      G4int         VtrackID;
      G4int         EleVetoNb;
      G4double      VetoT;
      G4double      edep;
      G4double      eHit;
      G4ThreeVector pos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<EleVetoHit> EleVetoHitsCollection;

extern G4Allocator<EleVetoHit> EleVetoHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* EleVetoHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) EleVetoHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void EleVetoHit::operator delete(void *aHit)
{
  EleVetoHitAllocator.FreeSingle((EleVetoHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
