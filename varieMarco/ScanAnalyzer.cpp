// Background studies and sensitivity - 1 scan point in sqrt(s)
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
std::string to_string_with_precision(const T a_value, const int n = 6){ //CLASS FIXING THE DIGITS CASTING FLOAT TO STRING
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return std::move(out).str();
}

void DoScan(const char* OutputName){

//target and beam multiplicity
double MassEl = 0.511; //MeV
double PoT = 1e10;
double Na  = 6.022e23;
double Z_C = 6;
double D_C = 3.5;
double A_C = 12;
double TThickness = 0.0097; //in cm
double CmtoPb = 1e-36;
double Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

map<double, double> mSCrossSection, mFCrossSection, mGCrossSection;
map<double, double> mSInstantLumi, mFInstantLumi, mGInstantLumi;
vector<double> SAcceptance, GAcceptance;
vector<double> eSAcceptance, eGAcceptance;
vector<double> SYield, FYield, GYield, SMYield;
vector<double> eSYield, eFYield, eGYield, eSMYield;
vector<double> FullOvGG;
vector<double> eFullOvGG;
vector<double> EBeam, SqrtS;
vector<double> BEM, eSqrtS;

ifstream EBeamInput("/home/mancinima/padme-fw/varieMarco/Run4EnPoints.txt");
double val;
while (EBeamInput >> val) {
    cout<<val<<endl;
    EBeam.push_back(val);
    SqrtS.push_back(TMath::Sqrt(val));
    BEM.push_back(val*0.003);
    eSqrtS.push_back(TMath::Sqrt(val)*0.003);
    cout<<"beam energy "<<val<<endl;
}
EBeamInput.close();

//random generator to exstimate systematic effects
// TRandom3 *r3=new TRandom3();

//creating output file with TGraphErrors
TFile *FileOut = new TFile(OutputName, "RECREATE");

TFile* FileIn = new TFile("/home/mancinima/padme-fw/varieMarco/Run4Projection/merged_output.root", "READ");
ofstream TxtOut;
TxtOut.open("Run4ToyScan.txt");
TxtOut<<"#"<<"E_Beam"<<" "<<"Sqrt(s)"<<" "<<"Signal Acc"<<" "<<"N(2CL)/NPoT"<<" "<<"N(ee)/NPoT"<<" "<<"N(AA)/NPoT"<<" "<<"N(ee)/N(AA)"<<"\n";
cout<<EBeam.size()<<endl;
for(int iE = 0; iE<EBeam.size(); iE++){
    ifstream StxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaSCh/BhabhaSCh_" + to_string_with_precision(EBeam[iE], 2) +".txt").c_str()); 
    string Sline;
    ifstream FtxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaFull/BhabhaFull_" + to_string_with_precision(EBeam[iE], 2) +".txt").c_str()); 
    string Fline;
    ifstream GtxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProduction/AAProduction_" + to_string_with_precision(EBeam[iE], 2) +".txt").c_str());
    string Gline;
    string searchString = "#Cross_";
    regex numberRegex("\\d+\\.?\\d*(?:[eE][-+]?\\d+)?");

    while (getline(StxtIn, Sline)) { // Leggi ogni linea nel file di testo
        if (Sline.find(searchString) != string::npos) { // Se la linea contiene la stringa cercata
            smatch match;
            if (regex_search(Sline, match, numberRegex)) {
                string numberString = match.str();
                mSCrossSection[EBeam[iE]] = stod(numberString);
                mSInstantLumi[EBeam[iE]] = stod(numberString) * Luminosity_C;
                // cout << "Numero trovato: " << stod(numberString) << endl;
                break; 
            }
        }
    }
    StxtIn.close(); //chiudi il file di testo SChannel
    cout<<"la sezione d'urto letta dal file txt in input Sch: "<<mSCrossSection[EBeam[iE]]<<" pb  all'energia di: "<<EBeam[iE]<<" MeV"<<endl;

    while (getline(FtxtIn, Fline)) { 
        if (Fline.find(searchString) != string::npos) { 
            smatch match;
            if (regex_search(Fline, match, numberRegex)) {
                string numberString = match.str();
                mFCrossSection[EBeam[iE]] = stod(numberString);
                mFInstantLumi[EBeam[iE]] = stod(numberString) * Luminosity_C;
                // cout << "Numero trovato: " << stod(numberString) << endl;
                break; 
            }
        }
    }
    FtxtIn.close(); 
    cout<<"la sezione d'urto letta dal file txt in input Full: "<<mFCrossSection[EBeam[iE]]<<" pb all'energia di: "<<EBeam[iE]<<" MeV"<<endl;

    while (getline(GtxtIn, Gline)) { 
        if (Gline.find(searchString) != string::npos) { 
            smatch match;
            if (regex_search(Gline, match, numberRegex)) {
                string numberString = match.str();
                mGCrossSection[EBeam[iE]] = stod(numberString);
                mGInstantLumi[EBeam[iE]] = stod(numberString) * Luminosity_C;
                // cout << "Numero trovato: " << stod(numberString) << endl;
                break; 
            }
        }
    }
    StxtIn.close(); 
    cout<<"la sezione d'urto letta dal file txt in input AA: "<<mGCrossSection[EBeam[iE]]<<" pb all'energia di: "<<EBeam[iE]<<" MeV"<<endl;

    //lettura TH2D in input e creazione dei TGraph
    TDirectory* Sdir  = (TDirectory*)FileIn->GetDirectory(TString::Format("BhabhaSCh_dir%.2f", EBeam[iE]));
    TH2D* SOccNoCuts  = (TH2D*)Sdir->Get(("hSOcc_P3_" + to_string_with_precision(EBeam[iE], 2)).c_str());
    TH2D* SOccAllCuts = (TH2D*)Sdir->Get(("hSOccAllCuts_P3_" + to_string_with_precision(EBeam[iE], 2)).c_str());
    
    TDirectory* Fdir  = (TDirectory*)FileIn->GetDirectory(TString::Format("BhabhaFull_dir%.2f", EBeam[iE]));
    TH2D* FOccNoCuts  = (TH2D*)Fdir->Get(("hFOcc_P3_"+ to_string_with_precision(EBeam[iE], 2)).c_str());
    TH2D* FOccAllCuts = (TH2D*)Fdir->Get(("hFOccAllCuts_P3_" + to_string_with_precision(EBeam[iE], 2)).c_str());
    
    TDirectory* Gdir  = (TDirectory*)FileIn->GetDirectory(TString::Format("AAProduction_dir%.2f", EBeam[iE]));
    TH2D* GOccNoCuts  = (TH2D*)Gdir->Get(("hGOcc_P3_"+ to_string_with_precision(EBeam[iE], 2)).c_str());
    TH2D* GOccAllCuts = (TH2D*)Gdir->Get(("hGOccAllCuts_P3_" + to_string_with_precision(EBeam[iE], 2)).c_str());

    // double effRnd = r3->Uniform(0.975, 1.); //random variable added to scan point in order to evaluate possible systematic effect of the broken SU
    double effRnd = 1.;
    SYield.push_back(effRnd*mSInstantLumi[EBeam[iE]] * SOccAllCuts->GetEntries() / SOccNoCuts->GetEntries()); 
    eSYield.push_back(mSInstantLumi[EBeam[iE]]*
            (SOccAllCuts->GetEntries()*TMath::Sqrt(SOccNoCuts->GetEntries()) + TMath::Sqrt(SOccAllCuts->GetEntries())*SOccNoCuts->GetEntries())/
            (SOccNoCuts->GetEntries()*SOccNoCuts->GetEntries()));

    FYield.push_back(effRnd*mFInstantLumi[EBeam[iE]] * FOccAllCuts->GetEntries() / FOccNoCuts->GetEntries());
    eFYield.push_back(mFInstantLumi[EBeam[iE]]*
            (FOccAllCuts->GetEntries()*TMath::Sqrt(FOccNoCuts->GetEntries()) + TMath::Sqrt(FOccAllCuts->GetEntries())*FOccNoCuts->GetEntries())/
            (FOccNoCuts->GetEntries()*FOccNoCuts->GetEntries()));

    GYield.push_back(effRnd*mGInstantLumi[EBeam[iE]] * GOccAllCuts->GetEntries() / GOccNoCuts->GetEntries()); 
    eGYield.push_back(mGInstantLumi[EBeam[iE]]*
            (GOccAllCuts->GetEntries()*TMath::Sqrt(GOccNoCuts->GetEntries()) + TMath::Sqrt(GOccAllCuts->GetEntries())*GOccNoCuts->GetEntries())/
            (GOccNoCuts->GetEntries()*GOccNoCuts->GetEntries()));

    SMYield.push_back(FYield[iE] + GYield[iE]); 
    eSMYield.push_back(eFYield[iE] + eGYield[iE]);

    FullOvGG.push_back((mFInstantLumi[EBeam[iE]]*FOccAllCuts->GetEntries())/(mGInstantLumi[EBeam[iE]]*GOccAllCuts->GetEntries()));
    eFullOvGG.push_back((mFInstantLumi[EBeam[iE]]/mGInstantLumi[EBeam[iE]])*
            (FOccAllCuts->GetEntries()*TMath::Sqrt(GOccAllCuts->GetEntries()) + TMath::Sqrt(FOccAllCuts->GetEntries())*GOccAllCuts->GetEntries())/
            (GOccAllCuts->GetEntries()*GOccAllCuts->GetEntries()));

    SAcceptance.push_back(effRnd*SOccAllCuts->GetEntries() / SOccNoCuts->GetEntries());
    eSAcceptance.push_back((SOccAllCuts->GetEntries()*TMath::Sqrt(SOccNoCuts->GetEntries()) + TMath::Sqrt(SOccAllCuts->GetEntries())*SOccNoCuts->GetEntries())/
            (SOccNoCuts->GetEntries()*SOccNoCuts->GetEntries()));
    
    GAcceptance.push_back(effRnd*GOccAllCuts->GetEntries() / GOccNoCuts->GetEntries());
    eGAcceptance.push_back((GOccAllCuts->GetEntries()*TMath::Sqrt(GOccNoCuts->GetEntries()) + TMath::Sqrt(GOccAllCuts->GetEntries())*GOccNoCuts->GetEntries())/
            (GOccNoCuts->GetEntries()*GOccNoCuts->GetEntries()));
    
    cout<<" "<<EBeam[iE]<<" "<<SqrtS[iE]<<" "<<SAcceptance[iE]<<" "<<SMYield[iE]<<" "<<FYield[iE]<<" "<<GYield[iE]<<" "<<FullOvGG[iE]<<endl;
    TxtOut<<" "<<EBeam[iE]<<" "<<SqrtS[iE]<<" "<<SAcceptance[iE]<<" "<<SMYield[iE]<<" "<<FYield[iE]<<" "<<GYield[iE]<<" "<<FullOvGG[iE]<<"\n";
}
TxtOut.close();

// defining fitting function for bkg yield and sigmna acceptance
// TF1 *SFitFuncLow = new TF1("SFitFuncLow", "pol1", 200, 250);
// TF1 *SFitFuncRes = new TF1("SFitFuncRes", "pol1", 240, 405);
// TF1 *SFitFuncFull = new TF1("SFitFuncFull", "pol3", 200, 405);
// TF1 *SMFitFuncLow = new TF1("SMFitFuncLow", "pol1", 200, 250);
// TF1 *SMFitFuncRes = new TF1("SMFitFuncRes", "pol1", 240, 405);
// TF1 *SMFitFuncFull = new TF1("SMFitFuncFull", "pol3", 200, 405);
TF1 *SFitFuncLow = new TF1("SFitFuncLow", "pol1", 13.5, 16.5);
// TF1 *SFitFuncRes = new TF1("SFitFuncRes", "pol1", 14, 21);
TF1 *SFitFuncFull = new TF1("SFitFuncFull", "pol3", 13.5, 16.5);
TF1 *SMFitFuncLow = new TF1("SMFitFuncLow", "pol1", 13.5, 16.5);
// TF1 *SMFitFuncRes = new TF1("SMFitFuncRes", "pol1", 14, 21);
TF1 *SMFitFuncFull = new TF1("SMFitFuncFull", "pol3", 13.5, 16.5);


static const int nPoint = EBeam.size();

// TGraphErrors *gSAcceptance = new TGraphErrors(nPoint, &EBeam[0], &SAcceptance[0], &BEM[0], &eSAcceptance[0]);
TGraphErrors *gSAcceptance = new TGraphErrors(nPoint, &SqrtS[0], &SAcceptance[0], &eSqrtS[0], &eSAcceptance[0]);
gSAcceptance->Fit(SFitFuncLow, "R");
// gSAcceptance->Fit(SFitFuncRes, "R+");
// gSAcceptance->Fit(SFitFuncFull, "R");

// TGraphErrors *gGAcceptance = new TGraphErrors(nPoint, &EBeam[0], &GAcceptance[0], &BEM[0], &eGAcceptance[0]);
TGraphErrors *gGAcceptance = new TGraphErrors(nPoint, &SqrtS[0], &GAcceptance[0], &eSqrtS[0], &eGAcceptance[0]);
// TGraphErrors *gSYield = new TGraphErrors(nPoint, &EBeam[0], &SYield[0], &BEM[0], &eSYield[0]);
TGraphErrors *gSYield = new TGraphErrors(nPoint, &SqrtS[0], &SYield[0], &eSqrtS[0], &eSYield[0]);
// TGraphErrors *gFYield = new TGraphErrors(nPoint, &EBeam[0], &FYield[0], &BEM[0], &eFYield[0]);
TGraphErrors *gFYield = new TGraphErrors(nPoint, &SqrtS[0], &FYield[0], &eSqrtS[0], &eFYield[0]);
// TGraphErrors *gGYield = new TGraphErrors(nPoint, &EBeam[0], &GYield[0], &BEM[0], &eGYield[0]);
TGraphErrors *gGYield = new TGraphErrors(nPoint, &SqrtS[0], &GYield[0], &eSqrtS[0], &eGYield[0]);
// TGraphErrors *gSMYield = new TGraphErrors(nPoint, &EBeam[0], &SMYield[0], &BEM[0], &eSMYield[0]);
TGraphErrors *gSMYield = new TGraphErrors(nPoint, &SqrtS[0], &SMYield[0], &eSqrtS[0], &eSMYield[0]);
// gSMYield->Fit(SMFitFuncLow,"R");
// gSMYield->Fit(SMFitFuncRes,"R+");
gSMYield->Fit(SMFitFuncFull, "R");

// TGraphErrors *gFullOvGG = new TGraphErrors(nPoint, &EBeam[0], &FullOvGG[0], &BEM[0], &eFullOvGG[0]);
TGraphErrors *gFullOvGG = new TGraphErrors(nPoint, &SqrtS[0], &FullOvGG[0], &eSqrtS[0], &eFullOvGG[0]);
// gFullOvGG->Fit(fitFunc,"E");

// TH1F *hSAccRes = new TH1F("hSAccRes", "hSAccRes", 50, -0.05, 0.05);
// TH1F *hSMYieldRes = new TH1F("hSMYieldRes", "hSMYieldRes", 50, -0.05, 0.05);
// TH1F *hEBeamDiff = new TH1F("hEBeamDiff", "hEBeamDiff", 100, -2, 2);
// TH1F *hEDiff1 = new TH1F("hEDiff1", "hEDiff1", 100, -2, 2);
// TH1F *hEDiff2 = new TH1F("hEDiff2", "hEDiff2", 100, -2, 2);

// for (int i = 0; i < nPoint; ++i) {
//     if(i!=nPoint) hEBeamDiff->Fill(EBeam[i]- EBeam[i+1]);
//     if(EBeam[i]>350 || EBeam[i]<250){
//         hSAccRes->Fill(SAcceptance[i] - SFitFuncLow->Eval(SqrtS[i]));
//         hSMYieldRes->Fill(SMYield[i] - SMFitFuncLow->Eval(SqrtS[i]));
//     }
//     // else{
//     //     hSAccRes->Fill(SAcceptance[i] - SFitFuncRes->Eval(SqrtS[i]));
//     //     hSMYieldRes->Fill(SMYield[i] - SMFitFuncRes->Eval(SqrtS[i]));
//     // }
//     // hSAccRes->Fill(SAcceptance[i] - SFitFuncFull->Eval(SqrtS[i]));
//     // hSMYieldRes->Fill(SMYield[i] - SMFitFuncFull->Eval(SqrtS[i]));
//     // if(i<23) hEDiff1->Fill(EBeam[i]- EBeam[i+1]);
//     // if(i>22 && i!=nPoint) hEDiff2->Fill(EBeam[i]- EBeam[i+1]);
// }

gSAcceptance->SetTitle("SAcceptance; EBeam [MeV]; Acceptance");
gSAcceptance->SetMarkerStyle(20);
gSAcceptance->SetMarkerColor(kMagenta);
gSAcceptance->SetMarkerSize(0.8);
gGAcceptance->SetTitle("GAcceptance; EBeam [MeV]; Acceptance");
gGAcceptance->SetMarkerStyle(20);
gGAcceptance->SetMarkerColor(kBlue);
gGAcceptance->SetMarkerSize(0.8);

gSYield->SetTitle("SYield; EBeam [MeV]; N_{Sch}(e^{+}e^{-})/N^{PoT}");
gSYield->SetMarkerStyle(20);
gSYield->SetMarkerColor(kMagenta);
gSYield->SetMarkerSize(0.8);
gFYield->SetTitle("FYield; EBeam [MeV]; N_{Bhabha}(e^{+}e^{-})/N^{PoT}");
gFYield->SetMarkerStyle(20);
gFYield->SetMarkerColor(kGreen);
gFYield->SetMarkerSize(0.8);
gGYield->SetTitle("GYield; EBeam [MeV]; N(#gamma#gamma)/N^{PoT}");
gGYield->SetMarkerStyle(20);
gGYield->SetMarkerColor(kBlue);
gGYield->SetMarkerSize(0.8);
gSMYield->SetTitle("SMYield; EBeam [MeV]; N(2CL)/N^{PoT}");
gSMYield->SetMarkerStyle(20);
gSMYield->SetMarkerColor(kBlack);
gSMYield->SetMarkerSize(0.8);
gFullOvGG->SetTitle("gFullOvGG; EBeam [MeV]; N_{Bhabha}(e^{+}e^{-})/N(#gamma#gamma))");
gFullOvGG->SetMarkerStyle(20);
gFullOvGG->SetMarkerColor(kBlack);
gFullOvGG->SetMarkerSize(0.8);

// gStyle->SetOptFit(1111);

FileOut->cd();
gSAcceptance->Write();
gGAcceptance->Write();
gSYield->Write();
gFYield->Write();
gGYield->Write();
gSMYield->Write();
gFullOvGG->Write();

// hSAccRes->Write();
// hSMYieldRes->Write();
// hEBeamDiff->Write();
// hEDiff1->Write();
// hEDiff2->Write();

FileOut->Close();
}
