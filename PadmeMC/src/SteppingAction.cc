#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VProcess.hh"
#include "G4TrackStatus.hh"
#include "HistoManager.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "HistoManager.hh"
#include "MCTruthManager.hh"
#include "MCTruthParticle.hh"

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
  fMCTruthManager = MCTruthManager::GetInstance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// MR 26/11/2026 classify a physical volume by name only the first time it is seen
unsigned int SteppingAction::VolumeFlags(const G4VPhysicalVolume* pv)
{
  if (pv == fLastVolume) return fLastVolumeFlags; // pre and post volume are usually the same
  unsigned int flags = 0;
  auto it = fVolumeFlags.find(pv);
  if (it != fVolumeFlags.end()) {
    flags = it->second;
  } else {
    const G4String& n = pv->GetName();
    if (n == "Target")          flags |= kVolTarget;
    if (n == "TargetSupport")   flags |= kVolTargetSupport;
    if (n == "ECal")            flags |= kVolECal;
    if (n == "world_volume_PV") flags |= kVolWorldPV;
    if (n == "World")           flags |= kVolWorld;
    if (n == "SACCry")          flags |= kVolSACCry;
    if (n == "BeamSLTB2" || n == "BeamSLTB3" || n == "BeamSLTB4" || n == "BeamSLTB5" ||
        n == "LinacWall" || n == "BeamLineInLinacPipe" || n == "BeamLineInWallPipe") flags |= kVolKill;
    if (n == "BeamLineMylarWindow") flags |= kVolMylarFoil; // MR 27/11/2026 foil only, not "BeamLineMylarWinFlange"
    fVolumeFlags[pv] = flags;
  }
  fLastVolume = pv;
  fLastVolumeFlags = flags;
  return flags;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // MR 26/11/2026 volumes are identified with cached flags instead of string comparisons
  const G4VPhysicalVolume* preVolume  = step->GetPreStepPoint()->GetPhysicalVolume();
  const G4VPhysicalVolume* postVolume = step->GetPostStepPoint()->GetPhysicalVolume(); // 0 when leaving the world
  const unsigned int preFlags  = VolumeFlags(preVolume);
  const unsigned int postFlags = postVolume ? VolumeFlags(postVolume) : 0;

  // MR 27/11/2026 tag the primaries that emit bremsstrahlung in the Mylar foil of the BTF window:
  // the energies of the eBrem photons are summed per track and written in beam_output.txt
  // (see EventAction::WriteBeamOutLines). A discrete interaction happens inside the pre-step volume.
  // Same for the ionisation: kinetic energies of the eIoni delta electrons.
  if ((preFlags & kVolMylarFoil) && step->GetTrack()->GetParentID() == 0) {
    const G4VProcess* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
    if (proc) {
      const G4String& pname = proc->GetProcessName();
      if (pname == "eBrem") {
        G4double k = 0.;
        for (const G4Track* sec : *step->GetSecondaryInCurrentStep())
          if (sec->GetDefinition() == G4Gamma::GammaDefinition()) k += sec->GetKineticEnergy();
        fMylarBremK[step->GetTrack()->GetTrackID()] += k;
      } else if (pname == "eIoni") {
        G4double k = 0.;
        for (const G4Track* sec : *step->GetSecondaryInCurrentStep())
          if (sec->GetDefinition() == G4Electron::ElectronDefinition()) k += sec->GetKineticEnergy();
        fMylarIoniK[step->GetTrack()->GetTrackID()] += k;
      }
    }
  }

  // Save MCTruth kinematics for physics processes (Bremsstrahlung, Bhabha, Annihilation) in Target
  if (fMCTruthManager->IsEnabled()) {
//     if (step->GetPostStepPoint()->GetPhysicalVolume() != 0){ 
//       //(step->GetPostStepPoint()->GetPosition().x()*step->GetPostStepPoint()->GetPosition().x() + step->GetPostStepPoint()->GetPosition().y()*step->GetPostStepPoint()->GetPosition().y()) > 900){
//        G4Track* track = step->GetTrack();
//       G4String proc = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
// 	if (step->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "FaradayFR4" &&
//     step->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "FaradayCopper") {

    

//                 auto eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

// 				auto vtx = track->GetVertexPosition();

// 				G4cout << "taggoneM "
// 					<< eventID << " "
// 					<< track->GetTrackID() << " "
// 					<< track->GetParentID() << " "
// 					<< track->GetDefinition()->GetPDGEncoding() << " "
// 					<< vtx.x() << " " << vtx.y() << " " << vtx.z() << " "
// 					<< step->GetPostStepPoint()->GetMomentum().x() << " "
// 					<< step->GetPostStepPoint()->GetMomentum().y() << " "
// 					<< step->GetPostStepPoint()->GetMomentum().z()
// 					<< G4endl;
    
// }
//    // if ((track->GetDefinition()->GetPDGEncoding() ==22 && step->GetPostStepPoint()->GetTotalEnergy()/MeV >1) || track->GetDefinition()->GetPDGEncoding() != 22){
// //	  (track->GetDefinition()->GetPDGEncoding() == -11 && step->GetPostStepPoint()->GetTotalEnergy() < 200) ||
// //	  track->GetDefinition()->GetPDGEncoding() != -11){
// 	// if ((((step->GetPostStepPoint()->GetPosition().x()*step->GetPostStepPoint()->GetPosition().x() + step->GetPostStepPoint()->GetPosition().y()*step->GetPostStepPoint()->GetPosition().y()) >= 0 && track->GetDefinition()->GetPDGEncoding() ==-11) || track->GetDefinition()->GetPDGEncoding() !=-11) &&
// 	//     step->GetPreStepPoint() && step->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "ChamberECalWindow" &&
// 	//     step->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "world_volume_PV"){


// //	  G4cout << "Step da ecalwindow a vuoto " << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() <<
// //	    " trackPDG = " << track->GetDefinition()->GetPDGEncoding() <<
// //	    " trackPostStepTotalEnergy = " << step->GetPostStepPoint()->GetTotalEnergy()/MeV <<
// //	    " trackPostStepPosX = " << step->GetPostStepPoint()->GetPosition().x() <<
// //	    " trackPostStepPosY = " << step->GetPostStepPoint()->GetPosition().y() <<
// //	    " trackPostStepPosZ = " << step->GetPostStepPoint()->GetPosition().z() <<
// //	    " trackPostStepMomX = " << step->GetPostStepPoint()->GetMomentum().x() <<
// //	    " trackPostStepMomY = " << step->GetPostStepPoint()->GetMomentum().y() <<
// //	    " trackPostStepMomZ = " << step->GetPostStepPoint()->GetMomentum().z() <<
// //	    " process = " << proc << G4endl;
// 	//   G4cout << "taggone"
// 	// 	 << " " <<  ((G4Event*)G4RunManager::GetRunManager()->GetCurrentEvent())->GetEventID()
// 	// 	 << " " << track->GetDefinition()->GetPDGEncoding()
// 	// 	 << " " << step->GetPostStepPoint()->GetTotalEnergy()/MeV
// 	// 	 << " " << step->GetPostStepPoint()->GetPosition().x() 
// 	// 	 << " " << step->GetPostStepPoint()->GetPosition().y() 
// 	// 	 << " " << step->GetPostStepPoint()->GetPosition().z() 
// 	// 	 << " " << step->GetPostStepPoint()->GetMomentum().x() 
// 	// 	 << " " << step->GetPostStepPoint()->GetMomentum().y() 
// 	// 	 << " " << step->GetPostStepPoint()->GetMomentum().z() 
// 	// 	 << G4endl;
// 	}
// //	else if (
// //		 step->GetPreStepPoint() && step->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "world_volume_PV" &&
// //		 step->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "ChamberECalWindow"){
// //	  G4cout << "Step da vuoto ad ecalwindow " << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() <<
// //	    " trackPDG = " << track->GetDefinition()->GetPDGEncoding() <<
// //	    " trackPostStepTotalEnergy = " << step->GetPostStepPoint()->GetTotalEnergy()/MeV <<
// //	    " trackPostStepPosX = " << step->GetPostStepPoint()->GetPosition().x() <<
// //	    " trackPostStepPosY = " << step->GetPostStepPoint()->GetPosition().y() <<
// //	    " trackPostStepPosZ = " << step->GetPostStepPoint()->GetPosition().z() <<
// //	    " trackPostStepMomX = " << step->GetPostStepPoint()->GetMomentum().x() <<
// //	    " trackPostStepMomY = " << step->GetPostStepPoint()->GetMomentum().y() <<
// //	    " trackPostStepMomZ = " << step->GetPostStepPoint()->GetMomentum().z() <<
// //	    " process = " << proc << G4endl;
// //	}
//     //  }
// 	//}
   
    if (postVolume != 0 && ((postFlags & kVolTarget) || (preFlags & kVolTargetSupport)) ) { // TargetSupport is from preStep
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

    // const std::vector<const G4Track*> *sec = step->GetSecondaryInCurrentStep();
	  // size_t nSec = (*sec).size();
     //if (proc != "Transportation") std::cout<< "mctruth proc:" << proc << " trackPDG:" << track->GetDefinition()->GetPDGEncoding() << " nSec:" << nSec <<" TrackID:" << track->GetTrackID() << std::endl;


      if ( (proc == "eBrem") || (proc == "eIoni") ) {
        //const G4TrackVector* sec = step->GetSecondary();
        const std::vector<const G4Track*> *sec = step->GetSecondaryInCurrentStep();
        size_t nSec = (*sec).size();
        if (nSec>0) {
          MCTruthVertex* tvtx = fMCTruthManager->AddVertex(proc,step->GetPostStepPoint()->GetPosition(),step->GetPostStepPoint()->GetGlobalTime());
          tvtx->AddParticleIn(track->GetDefinition()->GetPDGEncoding(),step->GetPreStepPoint()->GetTotalEnergy(),step->GetPreStepPoint()->GetMomentum());
          tvtx->AddParticleOut(track->GetDefinition()->GetPDGEncoding(),step->GetPostStepPoint()->GetTotalEnergy(),step->GetPostStepPoint()->GetMomentum());

	  // add G4TrackID of first secondary at this vertex
	  //TEST G4cout << "SteppingAction Created a vertex id = " << fMCTruthManager->GetNVertices()-1 << " proc = " << proc.data() << " trackid = " << step->GetTrack()->GetTrackID() << " volume = " << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << " secondaries " << nSec ;

	  fMCTruthManager->AddG4TrackSecondaryToVertex(step->GetTrack()->GetTrackID(),fMCTruthManager->GetNVertices()-1,0,0);
	  
	  for(size_t i = 0; i < nSec; i++) {
            tvtx->AddParticleOut((*sec)[i]->GetDefinition()->GetPDGEncoding(),(*sec)[i]->GetTotalEnergy(),(*sec)[i]->GetMomentum());
	    // cannot add G4TrackID of i-th secondary at this vertex in outgoing position i+1, BUT HERE THE GetTrackID() IS ZERO BECAUSE THE TRACK IS NOT ON STACK, WILL BE UPDATED IN STACKING ACTION
	    //	    fMCTruthManager->AddG4TrackSecondaryToVertex((*sec)[i]->GetTrackID(),fMCTruthManager->GetNVertices()-1,i+1,0);
	    // this is done in StackingAction
	    
	    //TEST G4cout << " secondary " << i << " secondary trackid " << (*sec)[i]->GetTrackID();
          }
	  //TEST G4cout << G4endl;

	  if (nSec!=1) printf("WARNING eBrem or eIoni with %d secondaries",(int)nSec);
        }
            }
            if (proc == "annihil") {
        //const G4TrackVector* sec = step->GetSecondary();
        const std::vector<const G4Track*> *sec = step->GetSecondaryInCurrentStep();
        size_t nSec = (*sec).size();
        if (nSec>0) {
          MCTruthVertex* tvtx = fMCTruthManager->AddVertex(proc,step->GetPostStepPoint()->GetPosition(),step->GetPostStepPoint()->GetGlobalTime());
          tvtx->AddParticleIn(track->GetDefinition()->GetPDGEncoding(),step->GetPreStepPoint()->GetTotalEnergy(),step->GetPreStepPoint()->GetMomentum());

	  //TEST G4cout << "SteppingAction Created a vertex id " << fMCTruthManager->GetNVertices()-1 << " proc = " << proc.data() << " trackid = " << step->GetTrack()->GetTrackID() << " volume = " << step->GetPostStepPoint()->GetPhysicalVolume()->GetName() << " secondaries " << nSec ;
          for(size_t i = 0; i < nSec; i++) {
            tvtx->AddParticleOut((*sec)[i]->GetDefinition()->GetPDGEncoding(),(*sec)[i]->GetTotalEnergy(),(*sec)[i]->GetMomentum());

	    // cannot add G4TrackID of i-th secondary at this vertex in outgoing position i, SINCE HERE THE GetTrackID() IS ZERO BECAUSE THE TRACK IS NOT ON STACK, WILL BE UPDATED IN STACKING ACTION
	    // fMCTruthManager->AddG4TrackSecondaryToVertex((*sec)[i]->GetTrackID(),fMCTruthManager->GetNVertices()-1,i,0);

	    //TEST G4cout << " secondary << " << i << " secondary trackid " << (*sec)[i]->GetTrackID();
          }
	  //TEST G4cout << G4endl;
          if (nSec!=2) printf("WARNING annihil with %d secondaries",(int)nSec);
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
    if(postVolume!=0){
      if(postFlags & kVolSACCry) {
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
  

  if(postVolume!=0){
    if((postFlags & kVolECal) && (preFlags & kVolWorldPV)) {
      for (int itype = 0; itype < 2; itype++){
	if (fMCTruthManager->IsG4TrackInVertex(track->GetTrackID(),itype)){
	  std::pair<G4int, G4int> vtxpos = fMCTruthManager->GetG4TrackInVertex(track->GetTrackID(),itype);
	  //	  std::cout << "G4TrackID " << track->GetTrackID() << " is in the vertex list type = " << itype;
	  //	  std::cout << " in position " << vtxpos.second << " for the vertex number " << vtxpos.first << std::endl;
	  G4int oldvtxid = vtxpos.first;
	  G4int oldvtxparticlepos = vtxpos.second;
	  MCTruthVertex* oldvtx = fMCTruthManager->Vertex(oldvtxid);
	  G4String oldvtxprocess = oldvtx->GetProcess();
	  MCTruthParticle* oldparticle = oldvtx->ParticleOut(oldvtxparticlepos);
	  
	  // create a new vertex with 1 particleIn and 1 particleOut
	  // vertex name "AtECal" and can be parsed
	  G4String processName = "AtECal_"+oldvtxprocess+"_VtxID_"+std::to_string(oldvtxid)+"_Outposition_"+std::to_string(oldvtxparticlepos)+"_type_"+std::to_string(itype);

	  //	  G4cout << "Creating vertex name " << processName << " particleIn " << oldparticle->GetPDGCode() << " Energy = " << oldparticle->GetEnergy() << " particleOut " <<
	  //	    track->GetDefinition()->GetPDGEncoding() << " Energy = " << track->GetTotalEnergy() << " old nvtx = " << fMCTruthManager->GetNVertices();
	  // vertex position = track position
	  MCTruthVertex* newvtx = fMCTruthManager->AddVertex(processName,track->GetPosition(),track->GetGlobalTime());
	  // particleIn momentum is the momentumOut of the vtx matched
	  //G4cout << " new nvtx " << fMCTruthManager->GetNVertices() << G4endl;
	  newvtx->AddParticleIn(oldparticle->GetPDGCode(),oldparticle->GetEnergy(),oldparticle->GetMomentum());
	  // particleOut is the present track momentum
	  newvtx->AddParticleOut(track->GetDefinition()->GetPDGEncoding(),track->GetTotalEnergy(),track->GetMomentum());
	}
      }
    }
  }
  
  //Analyze ECal tracks
  if (fEnableECalAnalysis) {
    if(postVolume!=0){
      if((postFlags & kVolECal) && (preFlags & kVolWorld)) {
	  
	


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
	  if(!(preFlags & kVolTarget) && (postFlags & kVolTarget)) {
	    PositronMomentum = G4ThreeVector(-1000,-1000,-1000);
	    nt = 0;
	    
	  }
	  
	  if((preFlags & kVolTarget) && !(postFlags & kVolTarget)) {
	    // G4cout << " Getting the momentum:   " << track->GetMomentum() << G4endl;
	    PositronMomentum = G4ThreeVector(track->GetMomentum());
	  }
	  
    	  if(postFlags & kVolTarget) {
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
		//              G4cout<<" else Process: "<<lastProc<< " code " << ProcID << " " <<BeamPartE<<G4endl;
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
  // Killing in the collimators (BeamSLTB2, BeamSLTB3, BeamSLTB4 and BeamSLTB5 region)
  // and in the walls and beamlines (LinacWall, BeamLineInLinacPipe, BeamLineInWallPipe):
  // see VolumeFlags() for the list of volumes
  if(postVolume!=0 && (postFlags & kVolKill)){
    track->SetTrackStatus(fStopAndKill);
  }

//*************************************
// Checking flag 10
//*************************************
  // MR 26/11/2026 the check on "BeamLineBeamFlag10" did nothing (kill commented out): removed.
  // To kill there, add the name to kVolKill in VolumeFlags().
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

