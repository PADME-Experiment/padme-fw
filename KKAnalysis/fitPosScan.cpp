// -- this code is for position scan runs with higher multiplicity 
// -- do a fit to single peak in the charge distribution
#include "TFile.h"
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

void processrun(int nrun, double Xpos, double Ypos) {
    // Choose #RUN and X and Y !!!
    //int nrun = 318;
    //double X = 0;
    //double Y = 0;
    TString infileName = Form("run%d.root",nrun);
    double Energy = 285; //MeV
    double Voltage = 650; //V
    double A, Q, sQ;
    double QCh[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];

    double xmin = -5.;
    double xmax = 120.;

    // Define histogram
    TString xs = Form("%i", abs((int)Xpos)); // Format with 1 decimal if needed
    TString ys = Form("%i", abs((int)Ypos));

    if (Xpos < 0) xs.Prepend("m");
    if (Ypos < 0) ys.Prepend("m");
    
    TH1F* hQ = new TH1F(Form("hQ_%i_X%s_Y%s", nrun, xs.Data(), ys.Data()), "; Q [pC]; Number of events", 1000, -5., 120.);

    // Open ROOT file for input
    TFile *infile = new TFile(Form("/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/runs/%s", infileName.Data()));
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

    // Draw the histogram
    // TCanvas *c1 = new TCanvas(); c1->cd();
    // hQ->SetTitle("");
    //hQ->Draw(); 
    //c1->Draw();
   
    // Open new ROOT file for output
    infileName = infileName(0, infileName.Last('.'));
    TFile *newfile = new TFile(Form("/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/PositionFits/%sfit.root",infileName.Data()), "RECREATE");

    // Define the fitting function
    TF1 *gaussfit = new TF1("gaussfit", "gaus", xmin, xmax);

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
    double ampQerr = gaussfit->GetParError(0);
    double meanQ = gaussfit->GetParameter(1);
    double meanQerr = gaussfit->GetParError(1);
    double sigmaQ = gaussfit->GetParameter(2);
    double sigmaQerr = gaussfit->GetParError(2);

    // Open new .txt file to write results 
    std::ofstream PosScan("/home/mancinima/LeadGlassCalib2024/KKAnalysis/output/PositionFits/PositionScan.txt", std::ios::app);
    if (!PosScan) { std::cerr << "Error opening file!" << std::endl; }
    // Write numbers to file
    PosScan << nrun << " " << Xpos << " " << Ypos << " " << meanQ << " " << meanQerr << " " << sigmaQ << " " << sigmaQerr << std::endl;    
    printf("Numbers for run%d written to file.\n", nrun);
    // Close file
    PosScan.close();

    // Draw histogram
    // c1->cd();
    // hQ->SetLineColor(38);
    // hQ->SetFillColor(38);
    // //double ymax = hQ->GetBinContent(hQ->GetMaximumBin());
    // //hQ->GetYaxis()->SetRangeUser(0,1.21*ymax);
    // c1->Update();
    // gStyle->SetOptFit(1);
    // TPaveStats *stats = (TPaveStats*)hQ->GetListOfFunctions()->FindObject("stats");
    // if (stats) { 
    //     stats->SetX1NDC(0.75); 
    //     //stats->SetX2NDC(0.9); 
    //     stats->SetY1NDC(0.6); 
    //     //stats->SetY2NDC(0.9); 
    // }
    // c1->Update();

    //TLegend *l1 = new TLegend(0.65,0.65,0.9,0.775);
    //l1->AddEntry(hQ, "LeadGlass data");
    //l1->AddEntry(sumgauss, "Fit to the data");
    //l1->Draw();
    // TLatex lc1; lc1.SetNDC(); lc1.SetTextSize(0.04); lc1.SetTextAlign(13);
    // TString runNumber = baseName;
    // lc1.DrawLatex(0.15,0.85,runNumber);
    // TString xy = TString::Format("X = %0.f mm; Y = %0.f mm", Xpos, Ypos);
    // lc1.DrawLatex(0.15,0.8,xy); 

    
    hQ->Write();
    // c1->Write();

    // c1->SaveAs(Form("%s_hCh1ChargeFit.png",baseName.Data()));
    // c1->SaveAs(Form("%s_hCh1ChargeFit.eps",baseName.Data()));
    //c1->SaveAs(Form("%s_hCh1ChargeFit.pdf",baseName.Data()));

    newfile->Close();
    infile->Close();
}

int fitPosScan() {
    double X;
    double Y;
    // POSITION SCAN X
    Y = 0.;
    X = 0.;
    for (int nrun = 318; nrun < 329; ++nrun) {
        printf("Processing run%d -- X = %0.f, Y = %0.f\n", nrun, X, Y);
        processrun(nrun,X,Y);
        X -= 5.;
    }
    X = 5.;
    for (int nrun = 329; nrun < 343; ++nrun) {
        printf("Processing run%d -- X = %0.f, Y = %0.f\n", nrun, X, Y);
        processrun(nrun,X,Y);
        X += 5.;
    }
    // POSITION SCAN Y
    X = 0.;
    Y = 0.;
    for (int nrun = 343; nrun < 359; ++nrun) {
        if (nrun == 355) { continue; }
        printf("Processing run%d -- X = %0.f, Y = %0.f\n", nrun, X, Y);
        processrun(nrun,X,Y);
        Y -= 5.;
    }
    Y = 5.;
    for (int nrun = 359; nrun < 371; ++nrun) {
        printf("Processing run%d -- X = %0.f, Y = %0.f\n", nrun, X, Y);
        processrun(nrun,X,Y);
        Y += 5.;
    }
    return 0;
}