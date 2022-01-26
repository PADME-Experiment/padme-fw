#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TH2F.h"
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

  virtual void PrepareDebugHistos();
  virtual void PrepareTmpHistos();
  virtual void SaveDebugHistos();

  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  void ReconstructMultiHit (std::vector<TRecoVHit *> &hitArray);
  void PrintConfig();

  Short_t CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcTime(UShort_t);
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t,UShort_t,Int_t,Int_t&);
  Double_t SetEVetoChaGain();
  Double_t SetPVetoChaGain();
  Double_t ZeroCrossingFinder(UShort_t x1, UShort_t x2, double h1, double h2);
  Double_t ZSupHit(Float_t thr,UShort_t NAvg);  //M. Raggi 30/10/2018

  void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin);
  void HistoFill();//Beth 30/7/19 fills debugging histograms created in PrepareDebugHistos

  void SetAbsSignals();

  //  Double_t AvgSignals(Int_t sample);//average waveform, normalised to height 1

  Double_t TailHeight(Int_t DeltaT);//DeltaT in samples

  Int_t GetChID(){return fChID;};
  void  SetChID(Int_t ChID){fChID=ChID;};
  
  Int_t GetElChID(){return fElChID;};
  void  SetElChID(Int_t ChID){fElChID=ChID;};

  Int_t GetBdID(){return fBdID;};
  void  SetBdID(Int_t BdID){fBdID=BdID;};

  Int_t GetTrigMask(){return fTrigMask;};
  void  SetTrigMask(Int_t TrigMask){fTrigMask=TrigMask;};

  Int_t GetEventNumber(){return fRawEvNo;};
  void  SetEventNumber(Int_t RawEvNo){
    fRawEvNo=RawEvNo;
    EventCounter++;
  };
  
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
  Double_t fRMS1000;

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  
  Int_t fMinAmplitude;

  Double_t fSignalThreshold;
  Double_t fSignalPercentage;
  
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;
  Double_t fAmpli;
  Double_t fAmpli1;
  Double_t fAmpli2;
  Double_t fmVtoMeV;
  Double_t fEnergy;

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fMultihit;
  Bool_t fUseAbsSignals;
  Bool_t fUseOverSample;

  Bool_t fNewPed;
  Bool_t fTimeCut;

  //mode variables
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;

  Int_t fProcessing ;
  Int_t fPeakSearchWidth    ;
  Double_t fZeroSuppression    ;
  Double_t fChargeCut          ;
  Double_t fDPParameterR1      ;
  Double_t fDPParameterR2      ;
  Double_t fDPParameterC       ;
  
  Short_t  GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};

  Int_t fRawEvNo;
  Int_t EventCounter=-1;//is increased before each event is processed, so the first event will have EventCounter=0
  // Debug histogram structure

  TFile * fileOut;

  std::vector<TH1F*> hSignal;
  std::vector<TH1F*> hRaw;//Raw+TSpectrum
  std::vector<TH1F*> hDeriv;//Deriv+TSpectrum
  std::vector<TH1F*> hDerivNoZero;//Deriv+TSpectrum
  std::vector<Bool_t> RawPrintFlag;//For each signal saved in hRaw assign a flag to decide whether or not to save it
  std::vector<Bool_t> DerivPrintFlag;
  std::vector<Bool_t> DerivNoZeroPrintFlag;

  TH1D *  HTSpec = new TH1D("tspec","tspec",1000,0.,1000.);
  TH1F *  HNoZeroDeriv = new TH1F("nzd","nzd",1000,0.,1000.);
  TH1F *  hEvent;
  TH1F *  hNoHitsRaw;//no hits reconstructed by tspectrum on raw signal
  TH1F *  hNoHitsDeriv;//no hits reconstructed by tspectrum on derivative
  TH1F *  hDeltaNoHits;
  TH1F *  hDeltaTime;//difference in time between raw and derivative reconstructions
  TH1F *  hDeltaTimeOneHit;//
  TH1F *  hDeltaTimeTwoHit;//
  TH1F *  hZeroCrossTimeDiff;//difference in time between derivative peak and zerocrossing
  TH1F *  hZeroCrossTimeDiffOneHit;//difference in time between derivative peak and zerocrossing
  TH1F *  hRawV;//
  TH1F *  hRawVCorrect;//
  TH1F *  hRawVOneHit;//
  TH1F *  hRawVMultiHit;//
  TH1F *  hRawVMultiHitCorrect;//
  TH1F *  hDerivV;//
  TH1F *  hOccupancyRaw;
  TH1F *  hOccupancyDeriv;
  TH1F *  hOccupancyRawPerEvent;
  TH1F *  hOccupancyDerivPerEvent;
  TH1F *  hOccupancyRatio;
  TH1F *  hMinTimeDiffRaw;//minimum time between hits in the same channel
  TH1F *  hMinTimeDiffDeriv;
  TH1F *  hVRatio;
  TH1F *  hNZSupEvents;
  TH1F *  hNoiseRMSAvg;

  TH2F *  hRawVvsDerivZeroCrossTimeDiff;//
  TH2F *  hDerivVvsDeltaTime;//

  TH1F *  hDeltaNoHitsPerChannel[96];
  TH1F *  hDeltaTimePerChannel[96];
  TH1F *  hRawVPerChannel[96];//
  TH1F *  hRawVOneHitPerChannel[96];//
  TH1F *  hRawVCorrectPerChannel[96];//
  TH1F *  hDerivVPerChannel[96];//
  TH1F *  hNoHitsRawPerChannel[96];//
  TH1F *  hNoHitsDerivPerChannel[96];//
  TH1F *  hMinTimeDiffRawPerChannel[96];
  TH1F *  hMinTimeDiffDerivPerChannel[96];
  TH1F *  hNoiseRMSPerChannel[96];

  TH2F *  hRawVvsDeltaTimePerChannel[96];//
  TH2F *  hDerivVvsDeltaTimePerChannel[96];//

  Int_t nfoundraw;
  Int_t nfoundderiv;

  std::vector<Int_t> nRawHitVec;//no hits found in each channel each event from raw signal
  std::vector<Int_t> nDerivHitVec;//no hits found in each channel each event from deriv 

  std::vector<Double_t> tRawHitVec;//time of hits found in ns
  std::vector<Double_t> tDerivHitVec;
  std::vector<Double_t> tRawZeroCrossVec;//time of zero crossing of raw signal
  std::vector<Double_t> tDerivZeroCrossVec;//time of zero crossing of derivative

  std::vector<Double_t> tRawSortHitVec;
  std::vector<Double_t> tDerivSortHitVec;
  std::vector<Double_t> tRawSortZeroCrossVec;
  std::vector<Double_t> tDerivSortZeroCrossVec;

  std::vector<Double_t> tRawDiffHitVec;
  std::vector<Double_t> tDerivDiffHitVec;

  std::vector<Double_t> vRawHitVec;
  std::vector<Double_t> vDerivHitVec;
  std::vector<Double_t> vRawSortHitVec;
  std::vector<Double_t> vDerivSortHitVec;
  std::vector<Double_t> vRawSortTailCorrectHitVec;
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

  TList* hListWaveformDeriv;

  TTree* Reco;

};

