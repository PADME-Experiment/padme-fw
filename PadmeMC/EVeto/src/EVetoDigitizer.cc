// EVetoDigitizer.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2106-07-22
// --------------------------------------------------------------

#include "EVetoDigitizer.hh"

#include "EVetoHit.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include <vector>


EVetoDigitizer::EVetoDigitizer(G4String name)
:G4VDigitizerModule(name)
{
  G4String colName = "EVetoDigiCollection";
  collectionName.push_back(colName);
}

EVetoDigitizer::~EVetoDigitizer()
{}

void EVetoDigitizer::Digitize()
{
  const double EVetoDigiTimeWindow = 5.;

  EVetoDigiCollection* eVetoDigiCollection = new EVetoDigiCollection("EVetoDigitizer","EVetoDigiCollection");

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for EVeto
  G4int eVetoHCID = theDM->GetHitsCollectionID("EVetoCollection");
  if (eVetoHCID<0) return;
  EVetoHitsCollection* eVetoHC = 0;
  eVetoHC = (EVetoHitsCollection*)(theDM->GetHitsCollection(eVetoHCID));
  
  std::vector<EVetoDigi*> digis[100];
  for(int ich = 0;ich < 100;ich ++) {
    digis[ich].clear();
  }
  
  // If hits are present, digitize them
  if (eVetoHC) {

    // Loop over all hits
    G4int n_hit = eVetoHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // Get hit information
      G4int    hChannel = (*eVetoHC)[i]->GetChannelId();
      G4double hTime    = (*eVetoHC)[i]->GetTime();
      G4double hEnergy  = (*eVetoHC)[i]->GetEnergy();
      G4ThreeVector hLocPos = (*eVetoHC)[i]->GetLocalPosition();
      
      // Check if the hit is related to some of the present digis
      
      int newdigi = 1;
      for(unsigned int idigi = 0;idigi<digis[hChannel].size();idigi++) {
	if(fabs(digis[hChannel][idigi]->GetTime()/digis[hChannel][idigi]->GetEnergy()  - hTime ) < EVetoDigiTimeWindow ) {
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
	EVetoDigi* digi = new EVetoDigi();
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
	eVetoDigiCollection->insert( digis[ich][idigi] );
	//digis[ich][idigi]->Print();	  
      }
    }

  }

  StoreDigiCollection(eVetoDigiCollection);

}
