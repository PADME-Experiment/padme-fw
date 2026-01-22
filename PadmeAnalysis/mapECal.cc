#include <TEllipse.h>

void mapECal(){

  //TFile *f = TFile::Open("231003_muons_cosmic681_fixed_tresholdML50_rebin_trytrek.root");
  //TDirectory* dir = (TDirectory*) f->Get("ECalMLMuons");
  // TFile *outRoot = new TFile("231003_ECalMap.root","RECREATE");

  //TH2F *AllHits = (TH2F*) dir->Get("ECalMLMuonsECal_Map_AllHits");
  //TH2F *FoundHits = (TH2F*) dir->Get("ECalMLMuonsECal_Map_FoundHits");

  //TH2F *map = new TH2F("ECalMap","ECal muon registration efficiency",32,0.0,31.0,32,0.0,31.0);
  //map->Divide(FoundHits,AllHits);
  //outRoot->Write();



    std::ifstream fin("/home/kalina/software/padme-fw-BL/padme-fw/PadmeReco/config/Calibration/ECalEnergyCalibration_5.dat");

    int NX = 29;
    int NY = 29;

    TH2F* h = new TH2F("h", "Coefficients",
                       NX, 0, NX,
                       NY, 0, NY);

    int x, y;
    float col3, col4, value;

    while (fin >> x >> y >> col3 >> col4 >> value) {
      if(value < 350)
        h->SetBinContent(x+1, y+1, value);
    }

    fin.close();

    
    TCanvas* c = new TCanvas("c");
    h->Draw("COLZ");

    const double xc = 14.5;
    const double yc = 14.5;

    TEllipse* circ1 = new TEllipse(xc, yc, 4.5, 4.5);
    TEllipse* circ2 = new TEllipse(xc, yc, 13.5, 13.5);

   
    for (TEllipse* e : {circ1, circ2}) {
        e->SetFillStyle(0);        
        e->SetLineColor(kRed);
        e->SetLineStyle(2);        
        e->SetLineWidth(3);       
        e->Draw("same");
    }
}
