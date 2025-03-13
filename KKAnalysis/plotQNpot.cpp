#include <iostream>
#include <fstream>
#include <vector>
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TLatex.h"

void plotQNpot() {
    // --- LEAD GLASS MULTIPLICITY SCAN DATA ---
    std::ifstream infile("MultiScan.txt");
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }
    std::cout << "Reading MultiScan file..." << std::endl;
    std::string header;
    std::getline(infile, header); // Read and discard the first line

    double nrun, Npot, meanQ, meanQerr, sigmaQ, sigmaQerr;
    std::vector<double> vecnrun, vecNpot, vecQ, vecQerr, vecQE;

    // Read data from file
    while (infile >> nrun >> Npot >> meanQ >> meanQerr >> sigmaQ >> sigmaQerr) {
        vecnrun.push_back(nrun);
        vecNpot.push_back(Npot);
        vecQ.push_back(meanQ);
        vecQE.push_back(meanQ/285.);
        //vecQerr.push_back(meanQerr);
        vecQerr.push_back(meanQerr);
    }
    infile.close();

    // --- FITPIX CORRECTIONS ---
    std::ifstream fpixfile("../../fitpix_test_run_10_2024/fpix_corr_MultiScan3.txt");
    if (!fpixfile.is_open()) {
        std::cerr << "Error: Could not open the fitpix corrections file!" << std::endl;
        return;
    }
    std::cout << "Reading fpix_corr file..." << std::endl;
    std::string headerr;
    std::getline(fpixfile, headerr); // Read and discard the first line

    double nrunfpix, Nfpix, NfpixErr, Ncorr, NcorrErr;
    //double meanX, meanXerr, meanY, meanYerr, sigmaX, sigmaXerr, sigmaY, sigmaYerr;
    std::vector<double> vecnrunfpix, vecNfpix, vecNfpixErr, vecNcorr, vecNcorrErr;
    //std::vector<double> vecmeanX, vecmeanXerr, vecmeanY, vecmeanYerr, vecsigmaX, vecsigmaXerr, vecsigmaY, vecsigmaYerr;


    // Read data from file
    while (fpixfile >> nrunfpix >> Nfpix >> NfpixErr >> Ncorr >> NcorrErr) {
        //>> meanX >> meanXerr >> meanY >> meanYerr >> sigmaX >> sigmaXerr >> sigmaY >> sigmaYerr) {
        vecnrunfpix.push_back(nrunfpix);

        vecNfpix.push_back(Nfpix);
        vecNfpixErr.push_back(NfpixErr);
        std::cout << "Nfpix = " << Nfpix << std::endl;

        vecNcorr.push_back(Ncorr);
        vecNcorrErr.push_back(NcorrErr);
        std::cout << "N = " << Ncorr << std::endl;

        //vecmeanX.push_back(meanX);
        //vecmeanXerr.push_back(meanXerr);
        //vecmeanY.push_back(meanY);
        //vecmeanYerr.push_back(meanYerr);

        //vecsigmaX.push_back(sigmaX);
        //vecsigmaXerr.push_back(sigmaXerr);
        //vecsigmaY.push_back(sigmaY);
        //vecsigmaYerr.push_back(sigmaYerr);
    }
    fpixfile.close();

    // Check if the vectors have the same size
    if (vecnrun.size() != vecnrunfpix.size()) {
        std::cerr << "Error: Vectors don't have the same size!" << std::endl;
        return;
    }

    // MAKE THE PLOT 
    int nPoints = vecnrun.size(); // Number of points

    //TCanvas *c1 = new TCanvas("c1", "ChargeNpot"); c1->cd();
    //c1->SetTicks(1,1);
    //c1->SetGrid(); gStyle->SetGridColor(kGray);

    // Plot the Q(Npot)
    TGraphErrors *gQN = new TGraphErrors(nPoints, &vecNfpix[0], &vecQ[0], 0, &vecQerr[0]);
    gQN->SetName("graphQN");
    gQN->SetTitle(";N_{POT}; Q [pC]");
    gQN->SetMarkerStyle(20);
    gQN->SetMarkerSize(1.2);
    gQN->SetMarkerColor(kBlue);
    //gQN->Draw("APE");

    //TF1 *linefit = new TF1("linefit", "[0]+[1]*x",0,2000);
    //linefit->SetLineColor(kOrange+1);
    //gQN->Fit("linefit","R");

    // Plot the Q/E(Npot)
    TGraphErrors *gQEN = new TGraphErrors(nPoints, &vecNpot[0], &vecQE[0], 0, &vecQerr[0]);
    gQEN->SetName("graphQN");
    gQEN->SetTitle(";N_{POT}; Q [pC]");
    gQEN->SetMarkerStyle(20);
    //gQEN->SetMarkerSize(0.8);
    gQEN->SetMarkerColor(kBlue);
    //gQEN->Draw("APE");

    //TCanvas *c2 = new TCanvas("c2", "ChargeNcorr"); c2->cd();
    //c2->SetTicks(1,1);
    //c2->SetGrid(); gStyle->SetGridColor(kGray);

    // Plot the Q(Ncorr)
    TGraphErrors *gQNcorr = new TGraphErrors(nPoints, &vecNcorr[0], &vecQ[0], &vecNcorrErr[0], &vecQerr[0]);
    gQNcorr->SetName("graphQNcorr");
    gQNcorr->SetTitle(";N_{POT}; Q [pC]");
    gQNcorr->SetMarkerStyle(21);
    gQNcorr->SetMarkerSize(1.2);
    gQNcorr->SetMarkerColor(kRed);
    //gQNcorr->Draw("APE");

    TF1 *linefit = new TF1("linefit", "[0]+[1]*x",0,2000);
    linefit->SetLineColor(kOrange+1);
    gQNcorr->Fit("linefit","R");

    // Plot the Q/E(Ncorr)
    TGraphErrors *gQENcorr = new TGraphErrors(nPoints, &vecNcorr[0], &vecQE[0], &vecNcorrErr[0], &vecQerr[0]);
    gQENcorr->SetName("graphQNcorr");
    gQENcorr->SetTitle(";N_{POT}; Q [pC]");
    gQENcorr->SetMarkerStyle(23);
    gQENcorr->SetMarkerSize(1);
    gQENcorr->SetMarkerColor(kRed);
    //gQENcorr->Draw("APE");

    //TF1 *linefit = new TF1("linefit", "pol1");
    //linefit->SetLineColor(kOrange+1);
    gQENcorr->Fit("linefit");

    // Plot meanX, meanY, sigmaX, sigmaY
    //TGraphErrors *gmeanX = new TGraphErrors(nPoints, &vecNpot[0], &vecmeanX[0], 0, &vecmeanXerr[0]);
    //TGraphErrors *gmeanY = new TGraphErrors(nPoints, &vecNpot[0], &vecmeanY[0], 0, &vecmeanYerr[0]);
    //TGraphErrors *gsigmaX = new TGraphErrors(nPoints, &vecNpot[0], &vecsigmaX[0], 0, &vecsigmaXerr[0]);
    //TGraphErrors *gsigmaY = new TGraphErrors(nPoints, &vecNpot[0], &vecsigmaY[0], 0, &vecsigmaYerr[0]);


    TCanvas *c3 = new TCanvas("c3", "ChargeNpotNcorr"); c3->cd();
    c3->SetTicks(1,1);
    c3->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1); 

    TMultiGraph *mgQN = new TMultiGraph();
    mgQN->SetName("mgQN");
    mgQN->SetTitle("; N_{POT}; Q [pC]");
    mgQN->GetXaxis()->SetLimits(0, 1750);
    mgQN->GetYaxis()->SetRangeUser(0, 220);
    mgQN->Add(gQN);
    mgQN->Add(gQNcorr);
    mgQN->Draw("APE");

    c3->Update();
    TPaveStats *stats = (TPaveStats*)gQNcorr->GetListOfFunctions()->FindObject("stats");
    if (stats) { 
        //std::cout << stats->GetX1NDC() << std::endl;
        //std::cout << stats->GetX2NDC() << std::endl;
        //std::cout << stats->GetY1NDC() << std::endl;
        //std::cout << stats->GetY2NDC() << std::endl;
        stats->SetX1NDC(0.5); 
        stats->SetX2NDC(0.9); 
        stats->SetY1NDC(0.1); 
        stats->SetY2NDC(0.26); 
    }
    //c3->Update();

    TLegend *lQN = new TLegend(0.1,0.75,0.4,0.9);
    lQN->AddEntry(gQN, "N_{POT} uncorrected  ");
    lQN->AddEntry(gQNcorr, "N_{POT} corrected");
    lQN->Draw();    

    //TLatex lgrQ; lgrQ.SetNDC(); lgrQ.SetTextSize(0.04); lgrQ.SetTextAlign(13);
    //TString k = TString::Format("Q/E = %.6f pC/MeV \n", linefit->GetParameter(1));
    //lgrQ.DrawLatex(0.15,0.85,k);


    TCanvas *c4 = new TCanvas("c4", "ChargeEnergy_NpotNcorr"); c4->cd();
    c4->SetTicks(1,1);
    c4->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1); 

    TMultiGraph *mgQEN = new TMultiGraph();
    mgQEN->SetName("mgQEN");
    mgQEN->SetTitle("; N_{POT}; Q/E [pC/MeV]");
    mgQEN->GetXaxis()->SetLimits(0, 1500);
    //mgQEN->GetYaxis()->SetRangeUser(0, 220);
    mgQEN->Add(gQEN);
    mgQEN->Add(gQENcorr);
    mgQEN->Draw("APE");
    c4->Update();

    TPaveStats *stats1 = (TPaveStats*)gQENcorr->GetListOfFunctions()->FindObject("stats");
    if (stats1) { 
        stats1->SetX1NDC(0.5); 
        stats1->SetX2NDC(0.9); 
        stats1->SetY1NDC(0.1); 
        stats1->SetY2NDC(0.26); 
    }
    //c4->Update();

    TLegend *lQEN = new TLegend(0.1,0.75,0.4,0.9);
    lQEN->AddEntry(gQEN, "N_{POT} uncorrected  ");
    lQEN->AddEntry(gQENcorr, "N_{POT} corrected");
    lQEN->Draw();    

    // Create a .root file
    //TFile *outfile = new TFile("ChargeNpot.root", "RECREATE");
    // Write the TGraphErrors object into the file
    //gQNpot->Write("chargeNpot");

    c3->SaveAs("chargeNpot2.png");
    c3->SaveAs("chargeNpot2.eps");

    /*
    TCanvas *cmeanX = new TCanvas("cmeanX");
    cmeanX->SetTicks(1,1); 
    cmeanX->SetGrid(); gStyle->SetGridColor(kGray); gStyle->SetOptFit(1);
    cmeanX->Update();
    gmeanX->SetMarkerStyle(20);
    //gmeanX->SetMarkerSize(0.8);
    gmeanX->SetMarkerColor(kBlue+2);
    gmeanX->GetXaxis()->SetLimits(0, 1200);
    gmeanX->GetYaxis()->SetRangeUser(0, 256);
    gmeanX->SetTitle("; N_POT; beam meanX [px]");
    gmeanX->Draw("APE");
    gmeanX->Fit("pol0");

    TCanvas *cmeanY = new TCanvas("cmeanY");
    cmeanY->SetTicks(1,1); 
    cmeanY->SetGrid(); gStyle->SetGridColor(kGray);
    cmeanY->Update();
    gmeanY->SetMarkerStyle(20);
    //gmeanY->SetMarkerSize(0.8);
    gmeanY->SetMarkerColor(kBlue+2);
    gmeanY->GetXaxis()->SetLimits(0, 1200);
    gmeanY->GetYaxis()->SetRangeUser(0, 256);
    gmeanY->SetTitle("; N_POT; beam meanY [px]");
    gmeanY->Draw("APE");
    gmeanY->Fit("pol0");

    TCanvas *csigmaX = new TCanvas("csigmaX");
    csigmaX->SetTicks(1,1); 
    csigmaX->SetGrid(); gStyle->SetGridColor(kGray);
    csigmaX->Update();
    gsigmaX->SetMarkerStyle(20);
    //gsigmaX->SetMarkerSize(0.8);
    gsigmaX->SetMarkerColor(kBlue+2);
    gsigmaX->GetXaxis()->SetLimits(0, 1200);
    gsigmaX->GetYaxis()->SetRangeUser(0, 30);
    gsigmaX->SetTitle("; N_POT; beam sigmaX [px]");
    gsigmaX->Draw("APE");
    gsigmaX->Fit("pol0");

    TCanvas *csigmaY = new TCanvas("csigmaY");
    csigmaY->SetTicks(1,1); 
    csigmaY->SetGrid(); gStyle->SetGridColor(kGray);
    csigmaY->Update();
    gsigmaY->SetMarkerStyle(20);
    //gsigmaY->SetMarkerSize(0.8);
    gsigmaY->SetMarkerColor(kBlue+2);
    gsigmaY->GetXaxis()->SetLimits(0, 1200);
    gsigmaY->GetYaxis()->SetRangeUser(0, 30);
    gsigmaY->SetTitle("; N_POT; beam sigmaY [px]");
    gsigmaY->Draw("APE");
    gsigmaY->Fit("pol0");
    */

    // Close the ROOT file
    //outfile->Close();
}