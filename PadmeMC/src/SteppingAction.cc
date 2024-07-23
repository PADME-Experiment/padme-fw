#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
#include "G4TrackStatus.hh"
#include "HistoManager.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "HistoManager.hh"
#include "MCTruthManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction()
{ 
  fEventAction = (EventAction*) G4RunManager::GetRunManager()->GetUserEventAction(); 
  fSACEnergyThr=5*MeV;
  bpar = BeamParameters::GetInstance();

  // Analyses are disabled by default
  fEnableSACAnalysis = 0;
  fEnableECalAnalysis = 1;
  fHistoManager = HistoManager::GetInstance();
  fMCTruthManager = MCTruthManager::GetInstance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{

  // Save MCTruth kinematics for physics processes (Bremsstrahlung, Bhabha, Annihilation) in Target
  if (fMCTruthManager->IsEnabled()) {
    if (step->GetPostStepPoint()->GetPhysicalVolume() != 0 && step->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "Target") {
      G4Track* track = step->GetTrack();
      G4String proc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
      // Enable for some debug printout
      //if (proc != "Transportation") {
      //	G4cout << "Vertex in Target with process " << proc << " at " << step->GetPostStepPoint()->GetPosition() << G4endl;
      //	printf("\tIn\t%s\tE = %.3f MeV\n",track->GetDefinition()->GetParticleName().data(),step->GetPreStepPoint()->GetTotalEnergy()/MeV);
      //	printf("\tOut\t%s\tE = %.3f MeV\n",track->GetDefinition()->GetParticleName().data(),step->GetPostStepPoint()->GetTotalEnergy()/MeV);
      //	const std::vector<const G4Track*> *sec = step->GetSecondaryInCurrentStep();
      //	size_t nSec = (*sec).size();
      //	for(size_t i = 0; i < nSec; i++) {
      //	    printf("\tOut\t%s\tE = %.3f MeV\n",(*sec)[i]->GetDefinition()->GetParticleName().data(),(*sec)[i]->GetTotalEnergy()/MeV);
      //	}
      //	if ( ((proc == "eBrem") || (proc == "eIoni")) && (nSec != 1) )
      //	  printf("WARNING eBrem or eIoni with %d secondaries\n",nSec);
      //	if ( (proc == "annihil") && (nSec != 2) )
      //	  printf("WARNING annihil with %d secondaries\n",nSec);
      //	if ( (proc != "eBrem") && (proc != "eIoni") && (proc != "annihil" && (proc != "msc")) )
      //	  printf("INFO found process %s\n",proc.data());
      //}
      if ( (proc == "eBrem") || (proc == "eIoni") ) {
	//const G4TrackVector* sec = step->GetSecondary();
	const std::vector<const G4Track*> *sec = step->GetSecondaryInCurrentStep();
	size_t nSec = (*sec).size();
	if (nSec>0) {
	  MCTruthVertex* tvtx = fMCTruthManager->AddVertex(proc,step->GetPostStepPoint()->GetPosition(),step->GetPostStepPoint()->GetGlobalTime());
	  tvtx->AddParticleIn(track->GetDefinition()->GetPDGEncoding(),step->GetPreStepPoint()->GetTotalEnergy(),step->GetPreStepPoint()->GetMomentum());
	  tvtx->AddParticleOut(track->GetDefinition()->GetPDGEncoding(),step->GetPostStepPoint()->GetTotalEnergy(),step->GetPostStepPoint()->GetMomentum());
	  for(size_t i = 0; i < nSec; i++) {
	    tvtx->AddParticleOut((*sec)[i]->GetDefinition()->GetPDGEncoding(),(*sec)[i]->GetTotalEnergy(),(*sec)[i]->GetMomentum());
	  }
	  if (nSec!=1) printf("WARNING eBrem or eIoni with %d secondaries",nSec);
	}
      }
      if (proc == "annihil") {
	//const G4TrackVector* sec = step->GetSecondary();
	const std::vector<const G4Track*> *sec = step->GetSecondaryInCurrentStep();
	size_t nSec = (*sec).size();
	if (nSec>0) {
	  MCTruthVertex* tvtx = fMCTruthManager->AddVertex(proc,step->GetPostStepPoint()->GetPosition(),step->GetPostStepPoint()->GetGlobalTime());
	  tvtx->AddParticleIn(track->GetDefinition()->GetPDGEncoding(),step->GetPreStepPoint()->GetTotalEnergy(),step->GetPreStepPoint()->GetMomentum());
	  for(size_t i = 0; i < nSec; i++) {
	    tvtx->AddParticleOut((*sec)[i]->GetDefinition()->GetPDGEncoding(),(*sec)[i]->GetTotalEnergy(),(*sec)[i]->GetMomentum());
	  }
	  if (nSec!=2) printf("WARNING annihil with %d secondaries",nSec);
	}
      }
    }
  }

  static int nt=0;

  G4Track* track = step->GetTrack();

//  if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){
//    if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="Q1Quad") {
//
//      G4cout <<"Q1 pre "<<step->GetPostStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition())<<G4endl;
//      G4cout <<"Q1 post "<<step->GetPostStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition())<<G4endl;
//      
//      //<<" "<<
//      //	step->GetPostStepPoint()->GetPosition().y()<<" "<<step->GetPostStepPoint()->GetPosition().z()<<G4endl;
//
//    }
//  }

//  if(step->GetPreStepPoint()->GetPhysicalVolume()!=0){
//    if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="Q1Quad") {
//
//      G4cout <<"Q1 pre "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition())<<G4endl;
//      G4cout <<"Q1 post "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition())<<G4endl;
//      //      G4cout<<"BX = "<<3.*step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition().y()<<G4endl;
//      //G4cout<<"BY = "<<3.*step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition().x()<<G4endl;
//      //<<" "<<
//      //	step->GetPostStepPoint()->GetPosition().y()<<" "<<step->GetPostStepPoint()->GetPosition().z()<<G4endl;
//
//    }
//  }
//
//
//  if(step->GetPreStepPoint()->GetPhysicalVolume()!=0){
//    if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="Q2Quad") {
//
//      G4cout <<"Q2 pre "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition())<<G4endl;
//      G4cout <<"Q2 post "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition())<<G4endl;
//      
//      //<<" "<<
//      //	step->GetPostStepPoint()->GetPosition().y()<<" "<<step->GetPostStepPoint()->GetPosition().z()<<G4endl;
//
//    }
//  }
//
//  if(step->GetPreStepPoint()->GetPhysicalVolume()!=0){
//    if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="Q3Quad") {
//
//      G4cout <<"Q3 pre "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition())<<G4endl;
//      G4cout <<"Q3 post "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition())<<G4endl;
//      
//      //      G4cout <<"Q3 post world"<<" "<<step->GetPostStepPoint()->GetPosition()<<G4endl;
//
//    }
//  }
//
//
//  if(step->GetPreStepPoint()->GetPhysicalVolume()!=0){
//    if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="Q4Quad") {
//
//      G4cout <<"Q4 pre "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPreStepPoint()->GetPosition())<<G4endl;
//      G4cout <<"Q4 post "<<step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(step->GetPostStepPoint()->GetPosition())<<G4endl;
//      
//      //      G4cout <<"Q4 post world"<<" "<<step->GetPostStepPoint()->GetPosition()<<G4endl;
//    }
//  }

//Analyze SAC track
  if (fEnableSACAnalysis) {
    if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){
      if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="SACCry") {
	//      G4cout << "Next volume " <<  step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << " "<<track->GetKineticEnergy()<< G4endl;
	// G4cout << "E Thr " <<fSACEnergyThr<<G4endl;
	// processing hit when entering the volume SAC Cry
	if (track->GetKineticEnergy()>fSACEnergyThr)  
	  fEventAction->AddSACHitsStep(track->GetKineticEnergy(),
				       track->GetGlobalTime(),ClassifyTrack(step->GetTrack()),
				       step->GetPostStepPoint()->GetPosition().x(),
				       step->GetPostStepPoint()->GetPosition().y(),
				       step->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo()
				       );
      
	//      G4cout << "Next volume " <<  step->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() <<" "<<track->GetKineticEnergy()<<" X "<<step->GetPostStepPoint()->GetPosition().x()<<" Y "<<step->GetPostStepPoint()->GetPosition().y()<<" T "<<step->GetPostStepPoint()->GetGlobalTime()<<G4endl;
	track->SetTrackStatus(fStopAndKill);      
      }
    }
  }
  
  //G4cout<<"Ha tuka 1"<<std::endl;
  //Analyze ECal tracks
  if (fEnableECalAnalysis) {
    if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){
      //G4cout<<"Ha tuka 2 "<<step->GetPreStepPoint()->GetPhysicalVolume()->GetName()<<" "<<step->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<std::endl;
      if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="ECal" && 
	 step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="world_volume_PV"){ // was: World") {
        //G4cout<<"Ha tuka 3"<<std::endl;
	////PRENDI LE VARIABILI DALLA GEOMETRIA PLZ!!!!!!!!!!!!!!!!!!!!!
	if(abs(step->GetPostStepPoint()->GetPosition().x())>5.*cm ||    
	   abs(step->GetPostStepPoint()->GetPosition().y())>5.*cm){
	  //G4cout<<"Ha tuka 4"<<std::endl;//see if ECal hits are added at all
	  fEventAction->AddCalHitsStep(track->GetKineticEnergy(),
				       track->GetGlobalTime(),
				       ClassifyTrack(step->GetTrack()),
				       step->GetPostStepPoint()->GetPosition().x(),
				       step->GetPostStepPoint()->GetPosition().y()
				       );
	
	  //  G4cout << "Next volume " <<  step->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() <<" "<<track->GetKineticEnergy()<<
	  //	      " X "<<step->GetPostStepPoint()->GetPosition().x()<<" Y "<<step->GetPostStepPoint()->GetPosition().y()<<" T "<<step->GetPostStepPoint()->GetGlobalTime()<<G4endl;
	  //	track->SetTrackStatus(fStopAndKill);      
	}
      }
    }
  }

  //  G4cout<<"Primaries "<< bpar->GetNPositronsPerBunch() <<G4endl;

  static G4String lastProc="";
  static G4String prevProc="";
  
  //Cerca il primario
  //  if(bpar->GetNPositronsPerBunch()==1){
  if(bpar->GetNPositronsPerBunch()<50){
    if(track->GetTrackID()==1){ //primary particle
      if(track->GetParticleDefinition() == G4Positron::PositronDefinition()){
	if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){ 
	  prevProc = lastProc;
	  lastProc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
	  if(lastProc == "annihil" ||lastProc == "eBrem") {
	    nt++;
	  }
	  //	    G4cout << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
	  //		   << "  ----  > " 
	  //		   << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
	  //	    G4cout << " Getting the momentum:   " << track->GetMomentum() 
	  //		   << "     "   << prevProc << "      " << lastProc << G4endl;
	  ///	}
	  //Be somewhere
	  // G4cout << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
	  // 	 << "  ----> " 
	  // 	 << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
	  if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()!="Target" 
	     && step->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "Target") {
	    PositronMomentum = G4ThreeVector(-1000,-1000,-1000);
	    nt = 0;
	    
	  }
	  
	  if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="Target" 
	     && step->GetPostStepPoint()->GetPhysicalVolume()->GetName() != "Target") {
	    // G4cout << " Getting the momentum:   " << track->GetMomentum() << G4endl;
	    PositronMomentum = G4ThreeVector(track->GetMomentum());
	  }
	  
    	  if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="Target") {
	    //      G4cout<<"track->GetParticleDefinition() "<<track->GetParticleDefinition()<<G4endl;
	    G4String lastProc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
	    //	    if(lastProc!="Transportation" && lastProc!="eIoni" && ProcID==-1){
	    if(lastProc!="Transportation" && ProcID==-1){
	      //	  G4cout<<lastProc<<G4endl;
	      if(lastProc=="eBrem"){
		ProcID=1.;
		//		track->SetTrackStatus(fStopAndKill);
		//		G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
	      }else if(lastProc=="annihil"){
		ProcID=2.;
		//	       	track->SetTrackStatus(fStopAndKill);
		//		G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
	      }else if(lastProc=="eIoni"){
		ProcID=3.;
		//		track->SetTrackStatus(fStopAndKill);
		G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
	      }else{
		ProcID=-1.;
		//		G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
	      }
	      //   G4cout<<"Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
	    }
	    VertexPos = track->GetPosition();
	    //	  if(track->GetCurrentStepNumber()==1) BeamPartE  = track->GetTotalEnergy();
	    BeamPartE  = track->GetTotalEnergy();
	    //	  G4cout<<"Volume: "<<step->GetPostStepPoint()->GetPhysicalVolume()<<G4endl;
	    //	  G4cout<<"Volume: "<<step->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<G4endl;
	  }
	}
      }
    }
  }

  //*************************************
  // Killing particle in the LINAC region
  //*************************************
  if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){			
    //Killing in the collimators
    if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="BeamSLTB3" || step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="BeamSLTB4"||
       step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="BeamSLTB2"){
      //      G4cout<<"Killin particle in SLTB "<<step->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<G4endl;                                      
      track->SetTrackStatus(fStopAndKill);
    }
    //Killing in the walls and beamlines
    if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="LinacWall" || step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="BeamLineInLinacPipe"
       || step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="BeamLineInWallPipe"){
      //      G4cout<<"Killing particle in  "<<step->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<G4endl;                                      
      track->SetTrackStatus(fStopAndKill);
    }
  }
  
}

G4int SteppingAction::ClassifyTrack(G4Track* track){
  G4ParticleDefinition* particleType = track->GetDefinition();
  if ( particleType == G4Gamma::GammaDefinition() ) {
    return 1;
  } else if ( particleType == G4Positron::PositronDefinition() ) {
    return 2;
  } else if ( particleType == G4Electron::ElectronDefinition() ) {
    return 3;
  } else if ( particleType == G4Neutron::NeutronDefinition() ) {
    return 4;
  } else if ( particleType == G4PionPlus::PionPlusDefinition() || particleType == G4PionMinus::PionMinusDefinition() ) {
    return 5;
  } else if ( particleType == G4PionZero::PionZeroDefinition() ) {
    return 6;
  } else if ( particleType == G4MuonPlus::MuonPlusDefinition() || particleType == G4MuonMinus::MuonMinusDefinition() ) {
    return 7;
  } else return -1;
}

void SteppingAction::SetPhysProc(float value){
  ProcID=-1;
}

G4double SteppingAction::SetGammaAngle(G4ThreeVector GammaDir,G4ThreeVector BeamDir)
{
  G4double product=0;
  G4double Theta;
  for (int i=0; i<3; i++)  product+= GammaDir[i]*BeamDir[i];

  Theta = acos (product);// * 180.0 / 3.14159265;  //in degreees
  //  G4cout<<"product"<< product <<"Theta " <<Theta<<G4endl;
  return Theta;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

