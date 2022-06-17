#include "ETagDigitizer.hh"
#include "ETagHit.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include <vector>


ETagDigitizer::ETagDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "ETagDigiCollection";
  collectionName.push_back(colName);
}

ETagDigitizer::~ETagDigitizer()
{}

void ETagDigitizer::Digitize()
{
  const double ETagDigiTimeWindow = 5.;

  ETagDigiCollection* eTagDigiCollection = new ETagDigiCollection("ETagDigitizer","ETagDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for ETag
  G4int pVHCID = theDM->GetHitsCollectionID("ETagCollection");
  if (pVHCID<0) return;
  ETagHitsCollection* pVHC = 0;
  pVHC = (ETagHitsCollection*)(theDM->GetHitsCollection(pVHCID));  
  std::vector <ETagDigi* >  digis[100];
  for(int ich = 0;ich < 100;ich ++) {
    digis[ich].clear();
  }
 
  // If hits are present, digitize them
  if (pVHC) {
    // Zero digi vectors
    G4double dEnergy[100];
    G4double dTime[100];
    for (G4int i=0;i<50;i++) {
      dEnergy[i] = 0.;
      dTime[i] = 1.E23; // Infinite ;)
    }

    // Loop over all hits
    G4int n_hit = pVHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*pVHC)[i]->GetChannelId();
      G4double hTime    = (*pVHC)[i]->GetTime();
      G4double hEnergy  = (*pVHC)[i]->GetEnergy();
      G4ThreeVector hLocPos = (*pVHC)[i]->GetLocalPosition();
      
      // Add information to digi (just an example)
      dEnergy[hChannel] += hEnergy;
      if (hTime < dTime[hChannel]) dTime[hChannel] = hTime;
      
      // Check if the hit is related to some of the present digis
      
      int newdigi = 1;
      for(unsigned int idigi = 0;idigi<digis[hChannel].size();idigi++) {
	if(fabs(digis[hChannel][idigi]->GetTime()/digis[hChannel][idigi]->GetEnergy()  - hTime ) < ETagDigiTimeWindow ) {
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
	ETagDigi* digi = new ETagDigi();
	digi->SetChannelId( hChannel );
	digi->SetTime(hTime*hEnergy);
	digi->SetTimeSpread( hTime*hTime*hEnergy  );
	digi->SetEnergy(hEnergy);
	digi->SetPosition(G4ThreeVector(0.,0.,0.));
	digi->SetLocalPosition(hLocPos*hEnergy);
	digi->SetNHits(1);
	digis[hChannel].push_back(digi);
	std::cout << "Size of ETagDigi:   " << sizeof(ETagDigi ) 
	 	  << "  Size of unsigned short:  "    << sizeof(unsigned short)
	 	  << std::endl; 
	
      }           
    }
    
    int nch=0;
    double etot=0;
    for(unsigned int ich=0;ich<100;ich++) {
      etot = 0.;
      for(unsigned int idigi=0;idigi < digis[ich].size();idigi++) {
	//Compute the proper quantities of the digi
	etot+=digis[ich][idigi]->GetEnergy();
	digis[ich][idigi]->SetTime(digis[ich][idigi]->GetTime() / digis[ich][idigi]->GetEnergy());
	digis[ich][idigi]->SetLocalPosition(digis[ich][idigi]->GetLocalPosition()/digis[ich][idigi]->GetEnergy());
	if(digis[ich][idigi]->GetNHits() > 1) {
	  digis[ich][idigi]->SetTimeSpread( std::sqrt(	digis[ich][idigi]->GetTimeSpread()/digis[ich][idigi]->GetEnergy() -  digis[ich][idigi]->GetTime()* digis[ich][idigi]->GetTime()      ) );
	} else {
	  digis[ich][idigi]->SetTimeSpread(0);
	}
	eTagDigiCollection->insert( digis[ich][idigi] );
	//digis[ich][idigi]->Print();	  
      }      
      if (etot > 0.1 ) nch++;
    }
    
    std::cout << "Number of channels fired: " << nch << "  NDigis:  "<<  eTagDigiCollection->GetSize() << std::endl;
    



  //   // Create digis if energy is not zero
  //   for (G4int i=0;i<100;i++) {
  //     if (dEnergy[i] > 0.) {
  // 	ETagDigi* digi = new ETagDigi();
  // 	digi->SetChannelId(i);
  // 	digi->SetTime(dTime[i]);
  // 	digi->SetTimeSpread(0.);
  // 	digi->SetEnergy(dEnergy[i]);
  // 	digi->SetPosition(G4ThreeVector(0.,0.,0.));
  // 	digi->SetLocalPosition(G4ThreeVector(0.,0.,0.));
  // 	eTagDigiCollection->insert(digi);
  // 	digi->Print();
  //     }
  //   }
  
  }

  StoreDigiCollection(eTagDigiCollection);

}
