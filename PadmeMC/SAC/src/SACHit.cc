// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#include "SACHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<SACHit> SACHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SACHit::SACHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SACHit::~SACHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SACHit::SACHit(const SACHit& right)
  : G4VHit()
{
  fTrackId      = right.fTrackId;
  fParticleType = right.fParticleType;
  fChannelId    = right.fChannelId;
  fEnergy       = right.fEnergy;
  fTime         = right.fTime;
  fPosition     = right.fPosition;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const SACHit& SACHit::operator=(const SACHit& right)
{
  fTrackId      = right.fTrackId;
  fParticleType = right.fParticleType;
  fChannelId    = right.fChannelId;
  fEnergy       = right.fEnergy;
  fTime         = right.fTime;
  fPosition     = right.fPosition;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int SACHit::operator==(const SACHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SACHit::Draw()
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

void SACHit::Print()
{
  G4cout << "  trackID: " << fTrackId << "  channel: " << fChannelId
         << "  energy deposit: " << G4BestUnit(fEnergy,"Energy")
         << "  position: " << G4BestUnit(fPosition,"Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

