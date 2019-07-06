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

void DigitizerChannelReco::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}


void DigitizerChannelReco::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){

  fGlobalMode = gMode;

  H1 = new TH1D("h1","h1",1000,0.,1000.);
  //hListCal    = new TList();  // needs to be simplified
  //hPedCalo = new TH1D*[100];

  /*for(int kk=0;kk<100;kk++){
    hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),600,3700.,3900);
    hListCal->Add(hPedCalo[kk]);
  }*/
  
  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
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


  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0); //if MultiHit=1, Peaks finding with TSpectrum
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);


 fUsePulseProcessing  = cfg->GetParOrDefault("RECO","UsePulseProcessing",1);
 fPeakSearchWidth     = cfg->GetParOrDefault("RECO","PeakSearchWidth",6);
 fZeroSuppression     = cfg->GetParOrDefault("RECO","ZeroSuppression",5.);
 fChargeCut           = cfg->GetParOrDefault("RECO","ChargeCut",0.01);
 fDPParameterR1       = cfg->GetParOrDefault("RECO","fDPParameterR1",650.);
 fDPParameterR2       = cfg->GetParOrDefault("RECO","fDPParameterR2",100.);
 fDPParameterC        = cfg->GetParOrDefault("RECO","fDPParameterC",0.030e-9);




  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
  if (fMultihit==0)
    {
      if (fabs(fAmpThresholdLow-fAmpThresholdHigh)<0.001)
	{
	  std::cout<<cfg->GetName()<<" Digitizer config. ERROR: inconsistent confuguration .... fAmpThresholdLow and fAmpThresholdHigh cannot be ~equal if singleHit reco is selected"<<std::endl;
	}
    }
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


/*Double_t DigitizerChannelReco::CalcPedestal() {
  fPed = 0.;
  fNPedSamples = 0;
  //Call this only after the maximum is set
  if(fIMax - fPedOffset < 0) {
    //What to do if not enough samples for pedestal calculation... 
    //Use the first few samples
    fPed = fSamples[0];
    fNPedSamples = 1;
    return fPed;
  } 

  for(Short_t i = fIMax - fPedOffset; 
      i >= 0 &&  fNPedSamples <  fPedMaxNSamples; 
      --i,++fNPedSamples) {
    //    std::cout << i << "  " << fNPedSamples << std::endl;
    fPed+= fSamples[i];
  }
  

  fPed /= fNPedSamples;

  return fPed;
}
*/

Double_t DigitizerChannelReco::CalcPedestal() {  //the pedestal is calculated using the first 200 samples
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
  static Double_t AbsSamRec[1024];
  static Double_t AbsSamRecDP[1024];


  for(UShort_t s=0;s<iMax;s++){
    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+ped)/4096.*1000.; //in mV positivo
    //std::cout<< s << "     "  << fSamples[s]  <<" V "<< AbsSamRec[s]  <<std::endl;
  }
  for(UShort_t s=iMax;s<1024;s++){
    AbsSamRec[s] = 0;
  }

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
      fAmpli=yp;
      Time=xp*fTimeBin;
      Int_t bin    = H1->GetXaxis()->FindBin(xp);

      TRecoVHit *Hit = new TRecoVHit();  
      Hit->SetTime(Time);
      Hit->SetEnergy(fAmpli);
      hitArray.push_back(Hit);
    }
    H1->Reset();
    //    delete s;
  }

  Time = Time*fTimeBin;
  return Time;

}

void DigitizerChannelReco::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  //  Double_t IsZeroSup = ZSupHit(5.,1000.);
  CalcCharge(fIMax);
  //if (fCharge < .01) return;
  // come back to a Veto setup
  if (fCharge < fChargeCut) return;
  double time = CalcTime(fIMax);

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(time);
  // M. Raggi going to charge
  // Double_t fEnergy= fCharge*1000./15; //going from nC to MeV using 15pC/MeV
  // Hit->SetEnergy(fCharge);
  // come back to a Veto setup
  Hit->SetEnergy(fCharge);
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

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    CalcMaximum();
    //    std::cout<<" fIMax =  "<<fIMax << " Amplitude   " << fPed - fMax <<std::endl;
    CalcPedestal();
    //    if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}



