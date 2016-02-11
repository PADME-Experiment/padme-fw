#include "PrimaryGeneratorAction.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

#include "DetectorConstruction.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* myDC)
{
  fBeamGenerator = new BeamGenerator(myDC);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fBeamGenerator;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fBeamGenerator->GenerateBeam(anEvent);
}
