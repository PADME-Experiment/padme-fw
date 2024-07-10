
// Written By M. Raggi 6/12/2019
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
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include <tensorflow/core/protobuf/config.pb.h>
#include <tensorflow/c/c_api.h>

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class TF_SessionOptions;

class DigitizerChannelECalML : public ChannelVReco {
public:
  DigitizerChannelECalML(){;};
  ~DigitizerChannelECalML();

  virtual int SetConfig(TF_SessionOptions* opts, tensorflow::ConfigProto& config, TF_Status* status_);

  virtual void SetDigis(UShort_t n,Short_t* arr){fNSamples = n;fSamples = arr; };
  static void deallocator(void * data, size_t len, void * arg){};
  virtual void ApplyML(float* MLarr);
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray);
  virtual void Init(PadmeVRecoConfig *cfg){return;}
  virtual void Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg);
  
  void PrintConfig();
  void SetAbsSignals();
  Short_t CalcMaximum();
  Double_t CalcPedestal();

  
  //Int_t GetElChID(){return fElChID;};
  void  SetElChID(Int_t ChID){fElChID=ChID;};

  //Int_t GetBdID(){return fBdID;};
  void  SetBdID(Int_t BdID){fBdID=BdID;};
  
  
  //  void SetAnalogOffSets();
  
  
 
private:
  //What do we operate
  UShort_t fNSamples;
  Short_t *fSamples;
  static const int UPfactor=4;
  float floatSamples[1024];
  float* fSamplesML;
  Short_t fMax;
  Short_t fIMax;
  Double_t fPed;
  Double_t fCharge;
  // Double_t fChargeSin;
  Double_t fTime;
  Double_t fTimeSin;
  Double_t fTimeOv;
  UShort_t fNPedSamples;

  //  Int_t fChID;
  Int_t fElChID;
  Int_t fBdID;
  Int_t fBadInd; //MR 26/03/2020
  //  Int_t fTrigMask;
  Double_t fRMS1000;
  Double_t fRMS200;
  Double_t fAvg200;
  Double_t fTrig;
  Double_t fVMax;

  Int_t fNSat;
  Int_t fCountsLastSat;
  Int_t fFirstSat;
  Int_t fmyNSat;

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
  Bool_t fUseOverSample;
  Bool_t fIntCorrection;
  Bool_t fSaturatioCorrection;
  Bool_t fSaveAnalog;
  Double_t fZeroSuppression;

  Int_t fUsePulseProcessing ;
  Double_t fDPParameterR1      ;
  Double_t fDPParameterR2      ;
  Double_t fDPParameterC       ;

  //for MH io
  Double_t fEnThrToDefineHit;
  Double_t fEnThrToDefineHit_sat;
  Double_t fDTimeForHitInSameCh;
  Double_t fVThrToDefineHit;
  Double_t fVThrToDefineHit_sat;
  TString  fMHTemplateFile;

  Double_t fPedCh[32];//Adc channel pedestals
  std::map < std::pair<int,int>,double> fPedMap;
  //  std::map < std::pair<int,int>,double> fCalibMap;   //moved into ECalMLCalibration class M. Raggi 19/04/2019
  //  std::map < std::pair<int,int>,double> fT0Map;       //moved into ECalMLCalibration class M. Raggi 19/04/2019

  std::map < std::pair<int,int>,double> TempPedMap;
  std::map < std::pair<int,int>,double> TempMapCount;
  //mode variables
  GlobalRecoConfigOptions* fGlobalMode;
  LocalRecoConfigOptions*  fLocalMode;




  //  Int_t m;
  Double_t Zsup;
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


  Double_t fTemplate[5001];
  Bool_t fFirstHit;
  Bool_t fSaturatedHit;
  
  TH1D* histo4;
  TH1D* histo5;
  Int_t fCountEvent;
  Int_t fCountSavedEvent;
  Int_t fCountEventControl;

  Double_t fAmplitude;
  Double_t fDiffTimeWave; 
  Double_t fEnergySecondHit;

  TF_Status* status;
  TF_Session* session;
  TF_Graph*  graph;
  TF_SessionOptions* opts;
  
  int NumInputs;

  TF_Output* Input;

  int NumOutputs;

  TF_Output* Output;
  
  TF_Tensor** InputValues;
  TF_Tensor** OutputValues;
  
  int ndims;
  //int64_t dims[3];
  size_t ndata;

  Int_t MINDIST;
  float filteredSamples[1024*UPfactor];
  Int_t THRESHOLD;
  
};
#endif
