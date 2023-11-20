// Exclusion plots toy
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h> 
#include <TString.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TGraph.h> 
#include <TMultiGraph.h> 
#include <TAxis.h>
#include <TRandom.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TVector3.h>
#include <regex>

using namespace std;

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6){ //FIXING THE DIGITS CASTING FLOAT TO STRING
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

void DoScan(const char* OutputName){

//target and beam multiplicity
double MassEl = 0.511; //MeV
double Na  = 6.022e23;
double Z_C = 6;
double D_C = 3.5;
double A_C = 12;
double PreFactor = 3.8e-7;
double TThickness = 0.0097; //in cm
double CmtoPb = 1e-36;
double Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

vector<double> SAcceptance;
vector<double> eSAcceptance;
vector<double> SMYield;
vector<double> eSMYield;
vector<double> EBeam, SqrtS, NPoT;
vector<double> BEM, eSqrtS, eNPoT;
vector<double> Disc;
vector<double> eDisc;
double Bin9[5], NPoT9[5], Disc9[5];


ifstream EBeamInput("/home/mancinima/padme-fw/varieMarco/NPotE.txt");
double Ein, PoTin;
while (EBeamInput >> Ein >> PoTin) {
    cout<<Ein<<" - "<<PoTin<<endl;
    EBeam.push_back(Ein);
    SqrtS.push_back(TMath::Sqrt(Ein));
    BEM.push_back(Ein*0.003); //nominal beam energy spread before TARGET PROBLEM
    eSqrtS.push_back(TMath::Sqrt(Ein)*0.003);
    NPoT.push_back(PoTin);
    eNPoT.push_back(TMath::Sqrt(PoTin));
}
EBeamInput.close();
cout<<EBeam.size()<<endl;
static const int nPoint = EBeam.size();

double E[2000], MassX[2000];
double gve2[2000][nPoint], gve[2000][nPoint];
double gve2OLD[2000][nPoint], gveOLD[2000][nPoint];
double gve2_bin9[2000][5], gve_bin9[2000][5];

for(int i = 0; i<2000; i++){
    E[i] = 240 + i*0.05;
    MassX[i] = TMath::Sqrt(2*MassEl*E[i]);
    // cout<<"Energy thin scan: "<<E[i]<<" - InvMass: "<<MassX[i]<<endl;
}

//creating output file
TFile *FileOut = new TFile(OutputName, "RECREATE");

//reading input file with TGraphErrors
TFile* FileIn = new TFile("/home/mancinima/padme-fw/varieMarco/FULL.root", "READ");

TGraph *gSM = new TGraph();
TGraph *gAcc = new TGraph();
gSM = static_cast<TGraphErrors *>(FileIn->Get("Graph;6"));
gAcc = static_cast<TGraphErrors *>(FileIn->Get("Graph;1"));

cout<<"E_Beam"<<" "<<"NPoT"<<" "<<"Sqrt(s)"<<" "<<"Signal Acc"<<" "<<"N(2CL)/NPoT"<<" "<<"90%CL-excl"<<endl;

for(int iE = 0; iE<EBeam.size(); iE++){
    if (gSM && gAcc) {
        double xSM, ySM, eySM;
        // cout<<xSM<<endl;
        gSM->GetPoint(iE, xSM, ySM);
        SMYield.push_back(ySM);
        eSMYield.push_back(gSM->GetErrorY(iE));
        double xAcc, yAcc, eyAcc;
        gAcc->GetPoint(iE, xAcc, yAcc);
        SAcceptance.push_back(yAcc);
        eSAcceptance.push_back(gAcc->GetErrorY(iE));
        
    }
    else{
        cout<<"NON LEGGE I TGRAPH"<<endl;
        break;
    }

    Disc.push_back((1.3*TMath::Sqrt(SMYield[iE]*NPoT[iE]))/SAcceptance[iE]);
    cout<<" "<<EBeam[iE]<<" "<<NPoT[iE]<<" "<<SqrtS[iE]<<" "<<SAcceptance[iE]<<" "<<SMYield[iE]<<" "<<Disc[iE]<<endl;

    if(iE>1 && iE<11){
        Bin9[0] += SMYield[iE] * NPoT[iE];
        NPoT9[0] += NPoT[iE];
    } if(iE>10 && iE<20){
        Bin9[1] += SMYield[iE] * NPoT[iE];
        NPoT9[1] += NPoT[iE];
    } if(iE>19 && iE<29){
        Bin9[2] += SMYield[iE] * NPoT[iE];
        NPoT9[2] += NPoT[iE];
    } if(iE>28 && iE<38){
        Bin9[3] += SMYield[iE] * NPoT[iE];
        NPoT9[3] += NPoT[iE];
    } if(iE>37 && iE<47){
        Bin9[4] += SMYield[iE] * NPoT[iE];
        NPoT9[4] += NPoT[iE];
    }

    for(int k = 0; k < 2000; k++){
        gve2OLD[k][iE] = ((Disc[iE] * BEM[iE] * A_C * 1e24 * 9e-8)/(NPoT[iE] * PreFactor * Na * Z_C * D_C)) / TMath::Gaus(EBeam[iE], E[k], BEM[iE]);
        if(gve2OLD[k][iE] > 1.e-4) gve2OLD[k][iE] = 1.e-4;
        gveOLD[k][iE] = TMath::Sqrt(gve2OLD[k][iE]);    
        // cout<<"gve2OLD[k][iE]= "<< gve2OLD[k][iE] << endl;

        gve2[k][iE] = ((Disc[iE] * BEM[iE]*10 * A_C * 1e24 * 9e-8)/(NPoT[iE] * PreFactor * Na * Z_C * D_C)) / TMath::Gaus(EBeam[iE], E[k], BEM[iE]*10);
        if(gve2[k][iE] > 1.e-4) gve2[k][iE] = 1.e-4;
        gve[k][iE] = TMath::Sqrt(gve2[k][iE]);
        // cout<<"gve2[k][iE]= "<< gve2[k][iE] << endl;  
    }

}

for(int iB = 0; iB<5; iB++){
    double a = iB*9 +6;
    cout<<"prova: "<<Bin9[iB]<<" "<<NPoT9[iB]<<" "<<SAcceptance[a]<<endl;
    Disc9[iB] = 1.3*TMath::Sqrt(Bin9[iB])/SAcceptance[a];
    cout<<"quanti ne faccio? "<<Disc9[iB]<<endl;
    for(int k = 0; k < 2000; k++){
        gve2_bin9[k][iB] = ((Disc9[iB] * BEM[a]*10 * A_C * 1e24 * 9e-8)/(NPoT9[iB] * PreFactor * Na * Z_C * D_C)) / TMath::Gaus(EBeam[a], E[k], BEM[a]*10);
        if(gve2_bin9[k][iB] > 1.e-4) gve2_bin9[k][iB] = 1.e-4;
        gve_bin9[k][iB] = TMath::Sqrt(gve2_bin9[k][iB]);    
        // cout<<"gve2OLD[k][iE]= "<< gve2OLD[k][iE] << endl;
    }
}

double mingve[2000], mingveOLD[2000], mingve9points[2000], NA64[2000], Kloe[2000];
for(int k = 0; k<2000; k++){
    mingve[k] = TMath::MinElement(nPoint, gve[k]);
    mingveOLD[k] = TMath::MinElement(nPoint, gveOLD[k]);
    mingve9points[k] = TMath::MinElement(5, gve_bin9[k]);
    NA64[k] = 2e-4;
    Kloe[k] = 6e-4;
}

TGraph *ggveOLD = new TGraph(2000, &MassX[0], &mingveOLD[0]);
ggveOLD->SetTitle("NoTargetEffects");
ggveOLD->SetLineColor(kTeal-6);
ggveOLD->SetLineWidth(1);

TGraph *ggve = new TGraph(2000, &MassX[0], &mingve[0]);
ggve->SetTitle("ElectronMotion");
ggve->SetLineColor(kRed);
ggve->SetLineWidth(1);

TGraph *ggve9p = new TGraph(2000, &MassX[0], &mingve9points[0]);
ggve9p->SetTitle("ElectronMotion summing 9 bins");
ggve9p->SetLineColor(kBlue);
ggve9p->SetLineWidth(1);

TGraph *gNA64 = new TGraph(2000, &MassX[0], &NA64[0]);
TGraph *gKloe = new TGraph(2000, &MassX[0], &Kloe[0]);
TMultiGraph *mg = new TMultiGraph();
mg->Add(ggve);
mg->Add(ggve10p);
mg->Add(ggveOLD);
mg->Add(gNA64);
mg->Add(gKloe);

TCanvas *c1 = new TCanvas("c1", "c1", 900, 700);
mg->Draw("AC");

TGraphErrors *gSAcceptance = new TGraphErrors(nPoint, &SqrtS[0], &SAcceptance[0], &eSqrtS[0], &eSAcceptance[0]);
TGraphErrors *gSMYield = new TGraphErrors(nPoint, &SqrtS[0], &SMYield[0], &eSqrtS[0], &eSMYield[0]);

gSAcceptance->SetTitle("SAcceptance; EBeam [MeV]; Acceptance");
gSAcceptance->SetMarkerStyle(20);
gSAcceptance->SetMarkerColor(kMagenta);
gSAcceptance->SetMarkerSize(0.8);

gSMYield->SetTitle("SMYield; EBeam [MeV]; N(2CL)/N^{PoT}");
gSMYield->SetMarkerStyle(20);
gSMYield->SetMarkerColor(kBlack);
gSMYield->SetMarkerSize(0.8);

FileOut->cd();
gSAcceptance->Write();
gSMYield->Write();
mg->Write();

FileOut->Close();
}
