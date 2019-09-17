#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TList.h"
#include "TTree.h"

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class DigitizerChannelReco : public ChannelVReco {
public:
  DigitizerChannelReco(){;};
  virtual ~DigitizerChannelReco();

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg){return ;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);

  //Beth 7/6/19 copied from Mauro's DigitizerChannelECal structure to create a set of debug histograms that are produced only in debug mode
  virtual void PrepareDebugHistos();
  virtual void PrepareTmpHistos();
  virtual void SaveDebugHistos();

  void SetGlobalRunningMode(GlobalRecoConfigOptions* o){fGlobalMode = o;}
  void SetLocalRunningMode(LocalRecoConfigOptions* o)  {fLocalMode  = o;}
  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  void ReconstructMultiHit (std::vector<TRecoVHit *> &hitArray);
  void PrintConfig();

  Short_t CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcTime(UShort_t);
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t,UShort_t);

  Double_t ZSupHit(Float_t thr,UShort_t NAvg);  //M. Raggi 30/10/2018

  void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin);
  void HistoFill();//Beth 30/7/19 fills debugging histograms created in PrepareDebugHistos

  void SetAbsSignals();

  Int_t GetChID(){return fChID;};
  void  SetChID(Int_t ChID){fChID=ChID;};
  
  Int_t GetElChID(){return fElChID;};
  void  SetElChID(Int_t ChID){fElChID=ChID;};

  Int_t GetBdID(){return fBdID;};
  void  SetBdID(Int_t BdID){fBdID=BdID;};

  Int_t GetTrigMask(){return fTrigMask;};
  void  SetTrigMask(Int_t TrigMask){fTrigMask=TrigMask;};

private:
  //What do we operate
  UShort_t fNSamples;
  Short_t *fSamples;
  Short_t fMax;
  Short_t fIMax;
  Double_t fPed;
  Double_t fCharge;
  Double_t fTime;
  UShort_t fNPedSamples;

  Int_t fChID;
  Int_t fElChID;
  Int_t fBdID;
  Int_t fTrigMask;

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  Int_t fProcessing;
  
  Int_t fMinAmplitude;  

  Double_t fSignalThreshold;
  Double_t fSignalPercentage;
  
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;
  Double_t fAmpli1;
  Double_t fAmpli2;

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fUseAbsSignals;
  Bool_t fUseOverSample;

  //Beth 30/7/19 to investigate different stages of improvement in reconstruction
  Bool_t fNewPed;
  Bool_t fTimeCut;

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
  //Veto variables 
  Double_t fCh        [100];
  Double_t fCry       [100];//fCrystal mapping
  Double_t fMeasure   [100];//fCrystal position in mm
  Double_t fPedCh     [100];//Adc channel pedestals
  Double_t fQCh       [100];//Channel charge
  Double_t fCalibCh   [100];//Calibration Constant fStrips
  Double_t fEConvConst[100];//Calibration Constant fStrips
  Double_t fQtot        ;
  Double_t fXbeam       ;
  Double_t fYbeam       ;
  */

  Short_t  GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};

  // Debug histogram structure
  TFile * fileOut;

  TH1F * hSignalTmp;
  TH1F * hdxdtTmp;
  TH1F * hdxdtFinal;

  TH1F * hSignal[96];
  TH1F * hRCVoltage[96];
  TH1F * hPedSpectrum[96];

  TH1F * hAmpSpectrum[96];
  TH1F * hAmpChargeRatio[96];
  TH2F * hAmpCharge2D[96];

  TH1F * hAmpSpectrumRC[96];
  TH1F * hAmpChargeRatioRC[96];
  TH2F * hAmpCharge2DRC[96];


  TH1F * hdxdtMaxTime;
  TH1F * hSigMax;
  TH1F * hSigFinal;

  TH1F * hSat;

  TH1F * hSigOv;
  TH1F * hSigOvSm;
  TH1F * hdxdtSigOvSm;

  TH1D *  H1;
  TH1D *  H2;
  //  Int_t m;
  Int_t ElCh;
  // calc charge
  Double_t AbsSamRec[1024];

  TList* hListWaveform; //Waveform histograms
  TList* hListAmp; //Amplitude spectra
  TList* hListPed; //Pedestal spectra
  TList* hListAmpChargeRatio; //Spectra of ratio of amplitude to charge
  TList* hListAmpCharge2D; //2D amplitude/charge plots
  TList* hListTmp;  // More general histograms 

  TList* hListWaveformRC; //Waveform histograms
  TList* hListAmpRC; //Amplitude spectra
  TList* hListPedRC; //Pedestal spectra
  TList* hListAmpChargeRatioRC; //Spectra of ratio of amplitude to charge
  TList* hListAmpCharge2DRC; //2D amplitude/charge plots

  TTree* Reco;

};

