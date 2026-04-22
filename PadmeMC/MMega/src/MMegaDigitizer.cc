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
#include "TRandom3.h"
#include <vector>
#include <map>


MMegaDigitizer::MMegaDigitizer(G4String name)
:G4VDigitizerModule(name)
{ InitializeAmplificationFluctuation();
  G4String colName = "MMegaDigiCollection";
  collectionName.push_back(colName);
}

MMegaDigitizer::~MMegaDigitizer()
{}

TF1* MMegaDigitizer::ampl_dist = nullptr;

void MMegaDigitizer::InitializeAmplificationFluctuation(){
    
    if (!ampl_dist) {
        
        // Initialize function according to parameters fitted from real data
        // coming from may 2024 Test Beam at LNF 
        ampl_dist = new TF1("AmplificationFluctuation", "landau", 1, 2600*300);  // [0, 2500] Charge in ADC Counts
        ampl_dist->SetParameter(0, 0.0134);
        ampl_dist->SetParameter(1, 10000.);
        ampl_dist->SetParameter(2, 800.);

    }
}
G4double MMegaDigitizer::GetAmplificationFluctuation(){
    return ampl_dist->GetRandom();
}
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

  G4cout<< MMegaHC<<" "<<MMegaHC->entries()<<G4endl;
  // If hits are present, digitize them  
  if (MMegaHC) {
    
    std::map<G4int, G4double> dTime;
    std::map<G4int, G4double> dCharge;
    std::map<G4int, G4int> dNHitxCh;

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
      G4ThreeVector  hVertexPos   = (*MMegaHC)[i]->GetVertexPosition();
      G4double       hETrack             = (*MMegaHC)[i]->GetETrack();
      G4int          hTrackId   = (*MMegaHC)[i]->GetTrackID();
      G4int          hParentId   = (*MMegaHC)[i]->GetParentID();
      //only digitize electrons and positrons
      if(hTrackType == 2 || hTrackType == 3){ 

        //G4cout<<"tagg "<<hTrackId<< " "<<hParentId<<" "<<hPosition.x()<< " "<< hPosition.y()<<" "<<hPosition.z()<<" "<<hVertexPos.x()<< " "<< hVertexPos.y()<<" "<<hVertexPos.z()<<G4endl;
        // Generate ionizations for current hit
        MMegaIonizations* ioni = new MMegaIonizations(hLocalPositionStart, hLocalPositionEnd, hEnergy);
        
        if(hEnergy>=ioni->GetIonizationEnergy()) {
          // fill digi quantities
          G4int NPrimary = ioni->GetNHits();

          for(G4int j = 0; j < NPrimary; j++){

            // This line was used to compute charge just by multiplying
            // the charge of one drift electron times the charge
            // G4double charge = ioni->GetElectronCharge()*ioni->GetGain();
            G4int id = ioni->GetID(j);
            //G4cout<<"id: "<<id<<" NHits:"<<NHits<<G4endl;
            G4double r = ioni->GetRadius(j);

            // G4cout << "MMegaDigitier StripID : " << id << G4endl;
            // G4cout << "MMegaDigitier Charge : " << charge << G4endl;
            // G4cout << "MMegaDigitier Time : " << t << G4endl;
            // G4cout << "MMegaDigitier z : " << t*0.105 << G4endl;
             G4double charge = 1; // number of primary electron!
              if (r <= ioni->GetFirstZoneRadius()){
                charge *= ioni->GetFirstGain(); //weighted for the gain
              }
              else if(r <=ioni->GetSecondZoneRadius()){
                charge *= ioni->GetSecondGain(); //weighted for the gain
              }else{
                charge *=ioni->GetExternalGain();
              }
            if (dCharge[id] == 0.) {
              // Now instead we extract it from a data driven distribution
              dCharge[id] = charge;
            } else{
              dCharge[id] += charge;
            }
            // if(charge>0){
            if (dNHitxCh[id] == 0.) {
              // Now instead we extract it from a data driven distribution
              dNHitxCh[id] = 1;
            } else{
              dNHitxCh[id]++;
            }
            //}
            //Collects how many primaries reach each strip
            
            // Time resolution depends on charge so we smear the drift time
            // at some point remember to sum in hTime to!
            G4double t = ioni->GetTime(j);// + ioni->GetTimeSpread(dCharge[id]); // ns
            
            
            //save a digi for every hit
            // MMegaDigi* digi = new MMegaDigi();
            // digi->SetID(id);
            
            // digi->SetTime(t);
            // digi->SetCharge(dCharge[id]);
            // mMegaDigiCollection->insert(digi);
            if (dTime[id] == 0.) {
              // If yes, substitute the existing value with the new one
              dTime[id] = t*charge;
            }

            else{
              dTime[id] += t*charge;
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
      //G4cout<<"id saved: "<<it->first<<G4endl;
      G4double ampl = GetAmplificationFluctuation();
      //digi->SetNHits(it->second.size());
      digi->SetTime((it->second)/dCharge[it->first]);
      G4double stripcharge = (dCharge[it->first]);
      // if(stripcharge > 160){ 
      //   stripcharge-=160;
      // }
      // if(stripcharge > 160){
      //   stripcharge-=160;
      // }
      // if(stripcharge > 160){
      //   stripcharge-=160;
      // }
      digi->SetCharge(stripcharge*ampl/4);
      digi->SetNHitxCh(dNHitxCh[it->first]);
      mMegaDigiCollection->insert(digi);
      // digi->Print();
    } // end of storing digis
  } // end of if MMegaHC
  
  StoreDigiCollection(mMegaDigiCollection);

}
