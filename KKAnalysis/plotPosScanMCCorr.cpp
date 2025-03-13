#include <iostream>
#include <fstream>
#include <vector>
#include "TGraphErrors.h"
#include "TGraph2D.h"
#include "TGraph2DErrors.h"
#include "TMultiGraph.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TStyle.h"
#include <iostream>
#include <string.h>

void plotPosScanMCCorr() {

    TGraphErrors *gQX = new TGraphErrors(); gQX->SetName("gQX");
    TGraphErrors *gQY = new TGraphErrors(); gQY->SetName("gQY");

    //TGraphErrors *gQXnorm = new TGraphErrors(); gQXnorm->SetName("gQXnorm");
    TGraphErrors *gQYnorm = new TGraphErrors(); gQYnorm->SetName("gQYnorm");

    TGraphErrors *gNfpix = new TGraphErrors(); gNfpix->SetName("gNfpix");

    // --- FITPIX CORRECTIONS ---
    std::string runNcorr;
    double Nfpix, NfpixErr, Ncut, NcutErr, Ncorr, NcorrErr;
    std::vector<std::string> vecrunNfpix;
    std::vector<double> vecNfpix, vecNfpixErr, vecNcorr, vecNcorrErr; 

    // Open .txt file -- fpix corrections 
    std::ifstream fpixfile("../fitpix_test_run_10_2024/fpix_corr_PosScan.txt");
    if (!fpixfile.is_open()) {
        std::cerr << "Error: Could not open the corrections file!" << std::endl;
        return;
    }
    std::cout << "Reading fpix_corr file..." << std::endl;
    std::string headerr;
    std::getline(fpixfile, headerr); // Read and discard the first line
    
    // Read data from file -- fpix correction
    while (fpixfile >> runNcorr >> Nfpix >> NfpixErr >> Ncut >> NcutErr >> Ncorr >> NcorrErr) {
        vecrunNfpix.push_back(runNcorr);
        vecNfpix.push_back(Nfpix);
        vecNfpixErr.push_back(NfpixErr);
        vecNcorr.push_back(Ncorr);
        vecNcorrErr.push_back(NcorrErr);
    }
    fpixfile.close();
    std::cout << "fpix_corr file closed!" << std::endl;



    // --- LEADGLASS DATA ---
    std::string runN;
    double X, Y, Q, Qerr, sigmaQ, sigmaQerr, Q500, Q500err;
    std::vector<std::string> vecrunN;
    std::vector<double> vecX, vecY, vecQ, vecQerr;
    std::vector<double> vecQ500, vecQ500err;
    

    // Open .txt file -- PbGlass data
    std::ifstream infile("PosScan.txt");
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open the input file!" << std::endl;
        return;
    }
    std::cout << "Reading PositionScanNrun.txt file..." << std::endl;
    std::string header;
    std::getline(infile, header); // Read and discard the first line
    
    // Read data from file -- PbGlass data
    while (infile >> runN >> X >> Y >> Q >> Qerr >> sigmaQ >> sigmaQerr) {
        //std::cout << runN << std::endl;
        vecrunN.push_back(runN);
        vecX.push_back(X);
        vecY.push_back(Y);
        vecQ.push_back(Q);
        vecQerr.push_back(Qerr);
        Q500 = 0;
        Q500err = 0;

        vecQ500.push_back(Q500);
        vecQ500err.push_back(Q500err);
        //std::cout << "Q = " << Q << std::endl;
        //std::cout << "Q500 = " << Q500 << std::endl;
    }
    infile.close();
    std::cout << "PositionScanNrun.txt file closed!" << std::endl;



    // Apply the correction for the given runs in the fpixfile
    for (int iPos = 0; iPos < vecrunN.size(); ++iPos) { // loop through positionScan runs
        std::cout << vecrunN[iPos] << std::endl;
        for (int iFpix = 0; iFpix < vecrunNfpix.size(); ++iFpix) { // loop though runs with fpix data
            std::cout << "--- " << vecrunNfpix[iFpix] << std::endl;
            if (vecrunN[iPos] == vecrunNfpix[iFpix]) {
                std::cout << "Applying correction for " << vecrunN[iPos] << std::endl;

                std::cout << "Q = " << vecQ[iPos] << std::endl;
                Q500 = vecQ[iPos]*500./vecNcorr[iFpix];
                std::cout << "Q500 = Q*500/Ncorr = " << Q500 << std::endl;
                Q500err = Q500*sqrt(pow(Qerr/Q,2) + pow(NcorrErr/Ncorr,2));

                gNfpix->AddPointError(vecY[iPos],vecNfpix[iFpix],0,vecNfpixErr[iFpix]);

                gQYnorm->AddPointError(vecY[iPos],Q500,0,Q500err);

                break;
            }
        }
    }
    // Draw the graphs
    int N = vecX.size(); // Number of points
    //std::cout << N << std::endl;

    TGraph2D *gQXY = new TGraph2D(N, &vecX[0], &vecY[0], &vecQ[0]); gQXY->SetName("gQXY");
    TGraph2DErrors *gQXYerr = new TGraph2DErrors(N, &vecX[0], &vecY[0], &vecQ[0], 0, 0, &vecQerr[0]); gQXYerr->SetName("gQXYerr");

    //if (vecQ.size() != vecQ500.size()) {
    //    std::cerr << "Error: Vectors are not the same size!" << std::endl;
    //    return;
    //}

    //========== POSITION SCAN X ==========
    for(int i = 0; i < 25; i++){
        //std::cout << i << " " << vecX[i] << " " << vecQ[i] << std::endl;
        gQX->SetPoint(i,vecX[i],vecQ[i]);
        gQX->SetPointError(i,0,vecQerr[i]);
        //gQXnorm->SetPoint(i,vecX[i],vecQ500[i]);
        //gQXnorm->SetPointError(i,0,vecQ500[i]*(vecQerr[i]/vecQ[i]));
    }

    //========== POSITION SCAN Y ==========
    for(int i = 0; i < 27; i++){
        int n = i+25;
        //std::cout << i << " " << n << " " << vecY[n] << " " << vecQ[n] << " " << vecQ500[n] << std::endl;
        gQY->SetPoint(i,vecY[n],vecQ[n]);
        gQY->SetPointError(i, 0, vecQerr[n]);
        //gQYnorm->SetPoint(i,vecY[n],vecQ500[n]);
        //gQYnorm->SetPointError(i,0,vecQ500err[n]);
    }
    //for (int i = gQYnorm->GetN() - 1; i >= 0; i--) {
    //    if (gQYnorm->GetPointY(i) == 0) {
    //        gQYnorm->RemovePoint(i);
    //    }
    //}
    
    vecX.clear();
    vecY.clear();
    vecQ.clear();
    vecQerr.clear();
    vecQ500.clear();
    //vecNtable.clear();
    vecrunNfpix.clear();
    vecNfpix.clear();
    vecNfpixErr.clear();
    vecNcorr.clear();
    vecNcorrErr.clear();

    TCanvas *cQX = new TCanvas("cQX");
    cQX->SetTicks(1,1); cQX->SetGrid(); gStyle->SetGridColor(kGray);
    cQX->Update();
    gQX->SetMarkerStyle(20);
    gQX->SetMarkerSize(0.8);
    gQX->SetMarkerColor(kBlue+2);
    gQX->GetXaxis()->SetLimits(-80, 80);
    gQX->GetYaxis()->SetRangeUser(0, 100);
    gQX->SetTitle("; X [mm]; Q [pC]");
    gQX->Draw("APE");

    //TCanvas *cQXnorm = new TCanvas("cQXnorm");
    //gQXnorm->SetMarkerStyle(20);
    //gQX->SetMarkerSize(0.4);
    //gQXnorm->SetMarkerColor(kOrange+1);
    //gQXnorm->GetXaxis()->SetLimits(-80, 80);
    //gQXnorm->GetYaxis()->SetRangeUser(0, 100);
    //gQXnorm->SetTitle("; X [mm]; Q [pC]");
    //gQXnorm->Draw("APE");

    //TMultiGraph *mgQX = new TMultiGraph();
    //mgQX->SetName("mgQX");
    //mgQX->SetTitle("; X [mm]; Q [pC]");
    //mgQX->Add(gQX);
    //mgQX->Add(gQXnorm);
    //mgQX->Draw("APE");

    TCanvas *cQY = new TCanvas("cQY");
    cQY->SetTicks(1,1); 
    cQY->SetGrid(); gStyle->SetGridColor(kGray);
    cQY->Update();
    gQY->SetMarkerStyle(20);
    gQY->SetMarkerSize(1.); 
    gQY->SetMarkerColor(kBlue+2);
    gQY->GetXaxis()->SetLimits(-80, 80);
    gQY->GetYaxis()->SetRangeUser(0, 100);
    gQY->SetTitle("; Y [mm]; Q [pC]");
    gQY->Draw("APE");

    TCanvas *cQYnorm = new TCanvas("cQYnorm");
    cQYnorm->SetTicks(1,1); 
    cQYnorm->SetGrid(); gStyle->SetGridColor(kGray);
    cQYnorm->Update();
    gQYnorm->SetMarkerStyle(23);
    gQYnorm->SetMarkerSize(1.); 
    gQYnorm->SetMarkerColor(kOrange+10);
    gQYnorm->GetXaxis()->SetLimits(-80, 80);
    gQYnorm->GetYaxis()->SetRangeUser(0, 100);
    gQYnorm->SetTitle("; Y [mm]; Q [pC]");
    //gQYnorm->Draw("APE");
    TMultiGraph *mgQY = new TMultiGraph();
    mgQY->SetName("mgQY");
    mgQY->SetTitle("; Y [mm]; Q [pC]");
    mgQY->GetXaxis()->SetLimits(-80, 80);
    mgQY->GetYaxis()->SetRangeUser(0, 100);
    mgQY->Add(gQY);
    mgQY->Add(gQYnorm);
    mgQY->Draw("APE");

    TLegend *l1 = new TLegend(0.1,0.8,0.35,0.9);
    l1->AddEntry(gQY, "data");
    l1->AddEntry(gQYnorm, "normalised data");
    l1->Draw();    
    
    /*
    TCanvas *cQXY = new TCanvas("cQXY");
    gQXY->SetMarkerStyle(20);
    //gQXY->SetMarkerSize(0.6); 
    gQXY->SetMarkerColor(kBlue+2);
    gQXY->SetTitle("Position Scan XY; X [mm]; Y [mm]; Q [pC]");
    //gQXY->Draw("surf1");

    TCanvas *cQXYerr = new TCanvas("cQXYerr");
    gQXYerr->SetMarkerStyle(20);
    gQXYerr->SetMarkerColor(kBlue+2);
    gQXYerr->SetTitle("Position Scan XY; X [mm]; Y [mm]; Q [pC]");
    //gQXYerr->Draw("APE");
    */
    /*
    TCanvas *cNfpix = new TCanvas("cNfpix");
    cNfpix->SetTicks(1,1); 
    cNfpix->SetGrid(); gStyle->SetGridColor(kGray);
    cNfpix->Update();
    gNfpix->SetMarkerStyle(20);
    //gNfpix->SetMarkerSize(0.8);
    gNfpix->SetMarkerColor(kBlue+2);
    gNfpix->GetXaxis()->SetLimits(-80, 80);
    gNfpix->GetYaxis()->SetRangeUser(0, 1000);
    gNfpix->SetTitle("; Y [mm]; beam multiplicity");
    gNfpix->Draw("APE");
    */

    // Create a .root file
    TFile *outfile = new TFile("out_positionScanMCcut.root", "RECREATE");
    gQX->Write();
    gQY->Write();
    gQXY->Write();
    gQXYerr->Write();

    // Close the ROOT file
    outfile->Close();

    // Save the graph to a file
    cQX->SaveAs("cQX1.png");    cQX->SaveAs("cQX1.eps");
    cQY->SaveAs("cQY1.png");    cQY->SaveAs("cQY1.eps");
    //cQXY->SaveAs("cQXY1.png");
    //cQXYerr->SaveAs("cQXYerr1.png");
    //cQXnorm->SaveAs("cQXnorm1.png");
    cQYnorm->SaveAs("cQYnormMC.png");    cQYnorm->SaveAs("cQYnormMC.eps");
    //cNfpix->SaveAs("cNfpix1.png");    cNfpix->SaveAs("cNfpix1.eps");
}
