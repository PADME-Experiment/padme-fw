#include "MagnetHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<MagnetHit> MagnetHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MagnetHit::MagnetHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MagnetHit::~MagnetHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MagnetHit::MagnetHit(const MagnetHit& right)
  : G4VHit()
{
  fHitTrackId  = right.fHitTrackId;
  fHitTime     = right.fHitTime;
  fHitEnergy   = right.fHitEnergy;
  fHitPosition = right.fHitPosition;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const MagnetHit& MagnetHit::operator=(const MagnetHit& right)
{
  fHitTrackId  = right.fHitTrackId;
  fHitTime     = right.fHitTime;
  fHitEnergy   = right.fHitEnergy;
  fHitPosition = right.fHitPosition;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int MagnetHit::operator==(const MagnetHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MagnetHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(fHitPosition);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MagnetHit::Print()
{
  G4cout << "  Track id: " << fHitTrackId
         << "  Time: "     << G4BestUnit(fHitTime,"Time")
         << "  Energy: "   << G4BestUnit(fHitEnergy,"Energy")
         << "  Position: " << G4BestUnit(fHitPosition,"Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

