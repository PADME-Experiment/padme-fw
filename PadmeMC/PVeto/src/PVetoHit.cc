// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#include "PVetoHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<PVetoHit> PVetoHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PVetoHit::PVetoHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PVetoHit::~PVetoHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PVetoHit::PVetoHit(const PVetoHit& right)
  : G4VHit()
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fPosition = right.fPosition;
  fLocalPosition = right.fLocalPosition;
  fTrackId = right.fTrackId;
  fTrackEnergy = right.fTrackEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const PVetoHit& PVetoHit::operator=(const PVetoHit& right)
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  fPosition = right.fPosition;
  fLocalPosition = right.fLocalPosition;
  fTrackId = right.fTrackId;
  fTrackEnergy = right.fTrackEnergy;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int PVetoHit::operator==(const PVetoHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PVetoHit::Draw()
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

void PVetoHit::Print()
{
  G4cout << "- channel: " << fChannelId
	 << " time: " << G4BestUnit(fTime,"Time")
         << " energy deposit: " << G4BestUnit(fEnergy,"Energy")
         << " global position: " << G4BestUnit(fPosition,"Length")
         << " local position: " << G4BestUnit(fLocalPosition,"Length")
         << " track id: " << fTrackId
         << " track energy: " << G4BestUnit(fTrackEnergy,"Energy")
	 << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

