void subtractHistos(){
  TFile *f = TFile::Open("240605.root");
  TDirectory* dir = (TDirectory*) f->Get("SPA_background");
  TFile *outRoot = new TFile("SPAhistos.root","RECREATE");

  TH1F *Histo1 = (TH1F*) dir->Get("SPA_background_ECal_MissingMass_oneCluster");
  TH1F *Histo2 = (TH1F*) dir->Get("SPA_background_ECal_MissingMass_oneCluster_GGcut");

  TCanvas *c1 = new TCanvas();
  c1->cd();
  TH1F *diff = (TH1F*) Histo1->Clone();
  diff->Add(Histo2,-1);
  diff->Draw();
  

  outRoot->Write();
}
