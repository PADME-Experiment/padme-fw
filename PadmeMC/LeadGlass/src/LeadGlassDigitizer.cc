#include "LeadGlassDigitizer.hh"
#include "LeadGlassDigi.hh"
#include "LeadGlassHit.hh"

#include "LeadGlassGeometry.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include <vector>


LeadGlassDigitizer::LeadGlassDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "LeadGlassDigiCollection";
  collectionName.push_back(colName);
  LeadGlassGeometry* geo = LeadGlassGeometry::GetInstance(); //MR
}

LeadGlassDigitizer::~LeadGlassDigitizer()
{}

void LeadGlassDigitizer::Digitize()
{
  LeadGlassDigiCollection* LeadGlassdigiCollection = new LeadGlassDigiCollection("LeadGlassDigitizer","LeadGlassDigiCollection");
  
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for LeadGlass
  G4int pVHCID = theDM->GetHitsCollectionID("LeadGlassCollection");
  
  if (pVHCID<0) return;
  LeadGlassHitsCollection* pVHC = 0;
  pVHC = (LeadGlassHitsCollection*)(theDM->GetHitsCollection(pVHCID));  
  std::vector <LeadGlassDigi* >  digis[1];
  //for(int ich = 0;ich < 100;ich ++) {
  //  digis[ich].clear();
  //}
 
  // If hits are present, digitize them
  // MR 7/12/2023 messing up everything!
  if (pVHC) {
    
    //// Zero digi vectors
    //G4double dEnergy[100];
    //G4double dTime[100];
    //for (G4int i=0;i<50;i++) {
    //  dEnergy[i] = 0.;
    //  dTime[i] = 1.E23; // Infinite ;)
    //}
    G4double fETotLG=0; //MR
    G4double hTime;    
    // Loop over all hits
    G4int n_hit = pVHC->entries();
    for (G4int i=0;i<n_hit;i++) { 
      // Get hit information
      hTime    = (*pVHC)[i]->GetTime();
      G4double hEnergy  = (*pVHC)[i]->GetEnergy();
      G4ThreeVector hLocPos = (*pVHC)[i]->GetLocalPosition();
      fETotLG+=hEnergy; //Compute total deposited energy
    }
      LeadGlassDigi* digi = new LeadGlassDigi();
      digi->SetTime(hTime); //MR need to do it properly
      digi->SetEnergy(fETotLG);
      digis[0].push_back(digi);
    }
    LeadGlassdigiCollection->insert(digis[0][0]);
    StoreDigiCollection(LeadGlassdigiCollection);
    
  }
 
