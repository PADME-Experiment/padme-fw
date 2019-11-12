void compare(std::string hname, double scalef=-1, TFile* fData=_file0, TFile* fMC=_file1)
{
  //  TFile* fData = _file0;
  //  TFile* fMC   = _file1;

  TH1D* hData = (TH1D*)fData->Get(hname.c_str());
  TH1D* hMC   = (TH1D*)fMC->Get(hname.c_str());
  
  TCanvas* cData = new TCanvas("data","data",500,500);
  hData->Draw();
  TCanvas* cMC = new TCanvas("mc","mc",500,500);
  hData->Draw();
  //  return;

  if (scalef<0) 
    {
      // scale factor is not given; compute it 
      TH1D* hDN = (TH1D*)fData->Get("nPOT");
      TH1D* hMCN = (TH1D*)fMC->Get("nPOT");
      double nPOTdata = hDN->GetBinContent(2);
      //      std::cout<<"Total num. of POTs is Data = "<<nPOTdata<<std::endl;
      double nPOTMC = hMCN->GetBinContent(2);
      TH1D* hPOTdisMC=(TH1D*)fMC->Get("NposInBunch_beam"); double sc = 20000./hPOTdisMC->GetMean();
      TH1D* hPOTdisDT=(TH1D*)fData->Get("NposInBunch_beam"); 
      std::cout<<"Total num. of POTs is data = "<<nPOTdata<<std::endl;
      std::cout<<"Total num. of POTs is MC   = "<<nPOTMC  <<std::endl;
      std::cout<<"Average nPOT/bunch in data = "<<hPOTdisDT->GetMean()<<std::endl;
      std::cout<<"Average nPOT/bunch in MC   = "<<hPOTdisMC->GetMean()<<std::endl; 
      nPOTMC =nPOTMC*sc;
      std::cout<<"Total CORR num. of POTs is MC   = "<<nPOTMC<<std::endl;
      scalef = nPOTdata/nPOTMC;
      std::cout<<"Scaling factor for MC is "<<scalef<<std::endl;
    }

  TH1D* hsMC = hMC->Clone();
  hsMC->Scale(scalef);
  double ymaxMC = hsMC->GetMaximum();
  double ymaxDT = hsMC->GetMaximum();

  double ymax = ymaxMC;
  if (ymaxDT>ymax) ymax=ymaxDT;
  ymax = 1.3*ymax;
  
  TH1D* hFrame = hMC->Clone();
  hFrame->Scale(0.);
  hFrame->SetMaximum(ymax);

  hFrame->Draw();
  hsMC->SetLineColor(kBlack);
  hsMC->Draw("SAME");
  hData->SetMarkerStyle(20);
  hData->SetMarkerSize(0.5);
  hData->SetMarkerColor(kBlack);
  hData->Draw("SAMEPE");

  cout<<"Data/MC ratio = "<<hData->GetEntries()/hMC->GetEntries()<<std::endl;
  cout<<"Data/sMC ratio = "<<hData->GetEntries()/(scalef*hMC->GetEntries())<<std::endl;

  TH1D* hratio = hMC->Clone();
  hratio->Scale(0.);
  int nbinx = hratio->GetNbinsX();
  double rat, erat;
  for (int i=1; i<nbinx+1; ++i)
    {
      rat =0.;
      erat =0.;
      if (fabs(hsMC->GetBinContent(i))>1e-10) {
	rat = hData->GetBinContent(i)/hsMC->GetBinContent(i);
	erat = sqrt(hData->GetBinContent(i)+scalef*scalef*hMC->GetBinContent(i))/hsMC->GetBinContent(i);
      }
      hratio->SetBinContent(i, rat); 
      hratio->SetBinError(i, erat); 
      std::cout<<"Bin "<<i<<"  ratio = "<<hratio->GetBinContent(i)<<std::endl;
      std::cout<<"Bin "<<i<<" eratio = "<<hratio->GetBinError(i)<<std::endl;
    }
  TCanvas* c2 = new TCanvas("ratio","ratio",600,300);
  hratio->Draw();

   // Define the Canvas
   TCanvas *c = new TCanvas("c", "canvas", 800, 800);

   // Upper plot will be in pad1
   TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
   pad1->SetBottomMargin(0); // Upper and lower plot are joined
   //pad1->SetGridx();         // Vertical grid
   pad1->Draw();             // Draw the upper pad: pad1
   pad1->cd();               // pad1 becomes the current pad

   /*
   h1->SetStats(0);          // No statistics on upper plot
   h1->Draw();               // Draw h1
   h2->Draw("same");         // Draw h2 on top of h1
   */
   hFrame->SetStats(0);  
   hsMC->SetStats(0);  
   hData->SetStats(0);  
   hFrame->Draw();
   hsMC->Draw("SAME");
   hData->Draw("SAMEPE");


   // Do not draw the Y axis label on the upper plot and redraw a small
   // axis instead, in order to avoid the first label (0) to be clipped.
   //////////////hFrame->GetYaxis()->SetLabelSize(0.);
   //TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
   //axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   //axis->SetLabelSize(15);
   //axis->Draw();

   // lower plot will be in pad
   c->cd();          // Go back to the main canvas before defining pad2
   TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
   pad2->SetTopMargin(0);
   pad2->SetBottomMargin(0.2);
   //pad2->SetGridx(); // vertical grid
   pad2->SetGridy(); // horizontal grid
   pad2->Draw();
   pad2->cd();       // pad2 becomes the current pad

   /*
   // Define the ratio plot
   TH1F *h3 = (TH1F*)h1->Clone("h3");
   h3->SetLineColor(kBlack);
   h3->SetMinimum(0.8);  // Define Y ..
   h3->SetMaximum(1.35); // .. range
   h3->Sumw2();
   h3->SetStats(0);      // No statistics on lower plot
   h3->Divide(h2);
   h3->SetMarkerStyle(21);
   h3->Draw("ep");       // Draw the ratio plot
   */
   pad2->SetLogy();
   hratio->SetStats(0);  
   hratio->SetMaximum(10.);
   hratio->SetMinimum(0.1);
   //hratio->SetMinimum(0.1);
   hratio->SetMarkerStyle(20);
   hratio->SetMarkerSize(0.5);
   hratio->Draw("ep");
   
   /*
   // h1 settings
   h1->SetLineColor(kBlue+1);
   h1->SetLineWidth(2);

   // Y axis h1 plot settings
   h1->GetYaxis()->SetTitleSize(20);
   h1->GetYaxis()->SetTitleFont(43);
   h1->GetYaxis()->SetTitleOffset(1.55);

   // h2 settings
   h2->SetLineColor(kRed);
   h2->SetLineWidth(2);
   */

   // Ratio plot (h3) settings
   //h3->SetTitle(""); // Remove the ratio title
   hratio->SetTitle("");

   // Y axis ratio plot settings
   hratio->GetYaxis()->SetTitle("Data/MC");
   hratio->GetYaxis()->SetNdivisions(505);
   hratio->GetYaxis()->SetTitleSize(20);
   hratio->GetYaxis()->SetTitleFont(43);
   hratio->GetYaxis()->SetTitleOffset(1.55);
   hratio->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   hratio->GetYaxis()->SetLabelSize(15);

   // X axis ratio plot settings
   hratio->GetXaxis()->SetTitleSize(20);
   hratio->GetXaxis()->SetTitleFont(43);
   hratio->GetXaxis()->SetTitleOffset(4.);
   hratio->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   hratio->GetXaxis()->SetLabelSize(15);


}
