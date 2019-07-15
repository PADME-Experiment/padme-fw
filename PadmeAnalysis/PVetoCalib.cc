

pvetocalib() {
  char name[64];
  TFile *reco = new TFile("OutputFileRoot-dt.root","READ","data");

  double mean[96];
  double sigma[96];
  double chindex[96];

  reco->cd();
  TH1D *his;

  for(int i = 0;i<96;i++) {
    sprintf(name,"PVetoSACdT_%d",i);
    chindex[i] = i*1.;

    his = (TH1D *) reco->Get(name);
    double maxbin = his->GetMaximumBin();
    double maxplace =  his->GetBinCenter(maxbin);  //-25. + h1d->GetMaximumBin()*(50./100);
    
    his->Fit("gaus","","",maxplace-0.75,maxplace+0.75);
    TF1 * gfit = (TF1 *)his->GetFunction("gaus");
    if (gfit == NULL) continue;

    mean [i] = gfit->GetParameter(1); 
    sigma [i] = gfit->GetParameter(2); 





  }



  TGraph *grmean = new TGraph(96,chindex,mean);
  TGraph *grsigma = new TGraph(96,chindex,sigma);

  TCanvas *c2 = new TCanvas();
  c2->cd();
  grmean->Draw("ALP");
  
  
  TCanvas *c4 = new TCanvas();
  c4->cd();
  grsigma->Draw("ALP");

  Double_t res[96];
  for(int i = 0;i<96;i++) {
    res[i] = 0;
    if(sigma[i] < 1.25) {
      printf("A timeOffset[%d]=%lf;\n",i,mean[i]);
      res[i] = mean[i] - (30.8972 + 0.13841*i);
    } else {
      printf("B timeOffset[%d]=%lf;\n",i,30.8972 + 0.13841*i);
    }
  }

  TGraph *grres = new TGraph(96,chindex,res);
  TCanvas *c3 = new TCanvas();
  c3->cd();
  grres->Draw("ALP");
  


}
