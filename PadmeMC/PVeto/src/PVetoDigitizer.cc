#include "PVetoDigitizer.hh"

#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

PVetoDigitizer::PVetoDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "PVetoDigiCollection";
  collectionName.push_back(colName);
}

PVetoDigitizer::~PVetoDigitizer()
{}

void PVetoDigitizer::Digitize()
{
  fPVetoDigiCollection = new PVetoDigiCollection("PVetoDigitizer","PVetoDigiCollection");
}
