#include "PrimaryGeneratorAction.hh"

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

//#include "MyEvent.hh"
//#include "MyEventGeneration.hh"
#include "Constants.hh"
#include "TargetGeometry.hh"
#include "DetectorConstruction.hh"
#include "BeamParameters.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* myDC)
  :fDetector(myDC)
{

  fEvent = 0;

  fTargetGeometry = TargetGeometry::GetInstance();

  // Create default particle gun
  fParticleGun = new G4ParticleGun();

  /*
  evt = MyEvent::GetInstance()->GetGenEvent();
  evt->SetUbosonMass(UMass*MeV);
  evt->SetUbosonEps(epsilon);
  evt->SetBeamEnergy(BeamEnergy*MeV);
  */
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 

  fEvent = anEvent;

  BeamParameters* bpar = BeamParameters::GetInstance();

  //  std::cout << "============================ Generate Primaries" << std::endl;
  //evt->ClearEvent();
  static int nev;
  nev++;
  if(nev%10000 == 0) std::cout << "Generating event number " << nev << std::endl;
  
  /*
  if(IsCalibRun==1){
    //    std::cout << "==== Generating calibration event === " << std::endl;
    G4double ECalX      = ECalSizeX*cm;
    G4double ECalY      = ECalSizeY*cm;
    G4double ECalLength = ECalSizeZ*cm;
    G4double ECryLength = ECalLength;
    G4double ECryX      = ECalX/ECalNRow;
    G4double ECryY      = ECalY/ECalNCol;
    G4int i=((nev-1)/ECalNCol)%ECalNRow; //put nrow Nrow !!!
    G4int j=(nev-1)%ECalNCol;
    G4double PosXCry=-ECalX*0.5+0.5*ECryX+i*ECryX;
    G4double PosYCry=-ECalY*0.5+0.5*ECryY+j*ECryY;
    G4ThreeVector positionCry = G4ThreeVector(PosXCry,PosYCry,ECalPosiZ*cm-ECryLength/2.);
    G4int HoleFlag=0; //should be parametric in NRow NCol
    if( fabs(PosXCry)<ECalInnHole*cm && fabs(PosYCry)<ECalInnHole*cm ){
      HoleFlag=1;  //means is in the hole 
    }else{
      HoleFlag=0;
    }	
    if(HoleFlag==0){
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
      particleGun->SetParticleDefinition(particle);
      particleGun->SetNumberOfParticles(NPrimaries);  
      particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,0.*cm)); 
      particleGun->SetParticleMomentumDirection(positionCry.unit()); //with Y=0 goes to the Gap!!!

      //Fill all the information about the primaries before generating the event:
      MyParticle part(particleGun->GetParticleDefinition ()->GetPDGMass ());
      part.setType(particleGun->GetParticleDefinition ()->GetParticleName ()); 
      part.setProcess("beam");

      part.setEnergy(particleGun->GetParticleEnergy());
      part.calcMomentum(
			particleGun->GetParticleMomentumDirection().x(),
			particleGun->GetParticleMomentumDirection().y(),
			particleGun->GetParticleMomentumDirection().z()  );
      part.setPVtx(particleGun->GetParticlePosition ().x(),
		   particleGun->GetParticlePosition ().y(),
		   particleGun->GetParticlePosition ().z());
      evt->AddProduct(part);

      particleGun->GeneratePrimaryVertex(anEvent);
    }
  }else if(IsCalibRun==2){ //ENERGY RESOLUTION for  energy resolution use energy in the Constants.hh
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    particleGun->SetParticleDefinition(particle);
    particleGun->SetNumberOfParticles(1); 
    particleGun->SetParticleEnergy(50.*MeV);
    //Fill all the information about the primaries before generating the event:
    G4ThreeVector positionCry = G4ThreeVector(5.5*cm,5.5*cm,200.*cm);
    particleGun->SetParticleMomentumDirection(positionCry.unit()); //with Y=0 goes to the Gap!!!
    particleGun->GeneratePrimaryVertex(anEvent);
    //Gun dritto di fotoni di energia fissa
  }else if(IsCalibRun==3){ //ENERGY RESOLUTION for  energy resolution use energy in the Constants.hh
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    particleGun->SetParticleDefinition(particle);
    particleGun->SetNumberOfParticles(1); 
    particleGun->SetParticleEnergy(500.*MeV);
    particleGun->SetParticlePosition(G4ThreeVector(1.1*cm,-19.1*cm+ECalCellSize/2,(ECalPosiZ-ECalSizeZ*0.5-20.)*cm));
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
    particleGun->GeneratePrimaryVertex(anEvent);
    // ************STANDARD EVENT GENERATION include DP and 3Gamma
  } else {//    std::cout << "==== Generating general event === " << std::endl;
  */


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

    /*
    // Generate primary
    int Dummy = SetPositronBeamParameters();
    //if(BeamESpread==1){
    //  evt->SetBeamEnergy(NewBeamEnergy);
    //}else{
    //  evt->SetBeamEnergy(BeamEnergy);
    //}
    evt->GenerateEvent(1,
		       particleGun->GetParticleMomentumDirection().x(),
		       particleGun->GetParticleMomentumDirection().y(),
		       particleGun->GetParticleMomentumDirection().z());
    
    //Loop over the final state particles and trace them through the detector
    std::vector<MyParticle>* particles = evt->getParticles();
    std::vector<MyParticle>::iterator it;
    //    std::cout << "Number of primaries :" << particles->size() << std::endl;
    
    it = particles->begin();
    while (it != particles->end()) {
      //	  if(1) {
      //	std::cout << "Found particle " << it->getType() << std::endl;
      if(it->getType()!="gamma" && UDecType==1){
	it++;
	continue;
      }
      particle = particleTable->FindParticle(it->getType());
      particleGun->SetParticleDefinition(particle);
      
      double *p = it->get4Momentum();
      
      //        cout<<"UBOSON 3 momentum "<< p <<" "<<particle<<endl;
      const double *pvtx = it->getPVtx();
      particleGun->SetParticleEnergy(p[0]);
      particleGun->SetParticleMomentumDirection(G4ThreeVector(p[1],p[2],p[3]).unit());
      //	particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,0.*cm));
      //		  std::cout << "Particle " << it->getType() << "  Production vertex: "
      //		    << pvtx[0]+ StartX<< "\t" << pvtx[1] + StartY << "\t" << pvtx[2] << "\t"
      //		    << std::endl;
      particleGun->SetParticlePosition(G4ThreeVector(pvtx[0]*m+StartX,pvtx[1]*m+StartY,pvtx[2]*m ));
      particleGun->GeneratePrimaryVertex(anEvent);
      it++;
    }
    */
	
  }//END OF UBOSON GENERATION!!
  
  //*********************
  //Three photon events
  //*********************
  for(int iggg = 0; iggg < nThreePhotonDecays; iggg++) {

    // Generate primary e+ which will decay to three gammas and store it in particleGun
    SetPositronBeamParameters();

    // Generate gamma+gamma+gamma final state
    CreateFinalStateThreeGamma();

    /*
    //U Boson MC 
    G4double NewBeamEnergy = BeamEnergy;
    if(BeamESpread==1){
      NewBeamEnergy = G4RandGauss::shoot(BeamEnergy,SigmaBeamE);
      //particleGun->SetParticleEnergy(NewBeamEnergy*MeV);
    }
    evt->SetBeamEnergy(NewBeamEnergy);
    //Set all the beam characteristics according to beam flags
    SetPositronBeamParameters(); 
    evt->GenerateEvent(2);
    G4double StartX=0;
    G4double StartY=0;      
    if(BeamSpot==1){
      StartX= G4RandGauss::shoot(0.,SigmaBeamX);
      StartY= G4RandGauss::shoot(0.,SigmaBeamY);
    }
    //Loop over the final state particles and trace them through the detector
    std::vector<MyParticle>* particles = evt->getParticles();
    std::vector<MyParticle>::iterator it;
    it = particles->begin();
    while (it != particles->end()) {
      particle = particleTable->FindParticle(it->getType());
      particleGun->SetParticleDefinition(particle);
      double *p = it->get4Momentum();
      const double *pvtx = it->getPVtx();	    
      particleGun->SetParticleEnergy(p[0]);
      particleGun->SetParticleMomentumDirection(G4ThreeVector(p[1],p[2],p[3]).unit());
      //	particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,0.*cm));	    
      //	  std::cout << "Particle " << it->getType() << "  Production vertex: " 
      //	    << pvtx[0] << "\t" << pvtx[1] << "\t" << pvtx[2] << "\t" 
      //	    << std::endl;
      
      //	std::cout << "Particle " << it->getType() << "  Production vertex: \t" 
      //		  << pvtx[0] << "\t" << pvtx[1] << "\t" << pvtx[2] << "\t" 
      //		  << "Particle momentum: " 
      //		  <<  (float) p[0] << "\t" << (float)  p[1] << "\t" << (float)  p[2] 
      //		  << std::endl;
      //
      particleGun->SetParticlePosition(G4ThreeVector(pvtx[0]*m+StartX,pvtx[1]*m+StartY,pvtx[2]*m ));
      particleGun->GeneratePrimaryVertex(anEvent);
      //	  }
      it++;
      //	cout<<"Generating 3 gamma "<<std::endl;
    }
    */

  }//END OF 3 photon generation
  
  //******************************************************
  //General BG generator particles on the target per event 
  //******************************************************
  for(int ip = 0; ip < nPositrons; ip++) { 
    //Set all the beam characteristics according to beam flags
    SetPositronBeamParameters(); 
    fParticleGun->GeneratePrimaryVertex(fEvent);
  }

}

int PrimaryGeneratorAction::SetPositronBeamParameters()
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
  
  // Positron KINETIC energy, i.e. (E-m) with E=sqrt(p^2+m^2)
  G4double beamE = bpar->GetBeamEnergy();
  if ( bpar->BeamEnergyApplySpread() ) beamE = G4RandGauss::shoot(beamE,bpar->GetBeamEnergySpread());
  fParticleGun->SetParticleEnergy(beamE);
  G4cout << "PGA - Positron energy " << beamE << G4endl;

  // All positrons are generated 1um before the front face of the target
  G4double xPos = bpar->GetBeamCenterPosX();
  G4double yPos = bpar->GetBeamCenterPosY();
  G4double zPos = fTargetGeometry->GetTargetFrontFacePosZ()-1.*um;
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

void PrimaryGeneratorAction::CreateFinalStateUboson()
{

  // Define mass of the Uboson
  G4double ubosonM = BeamParameters::GetInstance()->GetUbosonMass();

  //Define the process: X->Uboson+gamma

  // Choose random decay point along e+ path within Target
  // N.B. as the target is very thin, we assume that the decay point x,y coordinates do not vary along the e+ path
  G4double Dx = fParticleGun->GetParticlePosition().x();
  G4double Dy = fParticleGun->GetParticlePosition().y();
  G4double Dz = fTargetGeometry->GetTargetFrontFacePosZ()+G4UniformRand()*fTargetGeometry->GetTargetSizeZ();
  G4double Dt = fParticleGun->GetParticleTime()+Dz/c_light;
  G4cout << "PGA - Vtx " << Dx << " " << Dy << " " << Dz << " T " << Dt << G4endl;

  // e+ is stored in particleGun
  G4double pM = fParticleGun->GetParticleDefinition()->GetPDGMass();
  G4double pp[4];
  pp[0] = fParticleGun->GetParticleEnergy()+pM; // Total energy
  G4double pP = sqrt(pp[0]*pp[0]-pM*pM); // Momentum
  pp[1] = pP*fParticleGun->GetParticleMomentumDirection().x();
  pp[2] = pP*fParticleGun->GetParticleMomentumDirection().y();
  pp[3] = pP*fParticleGun->GetParticleMomentumDirection().z();
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

  // Add gamma to event kinematics
  fParticleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("gamma"));
  fParticleGun->SetParticleEnergy(gp[0]);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(gp[1],gp[2],gp[3]).unit());
  fParticleGun->SetParticlePosition(G4ThreeVector(Dx,Dy,Dz));
  fParticleGun->SetParticleTime(Dt);

  // Add final gamma to event kinematics
  fParticleGun->GeneratePrimaryVertex(fEvent);

  /*
  MyParticle Uboson;
  MyParticle gamma;
  
  //Do the calculation in the Center of Mass system 
  Uboson.setMass(MU);
  Uboson.setType("Uboson");
  
  gamma.setMass(0.);
  gamma.setType("gamma");

  //Do the decay positronium -> gU
  //In the CM:
  
  double eu= (SQR(Uboson.m()) + SQR(virtualState.m()))/(2.*virtualState.m());
  double eg= (-SQR(Uboson.m()) + SQR(virtualState.m()))/(2.*virtualState.m());
  double pu = sqrt(SQR(eu) - SQR(Uboson.m()));
  
  Uboson.setEnergy(eu);
  gamma.setEnergy(eg);
  
  //  std::cout << "Energy of the U: " << eu << "\t Energy of g: "<< eg << std::endl;
  //  std::cout << "Momentum of the U: " << pu << "\t Momentum of g: "<< eg << std::endl;
  
  //Generate a random vector
  G4ThreeVector dir = G4RandomDirection();
  //  std::cout << "Random direction: " << dir.x() << "\t" << dir.y() << "\t" << dir.z() << std::endl;
  
  Uboson.calcMomentum(dir.x(),dir.y(),dir.z());
  gamma.calcMomentum(-dir.x(),-dir.y(),-dir.z());
  
  gamma.boostLAB(virtualState.get4Momentum());
  //  std::cout << "Gamma energy :"<< gamma.E() << std::endl;
  Uboson.boostLAB(virtualState.get4Momentum());


  //The primary decay is generated, set some parameters for U and get kinematics variables
  Uboson.calcGamma();
  Uboson.setPars(&eps,1);
  Uboson.calcWidthU();
  Uboson.calcTau();
  Uboson.calcBeta();

  //Generate a decay point
  Uboson.genDecayVtx();  

  products.push_back(gamma);
  //Now perform the decay of the U-boson, if possible:
  if(Uboson.m() > 2.*CLHEP::electron_mass_c2) {
    
    //In the CM of the U-boson:
    MyParticle ee(CLHEP::electron_mass_c2);
    ee.setType("e-");
    MyParticle ep(CLHEP::electron_mass_c2);
    ep.setType("e+");

    dir = G4RandomDirection();
    
    ee.setEnergy(Uboson.m()/2);
    ep.setEnergy(Uboson.m()/2);
    
    ee.calcMomentum(dir.x(),dir.y(),dir.z());
    ep.calcMomentum(-dir.x(),-dir.y(),-dir.z());
    
    ee.boostLAB(Uboson.get4Momentum());
    ep.boostLAB(Uboson.get4Momentum());
    
    ee.setPVtx(Uboson.getDVtx());
    ep.setPVtx(Uboson.getDVtx());

    products.push_back(ee);
    products.push_back(ep);
  }
  */
  
}

void PrimaryGeneratorAction::CreateFinalStateThreeGamma()
{
}
