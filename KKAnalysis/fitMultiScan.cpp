// -- this code is for fitting histograms for the multiplicity scan at 650V
// -- make a fit to single peak in the charge distribution
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TLatex.h"
#include <iostream>
#include <fstream>

void processrun(int nrun, double multi, double xmax) {
    printf("Processing run%d -- N_POT = %0.f\n", nrun, multi);

    TString infileName = Form("out_run%dcut.root",nrun);
    double Energy = 285; //MeV
    double Voltage = 650; //V
    double A, Q, sQ;
    double ChargeCh1;
    double xmin = -5;

    // Define histogram
    TH1F* hCh1Q = new TH1F("hCh1Charge","; Q [pC]; Number of events",250,xmin,xmax);

    // Open ROOT file for input
    TFile *infile = new TFile(Form("../out/%s",infileName.Data()));
    if(!infile || infile->IsZombie()) {
        fprintf(stderr,"ERROR - Cannot open file %s\n",infile->GetName());
        exit(1);
    }
    // Access the TTree
    TTree *tree = (TTree*)infile->Get("NTU");
    if (!tree) {
        std::cerr << "TTree not found in file!" << std::endl;
        infile->Close();
        exit(1);
    }
    //tree->Show(0);
    // Set branch address to link the variable with the branch
    tree->SetBranchAddress("QCh1", &ChargeCh1);

    Long64_t nEntries = tree->GetEntries();
    // Loop through all entries in the tree
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        // Get the ChargeCh1 and fill histogram
        hCh1Q->Fill(ChargeCh1);
    }

    ////
    TString baseName = infileName; 
    baseName = baseName(baseName.Last('_')+1, 10); baseName = baseName(0, baseName.Last('.')-3);
    printf("Fitting the histogram for LeadGlass %s ... \n",baseName.Data());
    ////
   
    // Open new ROOT file for output
    infileName = infileName(0, infileName.Last('.'));
    TFile *newfile = new TFile(Form("%sfit.root",infileName.Data()), "RECREATE");

    // Define the fitting function
    double xminfit = 2.;
    double xmaxfit = hCh1Q->GetXaxis()->GetXmax();
    TF1 *gaussfit = new TF1("gaussfit", "gaus", xminfit, xmaxfit);

    //gaussfit->SetParName(0,"A");
    //gaussfit->SetParName(1,"Q");
    //gaussfit->SetParName(2,"#sigma_{Q}");

    gaussfit->SetParameter(0,A);
    gaussfit->SetParameter(1,Q);
    gaussfit->SetParameter(2,sQ);

    // Fit the histogram -- charge distribution
    hCh1Q->Fit(gaussfit, "R");

    // Get the fit parameters
    double ampQ = gaussfit->GetParameter(0);
    double meanQ = gaussfit->GetParameter(1);
    double meanQerr = gaussfit->GetParError(1);
    double sigmaQ = gaussfit->GetParameter(2);
    double sigmaQerr = gaussfit->GetParError(2);

    // Open new .txt file to write results 
    std::ofstream MultiScan("MultiScan.txt", std::ios::app);
    if (!MultiScan) { std::cerr << "Error opening file!" << std::endl; }
    // Write numbers to file
    MultiScan << nrun << " " << multi << " " << meanQ << " " << meanQerr << " " << sigmaQ << " " << sigmaQerr << std::endl;    
    printf("Numbers for run%d written to file.\n", nrun);
    // Close file
    MultiScan.close();

    // Draw the histogram
    TCanvas *c1 = new TCanvas(); c1->cd();
    hCh1Q->SetLineColor(38);
    hCh1Q->SetFillColor(38);
    //double ymax = hCh1Q->GetBinContent(hCh1Q->GetMaximumBin());
    //hCh1Q->GetYaxis()->SetRangeUser(0,1.21*ymax);
    hCh1Q->Draw(); 
    c1->Update();
    gStyle->SetOptFit(1);
    gStyle->SetOptStat(11);
    TPaveStats *stats = (TPaveStats*)hCh1Q->GetListOfFunctions()->FindObject("stats");
    if (stats) { 
        stats->SetX1NDC(0.7); 
        //stats->SetX2NDC(0.9); 
        stats->SetY1NDC(0.6); 
        //stats->SetY2NDC(0.9); 
    }
    c1->Update();

    //TLegend *l1 = new TLegend(0.65,0.65,0.9,0.775);
    //l1->AddEntry(hCh1Q, "LeadGlass data");
    //l1->AddEntry(sumgauss, "Fit to the data");
    //l1->Draw();
    TLatex lc1; lc1.SetNDC(); lc1.SetTextSize(0.05); lc1.SetTextAlign(13);
    TString runNumber = baseName;
    lc1.DrawLatex(0.15,0.85,runNumber);
    TString Npot = TString::Format("N_{POT} = %0.f", multi);
    lc1.DrawLatex(0.15,0.8,Npot); 

    hCh1Q->Write();
    c1->Write();

    c1->SaveAs(Form("%s_hCh1ChargeFit.png",baseName.Data()));
    c1->SaveAs(Form("%s_hCh1ChargeFit.eps",baseName.Data()));
    //c1->SaveAs(Form("%s_hCh1ChargeFit.pdf",baseName.Data()));

    //newfile->Close();
    //infile->Close();
}

int fitMultiScan() {
    // Choose #RUN and N_POT and xmax
    //int nrun;
    processrun(388,50,15.);
    processrun(389,100,30.);
    processrun(395,200,60.);
    processrun(390,500,140.);
    processrun(392,1000,320.);
    return 0;
}