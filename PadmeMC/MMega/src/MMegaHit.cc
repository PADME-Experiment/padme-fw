// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-08
//
// --------------------------------------------------------------

#include "MMegaHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<MMegaHit> MMegaHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMegaHit::MMegaHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMegaHit::~MMegaHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMegaHit::MMegaHit(const MMegaHit& right)
  : G4VHit()
{
  fTrackType = right.fTrackType;
  fTrackId = right.fTrackId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fETrack = right.fETrack;
  fPosition = right.fPosition;
  fLocalPositionStart = right.fLocalPositionStart;
  fLocalPositionEnd = right.fLocalPositionEnd;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const MMegaHit& MMegaHit::operator=(const MMegaHit& right)
{
  fTrackType = right.fTrackType;
  fTrackId = right.fTrackId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fETrack = right.fETrack;
  fPosition = right.fPosition;
  fLocalPositionStart = right.fLocalPositionStart;
  fLocalPositionEnd = right.fLocalPositionEnd;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int MMegaHit::operator==(const MMegaHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MMegaHit::Draw()
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

void MMegaHit::Print()
{ 
  G4cout << "########### MMega Hit ################" << G4endl;
  G4cout << " time: " << G4BestUnit(fTime,"Time") << G4endl;
  G4cout << " track type: " << fTrackType << G4endl;
  G4cout << " track id: " << fTrackId << G4endl;
  G4cout << " energy lost: " << G4BestUnit(fEnergy,"Energy") << G4endl;
  G4cout << " track energy: "  << G4BestUnit(fETrack, "Energy") << G4endl;
  G4cout << " global position: " << G4BestUnit(fPosition,"Length") << G4endl;
  G4cout << " local position start: " << G4BestUnit(fLocalPositionStart,"Length") << G4endl;
  G4cout << " local position end: " << G4BestUnit(fLocalPositionEnd,"Length") << G4endl;
  G4cout << "######################################" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
