// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-08
//
// --------------------------------------------------------------

#include "MMegaDigi.hh"
#include "MMegaGeometry.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<MMegaDigi> MMegaDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMegaDigi::MMegaDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMegaDigi::~MMegaDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MMegaDigi::MMegaDigi(const MMegaDigi& right)
  : G4VDigi()
{
  fTime = right.fTime;
  fCharge = right.fCharge;
  fID = right.fID;
  //fNHits = right.fNHits;
  // fCenter = right.fCenter;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const MMegaDigi& MMegaDigi::operator=(const MMegaDigi& right)
{
  fTime = right.fTime;
  fCharge = right.fCharge;
  fID = right.fID;
  // fNHits = right.fNHits;
  // fCenter = right.fCenter;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int MMegaDigi::operator==(const MMegaDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MMegaDigi::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  // MMegaGeometry* geo = MMegaGeometry::GetInstance();
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

void MMegaDigi::Print()
{
  G4cout << "###### MMega Digi ######" << G4endl;
  G4cout << "ID:" << fID << G4endl;
  G4cout << "first hit time : " << G4BestUnit(fTime, "Time") << G4endl;
  G4cout << "total charge   :" << G4BestUnit(fCharge, "Electric charge") << G4endl;
  // G4cout << "Center:" << fCenter << G4endl;
  // G4cout << "NHits: " << fNHits << G4endl;
  G4cout << "########################" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
