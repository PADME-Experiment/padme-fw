#ifndef ECalCalib22_h
#define ECalCalib22_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "GeneralInfo.hh"
#include "HistoSvc.hh"
#include "TH1D.h"

class ECalCalib22 {


public:

  ECalCalib22(TString c = "config/UserAnalysis.conf", Int_t v = 0);

  ~ECalCalib22();
  static ECalCalib22* GetInstance();


  Bool_t Init(Bool_t fHistoMode,TString InputHistofile);
  Bool_t Process(PadmeAnalysisEvent*);
  Bool_t ChannelLandauFit();
  Bool_t Finalize();

 

private:
  static ECalCalib22* fInstance;
  Bool_t InitHistos();

  //  TRandom3* fRndm;

  Int_t fVerbose;

  PadmeAnalysisEvent* fEvent;
  GeneralInfo* fGeneralInfo;

  utl::ConfigParser* fCfgParser;
  Bool_t NewCalib = kFALSE; //---> Enables the creation of a new Calibration File 

  static const int NTOTCh = 613;
  Bool_t fHistoMode;
  TString InputHistofile;
  TString InputHistofileName;

  HistoSvc* fHS;
  Int_t fNClusters;
  Int_t ECalChNum[NTOTCh]={0};
  TH1D *ECalChHisto[NTOTCh];
  TH1D *ECalChHistoVert[NTOTCh];
  Double_t ECalChEff[NTOTCh]={0.};
  Double_t ECalChMVP[NTOTCh]={0.};
  Int_t MiddleCross[NTOTCh]={0};
  Int_t Cross[NTOTCh]={0};
  Int_t FCol[NTOTCh]={0};
  Int_t FRow[NTOTCh]={0};
  Int_t FBdId[NTOTCh]={0};
  Int_t FChId[NTOTCh]={0};
  Double_t OldCalib[NTOTCh]={0};
  int p=0;
  TString fNRun;	     
  Int_t fCurrentRun;     
  Int_t fCurrentRunIndex;

  // variable to be returned on request
  
};
#endif
