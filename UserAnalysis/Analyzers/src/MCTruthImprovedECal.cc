#include "MCTruthImprovedECal.hh"
#include "TLorentzVector.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "TMath.h"
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>



//Added the Finstance to allow to use the information in all the enalyzers
MCTruthImprovedECal* MCTruthImprovedECal::fInstance = 0;
MCTruthImprovedECal* MCTruthImprovedECal::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new MCTruthImprovedECal(); }
  return fInstance;
}

MCTruthImprovedECal::MCTruthImprovedECal(TString cfgFile, Int_t verbose)
{
  //  Neve=0;
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating MCTruthImprovedECal\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
}

MCTruthImprovedECal::~MCTruthImprovedECal(){
  delete fCfgParser;
  //delete fRndm;
}

Bool_t MCTruthImprovedECal::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing MCTruthImprovedECal\n");
  fEvent = event;
  fECal_clEvent = fEvent->ECalRecoCl; 
  fGeneralInfo = GeneralInfo::GetInstance();
  fXMin = -21.*(14+0.5);
  fXMax = 21.*(14+0.5);
  fXW = 21; // mm
  fNXBins = (fXMax-fXMin)/fXW;
  fYMin = -21.*(14+0.5);
  fYMax = 21.*(14+0.5);
  fYW = 21; // mm
  fNYBins = (fYMax-fYMin)/fYW;

  TString mapname = "Analyzers/ECal_map_2.txt";
  

  std::ifstream MapFile(mapname.Data());
  std::string line;
  if(MapFile.is_open()){
    int SU,px,py,BN,CH;
    while(getline(MapFile,line)){
      std::stringstream(line) >> SU >> px >> py >> BN >> CH;
      fCellMap[100*px+py] = 1;
      
      }
  }else{
    std::cout<<"file 'Analyzers/ECal_map_2.txt' not found -->check your paths"<<std::endl;
    exit(1);
  }

  InitHistos();

  
  fProcessIDsMap={{"eIoni",0}, {"eBrem",1}, {"annihil",2}, {"Bhabha",3}, {"Babayaga",4}, {"NoVtx",5}, {"BabayagaGG",6}};

  
  return true;
}

Bool_t MCTruthImprovedECal::InitHistos(){
  //MCTruthImprovedECal directory will contain all histograms related to this analysis
  //Histograms for MCTruthImprovedECal information
  //Int_t    NBinE=600.;
  //Double_t MaxE=NBinE;

  fHS->CreateList("MCTruthImprovedECal");
  for (int i=0; i<2; i++){
    fHS->BookHistoList ("MCTruthImprovedECal",Form("DTCluVtx_type%d",i),100,-5,5.);
    fHS->BookHisto2List("MCTruthImprovedECal",Form("DXCluVtx_type%d",i),100,-300.,300.,100,-100.,100.);
    fHS->BookHisto2List("MCTruthImprovedECal",Form("DYCluVtx_type%d",i),100,-300.,300.,100,-100.,100.);

    fHS->BookHisto2List("MCTruthImprovedECal",Form("YvsX_type%d",i),fNXBins,fXMin,fXMax,fNYBins,fYMin,fYMax);
    fHS->BookHisto2List("MCTruthImprovedECal",Form("YvsX_weightdx_type%d",i),fNXBins,fXMin,fXMax,fNYBins,fYMin,fYMax);
    fHS->BookHisto2List("MCTruthImprovedECal",Form("YvsX_weightdy_type%d",i),fNXBins,fXMin,fXMax,fNYBins,fYMin,fYMax);
    fHS->BookHisto2List("MCTruthImprovedECal",Form("EinVsEout_type%d",i),100,0.,300.,100,0.,300.);
  }
  return true;
}

Bool_t MCTruthImprovedECal::Process(){
  // MCTruthImprovedECal analysis
  
  // particle codes: 22 photon 11 electrons positron -11
  if(fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)){
    //std::cout<<"Event: "<<fEvent->RecoEvent->GetEventNumber()<<std::endl;
    CorrelateVtxClu();
    return true;
  }else{
    return false;
  }
}



Bool_t MCTruthImprovedECal::CorrelateVtxClu(){
  CluVtxCorr.clear(); //cluster-vertex couple 1-1. correspondence
  VtxCluCorr.clear(); //vertex-list of clusters associated to the vertex
  CluPcleCorr.clear();  //cluster-list of particles candidates (particle out from the vertex) couple
  CluECalVtxCorr.clear();//cluster-id of ECalVertex
  
//  TVector3 cluPos, VtxPos;
//  TRecoVCluster* clu;
//  double cluEnergy=0;
//  //double mcIPartE;
//  double VtxTime;
//  double cluTime =-99;
//  int NCluVtx    =0;
//  double cVal = 30.; //cm/ns
//
//  double TOFoffset = 13.6; //ns --> TOF offset moving towards the calorimeter //just assigned default value, evaluated precisely later in the code
//  const double DTlow     = -2.00;//4.0; //from fit on DT
//  const double DTup      = 7.27;//6.0; //from fit on DT
//  const double muX       =  1.442; //0.868;//1.389; //from fit on DX
//  const double muY       = -0.0731;//-0.100; //from fit on DY
//  const double sigmaX    = 10.57;//7.232; //from fit on DX
//  const double sigmaY    = 10.5;//7.632; //from fit on DY


  //Loop on vertex
  for(Int_t iV = 0; iV < fEvent->MCTruthEvent->GetNVertices(); iV++) {
    TMCVertex* mcVtx = fEvent->MCTruthEvent->Vertex(iV);

    // parse format of process to match this: 	  G4String processName = "AtECal_"+oldvtxprocess+"_VtxID_"+std::to_string(oldvtxid)+"_Outposition_"+std::to_string(oldvtxparticlepos)+"_type_"+std::to_string(itype);
    TObjArray *tx = mcVtx->GetProcess().Tokenize("_");
    if (tx->GetLast() != 7) continue;
    if ( ((TObjString *)(tx->At(0)))->String() != "AtECal" ) continue;

    TString originalProcessName = ((TObjString *)(tx->At(1)))->String();
    auto it = fProcessIDsMap.find(originalProcessName.Data());
    if (it == fProcessIDsMap.end()){
      std::cout <<"MCTruthImprovedECal >> Unrecognized processname ---" << originalProcessName.Data() << "----" << std::endl;
      continue;
    }
    int originalvtxid = ((TObjString *)(tx->At(3)))->String().Atoi();
    // check if original vertex is present    
    if (originalvtxid < 0 || originalvtxid >= fEvent->MCTruthEvent->GetNVertices()) {
      std::cout <<"MCTruthImprovedECal >> Original vtx id out of bound ---" << originalvtxid << "----" << std::endl;
      continue;
    }
    TMCVertex* mcVtxOriginal = fEvent->MCTruthEvent->Vertex(originalvtxid);

    // retrieve pcleout
    int originalpcleoutid = ((TObjString *)(tx->At(5)))->String().Atoi();
    // check if original plcout index is ok
    if (originalpcleoutid < 0 || originalpcleoutid >= mcVtxOriginal->GetNParticleOut() ){
      std::cout <<"MCTruthImprovedECal >> Original vtx pcleout id out of bound ---" << originalpcleoutid << "----" << std::endl;
      continue;
    }
    
    int ecalvtxtype = ((TObjString *)(tx->At(7)))->String().Atoi();
    if (ecalvtxtype != 0 && ecalvtxtype != 1) {
      std::cout <<"MCTruthImprovedECal >> ecalvtx type out of bound ---" << ecalvtxtype << "----" << std::endl;
      continue;
    }
    
    // retrieve position and momentum/energy of particle out at ECal
    TVector3 ecalvtxPosition = mcVtx->GetPosition();
    TLorentzVector ecalvtxPcleMom(mcVtx->ParticleOut(0)->GetMomentum(), mcVtx->ParticleOut(0)->GetEnergy());
    int ecalvtxPclePDGCode = mcVtx->ParticleOut(0)->GetPDGCode();
    double ecalVtxTime = mcVtx->GetTime();
    // retrieve momentum/energy of particle incoming (the original)
    TLorentzVector ecalvtxPcleInMom(mcVtx->ParticleIn(0)->GetMomentum(), mcVtx->ParticleIn(0)->GetEnergy());
    
    // match with ECal clusters
    Int_t cluIdx =-1;
    //Loop on clu
    for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
      TRecoVCluster* clu = fECal_clEvent->Element((int)h1);
      double cluEnergy = clu->GetEnergy();
      double cluTime = clu->GetTime();
      TVector3 cluPos(clu->GetPosition().X(),clu->GetPosition().Y(),fGeneralInfo->GetCOG().Z()); 
//      TVector3 cluDir = cluPos-fGeneralInfo->GetTargetPos(); // direction of the cluster wrt the target
//      cludir *= (1./cluDir.Mag());
      TVector3 cluDiff = cluPos-ecalvtxPosition; // vector "difference": from the true ecal position to the cluster
//      double cluDelta = cluDiff.Dot(cluDir); // projection of the vector "difference" along the cluster direction
//      TVector3 cluDiffOrt(cluDiff.X(),cluDiff.Y(),cluDiff.Z());
//      cluDiffOrt -= cluDelta*cluDir; // CDA vector: difference component ortogonal to the cluster "direction"
      fHS->FillHistoList ("MCTruthImprovedECal",Form("DTCluVtx_type%d",ecalvtxtype),cluTime-ecalVtxTime,1.);
      fHS->FillHisto2List("MCTruthImprovedECal",Form("DXCluVtx_type%d",ecalvtxtype),cluPos.X(),cluPos.X()-ecalvtxPosition.X(),1.);
      fHS->FillHisto2List("MCTruthImprovedECal",Form("DYCluVtx_type%d",ecalvtxtype),cluPos.Y(),cluPos.Y()-ecalvtxPosition.Y(),1.);
      if (TMath::Abs(cluTime-ecalVtxTime) < 5. && cluDiff.Perp() < 15) {
	fHS->FillHisto2List("MCTruthImprovedECal",Form("YvsX_type%d",ecalvtxtype),cluPos.X(),cluPos.Y(),1.);
	fHS->FillHisto2List("MCTruthImprovedECal",Form("YvsX_weightdx_type%d",ecalvtxtype),cluPos.X(),cluPos.Y(),cluPos.X()-ecalvtxPosition.X());
	fHS->FillHisto2List("MCTruthImprovedECal",Form("YvsX_weightdy_type%d",ecalvtxtype),cluPos.X(),cluPos.Y(),cluPos.Y()-ecalvtxPosition.Y());
	fHS->FillHisto2List("MCTruthImprovedECal",Form("EinVsEout_type%d",ecalvtxtype),ecalvtxPcleMom.E(),ecalvtxPcleInMom.E(),1.);
      }
    } //ECal cluster loop
  } //MCTruth Vtx Loop
      

  return true;



}


Int_t MCTruthImprovedECal::GetVtxFromCluID(Int_t CluId){
  map<Int_t, Int_t>::iterator it;
  it = CluVtxCorr.find(CluId);
  if(it!= CluVtxCorr.end()){
    return it->second;
  }else{
    return -1;
  }  
}
  
std::vector<Int_t> MCTruthImprovedECal::GetClusFromVtx(Int_t VtxId){
  map<Int_t, std::vector<Int_t>>::iterator it;
  it = VtxCluCorr.find(VtxId);
  if(it!= VtxCluCorr.end()){
    return it->second;
  }else{
    return {}; //returns empty vector if no clusters are associated with the vertex
  }  
}

std::vector<Int_t> MCTruthImprovedECal::GetPcleFromCluID(Int_t CluId){
map<Int_t, std::vector<Int_t>>::iterator it;
    it = CluPcleCorr.find(CluId);
    if(it!= CluPcleCorr.end()){
      return it->second;
    }else{
      return {}; //returns empty vector if no particles are associated with the cluster
    }  
  }

Int_t MCTruthImprovedECal::GetCluFromPcle(Int_t VtxId, Int_t PcleId){
  
  std::map<Int_t, std::vector<Int_t>>::iterator it;
  it = VtxCluCorr.find(VtxId);
  if(it!= VtxCluCorr.end()){
    std::vector<Int_t> associatedClusters = it->second;
    for(Int_t cluId : associatedClusters){
      std::map<Int_t, std::vector<Int_t>>::iterator pcleIt;
      pcleIt = CluPcleCorr.find(cluId);
      if(pcleIt != CluPcleCorr.end()){
        std::vector<Int_t> associatedParticles = pcleIt->second;
        if(std::find(associatedParticles.begin(), associatedParticles.end(), PcleId) != associatedParticles.end()){
          return cluId; //returns the cluster ID associated with the given vertex and particle
        }
      }
    }
  }
  return -1; //returns -1 if no cluster is found for the given vertex and particle

} //returns the cluster associated to a vertex and a particle out from that vertex




Bool_t MCTruthImprovedECal::Finalize()
{
  
  if (fVerbose) printf("---> Finalizing MCTruthImprovedECal\n");
  return 1;
}



