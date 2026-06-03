#include <TMath.h>
#include <TMinuit.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

//====================================================
// MEASURED SPOTS [mm]
//====================================================

double s1_meas = 0.60;
double s2_meas = 9.0;
double s3_meas = 12.0;
double s4_meas = 11.25;

double e1 = 0.05;
double e2 = 0.1;
double e3 = 0.5;
double e4 = 0.1;

//====================================================
// BEAM
//====================================================

double pbeam    = 270.3; // MeV/c
double me       = 0.511; // MeV
double Ebeam    = sqrt(pbeam*pbeam + me*me);
double beta_rel = pbeam/Ebeam;

//====================================================
// GLOBAL POSITIONS [mm]
//====================================================

double fitStartZ  = -732.0;   // Twiss parameters are fitted here
double plotStartZ = -2000.0;  // plot starts upstream
double plotEndZ   = 4000.0;
double dzPlot     = 1.0;      // plotting step [mm]

//====================================================

struct SigmaMatrix {
    double xx;    // mm^2
    double xxp;   // mm rad
    double xpxp;  // rad^2
};

struct Element {
    string name;
    double z;        // absolute position [mm]
    double xOverX0;  // material thickness / X0
};

//====================================================
// ABSOLUTE GEOMETRY [mm]
//====================================================

vector<Element> geometry = {
    {"target",   -732.0, 0.01/12.1},   // 0.1 mm target
    {"flange",   2290.0, 0.25/19.3},   // 2.5 mm flange
    {"L1",       2357.0, 0.0289},      // plane 1 mm
    {"L2",       2458.0, 0.0289},      // plane 2 mm
    {"ecal_cog", 2629.0, 0.0},
    {"tmm",      3383.0, 0.0}
};

//====================================================
// TWISS -> SIGMA
// emit in mm rad
// beta in mm
//====================================================

SigmaMatrix TwissToSigma(double emit, double alpha, double beta)
{
    SigmaMatrix s;

    double gamma = (1.0 + alpha*alpha)/beta;

    s.xx   = emit * beta;
    s.xxp  = -emit * alpha;
    s.xpxp = emit * gamma;

    return s;
}

//====================================================
// DRIFT
// L in mm
//====================================================

SigmaMatrix Drift(SigmaMatrix s, double L)
{
    SigmaMatrix out;

    out.xx   = s.xx + 2.0*L*s.xxp + L*L*s.xpxp;
    out.xxp  = s.xxp + L*s.xpxp;
    out.xpxp = s.xpxp;

    return out;
}

//====================================================
// MULTIPLE SCATTERING
//====================================================

void AddScatter(SigmaMatrix &s, double xOverX0)
{
    if(xOverX0 <= 0) return;

    double corr = 1.0 + 0.038*TMath::Log(xOverX0);
    if(corr < 0) corr = 0;

    double theta0 = (13.6/(beta_rel*pbeam)) * sqrt(xOverX0) * corr;

    s.xpxp += theta0*theta0;
}

//====================================================
// TRANSPORT FROM zNow TO zTarget
// Applies all scatterers crossed between the two positions.
//====================================================

bool TransportToZ(SigmaMatrix &s, double &zNow, double zTarget)
{
    if(zTarget < zNow) {
        s = Drift(s, zTarget - zNow);
        zNow = zTarget;
        return s.xx > 0;
    }

    for(const auto &el : geometry) {

        if(el.z <= zNow) continue;
        if(el.z > zTarget) break;

        s = Drift(s, el.z - zNow);
        zNow = el.z;

        if(s.xx <= 0) return false;

        AddScatter(s, el.xOverX0);
    }

    s = Drift(s, zTarget - zNow);
    zNow = zTarget;

    return s.xx > 0;
}

//====================================================
// CHI2
//====================================================

void fcn(Int_t &npar, Double_t *, Double_t &f, Double_t *par, Int_t){
    
    double emit  = par[0]; // mm rad
    double alpha = par[1];
    double beta  = par[2]; // mm

    SigmaMatrix s = TwissToSigma(emit, alpha, beta);

    if(s.xx <= 0) {
        f = 1e30;
        return;
    }

    double zNow = fitStartZ;

    //------------------------------------------------
    // sigma1 at TARGET
    //------------------------------------------------

    double sigma1 = sqrt(s.xx);

    //------------------------------------------------
    // cross target material
    //------------------------------------------------

    AddScatter(s, 0.01/12.1);

    //------------------------------------------------
    // sigma2 at PLANE 1 = L1
    //------------------------------------------------

    if(!TransportToZ(s, zNow, 2357.0)) {
        f = 1e30;
        return;
    }

    double sigma2 = sqrt(s.xx);

    //------------------------------------------------
    // sigma3 at ECAL COG
    // this automatically crosses L2 material on the way
    //------------------------------------------------

    if(!TransportToZ(s, zNow, 2629.0)) {
        f = 1e30;
        return;
    }

    double sigma3 = sqrt(s.xx);

    //------------------------------------------------
    // sigma4 at TMM
    //------------------------------------------------

    if(!TransportToZ(s, zNow, 3383.0)) {
        f = 1e30;
        return;
    }

    double sigma4 = sqrt(s.xx);

    //------------------------------------------------
    // CHI2: target - plane1 - cog - tmm
    //------------------------------------------------

    double chi2 = 0.0;

    chi2 += pow((sigma1 - s1_meas)/e1, 2);
    chi2 += pow((sigma2 - s2_meas)/e2, 2);
    chi2 += pow((sigma3 - s3_meas)/e3, 2);
    chi2 += pow((sigma4 - s4_meas)/e4, 2);

    f = chi2;
}

//====================================================
// MAIN
//====================================================

void fitTwiss()
{
    TMinuit minuit(3);
    minuit.SetFCN(fcn);

    //------------------------------------------------
    // PARAMETERS
    //------------------------------------------------

    minuit.DefineParameter(0, "emit",  1e-3,   1e-5, 1e-8, 1e3); // mm rad
    minuit.DefineParameter(1, "alpha", 0.0,    0.01, -100, 100);
    minuit.DefineParameter(2, "beta",  1000.0, 1.0,  1.0,  1e6); // mm

    //------------------------------------------------
    // MINIMIZATION
    //------------------------------------------------

    minuit.Migrad();

    double emit, emitErr;
    double alpha, alphaErr;
    double beta, betaErr;

    minuit.GetParameter(0, emit, emitErr);
    minuit.GetParameter(1, alpha, alphaErr);
    minuit.GetParameter(2, beta, betaErr);

    cout << "\n===== FIT RESULTS =====\n";
    cout << "emit  = " << emit  << " mm rad\n";
    cout << "alpha = " << alpha << "\n";
    cout << "beta  = " << beta  << " mm\n";

    //------------------------------------------------
    // TRACK EVOLUTION FOR PLOT
    //------------------------------------------------

    SigmaMatrix s = TwissToSigma(emit, alpha, beta);

    vector<double> zVec;
    vector<double> sigVec;
    vector<double> betaVec;

    //------------------------------------------------
    // Back-propagate from fitStartZ to plotStartZ
    // This is only for plotting upstream of the fit point.
    //------------------------------------------------

    double z = fitStartZ;

    if(plotStartZ < fitStartZ) {
        s = Drift(s, plotStartZ - fitStartZ);
        z = plotStartZ;
    }

    double betaMin = 1e30;
    double zBetaMin = 0.0;

    while(z <= plotEndZ)
    {
        if(s.xx <= 0) break;

        double sigma = sqrt(s.xx);
        double betaFunc = s.xx / emit;

        zVec.push_back(z);
        sigVec.push_back(sigma);      // mm
        betaVec.push_back(betaFunc);  // mm

        if(betaFunc < betaMin) {
            betaMin = betaFunc;
            zBetaMin = z;
        }

        double zNext = z + dzPlot;

        TransportToZ(s, z, zNext);
    }

    //------------------------------------------------
    // PRINT WAIST
    //------------------------------------------------

    cout << "\n===== WAIST =====\n";
    cout << "Minimum beta = " << betaMin << " mm\n";
    cout << "Waist position z = " << zBetaMin << " mm\n";

    //------------------------------------------------
    // PLOTS
    //------------------------------------------------

    int N = zVec.size();

    TGraph *gSigma = new TGraph(N, &zVec[0], &sigVec[0]);
    TGraph *gBeta  = new TGraph(N, &zVec[0], &betaVec[0]);

    TCanvas *c1 = new TCanvas("c1", "Beam Transport", 1200, 500);
    c1->Divide(2,1);

    c1->cd(1);
    gSigma->SetLineWidth(2);
    gSigma->SetLineColor(kBlue);
    gSigma->SetTitle("Beam Envelope;z [mm];#sigma_{x} [mm]");
    gSigma->Draw("AL");

    c1->cd(2);
    gBeta->SetLineWidth(2);
    gBeta->SetLineColor(kRed);
    gBeta->SetTitle("Beta Function;z [mm];#beta [mm]");
    gBeta->Draw("AL");

    c1->Update();
}