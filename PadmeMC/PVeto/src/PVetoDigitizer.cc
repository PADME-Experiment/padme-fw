#include "PVetoDigitizer.hh"

#include "PVetoHit.hh"

#include "G4DigiManager.hh"
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
  PVetoDigiCollection* pVetoDigiCollection = new PVetoDigiCollection("PVetoDigitizer","PVetoDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for PVeto
  G4int pVHCID = theDM->GetHitsCollectionID("PVetoCollection");
  PVetoHitsCollection* pVHC = 0;
  pVHC = (PVetoHitsCollection*)(theDM->GetHitsCollection(pVHCID));

  // If hits are present, digitize them
  if (pVHC) {

    // Zero digi vectors
    G4double dEnergy[100];
    G4double dTime[100];
    for (G4int i=0;i<100;i++) {
      dEnergy[i] = 0.;
      dTime[i] = 1.E23; // Infinite ;)
    }

    // Loop over all hits
    G4int n_hit = pVHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*pVHC)[i]->GetChannelId();
      G4double hTime    = (*pVHC)[i]->GetTime();
      G4double hEnergy  = (*pVHC)[i]->GetEnergy();
      G4ThreeVector hLocPos = (*pVHC)[i]->GetLocalPosition();

      // Add information to digi (just an example)
      dEnergy[hChannel] += hEnergy;
      if (hTime < dTime[hChannel]) dTime[hChannel] = hTime;

    }

    // Create digis if energy is not zero
    for (G4int i=0;i<100;i++) {
      if (dEnergy[i] > 0.) {
	PVetoDigi* digi = new PVetoDigi();
	digi->SetChannelId(i);
	digi->SetTime(dTime[i]);
	digi->SetTimeSpread(0.);
	digi->SetEnergy(dEnergy[i]);
	digi->SetPosition(G4ThreeVector(0.,0.,0.));
	digi->SetLocalPosition(G4ThreeVector(0.,0.,0.));
	pVetoDigiCollection->insert(digi);
	digi->Print();
      }
    }
  }

  StoreDigiCollection(pVetoDigiCollection);

}
