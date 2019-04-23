#ifndef DIGITIZER_CHANNEL_RECO_H
#define DIGITIZER_CHANNEL_RECO_H

#include "ChannelVReco.hh"
#include "utlConfigParser.hh"
#include "TObject.h"
#include "TVector3.h"

class GlobalRecoConfigOptions;
typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;


  /*
 //vertical Strips
 X2  X3  X4  X5
 X6   X7  X8  X9
 X10 X11 X12 X13
 X14 X15 X16 X17

 //horizontal Strips
 Y2   Y3  Y4  Y5
 Y6   Y7  Y8  Y9
 Y10 Y11 Y12 Y13
 Y14 Y15 Y16 Y17

 Strip X1, X18, X19, Y1, Y18, Y19 biased but not connected to front-end channels 
                    x    y
                  X17\   |Y17 
                   ...\  |...
                     X2\ |Y2
        beam z -------->\|
                
*/
//0-15 board upstream; 16-31 downstream upstream
//ADC board 28, Ch0-31

class DigitizerChannelTarget : public ChannelVReco {
public:
  DigitizerChannelTarget();
  ~DigitizerChannelTarget(){;};

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg){return;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);

  void ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray);
  void ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray);
  void PrintConfig();

  Short_t CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcTime(UShort_t);

  Short_t GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};

  void SetAbsSignals();
  
  Double_t CalcPosition(UShort_t);

private:
  //What do we operate
  UShort_t fNSamples;
  Short_t *fSamples;
  Short_t fMax;
  Short_t fIMax;
  Double_t fPed;
  //Double_t fCharge;
  //Double_t fTime;
  UShort_t fNPedSamples;

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

  Double_t fAmpThresholdLow;
  Double_t fAmpThresholdHigh;

  Bool_t fMultihit;
  Bool_t fUseAbsSignals;

  // Global and local running options
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;


  //Target variables

 
  Double_t fCh      [32];
  Double_t fStrip   [32];//fStrips mapping
  Double_t fMeasure [32];//fStrips position in mm
  Double_t fAxis    [32];//fStrips geometry
  Double_t fPedCh   [32];//Adc channel pedestals
  Double_t fQCh     [32];//Channel charge
  Double_t fCalibCh [32];//Calibration Constant fStrips
  Double_t fCCD         ;  //CCD
  Double_t fQXtot       ;
  Double_t fQYtot       ;
  Double_t fQtot        ;
  Double_t feQXtot      ;
  Double_t feQYtot      ;
  Double_t feQtot       ;
  Double_t fXbeam       ;
  Double_t feXbeam      ;
  Double_t fYbeam       ;
  Double_t feYbeam      ;
  Double_t fAverageGain	;		 
  Double_t fHVsign    	;
  Double_t fDefaultRuler; 
  Double_t fRuler       ; 		 

};

#endif
