void fitBrems(){
  TFile *f = TFile::Open("250417_analysis/MCData_25000e_0A_ana_new.root");
  TDirectory* dir = (TDirectory*) f->Get("SPA_acceptance;1");
  TFile *outRoot = new TFile("fitBrems.root","RECREATE");


  TH2D *Histo = (TH2D*) dir->Get("SPA_acceptance_SAC_PVeto_EnergyvsChID_inTime_banan");
  
  
  TF1 *f1 = new TF1("f1","[0]+[1]*x+[2]*x*x",30,400);
  Histo->Fit(f1,"R");
  Histo->Draw();
  f1->Draw("same");

  outRoot->Write();
}

void getLineParams(double x1, double y1, double x2, double y2, double &m, double &b) {
    m = (y2 - y1) / (x2 - x1);
    b = y1 - m * x1;
}




void fitBrems2(){
  TFile *f = TFile::Open("250417_analysis/MCData_25000e_0A_ana_new.root");
  TDirectory* dir = (TDirectory*) f->Get("SPA_acceptance;1");
  TFile *outRoot = new TFile("fitBrems.root","RECREATE");

  double x1 = 85, y1 = 386;//upper point of upper line
  double x2 = 3, y2 = 71;//lower point of upper line
  double x3 = 87, y3 = 319;//upper point of lower line
  double x4 = 51, y4 = 136;//lower point of lower line
  double x5 = 51, y5 = 136;//upper point of third line
  double x6 = 13, y6 = 16;//lower point of third line

  double m1, b1;
  getLineParams(x1, y1, x2, y2, m1, b1);
  double m2, b2;
  getLineParams(x3, y3, x4, y4, m2, b2);
  double m3, b3;
  getLineParams(x5, y5, x6, y6, m3, b3);


  TH2D *Histo = (TH2D*) dir->Get("SPA_acceptance_SAC_PVeto_ChIDvsEnergy_inTime_banan");
  
  TProfile *HistoProf = new TProfile("HistoProf", "SPA_acceptance_SAC_PVeto_ChIDvsEnergy_inTime_banan", 86, -446.0,500.0);
  // Histo->Rebin2D(8,1);
  // Histo->GetYaxis()->SetRangeUser(0,415);
  
  TH1D *hproj;
  TH1F *hMeanvsZ = new TH1F("hMeanvsZ","Mean", 86, -446.0,500.);
  TH1F *hSigmavsZ = new TH1F("hSigmavsZ","Sigma", 86, -446.0,500.);


  TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
  
  Histo->Draw();
  //return;
  for (int i = 1; i <= Histo->GetNbinsX(); ++i) {
        for (int j = 1; j <= Histo->GetNbinsY(); ++j) {
            double x = Histo->GetXaxis()->GetBinCenter(i);
            double y = Histo->GetYaxis()->GetBinCenter(j);
            if (y >= m1 * x + b1) {
                Histo->SetBinContent(i, j, 0); // clear above line1
            }
	    if (y <= m2 * x + b2) {
                Histo->SetBinContent(i, j, 0); // clear below line 2
            }
	    if (y <= m3 * x + b3) {
                Histo->SetBinContent(i, j, 0); // clear below line 3
            }
	    else {
	      HistoProf->Fill(x, y, Histo->GetBinContent(i, j));
	    }
        }
    }

  TF1 *f1 = new TF1("f1","[0]+[1]*x+[2]*x*x",-400,500);
  Histo->Fit(f1,"R");
  Histo->Draw("same");
  
  TLine *line1 = new TLine(x1, y1, x2, y2);
  line1->SetLineColor(kRed);
  line1->SetLineWidth(2);
  line1->Draw("same");
  
  TLine *line2 = new TLine(x3, y3, x4, y4);
  line2->SetLineColor(kRed);
  line2->SetLineWidth(2);
  line2->Draw("same");
  
  TLine *line3 = new TLine(x5, y5, x6, y6);
  line3->SetLineColor(kRed);
  line3->SetLineWidth(2);
  line3->Draw("same");
  
  f1->Draw("same");

  // Get parameters of f1 after fitting
  double p0 = f1->GetParameter(0);
  double p1 = f1->GetParameter(1);
  double p2 = f1->GetParameter(2);

  // Define two new functions by shifting f1 up and down by 30 units
  double p0_upper = p0 + 30;
  double p0_lower = p0 - 30;

  // Print the parameters of the original and shifted functions
  std::cout << "Original f1(x): " << p0 << " + " << p1 << " * x + " << p2 << " * x^2" << std::endl;
  std::cout << "Upper f1(x) + 30: " << p0_upper << " + " << p1 << " * x + " << p2 << " * x^2" << std::endl;
  std::cout << "Lower f1(x) - 30: " << p0_lower << " + " << p1 << " * x + " << p2 << " * x^2" << std::endl;

  return;
  TCanvas *c2 = new TCanvas("c2", "c2", 800, 600);
  TF1 *pol2Fit = new TF1("pol2Fit", "pol2", -600, 600);
  HistoProf->Fit(pol2Fit, "R");
  HistoProf->Draw();

  
  outRoot->WriteObject(c1,"c1");
  outRoot->WriteObject(c2,"c2");

  for(int i = -1000; i<= Histo->GetNbinsX();i++) {
    hproj = Histo->ProjectionY("_py",i-10,i+10);
    if(hproj->Integral() > 100) {
      int minbin = hproj->GetMinimumBin();
      int maxbin = hproj->GetMaximumBin();
      //Fit with gauss
      TF1 *f2 = new TF1("f2", "gaus",minbin,maxbin);
      hproj->Fit("f2","R");

      //Get the params
      float mean = f2->GetParameter(1);
      float meanE = f2->GetParError(1);

      float sigma = f2->GetParameter(2);
      float sigmaE = f2->GetParError(2);

      TF1 *f3 = new TF1("f3", "gaus",mean-sigma,mean+sigma);
      hproj->Fit("f3","R");
      float meanNew = f3->GetParameter(1);
      float meanENew = f3->GetParError(1);

      float sigmaNew = f3->GetParameter(2);
      float sigmaENew = f3->GetParError(2);
      
      hMeanvsZ->SetBinContent(i,meanNew);hMeanvsZ->SetBinError(i,meanENew);
      hSigmavsZ->SetBinContent(i,sigmaNew);hSigmavsZ->SetBinError(i,sigmaENew);
      //hDeltaMean->SetBinContent(i,-(mean-2*i - 1 ));
       delete f2;
       delete f3;
      
    }
  }

  
  TCanvas *c3 = new TCanvas("c3", "c3", 800, 600);
  hMeanvsZ->Draw();
  TCanvas *c4 = new TCanvas("c4", "c4", 800, 600);
  hSigmavsZ->Draw();
  
  

  outRoot->WriteObject(c3,"c3");
  outRoot->WriteObject(c4,"c4");

  TH1F *hRMSvsZ = new TH1F("hRMSvsZ", "RMS per Bin", 86, -446.0,500.);

  for (int i = 1; i <= HistoProf->GetNbinsX(); ++i) {
      double x = HistoProf->GetBinCenter(i);
      double y_obs = HistoProf->GetBinContent(i);
      double y_fit = pol2Fit->Eval(x);
      
      double rms = sqrt(pow(y_obs - y_fit, 2));
      
      hRMSvsZ->SetBinContent(i, rms);
  }

  TCanvas *c5 = new TCanvas("c5", "RMS HistoProf", 800, 600);
  hRMSvsZ->Draw();
  

  outRoot->WriteObject(c5,"c5");

  
}
