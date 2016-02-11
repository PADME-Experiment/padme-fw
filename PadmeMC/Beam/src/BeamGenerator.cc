// BeamGenerator.cc
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "BeamGenerator.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomDirection.hh"
#include "G4Poisson.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"

BeamGenerator::BeamGenerator(DetectorConstruction* myDC)
 :fDetector(myDC)
{

  fEvent = 0;

  // Create default particle gun
  fParticleGun = new G4ParticleGun();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamGenerator::~BeamGenerator()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeamGenerator::GenerateBeam(G4Event* anEvent)
{ 

  fEvent = anEvent;

  BeamParameters* bpar = BeamParameters::GetInstance();

  //  std::cout << "============================ Generate Primaries" << std::endl;
  //evt->ClearEvent();
  static int nev;
  nev++;
  if(nev%10000 == 0) std::cout << "Generating event number " << nev << std::endl;
  
  G4int nTotPositrons = bpar->GetNPositronsPerBunch();
  if (bpar->NPositronsPerBunchApplySpread()) {
    nTotPositrons = G4Poisson(nTotPositrons);
  }
  G4int nUbosonDecays = bpar->GetNUbosonDecaysPerBunch();
  G4int nThreePhotonDecays = bpar->GetNThreePhotonDecaysPerBunch();
  G4int nPositrons = nTotPositrons-nUbosonDecays-nThreePhotonDecays;
  G4cout << "PGA - Ntot " << nTotPositrons << " Npos " << nPositrons << " nUboson " << nUbosonDecays << " n3gamma " << nThreePhotonDecays << G4endl;

  //********************
  //U Boson generator MC
  //********************

  for(int ib = 0; ib < nUbosonDecays; ib++) {

    // Generate primary e+ which will decay to Uboson+gamma and store it in particleGun
    SetPositronBeamParameters();

    // Generate Uboson+gamma final state
    CreateFinalStateUboson();
	
  }
  
  //*********************
  //Three photon events
  //*********************
  for(int iggg = 0; iggg < nThreePhotonDecays; iggg++) {

    // Generate primary e+ which will decay to three gammas and store it in particleGun
    SetPositronBeamParameters();

    // Generate gamma+gamma+gamma final state
    CreateFinalStateThreeGamma();

  }
  
  //******************************************************
  //General BG generator particles on the target per event 
  //******************************************************
  for(int ip = 0; ip < nPositrons; ip++) { 

    // Generate primary e+
    SetPositronBeamParameters();

    // Add primary e+ to event kinematics
    fParticleGun->GeneratePrimaryVertex(fEvent);

  }

}

int BeamGenerator::SetPositronBeamParameters()
{

  BeamParameters* bpar = BeamParameters::GetInstance();

  // Generate a primary e+ with right time/energy/position/direction and save it in particleGun

  fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("e+"));
  fParticleGun->SetNumberOfParticles(1);
  
  G4double parTime = 0.;
  if ( bpar->BeamApplyBunchStructure() ) {
    G4double bunchLen = bpar->GetBunchTimeLength();
    G4double ubunchLen = bpar->GetMicroBunchTimeLength();
    G4double ubunchDly = bpar->GetMicroBunchTimeDelay();
    G4double nubunch = bunchLen/(ubunchDly+ubunchLen);
    G4int ubunchNow = int(G4UniformRand()*nubunch);
    parTime = ubunchNow*(ubunchDly+ubunchLen)+G4UniformRand()*ubunchLen;
  }
  fParticleGun->SetParticleTime(parTime);
  G4cout << "PGA - Positron time " << parTime << G4endl;
  
  // Set beam momentum
  G4double beamP = bpar->GetBeamMomentum();
  if ( bpar->BeamMomentumApplySpread() ) beamP = G4RandGauss::shoot(beamP,bpar->GetBeamMomentumSpread());
  fParticleGun->SetParticleMomentum(beamP);
  G4cout << "PGA - Positron momentum " << beamP << G4endl;

  // All positrons are generated 1um before the front face of the target
  G4double xPos = bpar->GetBeamCenterPosX();
  G4double yPos = bpar->GetBeamCenterPosY();
  G4double zPos = fDetector->GetTargetFrontFaceZ()-1.*um;
  if ( bpar->BeamCenterPosApplySpread() ) {
    xPos = G4RandGauss::shoot(xPos,bpar->GetBeamCenterPosXSpread());
    yPos = G4RandGauss::shoot(yPos,bpar->GetBeamCenterPosYSpread());
    // Here we need something to limit xPos/yPos to physical values
  }
  fParticleGun->SetParticlePosition(G4ThreeVector(xPos,yPos,zPos));
  G4cout << "PGA - Positron position " << xPos << " " << yPos << " "<< zPos << G4endl;

  // Positron direction
  G4double pX = bpar->GetBeamDirection().x();
  G4double pY = bpar->GetBeamDirection().y();
  G4double pZ = bpar->GetBeamDirection().z();
  if ( bpar->BeamApplyEmittance() ) { //Still to be tested
    pX = G4RandGauss::shoot(pX,bpar->GetBeamEmittanceX());	
    pY = G4RandGauss::shoot(pY,bpar->GetBeamEmittanceY());
  }
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(pX,pY,pZ).unit());
  G4cout << "PGA - Positron direction " << pX << " " << pY << " "<< pZ << G4endl;
  
  return 1;
}

void BeamGenerator::CreateFinalStateUboson()
{

  // Get mass of the Uboson
  G4double ubosonM = BeamParameters::GetInstance()->GetUbosonMass();

  //Define the process: X->Uboson+gamma

  // Choose random decay point along e+ path within Target
  // N.B. as the target is very thin, we assume that the decay point x,y coordinates do not vary along the e+ path
  G4double Dx = fParticleGun->GetParticlePosition().x();
  G4double Dy = fParticleGun->GetParticlePosition().y();
  G4double Dz = fDetector->GetTargetFrontFaceZ()+G4UniformRand()*fDetector->GetTargetThickness();
  G4double Dt = fParticleGun->GetParticleTime()+Dz/c_light;
  G4cout << "PGA - Vtx " << Dx << " " << Dy << " " << Dz << " T " << Dt << G4endl;

  // e+ is stored in particleGun
  G4double pM = fParticleGun->GetParticleDefinition()->GetPDGMass(); // Mass
  G4double pP = fParticleGun->GetParticleMomentum(); // Momentum
  G4double pp[4];
  pp[0] = fParticleGun->GetParticleEnergy()+pM; // Total energy (kinetic+mass)
  pp[1] = pP*fParticleGun->GetParticleMomentumDirection().x(); // Momentum x
  pp[2] = pP*fParticleGun->GetParticleMomentumDirection().y(); // Momentum y
  pp[3] = pP*fParticleGun->GetParticleMomentumDirection().z(); // Momentum z
  G4cout << "PGA - P(e+) " << pp[0] << " " << pp[1] << " " << pp[2] << " " << pp[3] << G4endl;

  // e- is at rest
  G4double ep[4];
  ep[0] = G4ParticleTable::GetParticleTable()->FindParticle("e-")->GetPDGMass();
  ep[1]=ep[2]=ep[3]=0.*MeV;
  G4cout << "PGA - P(e-) " << ep[0] << " " << ep[1] << " " << ep[2] << " " << ep[3] << G4endl;

  // Compute four-momentum and mass of the virtual state e+e-
  G4double vsp[4];
  for(G4int i=0; i<4; i++) { vsp[i] = pp[i]+ep[i]; }
  G4double vsM = sqrt( vsp[0]*vsp[0]-vsp[1]*vsp[1]-vsp[2]*vsp[2]-vsp[3]*vsp[3] );
  G4cout << "PGA - P(VS) " << vsp[0] << " " << vsp[1] << " " << vsp[2] << " " << vsp[3]
	 << " M(VS) " << vsM << G4endl;

  // === Compute Uboson+gamma final state in the CM ===

  //Generate a random unit vector for the Uboson decay direction
  G4ThreeVector dir = G4RandomDirection();

  // Compute Uboson four-momentum
  G4double UpCM[4];
  UpCM[0] = (ubosonM*ubosonM+vsM*vsM)/(2.*vsM); // Uboson total energy
  G4double UP = sqrt(UpCM[0]*UpCM[0]-ubosonM*ubosonM); // Uboson momentum
  UpCM[1] = UP*dir.x();
  UpCM[2] = UP*dir.y();
  UpCM[3] = UP*dir.z();
  G4cout << "PGA - PCM(Uboson) " << UpCM[0] << " " << UpCM[1] << " " << UpCM[2] << " " << UpCM[3] << G4endl;

  // Compute gamma four-momentum
  G4double gpCM[4];
  gpCM[0] = (-ubosonM*ubosonM+vsM*vsM)/(2.*vsM); // Gamma total energy
  gpCM[1] = -gpCM[0]*dir.x();
  gpCM[2] = -gpCM[0]*dir.y();
  gpCM[3] = -gpCM[0]*dir.z();
  G4cout << "PGA - PCM(gamma) " << gpCM[0] << " " << gpCM[1] << " " << gpCM[2] << " " << gpCM[3] << G4endl;

  // === Boost UBoson+gamma final state to the lab ===

  G4double Up[4];
  Up[0] = ( UpCM[0]*vsp[0]+UpCM[1]*vsp[1]+UpCM[2]*vsp[2]+UpCM[3]*vsp[3] )/vsM;
  G4double UC = (UpCM[0]+Up[0])/(vsM+vsp[0]);
  Up[1] = UpCM[1]+UC*vsp[1];
  Up[2] = UpCM[2]+UC*vsp[2];
  Up[3] = UpCM[3]+UC*vsp[3];
  G4cout << "PGA - P(Uboson) " << Up[0] << " " << Up[1] << " " << Up[2] << " " << UpCM[3] << G4endl;

  G4double gp[4];
  gp[0] = ( gpCM[0]*vsp[0]+gpCM[1]*vsp[1]+gpCM[2]*vsp[2]+gpCM[3]*vsp[3] )/vsM;
  G4double gC = (gpCM[0]+gp[0])/(vsM+vsp[0]);
  gp[1] = gpCM[1]+gC*vsp[1];
  gp[2] = gpCM[2]+gC*vsp[2];
  gp[3] = gpCM[3]+gC*vsp[3];
  G4cout << "PGA - P(gamma) " << gp[0] << " " << gp[1] << " " << gp[2] << " " << gp[3] << G4endl;

  // Create gamma primary particle with generated four-momentum
  G4PrimaryParticle* gamma = new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("gamma"),
						   gp[1],gp[2],gp[3],gp[0]);

  // Create primary vertex at generated position/time
  G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(Dx,Dy,Dz),Dt);
  vtx->SetPrimary(gamma);

  // Add primary vertex to event
  fEvent->AddPrimaryVertex(vtx);
  
}

void BeamGenerator::CreateFinalStateThreeGamma()
{
}
