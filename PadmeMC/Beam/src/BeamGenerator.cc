// BeamGenerator.cc
// --------------------------------------------------------------
// History:
//
// 2016-02-11 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//
// --------------------------------------------------------------

#include "BeamGenerator.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomDirection.hh"
#include "G4Poisson.hh"
#include "Randomize.hh"

#include "BeamMessenger.hh"
#include "DetectorConstruction.hh"

BeamGenerator::BeamGenerator(DetectorConstruction* myDC)
 :fDetector(myDC)
{

  fEvent = 0;

  // Connect to BeamMessenger
  fBeamMessenger = new BeamMessenger(this);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamGenerator::~BeamGenerator()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeamGenerator::GenerateBeam(G4Event* anEvent)
{ 

  fEvent = anEvent;

  BeamParameters* bpar = BeamParameters::GetInstance();

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
  G4cout << "BeamGenerator - Ntot " << nTotPositrons << " Npos " << nPositrons << " nUboson " << nUbosonDecays << " n3gamma " << nThreePhotonDecays << G4endl;

  //********************
  //U Boson generator MC
  //********************

  for(int ib = 0; ib < nUbosonDecays; ib++) {

    // Generate primary e+ which will decay to Uboson+gamma
    GeneratePrimaryPositron();

    // Generate Uboson+gamma final state
    CreateFinalStateUboson();
	
  }
  
  //*********************
  //Three photon events
  //*********************
  for(int iggg = 0; iggg < nThreePhotonDecays; iggg++) {

    // Generate primary e+ which will decay to three gammas
    GeneratePrimaryPositron();

    // Generate gamma+gamma+gamma final state
    CreateFinalStateThreeGamma();

  }
  
  //******************************************************
  //General BG generator particles on the target per event 
  //******************************************************
  for(int ip = 0; ip < nPositrons; ip++) { 

    // Generate primary e+
    GeneratePrimaryPositron();
    //G4cout << "Positron " << fPositron.t << " " << fPositron.pos << " " << fPositron.P << " " << fPositron.dir << G4endl;

    // Create e+ primary particle with generated four-momentum
    G4PrimaryParticle* positron = new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("e+"),
							fPositron.p.x(),fPositron.p.y(),fPositron.p.z(),fPositron.E);

    // Create primary vertex at generated position/time
    G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(fPositron.pos.x(),fPositron.pos.y(),fPositron.pos.z()),fPositron.t);
    vtx->SetPrimary(positron);

    // Add primary vertex to event
    fEvent->AddPrimaryVertex(vtx);

  }

}

void BeamGenerator::GeneratePrimaryPositron()
{

  BeamParameters* bpar = BeamParameters::GetInstance();

  // Generate a primary e+ with right time/energy/position/direction

  // Get mass of positron
  fPositron.m = G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGMass(); // Mass
  G4cout << "BeamGenerator - Positron mass " << fPositron.m << G4endl;

  // Assign a time using bunch time structure (if required)
  G4double parTime = 0.;
  if ( bpar->BeamApplyBunchStructure() ) {
    G4double bunchLen = bpar->GetBunchTimeLength();
    G4double ubunchLen = bpar->GetMicroBunchTimeLength();
    G4double ubunchDly = bpar->GetMicroBunchTimeDelay();
    G4double nubunch = bunchLen/(ubunchDly+ubunchLen);
    G4int ubunchNow = int(G4UniformRand()*nubunch);
    parTime = ubunchNow*(ubunchDly+ubunchLen)+G4UniformRand()*ubunchLen;
  }
  fPositron.t = parTime;
  G4cout << "BeamGenerator - Positron time " << fPositron.t << G4endl;

  // All positrons are generated 1um before the front face of the target
  G4double xPos = bpar->GetBeamCenterPosX();
  G4double yPos = bpar->GetBeamCenterPosY();
  G4double zPos = fDetector->GetTargetFrontFaceZ()-1.*um;
  if ( bpar->BeamCenterPosApplySpread() ) {
    xPos = G4RandGauss::shoot(xPos,bpar->GetBeamCenterPosXSpread());
    yPos = G4RandGauss::shoot(yPos,bpar->GetBeamCenterPosYSpread());
    // Here we need something to limit xPos/yPos to physical values
  }
  fPositron.pos = G4ThreeVector(xPos,yPos,zPos);
  G4cout << "BeamGenerator - Positron position " << fPositron.pos << G4endl;
  
  // Set positron momentum
  fPositron.P = bpar->GetBeamMomentum();
  if ( bpar->BeamMomentumApplySpread() ) fPositron.P = G4RandGauss::shoot(fPositron.P,bpar->GetBeamMomentumSpread());
  G4cout << "BeamGenerator - Positron momentum " << fPositron.P << G4endl;

  // Set positron direction
  G4double pX = bpar->GetBeamDirection().x();
  G4double pY = bpar->GetBeamDirection().y();
  G4double pZ = bpar->GetBeamDirection().z();
  if ( bpar->BeamApplyEmittance() ) { //Still to be tested
    pX = G4RandGauss::shoot(pX,bpar->GetBeamEmittanceX());	
    pY = G4RandGauss::shoot(pY,bpar->GetBeamEmittanceY());
  }
  fPositron.dir = G4ThreeVector(pX,pY,pZ).unit();
  G4cout << "BeamGenerator - Positron direction " << fPositron.dir << G4endl;

  // Set positron total energy
  fPositron.E = sqrt(fPositron.P*fPositron.P+fPositron.m*fPositron.m);
  G4cout << "BeamGenerator - Positron energy " << fPositron.E << " " << fPositron.E-fPositron.P << G4endl;

  // Set positron momentum vector
  fPositron.p = G4ThreeVector(fPositron.P*fPositron.dir.x(),
			      fPositron.P*fPositron.dir.y(),
			      fPositron.P*fPositron.dir.z());
  G4cout << "BeamGenerator - Positron momentum vector " << fPositron.p << G4endl;

}

//void BeamGenerator::CreateFinalStateUboson(G4ParticleGun* pGun)
void BeamGenerator::CreateFinalStateUboson()
{

  // Get mass of the Uboson
  G4double ubosonM = BeamParameters::GetInstance()->GetUbosonMass();

  //Define the process: X->Uboson+gamma

  // Choose random decay point along e+ path within Target
  // N.B. as the target is very thin, we assume that the decay point x,y coordinates and time
  // do not vary along the e+ path (can be fixed if needed)
  G4double Dx = fPositron.pos.x();
  G4double Dy = fPositron.pos.y();
  G4double Dz = fDetector->GetTargetFrontFaceZ()+G4UniformRand()*fDetector->GetTargetThickness();
  G4double Dt = fPositron.t; // Should take into account time travelled inside Target
  G4cout << "BeamGenerator - Vtx " << Dx << " " << Dy << " " << Dz << " T " << Dt << G4endl;

  // Get e+ four-momentum
  G4double pp[4];
  pp[0] = fPositron.E;     // Total energy
  pp[1] = fPositron.p.x(); // Momentum x
  pp[2] = fPositron.p.y(); // Momentum y
  pp[3] = fPositron.p.z(); // Momentum z
  G4cout << "BeamGenerator - P(e+) " << pp[0] << " " << pp[1] << " " << pp[2] << " " << pp[3] << G4endl;

  // e- is at rest
  G4double ep[4];
  ep[0] = G4ParticleTable::GetParticleTable()->FindParticle("e-")->GetPDGMass();
  ep[1]=ep[2]=ep[3]=0.*MeV;
  G4cout << "BeamGenerator - P(e-) " << ep[0] << " " << ep[1] << " " << ep[2] << " " << ep[3] << G4endl;

  // Compute four-momentum and mass of the virtual state e+e-
  G4double vsp[4];
  for(G4int i=0; i<4; i++) { vsp[i] = pp[i]+ep[i]; }
  G4double vsM = sqrt( vsp[0]*vsp[0]-vsp[1]*vsp[1]-vsp[2]*vsp[2]-vsp[3]*vsp[3] );
  G4cout << "BeamGenerator - P(VS) " << vsp[0] << " " << vsp[1] << " " << vsp[2] << " " << vsp[3]
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
  G4cout << "BeamGenerator - PCM(Uboson) " << UpCM[0] << " " << UpCM[1] << " " << UpCM[2] << " " << UpCM[3] << G4endl;

  // Compute gamma four-momentum
  G4double gpCM[4];
  gpCM[0] = (-ubosonM*ubosonM+vsM*vsM)/(2.*vsM); // Gamma total energy
  gpCM[1] = -gpCM[0]*dir.x();
  gpCM[2] = -gpCM[0]*dir.y();
  gpCM[3] = -gpCM[0]*dir.z();
  G4cout << "BeamGenerator - PCM(gamma) " << gpCM[0] << " " << gpCM[1] << " " << gpCM[2] << " " << gpCM[3] << G4endl;

  // === Boost UBoson+gamma final state to the lab ===

  G4double Up[4];
  Up[0] = ( UpCM[0]*vsp[0]+UpCM[1]*vsp[1]+UpCM[2]*vsp[2]+UpCM[3]*vsp[3] )/vsM;
  G4double UC = (UpCM[0]+Up[0])/(vsM+vsp[0]);
  Up[1] = UpCM[1]+UC*vsp[1];
  Up[2] = UpCM[2]+UC*vsp[2];
  Up[3] = UpCM[3]+UC*vsp[3];
  G4cout << "BeamGenerator - P(Uboson) " << Up[0] << " " << Up[1] << " " << Up[2] << " " << Up[3] << G4endl;

  G4double gp[4];
  gp[0] = ( gpCM[0]*vsp[0]+gpCM[1]*vsp[1]+gpCM[2]*vsp[2]+gpCM[3]*vsp[3] )/vsM;
  G4double gC = (gpCM[0]+gp[0])/(vsM+vsp[0]);
  gp[1] = gpCM[1]+gC*vsp[1];
  gp[2] = gpCM[2]+gC*vsp[2];
  gp[3] = gpCM[3]+gC*vsp[3];
  G4cout << "BeamGenerator - P(gamma) " << gp[0] << " " << gp[1] << " " << gp[2] << " " << gp[3] << G4endl;

  // Create UBoson primary particle with generated four-momentum (use geantino)
  G4PrimaryParticle* uboson = new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("geantino"),
						   Up[1],Up[2],Up[3],Up[0]);

  // Create gamma primary particle with generated four-momentum
  G4PrimaryParticle* gamma = new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("gamma"),
						   gp[1],gp[2],gp[3],gp[0]);

  // Create primary vertex at generated position/time
  G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(Dx,Dy,Dz),Dt);
  vtx->SetPrimary(uboson);
  vtx->SetPrimary(gamma);

  // Add primary vertex to event
  fEvent->AddPrimaryVertex(vtx);
  
}

void BeamGenerator::CreateFinalStateThreeGamma()
{

  static G4int iline = 0;

  // Get file with list of three-gamma events kinematics
  G4String file3g = BeamParameters::GetInstance()->GetThreePhotonDecaysFilename();

  std::ifstream infile;
  std::string Line = "";
  infile.open(file3g.data());

  G4int il=0;
  while (!infile.eof() && il <= iline) {
     getline(infile,Line);
     il++;
  }
  infile.close();

  // If we did not reach EOF, generate 3 photons
  if (il == iline + 1) {
 
    // Choose random decay point along e+ path within Target
    // N.B. as the target is very thin, we assume that the decay point x,y coordinates and time
    // do not vary along the e+ path (can be fixed if needed)
    G4double Dx = fPositron.pos.x();
    G4double Dy = fPositron.pos.y();
    G4double Dz = fDetector->GetTargetFrontFaceZ()+G4UniformRand()*fDetector->GetTargetThickness();
    G4double Dt = fPositron.t; // Should take into account time travelled inside Target
    G4cout << "BeamGenerator - Vtx " << Dx << " " << Dy << " " << Dz << " T " << Dt << G4endl;
   
    // Create primary vertex at decay point
    G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(Dx,Dy,Dz),Dt);

    // Decode input line
    std::istringstream iss(Line);

    // Skip first three fields in the line
    G4int it;
    G4double dt1,dt2;
    iss >> it >> dt1 >> dt2;

    // Loop over the three photons
    G4double p[4]; // Vector to store four-momentum of the gamma
    for(G4int j=0; j<3; j++) {

      iss >> p[1] >> p[2] >> p[3]; // Get gamma momentum
      for(G4int k=1; k<=3; k++) { p[k] *= GeV; } // Values are given in GeV
      p[0] = sqrt(p[1]*p[1]+p[2]*p[2]+p[3]*p[3]); // Compute total energy of the gamma
      G4cout << "BeamGenerator - P(gamma) " << j << " " << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << G4endl;

      // Need to add the rotation of the 3 gammas system along the direction of the primary positron (!!!)

      // Create gamma primary particle with generated four-momentum
      G4PrimaryParticle* gamma = new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("gamma"),
						       p[1],p[2],p[3],p[0]);
      vtx->SetPrimary(gamma);

    }

    // Add primary vertex to event
    fEvent->AddPrimaryVertex(vtx);

    // Skip to next line
    iline++;

  } else {
      G4cout << "BeamGenerator - WARNING - Reached end of three-gamma decays input file" << G4endl;
  }

}
