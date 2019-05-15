// Written By M. Raggi 6/12/2018
#include "DigitizerChannelECal.hh"
#include "TMath.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TRandom.h"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

void DigitizerChannelECal::PrintConfig(){
  std::cout << "Hi I'm the ECal: " << std::endl;
  std::cout << "Signal width: "   << fSignalWidth << " samples" << std::endl;
  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl; 
  std::cout << "fUseOverSample: " << fUseOverSample << std::endl;  
//  Int_t NBD=8;
//  for(int i=0;i<32;i++){
//    std::cout<<"Ped "<<i<<" "<<fPedMap[std::make_pair(NBD,i)]<<" "<<std::endl;
//  }  

 if(fGlobalMode->GetGlobalDebugMode()!=0){
   std::cout<<"General Config Flags ECal Digi***************** "<<std::endl;
   std::cout<<"fIsPed          "<<fGlobalMode->IsPedestalMode()<< std::endl;
   std::cout<<"fIsReco         "<<fGlobalMode->IsRecoMode()<< std::endl;
   std::cout<<"fIsMonitor      "<<fGlobalMode->IsMonitorMode()<< std::endl;
   std::cout<<"fIsCosmic       "<<fGlobalMode->IsCosmicsMode()<< std::endl;
   std::cout<<"fIsDebug  ECAL  "<<fGlobalMode->GetGlobalDebugMode()<< std::endl;
  }
}

void DigitizerChannelECal::Init(GlobalRecoConfigOptions *gOptions,
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

  fMinAmplitude    = cfg->GetParOrDefault("RECO","MinAmplitude",10);
  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);

  fMultihit       = cfg->GetParOrDefault("RECO","Multihit",0);
  fUseAbsSignals  = cfg->GetParOrDefault("RECO","UseAbsSignals",0);
  fUseOverSample  = cfg->GetParOrDefault("RECO","UseOverSample",0); //M. Raggi: 03/05/2019  
  fIntCorrection = cfg->GetParOrDefault("RECO","UseIntegralCorrection",0); //M. Raggi: 15/05/2019  
 
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;

  SetAnalogOffSets(); //M. Raggi: 21/01/2019  read anaolg offsets values from files
  PrintConfig();
  PrepareTmpHistos();  //Temp histo servono anche in non debug mode
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    PrepareDebugHistos(); //debugging mode histos
  }
//else{
//    PrepareTmpHistos();  //Temp histo
//  }
}
void DigitizerChannelECal::PrepareTmpHistos(){
  hListTmp    = new TList();
  if(!fUseOverSample){
    hListTmp->Add(hdxdt   = new TH1F("hdxdt","hdxdt",1000,0.,1000.));
    hListTmp->Add(hSignal = new TH1F("hSignal","hSignal",1000,0.,1000.));
  }
  // over sampled histograms
  Int_t nbinx=4000;
  if(fUseOverSample){
    hListTmp->Add(hSigOv       = new TH1F("hSigOv","hSigOv",nbinx,0.,nbinx/4));
    hListTmp->Add(hSigOvSm     = new TH1F("hSigOvSm","hSigOvSm",nbinx,0.,nbinx/4));
    hListTmp->Add(hdxdtSigOvSm = new TH1F("hdxdtSigOvSm","hdxdtSigOvSm",nbinx,0.,nbinx/4));
  }
}

void DigitizerChannelECal::PrepareDebugHistos(){
  fileOut    = new TFile("ECalAn.root", "RECREATE");
  hListCal   = new TList();  
  hListEv    = new TList();  
  ECal = new TTree("ECAL","ECAL");

  ECal->Branch("ElCh",&ElCh);
  ECal->Branch("Raw",&Raw);
  ECal->Branch("Col",&Col);
  ECal->Branch("Zsup",&Zsup);
  ECal->Branch("Avg200",&fAvg200);
  ECal->Branch("HitE",&HitE);
  ECal->Branch("HitEHyb",&HitEHyb);
  ECal->Branch("Hit200E",&HitE200);
  ECal->Branch("HitT",&HitT);
  ECal->Branch("Trig",&fTrig); // 0 reco 1 ped 2 cosmic
//  ECal->Branch("ETotInner",&IEnner);
//  ECal->Branch("EInner",&HitEInner);

  hPedCalo = new TH1D*[32];
  hAvgCalo = new TH1D*[32];
  hPedMean = new TH1D*[32];
  hVMax    = new TH1D*[32];
  h200QCh  = new TH1D*[32]; //CT
  hQCh     = new TH1D*[32]; //CT

  hListTmp->Add(hSat    = new TH1F("hSat","hSat",1000,0.,1000.));
  //  hListTmp->Add(hDiff    = new TH1F("hDiff","hDiff",4000,0.,1000.));
  hListCal->Add(hTime= new TH1F("hTime","hTime",1000,0.,1000.));
  hListCal->Add(hTimeCut= new TH1F("hTimeCut","hTimeCut",1000,0.,1000.));
  hListCal->Add(hTimeOv= new TH1F("hTimeOv","hTimeOv",1000,0.,1000.));
  hListCal->Add(hdxdtMax= new TH1F("hdxdtMax","hdxdtMax",1600,-200.,3000.));
  hListCal->Add(hdxdtRMS= new TH1F("hdxdtRMS","hdxdtRMS",1000,0.,200.));
  
  for(int kk=0;kk<32;kk++){
    hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),1200,3300.,3900);
    hAvgCalo[kk] = new TH1D(Form("hAvgCalo%d",kk),Form("hAvgCalo%d",kk),1200,3300.,3900);
    hPedMean[kk] = new TH1D(Form("hSig%d",kk),Form("hSig%d",kk),1000,0.,1000.);
    hVMax[kk]    = new TH1D(Form("hVMax%d",kk),Form("hVMax%d",kk),1000,0.,1000.); // in mV
    h200QCh[kk]  = new TH1D(Form("h200QCh%d",kk),Form("h200QCh%d",kk),600,-200,400); //CT
    hQCh[kk]     = new TH1D(Form("hQCh%d",kk),Form("hQCh%d",kk),600,-200,400); //CT
    hListCal->Add(hPedCalo[kk]);
    hListCal->Add(hAvgCalo[kk]);
    hListCal->Add(hPedMean[kk]);
    hListCal->Add(hVMax[kk]);
    hListCal->Add(h200QCh[kk]); //CT
    hListCal->Add(hQCh[kk]); //CT
  }

}

void DigitizerChannelECal::SaveDebugHistos(){
  fileOut->cd();
  hListCal->Write(); //use it in monitor mode only  
  ECal->Write();
  // fileOut->Write();
  //  hListCal->ls();
  fileOut->Close();
}

void DigitizerChannelECal::SetAbsSignals(){
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fSamples[i] < 2048) {
      fSamples[i] = 4096 - fSamples[i];
    }
  }
}
 
// Aho pero' questo trova il minimo (il segnale e'a scendere).
// perche' i counts sono discendenti.
Short_t DigitizerChannelECal::CalcMaximum() {

  fMax = 32767; // 2^15 - 1
  for(UShort_t i = 0;i<fNSamples;i++){
    if (fMax > fSamples[i]) { 
      fMax = fSamples[i];
      fIMax = i;
    }
  }
  return fMax;
}

// Computes pedestals as average of 1000 samples; store in structures the average of the observed pedestals
Double_t DigitizerChannelECal::CalcPedestal() {
  Int_t Ch   = GetElChID();
  Int_t BID  = GetBdID();
  TempPedMap[std::make_pair(BID,Ch)]   = TempPedMap[std::make_pair(BID,Ch)] + TMath::Mean(1000,&fSamples[0]);
  TempMapCount[std::make_pair(BID,Ch)] = TempMapCount[std::make_pair(BID,Ch)]+1;
  fPedMap[std::make_pair(BID,Ch)] = TempPedMap[std::make_pair(BID,Ch)]/TempMapCount[std::make_pair(BID,Ch)];
  return fPed;
}

// Cleans the pedestals map structure
void DigitizerChannelECal::ResetPedestal() {
  std::cout<<"Cleaning Ecal pedestal map structure "<<std::endl;
  TempPedMap.clear();
  TempMapCount.clear();
  fPedMap.clear();
}

// Compute zero suppression: returns 1 if the events has to be suppressed
Double_t DigitizerChannelECal::ZSupHit(Float_t Thr, UShort_t NAvg) {
  fRMS1000  = TMath::RMS(NAvg,&fSamples[0]);
  Double_t ZSupHit=-1;
  //  std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  if(fRMS1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;
    //    std::cout<<"compute zero supp "<<rms1000<<" Zsup "<<ZSupHit<<std::endl;
  }
  return ZSupHit;
}

Double_t DigitizerChannelECal::CalcChargeSin(UShort_t iStart) {
  
  Short_t begin = iStart;  //to become iStart
  Short_t end   = 1000;

  Int_t Ch   = GetElChID();
  Int_t BID  = GetBdID();
  Double_t ChargeSin=0;
  Double_t Charge200=0;
  Double_t ChargeHyb=0;

  //  Int_t TrigType = GetTrigMask();
  //  if(TrigType!=0) return -1.;
  //  std::cout<<"Trig type "<<TrigType<<std::endl;
  fAvg200  = TMath::Mean(iStart,&fSamples[0]);
  fRMS200  = TMath::RMS(iStart,&fSamples[0]);
  Zsup  = fRMS1000;  
  for(Short_t s=0;s<end;s++){
    AbsSamRec[s]    = (Double_t) (-1.*fSamples[s]+fPedMap[std::make_pair(BID,Ch)])/4096*1000.; //in mV positivi
    AbsSamRec200[s] = (Double_t) (-1.*fSamples[s]+fAvg200)/4096*1000.; //in mV positivi using first Istart samples

    // Hybrid calculation just an attempt most probably useless
    if(fRMS200 < 5){ 
      AbsSamRecHyb[s] = (Double_t) (-1.*fSamples[s]+fAvg200)/4096*1000.; //in mV positivi
    }else{
      AbsSamRecHyb[s] = (Double_t) (-1.*fSamples[s]+fPedMap[std::make_pair(BID,Ch)])/4096*1000.; //in mV positivi     
    }
    //    std::cout<<s<<" Sample "<<AbsSamRec[s]<<std::endl;
    if(s>iStart && s<1000) ChargeSin += 1*AbsSamRec[s]   *1e-3/fImpedance*fTimeBin*1e-9/1E-12; 
    if(s>iStart && s<1000) Charge200 += 1*AbsSamRec200[s]*1e-3/fImpedance*fTimeBin*1e-9/1E-12; 
    if(s>iStart && s<1000) ChargeHyb += 1*AbsSamRecHyb[s]*1e-3/fImpedance*fTimeBin*1e-9/1E-12;     
  }
  
  if(fGlobalMode->GetGlobalDebugMode()!=0){
    //filling NTU Variables
    HitE    = ChargeSin/15.;
    HitE200 = Charge200/15.;
    HitEHyb = ChargeHyb/15.;
    Int_t code=BID*100+Ch;
    ElCh=code;
    Raw=GetChID()%100;
    Col=GetChID()/100;
    //    ECal->Fill();
  }
  if(BID==14 && fGlobalMode->GetGlobalDebugMode()!=0){
    char name[50];
    //     std::cout<<Ch<<" "<<BID<<" Charge "<<ChargeSin<<std::endl;
    sprintf(name,"hPedCalo%d",Ch);
    // std::cout<<Ch<<" "<<BID<<" Charge "<<name<<std::endl;
    histo =(TH1D*) hListCal->FindObject(name);
    histo->Fill(fAvg200);

    sprintf(name,"hAvgCalo%d",Ch);
    histo =(TH1D*) hListCal->FindObject(name);
    double pedAvg = fPedMap[std::make_pair(BID,Ch)];
    histo->Fill(pedAvg);
    
    sprintf(name,"hQCh%d",Ch);
    histo =(TH1D*) hListCal->FindObject(name);
    histo->Fill(ChargeSin/15.);

    sprintf(name,"h200QCh%d",Ch);
    histo =(TH1D*) hListCal->FindObject(name);
    histo->Fill(Charge200/15.);
  }

//  if(ChargeSin/15.<-15) {
//    if(fIsGlobalDebug){
//      Int_t code=BID*100+Ch;
//      H1 = new TH1D(Form("hV%d",code),Form("hV%d",code),end,0.,end);
//      H1->SetContent(AbsSamRec);
//      //     H2 = new TH1D(Form("h2V%d",m),Form("h2V%d",m),end,0.,end);
//      //      H2->SetContent(AbsSamRec200);
//      //  
//      fileOut->cd();
//      H1->Write();
//      //      H2->Write();
//      //      H1->Reset();
//      //      H2->Reset();
//      //      m++;
//    }
//  }
  fCharge=Charge200;
  return fCharge;
}

Double_t DigitizerChannelECal::CalcCharge(UShort_t iMax) {
  
  Short_t begin = iMax-fPreSamples > 0? iMax-fPreSamples:0;
  Short_t end   = iMax+fPostSamples < fNSamples? iMax+fPostSamples:fNSamples;
 
  //  std::cout << "Begin: "<< begin << "  end: " << end << std::endl;
  fCharge=0.;
  for(Short_t i = begin;i<end;++i) {
    fCharge+=1.* fSamples[i];
  }
  
//  std::cout << "Integral: " << fCharge << "  Ped:  " << fPed << " Base: " << (end-begin) * fPed << std::endl;
//  std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
//  std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;

  fCharge = ((1.*end-1.*begin) * fPed) - fCharge;  //subtract events pedestral
  fCharge *= (fVoltageBin*fTimeBin/fImpedance);    // charge in nC
  return fCharge;
}

// try using oversampled histograms to improve time resolution
Double_t DigitizerChannelECal::CalcTimeOver(UShort_t iDer) {
  Double_t dxdt[4096];
  Double_t Over4[4096];
  Double_t Temp[4096];
  Double_t Temp1[4096];
  Int_t MaxSam = 4000;

  // oversample the signal by factor 4 with linear interpolation
  //  if(fGlobalMode->GetGlobalDebugMode()){
  histo   = (TH1D*)  hListTmp->FindObject("hdxdtSigOvSm");
  histo1  = (TH1D*)  hListTmp->FindObject("hSigOvSm");
  histoOv = (TH1D*)  hListTmp->FindObject("hSigOv");
  OverSample4(fSamples,Over4,1001);    
  for(Int_t kk=0;kk<=MaxSam;kk++){
    histoOv->SetBinContent(kk,Over4[kk]);
  }
    //  }
  
  Int_t npeaks=4;
  Int_t nsmooth=5*4;
  // Smooth the signal by averaging nsmooth samples 
  for(Int_t ll=0;ll<=MaxSam-nsmooth/2;ll++){
    if(ll>nsmooth/2){
    Temp[ll] =TMath::Mean(nsmooth,&Over4[ll-nsmooth/2]); // averaging over ±nsmooth/2 samples on the oversampled histogram 
    Temp1[ll]=(-1.*Temp[ll]+fAvg200)/4096*1000.;  // transform in postive mV using first Istart samples for pedestal
    }else{
      Temp[ll]=0;
      Temp1[ll]=0;
    }
    histo1->SetBinContent(ll,Temp1[ll]);
    // compute raw derivative subracting samples
    if(ll>iDer+nsmooth/2 && ll<2200){ 
      dxdt[ll]=-(Temp[ll]-Temp[ll-iDer]);
    }else{
      dxdt[ll]=0;
    }
    histo->SetBinContent(ll,dxdt[ll]);
    //  std::cout<<ll<<" sam "<<Temp[ll]<<" "<<Temp1[ll]<<" "<<dxdt[ll]<<std::endl;
  }
  Int_t Ch   = GetElChID();
  Int_t BID  = GetBdID();

  Int_t MaxBin = histo->GetMaximumBin();
  Int_t Max    = histo->GetMaximum();
  fTimeOv = (Double_t)MaxBin/4*fTimeBin;   //convert the time back in ns

  if(fGlobalMode->GetGlobalDebugMode()){  
    hdxdtMax->Fill(Max);
    hdxdtRMS->Fill(TMath::RMS(1000,&dxdt[0]));
    if(Max>200) hTimeCut->Fill(fTimeOv);
    hTimeOv->Fill(fTimeOv);
  }

  //  std::cout<<" MaxBin "<<MaxBin<<" Max "<<Max<<" "<<fTimeOv<<" "<<std::endl;


  if(fGlobalMode->GetGlobalDebugMode()){
    Double_t rnd=((double) rand() / (RAND_MAX));
    if(rnd<0.02){ 
      //      hListEv->Write();
      hListTmp->Write();
    }
  }
  //  Int_t TOver4=GetStartTime(Over4,150);
  histo->Reset();
  histo1->Reset();
  histoOv->Reset();
  return fTimeOv;
}

// first approximation timing algorithm to be optimized
Double_t DigitizerChannelECal::CalcTimeSing(UShort_t iDer) {
  Int_t ll;
  Double_t dxdt[1001];
  Double_t Temp[1001];
  Double_t Temp1[1001];
  Double_t Over4[4096];

  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    histo   = (TH1D*)  hListTmp->FindObject("hdxdt");
    histo1  = (TH1D*)  hListTmp->FindObject("hSignal");
  }else{
    histo   = (TH1D*)  hListTmp->FindObject("hdxdt");
    histo1  = (TH1D*)  hListTmp->FindObject("hSignal");
  }
  Int_t npeaks=4;
  Int_t nsmooth=5;
  // Smooth the signal by averaging nsmooth samples 
  for(ll>0;ll<1001;ll++){
    if(ll>nsmooth/2){
    Temp[ll] =TMath::Mean(nsmooth,&fSamples[ll-nsmooth/2]); // averaging over ±nsmooth/2 samples 
    Temp1[ll]=(-1.*Temp[ll]+fAvg200)/4096*1000.;  // transform in postive mV using first Istart samples for pedestal
    }else{
      Temp[ll]=0;
      Temp1[ll]=0;
    }
    histo1->SetBinContent(ll,Temp1[ll]);
    // compute raw derivative subracting samples
       if(ll>iDer+nsmooth/2 && ll<550){ 
      dxdt[ll]=-(Temp[ll]-Temp[ll-iDer]);
    }else{
      dxdt[ll]=0;
    }
    histo->SetBinContent(ll,dxdt[ll]);
    //    std::cout<<ll<<" sam "<<Temp[ll]<<" "<<Temp1[ll]<<" "<<dxdt[ll]<<std::endl;
  }
  Int_t Ch   = GetElChID();
  Int_t BID  = GetBdID();

  Int_t MaxBin = histo->GetMaximumBin();
  Int_t Max    = histo->GetMaximum();
  fTimeSin = (Double_t)MaxBin*fTimeBin; 

  //  if(Max>30) 
  //  std::cout<<Ch<<" "<<BID<<" Max "<<Max<<" M bin "<<MaxBin<<" ftime "<<fTimeSin<<" "<<fCharge/15.<<std::endl;

//  //// TSPECTRUM search is too time consuming.
 // double XBig=10000.;
 // fTimeSin=XBig;
 // if(Max>100){
 //   TSpectrum *s = new TSpectrum(npeaks);
 //   //  Double_t peak_thr  = fAmpThresholdLow/Max;   //minimum peak height allowed.
 //   Int_t nfound = s->Search(histo,6,"",0.3);   //corrected for 2.5GHz cannot be less then 0.05
 //   // std::cout<<"found Npeaks "<<nfound<<""<<std::endl;
 //   // ROOT 6 version
 //   //    Double_t *xpeaks = s->GetPositionX();
 //   //    Double_t *ypeaks = s->GetPositionY();
 //   // ROOT 5 version
 //   Float_t *xpeaks = s->GetPositionX();
 //   Float_t *ypeaks = s->GetPositionY();
 //   //    std::cout<<"found Npeaks "<<nfound<<""<<std::endl;
 //   for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
 //     // ROOT 6 version
 //     //      Double_t xp   = xpeaks[ll];
 //     //      Double_t yp   = ypeaks[ll];
 //     // ROOT 5 version
 //     Float_t xp   = xpeaks[ll];
 //     Float_t yp   = ypeaks[ll];
 //     if(xp<fTimeSin) fTimeSin = xp*fTimeBin; //convert time in ns get it from data
 //     //    fTimeSin = Max*fTimeBin; //convert time in ns get it from data
 //   }
 // }

  if(fGlobalMode->GetGlobalDebugMode()){  
    hdxdtMax->Fill(Max);
    hdxdtRMS->Fill(TMath::RMS(1000,&dxdt[0]));
    if(Max>100) hTimeCut->Fill(fTimeSin);
    hTime->Fill(fTimeSin);
  }
 
  if(fGlobalMode->GetGlobalDebugMode()){
    Double_t rnd=((double) rand() / (RAND_MAX));
    if(rnd<0.02){ 
      //      hListEv ->Write();
      hListTmp->Write();
    }
  }


//  hTime->Fill(fTimeSin);
  //  histo->Fit("gaus");
  // std::cout<<"fTime "<<fTimeSin<<std::endl;
  //  hTime->Write();
 
  histo->Reset();
  histo1->Reset();
  return fTimeSin;
}

Double_t DigitizerChannelECal::CalcTime(UShort_t iMax) {
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

void DigitizerChannelECal::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  Double_t IsZeroSup = ZSupHit(5.,1000.);
  IsSaturated(); //check if the event is saturated M. Raggi 03/2019
  if(IsZeroSup==1 && !fGlobalMode->IsPedestalMode()) return; //perform zero suppression unless you are doing pedestals
  fTrig = GetTrigMask();
  if(GetTrigMask()!=2) CalcChargeSin(250);  //Physics in ECAL starts ~250 ns
  if(GetTrigMask()==2) CalcChargeSin(40);   //Cosmics in ECal start  ~40 ns
  
  // M. Raggi going to energy with Nominal Calibration
  Double_t fEnergy= fCharge/15.; //going from pC to MeV using 15pC/MeV
  if (fEnergy < 1.) return; //cut at 1 MeV nominal
  if(fUseOverSample){
    //    std::cout<<" over sampled "<<std::endl;
    HitT = CalcTimeOver(40);
  }else{
    //    std::cout<<" NON over sampled "<<std::endl;
    HitT = CalcTimeSing(10);
  } 
  if(fIntCorrection){ 
    Double_t QIntCorr = CorrectIntegrationTime(HitT,1000.);
    //    std::cout << "Hit charge:  " << HitE200 << "  Time: " << fTime << std::endl; 
    HitE200 /= QIntCorr; //correct for non integrated charge
    //    std::cout << "Hit charge:  " << HitE200 << "  Time: " << fTime << std::endl; 
  }
  //Filling hit structure
  TRecoVHit *Hit = new TRecoVHit();
  //  Hit->SetTime(fTimeSin);
  Hit->SetTime(HitT);
  //  Hit->SetEnergy(fEnergy);
  Hit->SetEnergy(HitE200);
  hitArray.push_back(Hit);
  if(fGlobalMode->GetGlobalDebugMode()) ECal->Fill();
  //  std::cout << "Hit charge:  " << fCharge << "  Time: " << fTime << std::endl; 
}


//M. Raggi: 21/01/2018 
//reads board pedestal from file in config/BDped 
void DigitizerChannelECal::SetAnalogOffSets(){
  std::ifstream PrevPed; 
  char fname[50];
  // Reading first half of the pedestals
  for(Int_t NBD=0;NBD<10;NBD++){
    //    sprintf(fname,"config/BDPed/PedBD%d.ped",NBD);
    sprintf(fname,"config/BDPed/Ped%d.txt",NBD);
    PrevPed.open(fname);
    if(PrevPed.is_open()){
      double temp;
      for(int i=0;i<32;i++){
	PrevPed >> temp >> fPedCh[i] >> temp>> temp;//reads channel number and discards it
	fPedMap[std::make_pair(NBD,i)] = fPedCh[i];
      }
    PrevPed.close();
    }
    else{ 
      std::cout << "No previous data available for board "<<NBD<<" resorting to default pedestal (3800)"<<std::endl;
    } 
  }

  // reading second half of the pedestals
  for(Int_t NBD=14;NBD<24;NBD++){
    sprintf(fname,"config/BDPed/PedBD%d.ped",NBD);
    //sprintf(fname,"config/BDPed/Ped%d.txt",NBD);
    PrevPed.open(fname);
    if(PrevPed.is_open()){
      double temp;
      for(int i=0;i<32;i++){
	//      PrevPed >> temp >> fPedCh[i] >> temp>> temp >> fCalibCh[i];//reads channel number and discards it
	PrevPed >> temp >> fPedCh[i] >> temp>> temp;//reads channel number and discards it
	//	std::cout<<NBD<<" PEDCH "<<i<<" "<<fPedCh[i]<<std::endl;      
	fPedMap[std::make_pair(NBD,i)] = fPedCh[i];
      }
    PrevPed.close();
    }
    else{ 
      std::cout << "No previous data available for board "<<NBD<<" resorting to default pedestal (3800)"<<std::endl;
    } 
  }

}

void DigitizerChannelECal::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  std::cout<<"Not yet implemented "<<std::endl;   
}

void DigitizerChannelECal::Reconstruct(std::vector<TRecoVHit *> &hitArray){
  fTrigMask = GetTrigMask();
  if(fUseAbsSignals) {
    SetAbsSignals();
  }
  CalcMaximum();
  if(fGlobalMode->IsPedestalMode()) CalcPedestal();
  //  if(GetTrigMask()==0) CalcPedestal();
  // if(fPed - fMax < fMinAmplitude ) return; //altro taglio da capire fatto in che unita'? conteggi?

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  // std::cout << "Pedestal: " << fPed << "  from: " << fNPedSamples << " samples"<< std::endl;
}

DigitizerChannelECal::~DigitizerChannelECal(){
  if(fGlobalMode->IsPedestalMode()) std::cout<<"Saving ECal BD pedestals "<<std::endl;
  for(Int_t KK=0;KK<10;KK++){
    if(fTrigMask==128 && fGlobalMode->IsPedestalMode()) SaveBDPed(KK);  //save only if pedestal run type is selected
  }
  
  for(Int_t KK=14;KK<24;KK++){
    if(fTrigMask==128 && fGlobalMode->IsPedestalMode()) SaveBDPed(KK); //save only if pedestal run type is selected
  }
  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode()){
    SaveDebugHistos();
  }
}

void DigitizerChannelECal::SaveBDPed(Int_t BID){
  TString fName;
  fName = Form("config/BDPed/Ped%d.txt",BID);
  std::cout<<"Saving Ecal Pedestal "<<fName<<std::endl;
  std::ofstream cout(fName);
  for(Int_t Ch=0;Ch<32;Ch++){
    cout<<Ch<<" "<<fPedMap[std::make_pair(BID,Ch)]<<std::endl;
  }
  cout.close();
}

// M. Raggi 15/05/2019
// Provides correct amount of deposited charge correcting for shorter integration 
// window in the digitizer due to different hit arrival time. 
// Be carefull if the arrival time is wrong you will get worst charge determination 
Double_t DigitizerChannelECal::CorrectIntegrationTime(Double_t TStart,Double_t TStop){
  Double_t CorrectedCharge=0;
  Double_t IntWindowEnd=1000.; //ns 
  Double_t IntWindowWdt=0.;    //ns 
  Double_t tau=300.;    //BGO decay time in ns need tuning
  IntWindowWdt=IntWindowEnd-TStart;   // compute the effective integration window
  //  Double_t Correction =( (tau)*exp(-0./tau)-(tau)*exp(-IntWindowWdt/tau) )/tau;
  Double_t Correction = exp(-0./tau)-exp(-IntWindowWdt/tau);
  CorrectedCharge=fCharge/Correction;
  //  std::cout<<"Hit Energy "<<fCharge<<" Hit Time "<<TStart<<" TWind "<<IntWindowWdt<<std::endl;
  //  std::cout<<"All time   "<<(tau)*exp(-0./tau)-(tau)*exp(-3000/tau)<<" fraction "<<Correction<<" QCorr "<<CorrectedCharge<<std::endl;
  //  std::cout<<" fraction "<<Correction<<" QCorr "<<CorrectedCharge<<std::endl;
  return Correction;
}

// Increase the number of samples to 4Gs interpolating. M. Raggi preliminary needs tests 04/2019
//void DigitizerChannelECal::OverSample4(Double_t* v, Double_t* o, Int_t n) {
void DigitizerChannelECal::OverSample4(Short_t* v, Double_t* o, Int_t n) {
  int j,k;
  double v1[2048];
  for (j=0;j<n;j++)
  {
     v1[2*j]=v[j];
     v1[2*j+1]=(v[j]+v[j+1])/2.;
     // std::cout<<"Azz "<<j<<" "<<v[j]<<" "<<v1[j]<<std::endl;
  }
  v1[2*n]=v[n];
  for (j=0;j<2*n;j++){
     o[2*j]=v1[j];
     o[2*j+1]=(v1[j]+v1[j+1])/2.;
//     //     std::cout<<j<<" "<<v1[j]<<std::endl;
  }
  o[4*n]=v1[2*n];
  for(k=0;k<4*n;k++){
    //std::cout<<k<<" "<<v[k/4]<<" "<<o[k]<<std::endl;
  }
}

// Trying to implement zero crossing. M. Raggi preliminary needs tests 04/2019
Int_t DigitizerChannelECal::GetStartTime(Double_t* v, Int_t nshift) {
  //  Int_t nshift = 6;
  Int_t bins=950*4;
  histoDiff = (TH1D*) hListTmp->FindObject("hDiff");
  Double_t orig[4*1024];
  Double_t shift[4*1024];
  Double_t diff[4*1024];
  nshift=10;
  for (Int_t i=0;i<bins-nshift;i++){ 
    shift[i]=-1.;
  }
//  for (Int_t i=0;i<bins-nshift;i++) {
//    orig[i] = v[i];
//    shift[i+nshift] = 0.35*orig[i];
//  }

  for (Int_t i=nshift;i<bins;i++) {
    orig[i] = v[i];
    shift[i-nshift] = 0.35*orig[i];
  }

  for (Int_t i=0;i<bins-nshift;i++) {
    diff[i] = orig[i]-shift[i];
    //    std::cout<<i<<" "<<orig[i]<<" "<<shift[i]<<" "<<diff[i]<<" "<<std::endl;
    histoDiff->SetBinContent(i,diff[i]);
  }
  
  for (Int_t i=0;i<bins;i++) {
    Double_t diff = v[i];
    if (diff<0.) return i;
  }
  return 16384;
}

// checks if the signal is saturated or if there are strange samples
Bool_t DigitizerChannelECal::IsSaturated(){
  Bool_t IsSaturated=false;
  Short_t min  = TMath::MinElement(1000,&fSamples[0]); 
  Short_t max  = TMath::MaxElement(1000,&fSamples[0]); 
  Short_t nsat = 0;
  Int_t Ch     = GetElChID();
  Int_t BID    = GetBdID();

  //  if(min < 5 || max>5050){ 
  if(min < 5){ 
    IsSaturated=true;
    if(fGlobalMode->GetGlobalDebugMode()) histoSat = (TH1D*) hListTmp->FindObject("hSat"); // swt the debug flag.
    
    //    std::cout<<"saturated!! "<<min<<" BID "<<BID<<" CH "<<Ch<<"fPed "<<fPedMap[std::make_pair(BID,Ch)]<<std::endl;
    for(int ll=0;ll<1001;ll++){
      if(fSamples[ll]<5) nsat++;
      if(fGlobalMode->GetGlobalDebugMode()) histoSat->SetBinContent(ll,fSamples[ll]);
    }
    //    std::cout<<"saturated!! "<<min<<" BID "<<BID<<" CH "<<Ch<<" nsat "<<nsat<<std::endl;
    //    if(fIsGlobalDebug) histoSat->Write();
    if(fGlobalMode->GetGlobalDebugMode()) histoSat->Reset();
  }
  return IsSaturated;
};

