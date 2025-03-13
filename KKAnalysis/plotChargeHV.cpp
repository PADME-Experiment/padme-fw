#include <iostream>
#include <fstream>
#include <vector>
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TFitResult.h"
#include "TMatrixD.h"

double gainfit(double *x, double *par) {
    double gain = par[0] * pow(x[0], par[1]);
    return gain;
}

void plotChargeHV() {
    // Choose ENERGY !!!
    double Energy = 450.; // 450 // 285 // 204.6 MeV

    TString infileName = Form("HVScan%d.txt",(int)Energy); // HVScan450 // HVScan285 // HVScan204
    double HVmin, HVmax;
    //if (Energy == 450.) { HVmin = 750.; HVmax = 1100.;}
    //if (Energy == 285.) { HVmin = 800.; HVmax = 1100.;}
    //if (Energy == 202.5) { HVmin = 800.; HVmax = 1050.;}
    
    TString baseName = infileName; 
    baseName = baseName(0, baseName.Last('.'));

    // Open file
    std::ifstream infile(infileName);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    // Skip the first row (header)
    std::string header;
    std::getline(infile, header); // Read and discard the first line

    std::vector<double> vecHV, vecQE, vecQEerr, vecLogHV, vecLogQE, vecLogQEerr; //vecGain, vecGainErr, vecLogGain, vecLogGainErr;
    double HV, QE, QEerr; //Gain, GainErr;

    // Read data from file
    while (infile >> HV >> QE >> QEerr) {
        vecHV.push_back(HV/1000.);
        vecLogHV.push_back(log(HV/1000.));
        vecQE.push_back(QE);
        vecQEerr.push_back(QEerr);
        vecLogQE.push_back(log(QE));
        vecLogQEerr.push_back(QEerr/QE);
        //Gain = QE*1e-12/(1.6e-19*0.331);
        //vecGain.push_back(Gain);
        //GainErr = Gain*(QEerr/QE + 0.006/0.331);
        //vecGainErr.push_back(GainErr);
        //vecLogGain.push_back(log(Gain));
        //vecLogGainErr.push_back(GainErr/Gain);
    }
    infile.close();

    // Number of points
    int n = vecHV.size();
    double xmin = 0.650;
    double xmax = 1.250;

    // Draw the graphs
    TCanvas *c1 = new TCanvas("c1", "QEHV"); c1->cd();
    c1->SetTicks(1,1);
    c1->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1);
    gStyle->SetStatX(0.46);
    gStyle->SetStatY(0.9);  
    c1->Update();
    // Plot the Q/E(HV)
    TGraphErrors *gQE = new TGraphErrors(n, &vecHV[0], &vecQE[0], 0, &vecQEerr[0]);
    gQE->SetName("graphQE");
    gQE->SetTitle(";HV [kV]; Q/E [pC/MeV]");
    gQE->SetMarkerStyle(20);    // Full circle marker
    gQE->SetMarkerColor(kBlue); // Blue color for markers
    gQE->SetLineColor(kBlue);   // Blue color for line
    gQE->GetXaxis()->SetLimits(xmin,xmax);
    gQE->GetYaxis()->SetRangeUser(0.0,0.07);    
    gQE->Draw("APE"); 

    TF1 *fitfun = new TF1("fitfun", "gainfit", xmin, xmax, 2);
    fitfun->SetParameter(0, 1);
    fitfun->SetParameter(1, 8.0);
    gQE->Fit(fitfun, "R");
    //c1->SetLeftMargin(0.12);
    c1->Update();


    TCanvas *c2 = new TCanvas("c2", "logQEHV"); c2->cd();
    c2->SetTicks(1,1);
    c2->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1);
    gStyle->SetStatX(0.46);
    gStyle->SetStatY(0.9);  
    c2->SetLogy();  
    c2->Update();
    // Plot the Q/E(HV)
    TGraphErrors *gLogQE = new TGraphErrors(n, &vecHV[0], &vecQE[0], 0, &vecQEerr[0]);
    gLogQE->SetName("graphLogQE");
    gLogQE->SetTitle(";HV [kV]; Q/E [pC/MeV]");
    gLogQE->SetMarkerStyle(20);    // Full circle marker
    gLogQE->SetMarkerColor(kBlue); // Blue color for markers
    gLogQE->SetLineColor(kBlue);   // Blue color for line
    gLogQE->GetXaxis()->SetLimits(xmin,xmax);
    gLogQE->GetYaxis()->SetRangeUser(0.0005,0.2);    
    gLogQE->Draw("APE");

    //TF1 *fitfun = new TF1("fitfun", "gainfit", xmin, xmax, 2);
    fitfun->SetParameter(0, 1);
    fitfun->SetParameter(1, 8.);
    gLogQE->Fit(fitfun, "R");
    //c2->SetLeftMargin(0.12);

    TLatex lgrQ; lgrQ.SetNDC(); lgrQ.SetTextSize(0.04); lgrQ.SetTextAlign(13);
    TString energy = TString::Format("Energy = %.0f MeV", Energy);
    lgrQ.DrawLatex(0.15,0.7,energy);
    TString eq1 = TString::Format("Q/E = %.2f*(HV)^%.2f \n", fitfun->GetParameter(0), fitfun->GetParameter(1));
    lgrQ.DrawLatex(0.15,0.625,eq1);
    c2->Update();
    
    
    TCanvas *c3 = new TCanvas("c3", "logQElogHV"); c3->cd();
    c3->SetTicks(1,1);
    c3->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1);
    gStyle->SetStatX(0.46);
    gStyle->SetStatY(0.9);    
    c3->Update();
    // Plot the Q/E(HV)
    TGraphErrors *gLogQELogHV = new TGraphErrors(n, &vecLogHV[0], &vecLogQE[0], 0, &vecLogQEerr[0]);
    gLogQELogHV->SetName("graphLogQELogHV");
    gLogQELogHV->SetTitle(";ln(HV) [kV]; ln(Q/E) [pC/MeV]");
    gLogQELogHV->SetMarkerStyle(20);    // Full circle marker
    gLogQELogHV->SetMarkerColor(kBlue); // Blue color for markers
    gLogQELogHV->SetLineColor(kBlue);   // Blue color for line
    gLogQELogHV->GetXaxis()->SetLimits(log(xmin),log(xmax));
    gLogQELogHV->GetYaxis()->SetRangeUser(-8.,-1.);
    gLogQELogHV->Draw("APE");

    TF1 *linefit = new TF1("linefit", "[0] + [1]*(x)", log(xmin), log(xmax));
    linefit->SetParameter(0,-1.);
    linefit->SetParameter(1,8.);
    // Get fit results 
    TFitResultPtr fitresult = gLogQELogHV->Fit(linefit, "RS");
    TMatrixD cov = fitresult->GetCorrelationMatrix();
    TMatrixD cor = fitresult->GetCovarianceMatrix();
    cov.Print();
    cor.Print();
    //linefit->Draw("same");

    //TLatex lgrQ; lgrQ.SetNDC(); lgrQ.SetTextSize(0.04); lgrQ.SetTextAlign(13);
    //TString energy = TString::Format("Energy = %.0f MeV", Energy);
    lgrQ.DrawLatex(0.15,0.7,energy);
    TString eq2 = TString::Format("ln(Q/E) = %.2f + %.2f*ln(HV) \n", linefit->GetParameter(0), linefit->GetParameter(1));
    lgrQ.DrawLatex(0.15,0.625,eq2);
    c3->Update();

    // Print result for Q/E(650V)
    double p0 = linefit->GetParameter(0);
    double p1 = linefit->GetParameter(1);
    double p0err = linefit->GetParError(0);
    double p1err = linefit->GetParError(1); 
    double logQE650 = p0 + p1*log(650.);
    std::cout << "log(Q/E(650)) = " << logQE650 << std::endl;
    double QE650 = exp(logQE650);
    std::cout << "Q/E(650) = " << QE650 << " pC/MeV" << std::endl;
    double logQE650err = logQE650*(sqrt(pow(p0err/p0, 2) + pow(p1err/p1, 2)));
    std::cout << "delta(log(Q/E(650))) = " << logQE650err << std::endl;
    double QE650err = QE650*sqrt(pow(logQE650err,2));
    std::cout << "delta(Q/E(650)) = " << QE650err << " pC/MeV" << std::endl;

    /*
    TCanvas *c4 = new TCanvas("c4", "Gain"); c4->cd();
    c4->SetTicks(1,1);
    c4->SetGrid(); gStyle->SetGridColor(kGray);
    gStyle->SetOptFit(1);
    gStyle->SetStatX(0.46);
    gStyle->SetStatY(0.9);    
    c4->Update();
    // Plot the Q/E(HV)
    TGraphErrors *gLogGain = new TGraphErrors(n, &vecLogHV[0], &vecLogGain[0], 0, &vecLogGainErr[0]);
    gLogGain->SetName("gLogGain");
    gLogGain->SetTitle(";ln(HV) [V]; ln(Gain)");
    gLogGain->SetMarkerStyle(20);    // Full circle marker
    gLogGain->SetMarkerColor(kBlue); // Blue color for markers
    gLogGain->SetLineColor(kBlue);   // Blue color for line
    gLogGain->GetXaxis()->SetLimits(log(650),log(1250));
    gLogGain->GetYaxis()->SetRangeUser(9.,15.);  
    gLogGain->Draw("APE");

    xmin = log(650.);
    //double xmax = log(HVmax);
    TF1 *linefit1 = new TF1("linefit1", "[0] + [1]*x", xmin, xmax);
    gLogGain->Fit(linefit1, "R");

    //TLatex lgrQ; lgrQ.SetNDC(); lgrQ.SetTextSize(0.04); lgrQ.SetTextAlign(13);
    //TString energy = TString::Format("Energy = %.0f MeV", Energy);
    //lgrQ.DrawLatex(0.15,0.7,energy);
    //TString eq1 = TString::Format("ln(Q/E) = %.2f + %.2f*ln(HV) \n", linefit->GetParameter(0), linefit->GetParameter(1));
    //lgrQ.DrawLatex(0.15,0.62,eq1);
    c4->Update();
    */

    /*
    double logQE650 = linefit->GetParameter(0) + linefit->GetParameter(1)*log(650);
    std::cout << "log(Q/E(650)) = " << logQE650 << std::endl;
    double QE650 = exp(logQE650);
    std::cout << "Q/E(650) = " << QE650 << " pC/MeV" << std::endl;
    std::cout << "Q1e(650) = " << QE650*402.5 << " pC" << std::endl;
    

    double logQE1000 = linefit->GetParameter(0) + linefit->GetParameter(1)*log(1000);
    std::cout << "log(Q/E(1000)) = " << logQE1000 << std::endl;
    double QE1000 = exp(logQE1000);
    std::cout << "Q/E(1000) = " << QE1000 << " pC/MeV" << std::endl;
    std::cout << "Q1e(1000) = " << QE1000*402.5 << " pC" << std::endl;


    double logQE900 = linefit->GetParameter(0) + linefit->GetParameter(1)*log(900);
    std::cout << "log(Q/E(900)) = " << logQE900 << std::endl;
    double QE900 = exp(logQE900);
    std::cout << "Q/E(900) = " << QE900 << " pC/MeV" << std::endl;
    //std::cout << "Q1e(900) = " << QE900*450 << " pC" << std::endl;
    */

    // Create a .root file
    TFile *outfile = new TFile("QEHV.root", "RECREATE");
    // Write the TGraphErrors object into the file
    gQE->Write(Form("Charge%s", baseName.Data()));
    //gLogQE->Write(Form("LogCharge%s", baseName.Data()));
    //gLogQE->Write(Form("LogCharge%s", baseName.Data()));

    c1->SaveAs(Form("ChargeHV%s.png", baseName.Data()));
    //c1->SaveAs(Form("Charge%s.eps", baseName.Data()));
    c2->SaveAs(Form("logChargeHV%s.png", baseName.Data()));
    c2->SaveAs(Form("logChargeHV%s.eps", baseName.Data()));

    c3->SaveAs(Form("logChargelogHV%s.png", baseName.Data()));
    c3->SaveAs(Form("logChargelogHV%s.eps", baseName.Data()));

    // Close the ROOT file
    outfile->Close();
    // Save the graph to a file
    //c1->SaveAs("sigmaEcut.pdf");
}