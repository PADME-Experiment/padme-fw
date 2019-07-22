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
  G4cout << "=== SACDigitizer parameters ===" << G4endl;
  G4cout << "Crystal length along Z: " << G4BestUnit(fCrystalLength,"Length") << G4endl;
  G4cout << "Light propagation speed in SAC crystal: " << fPropagationSpeed << " mm/ns" << G4endl;
  G4cout << "Number of photoelectorns produced per MeV of hit energy: " << fEHtoNPEConversion << "/MeV" << G4endl;
  G4cout << "Contribution of 1 p.e. to integral ADC signal: " << fPEtoSConversion << G4endl;
  G4cout << "Collection probability along Z: " << fCollMapNBins << " bins of "
	 << G4BestUnit(fCollMapBinLen,"Length") << " - Map:";
  for(G4int i=0;i<(G4int)fCollectionMap.size();i++) G4cout << " " << fCollectionMap[i];
  G4cout << G4endl;
  G4cout << "PMT transit time: " << G4BestUnit(fPMTTransitTime,"Time") << G4endl;
  G4cout << "PMT cable delay: " << G4BestUnit(fPMTCableDelay,"Time") << G4endl;
  //G4cout << "Minimum output signal time: " << G4BestUnit(fMinimumSignalTime,"Time") << G4endl;
  G4cout << G4endl;

}

SACDigitizer::~SACDigitizer()
{}

void SACDigitizer::DigitizeSinglHit()
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


void SACDigitizer::Digitize()
{
  const double SACDigiTimeWindow = 1.;

  SACDigiCollection* SACdigiCollection = new SACDigiCollection("SACDigitizer","SACDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for SAC
  G4int sacHCID = theDM->GetHitsCollectionID("SACCollection");
  if (sacHCID<0) return;
  SACHitsCollection* sacHC = 0;
  sacHC = (SACHitsCollection*)(theDM->GetHitsCollection(sacHCID));
  
  std::vector <SACDigi* >  digis[100];
  for(int ich = 0;ich < 100;ich ++) {
    digis[ich].clear();
  }
  
  // If hits are present, digitize them
  if (sacHC) {

    // Zero digi vectors
    G4double dEnergy[100];
    G4double dTime[100];
    for (G4int i=0;i<100;i++) {
      dEnergy[i] = 0.;
      dTime[i] = 1.E23; // Infinite ;)
    }

    // Loop over all hits
    G4int n_hit = sacHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*sacHC)[i]->GetChannelId();
      G4double hTime    = (*sacHC)[i]->GetTime();
      G4double hEnergy  = (*sacHC)[i]->GetEnergy();
      G4ThreeVector hLocPos = (*sacHC)[i]->GetLocalPosition();
      
      // Add information to digi (just an example)
      dEnergy[hChannel] += hEnergy;
      if (hTime < dTime[hChannel]) dTime[hChannel] = hTime;
      
      // Check if the hit is related to some of the present digis
      
      int newdigi = 1;
      for(unsigned int idigi = 0;idigi<digis[hChannel].size();idigi++) {
	if(fabs(digis[hChannel][idigi]->GetTime()/digis[hChannel][idigi]->GetEnergy()  - hTime ) < SACDigiTimeWindow ) {
	  //Energy deposit in the same channel close to previous energy deposit
	  digis[hChannel][idigi]->SetTime(
					  //(hEnergy*hTime + digis[hChannel][idigi]->GetTime()*digis[hChannel][idigi]->GetEnergy())
					  //					  / ( hEnergy + digis[hChannel][idigi]->GetEnergy())  
					  
					  hEnergy*hTime + digis[hChannel][idigi]->GetTime()
					  );
	  digis[hChannel][idigi]->SetTimeSpread(digis[hChannel][idigi]->GetTimeSpread() +  hTime*hEnergy*hTime);
	  
	  
	  digis[hChannel][idigi]->SetLocalPosition(
						   //(digis[hChannel][idigi]->GetLocalPosition()*digis[hChannel][idigi]->GetEnergy() + hLocPos*hEnergy)
						  ///( hEnergy + digis[hChannel][idigi]->GetEnergy())
						   hLocPos*hEnergy + digis[hChannel][idigi]->GetLocalPosition()
						  );
	  
	  
	  digis[hChannel][idigi]->SetEnergy(digis[hChannel][idigi]->GetEnergy() + hEnergy );
	  digis[hChannel][idigi]->SetNHits(digis[hChannel][idigi]->GetNHits()+1);
	  newdigi=0;	  
	  break;
	}
      }
      if(newdigi) {
	SACDigi* digi = new SACDigi();
	digi->SetChannelId( hChannel );
	digi->SetTime(hTime*hEnergy);
	digi->SetTimeSpread( hTime*hTime*hEnergy  );
	digi->SetEnergy(hEnergy);
	digi->SetPosition(G4ThreeVector(0.,0.,0.));
	digi->SetLocalPosition(hLocPos*hEnergy);
	digi->SetNHits(1);
	digis[hChannel].push_back(digi);
      }           
    }
    
    
    for(unsigned int ich=0;ich<100;ich++) {
      for(unsigned int idigi=0;idigi < digis[ich].size();idigi++) {
	//Compute the proper quantities of the digi
	digis[ich][idigi]->SetTime(digis[ich][idigi]->GetTime() / digis[ich][idigi]->GetEnergy());
	digis[ich][idigi]->SetLocalPosition(digis[ich][idigi]->GetLocalPosition()/digis[ich][idigi]->GetEnergy());
	if(digis[ich][idigi]->GetNHits() > 1) {
	  digis[ich][idigi]->SetTimeSpread( std::sqrt(	digis[ich][idigi]->GetTimeSpread()/digis[ich][idigi]->GetEnergy() -  digis[ich][idigi]->GetTime()* digis[ich][idigi]->GetTime()      ) );
	} else {
	  digis[ich][idigi]->SetTimeSpread(0);
	}
	SACdigiCollection->insert( digis[ich][idigi] );
	//digis[ich][idigi]->Print();	  
      }
    }
    
    



  //   // Create digis if energy is not zero
  //   for (G4int i=0;i<100;i++) {
  //     if (dEnergy[i] > 0.) {
  // 	SACDigi* digi = new SACDigi();
  // 	digi->SetChannelId(i);
  // 	digi->SetTime(dTime[i]);
  // 	digi->SetTimeSpread(0.);
  // 	digi->SetEnergy(dEnergy[i]);
  // 	digi->SetPosition(G4ThreeVector(0.,0.,0.));
  // 	digi->SetLocalPosition(G4ThreeVector(0.,0.,0.));
  // 	SACdigiCollection->insert(digi);
  // 	digi->Print();
  //     }
  //   }
  
  }

  StoreDigiCollection(SACdigiCollection);

}
