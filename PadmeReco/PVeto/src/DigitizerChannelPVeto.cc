#include "DigitizerChannelPVeto.hh"
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

void DigitizerChannelPVeto::PrintConfig(){
  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}

DigitizerChannelPVeto::~DigitizerChannelPVeto(){
  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode() || fSaveAnalog){
    SaveDebugHistos();
  }
}

void DigitizerChannelPVeto::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){

  fGlobalMode = gMode;
  std::string name;
  name = cfg->GetParOrDefault("DETECTOR","NAME","DIGI");
  name += "DIGI";
  hSig    = new TH1D("hSig","hSig",985,0.,985.);
  H1      = new TH1D("H1","H1",985,0.,985.);
  hdxdt   = new TH1D("hdxdt","hdxdt",1000,0.,1000.);
  htmp    = new TH1D("htmp","htmp",985,0.,985.);
  htmp->SetLineColor(1);

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

  fSaveAnalog = cfg->GetParOrDefault("Output","Analog",0); //M. Raggi: 03/03/2021  
  //  fSaveAnalog = true; //M. Raggi: 03/03/2021  

  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0); //if MultiHit=1, Peaks finding with TSpectrum
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);


  fUsePulseProcessing  = cfg->GetParOrDefault("RECO","UsePulseProcessing",1);
  fPeakSearchWidth     = cfg->GetParOrDefault("RECO","PeakSearchWidth",6);
  fZeroSuppression     = cfg->GetParOrDefault("RECO","ZeroSuppression",5.);
  fChargeCut           = cfg->GetParOrDefault("RECO","ChargeCut",0.01);
  fDPParameterR1       = cfg->GetParOrDefault("RECO","fDPParameterR1",650.);
  fDPParameterR2       = cfg->GetParOrDefault("RECO","fDPParameterR2",100.);
  fDPParameterC        = cfg->GetParOrDefault("RECO","fDPParameterC",0.030e-9);
  fmVtoMeV             = cfg->GetParOrDefault("RECO","ConversionFactor",29.52);

  
  std::cout << cfg->GetName() << "*******************************" <<  std::endl; 
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  std::cout << cfg->GetName() << "***I'M THE NEW PVETO DIGITIZER*" <<  std::endl;
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;  
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;  
  PrintConfig();
  
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog){
    PrepareDebugHistos(); //debugging mode histos
  }
  // if(Analog>0) PrepareDebugHistos(); //debugging mode histos
  //  }
  
  if (fMultihit==0)
    {
      if (fabs(fAmpThresholdLow-fAmpThresholdHigh)<0.001)
	{
	  std::cout<<cfg->GetName()<<" Digitizer config. ERROR: inconsistent confuguration .... fAmpThresholdLow and fAmpThresholdHigh cannot be ~equal if singleHit reco is selected"<<std::endl;
	}
    }
}

Double_t DigitizerChannelPVeto::CalcPedestal() {  //get NSamples from data card
  fPed = 0.;
  fNPedSamples = 0;
  
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

Double_t DigitizerChannelPVeto::ZSupHit(Float_t Thr, UShort_t NAvg) {
  Double_t rms1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;
  // come back to a veto setup 
  if(rms1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;   // signal to be removed
  }
  // Beth add the thr for noisy channels
  //std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  return ZSupHit;
}

Double_t DigitizerChannelPVeto::CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t iMax, UShort_t ped) {
  Int_t npeaks = 25;
  static TSpectrum SpectrumProcessor(npeaks);// = new TSpectrum(20);
  double Time   = 0.;
  fCharge = 0.;

  iMax=985;
  //currently looking for peaks with TSpectrum to obtain multi hit times
  static Double_t AbsSamRec[1024];
  static Double_t AbsSamRecDP[1024];

  //Check if they are needed or not and the real dimension
  Double_t  dxdt[1024];
  Double_t  SigCounts[1024];
  Double_t  SigmV[1024];

  for(UShort_t s=0;s<=iMax;s++){
    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+ped)/4096.*1000.; //in mV positivo
    //std::cout<< s << "     "  << fSamples[s]  <<" V "<< AbsSamRec[s]  <<std::endl;
  }
  for(UShort_t s=iMax+1;s<1024;s++){
    AbsSamRec[s] = 0;
  }

  hSig->SetContent(AbsSamRec);
  
  // check this number hard coded!!!!
  Int_t iDer=15;
  // compute derivative
  for(Int_t ll=0;ll<iMax+1;ll++){
    if(ll>=iDer){ 
      dxdt[ll]=(AbsSamRec[ll]-AbsSamRec[ll-iDer]);
    }else{
      dxdt[ll]=0;
    }
    htmp->SetBinContent(ll,dxdt[ll]);
    //    std::cout<<ll<<" sam "<<Temp[ll]<<" "<<Temp1[ll]<<" "<<dxdt[ll]<<std::endl;
  }

  Double_t dmax = htmp->GetMaximum();  //Max of the derivative
  Double_t VMax = hSig->GetMaximum();
  
  //  std::cout<<"Get Maximum     "<<VMax<<"   Get dxdt    "<<dmax<<std::endl;
  //if (VMax>fAmpThresholdHigh) std::cout<<VMax<<" VMax "<<std::endl;
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog){
    hVMax->Fill(VMax);
    hdxdtMax->Fill(dmax);
    hVmaxvsDxdtMax->Fill(VMax,dmax);
    hVmaxOvDxdt->Fill(VMax/dmax);
    hEnergy->Fill(dmax/20.); // what is this number??
  }
  std::vector<Float_t> xp;
  std::vector<Float_t> yp;

  std::vector<Float_t> x1p;
  std::vector<Float_t> y1p;

  //****************************************************
  //*  start searching for the paeaks in the RAW signals
  //****************************************************
  
  if(VMax>fAmpThresholdHigh){
    TSpectrum *s1 = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks
    Double_t peak_thr  = fAmpThresholdLow/VMax; //minimum peak height allowed. BUG??? 
    Int_t nfound = s1->Search(hSig,fPeakSearchWidth,"",peak_thr);     
    if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog) hNhitSig ->Fill(nfound);
    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      Double_t t1xp = (s1->GetPositionX())[ll];
      Double_t t1yp = (s1->GetPositionY())[ll];
      x1p.push_back(t1xp);
      y1p.push_back(t1yp);
      if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog) hSigV->Fill(t1yp);
    }
    

  //****************************************************
  //*  start searching for paeaks in the DERIVATIVE 
  //****************************************************
    Double_t MinDxDt=15.;  //be careful depends on how big is iDer
    Double_t PeakSearchDerWdt = 5.; //from
    TSpectrum *s = &SpectrumProcessor;
    Double_t Der_peak_thr  = 6/dmax;
    Int_t nfoundd = s->Search(htmp,PeakSearchDerWdt,"",Der_peak_thr);
    if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog){
      hNhitDx   ->Fill(nfoundd);
      hDeltaN   ->Fill(nfoundd-nfound);
      hDeltaNvsN->Fill(nfoundd,nfoundd-nfound);
    }
    for(Int_t ll=0;ll<nfoundd;ll++){ //peak loop per channel
      fCharge = 0.;
      Double_t txp = (s->GetPositionX())[ll];
      Double_t typ = (s->GetPositionY())[ll];
      xp.push_back(txp);
      yp.push_back(typ);
      if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog) hDxV->Fill(typ);

      Time=xp[ll]*fTimeBin;    // this is too naive can we do better?
      if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog){
	if(nfound==nfoundd) hDeltaT->Fill(xp[ll]-x1p[ll]);
	if(nfound==nfoundd) hDeltaV->Fill(yp[ll]-y1p[ll]);
      }
      //      std::cout<<"xp "<<xp[ll]<<" xp1 "<<x1p[ll]<<std::endl;
      //      Int_t bin = hSig->GetXaxis()->FindBin(xp);
      //      fAmpli = FindY(bin);
      fAmpli = hSig->GetBinContent(xp[ll]);  // sbagliato perche' c'e' l'offset con derivata
      //      fEnergy = fAmpli/fmVtoMeV;       // sbagliato perche' puo dipendere 
      fEnergy = yp[ll]/27.;       // sbagliato perche' puo dipendere dal canale
      //      if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog){
      //      hMHVMax->Fill(VMax);
      //      hMHdxdtMax->Fill(yp[ll]);
      //      hVmaxvsDxdtMax->Fill(VMax,dmax);
      //      hVmaxOvDxdt->Fill(VMax/dmax);
      //      hMHEnergy->Fill(yp[ll]/27.);
      // }
      TRecoVHit *Hit = new TRecoVHit();  
      Hit->SetTime(Time);
      //Hit->SetEnergy(fAmpli);
      Hit->SetEnergy(fEnergy);
      hitArray.push_back(Hit);
    }
    if( (nfoundd-nfound)>0 ){
//      hSig->Write();
//      htmp->Write();
    }
    //    delete s;
  }
  return Time;
}


Double_t DigitizerChannelPVeto::CalcChaTimeOrig(std::vector<TRecoVHit *> &hitArray,UShort_t iMax, UShort_t ped) {
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

  // H1->Reset();

  if (fUsePulseProcessing) {
    //    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,iMax-1 ,fTimeBin*1e-9);
    //    H1->SetContent(AbsSamRecDP);
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
  hOrVMax->Fill(VMax);
  //   std::cout<<"Get Maximum     "<<VMax<<"   Get Minimum    "<<VMin<<std::endl;
  
  //if (VMax>fAmpThresholdHigh) std::cout<<VMax<<" VMax "<<std::endl;

  //  fMinAmplitude
  if(VMax>fAmpThresholdHigh){
    TSpectrum *s = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks
    Double_t peak_thr  = fAmpThresholdLow /VMax; //minimum peak height allowed. 
    Int_t nfound = s->Search(H1,fPeakSearchWidth,"",peak_thr);     
    hNhitOrig ->Fill(nfound);
    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      fCharge = 0.;
      Double_t xp = (s->GetPositionX())[ll];
      Double_t yp = (s->GetPositionY())[ll];
      fAmpli=yp;
      Time=xp*fTimeBin;
      Int_t bin    = H1->GetXaxis()->FindBin(xp);

      hOrEnergy->Fill(fAmpli);
//      TRecoVHit *Hit = new TRecoVHit();  
//      Hit->SetTime(Time);
//      Hit->SetEnergy(fAmpli);
//      hitArray.push_back(Hit);
    }
    //    delete s;
  }

  Time = Time*fTimeBin;
  return Time;

}

void DigitizerChannelPVeto::PrepareDebugHistos(){
  fileOut    = new TFile("PVetoAn.root", "RECREATE");

  hListPVeto = new TList();  

  hListPVeto->Add(hNhitDx      = new TH1F("hNhitDx","hNhitDx",25,-0.5,24.5));
  hListPVeto->Add(hNhitSig     = new TH1F("hNhitSig","hNhitSig",25,-0.5,24.5));

  hListPVeto->Add(hDeltaN     = new TH1F("hDeltaN","hDeltaN",25,-12.5,12.5));
  hListPVeto->Add(hDeltaNvsN= new TH2F("hDeltaNvsN","hDeltaNvsN",25,-0.5,24.5,25,-12.5,12.5)); 

  hListPVeto->Add(hDeltaT     = new TH1F("hDeltaT","hDeltaT",25,-12.5,12.5));
  hListPVeto->Add(hDeltaV     = new TH1F("hDeltaV","hDeltaV",250,-125,125));
  //Single HIT
  hListPVeto->Add(hdxdtMax      = new TH1F("hdxdtMax","hdxdtMax",1000,0.,1000.));
  hListPVeto->Add(hVMax         = new TH1F("hVMax","hVMax",1000,0.,1000.));
  hListPVeto->Add(hVmaxvsDxdtMax= new TH2F("hVmaxvsDxdtMax","hVmaxvsDxdtMax",1000,0.,1000.,1000,0.,1000.)); 
  hListPVeto->Add(hVmaxOvDxdt   = new TH1F("hVmaxOvDxdt","hVmaxOvDxdt",500,0.,5.));
  hListPVeto->Add(hEnergy       = new TH1F("hEnergy","hEnergy",500,0.,5.));

  //Derivative hits
  hListPVeto->Add(hDxV = new TH1F("hDxV","hDxV",1000,0.,1000.));
  hListPVeto->Add(hDxE = new TH1F("hDxE","hDxE",500,0.,10.));

  //Signal hits
  hListPVeto->Add(hSigV = new TH1F("hSigV","hSigV",1000,0.,1000.));
  hListPVeto->Add(hSigE = new TH1F("hSigE","hSigE",500,0.,10.));

  //Time distributions of hit times
  hListPVeto->Add(hTHits = new TH1F("hTHits","hTHits",400,-300.,500.));

  //MULTI HIT
//  hListPVeto->Add(hMHdxdtMax      = new TH1F("hMHdxdtMax","hMHdxdtMax",1000,0.,1000.));
//  hListPVeto->Add(hMHVMax         = new TH1F("hMHVMax","hMHVMax",1000,0.,1000.));
//  hListPVeto->Add(hMHVmaxvsDxdtMax= new TH2F("hMHVmaxvsDxdtMax","hMHVmaxvsDxdtMax",1000,0.,1000.,1000,0.,1000.)); 
//  hListPVeto->Add(hMHVmaxOvDxdt   = new TH1F("hMHVmaxOvDxdt","hMHVmaxOvDxdt",500,0.,5.));
//  hListPVeto->Add(hMHEnergy       = new TH1F("hMHEnergy","hMHEnergy",500,0.,5.));
}

void DigitizerChannelPVeto::SaveDebugHistos(){
  fileOut->cd();
  if(fSaveAnalog) hListPVeto->Write(); //use it in monitor mode only  
  fileOut->Close();
}

//void DigitizerChannelPVeto::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
////  //  Double_t IsZeroSup = ZSupHit(5.,1000.);
////  //  CalcCharge(fIMax);
////  //if (fCharge < .01) return;
////  // come back to a Veto setup
//
////  if (fCharge < fChargeCut) return;
////  //  double time = CalcTime(fIMax);
////  // if (fTime < 20.) return;
////  
////  TRecoVHit *Hit = new TRecoVHit();
////  Hit->SetTime(time);
////  // M. Raggi going to charge
////  // Double_t fEnergy= fCharge*1000./15; //going from nC to MeV using 15pC/MeV
////  // Hit->SetEnergy(fCharge);
////  // come back to a Veto setup
////  Hit->SetEnergy(fCharge);
////  hitArray.push_back(Hit);
//  return;
////  // std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl;
//  
//}

//void DigitizerChannelPVeto::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){  //using TSpectrum
void DigitizerChannelPVeto::Reconstruct(std::vector<TRecoVHit *> &hitArray){  //using TSpectrum
  if( fUsePulseProcessing==0){ 
    Double_t IsZeroSup = ZSupHit(fZeroSuppression,1000.);  //usa the parameter signal window
    if(IsZeroSup) return;
    Double_t ped=CalcPedestal();  // takes the starting sample from data cards
    //std::cout<<"Pedestal    "<<ped<<std::endl;
    //ped=3650;
    CalcChaTime(hitArray,1000,ped);
    //  CalcChaTimeOrig(hitArray,1000,ped);
  }else{
    // Georgie pulse processing code goes here
    
  }
}


//void DigitizerChannelPVeto::Reconstruct(std::vector<TRecoVHit *> &hitArray){
//
////  if(fZeroSuppression > 1 ) {
////    if (ZSupHit(fZeroSuppression,1000.) < 0) return;   //execute zero suppression in channels
////  }
//
//  if(fMultihit) {
//    ReconstructMultiHit(hitArray);
//  } else {
//    //    CalcMaximum();
//    //    std::cout<<" fIMax =  "<<fIMax << " Amplitude   " << fPed - fMax <<std::endl;
//    CalcPedestal();
//    //    if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?
//    ReconstructSingleHit(hitArray);
//  }
//  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
//  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
//}



