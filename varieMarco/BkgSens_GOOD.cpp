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
#include <iostream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TVector3.h>
#include <regex>

using namespace std;

void BkgSens(int E0, int EStep, int NStepEBeam, double TargetX, double TargetY, double AngBeamDirX, double AngBeamDirY, const char* OutputName){

double pi2 = TMath::Pi()*2;
static const int NRows = 200000; //number of events generated with CalcHEP -> to be improved to a parameter in a script (?)
double MassEl = 0.000511; //in GeV
TVector3 TargetPosition (0.,0.,-1028.); //read by MC full
TVector3 ECalPosition (0.,0.,2672.); //read by MC full
double TargetCalo = abs(TargetPosition.Z() - ECalPosition.Z()); //distance along beam axis between target and calorimeter
double ShadowCut = 102/abs(TargetPosition.Z()-500); // in Rad
// histogram binning in energy and ECal reso - it depends on the beam as input
double EFin = E0 + NStepEBeam*EStep;
int EBeam = 0.;
double Range = 0.400; 
double Bin = 800;
double CaloNBins = 30;
double OccBins = 450;

//CUTS
double RECal = 300.; //external radius of ECal
double HalfHole = 50.; //maybe not needed
double RMax = 304.; // in mm //Modified
double RMin = 90;
double EMax = 0.200; //deleted because at TOY level seems to be unusefull
double EMin = 0.05;
double MagnetCut = ShadowCut*TargetCalo;
cout<<"Magnet Shadow on ECal = "<<MagnetCut<<" [mm]"<<endl;

// Target and beam multiplicity
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
double SNev_accepted, FNev_accepted, GNev_accepted, SMNev_accepted;

//creating output file with Histos
TFile *FileOut = new TFile(OutputName, "RECREATE");

TH1F *hSAcceptance = new TH1F("hSAcceptance", "hSAcceptance", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
TH1F *hFAcceptance = new TH1F("hFAcceptance", "hFAcceptance", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
TH1F *hGAcceptance = new TH1F("hGAcceptance", "hGAcceptance", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
TH1F *hSMAcceptance = new TH1F("hSMAcceptance", "hSMAcceptance", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
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

TH1F *hSYield = new TH1F("hSYield", "S-Channel YIELD", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
TH1F *hFYield = new TH1F("hFYield", "BhabhaFull YIELD", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
TH1F *hGYield = new TH1F("hGYield", "AA YIELD", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
TH1F *hSMYield = new TH1F("hSMYield", "SM YIELD", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
TH1F *hRatioCl = new TH1F("hRatioCl", "hRatioCl", NStepEBeam, E0-(EStep/2), EFin-(EStep/2));
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
hRatioCl->SetMarkerStyle(20);
hRatioCl->SetMarkerColor(kBlack);
hRatioCl->SetMarkerSize(2);

//CUTS TOMMASO - they are defined here once for type of process
map <double, TVector3> mTargetImpact; // impact point of the beam on target
map <double, TVector3> mBeamDirDisplacement; // displacement due to the beam direction
map <double, double> mSqrtS, mBeamMomentum;
map <double, double> mBG, mBeta, mGamma;
map <double, TVector3> mBoostMom;
map <double, double> mRadius_min, mEnergy_max, mEnergy_min;

for(int iEb=0; iEb<NStepEBeam; iEb++){
    EBeam = E0 + iEb*EStep; 
    // cout<<setprecision(2)<<EBeam;

    mTargetImpact[EBeam].SetXYZ(TargetX, TargetY, TargetPosition.Z()); //impact point of the beam on target 
    mBeamDirDisplacement[EBeam].SetXYZ(TargetX + TargetCalo*tan(AngBeamDirX), TargetY + TargetCalo*tan(AngBeamDirY), ECalPosition.Z());
    mSqrtS[EBeam] = TMath::Sqrt(2.*MassEl*EBeam*1000); // in MeV
    mBeamMomentum[EBeam] = TMath::Sqrt(EBeam*EBeam - 2*MassEl*MassEl*1e6); //in MeV
    mBG[EBeam] = mBeamMomentum[EBeam]/mSqrtS[EBeam]; // mBeta mGamma
    mGamma[EBeam] = TMath::Sqrt(mBG[EBeam]*mBG[EBeam] + 1.);
    mBeta[EBeam] = mBG[EBeam]/mGamma[EBeam];
    mBoostMom[EBeam].SetXYZ(mBeamDirDisplacement[EBeam].X()-mTargetImpact[EBeam].X(), mBeamDirDisplacement[EBeam].Y()-mTargetImpact[EBeam].Y(), mBeamDirDisplacement[EBeam].Z()-mTargetImpact[EBeam].Z());
    mBoostMom[EBeam] *= (mBeta[EBeam]/mBoostMom[EBeam].Mag());

    double tanQMax = RMax/(mBeamDirDisplacement[EBeam].Z()-mTargetImpact[EBeam].Z());
    double tLim = TMath::ATan(mGamma[EBeam]*tanQMax);
    if (tLim < TMath::Pi()*0.25) {
        cout << "No solution? " << tLim << " " << mGamma[EBeam] << endl;
    }

    mRadius_min[EBeam] = (mBeamDirDisplacement[EBeam].Z()-mTargetImpact[EBeam].Z())*TMath::Tan(0.5*TMath::Pi()-tLim)/mGamma[EBeam];// (fmBeamDirDisplacement.Z()-fRTarg.Z())*tanQMin; 
    mEnergy_max[EBeam] = mSqrtS[EBeam]*mGamma[EBeam]*0.5*(1.-TMath::Cos(2*tLim)); 
    mEnergy_min[EBeam] = mSqrtS[EBeam]*mGamma[EBeam]*0.5*(1.+TMath::Cos(2*tLim)); 

    cout << "GeneralInfo: run-level info for run from DYNAMIC CUTS at " << EBeam <<" MeV" << " Pbeam = " << mBeamMomentum[EBeam]<<endl;
    cout << " target = { "<< mTargetImpact[EBeam].X()<< " , "<< mTargetImpact[EBeam].Y() << " , " << mTargetImpact[EBeam].Z() << 
            " }; COG = { " << mBeamDirDisplacement[EBeam].X() << " , " << mBeamDirDisplacement[EBeam].Y() << " , "<< mBeamDirDisplacement[EBeam].Z() << " }" <<endl; 
    cout << " sqrt(s) = " << mSqrtS[EBeam] << " mBG = " << mBG [EBeam]<< " mBeta = " << mBeta[EBeam] << endl;
    cout << " energyRange = { " << mEnergy_min[EBeam] << " , " << mEnergy_max[EBeam] << " }; radiusRange = { " << mRadius_min[EBeam] << " , " << RMax << " }" << endl;

    const int numPoints = 100;
    double xmin[numPoints];
    double ymin[numPoints];
    double xmax[numPoints];
    double ymax[numPoints];
    double xECal[numPoints];
    double yECal[numPoints];
    for (int i = 0; i < numPoints; ++i) {
        double angle = 2 * TMath::Pi() * i / numPoints;
        xmin[i] = mRadius_min[EBeam] * TMath::Cos(angle) + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        ymin[i] = mRadius_min[EBeam] * TMath::Sin(angle) + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        xmax[i] = RMax * TMath::Cos(angle) + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        ymax[i] = RMax * TMath::Sin(angle) + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        xECal[i] = RECal * TMath::Cos(angle);
        yECal[i] = RECal * TMath::Sin(angle);
    }
    // preparing TGraph with Tommaso cuts to be superimpose to the TH2F with events in ECal
    TGraph* graphmin = new TGraph(numPoints, xmin, ymin);
    graphmin->SetTitle(Form("Circle with mRadius_min %.2f", mRadius_min[EBeam]));
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
    TMultiGraph *MGDynCut = new TMultiGraph(Form("MGDynCut_%i",EBeam),Form("MGDynCut_%i",EBeam));
    MGDynCut->Add(graphmin);
    MGDynCut->Add(graphmax);
    MGDynCut->Add(graphECal);
            
    ifstream StxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaSCh/BhabhaSCh_" + to_string(static_cast<int>(EBeam)) +".txt").c_str()); // Apri il file di testo in input
    string Sline;
    ifstream FtxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaFull/BhabhaFull_" + to_string(static_cast<int>(EBeam)) +"CUTTED.txt").c_str()); 
    string Fline;
    ifstream GtxtIn(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProduction/AAProduction_" + to_string(static_cast<int>(EBeam)) +".txt").c_str());
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
    TFile *BhabhaSChIn = TFile::Open(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/BhabhaSCh/BhabhaSCh_" + to_string(static_cast<int>(EBeam)) +".root").c_str()); 
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

    TFile *AAProductionIn = TFile::Open(("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProduction/AAProduction_" + to_string(static_cast<int>(EBeam)) +".root").c_str()); 
    //TFile *AAProductionIn  = new TFile("/home/mancinima/padme-fw/varieMarco/CalcHEPFiles/AAProd/AAprod.root");
    TTree *tGG  = (TTree*)AAProductionIn->Get(("tAAProduction_" + to_string(static_cast<int>(EBeam))).c_str()); 
    double GFinalP1_G1, GFinalP2_G1, GFinalP3_G1, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2; 
    tGG->SetBranchAddress("GFinalP1_G1", &GFinalP1_G1);
    tGG->SetBranchAddress("GFinalP2_G1", &GFinalP2_G1);
    tGG->SetBranchAddress("GFinalP3_G1", &GFinalP3_G1);
    tGG->SetBranchAddress("GFinalP1_G2", &GFinalP1_G2);
    tGG->SetBranchAddress("GFinalP2_G2", &GFinalP2_G2);
    tGG->SetBranchAddress("GFinalP3_G2", &GFinalP3_G2);

    //inizializing histos
    TH2F* hSOcc_pos = new TH2F(("hSOcc_pos_" + to_string(EBeam)).c_str(), ("hSOcc_pos_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hSOcc_ele = new TH2F(("hSOcc_ele_" + to_string(EBeam)).c_str(), ("hSOcc_ele_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hFOcc_pos = new TH2F(("hFOcc_pos_" + to_string(EBeam)).c_str(), ("hFOcc_pos_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hFOcc_ele = new TH2F(("hFOcc_ele_" + to_string(EBeam)).c_str(), ("hFOcc_ele_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hGOcc_G1 = new TH2F(("hGOcc_G1_" + to_string(EBeam)).c_str(), ("hGOcc_G1_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hGOcc_G2 = new TH2F(("hGOcc_G2_" + to_string(EBeam)).c_str(), ("hGOcc_G2_" + to_string(EBeam)).c_str(), OccBins, -450, 450, OccBins, -450, 450);

    TH2F* hSOccGeom_pos = new TH2F(("hSOccGeom_pos_" + to_string(EBeam)).c_str(), ("hSOccGeom_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hSOccGeom_ele = new TH2F(("hSOccGeom_ele_" + to_string(EBeam)).c_str(), ("hSOccGeom_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccGeom_pos = new TH2F(("hFOccGeom_pos_" + to_string(EBeam)).c_str(), ("hFOccGeom_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccGeom_ele = new TH2F(("hFOccGeom_ele_" + to_string(EBeam)).c_str(), ("hFOccGeom_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccGeom_G1 = new TH2F(("hGOccGeom_G1_" + to_string(EBeam)).c_str(), ("hGOccGeom_G1_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccGeom_G2 = new TH2F(("hGOccGeom_G2_" + to_string(EBeam)).c_str(), ("hGOccGeom_G2_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);

    TH2F* hSOccAllCuts_pos = new TH2F(("hSOccAllCuts_pos_" + to_string(EBeam)).c_str(), ("hSOccAllCuts_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hSOccAllCuts_ele = new TH2F(("hSOccAllCuts_ele_" + to_string(EBeam)).c_str(), ("hSOccAllCuts_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccAllCuts_pos = new TH2F(("hFOccAllCuts_pos_" + to_string(EBeam)).c_str(), ("hFOccAllCuts_pos_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hFOccAllCuts_ele = new TH2F(("hFOccAllCuts_ele_" + to_string(EBeam)).c_str(), ("hFOccAllCuts_ele_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccAllCuts_G1 = new TH2F(("hGOccAllCuts_G1_" + to_string(EBeam)).c_str(), ("hGOccAllCuts_G1_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);
    TH2F* hGOccAllCuts_G2 = new TH2F(("hGOccAllCuts_G2_" + to_string(EBeam)).c_str(), ("hGOccAllCuts_G2_" + to_string(EBeam)).c_str(), CaloNBins, -300, 300, CaloNBins, -300, 300);

    double EnBins = EBeam*5;
    TH1F* hSEnDist_pos = new TH1F(("hSEnDist_pos" + to_string(EBeam)).c_str(), ("hSEnDist_pos" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hSEnDist_ele = new TH1F(("hSEnDist_ele" + to_string(EBeam)).c_str(), ("hSEnDist_ele" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hSEnDistGeom_pos = new TH1F(("hSEnDistGeom_pos" + to_string(EBeam)).c_str(), ("hSEnDistGeom_pos" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hSEnDistGeom_ele = new TH1F(("hSEnDistGeom_ele" + to_string(EBeam)).c_str(), ("hSEnDistGeom_ele" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hSEnDistAllCuts_pos = new TH1F(("hSEnDistAllCuts_pos" + to_string(EBeam)).c_str(), ("hSEnDistAllCuts_pos" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hSEnDistAllCuts_ele = new TH1F(("hSEnDistAllCuts_ele" + to_string(EBeam)).c_str(), ("hSEnDistAllCuts_ele" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);

    TH1F* hFEnDist_pos = new TH1F(("hFEnDist_pos" + to_string(EBeam)).c_str(), ("hFEnDist_pos" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hFEnDist_ele = new TH1F(("hFEnDist_ele" + to_string(EBeam)).c_str(), ("hFEnDist_ele" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hFEnDistGeom_pos = new TH1F(("hFEnDistGeom_pos" + to_string(EBeam)).c_str(), ("hFEnDistGeom_pos" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hFEnDistGeom_ele = new TH1F(("hFEnDistGeom_ele" + to_string(EBeam)).c_str(), ("hFEnDistGeom_ele" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hFEnDistAllCuts_pos = new TH1F(("hFEnDistAllCuts_pos" + to_string(EBeam)).c_str(), ("hFEnDistAllCuts_pos" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hFEnDistAllCuts_ele = new TH1F(("hFEnDistAllCuts_ele" + to_string(EBeam)).c_str(), ("hFEnDistAllCuts_ele" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);

    TH1F* hGEnDist_G1 = new TH1F(("hGEnDist_G1" + to_string(EBeam)).c_str(), ("hGEnDist_G1" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hGEnDist_G2 = new TH1F(("hGEnDist_G2" + to_string(EBeam)).c_str(), ("hGEnDist_G2" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hGEnDistGeom_G1 = new TH1F(("hGEnDistGeom_G1" + to_string(EBeam)).c_str(), ("hGEnDistGeom_G1" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hGEnDistGeom_G2 = new TH1F(("hGEnDistGeom_G2" + to_string(EBeam)).c_str(), ("hGEnDistGeom_G2" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hGEnDistAllCuts_G1 = new TH1F(("hGEnDistAllCuts_G1" + to_string(EBeam)).c_str(), ("hGEnDistAllCuts_G1" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);
    TH1F* hGEnDistAllCuts_G2 = new TH1F(("hGEnDistAllCuts_G2" + to_string(EBeam)).c_str(), ("hGEnDistAllCuts_G2" + to_string(EBeam)).c_str(), EnBins, 0, EBeam*1e-3);

    //TVector3 for each of all 4 particles in the process
    TVector3 P1;
    TVector3 P2;
    TVector3 SP3, FP3, GP3;
    TVector3 SP4, FP4, GP4;

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
        double P1_En = TMath::Sqrt(MassEl*MassEl + P1_Ptot*P1_Ptot);
        double P2_En = MassEl;
        double E12_tot = P1_En + P2_En;

        // final state
        // total momentum
        double SP3_Ptot = SP3.Mag();
        double SP4_Ptot = SP4.Mag();
        double FP3_Ptot = FP3.Mag();
        double FP4_Ptot = FP4.Mag();
        double GP3_Ptot = GP3.Mag();
        double GP4_Ptot = GP4.Mag();
        // transverse momentum
        double SP3_transP = SP3.Perp();
        double SP4_transP = SP4.Perp();
        double FP3_transP = FP3.Perp();
        double FP4_transP = FP4.Perp();
        double GP3_transP = GP3.Perp();
        double GP4_transP = GP4.Perp();
        // energy 
        double SP3_En = sqrt(MassEl*MassEl + SP3_Ptot*SP3_Ptot);
        double SP4_En = sqrt(MassEl*MassEl + SP4_Ptot*SP4_Ptot);
        double FP3_En = sqrt(MassEl*MassEl + FP3_Ptot*FP3_Ptot);
        double FP4_En = sqrt(MassEl*MassEl + FP4_Ptot*FP4_Ptot);
        double GP3_En = sqrt(GP3_Ptot*GP3_Ptot);
        double GP4_En = sqrt(GP4_Ptot*GP4_Ptot);
        double FE34_tot = FP3_En + FP4_En;
        double GE34_tot = GP3_En + GP4_En;
        // if(E12_tot-E34_tot>1E-6){
        //     cout<<"No energy conservation!"<<" E12-E34 = "<<(E12_tot-E34_tot)<<endl;
        //     break;
        // }
        // polar angle
        double SP3_PolarTheta = SP3.Theta();
        double SP4_PolarTheta = SP4.Theta();
        double FP3_PolarTheta = FP3.Theta();
        double FP4_PolarTheta = FP4.Theta();
        double GP3_PolarTheta = GP3.Theta();
        double GP4_PolarTheta = GP4.Theta();
        // cos(polar angle)
        double SP3_CosPolar = SP3.CosTheta();
        double SP4_CosPolar = SP4.CosTheta();
        double FP3_CosPolar = FP3.CosTheta();
        double FP4_CosPolar = FP4.CosTheta();
        double GP3_CosPolar = GP3.CosTheta();
        double GP4_CosPolar = GP4.CosTheta();
        // azimuthal angle
        double SP3_Phi = SP3.Phi();
        double SP4_Phi = SP4.Phi();
        double FP3_Phi = FP3.Phi();
        double FP4_Phi = FP4.Phi();
        double GP3_Phi = GP3.Phi();
        double GP4_Phi = GP4.Phi();
        // impact point @ECal
        double SP3_x = (SP3.X()/SP3.Z())*TargetCalo + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        double SP3_y = (SP3.Y()/SP3.Z())*TargetCalo + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        double SP4_x = (SP4.X()/SP4.Z())*TargetCalo + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        double SP4_y = (SP4.Y()/SP4.Z())*TargetCalo + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        double FP3_x = (FP3.X()/FP3.Z())*TargetCalo + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        double FP3_y = (FP3.Y()/FP3.Z())*TargetCalo + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        double FP4_x = (FP4.X()/FP4.Z())*TargetCalo + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        double FP4_y = (FP4.Y()/FP4.Z())*TargetCalo + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        double GP3_x = (GP3.X()/GP3.Z())*TargetCalo + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        double GP3_y = (GP3.Y()/GP3.Z())*TargetCalo + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        double GP4_x = (GP4.X()/GP4.Z())*TargetCalo + mTargetImpact[EBeam].X() + mBeamDirDisplacement[EBeam].X();
        double GP4_y = (GP4.Y()/GP4.Z())*TargetCalo + mTargetImpact[EBeam].Y() + mBeamDirDisplacement[EBeam].Y();
        
        double SP3_R = tan(SP3_PolarTheta)*TargetCalo;
        double SP4_R = tan(SP4_PolarTheta)*TargetCalo;
        double FP3_R = tan(FP3_PolarTheta)*TargetCalo;
        double FP4_R = tan(FP4_PolarTheta)*TargetCalo;
        double GP3_R = tan(GP3_PolarTheta)*TargetCalo;
        double GP4_R = tan(GP4_PolarTheta)*TargetCalo;

        //filling Histo
        hSOcc_pos->Fill(SP3_x, SP3_y);
        hSOcc_ele->Fill(SP4_x, SP4_y);
        hSEnDist_pos->Fill(SP3_En);
        hSEnDist_ele->Fill(SP4_En);
        hFOcc_pos->Fill(FP3_x, FP3_y);
        hFOcc_ele->Fill(FP4_x, FP4_y);
        hFEnDist_pos->Fill(FP3_En);
        hFEnDist_ele->Fill(FP4_En);
        hGOcc_G1->Fill(GP3_x, GP3_y);
        hGOcc_G2->Fill(GP4_x, GP4_y);
        hGEnDist_G1->Fill(GP3_En);
        hGEnDist_G2->Fill(GP4_En);
        //performing geometrical cuts
        if(SP3_R<RMax && SP4_R<RMax && SP3_R>RMin && SP4_R>RMin && abs(SP3_y)<MagnetCut && abs(SP4_y)<MagnetCut){
            hSOccGeom_pos->Fill(SP3_x, SP3_y);
            hSOccGeom_ele->Fill(SP4_x, SP4_y);
            hSEnDistGeom_pos->Fill(SP3_En);
            hSEnDistGeom_ele->Fill(SP4_En);   
            //performing energetical cuts
            if(SP3_En<EMax && SP4_En<EMax && SP3_En>EMin && SP4_En>EMin){
                hSOccAllCuts_pos->Fill(SP3_x, SP3_y);
                hSOccAllCuts_ele->Fill(SP4_x, SP4_y);
                hSEnDistAllCuts_pos->Fill(SP3_En);
                hSEnDistAllCuts_ele->Fill(SP4_En);
            }
        }   
        if(FP3_R<RMax && FP4_R<RMax && FP3_R>RMin && FP4_R>RMin && abs(FP3_y)<MagnetCut && abs(FP4_y)<MagnetCut){
            hFOccGeom_pos->Fill(FP3_x, FP3_y);
            hFOccGeom_ele->Fill(FP4_x, FP4_y);
            hFEnDistGeom_pos->Fill(FP3_En);
            hFEnDistGeom_ele->Fill(FP4_En);   
            //performing energetical cuts
            if(FP3_En<EMax && FP4_En<EMax && FP3_En>EMin && FP4_En>EMin){
                hFOccAllCuts_pos->Fill(FP3_x, FP3_y);
                hFOccAllCuts_ele->Fill(FP4_x, FP4_y);
                hFEnDistAllCuts_pos->Fill(FP3_En);
                hFEnDistAllCuts_ele->Fill(FP4_En);
            }
        }   
        if(GP3_R<RMax && GP4_R<RMax && GP3_R>RMin && GP4_R>RMin && abs(GP3_y)<MagnetCut && abs(GP4_y)<MagnetCut){
            hGOccGeom_G1->Fill(GP3_x, GP3_y);
            hGOccGeom_G2->Fill(GP4_x, GP4_y);
            hGEnDistGeom_G1->Fill(GP3_En);
            hGEnDistGeom_G2->Fill(GP4_En);   
            //performing energetical cuts
            if(GP3_En<EMax && GP4_En<EMax && GP3_En>EMin && GP4_En>EMin){
                hGOccAllCuts_G1->Fill(GP3_x, GP3_y);
                hGOccAllCuts_G2->Fill(GP4_x, GP4_y);
                hGEnDistAllCuts_G1->Fill(GP3_En);
                hGEnDistAllCuts_G2->Fill(GP4_En);
            }
        }   
    }

    SNev_accepted = mSInstantLumi[EBeam]*(hSOccAllCuts_pos->GetEntries() / NRows); //POT ELIMINATI X VALUTARE YIELD
    FNev_accepted = mFInstantLumi[EBeam]*(hFOccAllCuts_pos->GetEntries() / NRows); //POT ELIMINATI X VALUTARE YIELD
    GNev_accepted = mGInstantLumi[EBeam]*(hGOccAllCuts_G1->GetEntries() / NRows); //POT ELIMINATI X VALUTARE YIELD
    SMNev_accepted = FNev_accepted + GNev_accepted; //POT ELIMINATI X VALUTARE YIELD
    hSAcceptance->Fill(EBeam, hSOccAllCuts_pos->GetEntries() / NRows);
    hFAcceptance->Fill(EBeam, hFOccAllCuts_pos->GetEntries() / NRows);
    hGAcceptance->Fill(EBeam, hGOccAllCuts_G1->GetEntries() / NRows);
    // hSMAcceptance->Fill(EBeam, ((hGOccupancyAllCuts_G1->GetEntries() / NRows) + (hFOccupancyAllCuts_pos->GetEntries() / NRows)));
    hSYield->Fill(EBeam, SNev_accepted);
    hFYield->Fill(EBeam, FNev_accepted);
    hGYield->Fill(EBeam, GNev_accepted);
    hSMYield->Fill(EBeam, SMNev_accepted);
    hRatioCl->Fill(EBeam, FNev_accepted/GNev_accepted);
        
    FileOut->cd();
    MGDynCut->Write();
    SCh_dir->cd();
    hSOcc_pos->Write();
    hSOcc_ele->Write();
    hSEnDist_pos->Write();
    hSEnDist_ele->Write();
    hSOccGeom_pos->Write();
    hSOccGeom_ele->Write();
    hSEnDistGeom_pos->Write();
    hSEnDistGeom_ele->Write();
    hSOccAllCuts_pos->Write();
    hSOccAllCuts_ele->Write();
    hSEnDistAllCuts_pos->Write();
    hSEnDistAllCuts_ele->Write();
    
    Full_dir->cd();
    hFOcc_pos->Write();
    hFOcc_ele->Write();
    hFEnDist_pos->Write();
    hFEnDist_ele->Write();
    hFOccGeom_pos->Write();
    hFOccGeom_ele->Write();
    hFEnDistGeom_pos->Write();
    hFEnDistGeom_ele->Write();
    hFOccAllCuts_pos->Write();
    hFOccAllCuts_ele->Write();
    hFEnDistAllCuts_pos->Write();
    hFEnDistAllCuts_ele->Write();
    
    GG_dir->cd();
    hGOcc_G1->Write();
    hGOcc_G2->Write();
    hGEnDist_G1->Write();
    hGEnDist_G2->Write();
    hGOccGeom_G1->Write();
    hGOccGeom_G2->Write();
    hGEnDistGeom_G1->Write();
    hGEnDistGeom_G2->Write();
    hGOccAllCuts_G1->Write();
    hGOccAllCuts_G2->Write();
    hGEnDistAllCuts_G1->Write();
    hGEnDistAllCuts_G2->Write();
}

TGraph *gSYield = new TGraph(hSYield);
gSYield->SetTitle("gSYield; E_{Beam} [MeV]; N_{s-ch}(e^{+}e^{-})/N^{PoT}");
TGraph *gFYield = new TGraph(hFYield);
gFYield->SetTitle("gFYield; E_{Beam} [MeV]; N_{full}(e^{+}e^{-})/N^{PoT}");
TGraph *gGYield = new TGraph(hGYield);
gGYield->SetTitle("gGYield; E_{Beam} [MeV]; N(#gamma#gamma)/N^{PoT}");
TGraph *gSMYield = new TGraph(hSMYield);
gSMYield->SetTitle("gSMYield; E_{Beam} [MeV]; N_{bkg}(2CL)/N^{PoT}");
TGraph *gRatioCl = new TGraph(hRatioCl);
gRatioCl->SetTitle("gRatioCl; E_{Beam} [MeV]; N_{full}(e^{+}e^{-})/N(#gamma#gamma)");
// gSYield->Draw();
// gFYield->Draw();
// gGYield->Draw();
// gSMYield->Draw();

FileOut->cd();
hSAcceptance->Write();
hFAcceptance->Write();
hGAcceptance->Write();
// hSMAcceptance->Write();
// hSYield->Write();
// hFYield->Write();
// hGYield->Write();
gSYield->Write();
gFYield->Write();
gGYield->Write();
gSMYield->Write();
gRatioCl->Write();

FileOut->Close();
}

