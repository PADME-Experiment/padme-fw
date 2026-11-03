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
    int SU,px,py,BN,CH, isdead;
    while(getline(MapFile,line)){
      std::stringstream(line) >> SU >> px >> py >> BN >> CH >> isdead;
      fCellMap[100*px+py] = isdead+1;
      if (isdead) fdeadcells.push_back(100*px+py);
      }
  }else{
    std::cout<<"file 'Analyzers/ECal_map_2.txt' not found -->check your paths"<<std::endl;
    exit(1);
  }

  fProcessIDsMap={{"eIoni",0}, {"eBrem",1}, {"annihil",2}, {"Bhabha",3}, {"Babayaga",4}, {"NoVtx",5}, {"BabayagaGG",6}};
  InitHistos();
  
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

  fpdgcodes[0] = -11;
  fpdgcodes[1] = 11;
  fpdgcodes[2] = 22; 
  std::map<TString, int>::iterator it;
  for (it = fProcessIDsMap.begin(); it != fProcessIDsMap.end(); it++){
    for (int i=0; i<3; i++){
      // efficiency to find an associated cluster Y vs X
      TString histoNameXY = Form("InAccYvsX_%s_%d",it->first.Data(),fpdgcodes[i]);
      fHS->BookHisto2List("MCTruthImprovedECal",histoNameXY.Data(),fNXBins,fXMin,fXMax,fNYBins,fYMin,fYMax);
      fHS->BookHisto2List("MCTruthImprovedECal",Form("%s_ass",histoNameXY.Data()),fNXBins,fXMin,fXMax,fNYBins,fYMin,fYMax);
      fHS->BookHisto2List("MCTruthImprovedECal",Form("%s_eff",histoNameXY.Data()),fNXBins,fXMin,fXMax,fNYBins,fYMin,fYMax);
      // efficiency to find an associated cluster E vs phi
      TString histoNameEQ = Form("InAccEvsPhi_%s_%d",it->first.Data(),fpdgcodes[i]);
      fHS->BookHisto2List("MCTruthImprovedECal",histoNameEQ.Data(),100,-TMath::Pi(),TMath::Pi(),40,0.,400.);
      fHS->BookHisto2List("MCTruthImprovedECal",Form("%s_ass",histoNameEQ.Data()),100,-TMath::Pi(),TMath::Pi(),40,0.,400.);
      fHS->BookHisto2List("MCTruthImprovedECal",Form("%s_eff",histoNameEQ.Data()),100,-TMath::Pi(),TMath::Pi(),40,0.,400.);
      // Distance true impact point vs reconstructed centroid given an associated cluster
      TString histoNamedRR = Form("InAccdRvsR_%s_%d",it->first.Data(),fpdgcodes[i]);
      TString histoNamedRDeadCell = Form("InAccdRvsdistanceToDeadCell_%s_%d",it->first.Data(),fpdgcodes[i]);
      fHS->BookHisto2List("MCTruthImprovedECal",Form("%s_ass",histoNamedRR.Data()),fNXBins,0.,fXMax,100,-200.,200.);
      fHS->BookHisto2List("MCTruthImprovedECal",Form("%s_ass",histoNamedRDeadCell.Data()),300,0.,fXMax,100,0.,300.);
      fHS->BookHisto2List("MCTruthImprovedECal",Form("%s_ass_noecalvtx",histoNamedRDeadCell.Data()),300,0.,fXMax,100,0.,300.);
    }
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
  VtxKeyMap.clear(); // vertex, MatchingStruct vector pair 
  CluKeyMap.clear(); // cluster,MatchingStruct vector pair 
  VtxPclePairKeyMap.clear(); // {vtx,pcle}, MatchingStruct vector pair
  VtxPclePairKeyMapToEcalvtx.clear(); // {vtx,pcle}, all ecal vertices pair
  
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
    insertMatchingVtxPcleToEcalvtx(iV,originalvtxid,originalpcleoutid); // fill all the maps, with keys: vtxid, cluid, {vtxid,particleid}

    // match with ECal clusters, loop on cluster list

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

	insertMatching(iV,h1,originalvtxid,originalpcleoutid,ecalvtxtype); // fill all the maps, with keys: vtxid, cluid, {vtxid,particleid}
      }
    } //ECal cluster loop
  } //MCTruth Vtx Loop
      
  //  DumpMapWithVtxKey();

  // to check the performance, must apply acceptance criteria

  //Loop on vertex
  for(Int_t iV = 0; iV < fEvent->MCTruthEvent->GetNVertices(); iV++) {
    TMCVertex* mcVtx = fEvent->MCTruthEvent->Vertex(iV);      
    TVector3 VtxPos = mcVtx->GetPosition();
    if (VtxPos.Z() > 0) continue; // skip vertices in the ECal

    std::map<TString,int>::iterator it = fProcessIDsMap.find(mcVtx->GetProcess());
    if (it == fProcessIDsMap.end()) {
      std::cout << "Process not found name = ---" << mcVtx->GetProcess().Data() << "---" << endl;
      continue;
    }
    
    for(Int_t iO = 0; iO<mcVtx->GetNParticleOut(); iO++) {
      TMCParticle* mcOPart = mcVtx->ParticleOut(iO);
      TVector3 pclePos =mcVtx->GetPosition();
      TVector3 pcleMom =mcOPart->GetMomentum();
      TLorentzVector pcle(pcleMom.X(),pcleMom.Y(),pcleMom.Z(),mcOPart->GetEnergy());
      TVector3 VtxPosAtCalo;
      VtxPosAtCalo.SetZ(fGeneralInfo->GetCOG().Z());//-72.8); //removed 6.5X0 faccia calorimetro
      VtxPosAtCalo.SetX(pclePos.X()+((pcleMom.X()/pcleMom.Z())*(VtxPosAtCalo.Z()-pclePos.Z())));
      VtxPosAtCalo.SetY(pclePos.Y()+((pcleMom.Y()/pcleMom.Z())*(VtxPosAtCalo.Z()-pclePos.Z())));      
      int pdgcode = mcVtx->ParticleOut(iO)->GetPDGCode();
      // acceptance condition: extrapolates to an ECal crystal
      int icellX = VtxPosAtCalo.X()/cellSize + ncells/2;
      int icellY = VtxPosAtCalo.Y()/cellSize + ncells/2;
      if(icellX>ncells || icellX<0) continue;
      if(icellY>ncells || icellY<0) continue;
      if(fCellMap[100*icellX+icellY]<=0) continue; // ECal map to remove holes and external parts

      fHS->FillHisto2List("MCTruthImprovedECal",Form("InAccYvsX_%s_%d",it->first.Data(),pdgcode),VtxPosAtCalo.X(),VtxPosAtCalo.Y());      
      fHS->FillHisto2List("MCTruthImprovedECal",Form("InAccEvsPhi_%s_%d",it->first.Data(),pdgcode),TMath::ATan2(VtxPosAtCalo.Y(),VtxPosAtCalo.X()),pcle.E());      

      // retrieve list of clusters associated to vertex from mctruth
      std::vector<Int_t> cluidvec = GetClusFromVtxPclePair(iV,iO);

      if (cluidvec.size()) {
        fHS->FillHisto2List("MCTruthImprovedECal",Form("InAccYvsX_%s_%d_ass",it->first.Data(),pdgcode),VtxPosAtCalo.X(),VtxPosAtCalo.Y());
        fHS->FillHisto2List("MCTruthImprovedECal",Form("InAccEvsPhi_%s_%d_ass",it->first.Data(),pdgcode),TMath::ATan2(VtxPosAtCalo.Y(),VtxPosAtCalo.X()),pcle.E());

        for (int ic = 0; ic < (int) cluidvec.size(); ic++){
          // quality of the reco clu
          TRecoVCluster* clu = fECal_clEvent->Element((int)cluidvec.at(ic));
          TVector3 cluPos(clu->GetPosition().X(),clu->GetPosition().Y(),fGeneralInfo->GetCOG().Z()); 
          TVector3 cluDir = cluPos-fGeneralInfo->GetTargetPos(); // direction of the cluster wrt the target
          cluDir *= (1./cluDir.Mag()); // normalize vector

          int ecalvtxid = GetECalVtxFromCluID(cluidvec.at(ic));
          if (ecalvtxid == -1) {
            std::cout << "MCTruthImprovedECal.cc: cannot retrieve ecalvtx id from cluster " << cluidvec.at(ic) << " " << ic << std::endl;
            continue;
          }
          TVector3 diffRecoTrue = cluPos;
          diffRecoTrue -= fEvent->MCTruthEvent->Vertex(ecalvtxid)->GetPosition();
          // component along reco distance
          double parallelComp = diffRecoTrue.Dot(cluDir);
          // ortogonal component
          TVector3 ortogonalComp = diffRecoTrue;
          ortogonalComp -= parallelComp*cluDir;
          double ortogonalDist = ortogonalComp.Mag();
          TVector3 crosso = ortogonalComp.Cross(cluDir);
          if (crosso.Z() > 0) ortogonalDist *= -1; // shift towards "closing" to the IP
          fHS->FillHisto2List("MCTruthImprovedECal",Form("InAccdRvsR_%s_%d_ass",it->first.Data(),pdgcode),cluPos.Perp(),ortogonalDist);
        }
      } 
      else {
        std::vector<Int_t> ecalvtxids = GetECalVtxFromVtxPclePair(iV,iO);
        Double_t distanceToDeadCell = 9999.0; // Initialize with a large value
        
        if (ecalvtxids.size() == 0) {

          for (std::vector<Int_t>::iterator itdead = fdeadcells.begin(); itdead != fdeadcells.end(); ++itdead) {
              int deadCellX = *itdead / 100;
              int deadCellY = *itdead % 100;
              double deadCellCenterX = (deadCellX - ncells/2 + 0.5) * cellSize;
              double deadCellCenterY = (deadCellY - ncells/2 + 0.5) * cellSize;
              TVector3 deadCellPos(deadCellCenterX, deadCellCenterY, fGeneralInfo->GetCOG().Z());
              double distanceToDeadCell_temp = (VtxPosAtCalo - deadCellPos).Perp();
              if (distanceToDeadCell_temp < distanceToDeadCell) {
                  distanceToDeadCell = distanceToDeadCell_temp;
              }
            }
          } else {
            for (std::vector<Int_t>::iterator itvtx = ecalvtxids.begin(); itvtx != ecalvtxids.end(); ++itvtx) {
              for (std::vector<Int_t>::iterator itdead = fdeadcells.begin(); itdead != fdeadcells.end(); ++itdead) {
                int deadCellX = *itdead / 100;
                int deadCellY = *itdead % 100;
                double deadCellCenterX = (deadCellX - ncells/2 + 0.5) * cellSize;
                double deadCellCenterY = (deadCellY - ncells/2 + 0.5) * cellSize;
                TVector3 deadCellPos(deadCellCenterX, deadCellCenterY, fGeneralInfo->GetCOG().Z());
                double distanceToDeadCell_temp = (fEvent->MCTruthEvent->Vertex(*itvtx)->GetPosition() - deadCellPos).Perp();
                if (distanceToDeadCell_temp < distanceToDeadCell) {
                  distanceToDeadCell = distanceToDeadCell_temp;
                }
              }
            }
          }  
          std::vector<Int_t> cluIds = GetClusFromVtx(iV);

         for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {

            if (std::find(cluIds.begin(), cluIds.end(), h1) == cluIds.end()) {
              continue; // skip clusters not associated with this vertex
            }
            TRecoVCluster* clu = fECal_clEvent->Element((int)h1);
            double cluEnergy = clu->GetEnergy();
            double cluTime = clu->GetTime();
            TVector3 cluPos(clu->GetPosition().X(),clu->GetPosition().Y(),fGeneralInfo->GetCOG().Z()); 
      //      TVector3 cluDir = cluPos-fGeneralInfo->GetTargetPos(); // direction of the cluster wrt the target
      //      cludir *= (1./cluDir.Mag());
                    
          TVector3 cluDiff = cluPos-VtxPosAtCalo;
          if(ecalvtxids.size() > 0) {
            std::cout << "MCTruthImprovedECal.cc: ecalvtxids.size() > 0 No ecalvtx found for vertex " << iV << " particle " << iO << ", distance to dead cell: " << distanceToDeadCell <<"cluDiff.Perp()" << cluDiff.Perp() << std::endl;

          }else{
            fHS->FillHisto2List("MCTruthImprovedECal",Form("InAccdRvsdistanceToDeadCell_%s_%d_ass_noecalvtx",it->first.Data(),pdgcode),distanceToDeadCell,cluDiff.Perp());

          }
          }// no associated cluster found
        }
      
    }
  }

  
  return true;
}


//Int_t MCTruthImprovedECal::GetVtxFromCluID(Int_t CluId){
//  map<Int_t, Int_t>::iterator it;
//  it = CluVtxCorr.find(CluId);
//  if(it!= CluVtxCorr.end()){
//    return it->second;
//  }else{
//    return -1;
//  }  
//}
//  
//std::vector<Int_t> MCTruthImprovedECal::GetClusFromVtx(Int_t VtxId){
//  map<Int_t, std::vector<Int_t>>::iterator it;
//  it = VtxCluCorr.find(VtxId);
//  if(it!= VtxCluCorr.end()){
//    return it->second;
//  }else{
//    return {}; //returns empty vector if no clusters are associated with the vertex
//  }  
//}
//
//std::vector<Int_t> MCTruthImprovedECal::GetPcleFromCluID(Int_t CluId){
//map<Int_t, std::vector<Int_t>>::iterator it;
//    it = CluPcleCorr.find(CluId);
//    if(it!= CluPcleCorr.end()){
//      return it->second;
//    }else{
//      return {}; //returns empty vector if no particles are associated with the cluster
//    }  
//  }
//
//Int_t MCTruthImprovedECal::GetCluFromPcle(Int_t VtxId, Int_t PcleId){
//  
//  std::map<Int_t, std::vector<Int_t>>::iterator it;
//  it = VtxCluCorr.find(VtxId);
//  if(it!= VtxCluCorr.end()){
//    std::vector<Int_t> associatedClusters = it->second;
//    for(Int_t cluId : associatedClusters){
//      std::map<Int_t, std::vector<Int_t>>::iterator pcleIt;
//      pcleIt = CluPcleCorr.find(cluId);
//      if(pcleIt != CluPcleCorr.end()){
//        std::vector<Int_t> associatedParticles = pcleIt->second;
//        if(std::find(associatedParticles.begin(), associatedParticles.end(), PcleId) != associatedParticles.end()){
//          return cluId; //returns the cluster ID associated with the given vertex and particle
//        }
//      }
//    }
//  }
//  return -1; //returns -1 if no cluster is found for the given vertex and particle
//
//} //returns the cluster associated to a vertex and a particle out from that vertex




Bool_t MCTruthImprovedECal::Finalize()
{  
  TString tagnames[2] = {"YvsX","EvsPhi"};
  if (fVerbose) printf("---> Finalizing MCTruthImprovedECal\n");

  for (int itag = 0; itag<2; itag++){ 
    std::map<TString, int>::iterator it;
    for (it = fProcessIDsMap.begin(); it != fProcessIDsMap.end(); it++){
      for (int i=0; i<3; i++){
	TString histoName = Form("InAcc%s_%s_%d",tagnames[itag].Data(),it->first.Data(),fpdgcodes[i]);
	TH2D* denoH = (TH2D*) fHS->GetHisto2List("MCTruthImprovedECal",histoName.Data());
	//	if (denoH->GetEntries() < 10000) continue;
	TH2D* numeH = (TH2D*) fHS->GetHisto2List("MCTruthImprovedECal",Form("%s_ass",histoName.Data()));
	TH2D* effiH = (TH2D*) fHS->GetHisto2List("MCTruthImprovedECal",Form("%s_eff",histoName.Data()));
      
	for (int i=0; i<denoH->GetXaxis()->GetNbins(); i++){
	  for (int j=0; j<denoH->GetYaxis()->GetNbins(); j++){
	    if (denoH->GetBinContent(i+1,j+1) > 10) {
	      double effi = numeH->GetBinContent(i+1,j+1)*100./denoH->GetBinContent(i+1,j+1)/30.;
	      // std::cout << "effi = " << effi << " nume = " << numeH->GetBinContent(i+1,j+1) << " deno = " << denoH->GetBinContent(i+1,j+1) << endl;
	      effiH->SetBinContent(i+1,j+1,effi);
	    }
	  }
	}
	
      }
    }
  }


  return 1;
}



