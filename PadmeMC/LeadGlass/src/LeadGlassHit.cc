// LeadGlassHit.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
// --------------------------------------------------------------

#include "LeadGlassHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<LeadGlassHit> LeadGlassHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LeadGlassHit::LeadGlassHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LeadGlassHit::~LeadGlassHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LeadGlassHit::LeadGlassHit(const LeadGlassHit& right)
  : G4VHit()
{
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fPosition = right.fPosition;
  fLocalPosition = right.fLocalPosition;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const LeadGlassHit& LeadGlassHit::operator=(const LeadGlassHit& right)
{
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fPosition = right.fPosition;
  fLocalPosition = right.fLocalPosition;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int LeadGlassHit::operator==(const LeadGlassHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LeadGlassHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(fPosition);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LeadGlassHit::Print()
{
  G4cout << "- time: " << G4BestUnit(fTime,"Time")
         << " energy: " << G4BestUnit(fEnergy,"Energy")
         << " global position: " << G4BestUnit(fPosition,"Length")
         << " local position: " << G4BestUnit(fLocalPosition,"Length")
	 << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
