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
// $Id: TargetHit.hh,v 1.1.1.1 2014/01/22 15:35:03 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef TargetHit_h
#define TargetHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TargetHit : public G4VHit
{
public:

  TargetHit();
  ~TargetHit();
  TargetHit(const TargetHit&);
  const TargetHit& operator=(const TargetHit&);
  G4int operator==(const TargetHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

public:
  
  void SetTrackID  (G4int track)      { trackID = track; };
  void SetEdep     (G4double de)      { edep = de; };
  void SetTime     (G4double time)    { TTime = time; };
  void SetPos      (G4ThreeVector xyz){ pos = xyz; };

  G4int GetTrackID()    { return trackID; };
  G4double GetEdep()    { return edep; };      
  G4double GetTime()    { return TTime; };
  G4double GetX()       { return pos.x(); };
  G4double GetY()       { return pos.y(); };
  G4double GetZ()       { return pos.z(); };
  G4ThreeVector GetPos(){ return pos; };

private:
  
  G4int         trackID;
  G4double      TTime;
  G4double      edep;
  G4ThreeVector pos;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<TargetHit> TargetHitsCollection;

extern G4Allocator<TargetHit> TargetHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* TargetHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) TargetHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void TargetHit::operator delete(void *aHit)
{
  TargetHitAllocator.FreeSingle((TargetHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
