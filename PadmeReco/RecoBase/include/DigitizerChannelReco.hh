// Written By M. Raggi 6/12/2019
// Copied from ECal by Beth 7/6/19
#ifndef DIGITIZER_CHANNEL_RECO_H
#define DIGITIZER_CHANNEL_RECO_H

#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include <map>
#include "TFile.h"
#include "TList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class DigitizerChannelReco : public ChannelVReco {
public:
  DigitizerChannelReco(){;};
  virtual ~DigitizerChannelReco();

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg){return;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);
  virtual void PrepareDebugHistos();
  virtual void PrepareTmpHistos();
  virtual void SaveDebugHistos();
  
  void SetGlobalRunningMode(GlobalRecoConfigOptions* o){fGlobalMode = o;}
  void SetLocalRunningMode(LocalRecoConfigOptions* o)  {fLocalMode  = o;}
  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  void ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray);
  void RCVoltage(Double_t* sampleArray, Double_t* hitArray, Int_t nosamples);
  void PrintConfig();

  void ResetPedestal();

  Bool_t pedestalsFromFirstSamples() {return (fPedestalMode==0);}
  Bool_t pedestalsFromAutoTrgEvents(){return (fPedestalMode==1);}
  Bool_t hybridPedestals()        {return (fPedestalMode==2);}

  Short_t CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcChargeSin(UShort_t); //single hit pedestal M. Raggi 22/01/2019
  Double_t CalcTime(UShort_t);
  Double_t CalcTimeSing(UShort_t);
  Double_t CalcTimeOver(UShort_t);
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t,UShort_t); //Beth 7/6/19 - used for multihit reconstruction 

  void HistoFill();

  //Standard hits corrections
  Double_t ScaleToFullInt(UShort_t);    // Scale the integral of the signal independetly of the start time. 
  Bool_t   IsSaturated();   // Check if the signal is saturated
  Double_t CorrectSaturation(UShort_t); // Correct saturated signals
  Double_t CorrectIntegrationTime(Double_t TStart,Double_t TStop); // Corrects for charge outside integration window

  Double_t ZSupHit(Float_t thr,UShort_t NAvg);  //M. Raggi 30/10/2018

  Short_t GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};
  void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin);

  void SetAbsSignals();
  
  Int_t GetChID(){return fChID;};
  void  SetChID(Int_t ChID){fChID=ChID;};

  Int_t GetElChID(){return fElChID;};
  void  SetElChID(Int_t ChID){fElChID=ChID;};

  Int_t GetBdID(){return fBdID;};
  void  SetBdID(Int_t BdID){fBdID=BdID;};

  Int_t GetTrigMask(){return fTrigMask;};
  void  SetTrigMask(Int_t TrigMask){fTrigMask=TrigMask;};

  void SetAnalogOffSets();
  void SaveBDPed(Int_t BID);


  //  void OverSample4(Double_t *v, Double_t *o,Int_t n);
  void OverSample4(Short_t *v, Double_t *o,Int_t n);
  Int_t GetStartTime(Double_t *v,Int_t nshift);

private:
  //What do we operate
  UShort_t fNSamples;
  Short_t *fSamples;
  Short_t fMax;
  Short_t fIMax;
  Double_t fPed;
  Double_t fCharge;
  // Double_t fChargeSin;
  Double_t fTime;
  Double_t fTimeSin;
  Double_t fTimeOv;
  UShort_t fNPedSamples;

  Int_t fChID;
  Int_t fElChID;
  Int_t fBdID;
  Int_t fTrigMask;
  Double_t fRMS1000;
  Double_t fRMS200;
  Double_t fAvg200;
  Double_t fTrig;

  // Added connection to general configuration 
  utl::ConfigParser *fConfigParser;
  PadmeVRecoConfig *fConfig;
  Int_t fRunType;

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  Int_t fPedestalMode;
  Int_t fProcessing;
  
  Int_t fMinAmplitude;

  Double_t fSignalThreshold;
  Double_t fSignalPercentage;
  
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;
  Double_t fAmpli1;//1 refers to multihit
  Double_t fAmpli2;//2 refers to RC processing

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fMultihit;
  Bool_t fUseAbsSignals;
  Bool_t fUseOverSample;
  Bool_t fIntCorrection;

  Bool_t fNewPed;
  Bool_t fTimeCut;
  
  Double_t fPedCh[32];//Adc channel pedestals
  std::map < std::pair<int,int>,double> fPedMap;
  //  std::map < std::pair<int,int>,double> fCalibMap;   //moved into ECalCalibration class M. Raggi 19/04/2019
  //  std::map < std::pair<int,int>,double> fT0Map;       //moved into ECalCalibration class M. Raggi 19/04/2019

  std::map < std::pair<int,int>,double> TempPedMap;
  std::map < std::pair<int,int>,double> TempMapCount;
  //mode variables
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;


  Int_t fUsePulseProcessing ;
  Int_t fPeakSearchWidth    ;
  Double_t fZeroSuppression    ;
  Double_t fChargeCut          ;
  Double_t fDPParameterR1      ;
  Double_t fDPParameterR2      ;
  Double_t fDPParameterC       ;
  

  /*
  Bool_t fIsPed;
  Bool_t fIsReco;
  Bool_t fIsCosmic;
  Bool_t fIsMonitor;
  Int_t  fIsGlobalDebug;
  Int_t  fIsLocalDebug;
  */

  // Debug histogram structure
  TFile * fileOut;
  TH1D ** hPedCalo;
  TH1D ** hAvgCalo;
  TH1D ** hPedMean;
  TH1D ** hVMax;
  TH1D ** h200QCh;
  TH1D ** hQCh;

  TH1D *  hVPeak;
  TH1D *  hMean;
  TH1D *  hAvg;
  TH1D *  h200Q;
  TH1D *  hQ;

  TH1F * hSignalTmp;
  TH1F * hdxdtTmp;

  TH1F * hSignal[10];
  TH1F * hSigFinal;
  TH1F * hSignalSmooth[10];

  TH1F * hdxdt[10];
  TH1F * hdxdtFinal;
  TH1F * hdxdtSmooth[10];

  TH1F * hDerivRatio[10];
  TH1F * hDerivRatioSmooth[10];

  TH1F * hPedSpectrum[96];

  TH1F * hAmpSpectrum[96];
  TH1F * hAmpChargeRatio[96];
  TH2F * hAmpCharge2D[96];

  TH1F * hAmpSpectrumRC[96];
  TH1F * hAmpChargeRatioRC[96];
  TH2F * hAmpCharge2DRC[96];


  TH1F * hdxdtMaxTime;
  TH1F * hSigMax;
  TH1F * hRCVoltage[10];

  TH1F * hTrialSignal;
  TH1F * hRCProcessedTrialSignal;
  TH1F * hChargeInt;
  TH1F * hChargeIntRC;
  TH1F * hdxdtRMS;
  TH1F * hTime;
  TH1F * hTIntCorr;
  TH1F * hTimeCut;
  TH1F * hTimeOv;
  TH1F * hSat;
  TH1F * hDiff;

  TH1F * hSigOv;
  TH1F * hSigOvSm;
  TH1F * hdxdtSigOvSm;



  TH1D *  H1;
  TH1D *  H2;
  //  Int_t m;
  Double_t Zsup;
  Double_t VMax;
  Double_t HitE;
  Double_t HitE200;
  Double_t HitEHyb;
  Double_t HitT;
  Int_t ElCh;
  Int_t Row;
  Int_t Col;
  Int_t IsSat;

  // calc charge
  Double_t AbsSamRec[1024];
  Double_t AbsSamRec200[1024];
  Double_t AbsSamRecHyb[1024];

  //Temporary histo pointers
  TH1D* histo;
  TH1D* histo1;
  TH1D* histo2;
  TH1D* histo3;
  TH1D* histoSat;

  TH1D* histoOv;
  TH1D* histoDiff;

  TList* hListCal; // single board related histograms 
  TList* hListWaveform; //Waveform histograms
  TList* hListAmp; //Amplitude spectra
  TList* hListPed; //Pedestal spectra
  TList* hListAmpChargeRatio; //Spectra of ratio of amplitude to charge
  TList* hListAmpCharge2D; //2D amplitude/charge plots
  TList* hListEv;  // More general histograms 
  TList* hListTmp;  // More general histograms 

  TList* hListCalRC; // single board related histograms 
  TList* hListWaveformRC; //Waveform histograms
  TList* hListAmpRC; //Amplitude spectra
  TList* hListPedRC; //Pedestal spectra
  TList* hListAmpChargeRatioRC; //Spectra of ratio of amplitude to charge
  TList* hListAmpCharge2DRC; //2D amplitude/charge plots
  TList* hListEvRC;  // More general histograms 
  TList* hListTmpRC;  // More general histograms 

  TTree* Reco;
  
};
#endif
