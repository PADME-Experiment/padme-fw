#include "SACDigitizer.hh"

#include "SACHit.hh"
#include "SACDigi.hh"

#include "SACGeometry.hh"

#include "G4Poisson.hh"
#include "G4UnitsTable.hh"
#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include <vector>

SACDigitizer::SACDigitizer(G4String name)
:G4VDigitizerModule(name)
{

  // Register digi collection
  G4String colName = "SACDigiCollection";
  collectionName.push_back(colName);

  SACGeometry* geo = SACGeometry::GetInstance();

  // Initialize digitization parameters
  fCrystalLength = geo->GetCrystalSizeZ(); // Length of crystal along Z
  fPropagationSpeed = geo->GetSACLightPropagationSpeed(); // Speed of light in SAC crystal
  fEHtoNPEConversion = geo->GetDigiEtoNPEConversion(); // Number of p.e.s produced per MeV energy
  fPEtoSConversion = geo->GetDigiPEtoSignalConversion(); // Contribution of 1 p.e. to integral ADC signal
  fPMTTransitTime = geo->GetDigiPMTTransitTime(); // PMT transit time from photocathode to anode
  fPMTCableDelay = geo->GetDigiPMTCableDelay(); // Delay due to connection cables

  // Compute minimal output signal time for SAC (assumes Target at -70cm wrt center of yoke)
  //fMinimumSignalTime = (70.*cm+geo->GetSACFrontFacePosZ())/(2.998E8*m/s)+fCrystalLength/fPropagationSpeed+fPMTTransitTime+fPMTCableDelay;

  // Relative collection efficiency as function of Z along the crystal (bin 0: front face, bin N: readout face)
  fCollectionMap = geo->GetDigiPECollectionMap();
  fCollMapNBins = geo->GetDigiPECollectionMapNBins();
  fCollMapBinLen = geo->GetDigiPECollectionMapBinLength();

  G4cout << G4endl;
  G4cout << "=== ECalDigitizer parameters ===" << G4endl;
  G4cout << "Crystal length along Z: " << G4BestUnit(fCrystalLength,"Length") << G4endl;
  G4cout << "Light propagation speed in SAC crystal: " << fPropagationSpeed << " mm/ns" << G4endl;
  G4cout << "Number of photoelectorns produced per MeV of hit energy: " << fEHtoNPEConversion << "/MeV" << G4endl;
  G4cout << "Contribution of 1 p.e. to integral ADC signal: " << fPEtoSConversion << G4endl;
  G4cout << "Collection probability along Z: " << fCollMapNBins << " bins of "
	 << G4BestUnit(fCollMapBinLen,"Length") << " - Map:";
  for(G4int i=0;i<fCollectionMap.size();i++) G4cout << " " << fCollectionMap[i];
  G4cout << G4endl;
  G4cout << "PMT transit time: " << G4BestUnit(fPMTTransitTime,"Time") << G4endl;
  G4cout << "PMT cable delay: " << G4BestUnit(fPMTCableDelay,"Time") << G4endl;
  //G4cout << "Minimum output signal time: " << G4BestUnit(fMinimumSignalTime,"Time") << G4endl;
  G4cout << G4endl;

}

SACDigitizer::~SACDigitizer()
{}

void SACDigitizer::Digitize()
{
  SACDigiCollection* sacDigiCollection = new SACDigiCollection("SACDigitizer","SACDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for SAC
  G4int sacHCID = theDM->GetHitsCollectionID("SACCollection");
  if (sacHCID<0) return;
  SACHitsCollection* sacHC = 0;
  sacHC = (SACHitsCollection*)(theDM->GetHitsCollection(sacHCID));

  // If hits are present, digitize them
  if (sacHC) {

    // Loop over all hits
    G4int n_hit = sacHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*sacHC)[i]->GetChannelId();
      G4double hEnergy  = (*sacHC)[i]->GetEnergy();

      // Get number of photoelectrons and time of first photoelectron for this hit
      G4double npe, tFirst;
      ComputeNpe((*sacHC)[i],&npe,&tFirst);

      // Get signal contribution of this hit and add PMT transit to digi time
      G4double hSignal = npe*fPEtoSConversion;
      G4double hTime = tFirst+fPMTTransitTime+fPMTCableDelay;

      // Look for corresponding digi or create it if not present
      SACDigi* digi = 0;
      for (G4int j=0; j < sacDigiCollection->entries(); j++) {
	if ((*sacDigiCollection)[j]->GetChannelId() == hChannel) {
	  digi = (*sacDigiCollection)[j];
	  break;
	}
      }
      if (digi == 0) {
	digi = new SACDigi();
	digi->SetChannelId(hChannel);
	//digi->SetQHistoStart(fMinimumSignalTime);
	sacDigiCollection->insert(digi);
      }

      // Add hit info to digi
      digi->AddEnergy(hEnergy);
      if (hTime < digi->GetTime()) digi->SetTime(hTime);
      digi->AddChargeAtTime(hSignal,hTime);

    } // End loop over hits

    // Print digis
    //for (G4int i=0; i < sacDigiCollection->entries(); i++) {
    //  (*sacDigiCollection)[i]->Print();
    //}

  }

  StoreDigiCollection(sacDigiCollection);

}

void SACDigitizer::ComputeNpe(SACHit* hit,G4double* npe,G4double* time)
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

  // Get the average number of photoelectrons produced by photocathode
  // for a hit of energy e at position iz, spread it with Poisson
  *npe = G4Poisson(e*fCollectionMap[iz]*fEHtoNPEConversion);

}
