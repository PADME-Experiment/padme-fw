// --------------------------------------------------------------
// History:
//
// Created by  Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#include "TMMHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<TMMHit> TMMHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TMMHit::TMMHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TMMHit::~TMMHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TMMHit::TMMHit(const TMMHit& right)
  : G4VHit()
{
  fTrackType = right.fTrackType;
  fTrackId = right.fTrackId;
  fParentId = right.fParentId;
  fVertexPos = right.fVertexPos;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fETrack = right.fETrack;
  fPosition = right.fPosition;
  fLocalPositionStart = right.fLocalPositionStart;
  fLocalPositionEnd = right.fLocalPositionEnd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const TMMHit& TMMHit::operator=(const TMMHit& right)
{
  fTrackType = right.fTrackType;
  fTrackId = right.fTrackId;
  fParentId = right.fParentId;
  fVertexPos = right.fVertexPos;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fETrack = right.fETrack;
  fPosition = right.fPosition;
  fLocalPositionStart = right.fLocalPositionStart;
  fLocalPositionEnd = right.fLocalPositionEnd;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int TMMHit::operator==(const TMMHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TMMHit::Draw()
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

void TMMHit::Print()
{ 
  G4cout << "########### TMM Hit ################" << G4endl;
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
