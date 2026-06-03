// Background studies and sensitivity - 1 scan point in sqrt(s)
#include <TH1.h>
#include <TH2.h>
#include "TROOT.h"
#include <TStyle.h> 
#include <TString.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TGraph.h> 
#include <TMultiGraph.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TVector3.h>
#include <regex>
#include "TSystem.h"

using namespace std;

template <typename T>
string to_string_with_precision(const T a_value, const int n = 6){ //CLASS FIXING THE DIGITS CASTING FLOAT TO STRING
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return move(out).str();
}

static void EnsureParentDir(const string& filename)
{
    auto pos = filename.find_last_of('/');
    if (pos == string::npos) return; // no path, current dir

    string dir = filename.substr(0, pos);

    // If it doesn't exist, create it recursively
    if (gSystem->AccessPathName(dir.c_str())) {
        int rc = gSystem->mkdir(dir.c_str(), /*recursive=*/true);
        if (rc != 0 || gSystem->AccessPathName(dir.c_str())) {
            cerr << "ERROR: cannot create/access output dir: " << dir << endl;
        }
    }
}

void ToyCalc_AA(double EBeam, int nEvent, array<TVector3,3> runconfig, double AngBeamDirX, double AngBeamDirY, const char* RootInput, const char* outputRoot)
{  

const int e10 = (int)lround(EBeam * 10.0);

// EBeam input in MeV --> Babayaga gives GeV, everything scaled to GeV
double pi2 = TMath::Pi()*2;
double MassEl = 0.000511; //in GeV
TVector3 TargetPosition = runconfig[0]; // target position as input [mm] 

TVector3 ECalPosition = runconfig[1]; // ECal position as input [mm] - MODIFIED FOR CHAMBER DIS-ALIGNEMENT
double TargetCalo = fabs(TargetPosition.Z() - ECalPosition.Z()); //distance along beam axis between target and calorimeter
TVector3 MagnetBore = runconfig[2]; // Magnet bore operture from MC/optimization studies as input [mm]
double ShadowCut_Y = MagnetBore.Y()/fabs(TargetPosition.Z()-MagnetBore.Z()); // in Rad - 102 are the half size of the magnet along vertical axis, 500 is the z position of the magnet center
double ShadowCut_X = MagnetBore.X()/fabs(TargetPosition.Z()-MagnetBore.Z()); // in Rad - 102 are the half size of the magnet along vertical axis, 500 is the z position of the magnet center

//CUTS
double RECal = 304.5; //external radius of ECal 
double SUsize = 21.;
double safetyfactor_rmin = 1.5*SUsize;
double safetyfactor_en = 0.6; 
double safetyfactor_REcal = 60; //mm Elisa's cut
double MeanDTheta = 3.113; //rad - to be set @3.14?
double MeanDPhi = 3.093; //rad - to be set @3.14?
double SigmaDTheta = 0.1105;
double SigmaDPhi = 0.0932;
double SigmaCut = 3;
double HalfHole = 50.; //maybe not needed
double RMax = 270;  // in mm
// double RMin = 90;
// double EMax = 0.300; //deleted because at TOY level seems to be unusefull
// double EMin = 0.05;
double MagnetCut_up    =  ShadowCut_Y*TargetCalo - ECalPosition.Y();
double MagnetCut_down  = -ShadowCut_Y*TargetCalo - ECalPosition.Y();
double MagnetCut_right =  ShadowCut_X*TargetCalo - ECalPosition.X();
double MagnetCut_left  = -ShadowCut_X*TargetCalo - ECalPosition.X();
cout << "Magnet Shadow on ECal Y (up,down) = (" << MagnetCut_up << ", " << MagnetCut_down << ") [mm]" << endl;
cout << "Magnet Shadow on ECal X (left,right) = (" << MagnetCut_left << ", " << MagnetCut_right << ") [mm]" << endl;

// histogram binning in energy and ECal reso - it depends on the beam as input
double CaloNBins = 30; //[mm]
double OccBins = 450; //[mm]
double angreso = SUsize/TargetCalo/sqrt(3);
int    NAngBin0Pi = TMath::Pi()/angreso;

TFile* BhabhaFullIn = TFile::Open(RootInput, "READ");
    if (!BhabhaFullIn || BhabhaFullIn->IsZombie()) {
        cerr << "ERROR: cannot open input file: " << RootInput << "\n";
        if (BhabhaFullIn) { delete BhabhaFullIn; }
        return;
    }
// cout << "BhabhaFullIn pointer: " << BhabhaFullIn << endl;

TTree* tFull = nullptr;
BhabhaFullIn->GetObject(Form("tBhabhaFull_%d", e10), tFull);
if (!tFull) {
    cerr << "ERROR: missing tree tBhabhaFull_" << e10
                << " in file " << RootInput << "\n";
    BhabhaFullIn->Close();
    delete BhabhaFullIn;
    return;
}
// cout << "tFull pointer: " << tFull << endl;

double P3pos_ini, P3ele_ini, FFinalP1_ele, FFinalP2_ele, FFinalP3_ele, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos;
tFull->SetBranchAddress("InitialP3_positron", &P3pos_ini);
tFull->SetBranchAddress("InitialP3_electron", &P3ele_ini); 
tFull->SetBranchAddress("FFinalP1_positron", &FFinalP1_pos); // electron is positron if BABAYAGA EVENTS (and viceversa)
tFull->SetBranchAddress("FFinalP2_positron", &FFinalP2_pos); // electron is electron if CALCHEP EVENTS (and positron is positron obv)
tFull->SetBranchAddress("FFinalP3_positron", &FFinalP3_pos);
tFull->SetBranchAddress("FFinalP1_electron", &FFinalP1_ele);
tFull->SetBranchAddress("FFinalP2_electron", &FFinalP2_ele);
tFull->SetBranchAddress("FFinalP3_electron", &FFinalP3_ele);

//creating output file with Histos
gROOT->cd(); 
// cout << "Output file name: " << outputRoot << endl;
EnsureParentDir(outputRoot);
TFile* FileOut = TFile::Open(outputRoot, "RECREATE");
// cout << "pointer to FileOut: " << FileOut << endl;

if (!FileOut || FileOut->IsZombie()) {
    cerr << "ERROR: cannot open output file for writing: " << outputRoot << "\n";
    if (FileOut) delete FileOut; // don't call Close() on a zombie
    BhabhaFullIn->Close();
    delete BhabhaFullIn;
    return;
}
// FileOut->cd();
// cout << "FileOut pointer: " << FileOut << endl;
FileOut->cd(); // make it current directory

//CUTS TOMMASO - they are defined here once for type of process
TVector3 TargetImpact; // impact point of the beam on target
TVector3 BeamDirDisplacement; // displacement due to the beam direction
double SqrtS, BeamMomentum;
double BG, Beta, Gamma;
TVector3 BoostMom;
double Radius_min, Energy_max, Energy_min;

TargetImpact.SetXYZ(0, 0, TargetPosition.Z()); //impact point of the beam on target 
BeamDirDisplacement.SetXYZ(0 + TargetCalo*tan(AngBeamDirX), 0 + TargetCalo*tan(AngBeamDirY), ECalPosition.Z());
SqrtS = TMath::Sqrt(2.*MassEl*EBeam*1e-3); // in GeV
BeamMomentum = TMath::Sqrt(EBeam*EBeam*1e-6 - 2*MassEl*MassEl); //in GeV
BG = BeamMomentum/SqrtS; // mBeta mGamma
Gamma = TMath::Sqrt(BG*BG + 1.);
Beta = BG/Gamma;
BoostMom.SetXYZ(BeamDirDisplacement.X()-TargetImpact.X(), BeamDirDisplacement.Y()-TargetImpact.Y(), BeamDirDisplacement.Z()-TargetImpact.Z());
BoostMom *= (Beta/BoostMom.Mag());

double tanQMax = RMax/(BeamDirDisplacement.Z()-TargetImpact.Z());
double tLim = TMath::ATan(Gamma*tanQMax);
if (tLim < TMath::Pi()*0.25) {
    cout << "No solution? " << tLim << " " << Gamma << endl;
}

Radius_min = ((BeamDirDisplacement.Z()-TargetImpact.Z())*TMath::Tan(0.5*TMath::Pi()-tLim)/Gamma) - safetyfactor_rmin; //Elisa's safety factor 25 sept  // (fmBeamDirDisplacement.Z()-fRTarg.Z())*tanQMin; 
Energy_max = SqrtS*Gamma*0.5*(1.-TMath::Cos(2*tLim))/safetyfactor_en; //Elisa's safety factor 25 sept
Energy_min = SqrtS*Gamma*0.5*(1.+TMath::Cos(2*tLim))*safetyfactor_en; //Elisa's safety factor 25 sept

cout << "GeneralInfo: run-level info for run from DYNAMIC CUTS at " << EBeam*1e-3 <<" GeV" << " Pbeam = " << BeamMomentum << "GeV" << endl;
cout << " target = { "<< TargetImpact.X()<< " , "<< TargetImpact.Y() << " , " << TargetImpact.Z() << 
        " }; ECAL = { "<< ECalPosition.X()<< " , "<< ECalPosition.Y() << " , " << ECalPosition.Z() << 
        " }; COG = { " << BeamDirDisplacement.X() << " , " << BeamDirDisplacement.Y() << " , "<< BeamDirDisplacement.Z() << " }" <<endl; 
cout << " sqrt(s) = " << SqrtS << " BG = " << BG << " Beta = " << Beta << endl;
cout << " energyRange = { " << Energy_min << " , " << Energy_max << " }; radiusRange = { " << Radius_min << " , " << RMax << " }" << endl;

const int numPoints = 100;
double xmin[numPoints];
double ymin[numPoints];
double xmax[numPoints];
double ymax[numPoints];
double xECal[numPoints];
double yECal[numPoints];
for (int i = 0; i < numPoints; ++i) {
    double angle = 2 * TMath::Pi() * i / numPoints;
    xmin[i] = Radius_min * TMath::Cos(angle) + TargetImpact.X() + BeamDirDisplacement.X();
    ymin[i] = Radius_min * TMath::Sin(angle) + TargetImpact.Y() + BeamDirDisplacement.Y();
    xmax[i] = RMax * TMath::Cos(angle) + TargetImpact.X() + BeamDirDisplacement.X();
    ymax[i] = RMax * TMath::Sin(angle) + TargetImpact.Y() + BeamDirDisplacement.Y();
    xECal[i] = RECal * TMath::Cos(angle);
    yECal[i] = RECal * TMath::Sin(angle);
}
// preparing TGraph with Tommaso cuts to be superimpose to the TH2F with events in ECal
TGraph* graphmin = new TGraph(numPoints, xmin, ymin);
graphmin->SetTitle(Form("Circle with mRadius_min %.2f", Radius_min));
graphmin->SetLineStyle(1);
graphmin->SetLineColor(kBlack);
TGraph* graphmax = new TGraph(numPoints, xmax, ymax);
graphmax->SetTitle(Form("Circle with Radius_max %.2f", RMax));
graphmax->SetLineStyle(1);
graphmax->SetLineColor(kBlack);
TGraph* graphECal = new TGraph(numPoints, xECal, yECal);
graphECal->SetTitle(Form("Circle with Radius_ECal %.2f", RECal));
graphECal->SetLineStyle(1);
graphECal->SetLineColor(kBlack);
TMultiGraph *MGDynCut = new TMultiGraph(Form("MGDynCut_%.2f",EBeam),Form("MGDynCut_%.2f",EBeam));
MGDynCut->Add(graphmin);
MGDynCut->Add(graphmax);
MGDynCut->Add(graphECal);
// cout << "fine TGraph" << endl;
    
//inizializing histos
int EnBins = (int)EBeam*2;
//initial state histos
TH1F* hFInitEnSum = new TH1F(("hFInitEnSum" + to_string_with_precision(EBeam, 2)).c_str(), ("hFInitEnSum" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, 2*EBeam*1e-3);
TH1F* hFP1 = new TH1F(("hFP1" + to_string_with_precision(EBeam, 2)).c_str(), ("hFP1" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, 2*EBeam*1e-3);
TH1F* hFP2 = new TH1F(("hFP2" + to_string_with_precision(EBeam, 2)).c_str(), ("hFP2" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, 2*EBeam*1e-3);

// occupancy histos before and after selection procedure
TH2F* hFOcc_P3_lab = new TH2F(("hFOcc_P3_lab_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOcc_P3_lab_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
TH2F* hFOcc_P4_lab = new TH2F(("hFOcc_P4_lab_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOcc_P4_lab_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
TH2F* hFOccAllCuts_P3_lab = new TH2F(("hFOccAllCuts_P3_lab_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccAllCuts_P3_lab_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hFOccAllCuts_P4_lab = new TH2F(("hFOccAllCuts_P4_lab_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccAllCuts_P4_lab_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hFOccAllCuts_1_lab = new TH2F(("hFOccAllCuts_1_lab_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccAllCuts_1_lab_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hFOccAllCuts_2_lab = new TH2F(("hFOccAllCuts_2_lab_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccAllCuts_2_lab_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);

// energy distribution before and after selection procedure
TH1F* hFEn_P3_lab = new TH1F(("hFEn_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
TH1F* hFEn_P4_lab = new TH1F(("hFEn_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
TH1F* hFEnAllCuts_P3_lab = new TH1F(("hFEnAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
TH1F* hFEnAllCuts_P4_lab = new TH1F(("hFEnAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
// energy distribution COM frame before and after selection procedure
TH1F* hFEn_P3_com = new TH1F(("hFEn_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);
TH1F* hFEn_P4_com = new TH1F(("hFEn_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);
TH1F* hFEnAllCuts_P3_com = new TH1F(("hFEnAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);
TH1F* hFEnAllCuts_P4_com = new TH1F(("hFEnAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);


// energy distribution of the most/less energetic cluster before and after selection procedure
TH1F* hFEn_1_lab = new TH1F(("hFEn_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
TH1F* hFEn_2_lab = new TH1F(("hFEn_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
TH1F* hFEnAllCuts_1_lab = new TH1F(("hFEnAllCuts_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
TH1F* hFEnAllCuts_2_lab = new TH1F(("hFEnAllCuts_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, EBeam*1e-3);
// energy distribution COM frame before and after selection procedure
TH1F* hFEn_1_com = new TH1F(("hFEn_1_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_1_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);
TH1F* hFEn_2_com = new TH1F(("hFEn_2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_2_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);
TH1F* hFEnAllCuts_1_com = new TH1F(("hFEnAllCuts_1_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_1_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);
TH1F* hFEnAllCuts_2_com = new TH1F(("hFEnAllCuts_2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_2_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS);


// En1 vs En2 before and after selection procedure
TH2F* hFEn_1vs2_lab = new TH2F(("hFEn_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS, EnBins, 0, SqrtS);
TH2F* hFEnAllCuts_1vs2_lab = new TH2F(("hFEnAllCuts_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS, EnBins, 0, SqrtS);
// En1 vs En2 COM frame before and after selection procedure
TH2F* hFEn_1vs2_com = new TH2F(("hFEn_2vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEn_2vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS, EnBins, 0, SqrtS);
TH2F* hFEnAllCuts_1vs2_com = new TH2F(("hFEnAllCuts_1vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnAllCuts_1vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, SqrtS, EnBins, 0, SqrtS);


// theta distribution before and after selection procedure
TH1F* hFTheta_P3_lab = new TH1F(("hFTheta_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFTheta_P4_lab = new TH1F(("hFTheta_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_P3_lab = new TH1F(("hFThetaAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_P4_lab = new TH1F(("hFThetaAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
// theta distribution COM frame before and after selection procedure
TH1F* hFTheta_P3_com = new TH1F(("hFTheta_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFTheta_P4_com = new TH1F(("hFTheta_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_P3_com = new TH1F(("hFThetaAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_P4_com = new TH1F(("hFThetaAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());


// theta distribution of the most/less energetic cluster before and after selection procedure
TH1F* hFTheta_1_lab = new TH1F(("hFTheta_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFTheta_2_lab = new TH1F(("hFTheta_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_1_lab = new TH1F(("hFThetaAllCuts_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_1_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_2_lab = new TH1F(("hFThetaAllCuts_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_2_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
// theta distribution COM frame before and after selection procedure
TH1F* hFTheta_1_com = new TH1F(("hFTheta_1_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_1_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFTheta_2_com = new TH1F(("hFTheta_2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_2_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_1_com = new TH1F(("hFThetaAllCuts_1_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_1_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaAllCuts_2_com = new TH1F(("hFThetaAllCuts_2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_2_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());

// theta1 vs theta2 before and after selection procedure
TH2F* hFTheta_1vs2_lab = new TH2F(("hFTheta_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFThetaAllCuts_1vs2_lab = new TH2F(("hFThetaAllCuts_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_1vs2_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
// theta1 vs theta2 COM frame before and after selection procedure
TH2F* hFTheta_1vs2_com = new TH2F(("hFTheta_1vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFTheta_2vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFThetaAllCuts_1vs2_com = new TH2F(("hFThetaAllCuts_1vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaAllCuts_1vs2_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());


// phi distribution lab frame before and after selection procedure
TH1F* hFPhi_P3_lab = new TH1F(("hFPhi_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhi_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFPhi_P4_lab = new TH1F(("hFPhi_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhi_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFPhiAllCuts_P3_lab = new TH1F(("hFPhiAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFPhiAllCuts_P4_lab = new TH1F(("hFPhiAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
// phi distribution COM frame before and after selection procedure
TH1F* hFPhi_P3_com = new TH1F(("hFPhi_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhi_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFPhi_P4_com = new TH1F(("hFPhi_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhi_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFPhiAllCuts_P3_com = new TH1F(("hFPhiAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFPhiAllCuts_P4_com = new TH1F(("hFPhiAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());

// energy sum distribution lab frame before and after selection procedure
TH1F* hFEnSum_lab = new TH1F(("hFEnSum_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnSum_lab" + to_string_with_precision(EBeam, 2)).c_str(), 2*EnBins, 0, 2*EBeam*1e-3);
TH1F* hFEnSumAllCuts_lab = new TH1F(("hFEnSumAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnSumAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), 2*EnBins, 0, 2*EBeam*1e-3);
//energy sum in CoM Frame before and after selection procedure
TH1F* hFEnSum_com = new TH1F(("hFEnSum_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnSum_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*EnBins, 0, 2*SqrtS);
TH1F* hFEnSumAllCuts_com = new TH1F(("hFEnSumAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnSumAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*EnBins, 0, 2*SqrtS);

// Theta sum distribution lab frame before and after selection procedure
TH1F* hFThetaSum_lab = new TH1F(("hFThetaSum_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaSum_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
TH1F* hFThetaSumAllCuts_lab = new TH1F(("hFThetaSumAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaSumAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi());
// Theta sum in CoM Frame before and after selection procedure
TH1F* hFThetaSum_com = new TH1F(("hFThetaSum_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaSum_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi());
TH1F* hFThetaSumAllCuts_com = new TH1F(("hFThetaSumAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFThetaSumAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi());

// Phi sum distribution lab frame before and after selection procedure
TH1F* hFPhiSum_lab = new TH1F(("hFPhiSum_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiSum_lab" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi());
TH1F* hFPhiSumAllCuts_lab = new TH1F(("hFPhiSumAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiSumAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi());
// Phi sum in CoM Frame before and after selection procedure
TH1F* hFPhiSum_com = new TH1F(("hFPhiSum_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiSum_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi());
TH1F* hFPhiSumAllCuts_com = new TH1F(("hFPhiSumAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiSumAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi());

// missing mass before and after selection procedure
TH1F* hFMissingMass = new TH1F(("hFMissingMass" + to_string_with_precision(EBeam, 2)).c_str(), ("hFMissingMass" + to_string_with_precision(EBeam, 2)).c_str(), 2*EnBins, 0, 2*EBeam*1e-3);
TH1F* hFMissingMassAllCuts = new TH1F(("hFMissingMassAllCuts" + to_string_with_precision(EBeam, 2)).c_str(), ("hFMissingMassAllCuts" + to_string_with_precision(EBeam, 2)).c_str(), 2*EnBins, 0, 2*EBeam*1e-3);

//theta vs phi before and after selection procedure
TH2F* hFPhiVSTheta_P3_lab = new TH2F(("hFPhiVSTheta_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSTheta_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFPhiVSTheta_P4_lab = new TH2F(("hFPhiVSTheta_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSTheta_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFPhiVSThetaAllCuts_P3_lab = new TH2F(("hFPhiVSThetaAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSThetaAllCuts_P3_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFPhiVSThetaAllCuts_P4_lab = new TH2F(("hFPhiVSThetaAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSThetaAllCuts_P4_lab" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());

//theta vs phi com frame before and after selection procedure
TH2F* hFPhiVSTheta_P3_com = new TH2F(("hFPhiVSTheta_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSTheta_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFPhiVSTheta_P4_com = new TH2F(("hFPhiVSTheta_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSTheta_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFPhiVSThetaAllCuts_P3_com = new TH2F(("hFPhiVSThetaAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSThetaAllCuts_P3_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());
TH2F* hFPhiVSThetaAllCuts_P4_com = new TH2F(("hFPhiVSThetaAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFPhiVSThetaAllCuts_P4_com" + to_string_with_precision(EBeam, 2)).c_str(), NAngBin0Pi, 0, TMath::Pi(), NAngBin0Pi, 0, TMath::Pi());

// theta1+theta2 vs phi1-phi2 lab frame
TH2F* hFDeltaTheta_DeltaPhi_lab = new TH2F(("hFDeltaTheta_DeltaPhi_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFDeltaTheta_DeltaPhi_lab" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi(), NAngBin0Pi, 0, TMath::Pi()); 
TH2F* hFDeltaTheta_DeltaPhiAllCuts_lab = new TH2F(("hFDeltaTheta_DeltaPhiAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), ("hFDeltaTheta_DeltaPhiAllCuts_lab" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi(), NAngBin0Pi, 0, TMath::Pi()); 
// theta1+theta2 vs phi1-phi2 com frame
TH2F* hFDeltaTheta_DeltaPhi_com = new TH2F(("hFDeltaTheta_DeltaPhi_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFDeltaTheta_DeltaPhi_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi(), NAngBin0Pi, 0, 2*TMath::Pi()); 
TH2F* hFDeltaTheta_DeltaPhiAllCuts_com = new TH2F(("hFDeltaTheta_DeltaPhiAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), ("hFDeltaTheta_DeltaPhiAllCuts_com" + to_string_with_precision(EBeam, 2)).c_str(), 2*NAngBin0Pi, 0, 2*TMath::Pi(), NAngBin0Pi, 0, 2*TMath::Pi()); 

// cut flow histo
const int nCuts = 10;
TH1F *hcutflow = new TH1F("hcutflow", "hcutflow", nCuts, 0.5, nCuts + 0.5);

// cout << " histo creati" << endl;

//TVector3 for each of all 4 particles in the process
TVector3 P1, P2;
TVector3 FP3, FP4;
TVector3 FP3_com, FP4_com;

double thetaP = TMath::ATan(TMath::Sqrt(TMath::Tan(AngBeamDirX)*TMath::Tan(AngBeamDirX) + TMath::Tan(AngBeamDirY)*TMath::Tan(AngBeamDirY)));
double phi;
if(AngBeamDirX == 0 && AngBeamDirY== 0) phi = 0;
else phi = TMath::ACos(TMath::Tan(AngBeamDirX)/TMath::Sqrt(TMath::Tan(AngBeamDirX)*TMath::Tan(AngBeamDirX) + TMath::Tan(AngBeamDirY)*TMath::Tan(AngBeamDirY)));
TVector3 Boost;
Boost.SetXYZ(TMath::Sin(thetaP)*TMath::Cos(phi), TMath::Sin(thetaP)*TMath::Sin(phi), TMath::Cos(thetaP));

// counters cutflow
int CutFlow[nCuts] = {0};
TString CutName[nCuts] = {"NoCut","InECal", "DeltaR", "DeltaTheta", "DeltaPhi", "R1", "R2", "EMin", "EMax", "PhiCut"};

Long64_t nToRun = min<Long64_t>(nEvent, tFull->GetEntries());

for(int iR = 0; iR<nToRun; iR++){
    tFull->GetEntry(iR);
    //defining 2->2 body process from Tree in input P1-P3 positrons, P2-P4 electrons
    // smearign here!
    P1.SetXYZ(0,0,P3pos_ini);
    P2.SetXYZ(0,0,P3ele_ini);
    FP3.SetXYZ(FFinalP1_pos,FFinalP2_pos,FFinalP3_pos);
    FP4.SetXYZ(FFinalP1_ele,FFinalP2_ele,FFinalP3_ele);
    // cout<<P1.Z()<<" "<<P2.Z()<<" "<<FP3.X()<<" "<<FP3.Y()<<" "<<FP3.Z()<<" "<<FP4.X()<<" "<<FP4.Y()<<" "<<FP4.Z()<<" "<<endl;

    // initial state
    double P1_Ptot = P1.Mag();
    double P2_Ptot = P2.Mag();
    double P1_En = TMath::Sqrt(P1.Mag2()); 
    double P2_En = 0; // MassEl*1e-3;
    double E12_tot = (P1_En + P2_En);
    //cout<< "Energies initial state: "<<E12_tot<<endl;

    // final state
    // total momentum
    double FP3_Ptot_lab = FP3.Mag();
    double FP4_Ptot_lab = FP4.Mag();
    // energy 
    double FP3_En_lab = sqrt(FP3.Mag2()); // MassEl*MassEl*1e-6 + 
    double FP4_En_lab = sqrt(FP4.Mag2()); // MassEl*MassEl*1e-6 + 
    double FE34_tot_lab = FP3_En_lab + FP4_En_lab;
    // cout<< "Energies by CalcHEP: " << FE34_tot_lab <<endl;

    // scattering angle
    double FP3_ScattAngle_lab = FP3.Theta();// scattering angle
    double FP4_ScattAngle_lab = FP4.Theta();// scattering angle
    // polar angle
    double FP3_PolarAngle_lab = FP3.Phi();
    double FP4_PolarAngle_lab = FP4.Phi();
    //radius at target before boosting
    double FP3_R_lab = tan(FP3_ScattAngle_lab)*TargetCalo;
    double FP4_R_lab = tan(FP4_ScattAngle_lab)*TargetCalo;
   
    // moving to CoM frame - transverse momentum remain the same component per component
    // coord in com frame
    FP3_com.SetXYZ(FP3.X(), FP3.Y(), (Gamma*FP3.Z() - BG*FP3_En_lab));
    FP4_com.SetXYZ(FP4.X(), FP4.Y(), (Gamma*FP4.Z() - BG*FP4_En_lab));
    // cout<<" BhabhaFull verificato CoM? "<<FP3_com.Mag()<<" = "<<FP4_com.Mag()<<endl;

    // final state - comframe
    // total momentum - comframe
    double FP3_Ptot_lab_com = FP3_com.Mag();
    double FP4_Ptot_com = FP4_com.Mag();
    // energy - com frame (derived with boost)
    double FP3_En_com = Gamma*FP3_En_lab - BG*FP3.Z();
    double FP4_En_com = Gamma*FP4_En_lab - BG*FP4.Z();
    double FE34_tot_com = FP3_En_com + FP4_En_com;
    // cout<< "Energies by CalcHEP: "<<SE34_tot<<" "<< FE34_tot <<" "<< GE34_tot<<endl;

    // // scattering angle
    double FP3_ScattAngle_com = FP3_com.Theta();
    double FP4_ScattAngle_com = FP4_com.Theta();
    // scattering angle
    // double FP3_ScattAngle_com = TMath::ATan(TMath::Sqrt(FP3_com.X()*FP3_com.X() + FP3_com.Y()*FP3_com.Y())/FP3_com.Z());
    // double FP4_ScattAngle_com = TMath::ATan(TMath::Sqrt(FP4_com.X()*FP4_com.X() + FP4_com.Y()*FP4_com.Y())/FP4_com.Z());
    // polar angle
    double FP3_PolarAngle_com = FP3_com.Phi();
    double FP4_PolarAngle_com = FP4_com.Phi();
    //radius at target before boosting
    double FP3_R_com = tan(FP3_ScattAngle_com)*TargetCalo;
    double FP4_R_com = tan(FP4_ScattAngle_com)*TargetCalo;    

    //boost in lab frame with beam displacement dependent on thetaX thetaY in input. They are took as a variable derived previously
    FP3.SetX((-BG*Boost.X()*FP3_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*FP3_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*FP3_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*FP3_com.Z());
    FP3.SetY((-BG*Boost.Y()*FP3_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*FP3_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*FP3_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*FP3_com.Z());
    FP3.SetZ((-BG*Boost.Z()*FP3_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*FP3_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*FP3_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*FP3_com.Z());
    FP4.SetX((-BG*Boost.X()*FP4_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*FP4_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*FP4_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*FP4_com.Z());
    FP4.SetY((-BG*Boost.Y()*FP4_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*FP4_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*FP4_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*FP4_com.Z());
    FP4.SetZ((-BG*Boost.Z()*FP4_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*FP4_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*FP4_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*FP4_com.Z());
    //energy
    double FP3_En_lab_boosted = Gamma*FP3_En_com - BG*Boost.X()*FP3_com.X() - BG*Boost.Y()*FP3_com.Y() - BG*Boost.Z()*FP3_com.Z();
    double FP4_En_lab_boosted = Gamma*FP4_En_com - BG*Boost.X()*FP4_com.X() - BG*Boost.Y()*FP4_com.Y() - BG*Boost.Z()*FP4_com.Z();
    double FE34_tot_lab_boosted = FP3_En_lab_boosted + FP4_En_lab_boosted;
    // cout<< "Energies after boosting forward: " << FE34_tot_lab << endl;
    //double MissM = sqrt(pow((MassEl*MassEl + P1_En*P1_En - FP3_En_lab*FP3_En_lab - FP4_En_lab*FP4_En_lab),2) - pow((P1.Mag2() + P2.Mag2() - FP3.Mag2() - FP4.Mag2()),2));
    double MissM = sqrt(pow((P1_En*P1_En - FP3_En_lab_boosted*FP3_En_lab_boosted - FP4_En_lab_boosted*FP4_En_lab_boosted),2) - pow((P1.Mag2() + P2.Mag2() - FP3.Mag2() - FP4.Mag2()),2));
    // cout<< "Energies after boosting forward: "<<SE34_tot<<" "<< FE34_tot <<" "<< GE34_tot<<endl;

    // impact point @ECal
    double FP3_x_lab = (FP3.X()/FP3.Z())*TargetCalo;
    double FP3_y_lab = (FP3.Y()/FP3.Z())*TargetCalo;
    double FP4_x_lab = (FP4.X()/FP4.Z())*TargetCalo;
    double FP4_y_lab = (FP4.Y()/FP4.Z())*TargetCalo;

    // radius on ECAL
    double FP3_r_ecal_lab = sqrt(FP3_x_lab*FP3_x_lab + FP3_y_lab*FP3_y_lab);
    double FP4_r_ecal_lab = sqrt(FP4_x_lab*FP4_x_lab + FP4_y_lab*FP4_y_lab);

    double DeltaR_ecal_lab = sqrt((FP3_x_lab-FP4_x_lab)*(FP3_x_lab-FP4_x_lab) + (FP3_y_lab-FP4_y_lab)*(FP3_y_lab-FP4_y_lab));

    // // scattering angle after boosting
    double FP3_FinalTheta_lab = FP3.Theta();
    double FP4_FinalTheta_lab = FP4.Theta();
    // scattering angle after boosting
    // double FP3_FinalTheta_lab = TMath::ATan(TMath::Sqrt(FP3.X()*FP3.X() + FP3.Y()*FP3.Y())/FP3.Z());
    // double FP4_FinalTheta_lab = TMath::ATan(TMath::Sqrt(FP4.X()*FP4.X() + FP4.Y()*FP4.Y())/FP4.Z());
    // polar anglen after boosting
    double FP3_FinalPhi_lab = FP3.Phi();
    double FP4_FinalPhi_lab = FP4.Phi();

    double delta_phi_com = fabs(FP3_PolarAngle_com - FP4_PolarAngle_com);
    double theta_sum_com = FP3_ScattAngle_com + FP4_ScattAngle_com;

    //filling Histo 
    // NOCUT
    // lab frame
    hFP1->Fill(P1_En);
    hFP2->Fill(P2_En);
    hFInitEnSum->Fill(E12_tot);
    hFOcc_P3_lab->Fill(FP3_x_lab, FP3_y_lab);
    hFOcc_P4_lab->Fill(FP4_x_lab, FP4_y_lab);
    hFEn_P3_lab->Fill(FP3_En_lab);
    hFEn_P4_lab->Fill(FP4_En_lab);
    if(FP3_En_lab>FP4_En_lab){
        hFEn_1_lab->Fill(FP3_En_lab);
        hFEn_2_lab->Fill(FP4_En_lab);
        hFEn_1vs2_lab->Fill(FP3_En_lab, FP4_En_lab);
        hFTheta_1_lab->Fill(FP3_ScattAngle_lab);
        hFTheta_2_lab->Fill(FP4_ScattAngle_lab);
        hFTheta_1vs2_lab->Fill(FP3_ScattAngle_lab, FP4_ScattAngle_lab);
    }
    else{
        hFEn_1_lab->Fill(FP4_En_lab);
        hFEn_2_lab->Fill(FP3_En_lab);
        hFEn_1vs2_lab->Fill(FP4_En_lab, FP3_En_lab);
        hFTheta_1_lab->Fill(FP4_ScattAngle_lab);
        hFTheta_2_lab->Fill(FP3_ScattAngle_lab);
        hFTheta_1vs2_lab->Fill(FP4_ScattAngle_lab, FP3_ScattAngle_lab);
    }
    hFTheta_P3_lab->Fill(FP3_ScattAngle_lab);
    hFTheta_P4_lab->Fill(FP4_ScattAngle_lab);
    hFPhi_P3_lab->Fill(fabs(FP3_PolarAngle_lab));
    hFPhi_P4_lab->Fill(fabs(FP4_PolarAngle_lab));
    hFEnSum_lab->Fill(FE34_tot_lab);
    hFThetaSum_lab->Fill(FP3_ScattAngle_lab + FP4_ScattAngle_lab);
    hFPhiSum_lab->Fill(fabs(FP3_PolarAngle_lab + FP4_PolarAngle_lab));
    hFPhiVSTheta_P3_lab->Fill(fabs(FP3_PolarAngle_lab), FP3_ScattAngle_lab);
    hFPhiVSTheta_P4_lab->Fill(fabs(FP4_PolarAngle_lab), FP4_ScattAngle_lab);
    hFDeltaTheta_DeltaPhi_lab->Fill(fabs(FP3_PolarAngle_lab - FP4_PolarAngle_lab), FP3_ScattAngle_lab + FP4_ScattAngle_lab);
    hFMissingMass->Fill(MissM);

    //com frame
    hFEn_P3_com->Fill(FP3_En_com);
    hFEn_P4_com->Fill(FP4_En_com);
    if(FP3_En_lab>FP4_En_lab){
        hFEn_1_com->Fill(FP3_En_com);
        hFEn_2_com->Fill(FP4_En_com);
        hFEn_1vs2_com->Fill(FP3_En_com, FP4_En_com);
        hFTheta_1_com->Fill(FP3_ScattAngle_com);
        hFTheta_2_com->Fill(FP4_ScattAngle_com);
        hFTheta_1vs2_com->Fill(FP3_ScattAngle_com, FP4_ScattAngle_com);
    }
    else{
        hFEn_1_com->Fill(FP4_En_com);
        hFEn_2_com->Fill(FP3_En_com);
        hFEn_1vs2_com->Fill(FP4_En_com, FP3_En_com);
        hFTheta_1_com->Fill(FP4_ScattAngle_com);
        hFTheta_2_com->Fill(FP3_ScattAngle_com);
        hFTheta_1vs2_com->Fill(FP4_ScattAngle_com, FP3_ScattAngle_com);
    }
    hFTheta_P3_com->Fill(FP3_ScattAngle_com);
    hFTheta_P4_com->Fill(FP4_ScattAngle_com);
    hFPhi_P3_com->Fill(fabs(FP3_PolarAngle_com));
    hFPhi_P4_com->Fill(FP4_PolarAngle_com);
    hFEnSum_com->Fill(FE34_tot_com);
    hFThetaSum_com->Fill(theta_sum_com);
    hFPhiSum_com->Fill(fabs(FP3_PolarAngle_com + FP4_PolarAngle_com));
    hFPhiVSTheta_P3_com->Fill(fabs(FP3_PolarAngle_com), FP3_ScattAngle_com);
    hFPhiVSTheta_P4_com->Fill(fabs(FP4_PolarAngle_com), FP4_ScattAngle_com);
    hFDeltaTheta_DeltaPhi_com->Fill(fabs(delta_phi_com), theta_sum_com);
    //no cut
    CutFlow[0]++;
    // CUTS SEQUENCE
    // FP3-FP4 not in ECal hole
    if(!(fabs(FP3_x_lab)>HalfHole || fabs(FP3_y_lab)>HalfHole)) continue;
    if(!(fabs(FP4_x_lab)>HalfHole || fabs(FP4_y_lab)>HalfHole)) continue;
    // FP4 not in ECal hole
    // if(!(fabs(FP3_x_lab)<HalfHole && fabs(FP3_y_lab)<HalfHole)) continue;
    // if(!(fabs(FP4_x_lab)<HalfHole && fabs(FP4_y_lab)<HalfHole)) continue;
    CutFlow[1]++;
    // magnet shadow upper/lower part P3
    if(!(FP3_y_lab>MagnetCut_down && FP3_y_lab<MagnetCut_up)) continue;
    // magnet shadow upper/lower part P4
    if(!(FP4_y_lab>MagnetCut_down && FP4_y_lab<MagnetCut_up)) continue;
    // magnet shadow left/right part P3
    if(!(FP3_x_lab>MagnetCut_left && FP3_x_lab<MagnetCut_right)) continue;
    // magnet shadow left/right part P4
    if(!(FP4_x_lab>MagnetCut_left && FP4_x_lab<MagnetCut_right)) continue;
    // performing delta R cut
    if(!(DeltaR_ecal_lab>safetyfactor_REcal)) continue;
    CutFlow[2]++;
    //performing delta theta cut
    if(!(theta_sum_com>MeanDTheta-SigmaCut*SigmaDTheta && theta_sum_com<MeanDTheta+SigmaCut*SigmaDTheta)) continue;
    CutFlow[3]++;
    //performing delta phi cut
    if(!(delta_phi_com>MeanDPhi-SigmaCut*SigmaDPhi && delta_phi_com<MeanDPhi+SigmaCut*SigmaDPhi)) continue;
    CutFlow[4]++;
    // performing Rmin-Rmax cut for both particles
    if(FP3_En_lab>FP4_En_lab){
        // performing R2 cut    
        if(!(FP3_R_lab>Radius_min && FP3_R_lab<RMax)) continue;
        CutFlow[5]++;
        // performing R2 cut
        if(!(FP4_R_lab>Radius_min && FP4_R_lab<RECal)) continue;
        CutFlow[6]++;
    } else{
        // performing R2 cut    
        if(!(FP4_R_lab>Radius_min && FP4_R_lab<RMax)) continue;
        CutFlow[5]++;
        // performing R2 cut
        if(!(FP3_R_lab>Radius_min && FP3_R_lab<RECal)) continue;
        CutFlow[6]++;    
    }
    // performing Emin cut
    if(!(FP3_En_lab>Energy_min && FP4_En_lab>Energy_min)) continue;
    CutFlow[7]++;   
    // performing Emax cut
    if(!(FP3_En_lab<Energy_max && FP4_En_lab<Energy_max)) continue;
    CutFlow[8]++;
    // performing phi cut Elisa    
    // if((fabs((fabs(FP3_PolarAngle_lab)-TMath::Pi()/2))<TMath::Pi()/6) && (fabs((fabs(FP4_PolarAngle_lab)-TMath::Pi()/2))<TMath::Pi()/6)) continue;
    CutFlow[9]++;
   
    //ALLCUT
    //lab frame
    hFOccAllCuts_P3_lab->Fill(FP3_x_lab, FP3_y_lab);
    hFOccAllCuts_P4_lab->Fill(FP4_x_lab, FP4_y_lab);
    hFEnAllCuts_P3_lab->Fill(FP3_En_lab);
    hFEnAllCuts_P4_lab->Fill(FP4_En_lab);
    if(FP3_En_lab>FP4_En_lab){
        hFOccAllCuts_1_lab->Fill(FP3_x_lab, FP3_y_lab);
        hFOccAllCuts_2_lab->Fill(FP4_x_lab, FP4_y_lab);
        hFEnAllCuts_1_lab->Fill(FP3_En_lab);
        hFEnAllCuts_2_lab->Fill(FP4_En_lab);
        hFEnAllCuts_1vs2_lab->Fill(FP3_En_lab, FP4_En_lab);
        hFThetaAllCuts_1_lab->Fill(FP3_ScattAngle_lab);
        hFThetaAllCuts_2_lab->Fill(FP4_ScattAngle_lab);
        hFThetaAllCuts_1vs2_lab->Fill(FP3_ScattAngle_lab, FP4_ScattAngle_lab);
    }
    else{
        hFOccAllCuts_1_lab->Fill(FP4_x_lab, FP4_y_lab);
        hFOccAllCuts_2_lab->Fill(FP3_x_lab, FP3_y_lab);
        hFEnAllCuts_1_lab->Fill(FP4_En_lab);
        hFEnAllCuts_2_lab->Fill(FP3_En_lab);
        hFEnAllCuts_1vs2_lab->Fill(FP4_En_lab, FP3_En_lab);
        hFThetaAllCuts_1_lab->Fill(FP4_ScattAngle_lab);
        hFThetaAllCuts_2_lab->Fill(FP3_ScattAngle_lab);
        hFThetaAllCuts_1vs2_lab->Fill(FP4_ScattAngle_lab, FP3_ScattAngle_lab);
    }
    hFThetaAllCuts_P3_lab->Fill(FP3_ScattAngle_lab);
    hFThetaAllCuts_P4_lab->Fill(FP4_ScattAngle_lab);
    hFPhiAllCuts_P3_lab->Fill(fabs(FP3_PolarAngle_lab));
    hFPhiAllCuts_P4_lab->Fill(fabs(FP4_PolarAngle_lab));
    hFEnSumAllCuts_lab->Fill(FE34_tot_lab);  
    hFThetaSumAllCuts_lab->Fill(FP3_ScattAngle_lab + FP4_ScattAngle_lab);  
    hFPhiSumAllCuts_lab->Fill(fabs(FP3_PolarAngle_lab + FP4_PolarAngle_lab));  
    hFPhiVSThetaAllCuts_P3_lab->Fill(fabs(FP3_PolarAngle_lab), FP3_ScattAngle_lab);
    hFPhiVSThetaAllCuts_P4_lab->Fill(fabs(FP4_PolarAngle_lab), FP4_ScattAngle_lab);
    hFDeltaTheta_DeltaPhiAllCuts_lab->Fill(FP3_PolarAngle_lab - FP4_PolarAngle_lab, FP3_ScattAngle_lab + FP4_ScattAngle_lab);
    hFMissingMassAllCuts->Fill(MissM);
                    
    //com frame
    hFEnAllCuts_P3_com->Fill(FP3_En_com);
    hFEnAllCuts_P4_com->Fill(FP4_En_com);
    if(FP3_En_lab>FP4_En_lab){
        hFEnAllCuts_1_com->Fill(FP3_En_com);
        hFEnAllCuts_2_com->Fill(FP4_En_com);
        hFEnAllCuts_1vs2_com->Fill(FP3_En_com, FP4_En_com);
        hFThetaAllCuts_1_com->Fill(FP3_ScattAngle_com);
        hFThetaAllCuts_2_com->Fill(FP4_ScattAngle_com);
        hFThetaAllCuts_1vs2_com->Fill(FP3_ScattAngle_com, FP4_ScattAngle_com);
    }
    else{
        hFEnAllCuts_1_com->Fill(FP4_En_com);
        hFEnAllCuts_2_com->Fill(FP3_En_com);
        hFEnAllCuts_1vs2_com->Fill(FP4_En_com, FP3_En_com);
        hFThetaAllCuts_1_com->Fill(FP4_ScattAngle_com);
        hFThetaAllCuts_2_com->Fill(FP3_ScattAngle_com);
        hFThetaAllCuts_1vs2_com->Fill(FP4_ScattAngle_com, FP3_ScattAngle_com);
    }
    hFThetaAllCuts_P3_com->Fill(FP3_ScattAngle_com);
    hFThetaAllCuts_P4_com->Fill(FP4_ScattAngle_com);
    hFPhiAllCuts_P3_com->Fill(fabs(FP3_PolarAngle_com));
    hFPhiAllCuts_P4_com->Fill(fabs(FP4_PolarAngle_com));
    hFEnSumAllCuts_com->Fill(FE34_tot_com);  
    hFThetaSumAllCuts_com->Fill(theta_sum_com);  
    hFPhiSumAllCuts_com->Fill(fabs(FP3_PolarAngle_com + FP4_PolarAngle_com));  
    hFPhiVSThetaAllCuts_P3_com->Fill(fabs(FP3_PolarAngle_com), FP3_ScattAngle_com);
    hFPhiVSThetaAllCuts_P4_com->Fill(fabs(FP4_PolarAngle_com), FP4_ScattAngle_com);
    hFDeltaTheta_DeltaPhiAllCuts_com->Fill(fabs(delta_phi_com), theta_sum_com);  
}   

cout << "N events after full selection: " << hFEnAllCuts_P3_com->Integral() << endl;

//normalizing histos
// hFP1->Scale(1./hFP1->Integral());
// hFP2->Scale(1./hFP2->Integral());
// hFInitEnSum->Scale(1./hFInitEnSum->Integral());
// hFOcc_P3_lab->Scale(1./hFOcc_P3_lab->Integral());
// hFOcc_P4_lab->Scale(1./hFOcc_P4_lab->Integral());
// hFEn_P3_lab->Scale(1./hFEn_P3_lab->Integral());
// hFEn_P4_lab->Scale(1./hFEn_P4_lab->Integral());
// hFEn_1_lab->Scale(1./hFEn_1_lab->Integral());
// hFEn_2_lab->Scale(1./hFEn_2_lab->Integral());
// hFEn_1vs2_lab->Scale(1./hFEn_1vs2_lab->Integral());
// hFTheta_1_lab->Scale(1./hFTheta_1_lab->Integral());
// hFTheta_2_lab->Scale(1./hFTheta_2_lab->Integral());
// hFTheta_1vs2_lab->Scale(1./hFTheta_1vs2_lab->Integral());
// hFTheta_P3_lab->Scale(1./hFTheta_P3_lab->Integral());
// hFTheta_P4_lab->Scale(1./hFTheta_P4_lab->Integral());
// hFPhi_P3_lab->Scale(1./hFPhi_P3_lab->Integral());
// hFPhi_P4_lab->Scale(1./hFPhi_P4_lab->Integral());
// hFEnSum_lab->Scale(1./hFEnSum_lab->Integral());
// hFThetaSum_lab->Scale(1./hFThetaSum_lab->Integral());
// hFPhiSum_lab->Scale(1./hFPhiSum_lab->Integral());
// hFPhiVSTheta_P3_lab->Scale(1./hFPhiVSTheta_P3_lab->Integral());
// hFPhiVSTheta_P4_lab->Scale(1./hFPhiVSTheta_P4_lab->Integral());
// hFDeltaTheta_DeltaPhi_lab->Scale(1./hFDeltaTheta_DeltaPhi_lab->Integral());
// hFMissingMass->Scale(1./hFMissingMass->Integral());
// hFEn_P3_com->Scale(1./hFEn_P3_com->Integral());
// hFEn_P4_com->Scale(1./hFEn_P4_com->Integral());
// hFEn_1_com->Scale(1./hFEn_1_com->Integral());
// hFEn_2_com->Scale(1./hFEn_2_com->Integral());
// hFEn_1vs2_com->Scale(1./hFEn_1vs2_com->Integral());
// hFTheta_1_com->Scale(1./hFTheta_1_com->Integral());
// hFTheta_2_com->Scale(1./hFTheta_2_com->Integral());
// hFTheta_1vs2_com->Scale(1./hFTheta_1vs2_com->Integral());
// hFTheta_P3_com->Scale(1./hFTheta_P3_com->Integral());
// hFTheta_P4_com->Scale(1./hFTheta_P4_com->Integral());
// hFPhi_P3_com->Scale(1./hFPhi_P3_com->Integral());
// hFPhi_P4_com->Scale(1./hFPhi_P4_com->Integral());
// hFEnSum_com->Scale(1./hFEnSum_com->Integral());
// hFThetaSum_com->Scale(1./hFThetaSum_com->Integral());
// hFPhiSum_com->Scale(1./hFPhiSum_com->Integral());
// hFPhiVSTheta_P3_com->Scale(1./hFPhiVSTheta_P3_com->Integral());
// hFPhiVSTheta_P4_com->Scale(1./hFPhiVSTheta_P4_com->Integral());
// hFDeltaTheta_DeltaPhi_com->Scale(1./hFDeltaTheta_DeltaPhi_com->Integral());

// hFOccAllCuts_P3_lab->Scale(1./hFOccAllCuts_P3_lab->Integral());
// hFOccAllCuts_P4_lab->Scale(1./hFOccAllCuts_P4_lab->Integral());
// hFEnAllCuts_P3_lab->Scale(1./hFEnAllCuts_P3_lab->Integral());
// hFEnAllCuts_P4_lab->Scale(1./hFEnAllCuts_P4_lab->Integral());
// hFEnAllCuts_1_lab->Scale(1./hFEnAllCuts_1_lab->Integral());
// hFEnAllCuts_2_lab->Scale(1./hFEnAllCuts_2_lab->Integral());
// hFEnAllCuts_1vs2_lab->Scale(1./hFEnAllCuts_1vs2_lab->Integral());
// hFThetaAllCuts_1_lab->Scale(1./hFThetaAllCuts_1_lab->Integral());
// hFThetaAllCuts_2_lab->Scale(1./hFThetaAllCuts_2_lab->Integral());
// hFThetaAllCuts_1vs2_lab->Scale(1./hFThetaAllCuts_1vs2_lab->Integral());
// hFThetaAllCuts_P3_lab->Scale(1./hFThetaAllCuts_P3_lab->Integral());
// hFThetaAllCuts_P4_lab->Scale(1./hFThetaAllCuts_P4_lab->Integral());
// hFPhiAllCuts_P3_lab->Scale(1./hFPhiAllCuts_P3_lab->Integral());
// hFPhiAllCuts_P4_lab->Scale(1./hFPhiAllCuts_P4_lab->Integral());
// hFEnSumAllCuts_lab->Scale(1./hFEnSumAllCuts_lab->Integral());
// hFThetaSumAllCuts_lab->Scale(1./hFThetaSumAllCuts_lab->Integral());
// hFPhiSumAllCuts_lab->Scale(1./hFPhiSumAllCuts_lab->Integral());
// hFPhiVSThetaAllCuts_P3_lab->Scale(1./hFPhiVSThetaAllCuts_P3_lab->Integral());
// hFPhiVSThetaAllCuts_P4_lab->Scale(1./hFPhiVSThetaAllCuts_P4_lab->Integral());
// hFDeltaTheta_DeltaPhiAllCuts_lab->Scale(1./hFDeltaTheta_DeltaPhiAllCuts_lab->Integral());
// hFMissingMassAllCuts->Scale(1./hFMissingMassAllCuts->Integral());
// hFEnAllCuts_P3_com->Scale(1./hFEnAllCuts_P3_com->Integral());
// hFEnAllCuts_P4_com->Scale(1./hFEnAllCuts_P4_com->Integral());
// hFEnAllCuts_1_com->Scale(1./hFEnAllCuts_1_com->Integral());
// hFEnAllCuts_2_com->Scale(1./hFEnAllCuts_2_com->Integral());
// hFEnAllCuts_1vs2_com->Scale(1./hFEnAllCuts_1vs2_com->Integral());
// hFThetaAllCuts_1_com->Scale(1./hFThetaAllCuts_1_com->Integral());
// hFThetaAllCuts_2_com->Scale(1./hFThetaAllCuts_2_com->Integral());
// hFThetaAllCuts_P3_com->Scale(1./hFThetaAllCuts_P3_com->Integral());
// hFThetaAllCuts_P4_com->Scale(1./hFThetaAllCuts_P4_com->Integral());
// hFPhiAllCuts_P3_com->Scale(1./hFPhiAllCuts_P3_com->Integral());
// hFPhiAllCuts_P4_com->Scale(1./hFPhiAllCuts_P4_com->Integral());
// hFEnSumAllCuts_com->Scale(1./hFEnSumAllCuts_com->Integral());
// hFThetaSumAllCuts_com->Scale(1./hFThetaSumAllCuts_com->Integral());
// hFPhiSumAllCuts_com->Scale(1./hFPhiSumAllCuts_com->Integral());
// hFPhiVSThetaAllCuts_P3_com->Scale(1./hFPhiVSThetaAllCuts_P3_com->Integral());
// hFPhiVSThetaAllCuts_P4_com->Scale(1./hFPhiVSThetaAllCuts_P4_com->Integral());
// hFDeltaTheta_DeltaPhiAllCuts_com->Scale(1./hFDeltaTheta_DeltaPhiAllCuts_com->Integral());

// double normfactorBKG = 2.9e5;
// double normfactorSig = 4625;
// hFP1->Scale(2.9e5/hFP1->Integral());
// hFP2->Scale(2.9e5/hFP2->Integral());
// hFInitEnSum->Scale(2.9e5/hFInitEnSum->Integral());
// hFOcc_P3_lab->Scale(2.9e5/hFOcc_P3_lab->Integral());
// hFOcc_P4_lab->Scale(2.9e5/hFOcc_P4_lab->Integral());
// hFEn_P3_lab->Scale(2.9e5/hFEn_P3_lab->Integral());
// hFEn_P4_lab->Scale(2.9e5/hFEn_P4_lab->Integral());
// hFEn_1_lab->Scale(2.9e5/hFEn_1_lab->Integral());
// hFEn_2_lab->Scale(2.9e5/hFEn_2_lab->Integral());
// hFEn_1vs2_lab->Scale(2.9e5/hFEn_1vs2_lab->Integral());
// hFTheta_1_lab->Scale(2.9e5/hFTheta_1_lab->Integral());
// hFTheta_2_lab->Scale(2.9e5/hFTheta_2_lab->Integral());
// hFTheta_1vs2_lab->Scale(2.9e5/hFTheta_1vs2_lab->Integral());
// hFTheta_P3_lab->Scale(2.9e5/hFTheta_P3_lab->Integral());
// hFTheta_P4_lab->Scale(2.9e5/hFTheta_P4_lab->Integral());
// hFPhi_P3_lab->Scale(2.9e5/hFPhi_P3_lab->Integral());
// hFPhi_P4_lab->Scale(2.9e5/hFPhi_P4_lab->Integral());
// hFEnSum_lab->Scale(2.9e5/hFEnSum_lab->Integral());
// hFThetaSum_lab->Scale(2.9e5/hFThetaSum_lab->Integral());
// hFPhiSum_lab->Scale(2.9e5/hFPhiSum_lab->Integral());
// hFPhiVSTheta_P3_lab->Scale(2.9e5/hFPhiVSTheta_P3_lab->Integral());
// hFPhiVSTheta_P4_lab->Scale(2.9e5/hFPhiVSTheta_P4_lab->Integral());
// hFDeltaTheta_DeltaPhi_lab->Scale(2.9e5/hFDeltaTheta_DeltaPhi_lab->Integral());
// hFMissingMass->Scale(2.9e5/hFMissingMass->Integral());
// hFEn_P3_com->Scale(2.9e5/hFEn_P3_com->Integral());
// hFEn_P4_com->Scale(2.9e5/hFEn_P4_com->Integral());
// hFEn_1_com->Scale(2.9e5/hFEn_1_com->Integral());
// hFEn_2_com->Scale(2.9e5/hFEn_2_com->Integral());
// hFEn_1vs2_com->Scale(2.9e5/hFEn_1vs2_com->Integral());
// hFTheta_1_com->Scale(2.9e5/hFTheta_1_com->Integral());
// hFTheta_2_com->Scale(2.9e5/hFTheta_2_com->Integral());
// hFTheta_1vs2_com->Scale(2.9e5/hFTheta_1vs2_com->Integral());
// hFTheta_P3_com->Scale(2.9e5/hFTheta_P3_com->Integral());
// hFTheta_P4_com->Scale(2.9e5/hFTheta_P4_com->Integral());
// hFPhi_P3_com->Scale(2.9e5/hFPhi_P3_com->Integral());
// hFPhi_P4_com->Scale(2.9e5/hFPhi_P4_com->Integral());
// hFEnSum_com->Scale(2.9e5/hFEnSum_com->Integral());
// hFThetaSum_com->Scale(2.9e5/hFThetaSum_com->Integral());
// hFPhiSum_com->Scale(2.9e5/hFPhiSum_com->Integral());
// hFPhiVSTheta_P3_com->Scale(2.9e5/hFPhiVSTheta_P3_com->Integral());
// hFPhiVSTheta_P4_com->Scale(2.9e5/hFPhiVSTheta_P4_com->Integral());
// hFDeltaTheta_DeltaPhi_com->Scale(2.9e5/hFDeltaTheta_DeltaPhi_com->Integral());

// hFOccAllCuts_P3_lab->Scale(2.9e5/hFOccAllCuts_P3_lab->Integral());
// hFOccAllCuts_P4_lab->Scale(2.9e5/hFOccAllCuts_P4_lab->Integral());
// hFEnAllCuts_P3_lab->Scale(2.9e5/hFEnAllCuts_P3_lab->Integral());
// hFEnAllCuts_P4_lab->Scale(2.9e5/hFEnAllCuts_P4_lab->Integral());
// hFEnAllCuts_1_lab->Scale(2.9e5/hFEnAllCuts_1_lab->Integral());
// hFEnAllCuts_2_lab->Scale(2.9e5/hFEnAllCuts_2_lab->Integral());
// hFEnAllCuts_1vs2_lab->Scale(2.9e5/hFEnAllCuts_1vs2_lab->Integral());
// hFThetaAllCuts_1_lab->Scale(2.9e5/hFThetaAllCuts_1_lab->Integral());
// hFThetaAllCuts_2_lab->Scale(2.9e5/hFThetaAllCuts_2_lab->Integral());
// hFThetaAllCuts_1vs2_lab->Scale(2.9e5/hFThetaAllCuts_1vs2_lab->Integral());
// hFThetaAllCuts_P3_lab->Scale(2.9e5/hFThetaAllCuts_P3_lab->Integral());
// hFThetaAllCuts_P4_lab->Scale(2.9e5/hFThetaAllCuts_P4_lab->Integral());
// hFPhiAllCuts_P3_lab->Scale(2.9e5/hFPhiAllCuts_P3_lab->Integral());
// hFPhiAllCuts_P4_lab->Scale(2.9e5/hFPhiAllCuts_P4_lab->Integral());
// hFEnSumAllCuts_lab->Scale(2.9e5/hFEnSumAllCuts_lab->Integral());
// hFThetaSumAllCuts_lab->Scale(2.9e5/hFThetaSumAllCuts_lab->Integral());
// hFPhiSumAllCuts_lab->Scale(2.9e5/hFPhiSumAllCuts_lab->Integral());
// hFPhiVSThetaAllCuts_P3_lab->Scale(2.9e5/hFPhiVSThetaAllCuts_P3_lab->Integral());
// hFPhiVSThetaAllCuts_P4_lab->Scale(2.9e5/hFPhiVSThetaAllCuts_P4_lab->Integral());
// hFDeltaTheta_DeltaPhiAllCuts_lab->Scale(2.9e5/hFDeltaTheta_DeltaPhiAllCuts_lab->Integral());
// hFMissingMassAllCuts->Scale(2.9e5/hFMissingMassAllCuts->Integral());
// hFEnAllCuts_P3_com->Scale(2.9e5/hFEnAllCuts_P3_com->Integral());
// hFEnAllCuts_P4_com->Scale(2.9e5/hFEnAllCuts_P4_com->Integral());
// hFEnAllCuts_1_com->Scale(2.9e5/hFEnAllCuts_1_com->Integral());
// hFEnAllCuts_2_com->Scale(2.9e5/hFEnAllCuts_2_com->Integral());
// hFEnAllCuts_1vs2_com->Scale(2.9e5/hFEnAllCuts_1vs2_com->Integral());
// hFThetaAllCuts_1_com->Scale(2.9e5/hFThetaAllCuts_1_com->Integral());
// hFThetaAllCuts_2_com->Scale(2.9e5/hFThetaAllCuts_2_com->Integral());
// hFThetaAllCuts_1vs2_com->Scale(2.9e5/hFThetaAllCuts_1vs2_com->Integral());
// hFThetaAllCuts_P3_com->Scale(2.9e5/hFThetaAllCuts_P3_com->Integral());
// hFThetaAllCuts_P4_com->Scale(2.9e5/hFThetaAllCuts_P4_com->Integral());
// hFPhiAllCuts_P3_com->Scale(2.9e5/hFPhiAllCuts_P3_com->Integral());
// hFPhiAllCuts_P4_com->Scale(2.9e5/hFPhiAllCuts_P4_com->Integral());
// hFEnSumAllCuts_com->Scale(2.9e5/hFEnSumAllCuts_com->Integral());
// hFThetaSumAllCuts_com->Scale(2.9e5/hFThetaSumAllCuts_com->Integral());
// hFPhiSumAllCuts_com->Scale(2.9e5/hFPhiSumAllCuts_com->Integral());
// hFPhiVSThetaAllCuts_P3_com->Scale(2.9e5/hFPhiVSThetaAllCuts_P3_com->Integral());
// hFPhiVSThetaAllCuts_P4_com->Scale(2.9e5/hFPhiVSThetaAllCuts_P4_com->Integral());
// hFDeltaTheta_DeltaPhiAllCuts_com->Scale(2.9e5/hFDeltaTheta_DeltaPhiAllCuts_com->Integral());

// cout << "file input chiuso " << endl;
//Fill Cutflow histo
for(int i = 1; i<=nCuts; i++){
    hcutflow->SetBinContent(i, CutFlow[i-1]);
    // cout << "hcutflow histo filling " << endl;

    hcutflow->GetXaxis()->SetBinLabel(i, CutName[i-1]);
    // cout << "hcutflow set bin label " << endl;
}
hcutflow->Scale(1./hcutflow->GetBinContent(1)); //normalized to the first bin
// normalization of the hcutflow histo removed to check the effect of phi cut
// cout << "hcutflow fine filling " << endl;

//write the output file.root    

TDirectory *Full_dir = FileOut->mkdir("BhabhaFull_dir" + TString::Format("%i", e10));   
// cout << "Full Dir pointer: " << Full_dir << endl;
TDirectory *labdir = Full_dir->mkdir("labdir"); 
// cout << " labdir pointer : " << labdir << endl;
TDirectory *comdir = Full_dir->mkdir("comdir");   
// cout << " comdir pointer : " << comdir << endl;

FileOut->cd();

Full_dir->cd();
MGDynCut->Write();
hcutflow->Write();

labdir->cd();
hFInitEnSum->Write();
hFOcc_P3_lab->Write();
hFOcc_P4_lab->Write();
hFEn_P3_lab->Write();
hFEn_P4_lab->Write();
hFEn_1_lab->Write();
hFEn_2_lab->Write();
hFEn_1vs2_lab->Write();
hFTheta_1_lab->Write();
hFTheta_2_lab->Write();
hFTheta_1vs2_lab->Write();
hFTheta_P3_lab->Write();
hFTheta_P4_lab->Write();
hFPhi_P3_lab->Write();
hFPhi_P4_lab->Write();
hFPhiVSTheta_P3_lab->Write();
hFPhiVSTheta_P4_lab->Write();
hFEnSum_lab->Write();
hFThetaSum_lab->Write();
hFPhiSum_lab->Write();
hFDeltaTheta_DeltaPhi_lab->Write();
hFMissingMass->Write();
hFOccAllCuts_P3_lab->Write();
hFOccAllCuts_P4_lab->Write();
hFOccAllCuts_1_lab->Write();
hFOccAllCuts_2_lab->Write();
hFEnAllCuts_P3_lab->Write();
hFEnAllCuts_P4_lab->Write();
hFEnAllCuts_1_lab->Write();
hFEnAllCuts_2_lab->Write();
hFEnAllCuts_1vs2_lab->Write();
hFThetaAllCuts_1_lab->Write();
hFThetaAllCuts_2_lab->Write();
hFThetaAllCuts_1vs2_lab->Write();
hFThetaAllCuts_P3_lab->Write();
hFThetaAllCuts_P4_lab->Write();
hFPhiAllCuts_P3_lab->Write();
hFPhiAllCuts_P4_lab->Write();
hFPhiVSThetaAllCuts_P3_lab->Write();
hFPhiVSThetaAllCuts_P4_lab->Write();
hFEnSumAllCuts_lab->Write();
hFThetaSumAllCuts_lab->Write();
hFPhiSumAllCuts_lab->Write();
hFDeltaTheta_DeltaPhiAllCuts_lab->Write();
hFMissingMassAllCuts->Write();

labdir->Write();

FileOut->cd();
Full_dir->cd();
comdir->cd();
hFEn_P3_com->Write();
hFEn_P4_com->Write();
hFEn_1_com->Write();
hFEn_2_com->Write();
hFEn_1vs2_com->Write();
hFTheta_1_com->Write();
hFTheta_2_com->Write();
hFTheta_1vs2_com->Write();
hFTheta_P3_com->Write();
hFTheta_P4_com->Write();
hFPhi_P3_com->Write();
hFPhi_P4_com->Write();
hFEnSum_com->Write();
hFThetaSum_com->Write();
hFPhiSum_com->Write();
hFPhiVSTheta_P3_com->Write();
hFPhiVSTheta_P4_com->Write();
hFDeltaTheta_DeltaPhi_com->Write();
hFEnAllCuts_P3_com->Write();
hFEnAllCuts_P4_com->Write();
hFEnAllCuts_1_com->Write();
hFEnAllCuts_2_com->Write();
hFEnAllCuts_1vs2_com->Write();
hFThetaAllCuts_1_com->Write();
hFThetaAllCuts_2_com->Write();
hFThetaAllCuts_1vs2_com->Write();
hFThetaAllCuts_P3_com->Write();
hFThetaAllCuts_P4_com->Write();
hFPhiAllCuts_P3_com->Write();
hFPhiAllCuts_P4_com->Write();
hFEnSumAllCuts_com->Write();
hFThetaSumAllCuts_com->Write();
hFPhiSumAllCuts_com->Write();
hFPhiVSThetaAllCuts_P3_com->Write();
hFPhiVSThetaAllCuts_P4_com->Write();
hFDeltaTheta_DeltaPhiAllCuts_com->Write();

comdir->Write();

Full_dir->Write();

cout << "Output file written " << endl;
FileOut->Close();
cout << "Output file closed " << endl;
// delete FileOut;
// cout << "output file deleted " << endl;
BhabhaFullIn->Close();
cout << "Input file closed " << endl;
// delete BhabhaFullIn;
// cout << "input file deleted " << endl;

}

int main(int argc, char** argv) {
    if (argc != 18) {
        cerr
          << "Usage:\n  " << argv[0]
          << " <EBeam(MeV)> <period_id> <nEvent> <thetaX(rad)> <thetaY(rad)> <run(3|4|5)>\n"
          << "  <TargetX> <TargetY> <TargetZ> <ECalX> <ECalY> <ECalZ> <MagX> <MagY> <MagZ>\n"
          << "  <input_root_file> <output_root_file>\n";
        return 1;
    }

    const double EBeam     = strtod(argv[1], nullptr);
    const int    period_id = atoi(argv[2]);   // currently unused, keep if you want
    const int    nEvent    = atoi(argv[3]);
    const double ThetaX    = strtod(argv[4], nullptr);
    const double ThetaY    = strtod(argv[5], nullptr);

    const int run = stoi(string(argv[6]));
    if (run < 3 || run > 5) {
        cerr << "Invalid run configuration. Choose 3, 4, or 5.\n";
        return 1;
    }

    TVector3 TargetPosition(
        strtod(argv[7],  nullptr),
        strtod(argv[8],  nullptr),
        strtod(argv[9],  nullptr)
    );
    TVector3 ECalPosition(
        strtod(argv[10], nullptr),
        strtod(argv[11], nullptr),
        strtod(argv[12], nullptr)
    );
    TVector3 MagnetBoreSize(
        strtod(argv[13], nullptr),
        strtod(argv[14], nullptr),
        strtod(argv[15], nullptr)
    );

    array<TVector3,3> runconfig{TargetPosition, ECalPosition, MagnetBoreSize};

    const char* inputRootFile  = argv[16];
    const char* outputRootFile = argv[17];

    cout << "##############Running ToyCalc_AA with parameters ##############" << endl;
    cout << "EBeam=" << EBeam << " MeV  nEvent=" << nEvent
         << " run=" << run << " thetaX=" << ThetaX << " thetaY=" << ThetaY << "\n"
         << "Input : " << inputRootFile << "\n"
         << "Output: " << outputRootFile << endl;

    ToyCalc_AA(EBeam, nEvent, runconfig, ThetaX, ThetaY, inputRootFile, outputRootFile);

    return 0;
}
