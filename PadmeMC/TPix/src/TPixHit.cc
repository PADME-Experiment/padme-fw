#include "TPixHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<TPixHit> TPixHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TPixHit::TPixHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TPixHit::~TPixHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TPixHit::TPixHit(const TPixHit& right)
  : G4VHit()
{
  fChannelId = right.fChannelId;
  fEnergy    = right.fEnergy;
  fTime      = right.fTime;
  fPosition  = right.fPosition;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const TPixHit& TPixHit::operator=(const TPixHit& right)
{
  fChannelId = right.fChannelId;
  fEnergy    = right.fEnergy;
  fTime      = right.fTime;
  fPosition  = right.fPosition;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int TPixHit::operator==(const TPixHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TPixHit::Draw()
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

void TPixHit::Print()
{
  G4cout << "- TPixHit: ChId " << fChannelId
	 << " E "   << G4BestUnit(fEnergy,"Energy")
         << " T "   << G4BestUnit(fTime,"Time")
	 << " XYZ " << G4BestUnit(fPosition,"Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

