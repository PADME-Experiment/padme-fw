// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#include "TMMDigi.hh"
#include "TMMGeometry.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<TMMDigi> TMMDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TMMDigi::TMMDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TMMDigi::~TMMDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TMMDigi::TMMDigi(const TMMDigi& right)
  : G4VDigi()
{
  fTime = right.fTime;
  fCharge = right.fCharge;
  fID = right.fID;
  //fNHits = right.fNHits;
  // fCenter = right.fCenter;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const TMMDigi& TMMDigi::operator=(const TMMDigi& right)
{
  fTime = right.fTime;
  fCharge = right.fCharge;
  fID = right.fID;
  // fNHits = right.fNHits;
  // fCenter = right.fCenter;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int TMMDigi::operator==(const TMMDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TMMDigi::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  // TMMGeometry* geo = TMMGeometry::GetInstance();
  if(pVVisManager)
  {
    // G4Circle circle(G4ThreeVector());
    // circle.SetScreenSize(2.);
    // circle.SetFillStyle(G4Circle::filled);
    // G4Colour colour(0.,1.,0.);
    // G4VisAttributes attribs(colour);
    // circle.SetVisAttributes(attribs);
    // pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TMMDigi::Print()
{
  G4cout << "###### TMM Digi ######" << G4endl;
  G4cout << "ID:" << fID << G4endl;
  G4cout << "first hit time : " << G4BestUnit(fTime, "Time") << G4endl;
  G4cout << "total charge   :" << G4BestUnit(fCharge, "Electric charge") << G4endl;
  // G4cout << "Center:" << fCenter << G4endl;
  // G4cout << "NHits: " << fNHits << G4endl;
  G4cout << "########################" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
