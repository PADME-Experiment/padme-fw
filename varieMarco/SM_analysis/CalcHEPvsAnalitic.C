{
#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TStyle.h> 
#include <TCanvas.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>


  
  using namespace std;
  double alpha = 0.0072997270;
  double p[1200];
  double s[1200];
  double SQRT_s [1200];
  double MassE = 0.511;
  double ThetaCM[1200];
  double Thetalab[1200];
  double logT;
  double rappT;
  double velC = 2.99792; //m/s
  double h = 6.58212; //eVs
  //double p_0 = 282;

  double CS_calchepT[4] = {5.1330e10, 5.2630e10, 5.3835e10, 5.4383e10};
  double err_T [4] = {2e8, 2e8, 9e8, 6e7};
  double CS_calchepS[4] = {4.2644e8, 3.0222e8, 1.9808e8, 1.5482e8};
  double err_S [4] = {11, 8, 5, 4};
  double CS_calchepA[4] = {3.6425e9, 2.7336e9, 1.9148e9, 1.5531e9};
  double err_A [4] = {2e4, 3e4, 2e4, 2e4};
  double ex[4] = {0, 0, 0, 0};


  double CS_T[1200], CS_S[1200], CS_A[1200];

  double p0 = 100;
  double a, b, c, d, e;
  double ERin = 5;
  double sinTheta[1200];
  for(int i = 0; i<1200; i++){
    p[i] = p0;
    s[i] = 2*MassE*MassE + 2*p[i]*MassE;
    SQRT_s[i] = TMath::Sqrt(s[i]);
    //cout<<s[i]<<endl;
    Thetalab[i] = TMath::ACos((ERin*p[i] + MassE*ERin - MassE*p[i] - MassE*MassE)/(TMath::Sqrt(ERin*ERin - MassE*MassE)*TMath::Sqrt(p[i]*p[i] - MassE*MassE)));
    ThetaCM[i] = TMath::ASin(TMath::Sin(Thetalab[i]) * TMath::Sqrt(ERin*ERin - MassE*MassE)/TMath::Sqrt(s[i]/4 - MassE*MassE));
    logT = TMath::Log(0.5 * (1-TMath::Cos(ThetaCM[i])));
    rappT = -0.5 + 1/(1- TMath::Cos(ThetaCM[i]));
    a = (1 - 4*MassE*MassE/s[i]);
    b = (s[i]*s[i]/2 + 2*MassE*MassE*MassE*MassE - 2*s[i]*MassE*MassE ) / ((s[i]/2 - 2*MassE*MassE)*(s[i]/2 - 2*MassE*MassE));
    c = -1 + (1 + 4*MassE*MassE/s[i])/a + TMath::Log((1 + a)/(1 - a));
    d = - (8*MassE*MassE*MassE*MassE)/(s[i]*s[i]*a);
    e = (s[i] * a)/(2*MassE*MassE) + TMath::Log((1 + a)/(1 - a));
    
    CS_S[i] = (((4*TMath::Pi()*alpha*alpha)/(3*s[i]*s[i])) * (s[i] + 4*MassE*MassE + 4*MassE*MassE*MassE*MassE/s[i])) * 1e12 * (velC*velC*h*h);
    CS_T[i] = ((TMath::Pi()*4*alpha*alpha/s[i]) * ( 0.5 + logT/a + b*rappT))* 1e12 * (velC*velC*h*h);
    CS_A[i] = ((2*TMath::Pi()*alpha*alpha)/(s[i]*a) * (c + d*e))* 1e12 * (velC*velC*h*h);
    p0 = p0 + 0.5;

    
  }
  
  double p_calchep [4] = {200, 282, 430, 550};
  double s_calchep[4], SQRT_s_calchep[4];
  for(int i = 0; i<4 ; i++){
    s_calchep[i] = 2*MassE*MassE + 2*p_calchep[i]*MassE;
    SQRT_s_calchep[i] = TMath::Sqrt(s_calchep[i]);
  }

  
  TGraph *S_analitic = new TGraph(1200, SQRT_s, CS_S);
  S_analitic->SetTitle("analytic S-Channel; #sqrt(s) [MeV]; #sigma [pb]");
  S_analitic->SetLineColor(kBlack);
  S_analitic->SetLineStyle(7);
  S_analitic->SetLineWidth(2);
  S_analitic->SetMaximum(5e11); S_analitic->SetMinimum(9e6);
  S_analitic->GetXaxis()->SetRangeUser(10.5, 26.1);
  TGraph *T_analitic = new TGraph(1200, SQRT_s, CS_T);
  T_analitic->SetTitle("analytic T-Channel; #sqrt(s) [MeV]; #sigma [pb]");
  T_analitic->SetLineColor(kBlack);
  T_analitic->SetLineStyle(1);
  T_analitic->SetLineWidth(2);
  T_analitic->SetMaximum(5e11); T_analitic->SetMinimum(9e6);
  T_analitic->GetXaxis()->SetRangeUser(10.5, 26.1);

  TGraph *A_analitic = new TGraph(1200, SQRT_s, CS_A);
  A_analitic->SetTitle("analytic #gamma#gamma-production; #sqrt(s) [MeV]; #sigma [pb]");
  A_analitic->SetLineColor(kBlack);
  A_analitic->SetLineStyle(9);
  A_analitic->SetLineWidth(2);
  A_analitic->SetMaximum(5e11); A_analitic->SetMinimum(9e6);
  A_analitic->GetXaxis()->SetRangeUser(10.5, 26.1);

  

  TGraphErrors *S_calchep = new TGraphErrors(4, SQRT_s_calchep, CS_calchepS, ex, err_S);
  S_calchep->SetTitle("CalcHEP S-Channel; #sqrt(s) [MeV]; #sigma [pb]");
  S_calchep->SetMarkerColor(kRed);
  S_calchep->SetMarkerSize(3);
  S_calchep->SetMarkerStyle(20);
  S_calchep->SetMaximum(5e11); S_calchep->SetMinimum(9e6);
  S_calchep->GetXaxis()->SetRangeUser(10.5, 26.1);

  TGraphErrors *T_calchep = new TGraphErrors(4, SQRT_s_calchep, CS_calchepT, ex, err_T);
  T_calchep->SetTitle("CalcHEP T-Channel; #sqrt(s) [MeV]; #sigma [pb]");
  T_calchep->SetMarkerColor(kGreen+3);
  T_calchep->SetMarkerSize(3);
  T_calchep->SetMarkerStyle(21);
  T_calchep->SetMaximum(5e11); T_calchep->SetMinimum(9e6);
  T_calchep->GetXaxis()->SetRangeUser(10.5, 26.1);

  TGraphErrors *A_calchep = new TGraphErrors(4, SQRT_s_calchep, CS_calchepA, ex, err_A);
  A_calchep->SetTitle("CalcHEP #gamma#gamma-production; #sqrt(s) [MeV]; #sigma [pb]");
  A_calchep->SetMarkerColor(kBlue);
  A_calchep->SetMarkerSize(3);
  A_calchep->SetMarkerStyle(22);
  A_calchep->SetMaximum(5e11); A_calchep->SetMinimum(9e6);
  A_calchep->GetXaxis()->SetRangeUser(10.5, 26.1);
  
  TMultiGraph *tot = new TMultiGraph();
  tot->Add(S_analitic, "C");
  tot->Add(T_analitic, "C");
  tot->Add(A_analitic, "C");
  tot->Add(S_calchep, "P");
  tot->Add(T_calchep, "P");
  tot->Add(A_calchep, "P");
  tot->SetTitle("T-, S-Channel and #gamma#gamma-production cross section; #sqrt(s) [MeV]; #sigma [pb]");
  tot->SetMaximum(8e11); T_calchep->SetMinimum(9e6);


  TMultiGraph *totA = new TMultiGraph();
  
  totA->SetTitle("#gamma#gamma-production cross section  ; #sqrt(s) [MeV]; #sigma [pb]");
  //totA->SetMaximum(5e11); T_calchep->SetMinimum(9e6);

  
  TCanvas *ST = new TCanvas("ST", "T- and S-Channel cross section", 900, 700);
  ST->SetGrid();
  tot->Draw("A");
  ST->BuildLegend();
  gPad->SetLogy();

  TCanvas *AA = new TCanvas("AA", "Pair creation #gamma#gamma cross section", 900, 700);
  AA->SetGrid();
  totA->Draw("A");
  AA->BuildLegend();
  gPad->SetLogy();

  
  
}
