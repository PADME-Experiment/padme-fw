#include "DigitizerChannelETag.hh"
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

void DigitizerChannelETag::PrintConfig(){
  //  std::cout << "Signal width: " << fSignalWidth << " samples" << std::endl;
  //  std::cout << "fUseAbsSignals: " << fUseAbsSignals << std::endl;  
}

DigitizerChannelETag::~DigitizerChannelETag(){
  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode() || fSaveAnalog){
    SaveDebugHistos();
  }
}

void DigitizerChannelETag::Init(GlobalRecoConfigOptions *gMode, PadmeVRecoConfig *cfg){
  detectorname=cfg->GetName();
  fGlobalMode = gMode;
  fAnalogsPrinted=0;
  std::string name;
  name = cfg->GetParOrDefault("DETECTOR","NAME","DIGI");
  name += "DIGI";

  hSig    = new TH1D("hSig","hSig",fIMax,0.,fIMax);

  fTimeBin        = cfg->GetParOrDefault("ADC","TimeBin",1.);
  fPedMaxNSamples = cfg->GetParOrDefault("RECO","NumberOfSamplesPedestal",100);  

  fAmpThresholdLow = cfg->GetParOrDefault("RECO","AmplThrLow",10.);
  fAmpThresholdHigh= cfg->GetParOrDefault("RECO","AmplThrHigh",20.);

  fSaveAnalog = cfg->GetParOrDefault("Output","Analog",0); //M. Raggi: 03/03/2021  
  fTotalAnalogs = cfg->GetParOrDefault("Output","TotalAnalogs",10); //Beth 23/2/22: total number of analog signals to write to ETagRecoAn.root

  fChannelEqualisation = cfg->GetParOrDefault("RECO","ChannelEqualisation",1);
  fTailCorrection      = cfg->GetParOrDefault("RECO","TailCorrection",0);

  fUsePulseProcessing  = cfg->GetParOrDefault("RECO","UsePulseProcessing",1);
  fDerivPoints         = cfg->GetParOrDefault("RECO","DerivPoints",15);
  fPeakSearchWidth     = cfg->GetParOrDefault("RECO","PeakSearchWidth",5);
  fZeroSuppression     = cfg->GetParOrDefault("RECO","ZeroSuppression",5.);
  fDerivAmpToEnergy    = cfg->GetParOrDefault("RECO","DerivAmpToEnergy",0.046);
  fmVtoMeV             = cfg->GetParOrDefault("RECO","ConversionFactor",29.52);
  
  std::cout << cfg->GetName() << "*******************************" <<  std::endl; 
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  std::cout << cfg->GetName() << "***I'M THE NEW ETAG DIGITIZER*" <<  std::endl;
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;  
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;  
  PrintConfig();

  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog){
    PrepareDebugHistos(); //debugging mode histos
  }

}

Double_t DigitizerChannelETag::CalcPedestal() {  //get NSamples from data card
  fPed = 0.;
  fNPedSamples = 0;
  // average of first fPedMaxNSamples
  for(Short_t i = 0 ; i  <   fPedMaxNSamples; i++) {
    fNPedSamples ++;
    fPed+= fSamples[i];
  }
  //std::cout << " fNPedSamples " << fNPedSamples <<" fPed Veto"<< std::endl;
  fPed /= fNPedSamples;//fNPedSamples is probably useless, just use fPedMaxSamples
  //std::cout <<  fPed << " fNPedSamples " << fNPedSamples << std::endl;
  return fPed;
}

Double_t DigitizerChannelETag::ZSupHit(Float_t Thr, UShort_t NAvg) {// NAvg = 1000, Thr by default is fZeroSuppression = 5. This parameter isn't in the config file, probably it should be?
  fRMS1000 = TMath::RMS(NAvg,&fSamples[0]);
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog){
    auto it = std::find(fListOfChannels.begin(), fListOfChannels.end(), GetChID());
    auto idx = it - fListOfChannels.begin();
    hNoiseRMSAvg->AddBinContent(idx+1,fRMS1000);
    hNoiseRMSPerChannel[GetChID()]->Fill(fRMS1000);
    hNZSupEvents->AddBinContent(idx+1,1);
  }

  Double_t ZSupHit=-1;
  // come back to a veto setup 
  if(fRMS1000>Thr){
    ZSupHit=0;
  }else{
    ZSupHit=1;   // signal to be removed
  }
  
// Beth add the thr for noisy channels
  return ZSupHit;
}

Double_t DigitizerChannelETag::CalcChaTime(std::vector<TRecoVHit *> &hitVec){//copied and modified from 211019-padmefw, Beth 18/2/22. 22/2/22 Made it independent of pedestal and iMax

  RawGetMax=0;
  DerivGetMax=0;
  
  int nfound=0;
  Int_t npeaks = 20;//maximum possible number of peaks
  static TSpectrum SpectrumProcessor(npeaks);
  double Time   = 0.;
  
  //currently looking for peaks with TSpectrum to obtain multi hit times
  //Int_t npeaks =25;
  static Double_t AbsSamRec[1024];
  static Double_t AbsSamRecDeriv[1024];
  static Double_t SamRecDeriv[1024];//not set to 0

  for(UShort_t s=0;s<fIMax;s++){
    AbsSamRec[s] = (Double_t) (fEqualSamples[s]);
    if(s>=fDerivPoints)    SamRecDeriv[s]=(AbsSamRec[s]-AbsSamRec[s-fDerivPoints]);
    else if(s<fDerivPoints) SamRecDeriv[s]=0;
    if(SamRecDeriv[s]<=0) AbsSamRecDeriv[s] = 0;//to prevent TSpectrum being confused by negative values, set negative values of the derivative to 0
    else if (SamRecDeriv[s]>0)
      AbsSamRecDeriv[s] = SamRecDeriv[s];
  }

  HTSpec->Reset();
  
  HTSpec->SetContent(AbsSamRecDeriv);

  Double_t VMax = HTSpec->GetMaximum();//check: should the reconstruction amp threshold be dependent on deriv amp or raw amp?
  //  Double_t VMin = HTSpec->GetMinimum(); //Beth 17/2/22: Not used

  TSpectrum *spec;
  TRecoVHit *Hit;
 
  Double_t xp;
  Double_t yp;
  Double_t rawAmp;

  //fampthresholds, fpeakthr should all go in the config file and then not be touched. fderivthr should probably be deleted and become fpeakthr

  /*if(GetChID()==58||GetChID()==60){//this goes in ZSup
    fAmpThresholdHigh=12;
    fAmpThresholdLow=12;
    }*/
  //  else{
    fAmpThresholdHigh=6;
    fAmpThresholdLow=6;
    //}
      tDerivHitVec.clear();
      vRawHitVec.clear();
      vTSpecYPHitVec.clear();

  if(VMax>fAmpThresholdHigh){
    spec = &SpectrumProcessor;
    Double_t peak_thr;
  
    double derivthr = fAmpThresholdLow/2.11;//2.11 is the ratio of amplitudes between raw signal and derivative. Check this, becuase it's a function of the number of samples used to take the derivative
    peak_thr = derivthr/VMax;   
    nfound = spec->Search(HTSpec,fPeakSearchWidth,"",peak_thr);     
    fAnalogPrint=0;
    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      //      fCharge = 0.;
      xp = (spec->GetPositionX())[ll];
      yp = (spec->GetPositionY())[ll];
      rawAmp = AbsSamRec[int(xp+0.5)];
      vRawHitVec.push_back(rawAmp);
      vTSpecYPHitVec.push_back(yp);
      Time=xp*fTimeBin;
      tDerivHitVec.push_back(Time);
      if(nfound == 1){
	RawRise = rawAmp;
	RawGetMax=*std::max_element(AbsSamRec, AbsSamRec + fIMax);
	DerivGetMax=*std::max_element(AbsSamRecDeriv, AbsSamRecDeriv + fIMax);
      }
      //if(Time<50||Time>350) 
      fAnalogPrint=0; // enable to produce waveforms
    }//end of nfound loop
  }//closes if(VMax>thr)

  for (auto ch : fListOfChannels) fNFoundPerChannel[ch] = 0;

  fNFoundPerChannel[GetChID()]=nfound;

  std::vector<int> index(nfound, 0);

  for (UShort_t i = 0 ; i != index.size() ; i++) {
    index[i] = i;
  }
  
  sort(index.begin(), index.end(),
       [&](const int& a, const int& b) {
	 return (tDerivHitVec[a] < tDerivHitVec[b]);
       }
       );

  if((index.size()!=nfound)){
    std::cout<<"CONFUSION!"<<std::endl;
    return -100;
  }
  
  //  Double_t hitV;//includes tail correction
  double tailfraction=0;
  double DeltaTSortSamples=0;
 
  for (UShort_t ii = 0 ; ii != index.size() ; ii++) {
    tDerivSortHitVec.push_back(tDerivHitVec[index[ii]]);
    if(ii>0&&tDerivSortHitVec[ii]<tDerivSortHitVec[ii-1]){ //check that the time sorting has worked correctly
      std::cout<<"SORTING ISN'T WORKING"<<std::endl;
      return -100;
    }

    vRawSortHitVec.push_back(vRawHitVec[index[ii]]);
    vTSpecYPSortHitVec.push_back(vTSpecYPHitVec[index[ii]]);

    if (ii==0) DeltaTSortSamples=tDerivSortHitVec[ii]/fTimeBin;
    else DeltaTSortSamples=tDerivSortHitVec[ii]/fTimeBin-tDerivSortHitVec[ii-1]/fTimeBin;
    if(fTailCorrection) tailfraction = TailHeightDerivative(DeltaTSortSamples);

    if(ii==0)    vTSpecYPCorrectHitVec.push_back(vTSpecYPSortHitVec[ii]);
    else    vTSpecYPCorrectHitVec.push_back(vTSpecYPSortHitVec[ii]-vTSpecYPCorrectHitVec[ii-1]*tailfraction); //for all hits after the first, apply the tail correction

    Hit = new TRecoVHit();
  
    Hit->SetTime(tDerivSortHitVec[ii]);

    fEnergy=vTSpecYPCorrectHitVec[ii]*fDerivAmpToEnergy;
    Hit->SetEnergy(fEnergy);

    hitVec.push_back(Hit);

  }//end loop over hits

  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode() || fSaveAnalog)  HitPlots(hitVec);
  
  return Time;
}

void DigitizerChannelETag::PrepareDebugHistos(){ //Beth 20/10/21 copied from 190917padme-fw, in turn copied from Mauro's DigitizerChannelECal structure to create a set of debug histograms that are produced only in debug mode
  TString fileoutname;

  if(detectorname=="ETag") fileoutname="ETagRecoAnDeriv.root";
  else if(detectorname=="ETag") fileoutname="ETagRecoAnDeriv.root";

  fileOut    = new TFile(fileoutname, "RECREATE");
  
  hNoEventsReconstructed     = new TH1F("NoEventsReconstructed","NoEventsReconstructed",4,0,2);//number of hits reconstructed by TSpectrum on derivatives    
  hOccupancy                 = new TH1F("hOccupancy","hOccupancyAvg",120,0,120);
  hOccupancy2D 		     = new TH2F("hOccupancy2D", "hOccupancy2D", 2, 0, 2, 4*15, 0, 4*15);
  hOccupancyOneHit           = new TH1F("hOccupancyOneHit","hOccupancyOneHitAvg",120,0,120);
  hNoHitsDeriv               = new TH1F("NoHitsDeriv","NoHitsDeriv",20,0,20);//number of hits reconstructed by TSpectrum on derivatives    
  hRawV                      = new TH1F("RawV","RawV",400,0,400);
  hRawVCorrect               = new TH1F("RawVCorrect","RawVCorrect",400,0,400);
  hRawVCorrectChannels20to70 = new TH1F("RawVCorrectChannels20to70","RawVCorrectChannels20to70",400,0,400);
  hRawVOneHit                = new TH1F("RawVOneHit","RawVOneHit",400,0,400);
  hRawVMultiHit              = new TH1F("RawVMultiHit","RawVMultiHit",400,0,400);
  hRawVMultiHitCorrect       = new TH1F("RawVMultiHitCorrect","RawVMultiHitCorrect",400,0,400);
  hDerivV                    = new TH1F("DerivV","DerivV",100,0,200);
  hDerivVOneHit              = new TH1F("DerivVOneHit","DerivVOneHit",100,0,200);
  hDerivVCorrect             = new TH1F("DerivVCorrect","DerivVCorrect",100,0,200);
  hHitTime                   = new TH1F("HitTime","HitTime",400,0,800);
  hHitEnergy                 = new TH1F("HitEnergy","HitEnergy",100,0,10);
  hHitEnergySingleHit        = new TH1F("HitEnergySingleHit","HitEnergySingleHit",100,0,10);
  hMinTimeDiffDeriv          = new TH1F("MinTimeDiffDeriv","MinTimeDiffDeriv",100,0,100);
  hVRatio                    = new TH1F("VRatio","VRatio",50,0,5);  
  hNZSupEvents               = new TH1F("hNZSupEvents","hNZSupEvents",120,0,120);
  hNoiseRMSAvg               = new TH1F("hNoiseRMSAvg","hNoiseRMSAvg",120,0,120);
  hYMaxRawYTSpecRatio        = new TH1F("hYMaxRawYTSpecRatio","hYMaxRawYTSpecRatio",50,0,2);
  hYRiseYTSpecRatio          = new TH1F("hYRiseYTSpecRatio","hYRiseYTSpecRatio",50,0,2);
  hYMaxDerivYTSpecRatio      = new TH1F("hYMaxDerivYTSpecRatio","hYMaxDerivYTSpecRatio",50,0,2);
  hYTSpecYMaxDiff            = new TH1F("hYTSpecYMaxDiff","HYTSpecYMaxDiff",100,-50,50);

  hAmpDiffVsUncorrectAmp                    = new TH2F("hAmpDiffVsUncorrectAmp","hAmpDiffVsUncorrectAmp",100,0,400,300,-100,200);
  hAmpDiffVsUncorrectAmpChannels20to70      = new TH2F("hAmpDiffVsUncorrectAmpChannels20to70","hAmpDiffVsUncorrectAmpChannels20to70",100,0,400,300,-100,200);

  hCorrectedAmpVsUncorrectAmp               = new TH2F("hCorrectedAmpVsUncorrectAmp","hCorrectedAmpVsUncorrectAmp",50,0,100,50,0,100);
  hCorrectedAmpVsUncorrectAmpChannels20to70 = new TH2F("hCorrectedAmpVsUncorrectAmpChannels20to70","hCorrectedAmpVsUncorrectAmpChannels20to70",50,0,100,50,0,100);

  hAmpDiffVsUncorrectAmpNotFirstHit         = new TH2F("hAmpDiffVsUncorrectAmpNotFirstHit","hAmpDiffVsUncorrectAmpNotFirstHit",100,0,400,300,-100,200);
  hCorrectedAmpVsUncorrectAmpNotFirstHit    = new TH2F("hCorrectedAmpVsUncorrectAmpNotFirstHit","hCorrectedAmpVsUncorrectAmpNotFirstHit",50,0,100,50,0,100);

  hYMaxRawYTSpecRatioVsYMax                    = new TH2F("hYMaxRawYTSpecRatioVsYMax","hYMaxRawYTSpecRatioVsYMax",100,0,100,50,0,2);
  //hYMaxVsYTSpecAllHits = new TH2F("hYMaxVsYTSpecAllHits","hYMaxVsYTSpecAllHits",100,0,100,100,0,100);
  //hYMaxVsYTSpecSingleHits = new TH2F("hYMaxVsYTSpecSingleHits","hYMaxVsYTSpecSingleHits",100,0,100,100,0,100);

  gUnAbsSigs = new TGraph(fNSamples);
  
  // TODO how to retrieve this automatically from channelIDmap???
  for (auto ii : fListOfChannels) {

    sprintf(name, "NoHitsDerivChannelPerEvent%d",ii);
    hNoHitsDerivPerChannel[ii] = new TH1F(name,name,20,0,20);

    sprintf(name, "RawVChannel%d",ii);
    hRawVPerChannel[ii] = new TH1F(name,name,400,0,400);
    
    sprintf(name, "RawVOneHitChannel%d",ii);
    hRawVOneHitPerChannel[ii] = new TH1F(name,name,400,0,400);

    sprintf(name, "RawVCorrectChannel%d",ii);
    hRawVCorrectPerChannel[ii] = new TH1F(name,name,400,0,400);
    
    sprintf(name, "DerivVChannel%d",ii);
    hDerivVPerChannel[ii] = new TH1F(name,name,400,0,400);

    sprintf(name, "DerivVOneHitChannel%d",ii);
    hDerivVOneHitPerChannel[ii] = new TH1F(name,name,400,0,400);

    sprintf(name, "DerivVCorrectChannel%d",ii);
    hDerivVCorrectPerChannel[ii] = new TH1F(name,name,400,0,400);

    sprintf(name,"hNoiseRMSPerChannel%d",ii);
    hNoiseRMSPerChannel[ii] = new TH1F(name,name,20,0,10);

  }
}

void DigitizerChannelETag::SaveDebugHistos(){
  if(fileOut!=0) std::cout<<"not zero "<<fileOut<<std::endl;
  fileOut->cd();
  
  std::cout<<fGlobalMode->IsPedestalMode()<<" "<<fGlobalMode->GetGlobalDebugMode()<<" "<<fSaveAnalog<<std::endl;

  if(fGlobalMode->IsPedestalMode() || fGlobalMode->GetGlobalDebugMode() || fSaveAnalog){
    hNoEventsReconstructed->Write();
    hOccupancy->Scale(1./hOccupancy->Integral());//hNoEventsReconstructed->GetEntries());
    hOccupancy->Write();
    hOccupancy2D->Write();
    hOccupancyOneHit->Scale(1./hOccupancyOneHit->Integral());//hNoEventsReconstructed->GetEntries());
    hOccupancyOneHit->Write();
    hNoHitsDeriv->Write();
    hRawV->Write();
    hDerivV->Write();
    hHitEnergy->Write();
    hHitEnergySingleHit->Write();
    hHitTime->Write();

    hRawVCorrect->Write();
    hRawVCorrectChannels20to70->Write();
    hRawVOneHit->Write();
    hDerivVOneHit->Write();
    hDerivVCorrect->Write();
    hAmpDiffVsUncorrectAmp->Write();
    hAmpDiffVsUncorrectAmpChannels20to70->Write();
    hAmpDiffVsUncorrectAmpNotFirstHit->Write();

    hCorrectedAmpVsUncorrectAmp->Write();
    hCorrectedAmpVsUncorrectAmpChannels20to70->Write();
    hCorrectedAmpVsUncorrectAmpNotFirstHit->Write();

    hNZSupEvents->Write();
    hNoiseRMSAvg->Divide(hNZSupEvents);
    hNoiseRMSAvg->Write();

    hYMaxRawYTSpecRatio->Write();
    hYRiseYTSpecRatio->Write();
    hYMaxDerivYTSpecRatio->Write();
    hYMaxRawYTSpecRatioVsYMax->Write();

    hYTSpecYMaxDiff->Write();
    //hYMaxVsYTSpecAllHits->Write();
    //hYMaxVsYTSpecSingleHits->Write();

    fileOut->mkdir("NoHitDerivChannel");  
    fileOut->cd("NoHitDerivChannel");  
    for (auto ii : fListOfChannels) hNoHitsDerivPerChannel[ii]->Write();

    fileOut->mkdir("RawVOneHitChannel");  
    fileOut->cd("RawVOneHitChannel");  
    for(auto ii : fListOfChannels) hRawVOneHitPerChannel[ii]->Write();

    fileOut->mkdir("RawVChannel");  
    fileOut->cd("RawVChannel");  
    for(auto ii : fListOfChannels) hRawVPerChannel[ii]->Write();

    fileOut->mkdir("RawVCorrectChannel");  
    fileOut->cd("RawVCorrectChannel");  
    for(auto ii : fListOfChannels) hRawVCorrectPerChannel[ii]->Write();

    fileOut->mkdir("DerivVOneHitChannel");  
    fileOut->cd("DerivVOneHitChannel");  
    for(auto ii : fListOfChannels) hDerivVOneHitPerChannel[ii]->Write();

    fileOut->mkdir("DerivVCorrectChannel");  
    fileOut->cd("DerivVCorrectChannel");  
    for(auto ii : fListOfChannels) hDerivVCorrectPerChannel[ii]->Write();

    fileOut->mkdir("DerivVChannel");  
    fileOut->cd("DerivVChannel");  
    for(auto ii : fListOfChannels) hDerivVPerChannel[ii]->Write();

    fileOut->mkdir("Waveforms");
    fileOut->cd("Waveforms");
    for(UInt_t ii = 0;ii<hRaw.size();ii++)
      hRaw[ii]->Write();  
  
    fileOut->mkdir("DerivativeTSpec");
    fileOut->cd("DerivativeTSpec");
    for(UInt_t ii = 0;ii<hDeriv.size();ii++)
      hDeriv[ii]->Write();  

    fileOut->mkdir("NegativeSignals");
    fileOut->cd("NegativeSignals");
    for(UInt_t ii = 0;ii<gUnAbsSigGraphs.size();ii++)
      gUnAbsSigGraphs[ii]->Write();  

    fileOut->mkdir("NoiseRMS");  
    fileOut->cd("NoiseRMS");  
    for(auto ii : fListOfChannels) hNoiseRMSPerChannel[ii]->Write();
    fileOut->Close();
    }
}

void DigitizerChannelETag::Reconstruct(std::vector<TRecoVHit *> &hitVec){  //using TSpectrum 
  // if(GetChID()==0) hNoEventsReconstructed->Fill(1);
  // if(GetChID()>60) return;//Beth 10/3/22: There are only 90 ETag channels but in ETag.cfg 96 channels are still listed
  tDerivHitVec          .clear();
  tDerivSortHitVec      .clear();
  tDerivDiffHitVec      .clear();
  vRawHitVec	        .clear();
  vRawSortHitVec        .clear();
  vRawCorrectHitVec     .clear();
  vTSpecYPHitVec        .clear();
  vTSpecYPSortHitVec    .clear();
  vTSpecYPCorrectHitVec .clear();
  if(fUsePulseProcessing==0){ 
    Double_t IsZeroSup = ZSupHit(fZeroSuppression,1000.);  //Beth 10/3/22: we should use the parameter signal window instead of hard-coded 1000. fZeroSuppression is not in ETag.cfg but defaults to 5.
    if(IsZeroSup) return;
    Double_t ped=CalcPedestal();  // takes the starting sample from data cards
    SetAbsSignals(ped);//Beth 21/2/22: moved from before zero suppression to after. Turns samples into positive mV and performs channel equalisation
    hSig->SetContent(fEqualSamples);
    CalcChaTime(hitVec);
  }
  else{
    // Giorgi pulse processing code goes here
  }
  AnalogPlotting(); //plot analog signals

}

void DigitizerChannelETag::SetAbsSignals(Double_t ped){
  Double_t ScaleFactor=1;
  if(fChannelEqualisation)  ScaleFactor=SetETagChaGain();   
  //fNSamples is 1024 but I can't find where it's set
  for(UShort_t i = 0;i<fNSamples;i++){
    fEqualSamples[i] = (ped-fSamples[i])/4096.*1000.; //convert fSamples to positive mV
    fEqualSamples[i] = (fEqualSamples[i])*ScaleFactor;//equalise channel gains
  }
}

void DigitizerChannelETag::AnalogPlotting(){
  //plot analog signals
  // if(fAnalogsPrinted<fTotalAnalogs){
    if(fAnalogPrint==1){//vRawCorrectHitVec.size()==1&&vTSpecYPHitVec[0]<20){
      hRaw.push_back((TH1F*)hSig->Clone());
      sprintf(name, "hRawEvent%iChannel%d", EventCounter,GetChID());
      hRaw[hRaw.size()-1]->SetNameTitle(name,name);
      
      hDeriv.push_back((TH1F*)HTSpec->Clone());
      sprintf(name, "hDerivEvent%iChannel%d", EventCounter,GetChID());
      hDeriv[hDeriv.size()-1]->SetNameTitle(name,name);
    
      for(int ii = 0; ii<fNSamples;ii++) gUnAbsSigs->SetPoint(ii,ii,fSamples[ii]);
      gUnAbsSigGraphs.push_back(gUnAbsSigs);

      fAnalogsPrinted++;
    }  
  // }
}

void DigitizerChannelETag::HitPlots(std::vector<TRecoVHit *> &hitVec){
  //  std::cout<<"Event "<<EventCounter<<" Ch "<<GetChID()<<" hitVec.size() "<<hitVec.size()<<" fNFoundPerChannel "<<fNFoundPerChannel[GetChID()]<<std::endl;
  hNoHitsDeriv->Fill(fNFoundPerChannel[GetChID()]);
  hNoHitsDerivPerChannel[GetChID()]->Fill(fNFoundPerChannel[GetChID()]);

    auto it = std::find(fListOfChannels.begin(), fListOfChannels.end(), GetChID());
    auto idx = it - fListOfChannels.begin();
  if(vRawCorrectHitVec.size()==1) hOccupancyOneHit->Fill(idx);

  //  double hitV=0;
  double AmpDiff=0;
  //  std::cout<<"Tot "<<vRawCorrectHitVec.size()<<std::endl;
  for(UInt_t myiHit=0;myiHit<fNFoundPerChannel[GetChID()];myiHit++){
    
    auto it = std::find(fListOfChannels.begin(), fListOfChannels.end(), GetChID());
    auto idx = it - fListOfChannels.begin();
    hOccupancy->Fill(idx);
    hOccupancy2D->Fill(PosMap[GetChID()].first, PosMap[GetChID()].second);
    hHitTime->Fill(tDerivSortHitVec[myiHit]);

    //uncorrected amplitude
    hDerivV->Fill(vTSpecYPSortHitVec[myiHit]);
    hDerivVPerChannel[GetChID()]->Fill(vTSpecYPSortHitVec[myiHit]);
    hHitEnergy->Fill(vTSpecYPCorrectHitVec[myiHit]*fDerivAmpToEnergy);

    //corrected amplitude
    hDerivVCorrect->Fill(vTSpecYPCorrectHitVec[myiHit]);//hitV);
    hDerivVCorrectPerChannel[GetChID()]->Fill(vTSpecYPCorrectHitVec[myiHit]);
    //    AmpDiff=vTSpecYPSortHitVec[myiHit]-hitV;
    //hAmpDiffVsUncorrectAmp->Fill(vTSpecYPSortHitVec[myiHit],AmpDiff);
    //    hCorrectedAmpVsUncorrectAmp->Fill(vTSpecYPSortHitVec[myiHit],hitV);

    hYTSpecYMaxDiff->Fill(vTSpecYPSortHitVec[myiHit]-vRawSortHitVec[myiHit]);
    //hYMaxVsYTSpecAllHits->Fill(vRawSortHitVec[myiHit],vTSpecYPSortHitVec[myiHit]);

    //corrected amplitude for "good" channels
    if(GetChID()>=20 &&GetChID()<=70){
      //      hRawVCorrectChannels20to70->Fill(hitV);
      //      hAmpDiffVsUncorrectAmpChannels20to70->Fill(vRawSortHitVec[myiHit],AmpDiff);
      //hCorrectedAmpVsUncorrectAmpChannels20to70->Fill(vRawSortHitVec[myiHit],hitV);
    }
    //events with exactly one hit
    hRawV->Fill(RawGetMax);
    hRawVPerChannel[GetChID()]->Fill(RawGetMax);

    if(fNFoundPerChannel[GetChID()]==1){
      hRawVOneHit->Fill(RawGetMax);
      hRawVOneHitPerChannel[GetChID()]->Fill(RawGetMax);
      hDerivVOneHit->Fill(vTSpecYPSortHitVec[myiHit]);
      hDerivVOneHitPerChannel[GetChID()]->Fill(vTSpecYPSortHitVec[myiHit]);
      hHitEnergySingleHit->Fill(vTSpecYPSortHitVec[myiHit]*fDerivAmpToEnergy);
      //hYMaxVsYTSpecSingleHits->Fill(vRawSortHitVec[myiHit],vTSpecYPSortHitVec[myiHit]);

      //ratio between derivative peak amplitude as found by TSpectrum and maximum amplitude of raw signal for single hit events
      hYMaxRawYTSpecRatio->Fill(RawGetMax/vTSpecYPSortHitVec[myiHit]);
      hYRiseYTSpecRatio->Fill(RawRise/vTSpecYPSortHitVec[myiHit]);
      hYMaxDerivYTSpecRatio->Fill(DerivGetMax/vTSpecYPSortHitVec[myiHit]);

      hYMaxRawYTSpecRatioVsYMax->Fill(RawGetMax,RawGetMax/vTSpecYPSortHitVec[myiHit]);
    }

    //hits after the first
    if(myiHit>1){
      hAmpDiffVsUncorrectAmpNotFirstHit->Fill(vRawSortHitVec[myiHit],AmpDiff);
      //      hCorrectedAmpVsUncorrectAmpNotFirstHit->Fill(vRawSortHitVec[myiHit],hitV);
    }
  }
  
}

Double_t DigitizerChannelETag::SetETagChaGain(){
  // ASF 11-9-2022: Set all ETag gains to 1 while no calibration
  Double_t ScaleFactor = 1.0;
  return ScaleFactor;
  //normalised to gaussian mean = 35 using run 665
  //  std::cout<<"Setting cha gain"<<std::endl;
  if(GetChID()==0) 	 ScaleFactor = 1.14579 ;
  if(GetChID()==1) 	 ScaleFactor = 1.13981 ;
  if(GetChID()==2) 	 ScaleFactor = 1.1624  ;
  if(GetChID()==3) 	 ScaleFactor = 1.0199  ;
  if(GetChID()==4) 	 ScaleFactor = 1.03848 ;
  if(GetChID()==5) 	 ScaleFactor = 1.31815 ;
  if(GetChID()==6) 	 ScaleFactor = 1.59532 ;
  if(GetChID()==7) 	 ScaleFactor = 1.26717 ;
  if(GetChID()==8) 	 ScaleFactor = 1.17466 ;
  if(GetChID()==9) 	 ScaleFactor = 1.59172 ;
  if(GetChID()==10)	 ScaleFactor = 1.41543 ;
  if(GetChID()==11)	 ScaleFactor = 1.59211 ;
  if(GetChID()==12)	 ScaleFactor = 1.24053 ;
  if(GetChID()==13)	 ScaleFactor = 1.59605 ;
  if(GetChID()==14)	 ScaleFactor = 1.41128 ;
  if(GetChID()==15)	 ScaleFactor = 1.66592 ;
  if(GetChID()==16)	 ScaleFactor = 1.66667 ;
  if(GetChID()==17)	 ScaleFactor = 1.59268 ;
  if(GetChID()==18)	 ScaleFactor = 0.968462;
  if(GetChID()==19)	 ScaleFactor = 0.913226;
  if(GetChID()==20)	 ScaleFactor = 1.19737 ;
  if(GetChID()==21)	 ScaleFactor = 1.14329 ;
  if(GetChID()==22)	 ScaleFactor = 1.63259 ;
  if(GetChID()==23)      ScaleFactor = 1.00445 ;
  if(GetChID()==24)	 ScaleFactor = 0.90624 ;
  if(GetChID()==25)	 ScaleFactor = 1.10906 ;
  if(GetChID()==26)	 ScaleFactor = 0.910591;
  if(GetChID()==27)	 ScaleFactor = 0.93724 ;
  if(GetChID()==28)	 ScaleFactor = 0.908172;
  if(GetChID()==29)	 ScaleFactor = 0.930454;
  if(GetChID()==30)	 ScaleFactor = 0.898969;
  if(GetChID()==31)	 ScaleFactor = 0.854727;
  if(GetChID()==32)	 ScaleFactor = 0.887678;
  if(GetChID()==33)	 ScaleFactor = 0.816444;
  if(GetChID()==34)	 ScaleFactor = 0.893628;
  if(GetChID()==35)	 ScaleFactor = 1.12612 ;
  if(GetChID()==36)	 ScaleFactor = 1.06171 ;
  if(GetChID()==37)	 ScaleFactor = 1.05226 ;
  if(GetChID()==38)	 ScaleFactor = 0.961816;
  if(GetChID()==39)	 ScaleFactor = 0.854641;
  if(GetChID()==40)	 ScaleFactor = 1.04747 ;
  if(GetChID()==41)	 ScaleFactor = 1.0973  ;
  if(GetChID()==42)	 ScaleFactor = 0.898218;
  if(GetChID()==43)	 ScaleFactor = 1.08689 ;
  if(GetChID()==44)	 ScaleFactor = 1.12929 ;
  if(GetChID()==45)	 ScaleFactor = 1.08844 ;
  if(GetChID()==46)	 ScaleFactor = 1.18264 ;
  if(GetChID()==47)	 ScaleFactor = 0.974706;
  if(GetChID()==48)	 ScaleFactor = 0.936744;
  if(GetChID()==49)	 ScaleFactor = 0.892796;
  if(GetChID()==50)	 ScaleFactor = 0.90297 ;
  if(GetChID()==51)	 ScaleFactor = 1.18523 ;
  if(GetChID()==56)	 ScaleFactor = 0.973889;
  if(GetChID()==57)	 ScaleFactor = 0.994545;
  if(GetChID()==58)	 ScaleFactor = 1.28083 ;
  if(GetChID()==59)	 ScaleFactor = 0.977371;
  if(GetChID()==60)	 ScaleFactor = 1.32175 ;
  if(GetChID()==61)	 ScaleFactor = 1.16763 ;
  if(GetChID()==62)	 ScaleFactor = 1.00344 ;
  if(GetChID()==63)	 ScaleFactor = 0.74402 ;
  if(GetChID()==64)	 ScaleFactor = 1.47513 ;
  if(GetChID()==65)	 ScaleFactor = 0.954032;
  if(GetChID()==66)	 ScaleFactor = 0.771184;
  if(GetChID()==67)	 ScaleFactor = 1.05539 ;
  if(GetChID()==68)	 ScaleFactor = 1.09955 ;
  if(GetChID()==69)	 ScaleFactor = 0.83747 ;
  if(GetChID()==70)	 ScaleFactor = 1.08944 ;
  if(GetChID()==71)	 ScaleFactor = 0.829015;
  if(GetChID()==72)	 ScaleFactor = 0.898273;
  if(GetChID()==73)	 ScaleFactor = 0.885728;
  if(GetChID()==74)	 ScaleFactor = 1.01983 ;
  if(GetChID()==75)	 ScaleFactor = 0.909889;
  if(GetChID()==76)	 ScaleFactor = 1.06069 ;
  if(GetChID()==77)	 ScaleFactor = 0.74599 ;
  if(GetChID()==78)	 ScaleFactor = 0.92206 ;
  if(GetChID()==79)	 ScaleFactor = 0.869043;
  if(GetChID()==80)	 ScaleFactor = 0.936432;
  if(GetChID()==81)	 ScaleFactor = 1.04356 ;
  if(GetChID()==82)	 ScaleFactor = 0.954175;
  if(GetChID()==83)	 ScaleFactor = 1.02682 ;
  if(GetChID()==84)	 ScaleFactor = 0.898411;
  if(GetChID()==85)	 ScaleFactor = 0.940743;
  if(GetChID()==86)	 ScaleFactor = 0.92174 ;
  if(GetChID()==87)	 ScaleFactor = 1.08519 ;
  if(GetChID()==88)	 ScaleFactor = 0.728288;
  if(GetChID()==89)	 ScaleFactor = 0.285807;
  return ScaleFactor;
}

Double_t DigitizerChannelETag::TailHeightDerivative(Int_t DeltaT){//DeltaT in samples. Returns fraction of maximum signal height that a signal will have at time DeltaT samples after the peak
  // ASF 26-09-2022: Set all of these to 0 for now
  // Not sure what they do yet, but change if appropriate
  Double_t HeightFrac=0;
  return HeightFrac;
  Double_t Frac[152];
  Frac[0]   =  1; 
  Frac[1]   =  0.997254     ;
  Frac[2]   =  0.980119     ;
  Frac[3]   =  0.964593     ;
  Frac[4]   =  0.941908     ;
  Frac[5]   =  0.866346     ;
  Frac[6]   =  0.786271     ;
  Frac[7]   =  0.744847     ;
  Frac[8]   =  0.681817     ;
  Frac[9]   =  0.605922     ;
  Frac[10]  =  0.52764      ;
  Frac[11]  =  0.45165      ;
  Frac[12]  =  0.372615     ;
  Frac[13]  =  0.297546     ;
  Frac[14]  =  0.223813     ;
  Frac[15]  =  0.156913     ;
  Frac[16]  =  0.0931688    ;
  Frac[17]  =  0.0337405    ;
  Frac[18]  = -0.0213706    ;
  Frac[19]  = -0.0669654    ;
  Frac[20]  = -0.109943     ;
  Frac[21]  = -0.152183     ;
  Frac[22]  = -0.181495     ;
  Frac[23]  = -0.206573     ;
  Frac[24]  = -0.224541     ;
  Frac[25]  = -0.249835     ;
  Frac[26]  = -0.264263     ;
  Frac[27]  = -0.274342     ;
  Frac[28]  = -0.288237     ;
  Frac[29]  = -0.296333     ;
  Frac[30]  = -0.302538     ;
  Frac[31]  = -0.307553     ;
  Frac[32]  = -0.308717     ;
  Frac[33]  = -0.311267     ;
  Frac[34]  = -0.310586     ;
  Frac[35]  = -0.309731     ;
  Frac[36]  = -0.30875      ;
  Frac[37]  = -0.306781     ;
  Frac[38]  = -0.305272     ;
  Frac[39]  = -0.300329     ;
  Frac[40]  = -0.293151     ;
  Frac[41]  = -0.290462     ;
  Frac[42]  = -0.284854     ;
  Frac[43]  = -0.2765	    ;
  Frac[44]  = -0.270725     ;
  Frac[45]  = -0.264902     ;
  Frac[46]  = -0.260046     ;
  Frac[47]  = -0.254269     ;
  Frac[48]  = -0.249207     ;
  Frac[49]  = -0.248261     ;
  Frac[50]  = -0.243831     ;
  Frac[51]  = -0.235288     ;
  Frac[52]  = -0.234772     ;
  Frac[53]  = -0.229418     ;
  Frac[54]  = -0.229754     ;
  Frac[55]  = -0.224102     ;
  Frac[56]  = -0.224207     ;
  Frac[57]  = -0.225017     ;
  Frac[58]  = -0.22231      ;
  Frac[59]  = -0.221657     ;
  Frac[60]  = -0.222108     ;
  Frac[61]  = -0.219502     ;
  Frac[62]  = -0.219925     ;
  Frac[63]  = -0.217694     ;
  Frac[64]  = -0.215415     ;
  Frac[65]  = -0.215071     ;
  Frac[66]  = -0.214287     ;
  Frac[67]  = -0.209114     ;
  Frac[68]  = -0.20973      ;
  Frac[69]  = -0.209181     ;
  Frac[70]  = -0.208435     ;
  Frac[71]  = -0.203314     ;
  Frac[72]  = -0.201796     ;
  Frac[73]  = -0.201014     ;
  Frac[74]  = -0.198875     ;
  Frac[75]  = -0.193166     ;
  Frac[76]  = -0.191475     ;
  Frac[77]  = -0.18906      ;
  Frac[78]  = -0.187891     ;
  Frac[79]  = -0.181985     ;
  Frac[80]  = -0.179791     ;
  Frac[81]  = -0.175351     ;
  Frac[82]  = -0.175295     ;
  Frac[83]  = -0.170323     ;
  Frac[84]  = -0.167377     ;
  Frac[85]  = -0.161963     ;
  Frac[86]  = -0.160955     ;
  Frac[87]  = -0.153541     ;
  Frac[88]  = -0.150095     ;
  Frac[89]  = -0.145247     ;
  Frac[90]  = -0.142584     ;
  Frac[91]  = -0.139147     ;
  Frac[92]  = -0.138427     ;
  Frac[93]  = -0.133732     ;
  Frac[94]  = -0.132274     ;
  Frac[95]  = -0.124256     ;
  Frac[96]  = -0.126357     ;
  Frac[97]  = -0.122266     ;
  Frac[98]  = -0.121186     ;
  Frac[99]  = -0.117688     ;
  Frac[100] = -0.118717     ;
  Frac[101] = -0.115563     ;
  Frac[102] = -0.114932     ;
  Frac[103] = -0.110287     ;
  Frac[104] = -0.110468     ;
  Frac[105] = -0.110157     ;
  Frac[106] = -0.109457     ;
  Frac[107] = -0.104666     ;
  Frac[108] = -0.103959     ;
  Frac[109] = -0.101067     ;
  Frac[110] = -0.100501     ;
  Frac[111] = -0.0948209    ;
  Frac[112] = -0.0958058    ;
  Frac[113] = -0.0907429    ;
  Frac[114] = -0.089684     ;
  Frac[115] = -0.0848281    ;
  Frac[116] = -0.0835924    ;
  Frac[117] = -0.0827835    ;
  Frac[118] = -0.0837584    ;
  Frac[119] = -0.0792539    ;
  Frac[120] = -0.0766105    ;
  Frac[121] = -0.0717581    ;
  Frac[122] = -0.0702401    ;
  Frac[123] = -0.0697511    ;
  Frac[124] = -0.0690636    ;
  Frac[125] = -0.0695008    ;
  Frac[126] = -0.0668789    ;
  Frac[127] = -0.0606407    ;
  Frac[128] = -0.0609923    ;
  Frac[129] = -0.0611206    ;
  Frac[130] = -0.0628294    ;
  Frac[131] = -0.0592398    ;
  Frac[132] = -0.0566166    ;
  Frac[133] = -0.0545669    ;
  Frac[134] = -0.0523624    ;
  Frac[135] = -0.0509565    ;
  Frac[136] = -0.0531841    ;
  Frac[137] = -0.0513809    ;
  Frac[138] = -0.0483326    ;
  Frac[139] = -0.0459102    ;
  Frac[140] = -0.0454662    ;
  Frac[141] = -0.0467786    ;
  Frac[142] = -0.0479043    ;
  Frac[143] = -0.0452532    ;
  Frac[144] = -0.0421873    ;
  Frac[145] = -0.0399463    ;
  Frac[146] = -0.0420936    ;
  Frac[147] = -0.0393451    ;
  Frac[148] = -0.043388     ;
  Frac[149] = -0.0408676    ;
  Frac[150] = -0.0420922    ;
  Frac[151] = -0.037542     ;
  Frac[152] = -0.0409631    ;
  Frac[153]  =  -0.0391775 ;
  Frac[154]  =  -0.0418197 ;
  Frac[155]  =  -0.0390806 ;  
  Frac[156]  =    -0.0408229 ;
  Frac[157]  =    -0.0381236 ;
  Frac[158]  =    -0.0404099 ;
  Frac[159]  =    -0.0388342 ;
  Frac[160]  =    -0.0418864 ;
  Frac[161]  =    -0.0362089 ;
  Frac[162]  =    -0.0397867 ;
  Frac[163]  =    -0.0341513 ;
  Frac[164]  =    -0.0373749 ;
  Frac[165]  =    -0.0334771 ;
  Frac[166]  =    -0.0361808 ;
  Frac[167]  =    -0.0307574 ;
  Frac[168]  =    -0.034892  ;
  Frac[169]  =    -0.0308241 ;
  Frac[170]  =    -0.0323418 ;
  Frac[171]  =    -0.0277144 ;
  Frac[172]  =    -0.0294595 ;
  Frac[173]  =    -0.0294106 ;
  Frac[174]  =    -0.0305658 ;
  Frac[175]  =    -0.0245063 ;
  Frac[176]  =    -0.029801  ;
  Frac[177]  =    -0.0273992 ;
  Frac[178]  =    -0.0278916 ;
  Frac[179]  =    -0.0278331 ;
  Frac[180]  =    -0.0325746 ;
  Frac[181]  =    -0.0302618 ;
  Frac[182]  =    -0.0326252 ;
  Frac[183]  =    -0.0287132 ;
  Frac[184]  =    -0.0303228 ;
  Frac[185]  =    -0.0280644 ;
  Frac[186]  =    -0.0317901 ;
  Frac[187]  =    -0.0298637 ;
  Frac[188]  =    -0.0304833 ;
  Frac[189]  =    -0.0262541 ;
  Frac[190]  =    -0.0300384 ;
  Frac[191]  =    -0.0278753 ;
  Frac[192]  =    -0.0277963 ;
  Frac[193]  =    -0.0274074 ;
  Frac[194]  =    -0.0247811 ;
  Frac[195]  =    -0.0221116 ;
  Frac[196]  =    -0.0229999 ;
  Frac[197]  =    -0.0198742 ;
  Frac[198]  =    -0.020908  ;
  Frac[199]  =    -0.0194822 ;
  Frac[200]  =    -0.0205471 ;
  Frac[201]  =    -0.015884  ;
  Frac[202]  =    -0.0185419 ;
  Frac[203]  =    -0.0142732 ;
  Frac[204]  =    -0.0185146 ;
  Frac[205]  =    -0.0142009 ;
  Frac[206]  =    -0.0135252 ;
  Frac[207]  =    -0.0106508 ;
  Frac[208]  =    -0.00973468;
  Frac[209]  =    -0.0101464 ;
  Frac[210]  =    -0.00836321;
  Frac[211]  =    -0.00826306;
  Frac[212]  =    -0.00947927;
  Frac[213]  =    -0.00749842;
  Frac[214]  =    -0.00702742;
  Frac[215]  =    -0.00558369;
  Frac[216]  =    -0.00793642;
  Frac[217]  =    -0.00540675;
  Frac[218]  =    -0.00414951;
  Frac[219]  =    -0.00336889;
  Frac[220]  =    -0.00553797;
  Frac[221]  =    -0.00389439;
  Frac[222]  =    -0.00497594;
  Frac[223]  =    -0.00729843;
  Frac[224]  =    -0.00641133;
  Frac[225]  =    -0.00677281;
  Frac[226]  =    -0.00458745;
  Frac[227]  =    -0.00302479;
  Frac[228]  =    -0.00393472;
  Frac[229]  =    -0.00628557;
  Frac[230]  =    -0.00620519;
  Frac[231]  =    -0.00488894;
  Frac[232]  =    -0.0061514 ;
  Frac[233]  =    -0.00773842;
  Frac[234]  =    -0.00627602;
  Frac[235]  =    -0.0044926 ;
  Frac[236]  =    -0.0059019 ;
  Frac[237]  =    -0.00607805;
  Frac[238] =     -0.00315486;
  Frac[239] =     -0.00809495;
  Frac[240] =     -0.00581927;
  Frac[241] =     -0.00692714;
  Frac[242] =     -0.0083298 ;
  Frac[243] =     -0.00686902;
  Frac[244] =     -0.00649547;
  Frac[245] =     -0.00638095;
  Frac[246] =     -0.00716051;
  Frac[247] =     -0.00640555;
  Frac[248] =     -0.00820994;
  Frac[249] =     -0.00831537;
  Frac[250] =     -0.00961994;
  Frac[251] =     -0.00790926;
  Frac[252] =     -0.00652269;
  Frac[253] =     -0.00774797;
  Frac[254] =     -0.00572676;
  Frac[255] =     -0.00605236;
  Frac[256] =     -0.0057508 ;
  Frac[257] =     -0.00453228;
  Frac[258] =     -0.0069553 ;
  Frac[259] =     -0.00519885;
  Frac[260] =     -0.00603265;
  Frac[261] =     -0.00523939;
  Frac[262] =     -0.00299523;
  Frac[263] =     -0.00096314;
  Frac[264] =     -0.00405634;
  Frac[265] =     -0.00369099;
  Frac[266] =     -0.00506103;
  Frac[267] =     -0.00441765;
  Frac[268] =     -0.00514607;
  Frac[269] =     -0.00259789;
  Frac[270] =     -0.00653257;
  Frac[271] =     -0.00386856;
  Frac[272] =     -0.00312448;
  Frac[273] =     -0.00145314;
  Frac[274] =     -0.00241513;
  Frac[275] =     -0.00087400;
  Frac[276] =     0.000892302;
  Frac[277] =     -0.00249199;
  Frac[278] =     -0.00282059;
  Frac[279] =     0.00117736 ;
  Frac[280] =     0.0012085  ;
  Frac[281] =     0.00132184 ;
  Frac[282] =     -0.00119959;
  Frac[283] =     0.00144912 ;
  Frac[284] =     0.000605838;
  Frac[285] =     -1.06317e-0;
  Frac[286] =     -0.00142336;
  Frac[287] =     -0.00288644;
  Frac[288] =     -0.00378898;
  Frac[289] =     -0.00194118;

  if(DeltaT<290)  HeightFrac = Frac[DeltaT];
  else HeightFrac=0;						     
  return HeightFrac;						     
 									     
 }                                                                        
