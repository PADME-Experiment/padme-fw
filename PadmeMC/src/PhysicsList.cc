//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: PhysicsList.cc,v 1.3 2014/04/30 08:57:41 veni Exp $

#include "PhysicsList.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include <iomanip>   
#include "Constants.hh"

#include "G4FastSimulationManagerProcess.hh"
#include "G4ParticlePropertyTable.hh"

PhysicsList::PhysicsList():  G4VUserPhysicsList()
{
  defaultCutValue = CutG4*cm;
  SetVerboseLevel(0);
  IsOpticsON=1;
}

PhysicsList::~ PhysicsList()
{
}

void PhysicsList::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  ConstructBosons();
  ConstructLeptons();
  ConstructMesons();
  ConstructBaryons();
  ConstructIons();
}

void PhysicsList::ConstructBosons()
{
  // pseudo-particles
  G4Geantino::GeantinoDefinition();
  G4ChargedGeantino::ChargedGeantinoDefinition();

  // gamma
  G4Gamma::GammaDefinition();

  // optical photon
  G4OpticalPhoton::OpticalPhotonDefinition();
}

#include "G4LeptonConstructor.hh"
void PhysicsList::ConstructLeptons()
{
  // Construct all leptons
  G4LeptonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

#include "G4MesonConstructor.hh"
void PhysicsList::ConstructMesons()
{
  //  Construct all mesons
  G4MesonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

#include "G4BaryonConstructor.hh"
void PhysicsList::ConstructBaryons()
{
  //  Construct all barions
  G4BaryonConstructor  pConstructor;
  pConstructor.ConstructParticle();  
}

#include "G4IonConstructor.hh"
void PhysicsList::ConstructIons()
{
  //  Construct light ions
  G4IonConstructor pConstructor;
  pConstructor.ConstructParticle();  
}

void PhysicsList::ConstructProcess()
{
  AddTransportation();
  AddParameterisation();
  ConstructEM();
  if(IsOpticsON==1) ConstructOpticalPhysics();
  if(IsNuclearON==1) ConstructNuclearProcesses();
  

  ConstructGeneral();
  //Here you put the transportation for neutrons!
 // this->RegisterPhysics( new G4EmExtraPhysics(ver) );
}

void PhysicsList::AddTransportation()
{
  G4VUserPhysicsList::AddTransportation();
}

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4SynchrotronRadiation.hh"

#include "G4hIonisation.hh"
#include "G4Cerenkov.hh"          //M. Raggi 11/03/2016

//#include "G4EmExtraPhysics.hh"        //M. Raggi 17/07/2014
//#include "G4VModularPhysicsList.hh" //M. Raggi 17/07/2014
//#include "G4PhysicsListHelper.hh"   //M. Raggi 17/07/2014

// INTRODUCING NUCLEAR REACTIONS
#if G4MAJORVERSION == 9
#if  G4MINORVERSION == 6
#include "G4GammaNuclearReaction.hh"  //M. Raggi 17/07/2014
#endif
#else
#endif
 

#include "G4PhotoNuclearProcess.hh"   //M. Raggi 17/07/2014

#include "G4QGSModel.hh"
#include "G4ExcitedStringDecay.hh"
//#include "SetFragmentationModel.hh"
#include "G4TheoFSGenerator.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4QGSModel.hh"
#include "G4GammaParticipants.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"

#include "G4CascadeInterface.hh"
#include "G4ElectroVDNuclearModel.hh"

#include "G4PhotoNuclearProcess.hh"
#include "G4ElectronNuclearProcess.hh"
#include "G4PositronNuclearProcess.hh"

#include "G4NeutronInelasticProcess.hh"

////////////////////
// Optical Processes
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpWLS.hh"


void PhysicsList::ConstructOpticalPhysics() {
  G4cout<<"Registering optical physics "<<G4endl;
  fCerenkovProcess           = new G4Cerenkov("Cerenkov");
  fScintillationProcess      = new G4Scintillation("Scintillation");
  fAbsorptionProcess         = new G4OpAbsorption();
  fRayleighScatteringProcess = new G4OpRayleigh();
  fBoundaryProcess           = new G4OpBoundaryProcess();
  //  fWLSProcess 	             = new G4OpWLS();
  G4cout<<"dumping cherenkov "<<G4endl;
  fCerenkovProcess     -> DumpPhysicsTable();
  G4cout<<"dumping scintillaition "<<G4endl;
  fScintillationProcess-> DumpPhysicsTable();
  //  fAbsorptionProcess->DumpPhysicsTable();
  //  fRayleighScatteringProcess->DumpPhysicsTable();
  
  //SetVerbose(1);
  //fBoundaryProcess->SetVerboseLevel(1);
  
  fCerenkovProcess->SetMaxNumPhotonsPerStep(30);
  fCerenkovProcess->SetTrackSecondariesFirst(true);
  
  fScintillationProcess->SetScintillationYieldFactor(1.);
  fScintillationProcess->SetTrackSecondariesFirst(true);
  // Use Birks Correction in the Scintillation process
  
  //  G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
  //  fScintillationProcess->AddSaturation(emSaturation);
  
  // Removed because obsolete (Geant 9.6) RP
  // fBoundaryProcess->SetModel(themodel);
  
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (particleName == "opticalphoton") {
      G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
      pmanager->AddDiscreteProcess(fAbsorptionProcess);
      pmanager->AddDiscreteProcess(fRayleighScatteringProcess);
      pmanager->AddDiscreteProcess(fBoundaryProcess);
      //      pmanager->AddDiscreteProcess(fWLSProcess);
    }
    
    if (fScintillationProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(fScintillationProcess);
      pmanager->SetProcessOrderingToLast(fScintillationProcess, idxAtRest);
      pmanager->SetProcessOrderingToLast(fScintillationProcess, idxPostStep);
    }
    if (fCerenkovProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(fCerenkovProcess);
      pmanager->SetProcessOrdering(fCerenkovProcess,idxPostStep);
    }
  }
}

#include "G4CascadeInterface.hh"

#include "G4MuonVDNuclearModel.hh"
#include "G4ElectroVDNuclearModel.hh"

//CHIPS model
//#include "G4ElectroNuclearReaction.hh"


void PhysicsList::ConstructNuclearProcesses(){
  theParticleIterator->reset();
  if (NuclearBertini == 1){
    G4CascadeInterface* bertiniModel = new G4CascadeInterface();
  }
  
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "gamma") {
      //      Chiral Invariant Phase Space Model (CHIPS)
#if G4MAJORVERSION == 9
#if  G4MINORVERSION == 6

	// if(NuclearCHIPS == 1 ) {
	  G4GammaNuclearReaction* lowEGammaModel = new G4GammaNuclearReaction();
	  lowEGammaModel->SetMaxEnergy(3.*GeV);
	  lowEGammaModel->SetMinEnergy(1.*MeV);
	  G4PhotoNuclearProcess* thePhotoNuclearProcess = new G4PhotoNuclearProcess();
	  thePhotoNuclearProcess->RegisterMe(lowEGammaModel);
	  pmanager->AddDiscreteProcess(thePhotoNuclearProcess);
	// } else if (NuclearBertini == 1) {
	  // G4ProtonInelasticProcess* inelProcess = new G4ProtonInelasticProcess();
	  // inelProcess->RegisterMe(bertiniModel);
	  // processManager->AddDiscreteProcess(inelProcess);	
	// }
#endif
#else
#endif

    } else if (particleName == "e-" || particleName == "e+") {
      // if(NuclearCHIPS == 1 ) {
      // 	G4ElectroNuclearReaction *eNuclearReaction = new G4ElectroNuclearReaction();
      // 	eNuclearReaction->SetMinEnergy(1.*MeV);
      // 	eNuclearReaction->SetMaxEnergy(10.*GeV);
      // 	G4ElectronNuclearProcess* eNuclearProcess = new G4ElectronNuclearProcess();
      // 	eNuclearProcess->RegisterMe(eNuclearReaction);
      // 	pmanager->AddDiscreteProcess(eNuclearProcess);
      // }
      
    }

    

  }
    
  
}

void PhysicsList::ConstructEM()
{
  G4TheoFSGenerator * theModel;
  G4GeneratorPrecompoundInterface * theCascade;
  G4QGSModel< G4GammaParticipants > * theStringModel;
  G4QGSMFragmentation * theFragmentation;
  G4ExcitedStringDecay * theStringDecay;

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
     
    if (particleName == "gamma") {
    	// gamma
    	// Construct processes for gamma
    	pmanager->AddDiscreteProcess(new G4GammaConversion());
    	pmanager->AddDiscreteProcess(new G4ComptonScattering());
    	pmanager->AddDiscreteProcess(new G4PhotoElectricEffect());
    	if(IsNuclearON==1){
    		// // Photonuclear process for the photons
    		// G4GammaNuclearReaction* lowEGammaModel = new G4GammaNuclearReaction();
    		// lowEGammaModel->SetMaxEnergy(3.5*GeV);
    		// G4PhotoNuclearProcess* thePhotoNuclearProcess = new G4PhotoNuclearProcess();
    		// thePhotoNuclearProcess->RegisterMe(lowEGammaModel);
    		// pmanager->AddDiscreteProcess(thePhotoNuclearProcess);
    	}
    } else if (particleName == "e-") {
    //electron
      // Construct processes for electron
      G4VProcess* theeminusMultipleScattering = new G4eMultipleScattering();
      G4VProcess* theeminusIonisation = new G4eIonisation();
      G4VProcess* theeminusBremsstrahlung = new G4eBremsstrahlung();
      G4VProcess* theeminusSincrotron = new G4SynchrotronRadiation();
      // add processes
      pmanager->AddProcess(theeminusMultipleScattering);
      pmanager->AddProcess(theeminusIonisation);
      pmanager->AddProcess(theeminusBremsstrahlung);
      if(IsSincrotronON) pmanager->AddProcess(theeminusSincrotron);
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering, idxAlongStep,  1);
      pmanager->SetProcessOrdering(theeminusIonisation, idxAlongStep,  2);
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeminusMultipleScattering, idxPostStep, 1);
      pmanager->SetProcessOrdering(theeminusIonisation, idxPostStep, 2);
      pmanager->SetProcessOrdering(theeminusBremsstrahlung, idxPostStep, 3);
      if(IsSincrotronON) pmanager->SetProcessOrdering(theeminusSincrotron, idxPostStep, 4);

      //electron nuclear reactions M. Raggi 18/07/2014  il codice compila ma bomba!!
      if(IsNuclearON==1){
    	  //G4ElectronNuclearProcess* einelProc = new G4ElectronNuclearProcess();
    	  //pmanager->AddDiscreteProcess(einelProc);

//    	  G4ElectronNuclearProcess* theElectronNuclearProcess = new G4ElectronNuclearProcess;
//
//    	  G4ElectroVDNuclearModel* theElectroReaction = new G4ElectroVDNuclearModel;
//    	  G4CascadeInterface* theGammaReaction = new G4CascadeInterface;
//    	  theModel = new G4TheoFSGenerator;
////
//    	  theStringModel = new G4QGSModel< G4GammaParticipants >;
//    	  theStringDecay = new G4ExcitedStringDecay(theFragmentation=new G4QGSMFragmentation);
//    	  theStringModel->SetFragmentationModel(theStringDecay);
//    	  theCascade = new G4GeneratorPrecompoundInterface;
//    	  theModel->SetTransport(theCascade);
//    	  theModel->SetHighEnergyGenerator(theStringModel);
////
//    	  theElectronNuclearProcess->RegisterMe(theElectroReaction);
//    	  pmanager->AddDiscreteProcess(theElectronNuclearProcess);
//
//    	  G4ElectroVDNuclearModel* theElectronReaction = new G4ElectroVDNuclearModel;
//    	  G4ElectronNuclearReaction* theElectronReaction = new G4ElectronNuclearReaction;
    	  //G4ElectronNuclearProcess* theElectronNuclearProcess = new G4ElectronNuclearProcess;
    	  //G4ElectroVDNuclearModel* theElectronReaction = new G4ElectroVDNuclearModel;
    	  //pmanager->AddDiscreteProcess(theElectronNuclearProcess);
    	  }
    } else if (particleName == "e+") {
    //positron
      // Construct processes for positron
      G4VProcess* theeplusMultipleScattering = new G4eMultipleScattering();
      G4VProcess* theeplusIonisation         = new G4eIonisation();
      G4VProcess* theeplusBremsstrahlung     = new G4eBremsstrahlung();
      G4VProcess* theeplusAnnihilation       = new G4eplusAnnihilation();
      G4VProcess* theeplusSincrotron         = new G4SynchrotronRadiation();
      // add processes
      pmanager->AddProcess(theeplusMultipleScattering);
      pmanager->AddProcess(theeplusIonisation);
      pmanager->AddProcess(theeplusBremsstrahlung);
      pmanager->AddProcess(theeplusAnnihilation);
      if(IsSincrotronON) pmanager->AddProcess(theeplusSincrotron);
      // set ordering for AtRestDoIt
      pmanager->SetProcessOrderingToFirst(theeplusAnnihilation, idxAtRest);
      // set ordering for AlongStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxAlongStep,  1);
      pmanager->SetProcessOrdering(theeplusIonisation, idxAlongStep,  2);
      // set ordering for PostStepDoIt
      pmanager->SetProcessOrdering(theeplusMultipleScattering, idxPostStep, 1);
      pmanager->SetProcessOrdering(theeplusIonisation, idxPostStep, 2);
      pmanager->SetProcessOrdering(theeplusBremsstrahlung, idxPostStep, 3);
      pmanager->SetProcessOrdering(theeplusAnnihilation, idxPostStep, 4);
      if(IsSincrotronON) pmanager->SetProcessOrdering(theeplusSincrotron, idxPostStep, 5);

      //positron nuclear reactions  M. Raggi 18/07/2014
      if(IsNuclearON==1){
//    	  G4PositronNuclearProcess* pinelProc = new G4PositronNuclearProcess();
//    	  pmanager->AddDiscreteProcess(pinelProc);
//    	  G4ElectroVDNuclearModel* thePositronReaction = new G4ElectroVDNuclearModel;
//    	  pinelProc->RegisterMe(thePositronReaction);
//    	  pmanager->AddDiscreteProcess(thePositronNuclearProcess);
      }

    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
    //muon  
     // Construct processes for muon+
     G4VProcess* aMultipleScattering = new G4MuMultipleScattering();
     G4VProcess* aBremsstrahlung = new G4MuBremsstrahlung();
     G4VProcess* aPairProduction = new G4MuPairProduction();
     G4VProcess* anIonisation = new G4MuIonisation();
      // add processes
     pmanager->AddProcess(anIonisation);
     pmanager->AddProcess(aMultipleScattering);
     pmanager->AddProcess(aBremsstrahlung);
     pmanager->AddProcess(aPairProduction);
     // set ordering for AlongStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
     pmanager->SetProcessOrdering(anIonisation, idxAlongStep,  2);
     // set ordering for PostStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
     pmanager->SetProcessOrdering(anIonisation, idxPostStep, 2);
     pmanager->SetProcessOrdering(aBremsstrahlung, idxPostStep, 3);
     pmanager->SetProcessOrdering(aPairProduction, idxPostStep, 4);

//   Adding physics of neutrons by hands M. Raggi 1 August2014
    } else if(particleName == "neutron"   && IsNuclearON ) {
      //G4VProcess* G4neutroninelasticprocess = new  G4NeutronInelasticProcess();
      //pmanager->AddProcess(G4neutroninelasticprocess);
   } else if ((!particle->IsShortLived()) &&
              (particle->GetPDGCharge() != 0.0) && 
              (particle->GetParticleName() != "chargedgeantino")) {
     // all others charged particles except geantino
     G4VProcess* aMultipleScattering = new G4hMultipleScattering();
     G4VProcess* anIonisation = new G4hIonisation();
     // add processes
     pmanager->AddProcess(anIonisation);
     pmanager->AddProcess(aMultipleScattering);
     // set ordering for AlongStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxAlongStep,  1);
     pmanager->SetProcessOrdering(anIonisation, idxAlongStep,  2);
     // set ordering for PostStepDoIt
     pmanager->SetProcessOrdering(aMultipleScattering, idxPostStep, 1);
     pmanager->SetProcessOrdering(anIonisation, idxPostStep, 2);
    }
  }
}

#include "G4Decay.hh"
void PhysicsList::ConstructGeneral()
{
  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }  
}

void PhysicsList::AddParameterisation()
{
  // -- Fast simulation manager process for "mass geometry":
  G4FastSimulationManagerProcess* fastSimProcess_massGeom     = new G4FastSimulationManagerProcess("G4FSMP_massGeom");
  // -- Fast simulation manager process for "parallel geometry":
  //  G4FastSimulationManagerProcess* fastSimProcess_parallelGeom = new G4FastSimulationManagerProcess("G4FSMP_parallelGeom", "pionGhostWorld");
  theParticleIterator->reset();
  while( (*theParticleIterator)() )
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();
      // -- For the mass geometry, G4FSMP is a PostStep process, ordering does not matter:
      if (particle->GetParticleName() == "e+"  || 
          particle->GetParticleName() == "e-"  ||
          particle->GetParticleName() == "gamma")  pmanager->AddDiscreteProcess(fastSimProcess_massGeom);
      // -- For the parallel geometry, G4FSMP is an Along+PostStep process, and ordering matters:
    //  if (particle->GetParticleName() == "pi+" || 
    //      particle->GetParticleName() == "pi-") 
    //    {
    //      pmanager->AddProcess(fastSimProcess_parallelGeom);
    //      pmanager->SetProcessOrdering(fastSimProcess_parallelGeom, idxAlongStep, 1);
    //      pmanager->SetProcessOrdering(fastSimProcess_parallelGeom, idxPostStep);
    //    }
    }
}

void PhysicsList::SetCuts()
{
  if (verboseLevel >1){
    G4cout << "PhysicsList::SetCuts:";
  }  
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
  SetCutsWithDefault();   
}


