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

void processrun(int nrun, double Xpos, double Ypos) {
    // Choose #RUN and X and Y !!!
    //int nrun = 318;
    //double X = 0;
    //double Y = 0;
    TString infileName = Form("out_run%dcut.root",nrun);
    double Energy = 285; //MeV
    double Voltage = 650; //V
    double A, Q, sQ;

    ////
    TString baseName = infileName; 
    baseName = baseName(baseName.Last('_')+1, 10); baseName = baseName(0, baseName.Last('.')-3);
    printf("Fitting the histogram for LeadGlass %s ... \n",baseName.Data());
    ////

    // Open ROOT file for input
    TFile *infile = new TFile(infileName);
    if(!infile || infile->IsZombie()) {
        fprintf(stderr,"ERROR - Cannot open file %s\n",infile->GetName());
        exit(1);
    }
    // Get the histogram
    TH1F *hist = dynamic_cast<TH1F*>(infile->Get("hCh1Charge"));
    if(!hist) {
        fprintf(stderr, "ERROR - Histogram not found\n");
        exit(1);
    }
    // Clone the histogram
    TH1F *hCh1Q = dynamic_cast<TH1F*>(hist->Clone());

    // Draw the histogram
    TCanvas *c1 = new TCanvas(); c1->cd();
    hCh1Q->SetTitle("");
    //hCh1Q->Draw(); 
    //c1->Draw();
   
    // Open new ROOT file for output
    infileName = infileName(0, infileName.Last('.'));
    TFile *newfile = new TFile(Form("%sfit.root",infileName.Data()), "RECREATE");

    // Define the fitting function
    double xmin = 5.;
    double xmax = hCh1Q->GetXaxis()->GetXmax();
    TF1 *gaussfit = new TF1("gaussfit", "gaus", xmin, xmax);

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
    std::ofstream PosScan("PosScan.txt", std::ios::app);
    if (!PosScan) { std::cerr << "Error opening file!" << std::endl; }
    // Write numbers to file
    PosScan << nrun << " " << Xpos << " " << Ypos << " " << meanQ << " " << meanQerr << " " << sigmaQ << " " << sigmaQerr << std::endl;    
    printf("Numbers for run%d written to file.\n", nrun);
    // Close file
    PosScan.close();

    // Draw histogram
    c1->cd();
    hCh1Q->SetLineColor(38);
    hCh1Q->SetFillColor(38);
    //double ymax = hCh1Q->GetBinContent(hCh1Q->GetMaximumBin());
    //hCh1Q->GetYaxis()->SetRangeUser(0,1.21*ymax);
    c1->Update();
    gStyle->SetOptFit(1);
    TPaveStats *stats = (TPaveStats*)hCh1Q->GetListOfFunctions()->FindObject("stats");
    if (stats) { 
        stats->SetX1NDC(0.75); 
        //stats->SetX2NDC(0.9); 
        stats->SetY1NDC(0.6); 
        //stats->SetY2NDC(0.9); 
    }
    c1->Update();

    //TLegend *l1 = new TLegend(0.65,0.65,0.9,0.775);
    //l1->AddEntry(hCh1Q, "LeadGlass data");
    //l1->AddEntry(sumgauss, "Fit to the data");
    //l1->Draw();
    TLatex lc1; lc1.SetNDC(); lc1.SetTextSize(0.04); lc1.SetTextAlign(13);
    TString runNumber = baseName;
    lc1.DrawLatex(0.15,0.85,runNumber);
    TString xy = TString::Format("X = %0.f mm; Y = %0.f mm", Xpos, Ypos);
    lc1.DrawLatex(0.15,0.8,xy); 

    
    hCh1Q->Write();
    c1->Write();

    c1->SaveAs(Form("%s_hCh1ChargeFit.png",baseName.Data()));
    c1->SaveAs(Form("%s_hCh1ChargeFit.eps",baseName.Data()));
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