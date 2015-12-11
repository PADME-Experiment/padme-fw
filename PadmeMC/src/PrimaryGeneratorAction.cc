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
// $Id: PrimaryGeneratorAction.cc,v 1.7 2014/06/23 09:41:45 veni Exp $
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "MyEventGeneration.hh"
#include "Constants.hh"
#include "MyEvent.hh"

#include "G4ios.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(
                                               DetectorConstruction* myDC)
:myDetector(myDC)
{
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);

  evt = MyEvent::GetInstance()->GetGenEvent();
  evt->SetUbosonMass(UMass*MeV);
  evt->SetUbosonEps(epsilon);
  evt->SetBeamEnergy(BeamEnergy*MeV);

  // default particle  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("e+");
  
  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  particleGun->SetParticleEnergy(BeamEnergy*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
  //  std::cout << "============================ Generate Primaries" << std::endl;
  evt->ClearEvent();
  static int nev;
  nev++;
  if(nev%10000 == 0) std::cout << "Generating event number " << nev << std::endl;
  
  particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,position*cm));
  static G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();   
  static G4ParticleDefinition* particle ;
  
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
//************STANDARD EVENT GENERATION include DP and 3Gamma
  } else {//    std::cout << "==== Generating general event === " << std::endl;
      G4int NToGen = NPrimaries - NUBosons;
//    G4double ParTime=0.;
//    //    G4double position = -1.*m;
//    if(IsBeamTimeON==1){ 
//      G4double NMicroBunch=BunchLenght/(MicroBunchSpace+MicroBunchLength);
//      G4int BunchNow = int(G4UniformRand()*NMicroBunch);
//      ParTime=BunchNow*(MicroBunchSpace+MicroBunchLength)+G4UniformRand()*MicroBunchLength;
//      particleGun->SetParticleTime(ParTime);
//    }
//    
//    if(BeamSpot==1){
//     StartX  = G4RandGauss::shoot(0.,SigmaBeamX);
//     StartY  = G4RandGauss::shoot(0.,SigmaBeamY);
//      particleGun->SetParticlePosition(G4ThreeVector(StartX,StartY,position*cm));
//    }else{
//      particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,position*cm));
//    }
//    
//    if(BeamESpread==1){
//      NewBeamEnergy = G4RandGauss::shoot(BeamEnergy,SigmaBeamE);
//      particleGun->SetParticleEnergy(NewBeamEnergy*MeV);
//    } else{
//      particleGun->SetParticleEnergy(BeamEnergy*MeV);
//    }
//    if(BeamEmittance==1){ //Still to be tested
//      G4double PX = G4RandGauss::shoot(0,BeamDivX);	
//      G4double PY = G4RandGauss::shoot(0,BeamDivY);
//      G4double Norm= sqrt(PX*PX+PY*PY+1);
//      particleGun->SetParticleMomentumDirection(G4ThreeVector(PX,PY,1.));
//    }else{
//      particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
//    }
//    
    //********************
    //U Boson generator MC
    //********************
    for(int ib = 0; ib < NUBosons;ib++) {
      //Set all the beam characteristics according to beam flags
      int Dummy = SetBeamParamters(); 
      if(BeamESpread==1){
	evt->SetBeamEnergy(NewBeamEnergy);
      }else{
	evt->SetBeamEnergy(BeamEnergy);
      }
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
    }//END OF UBOSON GENERATION!!

    //*********************
    //Three photon events
    //*********************
    NToGen = NToGen - NThreePhoton;
    for(int iggg = 0; iggg<NThreePhoton ;iggg++) {
      //U Boson MC 
      G4double NewBeamEnergy = BeamEnergy;
      if(BeamESpread==1){
	NewBeamEnergy = G4RandGauss::shoot(BeamEnergy,SigmaBeamE);
	//particleGun->SetParticleEnergy(NewBeamEnergy*MeV);
      }
      evt->SetBeamEnergy(NewBeamEnergy);
      //Set all the beam characteristics according to beam flags
      int Dummy = SetBeamParamters(); 
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
    }//END OF 3 photon generation

    //******************************************************
    //General BG generator particles on the target per event 
    //******************************************************
    for(int ip=0;ip<NToGen;ip++) { 
      particle    = particleTable->FindParticle("e+");
      particleGun->SetParticleDefinition(particle);
      particleGun->SetNumberOfParticles(1);
      //Set all the beam characteristics according to beam flags
      int Dummy = SetBeamParamters(); 
      // and now you can fire
      MyParticle part(particleGun->GetParticleDefinition ()->GetPDGMass ());
      part.setType(particleGun->GetParticleDefinition ()->GetParticleName ()); 
      part.setProcess("beam");
      part.setEnergy(particleGun->GetParticleEnergy());
      part.calcMomentum(particleGun->GetParticleMomentumDirection().x(),
			particleGun->GetParticleMomentumDirection().y(),
			particleGun->GetParticleMomentumDirection().z());
      part.setPVtx(particleGun->GetParticlePosition ().x(),
		   particleGun->GetParticlePosition ().y(),
		   particleGun->GetParticlePosition ().z());
      evt->AddProduct(part);
      particleGun->GeneratePrimaryVertex(anEvent);
    }
  }
  MyEvent *TheEvent = MyEvent::GetInstance();
  //  G4cout << "PrimaryGeneratorAction::GeneratePrimaries  Number of primaries in that event: "<< TheEvent->GetGenEvent()->getParticles()->size() << G4endl;
}

int PrimaryGeneratorAction::SetBeamParamters()
{
  //  G4cout << "PrimaryGeneratorAction:: Setting beam parameter"<< G4endl;
  
  G4double ParTime=0.;
  //    G4double position = -1.*m;
  if(IsBeamTimeON==1){ 
    G4double NMicroBunch=BunchLenght/(MicroBunchSpace+MicroBunchLength);
    G4int BunchNow = int(G4UniformRand()*NMicroBunch);
    ParTime=BunchNow*(MicroBunchSpace+MicroBunchLength)+G4UniformRand()*MicroBunchLength;
    particleGun->SetParticleTime(ParTime);
  }
  
  if(BeamSpot==1){
    StartX  = G4RandGauss::shoot(0.,SigmaBeamX);
    StartY  = G4RandGauss::shoot(0.,SigmaBeamY);
    particleGun->SetParticlePosition(G4ThreeVector(StartX,StartY,position*cm));
  }else{
    particleGun->SetParticlePosition(G4ThreeVector(0.*cm,0.*cm,position*cm));
  }
  
  if(BeamESpread==1){
    NewBeamEnergy = G4RandGauss::shoot(BeamEnergy,SigmaBeamE);
    particleGun->SetParticleEnergy(NewBeamEnergy*MeV);
  } else{
    particleGun->SetParticleEnergy(BeamEnergy*MeV);
  }
  if(BeamEmittance==1){ //Still to be tested
    G4double PX = G4RandGauss::shoot(0,BeamDivX);	
    G4double PY = G4RandGauss::shoot(0,BeamDivY);
    G4double Norm= sqrt(PX*PX+PY*PY+1);
    particleGun->SetParticleMomentumDirection(G4ThreeVector(PX,PY,1.));
  }else{
    particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  }
  
  return 1;
}
