// Background studies and sensitivity - 1 scan point in sqrt(s)
#include <TH1.h>
#include <TH2.h>
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

void BkgSens(double EBeam, double AngBeamDirX, double AngBeamDirY, const char* SChInput, const char* FullInput, const char* AAInput, const char* OutputName){  

double pi2 = TMath::Pi()*2;
static const int NRows = 200000; //number of events generated with CalcHEP -> to be improved to a parameter in a script (?)
double MassEl = 0.511; //in MeV
TVector3 TargetPosition (0.,0.,-1028.); //read by MC full
TVector3 ECalPosition (0.,0.,2672.); //read by MC full
double TargetCalo = abs(TargetPosition.Z() - ECalPosition.Z()); //distance along beam axis between target and calorimeter
double ShadowCut = 102/abs(TargetPosition.Z()-500); // in Rad
// histogram binning in energy and ECal reso - it depends on the beam as input
double CaloNBins = 30;
double OccBins = 450;

//CUTS
double RECal = 304.5; //external radius of ECal 
double SUsize = 20.;
double HalfHole = 50.; //maybe not needed
double RMax = RECal;  // in mm
double RMin = 90;
double EMax = 0.300; //deleted because at TOY level seems to be unusefull
double EMin = 0.05;
double MagnetCut = ShadowCut*TargetCalo;
cout<<"Magnet Shadow on ECal = "<<MagnetCut<<" [mm]"<<endl;

//creating output file with Histos
TFile *FileOut = new TFile(OutputName, "RECREATE");

//CUTS TOMMASO - they are defined here once for type of process
TVector3 TargetImpact; // impact point of the beam on target
TVector3 BeamDirDisplacement; // displacement due to the beam direction
double SqrtS, BeamMomentum;
double BG, Beta, Gamma;
TVector3 BoostMom;
double Radius_min, Energy_max, Energy_min;

TargetImpact.SetXYZ(0, 0, TargetPosition.Z()); //impact point of the beam on target 
BeamDirDisplacement.SetXYZ(0 + TargetCalo*tan(AngBeamDirX), 0 + TargetCalo*tan(AngBeamDirY), ECalPosition.Z());
SqrtS = TMath::Sqrt(2.*MassEl*EBeam); // in MeV
BeamMomentum = TMath::Sqrt(EBeam*EBeam - 2*MassEl*MassEl); //in MeV
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

Radius_min = (BeamDirDisplacement.Z()-TargetImpact.Z())*TMath::Tan(0.5*TMath::Pi()-tLim)/Gamma;// (fmBeamDirDisplacement.Z()-fRTarg.Z())*tanQMin; 
Energy_max = SqrtS*Gamma*0.5*(1.-TMath::Cos(2*tLim)); 
Energy_min = SqrtS*Gamma*0.5*(1.+TMath::Cos(2*tLim)); 

cout << "GeneralInfo: run-level info for run from DYNAMIC CUTS at " << EBeam <<" MeV" << " Pbeam = " << BeamMomentum<<endl;
cout << " target = { "<< TargetImpact.X()<< " , "<< TargetImpact.Y() << " , " << TargetImpact.Z() << 
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
    
TDirectory *SCh_dir = FileOut->mkdir("BhabhaSCh_dir" + TString::Format("%.2f", EBeam));
TDirectory *Full_dir = FileOut->mkdir("BhabhaFull_dir" + TString::Format("%.2f", EBeam));
TDirectory *GG_dir = FileOut->mkdir("AAProduction_dir" + TString::Format("%.2f", EBeam));
TFile *BhabhaSChIn = TFile::Open(SChInput); 
TTree *tSch  = (TTree*)BhabhaSChIn->Get("tBhabhaSCh_" + TString::Format("%.2f", EBeam));
double P3pos_ini, P3ele_ini, SFinalP1_ele, SFinalP2_ele, SFinalP3_ele, SFinalP1_pos, SFinalP2_pos, SFinalP3_pos; // Definisci la variabile per il branch che desideri acquisire
tSch->SetBranchAddress("InitialP3_positron", &P3pos_ini);
tSch->SetBranchAddress("InitialP3_electron", &P3ele_ini);
tSch->SetBranchAddress("SFinalP1_electron", &SFinalP1_ele);
tSch->SetBranchAddress("SFinalP2_electron", &SFinalP2_ele);
tSch->SetBranchAddress("SFinalP3_electron", &SFinalP3_ele);
tSch->SetBranchAddress("SFinalP1_positron", &SFinalP1_pos);
tSch->SetBranchAddress("SFinalP2_positron", &SFinalP2_pos);
tSch->SetBranchAddress("SFinalP3_positron", &SFinalP3_pos);
    
TFile *BhabhaFullIn = TFile::Open(FullInput);
TTree *tFull  = (TTree*)BhabhaFullIn->Get("tBhabhaFull_" + TString::Format("%.2f", EBeam)); 
double FFinalP1_ele, FFinalP2_ele, FFinalP3_ele, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos; 
tFull->SetBranchAddress("FFinalP1_electron", &FFinalP1_ele);
tFull->SetBranchAddress("FFinalP2_electron", &FFinalP2_ele);
tFull->SetBranchAddress("FFinalP3_electron", &FFinalP3_ele);
tFull->SetBranchAddress("FFinalP1_positron", &FFinalP1_pos);
tFull->SetBranchAddress("FFinalP2_positron", &FFinalP2_pos);
tFull->SetBranchAddress("FFinalP3_positron", &FFinalP3_pos);

TFile *AAProductionIn = TFile::Open(AAInput);
TTree *tGG  = (TTree*)AAProductionIn->Get("tAAProduction_" + TString::Format("%.2f", EBeam)); 
double GFinalP1_G1, GFinalP2_G1, GFinalP3_G1, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2; 
tGG->SetBranchAddress("GFinalP1_G1", &GFinalP1_G1);
tGG->SetBranchAddress("GFinalP2_G1", &GFinalP2_G1);
tGG->SetBranchAddress("GFinalP3_G1", &GFinalP3_G1);
tGG->SetBranchAddress("GFinalP1_G2", &GFinalP1_G2);
tGG->SetBranchAddress("GFinalP2_G2", &GFinalP2_G2);
tGG->SetBranchAddress("GFinalP3_G2", &GFinalP3_G2);

//inizializing histos

TH2F* hSOcc_P3 = new TH2F(("hSOcc_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOcc_pos_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
TH2F* hSOcc_P4 = new TH2F(("hSOcc_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOcc_ele_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
TH2F* hFOcc_P3 = new TH2F(("hFOcc_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOcc_pos_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
TH2F* hFOcc_P4 = new TH2F(("hFOcc_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOcc_ele_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
TH2F* hGOcc_P3 = new TH2F(("hGOcc_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOcc_G1_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
TH2F* hGOcc_P4 = new TH2F(("hGOcc_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOcc_G2_" + to_string_with_precision(EBeam, 2)).c_str(), OccBins, -450, 450, OccBins, -450, 450);

TH2F* hSOccGeom_P3 = new TH2F(("hSOccGeom_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOccGeom_pos_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hSOccGeom_P4 = new TH2F(("hSOccGeom_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOccGeom_ele_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hFOccGeom_P3 = new TH2F(("hFOccGeom_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccGeom_pos_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hFOccGeom_P4 = new TH2F(("hFOccGeom_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccGeom_ele_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hGOccGeom_P3 = new TH2F(("hGOccGeom_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hGOccGeom_G1_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hGOccGeom_P4 = new TH2F(("hGOccGeom_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hGOccGeom_G2_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);

TH2F* hSOccAllCuts_P3 = new TH2F(("hSOccAllCuts_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOccAllCuts_pos_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hSOccAllCuts_P4 = new TH2F(("hSOccAllCuts_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hSOccAllCuts_ele_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hFOccAllCuts_P3 = new TH2F(("hFOccAllCuts_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccAllCuts_pos_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hFOccAllCuts_P4 = new TH2F(("hFOccAllCuts_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hFOccAllCuts_ele_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hGOccAllCuts_P3 = new TH2F(("hGOccAllCuts_P3_" + to_string_with_precision(EBeam, 2)).c_str(), ("hGOccAllCuts_G1_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
TH2F* hGOccAllCuts_P4 = new TH2F(("hGOccAllCuts_P4_" + to_string_with_precision(EBeam, 2)).c_str(), ("hGOccAllCuts_G2_" + to_string_with_precision(EBeam, 2)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);

double EnBins = static_cast<int>(EBeam)*5;
TH1F* hSEnDist_P3 = new TH1F(("hSEnDist_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hSEnDist_pos" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hSEnDist_P4 = new TH1F(("hSEnDist_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hSEnDist_ele" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hSEnDistGeom_P3 = new TH1F(("hSEnDistGeom_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hSEnDistGeom_pos" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hSEnDistGeom_P4 = new TH1F(("hSEnDistGeom_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hSEnDistGeom_ele" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hSEnDistAllCuts_P3 = new TH1F(("hSEnDistAllCuts_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hSEnDistAllCuts_pos" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hSEnDistAllCuts_P4 = new TH1F(("hSEnDistAllCuts_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hSEnDistAllCuts_ele" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);

TH1F* hFEnDist_P3 = new TH1F(("hFEnDist_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnDist_pos" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hFEnDist_P4 = new TH1F(("hFEnDist_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnDist_ele" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hFEnDistGeom_P3 = new TH1F(("hFEnDistGeom_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnDistGeom_pos" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hFEnDistGeom_P4 = new TH1F(("hFEnDistGeom_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnDistGeom_ele" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hFEnDistAllCuts_P3 = new TH1F(("hFEnDistAllCuts_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnDistAllCuts_pos" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hFEnDistAllCuts_P4 = new TH1F(("hFEnDistAllCuts_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hFEnDistAllCuts_ele" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);

TH1F* hGEnDist_P3 = new TH1F(("hGEnDist_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hGEnDist_G1" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hGEnDist_P4 = new TH1F(("hGEnDist_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hGEnDist_G2" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hGEnDistGeom_P3 = new TH1F(("hGEnDistGeom_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hGEnDistGeom_G1" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hGEnDistGeom_P4 = new TH1F(("hGEnDistGeom_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hGEnDistGeom_G2" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hGEnDistAllCuts_P3 = new TH1F(("hGEnDistAllCuts_P3" + to_string_with_precision(EBeam, 2)).c_str(), ("hGEnDistAllCuts_G1" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);
TH1F* hGEnDistAllCuts_P4 = new TH1F(("hGEnDistAllCuts_P4" + to_string_with_precision(EBeam, 2)).c_str(), ("hGEnDistAllCuts_G2" + to_string_with_precision(EBeam, 2)).c_str(), EnBins, 0, static_cast<int>(EBeam)*1e-3);

//TVector3 for each of all 4 particles in the process
TVector3 P1;
TVector3 P2;
TVector3 SP3, FP3, GP3;
TVector3 SP4, FP4, GP4;
TVector3 SP3_com, FP3_com, GP3_com;
TVector3 SP4_com, FP4_com, GP4_com;


double thetaP = TMath::ATan(TMath::Sqrt(TMath::Tan(AngBeamDirX)*TMath::Tan(AngBeamDirX) + TMath::Tan(AngBeamDirY)*TMath::Tan(AngBeamDirY)));
double phi;
if(AngBeamDirX == 0 && AngBeamDirY== 0) phi = 0;
else phi = TMath::ACos(TMath::Tan(AngBeamDirX)/TMath::Sqrt(TMath::Tan(AngBeamDirX)*TMath::Tan(AngBeamDirX) + TMath::Tan(AngBeamDirY)*TMath::Tan(AngBeamDirY)));
TVector3 Boost;
Boost.SetXYZ(TMath::Sin(thetaP)*TMath::Cos(phi), TMath::Sin(thetaP)*TMath::Sin(phi), TMath::Cos(thetaP));

for(int iR = 0; iR<NRows; iR++){
    tSch->GetEntry(iR);
    tFull->GetEntry(iR);
    tGG->GetEntry(iR);
    //defining 2->2 body process from Tree in input P1-P3 positrons, P2-P4 electrons
    P1.SetXYZ(0,0,P3pos_ini);
    P2.SetXYZ(0,0,P3ele_ini);
    SP3.SetXYZ(SFinalP1_pos,SFinalP2_pos,SFinalP3_pos);
    SP4.SetXYZ(SFinalP1_ele,SFinalP2_ele,SFinalP3_ele);

    FP3.SetXYZ(FFinalP1_pos,FFinalP2_pos,FFinalP3_pos);
    FP4.SetXYZ(FFinalP1_ele,FFinalP2_ele,FFinalP3_ele);

    GP3.SetXYZ(GFinalP1_G1,GFinalP2_G1,GFinalP3_G1);
    GP4.SetXYZ(GFinalP1_G2,GFinalP2_G2,GFinalP3_G2);
    // cout<<P1.Z()<<" "<<P2.Z()<<" "<<P3.X()<<" "<<P3.Y()<<" "<<P3.Y()<<" "<<P4.X()<<" "<<P4.Y()<<" "<<P4.Y()<<" "<<endl;

    // initial state
    double P1_Ptot = P1.Mag();
    double P2_Ptot = P2.Mag();
    double P1_En = TMath::Sqrt(P1.Mag2()); 
    double P2_En = 0; // MassEl*1e-3;
    double E12_tot = P1_En + P2_En;
    // cout<< "Energies initial state: "<<E12_tot<<endl;

    // final state
    // total momentum
    double SP3_Ptot = SP3.Mag();
    double SP4_Ptot = SP4.Mag();
    double FP3_Ptot = FP3.Mag();
    double FP4_Ptot = FP4.Mag();
    double GP3_Ptot = GP3.Mag();
    double GP4_Ptot = GP4.Mag();
    // energy 
    double SP3_En = sqrt(MassEl*MassEl*1e-6 + SP3.Mag2()); // 
    double SP4_En = sqrt(MassEl*MassEl*1e-6 + SP4.Mag2()); // MassEl*MassEl*1e-6 + 
    double FP3_En = sqrt(MassEl*MassEl*1e-6 + FP3.Mag2()); // MassEl*MassEl*1e-6 + 
    double FP4_En = sqrt(MassEl*MassEl*1e-6 + FP4.Mag2()); // MassEl*MassEl*1e-6 + 
    double GP3_En = sqrt(GP3.Mag2());
    double GP4_En = sqrt(GP4.Mag2());
    double SE34_tot = SP3_En + SP4_En;
    double FE34_tot = FP3_En + FP4_En;
    double GE34_tot = GP3_En + GP4_En;
    // cout<< "Energies by CalcHEP: "<<SE34_tot<<" "<< FE34_tot <<" "<< GE34_tot<<endl;

    // polar angle
    double SP3_PolarTheta = SP3.Theta();
    double SP4_PolarTheta = SP4.Theta();
    double FP3_PolarTheta = FP3.Theta();
    double FP4_PolarTheta = FP4.Theta();
    double GP3_PolarTheta = GP3.Theta();
    double GP4_PolarTheta = GP4.Theta();    
    //radius at target before boosting
    double SP3_R = tan(SP3_PolarTheta)*TargetCalo;
    double SP4_R = tan(SP4_PolarTheta)*TargetCalo;
    double FP3_R = tan(FP3_PolarTheta)*TargetCalo;
    double FP4_R = tan(FP4_PolarTheta)*TargetCalo;
    double GP3_R = tan(GP3_PolarTheta)*TargetCalo;
    double GP4_R = tan(GP4_PolarTheta)*TargetCalo;

    // moving to CoM frame - transverse momentum remain the same component per component
    //coord in com frame
    SP3_com.SetXYZ(SFinalP1_pos,SFinalP2_pos, (Gamma*SP3.Z() - BG*SP3_En));
    SP4_com.SetXYZ(SFinalP1_ele,SFinalP2_ele, (Gamma*SP4.Z() - BG*SP4_En));
    // cout<<" SChannel verificato CoM? "<<SP3_com.Mag()<<" = "<<SP4_com.Mag()<<endl;

    FP3_com.SetXYZ(FFinalP1_pos,FFinalP2_pos, (Gamma*FP3.Z() - BG*FP3_En));
    FP4_com.SetXYZ(FFinalP1_ele,FFinalP2_ele, (Gamma*FP4.Z() - BG*FP4_En));
    // cout<<" BhabhaFull verificato CoM? "<<FP3_com.Mag()<<" = "<<FP4_com.Mag()<<endl;

    GP3_com.SetXYZ(GFinalP1_G1,GFinalP2_G1, (Gamma*GP3.Z() - BG*GP3_En));
    GP4_com.SetXYZ(GFinalP1_G2,GFinalP2_G2, (Gamma*GP4.Z() - BG*GP4_En));
    // cout<<" eetoAA verificato CoM? "<<GP3_com.Mag()<<" = "<<GP4_com.Mag()<<endl;

    //energy in com frame (derived with boost)
    double SP3_En_com = Gamma*SP3_En - BG*SP3.Z();
    double SP4_En_com = Gamma*SP4_En - BG*SP4.Z();
    double FP3_En_com = Gamma*FP3_En - BG*FP3.Z();
    double FP4_En_com = Gamma*FP4_En - BG*FP4.Z();
    double GP3_En_com = Gamma*GP3_En - BG*GP3.Z();
    double GP4_En_com = Gamma*GP4_En - BG*GP4.Z();
    double SE34_tot_com = SP3_En_com + SP4_En_com;
    double FE34_tot_com = FP3_En_com + FP4_En_com;
    double GE34_tot_com = GP3_En_com + GP4_En_com;
    // cout<< "Energies in COM frame: "<<SE34_tot<<" "<< FE34_tot <<" "<< GE34_tot<<endl;

    //boost in lab frame with beam displacement dependent on thetaX thetaY in input. They are took as a variable derived previously
    SP3.SetX((-BG*Boost.X()*SP3_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*SP3_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*SP3_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*SP3_com.Z());
    SP3.SetY((-BG*Boost.Y()*SP3_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*SP3_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*SP3_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*SP3_com.Z());
    SP3.SetZ((-BG*Boost.Z()*SP3_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*SP3_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*SP3_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*SP3_com.Z());
    SP4.SetX((-BG*Boost.X()*SP4_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*SP4_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*SP4_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*SP4_com.Z());
    SP4.SetY((-BG*Boost.Y()*SP4_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*SP4_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*SP4_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*SP4_com.Z());
    SP4.SetZ((-BG*Boost.Z()*SP4_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*SP4_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*SP4_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*SP4_com.Z());

    FP3.SetX((-BG*Boost.X()*FP3_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*FP3_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*FP3_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*FP3_com.Z());
    FP3.SetY((-BG*Boost.Y()*FP3_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*FP3_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*FP3_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*FP3_com.Z());
    FP3.SetZ((-BG*Boost.Z()*FP3_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*FP3_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*FP3_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*FP3_com.Z());
    FP4.SetX((-BG*Boost.X()*FP4_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*FP4_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*FP4_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*FP4_com.Z());
    FP4.SetY((-BG*Boost.Y()*FP4_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*FP4_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*FP4_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*FP4_com.Z());
    FP4.SetZ((-BG*Boost.Z()*FP4_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*FP4_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*FP4_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*FP4_com.Z());

    GP3.SetX((-BG*Boost.X()*GP3_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*GP3_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*GP3_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*GP3_com.Z());
    GP3.SetY((-BG*Boost.Y()*GP3_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*GP3_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*GP3_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*GP3_com.Z());
    GP3.SetZ((-BG*Boost.Z()*GP3_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*GP3_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*GP3_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*GP3_com.Z());
    GP4.SetX((-BG*Boost.X()*GP4_En_com) + (1+(1-Gamma)*Boost.X()*Boost.X())*GP4_com.X() + ((1-Gamma)*Boost.X()*Boost.Y())*GP4_com.Y() +((1-Gamma)*Boost.X()*Boost.Z())*GP4_com.Z());
    GP4.SetY((-BG*Boost.Y()*GP4_En_com) + ((1-Gamma)*Boost.Y()*Boost.X())*GP4_com.X() + (1+(1-Gamma)*Boost.Y()*Boost.Y())*GP4_com.Y() +((1-Gamma)*Boost.Y()*Boost.Z())*GP4_com.Z());
    GP4.SetZ((-BG*Boost.Z()*GP4_En_com) + ((1-Gamma)*Boost.Z()*Boost.X())*GP4_com.X() + ((1-Gamma)*Boost.Y()*Boost.Z())*GP4_com.Y() +(1+(1-Gamma)*Boost.Z()*Boost.Z())*GP4_com.Z());
    
    //energy
    SP3_En = Gamma*SP3_En_com - BG*Boost.X()*SP3_com.X() - BG*Boost.Y()*SP3_com.Y() - BG*Boost.Z()*SP3_com.Z();
    SP4_En = Gamma*SP4_En_com - BG*Boost.X()*SP4_com.X() - BG*Boost.Y()*SP4_com.Y() - BG*Boost.Z()*SP4_com.Z();
    FP3_En = Gamma*FP3_En_com - BG*Boost.X()*FP3_com.X() - BG*Boost.Y()*FP3_com.Y() - BG*Boost.Z()*FP3_com.Z();
    FP4_En = Gamma*FP4_En_com - BG*Boost.X()*FP4_com.X() - BG*Boost.Y()*FP4_com.Y() - BG*Boost.Z()*FP4_com.Z();
    GP3_En = Gamma*GP3_En_com - BG*Boost.X()*GP3_com.X() - BG*Boost.Y()*GP3_com.Y() - BG*Boost.Z()*GP3_com.Z();
    GP4_En = Gamma*GP4_En_com - BG*Boost.X()*GP4_com.X() - BG*Boost.Y()*GP4_com.Y() - BG*Boost.Z()*GP4_com.Z();
    SE34_tot = SP3_En + SP4_En;
    FE34_tot = FP3_En + FP4_En;
    GE34_tot = GP3_En + GP4_En;
    // cout<< "Energies after boosting forward: "<<SE34_tot<<" "<< FE34_tot <<" "<< GE34_tot<<endl;

    // impact point @ECal
    double SP3_x = (SP3.X()/SP3.Z())*TargetCalo;
    double SP3_y = (SP3.Y()/SP3.Z())*TargetCalo;
    double SP4_x = (SP4.X()/SP4.Z())*TargetCalo;
    double SP4_y = (SP4.Y()/SP4.Z())*TargetCalo;
    double FP3_x = (FP3.X()/FP3.Z())*TargetCalo;
    double FP3_y = (FP3.Y()/FP3.Z())*TargetCalo;
    double FP4_x = (FP4.X()/FP4.Z())*TargetCalo;
    double FP4_y = (FP4.Y()/FP4.Z())*TargetCalo;
    double GP3_x = (GP3.X()/GP3.Z())*TargetCalo;
    double GP3_y = (GP3.Y()/GP3.Z())*TargetCalo;
    double GP4_x = (GP4.X()/GP4.Z())*TargetCalo;
    double GP4_y = (GP4.Y()/GP4.Z())*TargetCalo;

    //impact radius after boosting. Useful to build the reco efficiency at the edges
    double SP3_ImpR = TMath::Sqrt(SP3_x*SP3_x + SP3_y*SP3_y);
    double SP4_ImpR = TMath::Sqrt(SP4_x*SP4_x + SP4_y*SP4_y);
    double FP3_ImpR = TMath::Sqrt(FP3_x*FP3_x + FP3_y*FP3_y);
    double FP4_ImpR = TMath::Sqrt(FP4_x*FP4_x + FP4_y*FP4_y);
    double GP3_ImpR = TMath::Sqrt(GP3_x*GP3_x + GP3_y*GP3_y);
    double GP4_ImpR = TMath::Sqrt(GP4_x*GP4_x + GP4_y*GP4_y);

    //filling Histo
    hSOcc_P3->Fill(SP3_x, SP3_y);
    hSOcc_P4->Fill(SP4_x, SP4_y);
    hSEnDist_P3->Fill(SP3_En);
    hSEnDist_P4->Fill(SP4_En);
    hFOcc_P3->Fill(FP3_x, FP3_y);
    hFOcc_P4->Fill(FP4_x, FP4_y);
    hFEnDist_P3->Fill(FP3_En);
    hFEnDist_P4->Fill(FP4_En);
    hGOcc_P3->Fill(GP3_x, GP3_y);
    hGOcc_P4->Fill(GP4_x, GP4_y);
    hGEnDist_P3->Fill(GP3_En);
    hGEnDist_P4->Fill(GP4_En);
    //performing geometrical cuts
    if(SP3_R<RMax && SP4_R<RMax && SP3_R>RMin && SP4_R>RMin && abs(SP3_y)<MagnetCut && abs(SP4_y)<MagnetCut){
        hSOccGeom_P3->Fill(SP3_x, SP3_y);
        hSOccGeom_P4->Fill(SP4_x, SP4_y);
        hSEnDistGeom_P3->Fill(SP3_En);
        hSEnDistGeom_P4->Fill(SP4_En);   
        //performing energetical cuts
        if(SP3_En<EMax && SP4_En<EMax && SP3_En>EMin && SP4_En>EMin){
            if(SP3_ImpR>(RECal-SUsize) | SP4_ImpR>(RECal-SUsize)){
                hSOccAllCuts_P3->Fill(SP3_x, SP3_y, 0.7);
                hSOccAllCuts_P4->Fill(SP4_x, SP4_y, 0.7);
                hSEnDistAllCuts_P3->Fill(SP3_En, 0.7);
                hSEnDistAllCuts_P4->Fill(SP4_En, 0.7);
            }
            else{
                hSOccAllCuts_P3->Fill(SP3_x, SP3_y);
                hSOccAllCuts_P4->Fill(SP4_x, SP4_y);
                hSEnDistAllCuts_P3->Fill(SP3_En);
                hSEnDistAllCuts_P4->Fill(SP4_En);
            }
        }
    }   
    if(FP3_R<RMax && FP4_R<RMax && FP3_R>RMin && FP4_R>RMin && abs(FP3_y)<MagnetCut && abs(FP4_y)<MagnetCut){
        hFOccGeom_P3->Fill(FP3_x, FP3_y);
        hFOccGeom_P4->Fill(FP4_x, FP4_y);
        hFEnDistGeom_P3->Fill(FP3_En);
        hFEnDistGeom_P4->Fill(FP4_En);   
        //performing energetical cuts
        if(FP3_En<EMax && FP4_En<EMax && FP3_En>EMin && FP4_En>EMin){
            if(FP3_ImpR>(RECal-SUsize) | FP4_ImpR>(RECal-SUsize)){
                hFOccAllCuts_P3->Fill(FP3_x, FP3_y, 0.7);
                hFOccAllCuts_P4->Fill(FP4_x, FP4_y, 0.7);
                hFEnDistAllCuts_P3->Fill(FP3_En, 0.7);
                hFEnDistAllCuts_P4->Fill(FP4_En, 0.7);
            }
            else{
                hFOccAllCuts_P3->Fill(FP3_x, FP3_y);
                hFOccAllCuts_P4->Fill(FP4_x, FP4_y);
                hFEnDistAllCuts_P3->Fill(FP3_En);
                hFEnDistAllCuts_P4->Fill(FP4_En);
            }
        }
    }   
    if(GP3_R<RMax && GP4_R<RMax && GP3_R>RMin && GP4_R>RMin && abs(GP3_y)<MagnetCut && abs(GP4_y)<MagnetCut){
        hGOccGeom_P3->Fill(GP3_x, GP3_y);
        hGOccGeom_P4->Fill(GP4_x, GP4_y);
        hGEnDistGeom_P3->Fill(GP3_En);
        hGEnDistGeom_P4->Fill(GP4_En);   
        //performing energetical cuts
        if(GP3_En<EMax && GP4_En<EMax && GP3_En>EMin && GP4_En>EMin){
            if(GP3_ImpR>(RECal-SUsize) | GP4_ImpR>(RECal-SUsize)){
                hGOccAllCuts_P3->Fill(GP3_x, GP3_y, 0.7);
                hGOccAllCuts_P4->Fill(GP4_x, GP4_y, 0.7);
                hGEnDistAllCuts_P3->Fill(GP3_En, 0.7);
                hGEnDistAllCuts_P4->Fill(GP4_En, 0.7);
            }
            else{
                hGOccAllCuts_P3->Fill(GP3_x, GP3_y);
                hGOccAllCuts_P4->Fill(GP4_x, GP4_y);
                hGEnDistAllCuts_P3->Fill(GP3_En);
                hGEnDistAllCuts_P4->Fill(GP4_En);
            }
        }
    }   
}
        
FileOut->cd();
MGDynCut->Write();
SCh_dir->cd();
hSOcc_P3->Write();
hSOcc_P4->Write();
hSOccGeom_P3->Write();
hSOccGeom_P4->Write();
hSOccAllCuts_P3->Write();
hSOccAllCuts_P4->Write();
hSEnDist_P3->Write();
hSEnDist_P4->Write();
hSEnDistGeom_P3->Write();
hSEnDistGeom_P4->Write();
hSEnDistAllCuts_P3->Write();
hSEnDistAllCuts_P4->Write();

Full_dir->cd();
hFOcc_P3->Write();
hFOcc_P4->Write();
hFOccGeom_P3->Write();
hFOccGeom_P4->Write();
hFOccAllCuts_P3->Write();
hFOccAllCuts_P4->Write();
hFEnDist_P3->Write();
hFEnDist_P4->Write();
hFEnDistGeom_P3->Write();
hFEnDistGeom_P4->Write();
hFEnDistAllCuts_P3->Write();
hFEnDistAllCuts_P4->Write();

GG_dir->cd();
hGOcc_P3->Write();
hGOcc_P4->Write();
hGOccGeom_P3->Write();
hGOccGeom_P4->Write();
hGOccAllCuts_P3->Write();
hGOccAllCuts_P4->Write();
hGEnDist_P3->Write();
hGEnDist_P4->Write();
hGEnDistGeom_P3->Write();
hGEnDistGeom_P4->Write();
hGEnDistAllCuts_P3->Write();
hGEnDistAllCuts_P4->Write();

FileOut->Close();
}

int main(int argc, const char* argv[]) {
    if (argc < 7) {
        cerr << "Usage: " << argv[0] << " <energy value> <thetaX> <ThetaY> <input_root_file0> <input_root_file1> <input_root_file2> <output_root_file>" << endl;
        return 1;
    }
    double EBeam = strtod(argv[1], NULL);
    // cout<<setprecision(2)<<EBeam;
    double ThetaX = strtod(argv[2], NULL);
    double ThetaY = strtod(argv[3], NULL);
    const char* inputRootFileName0 = argv[4];
    const char* inputRootFileName1 = argv[5];
    const char* inputRootFileName2 = argv[6];
    const char* outputRootFileName = argv[7];

    // Your C++ logic here to process the input files and generate the output

    cout << "Energy beam: " << EBeam << " ThetaX: " << ThetaX << " ThetaY: " << ThetaY << endl;
    cout << "Processing input files: " << inputRootFileName0 << ", " << inputRootFileName1 << ", " << inputRootFileName2 << endl;
    cout << "Generating output file: " << outputRootFileName << endl;

    // Sample code to read input files and write output
    // std::ifstream inputRootFile(inputRootFileName);
    // std::ifstream inputTxtFile(inputTxtFileName);
    // std::ofstream outputRootFile(outputRootFileName);

    BkgSens(EBeam, ThetaX, ThetaY, inputRootFileName0, inputRootFileName1, inputRootFileName2, outputRootFileName);

    // Close the file streams if you opened any

    return 0;
}



