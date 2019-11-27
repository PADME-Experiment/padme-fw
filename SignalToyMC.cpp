#include "TMath.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <iostream>
#include <stdio.h>
#include "TSpectrum.h"
#include "TMultiGraph.h"
#include "TRandom.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TRatioPlot.h"

#define NPOINTS 1024
#define BinsPerNs 1024./400

void u_expo(Double_t *t, Double_t *uin){//creates array of 1024 points of linearly rising and then exponentially faling signal after time = risetime
  //Time is in ns!
  float tau=20;//ns
  float risetime = 7.;//ns
  for(int i=0;i<NPOINTS;i++) {
    // Step function at 10 ns -> 1V 
    if(t[i] < risetime ) {
      uin[i] =t[i]/risetime;
      //      std::cout<<"i " <<i<<" t "<<t[i]<<" uin "<<uin[i]<<" case1"<<std::endl;
    } else { 
      uin[i] = exp(-(t[i] - risetime)/tau);
      //      std::cout<<"i " <<i<<" t "<<t[i]<<" uin "<<uin[i]<<" case2"<<std::endl;
    }
  }
}

void u_gauss(Double_t *t, Double_t *uin){//creates array of 1024 points of a gaussian signal
  double sigma = 0.7;
  float mu = 20.;
  
  for(int i=0;i<NPOINTS;i++){
    t[i]=i;
    uin[i]=TMath::Gaus(i, mu, sigma, 1);//returns the value of a normalalised gaussian distribution at point i
  }
}

void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int npoints, Double_t timebin);
vector<Double_t> CalcChaTime(vector<Double_t> fSamples, Int_t fProcessing, TH1D* hTSpec, TH1D* hPreTSpec, TCanvas* cTSpec, Int_t& TSpecNo);

void SignalToyMC(){

  Double_t twindow[NPOINTS];//ns
  Double_t singlesignal[NPOINTS];
  Int_t tbins[NPOINTS];//samples

  Double_t time_bin_width=400./NPOINTS; //ns - 1 signal window = 400ns long
  for(int i=0;i<NPOINTS;i++) {
    twindow[i] = i*time_bin_width;
    tbins[i] = i;
  }

  Int_t lambda=3;//mean number of hits per event
  Int_t fProcessing=2;
    
  TCanvas *cToy = new TCanvas("cToy","cToy",0,0,800,600);
  TCanvas *cTSpectrum = new TCanvas("cTSpectrum","cTSpectrum",0,0,800,600);//TSpectrum results
  TCanvas *cTimeDiffUnProc = new TCanvas("cTimeDiffUnProc","cTimeDiffUnProc",0,0,800,600);//Difference in time between arrival of ToyMC created signal and TSpectrum reconstructed peak of raw data
  TCanvas *cTimeDiffProc = new TCanvas("cTimeDiffProc","cTimeDiffProc",0,0,800,600);//Difference in time between arrival of ToyMC created signal and TSpectrum reconstructed peak after RRC processing
  TCanvas *cArrivalTimeSpec = new TCanvas("cArrivalTimeSpec","cArrivalTimeSpec",0,0,800,600);//Spectrum of difference in arrival times (and reconstructed times) of consecutive hits
  TCanvas *cRatioUnProc = new TCanvas("cRatioUnProc","cRatioUnProc",0,0,800,600);//Ratio of reconstructed hit time differences to ToyMC created hit time differences
  TCanvas *cRatioProc = new TCanvas("cRatioProc","cRatioProc",0,0,800,600);//Ratio of reconstructed hit time differences from RRC Processed signals to ToyMC created hit time differences
  
  TH1F *hTimeDiffUnProc = new TH1F("tdiffunproc","tdiffunproc",1024,0,50);//Difference in time between arrival of ToyMC created signal and TSpectrum reconstructed peak (raw data)
  TH1F *hTimeDiffProc = new TH1F("tdiffproc","tdiffproc",1024,0,50);//Difference in time between arrival of ToyMC created signal and TSpectrum reconstructed peak (RRC Processing)
  TH1F *hArrivalTimeSpec = new TH1F("ArrivalTimeSpec","ArrivalTimeSpec",100,0,400);//Spectrum of difference in arrival times of consecutive hits
  TH1F *hRecoTimeSpecProc = new TH1F("RecoTimeSpecProc","RecoTimeSpecProc",100,0,400);//Spectrum of difference in reconstructed times of consecutive hits (RRC Processing)
  TH1F *hRecoTimeSpecUnProc = new TH1F("RecoTimeSpecUnProc","RecoTimeSpecUnProc",100,0,400);//Spectrum of difference in reconstructed times of consecutive hits (raw data)

  TMultiGraph *gfinal = new TMultiGraph();
  TGraph *gsum = new TGraph();
  TGraph *gsumProc = new TGraph();
  
  TH1D *hRaw = new TH1D("hRaw","hRaw", 1024,0.,1024.);//Signal before digital processing
  TH1D *hTSpecRecoUnProc = new TH1D("hTSpecRecoUnProc","hTSpecRecoUnProc", 1024,0.,1024.);//Signal after TSpectrum (will have markers of TSpectrum-found peaks)
  TH1D *hTSpecRecoProc = new TH1D("hTSpecRecoProc","hTSpecRecoProc", 1024,0.,1024.);//Signal after TSpectrum && digital processing (will have markers of TSpectrum-found peaks)

  int currentsighalfpos;
  int finalsighalfpos;

  Int_t TotalToyNo=0;//total number of hits in all event found by TSpectrum
  Int_t TotalTSpecUnProcNo=0;//total number of hits in all event found by TSpectrum **without** RRC Processing
  Int_t TotalTSpecProcNo=0;//total number of hits in all event found by TSpectrum with RRC Processing

  double  sigProc[NPOINTS];
  
  int eventtot = 100e3;//number of events to run over
  for(int eventnumber =0;eventnumber<eventtot;eventnumber++){
    
    Int_t nhits=gRandom->Poisson(lambda);//number of hits follows a Poisson distribution with lambda hits per event window on average
    if(nhits!=0)   u_expo(twindow,singlesignal);
    //if(nhits!=0 ) u_gauss(twindow,singlesignal);
    vector<Double_t> arrivaltimens;//time of arrival of signal in ns
    vector<Int_t>    arrivaltimebins;
    //    arrivaltimens.push_back(gRandom->Uniform(200));//gRandom->Uniform(400));//initial signal can arrive at any point in the digitizer window
    
    vector< vector<Double_t> > signals;//holds the individual signals coming from each hit
    vector<Double_t> sigfinal;//holds the sum of all individual signals in a single event
    vector<Double_t> currentsigmax;
    vector<Double_t> finalsigmax;
    vector<Int_t> sigmaxposition;
    
    for(int i=0;i<nhits;i++) {
      TotalToyNo++;
      vector<Double_t> currentsig;
      Double_t AmpScale=gRandom->Uniform(1,10);//TSpectrum looks for peaks of height >=1/10 the maximum amplitude in the signal, so what's its efficiency in reconstructing signals that in principle should all be reconstructable?
      /*      if (i<nhits-1){//for every hit before the last, create another hit afterwards
        if(arrivaltimens[0]<200) arrivaltimens.push_back(gRandom->Uniform(arrivaltimens[0],arrivaltimens[0]+200));//the beam window is 200ns long, so any hits have to come within 200ns of the original signal
	else arrivaltimens.push_back(gRandom->Uniform(arrivaltimens[0],400));//if the original hit comes after 200ns then the maximum time of arrival of any hit is 400ns
	}*/

      arrivaltimens.push_back(gRandom->Uniform(80,280));
      arrivaltimebins.push_back(int(arrivaltimens[i]*BinsPerNs));
      //std::cout<<"eventnumber "<<eventnumber<<" hit number = "<<i<<" arrivaltimebins[i] "<<arrivaltimebins[i]<<" AmpScale "<<AmpScale<<std::endl;
      sigmaxposition.push_back(arrivaltimebins[i]+7*BinsPerNs);//the position of the signal maximum is the arrival time + the 7ns risetime
      for(int j=0;j<NPOINTS;j++){
	if(i==0&&j<arrivaltimebins[i]) {//before the arrival of the first hit, the signal is 0
	  sigfinal.push_back(0);
	  currentsig.push_back(0);
	}
	else if(i==0&&j>=arrivaltimebins[0]){//once the first signal has arrived, the signal is a singlesignal shifted by arrivaltimebins[0]
	  sigfinal.push_back(AmpScale*singlesignal[j-arrivaltimebins[0]]);
	  currentsig.push_back(AmpScale*singlesignal[j-arrivaltimebins[0]]);
	}
	else if(i>0&&j>=arrivaltimebins[i]){//after the arrival of subsequent signals, the signal is the sum of the new signal and all previous signals
	  sigfinal[j]+=AmpScale*singlesignal[j-arrivaltimebins[i]];
	  currentsig.push_back(AmpScale*singlesignal[j-arrivaltimebins[i]]);
	}
	else if(i>0&&j<arrivaltimebins[i]) currentsig.push_back(0);
	//std::cout<<j<<"\t"<<sigfinal[j]<<"\t"<<currentsig[j]<<std::endl;
	if(eventnumber==eventtot-1){
	  gsum->SetPoint(j,tbins[j],sigfinal[j]);
	  //	  std::cout<<gsig->GetMaximum()<<" but here "<<signals[i][j]<<" "<<tbins[j]<<std::endl;
	}
      }
      signals.push_back(currentsig);      
      currentsigmax.push_back(currentsig[sigmaxposition[i]]);
      finalsigmax.push_back(sigfinal[sigmaxposition[i]]);
      DigitalProcessingRRC(sigfinal.data(),sigProc,1024,0.4);
    }
    
    if(nhits==0)    for(int j=0;j<NPOINTS;j++)     gsum->SetPoint(j,tbins[j],0);
    if(eventnumber==eventtot-1){
      for(int i=0;i<nhits;i++){
	TGraph *gsig = new TGraph();//graph of signals from individial hits in an event
	for(int j=0;j<NPOINTS;j++){
	  gsig->SetPoint(j,tbins[j],signals[i][j]);
	  gsumProc->SetPoint(j,tbins[j],sigProc[j]);
	}
	gsig->SetLineColor(kRed);
	gfinal->Add(gsig,"l");
	gsumProc->SetLineColor(kBlue);
	gfinal->Add(gsumProc,"l");
      }
    }
    if(eventnumber==eventtot-1)    gfinal->Add(gsum,"l");
    
    vector<Double_t> tProcRecoBins; //Reconstructed hit times from TSpectrum on raw data
    vector<Double_t> tUnProcRecoBins; //Reconstructed hit times from TSpectrum on RRC processed data

    if(nhits==0) continue;

    tUnProcRecoBins=CalcChaTime(sigfinal,1, hTSpecRecoUnProc, hRaw, cTSpectrum, TotalTSpecUnProcNo);
    if(fProcessing==2)    tProcRecoBins=CalcChaTime(sigfinal,2, hTSpecRecoProc, hRaw, cTSpectrum, TotalTSpecProcNo);
    if(eventnumber%100==0)    std::cout<<"eventnumber "<<eventnumber<<" nhits = "<<nhits<<" tUnProcRecoBins.size() "<<tUnProcRecoBins.size()<<" tProcRecoBins.size() = "<<tProcRecoBins.size()<<std::endl;
    
    std::sort(arrivaltimebins.begin(),arrivaltimebins.end());//sort arrivaltimebins into time order to then plot the difference in time of hit arrivals
    if(tUnProcRecoBins.size()==0) continue;    
    std::sort(tUnProcRecoBins.begin(),tUnProcRecoBins.end());

    for(int i=0;i<nhits;i++)    {
      hTimeDiffUnProc->Fill(tUnProcRecoBins[i]-arrivaltimebins[i]);
      if(i<nhits-1){
	hArrivalTimeSpec->Fill(arrivaltimebins[i+1]-arrivaltimebins[i]);
	hRecoTimeSpecUnProc->Fill(tUnProcRecoBins[i+1]-tUnProcRecoBins[i]);
      }
    }
    
    if(tProcRecoBins.size()==0) continue;    
    std::sort(tProcRecoBins.begin(),tProcRecoBins.end());

    for(int i=0;i<nhits;i++)    {
      hTimeDiffProc->Fill(tProcRecoBins[i]-arrivaltimebins[i]);
      if(i<nhits-1)	hRecoTimeSpecProc->Fill(tProcRecoBins[i+1]-tProcRecoBins[i]);
      }
    
  }
  cToy->cd();
  gfinal->Draw("same");
  std::cout<<"gfinal entries "<<gfinal->GetListOfGraphs()->GetEntries()<<std::endl;
  std::cout<<"gfinal drawn"<<std::endl;
  
  cTimeDiffUnProc->cd();
  hTimeDiffUnProc->Draw();
  std::cout<<"hTimeDiffUnProc drawn"<<std::endl;
  cTimeDiffProc->cd();
  hTimeDiffProc->Draw();
  std::cout<<"hTimeDiffProc drawn"<<std::endl;

  double ymax=hRecoTimeSpecUnProc->GetMaximum();
  if(hRecoTimeSpecProc->GetMaximum()>ymax) ymax=hRecoTimeSpecProc->GetMaximum();
  if(hArrivalTimeSpec->GetMaximum()>ymax)  ymax=hArrivalTimeSpec->GetMaximum();

  cArrivalTimeSpec->cd();
  hArrivalTimeSpec->Draw();
  hArrivalTimeSpec->SetMaximum(1.1*ymax);
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
  EfficiencyRatioUnProc->GetUpperPad()->cd();
  hArrivalTimeSpec->Draw();
  hRecoTimeSpecUnProc->Draw("same");
  cRatioUnProc->cd();
  EfficiencyRatioUnProc->Draw();

  TRatioPlot *EfficiencyRatioProc = new TRatioPlot(hRecoTimeSpecProc,hArrivalTimeSpec);
  cRatioProc->cd();
  EfficiencyRatioProc->Draw();
  EfficiencyRatioProc->GetUpperPad()->cd();
  hArrivalTimeSpec->Draw();
  hRecoTimeSpecProc->Draw("same");
  
  std::cout<<"Total generated hits = "<<TotalToyNo<<" Total TSpectrum reconstructed hits no processing = "<<TotalTSpecUnProcNo<<" Total TSpectrum reconstructed hits with processing = "<<TotalTSpecProcNo<<std::endl;
  
  }


vector<Double_t> CalcChaTime(vector<Double_t> fSamples, Int_t fProcessing, TH1D* hTSpec, TH1D* hPreTSpec, TCanvas* cTSpec, Int_t& TSpecNo){
  hTSpec->Reset();
  hPreTSpec->Reset();

  Double_t fPeakSearchWidth=0.;
  if(fProcessing==1)  fPeakSearchWidth=0.7*BinsPerNs;
  if(fProcessing==2)  fPeakSearchWidth=13;
  Double_t fAmpThresholdLow=10;
  Double_t fTimeBin=0.4;
  Int_t npeaks = 10;
  Int_t nfound;
  //  static TSpectrum SpectrumProcessor(20);// = new TSpectrum(20);
  vector<Double_t> Time;//

  //currently looking for peaks with TSpectrum to obtain multi hit times
  Double_t AbsSamRec[1024];
  Double_t AbsSamRecDP[1024];

  for(UShort_t i=0;i<1024;i++){
    AbsSamRec[i] = (fSamples[i])/4096.*1000.; //in mV positivo
    //    std::cout<< i << "\t"  << fSamples[i]  <<"\tV "<< AbsSamRec[i]  <<std::endl;
  }

  //  std::cout<<"fProcessing = "<<fProcessing<<std::endl;
  
  if(fProcessing==1)       hTSpec->SetContent(AbsSamRec); //+ve mV
  if(fProcessing==2){
    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,1024-1 ,fTimeBin);
    hTSpec->SetContent(AbsSamRecDP);
  }
  
  hPreTSpec->SetContent(AbsSamRec);
  cTSpec->cd();
  hPreTSpec->Draw();
  hTSpec->Draw("same");
  
  //cTSpec->cd();
  Double_t VMax = hTSpec->GetMaximum(); //mV
  Double_t VMin = hTSpec->GetMinimum(); //mV

  Double_t VMax2 = hPreTSpec->GetMaximum(); //mV
  Double_t VMin2 = hPreTSpec->GetMinimum(); //mV

  TSpectrum *s = new TSpectrum(2*npeaks);
  //  TSpectrum *s = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks

  //  Double_t peak_thr  = fAmpThresholdLow /VMax; //minimum peak height allowed - peaks with amplitude less than threshold*highest_peak are discarded. 0<threshold<1
  
  nfound = s->Search(hTSpec,fPeakSearchWidth,"nobackground, nodraw",0.1);     
  //  std::cout<<"TSpectrum finds "<<nfound<<" peaks"<<std::endl;

  for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
    TSpecNo++;
    Double_t xp = (s->GetPositionX())[ll];
    Double_t yp = (s->GetPositionY())[ll];
    //std::cout<<"TSpectrum peak "<<ll<<" xp "<<xp<<" yp "<<yp<<std::endl;
    Time.push_back(xp);
  }
  
  return Time;

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
