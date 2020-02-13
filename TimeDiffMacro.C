#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include "TH1.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TRatioPlot.h"
#include "TLine.h"
 
#define NPOINTS 1024
#define NsPerBin 400./1024.
#define BinsPerNs 1024./400.
 
char name[256];
Int_t fProcessing = 2;
 
void TimeDiffMacro(){
  TFile *file = TFile::Open("GeneratedSignals.root","read");
  if (!file) {
    std::cout<<"No file named GeneratedSignals.root found"<<std::endl;
    return;
  }
   
  TTree *readtree;
  file->GetObject("SigTree",readtree);
   
  Long64_t TotEvents=readtree->GetEntries();
  // std::cout<<"TotEvents: "<<TotEvents<<std::endl;
  // Int_t EventNumbers;//Index of event
  Int_t HitsPerEvent;//Number of hits per event
  Int_t NoUnProcReco;
  Int_t NoProcReco;
   
  std::vector<Int_t> *HitTimes=new std::vector<Int_t>;//Hit arrival time in bins
  std::vector<Double_t> *tProcRecoBins=new std::vector<Double_t>; //Reconstructed hit times from TSpectrum on raw data
  std::vector<Double_t> *tUnProcRecoBins=new std::vector<Double_t>; //Reconstructed hit times from TSpectrum on RRC processed data
 
  //  readtree->SetBranchAddress("EventNo",&EventNumbers);
  readtree->SetBranchAddress("HitsPerEvent",&HitsPerEvent);  
  readtree->SetBranchAddress("HitTimes",&HitTimes);
  readtree->SetBranchAddress("TRecoUnProc",&tUnProcRecoBins);
  readtree->SetBranchAddress("TRecoProc",&tProcRecoBins);
  readtree->SetBranchAddress("NoRecoUnProc",&NoUnProcReco);
  readtree->SetBranchAddress("NoRecoProc",&NoProcReco);
  
  std::vector<Int_t> HitArrIndex;//Holds the index in HitTimes of the first hit of each event
 
  Int_t HitCounter;
  HitCounter=0;

  Double_t TimeSpecEnds =400*NsPerBin;//largest time difference (in ns) to be shown in time spectra
  //  std::cout<<"TimeSpecEnds = "<<TimeSpecEnds<<std::endl;
  
  TCanvas *cArrivalTimeSpec = new TCanvas("cArrivalTimeSpec","cArrivalTimeSpec",0,0,800,800);//Spectrum of difference in arrival times (and reconstructed times) of consecutive hits
  TCanvas *cRatioUnProc = new TCanvas("cRatioUnProc","cRatioUnProc",0,0,800,800);//Ratio of reconstructed hit time differences to ToyMC created hit time differences
  TCanvas *cRatioProc = new TCanvas("cRatioProc","cRatioProc",0,0,800,800);//Ratio of reconstructed hit time differences from RRC Processed signals to ToyMC created hit time differences

  TH1F *hArrivalTimeSpec = new TH1F("ArrivalTimeSpec","ArrivalTimeSpec",100,0,TimeSpecEnds);//Spectrum of difference in arrival times of consecutive hits (ns)
  TH1F *hRecoTimeSpecProc = new TH1F("RecoTimeSpecProc","RecoTimeSpecProc",100,0,TimeSpecEnds);//Spectrum of difference in reconstructed times of consecutive hits (RRC Processing) (ns)
  TH1F *hRecoTimeSpecUnProc = new TH1F("RecoTimeSpecUnProc","RecoTimeSpecUnProc",100,0,TimeSpecEnds);//Spectrum of difference in reconstructed times of consecutive hits (raw data) (ns)

  for(Long64_t EventIndex=0;EventIndex<TotEvents;EventIndex++){
    readtree->GetEntry(EventIndex);
    if(EventIndex%100==0) std::cout<<"EventNo "<<EventIndex<<", ToyHits "<<HitsPerEvent<<", UnProcRecoHits "<<NoUnProcReco<<", ProcRecoHits "<<NoProcReco<<std::endl;
    for(Int_t ii=0;ii<HitsPerEvent-1;ii++){
      hArrivalTimeSpec->Fill((HitTimes->at(ii+1)-HitTimes->at(ii))*NsPerBin);
    }
    for(Int_t ii=0;ii<NoUnProcReco-1;++ii){
      std::cout<<"hippos"<<std::endl;
      hRecoTimeSpecUnProc->Fill((tUnProcRecoBins->at(ii+1)-tUnProcRecoBins->at(ii))*NsPerBin);
    }
    for(Int_t ii=0;ii<NoProcReco-1;++ii){
      hRecoTimeSpecProc->Fill((tProcRecoBins->at(ii+1)-tProcRecoBins->at(ii))*NsPerBin);
    }
  }

  double ymax=hRecoTimeSpecUnProc->GetMaximum();
  if(hRecoTimeSpecProc->GetMaximum()>ymax) ymax=hRecoTimeSpecProc->GetMaximum();
  if(hArrivalTimeSpec->GetMaximum()>ymax)  ymax=hArrivalTimeSpec->GetMaximum();

  cArrivalTimeSpec->cd();
  hArrivalTimeSpec->Draw();
  hArrivalTimeSpec->SetMaximum(1.1*ymax);
  hArrivalTimeSpec->GetXaxis()->SetTitle("#Delta T_{hits} (ns)");
  std::cout<<"hArrivalTimeSpec drawn"<<std::endl;
   
  hRecoTimeSpecUnProc->SetLineColor(kBlack);
  hRecoTimeSpecUnProc->Draw("same");
  std::cout<<"hRecoTimeSpecUnProc drawn"<<std::endl;

  if(hRecoTimeSpecProc->GetEntries()!=0){
    hRecoTimeSpecProc->SetLineColor(kRed);
    hRecoTimeSpecProc->Draw("same");
    std::cout<<"hRecoTimeSpecProc drawn"<<std::endl;
  }

  TLegend* legend = new TLegend(0.7,0.5,0.98,0.75);
  gStyle->SetLegendTextSize(0.03);
  legend->AddEntry(hArrivalTimeSpec,"ToyMC hit arrival time");
  legend->AddEntry(hRecoTimeSpecUnProc,"TSpectrum reconstructed time");
  legend->AddEntry((TObject*)0, "(raw data)","");
  if(fProcessing==2){
    legend->AddEntry(hRecoTimeSpecProc,"TSpectrum reconstructed time");
    legend->AddEntry((TObject*)0,"(RRC processed data)","");
  }
  
  legend->Draw("same");

  TRatioPlot *EfficiencyRatioUnProc = new TRatioPlot(hRecoTimeSpecUnProc,hArrivalTimeSpec);
  cRatioUnProc->cd();
  EfficiencyRatioUnProc->Draw();
  EfficiencyRatioUnProc->GetUpperPad()->cd();
  hArrivalTimeSpec->Draw("AH");
  hRecoTimeSpecUnProc->Draw("same");
  EfficiencyRatioUnProc->GetLowerPad()->cd();
  TLine *line = new TLine(0,0.95,TimeSpecEnds,0.95);
  line->SetLineColor(kRed);
  line->Draw("same");
  
  TRatioPlot *EfficiencyRatioProc = new TRatioPlot(hRecoTimeSpecProc,hArrivalTimeSpec);
  cRatioProc->cd();
  EfficiencyRatioProc->Draw();
  EfficiencyRatioProc->GetUpperPad()->cd();
  hArrivalTimeSpec->Draw("AH");
  hRecoTimeSpecProc->Draw("same");
  EfficiencyRatioProc->GetLowerPad()->cd();
  line->Draw("same");
  
  //  std::cout<<"Total generated hits = "<<TotalToyNo<<" Total TSpectrum reconstructed hits no processing = "<<TotalTSpecUnProcNo<<" Total TSpectrum reconstructed hits with processing = "<<TotalTSpecProcNo<<" Number of events skipped for having 0 hits created = "<<zerohiteventnumber<<std::endl;
}
