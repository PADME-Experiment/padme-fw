#include <iostream>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TSpectrum.h"
#include "TPDF.h"

#define NPOINTS 1024
#define BinsPerNs 1024./400.
#define NoEventsToPrint 10

char name[256];
Int_t fProcessing = 2;

void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int npoints, Double_t timebin);

TGraph *GraphFilling(Int_t x[],Double_t y[]);

void GraphDrawing(TGraph *graph[], TGraph *procgraph[], Int_t NoEvents);

Int_t CalcChaTime(Double_t fSamples[], Int_t fProcessing, TH1D* hTSpec, TH1D* hPreTSpec, TCanvas* cTSpec[NoEventsToPrint], Int_t& TSpecNo, Int_t EventNo, std::vector<Double_t> *Time);

int main(Int_t argc, char **argv){
  TFile *file = TFile::Open("GeneratedSignals.root","update");
  if (!file) {
    std::cout<<"No file named GeneratedSignals.root found"<<std::endl;
    return 0;
  }
  
  TTree *readtree;
  file->GetObject("SigTree",readtree);
  
  Int_t TotEvents=readtree->GetEntries();
  
  Double_t sigfinal[1024];//Final signal - what the DAQ would register, so the sum of all individual hits
  Double_t  sigProc[NPOINTS];

  Int_t EventIndex;//Index of event
  Int_t HitsPerEvent;//Number of hits per event
  Int_t tbins[NPOINTS];//Time in bins
  
  std::vector<Int_t> *HitTimes = 0;//Hit arrival time in bins
  std::vector<Double_t> *HitAmp = 0;//Hit amplitude

  //read branches written in SignalGeneratorToy.cpp
  readtree->SetBranchAddress("SigFinal",sigfinal);
  readtree->SetBranchAddress("EventNo",&EventIndex);
  readtree->SetBranchAddress("HitsPerEvent",&HitsPerEvent);
  readtree->SetBranchAddress("HitTimes",&HitTimes);//Hit arrival time in bins
  readtree->SetBranchAddress("HitAmplitude",&HitAmp);
  readtree->SetBranchAddress("TBins",&tbins);//Runs from 1 to 1024

  std::vector<Double_t> *tProcRecoBins= new std::vector<Double_t>; //Reconstructed hit times from TSpectrum on raw data
  std::vector<Double_t> *tUnProcRecoBins =new std::vector<Double_t>; //Reconstructed hit times from TSpectrum on RRC processed data

  Int_t NoRecoUnProc=0;//Number of hits reconstructed by TSpectrum without digital processing
  Int_t NoRecoProc=0;//Number of hits reconstructed by TSpectrum from digitally processed signals
  
  //create new branch for reconstructed time
  TBranch *BTRecoUnProc=readtree->Branch("TRecoUnProc","vector<Double_t>",&tUnProcRecoBins);
  TBranch *BTRecoProc  =readtree->Branch("TRecoProc","vector<Double_t>",&tProcRecoBins);
  TBranch *BNoRecoUnProc=readtree->Branch("NoRecoUnProc",&NoRecoUnProc,"NoRecoUnProc/I");
  TBranch *BNoRecoProc=readtree->Branch("NoRecoProc",&NoRecoProc,"NoRecoProc/I");
  
  readtree->Print();
  std::cout<<TotEvents<<std::endl;

  Int_t TotalTSpecUnProcNo=0;//total number of hits in all event found by TSpectrum **without** RRC Processing
  Int_t TotalTSpecProcNo=0;//total number of hits in all event found by TSpectrum with RRC Processing
  
  TH1D *hRaw = new TH1D("hRaw","hRaw", 1024,0.,1024.);//Signal before digital processing
  TH1D *hTSpecRecoUnProc = new TH1D("hTSpecRecoUnProc","hTSpecRecoUnProc", 1024,0.,1024.);//Signal after TSpectrum (will have markers of TSpectrum-found peaks)
  TH1D *hTSpecRecoProc = new TH1D("hTSpecRecoProc","hTSpecRecoProc", 1024,0.,1024.);//Signal after TSpectrum && digital processing (will have markers of TSpectrum-found peaks)

  TCanvas *cTSpectrum[NoEventsToPrint];
  TCanvas *cArrivalTimeSpec = new TCanvas("cArrivalTimeSpec","cArrivalTimeSpec",0,0,800,800);//Spectrum of difference in arrival times (and reconstructed times) of consecutive hits
  TCanvas *cRatioUnProc = new TCanvas("cRatioUnProc","cRatioUnProc",0,0,800,800);//Ratio of reconstructed hit time differences to ToyMC created hit time differences
  TCanvas *cRatioProc = new TCanvas("cRatioProc","cRatioProc",0,0,800,800);//Ratio of reconstructed hit time differences from RRC Processed signals to ToyMC created hit time differences
 
  TGraph  *grapharr[NoEventsToPrint];
  TGraph  *procgrapharr[NoEventsToPrint];
  
  TCanvas *cFinal= new TCanvas(name,name,0,0,800,800);//TSpectrum results
  cFinal->Print("TSpectrumResults.pdf[");//opens the file but doesn't print anything

  for(Int_t eventno=0;eventno<TotEvents;eventno++){
    readtree->GetEntry(eventno);
    DigitalProcessingRRC(sigfinal,sigProc,1024,0.4);
    if(eventno<NoEventsToPrint){
      grapharr[eventno] = new TGraph();
      if(fProcessing==2) procgrapharr[eventno] = new TGraph();
      for(int ii=0;ii<NPOINTS;ii++){
	grapharr[eventno]->SetPoint(ii,tbins[ii],sigfinal[ii]);
	if(fProcessing==2) procgrapharr[eventno]->SetPoint(ii,tbins[ii],sigProc[ii]);
      }
    }
    
    tProcRecoBins->clear();
    tUnProcRecoBins->clear();

    NoRecoUnProc = 10;//CalcChaTime(sigfinal,1, hTSpecRecoUnProc, hRaw, cTSpectrum, TotalTSpecUnProcNo,eventno,tUnProcRecoBins);
    std::sort(tUnProcRecoBins->begin(),tUnProcRecoBins->end());
    
    if(fProcessing==2){
      NoRecoProc = 5;//CalcChaTime(sigfinal,2, hTSpecRecoProc, hRaw, cTSpectrum, TotalTSpecProcNo,eventno,tProcRecoBins);
      std::sort(tProcRecoBins->begin(),tProcRecoBins->end());
    }

    if(fProcessing==1&&eventno%100==0) std::cout<<"Event "<<eventno<<" HitsPerEvent "<<HitsPerEvent<<" NoRecoUnProc "<<NoRecoUnProc<<std::endl;
    if(fProcessing==2&&eventno%100==0) std::cout<<"Event "<<eventno<<" HitsPerEvent "<<HitsPerEvent<<" NoRecoUnProc "<<NoRecoUnProc<<" NoRecoProc "<<NoRecoProc<<std::endl;

    BNoRecoUnProc->Fill();
    BNoRecoProc->Fill();
    BTRecoUnProc->Fill();
    BTRecoProc->Fill();
  }
  
  cFinal->Print("TSpectrumResults.pdf]");//closes the file
  GraphDrawing(grapharr,procgrapharr,NoEventsToPrint);
  readtree->Write();
  
  delete file;
}

void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int npoints, Double_t timebin) { //Beth, implemented from Venelin's idea 06/2019
  // Double_t R1=1300;//ohms
  // Double_t R2=100.; //ohms
  // Double_t C=0.015e-9; //nF

  //simulating RRC circuit. Circuit diagram in presentation from Beth 19/06/2019 
  //approximating voltage output to:
  //dQ/dt=(uin(t)/R2)-((R1+R2)/CR1R2)*Q(t)
  //uout(t)=uin(t)-Q(t)/C


  Double_t R1=1300;//ohms
  Double_t R2=100; //ohms
  Double_t C=0.015; //nF

  
  //Calculating the output pulse:
  Double_t integr=0;
  
  static Double_t ic[1024];

  Double_t bin_width=timebin;

  integr=0;
  ic[0]= uin[0]/R2;
  
  for(int i=1;i<npoints;i++) {
    integr+=ic[i-1]*bin_width; //integr = intgrated charge = charge of this bin + charge of previous bin + bin before...
    ic[i]= uin[i]/R2 - ((R1+R2)/(C*R1*R2))* integr; //ic = current through capacitor = dQ/dt
    uout[i] = uin[i] - integr/(C);
  }  
}

void GraphDrawing(TGraph *graph[], TGraph *procgraph[], Int_t NoEvents){
  TCanvas *canvasarr[NoEventsToPrint];
  
  for(int ii=0;ii<NoEvents;ii++){
    TMultiGraph *gfinal = new TMultiGraph(); 
    sprintf(name, "cToy%d", ii);
    canvasarr[ii]=new TCanvas(name,name,0,0,800,800);
    canvasarr[ii]->cd();
    if(fProcessing==2) procgraph[ii]->SetLineColor(kBlue);
    gfinal->Add(graph[ii],"l");
    if(fProcessing==2) gfinal->Add(procgraph[ii],"l");
    gfinal->Draw("same");
    if(ii<NoEvents-1) canvasarr[ii]->Print("SampleSignals.pdf(");
    if(ii==NoEvents-1) canvasarr[ii]->Print("SampleSignals.pdf)");
  }
}

//std::vector<Double_t> *CalcChaTime(Double_t fSamples[], Int_t fProcessing, TH1D* hTSpec, TH1D* hPreTSpec, TCanvas* cTSpec[NoEventsToPrint], Int_t& TSpecNo, Int_t EventNo){
Int_t CalcChaTime(Double_t fSamples[], Int_t fProcessing, TH1D* hTSpec, TH1D* hPreTSpec, TCanvas* cTSpec[NoEventsToPrint], Int_t& TSpecNo, Int_t EventNo, std::vector<Double_t> *Time){
  hTSpec->Reset();
  hPreTSpec->Reset();

  Double_t fPeakSearchWidth=0.;
  if(fProcessing==1)  fPeakSearchWidth=0.3*BinsPerNs;
  if(fProcessing==2)  fPeakSearchWidth=13;
  Double_t fAmpThresholdLow=10;
  Double_t fTimeBin=0.4;
  Int_t npeaks = 10;
  Int_t nfound=0;
  //  static TSpectrum SpectrumProcessor(20);// = new TSpectrum(20);
  //  std::vector<Double_t *> VTime;//

  //currently looking for peaks with TSpectrum to obtain multi hit times
  Double_t AbsSamRec[1024];
  Double_t AbsSamRecDP[1024];

  for(UShort_t i=0;i<1024;i++){
    AbsSamRec[i] = (fSamples[i])/4096.*1000.; //in mV positivo
    //    std::cout<< i << "\t"  << fSamples[i]  <<"\tV "<< AbsSamRec[i]  <<std::endl;
  }

  //  std::cout<<"fProcessing = "<<fProcessing<<std::endl;
  
  if(fProcessing==1){
    for(int i=0;i<1000;i++){
      hTSpec->SetBinContent(i,AbsSamRec[i]);
    }
    for(int i=1000;i<1024;i++){
      hTSpec->SetBinContent(i,0);
    }
  }
  if(fProcessing==2){
    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,1024,fTimeBin);
    for(int i=0;i<1000;i++){
      hTSpec->SetBinContent(i,AbsSamRecDP[i]);
    }
    for(int i=1000;i<1024;i++){
      hTSpec->SetBinContent(i,0);
    }
  }
  for(int i=0;i<1000;i++){
    hPreTSpec->SetBinContent(i,AbsSamRec[i]);
  }
  for(int i=1000;i<1025;i++){
    hPreTSpec->SetBinContent(i,0);
  }
    //  hPreTSpec->SetContent(AbsSamRec);
  //cTSpec->cd();
  Double_t VMax = hTSpec->GetMaximum(); //mV
  Double_t VMin = hTSpec->GetMinimum(); //mV

  Double_t VMax2 = hPreTSpec->GetMaximum(); //mV
  Double_t VMin2 = hPreTSpec->GetMinimum(); //mV

  TSpectrum *s = new TSpectrum(2*npeaks);
  //  TSpectrum *s = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks

  //  Double_t peak_thr  = fAmpThresholdLow /VMax; //minimum peak height allowed - peaks with amplitude less than threshold*highest_peak are discarded. 0<threshold<1
  
  if(fProcessing==1)  nfound = s->Search(hTSpec,fPeakSearchWidth,"nobackground, nodraw",0.1);
  if(fProcessing==2)  nfound = s->Search(hTSpec,fPeakSearchWidth,"nobackground, nodraw",0.1);     
  //std::cout<<"TSpectrum finds "<<nfound<<" peaks"<<std::endl;

  if(EventNo<NoEventsToPrint){
    if (fProcessing==1)  sprintf(name,"cTSpectrum%d",EventNo);
    else if (fProcessing==2)  sprintf(name,"cTSpectrumProc%d",EventNo);
    cTSpec[EventNo]= new TCanvas(name,name,0,0,800,800);//TSpectrum results
    cTSpec[EventNo]->cd();
    hPreTSpec->Draw();
    hTSpec->Draw("same");
    cTSpec[EventNo]->Print("TSpectrumResults.pdf");
  }

  for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
    TSpecNo++;
    Double_t xp = (s->GetPositionX())[ll];
    Double_t yp = (s->GetPositionY())[ll];
    //    std::cout<<"TSpectrum peak "<<ll<<" xp "<<xp<<" yp "<<yp<<std::endl;
    Time->push_back(xp);
  }

  //  std::cout<<"nfound "<<nfound<<" Time->size() "<<Time->size()<<std::endl;
  
  return Int_t(Time->size());
}
