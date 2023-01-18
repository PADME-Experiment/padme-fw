#include "ChannelVReco.hh"
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

class DigitizerChannelETag : public ChannelVReco {
public:

  DigitizerChannelETag(){;};
  virtual ~DigitizerChannelETag();

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitVec);
  virtual void Init(PadmeVRecoConfig *cfg){return ;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);

  void PrintConfig();

  Double_t CalcPedestal();
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitVec);

  Double_t SetETagChaGain();
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
  //Int_t fNFoundPerChannel[96]={0};  //Beth 23/2/22: for some reason GetChID() is sometimes 91???
  std::map<int, int> fNFoundPerChannel;


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
  TList* hListETag; // single board related histograms 

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
  TH2F * hOccupancy2D;
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

  // TODO how to retrieve this automatically from channelIDmap???
  std::vector<int> fListOfChannels = {
     0, 1, 2, 3,
     100, 101, 102, 103,
     200, 201, 202, 203,
     300, 301, 302, 303,
     400, 401, 402, 403,
     500, 501, 502, 503,
     600, 601, 602, 603,
     700, 701, 702, 703,
     800, 801, 802, 803,
     900, 901, 902, 903,
     1000, 1001, 1002, 1003,
     1100, 1101, 1102, 1103,
     1200, 1201, 1202, 1203,
     1300, 1301, 1302, 1303,
     1400, 1401, 1402, 1403,
     10, 11, 12, 13,
     110, 111, 112, 113,
     210, 211, 212, 213,
     310, 311, 312, 313,
     410, 411, 412, 413,
     510, 511, 512, 513,
     610, 611, 612, 613,
     710, 711, 712, 713,
     810, 811, 812, 813,
     910, 911, 912, 913,
     1010, 1011, 1012, 1013,
     1110, 1111, 1112, 1113,
     1210, 1211, 1212, 1213,
     1310, 1311, 1312, 1313,
     1410, 1411, 1412, 1413
  };
  std::map<int, std::pair<int,int>> PosMap = {
	{0, {0, 0}},
	{1, {0, 1}},
	{2, {0, 2}},
	{3, {0, 3}},
	{100, {0, 4}},
	{101, {0, 5}},
	{102, {0, 6}},
	{103, {0, 7}},
	{200, {0, 8}},
	{201, {0, 9}},
	{202, {0, 10}},
	{203, {0, 11}},
	{300, {0, 12}},
	{301, {0, 13}},
	{302, {0, 14}},
	{303, {0, 15}},
	{400, {0, 16}},
	{401, {0, 17}},
	{402, {0, 18}},
	{403, {0, 19}},
	{500, {0, 20}},
	{501, {0, 21}},
	{502, {0, 22}},
	{503, {0, 23}},
	{600, {0, 24}},
	{601, {0, 25}},
	{602, {0, 26}},
	{603, {0, 27}},
	{700, {0, 28}},
	{701, {0, 29}},
	{702, {0, 30}},
	{703, {0, 31}},
	{800, {0, 32}},
	{801, {0, 33}},
	{802, {0, 34}},
	{803, {0, 35}},
	{900, {0, 36}},
	{901, {0, 37}},
	{902, {0, 38}},
	{903, {0, 39}},
	{1000, {0, 40}},
	{1001, {0, 41}},
	{1002, {0, 42}},
	{1003, {0, 43}},
	{1100, {0, 44}},
	{1101, {0, 45}},
	{1102, {0, 46}},
	{1103, {0, 47}},
	{1200, {0, 48}},
	{1201, {0, 49}},
	{1202, {0, 50}},
	{1203, {0, 51}},
	{1300, {0, 52}},
	{1301, {0, 53}},
	{1302, {0, 54}},
	{1303, {0, 55}},
	{1400, {0, 56}},
	{1401, {0, 57}},
	{1402, {0, 58}},
	{1403, {0, 59}},
	{10, {1, 0}},
	{11, {1, 1}},
	{12, {1, 2}},
	{13, {1, 3}},
	{110, {1, 4}},
	{111, {1, 5}},
	{112, {1, 6}},
	{113, {1, 7}},
	{210, {1, 8}},
	{211, {1, 9}},
	{212, {1, 10}},
	{213, {1, 11}},
	{310, {1, 12}},
	{311, {1, 13}},
	{312, {1, 14}},
	{313, {1, 15}},
	{410, {1, 16}},
	{411, {1, 17}},
	{412, {1, 18}},
	{413, {1, 19}},
	{510, {1, 20}},
	{511, {1, 21}},
	{512, {1, 22}},
	{513, {1, 23}},
	{610, {1, 24}},
	{611, {1, 25}},
	{612, {1, 26}},
	{613, {1, 27}},
	{710, {1, 28}},
	{711, {1, 29}},
	{712, {1, 30}},
	{713, {1, 31}},
	{810, {1, 32}},
	{811, {1, 33}},
	{812, {1, 34}},
	{813, {1, 35}},
	{910, {1, 36}},
	{911, {1, 37}},
	{912, {1, 38}},
	{913, {1, 39}},
	{1010, {1, 40}},
	{1011, {1, 41}},
	{1012, {1, 42}},
	{1013, {1, 43}},
	{1110, {1, 44}},
	{1111, {1, 45}},
	{1112, {1, 46}},
	{1113, {1, 47}},
	{1210, {1, 48}},
	{1211, {1, 49}},
	{1212, {1, 50}},
	{1213, {1, 51}},
	{1310, {1, 52}},
	{1311, {1, 53}},
	{1312, {1, 54}},
	{1313, {1, 55}},
	{1410, {1, 56}},
	{1411, {1, 57}},
	{1412, {1, 58}},
	{1413, {1, 59}}
  };

  std::map<int, TH1F *>  hRawVPerChannel;
  std::map<int, TH1F *>  hRawVCorrectPerChannel;
  std::map<int, TH1F *>  hRawVOneHitPerChannel;
  std::map<int, TH1F *>  hNoHitsDerivPerChannel;
  std::map<int, TH1F *>  hDerivVPerChannel;
  std::map<int, TH1F *>  hDerivVOneHitPerChannel;
  std::map<int, TH1F *>  hDerivVCorrectPerChannel;
  std::map<int, TH1F *>  hNoiseRMSPerChannel;

  TH2F  * hAmpDiffVsUncorrectAmp;
  TH2F  * hAmpDiffVsUncorrectAmpChannels20to70;
  TH2F  * hAmpDiffVsUncorrectAmpNotFirstHit;
  TH2F  * hCorrectedAmpVsUncorrectAmp;
  TH2F  * hCorrectedAmpVsUncorrectAmpChannels20to70;
  TH2F  * hCorrectedAmpVsUncorrectAmpNotFirstHit;
  TH2F  * hYMaxRawYTSpecRatioVsYMax;

  TGraph* gUnAbsSigs;
  std::vector<TGraph*> gUnAbsSigGraphs;

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

