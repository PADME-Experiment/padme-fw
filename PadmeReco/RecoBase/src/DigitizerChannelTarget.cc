#include "DigitizerChannelTarget.hh"
#include <iostream>
#include "TObject.h"
#include "TVector3.h"

void DigitizerChannelTarget::PrintConfig(){
  std::cout << "CCD: " << fCCD << std::endl;
}


void DigitizerChannelTarget::Init(PadmeVRecoConfig *cfg){

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
  
  fCCD            = cfg->GetParOrDefault("RECO","CCD",20);

  //Adc channel pedestals;
  fPedCh[0] = 2379.11;//3812.9;
  fPedCh[1] = 2357.91;//3779.4;
  fPedCh[2] = 2628.81;//3788.7;
  fPedCh[3] = 2457.88;//3782.7;
  fPedCh[4] = 2323.21;//3802.4;
  fPedCh[5] = 2287.45;//3789.3;
  fPedCh[6] = 2416.08;//3750.8;
  fPedCh[7] = 2466.10;//3771.0;
  fPedCh[8] = 2382.54;//3792.3;
  fPedCh[9] = 2512.34;//3763.2;
  fPedCh[10]= 2488.23;//3804.9;
  fPedCh[11]= 2264.82;//3765.5;
  fPedCh[12]= 2234.57;//3758.5;
  fPedCh[13]= 2491.47;//3795.3;
  fPedCh[14]= 2433.48;//3760.8;
  fPedCh[15]= 2357.79;//3763.3;
  fPedCh[16]= 2537.36;//3819.7;
  fPedCh[17]= 2355.11;//3807.9;
  fPedCh[18]= 2373.30;//3818.3;
  fPedCh[19]= 2235.69;//3807.5;
  fPedCh[20]= 2269.24;//3790.5;
  fPedCh[21]= 2401.07;//3840.6;
  fPedCh[22]= 2236.90;//3822.1;
  fPedCh[23]= 2370.31;//3780.9;
  fPedCh[24]= 2360.48;//3834.1;
  fPedCh[25]= 2318.00;//3810.3;
  fPedCh[26]= 2386.86;//3868.1;
  fPedCh[27]= 2430.60;//3838.1;
  fPedCh[28]= 2277.87;//3803.4;
  fPedCh[29]= 2278.70;//3796.0;
  fPedCh[30]= 2564.95;//3835.2;
  fPedCh[31]= 2305.52;//3824.0;


  //fStrips mapping
  fStrip[0] = 16; //X17
  fStrip[1] = 14; //X15
  fStrip[2] = 12; //X13
  fStrip[3] = 10; //X11
  fStrip[4] =  8; //X9
  fStrip[5] =  6; //X7
  fStrip[6] =  4; //X5
  fStrip[7] =  2; //X3
  fStrip[8] =  1; //X2
  fStrip[9] =  3; //X4
  fStrip[10]=  5; //X6
  fStrip[11]=  7; //X8
  fStrip[12]=  9; //X10
  fStrip[13]= 11; //X12
  fStrip[14]= 13; //X14 
  fStrip[15]= 15; //X16
  fStrip[16]=  8; //Y8
  fStrip[17]=  6; //Y6
  fStrip[18]=  4; //Y4
  fStrip[19]=  2; //Y2
  fStrip[20]=  3; //Y3
  fStrip[21]=  5; //Y5
  fStrip[22]=  7; //Y7
  fStrip[23]=  9; //Y9
  fStrip[24]= 11; //Y11
  fStrip[25]= 13; //Y13
  fStrip[26]= 15; //Y15
  fStrip[27]= 17; //Y17
  fStrip[28]= 16; //Y16
  fStrip[29]= 14; //Y14
  fStrip[30]= 12; //Y12
  fStrip[31]= 10; //Y10

  //fStrips geometry
  fAxis[0] =  0; //X17
  fAxis[1] =  0; //X15
  fAxis[2] =  0; //X13
  fAxis[3] =  0; //X11
  fAxis[4] =  0; //X9
  fAxis[5] =  0; //X7
  fAxis[6] =  0; //X5
  fAxis[7] =  0; //X3
  fAxis[8] =  0; //X2
  fAxis[9] =  0; //X4
  fAxis[10]=  0; //X6
  fAxis[11]=  0; //X8
  fAxis[12]=  0; //X10
  fAxis[13]=  0; //X12
  fAxis[14]=  0; //X14 
  fAxis[15]=  0; //X16
  fAxis[16]=  1; //Y8
  fAxis[17]=  1; //Y6
  fAxis[18]=  1; //Y4
  fAxis[19]=  1; //Y2
  fAxis[20]=  1; //Y3
  fAxis[21]=  1; //Y5
  fAxis[22]=  1; //Y7
  fAxis[23]=  1; //Y9
  fAxis[24]=  1; //Y11
  fAxis[25]=  1; //Y13
  fAxis[26]=  1; //Y15
  fAxis[27]=  1; //Y17
  fAxis[28]=  1; //Y16
  fAxis[29]=  1; //Y14
  fAxis[30]=  1; //Y12
  fAxis[31]=  1; //Y10

  fMeasure[0] =  7.5; //X17
  fMeasure[1] =  5.5; //X15
  fMeasure[2] =  3.5; //X13
  fMeasure[3] =  1.5; //X11
  fMeasure[4] = -0.5; //X9
  fMeasure[5] = -2.5; //X7
  fMeasure[6] = -4.5; //X5
  fMeasure[7] = -6.5; //X3
  fMeasure[8] = -7.5; //X2
  fMeasure[9] = -5.5; //X4
  fMeasure[10]= -3.5; //X6
  fMeasure[11]= -1.5; //X8
  fMeasure[12]=  0.5; //X10
  fMeasure[13]=  2.5; //X12
  fMeasure[14]=  4.5; //X14 
  fMeasure[15]=  6.5; //X16
  fMeasure[16]= -1.5; //Y8
  fMeasure[17]= -3.5; //Y6
  fMeasure[18]= -5.5; //Y4
  fMeasure[19]= -7.5; //Y2
  fMeasure[20]= -6.5; //Y3
  fMeasure[21]= -4.5; //Y5
  fMeasure[22]= -2.5; //Y7
  fMeasure[23]= -0.5; //Y9
  fMeasure[24]=  1.5; //Y11
  fMeasure[25]=  3.5; //Y13
  fMeasure[26]=  5.5; //Y15
  fMeasure[27]=  7.5; //Y17
  fMeasure[28]=  6.5; //Y16
  fMeasure[29]=  4.5; //Y14
  fMeasure[30]=  2.5; //Y12
  fMeasure[31]=  0.5; //Y10

  //Calibration Constant fStrips
  
  fCalibCh[0] = 1.75; //X17
  fCalibCh[1] = 1.75; //X15
  fCalibCh[2] = 1.75; //X13
  fCalibCh[3] = 1.75; //X11
  fCalibCh[4] = 1.75; //X9
  fCalibCh[5] = 1.75; //X7
  fCalibCh[6] = 1.75; //X5
  fCalibCh[7] = 1.75; //X3
  fCalibCh[8] = 1.75; //X2
  fCalibCh[9] = 1.75; //X4
  fCalibCh[10]= 1.75; //X6
  fCalibCh[11]= 1.75; //X8
  fCalibCh[12]= 1.75; //X10
  fCalibCh[13]= 1.75; //X12
  fCalibCh[14]= 1.75; //X14 
  fCalibCh[15]= 1.75; //X16
  fCalibCh[16]= 1.74*0+1.75; //Y8
  fCalibCh[17]= 1.79*0+1.75; //Y6
  fCalibCh[18]= 1.76*0+1.75; //Y4
  fCalibCh[19]= 1.85*0+1.75; //Y2
  fCalibCh[20]= 1.83*0+1.75; //Y3
  fCalibCh[21]= 1.81*0+1.75; //Y5
  fCalibCh[22]= 1.67*0+1.75; //Y7
  fCalibCh[23]= 1.21*0+1.75; //Y9
  fCalibCh[24]= 1.73*0+1.75; //Y11
  fCalibCh[25]= 1.66*0+1.75; //Y13
  fCalibCh[26]= 1.77*0+1.75; //Y15
  fCalibCh[27]= 1.75*0+1.75; //Y17
  fCalibCh[28]= 1.71*0+1.75; //Y16
  fCalibCh[29]= 1.69*0+1.75; //Y14
  fCalibCh[30]= 1.45*0+1.75; //Y12
  fCalibCh[31]= 1.75*0+1.75; //Y10

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
  
  for(Short_t i = 0 ; i  !=   fPedMaxNSamples; i++) {
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
  
  //std::cout << "Begin: "<< begin << "  end: " << end << std::endl;
  fCharge=0.;
  for(Short_t i = begin;i<end;++i) {
    fCharge+=1.* fSamples[i];
    //std::cout << i <<" fSamples[i] " << fSamples[i] << std::endl;
  }
   //std::cout << "Integral: " << fCharge << "  Ped:  " << fPed << " Base: " << (end-begin) * fPed << std::endl;
   //std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
   //std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;

   fCharge = fCharge- ((1.*end-1.*begin) * fPed);
   fCharge *= (fVoltageBin*fTimeBin/fImpedance);
   if (fabs(fCalibCh [fCh])>0.0001) fCharge = fCharge / fCalibCh [fCh];
     
   //std::cout << "Integral: " << fCharge << "  Ped:  " << fPed << " Base: " << (end-begin) * fPed << std::endl;
  return fCharge;
}

Double_t DigitizerChannelTarget::CalcTime(UShort_t iMax) {
  fTime = 0.;
  //currently lookingTVector3 only at the signal rising edge
  
  float t1;
  float t2;

  float t3;
  float t4;

  
  float val1;
  float val2;

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

  

  return fTime;
}



Double_t DigitizerChannelTarget::CalcPosition(UShort_t fCh) {
  
   Double_t fPosition;
   
   fPosition = fMeasure[fCh];
 
   return fPosition;
}
void DigitizerChannelTarget::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  Int_t fCh =  hitArray.size();
  CalcCharge(fCh);
  
  //if (fCharge < .1) return;
  CalcTime(fIMax);

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(fTime);
  Hit->SetEnergy(fCharge);
  TVector3 position;
  if(fAxis[fCh]==0)
  {
   position.SetXYZ( CalcPosition(fCh), -1000, 0);
  }
  else
  {
   position.SetXYZ( -1000, CalcPosition(fCh), 0);
  }
  
  Hit->SetPosition(position);
  hitArray.push_back(Hit);

  
  //std::cout << "fCh " <<  fCh << " Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
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



