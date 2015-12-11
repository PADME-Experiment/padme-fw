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
// $Id: EVetoHit.hh,v 1.2 2014/06/23 13:44:14 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef EVetoHit_h
#define EVetoHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EVetoHit : public G4VHit
{
  public:

      EVetoHit();
     ~EVetoHit();
      EVetoHit(const EVetoHit&);
      const EVetoHit& operator=(const EVetoHit&);
      G4int operator==(const EVetoHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID  (G4int track)      { VtrackID = track; };
      void SetEVetoNb  (G4int cry)        { EVetoNb = cry; };
      void SetEdep     (G4double de)      { edep = de; };
      void SetHitE     (G4double hite)    { eHit = hite; };
      void SetPos      (G4ThreeVector xyz){ pos = xyz; };
      void SetTime     (G4double time)    { VetoT = time; };
      
      G4int GetTrackID()    { return VtrackID; };
      G4int GetEVetoNb()    { return EVetoNb; };
      G4double GetEdep()    { return edep; };
      G4double GetHitE()    { return eHit; };
      G4double GetTime()    { return VetoT; };
      G4double GetX()       { return pos.x(); };
      G4double GetY()       { return pos.y(); };

      G4ThreeVector GetPos(){ return pos; };
      

  private:
  
      G4int         VtrackID;
      G4int         EVetoNb;
      G4double      VetoT;
      G4double      edep;
      G4double      eHit;
      G4ThreeVector pos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<EVetoHit> EVetoHitsCollection;

extern G4Allocator<EVetoHit> EVetoHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* EVetoHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) EVetoHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void EVetoHit::operator delete(void *aHit)
{
  EVetoHitAllocator.FreeSingle((EVetoHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
