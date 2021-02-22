#include "ChannelVReco.hh"
#include "GlobalRecoConfigOptions.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TFile.h"
#include "TList.h"

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

  void SetAbsSignals();
  
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

  //Configuration variables
  Int_t fSignalWidth;
  Int_t fPreSamples;
  Int_t fPostSamples;
  Int_t fPedOffset; 
  Int_t fPedMaxNSamples;
  
  Int_t fMinAmplitude;

  TH1D *H1;
  

  Double_t fSignalThreshold;
  Double_t fSignalPercentage;
  
  Double_t fTimeBin;
  Double_t fVoltageBin;
  Double_t fImpedance;
  Double_t fAmpli;
  Double_t fmVtoMeV;
  Double_t fEnergy;

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


};

