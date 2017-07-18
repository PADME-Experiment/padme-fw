// TPixDigitizer.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
// --------------------------------------------------------------

#include "TPixDigitizer.hh"

#include "TPixHit.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include <vector>

TPixDigitizer::TPixDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "TPixDigiCollection";
  collectionName.push_back(colName);
}

TPixDigitizer::~TPixDigitizer()
{}

void TPixDigitizer::Digitize()
{
  TPixDigiCollection* tPixDigiCollection = new TPixDigiCollection("TPixDigitizer","TPixDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for TPix
  G4int tPixHCID = theDM->GetHitsCollectionID("TPixCollection");
  if (tPixHCID<0) return;
  TPixHitsCollection* tPixHC = 0;
  tPixHC = (TPixHitsCollection*)(theDM->GetHitsCollection(tPixHCID));

  // If hits are present, digitize them
  if (tPixHC) {

    // Create digi vectors
    std::vector<G4int> dChannel;
    std::vector<G4double> dEnergy;
    std::vector<G4double> dTime;

    // Loop over all hits
    G4int n_hit = tPixHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*tPixHC)[i]->GetChannelId();
      G4double hTime    = (*tPixHC)[i]->GetTime();
      G4double hEnergy  = (*tPixHC)[i]->GetEnergy();

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
      TPixDigi* digi = new TPixDigi();
      digi->SetChannelId(dChannel[i]);
      digi->SetTime(dTime[i]);
      digi->SetEnergy(dEnergy[i]);
      tPixDigiCollection->insert(digi);
      //digi->Print();
    }
  }

  StoreDigiCollection(tPixDigiCollection);

}
