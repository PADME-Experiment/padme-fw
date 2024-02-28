#include "ECalSel.hh"
#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "HistoSvc.hh"
#include "MCTruthECal.hh"

#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "TVector3.h"
#include "TProfile.h"
#include "TString.h"
#include "TH1D.h"
#include "TFitResult.h"

#include "TFitResultPtr.h"
#include "TFile.h"
#include "TF1.h"

#include "TH2D.h"
#include "TLorentzVector.h"
#include <iostream>
#include <cmath>

ECalSel* ECalSel::fInstance = 0;
ECalSel* ECalSel::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new ECalSel(); }
  return fInstance;
}

ECalSel::ECalSel(){
  fRecoEvent   = NULL;
  fECal_hitEvent=0;  
  fVerbose        = 0;
  fECalEvents.clear();

  // single cluster conditions
  fTimeSafeMin = -1E10;// ns 
  fMaxTimeDistance =5; //5; // ns, sigma = 1.6 ns
  fMinGGDistance = 60; // mm
  // kinematic conditions are run-dependent. 
  fSafeEnergyFactor = 0.7; // Safety factor used for the energy min and max cuts
  fSafeSpaceMargin = 21; // mm, safety margin used for the radius min cut    //to be checked with new beam energy spread

  fFillLocalHistograms = false;
}

ECalSel::~ECalSel(){}

Bool_t ECalSel::Init(PadmeAnalysisEvent* event, Bool_t fHistoModeVal, TString InputHistofileVal){
  fEvent = event;
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl; 
  fGeneralInfo = GeneralInfo::GetInstance();
  fhSvcVal =  HistoSvc::GetInstance(); 
  fMCTruthECal = MCTruthECal::GetInstance();

  fFillLocalHistograms = false;
  InputHistofile = InputHistofileVal;
  fHistoMode = fHistoModeVal;
  // binning of theta vs phi to retrieve independently the beam direction

  fNThetaBins = 70;//20;
  //  fThetaWid = 0.003/fNThetaBins; // rad
  fThetaWid = 0.010/fNThetaBins; // rad
  fNPhiDirBins = 50;//was 25 50;
  //  const double pi = 3.1415927;
  fPhiDirWid = 2.*TMath::Pi()/fNPhiDirBins; // rad

// x and y slices
  NSlicesE =(Int_t)(fGeneralInfo->GetEnergyMax()-fGeneralInfo->GetEnergyMin())/spacing;  
  std::cout<<"NSlicesE: "<<NSlicesE<<" EnergyMax: "<<fGeneralInfo->GetEnergyMax()<<" EnergyMin: "<<fGeneralInfo->GetEnergyMin()<<" spacing: "<<spacing<<std::endl;
  fXMin = -21.*(14+0.5);
  fXMax = 21.*(14+0.5);
  fXW = 21; // mm
  fNXBins = (fXMax-fXMin)/fXW;
  fYMin = -21.*(14+0.5);
  fYMax = 21.*(14+0.5);
  fYW = 21; // mm
  fNYBins = (fYMax-fYMin)/fYW;

  InitHistos();

  return true;
}


void ECalSel::ProcessForCalib(){
  fSafeEnergyFactor = 0.7; // Safety factor used for the energy min and max cuts
  fSafeSpaceMargin = 42; // 2 cells in mm, safety margin used for the radius min and max cut    
  fFillLocalHistograms = false;
  fECalEvents.clear();
  TwoClusSel();
  // here the tag and probe selection might be added
}

Bool_t ECalSel::Process(){
  Int_t NEvent = fRecoEvent->GetEventNumber();
  //if(NEvent%10000==0) cout<<"ECalSel NEvent "<<NEvent<<endl;// -->si resetta ogni circa 500 evt per il MC

  fSafeEnergyFactor = 0.7;//0.7; // Safety factor used for the energy min and max cuts
  fSafeSpaceMargin = 31.5; // 1.5 cells in mm, safety margin used for the radius min and max cut    
  fFillLocalHistograms = true;
  fECalEvents.clear();
  TwoClusSel();
  OneClusSel();
  OneClusTagAndProbeSel();

  return true;
}


Int_t ECalSel::OneClusSel(){

  //  int runID = fRecoEvent->GetRunNumber();

  // selects cluster pairs due to the beam bremsstrahlung
  // returns the number of selected clusters
  // indices are in the vector of pairs fIndPair

  const double fMaxTimeDistance = 5; // ns, sigma = 1.6 ns
  const double minDistance = 125; // mm
  const double maxDistance = 145; // mm
  


  TRecoVCluster* tempClu[2];
  TVector3 cluPos[2];
  double cluTime[2];
  double cluEnergy[2];
  // loop on clusters

  int nOld = fECalEvents.size();

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
    tempClu[0] = fECal_clEvent->Element((int)h1);
    cluEnergy[0] = tempClu[0]->GetEnergy();
    if (cluEnergy[0] < fGeneralInfo->GetEnergyMax()) continue; // cluster should be harder than the 2gamma cluster max energy

    cluPos[0].SetXYZ(
		     tempClu[0]->GetPosition().X()-fGeneralInfo->GetCOG().X(),
		     tempClu[0]->GetPosition().Y()-fGeneralInfo->GetCOG().Y(),fGeneralInfo->GetCOG().Z());
    if (cluPos[0].Perp() > fGeneralInfo->GetRadiusMin()) continue; // cluster should be below the minimum radius of the 2gamma cluster pair

    cluTime[0] = tempClu[0]->GetTime();

    int isPaired = -1; // look for an additional cluster from the same interaction
    int npaired = 0;
    for (int h2=h1+1; h2< fECal_clEvent->GetNElements(); ++h2) {

      tempClu[1] = fECal_clEvent->Element((int)h2);
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluTime[1] = tempClu[1]->GetTime();

      cluPos[1].SetXYZ(
		       tempClu[1]->GetPosition().X()-fGeneralInfo->GetCOG().X(),
		       tempClu[1]->GetPosition().Y()-fGeneralInfo->GetCOG().Y(),fGeneralInfo->GetCOG().Z());

      double dt = cluTime[0]-cluTime[1];
      double dr = (cluPos[0]-cluPos[1]).Mag();
      TVector3 cog = (cluEnergy[0]/(cluEnergy[0]+cluEnergy[1]))*cluPos[0]+(cluEnergy[1]/(cluEnergy[0]+cluEnergy[1]))*cluPos[1];// cog of the two clusters

      if (fFillLocalHistograms){
	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SingleClu_DrVsDtAll"), dt, dr, 1.);
	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SingleClu_E1F_vs_E2F"), (cluEnergy[0]+cluEnergy[1])/fGeneralInfo->GetBeamMomentum(), cog.Perp(), 1.);
      }
      if (fabs(dt) < fMaxTimeDistance && dr > minDistance && dr < maxDistance) {
        
	isPaired = h2;
	if (fFillLocalHistograms)
	  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SingleClu_E1F_vs_E2FCut"), (cluEnergy[0]+cluEnergy[1])/fGeneralInfo->GetBeamMomentum(), cog.Perp(), 1.);

	npaired++;
      }
    } // inner cluster loop    

    if (fFillLocalHistograms)
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SingleClu_NPaired_vs_dE"), fGeneralInfo->GetBeamMomentum()-cluEnergy[0], npaired, 1.);

    if (npaired == 1){
      tempClu[1] = fECal_clEvent->Element((int)isPaired);
      ECalSelEvent selev;
      selev.flagEv = ev_single;
      selev.flagAlgo = estars;
      selev.indexECal[0] = h1;
      selev.indexECal[1] = isPaired;
      selev.indexECal[2] = -1;
      selev.totalE = cluEnergy[0] + cluEnergy[1];
      selev.avgT = cluTime[0];
      selev.cog.Set(cluPos[0].X(),cluPos[0].Y());
      selev.indexETagAss[0] = -1;
      selev.indexETagAss[1] = -1;
      selev.indexETagAss[2] = -1;
      fECalEvents.push_back(selev);
    }

  } // cluster loop


  if (fFillLocalHistograms) fhSvcVal->FillHistoList("ECalSel","NumberOfSingleClus",fECalEvents.size()-nOld);
  return fECalEvents.size()-nOld;
}


Int_t ECalSel::OneClusTagAndProbeSel(){

  // selects single clusters due to e+e-/gamma-gamma final states
  // returns the number of selected clusters
  // indices are in the vector of pairs fIndPair
  
  const double fMaxTimeDistance = 5; // ns, sigma = 1.6 ns


  TRecoVCluster* tempClu[2];
  TVector3 cluPos[2], cluPosRel[2];
  double cluTime[2];
  double cluEnergy[2];
  double cluTheta[2];
  TLorentzVector labMomenta[2];
  TLorentzVector labMomentaCM[2];
  // loop on clusters
  std::vector<double> rangeE;
  double EVal = fGeneralInfo->GetEnergyMin();
  for(int isli =0; isli<NSlicesE+1; isli++ ){
      rangeE.push_back(EVal);
      EVal= EVal+spacing;
  }

  int nOld = fECalEvents.size();

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {    
    tempClu[0] = fECal_clEvent->Element((int)h1);
    cluEnergy[0] = tempClu[0]->GetEnergy();

    cluPos[0].SetXYZ(
		     tempClu[0]->GetPosition().X(),
		     tempClu[0]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
    cluPosRel[0].SetXYZ(
			tempClu[0]->GetPosition().X()-fGeneralInfo->GetCOG().X(),
			tempClu[0]->GetPosition().Y()-fGeneralInfo->GetCOG().Y(),
			0.);
    cluTheta[0] = cluPos[0].Theta();
    TVector3 cluMom = cluPos[0]-fGeneralInfo->GetTargetPos();
    cluMom *= (1./cluMom.Mag());
    // angle between the cluster direction and the beam momentum
    double cosq = cluMom.Dot(fGeneralInfo->GetBoost())/(cluMom.Mag()*fGeneralInfo->GetBoost().Mag());
    // expected energies in the lab frame
    double pg = 0.5*fGeneralInfo->GetSqrts()/sqrt(1.-cosq*cosq + 
					      pow(fGeneralInfo->GetGam()*cosq,2) - 
						  2.*fGeneralInfo->GetBG()*fGeneralInfo->GetGam()*cosq + 
						  pow(fGeneralInfo->GetBG(),2) );

    cluMom *= pg;
    TVector3 otherCluMom = fGeneralInfo->GetBoost();
    otherCluMom *= (-fGeneralInfo->GetBeamEnergy()); 
    otherCluMom += cluMom;

    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DEVsE_noSel"), pg, cluEnergy[0]-pg, 1.);
    labMomenta[0].SetVectM(cluMom,0.); // define a photon-like tlorentzVector
    labMomentaCM[0].SetVectM(labMomenta[0].Vect(),0);
    labMomentaCM[0].Boost(-fGeneralInfo->GetBoost());
    int icellXh1 = cluPos[1].X()/cellSize+0.5 + ncells/2;
    int icellYh1 = cluPos[1].Y()/cellSize+0.5 + ncells/2;
          
    double Xexph2 = fGeneralInfo->GetTargetPos().X()+(otherCluMom.X()/otherCluMom.Z())*(fGeneralInfo->GetCOG().Z()-fGeneralInfo->GetTargetPos().Z());  
    double Yexph2 = fGeneralInfo->GetTargetPos().Y()+(otherCluMom.Y()/otherCluMom.Z())*(fGeneralInfo->GetCOG().Z()-fGeneralInfo->GetTargetPos().Z());  


    int icellXh2 = Xexph2/cellSize+0.5 + ncells/2;
    int icellYh2 = Yexph2/cellSize+0.5 + ncells/2;
        


    if (icellYh1 > 26) continue; //magnet shadow for the 1st clu
    if (icellYh1 < 4) continue; //magnet shadow for the 1st clu
    if (icellYh2 > 26) continue; //magnet shadow for the 2nd clu
    if (icellYh2 < 4) continue; //magnet shadow for the 2nd clu


  
    if (cluPosRel[0].Perp() < fGeneralInfo->GetRadiusMin()) continue; // cluster should be within the radius range of the 2gamma cluster pair
    if (cluPosRel[0].Perp() > fGeneralInfo->GetRadiusMax()) continue; // cluster should be within the radius range of the 2gamma cluster pair
    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DEVsE_radiusSel"), pg, cluEnergy[0]-pg, 1.);
    fhSvcVal->FillHistoList("ECalSel",Form("ECal_TP_DE_radiusSel"), cluEnergy[0]-pg, 1.);
    if(tempClu[0]->GetNHitsInClus()<3) continue;
    
    
    
      // if(mcVtx->GetProcess() =="eBrem"){
      //   fhSvcVal->FillHistoList("ECalSel","ECal_TP_Process",0.,1.);
      // }else if(mcVtx->GetProcess() =="eIoni"){ 
      //   fhSvcVal->FillHistoList("ECalSel","ECal_TP_Process",1.,1.);
      // }else{
      //   fhSvcVal->FillHistoList("ECalSel","ECal_TP_Process",2.,1.);
      // } 


    fhSvcVal->FillHisto2List("ECalSel","ECal_TP_DEVsE_NOcut_tag", fGeneralInfo->GetBeamEnergy()- pg, cluEnergy[0]-pg, 1.);//da mettere quello in phi
    if(cluEnergy[0]-pg<-40) fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DEVsE_cutted"),cluPos[0].X(),cluPos[0].Y(), 1.);

    
    if(!((cluEnergy[0]-pg)> -15 && (cluEnergy[0]-pg)<15)) continue; 

    if(fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)){
      TMCVertex* mcVtx = fEvent->MCTruthEvent->Vertex(fMCTruthECal->GetVtxFromCluID(h1));
    // TMCVertex* mcVtx = fEvent->MCTruthEvent->Vertex(fMCTruthECal->GetVtxFromCluID(h1));
    // if(mcVtx->GetProcess() =="eBrem") continue;
    
      //if(mcVtx->GetProcess() =="eBrem" ) continue; 
      std::pair<Int_t, Int_t> mcOPartcles;

      mcOPartcles = fMCTruthECal->GetCluPcleCorr(fMCTruthECal->GetVtxFromCluID(h1));
      TMCParticle* PcleOut;


      if(mcOPartcles.first==h1) PcleOut = mcVtx->ParticleOut(0);
      else if(mcOPartcles.second==h1) PcleOut = mcVtx->ParticleOut(1);
      TVector3 pcleMom = PcleOut->GetMomentum();

      fhSvcVal->FillHistoList("ECalSel","ECal_TP_DeltaPwithPcle",pcleMom.Mag() - cluMom.Mag(),1.);
      TVector3 VtxPos = mcVtx->GetPosition(); 
      TVector3 pclePos = VtxPos;

      TVector3 VtxPosAtCalo;
          VtxPosAtCalo.SetZ(fGeneralInfo->GetCOG().Z()-72.8); //removed 6.5X0 faccia calorimetro
          VtxPosAtCalo.SetX(pclePos.X()+((pcleMom.X()/pcleMom.Z())*(VtxPosAtCalo.Z()-pclePos.Z())));
          VtxPosAtCalo.SetY(pclePos.Y()+((pcleMom.Y()/pcleMom.Z())*(VtxPosAtCalo.Z()-pclePos.Z())));
      fhSvcVal->FillHistoList("ECalSel","ECal_TP_DeltaThetawithPcle",VtxPosAtCalo.Theta() - cluPos->Theta(),1.);
      
      fhSvcVal->FillHisto2List("ECalSel","ECal_TP_DThetavsDPwithPcle",VtxPosAtCalo.Theta() - cluPos->Theta(),pcleMom.Mag() - cluMom.Mag(), 1.);
      
      
    }

    fhSvcVal->FillHisto2List("ECalSel","ECal_TP_DE_Tag", fGeneralInfo->GetBeamEnergy()-pg, cluEnergy[0]-pg, 1.);
    fhSvcVal->FillHisto2List("ECalSel","ECal_TP_DEVsE_cut_tag", pg, fGeneralInfo->GetBeamEnergy()- pg, 1.);

    cluTime[0] = tempClu[0]->GetTime();


    int isPaired = -1; // look for an additional cluster from the same interaction
    int npaired = 0;
    int h2; 
    for (h2=0; h2< fECal_clEvent->GetNElements(); ++h2) {
      if (h1 == h2) continue;
       
      tempClu[1] = fECal_clEvent->Element((int)h2);
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluTime[1] = tempClu[1]->GetTime();

      cluPos[1].SetXYZ(
		       tempClu[1]->GetPosition().X(),
		       tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
      cluPosRel[1].SetXYZ(
		       tempClu[1]->GetPosition().X()-fGeneralInfo->GetCOG().X(),
		       tempClu[1]->GetPosition().Y()-fGeneralInfo->GetCOG().Y(),fGeneralInfo->GetCOG().Z());

     

      

      double dt = cluTime[0]-cluTime[1];
      double dr = (cluPos[0]-cluPos[1]).Mag();
      TVector3 cog = (cluEnergy[0]/(cluEnergy[0]+cluEnergy[1]))*cluPos[0]+(cluEnergy[1]/(cluEnergy[0]+cluEnergy[1]))*cluPos[1];// cog of the two clusters
      TVector3 cluMom2 = cluPos[1]-fGeneralInfo->GetTargetPos();

      cluMom2 = cluPos[1]-fGeneralInfo->GetTargetPos();
      cluMom2 *= (1./cluMom2.Mag());
      // angle between the cluster direction and the beam momentum
      double cosq2 = cluMom2.Dot(fGeneralInfo->GetBoost())/(cluMom2.Mag()*fGeneralInfo->GetBoost().Mag());
      // expected energies in the lab frame
      double pg2 = 0.5*fGeneralInfo->GetSqrts()/sqrt(1.-cosq2*cosq2 + 
                  pow(fGeneralInfo->GetGam()*cosq2,2) - 
                2.*fGeneralInfo->GetBG()*fGeneralInfo->GetGam()*cosq2 + 
                pow(fGeneralInfo->GetBG(),2));

      cluMom2 *= pg2;
      labMomenta[1].SetVectM(cluMom2,0.); // define a photon-like tlorentzVector
      labMomentaCM[1].SetVectM(labMomenta[1].Vect(),0);
      labMomentaCM[1].Boost(-fGeneralInfo->GetBoost());


      if (fFillLocalHistograms){
        fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DrVsDtAll"), dt, dr, 1.);
        fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_E1F_vs_E2F"), (cluEnergy[0]+cluEnergy[1])/fGeneralInfo->GetBeamMomentum(), cog.Perp(), 1.);
      }

      double Dphi = fabs(labMomentaCM[0].Vect().Phi()   - labMomentaCM[1].Vect().Phi());
      double DTheta = fabs(labMomentaCM[0].Vect().Theta()  + labMomentaCM[1].Vect().Theta());
      double CutRsq= 0.20; //squared radius of the signal region

      if (fabs(dt) < fMaxTimeDistance) {
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DEvsE_ProbeDtcut"), pg2, cluEnergy[1]-pg2, 1.);
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DTHEVsDPHIAbs_probe_nocut"), 
				 fabs(labMomentaCM[0].Vect().Phi()   - labMomentaCM[1].Vect().Phi()), 
				 labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
         fhSvcVal->FillHistoList("ECalSel",Form("ECal_TP_DPHIAbs_probe"), fabs(labMomentaCM[0].Vect().Phi()   - labMomentaCM[1].Vect().Phi()),1.);
         Double_t Eup = fGeneralInfo->GetBeamEnergy()-fGeneralInfo->GetEnergyMin();
         Double_t Edown = fGeneralInfo->GetBeamEnergy()-fGeneralInfo->GetEnergyMax();
         Int_t iSlice=0;
         while(Edown<Eup){
          //std::cout<<"Edown: "<<Edown<<" Edown+spacing: "<<Edown+spacing<<" fGeneralInfo->GetBeamEnergy()- pg: "<<fGeneralInfo->GetBeamEnergy()- pg<<std::endl;
          if((fGeneralInfo->GetBeamEnergy()- pg) >= (Edown) && (fGeneralInfo->GetBeamEnergy()- pg)<(Edown+spacing)){
             fhSvcVal->FillHistoList("ECalSel",Form("ECal_TP_DPHIAbs_probe_slice_%i", iSlice), fabs(labMomentaCM[0].Vect().Phi()   - labMomentaCM[1].Vect().Phi()),1.);
             //std::cout<<"I filled slice "<<iSlice<<" !!!"<<std::endl;
             }
            Edown+=spacing;
            iSlice+=1;
         }
         fhSvcVal->FillHisto2List("ECalSel","ECal_TP_DEVsE_NOcut_probe", fGeneralInfo->GetBeamEnergy()- pg, cluEnergy[1]- pg, 1.);//da mettere quello in phi
      if ((((Dphi-TMath::Pi())*(Dphi-TMath::Pi()))+((DTheta-TMath::Pi())*(DTheta-TMath::Pi())))<CutRsq){
         // boolean e stori variabili di interesse per es il pg2, ...

         fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DE_Probe"), fGeneralInfo->GetBeamEnergy()- pg, cluEnergy[1]-pg2, 1.);
         fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DTHEVsDPHIAbs_probe"), 
				 fabs(labMomentaCM[0].Vect().Phi()   - labMomentaCM[1].Vect().Phi()), 
				 labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);
          fhSvcVal->FillHisto2List("ECalSel","ECal_TP_DEVsE_cut_probe", fGeneralInfo->GetBeamEnergy()- pg, cluEnergy[1]- pg2, 1.);
          isPaired = h2;
          

          break;
        }
	if (fFillLocalHistograms)
	  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_E1F_vs_E2FCut"), (cluEnergy[0]+cluEnergy[1])/fGeneralInfo->GetBeamMomentum(), cog.Perp(), 1.);

	npaired++;
      }
    } // inner cluster loop    
    if(isPaired==-1){ //recovery running on the hits
    TVector3 HitPosExp;

    double Xexp = fGeneralInfo->GetTargetPos().X()+(otherCluMom.X()/otherCluMom.Z())*(fGeneralInfo->GetCOG().Z()-fGeneralInfo->GetTargetPos().Z());  
    double Yexp = fGeneralInfo->GetTargetPos().Y()+(otherCluMom.Y()/otherCluMom.Z())*(fGeneralInfo->GetCOG().Z()-fGeneralInfo->GetTargetPos().Z());  

    HitPosExp.SetX(Xexp);
    HitPosExp.SetY(Yexp);
    HitPosExp.SetZ(0);
    int HitTag[10000]={0};
    for(int iHitTag=0;iHitTag<tempClu[0]->GetNHitsInClus();iHitTag++){   
      //std::cout<<"Muoio in HitTag"<<iHitTag<<" "<<tempClu[0]->GetHitVecInClus().at(iHitTag)<<std::endl;   
      HitTag[tempClu[0]->GetHitVecInClus().at(iHitTag)]= 1;
    }
      for (int hh=0; hh< fECal_hitEvent->GetNHits(); hh++){
      if(HitTag[hh]==1) continue; 
      TRecoVHit* ihit = fECal_hitEvent->Hit(hh);
      TVector3 hitPos = ihit->GetPosition();

      double xHit = hitPos.X();
      double yHit = hitPos.Y();
      double dr = TMath::Sqrt(((HitPosExp.X()-xHit)*(HitPosExp.X()-xHit))+((HitPosExp.Y()-yHit)*(HitPosExp.Y()-yHit)));

      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DYvsDX_Hit_Probe"),HitPosExp.X()-xHit, HitPosExp.Y()-yHit, 1.);
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_DRvsDT_Hit_Probe"),ihit->GetTime()-cluTime[0],dr, 1.);

     
    }
  }
    if (fFillLocalHistograms) fhSvcVal->FillHisto2List("ECalSel",Form("ECal_TP_NPaired_vs_dE"), fGeneralInfo->GetBeamMomentum()-cluEnergy[0], npaired, 1.);

    ECalSelEvent selev;
    selev.flagEv = ev_single;
    selev.flagAlgo = tagAndProbe;
    selev.indexECal[0] = h1;
    selev.indexECal[1] = isPaired;
    selev.indexECal[2] = -1;

    // if (npaired) {
    //   tempClu[1] = fECal_clEvent->Element((int)isPaired);
    //   selev.totalE = cluEnergy[0] + tempClu[1]->GetEnergy();
    // }
    selev.avgT = cluTime[0];

    // here store expected position of probed cluster
    selev.cog.Set(
		  fGeneralInfo->GetTargetPos().X() + otherCluMom.X()/otherCluMom.Z()*(fGeneralInfo->GetCOG().Z()-fGeneralInfo->GetTargetPos().Z()), 
		  fGeneralInfo->GetTargetPos().Y() + otherCluMom.Y()/otherCluMom.Z()*(fGeneralInfo->GetCOG().Z()-fGeneralInfo->GetTargetPos().Z())
		  );
    selev.indexETagAss[0] = -1;
    selev.indexETagAss[1] = -1;
    selev.indexETagAss[2] = -1;
    fECalEvents.push_back(selev);


  } // cluster loop

  if (fFillLocalHistograms) fhSvcVal->FillHistoList("ECalSel","NumberOfTPClus",fECalEvents.size()-nOld);
  return fECalEvents.size()-nOld;
}




Int_t ECalSel::TwoClusSel(){
  // cuts for the energy-based selection
  const double deCutCenter = 0;   // MeV 
  const double deCutRadius = 15.; // MeV To be re-calibrated vs energy
  const double sigmadphi = 0.1; // rad (possible double peak present)

  const double sigmadthetastar = 0.008;

  int nOld = fECalEvents.size();

  // selects pairs of clusters compatible with the e+e- --> gamma-gamma/e+e- transition
  // returns the number of selected pairs
  // pair indices are in the vector of pairs fIndPair

  TTimeStamp evt = fRecoEvent->GetEventTime();
  long long int eventTime = static_cast<long long int>(evt.GetSec());
  long long int  deventTime = eventTime - fGeneralInfo->GetPeriodStartTime();// using the period start time and not the run start time <-- to be checked

  int NCluPairperEvent =0; 
  int NCluPairperEvent_EnCut=0;

  TRecoVCluster* tempClu[2];
  TVector3 cluPos[2];
  TVector3 cluPosRel[2];
  double cluTime[2];
  double cluEnergy[2];
  

  // fill general occupancy plot of ECal

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
    tempClu[0] = fECal_clEvent->Element((int)h1);
    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx_EweightAll"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), tempClu[0]->GetEnergy());
    fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsxAll"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), 1.);
    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
    cluPosRel[0] = cluPos[0]-fGeneralInfo->GetCOG();

    if (tempClu[0]->GetEnergy() > fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor && tempClu[0]->GetEnergy() < fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor) {
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx_Eweight"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), tempClu[0]->GetEnergy());
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_yvsx"), tempClu[0]->GetPosition().X(), tempClu[0]->GetPosition().Y(), 1.);
    }    
  }


  // loop on cluster pairs
  

  for (int h1=0; h1< fECal_clEvent->GetNElements(); ++h1) {
    Double_t tempChiEn=-1;
    tempClu[0] = fECal_clEvent->Element((int)h1);

    cluTime[0] = tempClu[0]->GetTime();
    if (cluTime[0] < fTimeSafeMin) continue; // require time in the safe region [TBchecked]
    cluEnergy[0] = tempClu[0]->GetEnergy();
    if (cluEnergy[0] < fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor) continue; // require a minimum energy
    if (cluEnergy[0] > fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor) continue; // require a maximum energy

    cluPos[0].SetXYZ(tempClu[0]->GetPosition().X(),tempClu[0]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());
    cluPosRel[0] = cluPos[0]-fGeneralInfo->GetCOG();
    if (cluPosRel[0].Perp() < fGeneralInfo->GetRadiusMin() - fSafeSpaceMargin) continue; // require a minimum radius at the ECAL
    if (cluPosRel[0].Perp() > fGeneralInfo->GetRadiusMax()) continue; // require a maximum radius at the ECAL, should we use a margin here as well?
    //da ridefinire con taglio magnete

    int icellX = cluPos[0].X()/cellSize+0.5 + ncells/2;
    int icellY = cluPos[0].Y()/cellSize+0.5 + ncells/2;
          


    if (icellY > 26) continue;
    if (icellY < 4) continue;


    int nPaired = 0; // number of paired clusters
    int isPaired = -1; //index of paired cluster

    for (int h2=h1+1; h2< fECal_clEvent->GetNElements(); ++h2) { //corretto partire da h1+1?
      tempClu[1] = fECal_clEvent->Element((int)h2);

      cluTime[1] = tempClu[1]->GetTime();
      if (cluTime[1] < fTimeSafeMin) continue;
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z());

      double dt = cluTime[0]-cluTime[1];
      double dr = (cluPos[0]-cluPos[1]).Mag();

      if (fFillLocalHistograms) fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DrVsDtAll"), dt, dr, 1.);

      if (cluEnergy[1] < fGeneralInfo->GetEnergyMin()*fSafeEnergyFactor) continue; // require a minimum energy
      if (cluEnergy[1] > fGeneralInfo->GetEnergyMax()/fSafeEnergyFactor) continue; // require a maximum energy

      cluPosRel[1] = cluPos[1]-fGeneralInfo->GetCOG();

      if (cluPosRel[1].Perp() < fGeneralInfo->GetRadiusMin() - fSafeSpaceMargin) continue; // require a minimum radius at the ECAL
      if (cluPosRel[1].Perp() > fGeneralInfo->GetRadiusMax()) continue; // require a maximum radius at the ECAL, should we use a margin here as well?
      
      int icellX = cluPos[1].X()/cellSize+0.5 + ncells/2;
      int icellY = cluPos[1].Y()/cellSize+0.5 + ncells/2;
          


      if (icellY > 26) continue;
      if (icellY < 4) continue;


      if (fFillLocalHistograms) fhSvcVal->FillHisto2List("ECalSel","ECal_SC_DrVsDt", dt, dr, 1.);

      if (fabs(dt) < fMaxTimeDistance && dr > fMinGGDistance) { 

          Double_t ChiEnergy = ((fGeneralInfo->GetBeamEnergy()-(cluEnergy[1]+cluEnergy[0]))/(fSigmaE*fSigmaE))*
                    ((fGeneralInfo->GetBeamEnergy()-(cluEnergy[1]+cluEnergy[0])));
          fhSvcVal->FillHistoList("ECalSel","ECal_SC_Chi2",ChiEnergy , 1.);
          if (nPaired == 0){ 
            isPaired = h2;
            tempChiEn = ChiEnergy;
          } else if(nPaired > 0){
            if(tempChiEn> ChiEnergy){
              isPaired = h2;
              tempChiEn = ChiEnergy;  
            }
          }
          nPaired++;
               
          }



     } // inner cluster loop
    
    // if (fabs(dt) < fMaxTimeDistance && dr > fMinGGDistance) { 
    //       if (nPaired == 0) isPaired = h2;
    //       else isPaired = -1;   // more than 1 cluster found connected -> reject the event
    //       nPaired++;
    //           }
    //  } // 


    fhSvcVal->FillHisto2List("ECalSel","ECal_SC_Chi2vsNPaired" , nPaired,tempChiEn, 1.);
    if (fFillLocalHistograms) fhSvcVal->FillHistoList("ECalSel","ECal_SC_NPaired", nPaired, 1.);

    // Having found a cluster pair, check the kinematics

    if (isPaired != -1) { // fill plots 
      NCluPairperEvent++;
      cluEnergy[0] = tempClu[0]->GetEnergy();
      cluPosRel[0] = cluPos[0]-fGeneralInfo->GetCOG();

      tempClu[1] = fECal_clEvent->Element((int)isPaired);
      cluEnergy[1] = tempClu[1]->GetEnergy();
      cluTime[1] = tempClu[1]->GetTime();
      cluPos[1].SetXYZ(tempClu[1]->GetPosition().X(),tempClu[1]->GetPosition().Y(),fGeneralInfo->GetCOG().Z()); 
      cluPosRel[1] = cluPos[1] - fGeneralInfo->GetCOG();
      
      // evaluate kinematics

      double pg[2]; // expected energies in the lab frame
      double cosq[2]; // angle between the cluster direction and the beam momentum
      TVector3 cluMomCrossBoost[2]; // vector product between cluster direction and beam momentum (normalised to 1)
      TLorentzVector labMomenta[2], labMomentaCM[2]; // momenta in the lab and CM frames

      for (int i = 0; i<2; i++){
          TVector3 cluMom = cluPos[i]-fGeneralInfo->GetTargetPos();
          cluMom *= (cluEnergy[i]/cluMom.Mag());

          // laboratory and cm momenta
          labMomenta[i].SetVectM(cluMom,0.); // define a photon-like tlorentzVector
          labMomentaCM[i].SetVectM(labMomenta[i].Vect(),0);
          labMomentaCM[i].Boost(-fGeneralInfo->GetBoost());

          // cosine and sine with respect to the boost
          cosq[i] = cluMom.Dot(fGeneralInfo->GetBoost())/(cluMom.Mag()*fGeneralInfo->GetBoost().Mag());
          cluMomCrossBoost[i] = cluMom.Cross(fGeneralInfo->GetBoost());
          cluMomCrossBoost[i] *= 1./(cluMom.Mag()*fGeneralInfo->GetBoost().Mag());

	        // energy expected in the lab
	        pg[i] = 0.5*fGeneralInfo->GetSqrts()/sqrt(1.-cosq[i]*cosq[i] + 
						  pow(fGeneralInfo->GetGam()*cosq[i],2) - 
						  2.*fGeneralInfo->GetBG()*fGeneralInfo->GetGam()*cosq[i] + 
						  pow(fGeneralInfo->GetBG(),2) );
      }      
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DEVsE"), pg[0], cluEnergy[0]-pg[0], 1.);
      fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DEVsE"), pg[1], cluEnergy[1]-pg[1], 1.);
      
      if (fFillLocalHistograms){
          fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DE1VsDE2"), cluEnergy[0]-pg[0], cluEnergy[1]-pg[1], 1.);
        
          fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DTHEVsDPHIAbs"), 
                fabs(labMomentaCM[0].Vect().Phi()   - labMomentaCM[1].Vect().Phi()), 
                labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1.);

          fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_DTHEVsEnergySum"), 
                labMomentaCM[0].Vect().Angle(labMomentaCM[1].Vect()), 
                cluEnergy[0]+cluEnergy[1], 1.);
      }
      //if(MC) controllare che eventi mi rimangono dopo i tagli

      // define an elliptical signal region for the energy-based selection
      double elliEnergy = 
	TMath::Power((cluEnergy[0]-pg[0]-deCutCenter)/deCutRadius,2) +
	TMath::Power((cluEnergy[1]-pg[1]-deCutCenter)/deCutRadius,2) ;

      // define an elliptical signal region for the angle-based selection
      double elliAngle = 
	TMath::Power((fabs(labMomentaCM[0].Vect().Phi() - labMomentaCM[1].Vect().Phi())- TMath::Pi())/sigmadthetastar,2) + 
	TMath::Power((labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta()  - TMath::Pi())/sigmadthetastar,2) ;
      
      //      double dphi = TMath::ASin((cluPosRel[1].Y()*cluPosRel[0].X()-cluPosRel[1].X()*cluPosRel[0].Y())/(cluPosRel[0].Perp()*cluPosRel[1].Perp()));
      double signPlane = -1;
      if (cluMomCrossBoost[0].Dot(cluMomCrossBoost[1]) > 0) signPlane = 1;
      double dphi = (
		     (fGeneralInfo->GetGam()*cosq[0]-fGeneralInfo->GetBG())*
		     (fGeneralInfo->GetGam()*cosq[1]-fGeneralInfo->GetBG()) +
		     signPlane*sqrt(1.-cosq[0]*cosq[0])*sqrt(1-cosq[1]*cosq[1]))/
	((fGeneralInfo->GetGam()-fGeneralInfo->GetBG()*cosq[0])*(fGeneralInfo->GetGam()-fGeneralInfo->GetBG()*cosq[1]));
      if (fabs(dphi) > 1) {
	std::cout << "ECalSel >> Inconsistent two-body kinematics " << dphi << " " << cosq[0] << " " << cosq[1] << " " << signPlane << std::endl;
	break;
      }
      dphi = TMath::ACos(dphi);

      if (fFillLocalHistograms) {
	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_dphiElli"), elliEnergy, dphi, 1.);
	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_ElliAngle_ElliEnergy"), elliEnergy, elliAngle, 1.);
      }

      if (elliEnergy < 4 && TMath::Abs(TMath::Pi()-dphi)/sigmadphi < 3) {
	for (int kk=0; kk<2; kk++){
	  double seedEnergy = fECal_hitEvent->Hit(tempClu[kk]->GetSeed())->GetEnergy();
	  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_Eseed_vs_Eexp"), pg[kk], seedEnergy, 1.);
	  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_NHits_vs_Eexp"), pg[kk], tempClu[kk]->GetNHitsInClus(), 1.);
	  
	}
	
	TVector2 cog(
		     (cluEnergy[0]*cluPos[0]+cluEnergy[1]*cluPos[1]).X()/(cluEnergy[0]+cluEnergy[1]),
		     (cluEnergy[0]*cluPos[0]+cluEnergy[1]*cluPos[1]).Y()/(cluEnergy[0]+cluEnergy[1])
		     );


  Double_t DeltaPhiAbs = fabs(labMomentaCM[0].Vect().Phi()   - labMomentaCM[1].Vect().Phi());
  Double_t DeltaTheta = labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta();

  if(!(DeltaPhiAbs>fMeanDPhi-3*fSigmaDPhi && DeltaPhiAbs<fMeanDPhi+3*fSigmaDPhi))  continue;
  if(!(DeltaTheta>fMeanDTheta-3*fSigmaDTheta && DeltaTheta<fMeanDTheta+3*fSigmaDTheta))  continue;

  NCluPairperEvent_EnCut++;

  fhSvcVal->FillHistoList("ECalSel","ECal_SC_E1plusE2", cluEnergy[0]+ cluEnergy[1]);

	fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_EVsT"), 0.5*(cluTime[0]+cluTime[1]), cluEnergy[0]+cluEnergy[1], 1.);
  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_SC_E1E2VsDT"), (cluTime[0]-cluTime[1]), cluEnergy[0]+cluEnergy[1], 1.);

	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_COGYX", cog.X(),cog.Y(),1.);		     
	fhSvcVal->FillHisto2List("ECalSel","ECalX_vs_Time", deventTime, cog.X(),1.);		     
	fhSvcVal->FillHisto2List("ECalSel","ECalY_vs_Time", deventTime, cog.Y(),1.);		     
	//std::cout << "deventTime " << deventTime << " cog = " << cog.X() << " " << cog.Y() << std::endl;

	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_YX",cluPos[0].X(),cluPos[0].Y(),1.);
	fhSvcVal->FillHisto2List("ECalSel","ECal_SC_YX",cluPos[1].X(),cluPos[1].Y(),1.);

	ECalSelEvent selev;
	selev.flagEv = ev_gg;
	selev.flagAlgo = estars;
	selev.indexECal[0] = h1;
	selev.indexECal[1] = isPaired;
	selev.indexECal[2] = -1;
	selev.totalE = cluEnergy[0] + cluEnergy[1];
	selev.avgT = 0.5*(cluTime[0]+cluTime[1]);
	selev.cog.Set(cog.X(),cog.Y());
	selev.indexETagAss[0] = -1;
	selev.indexETagAss[1] = -1;
	selev.indexETagAss[2] = -1;
	fECalEvents.push_back(selev);

	TLorentzVector photonMom[2];
	for (int j=0; j<2; j++) {
	  TVector3 rPos = cluPos[j]-fGeneralInfo->GetTargetPos();
	  rPos *= (cluEnergy[j]/rPos.Mag());
	  photonMom[j].SetXYZT(rPos.X(),rPos.Y(),rPos.Z(),cluEnergy[j]);
	}

	// Fill plots to check energy and radius range

	for (int kk = 0; kk < 2; kk++){
	  fhSvcVal->FillHisto2List("ECalSel",Form("ECal_E_Radius"), cluPosRel[kk].Perp(), tempClu[kk]->GetEnergy());
	  fhSvcVal->FillHistoList("ECalSel",Form("ECal_RadiusECut"),
				  (cluPosRel[kk].Perp()-fGeneralInfo->GetRadiusMin())/(fGeneralInfo->GetRadiusMax()-fGeneralInfo->GetRadiusMin()));
	  fhSvcVal->FillHistoList("ECalSel",Form("ECal_ERCut"),
				  (tempClu[kk]->GetEnergy()-fGeneralInfo->GetEnergyMin())/(fGeneralInfo->GetEnergyMax()-fGeneralInfo->GetEnergyMin()));

	  int xbin = (cluPos[kk].X() - fXMin)/fXW;
	  int ybin = (cluPos[kk].Y() - fYMin)/fYW;
	  if (xbin < 0 || xbin >= fNXBins){
	    std::cout << "X value out of range " << cluPos[kk].X() << " " << cluPos[kk].Y() << std::endl;
	    break;
	  }
	  if (ybin < 0 || ybin >= fNYBins){
	    std::cout << "Y value out of range " << cluPos[kk].X() << " " << cluPos[kk].Y() << std::endl;
	    continue;
	  }
	    
	  TString hname = Form("EExpVsEX_%d_Y_%d",xbin+1,ybin+1);
	  fhSvcVal->FillHisto2List("ECalScale",hname.Data(),pg[kk],cluEnergy[kk]/pg[kk]-1,1.); 

	}

	// evaluate best boost direction

	for (int ith = 0; ith<fNThetaBins; ith++){ // loop on theta
	  double thetaRot = fThetaWid*(ith+0.5);
	  for (int iph = 0; iph<fNPhiDirBins; iph++){ // loop on phi
	    double phiRot = fPhiDirWid*(iph+0.5);
	    TVector3 trialBoost(fGeneralInfo->GetBoost().Mag()*TMath::Sin(thetaRot)*TMath::Cos(phiRot),
				fGeneralInfo->GetBoost().Mag()*TMath::Sin(thetaRot)*TMath::Sin(phiRot),
				fGeneralInfo->GetBoost().Mag()*TMath::Cos(thetaRot)); 
	    trialBoost *= -1;
	    TLorentzVector pgstar[2];
	    for (int j=0; j<2; j++) {
	      pgstar[j].SetXYZT(photonMom[j].X(),photonMom[j].Y(),photonMom[j].Z(),photonMom[j].E());
	      pgstar[j].Boost(trialBoost);
	    }

	    double cosRel = pgstar[0].Vect().Dot(pgstar[1].Vect())/(pgstar[0].E()*pgstar[1].E());	

	    fhSvcVal->FillHisto2List("ECalSel","DPhiVsRotVsRunCos",thetaRot,phiRot,1.+cosRel); 
	    if (cosRel < -0.998) {
	      fhSvcVal->FillHisto2List("ECalSel","DPhiVsRotVsRun",thetaRot,phiRot,1.); 
	    }
	  }
	}


      }
    } // isPaired
  } // cluster loop
  
  fhSvcVal->FillHistoList("ECalSel","ECal_SC_NCluPairperEvent",NCluPairperEvent,1.);
  fhSvcVal->FillHistoList("ECalSel","ECal_SC_NCluPairperEvent_EnCut",NCluPairperEvent_EnCut,1.);


  fhSvcVal->FillHistoList("ECalSel","NumberOfECalCluPairs",fECalEvents.size()-nOld);
  return fECalEvents.size()-nOld;
}






Bool_t ECalSel::InitHistos()
{
  fhSvcVal->CreateList("ECalSel");

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx_Eweight", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsx_EweightAll", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_yvsxAll", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);

  fhSvcVal->BookHisto2List("ECalSel","ECal_E_Radius", 100,0.,TMath::Sqrt(fXMax*fXMax+fYMax*fYMax),100,0,500.);
  fhSvcVal->BookHistoList("ECalSel","ECal_RadiusECut", 100,-1.5,1.5);
  fhSvcVal->BookHistoList("ECalSel","ECal_ERCut", 100,-1.5,1.5);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_Eseed_vs_Eexp", 100, 0, 400., 100., 0., 200.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_NHits_vs_Eexp", 100, 0, 400., 50., 0., 50.);

  // single clusters from beam brems
  
  fhSvcVal->BookHisto2List("ECalSel","ECal_SingleClu_NPaired_vs_dE", 200,-100.,100.,10,0,10);
  double LMax = TMath::Sqrt((fXMax-fXMin)*(fXMax-fXMin)+(fYMax-fYMin)*(fYMax-fYMin));
  fhSvcVal->BookHisto2List("ECalSel","ECal_SingleClu_E1F_vs_E2F"   , 400,0.,4.,100,0.,LMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SingleClu_E1F_vs_E2FCut", 400,0.,4.,100,0.,LMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SingleClu_DrVsDtAll", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHistoList("ECalSel","NumberOfSingleClus", 10,0.,10.);

  // single clusters from tag and probe

  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEVsE_noSel", 100, 100,400, 800, -400, 400.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEVsE_radiusSel", 100, 100,400, 800, -400, 400.);
 
  fhSvcVal->BookHistoList("ECalSel","ECal_TP_DE_radiusSel", 800, -400, 400);
  
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_XvsY_DEradiusSel", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  EofTag = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEVsE_NOcut_tag", 100, 100,400, 800, -400, 400.);
  PhiofTag = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DPhiVsPhi_NOcut_tag", 1200, 0.,3*TMath::Pi(), 1200, 0., 3*TMath::Pi());

  EofTag_cut = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEVsE_cut_tag", 100, 100,400, 800, -400, 400.);
  PhiofTag_cut = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DPhiVsPhi_cut_tag", 1200, 0.,3*TMath::Pi(), 1200, 0., 3*TMath::Pi());

  fhSvcVal->BookHistoList("ECalSel","ECal_TP_DE_Tag", 800, 0, 400);

  fhSvcVal->BookHistoList("ECalSel","ECal_TP_Process", 4, -0.5, 3.5);

  fhSvcVal->BookHistoList("ECalSel","ECal_TP_DeltaThetawithPcle",120,-2.,3*TMath::Pi() );
  fhSvcVal->BookHistoList("ECalSel","ECal_TP_DeltaPwithPcle",300, -300., 300.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DThetavsDPwithPcle",120,-2.,3*TMath::Pi(), 300, -300., 300.);
 
  


  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DrVsDtAll", 800, -400,400, 200, 0, 600.);
    
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEvsE_ProbeDtcut", 100, 100,400,800, -400, 400);
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEvsE_ProbeDEcut", 100, 100,400,800, -400, 400);

  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_E1F_vs_E2F"   , 400,0.,4.,100,0.,LMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_E1F_vs_E2FCut", 400,0.,4.,100,0.,LMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_NPaired_vs_dE", 200,-100.,100.,10,0,10);
  fhSvcVal->BookHistoList("ECalSel","NumberOfTPClus", 10,0.,10.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEVsE_cutted", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  
  fhSvcVal->BookHistoList("ECalSel","ECal_TP_DE_Probe", 800, 0, 400);

  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DTHEVsDPHIAbs_probe_nocut", 1200, 0.,3*TMath::Pi(), 1200, 0., 3*TMath::Pi());

  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DTHEVsDPHIAbs_probe", 1200, 0.,3*TMath::Pi(), 1200, 0., 3*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DYvsDX_Hit_Probe", fNXBins*20,-(fXMax+fXMin),+(fXMax+fXMin), fNYBins*20,-(fYMax+fYMin),(fYMax+fYMin));
  fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DRvsDT_Hit_Probe", 800, -400,400, 1200, 0., TMath::Sqrt(((fXMax+fXMin)*(fXMax+fXMin))+((fYMax+fYMin)*(fYMax+fYMin))));

  //fhSvcVal->BookHistoList("ECalSel","ECal_TP_DPHIAbs_probe", 1200, 0.,4*TMath::Pi());
  PhiFullProbe= fhSvcVal->BookHistoList("ECalSel","ECal_TP_DPHIAbs_probe", 1200, 0.,4*TMath::Pi());
  if(!fHistoMode){
    for(int i=0; i<=NSlicesE; i++ ){
      PhiFullProbeSlice.push_back(fhSvcVal->BookHistoList("ECalSel",Form("ECal_TP_DPHIAbs_probe_slice_%i", i), 1200, 0.,4*TMath::Pi()));
  }
  }
  EofProbe = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEVsE_NOcut_probe", 100, 100,400, 800, -400, 400.);
  PhiofProbe = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DPhiVsPhi_NOcut_probe", 1200, 0.,3*TMath::Pi(), 1200, 0., 3*TMath::Pi());


  EofProbe_cut = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DEVsE_cut_probe", 100, 100,400, 800, -400, 400.);
  PhiofProbe_cut = fhSvcVal->BookHisto2List("ECalSel","ECal_TP_DPhiVsPhi_cut_probe", 1200, 0.,3*TMath::Pi(), 1200, 0., 3*TMath::Pi());


  // two-cluster sel

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DrVsDtAll", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DrVsDt", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_EVsT", 800, -400,400, 200, 0, 600.);
  fhSvcVal->BookHistoList("ECalSel","ECal_SC_Chi2", 100, 0., 20.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_Chi2vsNPaired", 10, 0., 10.,100, 0., 20. );

  fhSvcVal->BookHistoList("ECalSel","ECal_SC_NPaired", 10, 0., 10.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DEVsE", 100, 100,400, 800, -400, 400.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DE1VsDE2", 800, -400,400, 800, -400, 400.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DTHEVsDPHIAbs", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_DTHEVsEnergySum", 800, 0.,4*TMath::Pi(), 400, 0, 400);

  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_dphiElli", 200,0,40, 100, -TMath::Pi(), TMath::Pi());
  fhSvcVal->BookHisto2List("ECalSel","ECal_ElliAngle_ElliEnergy", 200,0.,40., 200, 0.,40.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_COGYX",100,-200,200,100,-200,200);
  fhSvcVal->BookHisto2List("ECalSel","ECalX_vs_Time",48,0,86400.,100,-200,200); //30-minute binning
  fhSvcVal->BookHisto2List("ECalSel","ECalY_vs_Time",48,0,86400.,100,-200,200); //30-minute binning
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_YX", fNXBins*10,fXMin,fXMax, fNYBins*10,fYMin,fYMax);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_EExpVsE", 800, 0,400, 400, 0, 400.);

  fhSvcVal->BookHistoList("ECalSel","NumberOfECalCluPairs", 5,0.,5.);
  fhSvcVal->BookHistoList("ECalSel","ECal_SC_E1plusE2", 300,0.,600.);
  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_E1E2VsMT", 800, -400,400, 300, 0.,600.);


  fhSvcVal->BookHisto2List("ECalSel","ECal_SC_E1E2VsDT", 300, -15,15, 300, 0.,600.);

  fhSvcVal->BookHistoList("ECalSel","ECal_SC_NCluPairperEvent", 30, 0., 30.);
  fhSvcVal->BookHistoList("ECalSel","ECal_SC_NCluPairperEvent_EnCut", 30, 0., 30.);





  fhSvcVal->BookHisto2List("ECalSel","DPhiVsRotVsRunCos",fNThetaBins,0,fNThetaBins*fThetaWid,fNPhiDirBins,0,2*TMath::Pi()); //to be done run-wise: first read runID, then book
  fhSvcVal->BookHisto2List("ECalSel","DPhiVsRotVsRun",fNThetaBins,0,fNThetaBins*fThetaWid,fNPhiDirBins,0,2*TMath::Pi()); //to be done run-wise: first read runID, then book


  const float fEExpMin = 90;
  const float fEExpMax = 410;
  const int fNEExpBins = 80;
  fhSvcVal->CreateList("ECalScale");

  for (int it = 0; it<fNXBins; it++){
    double xmin = fXMin + fXW*it;
    double xmax = fXMin + fXW*(it+1);

    for (int iq = 0; iq < fNYBins; iq++){
      double ymin = fYMin + fYW*iq;
      double ymax = fYMin + fYW*(iq+1);

      TString hname = Form("EExpVsEX_%d_Y_%d",it+1,iq+1);
      fhSvcVal->BookHisto2List("ECalScale",hname.Data(),fNEExpBins,fEExpMin,fEExpMax,100,-1,1);
//      fhSvcVal->GetHisto2List("ECalScale",hname.Data())->SetNameTitle
//	(Form("EExp vs E Xrange = (%f4.2,%f4.2)mm YRange = (%f4.2,%f4.2)",(float)xmin,(float)xmax,(float)ymin,(float)ymax));
    }
  }

  return true;
}


Bool_t ECalSel::TagProbeEff_macro(){
  TFile *fileIn = new TFile(InputHistofile.Data());
  if(!fileIn) std::cout<<"File not existing"<<std::endl;
  std::cout<<"File to analyze: "<< InputHistofile.Data()<<std::endl;
  PhiFullProbe =(TH1D*) fileIn->Get("ECalSel/ECal_TP_DPHIAbs_probe")->Clone();
  PhiFullProbe->Write();
  for(int i=0; i<NSlicesE; i++ ){
      PhiFullProbeSlice.push_back((TH1D*) fileIn->Get(Form("ECalSel/ECal_TP_DPHIAbs_probe_slice_%i", i))->Clone());
      if(!PhiFullProbeSlice[i]) std::cout<<"PhiFullProbeSlice["<<i<<"] non c'è "<<std::endl;
  }
  

  EofTag = (TH2D*) fileIn->Get("ECalSel/ECal_TP_DEVsE_NOcut_tag")->Clone(); 
  EofProbe_cut = (TH2D*) fileIn->Get("ECalSel/ECal_TP_DEVsE_cut_probe")->Clone(); 

  //MCTagProbeEff(); //wold be better if this one is called only if the fileIn is a MC prod -->but is tricky
  FitTagProbeEff();
  return true;
}


Bool_t ECalSel::MCTagProbeEff(){
      Double_t NumE=0.;
      Double_t DenE=0.;
      Double_t EffE=0.;
      // //Double_t NumPhi[NSlicesPhi]={0.};
      // // Double_t DenPhi[NSlicesPhi]={0.};
      // // Double_t EffPhi[NSlicesPhi]={0.};
      Double_t EnergyVal=0.;


      double Edown=fGeneralInfo->GetBeamEnergy()-fGeneralInfo->GetEnergyMax();
      double Eup=Edown+spacing;
      TString sliceOutname;
      
      sliceOutname = Form("MCsliceOut_%s", InputHistofile.Data());
      
      TFile *SliceOut = new TFile(sliceOutname, "recreate");
      std::cout<<"Slice output file: "<<sliceOutname<<std::endl;
      SliceOut->cd();
      
      TGraphErrors *EffGraphE = new TGraphErrors(NSlicesE);

      for(Int_t iSlice=0; iSlice<NSlicesE; iSlice++){

          //Tag
          EnergyVal = (Edown+Eup)/2;
           
          EofTag->GetXaxis()->SetRangeUser(Edown,Eup);
          TH1D *ProYTag =(TH1D*) EofTag->ProjectionY();
          ProYTag->SetName(Form("MCProYTag_%i",iSlice));
          ProYTag->GetXaxis()->SetRangeUser(-400, 400.);
          ProYTag->Write();

          TH1D *TagNoBackgroud = new TH1D(Form("MCTagNoBackgroud_%i",iSlice), Form("MCTagNoBackgroud_%i",iSlice),800, -400,400);
          for(int iBin =0; iBin<ProYTag->GetNbinsX(); iBin++){
              TagNoBackgroud->SetBinContent(iBin,ProYTag->GetBinContent(iBin)/*-ProYTarget->GetBinContent(iBin)*/);
              //std::cout<<"iBin: "<<iBin<<" Val Tag: "<<ProYTag->GetBinContent(iBin)<<" Val Target: "<<ProYTarget->GetBinContent(iBin)<<" Val New Histo: "<<ProYTag->GetBinContent(iBin)-ProYTarget->GetBinContent(iBin)<<std::endl;
          }
          TagNoBackgroud->GetXaxis()->SetRangeUser(-15,15);
          Int_t BinDown = TagNoBackgroud->FindBin(-15);
          Int_t BinUp = TagNoBackgroud->FindBin(15);

          TagNoBackgroud->Write();
          Double_t errDen=0;
          Double_t DenTemp =(Double_t) TagNoBackgroud->IntegralAndError(BinDown, BinUp, errDen);
          DenE= DenTemp;
          
          if(DenE==0 || std::isnan(DenTemp)){
            Eup+=spacing;
            Edown+=spacing;

            break;
          }

          //Probe 
          EofProbe_cut->GetXaxis()->SetRangeUser(Edown, Eup);
          TH1D *ProYProbe = (TH1D*) EofProbe_cut->ProjectionY();
          ProYProbe->SetName(Form("MCProbe_slice_%i",iSlice));
          ProYProbe->Write();
          TH1D *ProbeNoBackgroud = new TH1D(Form("MCProbeNoBackgroud_%i",iSlice), Form("MCProbeNoBackgroud_%i",iSlice),800, -400,400);
          for(int iBin =0; iBin<ProYTag->GetNbinsX(); iBin++){
              ProbeNoBackgroud->SetBinContent(iBin,ProYProbe->GetBinContent(iBin)/**ProbeBkgScale*/);
              //std::cout<<"iBin: "<<iBin<<" Val Tag: "<<ProYTag->GetBinContent(iBin)<<" Val Target: "<<ProYTarget->GetBinContent(iBin)<<" Val New Histo: "<<ProYTag->GetBinContent(iBin)-ProYTarget->GetBinContent(iBin)<<std::endl;
          }
          ProbeNoBackgroud->Write();
          Double_t errNum;
          Double_t NumTemp =(Double_t) ProbeNoBackgroud->IntegralAndError(0,800, errNum);
          NumE= NumTemp;
          if(std::isnan(NumTemp))
            {
            Eup+=spacing;
            Edown+=spacing;

             continue;
          }
          // if(Eup>(fGeneralInfo->GetBeamEnergy()-fGeneralInfo->GetEnergyMin())){

          //    break;
          // }
          EffE= NumTemp/DenTemp;
          std::cout<<" Slice: "<<iSlice<<" Emean: "<<EnergyVal<<" Num: "<<NumE<<" Den: "<<DenE<<" Eff: "<<EffE<<std::endl;
          double c1= (1/DenTemp)*(1/DenTemp)*errDen*errDen;
          double c2= (NumTemp/(DenTemp*DenTemp))*(NumTemp/(DenTemp*DenTemp))*errNum*errNum;
          Double_t errEffE = TMath::Sqrt(c1+c2);
         
          EffGraphE->SetPoint(iSlice,EnergyVal,EffE);
          EffGraphE->SetPointError(iSlice,0.,errEffE);

          Eup+=spacing;
          Edown+=spacing;

          
      }

      EffGraphE->SetTitle("MC EffGraphE; E_{beam}-E_{exp-tag} [MeV]; Efficiency");
      EffGraphE->SetName("MCEffGraphE");
      EffGraphE->Write();


      //overall efficiency evaluation

      EofTag->GetXaxis()->UnZoom();
      TH1D *ProYTagAll =(TH1D*) EofTag->ProjectionY();
      ProYTagAll->SetName(Form("MCProYTag_All"));
      ProYTagAll->GetXaxis()->SetRangeUser(-400, 400.);

      ProYTagAll->GetXaxis()->SetRangeUser(-15,15);
      Int_t BinDown = ProYTagAll->FindBin(-15);
      Int_t BinUp = ProYTagAll->FindBin(15);

      ProYTagAll->Write();
      Double_t errDen=0;
      Double_t DenTemp =(Double_t) ProYTagAll->IntegralAndError(BinDown, BinUp, errDen);
      DenE= DenTemp;
      
      
      //Probe 
      EofProbe_cut->GetXaxis()->UnZoom();
      TH1D *ProYProbeAll = (TH1D*) EofProbe_cut->ProjectionY();
      ProYProbeAll->SetName(Form("MCProbe_slice_All"));
      ProYProbeAll->Write();
      TH1D *ProbeNoBackgroudAll = new TH1D(Form("MCProbeNoBackgroud_All"), Form("MCProbeNoBackgroud_All"),800, -400,400);
      for(int iBin =0; iBin<ProYProbeAll->GetNbinsX(); iBin++){
          ProbeNoBackgroudAll->SetBinContent(iBin,ProYProbeAll->GetBinContent(iBin)/**ProbeBkgScale*/);
      }
      ProbeNoBackgroudAll->Write();
      Double_t errNum;
      Double_t NumTemp =(Double_t) ProbeNoBackgroudAll->IntegralAndError(0,800, errNum);
      NumE= NumTemp;
      
      EffE= NumTemp/DenTemp;
      double c1= (1/DenTemp)*(1/DenTemp)*errDen*errDen;
      double c2= (NumTemp/(DenTemp*DenTemp))*(NumTemp/(DenTemp*DenTemp))*errNum*errNum;
      Double_t errEffE = TMath::Sqrt(c1+c2);
      
      std::cout<<"############ Overall Efficiency for MC:  "<<EffE<<" +/- "<<errEffE<<"############" <<std::endl;




      
  return true;
}



Bool_t ECalSel::FitTagProbeEff(){
      Double_t NumE=0.;
      Double_t DenE=0.;
      Double_t EffE=0.;
      // //Double_t NumPhi[NSlicesPhi]={0.};
      // // Double_t DenPhi[NSlicesPhi]={0.};
      // // Double_t EffPhi[NSlicesPhi]={0.};
      Double_t EnergyVal=0.;
      TFile *target = new TFile("reco_run_385no_targ_full_slice.root");
      if(!target) {
        std::cout<<"!!!!!! File reco_run_385no_targ_full_slice.root not found-->exiting !!!!!! "<<std::endl;

        exit(1);
      }

      TH1D *PhiFullTarget;
      
      // PhiFullTarget = (TH1D*) target->Get("ECalSel/ECal_TP_DPHIAbs_probe")->Clone();


      double Edown=fGeneralInfo->GetBeamEnergy()-fGeneralInfo->GetEnergyMax();
      double Eup=Edown+spacing;
      TString sliceOutname;
      
      sliceOutname = Form("FitsliceOut_%s", InputHistofile.Data());
      
      TFile *SliceOut = new TFile(sliceOutname, "recreate");
      std::cout<<"Slice output file: "<<sliceOutname<<std::endl;
      SliceOut->cd();

      TGraphErrors *EffGraphE = new TGraphErrors(NSlicesE);

      for(Int_t iSlice=0; iSlice<NSlicesE; iSlice++){

          //Tag
          EnergyVal = (Edown+Eup)/2;
            
          EofTag->GetXaxis()->SetRangeUser(Edown,Eup);
          TH1D *ProYTag =(TH1D*) EofTag->ProjectionY();
           
          ProYTag->SetName(Form("FitProYTag_%i",iSlice));
          if(ProYTag->Integral()==0) break;
          TF1 *exponential = new TF1("exponential", "expo", -60,60); 
          exponential->SetParameter(0, 5);
          exponential->SetParameter(1, -1e-2);

          TFitResultPtr ExpFitResults= ProYTag->Fit(exponential,"RESQ");
          //TF1 *expGaus = new TF1("expGaus", "expo+gausn(2)", -60,60);
          // (0.5*(1.+TMath::Erf((15.-[3])/([4]*TMath::Sqrt(2.))))-0.5*(1.+TMath::Erf(([3]+15.)/([4]*TMath::Sqrt(2.)))))
          TF1 *expGaus = new TF1("expGaus", "expo+gausn(2)/(0.5*(1.+TMath::Erf((15.-[3])/([4]*TMath::Sqrt(2.))))-0.5*(1.+TMath::Erf((-[3]-15.)/([4]*TMath::Sqrt(2.)))))", -50,50); // +[2]*exp(((x-[3])/[4])*((x-[3])/[4]))
          expGaus->SetParameter(0, exponential->GetParameter(0));
          expGaus->SetParameter(1, exponential->GetParameter(1));

          expGaus->SetParameter(2, 1000);
          expGaus->SetParameter(3, -5);
          expGaus->SetParameter(4, 10);

          expGaus->SetParLimits(0, -10, 10);
          expGaus->SetParLimits(1, -1e-1,1e-1);
        
          
          expGaus->SetParLimits(2, 0, 10000);
          expGaus->SetParLimits(3, -10,3);
          expGaus->SetParLimits(4, 0, 20);
          
          
          TFitResultPtr fitResult = ProYTag->Fit(expGaus,"REQS");

          if(expGaus->GetChisquare()/expGaus->GetNDF()<0.6 ||expGaus->GetChisquare()/expGaus->GetNDF()>1.5) std::cout<<"For Slice "<<iSlice<<" Chi2 is out of range: "<<expGaus->GetChisquare()/expGaus->GetNDF()<<std::endl;
          ProYTag->Write();
          TF1 *expBkg = new TF1("expBkg", "exp([0]+[1]*x)", -60,60);
          expBkg->SetParameter(0, expGaus->GetParameter(0));
          expBkg->SetParameter(1, expGaus->GetParameter(1));
          
          Double_t BkgInt = (Double_t) expBkg->Integral(-15.,15.);

          
          Double_t DenTemp =(Double_t) (expGaus->Integral(-15.,15.))-BkgInt;
          DenE= DenTemp;
          Double_t errDen =  expGaus->GetParError(2);
          
          if(DenE==0 || std::isnan(DenTemp)){
            Eup+=spacing;
            Edown+=spacing;

            break;
          }

          //Probe 

          //new Part
          PhiFullTarget = (TH1D*) target->Get(Form("ECalSel/ECal_TP_DPHIAbs_probe_slice_%i",iSlice))->Clone();
          if(!PhiFullTarget) std::cout<<"I'm stupid, I can't find PhiFullTarget"<<std::endl;
          if(!PhiFullProbeSlice[iSlice]) std::cout<<"I'm stupid, I can't find PhiFullProbeSlice["<<iSlice<<"]"<<std::endl;
          SliceOut->cd();
          PhiFullTarget->GetXaxis()->SetRangeUser(0,2);
          Double_t scaleTargPhi = (Double_t) PhiFullTarget->Integral();
          PhiFullTarget->GetXaxis()->SetRangeUser(0,3*TMath::Pi());
          PhiFullProbeSlice[iSlice]->GetXaxis()->SetRangeUser(0,2);
          Double_t scaleProbePhi = (Double_t) PhiFullProbeSlice[iSlice]->Integral();
          PhiFullProbeSlice[iSlice]->GetXaxis()->SetRangeUser(0,3*TMath::Pi());
          PhiFullTarget->Scale(scaleProbePhi/scaleTargPhi);

          PhiFullTarget->GetXaxis()->SetRangeUser(2.8,3.4);
          PhiFullProbeSlice[iSlice]->GetXaxis()->SetRangeUser(2.8,3.4);
          PhiFullTarget->SetName(Form("PhiFullTarget_signal_%i", iSlice));
          PhiFullProbeSlice[iSlice]->SetName(Form("PhiFullProbe_signal_%i", iSlice));
          PhiFullTarget->Write();
          PhiFullProbeSlice[iSlice]->Write();
          Double_t ProbeBkgScale = 1 - (Double_t) PhiFullTarget->Integral()/(Double_t) PhiFullProbeSlice[iSlice]->Integral();
          std::cout<<"ProbeBkgScale: "<<ProbeBkgScale<<std::endl;



          //new part

           
          EofProbe_cut->GetXaxis()->SetRangeUser(Edown, Eup);
          TH1D *ProYProbe = (TH1D*) EofProbe_cut->ProjectionY();
          ProYProbe->SetName(Form("FitProbe_slice_%i",iSlice));
          ProYProbe->Write();
          TH1D *ProbeNoBackgroud = new TH1D(Form("FitProbeNoBackgroud_%i",iSlice), Form("FitProbeNoBackgroud_%i",iSlice),800, -400,400);
          for(int iBin =0; iBin<ProYTag->GetNbinsX(); iBin++){
              ProbeNoBackgroud->SetBinContent(iBin,ProYProbe->GetBinContent(iBin)*ProbeBkgScale);
              //std::cout<<"iBin: "<<iBin<<" Val Tag: "<<ProYTag->GetBinContent(iBin)<<" Val Target: "<<ProYTarget->GetBinContent(iBin)<<" Val New Histo: "<<ProYTag->GetBinContent(iBin)-ProYTarget->GetBinContent(iBin)<<std::endl;
          }
          ProbeNoBackgroud->Write();
          Double_t errNum;
          Double_t NumTemp =(Double_t) ProbeNoBackgroud->IntegralAndError(0,800, errNum);
          NumE= NumTemp;
          if(std::isnan(NumTemp))
            {
            Eup+=spacing;
            Edown+=spacing;

             continue;
          }
          // if(Eup>(fGeneralInfo->GetBeamEnergy()-fGeneralInfo->GetEnergyMin())){

          //    break;
          // }
          EffE= NumTemp/DenTemp;
          double c1= (1/DenTemp)*(1/DenTemp)*errDen*errDen;
          double c2= (NumTemp/(DenTemp*DenTemp))*(NumTemp/(DenTemp*DenTemp))*errNum*errNum;
          Double_t errEffE = TMath::Sqrt(c1+c2);
          std::cout<<" Slice: "<<iSlice<<" Emean: "<<EnergyVal<<" Num: "<<NumE<<" errNum: "<<errNum<<" Den: "<<DenE<<" P2: "<<expGaus->GetParameter(2)<<" Ratio: "<<DenE/expGaus->GetParameter(2)<<" Eff: "<<EffE<<" errEff"<<std::endl; 
          EffGraphE->SetPoint(iSlice,EnergyVal,EffE);
          EffGraphE->SetPointError(iSlice, 0, errEffE);
          Eup+=spacing;
          Edown+=spacing;

          
      }

      EffGraphE->SetTitle("Fit EffGraphE; E_{beam}-E_{exp-tag} [MeV]; Efficiency");
      EffGraphE->SetName("FitEffGraphE");
      EffGraphE->Write();

      //Overall efficiency 

      EofTag->GetXaxis()->UnZoom();
      TH1D *ProYTagAll =(TH1D*) EofTag->ProjectionY();
      ProYTagAll->SetName(Form("FitProYTag_All"));
      ProYTagAll->GetXaxis()->SetRangeUser(-400, 400.);

     

      TF1 *exponential = new TF1("exponential", "expo", -60,60); 
      exponential->SetParameter(0, 5);
      exponential->SetParameter(1, -1e-2);

      TFitResultPtr ExpFitResults= ProYTagAll->Fit(exponential,"RESQ");
      //TF1 *expGaus = new TF1("expGaus", "expo+gausn(2)", -60,60);
      // (0.5*(1.+TMath::Erf((15.-[3])/([4]*TMath::Sqrt(2.))))-0.5*(1.+TMath::Erf(([3]+15.)/([4]*TMath::Sqrt(2.)))))
      TF1 *expGaus = new TF1("expGaus", "expo+gausn(2)/(0.5*(1.+TMath::Erf((15.-[3])/([4]*TMath::Sqrt(2.))))-0.5*(1.+TMath::Erf((-[3]-15.)/([4]*TMath::Sqrt(2.)))))", -50,50); // +[2]*exp(((x-[3])/[4])*((x-[3])/[4]))
      expGaus->SetParameter(0, exponential->GetParameter(0));
      expGaus->SetParameter(1, exponential->GetParameter(1));

      expGaus->SetParameter(2, 10000);
      expGaus->SetParameter(3, -5);
      expGaus->SetParameter(4, 10);

      expGaus->SetParLimits(0, -10, 10);
      expGaus->SetParLimits(1, -1e-1,1e-1);
    
      
      expGaus->SetParLimits(2, 0, 100000);
      expGaus->SetParLimits(3, -10,3);
      expGaus->SetParLimits(4, 0, 20);
      
      
      TFitResultPtr fitResult = ProYTagAll->Fit(expGaus,"REQS");


      if(expGaus->GetChisquare()/expGaus->GetNDF()<0.6 ||expGaus->GetChisquare()/expGaus->GetNDF()>1.5) std::cout<<" Chi2 is out of range: "<<expGaus->GetChisquare()/expGaus->GetNDF()<<std::endl;
      ProYTagAll->Write();
      TF1 *expBkg = new TF1("expBkg", "exp([0]+[1]*x)", -60,60);
      expBkg->SetParameter(0, expGaus->GetParameter(0));
      expBkg->SetParameter(1, expGaus->GetParameter(1));
      
      Double_t BkgInt = (Double_t) expBkg->Integral(-15.,15.);

      
      Double_t DenTempAll =(Double_t) (expGaus->Integral(-15.,15.))-BkgInt;
      Double_t errDenAll =  expGaus->GetParError(2);
      // Int_t BinDown = TagNoBackgroudAll->FindBin(-15);
      // Int_t BinUp = TagNoBackgroudAll->FindBin(15);

       
      //Probe 

      PhiFullTarget = (TH1D*) target->Get(Form("ECalSel/ECal_TP_DPHIAbs_probe"))->Clone();

      PhiFullTarget->GetXaxis()->SetRangeUser(0,2);
      Double_t scaleTargPhi = (Double_t) PhiFullTarget->Integral();
      PhiFullTarget->GetXaxis()->SetRangeUser(0,3*TMath::Pi());
      PhiFullProbe->GetXaxis()->SetRangeUser(0,2);
      Double_t scaleProbePhi = (Double_t) PhiFullProbe->Integral();
      PhiFullProbe->GetXaxis()->SetRangeUser(0,3*TMath::Pi());
      PhiFullTarget->Scale(scaleProbePhi/scaleTargPhi);

      PhiFullTarget->GetXaxis()->SetRangeUser(2.8,3.4);
      PhiFullProbe->GetXaxis()->SetRangeUser(2.8,3.4);
      PhiFullTarget->SetName("PhiFullTarget_signal");
      PhiFullProbe->SetName("PhiFullProbe_signal");
      PhiFullTarget->Write();
      PhiFullProbe->Write();
      Double_t ProbeBkgScale = 1 - (Double_t)PhiFullTarget->Integral()/(Double_t) PhiFullProbe->Integral();
      std::cout<<"ProbeBkgScale: "<<ProbeBkgScale<<std::endl;

      EofProbe_cut->GetXaxis()->UnZoom();
      TH1D *ProYProbeAll = (TH1D*) EofProbe_cut->ProjectionY();
      ProYProbeAll->SetName(Form("FitProbe_slice_All"));
      ProYProbeAll->Write();
      TH1D *ProbeNoBackgroudAll = new TH1D(Form("FitProbeNoBackgroud_All"), Form("FitProbeNoBackgroud_All"),800, -400,400);
      for(int iBin =0; iBin<ProYProbeAll->GetNbinsX(); iBin++){
          ProbeNoBackgroudAll->SetBinContent(iBin,ProYProbeAll->GetBinContent(iBin)*ProbeBkgScale);
      }
      ProbeNoBackgroudAll->Write();
      Double_t errNumAll;
      Double_t NumTempAll =(Double_t) ProbeNoBackgroudAll->IntegralAndError(0,800, errNumAll);
      
      EffE= NumTempAll/DenTempAll;
      double c1= (1/DenTempAll)*(1/DenTempAll)*errDenAll*errDenAll;
      double c2= (NumTempAll/(DenTempAll*DenTempAll))*(NumTempAll/(DenTempAll*DenTempAll))*errNumAll*errNumAll;
      Double_t errEffE = TMath::Sqrt(c1+c2);

      std::cout<<"NumAll: "<<NumTempAll<<" DenTempAll: "<<DenTempAll<<std::endl;
      
      std::cout<<"############ Overall Efficiency for FIT:  "<<EffE<<" +/- "<<errEffE<<"############" <<std::endl;




      
  return true;
}





Bool_t ECalSel::Finalize(){
  
  if(fHistoMode) TagProbeEff_macro();
  return true;
}
