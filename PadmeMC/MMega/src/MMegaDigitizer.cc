// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#include "MMegaDigitizer.hh"
#include "MMegaHit.hh"
#include "MMegaIonizations.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include <vector>
#include <map>


MMegaDigitizer::MMegaDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "MMegaDigiCollection";
  collectionName.push_back(colName);
}

MMegaDigitizer::~MMegaDigitizer()
{}

void MMegaDigitizer::Digitize()
{
  // const G4double MMegaDigiTimeWindow = 700*us; 

  MMegaDigiCollection* mMegaDigiCollection = new MMegaDigiCollection("MMegaDigitizer","MMegaDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for MMega
  G4int MMegaHCID = theDM->GetHitsCollectionID("MMegaCollection");
  if (MMegaHCID<0) return;
  MMegaHitsCollection* MMegaHC = 0;
  MMegaHC = (MMegaHitsCollection*)(theDM->GetHitsCollection(MMegaHCID));  

 
  // If hits are present, digitize them  
  if (MMegaHC) {
    
    std::map<std::pair<G4int, G4int>, G4double> dChargeFront;
    std::map<std::pair<G4int, G4int>, G4double> dChargeRear;
    std::map<std::pair<G4int, G4int>, std::vector<G4double>> dTimeFront;
    std::map<std::pair<G4int, G4int>, std::vector<G4double>> dTimeRear;

    // Loop over all hits
    G4int n_hit = MMegaHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int         hTrackType = (*MMegaHC)[i]->GetPType();
      // G4int         hTrackId   = (*MMegaHC)[i]->GetTrackID();
      G4double      hTime      = (*MMegaHC)[i]->GetTime();
      G4double      hEnergy    = (*MMegaHC)[i]->GetEnergy();
      // G4double      hETrack    = (*MMegaHC)[i]->GetETrack();
      G4ThreeVector hPosition  = (*MMegaHC)[i]->GetPosition();
      G4ThreeVector hLocalPositionStart = (*MMegaHC)[i]->GetLocalPositionStart();
      G4ThreeVector hLocalPositionEnd   = (*MMegaHC)[i]->GetLocalPositionEnd();

      if(hTrackType == 2 || hTrackType == 3){ //only digitize electrons and positrons

        // Generate Ionizations for current hit
        MMegaIonizations* ioni = new MMegaIonizations(hLocalPositionStart, hLocalPositionEnd, hEnergy);

        G4int Nionizations = ioni->GetNIonizations();
        std::vector<G4int> hitStripsX = ioni->GetHitStripsX();
        std::vector<G4int> hitStripsY = ioni->GetHitStripsY();
        std::vector<G4int> isRear = ioni->GetisRear();
        std::vector<G4double> StripTimes = ioni->GetStripTimes();

        // fill digi quantities

        for(int i = 0; i < Nionizations; i++){
          int size = 0;
          if(isRear[i] == 0){
            dChargeFront[{hitStripsX[i],hitStripsY[i]}]++;
            dTimeFront[{hitStripsX[i],hitStripsY[i]}].push_back(StripTimes[i]+hTime);
          }
          else if(isRear[i] == 1){ 
            dChargeRear[{hitStripsX[i],hitStripsY[i]}]++;
            dTimeRear[{hitStripsX[i],hitStripsY[i]}].push_back(StripTimes[i]+hTime);
          }

          
        }
      }
    } //loop over hits

    // Iterate over the map using a range-based for loop
    for (const auto& entry : dChargeFront) {
      int ix = entry.first.first;
      int iy = entry.first.second;

      if(!entry.second>0){
        MMegaDigi* digi = new MMegaDigi();
        digi->SetXId(ix);
        digi->SetYId(iy);
        digi->SetisRear(0);
        digi->SetNHits(entry.second);
        digi->SetCharge(entry.second);
        digi->SetTime(dTimeFront[{ix,iy}]);
        mMegaDigiCollection->insert(digi);
        digi->Print();
      }
    }

    for (const auto& entry : dChargeRear) {
      int ix = entry.first.first;
      int iy = entry.first.second;

      if(!entry.second>0){
        MMegaDigi* digi = new MMegaDigi();
        digi->SetXId(ix);
        digi->SetYId(iy);
        digi->SetisRear(1);
        digi->SetNHits(entry.second);
        digi->SetCharge(entry.second);
        digi->SetTime(dTimeFront[{ix,iy}]);
        mMegaDigiCollection->insert(digi);
        digi->Print();
      }
    }
  }
  
  StoreDigiCollection(mMegaDigiCollection);

}
