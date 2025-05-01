void mapECal(){

TFile *f = TFile::Open("231003_muons_cosmic681_fixed_tresholdML50_rebin_trytrek.root");
  TDirectory* dir = (TDirectory*) f->Get("ECalMLMuons");
  TFile *outRoot = new TFile("231003_ECalMap.root","RECREATE");

  TH2F *AllHits = (TH2F*) dir->Get("ECalMLMuonsECal_Map_AllHits");
  TH2F *FoundHits = (TH2F*) dir->Get("ECalMLMuonsECal_Map_FoundHits");

  TH2F *map = new TH2F("ECalMap","ECal muon registration efficiency",32,0.0,31.0,32,0.0,31.0);
  map->Divide(FoundHits,AllHits);
  outRoot->Write();
  
}
