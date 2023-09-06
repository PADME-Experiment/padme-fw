//MANCINI MARCO PADME SENSITIVITY
#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TStyle.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <regex>

#include "EventSelector.h"

using namespace std;

void PADME_sensitivity(double TargetCalo, double E0, double EStep, int NStepEBeam, double ThetaBoostX, double ThetaBoostY, double TargetX, double TargetY, const char* FileName){

double pi2 = TMath::Pi()*2;
static const int Nev_CalcHep = 1000000;
double ElectronMass = 0.000511;
// double TargetCalo = 3700; //in mm 23.06.21 con Mauro e Tommaso

double CaloNBins = 120;
double OccBins = 450;

// DEFINIZIONE DEI TAGLI
double En_min = 0.01; 
double Radius_min = 90.;
double MagnetShadow = TargetCalo*atan2(110,1528); //semiapertura del magnete/distanza target-magnete * distanza target-calo
cout<<"Ombra del magnete in [mm]: "<<MagnetShadow<<endl;
double Radius_max = 270.;

// verificare se sono numeri corretti
double PoT = 1e10;
double Na  = 6.022e23;
double Z_C = 6;
double D_C = 3.5;
double A_C = 12;
double TThickness = 0.0097; //in cm
double CmtoPb = 1e-36;
double Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;


double PreFactor_X17 = 3.8e-7; //DATA FROM PROCEEDINGS DARME-NARDI
// double g_ve[10]      = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4} ;

TString process[4] = {"T", "S", "F", "G"}; //S=Schannel, F=BhabhaFull, G=GammaGamma

map<double, double> mSCrossSection, mFCrossSection, mGCrossSection;
map<double, double> mSInstantLumi, mFInstantLumi, mGInstantLumi;
double SNev_accepted, FNev_accepted, GNev_accepted;

// double E0 = 200;
// double EStep = 2.;
// int NStepEBeam = 7;
double EFin = E0 + NStepEBeam*EStep;
float EBeam = 0.;
double Range = 0.400;
double Bin = 1600;

TFile *FileOut = new TFile(FileName, "RECREATE");
TH1F *hSAcceptance = new TH1F("hSAcceptance", "hSAcceptance", NStepEBeam, E0-EStep/2, EFin-EStep/2);
TH1F *hFAcceptance = new TH1F("hFAcceptance", "hFAcceptance", NStepEBeam, E0-EStep/2, EFin-EStep/2);
TH1F *hGAcceptance = new TH1F("hGAcceptance", "hGAcceptance", NStepEBeam, E0-EStep/2, EFin-EStep/2);
hSAcceptance->SetMarkerStyle(20);
hSAcceptance->SetMarkerColor(kRed);
hSAcceptance->SetMarkerSize(1);
hFAcceptance->SetMarkerStyle(20);
hFAcceptance->SetMarkerColor(kGreen);
hFAcceptance->SetMarkerSize(1);
hGAcceptance->SetMarkerStyle(20);
hGAcceptance->SetMarkerColor(kBlue);
hGAcceptance->SetMarkerSize(1);

TH1F *hSYield = new TH1F("hSYield", "S-channel YIELD", NStepEBeam, E0-EStep/2, EFin-EStep/2);
TH1F *hFYield = new TH1F("hFYield", "BhabhaFull YIELD", NStepEBeam, E0-EStep/2, EFin-EStep/2);
TH1F *hGYield = new TH1F("hGYield", "AA YIELD", NStepEBeam, E0-EStep/2, EFin-EStep/2);
// TH1F *hAcceptance = new TH1F("hAcceptance", "hAcceptance", NStepEBeam, E0-EStep/2, EFin-EStep/2);
hSYield->SetMarkerStyle(20);
hSYield->SetMarkerColor(kRed);
hSYield->SetMarkerSize(2);
hFYield->SetMarkerStyle(20);
hFYield->SetMarkerColor(kGreen);
hFYield->SetMarkerSize(2);
hGYield->SetMarkerStyle(20);
hGYield->SetMarkerColor(kBlue);
hGYield->SetMarkerSize(2);

for(int iEb=0; iEb<NStepEBeam; iEb++){
    EBeam = E0 + iEb*EStep;
    cout<<"Total energy of the "<<iEb<<" iteration: "<<EBeam<<endl;

    ifstream StxtIn(("/home/marco/analysis/SM_analysis/CalcHepFiles/Schannel/BhabhaSCh_" + to_string(static_cast<int>(EBeam)) +".txt").c_str()); // Apri il file di testo in input
    string Sline;
    ifstream FtxtIn(("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaFull/BhabhaFull_" + to_string(static_cast<int>(EBeam)) +".txt").c_str()); 
    string Fline;
    ifstream GtxtIn(("/home/marco/analysis/SM_analysis/CalcHepFiles/AAProd/AAProduction_" + to_string(static_cast<int>(EBeam)) +".txt").c_str()); 
    string Gline;
    string searchString = "#Cross_";
    regex numberRegex("\\d+\\.?\\d*(?:[eE][-+]?\\d+)?");

    while (getline(StxtIn, Sline)) { // Leggi ogni linea nel file di testo
        if (Sline.find(searchString) != string::npos) { // Se la linea contiene la stringa cercata
            smatch match;
            if (regex_search(Sline, match, numberRegex)) {
                string numberString = match.str();
                mSCrossSection[EBeam] = stod(numberString);
                mSInstantLumi[EBeam] = stod(numberString) * Luminosity_C;
                // cout << "Numero trovato: " << stod(numberString) << endl;
                break; 
            }
        }
    }
    StxtIn.close(); //chiudi il file di testo Schannel
    cout<<"la sezione d'urto letta dal file txt in input Sch: "<<mSCrossSection[EBeam]<<"pb  all'energia di: "<<EBeam<<endl;

    while (getline(FtxtIn, Fline)) { 
        if (Fline.find(searchString) != string::npos) { 
            smatch match;
            if (regex_search(Fline, match, numberRegex)) {
                string numberString = match.str();
                mFCrossSection[EBeam] = stod(numberString);
                mFInstantLumi[EBeam] = stod(numberString) * Luminosity_C;
                // cout << "Numero trovato: " << stod(numberString) << endl;
                break; 
            }
        }
    }
    FtxtIn.close(); 
    cout<<"la sezione d'urto letta dal file txt in input Full: "<<mFCrossSection[EBeam]<<" pb all'energia di: "<<EBeam<<endl;

    while (getline(GtxtIn, Gline)) { 
        if (Gline.find(searchString) != string::npos) { 
            smatch match;
            if (regex_search(Gline, match, numberRegex)) {
                string numberString = match.str();
                mGCrossSection[EBeam] = stod(numberString);
                mGInstantLumi[EBeam] = stod(numberString) * Luminosity_C;
                // cout << "Numero trovato: " << stod(numberString) << endl;
                break; 
            }
        }
    }
    StxtIn.close(); 
    cout<<"la sezione d'urto letta dal file txt in input AA: "<<mGCrossSection[EBeam]<<" pb all'energia di: "<<EBeam<<endl;

    TDirectory *SCh_dir = FileOut->mkdir(("Sch_dir" + to_string(EBeam)).c_str());
    TDirectory *Full_dir = FileOut->mkdir(("Full_dir"+ to_string(EBeam)).c_str());
    TDirectory *GG_dir = FileOut->mkdir(("GG_dir"+ to_string(EBeam)).c_str());
    TFile *BhabhaSChIn = TFile::Open(("/home/marco/analysis/SM_analysis/CalcHepFiles/Schannel/BhabhaSCh_" + to_string(static_cast<int>(EBeam)) +".root").c_str()); 
    TTree *tSch  = (TTree*)BhabhaSChIn->Get(("tBhabhaSCh_" + to_string(static_cast<int>(EBeam))).c_str());
    double P3pos_ini, P3ele_ini, SFinalP1_ele, SFinalP2_ele, SFinalP3_ele, SFinalP1_pos, SFinalP2_pos, SFinalP3_pos; // Definisci la variabile per il branch che desideri acquisire
    tSch->SetBranchAddress("InitialP3_positron", &P3pos_ini);
    tSch->SetBranchAddress("InitialP3_electron", &P3ele_ini);
    tSch->SetBranchAddress("SFinalP1_electron", &SFinalP1_ele);
    tSch->SetBranchAddress("SFinalP2_electron", &SFinalP2_ele);
    tSch->SetBranchAddress("SFinalP3_electron", &SFinalP3_ele);
    tSch->SetBranchAddress("SFinalP1_positron", &SFinalP1_pos);
    tSch->SetBranchAddress("SFinalP2_positron", &SFinalP2_pos);
    tSch->SetBranchAddress("SFinalP3_positron", &SFinalP3_pos);
    
    TH1F* hInitialP3_pos = new TH1F(("hInitialP3_pos_" + to_string(EBeam)).c_str(), ("hInitialP3_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hInitialP3_ele = new TH1F(("hInitialP3_ele_" + to_string(EBeam)).c_str(), ("hInitialP3_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinalP1_ele = new TH1F(("hSFinalP1_ele_" + to_string(EBeam)).c_str(), ("hSFinalP1_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinalP2_ele = new TH1F(("hSFinalP2_ele_" + to_string(EBeam)).c_str(), ("hSFinalP2_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinalP3_ele = new TH1F(("hSFinalP3_ele_" + to_string(EBeam)).c_str(), ("hSFinalP3_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinalP1_pos = new TH1F(("hSFinalP1_pos_" + to_string(EBeam)).c_str(), ("hSFinalP1_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinalP2_pos = new TH1F(("hSFinalP2_pos_" + to_string(EBeam)).c_str(), ("hSFinalP2_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinalP3_pos = new TH1F(("hSFinalP3_pos_" + to_string(EBeam)).c_str(), ("hSFinalP3_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    
    TFile *BhabhaFullIn = TFile::Open(("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaFull/BhabhaFull_" + to_string(static_cast<int>(EBeam)) +".root").c_str()); 
    // TFile *BhabhaFullIn  = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaFull.root");
    TTree *tFull  = (TTree*)BhabhaFullIn->Get(("tBhabhaFull_" + to_string(static_cast<int>(EBeam))).c_str()); 
    double FFinalP1_ele, FFinalP2_ele, FFinalP3_ele, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos; 
    tFull->SetBranchAddress("FFinalP1_electron", &FFinalP1_ele);
    tFull->SetBranchAddress("FFinalP2_electron", &FFinalP2_ele);
    tFull->SetBranchAddress("FFinalP3_electron", &FFinalP3_ele);
    tFull->SetBranchAddress("FFinalP1_positron", &FFinalP1_pos);
    tFull->SetBranchAddress("FFinalP2_positron", &FFinalP2_pos);
    tFull->SetBranchAddress("FFinalP3_positron", &FFinalP3_pos);

    TH1F* hFFinalP1_ele = new TH1F(("hFFinalP1_ele_" + to_string(EBeam)).c_str(), ("hFFinalP1_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinalP2_ele = new TH1F(("hFFinalP2_ele_" + to_string(EBeam)).c_str(), ("hFFinalP2_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinalP3_ele = new TH1F(("hFFinalP3_ele_" + to_string(EBeam)).c_str(), ("hFFinalP3_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinalP1_pos = new TH1F(("hFFinalP1_pos_" + to_string(EBeam)).c_str(), ("hFFinalP1_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinalP2_pos = new TH1F(("hFFinalP2_pos_" + to_string(EBeam)).c_str(), ("hFFinalP2_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinalP3_pos = new TH1F(("hFFinalP3_pos_" + to_string(EBeam)).c_str(), ("hFFinalP3_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);

    TFile *AAProductionIn = TFile::Open(("/home/marco/analysis/SM_analysis/CalcHepFiles/AAProd/AAProduction_" + to_string(static_cast<int>(EBeam)) +".root").c_str()); 
    //TFile *AAProductionIn  = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/AAprod.root");
    TTree *tGG  = (TTree*)AAProductionIn->Get(("tAAProduction_" + to_string(static_cast<int>(EBeam))).c_str()); 
    double GFinalP1_G1, GFinalP2_G1, GFinalP3_G1, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2; 
    tGG->SetBranchAddress("GFinalP1_G1", &GFinalP1_G1);
    tGG->SetBranchAddress("GFinalP2_G1", &GFinalP2_G1);
    tGG->SetBranchAddress("GFinalP3_G1", &GFinalP3_G1);
    tGG->SetBranchAddress("GFinalP1_G2", &GFinalP1_G2);
    tGG->SetBranchAddress("GFinalP2_G2", &GFinalP2_G2);
    tGG->SetBranchAddress("GFinalP3_G2", &GFinalP3_G2);

    TH1F* hGFinalP1_G1 = new TH1F(("hGFinalP1_G1_" + to_string(EBeam)).c_str(), ("hGFinalP1_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinalP2_G1 = new TH1F(("hGFinalP2_G1_" + to_string(EBeam)).c_str(), ("hGFinalP2_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinalP3_G1 = new TH1F(("hGFinalP3_G1_" + to_string(EBeam)).c_str(), ("hGFinalP3_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinalP1_G2 = new TH1F(("hGFinalP1_G2_" + to_string(EBeam)).c_str(), ("hGFinalP1_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinalP2_G2 = new TH1F(("hGFinalP2_G2_" + to_string(EBeam)).c_str(), ("hGFinalP2_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinalP3_G2 = new TH1F(("hGFinalP3_G2_" + to_string(EBeam)).c_str(), ("hGFinalP3_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);

    TH1F* hInEnergy_pos = new TH1F(("hInEnergy_pos_" + to_string(EBeam)).c_str(), ("hInEnergy_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hInEnergy_ele = new TH1F(("hInEnergy_ele_" + to_string(EBeam)).c_str(), ("hInEnergy_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinMomentum_pos = new TH1F(("hSFinMomentum_pos_" + to_string(EBeam)).c_str(), ("hSFinMomentum_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinMomentum_ele = new TH1F(("hSFinMomentum_ele_" + to_string(EBeam)).c_str(), ("hSFinMomentum_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinMomentum_pos = new TH1F(("hFFinMomentum_pos_" + to_string(EBeam)).c_str(), ("hFFinMomentum_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinMomentum_ele = new TH1F(("hFFinMomentum_ele_" + to_string(EBeam)).c_str(), ("hSFinMomentum_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinMomentum_G1 = new TH1F(("hGFinMomentum_G1_" + to_string(EBeam)).c_str(), ("hGFinMomentum_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinMomentum_G2 = new TH1F(("hGFinMomentum_G2_" + to_string(EBeam)).c_str(), ("hGFinMomentum_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);    
    TH1F* hSFinEnergy_pos = new TH1F(("hSFinEnergy_pos_" + to_string(EBeam)).c_str(), ("hSFinEnergy_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hSFinEnergy_ele = new TH1F(("hSFinEnergy_ele_" + to_string(EBeam)).c_str(), ("hSFinEnergy_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFFinEnergy_pos = new TH1F(("hFFinEnergy_pos_" + to_string(EBeam)).c_str(), ("hFFinEnergy_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);    
    TH1F* hFFinEnergy_ele = new TH1F(("hFFinEnergy_ele_" + to_string(EBeam)).c_str(), ("hFFinEnergy_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinEnergy_G1 = new TH1F(("hGFinEnergy_G1_" + to_string(EBeam)).c_str(), ("hGFinEnergy_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGFinEnergy_G2 = new TH1F(("hGFinEnergy_G2_" + to_string(EBeam)).c_str(), ("hGFinEnergy_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);

    TH1F* hSEn_tot = new TH1F(("hSEn_tot_" + to_string(EBeam)).c_str(), ("hSEn_tot_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hFullEn_tot = new TH1F(("hFullEn_tot_" + to_string(EBeam)).c_str(), ("hFullEn_tot_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    TH1F* hGGEn_tot = new TH1F(("hGGEn_tot_" + to_string(EBeam)).c_str(), ("hGGEn_tot_" + to_string(EBeam)).c_str(), Bin, -Range, Range);

    TH1F* hSTheta_Scattering_COM = new TH1F(("hSTheta_Scattering_COM_" + to_string(EBeam)).c_str(), ("hSTheta_Scattering_COM_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hFTheta_Scattering_COM = new TH1F(("hFTheta_Scattering_COM_" + to_string(EBeam)).c_str(), ("hFTheta_Scattering_COM_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hGTheta_Scattering_COM = new TH1F(("hGTheta_Scattering_COM_" + to_string(EBeam)).c_str(), ("hGTheta_Scattering_COM_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);

    TH1F* hSPosTheta_Scattering = new TH1F(("hSPosTheta_Scattering_" + to_string(EBeam)).c_str(), ("hSPosTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hSEleTheta_Scattering = new TH1F(("hSEleTheta_Scattering_" + to_string(EBeam)).c_str(), ("hSEleTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hFPosTheta_Scattering = new TH1F(("hFPosTheta_Scattering_" + to_string(EBeam)).c_str(), ("hFPosTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hFEleTheta_Scattering = new TH1F(("hFEleTheta_Scattering_" + to_string(EBeam)).c_str(), ("hFEleTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hGG1Theta_Scattering = new TH1F(("hGG1Theta_Scattering_" + to_string(EBeam)).c_str(), ("hGG1Theta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hGG2Theta_Scattering = new TH1F(("hGG2Theta_Scattering_" + to_string(EBeam)).c_str(), ("hGG2Theta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);

    TH1F* hSPhiX_pos = new TH1F(("hSPhiX_pos_" + to_string(EBeam)).c_str(), ("hSPhiX_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hSPhiY_pos = new TH1F(("hSPhiY_pos_" + to_string(EBeam)).c_str(), ("hSPhiY_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hSPhiX_ele = new TH1F(("hSPhiX_ele_" + to_string(EBeam)).c_str(), ("hSPhiX_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hSPhiY_ele = new TH1F(("hSPhiY_ele_" + to_string(EBeam)).c_str(), ("hSPhiY_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);

    TH1F* hFPhiX_pos = new TH1F(("hFPhiX_pos_" + to_string(EBeam)).c_str(), ("hFPhiX_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hFPhiY_pos = new TH1F(("hFPhiY_pos_" + to_string(EBeam)).c_str(), ("hFPhiY_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hFPhiX_ele = new TH1F(("hFPhiX_ele_" + to_string(EBeam)).c_str(), ("hFPhiX_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hFPhiY_ele = new TH1F(("hFPhiY_ele_" + to_string(EBeam)).c_str(), ("hFPhiY_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);

    TH1F* hGPhiX_G1 = new TH1F(("hGPhiX_G1_" + to_string(EBeam)).c_str(), ("hGPhiX_G1_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hGPhiY_G1 = new TH1F(("hGPhiY_G1_" + to_string(EBeam)).c_str(), ("hGPhiY_G1_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hGPhiX_G2 = new TH1F(("hGPhiX_G2_" + to_string(EBeam)).c_str(), ("hGPhiX_G2_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    TH1F* hGPhiY_G2 = new TH1F(("hGPhiY_G2_" + to_string(EBeam)).c_str(), ("hGPhiY_G2_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);

    TH1F* hSx_pos = new TH1F(("hSx_pos_" + to_string(EBeam)).c_str(), ("hSx_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hSy_pos = new TH1F(("hSy_pos_" + to_string(EBeam)).c_str(), ("hSy_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hSx_ele = new TH1F(("hSx_ele_" + to_string(EBeam)).c_str(), ("hSx_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hSy_ele = new TH1F(("hSy_ele_" + to_string(EBeam)).c_str(), ("hSy_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);

    TH1F* hFx_pos = new TH1F(("hFx_pos_" + to_string(EBeam)).c_str(), ("hFx_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hFy_pos = new TH1F(("hFy_pos_" + to_string(EBeam)).c_str(), ("hFy_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hFx_ele = new TH1F(("hFx_ele_" + to_string(EBeam)).c_str(), ("hFx_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hFy_ele = new TH1F(("hFy_ele_" + to_string(EBeam)).c_str(), ("hFy_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);

    TH1F* hGx_G1 = new TH1F(("hGx_G1_" + to_string(EBeam)).c_str(), ("hGx_G1_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hGy_G1 = new TH1F(("hGy_G1_" + to_string(EBeam)).c_str(), ("hGy_G1_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hGx_G2 = new TH1F(("hGx_G2_" + to_string(EBeam)).c_str(), ("hGx_G2_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hGy_G2 = new TH1F(("hGy_G2_" + to_string(EBeam)).c_str(), ("hGy_G2_" + to_string(EBeam)).c_str(), 400, -1000, 1000);

    TH1F* hSRadius_pos = new TH1F(("hSRadius_pos_" + to_string(EBeam)).c_str(), ("hSRadius_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hSRadius_ele = new TH1F(("hSRadius_ele_" + to_string(EBeam)).c_str(), ("hSRadius_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hFRadius_pos = new TH1F(("hFRadius_pos_" + to_string(EBeam)).c_str(), ("hFRadius_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hFRadius_ele = new TH1F(("hFRadius_ele_" + to_string(EBeam)).c_str(), ("hFRadius_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hGRadius_G1 = new TH1F(("hGRadius_G1_" + to_string(EBeam)).c_str(), ("hGRadius_G1_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    TH1F* hGRadius_G2 = new TH1F(("hGRadius_G2_" + to_string(EBeam)).c_str(), ("hGRadius_G2_" + to_string(EBeam)).c_str(), 400, -1000, 1000);

    //histos per lo studio di sensitività e accettanze
    TH2F* hSOccupancy_pos = new TH2F(("hSOccupancy_pos_" + to_string(EBeam)).c_str(), ("hSOccupancy_pos_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hSOccupancy_ele = new TH2F(("hSOccupancy_ele_" + to_string(EBeam)).c_str(), ("hSOccupancy_ele_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hFOccupancy_pos = new TH2F(("hFOccupancy_pos_" + to_string(EBeam)).c_str(), ("hFOccupancy_pos_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hFOccupancy_ele = new TH2F(("hFOccupancy_ele_" + to_string(EBeam)).c_str(), ("hFOccupancy_ele_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hGOccupancy_G1 = new TH2F(("hGOccupancy_G1_" + to_string(EBeam)).c_str(), ("hGOccupancy_G1_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hGOccupancy_G2 = new TH2F(("hGOccupancy_G2_" + to_string(EBeam)).c_str(), ("hGOccupancy_G2_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);

    TH2F* hSOccupancyGeom_pos = new TH2F(("hSOccupancyGeom_pos_" + to_string(EBeam)).c_str(), ("hSOccupancyGeom_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hSOccupancyGeom_ele = new TH2F(("hSOccupancyGeom_ele_" + to_string(EBeam)).c_str(), ("hSOccupancyGeom_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccupancyGeom_pos = new TH2F(("hFOccupancyGeom_pos_" + to_string(EBeam)).c_str(), ("hFOccupancyGeom_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccupancyGeom_ele = new TH2F(("hFOccupancyGeom_ele_" + to_string(EBeam)).c_str(), ("hFOccupancyGeom_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccupancyGeom_G1 = new TH2F(("hGOccupancyGeom_G1_" + to_string(EBeam)).c_str(), ("hGOccupancyGeom_G1_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccupancyGeom_G2 = new TH2F(("hGOccupancyGeom_G2_" + to_string(EBeam)).c_str(), ("hGOccupancyGeom_G2_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);

    TH2F* hSOccupancyAllCuts_pos = new TH2F(("hSOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), ("hSOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hSOccupancyAllCuts_ele = new TH2F(("hSOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), ("hSOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccupancyAllCuts_pos = new TH2F(("hFOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), ("hFOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccupancyAllCuts_ele = new TH2F(("hFOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), ("hFOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccupancyAllCuts_G1 = new TH2F(("hGOccupancyAllCuts_G1_" + to_string(EBeam)).c_str(), ("hGOccupancyAllCuts_G1_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccupancyAllCuts_G2 = new TH2F(("hGOccupancyAllCuts_G2_" + to_string(EBeam)).c_str(), ("hGOccupancyAllCuts_G2_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);

    for(int it = 0; it<tSch->GetEntries(); it++){
        tSch->GetEntry(it);
        tFull->GetEntry(it);
        tGG->GetEntry(it);
        hInitialP3_pos->Fill(P3pos_ini);
        hInitialP3_ele->Fill(P3ele_ini);
        hSFinalP1_ele->Fill(SFinalP1_ele);
        hSFinalP2_ele->Fill(SFinalP2_ele);
        hSFinalP3_ele->Fill(SFinalP3_ele);
        // cout<<"Px Py e Pz dell'elettrone in uscita S: "<<SFinalP1_ele<<SFinalP2_ele<<SFinalP3_ele<<endl;
        hSFinalP1_pos->Fill(SFinalP1_pos);
        hSFinalP2_pos->Fill(SFinalP2_pos);
        hSFinalP3_pos->Fill(SFinalP3_pos);
        // cout<<"Px Py e Pz del positrone in uscita S: "<<SFinalP1_pos<<SFinalP2_pos<<SFinalP3_pos<<endl;
        hFFinalP1_ele->Fill(FFinalP1_ele);
        hFFinalP2_ele->Fill(FFinalP2_ele);
        hFFinalP3_ele->Fill(FFinalP3_ele);
        // cout<<"Px Py e Pz dell'elettrone in uscita F: "<<FFinalP1_ele<<FFinalP2_ele<<FFinalP3_ele<<endl;
        hFFinalP1_pos->Fill(FFinalP1_pos);
        hFFinalP2_pos->Fill(FFinalP2_pos);
        hFFinalP3_pos->Fill(FFinalP3_pos);
        // cout<<"Px Py e Pz del positrone in uscita F: "<<FFinalP1_pos<<FFinalP2_pos<<FFinalP3_pos<<endl;
        hGFinalP1_G1->Fill(GFinalP1_G1);
        hGFinalP2_G1->Fill(GFinalP2_G1);
        hGFinalP3_G1->Fill(GFinalP3_G1);
        // cout<<"Px Py e Pz del primo gamma in uscita G: "<<GFinalP1_G1<<GFinalP2_G1<<GFinalP3_G1<<endl;
        hGFinalP1_G2->Fill(GFinalP1_G2);
        hGFinalP2_G2->Fill(GFinalP2_G2);
        hGFinalP3_G2->Fill(GFinalP3_G2);
        // cout<<"Px Py e Pz del secondo gamma in uscita G: "<<GFinalP1_G2<<GFinalP2_G2<<GFinalP3_G2<<endl;
        
        Particle P1(ElectronMass, 0, 0, P3pos_ini);
        Particle P2(ElectronMass, 0, 0, P3ele_ini);
        // cout<<"prova: "<<hInitialP3_pos->GetBinContent(it)<<endl;
        Particle Sch_P3(ElectronMass, SFinalP1_pos, SFinalP2_pos, SFinalP3_pos);
        Particle Sch_P4(ElectronMass, SFinalP1_ele, SFinalP2_ele, SFinalP3_ele);
        Particle Full_P3(ElectronMass, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos);
        Particle Full_P4(ElectronMass, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos);
        Particle G1(0, GFinalP1_G1, GFinalP2_G1, GFinalP3_G1);
        Particle G2(0, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2);

        //INITIAL STATE -> T,S CHANNEL and Ee->AA HAVE SAME INITIAL STATE
        hInEnergy_pos->Fill(P1.Energy());
        hInEnergy_ele->Fill(P2.Energy());
        // cout<<"prova: "<<P1.Energy()<<P2.Energy()<<endl;

        //FINAL STATE 3MOMENTUM DISTRIBUTION - BhabhaFull, S CHANNEL and Ee->AA
        hSFinMomentum_pos->Fill(Sch_P3.TriMomentum());
        hSFinMomentum_ele->Fill(Sch_P4.TriMomentum());
        hFFinMomentum_pos->Fill(Full_P3.TriMomentum());
        hFFinMomentum_ele->Fill(Full_P4.TriMomentum());
        hGFinMomentum_G1->Fill(G1.TriMomentum());
        hGFinMomentum_G2->Fill(G2.TriMomentum());
        // cout<<"prova: "<<Sch_P3.TriMomentum()<<G2.TriMomentum()<<endl;

        //FINAL STATE ENERGY DISTRIBUTION - BhabhaFull, S CHANNEL and Ee->AA
        hSFinEnergy_pos->Fill(Sch_P3.Energy());
        hSFinEnergy_ele->Fill(Sch_P4.Energy());
        hFFinEnergy_pos->Fill(Full_P3.Energy());
        hFFinEnergy_ele->Fill(Full_P4.Energy());
        hGFinEnergy_G1->Fill(G1.Energy());
        hGFinEnergy_G2->Fill(G2.Energy());

        //ENERGY CONSERVATION CHECK
        hSEn_tot->Fill(Sch_P3.Energy() + Sch_P4.Energy());
        hFullEn_tot->Fill(Full_P3.Energy() + Full_P4.Energy());
        hGGEn_tot->Fill(G1.Energy() + G2.Energy());

        //FINAL STATE ANGULAR DISTRIBUTION BETWEEN FINAL PARTICLES - BhabhaFull, S CHANNEL and Ee->AA
        //CoM frame
        hSTheta_Scattering_COM->Fill(Sch_P3.CoMScatteringAngle(Sch_P4));
        hFTheta_Scattering_COM->Fill(Full_P3.CoMScatteringAngle(Full_P4));
        hGTheta_Scattering_COM->Fill(G1.CoMScatteringAngle(G2));
        //lab frame
        hSPosTheta_Scattering->Fill(Sch_P3.ScatteringAngle());
        hSEleTheta_Scattering->Fill(Sch_P4.ScatteringAngle());
        hFPosTheta_Scattering->Fill(Full_P3.ScatteringAngle());
        hSEleTheta_Scattering->Fill(Full_P4.ScatteringAngle());
        hGG1Theta_Scattering->Fill(G1.ScatteringAngle());
        hGG1Theta_Scattering->Fill(G2.ScatteringAngle());

        //TRANSVERSE FINAL POSITION AT 3000 MM FROM TARGET - THETA PHI DEF
        hSPhiX_pos->Fill(Sch_P3.PhiX(ThetaBoostX));
        hSPhiY_pos->Fill(Sch_P3.PhiY(ThetaBoostY));
        hSPhiX_ele->Fill(Sch_P4.PhiX(ThetaBoostX));
        hSPhiY_ele->Fill(Sch_P4.PhiY(ThetaBoostY));
        hFPhiX_pos->Fill(Full_P3.PhiX(ThetaBoostX));
        hFPhiY_pos->Fill(Full_P3.PhiY(ThetaBoostY));
        hFPhiX_ele->Fill(Full_P4.PhiX(ThetaBoostX));
        hFPhiY_ele->Fill(Full_P4.PhiY(ThetaBoostY));
        hGPhiX_G1->Fill(G1.PhiX(ThetaBoostX));
        hGPhiY_G1->Fill(G1.PhiY(ThetaBoostY));
        hGPhiX_G2->Fill(G2.PhiX(ThetaBoostX));
        hGPhiY_G2->Fill(G2.PhiY(ThetaBoostY));

        //POSITION
        hSx_pos->Fill(Sch_P3.R_x(TargetCalo, ThetaBoostX, TargetX));
        hSy_pos->Fill(Sch_P3.R_y(TargetCalo, ThetaBoostY, TargetY));
        hSx_ele->Fill(Sch_P4.R_x(TargetCalo, ThetaBoostX, TargetX));
        hSy_ele->Fill(Sch_P4.R_y(TargetCalo, ThetaBoostY, TargetY));
        hFx_pos->Fill(Full_P3.R_x(TargetCalo, ThetaBoostX, TargetX));
        hFy_pos->Fill(Full_P3.R_y(TargetCalo, ThetaBoostY, TargetY));
        hFx_ele->Fill(Full_P4.R_x(TargetCalo, ThetaBoostX, TargetX));
        hFy_ele->Fill(Full_P4.R_y(TargetCalo, ThetaBoostY, TargetY));
        hGx_G1->Fill(G1.R_x(TargetCalo, ThetaBoostX, TargetX));
        hGy_G1->Fill(G1.R_y(TargetCalo, ThetaBoostY, TargetY));
        hGx_G2->Fill(G2.R_x(TargetCalo, ThetaBoostX, TargetX));
        hGy_G2->Fill(G2.R_y(TargetCalo, ThetaBoostY, TargetY));

        //final state radius at target
        hSRadius_pos->Fill(Sch_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY));
        // cout<<"prova hSRadius_pos: "<<Sch_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY)<<endl;
        hSRadius_ele->Fill(Sch_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY));
        // cout<<"prova hSRadius_ele: "<<Sch_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY)<<endl;
        hFRadius_pos->Fill(Full_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY));
        // cout<<"prova hFRadius_pos: "<<Full_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY)<<endl;
        hFRadius_ele->Fill(Full_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY));
        // cout<<"prova hFRadius_ele: "<<Full_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY)<<endl;
        hGRadius_G1->Fill(G1.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY));
        // cout<<"prova hGRadius_G1: "<<G1.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY)<<endl;
        hGRadius_G2->Fill(G2.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY));
        // cout<<"prova hGRadius_G2: "<<G2.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY)<<endl;

        EventAnalyzer SAnalyzer(hSOccupancy_pos, hSOccupancy_ele,
                                hSOccupancyGeom_pos, hSOccupancyGeom_ele,
                                hSOccupancyAllCuts_pos, hSOccupancyAllCuts_ele);
        EventAnalyzer FAnalyzer(hFOccupancy_pos, hFOccupancy_ele,
                                hFOccupancyGeom_pos, hFOccupancyGeom_ele,
                                hFOccupancyAllCuts_pos, hFOccupancyAllCuts_ele);
        EventAnalyzer GAnalyzer(hGOccupancy_G1, hGOccupancy_G2,
                                hGOccupancyGeom_G1, hGOccupancyGeom_G2,
                                hGOccupancyAllCuts_G1, hGOccupancyAllCuts_G2);

        SAnalyzer.AnalyzeEventsFull(Radius_max, Radius_min, En_min, 
                                Sch_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY), Sch_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY),
                                Sch_P3.R_x(TargetCalo, ThetaBoostX, TargetX), Sch_P3.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Sch_P4.R_x(TargetCalo, ThetaBoostX, TargetX), Sch_P4.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Sch_P3.Energy(), Sch_P4.Energy(),
                                MagnetShadow);
        FAnalyzer.AnalyzeEventsFull(Radius_max, Radius_min, En_min, 
                                Full_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY), Full_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY),
                                Full_P3.R_x(TargetCalo, ThetaBoostX, TargetX), Full_P3.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Full_P4.R_x(TargetCalo, ThetaBoostX, TargetX), Full_P4.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Full_P3.Energy(), Full_P4.Energy(),
                                MagnetShadow);
        GAnalyzer.AnalyzeEventsFull(Radius_max, Radius_min, En_min, 
                                G1.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY), G2.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY),
                                G1.R_x(TargetCalo, ThetaBoostX, TargetX), G1.R_y(TargetCalo, ThetaBoostY, TargetY),
                                G2.R_x(TargetCalo, ThetaBoostX, TargetX), G2.R_y(TargetCalo, ThetaBoostY, TargetY),
                                G1.Energy(), G2.Energy(),
                                MagnetShadow);
    }

    SNev_accepted = mSInstantLumi[EBeam]*(hSOccupancyAllCuts_pos->GetEntries() / Nev_CalcHep); //POT ELIMINATI X VALUTARE YIELD
    FNev_accepted = mFInstantLumi[EBeam]*(hFOccupancyAllCuts_pos->GetEntries() / Nev_CalcHep); //POT ELIMINATI X VALUTARE YIELD
    GNev_accepted = mGInstantLumi[EBeam]*(hGOccupancyAllCuts_G1->GetEntries() / Nev_CalcHep); //POT ELIMINATI X VALUTARE YIELD
    hSAcceptance->Fill(EBeam, hSOccupancyAllCuts_pos->GetEntries() / Nev_CalcHep);
    hFAcceptance->Fill(EBeam, hFOccupancyAllCuts_pos->GetEntries() / Nev_CalcHep);
    hGAcceptance->Fill(EBeam, hGOccupancyAllCuts_G1->GetEntries() / Nev_CalcHep);
    hSYield->Fill(EBeam, SNev_accepted);
    hFYield->Fill(EBeam, FNev_accepted);
    hGYield->Fill(EBeam, GNev_accepted);


    FileOut->cd();
    if(iEb==0){
        hInitialP3_pos->Write();
        hInitialP3_ele->Write();
        hInEnergy_pos->Write();
        hInEnergy_ele->Write();
    }
    SCh_dir->cd();
    hSFinalP1_pos->Write();
    hSFinalP2_pos->Write();
    hSFinalP3_pos->Write();
    hSFinalP1_ele->Write();
    hSFinalP2_ele->Write();
    hSFinalP3_ele->Write();
    hSFinMomentum_pos->Write();
    hSFinMomentum_ele->Write();
    hSFinEnergy_pos->Write();
    hSFinEnergy_ele->Write();
    hSEn_tot->Write();
    hSTheta_Scattering_COM->Write();
    hSPosTheta_Scattering->Write();
    hSEleTheta_Scattering->Write();
    hSPhiX_pos->Write();
    hSPhiY_pos->Write();
    hSPhiX_ele->Write();
    hSPhiY_ele->Write();
    hSx_pos->Write();
    hSy_pos->Write();
    hSx_ele->Write();
    hSy_ele->Write();
    hSRadius_pos->Write();
    hSRadius_ele->Write();
    hSOccupancy_pos->Write();
    hSOccupancy_ele->Write();
    hSOccupancyGeom_pos->Write();
    hSOccupancyGeom_ele->Write();
    hSOccupancyAllCuts_pos->Write();
    hSOccupancyAllCuts_ele->Write();

    Full_dir->cd();
    hFFinalP1_pos->Write();
    hFFinalP2_pos->Write();
    hFFinalP3_pos->Write();
    hFFinalP1_ele->Write();
    hFFinalP2_ele->Write();
    hFFinalP3_ele->Write();
    hFFinMomentum_pos->Write();
    hFFinMomentum_ele->Write();
    hFFinEnergy_pos->Write();
    hFFinEnergy_ele->Write();
    hFullEn_tot->Write();
    hFTheta_Scattering_COM->Write();
    hFPosTheta_Scattering->Write();
    hSEleTheta_Scattering->Write();
    hFPhiX_pos->Write();
    hFPhiY_pos->Write();
    hFPhiX_ele->Write();
    hFPhiY_ele->Write();
    hFx_pos->Write();
    hFy_pos->Write();
    hFx_ele->Write();
    hFy_ele->Write();
    hFRadius_pos->Write();
    hFRadius_ele->Write();
    hFOccupancy_pos->Write();
    hFOccupancy_ele->Write();
    hFOccupancyGeom_pos->Write();
    hFOccupancyGeom_ele->Write();
    hFOccupancyAllCuts_pos->Write();
    hFOccupancyAllCuts_ele->Write();

    GG_dir->cd();
    hGFinalP1_G1->Write();
    hGFinalP2_G1->Write();
    hGFinalP3_G1->Write();
    hGFinalP1_G2->Write();
    hGFinalP2_G2->Write();
    hGFinalP3_G2->Write();
    hGFinMomentum_G1->Write();
    hGFinMomentum_G2->Write();
    hGFinEnergy_G1->Write();
    hGFinEnergy_G2->Write();
    hGGEn_tot->Write();
    hGTheta_Scattering_COM->Write();
    hGG1Theta_Scattering->Write();
    hGG1Theta_Scattering->Write();
    hGPhiX_G1->Write();
    hGPhiY_G1->Write();
    hGPhiX_G2->Write();
    hGPhiY_G2->Write();
    hGx_G1->Write();
    hGy_G1->Write();
    hGx_G2->Write();
    hGy_G2->Write();    
    hGRadius_G1->Write();
    hGRadius_G2->Write();
    hGOccupancy_G1->Write();
    hGOccupancy_G2->Write();
    hGOccupancyGeom_G1->Write();
    hGOccupancyGeom_G2->Write();
    hGOccupancyAllCuts_G1->Write();
    hGOccupancyAllCuts_G2->Write();

}

TGraph *gSYield = new TGraph(hSYield);
TGraph *gFYield = new TGraph(hFYield);
TGraph *gGYield = new TGraph(hGYield);
// gSYield->Draw();
// gFYield->Draw();
// gGYield->Draw();

FileOut->cd();
hSAcceptance->Write();
hFAcceptance->Write();
hGAcceptance->Write();
// hSYield->Write();
// hFYield->Write();
// hGYield->Write();
gSYield->Write();
gFYield->Write();
gGYield->Write();

FileOut->Close();
}