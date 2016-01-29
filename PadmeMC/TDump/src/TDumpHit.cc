#include "TDumpHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<TDumpHit> TDumpHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TDumpHit::TDumpHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TDumpHit::~TDumpHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TDumpHit::TDumpHit(const TDumpHit& right)
  : G4VHit()
{
  trackID   = right.trackID;
  PType     = right.PType;
  edep      = right.edep;
  TDumpHitT = right.TDumpHitT;
  pos       = right.pos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const TDumpHit& TDumpHit::operator=(const TDumpHit& right)
{
  trackID   = right.trackID;
  PType     = right.PType;
  edep      = right.edep;
  TDumpHitT = right.TDumpHitT;
  pos       = right.pos;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int TDumpHit::operator==(const TDumpHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TDumpHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TDumpHit::Print()
{
  G4cout << "  trackID: " << trackID << "  PType: " << PType
         << "  energy deposit: " << G4BestUnit(edep,"Energy")
         << "  time: " << G4BestUnit(TDumpHitT,"Time")
         << "  position: " << G4BestUnit(pos,"Length") << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

