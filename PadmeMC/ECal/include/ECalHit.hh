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
// $Id: ECalHit.hh,v 1.2 2014/06/23 13:44:14 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef ECalHit_h
#define ECalHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class ECalHit : public G4VHit
{
  public:

      ECalHit();
     ~ECalHit();
      ECalHit(const ECalHit&);
      const ECalHit& operator=(const ECalHit&);
      G4int operator==(const ECalHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID(G4int track)   { trackID = track; };
      void SetCryNb(G4int cry)       { CryNb = cry; }; 
      void SetPType(G4int typ)       { PType = typ; }; 
      void SetEdep(G4double de)      { edep = de; };
      void SetTime(G4double HitT)    { ECalHitT = HitT; };
      void SetPos(G4ThreeVector xyz) { pos = xyz; };
      
      G4int GetTrackID()    { return trackID; };
      G4int GetCryNb()      { return CryNb; };
      G4double GetEdep()    { return edep; };
      G4double GetTime()    { return ECalHitT; };
      G4int GetPType()      { return PType; };
      G4ThreeVector GetPos(){ return pos; };
      
  private:
  
      G4int         trackID;
      G4int         PType;
      G4int         CryNb;
      G4double      edep;
      G4double      ECalHitT;
      G4ThreeVector pos;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<ECalHit> ECalHitsCollection;

extern G4Allocator<ECalHit> ECalHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* ECalHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) ECalHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void ECalHit::operator delete(void *aHit)
{
  ECalHitAllocator.FreeSingle((ECalHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
