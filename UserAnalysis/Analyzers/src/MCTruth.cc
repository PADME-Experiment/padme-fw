// Written by M. Raggi   28/03/2022 
// Last modified by M. Raggi 30/02/2022
#include "MCTruth.hh"

MCTruth::MCTruth(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating MCTruth\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

MCTruth::~MCTruth(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t MCTruth::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing MCTruth\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t MCTruth::InitHistos(){
  // MCTruth directory will contain all histograms related to this analysis
  // Histograms for MCTruth information
  fHS->CreateList("MCTruth");
  fHS->BookHistoList("MCTruth","Vertices",500,0.,500.); // Number of vertices in event
  fHS->BookHistoList("MCTruth","Vertex Type",10,0.,10.); // 0:eBrem - 1:eIoni - 2:annihil - 9:other
  fHS->BookHistoList("MCTruth","VertexNPart",10,0.,10.); // 0:eBrem - 1:eIoni - 2:annihil - 9:other
  fHS->BookHistoList("MCTruth","Vertex_Z",400,-4000.,0.);
  fHS->BookHistoList("MCTruth","Vertex_Z_all",400,-4000.,0.);
  fHS->BookHisto2List("MCTruth","Vertex_XY",100,-20.,20.,100,-20.,20.);

  fHS->BookHistoList("MCTruth","Bremstrahlung Gamma Energy",1000,0.,1000.);
  fHS->BookHistoList("MCTruth","Bremstrahlung Positron Energy",1000,0.,1000.);
  fHS->BookHistoList("MCTruth","Bremstrahlung Total Energy",1000,0.,1000.);
  fHS->BookHistoList("MCTruth","Bremstrahlung TotEDiff",650,-50.,600.);
  //  fHS->BookHistoList("MCTruth","Bremstrahlung TotEDiff",650,-50.,600.);
  fHS->BookHisto2List("MCTruth","Bremstrahlung EvsAn",200,0.,0.2,250,0.,500.);
  fHS->BookHisto2List("MCTruth","Bremstrahlung ThEPvsThG",200,0.,0.2,200,0.,0.2);
  
  // standard gg events
  fHS->BookHistoList("MCTruth","GG Gamma1 Energy",1000,0.,1000.);
  fHS->BookHistoList("MCTruth","GG Gamma2 Energy",1000,0.,1000.);
  fHS->BookHistoList("MCTruth","GG Total Energy",1000,0.,1000.);
  fHS->BookHistoList("MCTruth","GG TotEDiff",650,-50.,600.);

  fHS->BookHistoList("MCTruth","EE Positron Energy",1000,0.,1000.);
  fHS->BookHistoList("MCTruth","EE Electron Energy",1000,0.,1000.);
  
  return true;
}

Bool_t MCTruth::Process(){
  //Check if is MC or data
  Bool_t isMC = false;
  if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) {
    isMC=true;
    //    std::cout<<"input data are simulatetd "<<std::endl;
  }
  // MCTruth analysis
  // particle codes: 22 photon 11 electrons positron -11
  double EGamma,Eposi,ETot,mcIPartE;
  double Px,Py,Pz;
  
  if(fEvent->MCTruthEvent){
    fHS->FillHistoList("MCTruth","Vertices",fEvent->MCTruthEvent->GetNVertices(),1.);
    //    cout<<"N Particle Vert "<<fEvent->MCTruthEvent->GetNVertices()<<endl;
    if (fEvent->MCTruthEvent->GetNVertices()>0) {
      for(Int_t iV = 0; iV<fEvent->MCTruthEvent->GetNVertices(); iV++) {
	TMCVertex* mcVtx = fEvent->MCTruthEvent->Vertex(iV);
	//	cout<<"DDD "<<mcVtx->GetPosition().x()<<endl;
	fHS->FillHisto2List("MCTruth","Vertex_XY",mcVtx->GetPosition().x(),mcVtx->GetPosition().y(),1.);
	fHS->FillHistoList("MCTruth","Vertex_Z",mcVtx->GetPosition().z(),1.);
	fHS->FillHistoList("MCTruth","Vertex_Z_all",mcVtx->GetPosition().z()-2542.,1.);
	
	if (mcVtx->GetProcess() == "eBrem") {
	  fHS->FillHistoList("MCTruth","Vertex Type",0,1.);
	  for(Int_t iN = 0; iN<mcVtx->GetNParticleIn(); iN++) {
	    TMCParticle* mcIPart = mcVtx->ParticleIn(iN); //just One positron
	    mcIPartE=mcIPart->GetEnergy();
	    if(mcIPart->GetPDGCode() != -11) cout<<"Vertex with non positron incoming particle "<<mcIPart->GetPDGCode()<<" energy "<<mcIPart->GetEnergy()<<endl;
	  }
	  double PX_pos,PX_G,thetaX_G,thetaX_pos;
	  for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
	    TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
	    //	    cout<<"Particle Type "<<mcOPart->GetPDGCode()<<endl;
	    if (mcOPart->GetPDGCode() == 22) {
	      fHS->FillHistoList("MCTruth","Bremstrahlung Gamma Energy",mcOPart->GetEnergy(),1.);
	      EGamma=mcOPart->GetEnergy();
	      PX_G =mcOPart->GetMomentum().X();
	      thetaX_G = asin(PX_G/Eposi);	    
	    }
	    if (mcOPart->GetPDGCode() == -11) {
	      fHS->FillHistoList("MCTruth","Bremstrahlung Positron Energy",mcOPart->GetEnergy(),1.);
	      //	      fHS->FillHistoList("MCTruth","Bremstrahlung ",mcOPart->GetEnergy(),1.);
	      Eposi=mcOPart->GetEnergy();
	      Px=mcOPart->GetMomentum().X();
	      Py=mcOPart->GetMomentum().Y();
	      Pz=mcOPart->GetMomentum().Z();

	      PX_pos =mcOPart->GetMomentum().X();
	      thetaX_pos = asin(PX_pos/Eposi);	    
	      //	      cout<<"PX "<<mcOPart->GetMomentum().X()<<" "<<thetaX<<endl;
	      fHS->FillHisto2List("MCTruth","Bremstrahlung EvsAn",thetaX_pos,Eposi);
	    }
	    fHS->FillHisto2List("MCTruth","Bremstrahlung ThEPvsThG",thetaX_pos,thetaX_G);
	  }
	  ETot=EGamma+Eposi;
	  fHS->FillHistoList("MCTruth","Bremstrahlung Total Energy",ETot,1.);
	} else if (mcVtx->GetProcess() == "eIoni") {
	  fHS->FillHistoList("MCTruth","Vertex Type",1,1.);
	  for(Int_t iN = 0; iN<mcVtx->GetNParticleIn(); iN++) {
	    TMCParticle* mcIPart = mcVtx->ParticleIn(iN); //just One positron
	    mcIPartE=mcIPart->GetEnergy();
	  }
	  for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
	    TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
	    //	    //	    cout<<"Particle Type "<<mcOPart->GetPDGCode()<<endl;
	    if (mcOPart->GetPDGCode() == -11) fHS->FillHistoList("MCTruth","EE Positron Energy",mcOPart->GetEnergy(),1.);
	    if (mcOPart->GetPDGCode() == 11)  fHS->FillHistoList("MCTruth","EE Electron Energy",mcOPart->GetEnergy(),1.); 
	  }
	} else if (mcVtx->GetProcess() == "annihil") {	  
	  fHS->FillHistoList("MCTruth","Vertex Type",2,1.);
	  //	  std::vector<double> EGamma;
	  Int_t NPhot=0;
	  for(Int_t iN = 0; iN<mcVtx->GetNParticleIn(); iN++) {
	    TMCParticle* mcIPart = mcVtx->ParticleIn(iN); //just One positron
	    mcIPartE=mcIPart->GetEnergy();
	  }
	  for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
	    TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
	    //	    //	    cout<<"Particle Type "<<mcOPart->GetPDGCode()<<endl;
	    if (mcOPart->GetPDGCode() == 22) {
	      NPhot++;
	      if(NPhot==1) fHS->FillHistoList("MCTruth","GG Gamma1 Energy",mcOPart->GetEnergy(),1.);
	      if(NPhot==2) fHS->FillHistoList("MCTruth","GG Gamma2 Energy",mcOPart->GetEnergy(),1.);
	      //	      EGamma.push_back(mcOPart->GetEnergy());
	    }
	  }
	  //	  ETot=EGamma+Eposi;
	  //	  fHS->FillHistoList("MCTruth","Bremstrahlung Total Energy",ETot,1.);
	  //	  fHS->FillHistoList("MCTruth","Bremstrahlung TotEDiff",ETot-mcIPartE,1.);
	} else {
	  fHS->FillHistoList("MCTruth","Vertex Type",9,1.);
	}
      }
    }
  }

  return true;
}

Bool_t MCTruth::Finalize()
{
  std::cout<<"End of MC Truth        = "<<std::endl;
  if (fVerbose) printf("---> Finalizing MCTruth\n");
  return true;
}


