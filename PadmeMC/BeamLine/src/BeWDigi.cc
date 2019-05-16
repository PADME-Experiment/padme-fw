// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2019-04-29
//
// --------------------------------------------------------------

#include "BeWDigi.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<BeWDigi> BeWDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeWDigi::BeWDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeWDigi::~BeWDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeWDigi::BeWDigi(const BeWDigi& right)
  : G4VDigi()
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const BeWDigi& BeWDigi::operator=(const BeWDigi& right)
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int BeWDigi::operator==(const BeWDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeWDigi::Draw()
{
  G4VVisManager* visManager = G4VVisManager::GetConcreteInstance();
  if(visManager)
  {
    //G4Circle circle(fPosition); 
    //circle.SetScreenSize(2.);
    //circle.SetFillStyle(G4Circle::filled);
    //G4Colour colour(0.,1.,0.);
    //G4VisAttributes attribs(colour);
    //circle.SetVisAttributes(attribs);
    //visManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeWDigi::Print()
{
  G4cout << "- channel: " << fChannelId
	 << " time: " << G4BestUnit(fTime,"Time")
         << " energy: " << G4BestUnit(fEnergy,"Energy")
	 << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
