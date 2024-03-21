#include "DigitizerChannelTarget.hh"
#include <iostream>
#include "TObject.h"
#include "TVector3.h"

DigitizerChannelTarget::DigitizerChannelTarget()
{
  fGlobalMode = NULL;
  fLocalMode  = NULL;
}
void DigitizerChannelTarget::PrintConfig(){
  std::cout << "TimeBin "		 << fTimeBin	      << std::endl;
  std::cout << "VoltageBin "		 << fVoltageBin       << std::endl;
  std::cout << "InputImpedance " 	 << fImpedance        << std::endl;
  std::cout << "SignalWindow "		 << fSignalWidth      << std::endl;
  std::cout << "PedestalOffset " 	 << fPedOffset        << std::endl;
  std::cout << "SignalPreSamples "	 << fPreSamples       << std::endl;
  std::cout << "SignalPostSamples "	 << fPostSamples      << std::endl;
  std::cout << "NumberOfSamplesPedestal "<< fPedMaxNSamples   << std::endl;
  std::cout << "MinAmplitude "		 << fMinAmplitude     << std::endl;
  std::cout << "AmplThrLow "		 << fAmpThresholdLow  << std::endl;
  std::cout << "AmplThrHigh "		 << fAmpThresholdHigh << std::endl;
  std::cout << "Multihit "		 << fMultihit	      << std::endl;
  std::cout << "UseAbsSignals "  	 << fUseAbsSignals    << std::endl;
  std::cout << "CCD "			 << fCCD	      << std::endl;
  std::cout << "AverageGain "		 << fAverageGain      << std::endl;
  std::cout << "HVsign " 		 << fHVsign	      << std::endl;
  std::cout << "DefaultRuler "		 << fDefaultRuler     << std::endl;
  std::cout << "Ruler "		         << fRuler            << std::endl;
  std::cout << "PosCh1 " 		 << fMeasure [0]      << std::endl;
  std::cout << "PosCh2 " 		 << fMeasure [1]      << std::endl;
  std::cout << "PosCh3 " 		 << fMeasure [2]      << std::endl;
  std::cout << "PosCh4 " 		 << fMeasure [3]      << std::endl;
  std::cout << "PosCh5 " 		 << fMeasure [4]      << std::endl;
  std::cout << "PosCh6 " 		 << fMeasure [5]      << std::endl;
  std::cout << "PosCh7 " 		 << fMeasure [6]      << std::endl;
  std::cout << "PosCh8 " 		 << fMeasure [7]      << std::endl;
  std::cout << "PosCh9 " 		 << fMeasure [8]      << std::endl;
  std::cout << "PosCh10 "		 << fMeasure [9]      << std::endl;
  std::cout << "PosCh11 "		 << fMeasure [10]     << std::endl;
  std::cout << "PosCh12 "		 << fMeasure [11]     << std::endl;
  std::cout << "PosCh13 "		 << fMeasure [12]     << std::endl;
  std::cout << "PosCh14 "		 << fMeasure [13]     << std::endl;
  std::cout << "PosCh15 "		 << fMeasure [14]     << std::endl;
  std::cout << "PosCh16 "		 << fMeasure [15]     << std::endl;
  std::cout << "PosCh17 "		 << fMeasure [16]     << std::endl;
  std::cout << "PosCh18 "		 << fMeasure [17]     << std::endl;
  std::cout << "PosCh19 "		 << fMeasure [18]     << std::endl;
  std::cout << "PosCh20 "		 << fMeasure [19]     << std::endl;
  std::cout << "PosCh21 "		 << fMeasure [20]     << std::endl;
  std::cout << "PosCh22 "		 << fMeasure [21]     << std::endl;
  std::cout << "PosCh23 "		 << fMeasure [22]     << std::endl;
  std::cout << "PosCh24 "		 << fMeasure [23]     << std::endl;
  std::cout << "PosCh25 "		 << fMeasure [24]     << std::endl;
  std::cout << "PosCh26 "		 << fMeasure [25]     << std::endl;
  std::cout << "PosCh27 "		 << fMeasure [26]     << std::endl;
  std::cout << "PosCh28 "		 << fMeasure [27]     << std::endl;
  std::cout << "PosCh29 "		 << fMeasure [28]     << std::endl;
  std::cout << "PosCh30 "		 << fMeasure [29]     << std::endl;
  std::cout << "PosCh31 "		 << fMeasure [30]     << std::endl;
  std::cout << "PosCh32 "		 << fMeasure [31]     << std::endl;
}


void DigitizerChannelTarget::Init(GlobalRecoConfigOptions* gMode,
				  PadmeVRecoConfig *cfg)
{

  fGlobalMode = gMode;
    
  fTimeBin           = cfg->GetParOrDefault( "ADC","TimeBin"                ,      1.);
  fVoltageBin        = cfg->GetParOrDefault( "ADC","VoltageBin"             ,0.000244);
  fImpedance         = cfg->GetParOrDefault( "ADC","InputImpedance"         ,     50.);
  fSignalWidth       = cfg->GetParOrDefault("RECO","SignalWindow"           ,    1024);
  fPedOffset         = cfg->GetParOrDefault("RECO","PedestalOffset"         ,     100); 
  fPreSamples        = cfg->GetParOrDefault("RECO","SignalPreSamples"       ,    1024);
  fPostSamples       = cfg->GetParOrDefault("RECO","SignalPostSamples"      ,    1024);
  fPedMaxNSamples    = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",     100);  
  fMinAmplitude      = cfg->GetParOrDefault("RECO","MinAmplitude"           ,      10);
  fAmpThresholdLow   = cfg->GetParOrDefault("RECO","AmplThrLow"             ,     10.);
  fAmpThresholdHigh  = cfg->GetParOrDefault("RECO","AmplThrHigh"            ,     20.);
  fMultihit          = cfg->GetParOrDefault("RECO","Multihit"               ,       0);
  fUseAbsSignals     = cfg->GetParOrDefault("RECO","UseAbsSignals"          ,       0);
  /* 
  fCCD               = cfg->GetParOrDefault("RECO","CCD"                    ,      12);
  fAverageGain       = cfg->GetParOrDefault("RECO","AverageGain"            ,    1300);
  fHVsign            = cfg->GetParOrDefault("RECO","HVsign"                 ,      -1);
  */
  fDefaultRuler      = cfg->GetParOrDefault("RECO","DefaultRuler"           ,    52.0);
  fRuler             = cfg->GetParOrDefault("RECO","Ruler"                  ,    52.0);
  fMeasure [0]       = cfg->GetParOrDefault("RECO","PosCh1"		    ,	   -8);
  fMeasure [1]       = cfg->GetParOrDefault("RECO","PosCh2"		    ,	   -7);
  fMeasure [2]       = cfg->GetParOrDefault("RECO","PosCh3"		    ,	   -6);
  fMeasure [3]       = cfg->GetParOrDefault("RECO","PosCh4"		    ,	   -5);
  fMeasure [4]       = cfg->GetParOrDefault("RECO","PosCh5"		    ,	   -4);
  fMeasure [5]       = cfg->GetParOrDefault("RECO","PosCh6"		    ,	   -3);
  fMeasure [6]       = cfg->GetParOrDefault("RECO","PosCh7"		    ,	   -2);
  fMeasure [7]       = cfg->GetParOrDefault("RECO","PosCh8"		    ,	   -1);
  fMeasure [8]       = cfg->GetParOrDefault("RECO","PosCh9"		    ,	    0);
  fMeasure [9]       = cfg->GetParOrDefault("RECO","PosCh10"		    ,	    1);
  fMeasure [10]      = cfg->GetParOrDefault("RECO","PosCh11"		    ,	    2);
  fMeasure [11]      = cfg->GetParOrDefault("RECO","PosCh12"		    ,	    3);
  fMeasure [12]      = cfg->GetParOrDefault("RECO","PosCh13"		    ,	    4);
  fMeasure [13]      = cfg->GetParOrDefault("RECO","PosCh14"		    ,	    5);
  fMeasure [14]      = cfg->GetParOrDefault("RECO","PosCh15"		    ,	    6);
  fMeasure [15]      = cfg->GetParOrDefault("RECO","PosCh16"		    ,	    7);
  fMeasure [16]      = cfg->GetParOrDefault("RECO","PosCh17"		    ,	   -8);
  fMeasure [17]      = cfg->GetParOrDefault("RECO","PosCh18"		    ,	   -7);
  fMeasure [18]      = cfg->GetParOrDefault("RECO","PosCh19"		    ,	   -6);
  fMeasure [19]      = cfg->GetParOrDefault("RECO","PosCh20"		    ,	   -5);
  fMeasure [20]      = cfg->GetParOrDefault("RECO","PosCh21"		    ,	   -4);
  fMeasure [21]      = cfg->GetParOrDefault("RECO","PosCh22"		    ,	   -3);
  fMeasure [22]      = cfg->GetParOrDefault("RECO","PosCh23"		    ,	   -2);
  fMeasure [23]      = cfg->GetParOrDefault("RECO","PosCh24"		    ,	   -1);
  fMeasure [24]      = cfg->GetParOrDefault("RECO","PosCh25"		    ,	    0);
  fMeasure [25]      = cfg->GetParOrDefault("RECO","PosCh26"		    ,	    1);
  fMeasure [26]      = cfg->GetParOrDefault("RECO","PosCh27"		    ,	    2);
  fMeasure [27]      = cfg->GetParOrDefault("RECO","PosCh28"		    ,	    3);
  fMeasure [28]      = cfg->GetParOrDefault("RECO","PosCh29"		    ,	    4);
  fMeasure [29]      = cfg->GetParOrDefault("RECO","PosCh30"		    ,	    5);
  fMeasure [30]      = cfg->GetParOrDefault("RECO","PosCh31"		    ,	    6);
  fMeasure [31]      = cfg->GetParOrDefault("RECO","PosCh32"		    ,	    7);

  std::cout << cfg->GetName() << "*******************************" <<  std::endl;

  PrintConfig();
}


void DigitizerChannelTarget::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}

Short_t DigitizerChannelTarget::CalcMaximum() {

  fMax = 32767; // 2^15 - 1
  
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) {
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}


Double_t DigitizerChannelTarget::CalcPedestal() {
  fPed = 0.;
  fNPedSamples = 0;
  
  for(Short_t i = fPedOffset ; i  !=   fPedMaxNSamples; i++) {
       fNPedSamples ++;
       //std::cout << i << " fNPedSamples " << fNPedSamples << std::endl;
       fPed+= fSamples[i];
  }
  
  fPed /= fNPedSamples;
  //std::cout <<  fPed << " fNPedSamples " << fNPedSamples << std::endl;
  return fPed;
}



Double_t DigitizerChannelTarget::CalcCharge(UShort_t fCh) {
  
  Short_t begin = fPedMaxNSamples;
  Short_t end   =    fPostSamples;
   
  Double_t hCharge=0.;
  
  for(Short_t i = begin;i<end;++i) {
    hCharge+=1.* fSamples[i];
    std::cout<<i<<" "<<hCharge<<std::endl;
  }
   hCharge = hCharge- ((1.*end-1.*begin) * fPed);
// hCharge *= (fVoltageBin*fTimeBin/fImpedance/fAverageGain);//fTimeBin in ns than charge in nC   
   hCharge *= (fVoltageBin*fTimeBin/fImpedance);             //fTimeBin in ns than charge in nC (charge in output of the amplifier)   
   hCharge *= 1000;                                          //charge in pC
// fCharge *= (1/1.60217662e-7/fCCD/36);                     //electron charge and 36 e-h/um // going to calib step 
// if( fHVsign<0 && fCh>15 ) hCharge = - hCharge;            // going to calib step 
// if( fHVsign>0 && fCh<16 ) hCharge = - hCharge;            // going to calib step 

  return hCharge;
}

Double_t DigitizerChannelTarget::CalcTime(UShort_t iMax) {
  
  Double_t hTime =  fPreSamples;
  return hTime;
}



Double_t DigitizerChannelTarget::CalcPosition(UShort_t fCh) {
  
   Double_t fPosition;
   
   fPosition = fMeasure[fCh];
   
   if(fCh<16) fPosition = fPosition - fDefaultRuler + fRuler ;
 
   return fPosition;
}
void DigitizerChannelTarget::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){

  //GABRIELE 7-NOV-2018 **********   fCh IS NOT THE CHANNEL OR STRIP BUT THE SIZE OF THE RECOSTRUCTED HIT ARRAY UP TO NOW
  Int_t fCh        =  hitArray.size();
  Double_t hCharge = CalcCharge(fCh);
  
  //if (fCharge < .1) return;
  Double_t hTime   = CalcTime(fIMax);

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime  (hTime  );
  Hit->SetEnergy(hCharge);
  TVector3 position;
  
  if(fCh<16)
  {
   position.SetXYZ( CalcPosition(fCh), -1000, 0);
  }
  else
  {
   position.SetXYZ( -1000, CalcPosition(fCh), 0);
  }
  
  Hit->SetPosition(position);
  hitArray.push_back(Hit);

  
  //std::cout << "fCh " <<  fCh << " Hit charge:  " << hCharge << "  Time: " << hTime << std::endl;
  
}

void DigitizerChannelTarget::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  
  
}


void DigitizerChannelTarget::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  if(fUseAbsSignals) {
    SetAbsSignals();
  }

  CalcMaximum();
  CalcPedestal();
  if(fPed - fMax < fMinAmplitude ) return;

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}



