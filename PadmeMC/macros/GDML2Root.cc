{
TGeoManager::Import("Padme.gdml");
TFile *_file0 = TFile::Open("Padme.root","RECREATE");
gGeoManager->Write();
_file0->Close();
}
