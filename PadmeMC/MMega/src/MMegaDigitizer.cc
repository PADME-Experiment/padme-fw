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
    
    std::map<G4int, G4double> dTime;
    std::map<G4int, G4double> dCharge;

    // Loop over all hits
    G4int n_hit = MMegaHC->entries();
    
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int          hTrackType          = (*MMegaHC)[i]->GetPType();
      G4double       hTime               = (*MMegaHC)[i]->GetTime();
      G4double       hEnergy             = (*MMegaHC)[i]->GetEnergy();
      G4ThreeVector  hPosition           = (*MMegaHC)[i]->GetPosition();
      G4ThreeVector  hLocalPositionStart = (*MMegaHC)[i]->GetLocalPositionStart();
      G4ThreeVector  hLocalPositionEnd   = (*MMegaHC)[i]->GetLocalPositionEnd();
      G4double       hETrack             = (*MMegaHC)[i]->GetETrack();
      // G4int         hTrackId   = (*MMegaHC)[i]->GetTrackID();

      //only digitize electrons and positrons
      if(hTrackType == 2 || hTrackType == 3){ 
          
        // Generate ionizations for current hit
        MMegaIonizations* ioni = new MMegaIonizations(hLocalPositionStart, hLocalPositionEnd, hEnergy);
        G4int NHits = ioni->GetNHits();
        
        if(hEnergy>=ioni->GetIonizationEnergy()) {
          // fill digi quantities
          for(G4int j = 0; j < NHits; j++){

            // This line was used to compute charge just by multiplying
            // the charge of one drift electron times the charge
            // G4double charge = ioni->GetElectronCharge()*ioni->GetGain();
            G4int id = ioni->GetID(j);
            G4double r = ioni->GetRadius(j);

            // G4cout << "MMegaDigitier StripID : " << id << G4endl;
            // G4cout << "MMegaDigitier Charge : " << charge << G4endl;
            // G4cout << "MMegaDigitier Time : " << t << G4endl;
            // G4cout << "MMegaDigitier z : " << t*0.105 << G4endl;
            

            if (dCharge[id] == 0.) {
              // Now instead we extract it from a data driven distribution
              G4double charge = ioni->GetChargeFromDistribution();
              if (r <= ioni->GetFirstZoneRadius()){
                charge *= ioni->GetFirstGain();
              }
              else if(r <=ioni->GetSecondZoneRadius()){
                charge *= ioni->GetSecondGain();
              }
              dCharge[id] = charge;
            }

            // first version without time smearing
            // G4double t = ioni->GetTime(i) + hTime;
            // G4cout << "MMegaDigitizer GetTime = " << ioni->GetTime(j) << G4endl;
            // Time resolution depends on charge so we smear the drift time
            // at some point remember to sum in hTime to!
            G4double t = ioni->GetTime(j) + ioni->GetTimeSpread(dCharge[id]); // ns
            

            // // For now the charge is the sum of deposited charges
            // if (dCharge.find(id) != dCharge.end()) {
            //   // If the key exists, increment the existing value by the new value
            //   dCharge[id] += charge;
            // } else {
            //   // If the key doesn't exist, insert the new key-value pair
            //   dCharge.insert({id, charge});
            // }

            // // For now the time is the earliest hit time
            // if (dTime.find(id) != dTime.end()) {
            //   // If the key exists, check if the new value is lower than the existing one
            //   if (t < dTime[id]) {
            //     // If yes, substitute the existing value with the new one
            //     dTime[id] = t;
            //   }
            // } else {
            //   // If the key doesn't exist, insert the new key-value pair
            //   dTime.insert({id, t});
            // }
            
            if (dTime[id] == 0.) {
              // If yes, substitute the existing value with the new one
              dTime[id] = t;
            }

            else if(t < dTime[id]){
              dTime[id] = t;
            }
        

          } // loop over ionizations
        } // if ionization energy
        delete ioni;
      } // if track type
    } //loop over hits

    //Iterate over the maps using a range-based for loop
    std::map<G4int, std::vector<G4double>>::iterator it;
    for (auto it = dTime.begin(); it != dTime.end(); ++it) {
      MMegaDigi* digi = new MMegaDigi();
      digi->SetID(it->first);
      //digi->SetNHits(it->second.size());
      digi->SetTime(it->second);
      digi->SetCharge(dCharge[it->first]);
      mMegaDigiCollection->insert(digi);
      // digi->Print();
    } // end of storing digis
  } // end of if MMegaHC
  
  StoreDigiCollection(mMegaDigiCollection);

}
