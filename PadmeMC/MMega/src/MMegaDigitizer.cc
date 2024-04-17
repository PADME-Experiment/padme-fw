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

  // MMegaDigiCollection* mMegaDigiCollection = new MMegaDigiCollection("MMegaDigitizer","MMegaDigiCollection");
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
      G4int         hTrackType          = (*MMegaHC)[i]->GetPType();
      G4double       hTime               = (*MMegaHC)[i]->GetTime();
      G4double       hEnergy             = (*MMegaHC)[i]->GetEnergy();
      G4ThreeVector hPosition           = (*MMegaHC)[i]->GetPosition();
      G4ThreeVector hLocalPositionStart = (*MMegaHC)[i]->GetLocalPositionStart();
      G4ThreeVector hLocalPositionEnd   = (*MMegaHC)[i]->GetLocalPositionEnd();
      G4double       hETrack             = (*MMegaHC)[i]->GetETrack();
      // G4int         hTrackId   = (*MMegaHC)[i]->GetTrackID();


      if(hTrackType == 2 || hTrackType == 3){ //only digitize electrons and positrons
        if(hETrack >= 1.0*MeV){
          
          // Generate Ionizations for current hit
          MMegaIonizations* ioni = new MMegaIonizations(hLocalPositionStart, hLocalPositionEnd, hEnergy);
          G4int Nionizations = ioni->GetNIonizations();

          // fill digi quantities
          for(G4int i = 0; i < Nionizations; i++){
            
            G4int id = (ioni->GetIDs())[i];
            G4double t = (ioni->GetTimes())[i]+hTime;
            G4double r = (ioni->GetRadii())[i];

            G4double charge = ioni->GetElectronCharge()*ioni->GetGain(); //for one drift electron

            if (r <= ioni->GetFirstZoneRadius()){
              charge *= ioni->GetFirstGain();
            }
            else if(r <=ioni->GetSecondZoneRadius()){
              charge *= ioni->GetSecondGain();
            }

            // this commented block compiles but returns segmentation fault
            G4double old_charge = dCharge[id];
            G4double new_charge = old_charge + charge;
            dCharge[id] = new_charge;
            
            std::map<G4int, std::vector<G4double>>::iterator it = dTimes.find(id);
            if (it != dTimes.end()) {
              it->second.push_back(t);
            }
            else {
              std::vector<G4double> v;
              v.push_back(t);
              dTimes[id] = v;
            }
            

            // G4cout << "-----------TEST PRINT-----------"<<G4endl;
            // G4cout << " id = " << id << G4endl;
            // G4cout << " t  = " << t << G4endl;
            // G4cout << " r  = " << r << G4endl;
            // G4cout << " charge = " << charge << G4endl;

            

          } // loop over ionizations
        } // if over energy
      } // if track type


    } //loop over hits

    //Iterate over the maps using a range-based for loop
    std::map<G4int, std::vector<G4double>>::iterator it;
    for (auto it = dTimes.begin(); it != dTimes.end(); ++it) {
      MMegaDigi* digi = new MMegaDigi();
      digi->SetID(it->first);
      digi->SetNHits(it->second.size());
      digi->SetTimes(it->second);
      // get the right charge for the strip and multiply by number of collected electrons
      digi->SetCharge(dCharge[it->first] * it->second.size());
      mMegaDigiCollection->insert(digi);
      digi->Print();
    } // end of storing digis

  } // end of if MMegaHC
  
  StoreDigiCollection(mMegaDigiCollection);

}
