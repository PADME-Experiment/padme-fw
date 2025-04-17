#include "ECalDigitizer.hh"

#include "ECalHit.hh"
#include "ECalDigi.hh"

#include "ECalGeometry.hh"

#include "G4Poisson.hh"
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

  ECalGeometry* geo = ECalGeometry::GetInstance();

  // Initialize digitization parameters
  fCrystalLength = geo->GetCrystalSizeZ(); // Length of crystal along Z
  fPropagationSpeed = geo->GetBGOLightPropagationSpeed(); // Speed of light in BGO
  fEHtoNPEConversion = geo->GetDigiEtoNPEConversion(); // Number of p.e.s produced per MeV energy
  fPEtoSConversion = geo->GetDigiPEtoSignalConversion(); // Contribution of 1 p.e. to integral ADC signal
  fPMTTransitTime = geo->GetDigiPMTTransitTime(); // PMT transit time from photocathode to anode
  fPMTCableDelay = geo->GetDigiPMTCableDelay(); // Delay due to connection cables

  // Relative collection efficiency as function of Z along the crystal (bin 0: front face, bin N: readout face)
  fCollectionMap = geo->GetDigiPECollectionMap();
  fCollMapNBins = geo->GetDigiPECollectionMapNBins();
  fCollMapBinLen = geo->GetDigiPECollectionMapBinLength();

  G4cout << G4endl;
  G4cout << "=== ECalDigitizer parameters ===" << G4endl;
  G4cout << "Crystal length along Z: " << G4BestUnit(fCrystalLength,"Length") << G4endl;
  G4cout << "Light propagation speed in BGO: " << fPropagationSpeed << " mm/ns" << G4endl;
  G4cout << "Number of photoelectorns produced per MeV of hit energy: " << fEHtoNPEConversion << "/MeV" << G4endl;
  G4cout << "Contribution of 1 p.e. to integral ADC signal: " << fPEtoSConversion << G4endl;
  G4cout << "Collection probability along Z: " << fCollMapNBins << " bins of "
	 << G4BestUnit(fCollMapBinLen,"Length") << " - Map:";
  for(G4int i=0;i<(G4int)fCollectionMap.size();i++) G4cout << " " << fCollectionMap[i];
  G4cout << G4endl;
  G4cout << "PMT transit time: " << G4BestUnit(fPMTTransitTime,"Time") << G4endl;
  G4cout << "PMT cable delay: " << G4BestUnit(fPMTCableDelay,"Time") << G4endl;
  G4cout << G4endl;

}

ECalDigitizer::~ECalDigitizer()
{}

void ECalDigitizer::DigitizeSingleDigi()
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

      // Get number of photoelectrons and time of first photoelectron for this hit
      G4double npe, tFirst;
      ComputeNpe((*eCalHC)[i],&npe,&tFirst);

      // Get signal contribution of this hit and add PMT transit to digi time
      G4double hSignal = npe*fPEtoSConversion;
      G4double hTime = tFirst+fPMTTransitTime+fPMTCableDelay;

      // Add hit contribution to corresponding digi (create digi if needed)
      G4int found = 0;
      for (G4int i=0; i < (G4int)fDChannel.size(); i++) {
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
    // Active means at least 1 hit and total signal>0
    for (G4int i=0; i < (G4int)fDChannel.size(); i++) {
      if (fDSignal[i]>0.) {
	ECalDigi* digi = new ECalDigi();
	digi->SetChannelId(fDChannel[i]);
	digi->SetTime(fDTime[i]);
	digi->SetEnergy(fDEnergy[i]);
	digi->SetSignal(fDSignal[i]);
	eCalDigiCollection->insert(digi);
	//digi->Print();
      }
    }
  }

  StoreDigiCollection(eCalDigiCollection);

}

void ECalDigitizer::ComputeNpe(ECalHit* hit,G4double* npe,G4double* time)
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


void ECalDigitizer::Digitize()
{
  const double ECalDigiTimeWindow = 5.;

  ECalDigiCollection* ECaldigiCollection = new ECalDigiCollection("ECalDigitizer","ECalDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();
  // Get access to hit collection for ECal
  G4int ECalHCID = theDM->GetHitsCollectionID("ECalCollection");
  if (ECalHCID<0) return;
  ECalHitsCollection* ECalHC = 0;
  ECalHC = (ECalHitsCollection*)(theDM->GetHitsCollection(ECalHCID));
  
  std::vector <ECalDigi* >  digis[10000];
  for(int ich = 0;ich < 10000;ich ++) {
    digis[ich].clear();
  }
  
  // If hits are present, digitize them
  if (ECalHC) {

    // Zero digi vectors
    G4double dEnergy[10000];
    G4double dTime[10000];
    for (G4int i=0;i<10000;i++) {
      dEnergy[i] = 0.;
      dTime[i] = 1.E23; // Infinite ;)
    }
    // Loop over all hits
    G4int n_hit = ECalHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*ECalHC)[i]->GetChannelId();
      //std::cout<<"chID ECALDIGITIZE " << hChannel << std::endl;
      G4double hTime    = (*ECalHC)[i]->GetTime();
      G4double hEnergy  = (*ECalHC)[i]->GetEnergy();
      G4ThreeVector hLocPos = (*ECalHC)[i]->GetLocalPosition();
      // Add information to digi (just an example)
      dEnergy[hChannel] += hEnergy;      
      if (hTime < dTime[hChannel]) dTime[hChannel] = hTime;   // collect time of first hit.
      
      // Check if the hit is related to some of the present digis
      
      int newdigi = 1;
      for(unsigned int idigi = 0;idigi<digis[hChannel].size();idigi++) {
	if(fabs(digis[hChannel][idigi]->GetTime()/digis[hChannel][idigi]->GetEnergy()  - hTime ) < ECalDigiTimeWindow ) {
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
	ECalDigi* digi = new ECalDigi();
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
    
    for(unsigned int ich=0;ich<10000;ich++) {
      for(unsigned int idigi=0;idigi < digis[ich].size();idigi++) {
	//Compute the proper quantities of the digi
	digis[ich][idigi]->SetTime(digis[ich][idigi]->GetTime() / digis[ich][idigi]->GetEnergy());
	digis[ich][idigi]->SetLocalPosition(digis[ich][idigi]->GetLocalPosition()/digis[ich][idigi]->GetEnergy());
	if(digis[ich][idigi]->GetNHits() > 1) {
	  digis[ich][idigi]->SetTimeSpread( std::sqrt(	digis[ich][idigi]->GetTimeSpread()/digis[ich][idigi]->GetEnergy() -  digis[ich][idigi]->GetTime()* digis[ich][idigi]->GetTime()      ) );
	} else {
	  digis[ich][idigi]->SetTimeSpread(0);
	}
	ECaldigiCollection->insert( digis[ich][idigi] );
	//digis[ich][idigi]->Print();	  
      }
    }
    
    



  //   // Create digis if energy is not zero
  //   for (G4int i=0;i<100;i++) {
  //     if (dEnergy[i] > 0.) {
  // 	ECalDigi* digi = new ECalDigi();
  // 	digi->SetChannelId(i);
  // 	digi->SetTime(dTime[i]);
  // 	digi->SetTimeSpread(0.);
  // 	digi->SetEnergy(dEnergy[i]);
  // 	digi->SetPosition(G4ThreeVector(0.,0.,0.));
  // 	digi->SetLocalPosition(G4ThreeVector(0.,0.,0.));
  // 	ECaldigiCollection->insert(digi);
  // 	digi->Print();
  //     }
  //   }
  
  }

  StoreDigiCollection(ECaldigiCollection);

}
