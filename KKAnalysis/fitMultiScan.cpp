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

#define NTUPLE_N_BOARDS 1
#define NTUPLE_N_CHANNELS 2
#define VPP 1.

void processrun(int nrun, double multi, double xmax) {
    printf("Processing run%d -- N_POT = %0.f\n", nrun, multi);

    TString infileName = Form("run%d.root",nrun);
    double Energy = 285; // MeV
    double Voltage = 650; // V
    double A, Q, sQ;
    double QCh[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
    double xmin = -5;

    // Define histogram
    TH1F* hQ = new TH1F(Form("hQ_%i", nrun),"; Q [pC]; Number of events",250,xmin,xmax);

    // Open ROOT file for input
    TFile *infile = new TFile(Form("/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/runs/%s",infileName.Data()));
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
    tree->SetBranchAddress("QCh", &QCh);

    Long64_t nEntries = tree->GetEntries();
    // Loop through all entries in the tree
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        // Access values from the branch
        for (int brd = 0; brd < NTUPLE_N_BOARDS; brd++) {
            for (int ch = 0; ch < NTUPLE_N_CHANNELS; ch++) {
                // std::cout << "Charge[" << brd << "][" << ch << "] = " << QCh[brd][ch] << std::endl;
                // Get the QCh and fill histogram
                if(ch==0) hQ->Fill(QCh[brd][ch]);
                else continue;
            }
        }
        
    }

    ////
    TString baseName = infileName; 
    baseName = baseName(baseName.Last('_')+1, 10); baseName = baseName(0, baseName.Last('.')-3);
    printf("Fitting the histogram for LeadGlass %s ... \n",baseName.Data());
    ////
   
    // Open new ROOT file for output
    infileName = infileName(0, infileName.Last('.'));
    TFile *newfile = new TFile(Form("/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/multiFits/%sfit.root",infileName.Data()), "RECREATE");

    // Define the fitting function
    double xminfit = 2.;
    double xmaxfit = hQ->GetXaxis()->GetXmax();
    TF1 *gaussfit = new TF1("gaussfit", "gaus", xminfit, xmaxfit);

    //gaussfit->SetParName(0,"A");
    //gaussfit->SetParName(1,"Q");
    //gaussfit->SetParName(2,"#sigma_{Q}");

    gaussfit->SetParameter(0,A);
    gaussfit->SetParameter(1,Q);
    gaussfit->SetParameter(2,sQ);

    // Fit the histogram -- charge distribution
    hQ->Fit(gaussfit, "R");

    // Get the fit parameters
    double ampQ = gaussfit->GetParameter(0);
    double meanQ = gaussfit->GetParameter(1);
    double meanQerr = gaussfit->GetParError(1);
    double sigmaQ = gaussfit->GetParameter(2);
    double sigmaQerr = gaussfit->GetParError(2);

    // Open new .txt file to write results 
    std::ofstream MultiScan("/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/multiFits/MultiScan.txt", std::ios::app);
    if (!MultiScan.is_open()) { 
        std::cerr << "Error opening file MultiScan.txt!" << std::endl;
        return;  // Stop execution
    }
    // Write numbers to file
    MultiScan << nrun << " " << multi << " " << meanQ << " " << meanQerr << " " << sigmaQ << " " << sigmaQerr << std::endl;    
    printf("Numbers for run%d written to file.\n", nrun);
    // Close file
    MultiScan.close();

    // Draw the histogram
    // TCanvas *c1 = new TCanvas(); c1->cd();
    // hQ->SetLineColor(38);
    // hQ->SetFillColor(38);
    // //double ymax = hQ->GetBinContent(hQ->GetMaximumBin());
    // //hQ->GetYaxis()->SetRangeUser(0,1.21*ymax);
    // hQ->Draw(); 
    // c1->Update();
    // gStyle->SetOptFit(1);
    // gStyle->SetOptStat(11);
    // TPaveStats *stats = (TPaveStats*)hQ->GetListOfFunctions()->FindObject("stats");
    // if (stats) { 
    //     stats->SetX1NDC(0.7); 
    //     //stats->SetX2NDC(0.9); 
    //     stats->SetY1NDC(0.6); 
    //     //stats->SetY2NDC(0.9); 
    // }
    // c1->Update();

    //TLegend *l1 = new TLegend(0.65,0.65,0.9,0.775);
    //l1->AddEntry(hQ, "LeadGlass data");
    //l1->AddEntry(sumgauss, "Fit to the data");
    //l1->Draw();
    // TLatex lc1; lc1.SetNDC(); lc1.SetTextSize(0.05); lc1.SetTextAlign(13);
    // TString runNumber = baseName;
    // lc1.DrawLatex(0.15,0.85,runNumber);
    // TString Npot = TString::Format("N_{POT} = %0.f", multi);
    // lc1.DrawLatex(0.15,0.8,Npot); 

    hQ->Write();
    // c1->Write();

    // c1->SaveAs(Form("%s_hChargeFit.png",baseName.Data()));
    // c1->SaveAs(Form("%s_hChargeFit.eps",baseName.Data()));
    //c1->SaveAs(Form("%s_hChargeFit.pdf",baseName.Data()));

    newfile->Close();
    // infile->Close();
    // delete newfile;
    // delete infile;
}

int fitMultiScan() {
    // Choose #RUN and N_POT and xmax
    //int nrun;
    processrun(385,2,15.);
    processrun(386,5,15.);
    processrun(387,10,15.);
    processrun(388,50,15.);
    processrun(389,100,30.);
    processrun(395,200,60.);
    processrun(390,500,140.);
    processrun(392,1000,320.);

    return 0;
}