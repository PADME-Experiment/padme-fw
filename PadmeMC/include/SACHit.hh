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
// $Id: SACHit.hh,v 1.2 2014/06/23 13:44:14 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef SACHit_h
#define SACHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SACHit : public G4VHit
{
  public:

      SACHit();
     ~SACHit();
      SACHit(const SACHit&);
      const SACHit& operator=(const SACHit&);
      G4int operator==(const SACHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID  (G4int track) { VtrackID = track; };
      void SetSACNb(G4int cry)  { SACNb = cry; };
      void SetPType(G4int typ)       { PType = typ; };
      void SetEdep(G4double de)      { edep = de; };
      void SetPos(G4ThreeVector xyz) { pos = xyz; };
      void SetTime(G4double time)    { VetoT = time; };
      
      G4int GetTrackID()    { return VtrackID; };
      G4int GetSACNb() { return SACNb; };
      G4double GetEdep()    { return edep; };      
      G4double GetTime()    { return VetoT; };
      G4int GetPType()      { return PType; };
      G4ThreeVector GetPos(){ return pos; };
      G4double GetX(){ return pos.x(); };
      G4double GetY(){ return pos.y(); };
      G4double GetZ(){ return pos.z(); };

  private:
  
      G4int         VtrackID;
      G4int         PType;
      G4int         SACNb;
      G4double      VetoT;
      G4double      edep;
      G4ThreeVector pos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<SACHit> SACHitsCollection;

extern G4Allocator<SACHit> SACHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SACHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) SACHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void SACHit::operator delete(void *aHit)
{
  SACHitAllocator.FreeSingle((SACHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
