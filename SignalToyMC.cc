#define NPOINTS 1024
#define BinsPerNs 1024./400

void u_expo(Double_t *t, Double_t *uin){//creates array of 10000 points of linearly rising and then exponentially faling signal after time = risetime
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

void SignalToyMC(){

  Double_t twindow[NPOINTS];//ns
  Double_t singlesignal[NPOINTS];
  vector<Double_t> sigfinal;//holds the sum of all individual signals
  Int_t tbins[NPOINTS];//samples

  Double_t time_bin_width=400./NPOINTS; //ns - 1 signal window = 400ns long
  for(int i=0;i<NPOINTS;i++) {
    twindow[i] = i*time_bin_width;
    tbins[i] = i;
  }

  Int_t lambda=3;//mean number of hits per event

  Int_t nhits=TRandom::gRandom->Poisson(lambda);//number of hits follows a Poisson distribution with lambda hits per event window on average
  std::cout<<"nhits = "<<nhits<<std::endl;
  if(nhits!=0)   u_expo(twindow,singlesignal);

  vector<Double_t> arrivaltimens;//time of arrival of signal in ns
  vector<Int_t>    arrivaltimebins;
  arrivaltimens.push_back(TRandom::gRandom->Uniform(200));//TRandom::gRandom->Uniform(400));//initial signal can arrive at any point in the digitizer window

  vector< vector<Double_t> > signals;//holds the individual signals coming from each hit

  for(int i=0;i<nhits;i++) {
    vector<Double_t> currentsig;
    if (i<nhits-1){//for every hit before the last, create another hit afterwards
      if(arrivaltimens[0]<200) arrivaltimens.push_back(TRandom::gRandom->Uniform(arrivaltimens[0],arrivaltimens[0]+200));//the beam window is 200ns long, so any hits have to come within 200ns of the original signal
      else arrivaltimens.push_back(TRandom::gRandom->Uniform(arrivaltimens[0],400));//if the original hit comes after 200ns then the maximum time of arrival of any hit is 400ns
    }
    
    arrivaltimebins.push_back(int(arrivaltimens[i]*BinsPerNs));
    for(int j=0;j<NPOINTS;j++){
      if(i==0&&j<arrivaltimebins[i]) {
	sigfinal.push_back(0);
	currentsig.push_back(0);
      }
      else if(i==0&&j>=arrivaltimebins[0]){
	sigfinal.push_back(singlesignal[j-arrivaltimebins[0]]);
	currentsig.push_back(singlesignal[j-arrivaltimebins[0]]);
      }
      else if(i>0&&j>=arrivaltimebins[i]){
	vector<Double_t> sig;
	sigfinal[j]+=singlesignal[j-arrivaltimebins[i]];
	currentsig.push_back(singlesignal[j-arrivaltimebins[i]]);
      }
      else if(i>0&&j<arrivaltimebins[i]) currentsig.push_back(0);
    }
    signals.push_back(currentsig);
  }
 
  //  for(int i=1;i<NPOINTS;i++)	std::cout<<i<<"\t"<<sigfinal[i]<<"\t"<<(sigfinal[i]-sigfinal[i-1])<<std::endl;//(sigfinal[j]-sigfinal[j-1]) is the gradient in counts/bin
  
  //  for(int i=0;i<nhits;i++)    for(int j=0;j<NPOINTS;j++) std::cout<<"i "<<i<<"\t j "<<j<<"\t"<<sigfinal[j]<<"\t"<<signals[i][j]<<std::endl;

   TCanvas *c = new TCanvas("c","",0,0,800,600);
   //TGraph *gsigfinal = new TGraph(tbins,sigfinal);
   // gsigfinal->Draw("al");

   TMultiGraph *gfinal = new TMultiGraph();
   for(i=0;i<nhits;i++){
       TGraph *gsig = new TGraph();     
       for(j=0;j<NPOINTS;j++) gsig->SetPoint(j,tbins[j],signals[i][j]);
       gsig->SetLineColor(kRed);
       gfinal->Add(gsig,"l");
   }
   TGraph *gsum = new TGraph();
   for(i=0;i<NPOINTS;i++){
     if(nhits>0) gsum->SetPoint(i,tbins[i],sigfinal[i]);
     else gsum->SetPoint(i,tbins[i],0);
   }
   gfinal->Add(gsum,"l");
   gfinal->Draw("same");

   Int_t fProcessing=2;

   if(nhits>0) CalcChaTime(sigfinal,fProcessing);

   for(int i=0;i<nhits;i++)    std::cout<<"arrivaltimens["<<i<<"]= "<<arrivaltimens[i]<<" arrivaltimebins["<<i<<"]= "<<arrivaltimebins[i]<<std::endl;

}


Double_t CalcChaTime(vector<Double_t> fSamples, Int_t fProcessing) {
  std::cout<<"In CalcChaTime"<<std::endl;
  if(fProcessing==1)  Double_t fPeakSearchWidth=50;
  if(fProcessing==2)  Double_t fPeakSearchWidth=10;
  Double_t fAmpThresholdLow=10;
  Double_t fTimeBin=0.4;
  Int_t npeaks = 10;
  Int_t nfound;
  //  static TSpectrum SpectrumProcessor(20);// = new TSpectrum(20);
  double Time   = 0.;

  //currently looking for peaks with TSpectrum to obtain multi hit times
  Double_t AbsSamRec[1024];
  Double_t AbsSamRecDP[1024];

  for(UShort_t i=0;i<1024;i++){
    AbsSamRec[i] = (fSamples[i])/4096.*1000.; //in mV positivo
    //    std::cout<< i << "\t"  << fSamples[i]  <<"\tV "<< AbsSamRec[i]  <<std::endl;
  }

  TH1D* H1 = new TH1D("","", 1024,0.,1024.);
  std::cout<<"fProcessing = "<<fProcessing<<std::endl;
  if(fProcessing==1)       H1->SetContent(AbsSamRec); //+ve mV

  else if(fProcessing==2){
    DigitalProcessingRRC(AbsSamRec,AbsSamRecDP,1024-1 ,fTimeBin*1e-9);
    H1->SetContent(AbsSamRecDP);
  }

   for(Int_t j=0;j<1024;j++)     std::cout<< j << "\t"  << fSamples[j]  <<"\tV "<< AbsSamRec[j]<<"\t"<<H1->GetBinContent(j)<<std::endl;

   TCanvas *cl = new TCanvas("l","",0,0,800,600);
   H1->Draw();

  Double_t VMax = H1->GetMaximum(); //mV
  Double_t VMin = H1->GetMinimum(); //mV

  TSpectrum *s = new TSpectrum(2*npeaks);
  //  TSpectrum *s = &SpectrumProcessor;// new TSpectrum(2*npeaks);  //npeaks max number of peaks
  //  Double_t peak_thr  = fAmpThresholdLow /VMax; //minimum peak height allowed - peaks with amplitude less than threshold*highest_peak are discarded. 0<threshold<1
  
  nfound = s->Search(H1,fPeakSearchWidth,"nobackground",0.1);     
  std::cout<<"TSpectrum finds "<<nfound<<" peaks"<<std::endl;

  for(Int_t ll=0;ll<nfound;ll++){ //peak loop per channel
    Double_t xp = (s->GetPositionX())[ll];
    Double_t yp = (s->GetPositionY())[ll];
    std::cout<<ll<<"\t"<<xp<<std::endl;
    Time=xp*fTimeBin;
    Int_t bin    = H1->GetXaxis()->FindBin(xp);
    }

  Time = Time*fTimeBin;
  return Time;

}


void DigitalProcessingRRC(Double_t *uin, Double_t *uout,int NPOINTS, Double_t timebin) { //Beth, implemented from Venelin's idea 06/2019
  // Double_t R1=1300;//ohms
  // Double_t R2=100.; //ohms
  // Double_t C=0.015e-9; //nF


  //simulating RRC circuit. Circuit diagram in presentation from Beth 19/06/2019 
  //approximating voltage output to:
  //dQ/dt=(uin(t)/R2)-((R1+R2)/CR1R2)*Q(t)
  //uout(t)=uin(t)-Q(t)/C


  Double_t R1=650;//ohms
  Double_t R2=100; //ohms
  Double_t C=0.03e-9; //nF

  
  //Calculating the output pulse:
  Double_t integr=0;
  
  static Double_t ic[1024];

  Double_t bin_width=timebin;

  integr=0;
  ic[0]= uin[0]/R2;

  for(int i=1;i<NPOINTS;i++) {
    integr+=ic[i-1]*bin_width; //integr = intgrated charge = charge of this bin + charge of previous bin + bin before...
    ic[i]= uin[i]/R2 - ((R1+R2)/(C*R1*R2))* integr; //ic = current through capacitor = dQ/dt
    uout[i] = uin[i] - integr/(C);
  }  

}
