#include "DigitizerChannelReco.hh"
#include "TObject.h"
#include "TVector3.h"
#include "TSpectrum.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TTree.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

static int waveformhit=0;
int waveformhitmax=10;
char name[256];

//running parameters:
//int fNewPed = 0; //0 = old pedestal calculation, 1 = new pedestal calculation
//int fTimeCut=1; //0 = no time cut, 1= time cut

void DigitizerChannelReco::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}


// virtual DigitizerChannelReco::~DigitizerChannelReco(){;}


void DigitizerChannelReco::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){

  fGlobalMode = gMode;
  std::string name;
  name = cfg->GetParOrDefault("DETECTOR","NAME","DIGI");
  name += "DIGI";
  H1 = new TH1D(name.c_str(),name.c_str(),1000,0.,1000.);
  name = cfg->GetParOrDefault("DETECTOR","NAME","DIGI");
  name += "2";
  H2 = new TH1D(name.c_str(),name.c_str(),990,0.,990.);
  //hListCal    = new TList();  // needs to be simplified
  //hPedCalo = new TH1D*[100];

  /*for(int kk=0;kk<100;kk++){
    hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),600,3700.,3900);
    hListCal->Add(hPedCalo[kk]);
  }*/
  
  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);//reads in variable "TimeBin" under heading "ADC" in file cfg (passed to method when called) and sets it to local variable named fTimeBin
  fVoltageBin     = cfg->GetParOrDefault("ADC","VoltageBin",0.000244);
  fImpedance      = cfg->GetParOrDefault("ADC","InputImpedance",50.);

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);
  
  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);  

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);

  fProcessing     = cfg->GetParOrDefault("RECO","Processing",0);
  //  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0); //if MultiHit=1, Peaks finding with TSpectrum
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);

  fNewPed  = cfg->GetParOrDefault("RECO","NewPed",1);
  fTimeCut  = cfg->GetParOrDefault("RECO","TimeCut",0);
  

 fUsePulseProcessing  = cfg->GetParOrDefault("RECO","UsePulseProcessing",1);
 fPeakSearchWidth     = cfg->GetParOrDefault("RECO","PeakSearchWidth",6);
 fZeroSuppression     = cfg->GetParOrDefault("RECO","ZeroSuppression",5.);
 fChargeCut           = cfg->GetParOrDefault("RECO","ChargeCut",0.01);
 fDPParameterR1       = cfg->GetParOrDefault("RECO","fDPParameterR1",650.);
 fDPParameterR2       = cfg->GetParOrDefault("RECO","fDPParameterR2",100.);
 fDPParameterC        = cfg->GetParOrDefault("RECO","fDPParameterC",0.030e-9);




  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
  PrepareTmpHistos();  //Temp histo servono anche in non debug mode
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    std::cout<<"preparing debug histos"<<std::endl;
    PrepareDebugHistos(); //debugging mode histos
  }
  //if (fMultihit==0)
  if (fProcessing==0)
    {
      if (fabs(fAmpThresholdLow-fAmpThresholdHigh)<0.001)
	{
	  std::cout<<cfg->GetName()<<" Digitizer config. ERROR: inconsistent confuguration .... fAmpThresholdLow and fAmpThresholdHigh cannot be ~equal if singleHit reco is selected"<<std::endl;
	}
    }
}

void DigitizerChannelReco::PrepareTmpHistos(){ //Copied from DigitizerChannelEco.cc Beth 7/6/2019
  hListTmp    = new TList();
  if(!fUseOverSample){
    hListTmp->Add(hdxdtTmp   = new TH1F("hdxdt","hdxdt",1000,0.,1000.));
    hListTmp->Add(hSignalTmp = new TH1F("hSignal","hSignal",1000,0.,1000.));
  }
  // over sampled histograms
  Int_t nbinx=4000;
  if(fUseOverSample){
    hListTmp->Add(hSigOv       = new TH1F("hSigOv","hSigOv",nbinx,0.,nbinx/4));
    hListTmp->Add(hSigOvSm     = new TH1F("hSigOvSm","hSigOvSm",nbinx,0.,nbinx/4));
    hListTmp->Add(hdxdtSigOvSm = new TH1F("hdxdtSigOvSm","hdxdtSigOvSm",nbinx,0.,nbinx/4));
  }


}

void DigitizerChannelReco::PrepareDebugHistos(){
  std::cout<<"I'm in Debug fNewPed = "<<fNewPed<<" fTimeCut = "<<fTimeCut<<std::endl;
  
  // char* outputfilename="RecoAn";
  // outputfilename+="SingleHit";
  // if(fNewPed == 1 && fTimeCut == 1) outputfilename += "TimeCutNewPed.root";
  // if(fNewPed == 1 && fTimeCut == 0) outputfilename += "NoTimeCutNewPed.root";
  // if(fNewPed == 0 && fTimeCut == 1) outputfilename += "TimeCutOldPed.root";
  // if(fNewPed == 0 && fTimeCut == 0) outputfilename += "NoTimeCutOldPed.root";

  // if(waveformhit==0) std::cout<<outputfilename<<std::endl;
  
  fileOut    = new TFile("RecoAn.root", "RECREATE");

  fileOut->mkdir("Waveforms");
  fileOut->mkdir("AmpSpectra");
  fileOut->mkdir("PedSpectra");
  fileOut->mkdir("AmpChargeSpectra");
  fileOut->mkdir("AmpCharge2D");

  if(fProcessing==2){
    fileOut->cd("AmpSpectra");
    gDirectory->mkdir("MultiHit");
    gDirectory->mkdir("RCProcessed");

    fileOut->cd("AmpChargeSpectra");
    gDirectory->mkdir("MultiHit");
    gDirectory->mkdir("RCProcessed");

    fileOut->cd("AmpCharge2D");
    gDirectory->mkdir("MultiHit");
    gDirectory->mkdir("RCProcessed");
  }

  hListCal            = new TList(); // single board related histograms  
  hListWaveform       = new TList(); //Waveform histograms
  hListAmp            = new TList(); //Amplitude spectra
  hListPed            = new TList(); //Pedestal spectra
  hListAmpChargeRatio = new TList(); //Spectra of ratio of amplitude to charge
  hListAmpCharge2D    = new TList(); //2D amplitude/charge plots
  hListEv             = new TList(); // More general histograms 

  //these RC lists are used to store the RC plots. If fProcessing==2 then lists without RC will be multihit.
  if(fProcessing==2){
    hListCalRC            = new TList(); // single board related histograms  
    hListAmpRC            = new TList(); //Amplitude spectra
    hListAmpChargeRatioRC = new TList(); //Spectra of ratio of amplitude to charge
    hListAmpCharge2DRC    = new TList(); //2D amplitude/charge plots
    }

  Reco = new TTree("RECO","RECO");//Tree variables:
  Reco->Branch("ElCh",&ElCh);
  // Reco->Branch("Row",&Row);
  // Reco->Branch("Col",&Col);
  // Reco->Branch("Zsup",&Zsup);
  // Reco->Branch("Avg200",&fAvg200);
  // Reco->Branch("HitE",&HitE);
  // Reco->Branch("HitEHyb",&HitEHyb);
  // Reco->Branch("Hit200E",&HitE200);
  // Reco->Branch("HitT",&HitT);
  // Reco->Branch("Trig",&fTrig); // 0 reco 1 ped 2 cosmic
  // Reco->Branch("IsSat",&IsSat);
  // Reco->Branch("VMax",&VMax);

  // hPedCalo = new TH1D*[32];
  // hAvgCalo = new TH1D*[32];
  // hPedMean = new TH1D*[32];
  // hVMax    = new TH1D*[32];
  // h200QCh  = new TH1D*[32]; //CT
  // hQCh     = new TH1D*[32]; //CT

  hListTmp->Add(hSat    = new TH1F("hSat","hSat",1000,0.,1000.));
  //  hListTmp->Add(hDiff    = new TH1F("hDiff","hDiff",4000,0.,1000.));

  //Waveform analysis histograms
  for(int i =0;i<waveformhitmax;i++){
    sprintf(name, "hSignal%d", i);
    hListWaveform->Add(hSignal[i] = new TH1F(name,"Waveform",1024,0.,1024.)); //Signal waveform
    sprintf(name, "hSignalSmooth%d", i);
    hListWaveform->Add(hSignalSmooth[i] =  new TH1F(name,"Smoothed Waveform",1024,0,1024));//smoothed waveform
    sprintf(name, "hdxdt%d", i);
    hListWaveform->Add(hdxdt[i] = new TH1F(name,"Waveform derivative",1024,0.,1024.)); //Signal derivative
    sprintf(name, "hdxdtSmooth%d", i);
    hListWaveform->Add(hdxdtSmooth[i] = new TH1F(name,"Derivative of SignalSmooth",1024,0.,1024.)); //Derivative of smoothed signal
    sprintf(name, "hdxdtRatio%d", i);
    hListWaveform->Add(hDerivRatio[i] = new TH1F(name,"Ratio of Derivative to Signal", 1024,0.,1024.));
    sprintf(name, "hdxdtRatioSmooth%d", i);
    hListWaveform->Add(hDerivRatioSmooth[i] = new TH1F(name,"Ratio of Derivative to SignalSmooth", 1024,0.,1024.));
    sprintf(name, "hRCVoltage%d", i);
    hListWaveform->Add(hRCVoltage[i]= new TH1F(name,"hRCVoltage",1024,0.,1024.));
  }


  //More generic histograms to check reconstruction
  sprintf(name,"charge integrated between %d samples below max signal bin and %d samples above",fPreSamples,fPostSamples);
  hListCal->Add(hChargeInt = new TH1F(name,"hChargeInt",1000,0.,3500.));
  
  if(fProcessing==2){
    sprintf(name,"charge integrated for RC signals between %d samples below max signal bin and %d samples above",fPreSamples,fPostSamples);
    hListCalRC->Add(hChargeIntRC = new TH1F(name,"hChargeIntRC",1000,0.,3500.));
    }

  for (int i=0; i<96; i++) {  //Here we need one histogram for each channel
    sprintf(name, "HitAmplitudeChannel%d",i); //This histogram stores the the spectrum of signal amplitudes

    if(fProcessing==0)   hListAmp->Add(hAmpSpectrum[i] = new TH1F(name,"HitAmplitude",100,0.0,4000.0));
    else                 hListAmp->Add(hAmpSpectrum[i] = new TH1F(name,"HitAmplitude",100,0.0,600.0));
    
    if(fProcessing==2){
      sprintf(name, "RCHitAmplitudeChannel%d",i); //This histogram stores the the spectrum of signal amplitudes
      hListAmpRC->Add(hAmpSpectrumRC[i] = new TH1F(name,"RCHitAmplitude",100,0.0,600.0));
    }
    
    sprintf(name, "HitPedestalChannel%d",i); //This histogram stores the spectrum of hit pedestals
    hListPed->Add(hPedSpectrum[i] = new TH1F(name,"HitPedestal",100,1500.0,4000.0));

    sprintf(name, "AmplitudetoChargeRatioChannel%d",i);

    if(fProcessing==0&&fNewPed==0)        hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,20000.0));
    else if(fProcessing==0&&fNewPed==1)   hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,5000.0));
    else if(fProcessing!=0&&GetChID()<70) hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,2.0));
    else if(fProcessing!=0&&GetChID()<96) hListAmpChargeRatio->Add(hAmpChargeRatio[i] = new TH1F(name,"AmplitudetoChargeRatio",100,0.0,1.0));
    if(fProcessing==2){
      sprintf(name, "RCAmplitudetoChargeRatioChannel%d",i);
      if (GetChID()<90)      hListAmpChargeRatioRC->Add(hAmpChargeRatioRC[i] = new TH1F(name,"RCAmplitudetoChargeRatio",100,0.0,25.0));
      else                   hListAmpChargeRatioRC->Add(hAmpChargeRatioRC[i] = new TH1F(name,"RCAmplitudetoChargeRatio",100,0.0,25.0));
    }

    sprintf(name, "AmplitudeVsChargeChannel%d",i);
    if(fProcessing ==0)              hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,4000.,1000.,0.,4000.));
    else if(fProcessing!=0&&i<82)    hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,500.,1000.,0.,1500.));
    else if(fProcessing!=0&&i<96)    hListAmpCharge2D->Add(hAmpCharge2D[i] = new TH2F(name,"AmplitudeVsCharge",500,0.,1000.,1000.,0.,4000.));
    if(fProcessing==2){
      sprintf(name, "RCAmplitudeVsChargeChannel%d",i);
      hListAmpCharge2DRC->Add(hAmpCharge2DRC[i] = new TH2F(name,"RCAmplitudeVsCharge",500,0.,4000.,700.,0.,3500.));
    }
  }


  hListWaveform->Add(hTrialSignal = new TH1F("hTrialSignal","hTrialSignal",1000,0.,1200.));
  hListWaveform->Add(hRCProcessedTrialSignal = new TH1F("hRCProcessedTrialSignal","hRCProcessedTrialSignal",1000,0.,1200.));
  hListWaveform->Add(hSigFinal = new TH1F("hSigFinal","hSigFinal",1000,0.,1000.));
  hListWaveform->Add(hSigMax = new TH1F("hSigMax","hSigMax",1000,0.,1000.));
  hListWaveform->Add(hdxdtFinal = new TH1F("hdxdtFinal","hdxdtFinal",1000,0.,1000.));
  hListWaveform->Add(hdxdtMaxTime= new TH1F("hdxdtMaxTime","hdxdtMaxTime",400,0.,400.));//Time of max derivative

  // hListCal->Add(hTime= new TH1F("hTime","hTime",1000,0.,1000.));//Time of maximum signal
  // hListCal->Add(hTimeCut= new TH1F("hTimeCut","hTimeCut",1000,0.,1000.));
  // hListCal->Add(hTimeOv= new TH1F("hTimeOv","hTimeOv",1000,0.,1000.hc));
  // hListCal->Add(hdxdtRMS= new TH1F("hdxdtRMS","hdxdtRMS",1000,0.,200.));
  // hListCal->Add(hTIntCorr= new TH1F("hTIntCorr","hTIntCorr",500,0.,1.));
  
  // for(int kk=0;kk<32;kk++){
  //   hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),1200,3300.,3900);
  //   hAvgCalo[kk] = new TH1D(Form("hAvgCalo%d",kk),Form("hAvgCalo%d",kk),1200,3300.,3900);
  //   hPedMean[kk] = new TH1D(Form("hSig%d",kk),Form("hSig%d",kk),1000,0.,1000.);
  //   hVMax[kk]    = new TH1D(Form("hVMax%d",kk),Form("hVMax%d",kk),1000,0.,1000.); // in mV
  //   h200QCh[kk]  = new TH1D(Form("h200QCh%d",kk),Form("h200QCh%d",kk),600,-200,400); //CT
  //   hQCh[kk]     = new TH1D(Form("hQCh%d",kk),Form("hQCh%d",kk),600,-200,400); 
    //CT
    // hListCal->Add(hPedCalo[kk]);
    // hListCal->Add(hAvgCalo[kk]);
    // hListCal->Add(hPedMean[kk]);
    // hListCal->Add(hVMax[kk]);
    // hListCal->Add(h200QCh[kk]); //CT
    // hListCal->Add(hQCh[kk]); //CT
  //}

}

void DigitizerChannelReco::SaveDebugHistos(){
  fileOut->cd();
  Reco->Write();
  hListCal->Write(); //use it in monitor mode only  

  fileOut->cd("Waveforms");
  hListWaveform->Write();

  fileOut->cd("AmpSpectra");

  if(fProcessing!=2)  hListAmp->Write();

  if(fProcessing==2){
    gDirectory->cd("MultiHit");
    hListAmp->Write();
    gDirectory->cd("../RCProcessed");
    hListAmpRC->Write();
  }

  fileOut->cd("PedSpectra");
  hListPed->Write();

  fileOut->cd("AmpChargeSpectra");
  if(fProcessing!=2)  hListAmpChargeRatio->Write();

  if(fProcessing==2){
    gDirectory->cd("MultiHit");
    hListAmpChargeRatio->Write();
    gDirectory->cd("../RCProcessed");
    hListAmpChargeRatioRC->Write();
  }

  fileOut->cd("AmpCharge2D");
  if(fProcessing!=2)  hListAmpCharge2D->Write();
  
  if(fProcessing==2){
    gDirectory->cd("MultiHit");
    hListAmpCharge2D->Write();
    gDirectory->cd("../RCProcessed");
    hListAmpCharge2DRC->Write();
  }

  // fileOut->Write();
  //  hListCal->ls();
  fileOut->Close();
}

void DigitizerChannelReco::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}
 
Short_t DigitizerChannelReco::CalcMaximum() {

  fMax = 32767; // 2^15 - 1
  
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) {
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}

Double_t DigitizerChannelReco::CalcPedestal() {
  //"old" pedestal calculation
  if(fNewPed==0){
    fPed = 0.;
    fNPedSamples = 0;
    //Call this only after the maximum is set
    if(fIMax - fPedOffset < 0) {
      //What to do if not enough samples for pedestal calculation... 
      //Use the first few samples
      fPed = fSamples[0];
      fNPedSamples = 1;
    } 
    
    for(Short_t i = fIMax - fPedOffset; 
	i >= 0 &&  fNPedSamples <  fPedMaxNSamples; 
	--i,++fNPedSamples) {
      //    std::cout << i << "  " << fNPedSamples << std::endl;
      fPed+= fSamples[i];
    }
       
    fPed /= fNPedSamples;
  }
 
  //"new" pedestal calculation - the pedestal is calculated using the first 200 samples
  if(fNewPed==1){
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
  }
    return fPed;
}

Double_t DigitizerChannelReco::ZSupHit(Float_t Thr, UShort_t NAvg) {
  Double_t rms1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;
  //  if(rms1000>Thr){
  //  if(rms1000>4.){
  // come back to a veto setup 
  if(rms1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;
  }
  //std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  return ZSupHit;
}

Double_t DigitizerChannelReco::CalcCharge(UShort_t iMax) {
  
  Short_t begin = iMax-fPreSamples > 0? iMax-fPreSamples:0;
  Short_t end = iMax+fPostSamples < fNSamples? iMax+fPostSamples:fNSamples;
 
  // M. Raggi for testing
  //begin = 180;
  //end =  1000;
  // come back to a veto setup

  //  std::cout << "Begin: "<< begin << "  end: " << end << std::endl;
  fCharge=0.;
  for(Short_t i = begin;i<end;++i) {
    fCharge+=1.* fSamples[i];
  }
  
  // std::cout << "Integral: " << fCharge << "  Ped:  " << fPed << " Base: " << (end-begin) * fPed << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;

  fCharge = ((1.*end-1.*begin) * fPed) - fCharge;  //subtract events pedestral
  fCharge *= (fVoltageBin*fTimeBin/fImpedance);
  return fCharge;
}

Double_t DigitizerChannelReco::CalcTime(UShort_t iMax) {
  double Time = 0.;
  //currently looking only at the signal rising edge
  
  float t1=0.;
  float t2=0.;

  float t3=0.;
  float t4=0.;

  
  float val1=0.;
  float val2=0.;

  int t1_ok=0;
  int t2_ok=0;

  int t3_ok=0;
  int t4_ok=0;

  float max = ( fPed - fMax);

  Short_t begin =  - fPreSamples > 0? fIMax - fPreSamples:0;
  for(Short_t i = begin ;i < fIMax;i++) {
    val1 = fPed - fSamples[i];
    val2 = fPed - fSamples[i+1];
    
    // if( t1_ok == 0 && val1 < TH_RATIO_1*max && val2 > TH_RATIO_1*max) {
    //   t1 = i +  (TH_RATIO_1*max - val1)/(val2 - val1)  ;
    //   t1_ok = 1;
    // }
    // if( t1_ok = 1 && t2_ok == 0 && val1 < TH_RATIO_2*max && val2 > TH_RATIO_2*max) {
    //   t2 = i+ (TH_RATIO_2*max - val1)/(val2 - val1)   ;
    //   t2_ok = 1;
    // }

    if( t3_ok == 0 && val1 <= fAmpThresholdLow && val2 >fAmpThresholdLow) {
      t3 = 1.*i + (fAmpThresholdLow - val1)/(val2 - val1);
      t3_ok = 1;
    }
    if( t3_ok == 1 && t4_ok == 0 && val1 <= fAmpThresholdHigh && val2 > fAmpThresholdHigh) {
      t4 = 1.*i + (fAmpThresholdHigh - val1)/(val2 - val1);
      t4_ok = 1;
    }
  }
  
  //  fTime = (t1 + t2)/2.;
  if (fabs(fAmpThresholdHigh - fAmpThresholdLow)<0.001)
    {
      std::cout<<" Digitizer config. ERROR: inconsistent confuguration .... fAmpThresholdLow and fAmpThresholdHigh cannot be ~equal if singleHit reco is selected"<<std::endl; 
      Time = t3*fTimeBin;
      return Time;
    }
  Time = t3 - (t4-t3)*fAmpThresholdLow/(fAmpThresholdHigh - fAmpThresholdLow);
  Time = Time*fTimeBin;
  return Time;
}


void DigitizerChannelReco::DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin) {

  // Double_t R1=1300;//ohms
  // Double_t R2=100.; //ohms
  // Double_t C=0.015e-9; //nF

  Double_t R1=fDPParameterR1;//ohms
  Double_t R2=fDPParameterR2; //ohms
  Double_t C=fDPParameterC; //nF

  
  //Calculating the output pulse:
  Double_t integr=0;
  
  static Double_t ic[1024];

  Double_t bin_width=timebin;

  integr=0;
  ic[0]= uin[0]/R2;

  for(int i=1;i<NPOINTS;i++) {
    integr+=ic[i-1]*bin_width;
    ic[i]= uin[i]/R2 - ((R1+R2)/(C*R1*R2))* integr;
    uout[i] = uin[i] - integr/(C);
  }  
}

Double_t DigitizerChannelReco::CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t iMax, UShort_t ped) {
  Int_t npeaks = 10;
  static TSpectrum SpectrumProcessor(20);// = new TSpectrum(20);
  double Time   = 0.;
  fCharge = 0.;
  //currently looking for peaks with TSpectrum to obtain multi hit times
  //Int_t npeaks =25;

  Double_t ProcessedAmp[1024];

  static Double_t AbsSamRec[1024];
  static Double_t AbsSamRecDP[1024];


  for(UShort_t s=0;s<iMax;s++){
    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+ped)/4096.*1000.; //in mV positivo
    //std::cout<< s << "     "  << fSamples[s]  <<" V "<< AbsSamRec[s]  <<std::endl;
  }

  if(fProcessing==1)       H1->SetContent(AbsSamRec);
  else if(fProcessing==2){
    RCVoltage(AbsSamRec, ProcessedAmp, fNSamples);
    H1->SetContent(ProcessedAmp);
  }
  


  for(UShort_t s=iMax;s<1024;s++){
    AbsSamRec[s] = 0;
  }

  // H1->Reset();

  if (fUsePulseProcessing) {
    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,iMax-1 ,fTimeBin*1e-9);
    H1->SetContent(AbsSamRecDP);
  } else {
    H1->SetContent(AbsSamRec);
  }

  // for(UShort_t s = 0;s<256;s++) {
  //   AbsSamRecRebin[s]=0;
  //   for(int i = 0;i<4;i++) {
  //     AbsSamRecRebin[s]+=AbsSamRecDP[4*s + i];
  //   }
  // }

  
  //  H1->Rebin(4);


  Double_t VMax = H1->GetMaximum();
  Double_t VMin = H1->GetMinimum();
  //   std::cout<<"Get Maximum     "<<VMax<<"   Get Minimum    "<<VMin<<std::endl;
  
  //if (VMax>fAmpThresholdHigh) std::cout<<VMax<<" VMax "<<std::endl;

  //  fMinAmplitude
  if(VMax>fAmpThresholdHigh){
    TSpectrum *s = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks
    Double_t peak_thr  = fAmpThresholdLow /VMax; //minimum peak height allowed. 
    Int_t nfound = s->Search(H1,fPeakSearchWidth,"",peak_thr);     

    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      fCharge = 0.;
      Double_t xp = (s->GetPositionX())[ll];
      Double_t yp = (s->GetPositionY())[ll];
      fAmpli1=yp;
      Time=xp*fTimeBin;
      Int_t bin    = H1->GetXaxis()->FindBin(xp);
    
      //fEnergy = fCharge/pCMeV;	  
      TRecoVHit *Hit = new TRecoVHit();  
      Hit->SetTime(Time);
      //      Hit->SetEnergy(fCharge);
      //Hit->SetEnergy(fCharge);     
      Hit->SetEnergy(fAmpli1);//amplitude in digitizer units

      hitArray.push_back(Hit);

    }
    H1->Reset();
   }

  Time = Time*fTimeBin;
  return Time;

}

void DigitizerChannelReco::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  //  Double_t IsZeroSup = ZSupHit(5.,1000.);
  CalcCharge(fIMax);
  //  std::cout<<"GetChID() before cut = "<< GetChID()<<std::endl;
  //if (fCharge < .01) return;
  // come back to a Veto setup

  //if (fCharge < 2.) return; //changed by Beth 27/6/19 see note below
  if (fCharge < fChargeCut) return;
  
  //this is in nC, so it's = 10pC - big cut. Should be closer to 25 photo electrons*electron charge*CPM gain, so about 1pC or 0.001nC. Therefore a cut of 5pC Should also go within config file 
  double time = CalcTime(fIMax);
  
  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(time);
  // M. Raggi going to charge
  // Double_t fEnergy= fCharge*1000./15; //going from nC to MeV using 15pC/MeV
  // Hit->SetEnergy(fCharge);
  // come back to a Veto setup
  Hit->SetEnergy(fCharge);

  //std::cout<<"GetChID() after cut = "<< GetChID()<<std::endl;
  //HistoFill();

  hitArray.push_back(Hit);

  // std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelReco::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){  //using TSpectrum
  
  Double_t ped=CalcPedestal();
  //std::cout<<"Pedestal    "<<ped<<std::endl;
  //ped=3650;
  CalcChaTime(hitArray,1000,ped);
}


void DigitizerChannelReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){

  if(fZeroSuppression > 1 ) {
    if (ZSupHit(fZeroSuppression,1000.) < 0) return; 
  }

  /*
  UShort_t nsmooth = 20; //real  = 2*nsmooth + 1
  Short_t samples[1024];
  
  for(UShort_t i = nsmooth; i<fNSamples-nsmooth;i++){
    int sum = 0;
    for(UShort_t j = i - nsmooth; j<= i + nsmooth; j++) {
      sum+= fSamples[j];
    }    
    samples[i] = sum /(2*nsmooth + 1);
  }


  int ndiff = 9;
  
  for(UShort_t i = 0;i<=ndiff;i++) {
    fSamples[i] = 0;
    fSamples[fNSamples-1-i]=0; 
  }
	
  for(UShort_t i = ndiff; i<fNSamples-ndiff;i++){
    fSamples[i] =  samples[i] - samples[i-ndiff];
  }


  */
  
  if(fUseAbsSignals) {
    SetAbsSignals();
  }

  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){ //only do histofill in debug mode (therefore only create histograms in debug mode)
    CalcMaximum();
    CalcPedestal();
    if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?
    //std::cout<<"ped = "<<ped<<std::endl;
    HistoFill();
  }

  if(fProcessing!=0) {                //if not in single hit mode, run multihit analysis
    ReconstructMultiHit(hitArray);
  } 
  else {                              //if in single hit mode run single hit analysis
    CalcMaximum();
    //std::cout<<" fIMax =  "<<fIMax<<std::endl;
    CalcPedestal();
    if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?
    ReconstructSingleHit(hitArray);
  }
}
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;


void DigitizerChannelReco::HistoFill(){  

  static  Double_t Signal[1024];
  static  Double_t SmoothedSignal[1024];
  static Double_t SignalDerivative[1024];
  static Double_t SignalDerivativeSmoothed[1024];
  static Double_t DerivativetoSignalRatio[1024];
  static Double_t DerivativetoSignalRatioSmoothed[1024];

  const Int_t npeaks =50;//maximum number of peaks to be found
  static Int_t nfound1=-1;
  static Int_t nfound2=0;
  
  static Double_t MultiSignal[npeaks];//amplitude of peaks found with TSpectrum
  static Double_t RCSignal[npeaks];//amplitude of peaks of RC processed signal found with TSpectrum

  static Double_t MultiCharge[npeaks];//amplitude of peaks found with TSpectrum
  static Double_t RCCharge[npeaks];//amplitude of peaks of RC processed signal found with TSpectrum

  static TSpectrum sp(20);
  TSpectrum *s = &sp; 
    //= new TSpectrum(2*npeaks);  //npeaks max number of peaks

  Int_t    dxdtMaxbin; //bin containing maximum dxdt
  Double_t sigmax; //maximum signal
  Int_t    sigmaxbin;  //bin containing maximum signal
  Double_t ChargeInt =0; //Integral of charge from fPreSamples below max signal to fPostSamples after
  Double_t ChargeIntRC =0; //Integral of charge from fPreSamples below max signal to fPostSamples after

  int derivbins = 10;
  int nsmoothing =1;//Number of samples either side used to smooth the signal

  //  std::cout<<"fPed = "<<fPed<<std::endl;

    for(Int_t SignalSample = 0;SignalSample<fNSamples;SignalSample++){
      Signal[SignalSample]=fPed-(1.*fSamples[SignalSample]);
      hSigFinal->SetBinContent(SignalSample,Signal[SignalSample]);//stores last hit in last event, used to fill sigmax histo
      hSigMax->Fill(hSigFinal->GetMaximum()/4096*1e3);//in mV
    } 
    
    for(int SmoothSample = 0; SmoothSample < fNSamples; ++SmoothSample){
      double SignalTot=0;
      for(int k = -1*nsmoothing;k<nsmoothing+1;k++){
	if(SmoothSample <nsmoothing||SmoothSample>(fNSamples-nsmoothing)) continue;
	SignalTot += Signal[SmoothSample+k];
      }
      SmoothedSignal[SmoothSample]=SignalTot/(2.*nsmoothing+1.);
    }

    if (fProcessing!=0){ //Beth 11/7/19 - include TSpectrum processing in Histofill so that you have direct access to variables here, then you can plot things directly, meaning that you can directly compare pre- and post-RC processing 
      double Time1   = 0.; //1 refers to multihi
      double Time2   = 0.; //2 refers to RC processing
      Double_t pCMeV= 3.2E5*2*1.67E-7; 
      Int_t NIntSamp= fSignalWidth/fTimeBin; 
      
      Double_t ProcessedAmp[1024];
      
      H1->SetContent(Signal);
      RCVoltage(Signal, ProcessedAmp, fNSamples);
      H2->SetContent(ProcessedAmp);
      
      Double_t VMax1 = H1->GetMaximum();
      Double_t VMin1 = H1->GetMinimum();
      Double_t VMax2 = H2->GetMaximum();
      Double_t VMin2 = H2->GetMinimum();
      
      //if (VMax1>fAmpThresholdHigh) std::cout<<VMax1<<" VMax1 "<<std::endl;
      //if (VMax2>fAmpThresholdHigh) std::cout<<VMax2<<" VMax2 "<<std::endl;
      
      if(VMax1>fAmpThresholdHigh){
	Double_t peak_thr1  = fAmpThresholdLow/VMax1; //minimum peak height allowed. 

	nfound1 = s->Search(H1,10,"",peak_thr1);     //10 = sigma
      }
	
      if(fProcessing==2&&(VMax2>fAmpThresholdHigh)){
	Double_t peak_thr2  = fAmpThresholdLow/VMax2; //minimum peak height allowed. 
	nfound2 = s->Search(H2,10,"",peak_thr2);     //10 = sigma
      }
      
      for(Int_t ll=0;ll<nfound1;ll++){ //peak loop per channel
	fCharge = 0.;
	//Float_t xp   = xpeaks[ll];
	Double_t xp = (s->GetPositionX())[ll];
	//Float_t yp   = ypeaks[ll];
	Double_t yp = (s->GetPositionY())[ll];
	fAmpli1=yp;
	MultiSignal[ll]=fAmpli1;
	MultiCharge[ll]=-1;
	
	Time1=xp*fTimeBin;
	Int_t bin    = H1->GetXaxis()->FindBin(xp);
	//	  if(bin<1000){
	// for (Int_t ii=bin-NIntSamp;ii<bin+NIntSamp;ii++) {
	if((bin-fPreSamples>0)&&(bin+fPostSamples<fNSamples)){
	  int donebefore =0;
	  int doneafter =0;
	  for (Int_t ii=bin-fPreSamples;ii<bin+fPostSamples;ii++) {
	    if (H1->GetBinContent(ii)<0.005&&donebefore==0&&ii-bin<0){
	      //std::cout<<"Multi hit: samples before bin = "<<ii-bin<<" H1 Bin content = "<<H1->GetBinContent(ii)<<std::endl;
	      donebefore =1;
	    }
	    if (H1->GetBinContent(ii)<0.005&&doneafter==0&&ii-bin>0){
	      //std::cout<<"Multi hit: samples after bin = "<<ii-bin<<" H1 Bin content = "<<H1->GetBinContent(ii)<<std::endl;
	      doneafter =1;
	    }
	    //	      if(H1->GetBinContent(ii)>0.005){ 
	    MultiCharge[ll] += H1->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;//charge in pC as opposed to for single hit where it's in nC
	  }
	}
      }
    

      //fEnergy = fCharge/pCMeV;	  
      if(fProcessing==2){
	for(Int_t ll=0;ll<nfound2;ll++){ //peak loop per channel
	  fCharge = 0.;
	  //Float_t xp   = xpeaks[ll];
	  Double_t xp = (s->GetPositionX())[ll];
	  //Float_t yp   = ypeaks[ll];
	  Double_t yp = (s->GetPositionY())[ll];
	  fAmpli2=yp;
	  RCSignal[ll]=fAmpli2;
	  RCCharge[ll]=-1;
	  
	  Time2=xp*fTimeBin;
	  Int_t bin    = H2->GetXaxis()->FindBin(xp);
	  //	    if(bin<1000){
	  //	      for (Int_t ii=bin-NIntSamp;ii<bin+NIntSamp;ii++) {
	  if((bin-fPreSamples>0)&&(bin+fPostSamples<fNSamples)){
	    int donebefore =0;
	    int doneafter =0;	     
	    for (Int_t ii=bin-30;ii<bin+30;ii++) {
	      if (H2->GetBinContent(ii)<0.005&&donebefore==0&&ii-bin<0){
		//		std::cout<<"RC hit: samples before bin = "<<ii-bin<<" H2 Bin content = "<<H2->GetBinContent(ii)<<std::endl;
		donebefore =1;
	      }
	      if (H2->GetBinContent(ii)<0.005&&doneafter==0&&ii-bin>0){
		//		std::cout<<"RC hit: samples after bin = "<<ii-bin<<" H2 Bin content = "<<H2->GetBinContent(ii)<<std::endl;
		doneafter =1;
	      }
	      //	      if(H2->GetBinContent(ii)>0.005){ 
	      RCCharge[ll] += H2->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;//charge in pC as opposed to for single hit where it's in nC
	    }
	  }
	  //fEnergy = fCharge/pCMeV;	  
	}
      }
    }
    //    }
    //}
    


    //Beth 17/6/19 to try to optimise R1, R2, C, we create a sample input signal with form exp(-t/20.5), where 20.5 has been found by using root to fit the exponential tale of a signal

    Double_t TrialSamp[1024];

    for(int itri=0;itri<1024;itri++){
      if(itri>200){
	TrialSamp[itri]=100*exp(-(itri-200)*fTimeBin/20.5);
      }
      else{TrialSamp[itri]=0;}
    }

    Double_t TrialOut[fNSamples];
    Double_t SignalOut[fNSamples];
    
    RCVoltage(TrialSamp, TrialOut, fNSamples);//reconstruct what would happen if signal were passed through an RC circuit
    RCVoltage(Signal, SignalOut, fNSamples);
      
    for(int iSample = 0; iSample < fNSamples; ++iSample){
      //std::cout<<"iSample = "<<iSample<<std::endl;
      SignalDerivative[iSample] = 0;
      if(iSample>=derivbins&&iSample<fNSamples){ //otherwise the derivative doesn't make sense - you can't calculate the derivative of the first bin because you have nothing before it to compare it to
	//	SignalDerivative[iSample] = (Signal[iSample+derivbins]-Signal[iSample-derivbins])/((2*derivbins)*fTimeBin); //fTimeBin = time between samples.
	SignalDerivative[iSample] = (Signal[iSample]-Signal[iSample-derivbins])/((derivbins)*fTimeBin); //Calculating the derivative only backwards gives the highest possible value for the derivative as it isn't confused by the peak
	hdxdtFinal->SetBinContent(iSample, SignalDerivative[iSample]);
      }
      SignalDerivativeSmoothed[iSample] =0;
      if(iSample>=derivbins&&iSample<fNSamples){
	//	  SignalDerivativeSmoothed[iSample] = (SmoothedSignal[iSample+derivbins]-SmoothedSignal[iSample-derivbins])/((2*derivbins)*fTimeBin);
	SignalDerivativeSmoothed[iSample] = (SmoothedSignal[iSample]-SmoothedSignal[iSample-derivbins])/((derivbins)*fTimeBin);
      }
      DerivativetoSignalRatio[iSample] = SignalDerivative[iSample]/Signal[iSample];
      DerivativetoSignalRatioSmoothed[iSample] = SignalDerivativeSmoothed[iSample]/SmoothedSignal[iSample];
    }
    
    
    dxdtMaxbin = hdxdtFinal->TH1::GetMaximumBin();//Returns bin number of bin of maximum content - that is the location of the histogram peak
    hdxdtMaxTime->Fill(dxdtMaxbin*fTimeBin);//Converts bin number to time
    
    sigmaxbin = hSigFinal->TH1::GetMaximumBin();
  
    Int_t begin = sigmaxbin-10>0? sigmaxbin-10:0;//if(sigmaxbin-10)>0, begin=sigmaxbun-10, else begin = 0
    Int_t end = sigmaxbin+150>fNSamples? sigmaxbin+150:fNSamples;

    for(int i=begin;i<end;i++){
      if(fProcessing==0)    ChargeInt+=Signal[i]/(4096*fImpedance)*fTimeBin*1e-9/1E-12; //in pC
      else{
	ChargeInt+=fAmpli1/(4096*fImpedance)*fTimeBin*1e-9/1E-12; //in pC
	ChargeIntRC+=fAmpli2/(4096*fImpedance)*fTimeBin*1e-9/1E-12; //in pC
      }
    }
    hChargeInt->Fill(ChargeInt);
    
    //  if(fMultihit==1&&fNewPed==1&&(TMath::MaxElement(fNSamples,Signal)/fCharge)>25) std::cout<<"GetChID() "<<GetChID()<<" TMath::MaxElement(fNSamples,Signal)/fCharge = "<<TMath::MaxElement(fNSamples,Signal)/fCharge<<std::endl;
    if(fProcessing==0){
      if(fTimeCut==1){ //If we're using the time cut...
	if(CalcTime(fIMax)>10.) { //use the time cut
	  hPedSpectrum[GetChID()]->Fill(fPed);
	  hAmpSpectrum[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal));
	  hAmpChargeRatio[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal)/fCharge);
	  hAmpCharge2D[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal),fCharge);
	}
      }

      else if (fTimeCut==0){ //Otherwise, if we're not using the time cut, fill the histograms anyway
	hPedSpectrum[GetChID()]->Fill(fPed);
	hAmpSpectrum[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal));
	hAmpChargeRatio[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal)/fCharge);
	//      if(((GetChID()==60||GetChID()==71)&&fCharge>500)||(GetChID()==80&&fCharge>1500)||(GetChID()==87&&fCharge>3500))      std::cout<<"GetChID() "<<GetChID()<<" amp = "<<TMath::MaxElement(fNSamples,Signal)<<" charge = "<<fCharge<<std::endl;
	hAmpCharge2D[GetChID()]->Fill(TMath::MaxElement(fNSamples,Signal),fCharge);
      }
    }

    else{
      hPedSpectrum[GetChID()]->Fill(fPed);
      //      std::cout<<"nfound1 = "<<nfound1<<std::endl;
      for(int i = 0;i<nfound1;i++){
	hAmpSpectrum[GetChID()]->Fill(MultiSignal[i]);
	if(MultiCharge[i]>0)	hAmpChargeRatio[GetChID()]->Fill(MultiSignal[i]/MultiCharge[i]);

	//	if(GetChID()<90&&MultiSignal[i]/MultiCharge[i]>25) std::cout<<"GetChID() "<<GetChID()<<" MultiSignal[i] = "<< MultiSignal[i]<<" MultiCharge[i] = "<<MultiCharge[i]<<"  MultiSignal[i]/MultiCharge[i] = "<<MultiSignal[i]/MultiCharge[i]<<std::endl;

	if(MultiCharge[i]>0)    hAmpCharge2D[GetChID()]->Fill(MultiSignal[i],MultiCharge[i]);
      }

      if(fProcessing==2){
	for(int i = 0;i<nfound2;i++){
	  hAmpSpectrumRC[GetChID()]->Fill(RCSignal[i]);
	  
	  if(RCCharge[i]>0)  hAmpChargeRatioRC[GetChID()]->Fill(RCSignal[i]/RCCharge[i]);
	  if(RCCharge[i]>0)  hAmpCharge2DRC[GetChID()]->Fill(RCSignal[i],RCCharge[i]);
	}
      }

    }

  if(waveformhit<waveformhitmax){
    //    std::cout<<"waveformhit = " << waveformhit << " GetChID() = " <<GetChID()<<std::endl;
    for(int i=0;i<fNSamples;i++){
      hSignal[waveformhit]->SetBinContent(i,Signal[i]);
      hSignalSmooth[waveformhit]->SetBinContent(i,SmoothedSignal[i]);
      hdxdt[waveformhit]->SetBinContent(i, SignalDerivative[i]);
      hdxdtSmooth[waveformhit]->SetBinContent(i, SignalDerivativeSmoothed[i]);
      hDerivRatio[waveformhit]->SetBinContent(i,DerivativetoSignalRatio[i]);
      hDerivRatioSmooth[waveformhit]->SetBinContent(i,DerivativetoSignalRatioSmoothed[i]);
      hRCVoltage[waveformhit]->SetBinContent(i+1, SignalOut[i]);
      hTrialSignal->SetBinContent(i,TrialSamp[i]);
      hRCProcessedTrialSignal->SetBinContent(i,TrialOut[i]);
    }
  }

  if(waveformhit==0) std::cout<<"parameters are: fNewPed = "<<fNewPed<<" fTimeCut = "<<fTimeCut<<std::endl;
  waveformhit++;//after every hit increase the value of the hit counting variable

}

void DigitizerChannelReco::RCVoltage(Double_t* uin, Double_t* uout, Int_t sampleno){    //Calculating voltage output by an RC circuit 13/6/19
    
    // Double_t R1=150.;//ohms
    // Double_t R2=50.; //ohms
    // Double_t C=.1e-9; //nF
    
    Double_t R1=1300.;//resistor in parallel (ohms) - original guess 150 Ohms
    Double_t R2=100.; //resistor in series with capacitor - over which voltage is measured (ohms) - original guess 150 Ohms
    Double_t C=0.015e-9; //nF capacitor in series with R2 - original guess 0.13e-9
    //Calculating the output pulse:
    Double_t integr=0;//charge integral
    
    Double_t ic[sampleno];//curent through capacitor
    
    Double_t bin_width=fTimeBin*1e-9;
    //Double_t bin_width=0.2e-9; //0.2 ns
    // std::cout << bin_width << std::endl;
    
    for(int i=0;i<sampleno;i++) {
      //      uin[i]=Signal[i];
      integr=0;
      for(int j=0;j<i;j++) {
	integr+=ic[j]*bin_width;
	//      cout << integr;
      }
      ic[i]= uin[i]/R2 - ((R1+R2)/(C*R1*R2))* integr;
      
      //    cout << integr;
      uout[i] = uin[i] - integr/(C);
    }
}


DigitizerChannelReco::~DigitizerChannelReco(){
  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode()){
    SaveDebugHistos();
  }
}; //Beth 7/6/19 - destructor, without this it will crash when the compiling process tries to finish //content copied from DigitizerChannelECal.cc 10/6/19

