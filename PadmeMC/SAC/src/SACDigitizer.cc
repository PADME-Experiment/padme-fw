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

    // Loop over all hits
    G4int n_hit = sacHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*sacHC)[i]->GetChannelId();
      G4double hTime    = (*sacHC)[i]->GetTime();
      G4double hEnergy  = (*sacHC)[i]->GetEnergy();

      // Look for corresponding digi or create it if not present
      SACDigi* digi = 0;
      for (G4int j=0; j < sacDigiCollection->entries(); j++) {
	if ((*sacDigiCollection)[j]->GetChannelId() == hChannel) {
	  digi = (*sacDigiCollection)[j];
	  break;
	}
      }
      if (digi == 0) {
	digi = new SACDigi();
	digi->SetChannelId(hChannel);
	sacDigiCollection->insert(digi);
      }

      // Add hit info to digi
      digi->AddEnergy(hEnergy);
      if (hTime < digi->GetTime()) digi->SetTime(hTime);
      digi->AddEnergyAtTime(hEnergy,hTime);

    } // End loop over hits

    // Print digis
    //for (G4int i=0; i < sacDigiCollection->entries(); i++) {
    //  (*sacDigiCollection)[i]->Print();
    //}

  }

  StoreDigiCollection(sacDigiCollection);

}
