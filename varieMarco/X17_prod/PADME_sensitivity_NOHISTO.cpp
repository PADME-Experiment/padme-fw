//MANCINI MARCO PADME SENSITIVITY
#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TStyle.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TGraph.h> 
#include <TMultiGraph.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <regex>

#include "EventSelector.h"

using namespace std;

void PADME_sensitivity(double TargetCalo, double E0, double EStep, int NStepEBeam, double ThetaBoostX, double ThetaBoostY, double TargetX, double TargetY, const char* FileName){

double pi2 = TMath::Pi()*2;
static const int Nev_CalcHep = 1000000;
double ElectronMass = 0.000511;
double ElecM = 0.511;
// double TargetCalo = 3700; //in mm 23.06.21 con Mauro e Tommaso

double CaloNBins = 120;
double OccBins = 450;

// DEFINIZIONE DEI TAGLI
double En_min = 0.02; 
double Radius_max = 270.;
double Radius_min = 90.; //092523 trying to compute dynamic cuts on Tommaso work
double MagnetShadow = TargetCalo*TMath::ATan2(110, 1528); //semiapertura del magnete/distanza target-magnete * distanza target-calo
// QUESTE MISURE DA MC SONO SBAGLIATE FORSE - NON CORRISPONDONO AI DATI!
cout<<"Ombra del magnete in [mm]: "<<MagnetShadow<<endl;

// verificare se sono numeri corretti
double PoT = 1e10;
double Na  = 6.022e23;
double Z_C = 6;
double D_C = 3.5;
double A_C = 12;
double TThickness = 0.0097; //in cm
double CmtoPb = 1e-36;
double Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

double PreFactor_X = 3.8e-7; //DATA FROM PROCEEDINGS DARME-NARDI
double g_ve[10]    = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4};
double BEM = 0.;

TString process[4] = {"T", "S", "F", "G"}; //S=SChannel, F=BhabhaFull, G=GammaGamma

map<double, double> mSCrossSection, mFCrossSection, mGCrossSection;
map<double, double> mSInstantLumi, mFInstantLumi, mGInstantLumi;
double SNev_accepted, FNev_accepted, GNev_accepted, SMNev_accepted;

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
TH1F *hSMAcceptance = new TH1F("hSMAcceptance", "hSMAcceptance", NStepEBeam, E0-EStep/2, EFin-EStep/2);
hSAcceptance->SetMarkerStyle(20);
hSAcceptance->SetMarkerColor(kRed);
hSAcceptance->SetMarkerSize(1);
hFAcceptance->SetMarkerStyle(20);
hFAcceptance->SetMarkerColor(kGreen);
hFAcceptance->SetMarkerSize(1);
hGAcceptance->SetMarkerStyle(20);
hGAcceptance->SetMarkerColor(kBlue);
hGAcceptance->SetMarkerSize(1);
hSMAcceptance->SetMarkerStyle(20);
hSMAcceptance->SetMarkerColor(kBlack);
hSMAcceptance->SetMarkerSize(1);

TH1F *hSYield = new TH1F("hSYield", "S-Channel YIELD", NStepEBeam, E0-EStep/2, EFin-EStep/2);
TH1F *hFYield = new TH1F("hFYield", "BhabhaFull YIELD", NStepEBeam, E0-EStep/2, EFin-EStep/2);
TH1F *hGYield = new TH1F("hGYield", "AA YIELD", NStepEBeam, E0-EStep/2, EFin-EStep/2);
TH1F *hSMYield = new TH1F("hSMYield", "SM YIELD", NStepEBeam, E0-EStep/2, EFin-EStep/2);
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
hSMYield->SetMarkerStyle(20);
hSMYield->SetMarkerColor(kBlack);
hSMYield->SetMarkerSize(2);

map <double, vector<double>> mProdX;
map <double, vector<double>> mGVE_squared;
vector <double> vEtest;  
vector <double> vMassX;
map <double, double> mLowerLimitX;
map <double, double> mX_discover;
map <double, TVector3> mRTarg;
map <double, TVector3> mCOGatECal;
map <double, double> mSqrtS, mBeamMomentum;
map <double, double> mBG, mBeta, mGamma;
map <double, TVector3> mBoostMom;
map <double, double> mRadius_min, mEnergy_max, mEnergy_min;

double maxEn = ((E0 + EStep*NStepEBeam)*1.80)/1000;
double minEn = E0*0.20/1000;
for(double i = minEn; i<=maxEn; i+=EStep*0.05){
    vEtest.push_back(i);
    vMassX.push_back(TMath::Sqrt(i*2*ElectronMass));
}

for(int iEb=0; iEb<NStepEBeam; iEb++){
    EBeam = E0 + iEb*EStep;

    mRTarg[EBeam].SetXYZ(TargetX, TargetY, 0);
    mCOGatECal[EBeam].SetXYZ(TMath::Tan(ThetaBoostX)*TargetCalo, TMath::Tan(ThetaBoostY)*TargetCalo, TargetCalo);
    mSqrtS[EBeam] = TMath::Sqrt(2.*ElecM*EBeam);
    mBeamMomentum[EBeam] = TMath::Sqrt(EBeam*EBeam - 2*ElecM*ElecM);
    mBG[EBeam] = mBeamMomentum[EBeam]/mSqrtS[EBeam]; // beta gamma
    mGamma[EBeam] = TMath::Sqrt(mBG[EBeam]*mBG[EBeam]+1.);
    mBeta[EBeam] = mBG[EBeam]/mGamma[EBeam];
    // CAPIRE LLA FUNZIONE DA HEADER DI TOMMASO
    mBoostMom[EBeam].SetXYZ(mCOGatECal[EBeam].X()-mRTarg[EBeam].X(),mCOGatECal[EBeam].Y()-mRTarg[EBeam].Y(),mCOGatECal[EBeam].Z()-mRTarg[EBeam].Z());
    mBoostMom[EBeam] *= (mBeta[EBeam]/mBoostMom[EBeam].Mag());

  // if K = RMax/D is the max tangent in the lab, pi/2 - t < q*/2 < t, where t = atan(gamma RMax/D) must be > pi/4
  // t = pi/4 if gam = 1/K, i.e. at ~ 150 MeV

    double tanQMax = Radius_max/(mCOGatECal[EBeam].Z()-mRTarg[EBeam].Z());
    double tLim = TMath::ATan(mGamma[EBeam]*tanQMax);
    if (tLim < TMath::Pi()*0.25) {
        std::cout << "No solution? " << tLim << " " << mGamma[EBeam] << std::endl;
    }
  
  //  double tanQMin = 1./(fGam*fGam*tanQMax);
  
    mRadius_min[EBeam] = (mCOGatECal[EBeam].Z()-mRTarg[EBeam].Z())*TMath::Tan(0.5*TMath::Pi()-tLim)/mGamma[EBeam] ;// (fCOGAtECal.Z()-fRTarg.Z())*tanQMin; 
    mEnergy_max[EBeam] = mSqrtS[EBeam]*mGamma[EBeam]*0.5*(1.-TMath::Cos(2*tLim)); 
    mEnergy_min[EBeam] = mSqrtS[EBeam]*mGamma[EBeam]*0.5*(1.+TMath::Cos(2*tLim)); 

    cout << "GeneralInfo: run-level info for run from DYNAMIC CUTS at " << EBeam <<" MeV";
    cout << " Pbeam = " << mBeamMomentum[EBeam]<<endl;
    cout << " target = { "<< mRTarg[EBeam].X()<< " , "<< mRTarg[EBeam].Y() << " , " << mRTarg[EBeam].Z() << " }; COG = { " << mCOGatECal[EBeam].X() << " , " << mCOGatECal[EBeam].Y() << " , "<< mCOGatECal[EBeam].Z() << " }" <<endl; 
    cout << " sqrt(s) = " << mSqrtS[EBeam] << " bg = " << mBG[EBeam] << " beta = " << mBeta[EBeam] << endl;
    cout << " energyRange = { " << mEnergy_min[EBeam] << " , " << mEnergy_max[EBeam] << " }; radiusRange = { " << mRadius_min[EBeam] << " , " << Radius_max << " }" << endl;

    cout<<"Total energy of the "<<iEb<<" iteration: "<<EBeam<<" MeV"<<endl;

    ifstream StxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/SChannel/BhabhaSCh_" + to_string(static_cast<int>(EBeam)) +"CUTTED.txt").c_str()); // Apri il file di testo in input
    string Sline;
    ifstream FtxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaFull/BhabhaFull_" + to_string(static_cast<int>(EBeam)) +"CUTTED.txt").c_str()); 
    string Fline;
    ifstream GtxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProd/AAProduction_" + to_string(static_cast<int>(EBeam)) +".txt").c_str()); 
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
    StxtIn.close(); //chiudi il file di testo SChannel
    cout<<"la sezione d'urto letta dal file txt in input Sch: "<<mSCrossSection[EBeam]<<" pb  all'energia di: "<<EBeam<<" MeV"<<endl;

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
    cout<<"la sezione d'urto letta dal file txt in input Full: "<<mFCrossSection[EBeam]<<" pb all'energia di: "<<EBeam<<" MeV"<<endl;

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
    cout<<"la sezione d'urto letta dal file txt in input AA: "<<mGCrossSection[EBeam]<<" pb all'energia di: "<<EBeam<<" MeV"<<endl;

    TDirectory *SCh_dir = FileOut->mkdir(("Sch_dir" + to_string(EBeam)).c_str());
    TDirectory *Full_dir = FileOut->mkdir(("Full_dir"+ to_string(EBeam)).c_str());
    TDirectory *GG_dir = FileOut->mkdir(("GG_dir"+ to_string(EBeam)).c_str());
    TFile *BhabhaSChIn = TFile::Open(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/SChannel/BhabhaSCh_" + to_string(static_cast<int>(EBeam)) +"CUTTED.root").c_str()); 
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

    
    TFile *BhabhaFullIn = TFile::Open(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaFull/BhabhaFull_" + to_string(static_cast<int>(EBeam)) +"CUTTED.root").c_str()); 
    // TFile *BhabhaFullIn  = new TFile("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaFull/BhabhaFull.root");
    TTree *tFull  = (TTree*)BhabhaFullIn->Get(("tBhabhaFull_" + to_string(static_cast<int>(EBeam))).c_str()); 
    double FFinalP1_ele, FFinalP2_ele, FFinalP3_ele, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos; 
    tFull->SetBranchAddress("FFinalP1_electron", &FFinalP1_ele);
    tFull->SetBranchAddress("FFinalP2_electron", &FFinalP2_ele);
    tFull->SetBranchAddress("FFinalP3_electron", &FFinalP3_ele);
    tFull->SetBranchAddress("FFinalP1_positron", &FFinalP1_pos);
    tFull->SetBranchAddress("FFinalP2_positron", &FFinalP2_pos);
    tFull->SetBranchAddress("FFinalP3_positron", &FFinalP3_pos);

    TFile *AAProductionIn = TFile::Open(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProd/AAProduction_" + to_string(static_cast<int>(EBeam)) +".root").c_str()); 
    //TFile *AAProductionIn  = new TFile("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProd/AAprod.root");
    TTree *tGG  = (TTree*)AAProductionIn->Get(("tAAProduction_" + to_string(static_cast<int>(EBeam))).c_str()); 
    double GFinalP1_G1, GFinalP2_G1, GFinalP3_G1, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2; 
    tGG->SetBranchAddress("GFinalP1_G1", &GFinalP1_G1);
    tGG->SetBranchAddress("GFinalP2_G1", &GFinalP2_G1);
    tGG->SetBranchAddress("GFinalP3_G1", &GFinalP3_G1);
    tGG->SetBranchAddress("GFinalP1_G2", &GFinalP1_G2);
    tGG->SetBranchAddress("GFinalP2_G2", &GFinalP2_G2);
    tGG->SetBranchAddress("GFinalP3_G2", &GFinalP3_G2);

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

    const int numPoints = 200;
    double* xmin = new double[numPoints];
    double* ymin = new double[numPoints];
    double* xmax = new double[numPoints];
    double* ymax = new double[numPoints];
    double* xminboost = new double[numPoints];
    double* yminboost = new double[numPoints];
    double* xmaxboost = new double[numPoints];
    double* ymaxboost = new double[numPoints];
    double* xECal = new double[numPoints];
    double* yECal = new double[numPoints];

    // Generate the points of the circle
    for (int i = 0; i < numPoints; ++i) {
        double angle = 2 * TMath::Pi() * i / numPoints;
        xmin[i] = mRadius_min[EBeam] * TMath::Cos(angle);
        ymin[i] = mRadius_min[EBeam] * TMath::Sin(angle);
        xmax[i] = Radius_max * TMath::Cos(angle);
        ymax[i] = Radius_max * TMath::Sin(angle);
        xminboost[i] = mRadius_min[EBeam] * TMath::Cos(angle) + TargetX + TMath::Tan(ThetaBoostX)*TargetCalo;
        yminboost[i] = mRadius_min[EBeam] * TMath::Sin(angle) + TargetY + TMath::Tan(ThetaBoostY)*TargetCalo;
        xmaxboost[i] = Radius_max * TMath::Cos(angle) + TargetX + TMath::Tan(ThetaBoostX)*TargetCalo;
        ymaxboost[i] = Radius_max * TMath::Sin(angle) + TargetY + TMath::Tan(ThetaBoostY)*TargetCalo;
        xECal[i] = 300. * TMath::Cos(angle);
        yECal[i] = 300. * TMath::Sin(angle);
    }
    TGraph* graphmin = new TGraph(numPoints, xmin, ymin);
    graphmin->SetTitle(Form("Circle with mRadius_min %.2f", mRadius_min[EBeam]));
    graphmin->SetLineStyle(1);
    graphmin->SetLineColor(kBlack);
    TGraph* graphmax = new TGraph(numPoints, xmax, ymax);
    graphmax->SetTitle(Form("Circle with Radius_max %.2f", Radius_max));
    graphmax->SetLineStyle(1);
    graphmax->SetLineColor(kBlack);
    TGraph* graphminboost = new TGraph(numPoints, xminboost, yminboost);
    graphminboost->SetTitle(Form("Circle with mRadius_min %.2f", mRadius_min[EBeam]));
    graphminboost->SetLineStyle(1);
    graphminboost->SetLineColor(kBlack);
    TGraph* graphmaxboost = new TGraph(numPoints, xmaxboost, ymaxboost);
    graphmaxboost->SetTitle(Form("Circle with Radius_max %.2f", Radius_max));
    graphmaxboost->SetLineStyle(1);
    graphmaxboost->SetLineColor(kBlack);
    TGraph* graphECal = new TGraph(numPoints, xECal, yECal);
    graphECal->SetTitle(Form("Circle with Radius_ECal %.2f", 300.));
    graphECal->SetLineStyle(1);
    graphECal->SetLineColor(kBlack);
    TMultiGraph *MGDynCut = new TMultiGraph(Form("MGDynCut_%.2f",EBeam),Form("MGDynCut_%.2f",EBeam));
    MGDynCut->Add(graphmin);
    MGDynCut->Add(graphmax);
    MGDynCut->Add(graphminboost);
    MGDynCut->Add(graphmaxboost);
    MGDynCut->Add(graphECal);

    double NEntries = tSch->GetEntries();
    for(int it = 0; it<=NEntries; it++){
        tSch->GetEntry(it);
        tFull->GetEntry(it);
        tGG->GetEntry(it);
            
        Particle P1(ElectronMass, 0, 0, P3pos_ini);
        Particle P2(ElectronMass, 0, 0, P3ele_ini);
        // cout<<"prova: "<<hInitialP3_pos->GetBinContent(it)<<endl;
        Particle Sch_P3 (ElectronMass, SFinalP1_pos, SFinalP2_pos, SFinalP3_pos);
        Particle Sch_P4 (ElectronMass, SFinalP1_ele, SFinalP2_ele, SFinalP3_ele);
        Particle Full_P3 (ElectronMass, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos);
        Particle Full_P4 (ElectronMass, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos);
        Particle G1 (0, GFinalP1_G1, GFinalP2_G1, GFinalP3_G1);
        Particle G2 (0, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2);
        
        EventAnalyzer SAnalyzer(hSOccupancy_pos, hSOccupancy_ele,
                                hSOccupancyGeom_pos, hSOccupancyGeom_ele,
                                hSOccupancyAllCuts_pos, hSOccupancyAllCuts_ele);
        EventAnalyzer FAnalyzer(hFOccupancy_pos, hFOccupancy_ele,
                                hFOccupancyGeom_pos, hFOccupancyGeom_ele,
                                hFOccupancyAllCuts_pos, hFOccupancyAllCuts_ele);
        EventAnalyzer GAnalyzer(hGOccupancy_G1, hGOccupancy_G2,
                                hGOccupancyGeom_G1, hGOccupancyGeom_G2,
                                hGOccupancyAllCuts_G1, hGOccupancyAllCuts_G2);

        SAnalyzer.AnalyzeEventsFull(Radius_max, Radius_min, En_min, EBeam,
                                Sch_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY), Sch_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY),
                                Sch_P3.R_x(TargetCalo, ThetaBoostX, TargetX), Sch_P3.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Sch_P4.R_x(TargetCalo, ThetaBoostX, TargetX), Sch_P4.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Sch_P3.Energy(), Sch_P4.Energy(),
                                MagnetShadow);
        FAnalyzer.AnalyzeEventsFull(Radius_max, Radius_min, En_min, EBeam,
                                Full_P3.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY), Full_P4.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY),
                                Full_P3.R_x(TargetCalo, ThetaBoostX, TargetX), Full_P3.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Full_P4.R_x(TargetCalo, ThetaBoostX, TargetX), Full_P4.R_y(TargetCalo, ThetaBoostY, TargetY),
                                Full_P3.Energy(), Full_P4.Energy(),
                                MagnetShadow);
        GAnalyzer.AnalyzeEventsFull(Radius_max, Radius_min, En_min, EBeam,
                                G1.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY), G2.TransverseRadius(TargetCalo, ThetaBoostX, ThetaBoostY, TargetX, TargetY),
                                G1.R_x(TargetCalo, ThetaBoostX, TargetX), G1.R_y(TargetCalo, ThetaBoostY, TargetY),
                                G2.R_x(TargetCalo, ThetaBoostX, TargetX), G2.R_y(TargetCalo, ThetaBoostY, TargetY),
                                G1.Energy(), G2.Energy(),
                                MagnetShadow);
    }

    SNev_accepted = PoT * mSInstantLumi[EBeam]*(hSOccupancyAllCuts_pos->GetEntries() / NEntries); //POT ELIMINATI X VALUTARE YIELD
    FNev_accepted = PoT * mFInstantLumi[EBeam]*(hFOccupancyAllCuts_pos->GetEntries() / NEntries); //POT ELIMINATI X VALUTARE YIELD
    GNev_accepted = PoT * mGInstantLumi[EBeam]*(hGOccupancyAllCuts_G1->GetEntries() / NEntries); //POT ELIMINATI X VALUTARE YIELD
    SMNev_accepted = FNev_accepted + GNev_accepted; //POT ELIMINATI X VALUTARE YIELD
    hSAcceptance->Fill(EBeam, hSOccupancyAllCuts_pos->GetEntries() / NEntries);
    hFAcceptance->Fill(EBeam, hFOccupancyAllCuts_pos->GetEntries() / NEntries);
    hGAcceptance->Fill(EBeam, hGOccupancyAllCuts_G1->GetEntries() / NEntries);
    // hSMAcceptance->Fill(EBeam, ((hGOccupancyAllCuts_G1->GetEntries() / NEntries) + (hFOccupancyAllCuts_pos->GetEntries() / NEntries)));
    hSYield->Fill(EBeam, SNev_accepted);
    hFYield->Fill(EBeam, FNev_accepted);
    hGYield->Fill(EBeam, GNev_accepted);
    hSMYield->Fill(EBeam, SMNev_accepted);

    // X17 PRODUCTION
    mLowerLimitX[EBeam] = 1.3*TMath::Sqrt(SMNev_accepted);
    cout<<"mLowerLimitX[EBeam]= "<<mLowerLimitX[EBeam]<<endl;
    mX_discover[EBeam] = mLowerLimitX[EBeam]*(hSOccupancyAllCuts_pos->GetEntries()/NEntries);
    cout<<"mX_discover[EBeam]= "<<mX_discover[EBeam]<<endl;
    BEM = EBeam * 0.0032;
    cout<<"Beam Energy spread = "<<BEM<<endl;
    for(int i = 0; i<= vEtest.size(); i++){
        mProdX[EBeam].push_back((PoT * hSOccupancyAllCuts_pos->GetEntries()/NEntries) * PreFactor_X *((g_ve[0]*g_ve[0])/(9e-8)) * ((Na * Z_C * D_C)/(A_C * 1e24)) * (1/BEM) * TMath::Gaus(vEtest[i], EBeam, BEM));
        mGVE_squared[EBeam].push_back((mX_discover[EBeam] * BEM *  A_C * 1e24 * 9e-8)/( PoT * PreFactor_X * Na * Z_C * D_C) / (TMath::Gaus(vEtest[i], EBeam, BEM)));
    }

    FileOut->cd();
    MGDynCut->Write();

    SCh_dir->cd();
    hSOccupancy_pos->Write();
    hSOccupancy_ele->Write();
    hSOccupancyGeom_pos->Write();
    hSOccupancyGeom_ele->Write();
    hSOccupancyAllCuts_pos->Write();
    hSOccupancyAllCuts_ele->Write();
    
    Full_dir->cd();
    hFOccupancy_pos->Write();
    hFOccupancy_ele->Write();
    hFOccupancyGeom_pos->Write();
    hFOccupancyGeom_ele->Write();
    hFOccupancyAllCuts_pos->Write();
    hFOccupancyAllCuts_ele->Write();

    GG_dir->cd();
    hGOccupancy_G1->Write();
    hGOccupancy_G2->Write();
    hGOccupancyGeom_G1->Write();
    hGOccupancyGeom_G2->Write();
    hGOccupancyAllCuts_G1->Write();
    hGOccupancyAllCuts_G2->Write();

}

//X17 production e research

TGraph *gSYield = new TGraph(hSYield);
TGraph *gFYield = new TGraph(hFYield);
TGraph *gGYield = new TGraph(hGYield);
TGraph *gSMYield = new TGraph(hSMYield);

FileOut->cd();
hSAcceptance->Write();
hFAcceptance->Write();
hGAcceptance->Write();
gSYield->Write();
gFYield->Write();
gGYield->Write();
gSMYield->Write();

FileOut->Close();
}