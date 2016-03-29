//
// ********************************************************************
//  PADME modular physics list
//  Based on QGSP_BERT + OpticalPhotons
//
// ********************************************************************
//

#include <iomanip>   

#include "globals.hh"
#include "G4ios.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"

#include "G4StepLimiterBuilder.hh"
#include "G4NeutronTrackingCut.hh"

#include "G4OpticalPhysics.hh"

#include "G4DataQuestionaire.hh"
#include "HadronPhysicsQGSP_BERT.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"

#include "HadronPhysicsQGSP_BIC.hh"
#include "HadronPhysicsQGSP_BIC_HP.hh"

#include "PADME_PHYS.hh"

#include "Constants.hh"

PADME_PHYS::PADME_PHYS(G4int ver): G4VModularPhysicsList()
{
  G4DataQuestionaire it(photon);
  G4cout << "<<< Geant4 Physics List simulation engine: PADME_PHYS"<<G4endl;
  G4cout <<G4endl;
  
  this->defaultCutValue = 0.7*CLHEP::mm;  
  this->SetVerboseLevel(ver);

  // EM Physics
  // includes:
  //    Multiple Scattering:
  //       < 100*MeV: G4UrbanMscModel95
  //       > 100*MeV: G4WentzelVIModel
  //    CoulombScattering
  //       > 100*MeV: 
  //    Ionisation
  //    Bremsstrahlung
  //    eplusAnnihilation
  this->RegisterPhysics( new G4EmStandardPhysics(ver) );
  

  // Synchroton Radiation & GN Physics
  // includes: 
  //     BertiniElectroNuclearBuilder
  //         Gamma - Bertini Cascade
  //         e+/e- - the virtual photon is converted to a real photon; -> Bertini
  if(IsNuclearON==1){
    this->RegisterPhysics( new G4EmExtraPhysics(ver) );
  }

  // Scintillation and Cerenkov radiation
  // includes: optical photon propagation
  this->RegisterPhysics( new G4OpticalPhysics(ver) );

  // Decays
  this->RegisterPhysics( new G4DecayPhysics(ver) );

  if(IsNuclearON==1){
    
    // Hadron Elastic scattering
    if(NuclearHP == 1) {
      this->RegisterPhysics( new G4HadronElasticPhysicsHP(ver) );
    } else {
      this->RegisterPhysics( new G4HadronElasticPhysics(ver) );
    }
    // Hadron Physics
    if (NuclearBertini == 1) {
      if(NuclearHP == 1) {
	this->RegisterPhysics( new HadronPhysicsQGSP_BERT_HP(ver));
      } else {
      this->RegisterPhysics( new HadronPhysicsQGSP_BERT(ver));
      }
    } else if (NuclearBinary == 1 ) {
      if(NuclearHP == 1) {
	this->RegisterPhysics(  new HadronPhysicsQGSP_BIC_HP(ver));
      } else {
	this->RegisterPhysics(  new HadronPhysicsQGSP_BIC(ver));
      }
    }

    // // Stopping Physics
    // this->RegisterPhysics( new G4StoppingPhysics(ver) );
    
    // Ion Physics
    this->RegisterPhysics( new G4IonPhysics(ver));
  }

  // Neutron tracking cut
  this->RegisterPhysics( new G4NeutronTrackingCut(ver));
  
  //Step limiter
  this->RegisterPhysics(new G4StepLimiterBuilder());

}

 
PADME_PHYS::~PADME_PHYS() {
} 



void PADME_PHYS::SetCuts()
{
  if (this->verboseLevel >1){
    G4cout << "PADME_PHYS::SetCuts:";
  }  
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 

  this->SetCutsWithDefault();   
  
//  if (this->verboseLevel >0)
//    G4VUserPhysicsList::DumpCutValuesTable();  
}
