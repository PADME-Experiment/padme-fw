#include "TFile.h"
#include "TTree.h"
#include "TString.h"
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

double fitfun(double *x, double *p) {
    double fun = p[0] * exp(-0.5 * pow((x[0] - p[1])/p[2], 2)) +
                 p[3] * exp(-0.5 * pow((x[0] - p[4])/p[5], 2)) +
                 p[6] * exp(-0.5 * pow((x[0] - p[7])/p[8], 2)) + 
                 p[9] * exp(-0.5 * pow((x[0] - p[10])/p[11], 2)) + 
                 p[12] * exp(-0.5 * pow((x[0] - p[13])/p[14], 2)) + 
                 p[15] * exp(-0.5 * pow((x[0] - p[16])/p[17], 2));
    return fun;
}

double sigmafitfun(double *x, double *p) {
    double E = x[0]/1000;
    double sigmafun = sqrt(pow(p[0]/sqrt(E), 2) + pow(p[1]/E, 2) + pow(p[2], 2));
    return sigmafun;
}

void processrun(int nrun, double energy, int npeaks, double xmax, std::vector<double> vecA, std::vector<double> vecQ, std::vector<double> vecSQ) {
    printf("Processing run%d -- Energy = %0.f\n", nrun, energy);

    int writetofile = 1;
    TString infileName = Form("out_run%dcut.root",nrun);
    double Voltage = 900; //V
    double ChargeCh1;

    // Some default values
    //npeaks = 6;
    //double xmax = 70.;
    double smax = 0.35;
    double xmin = -5.;
    double Nbins = (xmax-xmin)*10;

    // Define histogram
    TH1F* hCh1Q = new TH1F("hCh1Charge","; Q [pC]; Number of events",Nbins,xmin,xmax);

    // Open ROOT file for input
    TFile *infile = new TFile(Form("../%s",infileName.Data()));
    if(!infile || infile->IsZombie()) {
        fprintf(stderr,"ERROR - Cannot open file %s\n",infile->GetName());
        exit(1);
    }
    else {printf("Opening file %s\n",infileName.Data());}

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

    // Open new ROOT file for output
    infileName = infileName(0, infileName.Last('.'));
    TFile *newfile = new TFile(Form("%sfit.root",infileName.Data()), "RECREATE");

    ////
    TString baseName = infileName; 
    baseName = baseName(baseName.Last('_')+1, 6); 
    //baseName = baseName(0, baseName.Last('.')-3);
    printf("Fitting the histogram for LeadGlass %s ... \n", baseName.Data());
    ////

    // Define the fitting function = sum of gaussians
    double xminfit = -2.;
    double xmaxfit = hCh1Q->GetXaxis()->GetXmax() - 15.;
    std::cout << xminfit << " " << xmaxfit << std::endl;
    TF1 *sumgauss = new TF1("sumgauss", fitfun, xminfit, xmaxfit, 3*npeaks);

    sumgauss->SetParName(0,"A_{0}");
    sumgauss->SetParName(1,"Q_{0e}");
    sumgauss->SetParName(2,"#sigma_{0}");
    sumgauss->SetParName(3,"A_{1}");
    sumgauss->SetParName(4,"Q_{1e}");
    sumgauss->SetParName(5,"#sigma_{1}");
    sumgauss->SetParName(6,"A_{2}");
    sumgauss->SetParName(7,"Q_{2e}");
    sumgauss->SetParName(8,"#sigma_{2}");
    sumgauss->SetParName(9,"A_{3}");
    sumgauss->SetParName(10,"Q_{3e}");
    sumgauss->SetParName(11,"#sigma_{3}");
    sumgauss->SetParName(12,"A_{4}");
    sumgauss->SetParName(13,"Q_{4e}");
    sumgauss->SetParName(14,"#sigma_{4}");
    sumgauss->SetParName(15,"A_{5}");
    sumgauss->SetParName(16,"Q_{5e}");
    sumgauss->SetParName(17,"#sigma_{5}");

    sumgauss->SetParameter(0,vecA[0]);
    sumgauss->SetParameter(1,vecQ[0]); 
    sumgauss->SetParameter(2,vecSQ[0]); 

    sumgauss->SetParameter(3,vecA[1]);
    sumgauss->SetParameter(4,vecQ[1]); 
    sumgauss->SetParameter(5,vecSQ[1]);

    sumgauss->SetParameter(6,vecA[2]);
    sumgauss->SetParameter(7,vecQ[2]); 
    sumgauss->SetParameter(8,vecSQ[2]); 

    sumgauss->SetParameter(9,vecA[3]);
    sumgauss->SetParameter(10,vecQ[3]); 
    sumgauss->SetParameter(11,vecSQ[3]); 

    sumgauss->SetParameter(12,vecA[4]);
    sumgauss->SetParameter(13,vecQ[4]); //sumgauss->SetParLimits(13, vecQ[4]-1., vecQ[4]+1.);
    sumgauss->SetParameter(14,vecSQ[4]); //sumgauss->SetParLimits(14, 0., vecSQ[4]+1.);

    sumgauss->SetParameter(15,vecA[5]);
    sumgauss->SetParameter(16,vecQ[5]); //sumgauss->SetParLimits(16, vecQ[5]-1., vecQ[5]+1.);
    sumgauss->SetParameter(17,vecSQ[5]); //sumgauss->SetParLimits(17, 0., vecSQ[5]+1.);

    //sumgauss->ReleaseParameter(16);
    //sumgauss->ReleaseParameter(17);
    
    // Fit the histogram -- charge distribution
    hCh1Q->Fit(sumgauss, "R");

    // Open new .txt file to write results 
    std::ofstream EnergyScan("EnergyScan.txt", std::ios::app);
    if (!EnergyScan) { std::cerr << "Error opening file!" << std::endl; }

    // Get the fit parameters
    std::vector<double> vecE, vecMeanQ, vecMeanQerr, vecSigmaQ, vecSigmaQerr, vecSigmaQrel, vecSigmaQrelErr;
    //printf("Fit parameters for the sum of Gaussians:\n");
    for (int i = 0; i < npeaks; i++) {
        double ampQ = sumgauss->GetParameter(3*i);
        //double meanQ0 = sumgauss->GetParameter(1);
        double meanQ = sumgauss->GetParameter(3*i+1);
        double meanQerr = sumgauss->GetParError(3*i+1);
        double sigmaQ = sumgauss->GetParameter(3*i+2);
        double sigmaQerr = sumgauss->GetParError(3*i+2);

        vecE.push_back(i*energy);
        vecMeanQ.push_back(meanQ);
        vecMeanQerr.push_back(meanQerr);
        vecSigmaQ.push_back(sigmaQ);
        vecSigmaQrel.push_back(sigmaQ/meanQ);
        vecSigmaQrelErr.push_back((sigmaQ/meanQ)*((sigmaQerr/sigmaQ)+(meanQerr/meanQ)));

        //TString textQ = TString::Format("Q%d = %.2f \n", i, meanQ);
        //txt1.DrawLatex(xpos,ypos-i*yoffset,textQ);

        // Write numbers to file
        if(writetofile) {
            EnergyScan << i << " " << energy << " " << meanQ << " " << meanQerr << " " << sigmaQ << " " << sigmaQerr << std::endl;    
            printf("Numbers written to file.\n");
        }
    }

    // Draw Charge histogram
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
    if (stats) { stats->SetX1NDC(0.6); stats->SetX2NDC(0.9); stats->SetY1NDC(0.2); stats->SetY2NDC(0.9); }
    c1->Update();

    //TLegend *l1 = new TLegend(0.65,0.65,0.9,0.775);
    //l1->AddEntry(hCh1Q, "LeadGlass data");
    //l1->AddEntry(sumgauss, "Fit to the data");
    //l1->Draw();
    TLatex lc1; lc1.SetNDC(); lc1.SetTextSize(0.04); lc1.SetTextAlign(13);
    TString runNumber = baseName;
    lc1.DrawLatex(0.4,0.85,runNumber);
    TString strEnergy = TString::Format("E = %.0f MeV", energy);
    lc1.DrawLatex(0.4,0.8,strEnergy); 
    TString strVolt = TString::Format("HV = %.0f V", Voltage);
    lc1.DrawLatex(0.4,0.75,strVolt);

    
    // Draw Charge - Energy graph
    TCanvas* cgrQ = new TCanvas("cgrQ"); cgrQ->cd(); cgrQ->Update();
    cgrQ->SetTicks(1,1);
    cgrQ->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1);
    gStyle->SetStatX(0.46);
    gStyle->SetStatY(0.9);    
    cgrQ->Update();
    TGraphErrors *graphQ = new TGraphErrors(npeaks, &vecE[0], &vecMeanQ[0], 0, &vecMeanQerr[0]);
    graphQ->SetName("graphQ");
    graphQ->SetTitle(";Energy [MeV]; Q [pC]");
    graphQ->SetMarkerStyle(20);
    graphQ->SetMarkerSize(1.2);
    graphQ->SetMarkerColor(kBlue);
    graphQ->Draw("AP");
    TF1 *linefit = new TF1("linefit", "[0] + [1]*x", 0, npeaks*energy);
    graphQ->Fit(linefit);


    TLatex lgrQ; lgrQ.SetNDC(); lgrQ.SetTextSize(0.04); lgrQ.SetTextAlign(13);
    //TString runNumber = baseName;
    lgrQ.DrawLatex(0.15,0.7,runNumber);
    //TString energy = TString::Format("E = %.0f MeV", Energy);
    lgrQ.DrawLatex(0.15,0.65,strEnergy); 
    //TString volt = TString::Format("HV = %.0f V", Voltage);
    lgrQ.DrawLatex(0.15,0.6,strVolt);
    TString k = TString::Format("Q/E = %.6f pC/MeV \n", linefit->GetParameter(1));
    lgrQ.DrawLatex(0.15,0.54,k);

    cgrQ->Update();
    //cgrQ->Draw();

    // Draw Sigma/Mean charge - Energy graph
    TCanvas* cgrSigmaQ = new TCanvas("cgrSigmaQ"); cgrSigmaQ->cd();
    cgrSigmaQ->SetTicks(1,1);
    cgrSigmaQ->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1);
    cgrSigmaQ->Update();
    TGraphErrors *graphSigmaQ = new TGraphErrors(npeaks, &vecE[0], &vecSigmaQrel[0], 0, &vecSigmaQrelErr[0]);
    graphSigmaQ->RemovePoint(0);
    graphSigmaQ->SetName("graphSigmaQ");
    graphSigmaQ->SetTitle(";Energy [MeV]; #sigma_{Q}/Q");
    graphSigmaQ->GetXaxis()->SetRangeUser(0.,2500.);
    graphSigmaQ->GetYaxis()->SetRangeUser(0.,smax);
    graphSigmaQ->SetMarkerStyle(20);
    graphSigmaQ->SetMarkerSize(1.2);
    graphSigmaQ->SetMarkerColor(kBlue+2);
    graphSigmaQ->Draw("AP");
    TF1 *sigmafit = new TF1("sigmafit", sigmafitfun, 0, npeaks*energy, 3);
    sigmafit->SetParameter(0,0.01); sigmafit->SetParLimits(0,0.,1.);
    sigmafit->SetParameter(1,0.0001); sigmafit->SetParLimits(1,0.,1.);
    sigmafit->SetParameter(2, 0.06); sigmafit->SetParLimits(2,0.,1.);
    graphSigmaQ->Fit(sigmafit);
    
    TLatex lgrSigmaQ; lgrSigmaQ.SetNDC(); lgrSigmaQ.SetTextSize(0.04); lgrSigmaQ.SetTextAlign(13);
    //TString runNumber = baseName;
    lgrSigmaQ.DrawLatex(0.5,0.85,runNumber);
    //TString energy = TString::Format("Energy = %f MeV", Energy);
    lgrSigmaQ.DrawLatex(0.5,0.8,strEnergy);
    //TString volt = TString::Format("HV = %.0f V", Voltage);
    lgrSigmaQ.DrawLatex(0.5,0.75,strVolt);
    TString sigmaformula = TString::Format("#frac{#sigma_{E}}{E} = #frac{%.3f}{#sqrt{E[GeV]}} #oplus #frac{%.3f}{E[GeV]} #oplus %.3f \n", 
                    sigmafit->GetParameter(0), sigmafit->GetParameter(1), sigmafit->GetParameter(2));
    lgrSigmaQ.DrawLatex(0.5,0.69,sigmaformula);

    cgrSigmaQ->Update();
    //cgrSigmaQ->Draw(); 

    hCh1Q->Write();
    c1->Write();
    graphQ->Write();
    cgrQ->Write();
    graphSigmaQ->Write();
    cgrSigmaQ->Write();

    c1->SaveAs(Form("%s_hCh1ChargeFit.png",baseName.Data()));
    c1->SaveAs(Form("%s_hCh1ChargeFit.eps",baseName.Data()));
    cgrQ->SaveAs(Form("%s_grQEFit.png",baseName.Data()));
    cgrQ->SaveAs(Form("%s_grQEFit.eps",baseName.Data()));
    cgrSigmaQ->SaveAs(Form("%s_grSigmaQ.png",baseName.Data()));
    cgrSigmaQ->SaveAs(Form("%s_grSigmaQ.eps",baseName.Data()));

    EnergyScan.close();
    //newfile->Close();
    //infile->Close();
}

int fitEnergyScan() {
    double Energy;
    int Np = 6;
    std::vector<double> vecA, vecmeanQ, vecsigmaQ; 
    // 450 MeV
    Energy = 450;
    vecmeanQ = {0., 4.5, 9., 13.5, 18., 22.5};
    vecsigmaQ = {0.4, 0.6, 0.8, 1., 1., 1.1};
    vecA = {900, 700, 500, 200, 100, 50};
    processrun(316, Energy, Np, 40., vecA, vecmeanQ, vecsigmaQ);

    // 400 MeV
    Energy = 400;
    vecmeanQ = {0., 4., 8., 12., 16., 20.};
    vecsigmaQ = {0.4, 0.6, 0.8, 1., 1., 1.1};
    vecA = {800, 400, 400, 300, 200, 100};
    processrun(315, Energy, Np, 37., vecA, vecmeanQ, vecsigmaQ);

    // 350 MeV
    Energy = 350;
    vecmeanQ = {0., 3.5, 7., 10.5, 14., 17.5};
    vecsigmaQ = {0.4, 0.6, 0.8, 1., 1., 1.3};
    vecA = {3000, 2000, 1000, 500, 200, 100};
    processrun(314, Energy, Np, 35., vecA, vecmeanQ, vecsigmaQ);

    // 300 MeV
    Energy = 300;
    vecmeanQ = {0., 3., 6., 9., 12., 15.};
    vecsigmaQ = {0.4, 0.6, 0.8, 1., 1., 1.1};
    vecA = {800, 400, 400, 300, 200, 100};
    processrun(313, Energy, Np, 32, vecA, vecmeanQ, vecsigmaQ);

    // 250 MeV
    Energy = 250;
    vecmeanQ = {0., 2.5, 5., 7.5, 10., 12.5};
    vecsigmaQ = {0.4, 0.5, 0.6, 0.7, 0.9, 1.3};
    vecA = {700, 600, 500, 300, 150, 20};
    processrun(312, Energy, 5, 26., vecA, vecmeanQ, vecsigmaQ);

    // 202 MeV
    //Energy = 202;
    //vecmeanQ = {0., 2., 4., 6., 8., 10.};
    //vecsigmaQ = {0.3, 0.4, 0.7, 0.7, 1., 1.5};
    //vecA = {500,500,400,200,100,50};
    //processrun(311, Energy, 27, vecA, vecmeanQ, vecsigmaQ);

    return 0;
}