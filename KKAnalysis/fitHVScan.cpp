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

void processrun(int nrun, double voltage, double energy, int npeaks, double xmax, double fitmax, std::vector<double> vecA, std::vector<double> vecQ, std::vector<double> vecSQ) {
    printf("Processing run%d -- HV = %0.f\n", nrun, voltage);

    int writetofile = 1;
    TString infileName = Form("out_run%d.root",nrun);
    double ChargeCh1;

    // Some default values
    //npeaks = 6;
    //double xmax = 70.;
    double smax = 0.35;
    double xmin = -2.;
    double Nbins = 300;

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
    //double xminfit = -2.;
    //double xmaxfit = xmax*0.8;
    //std::cout << xminfit << " " << xmaxfit << std::endl;
    TF1 *sumgauss = new TF1("sumgauss", fitfun, xmin, fitmax, 3*npeaks);

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
    sumgauss->SetParameter(1,vecQ[0]); //sumgauss->SetParLimits(1, vecQ[0]-1., vecQ[0]+1.);
    sumgauss->SetParameter(2,vecSQ[0]); //sumgauss->SetParLimits(2, 0., vecSQ[0]+3.);

    sumgauss->SetParameter(3,vecA[1]);
    sumgauss->SetParameter(4,vecQ[1]); //sumgauss->SetParLimits(4, vecQ[1]-3., vecQ[1]+3.);
    sumgauss->SetParameter(5,vecSQ[1]); //sumgauss->SetParLimits(5, 0., vecSQ[1]+3.);

    sumgauss->SetParameter(6,vecA[2]);
    sumgauss->SetParameter(7,vecQ[2]); //sumgauss->SetParLimits(7, vecQ[2]-3., vecQ[2]+3.);
    sumgauss->SetParameter(8,vecSQ[2]); //sumgauss->SetParLimits(8, 0., vecSQ[2]+3.);

    sumgauss->SetParameter(9,vecA[3]);
    sumgauss->SetParameter(10,vecQ[3]); //sumgauss->SetParLimits(10, vecQ[3]-1., vecQ[3]+1.);
    sumgauss->SetParameter(11,vecSQ[3]); //sumgauss->SetParLimits(11, 0., vecSQ[3]+1.);

    sumgauss->SetParameter(12,vecA[4]);
    sumgauss->SetParameter(13,vecQ[4]); //sumgauss->SetParLimits(13, vecQ[4]-1., vecQ[4]+2.);
    sumgauss->SetParameter(14,vecSQ[4]); //sumgauss->SetParLimits(14, 0., vecSQ[4]+1.);

    sumgauss->SetParameter(15,vecA[5]);
    sumgauss->SetParameter(16,vecQ[5]); //sumgauss->SetParLimits(16, vecQ[5]-1., vecQ[5]+2.);
    sumgauss->SetParameter(17,vecSQ[5]); //sumgauss->SetParLimits(17, 0., vecSQ[5]+1.);

    //sumgauss->ReleaseParameter(16);
    //sumgauss->ReleaseParameter(17);
    
    // Fit the histogram -- charge distribution
    hCh1Q->Fit(sumgauss, "R");

    // Make a better fit
    //TF1 *sumgauss1 = new TF1("sumgauss1", fitfun, xmin, xmax, 3*N);
    //for (int i = 0; i < 3*N; i++) {
    //    sumgauss1->SetParameter(i,sumgauss->GetParameter(i));
    //}
    //hCh1Q->Fit(sumgauss1);

    // Open new .txt file to write results 
    std::ofstream HVScan(Form("HVScan%0.fnew.txt",energy), std::ios::app);
    if (!HVScan) { std::cerr << "Error opening file!" << std::endl; }

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
    if (stats) { stats->SetX1NDC(0.67); stats->SetX2NDC(0.98); stats->SetY1NDC(0.2); stats->SetY2NDC(0.935); }
    c1->Update();

    //TLegend *l1 = new TLegend(0.65,0.65,0.9,0.775);
    //l1->AddEntry(hCh1Q, "LeadGlass data");
    //l1->AddEntry(sumgauss, "Fit to the data");
    //l1->Draw();
    TLatex lc1; lc1.SetNDC(); lc1.SetTextSize(0.04); lc1.SetTextAlign(13);
    TString runNumber = baseName;
    lc1.DrawLatex(0.45,0.85,runNumber);
    TString strEnergy = TString::Format("E = %.0f MeV", energy);
    lc1.DrawLatex(0.45,0.8,strEnergy); 
    TString strVolt = TString::Format("HV = %.0f V", voltage);
    lc1.DrawLatex(0.45,0.75,strVolt);

    
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

    // Write p0 to HVScan file
    if(writetofile) {
        HVScan << voltage << " " << linefit->GetParameter(1) << " " << linefit->GetParError(1) << std::endl;    
        printf("Numbers written to file.\n");    
    }

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
    sigmafit->SetParameter(0,0.01); //sigmafit->SetParLimits(0,0.,1.);
    sigmafit->SetParameter(1,0.0001); //sigmafit->SetParLimits(1,0.,1.);
    sigmafit->SetParameter(2, 0.06); //sigmafit->SetParLimits(2,0.,1.);
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

    c1->SaveAs(Form("%s_hCh1ChargeFit1.png",baseName.Data()));
    c1->SaveAs(Form("%s_hCh1ChargeFit1.eps",baseName.Data()));
    cgrQ->SaveAs(Form("%s_grQEFit1.png",baseName.Data()));
    cgrQ->SaveAs(Form("%s_grQEFit1.eps",baseName.Data()));
    cgrSigmaQ->SaveAs(Form("%s_grSigmaQ1.png",baseName.Data()));
    cgrSigmaQ->SaveAs(Form("%s_grSigmaQ1.eps",baseName.Data()));

    HVScan.close();
    //newfile->Close();
    //infile->Close();
}

int fitHVScan() {
    double Energy = 450; // MeV
    double HV;
    int Np = 6;
    std::vector<double> vecA, vecmeanQ, vecsigmaQ; 

    if (Energy == 450) {
        // 1100 V
        HV = 1100;
        vecmeanQ = {0., 20., 40., 60., 80., 100.};
        vecsigmaQ = {0.5, 2., 3., 3., 4., 5.};
        vecA = {1000, 600, 500, 300, 200, 100};
        processrun(301, HV, Energy, Np, 140., 115., vecA, vecmeanQ, vecsigmaQ);

        // 1050 V
        HV = 1050;
        vecmeanQ = {0., 14., 28., 42., 56., 70.};
        vecsigmaQ = {0.4, 1.5, 2., 2.5, 3., 3.};
        vecA = {500, 200, 200, 100, 50, 20};
        processrun(302, HV, Energy, Np, 100., 80., vecA, vecmeanQ, vecsigmaQ);

        // 1000 V
        HV = 1000;
        vecmeanQ = {0., 10., 20., 30., 40., 50.};
        vecsigmaQ = {0.4, 1.4, 1.5, 2., 2., 2.5};
        vecA = {300, 100, 100, 50, 20, 10};
        processrun(303, HV, Energy, Np, 70., 55., vecA, vecmeanQ, vecsigmaQ);

        // 950 V
        HV = 950;
        vecmeanQ = {0., 6.75, 13.5, 20.25, 27., 33.75};
        vecsigmaQ = {0.4, 0.8, 1.1, 1.3, 1.4, 1.5};
        vecA = {300, 100, 100, 50, 20, 10};
        processrun(304, HV, Energy, Np, 45., 37., vecA, vecmeanQ, vecsigmaQ);
        
        // 900 V
        HV = 900;
        vecmeanQ = {0., 4.5, 9., 13.5, 18., 22.5};
        vecsigmaQ = {0.4, 0.6, 0.8, 1., 1.2, 1.4};
        vecA = {700, 500, 500, 300, 100, 50};
        processrun(305, HV, Energy, Np, 35., 25., vecA, vecmeanQ, vecsigmaQ);

        // 850 V
        HV = 850;
        vecmeanQ = {0., 2.5, 5.5, 8.5, 11.5, 14.5};
        vecsigmaQ = {0.4, 0.5, 0.7, 0.8, 0.9, 1.};
        vecA = {200, 200, 120, 80, 40, 20};
        processrun(306, HV, Energy, Np, 24., 16., vecA, vecmeanQ, vecsigmaQ);

        // 800 V
        HV = 800;
        vecmeanQ = {0., 1.5, 3., 5.5, 7., 8.5};
        vecsigmaQ = {0.3, 0.3, 0.4, 0.4, 0.4, 0.4};
        vecA = {200, 200, 120, 80, 40, 20};
        processrun(307, HV, Energy, Np, 15, 9.75, vecA, vecmeanQ, vecsigmaQ);

        // 750 V
        HV = 750;
        vecmeanQ = {0., 0.8, 2., 3., 4.25, 5.5};
        vecsigmaQ = {0.2, 0.22, 0.25, 0.25, 0.4, 0.5};
        vecA = {200, 250, 250, 200, 100, 50};
        processrun(308, HV, Energy, Np, 10., 6., vecA, vecmeanQ, vecsigmaQ);

        // 700 V -- only 5 peaks visible
        //HV = 700;
        //Np = 5;
        //vecmeanQ = {0., 0.8, 1.6, 2.4, 3.};
        //vecsigmaQ = {0.2, 0.2, 0.25, 0.3, 0.3};
        //vecA = {700, 500, 500, 300};
        //processrun(309, HV, Energy, Np, 8., 3.5, vecA, vecmeanQ, vecsigmaQ);

        // 650 V --- very difficult to fit
        //HV = 650;
        //Np = 4;
        //vecmeanQ = {0., 0.75, 1.5, 2.5};
        //vecsigmaQ = {0.4, 0.5, 0.7, 0.8};
        //vecA = {700, 500, 500, 300};
        //processrun(310, HV, Energy, Np, 8., 4., vecA, vecmeanQ, vecsigmaQ);
    }

    if (Energy == 285) {
        HV = 1100;
        vecmeanQ = {0., 12.5, 25., 37.5, 50., 62.5};
        vecsigmaQ = {0.3, 1., 1.5, 2., 2., 3.};
        vecA = {700, 300, 300, 250, 200, 100};
        processrun(300, HV, Energy, Np, 100., 70., vecA, vecmeanQ, vecsigmaQ);

        // 1050 V
        HV = 1050;
        vecmeanQ = {0., 9., 18., 27., 36., 45.};
        vecsigmaQ = {0.4, 1.2, 1.8, 2.1, 2.5, 2.5};
        vecA = {500, 250, 250, 200, 150, 100};
        processrun(293, HV, Energy, Np, 80., 49., vecA, vecmeanQ, vecsigmaQ);

        // 1000 V
        HV = 1000;
        vecmeanQ =  {0., 6., 12., 18., 24., 30.};
        vecsigmaQ = {0.4, 0.9, 1.3, 1.5, 1.7, 2.};
        vecA = {400, 250, 250, 100, 100, 50};
        processrun(294, HV, Energy, Np, 50., 34., vecA, vecmeanQ, vecsigmaQ);

        // 950 V
        HV = 950;
        vecmeanQ = {0., 4., 8., 12., 16., 20.};
        vecsigmaQ = {0.4, 0.7, 0.9, 1., 1.1, 1.2};
        vecA = {300, 200, 200, 200, 100, 50};
        processrun(295, HV, Energy, Np, 40., 23., vecA, vecmeanQ, vecsigmaQ);
        
        // 900 V
        HV = 900;
        vecmeanQ = {0., 2.5, 5., 7.5, 10., 12.5};
        vecsigmaQ = {0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
        vecA = {200, 250, 250, 200, 100, 50};
        processrun(296, HV, Energy, Np, 30., 15., vecA, vecmeanQ, vecsigmaQ);

        // 850 V
        HV = 850;
        vecmeanQ = {0., 1.5, 3., 4.5, 6., 7.5};
        vecsigmaQ = {0.05, 0.4, 0.5, 0.5, 0.6, 0.2};
        vecA = {200, 300, 350, 280, 200, 200};
        processrun(297, HV, Energy, 5, 25., 7.75, vecA, vecmeanQ, vecsigmaQ);

        // 800 V
        HV = 800;
        vecmeanQ = {0., 0.8, 2., 3., 4.2, 5.4};
        vecsigmaQ = {0.1, 0.2, 0.3, 0.3, 0.5, 0.5};
        vecA = {300, 300, 350, 200, 140, 120};
        processrun(298, HV, Energy, Np, 12, 6., vecA, vecmeanQ, vecsigmaQ);

        // 750 V -- only 4 peaks
        //HV = 750;
        //vecmeanQ = {0., 0.9, 1.8, 2.7};
        //vecsigmaQ = {0.2, 0.22, 0.25, 0.25, 0.4, 0.5};
        //vecA = {200, 250, 250, 200, 100, 50};
        //processrun(308, HV, Energy, Np, 10., 6.2, vecA, vecmeanQ, vecsigmaQ);
    }

    return 0;
}