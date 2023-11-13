
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

using namespace std;

class Particle{
private:
    double mass_;
    double px_;
    double py_;
    double pz_;

public:
    Particle(double mass, double px, double py, double pz)
        : mass_(mass), px_(px), py_(py), pz_(pz) {}

    // Calcola l'impulso trasverso (transverse momentum) della particella
    double TransverseMomentum() const {
        return sqrt(px_ * px_ + py_ * py_);
    }
    
    //Calcola il modulo del trimomento
    double TriMomentum() const {
        return sqrt(px_ * px_ + py_ * py_ + pz_*pz_);
    }

    // Calcola l'energia totale della particella
    double Energy() const {
        return sqrt(mass_*mass_ + TriMomentum()*TriMomentum());
    }

    // Calcola l'angolo di scattering della particella
    double ScatteringAngle() const {
        return atan2(TransverseMomentum(), pz_);
    }

    // Calcola l'angolo di scattering della particella nel CoM frame
    double CoMScatteringAngle(const Particle& targetParticle) const {
        double ptDiff = TransverseMomentum() - targetParticle.TransverseMomentum();
        double pzDiff = pz_ - targetParticle.pz_;

        return atan2(ptDiff, pzDiff);
    }

    //Calcola l'angolo tra l'impulso x e z
    double PhiX() const {
        return atan2(px_, pz_);
    }

    //Calcola l'angolo tra l'impulso y e z
    double PhiY() const {
        return atan2(px_, pz_);
    }

    // Calcola il raggio ad una distanza Delta dal bersaglio in mm
    double TransverseRadius(double DeltaZ) const {
        return atan2(TransverseMomentum(), pz_)*DeltaZ;
    }

    //Calcola la coordinata x ad una distanza Delta dal bersaglio in mm
    double R_x(double DeltaZ) const{
        return PhiX()*DeltaZ;
    }

    //Calcola la coordinata y ad una distanza Delta dal bersaglio in mm
    double R_y(double DeltaZ) const{
        return PhiY()*DeltaZ;
    }
};

class EventAnalyzer {
public:
    EventAnalyzer(TH2F* h1, TH2F* h2, TH2F* h3, TH2F* h4, TH2F* h5, TH2F* h6): 
                    h1_(h1), h2_(h2), h3_(h3), h4_(h4), h5_(h5), h6_(h6) {}

    ~EventAnalyzer() {}

    void AnalyzeEvents(const double Radius_max, const double En_min, double Radius_min,
                        const double Radius_1, const double Radius_2,
                        const double x_1, const double y_1,
                        const double x_2, const double y_2,
                        const double FinEnergy_1, const double FinEnergy_2,
                        double Shadow) {            
            // Analisi eventi con tutti i tagli                        
            if (AllCutsPassed(Radius_min, Radius_max, En_min, Shadow)) {
                FillHistograms();
            }
    }

    void GeometricCutsOnly(const double Radius_min, double Radius_max, double En_min, double Shadow) {
        // Analisi degli eventi con solo i tagli geometrici (radiuscut e shadowcut)
        if (GeometricCutsPassed(Radius_min, Radius_max, Shadow)) {
            FillHistograms();
        }
    }

    void NoCuts() {
        // Analisi degli eventi senza nessun taglio
        FillHistograms();
    }

private:
    bool AllCutsPassed(double Radius_min, double Radius_max, double En_min, double Shadow) {
        bool radiusCut = (Radius_1 > Radius_min && Radius_2 > Radius_min &&
                            Radius_1 < Radius_max && Radius_2 < Radius_max);
        bool energyCut = (FinEnergy_1 > En_min && FinEnergy_2 > En_min);
        bool shadowCut = (abs(y_1) < Shadow && abs(y_2) < Shadow);
        return (radiusCut && energyCut && shadowCut);
    }

    bool GeometricCutsPassed(double Radius_min, double Radius_max, double Shadow) {
        bool radiusCut = (Radius_1 > Radius_min && Radius_2 > Radius_min &&
                            Radius_1 < Radius_max && Radius_2 < Radius_max);
        bool shadowCut = (abs(y_1) < Shadow && abs(y_2) < Shadow);
        return (radiusCut && shadowCut);
    }

    void FillHistograms() {
        h1_->Fill(x_1, y_1);
        h2_->Fill(x_2, y_2);
        h3_->Fill(x_1, y_1);
        h4_->Fill(x_2, y_2);
        h5_->Fill(x_1, y_1);
        h6_->Fill(x_2, y_2);
    }

    private:
    TH2F* h1_;
    TH2F* h2_;
    TH2F* h3_;
    TH2F* h4_;
    TH2F* h5_;
    TH2F* h6_;
    double Radius_1;
    double Radius_2;
    double x_1;
    double y_1;
    double x_2;
    double y_2;
    double FinEnergy_1;
    double FinEnergy_2;
};

void PADME_sensitivity(){

double pi2 = TMath::Pi()*2;
static const int Nev_CalcHep = 10000;
double numCol    = 11;
double numRow = 10000;
double ElectronMass = 0.000511;
double TargetCalo = 3720; //in mm 20230213 DeltaZ target-ECal  
//double semiL = 50; //lato mezzi del quadrato centrale
//double geometry_radius = 300; //raggio geometrico del calorimetro

double CaloRes = 120;
double res_occ = 450;

double En_min = 70; // fissata energia del secondo valore a 0 MeV 
double Radius_min = 90.;
double MagnetShadow = 256.;
double Radius_max = 270.;

// da capire come scrivere queste cose meglio
double PoT[20] = {1e10, 2e10, 3e10, 4e10, 5e10, 6e10, 7e10, 8e10, 9e10, 10e10, 11e10, 12e10, 13e10, 14e10, 15e10, 16e10, 17e10, 18e10, 19e10, 2e11};
double Na  = 6.022e23;
double Z_C = 6;
double D_C = 3.5;
double A_C = 12;
double TThickness = 0.01; //in cm
double CmtoPb = 1e-36;

double PreFactor_X17 = 3.8e-7; //DATA FROM PROCEEDINGS DARME-NARDI
double g_ve[10]      = {1e-4, 2e-4, 3e-4, 4e-4, 5e-4, 6e-4, 7e-4, 8e-4, 9e-4, 10e-4} ;

TString process[4] = {"T", "S", "F", "G"}; //S=Schannel, F=BhabhaFull, G=GammaGamma

map<double, TH1F*> mhInitialP3_pos, mhInitialP3_ele;
map<double, TH1F*> mhSFinalP1_ele, mhSFinalP2_ele, mhSFinalP3_ele, mhSFinalP1_pos, mhSFinalP2_pos, mhSFinalP3_pos;
map<double, TH1F*> mhFFinalP1_ele, mhFFinalP2_ele, mhFFinalP3_ele, mhFFinalP1_pos, mhFFinalP2_pos, mhFFinalP3_pos;
map<double, TH1F*> mhGFinalP1_G1, mhGFinalP2_G1, mhGFinalP3_G1, mhGFinalP1_G2, mhGFinalP2_G2, mhGFinalP3_G2;

//introduzione delle mappe di istogrammi per ciscun punto dello scan in energia
map<double, TH1F*> mhInEnergy_pos, mhInEnergy_ele;
map<double, TH1F*> mhSFinMomentum_pos, mhSFinMomentum_ele, mhFFinMomentum_pos, mhFFinMomentum_ele, mhGFinMomentum_G1, mhGFinMomentum_G2;
map<double, TH1F*> mhSFinEnergy_pos, mhSFinEnergy_ele, mhFFinEnergy_pos, mhFFinEnergy_ele, mhGFinEnergy_G1, mhGFinEnergy_G2;
map<double, TH1F*> mhSEn_tot, mhFullEn_tot, mhGGEn_tot;
map<double, TH1F*> mhSTheta_Scattering_COM, mhFTheta_Scattering_COM, mhGTheta_Scattering_COM;
map<double, TH1F*> mhSPosTheta_Scattering, mhSEleTheta_Scattering, mhFPosTheta_Scattering, mhFEleTheta_Scattering, mhGG1Theta_Scattering, mhGG2Theta_Scattering;
map<double, TH1F*> mhSPhiX_pos, mhSPhiY_pos, mhSPhiX_ele, mhSPhiY_ele;
map<double, TH1F*> mhFPhiX_pos, mhFPhiY_pos, mhFPhiX_ele, mhFPhiY_ele;
map<double, TH1F*> mhGPhiX_G1, mhGPhiY_G1, mhGPhiX_G2, mhGPhiY_G2;
map<double, TH1F*> mhSx_pos, mhSy_pos, mhSx_ele, mhSy_ele;
map<double, TH1F*> mhFx_pos, mhFy_pos, mhFx_ele, mhFy_ele;
map<double, TH1F*> mhGx_G1, mhGy_G1, mhGx_G2, mhGy_G2;
map<double, TH1F*> mhSRadius_pos, mhSRadius_ele, mhFRadius_pos, mhFRadius_ele, mhGRadius_G1, mhGRadius_G2;

map<double, TH2F*> mhSOccupancy_pos, mhSOccupancy_ele, mhFOccupancy_pos, mhFOccupancy_ele, mhGOccupancy_G1, mhGOccupancy_G2;
map<double, TH2F*> mhSOccupancyGeom_pos, mhSOccupancyGeom_ele, mhFOccupancyGeom_pos, mhFOccupancyGeom_ele, mhGOccupancyGeom_G1, mhGOccupancyGeom_G2;
map<double, TH2F*> mhSOccupancyAllCuts_pos, mhSOccupancyAllCuts_ele, mhFOccupancyAllCuts_pos, mhFOccupancyAllCuts_ele, mhGOccupancyAllCuts_G1, mhGOccupancyAllCuts_G2;
map<double, double> mSAcceptance, mFAcceptance, mGAcceptance;
// map<double, TH1F*> mhSAcceptance, mhFAcceptance, mhGAcceptance;

// map<double, TH1F*> mhSRadDistribution_pos, mhSRadDistribution_ele, mhFRadDistribution_pos, mhFRadDistribution_ele, mhGRadDistribution_G1, mhGRadDistribution_G2;

//FARE UNA ROUTINE CLASSE QUALCOSA PER CALCOLARE LUMINOSITà DIPENDENTE DALL'EVENTO SIMULATO
//to be checked with new CalcHep simulations 20230214
map<double, double> mSCrossSection, mFCrossSection, mGCrossSection;
// double Sigma_Full = 5.115879e11; //pb
// double Sigma_S    = 3.014359e8;  //pb
// double Sigma_AA   = 2.733573e9;  //pb

// double Luminosity_C;
// double N_Full_C[20], N_Schannel_C[20], N_Tchannel_C[20], N_AA_C[20], N_X17_C[20][10];

// Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

// for(int k = 0; k<20; k++){
// N_Full_C[k]     = Luminosity_C * Sigma_Full * PoT[k];
// N_Schannel_C[k] = Luminosity_C * Sigma_S * PoT[k]; 
// N_AA_C[k]       = Luminosity_C * Sigma_AA * PoT[k];

// for(int i = 0; i<10; i++){
//     N_X17_C[k][i] = PoT[k] * PreFactor_X17 * ((g_ve[i]*g_ve[i])/(9e-8)) * (1/0.7) * ((Na * Z_C * D_C)/(A_C * 1e24)); //EQUATION 13 Proceedings LUC  <- il primo indice sono i POT, il secondo il valore del COUPLING g_ve
// }
// }

double E0 = 0.282;
int NEbeam = 1;
double EBeam = 0.;
double Range = 0.400;
double Bin = 1600;

TFile *FileOut = new TFile("PADME_Sens_Output.root", "RECREATE");
TDirectory *SCh_dir = FileOut->mkdir("Sch_dir");
TDirectory *Full_dir = FileOut->mkdir("Full_dir");
TDirectory *GG_dir = FileOut->mkdir("GG_dir");


TH1F *hSAcceptance = new TH1F("hSAcceptance", "hSAcceptance", 100, 0, 1);
TH1F *hFAcceptance = new TH1F("hFAcceptance", "hFAcceptance", 100, 0, 1);
TH1F *hGAcceptance = new TH1F("hGAcceptance", "hGAcceptance", 100, 0, 1)


for(int iEb=0; iEb<NEbeam; iEb++){
    EBeam = E0 + iEb*0.005;
    // TFile *BhabhaSChIn    = TFile::Open(("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaSCh_" + to_string(EBeam) +".root").c_str()); 
    TFile *BhabhaSChIn  = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaSCh.root");
    TTree *tSch  = (TTree*)BhabhaSChIn->Get("tBhabhaSCh"); 
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
    mhInitialP3_pos[EBeam] = hInitialP3_pos;
    TH1F* hInitialP3_ele = new TH1F(("hInitialP3_ele_" + to_string(EBeam)).c_str(), ("hInitialP3_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhInitialP3_ele[EBeam] = hInitialP3_ele;
    TH1F* hSFinalP1_ele = new TH1F(("hSFinalP1_ele_" + to_string(EBeam)).c_str(), ("hSFinalP1_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinalP1_ele[EBeam] = hSFinalP1_ele;
    TH1F* hSFinalP2_ele = new TH1F(("hSFinalP2_ele_" + to_string(EBeam)).c_str(), ("hSFinalP2_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinalP2_ele[EBeam] = hSFinalP2_ele;
    TH1F* hSFinalP3_ele = new TH1F(("hSFinalP3_ele_" + to_string(EBeam)).c_str(), ("hSFinalP3_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinalP3_ele[EBeam] = hSFinalP3_ele;
    TH1F* hSFinalP1_pos = new TH1F(("hSFinalP1_pos_" + to_string(EBeam)).c_str(), ("hSFinalP1_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinalP1_pos[EBeam] = hSFinalP1_pos;
    TH1F* hSFinalP2_pos = new TH1F(("hSFinalP2_pos_" + to_string(EBeam)).c_str(), ("hSFinalP2_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinalP2_pos[EBeam] = hSFinalP2_pos;
    TH1F* hSFinalP3_pos = new TH1F(("hSFinalP3_pos_" + to_string(EBeam)).c_str(), ("hSFinalP3_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinalP3_pos[EBeam] = hSFinalP3_pos;
    
    // TFile *BhabhaFullIn = TFile::Open(("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaFull_" + to_string(EBeam) +".root").c_str()); 
    TFile *BhabhaFullIn  = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/BhabhaFull.root");
    TTree *tFull  = (TTree*)BhabhaFullIn->Get("tBhabhaFull"); 
    double FFinalP1_ele, FFinalP2_ele, FFinalP3_ele, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos; 
    tFull->SetBranchAddress("FullFinalP1_electron", &FFinalP1_ele);
    tFull->SetBranchAddress("FullFinalP2_electron", &FFinalP2_ele);
    tFull->SetBranchAddress("FullFinalP3_electron", &FFinalP3_ele);
    tFull->SetBranchAddress("FullFinalP1_positron", &FFinalP1_pos);
    tFull->SetBranchAddress("FullFinalP2_positron", &FFinalP2_pos);
    tFull->SetBranchAddress("FullFinalP3_positron", &FFinalP3_pos);

    TH1F* hFFinalP1_ele = new TH1F(("hFFinalP1_ele_" + to_string(EBeam)).c_str(), ("hFFinalP1_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinalP1_ele[EBeam] = hFFinalP1_ele;
    TH1F* hFFinalP2_ele = new TH1F(("hFFinalP2_ele_" + to_string(EBeam)).c_str(), ("hFFinalP2_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinalP2_ele[EBeam] = hFFinalP2_ele;
    TH1F* hFFinalP3_ele = new TH1F(("hFFinalP3_ele_" + to_string(EBeam)).c_str(), ("hFFinalP3_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinalP3_ele[EBeam] = hFFinalP3_ele;
    TH1F* hFFinalP1_pos = new TH1F(("hFFinalP1_pos_" + to_string(EBeam)).c_str(), ("hFFinalP1_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinalP1_pos[EBeam] = hFFinalP1_pos;
    TH1F* hFFinalP2_pos = new TH1F(("hFFinalP2_pos_" + to_string(EBeam)).c_str(), ("hFFinalP2_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinalP2_pos[EBeam] = hFFinalP2_pos;
    TH1F* hFFinalP3_pos = new TH1F(("hFFinalP3_pos_" + to_string(EBeam)).c_str(), ("hFFinalP3_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinalP3_pos[EBeam] = hFFinalP3_pos;

    // TFile *AAProduction = TFile::Open(("/home/marco/analysis/SM_analysis/CalcHepFiles/AAprod_" + to_string(EBeam) +".root").c_str()); 
    TFile *AAProductionIn  = new TFile("/home/marco/analysis/SM_analysis/CalcHepFiles/AAprod.root");
    TTree *tGG  = (TTree*)AAProductionIn->Get("tAAprod"); 
    double GFinalP1_G1, GFinalP2_G1, GFinalP3_G1, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2; 
    tGG->SetBranchAddress("FinalP1_G1", &GFinalP1_G1);
    tGG->SetBranchAddress("FinalP2_G1", &GFinalP2_G1);
    tGG->SetBranchAddress("FinalP3_G1", &GFinalP3_G1);
    tGG->SetBranchAddress("FinalP1_G2", &GFinalP1_G2);
    tGG->SetBranchAddress("FinalP2_G2", &GFinalP2_G2);
    tGG->SetBranchAddress("FinalP3_G2", &GFinalP3_G2);

    TH1F* hGFinalP1_G1 = new TH1F(("hGFinalP1_G1_" + to_string(EBeam)).c_str(), ("hGFinalP1_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinalP1_G1[EBeam] = hGFinalP1_G1;
    TH1F* hGFinalP2_G1 = new TH1F(("hGFinalP2_G1_" + to_string(EBeam)).c_str(), ("hGFinalP2_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinalP2_G1[EBeam] = hGFinalP2_G1;
    TH1F* hGFinalP3_G1 = new TH1F(("hGFinalP3_G1_" + to_string(EBeam)).c_str(), ("hGFinalP3_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinalP3_G1[EBeam] = hGFinalP3_G1;
    TH1F* hGFinalP1_G2 = new TH1F(("hGFinalP1_G2_" + to_string(EBeam)).c_str(), ("hGFinalP1_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinalP1_G2[EBeam] = hGFinalP1_G2;
    TH1F* hGFinalP2_G2 = new TH1F(("hGFinalP2_G2_" + to_string(EBeam)).c_str(), ("hGFinalP2_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinalP2_G2[EBeam] = hGFinalP2_G2;
    TH1F* hGFinalP3_G2 = new TH1F(("hGFinalP3_G2_" + to_string(EBeam)).c_str(), ("hGFinalP3_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinalP3_G2[EBeam] = hGFinalP3_G2;

    TH1F* hInEnergy_pos = new TH1F(("hInEnergy_pos_" + to_string(EBeam)).c_str(), ("hInEnergy_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhInEnergy_pos[EBeam] = hInEnergy_pos;
    TH1F* hInEnergy_ele = new TH1F(("hInEnergy_ele_" + to_string(EBeam)).c_str(), ("hInEnergy_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhInEnergy_ele[EBeam] = hInEnergy_ele;

    TH1F* hSFinMomentum_pos = new TH1F(("hSFinMomentum_pos_" + to_string(EBeam)).c_str(), ("hSFinMomentum_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinMomentum_pos[EBeam] = hSFinMomentum_pos;
    TH1F* hSFinMomentum_ele = new TH1F(("hSFinMomentum_ele_" + to_string(EBeam)).c_str(), ("hSFinMomentum_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinMomentum_ele[EBeam] = hSFinMomentum_ele;
    TH1F* hFFinMomentum_pos = new TH1F(("hFFinMomentum_pos_" + to_string(EBeam)).c_str(), ("hFFinMomentum_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinMomentum_pos[EBeam] = hFFinMomentum_pos;
    TH1F* hFFinMomentum_ele = new TH1F(("hFFinMomentum_ele_" + to_string(EBeam)).c_str(), ("hSFinMomentum_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinMomentum_ele[EBeam] = hFFinMomentum_ele;
    TH1F* hGFinMomentum_G1 = new TH1F(("hGFinMomentum_G1_" + to_string(EBeam)).c_str(), ("hGFinMomentum_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinMomentum_G1[EBeam] = hGFinMomentum_G1;
    TH1F* hGFinMomentum_G2 = new TH1F(("hGFinMomentum_G2_" + to_string(EBeam)).c_str(), ("hGFinMomentum_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinMomentum_G2[EBeam] = hGFinMomentum_G2;
    
    TH1F* hSFinEnergy_pos = new TH1F(("hSFinEnergy_pos_" + to_string(EBeam)).c_str(), ("hSFinEnergy_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinEnergy_pos[EBeam] = hSFinEnergy_pos;
    TH1F* hSFinEnergy_ele = new TH1F(("hSFinEnergy_ele_" + to_string(EBeam)).c_str(), ("hSFinEnergy_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSFinEnergy_ele[EBeam] = hSFinEnergy_ele;
    TH1F* hFFinEnergy_pos = new TH1F(("hFFinEnergy_pos_" + to_string(EBeam)).c_str(), ("hFFinEnergy_pos_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinEnergy_pos[EBeam] = hFFinEnergy_pos;
    TH1F* hFFinEnergy_ele = new TH1F(("hFFinEnergy_ele_" + to_string(EBeam)).c_str(), ("hFFinEnergy_ele_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFFinEnergy_ele[EBeam] = hFFinEnergy_ele;
    TH1F* hGFinEnergy_G1 = new TH1F(("hGFinEnergy_G1_" + to_string(EBeam)).c_str(), ("hGFinEnergy_G1_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinEnergy_G1[EBeam] = hGFinEnergy_G1;
    TH1F* hGFinEnergy_G2 = new TH1F(("hGFinEnergy_G2_" + to_string(EBeam)).c_str(), ("hGFinEnergy_G2_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGFinEnergy_G2[EBeam] = hGFinEnergy_G2;

    TH1F* hSEn_tot = new TH1F(("hSEn_tot_" + to_string(EBeam)).c_str(), ("hSEn_tot_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhSEn_tot[EBeam] = hSEn_tot;
    TH1F* hFullEn_tot = new TH1F(("hFullEn_tot_" + to_string(EBeam)).c_str(), ("hFullEn_tot_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhFullEn_tot[EBeam] = hFullEn_tot;
    TH1F* hGGEn_tot = new TH1F(("hGGEn_tot_" + to_string(EBeam)).c_str(), ("hGGEn_tot_" + to_string(EBeam)).c_str(), Bin, -Range, Range);
    mhGGEn_tot[EBeam] = hGGEn_tot;

    TH1F* hSTheta_Scattering_COM = new TH1F(("hSTheta_Scattering_COM_" + to_string(EBeam)).c_str(), ("hSTheta_Scattering_COM_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhSTheta_Scattering_COM[EBeam] = hSTheta_Scattering_COM;
    TH1F* hFTheta_Scattering_COM = new TH1F(("hFTheta_Scattering_COM_" + to_string(EBeam)).c_str(), ("hFTheta_Scattering_COM_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhFTheta_Scattering_COM[EBeam] = hFTheta_Scattering_COM;
    TH1F* hGTheta_Scattering_COM = new TH1F(("hGTheta_Scattering_COM_" + to_string(EBeam)).c_str(), ("hGTheta_Scattering_COM_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhGTheta_Scattering_COM[EBeam] = hGTheta_Scattering_COM;

    TH1F* hSPosTheta_Scattering = new TH1F(("hSPosTheta_Scattering_" + to_string(EBeam)).c_str(), ("hSPosTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhSPosTheta_Scattering[EBeam] = hSPosTheta_Scattering;
    TH1F* hSEleTheta_Scattering = new TH1F(("hSEleTheta_Scattering_" + to_string(EBeam)).c_str(), ("hSEleTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhSEleTheta_Scattering[EBeam] = hSEleTheta_Scattering;
    TH1F* hFPosTheta_Scattering = new TH1F(("hFPosTheta_Scattering_" + to_string(EBeam)).c_str(), ("hFPosTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhFPosTheta_Scattering[EBeam] = hFPosTheta_Scattering;
    TH1F* hFEleTheta_Scattering = new TH1F(("hFEleTheta_Scattering_" + to_string(EBeam)).c_str(), ("hFEleTheta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhFEleTheta_Scattering[EBeam] = hFEleTheta_Scattering;
    TH1F* hGG1Theta_Scattering = new TH1F(("hGG1Theta_Scattering_" + to_string(EBeam)).c_str(), ("hGG1Theta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhGG1Theta_Scattering[EBeam] = hGG1Theta_Scattering;
    TH1F* hGG2Theta_Scattering = new TH1F(("hGG2Theta_Scattering_" + to_string(EBeam)).c_str(), ("hGG2Theta_Scattering_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhGG2Theta_Scattering[EBeam] = hGG2Theta_Scattering;

    TH1F* hSPhiX_pos = new TH1F(("hSPhiX_pos_" + to_string(EBeam)).c_str(), ("hSPhiX_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhSPhiX_pos[EBeam] = hSPhiX_pos;
    TH1F* hSPhiY_pos = new TH1F(("hSPhiY_pos_" + to_string(EBeam)).c_str(), ("hSPhiY_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhSPhiY_pos[EBeam] = hSPhiY_pos;
    TH1F* hSPhiX_ele = new TH1F(("hSPhiX_ele_" + to_string(EBeam)).c_str(), ("hSPhiX_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhSPhiX_ele[EBeam] = hSPhiX_ele;
    TH1F* hSPhiY_ele = new TH1F(("hSPhiY_ele_" + to_string(EBeam)).c_str(), ("hSPhiY_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhSPhiY_ele[EBeam] = hSPhiY_ele;

    TH1F* hFPhiX_pos = new TH1F(("hFPhiX_pos_" + to_string(EBeam)).c_str(), ("hFPhiX_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhFPhiX_pos[EBeam] = hFPhiX_pos;
    TH1F* hFPhiY_pos = new TH1F(("hFPhiY_pos_" + to_string(EBeam)).c_str(), ("hFPhiY_pos_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhFPhiY_pos[EBeam] = hFPhiY_pos;
    TH1F* hFPhiX_ele = new TH1F(("hFPhiX_ele_" + to_string(EBeam)).c_str(), ("hFPhiX_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhFPhiX_ele[EBeam] = hFPhiX_ele;
    TH1F* hFPhiY_ele = new TH1F(("hFPhiY_ele_" + to_string(EBeam)).c_str(), ("hFPhiY_ele_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhFPhiY_ele[EBeam] = hFPhiY_ele;

    TH1F* hGPhiX_G1 = new TH1F(("hGPhiX_G1_" + to_string(EBeam)).c_str(), ("hGPhiX_G1_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhGPhiX_G1[EBeam] = hGPhiX_G1;
    TH1F* hGPhiY_G1 = new TH1F(("hGPhiY_G1_" + to_string(EBeam)).c_str(), ("hGPhiY_G1_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhGPhiY_G1[EBeam] = hGPhiY_G1;
    TH1F* hGPhiX_G2 = new TH1F(("hGPhiX_G2_" + to_string(EBeam)).c_str(), ("hGPhiX_G2_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhGPhiX_G2[EBeam] = hGPhiX_G2;
    TH1F* hGPhiY_G2 = new TH1F(("hGPhiY_G2_" + to_string(EBeam)).c_str(), ("hGPhiY_G2_" + to_string(EBeam)).c_str(), 256*pi2, -pi2/4, pi2/4);
    mhGPhiY_G2[EBeam] = hGPhiY_G2;

    TH1F* hSx_pos = new TH1F(("hSx_pos_" + to_string(EBeam)).c_str(), ("hSx_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhSx_pos[EBeam] = hSx_pos;
    TH1F* hSy_pos = new TH1F(("hSy_pos_" + to_string(EBeam)).c_str(), ("hSy_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhSy_pos[EBeam] = hSy_pos;
    TH1F* hSx_ele = new TH1F(("hSx_ele_" + to_string(EBeam)).c_str(), ("hSx_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhSx_ele[EBeam] = hSx_ele;
    TH1F* hSy_ele = new TH1F(("hSy_ele_" + to_string(EBeam)).c_str(), ("hSy_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhSy_ele[EBeam] = hSy_ele;

    TH1F* hFx_pos = new TH1F(("hFx_pos_" + to_string(EBeam)).c_str(), ("hFx_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhFx_pos[EBeam] = hFx_pos;
    TH1F* hFy_pos = new TH1F(("hFy_pos_" + to_string(EBeam)).c_str(), ("hFy_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhFy_pos[EBeam] = hFy_pos;
    TH1F* hFx_ele = new TH1F(("hFx_ele_" + to_string(EBeam)).c_str(), ("hFx_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhFx_ele[EBeam] = hFx_ele;
    TH1F* hFy_ele = new TH1F(("hFy_ele_" + to_string(EBeam)).c_str(), ("hFy_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhFy_ele[EBeam] = hFy_ele;

    TH1F* hGx_G1 = new TH1F(("hGx_G1_" + to_string(EBeam)).c_str(), ("hGx_G1_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhGx_G1[EBeam] = hGx_G1;
    TH1F* hGy_G1 = new TH1F(("hGy_G1_" + to_string(EBeam)).c_str(), ("hGy_G1_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhGy_G1[EBeam] = hGy_G1;
    TH1F* hGx_G2 = new TH1F(("hGx_G2_" + to_string(EBeam)).c_str(), ("hGx_G2_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhGx_G2[EBeam] = hGx_G2;
    TH1F* hGy_G2 = new TH1F(("hGy_G2_" + to_string(EBeam)).c_str(), ("hGy_G2_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhGy_G2[EBeam] = hGy_G2;

    TH1F* hSRadius_pos = new TH1F(("hSRadius_pos_" + to_string(EBeam)).c_str(), ("hSRadius_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhSRadius_pos[EBeam] = hSRadius_pos;
    TH1F* hSRadius_ele = new TH1F(("hSRadius_ele_" + to_string(EBeam)).c_str(), ("hSRadius_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhSRadius_ele[EBeam] = hSRadius_ele;
    TH1F* hFRadius_pos = new TH1F(("hFRadius_pos_" + to_string(EBeam)).c_str(), ("hFRadius_pos_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhFRadius_pos[EBeam] = hFRadius_pos;
    TH1F* hFRadius_ele = new TH1F(("hFRadius_ele_" + to_string(EBeam)).c_str(), ("hFRadius_ele_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhFRadius_ele[EBeam] = hFRadius_ele;
    TH1F* hGRadius_G1 = new TH1F(("hGRadius_G1_" + to_string(EBeam)).c_str(), ("hGRadius_G1_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhGRadius_G1[EBeam] = hGRadius_G1;
    TH1F* hGRadius_G2 = new TH1F(("hGRadius_G2_" + to_string(EBeam)).c_str(), ("hGRadius_G2_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    mhGRadius_G2[EBeam] = hGRadius_G2;

    //histos per lo studio di sensitività e accettanze
    TH2F* hSOccupancy_pos = new TH2F(("hSOccupancy_pos_" + to_string(EBeam)).c_str(), ("hSOccupancy_pos_" + to_string(EBeam)).c_str(), res_occ, -450, 450, res_occ, -450, 450);
    mhSOccupancy_pos[EBeam] = hSOccupancy_pos;
    TH2F* hSOccupancy_ele = new TH2F(("hSOccupancy_ele_" + to_string(EBeam)).c_str(), ("hSOccupancy_ele_" + to_string(EBeam)).c_str(), res_occ, -450, 450, res_occ, -450, 450);
    mhSOccupancy_ele[EBeam] = hSOccupancy_ele;
    TH2F* hFOccupancy_pos = new TH2F(("hFOccupancy_pos_" + to_string(EBeam)).c_str(), ("hFOccupancy_pos_" + to_string(EBeam)).c_str(), res_occ, -450, 450, res_occ, -450, 450);
    mhFOccupancy_pos[EBeam] = hFOccupancy_pos;
    TH2F* hFOccupancy_ele = new TH2F(("hFOccupancy_ele_" + to_string(EBeam)).c_str(), ("hFOccupancy_ele_" + to_string(EBeam)).c_str(), res_occ, -450, 450, res_occ, -450, 450);
    mhFOccupancy_ele[EBeam] = hFOccupancy_ele;
    TH2F* hGOccupancy_G1 = new TH2F(("hGOccupancy_G1_" + to_string(EBeam)).c_str(), ("hGOccupancy_G1_" + to_string(EBeam)).c_str(), res_occ, -450, 450, res_occ, -450, 450);
    mhGOccupancy_G1[EBeam] = hGOccupancy_G1;
    TH2F* hGOccupancy_G2 = new TH2F(("hGOccupancy_G2_" + to_string(EBeam)).c_str(), ("hGOccupancy_G2_" + to_string(EBeam)).c_str(), res_occ, -450, 450, res_occ, -450, 450);
    mhGOccupancy_G2[EBeam] = hGOccupancy_G2;

    TH2F* hSOccupancyGeom_pos = new TH2F(("hSOccupancyGeom_pos_" + to_string(EBeam)).c_str(), ("hSOccupancyGeom_pos_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhSOccupancyGeom_pos[EBeam] = hSOccupancyGeom_pos;
    TH2F* hSOccupancyGeom_ele = new TH2F(("hSOccupancyGeom_ele_" + to_string(EBeam)).c_str(), ("hSOccupancyGeom_ele_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhSOccupancyGeom_ele[EBeam] = hSOccupancyGeom_ele;
    TH2F* hFOccupancyGeom_pos = new TH2F(("hFOccupancyGeom_pos_" + to_string(EBeam)).c_str(), ("hFOccupancyGeom_pos_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhFOccupancyGeom_pos[EBeam] = hFOccupancyGeom_pos;
    TH2F* hFOccupancyGeom_ele = new TH2F(("hFOccupancyGeom_ele_" + to_string(EBeam)).c_str(), ("hFOccupancyGeom_ele_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhFOccupancyGeom_ele[EBeam] = hFOccupancyGeom_ele;
    TH2F* hGOccupancyGeom_G1 = new TH2F(("hGOccupancyGeom_G1_" + to_string(EBeam)).c_str(), ("hGOccupancyGeom_G1_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhGOccupancyGeom_G1[EBeam] = hGOccupancyGeom_G1;
    TH2F* hGOccupancyGeom_G2 = new TH2F(("hGOccupancyGeom_G2_" + to_string(EBeam)).c_str(), ("hGOccupancyGeom_G2_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhGOccupancyGeom_G2[EBeam] = hGOccupancyGeom_G2;

    TH2F* hSOccupancyAllCuts_pos = new TH2F(("hSOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), ("hSOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhSOccupancyAllCuts_pos[EBeam] = hSOccupancyAllCuts_pos;
    TH2F* hSOccupancyAllCuts_ele = new TH2F(("hSOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), ("hSOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhSOccupancyAllCuts_ele[EBeam] = hSOccupancyAllCuts_ele;
    TH2F* hFOccupancyAllCuts_pos = new TH2F(("hFOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), ("hFOccupancyAllCuts_pos_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhFOccupancyAllCuts_pos[EBeam] = hFOccupancyAllCuts_pos;
    TH2F* hFOccupancyAllCuts_ele = new TH2F(("hFOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), ("hFOccupancyAllCuts_ele_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhFOccupancyAllCuts_ele[EBeam] = hFOccupancyAllCuts_ele;
    TH2F* hGOccupancyAllCuts_G1 = new TH2F(("hGOccupancyAllCuts_G1_" + to_string(EBeam)).c_str(), ("hGOccupancyAllCuts_G1_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhGOccupancyAllCuts_G1[EBeam] = hGOccupancyAllCuts_G1;
    TH2F* hGOccupancyAllCuts_G2 = new TH2F(("hGOccupancyAllCuts_G2_" + to_string(EBeam)).c_str(), ("hGOccupancyAllCuts_G2_" + to_string(EBeam)).c_str(), CaloRes, -300, 300, CaloRes, -300, 300);
    mhGOccupancyAllCuts_G2[EBeam] = hGOccupancyAllCuts_G2;

    // TH1F* hSAcceptance = new TH1F(("hSAcceptance_" + to_string(EBeam)).c_str(), ("hSAcceptance_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    // mhSAcceptance[EBeam] = hSAcceptance;
    // TH1F* hFAcceptance = new TH1F(("hFAcceptance_" + to_string(EBeam)).c_str(), ("hFAcceptance_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    // mhFAcceptance[EBeam] = hFAcceptance;
    // TH1F* hGAcceptance = new TH1F(("hGAcceptance_" + to_string(EBeam)).c_str(), ("hGAcceptance_" + to_string(EBeam)).c_str(), 400, -1000, 1000);
    // mhGAcceptance[EBeam] = hGAcceptance;

    for(int it = 0; it<tSch->GetEntries(); it++){
        tSch->GetEntry(it);
        tFull->GetEntry(it);
        tGG->GetEntry(it);
        mhInitialP3_pos[EBeam]->Fill(P3pos_ini);
        mhInitialP3_ele[EBeam]->Fill(P3ele_ini);
        mhSFinalP1_ele[EBeam]->Fill(SFinalP1_ele);
        mhSFinalP2_ele[EBeam]->Fill(SFinalP2_ele);
        mhSFinalP3_ele[EBeam]->Fill(SFinalP3_ele);
        // cout<<"Px Py e Pz dell'elettrone in uscita S: "<<SFinalP1_ele<<SFinalP2_ele<<SFinalP3_ele<<endl;
        mhSFinalP1_pos[EBeam]->Fill(SFinalP1_pos);
        mhSFinalP2_pos[EBeam]->Fill(SFinalP2_pos);
        mhSFinalP3_pos[EBeam]->Fill(SFinalP3_pos);
        // cout<<"Px Py e Pz del positrone in uscita S: "<<SFinalP1_pos<<SFinalP2_pos<<SFinalP3_pos<<endl;
        mhFFinalP1_ele[EBeam]->Fill(FFinalP1_ele);
        mhFFinalP2_ele[EBeam]->Fill(FFinalP2_ele);
        mhFFinalP3_ele[EBeam]->Fill(FFinalP3_ele);
        // cout<<"Px Py e Pz dell'elettrone in uscita F: "<<FFinalP1_ele<<FFinalP2_ele<<FFinalP3_ele<<endl;
        mhFFinalP1_pos[EBeam]->Fill(FFinalP1_pos);
        mhFFinalP2_pos[EBeam]->Fill(FFinalP2_pos);
        mhFFinalP3_pos[EBeam]->Fill(FFinalP3_pos);
        // cout<<"Px Py e Pz del positrone in uscita F: "<<FFinalP1_pos<<FFinalP2_pos<<FFinalP3_pos<<endl;
        mhGFinalP1_G1[EBeam]->Fill(GFinalP1_G1);
        mhGFinalP2_G1[EBeam]->Fill(GFinalP2_G1);
        mhGFinalP3_G1[EBeam]->Fill(GFinalP3_G1);
        // cout<<"Px Py e Pz del primo gamma in uscita G: "<<GFinalP1_G1<<GFinalP2_G1<<GFinalP3_G1<<endl;
        mhGFinalP1_G2[EBeam]->Fill(GFinalP1_G2);
        mhGFinalP2_G2[EBeam]->Fill(GFinalP2_G2);
        mhGFinalP3_G2[EBeam]->Fill(GFinalP3_G2);
        // cout<<"Px Py e Pz del secondo gamma in uscita G: "<<GFinalP1_G2<<GFinalP2_G2<<GFinalP3_G2<<endl;
        Particle P1(ElectronMass, 0, 0, P3pos_ini);
        Particle P2(ElectronMass, 0, 0, P3ele_ini);
        cout<<"prova: "<<mhInitialP3_pos[EBeam]->GetBinContent(k)<<endl;

        Particle Sch_P3(ElectronMass, hSFinalP1_pos, SFinalP2_pos, SFinalP3_pos);
        Particle Sch_P4(ElectronMass, SFinalP1_ele, SFinalP2_ele, SFinalP3_ele);
        Particle Full_P3(ElectronMass, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos);
        Particle Full_P4(ElectronMass, FFinalP1_pos, FFinalP2_pos, FFinalP3_pos);
        Particle G1(0, GFinalP1_G1, GFinalP2_G1, GFinalP3_G1);
        Particle G2(0, GFinalP1_G2, GFinalP2_G2, GFinalP3_G2);

        //INITIAL STATE -> T,S CHANNEL and Ee->AA HAVE SAME INITIAL STATE
        mhInEnergy_pos[EBeam]->Fill(P1.Energy());
        mhInEnergy_ele[EBeam]->Fill(P2.Energy());
        // cout<<"prova: "<<P1.Energy()<<P2.Energy()<<endl;

        //FINAL STATE 3MOMENTUM DISTRIBUTION - BhabhaFull, S CHANNEL and Ee->AA
        mhSFinMomentum_pos[EBeam]->Fill(Sch_P3.TriMomentum());
        mhSFinMomentum_ele[EBeam]->Fill(Sch_P4.TriMomentum());
        mhFFinMomentum_pos[EBeam]->Fill(Full_P3.TriMomentum());
        mhFFinMomentum_ele[EBeam]->Fill(Full_P4.TriMomentum());
        mhGFinMomentum_G1[EBeam]->Fill(G1.TriMomentum());
        mhGFinMomentum_G2[EBeam]->Fill(G2.TriMomentum());
        cout<<"prova: "<<Sch_P3.TriMomentum()<<G2.TriMomentum()<<endl;


        //FINAL STATE ENERGY DISTRIBUTION - BhabhaFull, S CHANNEL and Ee->AA
        mhSFinEnergy_pos[EBeam]->Fill(Sch_P3.Energy());
        mhSFinEnergy_ele[EBeam]->Fill(Sch_P4.Energy());
        mhFFinEnergy_pos[EBeam]->Fill(Full_P3.Energy());
        mhFFinEnergy_ele[EBeam]->Fill(Full_P4.Energy());
        mhGFinEnergy_G1[EBeam]->Fill(G1.Energy());
        mhGFinEnergy_G2[EBeam]->Fill(G2.Energy());

        //ENERGY CONSERVATION CHECK
        mhSEn_tot[EBeam]->Fill(Sch_P3.Energy() + Sch_P4.Energy());
        mhFullEn_tot[EBeam]->Fill(Full_P3.Energy() + Full_P4.Energy());
        mhGGEn_tot[EBeam]->Fill(G1.Energy() + G2.Energy());

        //FINAL STATE ANGULAR DISTRIBUTION BETWEEN FINAL PARTICLES - BhabhaFull, S CHANNEL and Ee->AA
        //CoM frame
        mhSTheta_Scattering_COM[EBeam]->Fill(Sch_P3.CoMScatteringAngle(Sch_P4));
        mhFTheta_Scattering_COM[EBeam]->Fill(Full_P3.CoMScatteringAngle(Full_P4));
        mhGTheta_Scattering_COM[EBeam]->Fill(G1.CoMScatteringAngle(G2));
        //lab frame
        mhSPosTheta_Scattering[EBeam]->Fill(Sch_P3.ScatteringAngle());
        mhSEleTheta_Scattering[EBeam]->Fill(Sch_P4.ScatteringAngle());
        mhFPosTheta_Scattering[EBeam]->Fill(Full_P3.ScatteringAngle());
        mhSEleTheta_Scattering[EBeam]->Fill(Full_P4.ScatteringAngle());
        mhGG1Theta_Scattering[EBeam]->Fill(G1.ScatteringAngle());
        mhGG1Theta_Scattering[EBeam]->Fill(G2.ScatteringAngle());

        //TRANSVERSE FINAL POSITION AT 3000 MM FROM TARGET - THETA PHI DEF
        mhSPhiX_pos[EBeam]->Fill(Sch_P3.PhiX());
        mhSPhiY_pos[EBeam]->Fill(Sch_P3.PhiY());
        mhSPhiX_ele[EBeam]->Fill(Sch_P4.PhiX());
        mhSPhiY_ele[EBeam]->Fill(Sch_P4.PhiY());
        mhFPhiX_pos[EBeam]->Fill(Full_P3.PhiX());
        mhFPhiY_pos[EBeam]->Fill(Full_P3.PhiY());
        mhFPhiX_ele[EBeam]->Fill(Full_P4.PhiX());
        mhFPhiY_ele[EBeam]->Fill(Full_P4.PhiY());
        mhGPhiX_G1[EBeam]->Fill(G1.PhiX());
        mhGPhiY_G1[EBeam]->Fill(G1.PhiY());
        mhGPhiX_G2[EBeam]->Fill(G2.PhiX());
        mhGPhiY_G2[EBeam]->Fill(G2.PhiY());

        //POSITION
        mhSx_pos[EBeam]->Fill(Sch_P3.R_x(TargetCalo));
        mhSy_pos[EBeam]->Fill(Sch_P3.R_y(TargetCalo));
        mhSx_ele[EBeam]->Fill(Sch_P4.R_x(TargetCalo));
        mhSy_ele[EBeam]->Fill(Sch_P4.R_y(TargetCalo));
        mhFx_pos[EBeam]->Fill(Full_P3.R_x(TargetCalo));
        mhFy_pos[EBeam]->Fill(Full_P3.R_y(TargetCalo));
        mhFx_ele[EBeam]->Fill(Full_P4.R_x(TargetCalo));
        mhFy_ele[EBeam]->Fill(Full_P4.R_y(TargetCalo));
        mhGx_G1[EBeam]->Fill(G1.R_x(TargetCalo));
        mhGy_G1[EBeam]->Fill(G1.R_y(TargetCalo));
        mhGx_G2[EBeam]->Fill(G2.R_x(TargetCalo));
        mhGy_G2[EBeam]->Fill(G2.R_y(TargetCalo));

        //final state radius at target
        mhSRadius_pos[EBeam]->Fill(Sch_P3.TransverseRadius(TargetCalo));
        mhSRadius_ele[EBeam]->Fill(Sch_P4.TransverseRadius(TargetCalo));
        mhFRadius_pos[EBeam]->Fill(Full_P3.TransverseRadius(TargetCalo));
        mhFRadius_ele[EBeam]->Fill(Full_P4.TransverseRadius(TargetCalo));
        mhGRadius_G1[EBeam]->Fill(G1.TransverseRadius(TargetCalo));
        mhGRadius_G2[EBeam]->Fill(G2.TransverseRadius(TargetCalo));

        EventAnalyzer SAnalyzer(mhSOccupancy_pos[EBeam], mhSOccupancy_ele[EBeam],
                                mhSOccupancyGeom_pos[EBeam], mhSOccupancyGeom_ele[EBeam],
                                mhSOccupancyAllCuts_pos[EBeam], mhSOccupancyAllCuts_ele[EBeam]);
        EventAnalyzer FAnalyzer(mhFOccupancy_pos[EBeam], mhFOccupancy_ele[EBeam],
                                mhFOccupancyGeom_pos[EBeam], mhFOccupancyGeom_ele[EBeam],
                                mhFOccupancyAllCuts_pos[EBeam], mhFOccupancyAllCuts_ele[EBeam]);
        EventAnalyzer GAnalyzer(mhGOccupancy_G1[EBeam], mhGOccupancy_G2[EBeam],
                                mhGOccupancyGeom_G1[EBeam], mhGOccupancyGeom_G2[EBeam],
                                mhGOccupancyAllCuts_G1[EBeam], mhGOccupancyAllCuts_G2[EBeam]);
        
        SAnalyzer.AnalyzeEvents(Radius_max, En_min, Radius_min, 
                                Sch_P3.TransverseRadius(TargetCalo), Sch_P4.TransverseRadius(TargetCalo),
                                Sch_P3.R_x(TargetCalo), Sch_P3.R_y(TargetCalo),
                                Sch_P4.R_x(TargetCalo), Sch_P4.R_y(TargetCalo),
                                Sch_P3.Energy(), Sch_P4.Energy(),
                                MagnetShadow);
        
        FAnalyzer.AnalyzeEvents(Radius_max, En_min, Radius_min, 
                                Full_P3.TransverseRadius(TargetCalo), Full_P4.TransverseRadius(TargetCalo),
                                Full_P3.R_x(TargetCalo), Full_P3.R_y(TargetCalo),
                                Full_P4.R_x(TargetCalo), Full_P4.R_y(TargetCalo),
                                Full_P3.Energy(), Full_P4.Energy(),
                                MagnetShadow);

        SAnalyzer.AnalyzeEvents(Radius_max, En_min, Radius_min, 
                                G1.TransverseRadius(TargetCalo), G2.TransverseRadius(TargetCalo),
                                G1.R_x(TargetCalo), G2.R_y(TargetCalo),
                                G2.R_x(TargetCalo), G2.R_y(TargetCalo),
                                G1.Energy(), G2.Energy(),
                                MagnetShadow);

}

mhInitialP3_pos[E0]->Draw();
// mhGFinalP1_G2[E0]->Draw();

// int nentries = mhInitialP3_pos[E0]->GetEntries();
// cout<<nentries<<endl;
// cout<<mhInitialP3_pos[E0]->GetBinContent(1)<<" "<<mhInitialP3_pos[E0]->GetBinContent(37593)<<mhInitialP3_pos[E0]->GetBinContent(0)<<" "<<mhInitialP3_pos[E0]->GetBinContent(11)<<endl;


double x = 0.;
for(int iE = 0; iE<NEbeam; iE++){
    EBeam = E0 + iE*0.005;
    for(Long64_t k=0; k<=Bin; k++){
    Particle P1(ElectronMass, 0, 0, );
    Particle P2(ElectronMass, 0, 0, mhInitialP3_ele[EBeam]->GetBinContent(x));
    cout<<"prova: "<<mhInitialP3_pos[EBeam]->GetBinContent(k)<<endl;

    Particle Sch_P3(ElectronMass, mhSFinalP1_pos[EBeam]->GetBinContent(k), mhSFinalP2_pos[EBeam]->GetBinContent(k), mhSFinalP3_pos[EBeam]->GetBinContent(k));
    Particle Sch_P4(ElectronMass, mhSFinalP1_ele[EBeam]->GetBinContent(k), mhSFinalP2_ele[EBeam]->GetBinContent(k), mhSFinalP3_ele[EBeam]->GetBinContent(k));
    Particle Full_P3(ElectronMass, mhFFinalP1_pos[EBeam]->GetBinContent(k), mhFFinalP2_pos[EBeam]->GetBinContent(k), mhFFinalP3_pos[EBeam]->GetBinContent(k));
    Particle Full_P4(ElectronMass, mhFFinalP1_pos[EBeam]->GetBinContent(k), mhFFinalP2_pos[EBeam]->GetBinContent(k), mhFFinalP3_pos[EBeam]->GetBinContent(k));
    Particle G1(0, mhGFinalP1_G1[EBeam]->GetBinContent(k), mhGFinalP2_G1[EBeam]->GetBinContent(k), mhGFinalP3_G1[EBeam]->GetBinContent(k));
    Particle G2(0, mhGFinalP1_G2[EBeam]->GetBinContent(k), mhGFinalP2_G2[EBeam]->GetBinContent(k), mhGFinalP3_G2[EBeam]->GetBinContent(k));

    //INITIAL STATE -> T,S CHANNEL and Ee->AA HAVE SAME INITIAL STATE
    mhInEnergy_pos[EBeam]->Fill(P1.Energy());
    mhInEnergy_ele[EBeam]->Fill(P2.Energy());
    // cout<<"prova: "<<P1.Energy()<<P2.Energy()<<endl;

    //FINAL STATE 3MOMENTUM DISTRIBUTION - BhabhaFull, S CHANNEL and Ee->AA
    mhSFinMomentum_pos[EBeam]->Fill(Sch_P3.TriMomentum());
    mhSFinMomentum_ele[EBeam]->Fill(Sch_P4.TriMomentum());
    mhFFinMomentum_pos[EBeam]->Fill(Full_P3.TriMomentum());
    mhFFinMomentum_ele[EBeam]->Fill(Full_P4.TriMomentum());
    mhGFinMomentum_G1[EBeam]->Fill(G1.TriMomentum());
    mhGFinMomentum_G2[EBeam]->Fill(G2.TriMomentum());
    cout<<"prova: "<<Sch_P3.TriMomentum()<<G2.TriMomentum()<<endl;


    //FINAL STATE ENERGY DISTRIBUTION - BhabhaFull, S CHANNEL and Ee->AA
    mhSFinEnergy_pos[EBeam]->Fill(Sch_P3.Energy());
    mhSFinEnergy_ele[EBeam]->Fill(Sch_P4.Energy());
    mhFFinEnergy_pos[EBeam]->Fill(Full_P3.Energy());
    mhFFinEnergy_ele[EBeam]->Fill(Full_P4.Energy());
    mhGFinEnergy_G1[EBeam]->Fill(G1.Energy());
    mhGFinEnergy_G2[EBeam]->Fill(G2.Energy());

    //ENERGY CONSERVATION CHECK
    mhSEn_tot[EBeam]->Fill(Sch_P3.Energy() + Sch_P4.Energy());
    mhFullEn_tot[EBeam]->Fill(Full_P3.Energy() + Full_P4.Energy());
    mhGGEn_tot[EBeam]->Fill(G1.Energy() + G2.Energy());

    //FINAL STATE ANGULAR DISTRIBUTION BETWEEN FINAL PARTICLES - BhabhaFull, S CHANNEL and Ee->AA
    //CoM frame
    mhSTheta_Scattering_COM[EBeam]->Fill(Sch_P3.CoMScatteringAngle(Sch_P4));
    mhFTheta_Scattering_COM[EBeam]->Fill(Full_P3.CoMScatteringAngle(Full_P4));
    mhGTheta_Scattering_COM[EBeam]->Fill(G1.CoMScatteringAngle(G2));
    //lab frame
    mhSPosTheta_Scattering[EBeam]->Fill(Sch_P3.ScatteringAngle());
    mhSEleTheta_Scattering[EBeam]->Fill(Sch_P4.ScatteringAngle());
    mhFPosTheta_Scattering[EBeam]->Fill(Full_P3.ScatteringAngle());
    mhSEleTheta_Scattering[EBeam]->Fill(Full_P4.ScatteringAngle());
    mhGG1Theta_Scattering[EBeam]->Fill(G1.ScatteringAngle());
    mhGG1Theta_Scattering[EBeam]->Fill(G2.ScatteringAngle());

    //TRANSVERSE FINAL POSITION AT 3000 MM FROM TARGET - THETA PHI DEF
    mhSPhiX_pos[EBeam]->Fill(Sch_P3.PhiX());
    mhSPhiY_pos[EBeam]->Fill(Sch_P3.PhiY());
    mhSPhiX_ele[EBeam]->Fill(Sch_P4.PhiX());
    mhSPhiY_ele[EBeam]->Fill(Sch_P4.PhiY());
    mhFPhiX_pos[EBeam]->Fill(Full_P3.PhiX());
    mhFPhiY_pos[EBeam]->Fill(Full_P3.PhiY());
    mhFPhiX_ele[EBeam]->Fill(Full_P4.PhiX());
    mhFPhiY_ele[EBeam]->Fill(Full_P4.PhiY());
    mhGPhiX_G1[EBeam]->Fill(G1.PhiX());
    mhGPhiY_G1[EBeam]->Fill(G1.PhiY());
    mhGPhiX_G2[EBeam]->Fill(G2.PhiX());
    mhGPhiY_G2[EBeam]->Fill(G2.PhiY());

    //POSITION
    mhSx_pos[EBeam]->Fill(Sch_P3.R_x(TargetCalo));
    mhSy_pos[EBeam]->Fill(Sch_P3.R_y(TargetCalo));
    mhSx_ele[EBeam]->Fill(Sch_P4.R_x(TargetCalo));
    mhSy_ele[EBeam]->Fill(Sch_P4.R_y(TargetCalo));
    mhFx_pos[EBeam]->Fill(Full_P3.R_x(TargetCalo));
    mhFy_pos[EBeam]->Fill(Full_P3.R_y(TargetCalo));
    mhFx_ele[EBeam]->Fill(Full_P4.R_x(TargetCalo));
    mhFy_ele[EBeam]->Fill(Full_P4.R_y(TargetCalo));
    mhGx_G1[EBeam]->Fill(G1.R_x(TargetCalo));
    mhGy_G1[EBeam]->Fill(G1.R_y(TargetCalo));
    mhGx_G2[EBeam]->Fill(G2.R_x(TargetCalo));
    mhGy_G2[EBeam]->Fill(G2.R_y(TargetCalo));

    //final state radius at target
    mhSRadius_pos[EBeam]->Fill(Sch_P3.TransverseRadius(TargetCalo));
    mhSRadius_ele[EBeam]->Fill(Sch_P4.TransverseRadius(TargetCalo));
    mhFRadius_pos[EBeam]->Fill(Full_P3.TransverseRadius(TargetCalo));
    mhFRadius_ele[EBeam]->Fill(Full_P4.TransverseRadius(TargetCalo));
    mhGRadius_G1[EBeam]->Fill(G1.TransverseRadius(TargetCalo));
    mhGRadius_G2[EBeam]->Fill(G2.TransverseRadius(TargetCalo));

    EventAnalyzer SAnalyzer(mhSOccupancy_pos[EBeam], mhSOccupancy_ele[EBeam],
                            mhSOccupancyGeom_pos[EBeam], mhSOccupancyGeom_ele[EBeam],
                            mhSOccupancyAllCuts_pos[EBeam], mhSOccupancyAllCuts_ele[EBeam]);
    EventAnalyzer FAnalyzer(mhFOccupancy_pos[EBeam], mhFOccupancy_ele[EBeam],
                            mhFOccupancyGeom_pos[EBeam], mhFOccupancyGeom_ele[EBeam],
                            mhFOccupancyAllCuts_pos[EBeam], mhFOccupancyAllCuts_ele[EBeam]);
    EventAnalyzer GAnalyzer(mhGOccupancy_G1[EBeam], mhGOccupancy_G2[EBeam],
                            mhGOccupancyGeom_G1[EBeam], mhGOccupancyGeom_G2[EBeam],
                            mhGOccupancyAllCuts_G1[EBeam], mhGOccupancyAllCuts_G2[EBeam]);
    
    SAnalyzer.AnalyzeEvents(Radius_max, En_min, Radius_min, 
                            Sch_P3.TransverseRadius(TargetCalo), Sch_P4.TransverseRadius(TargetCalo),
                            Sch_P3.R_x(TargetCalo), Sch_P3.R_y(TargetCalo),
                            Sch_P4.R_x(TargetCalo), Sch_P4.R_y(TargetCalo),
                            Sch_P3.Energy(), Sch_P4.Energy(),
                            MagnetShadow);
    
    FAnalyzer.AnalyzeEvents(Radius_max, En_min, Radius_min, 
                            Full_P3.TransverseRadius(TargetCalo), Full_P4.TransverseRadius(TargetCalo),
                            Full_P3.R_x(TargetCalo), Full_P3.R_y(TargetCalo),
                            Full_P4.R_x(TargetCalo), Full_P4.R_y(TargetCalo),
                            Full_P3.Energy(), Full_P4.Energy(),
                            MagnetShadow);

    SAnalyzer.AnalyzeEvents(Radius_max, En_min, Radius_min, 
                            G1.TransverseRadius(TargetCalo), G2.TransverseRadius(TargetCalo),
                            G1.R_x(TargetCalo), G2.R_y(TargetCalo),
                            G2.R_x(TargetCalo), G2.R_y(TargetCalo),
                            G1.Energy(), G2.Energy(),
                            MagnetShadow);
    }
    mSAcceptance[EBeam] = mhSOccupancyAllCuts_pos[EBeam]->GetNbinsX() / nentries;
    hSAcceptance->Fill(EBeam, mSAcceptance[EBeam]);
    mFAcceptance[EBeam] = mhFOccupancyAllCuts_pos[EBeam]->GetNbinsX() / nentries;
    hFAcceptance->Fill(EBeam, mFAcceptance[EBeam]);
    mGAcceptance[EBeam] = mhGOccupancyAllCuts_G1[EBeam]->GetNbinsX() / nentries;
    hGAcceptance->Fill(EBeam, mGAcceptance[EBeam]);

    mhInitialP3_pos[EBeam]->Write();
    mhInitialP3_ele[EBeam]->Write();
    mhInEnergy_pos[EBeam]->Write();
    mhInEnergy_ele[EBeam]->Write();

    mhSOccupancy_pos[EBeam]->Write();
    mhSOccupancy_ele[EBeam]->Write();
    mhSOccupancyGeom_pos[EBeam]->Write();
    mhSOccupancyGeom_ele[EBeam]->Write();
    mhSOccupancyAllCuts_pos[EBeam]->Write();
    mhSOccupancyAllCuts_ele[EBeam]->Write();
    mhFOccupancy_pos[EBeam]->Write();
    mhFOccupancy_ele[EBeam]->Write();
    mhFOccupancyGeom_pos[EBeam]->Write();
    mhFOccupancyGeom_ele[EBeam]->Write();
    mhFOccupancyAllCuts_pos[EBeam]->Write();
    mhFOccupancyAllCuts_ele[EBeam]->Write();
    mhGOccupancy_G1[EBeam]->Write();
    mhGOccupancy_G2[EBeam]->Write();
    mhGOccupancyGeom_G1[EBeam]->Write();
    mhGOccupancyGeom_G2[EBeam]->Write();
    mhGOccupancyAllCuts_G1[EBeam]->Write();
    mhGOccupancyAllCuts_G2[EBeam]->Write();

    hSAcceptance->Write();
    hFAcceptance->Write();
    hGAcceptance->Write();

    SCh_dir->cd();
    mhSFinalP1_pos[EBeam]->Write();
    mhSFinalP2_pos[EBeam]->Write();
    mhSFinalP3_pos[EBeam]->Write();
    mhSFinalP1_ele[EBeam]->Write();
    mhSFinalP2_ele[EBeam]->Write();
    mhSFinalP3_ele[EBeam]->Write();
    mhSFinMomentum_pos[EBeam]->Write();
    mhSFinMomentum_ele[EBeam]->Write();
    mhSFinEnergy_pos[EBeam]->Write();
    mhSFinEnergy_ele[EBeam]->Write();
    mhSEn_tot[EBeam]->Write();
    mhSTheta_Scattering_COM[EBeam]->Write();
    mhSPosTheta_Scattering[EBeam]->Write();
    mhSEleTheta_Scattering[EBeam]->Write();
    mhSPhiX_pos[EBeam]->Write();
    mhSPhiY_pos[EBeam]->Write();
    mhSPhiX_ele[EBeam]->Write();
    mhSPhiY_ele[EBeam]->Write();
    mhSx_pos[EBeam]->Write();
    mhSy_pos[EBeam]->Write();
    mhSx_ele[EBeam]->Write();
    mhSy_ele[EBeam]->Write();
    mhSRadius_pos[EBeam]->Write();
    mhSRadius_ele[EBeam]->Write();

    Full_dir->cd();
    mhFFinalP1_pos[EBeam]->Write();
    mhFFinalP2_pos[EBeam]->Write();
    mhFFinalP3_pos[EBeam]->Write();
    mhFFinalP1_ele[EBeam]->Write();
    mhFFinalP2_ele[EBeam]->Write();
    mhFFinalP3_ele[EBeam]->Write();
    mhFFinMomentum_pos[EBeam]->Write();
    mhFFinMomentum_ele[EBeam]->Write();
    mhFFinEnergy_pos[EBeam]->Write();
    mhFFinEnergy_ele[EBeam]->Write();
    mhFullEn_tot[EBeam]->Write();
    mhFTheta_Scattering_COM[EBeam]->Write();
    mhFPosTheta_Scattering[EBeam]->Write();
    mhSEleTheta_Scattering[EBeam]->Write();
    mhFPhiX_pos[EBeam]->Write();
    mhFPhiY_pos[EBeam]->Write();
    mhFPhiX_ele[EBeam]->Write();
    mhFPhiY_ele[EBeam]->Write();
    mhFx_pos[EBeam]->Write();
    mhFy_pos[EBeam]->Write();
    mhFx_ele[EBeam]->Write();
    mhFy_ele[EBeam]->Write();
    mhFRadius_pos[EBeam]->Write();
    mhFRadius_ele[EBeam]->Write();

    GG_dir->cd();
    mhGFinalP1_G1[EBeam]->Write();
    mhGFinalP2_G1[EBeam]->Write();
    mhGFinalP3_G1[EBeam]->Write();
    mhGFinalP1_G2[EBeam]->Write();
    mhGFinalP2_G2[EBeam]->Write();
    mhGFinalP3_G2[EBeam]->Write();
    mhGFinMomentum_G1[EBeam]->Write();
    mhGFinMomentum_G2[EBeam]->Write();
    mhGFinEnergy_G1[EBeam]->Write();
    mhGFinEnergy_G2[EBeam]->Write();
    mhGGEn_tot[EBeam]->Write();
    mhGTheta_Scattering_COM[EBeam]->Write();
    mhGG1Theta_Scattering[EBeam]->Write();
    mhGG1Theta_Scattering[EBeam]->Write();
    mhGPhiX_G1[EBeam]->Write();
    mhGPhiY_G1[EBeam]->Write();
    mhGPhiX_G2[EBeam]->Write();
    mhGPhiY_G2[EBeam]->Write();
    mhGx_G1[EBeam]->Write();
    mhGy_G1[EBeam]->Write();
    mhGx_G2[EBeam]->Write();
    mhGy_G2[EBeam]->Write();    
    mhGRadius_G1[EBeam]->Write();
    mhGRadius_G2[EBeam]->Write();

}

FileOut->Close();



// double N_PADME_S, N_PADME_G, N_PADME_F;
// double Squared_gve[20];
// map<double, double> Scan_S, Scan_F, Scan_G;
// map<double, double> S_ACC, F_ACC, G_ACC;
// double S_ACC[21][23], T_ACC[21][23], Full_ACC[21][23], AA_ACC[21][23];
// double Sensitivity[20][10], LowerLimitX17[20], X17_produced[20];
// double Acceptance_ratio[21][23];  


// //running PoT adn coupling
// TH2F * SENS = new TH2F("SENS", "PADME X17 detected over sqrt(BG) as a function of N-PoT and g_{Ve}", 20, 0.5e10, 2.05e11, 10, 0.5e-4, 1.05e-3);
// TH1F * SENSrad = new TH1F("SENSrad", "PADME Sensitivity as a function of R_{max}", 21, 177.5, 282.5);
// TH1F * DPOT = new TH1F("DPOT", "PADME X17 discovery potential as a function of N-PoT", 20, 0.5e10, 2.05e11);
// TH1F * CL90 = new TH1F("CL90", "PADME 90% CL as a function of N-PoT", 20, 0.5e10, 2.05e11);
// TH1F * exclusion_X17 = new TH1F("exclusion_X17", "g_{V_{e}} exclusion plot as a function of N-PoT", 20, 0.5e10, 2.05e11);

// //NA64 epsilon limit
// TH1F * NA64_limit = new TH1F("NA64_limit", "g_{e} NA64", 20, 0.5e10, 2.05e11);
// double ge_NA64[20], Disc[20];


// cout<<"SChannel Acc : "<<S_ACC[18][20]<<endl;
// cout<<"TChannel Acc : "<<T_ACC[18][20]<<endl;
// cout<<"Bhabha Full Acc : "<<Full_ACC[18][20]<<endl;
// cout<<"AA-prod Acc : "<<AA_ACC[18][20]<<endl;

// double SensRad[21];

// for(Int_t l = 0; l<20; l++){ //POT
// cout<<"\n ---------------------PoT = "<<PoT[l]<<" -------------"<<endl;
// NC_PADME_Full[l] = Full_ACC[18][20] * N_Full_C[l];
// NC_PADME_S[l] = S_ACC[18][20] * N_Schannel_C[l];
// NC_PADME_T[l] = T_ACC[18][20] * N_Tchannel_C[l];
// NC_PADME_AA[l] = AA_ACC[18][20] * N_AA_C[l];

// ge_NA64[l] = 2e-4;

// LowerLimitX17[l]= 1.3*TMath::Sqrt(NC_PADME_Full[l] + NC_PADME_AA[l]); //bhabha full + gamma gamma con 90CL 1.3sigma
// X17_produced[l] = LowerLimitX17[l] / S_ACC[18][20]; //il numero di candidati X17 entro il calorimetro -> 90CL * Accettanza S Channel
// Squared_gve[l]= TMath::Sqrt((X17_produced[l] * 0.7)/(PoT[l]*PreFactor_X17)*(9e-8) * ((A_C * 1e24)/(Na * Z_C * D_C)));
// CL90->Fill(PoT[l], LowerLimitX17[l]);
// exclusion_X17->Fill(PoT[l], Squared_gve[l]);
// NA64_limit->Fill(PoT[l], ge_NA64[l]);

// cout<<"N_tot S-channel : "<<NC_PADME_S[l]<<endl;
// cout<<"N_tot T-channel : "<<NC_PADME_T[l]<<endl;
// cout<<"N_tot Bhabha full : "<<NC_PADME_Full[l]<<endl;
// cout<<"N_tot Gamma-Gamma : "<<NC_PADME_AA[l]<<endl;
// // cout<<"90% Confidence level : "<<LowerLimitX17[l]<<endl; 
// // cout<<"Minimal g_Ve^{2}: "<<Squared_gve[l]<<endl; 

// for(Int_t j = 0; j<10; j++){ //EPSILON
//     cout<<"--------------g_Ve = "<<g_ve[j]<<" -----------"<<endl;

//     NC_PADME_X17[l][j] = S_ACC[18][20] * N_X17_C[l][j]; // questi sono il numero di X17 prodotti dalla formula 13_proceedings moltiplicati per S_Accettanza -> QUELLI RIVELATI DAL CALO
//     Sensitivity[l][j] = NC_PADME_X17[l][j] / TMath::Sqrt(NC_PADME_X17[l][j] + NC_PADME_Full[l] + NC_PADME_AA[l]); //tiene conto di tutte le accettanze
//     if(Sensitivity[l][j]<5){Disc[l] = g_ve[j];}
//     SENS->Fill(PoT[l], g_ve[j], Sensitivity[l][j]);
    
//     cout<<"N_tot X17 : "<<NC_PADME_X17[l][j]<<endl;
//     //file<<"Sensitivity : "<<Sensitivity[l][j]<<"\n";
    
// }
// DPOT->Fill(PoT[l],Disc[l]);
// }

// for(Int_t k=0; k<21; k++){
// SensRad[k] = (S_ACC[k][20] * N_X17_C[0][1]) / TMath::Sqrt((Full_ACC[k][20]*N_Full_C[0]) + (S_ACC[k][20] * N_X17_C[0][1])); //1e10 POT - g_Ve = 2e-4 - scan sensitivity in R_max
// if(TMath::IsNaN(SensRad[k])) {SensRad[k] = 0.;}
// SENSrad->Fill(Radius_max[k], SensRad[k]);
// cout<<"Sensitivity as a function of Rmax: "<<SensRad[k]<<endl;
// }


// TCanvas *C1 = new TCanvas("C1", "3-Momentum BhaBha Scattering->S-Channel distribution", 600, 700);
// C1->Divide(1,2);
// C1->cd(1);
// h1->Draw("HIST");
// gPad->SetLogy();
// C1->cd(2);
// h2->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C2 = new TCanvas("C2", "3-Momentum BhaBha Scattering-> T-Channel distribution", 600, 700);
// C2->Divide(1,2);
// C2->cd(1);
// h3->Draw("HIST");
// gPad->SetLogy();
// C2->cd(2);
// h4->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C1Full = new TCanvas("C1Full", "3-Momentum Bhabha Full distribution", 600, 700);
// C1Full->Divide(1,2);
// C1Full->cd(1);
// h1Full->Draw("HIST");
// gPad->SetLogy();
// C1Full->cd(2);
// h2Full->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C1G = new TCanvas("C1G", "3-Momentum #gamma#gamma production distribution", 600, 700);
// C1G->Divide(1,2);
// C1G->cd(1);
// h1g->Draw("HIST");
// gPad->SetLogy();
// C1G->cd(2);
// h2g->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C3P = new TCanvas("C3P", "Final 3-Momentum Positron S Channel, T Channel, Full and #gamma#gamma", 900, 700);
// // gStyle->SetOptStat(kFALSE);
// C3P->SetGrid();
// h1->Draw("HIST");
// h3->Draw("same HIST");
// h1Full->Draw("same HIST");
// h1g->Draw("same HIST");
// gPad->BuildLegend(0.07, 0.8, 0.25, 0.95);
// gPad->SetLogy();

// TCanvas *CHECK1 = new TCanvas("CHECK1", "Energy conservation check", 900, 700);
// CHECK1->Divide(2,2);
// CHECK1->cd(1);
// check1->Draw("HIST");
// CHECK1->cd(2);
// check2->Draw("HIST");
// CHECK1->cd(3);
// checkFull->Draw("HIST");
// CHECK1->cd(4);
// checkg->Draw("HIST");

// TCanvas *Ctheta = new TCanvas("Ctheta", "Angle between final particles in CoM frame", 900, 700);
// Ctheta->Divide(2,2);
// Ctheta->cd(1);
// htheta1->Draw("HIST");
// gPad->SetLogy();
// Ctheta->cd(2);
// htheta2->Draw("HIST");
// gPad->SetLogy();
// Ctheta->cd(3);
// hthetaFull->Draw("HIST");
// gPad->SetLogy();
// Ctheta->cd(4);
// hthetag->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C3 = new TCanvas("C3", "Energy BhaBha Scattering-> S-Channel distribution", 600, 700);
// C3->Divide(1,2);
// C3->cd(1);
// h5->Draw("HIST");
// gPad->SetLogy();
// C3->cd(2);
// h6->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C4 = new TCanvas("C4", "Energy BhaBha Scattering->T-Channel distribution", 600, 700);
// C4->Divide(1,2);
// C4->cd(1);
// h7->Draw("HIST");
// gPad->SetLogy();
// C4->cd(2);
// h8->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C2Full = new TCanvas("C2Full", "Energy Bhabha Full distribution", 600, 700);
// C2Full->Divide(1,2);
// C2Full->cd(1);
// h3Full->Draw("HIST");
// gPad->SetLogy();
// C2Full->cd(2);
// h4Full->Draw("HIST");
// gPad->SetLogy();

// TCanvas *C2g = new TCanvas("C2g", "Energy #gamma#gamma production distribution", 600, 700);
// C2g->Divide(1,2);
// C2g->cd(1);
// h3g->Draw("HIST");
// gPad->SetLogy();
// C2g->cd(2);
// h4g->Draw("HIST");
// gPad->SetLogy();

// TCanvas *CEn = new TCanvas("CEn", "Final Energy Positron S Channe, T Channel, Full and #gamma#gamma", 900, 700);
// // gStyle->SetOptStat(kFALSE);
// CEn->SetGrid();
// h5->Draw();
// h7->Draw("same HIST");
// h3Full->Draw("same HIST");
// h3g->Draw("same HIST");
// gPad->BuildLegend(0.07, 0.77, 0.27, 0.95);
// gPad->SetLogy();

// TCanvas *CHS = new TCanvas("CHS", "S-channel Final Positron distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHS->SetGrid();
// HS->Draw("hist");
// //HScut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();
// TCanvas *CHT = new TCanvas("CHT", "T-channel Final Positron distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHT->SetGrid();
// HT->Draw("hist");
// //HTcut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();
// TCanvas *CHFull = new TCanvas("CHFull", "Bhabha Full Final Positron distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHFull->SetGrid();
// HFull->Draw("hist");
// //HAcut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();
// TCanvas *CHG = new TCanvas("CHG", "#gamma_{1} Final distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHG->SetGrid();
// HA1->Draw("hist");
// //HAcut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();
// TCanvas *CHS1 = new TCanvas("CHS1", "S-channel Final Electron distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHS1->SetGrid();
// HS1->Draw("hist");
// //HS1cut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();
// TCanvas *CHT1 = new TCanvas("CHT1", "T-channel Final Electron distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHT1->SetGrid();
// HT1->Draw("hist");
// //HT1cut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();
// TCanvas *CHFull1 = new TCanvas("CHFull1", "Bhabha Full Final Electron distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHFull1->SetGrid();
// HFull1->Draw("hist");
// //HA1cut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();
// TCanvas *CHG1 = new TCanvas("CHG1", "#gamma_2 Final distribution", 900, 700);
// gStyle->SetOptStat(kFALSE);
// CHG1->SetGrid();
// HA1->Draw("hist");
// //HA1cut->Draw("hist same");
// //gPad->BuildLegend();
// gPad->SetLogy();

// TCanvas *C5 = new TCanvas("C5", "Final transverse position at 3720 mm from the target", 1250, 770);
// //gStyle->SetOptStat(kFALSE);
// C5->Divide(4,2);
// C5->cd(1);
// histT_Pos_tot->Draw("colz");
// C5->cd(2);
// histS_Pos_tot->Draw("colz");
// C5->cd(3);
// histFull_Pos_tot->Draw("colz");
// C5->cd(4);
// histAA1_tot->Draw("colz");
// C5->cd(5);
// histT_Elec_tot->Draw("colz");
// C5->cd(6);
// histS_Elec_tot->Draw("colz");
// C5->cd(7);
// histFull_Elec_tot->Draw("colz");
// C5->cd(8);
// histAA2_tot->Draw("colz");

// TCanvas *C6 = new TCanvas("C6", "Final transverse position at 3720 mm from the target - R_{max} = 270 mm", 1250, 770);
// //gStyle->SetOptStat(kFALSE);
// C6->Divide(4,2);
// C6->cd(1);
// h13->Draw("colz");
// C6->cd(2);
// h9->Draw("colz");
// C6->cd(3);
// h17->Draw("colz");
// C6->cd(4);
// hg1->Draw("colz");
// C6->cd(5);
// h14->Draw("colz");
// C6->cd(6);
// h10->Draw("colz");
// C6->cd(7);
// h18->Draw("colz");
// C6->cd(8);
// hg2->Draw("colz");

// TCanvas *C7 = new TCanvas("C7", "Final transverse position at 3720 mm from the target - R_{max} = 270 mm", 1250, 770);
// //gStyle->SetOptStat(kFALSE);
// C7->Divide(4,2);
// C7->cd(1);
// h15->Draw("colz");
// C7->cd(2);
// h11->Draw("colz");
// C7->cd(3);
// h19->Draw("colz");
// C7->cd(4);
// hg3->Draw("colz");
// C7->cd(5);
// h16->Draw("colz");
// C7->cd(6);
// h12->Draw("colz");
// C7->cd(7);
// h20->Draw("colz");
// C7->cd(8);
// hg4->Draw("colz");

// TCanvas *C8 = new TCanvas("C8", "Final transverse position at 3720 mm from the target - R_{max} = 270 mm & Magnet effect", 1250, 770);
// gStyle->SetOptStat(kTRUE);
// C8->Divide(1,2);
// // C8->cd(1);
// // h21->Draw("colz");
// // C8->cd(2);
// // h23->Draw("colz");
// C8->cd(1); //modified
// h25->Draw("colz");
// // C8->cd(4);
// // hg5->Draw("colz");
// // C8->cd(5);
// // h22->Draw("colz");
// // C8->cd(6);
// // h24->Draw("colz");
// C8->cd(2); //modified
// h26->Draw("colz");
// // C8->cd(8);
// // hg6->Draw("colz");

// TCanvas *Acc1D_S = new TCanvas("Acc1D_S", "PADME S-Channel Acceptance as a function of R_{max} and E>100 MeV", 900, 700);
// Acc1D_S->SetGrid();
// gStyle->SetOptStat(kFALSE);
// ACC_S->Draw("HIST");

// TCanvas *Acc1D_T = new TCanvas("Acc1D_T", "PADME T-Channel Acceptance as a function of R_{max} and E>100 MeV", 900, 700);
// Acc1D_T->SetGrid();
// gStyle->SetOptStat(kFALSE);
// ACC_T->Draw("HIST");

// TCanvas *Acc1D_Full = new TCanvas("Acc1D_Full", "PADME Bhabha Full Acceptance as a function of R_{max} and E>100 MeV", 900, 700);
// Acc1D_Full->SetGrid();
// gStyle->SetOptStat(kFALSE);
// ACC_Full->Draw("HIST");

// TCanvas *Acc1D_AA = new TCanvas("Acc1D_AA", "PADME #gamma#gamma Acceptance as a function of R_{max} and E>100 MeV", 900, 700);
// Acc1D_AA->SetGrid();
// gStyle->SetOptStat(kFALSE);
// ACC_AA->Draw("HIST");

// // TCanvas *Acc_rat = new TCanvas("Acc_rat", "Acceptance ratio as a function of R_{max} and energy", 900, 700);
// // Acc_rat->SetGrid();
// // gStyle->SetOptStat(kFALSE);
// // gStyle->SetPalette(kBird);
// // ACC_RATIO->Draw("colz");

// TCanvas *Sens3D = new TCanvas("Sens3D", "PADME X17 Sensitivity as a function of N-PoT and epsilon", 900, 700);
// Sens3D->SetGrid();
// gStyle->SetOptStat(kFALSE);
// SENS->Draw("colz");
// DPOT->Draw("hist same");

// TCanvas *SENSRAD = new TCanvas("SENSRAD", "PADME X17 Sensitivity R_{max} scan", 900, 700);
// SENSRAD->SetGrid();
// gStyle->SetOptStat(kFALSE);
// SENSrad->Draw("HIST");

// TCanvas *CL90_1D = new TCanvas("CL90_1D", "PADME X17 90% Confidence level as a function of N-PoT", 900, 700);
// CL90_1D->SetGrid();
// gStyle->SetOptStat(kFALSE);
// CL90->Draw("HIST");

// TCanvas *exclusion_X17_1D = new TCanvas("exclusion_X17_1D", "Exclusion plot #epsilon_{X17}^{2} as a function of N-PoT", 900, 700);
// //exclusion_X17_1D->SetGrid();
// gStyle->SetOptStat(kFALSE);
// gPad->SetFrameFillColor(kGreen-10);
// exclusion_X17->Draw("HIST");
// NA64_limit->Draw("HIST SAME");
// gPad->SetLogy();


// BhabhaOut->cd();
// h1->Write();
// h2->Write();
// h3->Write();
// h4->Write();
// h5->Write();
// h6->Write();
// h7->Write();
// h8->Write();
// h1Full->Write();
// h2Full->Write();
// h3Full->Write();
// h4Full->Write();
// h1g->Write();
// h2g->Write();
// h3g->Write();
// h4g->Write();
// htheta1->Write();
// htheta2->Write();
// hthetaFull->Write();
// hthetag->Write();




// BhabhaOut->Close();



//OUTPUT FILES
/*C1->SaveAs("S_Final3P.png");
C2->SaveAs("T_Final3P.png");
C1g->SaveAs("AA_Final3P.png");
C3P->SaveAs("Final3p_distribution.png");
CHECK1->SaveAs("Energy_Conservation_Check.png");
Ctheta->SaveAs("Theta_FinalParticles.png");
C3->SaveAs("S_FinalEnergy.png");
C4->SaveAs("T_FinalEnergy.png");
C2g->SaveAs("AA_FinalEnergy.png");
C5->SaveAs("Occupancy.png");
C6->SaveAs("geom_occupancy.png");
C7->SaveAs("geom_energy_Occupancy.png");
Acc3D_a->SaveAs("ACC_Scan_a.png");
Acc3D_b->SaveAs("ACC_Scan_b.png");
Acc1D_S->SaveAs("ACC_Schan_radius_Scan.png");
Acc1D_T->SaveAs("ACC_Tchan_radius_Scan.png");
Acc1D_AA->SaveAs("ACC_AA_radius_Scan.png");
Sens3D->SaveAs("SensitivityX17.png");
CL90_1D->SaveAs("CL90X17_stat.png");
exclusion_X17_1D->SaveAs("gve_exclusion.png");*/
}


