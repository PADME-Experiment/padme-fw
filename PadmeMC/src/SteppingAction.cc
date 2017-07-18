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
//#include "MyEvent.hh"
#include "Constants.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction()
{ 

  fEventAction = (EventAction*) G4RunManager::GetRunManager()->GetUserEventAction(); 
  fSACEnergyThr=5*MeV;
  bpar = BeamParameters::GetInstance();

  // Analyses are disabled by default
  fEnableSACAnalysis = 0;
  fEnableECalAnalysis = 0;
  fHistoManager = HistoManager::GetInstance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{ 
  static int nt=0;
//  //Devi capire che fare con il monitor e che cosa succede con piu' di un interazione!!!!   |-------|
  //printf("Stepping\n");
  // return; // Uncomment to disable MC analysis
  G4Track* track = step->GetTrack();
  //MySimEvent *evt = (MyEvent::GetInstance())->GetSimEvent();  
  
//  if(0){
//    //Storing everything that comes out of the target
//    if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){
//      if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="Target") {
//	G4String lastProc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
//	const G4TrackVector * trVec = step->GetSecondary ();
//	//std::cout << "Number of secondaries produced:  " << trVec->size() << "  Process: " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << std::endl;
//	
//	//      std::vector<G4Track*>::iterator  it;
//	
//	for(int i=0;i<trVec->size();i++) {
//	  G4Track *trk = trVec->at(i);
//	  //G4cout << "Particle type:  " << trk->GetDefinition()->GetParticleName () << G4endl;
//	  //Get the info for the secondary:
//	  
//	  MyParticle part(trk->GetDefinition()->GetPDGMass ());
//	  part.setProcess(step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName().data());
//	  part.setType(trk->GetDefinition()->GetParticleName ().data());
//	  part.setEnergy(trk->GetTotalEnergy());
//	  part.calcMomentum(trk->GetMomentumDirection ().x(),
//			    trk->GetMomentumDirection ().y(),
//			    trk->GetMomentumDirection ().z());
//	  part.setPVtx(trk->GetPosition ().x(),
//		       trk->GetPosition ().y(),
//		       trk->GetPosition ().z());
//	  evt->AddParticle(part);
//	}
//      }
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
  

  //Analyze ECal tracks
  if (fEnableECalAnalysis) {
    if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){
      if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="ECal" && 
	 step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="World") {
      
	////PRENDI LE VARIABILI DALLA GEOMETRIA PLZ!!!!!!!!!!!!!!!!!!!!!
	if(abs(step->GetPostStepPoint()->GetPosition().x())>5.*cm ||    
	   abs(step->GetPostStepPoint()->GetPosition().y())>5.*cm){
	  fEventAction->AddCalHitsStep(track->GetKineticEnergy(),
				       track->GetGlobalTime(),
				       ClassifyTrack(step->GetTrack()),
				       step->GetPostStepPoint()->GetPosition().x(),
				       step->GetPostStepPoint()->GetPosition().y()
				       );
	
	  //	    G4cout << "Next volume " <<  step->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() <<" "<<track->GetKineticEnergy()<<
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
  if(bpar->GetNPositronsPerBunch()==1){
    if(track->GetTrackID()==1){ //primary particle
      if(track->GetParticleDefinition() == G4Positron::PositronDefinition()){
	if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){ 
	  prevProc = lastProc;
	  lastProc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
	  if(lastProc == "annihil" ||lastProc == "eBrem") {
	    nt++;
	  }

	  if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="Target" 
	     && step->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "Target" && nt>1) {
	    G4cout << step->GetPreStepPoint()->GetPhysicalVolume()->GetName() 
		   << "  ----  > " 
		   << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
	    G4cout << " Getting the momentum:   " << track->GetMomentum() 
		   << "     "   << prevProc << "      " << lastProc << G4endl;
	  }
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
	    if(lastProc!="Transportation" && lastProc!="eIoni" && ProcID==-1){
	      //	  G4cout<<lastProc<<G4endl;
	      if(lastProc=="eBrem"){
		ProcID=1.;
		//		G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
	      }else if(lastProc=="annihil"){
		ProcID=2.;
		//		G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
	      }else if(lastProc=="eIoni"){
		ProcID=3.;
		//		G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
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

//  if (step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary){
//    if(track->GetVolume()->GetName()=="SAC") {
//      track->SetTrackStatus(fStopAndKill);
//    }
//  }
//  if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){			
//    if(step->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="Tracker") {
//      if(track->GetParticleDefinition()==G4Positron::PositronDefinition()||track->GetParticleDefinition()==G4Electron::ElectronDefinition()){
//	if(track->GetParticleDefinition()->GetPDGCharge()==1.){ 
//	  //  G4cout<<<<" " <<track->GetTrackID()<<G4endl;
//	  PositronE = track->GetTotalEnergy();					    
//          G4ThreeVector PositronDir = track->GetDynamicParticle()->GetMomentumDirection();  
//	  G4cout<<PositronE<<" e+ "<<PositronDir<<G4endl;                                      
//	  track->SetTrackStatus(fStopAndKill); 	       
//	}
//	else if(track->GetParticleDefinition()->GetPDGCharge()==-1.){
//	  //	  G4cout<<"electron "<<track->GetTotalEnergy()<<" " <<track->GetTrackID()<<G4endl;  
//	  ElectronE = track->GetTotalEnergy();					    
//	  G4ThreeVector ElectronDir = track->GetDynamicParticle()->GetMomentumDirection();  
//	  G4cout<<ElectronE<<" e- "<<ElectronDir<<G4endl;                                      
//	  track->SetTrackStatus(fStopAndKill);
//	}
//      }
//    }
//  }

//  Search for gamma leaving the target
  if(NPrimaries>0){
    //Cerca il primo gamma in uscita
    if(track->GetParticleDefinition() == G4Gamma::GammaDefinition()){
      if(step->GetPostStepPoint()->GetPhysicalVolume()!=0){
	if(step->GetPreStepPoint()->GetPhysicalVolume()->GetName()=="Target") {
	  if(track->GetCurrentStepNumber()==1 && track->GetParentID()==1){
	    //      G4cout<<"track->GetParticleDefinition() "<<track->GetParticleDefinition()<<G4endl;
	    //	  G4String lastProc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
	    //	  G4int parent      = track->GetParentID();
	    //	  G4int TrID        = track->GetTrackID();
	    //	  G4bool Primo      = step->IsFirstStepInVolume();
	    GammaE    = track->GetTotalEnergy();
	    //	  VertexPos = track->GetVertexPosition();
	    GammaDir  = track->GetVertexMomentumDirection();
	    G4ThreeVector BeamDir;
	    BeamDir[0]=0.;
	    BeamDir[1]=0.;
	    BeamDir[2]=1.;
	    ThetaGamma = SetGammaAngle(GammaDir,BeamDir);
	    if(ThetaGamma<0.015) fHistoManager->FillHisto(18,GammaE);
	    if(ProcID==1) fHistoManager->FillHisto2(38,ThetaGamma,GammaE);
	    if(ProcID==2) fHistoManager->FillHisto2(39,ThetaGamma,GammaE);
	    //  for(int i=0; i<4; i++) P4Miss[i]=TargetEleMom[i]+BeamMom[i]-GMom[i];
	    //  G4double Mmiss2 = P4Miss[3]*P4Miss[3]-P4Miss[2]*P4Miss[2]-P4Miss[1]*P4Miss[1]-P4Miss[0]*P4Miss[0];
	    //  NChild++;
	    //	    if(ThetaGamma) G4cout<<"Theta Gamma " <<ThetaGamma<<" "<<GammaE<<" "<<BeamPartE<<" "<<GammaE+BeamPartE<<G4endl;
	    
	  }
	}
      }
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

