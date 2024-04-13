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
    
    std::map<G4int, std::vector<G4double>> dTimes;
    std::map<G4int, G4double> dCharge;

    // Loop over all hits
    G4int n_hit = MMegaHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int         hTrackType = (*MMegaHC)[i]->GetPType();
      G4double      hTime      = (*MMegaHC)[i]->GetTime();
      G4double      hEnergy    = (*MMegaHC)[i]->GetEnergy();
      G4ThreeVector hPosition  = (*MMegaHC)[i]->GetPosition();
      G4ThreeVector hLocalPositionStart = (*MMegaHC)[i]->GetLocalPositionStart();
      G4ThreeVector hLocalPositionEnd   = (*MMegaHC)[i]->GetLocalPositionEnd();
      // G4int         hTrackId   = (*MMegaHC)[i]->GetTrackID();
      G4double      hETrack    = (*MMegaHC)[i]->GetETrack();



      if(hTrackType == 2 || hTrackType == 3){ //only digitize electrons and positrons
        if(hETrack >= 1.0*MeV){
          // Generate Ionizations for current hit
          MMegaIonizations* ioni = new MMegaIonizations(hLocalPositionStart, hLocalPositionEnd, hEnergy);

          G4int Nionizations = ioni->GetNIonizations();
          std::vector<G4int> IDs = ioni->GetIDs();
          std::vector<G4double> Times = ioni->GetTimes();
          std::vector<G4double> Radii = ioni->GetRadii();

          // fill digi quantities
          for(int i = 0; i < Nionizations; i++){
            dTimes[IDs[i]].push_back(Times[i]+hTime);
            G4double charge = ioni->GetElectronCharge()*ioni->GetGain(); //for one drift electron
            if (Radii[i] <= ioni->GetFirstZoneRadius()){
              charge *= ioni->GetFirstGain();
            }
            else if(Radii[i]<=ioni->GetSecondZoneRadius()){
              charge *= ioni->GetSecondGain();
            }
            dCharge[IDs[i]] = charge;
          }
        }
      }


    } //loop over hits

    // Iterate over the maps using a range-based for loop
    for (auto it1 = dTimes.begin(); it1 != dTimes.end(); ++it1) {
        MMegaDigi* digi = new MMegaDigi();
        digi->SetID(it1->first);
        digi->SetNHits(it1->second.size());
        digi->SetTimes(it1->second);
        //get the right charge for the strip an multiply by numer of collected electrons
        digi->SetCharge(dCharge[it1->first]*it1->second.size());
        mMegaDigiCollection->insert(digi);
        digi->Print();
    }

  }
  
  StoreDigiCollection(mMegaDigiCollection);

}
