#include <iostream>
#include <fstream>
#include <vector>
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TPaveStats.h"

void plotChargeEnergy() {
    std::ifstream infile("EnergyScan.txt");
    
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }
    std::string header;
    std::getline(infile, header); // Read and discard the first line

    // Vectors to make the graphs 
    std::vector<double> vecE, vecQ, vecQerr, vecSigmaQ, vecSigmaQerr;

    // Variables to temporarily store data from the file
    double N, E, Q, Qerr, sigmaQ, sigmaQerr;

    // Read data from file
    while (infile >> N >> E >> Q >> Qerr >> sigmaQ >> sigmaQerr) {
        vecE.push_back(N*E);
        vecQ.push_back(Q);
        vecQerr.push_back(Qerr);
    }
    infile.close();

    // Number of points
    int n = vecE.size();

    // Create a canvas
    TCanvas *c1 = new TCanvas("c1", "ChargeEnergy"); c1->cd();
    c1->SetTicks(1,1);
    c1->SetGrid(); gStyle->SetGridColor(kGray);
    // Plot the Q(E)
    TGraphErrors *gQE = new TGraphErrors(n, &vecE[0], &vecQ[0], 0, &vecQerr[0]);
    gQE->SetName("graphQ");
    gQE->SetTitle(";Energy [MeV]; Q [pC]");
    gQE->SetMarkerStyle(20);    // Full circle marker
    gQE->SetMarkerColor(kBlue); // Blue color for markers
    gQE->SetLineColor(kBlue);   // Blue color for line
    gQE->Draw("AP");

    TF1 *linefit = new TF1("linefit", "[0]+[1]*x", 0, 2500);
    gQE->Fit(linefit);

    c1->Update();
    gStyle->SetOptFit(1);
    TPaveStats *stats = (TPaveStats*)gQE->GetListOfFunctions()->FindObject("stats");
    if (stats) { stats->SetX1NDC(0.1); stats->SetX2NDC(0.46); stats->SetY1NDC(0.75); stats->SetY2NDC(0.9); }
    c1->Update();

    //TLatex lgrQ; lgrQ.SetNDC(); lgrQ.SetTextSize(0.04); lgrQ.SetTextAlign(13);
    //TString k = TString::Format("Q/E = %.6f pC/MeV \n", linefit->GetParameter(1));
    //lgrQ.DrawLatex(0.15,0.85,k);

    
    // Create a .root file
    TFile *outfile = new TFile("ChargeEnergy.root", "RECREATE");
    // Write the TGraphErrors object into the file
    gQE->Write("chargeEnergy");

    c1->SaveAs("chargeEnergy.png");
    c1->SaveAs("chargeEnergy.eps");

    // Close the ROOT file
    outfile->Close();
    // Save the graph to a file
    //c1->SaveAs("sigmaEcut.pdf");
}