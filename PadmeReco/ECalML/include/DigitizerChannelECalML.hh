
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

typedef  GlobalRecoConfigOptions LocalRecoConfigOptions;

class DigitizerChannelECalML : public ChannelVReco {
public:
  DigitizerChannelECalML(){;};
  ~DigitizerChannelECalML();

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
  void PrintConfig();

  void ResetPedestal();

  Bool_t pedestalsFromFirstSamples() {return (fPedestalMode==0);}
  Bool_t pedestalsFromAutoTrgEvents(){return (fPedestalMode==1);}
  Bool_t hybridPedestals()        {return (fPedestalMode==2);}

  Double_t Fix2019BrokenChip(Int_t BadIndFlag);
  Int_t GetBadInd(){return fBadInd;};
  void  SetBadInd(Int_t BadInd){fBadInd=BadInd;};

  Short_t CalcMaximum();
  Double_t CalcPedestal();
  Double_t CalcCharge(UShort_t);
  Double_t CalcChargeSin(UShort_t); //single hit pedestal M. Raggi 22/01/2019
  Double_t CalcTime(UShort_t);
  Double_t CalcTimeSing(UShort_t);
  Double_t CalcTimeOver(UShort_t);
  Double_t PeakSearch();

  void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin);
  //Standard hits corrections
  Double_t ScaleToFullInt(UShort_t);    // Scale the integral of the signal independetly of the start time. 
  Bool_t   IsSaturated();   // Check if the signal is saturated
  Double_t CorrectSaturation(); // Correct saturated signals
  Double_t CorrectIntegrationTime(Double_t TStart,Double_t TStop); // Corrects for charge outside integration window

  Double_t ZSupHit(Float_t thr,UShort_t NAvg);  //M. Raggi 30/10/2018

  Short_t GetMaximum(){return fMax;};
  Double_t GetPedestal(){return fPed;};

  void SetAbsSignals();
  
  //  Int_t GetChID(){return fChID;};
  //  void  SetChID(Int_t ChID){fChID=ChID;};

  Int_t GetElChID(){return fElChID;};
  void  SetElChID(Int_t ChID){fElChID=ChID;};

  Int_t GetBdID(){return fBdID;};
  void  SetBdID(Int_t BdID){fBdID=BdID;};

  //  Int_t GetTrigMask(){return fTrigMask;};
  //  void  SetTrigMask(Int_t TrigMask){fTrigMask=TrigMask;};

  void SetAnalogOffSets();
  void SaveBDPed(Int_t BID);


  //  void OverSample4(Double_t *v, Double_t *o,Int_t n);
  void OverSample4(Short_t *v, Double_t *o,Int_t n);
  Int_t GetStartTime(Double_t *v,Int_t nshift);
  
  
  
  //for multihit io
  Double_t CalcChargeSin(UShort_t,std::vector<Double_t> input);
  void DrawMeanWave(UShort_t iDerr, Double_t& FirstEnergy, Double_t& FirstTime, Double_t& SeconEnergy,Double_t& SecondTime,Double_t& ThirdEnergy,Double_t& ThirdTime, Bool_t& SecondHit, Bool_t& ThirdHit);
  void MakeDifferenceWaveformTeplate(std::vector<Double_t> input,Int_t MaxBin, std::vector<Double_t>& tempWave, std::vector<Double_t>& output, Bool_t& OutRMS);
  Double_t MakeDerivativeAndTakeMaxTime(Int_t iDer, Int_t nsmooth, std::vector<Double_t> wave, Double_t& maxValDerivative);
  Int_t GetMaximumPosition(std::vector<Double_t> wave);
  
  void DrawMeanSaturatedWave(UShort_t iDerr, Double_t& FirstEnergy, Double_t& FirstTime, Double_t& SeconEnergy,Double_t& SecondTime ,Double_t& ThirdEnergy,Double_t& ThirdTime, Bool_t& SecondHit, Bool_t& ThirdHit);
  void MakeDifferenceSaturatedWaveformTeplate(std::vector<Double_t> input,Int_t MaxBin, std::vector<Double_t>& tempWave, std::vector<Double_t>& output, Bool_t& OutRMS, Double_t maxwave);
  void MakeDifferenceSaturatedWaveformTeplate(std::vector<Double_t> input,Int_t DeltaSat, Int_t MaxBin,std::vector<Double_t>& TempWave,std::vector<Double_t>& output, Bool_t& OutRMS, Double_t maxWave);
  void FindDoubleHitSaturated(std::vector<Double_t> Wave,Double_t maxWave, Bool_t& firstHitSaturated,Double_t& positionFirstSat,Int_t& myNSat,Bool_t& secondHitSaturated, Double_t& positionSecondHit,Int_t& myNSatSecond);
void FindDoubleHitSaturatedAngularCoefficient(std::vector<Double_t> Wave,Double_t maxWave, Bool_t& firstHitSaturated,Double_t& positionFirstSat,Int_t& myNSat,Bool_t& secondHitSaturated, Double_t& positionSecondHit,Int_t& myNSatSecond);

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

  TH1F * hdxdt;
  TH1F * hdxdtMax;
  TH1F * hdxdtRMS;
  TH1F * hTime;
  TH1F * hTIntCorr;
  TH1F * hTimeCut;
  TH1F * hTimeOv;
  TH1F * hSignal;
  TH1F * hSat;
  TH1F * hDiff;
  TH1F * hSignalShifted;
  TH1F * hSignalShifted2;
  TH1F * hSignalShifted3;
  TH2F * hDMaxDerivativeMaxWaveVSEnergy_First;
  TH2F * hDMaxDerivativeMaxWaveVSEnergy_Second;
  TH2F * hDMaxDerivativeMaxWaveVSEnergy_Third;
  TH1F * hDiffTimeFirstSecondHit;
  TH1F * hDiffTimeFirstThirdHit;
  TH1F * hDiffTimeSecondThirdHit;
  TH2F * hAmplitudeVSEnergyTemplate;

  TH1F * hSigOv;
  TH1F * hSigOvSm;
  TH1F * hdxdtSigOvSm;



  TH1D *  H1;
  TH1D *  H2;
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

  //Temporary histo pointers
  TH1D* histo;
  TH1D* histo1;
  TH1D* histo2;
  TH1D* histo3;
  TH1D* histoSat;

  TH1D* histoOv;
  TH1D* histoDiff;

  TList* hListCal; // single board related histograms 
  TList* hListEv;  // More general histograms 
  TList* hListTmp;  // More general histograms 
  TTree* ECalML;
  
  
  
  //multihit io
  TH1F *hAmplitude;
  TH1F *hDiffSignal;
  TH1F *hTemplate; 
  TH1F *hCharge;
  TH2F *hAmplitudeVSCharge;
  TH1F *hDiffWavetemplate;
  TH1F *hDiffFirstHitSeconHitTime;
  TH1F *hChargeFirstHit;
  TH1F *hEnergyFirstHit;
  TH1F *hChargeSecondHit;
  TH1F *hEnergySecondHit;
  TH1F *hCharge3Hit;
  TH1F *hEnergy3Hit;
  TH1F *hECALsecondHitE;
  TH1F *hDiffTimeHitWaveform;
  TH1F *hCoeffAngularSaturation;
  TH1F *hConstant;
  TH1F *hdiffSatMaxWave;
  TH1F *hdiffSat1Wave;
  TH1F *hdiffSat_firstLastFunction;
  TH1F *hdiffSatContiguosWave;
  TH1F *hsatDerivative;
  TH1F *hNsat;
  TH1F *hNsat_OnlyOneRecognise;
  TH1F *hNsat_TwoRecognise;
  TH2F *hTemplateVsMaxVSEnergy;
  TH2F *hTemplateVMaxVSEnergy_;
  TH2F *hDiffRMSVSEnergyTemplate;
  TH1F *hDiffWavetemplate_0_10;
 TH1F *hDiffWavetemplate_10_30;
 TH1F *hDiffWavetemplate_30_50;
 TH1F *hDiffWavetemplate_50_100;
 TH1F *hDiffWavetemplate_100_150;
 TH1F *hDiffWavetemplate_150_200;
 TH1F *hDiffWavetemplate_200_250;
 TH1F *hDiffWavetemplate_250_300;

  TH1F *hECALfirsthitEnergyCorrected;
  TH1F *hECALsecondhitEnergyCorrected;
  TH1F *hECALthirdhitEnergyCorrected;
  TH1F *hECALfirsthitEnergy;
  TH1F *hECALsecondhitEnergy_Saved;
  TH1F *hECALthirdhitEnergy_Saved;

  TH1F *hZSupFromSw;
  
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
  
};
#endif
