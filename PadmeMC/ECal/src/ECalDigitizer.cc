#include "ECalDigitizer.hh"

#include "ECalHit.hh"
#include "ECalDigi.hh"

#include "ECalGeometry.hh"

#include "G4UnitsTable.hh"
#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include <vector>

ECalDigitizer::ECalDigitizer(G4String name)
:G4VDigitizerModule(name)
{

  // Register digi collection
  G4String colName = "ECalDigiCollection";
  collectionName.push_back(colName);

  // Initialize digitization parameters
  fCrystalLength = ECalGeometry::GetInstance()->GetCrystalSizeZ();
  fPropagationSpeed = (2.998E8*m/s)/2.57; // Speed of light in BGO
  fConversionFactor = 1.; // Need to decide final units

  // Collection efficiency as function of Z along the crystal (bin 0: front face, bin N: readout face)
  static const G4double cmap[] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  fCollMapNBins = sizeof(cmap)/sizeof(cmap[0]);   // Number of bins in the map
  fCollMapBinLen = fCrystalLength/fCollMapNBins;  // Length of a single bin along Z
  fCollectionMap.assign(cmap,cmap+fCollMapNBins); // Final std::vector with map

  //G4cout << "ECalDigitizer -"
  //	 << " length " << G4BestUnit(fCrystalLength,"Length")
  //	 << " speed " << fPropagationSpeed << " mm/ns"
  //	 << " conv " << fConversionFactor << " map";
  //for(G4int i=0;i<fCollectionMap.size();i++) G4cout << " " << fCollectionMap[i];
  //G4cout << G4endl;

}

ECalDigitizer::~ECalDigitizer()
{}

void ECalDigitizer::Digitize()
{
  ECalDigiCollection* eCalDigiCollection = new ECalDigiCollection("ECalDigitizer","ECalDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for ECal
  G4int eCalHCID = theDM->GetHitsCollectionID("ECalCollection");
  if (eCalHCID<0) return;
  ECalHitsCollection* eCalHC = 0;
  eCalHC = (ECalHitsCollection*)(theDM->GetHitsCollection(eCalHCID));

  // If hits are present, digitize them
  if (eCalHC) {

    // Reset digi vectors
    fDChannel.clear();
    fDTime.clear();
    fDEnergy.clear();
    fDSignal.clear();

    // Loop over all hits
    G4int n_hit = eCalHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*eCalHC)[i]->GetChannelId();
      //G4double hTime    = (*eCalHC)[i]->GetTime();
      G4double hEnergy  = (*eCalHC)[i]->GetEnergy();

      // Get contribution of this hit to the channel signal
      G4double hSignal, hTime;
      ComputeSignal((*eCalHC)[i],&hSignal,&hTime);

      // Loop over used channels
      G4int found = 0;
      for (G4int i=0; i < fDChannel.size(); i++) {
	if (fDChannel[i] == hChannel) {
	  fDEnergy[i] += hEnergy;
	  fDSignal[i] += hSignal;
	  if (hTime < fDTime[i]) fDTime[i] = hTime;
	  found = 1;
	  break;
	}
      }
      if (found == 0) {
	fDChannel.push_back(hChannel);
	fDEnergy.push_back(hEnergy);
	fDSignal.push_back(hSignal);
	fDTime.push_back(hTime);
      }

    }

    // Create digis for active channels
    for (G4int i=0; i < fDChannel.size(); i++) {
      ECalDigi* digi = new ECalDigi();
      digi->SetChannelId(fDChannel[i]);
      digi->SetTime(fDTime[i]);
      digi->SetEnergy(fDEnergy[i]);
      digi->SetSignal(fDSignal[i]);
      eCalDigiCollection->insert(digi);
      //digi->Print();
    }
  }

  StoreDigiCollection(eCalDigiCollection);

}

void ECalDigitizer::ComputeSignal(ECalHit* hit,G4double* signal,G4double* time)
{

  // Extract hit information
  G4double e = hit->GetEnergy();
  G4double t = hit->GetTime();
  G4double dz = 0.5*fCrystalLength-hit->GetLocalPosZ(); // Distance from hit to readout face of the crystal
  G4double dt = dz/fPropagationSpeed; // Time for light to reach readout face
  G4int iz = std::max(0,fCollMapNBins-int(dz/fCollMapBinLen)-1);
  //printf("EcalDigitizer - e %f t %f dz %f iz %d dt %f\n",e,t,dz,iz,dt);

  // Correct hit time with signal arrival time
  *time = t+dt;

  // Compute signal from energy and position
  *signal = e*fCollectionMap[iz]*fConversionFactor;

}
