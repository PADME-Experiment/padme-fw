#include "DigitizerChannelReco.hh"
#include <iostream>
#include "TMath.h"

void DigitizerChannelReco::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}


void DigitizerChannelReco::Init(PadmeVRecoConfig *cfg){

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


  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0);
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);

  fChargeCut      = cfg->GetParOrDefault("RECO","ChargeCut",0.001);//added by Beth 19/4/19
  
  
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  PrintConfig();
}


void DigitizerChannelReco::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}
 
Short_t DigitizerChannelReco::CalcMaximum() {

  fMax = 32767; // 2^15 - 1 signal amplitude in units of ADC
  
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) {
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}

Double_t DigitizerChannelReco::CalcPedestal() {
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

/*Double_t DigitizerChannelReco::CalcPedestal() {  //the pedestal is calculated using the first 200 samples //C&Ped by Beth from Federica O 15/5/19
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
  }*/


Double_t DigitizerChannelReco::ZSupHit(Float_t Thr, UShort_t NAvg) {
  Double_t rms1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;
  //  if(rms1000>Thr){
  if(rms1000>4.){
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
  begin = 180;
  end =  1000;

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
  fTime = 0.;
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

  Short_t begin = fIMax - fPreSamples > 0? fIMax - fPreSamples:0;
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
  fTime = t3 - (t4-t3)*fAmpThresholdLow/(fAmpThresholdHigh - fAmpThresholdLow);
  return fTime = fTime*fTimeBin;
}


void DigitizerChannelReco::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  Double_t IsZeroSup = ZSupHit(5.,1000.);
  CalcCharge(fIMax);
  if (fCharge < .01) return;//this is in nC, so it's = 10pC - big cut. Should be closer to 25 photo electrons*electron charge*CPM gain, so about 1pC or 0.001nC. Therefore a cut of 5pC Should also go within config file
  CalcTime(fIMax);

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(fTime);
  // M. Raggi going to charge
  Double_t fEnergy= fCharge*1000./15; //going from nC to MeV using 15pC/MeV //This is for the ECal. If they were coded in the config file then you could have an independent value for each detector, instead of using this conversion for every detector
  //  Double_t fEnergy = fMax;//Changed by Beth 19/4/19 to see what the effect of the charge calculation is on the Occupancy of the PVeto. Original, pre-19/4/19 is commented out in the line above
  //Double_t fEnergy = fPed-fMax; //Changed by Beth 24/4/19 again to see what the effect of the charge calculation is on the Occupancy of the PVeto, using maximum amplitude (with pedestal subtracted) as a calculation of energy
  //  Double_t fEnergy = fPed; // Changed by Beth 6/5/19 to inspect pedestals. Now (8/5/19) obsolete since hit pedestal can be accessed directly, thanks to changing the persistency in TRecoVHit.hh  
//  Hit->SetEnergy(fCharge);
  Hit->SetEnergy(fEnergy);
  Hit->SetPed(fPed);
  Hit->SetCharge(fCharge);
  Hit->SetAmplitude(fPed-fMax);

  for(UShort_t i = 0;i<fNSamples;i++){
    Hit->SetWaveform(i,fSamples[i]);
  }

  hitArray.push_back(Hit);

  // std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelReco::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  
  
}


void DigitizerChannelReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  if(fUseAbsSignals) {
    SetAbsSignals();
  }

  CalcMaximum();
  CalcPedestal();

  if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}
