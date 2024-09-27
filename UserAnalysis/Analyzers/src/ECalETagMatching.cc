#include "ECalETagMatching.hh"

#include "TRecoEvent.hh"
#include "TRecoVObject.hh"
#include "TRecoVHit.hh"
#include "TRecoVClusCollection.hh"
#include "TRecoVCluster.hh"
#include "HistoSvc.hh"
#include "TVector3.h"
#include "TProfile.h"
#include "TString.h"
#include "TLorentzVector.h"
#include <iostream>

ECalETagMatching* ECalETagMatching::fInstance = 0;
ECalETagMatching* ECalETagMatching::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalETagMatching(); }
  return fInstance;
}

ECalETagMatching::ECalETagMatching()
{
  
  fRecoEvent   = NULL;
  fVerbose        = 0;
  fhSvcVal = HistoSvc::GetInstance();
  fECalSel = ECalSel::GetInstance(); 
  fETagAn = ETagAn::GetInstance(); 
  fGeneralInfo = GeneralInfo::GetInstance(); 


}
ECalETagMatching::~ECalETagMatching()
{
}

Bool_t ECalETagMatching::Init(PadmeAnalysisEvent* event){
  fRecoEvent = event->RecoEvent;
  fECal_hitEvent = event->ECalRecoEvent;
  fECal_clEvent = event->ECalRecoCl; 
  

  // init the left/right ETag maps

  InitHistos();

  return true;
}


Bool_t ECalETagMatching::Process()
{
  //clear delle struct??
  ECalETagMatch();
  return true;
}

Int_t ECalETagMatching::ECalETagMatch(){
  // 
  // given single clusters compatible with the beam e+
  // try to match ETag activity to the ECalCluster -> fraction of matches
  for (int i=0; i< fECalSel->getNECalEvents(); i++){ // loop over ecal events found
    ECalSelEvent *ECalEv = fECalSel->getECalEvent(i);
    if (ECalEv->flagEv != ev_gg && ECalEv->flagEv != ev_ee) continue; 


    ETagECalAss *ECalClu[2];
    TVector3 cluPos[2];
    TRecoVCluster *tempClu[2];
    TLorentzVector labMomenta[2];
    TLorentzVector labMomentaCM[2];

    tempClu[0] = fECal_clEvent->Element((int)ECalEv->indexECal[0]);
    tempClu[1] = fECal_clEvent->Element((int)ECalEv->indexECal[1]);
    cluPos[0] = tempClu[0]->GetPosition();
    cluPos[1] = tempClu[1]->GetPosition();

    ECalClu[0] = fETagAn->getETagECalAss(ECalEv->indexETagAss[0]);
    ECalClu[1] = fETagAn->getETagECalAss(ECalEv->indexETagAss[1]);

    labMomenta[0] = ECalEv->labP[0];
    labMomenta[1] = ECalEv->labP[1];
    labMomentaCM[0] = ECalEv->cmP[0];
    labMomentaCM[1] = ECalEv->cmP[1];
    
    Double_t PhiCluster = labMomenta[0].Vect().Phi();
    if (TMath::Abs(cluPos[1].Y() - fGeneralInfo->GetCOG().Y()) < TMath::Abs(cluPos[0].Y() - fGeneralInfo->GetCOG().Y()))
      {
        PhiCluster = labMomenta[1].Vect().Phi();
      }
      


    if((ECalClu[0]->nAss[0]==0 && ECalClu[0]->nAss[1]==0) && (ECalClu[1]->nAss[0]==0 && ECalClu[1]->nAss[1]==0)){
        	
          double eff1=1, eff2=1;
          TFile *fEffETag = new TFile("/home/dimeco/calibTools/padme-fw/UserAnalysis/efftpgg.root");
          if(!fEffETag->IsOpen()) std::cout<< "efftpgg.root was not opened"<<std::endl;
          TH2D *hEff = (TH2D*) fEffETag->Get("ECal_ETag_GammaGamma_Num")->Clone();
          if(hEff == nullptr) std::cout<<"ECal_ETag_GammaGamma_Num does not exist"<<std::endl;

          eff1 = hEff->GetBinContent((ECalClu[0]->nETagBar)+1, (ECalClu[0]->closestSide)+1);
          eff2 = hEff->GetBinContent(hEff->FindBin( ECalClu[1]->nETagBar, ECalClu[1]->closestSide));
          // std::cout<<"Sono sopravvissuto con al getbincontent 2: "<<eff2<<std::endl;
          fEffETag->Close();

          
          fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_annihil"),ECalEv->totalE); 
        	fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_annihil_weighted"),ECalEv->totalE, eff1*eff2); 
        	fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_weights"),eff1*eff2); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_annihil",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_annihil", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_annihil", cluPos[1].X(), cluPos[1].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_COGYX_annihil",(ECalEv->cog).X() ,(ECalEv->cog).Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_annihil"), PhiCluster, labMomenta[0].Vect().Theta() + labMomenta[1].Vect().Theta(), 1);
          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_annihil"), PhiCluster, labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1);


    }else if((ECalClu[0]->assType & (1<<kSingleSideMatch))>0 && (ECalClu[1]->assType & (1<<kSingleSideMatch))>0){
          fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_bhabha"),ECalEv->totalE); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_bhabha",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_bhabha", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_bhabha", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_COGYX_bhabha",(ECalEv->cog).X() ,(ECalEv->cog).Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_bhabha"), PhiCluster, labMomenta[0].Vect().Theta() + labMomenta[1].Vect().Theta(), 1);
          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_bhabha"), PhiCluster, labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1);


          ECalEv->flagEv = ev_ee;
    }else if(((ECalClu[0]->assType & (1<<kSingleSideMatch))>0 && (ECalClu[1]->nAss[0]==0 && ECalClu[1]->nAss[1]==0)) ||((ECalClu[1]->assType & (1<<kSingleSideMatch))>0 && (ECalClu[0]->nAss[0]==0 && ECalClu[0]->nAss[1]==0))){
          fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_brem"),ECalEv->totalE); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_brem",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_brem", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_brem", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_COGYX_brem",(ECalEv->cog).X() ,(ECalEv->cog).Y());

          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_brem"), PhiCluster, labMomenta[0].Vect().Theta() + labMomenta[1].Vect().Theta(), 1);
          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_brem"), PhiCluster, labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1);

          ECalEv->flagEv = ev_eg;

    }else{
          fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_noAss"),ECalEv->totalE); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_noAss",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_noAss", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_noAss", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_COGYX_noAss",(ECalEv->cog).X() ,(ECalEv->cog).Y());

          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_noAss"), PhiCluster, labMomenta[0].Vect().Theta() + labMomenta[1].Vect().Theta(), 1);
          fhSvcVal->FillHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_noAss"), PhiCluster, labMomentaCM[0].Vect().Theta() + labMomentaCM[1].Vect().Theta(), 1);

          ECalEv->flagEv = ev_unknown;
    }
  }
  return 0;
}



Bool_t ECalETagMatching::InitHistos()
{ 


  double fXMin = -21. * (14 + 0.5);
  double fXMax = 21. * (14 + 0.5);
  double fXW = 21; // mm
  int fNXBins = (fXMax - fXMin) / fXW;
  double fYMin = -21. * (14 + 0.5);
  double fYMax = 21. * (14 + 0.5);
  double fYW = 21; // mm

  int  fNYBins = (fYMax - fYMin) / fYW;

  fhSvcVal->CreateList("ECalETagMatching");
	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_annihil"),400,0,400); 
	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_annihil_weighted"),400,0,400); 
	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_weights"),100,0,1); 
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_annihil", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_annihil", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_annihil", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_COGYX_annihil", 100, -200, 200, 100, -200, 200);
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_annihil"), 900, -TMath::Pi(), TMath::Pi(), 900, 0., 3 * TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_annihil"), 900, -TMath::Pi(), TMath::Pi(), 900, 0., TMath::Pi());


	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_bhabha"),400,0,400); 
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_bhabha", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_bhabha", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_bhabha", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_COGYX_bhabha", 100, -200, 200, 100, -200, 200);
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_bhabha"), 900, -TMath::Pi(), TMath::Pi(), 900, 0.,  TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_bhabha"), 900, -TMath::Pi(), TMath::Pi(), 600, 0., 2 * TMath::Pi());


	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_brem"),400,0,400); 
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_brem", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_brem", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_brem", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_COGYX_brem", 100, -200, 200, 100, -200, 200);
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_brem"), 900, -TMath::Pi(), TMath::Pi(), 900, 0.,  TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_brem"), 900, -TMath::Pi(), TMath::Pi(), 900, 0., 2 * TMath::Pi());


	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_noAss"),400,0,400); 
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_noAss", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_noAss", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_noAss", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_COGYX_noAss", 100, -200, 200, 100, -200, 200);
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHEVsPhi_Lab_noAss"), 900, -TMath::Pi(), TMath::Pi(), 900, 0., TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", Form("ECal_SC_DTHECMVsPhi_Lab_noAss"), 900, -TMath::Pi(), TMath::Pi(), 900, 0., 2* TMath::Pi());

  return true;
}


Bool_t ECalETagMatching::Finalize(){
  


  return true;
}
