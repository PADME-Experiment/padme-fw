#include "DigitizerChannelSAC.hh"
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

void DigitizerChannelSAC::PrintConfig(){
  std::cout << "Hi I'm the SAC: " << std::endl;
  std::cout << "Thr low  "<< fAmpThresholdLow  << std::endl;
  std::cout << "Time bin "<< fTimeBin  << std::endl;
  std::cout << "Time window per single hit "<< fSignalWidth  << std::endl;
  for(int i=0;i<32;i++){
    //  std::cout<<"Ped "<<i<<" "<<fPedCh[i]<<" "<<std::endl;
    // if(i%5==0) std::cout<<std::endl;
  }


 if(fGlobalMode->GetGlobalDebugMode()!=0){
   std::cout<<"General Config Flags SAC Digi***************** "<<std::endl;
   std::cout<<"fIsPed          "<<fGlobalMode->IsPedestalMode()<< std::endl;
   std::cout<<"fIsReco         "<<fGlobalMode->IsRecoMode()<< std::endl;
   std::cout<<"fIsMonitor      "<<fGlobalMode->IsMonitorMode()<< std::endl;
   std::cout<<"fIsCosmic       "<<fGlobalMode->IsCosmicsMode()<< std::endl;
   std::cout<<"fIsDebug  SAC  "<<fGlobalMode->GetGlobalDebugMode()<< std::endl;
  }
}


void DigitizerChannelSAC::Init(GlobalRecoConfigOptions* gOptions, PadmeVRecoConfig *cfg){


  //fGlobalMode = gMode;
  fGlobalMode = gOptions; //CT
  //H1 = new TH1D("h1","h1",990,0.,990.);

  //fGlobalMode = gMode;
  H1 = new TH1D("h1","h1",1000,0.,1000.);

  hListCal    = new TList();  // needs to be simplified
  hPedCalo = new TH1D*[32];

  for(int kk=0;kk<32;kk++){
    hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),600,3700.,3900);
    hListCal->Add(hPedCalo[kk]);
  }
  
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
  
  //Set a default Adc channel pedestals;
  for(int kk=0;kk<32;kk++){
    fPedCh[kk] = 3800;
  }
  std::cout << cfg->GetName() << "*******************************" <<  std::endl;
  SetAnalogOffSets(); //M. Raggi: 19/10/2018  read anaolg offsets values from file
  PrintConfig();
  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsPedestalMode()){
    PrepareDebugHistos(); //debugging mode histos
  }else{
    PrepareTmpHistos();  //Temp histos
  }
}

void DigitizerChannelSAC::PrepareTmpHistos(){
  hListTmp    = new TList();  
  hListTmp->Add(hdxdt   = new TH1F("hdxdt","hdxdt",1000,0.,1000.));
  hListTmp->Add(hSignal = new TH1F("hSignal","hSignal",1000,0.,1000.));
  hListTmp->Add(hSigOv  = new TH1F("hSigOv","hSigOv",4000,0.,1000.));
}

//M. Raggi: 19/10/2018
void DigitizerChannelSAC::SetAnalogOffSets(){
  std::ifstream PrevPed; //reading prevoius pedestal from file written by drawped32
  char fname[50];
  Int_t NBD=27;
  sprintf(fname,"config/BDPed/PedBD%d.ped",NBD);
  PrevPed.open(fname);
  if(PrevPed.is_open()){
    double temp;
    for(int i=0;i<32;i++){
      PrevPed >> temp >> fPedCh[i] >> temp>> temp >> fCalibCh[i];//reads channel number and discards it
      //      std::cout <<"PEDCH "<<i<<" "<<fPedCh[i]<<std::endl;      
    }
  }
  else{ 
    std::cout << "No previous data available for board "<<NBD<<" resorting to default pedestal (3800)"<<std::endl;
  } 
 
}

void DigitizerChannelSAC::PrepareDebugHistos(){
  fileOut    = new TFile("SACAn.root", "RECREATE");
  hListCal   = new TList();  
  hListEv    = new TList();  
  SAC = new TTree("SAC","SAC");

  SAC->Branch("ElCh",&fChID);
  SAC->Branch("Row",&Row);
  SAC->Branch("Col",&Col);
  SAC->Branch("Zsup",&Zsup);
  SAC->Branch("Avg200",&fAvg200);
  SAC->Branch("HitE",&HitE);
  SAC->Branch("ESpec",&fESpec);
  SAC->Branch("HitT",&fTime);
  SAC->Branch("NPeaks",&fNPeak);

  hPedCalo = new TH1D*[32];
  //  hAvgCalo = new TH1D*[32];
  hPedMean = new TH1D*[32];
  hVMax    = new TH1D*[32];
  //  h200QCh  = new TH1D*[32]; //CT
  hQCh     = new TH1D*[32]; //CT

  hListEv->Add(hdxdt   = new TH1F("hdxdt","hdxdt",1000,0.,1000.));
  hListEv->Add(hdxdtMax= new TH1F("hdxdtMax","hdxdtMax",1600,-200.,3000.));
  hListEv->Add(hdxdtRMS= new TH1F("hdxdtRMS","hdxdtRMS",1000,0.,200.));
  hListEv->Add(hSignal = new TH1F("hSignal","hSignal",1000,0.,1000.));
  hListEv->Add(hSat    = new TH1F("hSat","hSat",1000,0.,1000.));
  hListEv->Add(hSigOv  = new TH1F("hSigOv","hSigOv",4000,0.,1000.));
  hListEv->Add(hDiff   = new TH1F("hDiff","hDiff",4000,0.,1000.));

  hListCal->Add(hTime  = new TH1D("hTime","hTime",1000,0.,1000.));
  //hListCal->Add(hTimeCut= new TH1D("hTimeCut","hTimeCut",1000,0.,1000.));
  //hListCal->Add(hTimeOv= new TH1D("hTimeOv","hTimeOv",1000,0.,1000.));
  
  for(int kk=0;kk<32;kk++){
    hPedCalo[kk] = new TH1D(Form("hPedCalo%d",kk),Form("hPedCalo%d",kk),1200,3300.,3900);
    //  hAvgCalo[kk] = new TH1D(Form("hAvgCalo%d",kk),Form("hAvgCalo%d",kk),1200,3300.,3900);
    hPedMean[kk] = new TH1D(Form("hSig%d",kk),Form("hSig%d",kk),1000,0.,1000.);
    hVMax[kk]    = new TH1D(Form("hVMax%d",kk),Form("hVMax%d",kk),250,0.,500.); // in mV
    //  h200QCh[kk]  = new TH1D(Form("h200QCh%d",kk),Form("h200QCh%d",kk),600,-200,400); //CT
    hQCh[kk]     = new TH1D(Form("hQCh%d",kk),Form("hQCh%d",kk),250,0,500); //CT
    hListCal->Add(hPedCalo[kk]);
    //    hListCal->Add(hAvgCalo[kk]);
    hListCal->Add(hPedMean[kk]);
    hListCal->Add(hVMax[kk]);
    // hListCal->Add(h200QCh[kk]); //CT
    hListCal->Add(hQCh[kk]); //CT
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

/*Double_t DigitizerChannelSAC::CalcPedestal() {
  double rms;
  double avg;
  Int_t fCh       = GetChID();
  Int_t fChID     = GetChID();
  UInt_t fTrigMask= GetTrigMask();
  Int_t ElCh = fCh/10*5 +fCh%5;
  //std::cout<<ElCh<<" mask "<<fTrigMask<<std::endl;
  // Se non trovo eventi vuoti?
  //  fSigRms[] = TMath::RMS(1000,&fSamples[0]);
  rms = TMath::RMS(1000,&fSamples[0]);
  char name[50];
  //  if(rms<5){ // ci vorrebbe il cosmic trigger
  // Remove the TList !!
  sprintf(name,"hPedCalo%d",ElCh);
  histo=(TH1D*) hListCal->FindObject(name);
  if(fTrigMask==2){ // cosmic trigger sembra sporco il ped
    fSigAvg[ElCh] = TMath::Mean(1000,&fSamples[0]); 
    histo->Fill(fSigAvg[ElCh]);
    // avg = TMath::Mean(1000,&fSamples[0]);
    //    histo->Write();
  } 
  Double_t PedDiff= histo->GetMean()-fPedCh[ElCh];
  if(histo->GetEntries()>50 && PedDiff>2.){    // 2. value needs tuning
    //std::cout<<ElCh<<" Pedestal diff "<<PedDiff<<std::endl;
    fPedCh[ElCh]=histo->GetMean();
  }
  
  // fill histogram and take averge.
  //  std::cout << " RMS " << rms <<" fPed SAC "<<histo->GetMean()<<" el Ch"<<ElCh<< std::endl;
  return fPed;
}*/

Double_t DigitizerChannelSAC::CalcCharge(UShort_t fCh) {  //not used
  // can include a better algorithm to compute hit charge
  return fCharge;
}

Double_t DigitizerChannelSAC::CalcTime(UShort_t iMax, UShort_t fCh) {
  fTime = 0.;
  //Shold include a fit to the peak position starting from the yp provided by TSpectrum

  return fTime=fTime*fTimeBin;
}

Double_t DigitizerChannelSAC::CalcChaTime(std::vector<TRecoVHit *> &hitArray,UShort_t iMax) {
  fTime   = 0.;
  fCharge = 0.;
  static TSpectrum SpectrumProcessor(50);// = new TSpectrum(20);

  Int_t NImage=0;
  static const Double_t pCMeV= 3.2E5*2*1.67E-7; //Nominal Gain at 1500 x npe/MeV x echarge (in pC) needs tuning by calibration
  //  Double_t pCMeV= 1.; //Nominal Gain at 1500 x npe/MeV x echarge (in pC)
  static Int_t NIntSamp= fSignalWidth/fTimeBin; 
      
  //currently looking for peaks with TSpectrum to obtain multi hit times
  //M. Raggi 19/10/2018

  static const Int_t npeaks =50;
  static Double_t AbsSamRec[1024];

  Int_t fCh  = GetChID();
  Int_t ElCh = fCh/10*5 +fCh%5;
  if(ElCh<0) return -1;
  //  std::cout<<fCh<<" ElCh "<<ElCh<<std::endl;
  fAvg200  = TMath::Mean(80,&fSamples[0]);//questo fa la media sui primi 40 samples e la usa come piedistallo
  Double_t fRMS40  = TMath::RMS(80,&fSamples[0]);

  //  std::cout<<"fTrigMask" <<fTrigMask<<std::endl;
  if(fTrigMask!=2) return -1.;  //tengo solo cosmici
  //  std::cout<<"fTrigMask dopo " <<fTrigMask<<std::endl;

  char name[50];
  if(ElCh>=0){
    sprintf(name,"hPedCalo%d",ElCh);
    histo=(TH1D*) hListCal->FindObject(name);
    histo->Fill(fAvg200);
  }
  for(UShort_t s=0;s<iMax;s++){
    //    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+fPedCh[ElCh])/4096*1000.; //in mV positivi MR 
//07
    AbsSamRec[s] = (Double_t) (-1.*fSamples[s]+fAvg200)/4096*1000.; //in mV positivi using first Istart samples
  }

  H1->SetContent(AbsSamRec);
  //  char name[50];
  //  if(fTrigMask==2){
  Double_t VMax = H1->GetMaximum();
  Double_t VMin = H1->GetMinimum();
     
  Double_t Charge; //CT
  //Charge = (VMax*2*pow(10,-9))/(2*50); // in pC, CT
  //std::cout<<Charge<<" Charge "<< " fCh "<<GetChID()<<std::endl;
  //  if(VMax<-2*VMin && VMax>15.) std::cout<<VMax<<" VMax "<< " fCh "<<GetChID()<<" VMin "<<VMin<<std::endl;

//  if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsCosmicsMode()){
//    if(VMax>3 && ElCh>=0){
//      //	std::cout<<ElCh<<" VMax "<<VMax<<std::endl;
//      fileOut->cd();
//      sprintf(name,"hSig%d",ElCh);
//      histo=(TH1D*) hListCal->FindObject(name);
//      histo->SetContent(AbsSamRec);
//      histo->Write();
//      histo->Reset();
//      sprintf(name,"hVMax%d",ElCh);
//      histo=(TH1D*) hListCal->FindObject(name);
//      histo->Fill(VMax);
//    }
//  }
  
//  if(VMax>fAmpThresholdHigh && VMax>-2*VMin){ // zero suppression on Voltage normalize to energy.
  // if(VMax>fAmpThresholdLow){ // zero suppression on Voltage normalize to energy.


  if(VMax>fAmpThresholdHigh && VMax>-2*VMin){ // zero suppression on Voltage normalize to energy.
    TSpectrum *s = &SpectrumProcessor;//new TSpectrum(npeaks);

    Double_t peak_thr  = fAmpThresholdLow/VMax;   //minimum peak height allowed.
    //    Int_t nfound = s->Search(H1,3,"",peak_thr);   //corrected for 2.5GHz cannot be less then 0.05
    Int_t nfound = s->Search(H1,4,"",0.7);   //corrected for 2.5GHz cannot be less then 0.05
    Int_t fTrigMask=GetTrigMask();
    
    // ROOT 6 version
    //    Double_t *xpeaks = s->GetPositionX();
    //    Double_t *ypeaks = s->GetPositionY();
    // ROOT 5 version
        Float_t *xpeaks = s->GetPositionX();
        Float_t *ypeaks = s->GetPositionY();

    //    std::cout<<"found Npeaks "<<nfound<<""<<std::endl;
    fNPeak=nfound;
    
    if(fGlobalMode->GetGlobalDebugMode() || fGlobalMode->IsCosmicsMode()){
      if(VMax>10 && ElCh>=0){
//	//	std::cout<<ElCh<<" VMax "<<VMax<<std::endl;
	fileOut->cd();
	sprintf(name,"hSig%d",ElCh);
	histo=(TH1D*) hListCal->FindObject(name);
	histo->SetContent(AbsSamRec);
	//	histo->Write(); histo->Reset();
	sprintf(name,"hVMax%d",ElCh);
	histo=(TH1D*) hListCal->FindObject(name);
	histo->Fill(VMax);//CT hVMax contains histogram maximum
      }
//      //CT trying to fill hQCh
//      
//      Int_t Ch   = GetElChID();
      if(ElCh>=0){
	char name[50];
	sprintf(name,"hQCh%d",ElCh);
	histo =(TH1D*) hListCal->FindObject(name);
	//      std::cout<<"ElCh "<<ElCh<<
	//	if(140<xpeaks[0]*fTimeBin && xpeaks[0]<160) 
	histo->Fill(ypeaks[0]);//CT hQCh contains TSpectrum max
	//      
	sprintf(name,"hTime");
	histo =(TH1D*) hListCal->FindObject(name);
	histo->Fill(xpeaks[0]*fTimeBin);
      }
    }
    
   
    for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
      fCharge = 0.;

// ROOT 6 version
//      Double_t xp   = xpeaks[ll];
//      Double_t yp   = ypeaks[ll];
// ROOT 5 version
//      Float_t xp   = xpeaks[ll];
//      Float_t yp   = ypeaks[ll];
      Double_t xp   =s->GetPositionX()[ll];
      Double_t yp   =s->GetPositionY()[ll];

      fTime = xp*fTimeBin; //convert time in ns get it from data

      Int_t bin    = H1->GetXaxis()->FindBin(xp);
      if(bin<1000){
	for (Int_t ii=bin-NIntSamp;ii<bin+NIntSamp;ii++) {
	  //	  if(H1->GetBinContent(ii)>0.003) 
	  fCharge += H1->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;  //charge in pC
          Charge  += H1->GetBinContent(ii)*1e-3/fImpedance*fTimeBin*1e-9/1E-12;
	  //	  std::cout<<"Charge is "<<fCharge<<" "<<ElCh<<std::endl;
	}
	//std::cout<<nfound<<" "<<ll<<" Digi Charge  "<<fCharge<<" Time "<<fTime<<" yp "<<yp<<" xp "<<xp<<" EMeV "<<fCharge/pCMeV<<std::endl;
      }
      
      fEnergy      = fCharge/pCMeV; //this is really the energy
      fCalibEnergy = fCharge/pCMeV*fCalibCh[ElCh]; //calibrated energy of the hit
      
      TRecoVHit *Hit = new TRecoVHit();
      HitE=VMax;
      fESpec=yp;
      if(ElCh>=0) SAC->Fill(); //fil the tree
      if(yp>-3*VMin){
	Hit->SetTime(fTime);
	//	Hit->SetEnergy(fCharge);    // need to add hit status 
	Hit->SetEnergy(yp);    // need to add hit status 
	//	Hit->SetEnergy(fEnergy); //here, if you need, you can change the variable you need (at this point you can only use one)
	//Hit->SetEnergy(yp);               // need to add hit status to avoid saturations
	hitArray.push_back(Hit);
      }else{
	//	fileOut->cd();
	//	H1->Write();
	//	std::cout<<nfound<<" "<<ll<<" Digi Charge  "<<fCharge<<" Time "<<fTime<<" yp "<<yp<<" xp "<<xp<<" EMeV "<<fCharge/pCMeV<<" NImage "<<NImage<<std::endl;
	//	NImage++;
      }  
      //      std::cout<<ll<<" "<<xp<<" yp "<<ypeaks[ll]<<" Ch "<<fCh<<" VMax "<<VMax<<" thr "<<peak_thr<<" "<<fAmpThresholdLow<<std::endl;
    }
    //    std::cout<<"end ch"<<std::endl;
    H1->Reset();
  }
  return fTime = fTime*fTimeBin;
}

Double_t DigitizerChannelSAC::CalcPosition(UShort_t fCh) {
  
   Double_t fPosition;
   //Needs to be implemented
   fPosition = fMeasure[fCh];
   return fPosition;
}
void DigitizerChannelSAC::ReconstructSingleHit(std::vector<TRecoVHit *> &hitArray){
  //Double_t fchPed=CalcPedestal();
  CalcChaTime(hitArray,1000);
  //  IsSaturated(); //check if the signal is saturated //CT
}

void DigitizerChannelSAC::ReconstructMultiHit(std::vector<TRecoVHit *> &hitArray){
  //Double_t fchPed=CalcPedestal();
  CalcChaTime(hitArray,1000);
}

// chiamato per ongi waveform 
void DigitizerChannelSAC::Reconstruct(std::vector<TRecoVHit *> &hitArray){
//  if(fUseAbsSignals) {
//    SetAbsSignals();
//  }
//  Int_t fCh =  hit.GetChannelID();
  //  CalcMaximum();
  // if(fPed - fMax < fMinAmplitude ) return;

  if(fMultihit) {
    ReconstructMultiHit(hitArray);
  } else {
    ReconstructSingleHit(hitArray);
  }
  // std::cout << "Maximum: " << fMax << "  at position: "<< fIMax << std::endl;
  //  std::cout << "Pedestal: " << fCh << "  from: " << fNPedSamples << " samples"<< std::endl;
}

void DigitizerChannelSAC::SetAbsSignals(){
//  for(UShort_t i = 0;i<fNSamples;i++){
//    if (fSamples[i] < 2048) {
//      fSamples[i] = 4096 - fSamples[i];
//    }
//  }
}


DigitizerChannelSAC::~DigitizerChannelSAC(){
  SaveDebugHistos();
  //  hVPeak->Write();
//  fileOut->cd();    
//  for(Int_t ElCh=0;ElCh<25;ElCh++){
//    char name[50];
//    sprintf(name,"hVMax%d",ElCh);
//    hVPeak=(TH1D*) hListCal->FindObject(name);
//    hVPeak->Write();
//    
//    sprintf(name,"hPedCalo%d",ElCh);
//    hMean=(TH1D*) hListCal->FindObject(name);
//    hMean->Write();
//    
//    sprintf(name,"hQCh%d",ElCh);
//    hCharge=(TH1D*) hListCal->FindObject(name);
//    hCharge->Write();
//    
//    sprintf(name,"hChTime%d",ElCh);
//    hTime=(TH1D*) hListCal->FindObject(name);
//    hTime->Write(); 
//  }
};

void DigitizerChannelSAC::SaveDebugHistos(){
  fileOut->cd();
  hListCal->Write();  
  SAC->Write();
  fileOut->Close();
}

// check on saturated signals //CT 
Bool_t DigitizerChannelSAC::IsSaturated(){
  Bool_t IsSaturated=false;
  Short_t min  = TMath::MinElement(1000,&fSamples[0]); 
  Short_t max  = TMath::MaxElement(1000,&fSamples[0]); 
  Short_t nsat = 0;
  Int_t Ch     = GetElChID();
  Int_t BID    = GetBdID();
 
  if(min < 5){ 
    IsSaturated=true;
    if(fGlobalMode->GetGlobalDebugMode()) histoSat = (TH1D*) hListEv->FindObject("hSat"); // swt the debug flag.
    
    for(int ll=1;ll<1001;ll++){
      if(fSamples[ll]<5) nsat++;
      if(fGlobalMode->GetGlobalDebugMode()) histoSat->SetBinContent(ll,fSamples[ll]);
    }

    if(fGlobalMode->GetGlobalDebugMode()) histoSat->Reset();
  }
  return IsSaturated;

};

