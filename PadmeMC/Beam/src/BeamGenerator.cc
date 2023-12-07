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
//#include "TRandom3.h"

#include "BeamMessenger.hh"
#include "HistoManager.hh"
#include "DetectorConstruction.hh"
#include "MCTruthManager.hh"

BeamGenerator::BeamGenerator(DetectorConstruction* myDC)
 :fDetector(myDC)
{

  fEvent = 0;

  // Initialize calibration run to some sensible values
  //fCalibrationRun  = false;
  //fCalibRunEnergy  = 50.*MeV;
  //fCalibRunCenterX = 14.*cm;
  //fCalibRunCenterY = 14.*cm;
  //fCalibRunRadius  =  1.*cm;

  // Default Z position of beam is 1 um before the target.
  // Can be changed with /beam/position_z datacard
  //BeamParameters* bpar = BeamParameters::GetInstance();
  //bpar->SetBeamCenterPosZ(fDetector->GetTargetFrontFaceZ()-1.*um);

  // Connect to BeamMessenger
  fBeamMessenger = new BeamMessenger(this);

  // Connect to Histogram manager
  fHistoManager = HistoManager::GetInstance();

  // Connect to MCTruth manager
  fMCTruthMgr = MCTruthManager::GetInstance();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamGenerator::~BeamGenerator()
{
  delete fBeamMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeamGenerator::GenerateBeam(G4Event* anEvent)
{ 

  // Reset MCTruth event
  if (MCTruthManager::GetInstance()->IsEnabled()) MCTruthManager::GetInstance()->Clear();

  fEvent = anEvent;

  BeamParameters* bpar = BeamParameters::GetInstance();

  static int nev;
  nev++;
  if(nev%10000 == 0) std::cout << "Generating event number " << nev << std::endl;

  // Special calibration run
  if ( bpar->CalibrationRun() ) {
    // Origin of calibration beam is on back face of Target
    //bpar->SetBeamOriginPosZ(fDetector->GetTargetFrontFaceZ()+fDetector->GetTargetThickness());
    bpar->SetBeamCenterPosZ(fDetector->GetTargetFrontFaceZ()+fDetector->GetTargetThickness());
    GenerateCalibrationParticle();
    return;
  }

  // Main positron beam origin is set to 1 um before the front face of the Target
  //bpar->SetBeamOriginPosZ(fDetector->GetTargetFrontFaceZ()-1.*um);

  G4int nTotPositrons = bpar->GetNPositronsPerBunch();
  if (bpar->NPositronsPerBunchApplySpread()) {
    nTotPositrons = G4Poisson(nTotPositrons);
  }

  G4int nUbosonDecays = bpar->GetNUbosonDecaysPerBunch();
  G4int nThreePhotonDecays = bpar->GetNThreePhotonDecaysPerBunch();
  G4int nTwoPhotonDecays = bpar->GetNTwoPhotonDecaysPerBunch();
  G4int nBhaBha          = bpar->GetNBhaBhaPerBunch(); //MR 20/05/2021
  G4int nPositrons = nTotPositrons-nUbosonDecays-nTwoPhotonDecays -nThreePhotonDecays - nBhaBha;
  if (nPositrons<0) {
    G4cout << "BeamGenerator - WARNING - Negative number of primary positrons in event. Please check your settings" << G4endl;
    G4cout << "- Ntot " << nTotPositrons << " Npos " << nPositrons << " nUboson " << nUbosonDecays << " n3gamma " << nThreePhotonDecays<< " n2gamma " << nTwoPhotonDecays <<" NBhaBha "<<nBhaBha<<G4endl;
    nPositrons = 0;
    exit(1);
  }
  G4double decayLength = bpar->GetDecayLength();

  //********************
  //U Boson generator MC
  //********************

  for(int ib = 0; ib < nUbosonDecays; ib++) {
    // Generate primary e+ in front of Target which will decay to Uboson+gamma
    //GeneratePrimaryPositron();
    GenerateTargetPositron();

    // Generate Uboson+gamma final state
    CreateFinalStateUboson();
  }
  
  //*********************
  //Three photon events
  //*********************
  for(int iggg = 0; iggg < nThreePhotonDecays; iggg++) {
    // Generate primary e+ in front of Target which will decay to three gamma
    //GeneratePrimaryPositron();
    GenerateTargetPositron();
    // Generate gamma+gamma+gamma final state
    CreateFinalStateThreeGamma(decayLength);
  }
    
  //*********************
  //Two photon events
  //*********************
  for(int iggg = 0; iggg < nTwoPhotonDecays; iggg++) {
    // Generate primary e+ in front of Target which will decay to two gamma
    //GeneratePrimaryPositron();
    GenerateTargetPositron();
    // Generate gamma+gamma final state
    CreateFinalStateTwoGamma(decayLength);
  }

  //***********************************************
  // Generate BhaBha scattering  M. Raggi 20/05/2021
  //************************************************
  //  std::cout<<" calling BhaBha generator" <<nBhaBha<<std::endl;
  for(int iee = 0; iee<nBhaBha; iee++) {
    // std::cout<<" calling BhaBha generator" <<std::endl;
    // Generate primary e+ which will generate ee pairs
    GenerateTargetPositron();
    // Generate e+ e- final state
    CreateFinalStateBhaBha(decayLength);
  }

  //******************************************************
  //General BG generator particles on the target per event 
  //******************************************************
  fHistoManager->FillHisto(6,nPositrons);
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

    fHistoManager->FillHisto2(36,fPositron.pos.x(),fPositron.pos.y(),1.);
    fHistoManager->FillHisto(2,fPositron.E);
    fHistoManager->FillHisto(3,fPositron.t);
    fHistoManager->FillHisto(4,fPositron.t);
    fHistoManager->FillHisto(5,GetGammaAngle(G4ThreeVector(fPositron.p.x(),fPositron.p.y(),fPositron.p.z()),G4ThreeVector(0.,0.,1.)));
    // G4cout<<"ddd "<<GetGammaAngle(G4ThreeVector(fPositron.p.x(),fPositron.p.y(),fPositron.p.z()),G4ThreeVector(0.,0.,1.))<<G4endl;
    //    G4cout<<"Beam position "<<G4ThreeVector(fPositron.pos.x(),fPositron.pos.y(),fPositron.pos.z())<<G4endl;
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
  //  G4cout << "BeamGenerator - Positron mass " << fPositron.m << G4endl;

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
  //  G4cout << "BeamGenerator - Positron time " << fPositron.t << G4endl;

  // Set positron momentum
  fPositron.P = bpar->GetBeamMomentum();
  if ( bpar->BeamMomentumApplySpread() ) fPositron.P = G4RandGauss::shoot(fPositron.P,bpar->GetBeamMomentumSpread());
  //  G4cout << "BeamGenerator - Positron momentum " << fPositron.P << G4endl;

  // Define position and direction for center of beam
  //G4ThreeVector beam_pos = G4ThreeVector(bpar->GetBeamCenterPosX(),bpar->GetBeamCenterPosY(),bpar->GetBeamOriginPosZ());
  G4ThreeVector beam_pos = G4ThreeVector(bpar->GetBeamCenterPosX(),bpar->GetBeamCenterPosY(),bpar->GetBeamCenterPosZ());
  G4ThreeVector beam_dir = bpar->GetBeamDirection();
  beam_dir = beam_dir.unit();

  // Generate starting position of the positrons taking into account beam position and direction
  G4ThreeVector part_pos;
  if ( bpar->BeamCenterPosApplySpread() ) {
    // Generate initial particle position assuming beam generates at origin and is directed along Z
    G4double xPos = G4RandGauss::shoot(0.,bpar->GetBeamCenterPosXSpread());
    G4double yPos = G4RandGauss::shoot(0.,bpar->GetBeamCenterPosYSpread());
    G4double zPos = 0.;
    part_pos = G4ThreeVector(xPos,yPos,zPos);
    // Rotate initial position to the correct beam direction
    part_pos.rotateUz(beam_dir);
    // Finally displace to the real beam origin
    part_pos += beam_pos;    
  } else {
    // All particles originate at center of beam (no spread)
    part_pos = beam_pos;
  }
  fPositron.pos = part_pos;
  //  G4cout << "BeamGenerator - Beam postion " << beam_pos << " Positron position " << fPositron.pos << G4endl;
  
  // Add emittance around direction of beam axis
  G4ThreeVector part_dir;
  if ( bpar->BeamApplyEmittance() ) {

    // Extract theta and phi according to emittance
    // Here we could introduce a correlation with the particle position wrt the beam axis

    // Phi is uniform around the beam direction
    G4double phi   = twopi*G4UniformRand();

    // Theta is gaussian with sigma from a phi-based combination of emittance along X and Y
    G4double sigma_theta = bpar->GetBeamEmittanceX()*cos(phi)+bpar->GetBeamEmittanceX()*sin(phi);
    G4double theta = G4RandGauss::shoot(0.,sigma_theta);

    // Compute particle direction assuming beam is directed along Z (default direction)
    G4double pX = sin(theta)*cos(phi);
    G4double pY = sin(theta)*sin(phi);
    G4double pZ = cos(theta);
    part_dir = G4ThreeVector(pX,pY,pZ);

    // Now rotate to the real beam direction
    part_dir.rotateUz(beam_dir);

  } else {

    // Beam direction is along the beam axis
    part_dir = beam_dir;

  }
  fPositron.dir = part_dir.unit();
  //G4cout << "BeamGenerator - Beam direction " << beam_dir
  //	 << " Positron direction " << fPositron.dir << G4endl;


  // Set positron total energy
  fPositron.E = sqrt(fPositron.P*fPositron.P+fPositron.m*fPositron.m);
  //  G4cout << "BeamGenerator - Positron energy " << fPositron.E << " " << fPositron.E-fPositron.P << G4endl;

  // Set positron momentum vector
  fPositron.p = G4ThreeVector(fPositron.P*fPositron.dir.x(),
			      fPositron.P*fPositron.dir.y(),
			      fPositron.P*fPositron.dir.z());
  //  G4cout << "BeamGenerator - Positron momentum vector " << fPositron.p << G4endl;

}

void BeamGenerator::GenerateTargetPositron()
{

  // Generate a standard positron so that timing and energy are correct
  GeneratePrimaryPositron();

  BeamParameters* bpar = BeamParameters::GetInstance();

  // If the beam is generated at Target front face, do nothing
  if (bpar->GetBeamTargetPathLength() == 0.) return;

  // Correct energy for mylar window crossing (to be implemented)

  // Position positron in front of Target (need distribution)
  G4double x = G4RandGauss::shoot(0.,bpar->GetBeamTargetPosXSpread());
  G4double y = G4RandGauss::shoot(0.,bpar->GetBeamTargetPosYSpread());
  G4double z = bpar->GetBeamTargetPosZ();  
  fPositron.pos = G4ThreeVector(x,y,z);

  // Define direction (need emittance distribution)
  G4double emitX = bpar->GetBeamTargetEmittanceX();
  G4double emitY = bpar->GetBeamTargetEmittanceY();
  G4double phi = twopi*G4UniformRand();
  G4double sigmaTheta = emitX*cos(phi)+emitY*sin(phi);
  G4double theta = G4RandGauss::shoot(0.,sigmaTheta);

  // Compute direction vector
  G4double pX = sin(theta)*cos(phi);
  G4double pY = sin(theta)*sin(phi);
  G4double pZ = cos(theta);
  fPositron.dir = G4ThreeVector(pX,pY,pZ).unit();

  // Define momentum vector
  fPositron.P = sqrt(fPositron.E*fPositron.E-fPositron.m*fPositron.m);
  fPositron.p = G4ThreeVector(fPositron.P*pX,fPositron.P*pY,fPositron.P*pZ);

  // Correct positron time for path length from beam origin to Target
  fPositron.t = fPositron.t+bpar->GetBeamTargetPathLength()/(c_light*fPositron.P/fPositron.E);

}

//void BeamGenerator::CreateFinalStateUboson(G4ParticleGun* pGun)
void BeamGenerator::CreateFinalStateUboson()
{

  // Get mass of the Uboson
  G4double ubosonM = BeamParameters::GetInstance()->GetUbosonMass();
  //G4cout << "BeamGenerator - Uboson mass " << ubosonM/MeV << " MeV" << G4endl;

  //Define the process: X->Uboson+gamma

  /*
  // Choose random decay point along e+ path within Target
  // N.B. as the target is very thin, we assume that the decay point x,y coordinates and time
  // do not vary along the e+ path (can be fixed if needed)
  G4double Dx = fPositron.pos.x();
  G4double Dy = fPositron.pos.y();
  G4double Dz = fDetector->GetTargetFrontFaceZ()+G4UniformRand()*fDetector->GetTargetThickness();
  G4double Dt = fPositron.t; // Should take into account time travelled inside Target
  //  G4cout << "BeamGenerator - Vtx " << Dx << " " << Dy << " " << Dz << " T " << Dt << G4endl;
  */

  // Get theta and phi from positron direction (assume beam axis directed along Z)
  G4double theta_p = atan2(sqrt(fPositron.dir.x()*fPositron.dir.x()+fPositron.dir.y()*fPositron.dir.y()),fPositron.dir.z())*rad;
  G4double phi_p = atan2(fPositron.dir.y(),fPositron.dir.x())*rad;

  // Choose random decay point along e+ path within Target
  G4double z_decay = (fDetector->GetTargetFrontFaceZ()-fPositron.pos.z())+G4UniformRand()*fDetector->GetTargetThickness();
  G4double s_decay = z_decay/cos(theta_p);
  G4double Dx = fPositron.pos.x()+s_decay*sin(theta_p)*cos(phi_p);
  G4double Dy = fPositron.pos.y()+s_decay*sin(theta_p)*sin(phi_p);
  G4double Dz = fPositron.pos.z()+z_decay;
  G4double Dt = fPositron.t+s_decay/(c_light*fPositron.P/fPositron.E);

  // Get e+ four-momentum
  G4double pp[4];
  pp[0] = fPositron.E;     // Total energy
  pp[1] = fPositron.p.x(); // Momentum x
  pp[2] = fPositron.p.y(); // Momentum y
  pp[3] = fPositron.p.z(); // Momentum z
  //  G4cout << "BeamGenerator - P(e+) " << pp[0] << " " << pp[1] << " " << pp[2] << " " << pp[3] << G4endl;

  // e- is at rest
  G4double ep[4];
  ep[0] = G4ParticleTable::GetParticleTable()->FindParticle("e-")->GetPDGMass();
  ep[1]=ep[2]=ep[3]=0.*MeV;
  //  G4cout << "BeamGenerator - P(e-) " << ep[0] << " " << ep[1] << " " << ep[2] << " " << ep[3] << G4endl;

  // Compute four-momentum and mass of the virtual state e+e-
  G4double vsp[4];
  for(G4int i=0; i<4; i++) { vsp[i] = pp[i]+ep[i]; }
  G4double vsM = sqrt( vsp[0]*vsp[0]-vsp[1]*vsp[1]-vsp[2]*vsp[2]-vsp[3]*vsp[3] );
  //  G4cout << "BeamGenerator - P(VS) " << vsp[0] << " " << vsp[1] << " " << vsp[2] << " " << vsp[3]
  //	 << " M(VS) " << vsM << G4endl;

  // === Compute Uboson+gamma final state in the CM ===

  //Generate a random unit vector for the Uboson decay direction
  //G4ThreeVector dir = G4RandomDirection();

  // Generate unit vector for the Uboson direction using cross section formula depending on costheta
  G4double xmin = -1;
  G4double xmax = 1;
  G4double max = CS2(xmax,G4ParticleTable::GetParticleTable()->FindParticle("e-")->GetPDGMass(),ubosonM,pp[0]);
  G4double val,xval,yval;
  
  //TRandom3 rand(0);
  G4int flag=1;
  while (flag) {
    //xval=rand.Uniform(xmin,xmax);
    xval = xmin+G4UniformRand()*(xmax-xmin);
    val = CS2(xval,G4ParticleTable::GetParticleTable()->FindParticle("e-")->GetPDGMass(),ubosonM,pp[0]);
    //yval=rand.Uniform(0,max);
    yval = G4UniformRand()*max;
    if (yval<val) flag=0;
  }
  G4double costheta = xval;
  G4double sintheta = sqrt(1-costheta*costheta);
  //double phi=rand.Uniform(0,2*M_PI);
  G4double phi = G4UniformRand()*2.*M_PI;
  G4ThreeVector dir = G4ThreeVector(sintheta*cos(phi),sintheta*sin(phi),costheta);
  //  G4cout << "Cos(theta): " << costheta << G4endl;

  //G4cout << "UthetaCM: " << acos(dir.z()/sqrt(dir.x()*dir.x()+dir.y()*dir.y()+dir.z()*dir.z())) << G4endl;

  // Compute Uboson four-momentum
  G4double UpCM[4];
  UpCM[0] = (ubosonM*ubosonM+vsM*vsM)/(2.*vsM); // Uboson total energy
  G4double UP = sqrt(UpCM[0]*UpCM[0]-ubosonM*ubosonM); // Uboson momentum
  UpCM[1] = UP*dir.x();
  UpCM[2] = UP*dir.y();
  UpCM[3] = UP*dir.z();
  //  G4cout << "BeamGenerator - PCM(Uboson) " << UpCM[0] << " " << UpCM[1] << " " << UpCM[2] << " " << UpCM[3] << G4endl;

  // Compute gamma four-momentum
  G4double gpCM[4];
  gpCM[0] = (-ubosonM*ubosonM+vsM*vsM)/(2.*vsM); // Gamma total energy
  gpCM[1] = -gpCM[0]*dir.x();
  gpCM[2] = -gpCM[0]*dir.y();
  gpCM[3] = -gpCM[0]*dir.z();
  //  G4cout << "BeamGenerator - PCM(gamma) " << gpCM[0] << " " << gpCM[1] << " " << gpCM[2] << " " << gpCM[3] << G4endl;

  // === Boost UBoson+gamma final state to the lab ===

  G4double Up[4];
  Up[0] = ( UpCM[0]*vsp[0]+UpCM[1]*vsp[1]+UpCM[2]*vsp[2]+UpCM[3]*vsp[3] )/vsM;
  G4double UC = (UpCM[0]+Up[0])/(vsM+vsp[0]);
  Up[1] = UpCM[1]+UC*vsp[1];
  Up[2] = UpCM[2]+UC*vsp[2];
  Up[3] = UpCM[3]+UC*vsp[3];
  //  G4cout << "BeamGenerator - P(Uboson) " << Up[0] << " " << Up[1] << " " << Up[2] << " " << Up[3] << G4endl;

  G4double gp[4];
  gp[0] = ( gpCM[0]*vsp[0]+gpCM[1]*vsp[1]+gpCM[2]*vsp[2]+gpCM[3]*vsp[3] )/vsM;
  G4double gC = (gpCM[0]+gp[0])/(vsM+vsp[0]);
  gp[1] = gpCM[1]+gC*vsp[1];
  gp[2] = gpCM[2]+gC*vsp[2];
  gp[3] = gpCM[3]+gC*vsp[3];
  //  G4cout << "BeamGenerator - P(gamma) " << gp[0] << " " << gp[1] << " " << gp[2] << " " << gp[3] << G4endl;

  // Create UBoson primary particle with generated four-momentum (use geantino)
  G4PrimaryParticle* uboson = new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("geantino"),
						   Up[1],Up[2],Up[3],Up[0]);

  // Create gamma primary particle with generated four-momentum
  G4PrimaryParticle* gamma = new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("gamma"),
						   gp[1],gp[2],gp[3],gp[0]);

  // Compute gamma emission angle
  G4double Genergy = sqrt(gp[1]*gp[1]+gp[2]*gp[2]+gp[3]*gp[3]);
  G4double Gtheta = acos(gp[3]/Genergy);
  fHistoManager->FillHisto(17,Gtheta);
  fHistoManager->FillHisto2(37,Gtheta,Genergy,1.);

  // Create primary vertex at generated position/time
  G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(Dx,Dy,Dz),Dt);
  vtx->SetPrimary(uboson);
  vtx->SetPrimary(gamma);

  // Add primary vertex to event
  fEvent->AddPrimaryVertex(vtx);

  // Store Uboson event in MCTruth
  if (fMCTruthMgr->IsEnabled()) {
    MCTruthVertex* tvtx = fMCTruthMgr->AddVertex("UBosonGamma",G4ThreeVector(Dx,Dy,Dz),Dt);
    tvtx->AddParticleIn(G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGEncoding(),fPositron.E,fPositron.p);
    tvtx->AddParticleOut(G4ParticleTable::GetParticleTable()->FindParticle("geantino")->GetPDGEncoding(),Up[0],G4ThreeVector(Up[1],Up[2],Up[3]));
    tvtx->AddParticleOut(G4ParticleTable::GetParticleTable()->FindParticle("gamma")->GetPDGEncoding(),gp[0],G4ThreeVector(gp[1],gp[2],gp[3]));
  }
  
}

void BeamGenerator::CreateFinalStateThreeGamma(G4double decayLength)
{

  static G4int iline = 0;

  // Get file with list of three-gamma events kinematics
  G4String file3g = BeamParameters::GetInstance()->GetThreePhotonDecaysFilename();

  std::ifstream infile;
  std::string Line = "";
  infile.open(file3g.data());
  if(!infile) {
    std::cout << "BeamGenerator::CreateFinalStateThreeGamma - ERROR - ThreeGamma file " <<  file3g << " not found" << std::endl;
    exit(1);
  }
  G4int il=0;
  while (!infile.eof() && il <= iline) {
     getline(infile,Line);
     il++;
  }
  infile.close();

  // If we did not reach EOF, generate 3 photons
  if (il == iline + 1) {

    // Get theta and phi from positron direction (assume beam axis directed along Z)
    G4double theta = atan2(sqrt(fPositron.dir.x()*fPositron.dir.x()+fPositron.dir.y()*fPositron.dir.y()),fPositron.dir.z())*rad;
    G4double phi = atan2(fPositron.dir.y(),fPositron.dir.x())*rad;

    // Choose random decay point along e+ path within Target
    G4double z_decay = (fDetector->GetTargetFrontFaceZ()-fPositron.pos.z())+G4UniformRand()*fDetector->GetTargetThickness();
    G4double s_decay = z_decay/cos(theta);
    G4double Dx = fPositron.pos.x()+s_decay*sin(theta)*cos(phi);
    G4double Dy = fPositron.pos.y()+s_decay*sin(theta)*sin(phi);
    G4double Dz = fPositron.pos.z()+z_decay;
    G4double Dt = fPositron.t+s_decay/(c_light*fPositron.P/fPositron.E);

    // Create annihilation vertex in MCTruth
    MCTruthVertex* tvtx;
    if (fMCTruthMgr->IsEnabled()) {
      if (decayLength == 0.) {
	tvtx = fMCTruthMgr->AddVertex("ThreeGamma",G4ThreeVector(Dx,Dy,Dz),Dt);
      } else {
	tvtx = fMCTruthMgr->AddVertex("ThreeGammaAnnihil",G4ThreeVector(Dx,Dy,Dz),Dt);
      }
      tvtx->AddParticleIn(G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGEncoding(),fPositron.E,fPositron.p);
    }

    if (decayLength > 0.) {
      // Displace vertex according to decay length defined by user
      G4double dispS = G4RandExponential::shoot(decayLength);
      Dx += dispS*sin(theta)*cos(phi);
      Dy += dispS*sin(theta)*sin(phi);
      Dz += dispS*cos(theta);
      Dt += dispS/c_light; // Apporoximation: need to be corrected for particle mass and beam energy
    }

    // Create primary vertex at decay point
    G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(Dx,Dy,Dz),Dt);
    
    // If decay vertex is displaced, store it in MCTruth
    if ( fMCTruthMgr->IsEnabled() & (decayLength > 0.) )
      tvtx = fMCTruthMgr->AddVertex("ThreeGammaDecay",G4ThreeVector(Dx,Dy,Dz),Dt);

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
     
      // Rotate gamma momentum along the direction of the primary positron
      G4ThreeVector gamma_p = G4ThreeVector(p[1],p[2],p[3]);
      gamma_p.rotateUz(fPositron.dir);

      // Create gamma primary particle with generated four-momentum
      G4PrimaryParticle* gamma =
	new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("gamma"),
			      gamma_p.x(),gamma_p.y(),gamma_p.z(),p[0]);
      vtx->SetPrimary(gamma);

      // Store gamma in MCTruth
      if (fMCTruthMgr->IsEnabled())
	tvtx->AddParticleOut(G4ParticleTable::GetParticleTable()->FindParticle("gamma")->GetPDGEncoding(),p[0],gamma_p);

    }

    // Add primary vertex to event
    fEvent->AddPrimaryVertex(vtx);

    // Skip to next line
    iline++;

  } else {
    G4cout << "BeamGenerator::CreateFinalStateThreeGamma - WARNING - Reached end of three-gamma decays input file" << G4endl;
  }

}

void BeamGenerator::CreateFinalStateTwoGamma(G4double decayLength)
{

  static G4int iline = 0;

  // Get file with list of two-gamma events kinematics
  G4String file2g = BeamParameters::GetInstance()->GetTwoPhotonDecaysFilename();

  std::ifstream infile;
  std::string Line = "";
  infile.open(file2g.data());
  if(!infile) {
    std::cout << "BeamGenerator::CreateFinalStateTwoGamma - ERROR - TwoGamma file " <<  file2g << " not found" << std::endl;
    exit(1);
  }
  G4int il=0;
  while (!infile.eof() && il <= iline) {
     getline(infile,Line);
     il++;
  }
  infile.close();

  // If we did not reach EOF, generate 2 photons
  if (il == iline + 1) {
 
    // Get theta and phi from positron direction (assume beam axis directed along Z)
    G4double theta = atan2(sqrt(fPositron.dir.x()*fPositron.dir.x()+fPositron.dir.y()*fPositron.dir.y()),fPositron.dir.z())*rad;
    G4double phi = atan2(fPositron.dir.y(),fPositron.dir.x())*rad;

    // Choose random decay point along e+ path within Target
    G4double z_decay = (fDetector->GetTargetFrontFaceZ()-fPositron.pos.z())+G4UniformRand()*fDetector->GetTargetThickness();
    G4double s_decay = z_decay/cos(theta);
    G4double Dx = fPositron.pos.x()+s_decay*sin(theta)*cos(phi);
    G4double Dy = fPositron.pos.y()+s_decay*sin(theta)*sin(phi);
    G4double Dz = fPositron.pos.z()+z_decay;
    G4double Dt = fPositron.t+s_decay/(c_light*fPositron.P/fPositron.E);

    // Create annihilation vertex in MCTruth
    MCTruthVertex* tvtx;
    if (fMCTruthMgr->IsEnabled()) {
      if (decayLength == 0.) {
	tvtx = fMCTruthMgr->AddVertex("TwoGamma",G4ThreeVector(Dx,Dy,Dz),Dt);
      } else {
	tvtx = fMCTruthMgr->AddVertex("TwoGammaAnnihil",G4ThreeVector(Dx,Dy,Dz),Dt);
      }
      tvtx->AddParticleIn(G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGEncoding(),fPositron.E,fPositron.p);
    }

    if (decayLength > 0.) {
      // Displace vertex according to decay length defined by user
      G4double dispS = G4RandExponential::shoot(decayLength);
      Dx += dispS*sin(theta)*cos(phi);
      Dy += dispS*sin(theta)*sin(phi);
      Dz += dispS*cos(theta);
      Dt += dispS/c_light; // Apporoximation: need to be corrected for particle mass and beam energy
    }

    // Create primary vertex at decay point
    G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(Dx,Dy,Dz),Dt);
    
    // If decay vertex is displaced, store it in MCTruth
    if ( fMCTruthMgr->IsEnabled() & (decayLength > 0.) )
      tvtx = fMCTruthMgr->AddVertex("TwoGammaDecay",G4ThreeVector(Dx,Dy,Dz),Dt);

    // Decode input line
    std::istringstream iss(Line);

    // Skip first three fields in the line
    G4int it;
    G4double dt1,dt2;
    iss >> it >> dt1 >> dt2;

    // Loop over the two photons
    G4double p[4]; // Vector to store four-momentum of the gamma
    for(G4int j=0; j<2; j++) {

      iss >> p[1] >> p[2] >> p[3]; // Get gamma momentum
      for(G4int k=1; k<=3; k++) { p[k] *= GeV; } // Values are given in GeV
      p[0] = sqrt(p[1]*p[1]+p[2]*p[2]+p[3]*p[3]); // Compute total energy of the gamma
     
      // Rotate gamma momentum along the direction of the primary positron
      G4ThreeVector gamma_p = G4ThreeVector(p[1],p[2],p[3]);
      gamma_p.rotateUz(fPositron.dir);
     
      // Create gamma primary particle with generated four-momentum
      G4PrimaryParticle* gamma =
	new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("gamma"),
			      gamma_p.x(),gamma_p.y(),gamma_p.z(),p[0]);
      vtx->SetPrimary(gamma);

      // Store gamma in MCTruth
      if (fMCTruthMgr->IsEnabled())
	tvtx->AddParticleOut(G4ParticleTable::GetParticleTable()->FindParticle("gamma")->GetPDGEncoding(),p[0],gamma_p);

    }

    // Add primary vertex to event
    fEvent->AddPrimaryVertex(vtx);

    // Skip to next line
    iline++;

  } else {
    G4cout << "BeamGenerator::CreateFinalStateTwoGamma - WARNING - Reached end of two-gamma decays input file" << G4endl;
  }

}

//****************************************************
// Generate BhaBha events M. Raggi 20/05/2021
//****************************************************
void BeamGenerator::CreateFinalStateBhaBha(G4double decayLength)
{

  static G4int iline = 0;

  // Get electron/positron mass
  static const G4double me = G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGMass();

  // Get file with list of two-gamma events kinematics
  G4String fileBhaBha = BeamParameters::GetInstance()->GetBhaBhaFilename();

  std::ifstream infile;
  std::string Line = "";
  infile.open(fileBhaBha.data());
  if(!infile) {
    std::cout << "BeamGenerator::CreateFinalStateBhabha - ERROR - BhaBha file " <<  fileBhaBha << " not found" << std::endl;
    exit(1);
  }
  G4int il=0;
  while (!infile.eof() && il <= iline) {
     getline(infile,Line);
     il++;
  }
  infile.close();

  // If we did not reach EOF, generate 2 photons
  if (il == iline + 1) {
 
    // Get theta and phi from positron direction (assume beam axis directed along Z)
    G4double theta = atan2(sqrt(fPositron.dir.x()*fPositron.dir.x()+fPositron.dir.y()*fPositron.dir.y()),fPositron.dir.z())*rad;
    G4double phi = atan2(fPositron.dir.y(),fPositron.dir.x())*rad;

    // Choose random decay point along e+ path within Target
    G4double z_decay = (fDetector->GetTargetFrontFaceZ()-fPositron.pos.z())+G4UniformRand()*fDetector->GetTargetThickness();
    G4double s_decay = z_decay/cos(theta);
    G4double Dx = fPositron.pos.x()+s_decay*sin(theta)*cos(phi);
    G4double Dy = fPositron.pos.y()+s_decay*sin(theta)*sin(phi);
    G4double Dz = fPositron.pos.z()+z_decay;
    G4double Dt = fPositron.t+s_decay/(c_light*fPositron.P/fPositron.E);

    // Create annihilation vertex in MCTruth
    MCTruthVertex* tvtx;
    if (fMCTruthMgr->IsEnabled()) {
      if (decayLength == 0.) {
	tvtx = fMCTruthMgr->AddVertex("Bhabha",G4ThreeVector(Dx,Dy,Dz),Dt);
      } else {
	tvtx = fMCTruthMgr->AddVertex("BhabhaAnnihil",G4ThreeVector(Dx,Dy,Dz),Dt);
      }
      tvtx->AddParticleIn(G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGEncoding(),fPositron.E,fPositron.p);
    }

    if (decayLength > 0.) {
      // Displace vertex according to decay length defined by user
      G4double dispS = G4RandExponential::shoot(decayLength);
      Dx += dispS*sin(theta)*cos(phi);
      Dy += dispS*sin(theta)*sin(phi);
      Dz += dispS*cos(theta);
      Dt += dispS/c_light; // Apporoximation: need to be corrected for particle mass and beam energy
    }

    // Create primary vertex at decay point
    G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(Dx,Dy,Dz),Dt);
       
    // If decay vertex is displaced, store it in MCTruth
    if ( fMCTruthMgr->IsEnabled() & (decayLength > 0.) )
      tvtx = fMCTruthMgr->AddVertex("BhabhaDecay",G4ThreeVector(Dx,Dy,Dz),Dt);

    // Decode input line
    std::istringstream iss(Line);

    // Skip first three fields in the line
    G4int it;
    G4double dt1,dt2;
    iss >> it >> dt1 >> dt2;

    // Loop over electron and positron
    G4double p[4]; // Vector to store four-momentum of the lepton
    for(G4int j=0; j<2; j++) {
      iss >> p[1] >> p[2] >> p[3]; // Get lepton momentum
      for(G4int k=1; k<=3; k++) { p[k] *= GeV; } // Values are given in GeV
      if(j==0){
        fHistoManager->FillHisto(78,p[1]);
        fHistoManager->FillHisto(79,p[2]);
        fHistoManager->FillHisto(80,p[3]);
        fHistoManager->FillHisto(84,(p[2]/p[3])*300);
        //std::cout<<"Positron radius " << (p[2]/p[3])*300 << std::endl;
      }
      else{
        fHistoManager->FillHisto(81,p[1]);
        fHistoManager->FillHisto(82,p[2]);
        fHistoManager->FillHisto(83,p[3]);
        fHistoManager->FillHisto(85,(p[2]/p[3])*300);
        //std::cout<<"Electron radius " << (p[2]/p[3])*300 << std::endl;
      }
      p[0] = sqrt(p[1]*p[1]+p[2]*p[2]+p[3]*p[3]+me*me); // Compute total energy of the lepton
     
      // Rotate lepton momentum along the direction of the primary positron
      G4ThreeVector lepton_p = G4ThreeVector(p[1],p[2],p[3]);
      lepton_p.rotateUz(fPositron.dir);
     
      // Create e+/- primary particles with generated four-momentum (1: positron; 0: electron)
      G4PrimaryParticle* lepton; 
      if (j==1) lepton =
		  new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("e+"),
					lepton_p.x(),lepton_p.y(),lepton_p.z(),p[0]);
      if (j==0) lepton = 
		  new G4PrimaryParticle(G4ParticleTable::GetParticleTable()->FindParticle("e-"),
					lepton_p.x(),lepton_p.y(),lepton_p.z(),p[0]);
      vtx->SetPrimary(lepton);

       // Store lepton in MCTruth
     if (fMCTruthMgr->IsEnabled()) {
        if (j==1) tvtx->AddParticleOut(G4ParticleTable::GetParticleTable()->FindParticle("e+")->GetPDGEncoding(),p[0],lepton_p);
        if (j==0) tvtx->AddParticleOut(G4ParticleTable::GetParticleTable()->FindParticle("e-")->GetPDGEncoding(),p[0],lepton_p);
      }

    }

    // Add primary vertex to event
    fEvent->AddPrimaryVertex(vtx);

    // Skip to next line
    iline++;

  } else {
    G4cout << "BeamGenerator::CreateFinalStateBhabha - WARNING - Reached end of Bhabha decays input file" << G4endl;
  }
}

void BeamGenerator::GenerateCalibrationParticle()
{

  BeamParameters* bpar = BeamParameters::GetInstance();

  // Create primary vertex at center of back face of target with t=0.
  G4double vT = 0.*ns;
  G4double vX = 0.*cm;
  G4double vY = 0.*cm;
  G4double vZ = bpar->GetBeamCenterPosZ();
  G4PrimaryVertex* vtx = new G4PrimaryVertex(G4ThreeVector(vX,vY,vZ),vT);

  // Choose a point within circle on the surface of ECal
  G4double pX = bpar->GetCalibRunCenterX();
  G4double pY = bpar->GetCalibRunCenterY();
  G4String detector = bpar->GetCalibRunDetector();
  G4double pZ = 0.;
  if (detector == "ECal") {
    pZ = fDetector->GetECalFrontFaceZ();
  } else if (detector == "LeadGlass") {
    pZ = fDetector->GetLeadGlassFrontFaceZ();
  } else {
    G4cout << "BeamGenerator::GenerateCalibrationParticle - WARNING - Detector " << detector << " not allowed. Using ECal." << G4endl;
    pZ = fDetector->GetECalFrontFaceZ();
  }
  if (bpar->GetCalibRunRadius() != 0.) {
    // Flat distribution over circle
    G4double rd = bpar->GetCalibRunRadius()*sqrt(G4UniformRand());
    G4double th = 2.*pi*G4UniformRand();
    pX += rd*cos(th);
    pY += rd*sin(th);
  }

  // Compute unit vector from vertex to generated point
  G4ThreeVector vp = G4ThreeVector(pX-vX,pY-vY,pZ-vZ).unit();

  // Create particle pointing from vertex to generated point
  G4String partname = bpar->GetCalibRunParticle();
  //G4ParticleDefinition* part = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  G4ParticleDefinition* part = G4ParticleTable::GetParticleTable()->FindParticle(partname);
  G4double part_E = bpar->GetCalibRunEnergy();
  G4PrimaryParticle* particle = new G4PrimaryParticle(part,part_E*vp.x(),part_E*vp.y(),part_E*vp.z(),part_E);
  vtx->SetPrimary(particle);

  // Add primary vertex to event
  fEvent->AddPrimaryVertex(vtx);

}

G4double BeamGenerator::GetGammaAngle(G4ThreeVector gammaDir,G4ThreeVector beamDir)
{
  //G4double product=0;
  //for (int i=0; i<3; i++) product += gammaDir[i]*beamDir[i];
  //G4double theta = acos (product);// * 180.0 / 3.14159265;  //in degreees
  //  G4cout<<"product"<< product <<"Theta " <<theta<<G4endl;
  G4double angle = gammaDir.angle(beamDir)/rad;
  //  G4cout << "Mauro dice " << theta << " io dico " << angle << G4endl;
  return angle;
}

G4double BeamGenerator::CS(G4double x,G4double m_e,G4double m_U,G4double E_beam){

  G4double s=2*m_e*m_e+2*m_e*E_beam;
  G4double ret;

  ret = (s*s+m_U*m_U*m_U*m_U)/(1-x*x);
  ret = ret-(s-m_U*m_U)*(s-m_U*m_U)/2;

  return ret;

}

G4double BeamGenerator::CS2(G4double x,G4double m_e,G4double m_U,G4double E_beam){

  G4double s=2*m_e*m_e+2*m_e*E_beam;
  G4double p=(s-m_U*m_U)/2/sqrt(s);
  G4double ret;

  ret = p*p*(s*s+m_U*m_U*m_U*m_U)/(m_e*m_e+p*p*(1-x*x));
  ret = ret-(s-m_U*m_U)*(s-m_U*m_U)/2;

  return ret;

}
