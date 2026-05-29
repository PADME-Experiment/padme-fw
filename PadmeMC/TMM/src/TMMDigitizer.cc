// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
//
// --------------------------------------------------------------

#include "TMMDigitizer.hh"
#include "TMMHit.hh"
#include "TMMIonizations.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "TRandom3.h"
#include <vector>
#include <map>


TMMDigitizer::TMMDigitizer(G4String name)
:G4VDigitizerModule(name)
{ InitializeAmplificationFluctuation();
  G4String colName = "TMMDigiCollection";
  collectionName.push_back(colName);
}

TMMDigitizer::~TMMDigitizer()
{}

TF1* TMMDigitizer::ampl_dist = nullptr;

void TMMDigitizer::InitializeAmplificationFluctuation(){
    
    if (!ampl_dist) {
        
        // Initialize function according to parameters fitted from real data
        // coming from may 2024 Test Beam at LNF 
        ampl_dist = new TF1("AmplificationFluctuation", "landau", 1, 2600*300);  // [0, 2500] Charge in ADC Counts
        ampl_dist->SetParameter(0, 0.0134);
        ampl_dist->SetParameter(1, 100.); 
        ampl_dist->SetParameter(2, 8.);

        // ampl_dist->SetParameter(1, 10000.); 
        // ampl_dist->SetParameter(2, 8000.);

    }
}
G4double TMMDigitizer::GetAmplificationFluctuation(){
    return ampl_dist->GetRandom();
}
void TMMDigitizer::Digitize()
{
  // const G4double TMMDigiTimeWindow = 700*us; 
  TMMDigiCollection* TMMDigiColl = new TMMDigiCollection("TMMDigitizer","TMMDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for TMM
  G4int TMMHCID = theDM->GetHitsCollectionID("TMMCollection");
  if (TMMHCID<0) return;
  TMMHitsCollection* TMMHC = 0;
  TMMHC = (TMMHitsCollection*)(theDM->GetHitsCollection(TMMHCID));  

  //G4cout<< TMMHC<<" "<<TMMHC->entries()<<G4endl;
  // If hits are present, digitize them  
  if (TMMHC) {
    
    std::map<G4int, G4double> dTime;
    std::map<G4int, G4double> dCharge;
    //std::map<G4int, G4int> dNHitxCh;

    // Loop over all hits
    G4int n_hit = TMMHC->entries();
    
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int          hTrackType          = (*TMMHC)[i]->GetPType();
      G4double       hTime               = (*TMMHC)[i]->GetTime();
      G4double       hEnergy             = (*TMMHC)[i]->GetEnergy();
      G4ThreeVector  hPosition           = (*TMMHC)[i]->GetPosition();
      G4ThreeVector  hLocalPositionStart = (*TMMHC)[i]->GetLocalPositionStart();
      G4ThreeVector  hLocalPositionEnd   = (*TMMHC)[i]->GetLocalPositionEnd();
      G4ThreeVector  hVertexPos   = (*TMMHC)[i]->GetVertexPosition();
      G4double       hETrack             = (*TMMHC)[i]->GetETrack();
      G4int          hTrackId   = (*TMMHC)[i]->GetTrackID();
      G4int          hParentId   = (*TMMHC)[i]->GetParentID();
      //only digitize electrons and positrons
      if(hTrackType == 2 || hTrackType == 3){ 

        //G4cout<<"tagg "<<hTrackId<< " "<<hParentId<<" "<<hPosition.x()<< " "<< hPosition.y()<<" "<<hPosition.z()<<" "<<hVertexPos.x()<< " "<< hVertexPos.y()<<" "<<hVertexPos.z()<<G4endl;
        // Generate ionizations for current hit
        TMMIonizations* ioni = new TMMIonizations(hLocalPositionStart, hLocalPositionEnd, hEnergy);
        
        if(hEnergy>=ioni->GetIonizationEnergy()) {
          // fill digi quantities
          G4int NPrimary = ioni->GetNHits();

          for(G4int j = 0; j < NPrimary; j++){

            // This line was used to compute charge just by multiplying
            // the charge of one drift electron times the charge
            // G4double charge = ioni->GetElectronCharge()*ioni->GetGain();
            G4int id = ioni->GetID(j);
            //G4cout<<"id: "<<id<<" NHits:"<<NHits<<G4endl;
            //G4double r = ioni->GetRadius(j);
            
            // G4cout << "TMMDigitier StripID : " << id << G4endl;
            // G4cout << "TMMDigitier Charge : " << charge << G4endl;
            // G4cout << "TMMDigitier Time : " << t << G4endl;
            // G4cout << "TMMDigitier z : " << t*0.105 << G4endl;
             G4double charge = 1; // number of primary electron!
              // if (r <= ioni->GetFirstZoneRadius()){
              //   charge *= ioni->GetFirstGain(); //weighted for the gain
              // }
              // else if(r > ioni->GetFirstZoneRadius() && r <= ioni->GetSecondZoneRadius()){
              //   charge *= ioni->GetSecondGain(); //weighted for the gain
              // }else{
              //   charge *=ioni->GetExternalGain();
              // }
            if (dCharge[id] == 0.) {
              // Exstracted from a data driven distribution
              dCharge[id] = charge*ioni->GetGain();
            } else{
              dCharge[id] += charge*ioni->GetGain();
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
            // TMMDigi* digi = new TMMDigi();
            // digi->SetID(id);
            
            // digi->SetTime(t);
            // digi->SetCharge(dCharge[id]);
            // TMMDigiCollection->insert(digi);
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
      TMMDigi* digi = new TMMDigi();
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
      digi->SetCharge(stripcharge*ampl);
      //digi->SetNHitxCh(dNHitxCh[it->first]);
      TMMDigiColl->insert(digi);
      // digi->Print();
    } // end of storing digis
  } // end of if TMMHC
  
  StoreDigiCollection(TMMDigiColl);

}
