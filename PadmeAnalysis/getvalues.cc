//#include "HistoSvc.hh"
//#include <iostream>

void processfile(){

    TFile * froot = new TFile("OutputFileRoot.root","UPDATE");

    TDirectory * dir = (TDirectory *) froot->Get("FirstDataLook");

    dir->cd();
    TH1F *meanTimes = new TH1F("meanTimes","Sigma of the dt",90,0,90);
    meanTimes->GetYaxis()->SetTitle("Time, [ns]");
    meanTimes->GetXaxis()->SetTitle("Channel number, [#]");

    TH1F *his[100];

    double binCenter[100];

    int ich = 0;
    for (ich = 1; ich < 90; ich ++) {
      std::string kur =  "FirstDataLook_SAC22_vs_PVETO_"+std::to_string(ich);
      std::cout<< kur << std::endl;
      his[ich] = (TH1F *) dir->Get(kur.c_str());

      binCenter[ich] = his[ich]->GetBinCenter(his[ich]->GetMaximumBin());

      his[ich]->Fit("gaus", "Q", "",  binCenter[ich]-1.5, binCenter[ich]+1.5); // Q is quiet, "" is a drawing option, fit +/-2ns from the max value


      
      //std::cout << "Maximum : "<< his[0]->GetMaximum() << " Bin: " << his[0]->GetMaximumBin() << 
      //  " Bin center" << his[0]->GetBinCenter(his[0]->GetMaximumBin()) << std::endl;
      //meanTimes->SetBinContent(ich, abs(binCenter[ich]));
      //meanTimes->SetBinContent(ich, his[ich]->GetFunction("gaus")->GetParameter(2)); // 1 is mean of Gauss, 2 is sigma

      his[ich]->GetYaxis()->SetTitle("Events, [#]");
      his[ich]->GetXaxis()->SetTitle("Time difference, [ns]");
      //his[ich]->Draw();
      his[ich]->Write();
      
    }

    //meanTimes->Draw();

}
