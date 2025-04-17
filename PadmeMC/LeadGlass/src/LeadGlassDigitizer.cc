#include "LeadGlassGeometry.hh"
#include "LeadGlassDigitizer.hh"
#include "LeadGlassDigi.hh"
#include "LeadGlassHit.hh"

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
  //std::vector <LeadGlassDigi* >  digis[1];
  //for(int ich = 0;ich < 100;ich ++) {
  //  digis[ich].clear();
  //}
 
  // If hits are present, digitize them
  if (pVHC) {
    
    //// Zero digi vectors
    //G4double dEnergy[100];
    //G4double dTime[100];
    //for (G4int i=0;i<50;i++) {
    //  dEnergy[i] = 0.;
    //  dTime[i] = 1.E23; // Infinite ;)
    //}
    G4double eTotLG = 0.;
    G4double firstTime = 1.*s;
    G4double lastTime = 0.;
    G4double cogX = 0.;
    G4double cogY = 0.;
    G4double cogZ = 0.;

    // Loop over all hits
    G4int n_hit = pVHC->entries();
    for (G4int i=0; i<n_hit; i++) { 

      // Save first and last hit time
      G4double hTime    = (*pVHC)[i]->GetTime();
      if (hTime < firstTime) firstTime = hTime;
      if (hTime > lastTime) lastTime = hTime;

      // Compute total energy
      G4double hEnergy  = (*pVHC)[i]->GetEnergy();
      eTotLG += hEnergy; //Compute total deposited energy

      // Compute center of gravity
      G4ThreeVector hLocPos = (*pVHC)[i]->GetLocalPosition();
      cogX += hLocPos.x()*hEnergy;
      cogY += hLocPos.y()*hEnergy;
      cogZ += hLocPos.z()*hEnergy;

    }

    // Scale CoG to total energy
    if (eTotLG > 0.) {
      cogX /= eTotLG;
      cogY /= eTotLG;
      cogZ /= eTotLG;
    }

    // Avoid negative time spread when there are no hits
    if (n_hit == 0) firstTime = 0.;

    // Create single LeadGlass digi and store it
    LeadGlassDigi* digi = new LeadGlassDigi();
    digi->SetChannelId(0);
    digi->SetTime(firstTime);
    digi->SetTimeSpread(lastTime-firstTime);
    digi->SetEnergy(eTotLG);
    digi->SetLocalPosition(G4ThreeVector(cogX,cogY,cogZ));
    digi->SetPosition(G4ThreeVector(cogX,cogY,LeadGlassGeometry::GetInstance()->GetLeadGlassFrontFacePosZ()+cogZ));
    digi->SetNHits(n_hit);
    //digis[0].push_back(digi);
    LeadGlassdigiCollection->insert(digi);

  }
  //LeadGlassdigiCollection->insert(digis[0][0]);
  StoreDigiCollection(LeadGlassdigiCollection);
    
}
