#include "SACDigitizer.hh"

#include "SACHit.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include <vector>

SACDigitizer::SACDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "SACDigiCollection";
  collectionName.push_back(colName);
}

SACDigitizer::~SACDigitizer()
{}

void SACDigitizer::Digitize()
{
  SACDigiCollection* sacDigiCollection = new SACDigiCollection("SACDigitizer","SACDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for SAC
  G4int sacHCID = theDM->GetHitsCollectionID("SACCollection");
  SACHitsCollection* sacHC = 0;
  sacHC = (SACHitsCollection*)(theDM->GetHitsCollection(sacHCID));

  // If hits are present, digitize them
  if (sacHC) {

    // Create digi vectors
    std::vector<G4int> dChannel;
    std::vector<G4double> dEnergy;
    std::vector<G4double> dTime;

    // Loop over all hits
    G4int n_hit = sacHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*sacHC)[i]->GetChannelId();
      G4double hTime    = (*sacHC)[i]->GetTime();
      G4double hEnergy  = (*sacHC)[i]->GetEnergy();

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
      SACDigi* digi = new SACDigi();
      digi->SetChannelId(dChannel[i]);
      digi->SetTime(dTime[i]);
      digi->SetEnergy(dEnergy[i]);
      sacDigiCollection->insert(digi);
      digi->Print();
    }
  }

  StoreDigiCollection(sacDigiCollection);

}
