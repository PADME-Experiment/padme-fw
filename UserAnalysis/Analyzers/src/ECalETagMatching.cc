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

    tempClu[0] = fECal_clEvent->Element((int)ECalEv->indexECal[0]);
    tempClu[1] = fECal_clEvent->Element((int)ECalEv->indexECal[1]);
    cluPos[0] = tempClu[0]->GetPosition();
    cluPos[1] = tempClu[1]->GetPosition();

    ECalClu[0] = fETagAn->getETagECalAss(ECalEv->indexETagAss[0]);
    ECalClu[1] = fETagAn->getETagECalAss(ECalEv->indexETagAss[1]);


    if((ECalClu[0]->nAss[0]==0 && ECalClu[0]->nAss[1]==0) && (ECalClu[1]->nAss[0]==0 && ECalClu[1]->nAss[1]==0)){
        	fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_annihil"),ECalEv->totalE); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_annihil",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_annihil", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_annihil", cluPos[1].X(), cluPos[1].Y());


    }else if((ECalClu[0]->assType & (1<<kSingleSideMatch))>0 && (ECalClu[1]->assType & (1<<kSingleSideMatch))>0){
          fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_bhabha"),ECalEv->totalE); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_bhabha",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_bhabha", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_bhabha", cluPos[0].X(), cluPos[0].Y());


          ECalEv->flagEv = ev_ee;
    }else if(((ECalClu[0]->assType & (1<<kSingleSideMatch))>0 && (ECalClu[1]->nAss[0]==0 && ECalClu[1]->nAss[1]==0)) ||((ECalClu[1]->assType & (1<<kSingleSideMatch))>0 && (ECalClu[0]->nAss[0]==0 && ECalClu[0]->nAss[1]==0))){
          fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_brem"),ECalEv->totalE); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_brem",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_brem", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_brem", cluPos[0].X(), cluPos[0].Y());


            ECalEv->flagEv = ev_eg;

    }else{
          fhSvcVal->FillHistoList("ECalETagMatching",Form("ECalETag_noAss"),ECalEv->totalE); 
          fhSvcVal->FillHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_noAss",ECalEv->phi, ECalEv->theta );
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_noAss", cluPos[0].X(), cluPos[0].Y());
          fhSvcVal->FillHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_noAss", cluPos[0].X(), cluPos[0].Y());


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
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_annihil", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_annihil", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_annihil", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalSel", "ECal_SC_COGYX_annihil", 100, -200, 200, 100, -200, 200);


	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_bhabha"),400,0,400); 
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_bhabha", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_bhabha", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_bhabha", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalSel", "ECal_SC_COGYX_bhabha", 100, -200, 200, 100, -200, 200);


	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_brem"),400,0,400); 
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_brem", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_brem", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_brem", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalSel", "ECal_SC_COGYX_brem", 100, -200, 200, 100, -200, 200);


	fhSvcVal->BookHistoList("ECalETagMatching",Form("ECalETag_noAss"),400,0,400); 
  fhSvcVal->BookHisto2List("ECalETagMatching","ECalETag_DTHEVsDPHIAbs_noAss", 800, 0.,4*TMath::Pi(), 800, 0., 4*TMath::Pi());
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu0_noAss", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);
  fhSvcVal->BookHisto2List("ECalETagMatching", "ECal_SC_yvsx_clu1_noAss", fNXBins * 10, fXMin, fXMax, fNYBins * 10, fYMin, fYMax);

  fhSvcVal->BookHisto2List("ECalSel", "ECal_SC_COGYX_noAss", 100, -200, 200, 100, -200, 200);

  return true;
}


Bool_t ECalETagMatching::Finalize(){
  


  return true;
}
