// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-22
//
// --------------------------------------------------------------

#include "TargetDigitizer.hh"

#include "TargetHit.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include <vector>

TargetDigitizer::TargetDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "TargetDigiCollection";
  collectionName.push_back(colName);
}

TargetDigitizer::~TargetDigitizer()
{}

void TargetDigitizer::Digitize()
{
  TargetDigiCollection* targetDigiCollection = new TargetDigiCollection("TargetDigitizer","TargetDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for Target
  G4int targetHCID = theDM->GetHitsCollectionID("TargetCollection");
  if (targetHCID<0) return;
  TargetHitsCollection* targetHC = 0;
  targetHC = (TargetHitsCollection*)(theDM->GetHitsCollection(targetHCID));

  // If hits are present, digitize them
  if (targetHC) {

    // Create digi vectors
    std::vector<G4int> dChannel;
    std::vector<G4double> dEnergy;
    std::vector<G4double> dTime;

    // Loop over all hits
    G4int n_hit = targetHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      //G4int    hChannel = (*targetHC)[i]->GetChannelId();
      G4int    hChannel = 0; // This must be computed from the local position of the hit
      G4double hTime    = (*targetHC)[i]->GetTime();
      G4double hEnergy  = (*targetHC)[i]->GetEnergy();

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
      TargetDigi* digi = new TargetDigi();
      digi->SetChannelId(dChannel[i]);
      digi->SetTime(dTime[i]);
      digi->SetEnergy(dEnergy[i]);
      targetDigiCollection->insert(digi);
      //digi->Print();
    }
  }

  StoreDigiCollection(targetDigiCollection);

}
