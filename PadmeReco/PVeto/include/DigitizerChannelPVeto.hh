#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TList.h"

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class DigitizerChannelPVeto : public ChannelVReco {
public:

  DigitizerChannelPVeto(){;};
  virtual ~DigitizerChannelPVeto();

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg){return ;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);

  //  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  //  void ReconstructMultiHit (std::vector<TRecoVHit *> &hitArray);
  void PrintConfig();

  //  Short_t CalcMaximum();
  Double_t CalcPedestal();
  //  Double_t CalcCharge(UShort_t);
  //  Double_t CalcTime(UShort_t);
  Double_t CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t,UShort_t);
  Double_t CalcChaTimeOrig(std::vector<TRecoVHit *> &hitArray,UShort_t,UShort_t);
  Double_t ZSupHit(Float_t thr,UShort_t NAvg);  //M. Raggi 30/10/2018
  void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin);

  //  void SetAbsSignals();

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
  TList* hListPVeto; // single board related histograms 

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

  UShort_t fNSamples;
  Short_t *fSamples;
  Short_t fMax;
  Short_t fIMax;
  Double_t fPed;
  Double_t fCharge;
  Double_t fTime;
  UShort_t fNPedSamples;

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  
  Int_t fMinAmplitude;

  TH1D *H1;
  TH1D *hSig;
  TH1D *htmp;
  TH1D *hdxdt;

  TH1F *hNhitOrig; 
  
  TH1F *hSigV;
  TH1F *hSigE;

  TH1F *hDxV;
  TH1F *hDxE;

  Double_t fSignalThreshold;
  Double_t fSignalPercentage;
  
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;
  Double_t fAmpli;
  Double_t fEnergy;
  Double_t fmVtoMeV;

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fMultihit;
  Bool_t fUseAbsSignals;

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
  
  Short_t  GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};


};

