// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
//
// --------------------------------------------------------------

#include "MMDigitizer.hh"
#include "MMHit.hh"
#include "MMIonizations.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "TRandom3.h"
#include <vector>
#include <map>


MMDigitizer::MMDigitizer(G4String name)
:G4VDigitizerModule(name)
{ InitializeAmplificationFluctuation();
  G4String colName = "MMDigiCollection";
  collectionName.push_back(colName);
}

MMDigitizer::~MMDigitizer()
{}

TF1* MMDigitizer::ampl_dist = nullptr;

void MMDigitizer::InitializeAmplificationFluctuation(){
    
    if (!ampl_dist) {
        
        // Initialize function according to parameters fitted from real data
        // coming from may 2024 Test Beam at LNF 
        ampl_dist = new TF1("AmplificationFluctuation", "landau", 1, 2600*300);  // [0, 2500] Charge in ADC Counts
        ampl_dist->SetParameter(0, 0.0134);
        ampl_dist->SetParameter(1, 10000.);
        ampl_dist->SetParameter(2, 800.);

    }
}
G4double MMDigitizer::GetAmplificationFluctuation(){
    return ampl_dist->GetRandom();
}
void MMDigitizer::Digitize()
{
  // const G4double MMDigiTimeWindow = 700*us; 
  MMDigiCollection* MMDigiColl = new MMDigiCollection("MMDigitizer","MMDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for MM
  G4int MMHCID = theDM->GetHitsCollectionID("MMCollection");
  if (MMHCID<0) return;
  MMHitsCollection* MMHC = 0;
  MMHC = (MMHitsCollection*)(theDM->GetHitsCollection(MMHCID));  

  //G4cout<< MMHC<<" "<<MMHC->entries()<<G4endl;
  // If hits are present, digitize them  
  if (MMHC) {
    
    std::map<G4int, G4double> dTime;
    std::map<G4int, G4double> dCharge;
    //std::map<G4int, G4int> dNHitxCh;

    // Loop over all hits
    G4int n_hit = MMHC->entries();
    
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int          hTrackType          = (*MMHC)[i]->GetPType();
      G4double       hTime               = (*MMHC)[i]->GetTime();
      G4double       hEnergy             = (*MMHC)[i]->GetEnergy();
      G4ThreeVector  hPosition           = (*MMHC)[i]->GetPosition();
      G4ThreeVector  hLocalPositionStart = (*MMHC)[i]->GetLocalPositionStart();
      G4ThreeVector  hLocalPositionEnd   = (*MMHC)[i]->GetLocalPositionEnd();
      G4ThreeVector  hVertexPos   = (*MMHC)[i]->GetVertexPosition();
      G4double       hETrack             = (*MMHC)[i]->GetETrack();
      G4int          hTrackId   = (*MMHC)[i]->GetTrackID();
      G4int          hParentId   = (*MMHC)[i]->GetParentID();
      //only digitize electrons and positrons
      if(hTrackType == 2 || hTrackType == 3){ 

        //G4cout<<"tagg "<<hTrackId<< " "<<hParentId<<" "<<hPosition.x()<< " "<< hPosition.y()<<" "<<hPosition.z()<<" "<<hVertexPos.x()<< " "<< hVertexPos.y()<<" "<<hVertexPos.z()<<G4endl;
        // Generate ionizations for current hit
        MMIonizations* ioni = new MMIonizations(hLocalPositionStart, hLocalPositionEnd, hEnergy);
        
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
            
            // G4cout << "MMDigitier StripID : " << id << G4endl;
            // G4cout << "MMDigitier Charge : " << charge << G4endl;
            // G4cout << "MMDigitier Time : " << t << G4endl;
            // G4cout << "MMDigitier z : " << t*0.105 << G4endl;
             G4double charge = 1; // number of primary electron!
              if (r <= ioni->GetFirstZoneRadius()){
                charge *= ioni->GetFirstGain(); //weighted for the gain
              }
              else if(r > ioni->GetFirstZoneRadius() && r <= ioni->GetSecondZoneRadius()){
                charge *= ioni->GetSecondGain(); //weighted for the gain
              }else{
                charge *=ioni->GetExternalGain();
              }
            if (dCharge[id] == 0.) {
              // Exstracted from a data driven distribution
              dCharge[id] = charge;
            } else{
              dCharge[id] += charge;
            }
            // if(charge>0){
            // // if (dNHitxCh[id] == 0.) {
            //   // Now instead we extract it from a data driven distribution
            //   dNHitxCh[id] = 1;
            // } else{
            //   dNHitxCh[id]++;
            // }
            //}
            //Collects how many primaries reach each strip
            
            // Time resolution depends on charge so we smear the drift time
            // at some point remember to sum in hTime to!
            G4double t = ioni->GetTime(j);// + ioni->GetTimeSpread(dCharge[id]); // ns
            
            
            //save a digi for every hit
            // MMDigi* digi = new MMDigi();
            // digi->SetID(id);
            
            // digi->SetTime(t);
            // digi->SetCharge(dCharge[id]);
            // MMDigiCollection->insert(digi);
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
      MMDigi* digi = new MMDigi();
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
      //digi->SetNHitxCh(dNHitxCh[it->first]);
      MMDigiColl->insert(digi);
      // digi->Print();
    } // end of storing digis
  } // end of if MMHC
  
  StoreDigiCollection(MMDigiColl);

}
