#include "DigitizerChannelSAC.hh"

#include "TObject.h"
#include "TVector3.h"
#include "TSpectrum.h"
#include "TCanvas.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

void DigitizerChannelSAC::PrintConfig(){
  std::cout << "Hi I'm the SAC: " << std::endl;
  std::cout << "Thr low  "<< fAmpThresholdLow  << std::endl;
  std::cout << "Time bin "<< fTimeBin  << std::endl;
  std::cout << "Time window per single hit "<< fSignalWidth  << std::endl;
}


void DigitizerChannelSAC::Init(PadmeVRecoConfig *cfg){
  H1 = new TH1D("h1","h1",990,0.,990.);

  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
  fVoltageBin     = cfg->GetParOrDefault("ADC","VoltageBin",0.000244);
  fImpedance      = cfg->GetParOrDefault("ADC","InputImpedance",50.);

  fSignalWidth    = cfg->GetParOrDefault("RECO","SignalWindow",1024);  //defined per single hit M. Raggi 9/11/2018
  fPedOffset      = cfg->GetParOrDefault("RECO","PedestalOffset",100);
  
  fPreSamples     = cfg->GetParOrDefault("RECO","SignalPreSamples",1024);
  fPostSamples    = cfg->GetParOrDefault("RECO","SignalPostSamples",1024);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);  

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);

  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0);
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);
  
  //Adc channel pedestals;
  fPedCh[0] = 3800;
  fPedCh[1] = 3800;
  fPedCh[2] = 3800;
  fPedCh[3] = 3800;
  fPedCh[4] = 3800;
  fPedCh[5] = 3800;
  fPedCh[6] = 3800;
  fPedCh[7] = 3800;
  fPedCh[8] = 3800;
  fPedCh[9] = 3800;
  fPedCh[10]= 3800;
  fPedCh[11]= 3800;
  fPedCh[12]= 3800;
  fPedCh[13]= 3800;
  fPedCh[14]= 3800;
  fPedCh[15]= 3800;
  fPedCh[16]= 3800;
  fPedCh[17]= 3800;
  fPedCh[18]= 3800;
  fPedCh[19]= 3800;
  fPedCh[20]= 3800;
  fPedCh[21]= 3800;
  fPedCh[22]= 3800;
  fPedCh[23]= 3800;
  fPedCh[24]= 3800;
  fPedCh[25]= 3800;
  fPedCh[26]= 3800;
  fPedCh[27]= 3800;
  fPedCh[28]= 3800;
  fPedCh[29]= 3800;
  fPedCh[30]= 3800;
  fPedCh[31]= 3800;

  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  SetAnalogOffSets(); //M. Raggi: 19/10/2018
  PrintConfig();
}

//M. Raggi: 19/10/2018
void DigitizerChannelSAC::SetAnalogOffSets(){
  std::ifstream PrevPed; //reading prevoius pedestal from file written by drawped32
  char fname [50];
  Int_t NBD=27;
  sprintf(fname,"config/BDPed/PedBD%d.ped",NBD);
  PrevPed.open(fname);
  if(PrevPed.is_open()){
    double temp;
    for(int i=0;i<25;i++){
      PrevPed >> temp >> fPedCh[i] >> temp>> temp;//reads channel number and discards it
      //std::cout <<"PEDCH "<<i<<" "<<fPedCh[i]<<std::endl;
    }
  }
  else{ 
    std::cout << "No previous data available for board "<<NBD<<" resorting to default pedestal (3800)"<<std::endl;
  } 
 
}

//void DigitizerChannelSAC::SetAbsSignals(){
//  for(UShort_t i = 0;i<fNSamples;i++){
//    if (fSamples[i] < 2048) {
//      fSamples[i] = 4096 - fSamples[i];
//    }
//  }
//}

Short_t DigitizerChannelSAC::CalcMaximum() {

  fMax = 32767; // 2^15 - 1
  
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) {
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}

Double_t DigitizerChannelSAC::CalcPedestal() {
  fPed = 0.;
  fNPedSamples = 0;

  // average of first fPedMaxNSamples
  for(Short_t i = 0 ; i  <   fPedMaxNSamples; i++) {
       fNPedSamples ++;
       fPed+= fSamples[i];
  }
//  std::cout << " fNPedSamples " << fNPedSamples <<" fPed SAC"<< std::endl;
  fPed /= fNPedSamples;
  //std::cout <<  fPed << " fNPedSamples " << fNPedSamples << std::endl;
  return fPed;
}


Double_t DigitizerChannelSAC::CalcCharge(UShort_t fCh) {  //not used
  
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

Double_t DigitizerChannelSAC::CalcTime(UShort_t iMax, UShort_t fCh) {
  fTime = 0.;
  //currently looking for peaks with TSpectrum to obtain multi hit times
  //M. Raggi 19/10/2018
//  Int_t npeaks =25;
//  Double_t AbsSamRec[1024];
//  //  TH1D* H1 = new TH1D("h1","h1",990,0.,990.);
//  for(UShort_t s=0;s<iMax;s++){
//    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+fPedCh[fCh]*0+3800)/4096*1000.;
//  }
//  H1->SetContent(AbsSamRec);
//  Double_t VMax = H1->GetMaximum();
//  if(VMax>fAmpThresholdHigh){
//    TSpectrum *s = new TSpectrum(npeaks);
//    Double_t peak_thr  = fAmpThresholdLow/VMax; //minimum peak height allowed. 
//    Int_t nfound = s->Search(H1,2.,"",peak_thr);
//    
//    Double_t *xpeaks = s->GetPositionX();
//    Double_t *ypeaks = s->GetPositionY();
//    //    Float_t *xpeaks = s->GetPositionX();
//    //    Float_t *ypeaks = s->GetPositionY();
//    //    std::cout<<"found Npeaks "<<nfound<<""<<std::endl;
//    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
//      Float_t xp   = (Double_t) xpeaks[ll];
//    }
//    H1->Reset();
//  }
  return fTime=fTime*fTimeBin;
}

Double_t DigitizerChannelSAC::CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t iMax, UShort_t fChPedestal) {
  fTime   = 0.;
  fCharge = 0.;
  Double_t pCMeV= 3.2E5*2*1.67E-7; //Nominal Gain at 1500 x npe/MeV x echarge (in pC)
  //  Double_t pCMeV= 1.; //Nominal Gain at 1500 x npe/MeV x echarge (in pC)
  Int_t NIntSamp= fSignalWidth/fTimeBin; 
      
  //currently looking for peaks with TSpectrum to obtain multi hit times
  //M. Raggi 19/10/2018
  Int_t npeaks =35;
  Double_t AbsSamRec[1024];

  for(UShort_t s=0;s<iMax;s++){
    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+fChPedestal)/4096*1000.; //in mV positivo
  }
  H1->SetContent(AbsSamRec);
  Double_t VMax = H1->GetMaximum();
  //std::cout<<VMax<<" VMax "<< " fCh "<<fCh<<std::endl;
  if(VMax>fAmpThresholdHigh){ // zero suppression on Voltage normalize to energy.
    TSpectrum *s = new TSpectrum(npeaks);
    Double_t peak_thr  = fAmpThresholdLow/VMax; //minimum peak height allowed. 
    Int_t nfound = s->Search(H1,NIntSamp,"",peak_thr);    //corrected for 2.5GHz cannot be less then 0.05 
    // ROOT 6 version
    //    Double_t *xpeaks = s->GetPositionX();
    //    Double_t *ypeaks = s->GetPositionY();
    // ROOT 5 version
    Double_t *xpeaks = s->GetPositionX();
    Double_t *ypeaks = s->GetPositionY();
    //    std::cout<<"found Npeaks "<<nfound<<""<<std::endl;
    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      fCharge = 0.;
//      Double_t xp   = xpeaks[ll];
//      Double_t yp   = ypeaks[ll];
      Float_t xp   = xpeaks[ll];
      Float_t yp   = ypeaks[ll];
      fTime = xp*fTimeBin; //convert time in ns get it from data
      Int_t bin    = H1->GetXaxis()->FindBin(xp);
      if(bin<1000){
	for (Int_t ii=bin-NIntSamp;ii<bin+NIntSamp;ii++) {
	  if(H1->GetBinContent(ii)>0.001) fCharge += H1->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;  //charge in pC
	}
	//std::cout<<nfound<<" "<<ll<<" Digi Charge  "<<fCharge<<" Time "<<fTime<<" yp "<<yp<<" xp "<<xp<<" EMeV "<<fCharge/pCMeV<<std::endl;
      }
      fEnergy = fCharge/pCMeV;	  
      TRecoVHit *Hit = new TRecoVHit();  
      Hit->SetTime(fTime);
      //      Hit->SetEnergy(fCharge);
      Hit->SetEnergy(fEnergy);
      hitArray.push_back(Hit);  
      //      std::cout<<ll<<" "<<xp<<" yp "<<ypeaks[ll]<<" Ch "<<fCh<<" VMax "<<VMax<<" thr "<<peak_thr<<" "<<fAmpThresholdLow<<std::endl;
    }
    //    std::cout<<"end ch"<<std::endl;
    H1->Reset();
  }
  return fTime = fTime*fTimeBin;
}

Double_t DigitizerChannelSAC::CalcPosition(UShort_t fCh) {
  
   Double_t fPosition;
   
   fPosition = fMeasure[fCh];
 
   return fPosition;

}
void DigitizerChannelSAC::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  //GABRIELE 7-NOV-2018 **********   fCh IS NOT THE CHANNEL OR STRIP BUT THE SIZE OF THE RECOSTRUCTED HIT ARRAY UP TO NOW
  //Int_t fCh =  hitArray.size();
  //  CalcCharge(fCh);
  //  std::cout << "fCh " <<  fCh << " Hit Array " << std::endl;
  //if (fCharge < .1) return;
  //  CalcTime(fIMax);
  CalcChaTime(hitArray,1000,3800);

//  TRecoVHit *Hit = new TRecoVHit();
//  Hit->SetTime(fTime);
//  Hit->SetEnergy(fCharge);
//  TVector3 position;
//  if(fAxis[fCh]==0)
//  {
//   position.SetXYZ( CalcPosition(fCh), -1000, 0);  //update values with SAC postions
//  }
//  else
//  {
//   position.SetXYZ( -1000, CalcPosition(fCh), 0);
//  }
  
//  Hit->SetPosition(position);
//  hitArray.push_back(Hit);
  //std::cout << "fCh " <<  fCh << " Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelSAC::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){

  //GABRIELE 7-NOV-2018 **********   fCh IS NOT THE CHANNEL OR STRIP BUT THE SIZE OF THE RECOSTRUCTED HIT ARRAY UP TO NOW
  //Int_t fCh =  hitArray.size();
  //  CalcCharge(fCh);
  
  //if (fCharge < .1) return;
  //  CalcChaTime(hitArray,1000.,fCh);
  //  CalcTime(1000.,fCh);
  CalcChaTime(hitArray,1000,3800);
//  TRecoVHit *Hit = new TRecoVHit();
//  Hit->SetTime(fTime);
//  Hit->SetEnergy(fCharge);
//  TVector3 position;
//  
//  Hit->SetPosition(position);
//  hitArray.push_back(Hit);
  
  //std::cout << "fCh " <<  fCh << " Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelSAC::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  if(fUseAbsSignals) {
    SetAbsSignals();
  }

  //  CalcMaximum();
  //  CalcPedestal();
  // if(fPed - fMax < fMinAmplitude ) return;

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}

void DigitizerChannelSAC::SetAbsSignals(){
//  for(UShort_t i = 0;i<fNSamples;i++){
//    if (fSamples[i] < 2048) {
//      fSamples[i] = 4096 - fSamples[i];
//    }
//  }
}


