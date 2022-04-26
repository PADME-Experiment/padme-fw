#include "ChannelVReco.hh"
#include "PVetoCalibration.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TList.h"
#include "TGraph.h"

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class DigitizerChannelPVeto : public ChannelVReco {
public:

  DigitizerChannelPVeto(){;};
  virtual ~DigitizerChannelPVeto();

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitVec);
  virtual void Init(PadmeVRecoConfig *cfg){return ;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);//, PVetoCalibration *calib);

  void PrintConfig();

  Double_t CalcPedestal();
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitVec);

  Double_t SetPVetoChaGain();
  Double_t ZSupHit(Float_t thr,UShort_t NAvg);  //M. Raggi 30/10/2018

  void SetAbsSignals(Double_t ped);
  void AnalogPlotting(); //Beth 23/2/22: method to plot analog signals
  void HitPlots(std::vector<TRecoVHit *> &hitVec);
  Double_t TailHeight(Int_t DeltaT);
  Double_t TailHeightDerivative(Int_t DeltaT);

  // Debugging the reco
  virtual void PrepareDebugHistos();
  virtual void SaveDebugHistos();

  //get ChID From Beth Digi

  Int_t EventCounter=-1;//is increased before each event is processed, so the first event will have EventCounter=0
  Int_t fRawEvNo;
  Int_t fChID;
  Int_t fElChID;
  Int_t fBdID;
  Int_t fTrigMask;
  Double_t fRMS1000;
  Int_t fNFoundPerChannel[96]={0};  //Beth 23/2/22: for some reason GetChID() is sometimes 91???


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
  TFile * fileOut;
  Bool_t fSaveAnalog;
  Int_t fTotalAnalogs;
  Int_t fAnalogsPrinted;

  PVetoCalibration *fChannelCalibration;
  int fCalibrationFile;

  TList* hListPVeto; // single board related histograms 

  TString detectorname;

  std::vector<Double_t>   tDerivHitVec          ;
  std::vector<Double_t>   tDerivSortHitVec      ;
  std::vector<Double_t>   tDerivDiffHitVec      ;

  std::vector<Double_t>   vRawHitVec	        ;
  std::vector<Double_t>   vRawSortHitVec        ;
  std::vector<Double_t>   vRawCorrectHitVec     ;

  std::vector<Double_t>   vTSpecYPHitVec        ;
  std::vector<Double_t>   vTSpecYPSortHitVec    ;
  std::vector<Double_t>   vTSpecYPCorrectHitVec ;

  Double_t RawGetMax      ;
  Double_t RawRise        ;//Value of raw signal at point where TSpectrum finds peak. This will always be on the rising edge of the signal
  Double_t DerivGetMax    ;

  Bool_t fAnalogPrint;
  //Beth 18/2/22: My histograms
  
  std::vector<TH1F*> hRaw;//Raw+TSpectrum
  std::vector<TH1F*> hDeriv;//Deriv+TSpectrum

  TH1F * hNoEventsReconstructed    ;
  TH1F * hNoHitsDeriv              ;
  TH1F * hRawV                     ;
  TH1F * hRawVCorrect              ;
  TH1F * hRawVCorrectChannels20to70;
  TH1F * hRawVOneHit               ;
  TH1F * hRawVMultiHit             ;
  TH1F * hRawVMultiHitCorrect      ;
  TH1F * hDerivV                   ;
  TH1F * hDerivVOneHit             ;
  TH1F * hDerivVCorrect            ;
  TH1F * hOccupancy                ;
  TH1F * hOccupancyOneHit          ;
  TH1F * hMinTimeDiffDeriv         ;
  TH1F * hVRatio                   ;
  TH1F * hNZSupEvents              ;
  TH1F * hNoiseRMSAvg              ;
  TH1F * hYTSpecYMaxDiff           ;
  TH1F * hYMaxRawYTSpecRatio       ;
  TH1F * hYRiseYTSpecRatio         ;
  TH1F * hYMaxDerivYTSpecRatio     ;
  TH1F * hHitEnergy                ;
  TH1F * hHitEnergySingleHit       ;
  TH1F * hHitTime                  ;

  TH1F *  hRawVPerChannel[96];//
  TH1F *  hRawVCorrectPerChannel[96];//
  TH1F *  hRawVOneHitPerChannel[96];//
  TH1F *  hNoHitsDerivPerChannel[96];
  TH1F *  hDerivVPerChannel[96];
  TH1F *  hDerivVOneHitPerChannel[96];
  TH1F *  hDerivVCorrectPerChannel[96];//
  TH1F *  hNoiseRMSPerChannel[96];

  TH2F  * hAmpDiffVsUncorrectAmp;
  TH2F  * hAmpDiffVsUncorrectAmpChannels20to70;
  TH2F  * hAmpDiffVsUncorrectAmpNotFirstHit;
  TH2F  * hCorrectedAmpVsUncorrectAmp;
  TH2F  * hCorrectedAmpVsUncorrectAmpChannels20to70;
  TH2F  * hCorrectedAmpVsUncorrectAmpNotFirstHit;
  TH2F  * hYMaxRawYTSpecRatioVsYMax;

  TGraph* gUnAbsSigs;
  std::vector<TGraph*> gUnAbsSigGraphs;

  Double_t fCalibCh   [96]; 
  //  std::map<int,double> *fEnergyCalibMap;

   //Single Hit
  TH1F * hdxdtMax;      
  TH1F * hVMax;         
  TH2F * hVmaxvsDxdtMax;
  TH2F * hVMOvDxdtvsNHits;
  TH1F * hVmaxOvDxdt;
  TH1F * hEnergy;

  TH1F * hOrdxdtMax;      
  TH1F * hOrVMax;         
  TH2F * hOrVmaxvsDxdtMax;
  TH1F * hOrVmaxOvDxdt;
  TH1F * hOrEnergy;

  TH1F * hVMaxOrig;         

  //Multi Hit
  TH1F * hMHdxdtMax;      
  TH1F * hMHVMax;         
  TH2F * hMHVmaxvsDxdtMax;
  TH1F * hMHVmaxOvDxdt;
  TH1F * hMHEnergy;
  TH1F * hNhitDx;
  TH1F * hNhitSig;
  TH1F * hDeltaT;
  TH1F * hDeltaV;
  TH1F * hDeltaN;
  TH2F * hDeltaNvsN;
  TH1F * hTHits; 

  char name[256];

  UShort_t fNSamples;
  Short_t *fSamples;
  Double_t fEqualSamples[1024];//Beth 22/2/22: container for equalised voltage in mV

  Short_t fIMax=985; //last sample used in the analog signal, due to digitizer noise 
  Double_t fPed;
  Double_t fCharge;
  Double_t fTime;
  UShort_t fNPedSamples;

  //Configuration variables
  Int_t fPedMaxNSamples;
  Int_t fDerivPoints;

  TH1D *  HTSpec = new TH1D("tspec","tspec",1000,0.,1000.);
  TH1D *hSig;
  
  //Mauro histograms
  TH1D *  hdxdt   = new TH1D("hdxdt","hdxdt",fIMax,0.,fIMax);
  TH1D *  htmp    = new TH1D("htmp","htmp",fIMax,0.,fIMax);

  TH1F *hNhitOrig; 
  
  TH1F *hSigV;
  TH1F *hSigE;

  TH1F *hDxV;
  TH1F *hDxE;
  
  Double_t fTimeBin;
  Double_t fAmpli;
  Double_t fEnergy;
  Double_t fmVtoMeV;

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fChannelEqualisation; //Beth 23/2/22
  Bool_t fTailCorrection; //Beth 4/4/22

  //mode variables
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;

  Int_t fUsePulseProcessing ;
  Int_t fPeakSearchWidth    ;
  Double_t fZeroSuppression    ;
  Double_t fDerivAmpToEnergy;
  Double_t GetPedestal(){return fPed;};


};

