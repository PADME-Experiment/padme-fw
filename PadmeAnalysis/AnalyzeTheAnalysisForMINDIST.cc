using namespace std;

void Analyze(){
  
  TFile *f1 = TFile::Open("ROOT_ECalMLClusters/230822_11mindist_calibrated.root");
  TFile *f2 = TFile::Open("ROOT_ECalMLClusters/230713_9mindist_calibrated.root");
  TFile *f3 = TFile::Open("ROOT_ECalMLClusters/230809_7mindist_calibrated.root");
  TFile *f4 = TFile::Open("ROOT_ECalMLClusters/230714_5mindist.root");

  TFile *outRoot = new TFile("analysisOfTheAnalysis.root","RECREATE");


  TDirectory* dir1 = (TDirectory*) f1->Get("ECalMLClusters");
  TDirectory* dir2 = (TDirectory*) f2->Get("ECalMLClusters");
  TDirectory* dir3 = (TDirectory*) f3->Get("ECalMLClusters");
  TDirectory* dir4 = (TDirectory*) f4->Get("ECalMLClusters");

  TH1F *HistoECal = (TH1F*) dir1->Get("ECalMLClusters_ECal_TotalClusterEnergy_2Clus");
  TH1F *Histo11 = (TH1F*) dir1->Get("ECalMLClusters_ECalML_TotalClusterEnergy_2Clus");
  TH1F *Histo9 = (TH1F*) dir2->Get("ECalMLClusters_ECalML_TotalClusterEnergy_2Clus");
  TH1F *Histo7 = (TH1F*) dir3->Get("ECalMLClusters_ECalML_TotalClusterEnergy_2Clus");
  TH1F *Histo5 = (TH1F*) dir4->Get("ECalMLClusters_ECalML_TotalClusterEnergy_2Clus");

  TH1F *results = new TH1F("results","Total number of clusters",30000,90000,120000);

    float integralECal = HistoECal->Integral(170,990);
    std::cout<<integralECal<<endl;
  results->Fill(integralECal);
     float integral11 = Histo11->Integral(160,990);
     
  results->Fill(integral11);
  float integral9 = Histo9->Integral(160,990);
  results->Fill(integral9);
  float integral7 = Histo7->Integral(160,990);
  results->Fill(integral7);
  float integral5 = Histo5->Integral(160,990);
  results->Fill(integral5);

  int n=4;
  Double_t x[n], y[n], e[n];
  Double_t ex[4]={0,0,0,0};
  x[0]=5;
  y[0]=integral5;
  e[0]=TMath::Sqrt(integral5);
  x[1]=7;
  y[1]=integral7;
  e[1]=TMath::Sqrt(integral7);
  x[2]=9;
  y[2]=integral9;
  e[2]=TMath::Sqrt(integral9);
  x[3]=11;
  y[3]=integral11;
  e[3]=TMath::Sqrt(integral11);
  auto *gr1 = new TGraphErrors (n, x, y, ex, e);
  gr1->SetTitle("Number of cluster couples with total energy above 320 MeV");
  gr1->GetXaxis()->SetTitle("MINDIST");
  gr1->GetYaxis()->SetTitle("Number of clusters");
  gr1->SetMarkerColor(4);
  gr1->SetMarkerStyle(21);
   gr1->SetMarkerSize(3);
  gr1->SetLineWidth(2);
  
   

  TCanvas *c1 = new TCanvas();
   
  //auto c1 = new TCanvas("c1","Graph Example",200,10,700,500);
  c1->cd();
  //gr1->Draw();
  gr1->Draw("ap");
  TLine *line = new TLine(4,integralECal,12,integralECal);
  line->SetLineWidth(2);
  //line->SetLineColor(kGreen);
  line->Draw("same");
  //c1->Update();
  //gr1->Write("fig");
  outRoot->Write();
  
}
