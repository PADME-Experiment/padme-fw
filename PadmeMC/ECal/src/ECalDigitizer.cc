#include "ECalDigitizer.hh"

#include "ECalHit.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include <vector>

ECalDigitizer::ECalDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "ECalDigiCollection";
  collectionName.push_back(colName);
}

ECalDigitizer::~ECalDigitizer()
{}

void ECalDigitizer::Digitize()
{
  ECalDigiCollection* eCalDigiCollection = new ECalDigiCollection("ECalDigitizer","ECalDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for ECal
  G4int eCalHCID = theDM->GetHitsCollectionID("ECalCollection");
  ECalHitsCollection* eCalHC = 0;
  eCalHC = (ECalHitsCollection*)(theDM->GetHitsCollection(eCalHCID));

  // If hits are present, digitize them
  if (eCalHC) {

    // Create digi vectors
    std::vector<G4int> dChannel;
    std::vector<G4double> dEnergy;
    std::vector<G4double> dTime;

    // Loop over all hits
    G4int n_hit = eCalHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*eCalHC)[i]->GetChannelId();
      G4double hTime    = (*eCalHC)[i]->GetTime();
      G4double hEnergy  = (*eCalHC)[i]->GetEnergy();

      // Loop over used channels
      G4int found = 0;
      for (G4int i=0; i < dChannel.size(); i++) {
	if (dChannel[i] == hChannel) {
	  dEnergy[i] += hEnergy;
	  if (hTime < dTime[i]) dTime[i] = hTime;
	  found = 1;
	  break;
	}
      }
      if (found == 0) {
	dChannel.push_back(hChannel);
	dEnergy.push_back(hEnergy);
	dTime.push_back(hTime);
      }

    }

    // Create digis for active channels
    for (G4int i=0; i < dChannel.size(); i++) {
      ECalDigi* digi = new ECalDigi();
      digi->SetChannelId(dChannel[i]);
      digi->SetTime(dTime[i]);
      digi->SetEnergy(dEnergy[i]);
      eCalDigiCollection->insert(digi);
      digi->Print();
    }
  }

  StoreDigiCollection(eCalDigiCollection);

}
