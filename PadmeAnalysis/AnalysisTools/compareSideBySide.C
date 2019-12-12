void compareSideBySide(std::string hname="SS2gEvsR_passDt", TFile* f0=_file0, TFile* f1=_file1)
{
  gStyle->SetOptStat(0);
  new TCanvas("c1","c1",800,300);
  c1->Divide(2,1); 
  c1->cd(1);
  TH2D* h0; _file0->GetObject(hname.c_str(),h0);
  if (h0 != NULL){h0->Draw("ZCOL");}
  else {
    std::cout<<" histogram named <"<<hname<<"> not found in file named "<<f0->GetName()<<std::endl;
    delete c1;
    return;
  }
  c1->cd(2);
  TH2D* h1; _file1->GetObject(hname.c_str(),h1);
  if (h1 != NULL){h1->Draw("ZCOL");}
  else {
    std::cout<<" histogram named <"<<hname<<"> not found in file named "<<f1->GetName()<<std::endl;
    delete c1;
    return;
  }
  c1->SaveAs(("outpng/"+hname+".png").c_str());
  delete c1;
}
