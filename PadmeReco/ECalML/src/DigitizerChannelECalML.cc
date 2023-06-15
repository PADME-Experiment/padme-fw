  // Written By M. Raggi 6/12/2018
#include "DigitizerChannelECalML.hh"
#include "TMath.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TRandom.h"
//#include <tensorflow/c/c_api.h>


#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

void DigitizerChannelECalML::PrintConfig(){
  std::cout <<"**************************** DigitizerChannelECalML config {*********************"<<std::endl;
  std::cout << "Hi I'm the ECalML: " << std::endl;
  std::cout << "Signal width: "   << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl; 
  std::cout << "fPedestalMode:   " << fPedestalMode <<std::endl;
  std::cout << "fUseOverSample: " << fUseOverSample << std::endl;  
  std::cout << "fMultiHit: " << fMultihit << std::endl;
  std::cout <<"**************************** DigitizerChannelECalML config }*********************"<<std::endl;

//  Int_t NBD=8;
//  for(int i=0;i<32;i++){
//    std::cout<<"Ped "<<i<<" "<<fPedMap[std::make_pair(NBD,i)]<<" "<<std::endl;
//  }  

 if(fGlobalMode->GetGlobalDebugMode()!=0){
   std::cout<<"General Config Flags ECalML Digi***************** "<<std::endl;
   std::cout<<"fIsPed          "<<fGlobalMode->IsPedestalMode()<< std::endl;
   std::cout<<"fIsReco         "<<fGlobalMode->IsRecoMode()<< std::endl;
   std::cout<<"fIsMonitor      "<<fGlobalMode->IsMonitorMode()<< std::endl;
   std::cout<<"fIsCosmic       "<<fGlobalMode->IsCosmicsMode()<< std::endl;
   std::cout<<"fIsDebug  ECAL  "<<fGlobalMode->GetGlobalDebugMode()<< std::endl;
  }
}

void DigitizerChannelECalML::Init(GlobalRecoConfigOptions *gOptions,
				PadmeVRecoConfig *cfg)
{
// Setting flags for running modes.
  fGlobalMode = gOptions;

  
  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
  fVoltageBin     = cfg->GetParOrDefault("ADC","VoltageBin",0.000244);
  fImpedance      = cfg->GetParOrDefault("ADC","InputImpedance",50.);

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);
  
  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);  
  fPedestalMode   = cfg->GetParOrDefault("RECO","PedestalMode",0);  

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);

  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0);
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);
  fUseOverSample  = cfg->GetParOrDefault("RECO","UseOverSample",0); //M. Raggi: 03/05/2019  
  fIntCorrection  = cfg->GetParOrDefault("RECO","UseIntegralCorrection",0); //M. Raggi: 15/05/2019  
  fSaturatioCorrection = cfg->GetParOrDefault("RECO","UseSaturationCorrection",0); //M. Raggi: 15/05/2019  
  fZeroSuppression= cfg->GetParOrDefault("RECO","UseZeroSuppression",5.); //M. Raggi: 07/06/2019  
  fSaveAnalog = cfg->GetParOrDefault("Output","Analog",0); //M. Raggi: 15/05/2019  

  fUsePulseProcessing  = cfg->GetParOrDefault("RECO","UsePulseProcessing",0);
  fDPParameterR1       = cfg->GetParOrDefault("RECO","fDPParameterR1",650.);
  fDPParameterR2       = cfg->GetParOrDefault("RECO","fDPParameterR2",100.);
  fDPParameterC        = cfg->GetParOrDefault("RECO","fDPParameterC",0.30e-9);

  //MH parameters
  fEnThrToDefineHit  = cfg->GetParOrDefault("RECO","EnThrToDefineTheExistenceOf2ndOr3dhHits",2.);
  fEnThrToDefineHit_sat  = cfg->GetParOrDefault("RECO","EnThrToDefineTheExistenceOf2ndOr3dhHits_saturatedWave",2.);
  fVThrToDefineHit  = cfg->GetParOrDefault("RECO","VoltageThrToDefineTheExistenceOf2ndOr3dhHits",5.);
  fVThrToDefineHit_sat  = cfg->GetParOrDefault("RECO","VoltageThrToDefineTheExistenceOf2ndOr3dhHits_saturatedWave",50.);
  fDTimeForHitInSameCh  = cfg->GetParOrDefault("RECO","DTimeForHitInSameCh",25.);
  fMHTemplateFile = cfg->GetParOrDefault("RECO","MHTemplateFile","config/BGOwaveformTemplate.txt");

  PrintConfig();


  status = TF_NewStatus();
  graph  = TF_NewGraph();
  TF_Buffer* r_opts = TF_NewBufferFromString("",0);
  TF_Buffer* meta_g = TF_NewBuffer();

  opts = TF_NewSessionOptions();
  const char* tags[] = {"serve"};

  session = TF_LoadSessionFromSavedModel(opts, r_opts, "config/model", tags, 1, graph, meta_g, status);

  if ( TF_GetCode(status) != TF_OK ) exit(-1);

  NumInputs = 1;
  Input = (TF_Output*) malloc(sizeof(TF_Output) * NumInputs);
  TF_Output t0 = {TF_GraphOperationByName(graph, "serving_default_input_1"), 0};
    
  if(t0.oper == NULL)
    printf("ERROR: Failed TF_GraphOperationByName serving_default_input_1\n");
  else
    printf("TF_GraphOperationByName serving_default_input_1 is OK\n");
    
  Input[0] = t0;


  NumOutputs = 1;
  Output = (TF_Output* ) malloc(sizeof(TF_Output) * NumOutputs);
  TF_Output t2 = {TF_GraphOperationByName(graph, "StatefulPartitionedCall"), 0};
    
  if(t2.oper == NULL)
      printf("ERROR: Failed TF_GraphOperationByName StatefulPartitionedCall\n");
  else
    printf("TF_GraphOperationByName StatefulPartitionedCall is OK\n");
    
  Output[0] = t2;

  InputValues  = (TF_Tensor**)malloc(sizeof(TF_Tensor*)*NumInputs);
  OutputValues = (TF_Tensor**)malloc(sizeof(TF_Tensor*)*NumOutputs);
    
  ndims = 3;
  ndata = 1024*sizeof(float);

}

void DigitizerChannelECalML::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}

Short_t DigitizerChannelECalML::CalcMaximum() {
  fMax = 32767; // 2^15 - 1
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) { 
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}

Double_t DigitizerChannelECalML::CalcPedestal() {  //the pedestal is calculated using the first 200 samples
  fPed = 0.;
  fNPedSamples = 0;
  fPedMaxNSamples=200;

  // average of first fPedMaxNSamples
  for(Short_t i = 0 ; i  <   fPedMaxNSamples; i++) {
       fNPedSamples ++;
       fPed+= fSamples[i];
  }
  //std::cout << " fNPedSamples " << fNPedSamples <<" fPed Veto"<< std::endl;
  fPed /= fNPedSamples;
  //std::cout <<  fPed << " fNPedSamples " << fNPedSamples << std::endl;
  return fPed;
}


void DigitizerChannelECalML::ApplyML(float* MLarr){
  int64_t dims[] = {1,1024,1};
  TF_Tensor* int_tensor = TF_NewTensor(TF_FLOAT, dims, ndims, MLarr, ndata, deallocator, NULL);
  if (int_tensor != NULL)
    //printf("TF_NewTensor is OK\n");
    ;
  else
    printf("ERROR: Failed TF_NewTensor\n");
    
  InputValues[0] = int_tensor;
  TF_SessionRun(session, NULL, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, NULL, 0,NULL , status);


  if(TF_GetCode(status) == TF_OK)
    //printf("Session is OK\n");
    ;
  else
    printf("%s",TF_Message(status));

  

  fSamplesML = (float*)TF_TensorData(OutputValues[0]);

  // free(InputValues);
  // free(OutputValues);
  // free(Input);
  // free(Output);

}

void DigitizerChannelECalML::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  fTrigMask = GetTrigMask();
  if(fUseAbsSignals) {
    SetAbsSignals();
  }
  Short_t maximum=CalcMaximum();

  Double_t pedestal=CalcPedestal();
  
  float floatPed=(float)fPed;
  
  //std::cout<<"floatPed: "<<floatPed<<std::endl;

  for(int i = 0; i < 1024; ++i) {
    floatSamples[i] = floatPed - (float)fSamples[i];
    //floatSamples[i] = (float)fSamples[i];
  }
  //std::cout<<"floatSamples[57]: "<<floatSamples[57]<<std::endl;
  
  ApplyML(floatSamples);
  //std::cout<<"fSamplesML[57]: "<<fSamplesML[57]<<std::endl;
  
  MINDIST=7;
  
  for(int i = 0;i<1024;i++) {
    filteredSamples[i] = 0;
  }

  for(int i = MINDIST/2; i < 1024 - MINDIST/2;i++) {
    int isMax = 1;
    for(int j = 1; j<= MINDIST/2; j++) {
      if(fSamplesML[i] < fSamplesML [i-j]) isMax = 0;
      if(fSamplesML[i] < fSamplesML[i+j]) isMax = 0;
    }
    
    if(isMax) {
      filteredSamples[i] = fSamplesML[i];
      for(int j = 1; j<=MINDIST/2; j++) {
	filteredSamples[i] += fSamplesML[i-j] + fSamplesML[i+j];
      }
    }
  }
  
  THRESHOLD = 10;
  
  for(int i=0;i<1024;i++) {
    if(filteredSamples[i] > THRESHOLD){
      TRecoVHit *Hit = new TRecoVHit();
      Hit->SetTime(i);
      Hit->SetEnergy(filteredSamples[i]);
      hitArray.push_back(Hit);
    }
  }
  
  
}

DigitizerChannelECalML::~DigitizerChannelECalML(){
  TF_DeleteGraph(graph);
  TF_DeleteSession(session, status);
  TF_DeleteSessionOptions(opts);
  TF_DeleteStatus(status);
}


