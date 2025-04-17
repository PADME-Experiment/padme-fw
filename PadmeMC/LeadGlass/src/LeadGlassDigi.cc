// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-21
//
// --------------------------------------------------------------

#include "LeadGlassDigi.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<LeadGlassDigi> LeadGlassDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LeadGlassDigi::LeadGlassDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LeadGlassDigi::~LeadGlassDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LeadGlassDigi::LeadGlassDigi(const LeadGlassDigi& right)
  : G4VDigi()
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fTimeSpread = right.fTimeSpread;
  fEnergy = right.fEnergy;
  fPosition = right.fPosition;
  fLocalPosition = right.fLocalPosition;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const LeadGlassDigi& LeadGlassDigi::operator=(const LeadGlassDigi& right)
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fTimeSpread = right.fTimeSpread;
  fEnergy = right.fEnergy;
  fPosition = right.fPosition;
  fLocalPosition = right.fLocalPosition;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int LeadGlassDigi::operator==(const LeadGlassDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LeadGlassDigi::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(fPosition);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(0.,1.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LeadGlassDigi::Print()
{
//  G4cout << "- channel: " << fChannelId
//         << " NHits: " << fNHits
//	 << " time: " << G4BestUnit(fTime,"Time")
//	 << " time spread: " << G4BestUnit(fTimeSpread,"Time")
//         << " energy deposit: " << G4BestUnit(fEnergy,"Energy")
//    //         << " global position: " << G4BestUnit(fPosition,"Length")
//         << " local position: " << G4BestUnit(fLocalPosition,"Length")
//	 << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
