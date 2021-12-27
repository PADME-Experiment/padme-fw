#include"ChannelVReco.hh"

#include<iostream>
#include<stdlib.h>
#include<cassert>
#include<gsl/gsl_fft_halfcomplex_float.h>
#include<gsl/gsl_fft_real_float.h>


void ChannelVReco::Init(GlobalRecoConfigOptions *gOptions, PadmeVRecoConfig *cfg){

  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
  fVoltageBin     = cfg->GetParOrDefault("ADC","VoltageBin",0.000244);
  fImpedance      = cfg->GetParOrDefault("ADC","InputImpedance",50.);
  fmVtoMeV        = cfg->GetParOrDefault("RECO","ConversionFactor",29.52);

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);

  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);

  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0); //if MultiHit=1, Peaks finding with TSpectrum, MultiHit=2 -> signal deconvolution
  if(fMultihit == 2){
    fDeconvMultiplierRe           = cfg->GetConfigParser()->GetConfigVector<double>("RECO", "DeconvMultiplierRe");
    fDeconvMultiplierIm           = cfg->GetConfigParser()->GetConfigVector<double>("RECO", "DeconvMultiplierIm");
    fDeconvThreshold_mV           = cfg->GetConfigParser()->GetConfigSingle<float> ("RECO", "DeconvThreshold_mV");
    fDeconvIntegralAmplitudeRatio = cfg->GetConfigParser()->GetConfigSingle<float> ("RECO", "DeconvIntegralAmplitudeRatio");
  }
}

void ChannelVReco::ReconstructMultiHitSignalDeconvolution(std::vector<TRecoVHit *> &hitArray){
  std::vector<float> signal(fNSamples);
  float ped = 0;
  for(int i=0;i<200;i++){
    ped += fSamples[i];
  }
  ped /= 200;

  for(int t=0;t<fNSamples;t++){
    fSamples[t] = fSamples[t]-ped;
    signal  [t] = fSamples[t]/4.096;
  }
  const unsigned dft_len = std::min(fDeconvMultiplierRe.size(), fDeconvMultiplierIm.size());
  assert(2*dft_len+1<fNSamples);
  gsl_fft_real_float_radix2_transform(&signal[0], 1, fNSamples);
  for(unsigned w=dft_len;w<fNSamples-dft_len-1;w++) signal[w] = 0;
  std::vector<float> deriv(signal);
  signal[0]*=fDeconvMultiplierRe[0];
  deriv[0]=0;
  for(unsigned w=1;w<dft_len;w++){
    const float re = signal[w]*fDeconvMultiplierRe[w] - signal[fNSamples-w]*fDeconvMultiplierIm[w];
    const float im = signal[w]*fDeconvMultiplierIm[w] + signal[fNSamples-w]*fDeconvMultiplierRe[w];
    signal[w          ] = re;
    signal[fNSamples-w] = im;
    deriv [w          ] = re*w;
    deriv [fNSamples-w] = im*w;
  }
  gsl_fft_halfcomplex_float_radix2_inverse(&signal[0], 1, fNSamples);
  gsl_fft_halfcomplex_float_radix2_inverse(&deriv [0], 1, fNSamples);
  for(int t=0;t<fNSamples-24;t++) if(deriv[t]>0) deriv[t]=0;
  for(int t=0;t<100;t++){
    signal[t] = 0;
    deriv [t] = 0;
  }
  for(int t=fNSamples-24;t<fNSamples;t++){
    signal[t] = 0;
    deriv [t] = 0;
  }
  bool has_evt = false;
  for(int t=3; t<fNSamples-3-24;t++){
    if(signal[t]<fDeconvThreshold_mV &&
        deriv[t-3] > deriv[t] &&
        deriv[t-2] > deriv[t] &&
        deriv[t-1] > deriv[t] &&
        deriv[t+1] > deriv[t] &&
        deriv[t+2] > deriv[t] &&
        deriv[t+3] > deriv[t]){
      //has_evt = true;   // uncomment this in order to get waveforms to stdout
      TRecoVHit *Hit = new TRecoVHit();
      Hit->SetTime(t*fTimeBin);
      Hit->SetEnergy(-signal[t]/fmVtoMeV);//*fDeconvIntegralAmplitudeRatio*fTimeBin);
      hitArray.push_back(Hit);
    }
  }
  if(has_evt){
    for(int t=0;t<fNSamples;t++){
      std::cout
        <<"DECONV "
        <<"   "<<t*fTimeBin
        <<"   "<<fSamples[t]
        <<"   "<<signal[t]
        <<"   "<<deriv[t]/20.;
        if(signal[t]<fDeconvThreshold_mV &&
            deriv[t-3] > deriv[t] &&
            deriv[t-2] > deriv[t] &&
            deriv[t-1] > deriv[t] &&
            deriv[t+1] > deriv[t] &&
            deriv[t+2] > deriv[t] &&
            deriv[t+3] > deriv[t]){
          std::cout <<"   "<<signal[t];
        }else{
          std::cout <<"   "<<0;
          std::cout <<"   "<<0;
        }
      std::cout<<std::endl;
    }
    std::cout
      <<"DECONV " <<std::endl
      <<"DECONV " <<std::endl;
  }
}
