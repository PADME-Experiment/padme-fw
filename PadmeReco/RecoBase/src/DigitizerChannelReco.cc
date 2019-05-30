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

  H1 = new TH1D("h1","h1",990,0.,990.);
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
  fChargeCut      = cfg->GetParOrDefault("RECO","ChargeCut",0.001);//added by Beth 19/4/19
  
  
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

  fMax = 32767; // 2^15 - 1 signal amplitude in units of ADC
  
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
  //std::cout<<"in ZSupHit"<<std::endl;
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

Double_t DigitizerChannelReco::CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t iMax, UShort_t ped) {
  double Time   = 0.;
  fCharge = 0.;
  Double_t pCMeV= 3.2E5*2*1.67E-7;
  Int_t NIntSamp= fSignalWidth/fTimeBin; 

  //currently looking for peaks with TSpectrum to obtain multi hit times
  //Int_t npeaks =25;
  Int_t npeaks =50;
  Double_t AbsSamRec[1024];
  

  for(UShort_t s=0;s<iMax;s++){
    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+ped)/4096*1000.; //in mV positivo
    //std::cout<< s  <<" V "<< AbsSamRec[s]  <<std::endl;
  }
  
  H1->SetContent(AbsSamRec);
  
  
  Double_t VMax = H1->GetMaximum();
  Double_t VMin = H1->GetMinimum();
  //std::cout<<"Get Maximum     "<<VMax<<"   Get Minimum    "<<VMin<<std::endl;
  //if (VMax>800) fEventsSaturated++;
  
  //if (VMax>fAmpThresholdHigh) std::cout<<VMax<<" VMax "<<std::endl;
  
   if(VMax>fAmpThresholdHigh){
    TSpectrum *s = new TSpectrum(2*npeaks);  //npeaks max number of peaks
    Double_t peak_thr  = fAmpThresholdLow/VMax; //minimum peak height allowed. 
    //Int_t nfound = s->Search(H1,10,"",peak_thr);     
    Int_t nfound = s->Search(H1,10,"",peak_thr);     
    //Float_t *xpeaks = s->GetPositionX();
    //Float_t *ypeaks = s->GetPositionY();
    //std::cout<<"Threshold    file Config     "<<fAmpThresholdLow<<"  Peak in Search with TSpectrum   "<<peak_thr<<"Vmax    "<<VMax<<std::endl;
    //std::cout<<"found Npeaks "<<nfound<<" *xpeaks    "<<*xpeaks<<" *ypeaks    "<<*ypeaks<<std::endl;
    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      fCharge = 0.;
      //Float_t xp   = xpeaks[ll];
      Double_t xp = (s->GetPositionX())[ll];
      //Float_t yp   = ypeaks[ll];
      Double_t yp = (s->GetPositionY())[ll];
      fAmpli=yp;
      Time=xp*fTimeBin;
      Int_t bin    = H1->GetXaxis()->FindBin(xp);
      if(bin<1000){
	for (Int_t ii=bin-NIntSamp;ii<bin+NIntSamp;ii++) {
	  if(H1->GetBinContent(ii)>0.005) fCharge += H1->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;
	}
	//std::cout<<nfound<<"number of peak "<<ll<<" Digi Charge  "<<fCharge<<" Time "<<fTime<<" yp "<<yp<<" xp "<<xp<<" EMeV "<<fCharge/pCMeV<<std::endl;
        //std::cout<<nfound<<"number of peak "<<ll<<" fAmplitude     "<<fAmpli<<"   x peak"<<xp<<std::endl;
      }
      //fEnergy = fCharge/pCMeV;	  
      TRecoVHit *Hit = new TRecoVHit();  
      Hit->SetTime(Time);
      //      Hit->SetEnergy(fCharge);
      //Hit->SetEnergy(fCharge);     
      Hit->SetEnergy(fAmpli);
      
      for(Int_t i = 0;i<fNSamples;i++){
	Hit->SetWaveform(i,fPed-1.*fSamples[i]);
	//    std::cout<<"fSamples[i] = "<< fSamples[i]<< "Hit->GetWaveform() = "<<Hit->GetWaveform()[i]<<std::endl;
      }

      hitArray.push_back(Hit);
      //if (fAmpli>800) fPeaksSaturated++; 
      //if(fPeaksSaturated>1 && fEventsSaturated==0)fEventsSaturated++; 
      //std::cout<<xp<<" "<<xp<<" yp "<<yp<<" Ch "<<fCh<<" VMax "<<VMax<<" thr "<<peak_thr<<" "<<fAmpThresholdLow<<std::endl;
    }
    H1->Reset();
  }

  Time = Time*fTimeBin;
  return Time;

}

void DigitizerChannelReco::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  //  std::cout<<"Reconstructing single hit" <<std::endl;
  Double_t IsZeroSup = ZSupHit(5.,1000.);
  CalcCharge(fIMax);
  //if (fCharge < .01) return; //this is in nC, so it's = 10pC - big cut. Should be closer to 25 photo electrons*electron charge*CPM gain, so about 1pC or 0.001nC. Therefore a cut of 5pC should also go in config file
  // come back to a Veto setup
  if (fCharge < fChargeCut) return;
  double time = CalcTime(fIMax);

  TRecoVHit *Hit = new TRecoVHit();
  Hit->SetTime(time);
  // M. Raggi going to charge
  //Double_t fEnergy= fCharge*1000./15; //going from nC to MeV using 15pC/MeV //This is for the ECal. If they were coded in the config file then you could have an independent value for each detector, instead of using this conversion for every detector
  Hit->SetEnergy(fCharge);
  // come back to a Veto setup
  // Hit->SetEnergy(fEnergy);
  // std::cout<<"fEnergy is... "<< fEnergy <<std::endl;
  // Hit->SetPed(fPed);
  // Hit->SetCharge(fCharge);
  // Hit->SetAmplitude(fPed-fMax);
  
  for(Int_t i = 0;i<fNSamples;i++){
    Hit->SetWaveform(i,fPed-1.*fSamples[i]);
    //    std::cout<<"fSamples[i] = "<< fSamples[i]<< "Hit->GetWaveform() = "<<Hit->GetWaveform()[i]<<std::endl;
  }


  hitArray.push_back(Hit);

  // std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
  
}

void DigitizerChannelReco::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){  //using TSpectrum
    std::cout<<"Reconstructing multi hit" <<std::endl;
  Double_t ped=CalcPedestal();
  //std::cout<<"Pedestal    "<<ped<<std::endl;
  //ped=3650;
  CalcChaTime(hitArray,1000,ped);

}


void DigitizerChannelReco::Reconstruct(std::vector<TRecoVHit *> &hitArray){
 /* if(fUseAbsSignals) {
    SetAbsSignals();
  }*/
  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    CalcMaximum();
    //std::cout<<" fIMax =  "<<fIMax<<std::endl;
    CalcPedestal();
    if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}



