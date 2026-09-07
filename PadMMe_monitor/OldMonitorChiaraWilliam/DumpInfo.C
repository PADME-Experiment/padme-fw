#include <TFile.h>
#include <TH1.h>
#include <iostream>
#include <fstream>

void DumpInfo(string filename, string run) {
    // Apri il file ROOT
  TFile *file = TFile::Open(filename.c_str(), "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Errore: impossibile aprire il file ROOT!" << std::endl;
  }
  
  // Recupera gli istogrammi
  TH1 *h_chargeX = (TH1*)file->Get("h_chargeX_vs_time");
  TH1 *h_chargeY = (TH1*)file->Get("h_chargeY_vs_time");
  TH1 *h_muY = (TH1*)file->Get("h_muY_vs_time");
  TH1 *h_muX = (TH1*)file->Get("h_muX_vs_time");
  TH1 *h_sigmaX = (TH1*)file->Get("h_sigmaX_vs_time");
  
  if (!h_muY || !h_muX || !h_sigmaX) {
    std::cerr << "Errore: uno o più istogrammi non trovati!" << std::endl;
  }
  
  // Controllo consistenza binning
  int nBins = h_muX->GetNbinsX();
  if (h_muY->GetNbinsX() != nBins || h_sigmaX->GetNbinsX() != nBins) {
    std::cerr << "Errore: gli istogrammi hanno numeri di bin diversi!" << std::endl;
  }
  
  // Apri file di output
  string outfilename = "output_"+run+".txt";
  std::ofstream outFile(outfilename.c_str());
  //outFile << "#RunNumber = 80677 - EBeam = 270.25 - SqrtS = 16.6191"<< std::endl;
  outFile << "# period,qx,err_qx,qy,err_qy,x,err_x,y,err_y,sigmax,err_sigmax" << std::endl;
  
  // Loop sui bin (ROOT parte da 1!)
    for (int i = 1; i <= nBins; ++i) {
      double x = h_muX->GetBinContent(i);
      double err_x = h_muX->GetBinError(i);
      
      double y = h_muY->GetBinContent(i);
      double err_y = h_muY->GetBinError(i);

      double chargex = h_chargeX->GetBinContent(i);
      double err_chargex = h_chargeX->GetBinError(i);
      
      double chargey = h_chargeY->GetBinContent(i);
      double err_chargey = h_chargeY->GetBinError(i);

      
      double sigmax = h_sigmaX->GetBinContent(i);
      double err_sigmax = h_sigmaX->GetBinError(i);
      
      int period = i;
      
      outFile << period << ","
	      << chargex << "," << err_chargex << ","
	      << chargey << "," << err_chargey << ","
	      << x << "," << err_x << ","
	      << y << "," << err_y << ","
	      << sigmax << "," << err_sigmax << std::endl;
    }
    
    outFile.close();
    file->Close();
    
    std::cout << "Dati salvati in output.txt" << std::endl;
    
}
