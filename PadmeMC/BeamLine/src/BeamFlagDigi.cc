// --------------------------------------------------------------
// History:
//
// Created by Mauro Raggi (mauro.raggi@roma1.infn.it) 2019-08-29
//
// --------------------------------------------------------------

#include "BeamFlagDigi.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<BeamFlagDigi> BeamFlagDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamFlagDigi::BeamFlagDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamFlagDigi::~BeamFlagDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamFlagDigi::BeamFlagDigi(const BeamFlagDigi& right)
  : G4VDigi()
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const BeamFlagDigi& BeamFlagDigi::operator=(const BeamFlagDigi& right)
{
  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;
  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int BeamFlagDigi::operator==(const BeamFlagDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeamFlagDigi::Draw()
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

void BeamFlagDigi::Print()
{
  G4cout << "- channel: " << fChannelId
	 << " time: " << G4BestUnit(fTime,"Time")
         << " energy: " << G4BestUnit(fEnergy,"Energy")
	 << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
