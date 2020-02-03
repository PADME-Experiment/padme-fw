#include "TMath.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGraph.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "TSpectrum.h"
#include "TMultiGraph.h"
#include "TRandom.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TRatioPlot.h"
#include "TLine.h"
#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <algorithm>

#define NPOINTS 1024
#define NsPerBin 400./1024.
#define BinsPerNs 1024./400.

char name[256];

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

void u_scint(Double_t *t, Double_t *uin){//creates array of 10000 points of exponentially faling signal *after* time = offset with decay constant tau=20e-9s. This exponential doesn't have a non-zero rise time
//Time is in ns!
  float tau=20;//ns
  float risetime = 7.;//ns
  for(int i=0;i<NPOINTS;i++) {
    // Step function at 10 ns -> 1V 
    uin[i] =-1*exp(-(t[i]/risetime))+exp(-(t[i]/tau));
    //      std::cout<<"i " <<i<<" t "<<t[i]<<" uin "<<uin[i]<<std::endl;
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

int main(Int_t argc, char **argv){
  TFile *fileout = TFile::Open("GeneratedSignals.root","RECREATE");
  TTree *tree = new TTree("SigTree","Generated Signal Tree");
    
  Int_t tbins[NPOINTS];//samples
  Double_t twindow[NPOINTS];//holds sample times in ns
  Double_t time_bin_width=400./NPOINTS; //ns - 1 signal window = 400ns long  
  for(int i=0;i<NPOINTS;i++) {
    twindow[i] = i*time_bin_width;
  }

  Double_t singlesignal[NPOINTS];//a single pulse for a particle with arrival time 0 and amplitude 1

  int eventtot = 100;//e3;//number of events to run over
  Int_t TotalToyNo=0;//total number of hits generated
  Int_t lambda=3;//mean number of hits per event
  int zerohiteventnumber=0;

  std::vector<Double_t> arrivaltimens;//time of arrival of signal in ns, rewritten every event
  std::vector<Int_t>    arrivaltimebins;//time of arrival of signal in bins, rewritten every event
  std::vector<Int_t>    sortedarrivaltimebins;//time of arrival of signal in bins, rewritten every event
  
  std::vector<Double_t> hitamp;//Amplitude of signals
  std::vector<Double_t> sortedhitamp;//Amplitude of signals
  
  Double_t sigfinal[NPOINTS];//holds the sum of all individual signals in a single event
  Double_t currentsig[NPOINTS];//temporary container for individual hits, rewritten each hit

  Int_t eventnumber=0;
  Int_t nhits;  
  tree->Branch("HitTimes",&sortedarrivaltimebins);//Hit arrival time in bins, sorted by order of increasing time
  tree->Branch("HitAmplitude",&sortedhitamp);//Hit amplitude, sorted by order of increasing time
  tree->Branch("SigFinal",&sigfinal,"SigFinal[1024]/D");//Final signal - what the DAQ would register, so the sum of all individual hits
  tree->Branch("EventNo",&eventnumber,"EventNo/I");//Index of event
  tree->Branch("HitsPerEvent",&nhits,"HitsPerEvent/I");//Number of hits per event
  tree->Branch("TBins",&tbins,"TBins[1024]/I");//Time in bins

  for (eventnumber =0;eventnumber<eventtot;eventnumber++){
    
    nhits=gRandom->Poisson(lambda);//number of hits follows a Poisson distribution with lambda hits per event window on average
    //    std::cout<<"nhits "<<nhits<<std::endl;
    if(nhits!=0)   u_expo(twindow,singlesignal);
    //if(nhits!=0)   u_gauss(twindow,singlesignal);
    //if(nhits!=0)   u_scint(twindow,singlesignal);
    
    arrivaltimens.clear();//time of arrival of signal in ns
    arrivaltimebins.clear();
    hitamp.clear();
    sortedarrivaltimebins.clear();
    sortedhitamp.clear();
    //     arrivaltimens.push_back(gRandom->Uniform(50));//gRandom->Uniform(400));//initial signal can arrive at any point in the digitizer window
    
    std::fill(sigfinal, sigfinal+NPOINTS, 0);

    if(eventnumber%100==0) std::cout<<"eventnumber "<<eventnumber<<" nhits = "<<nhits<<std::endl;

    //std::cout<<eventnumber;
    for(int i=0; i<nhits; i++) {
      TotalToyNo++;
      std::fill(currentsig, currentsig+NPOINTS, 0);
      hitamp.push_back(gRandom->Uniform(1,10));//TSpectrum looks for peaks of height >=1/10 the maximum amplitude in the signal, so what's its efficiency in reconstructing signals that in principle should all be reconstructable?
      arrivaltimens.push_back(gRandom->Uniform(80,280));
      arrivaltimebins.push_back(int(arrivaltimens[i]*BinsPerNs));
      //std::cout<<"\t"<<arrivaltimebins[i];
      for(int j=0;j<NPOINTS;j++){
	tbins[j]=j;
	if(i==0&&j<arrivaltimebins[i]) {//before the arrival of the first hit, the signal is 0
	  sigfinal[j]=0;
	  currentsig[j]=0;
	}
	else if(i==0&&j>=arrivaltimebins[0]){//once the first signal has arrived, the signal is a singlesignal shifted by arrivaltimebins[0]
	  sigfinal[j]=(hitamp[i]*singlesignal[j-arrivaltimebins[0]]);
	  currentsig[j]=(hitamp[i]*singlesignal[j-arrivaltimebins[0]]);
	}
	else if(i>0&&j>=arrivaltimebins[i]){//after the arrival of subsequent signals, the signal is the sum of the new signal and all previous signals
	  sigfinal[j]+=hitamp[i]*singlesignal[j-arrivaltimebins[i]];
	  currentsig[j]=(hitamp[i]*singlesignal[j-arrivaltimebins[i]]);
	}
	else if(i>0&&j<arrivaltimebins[i]) 	  currentsig[j]=0;
	if(eventnumber==eventtot-1){
	}
      }
      //      std::cout<<sizeof(currentsig)/sizeof(currentsig[0])<<std::endl;
      
    }
    //    std::cout<<std::endl;

    //  for(int ii=1;ii<nhits;ii++) std::cout<<"\t"<<arrivaltimebins[ii]-arrivaltimebins[ii-1];
    //    std::cout<<std::endl;

    std::vector<int> index(arrivaltimebins.size(), 0);
  
    for (int i = 0 ; i != index.size() ; i++) {
      index[i] = i;
    }

    sort(index.begin(), index.end(),
	 [&](const int& a, const int& b) {
	   return (arrivaltimebins[a] < arrivaltimebins[b]);
	 }
	 );

    for (int ii = 0 ; ii != index.size() ; ++ii) {
      sortedhitamp.push_back(hitamp[index[ii]]);
      sortedarrivaltimebins.push_back(arrivaltimebins[index[ii]]);
    }

    // for(int ii=1;ii<nhits;ii++) std::cout<<"\t"<<sortedarrivaltimebins[ii]-sortedarrivaltimebins[ii-1];
    // std::cout<<std::endl;

    
    if(nhits==0){
      zerohiteventnumber++;
    }
    tree->Fill();
  }
  fileout->Write();
  delete fileout;
}
