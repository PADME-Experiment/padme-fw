// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-08
//
// --------------------------------------------------------------

#include "MMDigi.hh"
#include "MMGeometry.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<MMDigi> MMDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMDigi::MMDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMDigi::~MMDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMDigi::MMDigi(const MMDigi& right)
  : G4VDigi()
{
  fTime = right.fTime;
  fCharge = right.fCharge;
  fID = right.fID;
  //fNHits = right.fNHits;
  // fCenter = right.fCenter;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const MMDigi& MMDigi::operator=(const MMDigi& right)
{
  fTime = right.fTime;
  fCharge = right.fCharge;
  fID = right.fID;
  // fNHits = right.fNHits;
  // fCenter = right.fCenter;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int MMDigi::operator==(const MMDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MMDigi::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  // MMGeometry* geo = MMGeometry::GetInstance();
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

void MMDigi::Print()
{
  G4cout << "###### MM Digi ######" << G4endl;
  G4cout << "ID:" << fID << G4endl;
  G4cout << "first hit time : " << G4BestUnit(fTime, "Time") << G4endl;
  G4cout << "total charge   :" << G4BestUnit(fCharge, "Electric charge") << G4endl;
  // G4cout << "Center:" << fCenter << G4endl;
  // G4cout << "NHits: " << fNHits << G4endl;
  G4cout << "########################" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
