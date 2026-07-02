#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <array>

#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TMath.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TString.h"
#include "TF1.h"
#include "TMinuit.h"
#include "TGraphErrors.h"
#include "TRandom3.h"
#include "TCanvas.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TLegend.h"
#include "TFitter.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TLine.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompSVD.h"

using namespace std;

#define NTUPLE_N_BOARDS 2
#define NTUPLE_N_CHANNELS 32
#define BLOCKSIZE 100

// ADC <-> Physics mapping (user-provided)
int ADCsToPhysics[NTUPLE_N_CHANNELS] = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// LG reference
const int LG_BRD = 0;
const int LG_CH  = 31;
const int NCH = 16; // number of channels in one direction
const double delta_strip = 0.850; // size of strip in mm

static vector<double> XcalibConst(16, 1.0);
static vector<double> XcalibErr(16, 0.0);

const int expected_centerX_phys = 8;   
const int expected_centerY_phys = 9;

struct GoodnessOfFit {
    double chi2;
    double edm;
    double errdef;
    int nvpar;
    int nparx;
    int status;
};

// Struct to store results
struct RunEventCharge {
    int run;
    vector<int> eventNumber;
    int centerXphys, centerYphys;
    int centerXadc, centerYadc;

    // Calibrated X target charges
    vector<array<double,3>> XqTar;
    vector<array<double,3>> errXqTar; 
    // RAW X and Y target charges (no calibration)
    vector<array<double,3>> XqTar_raw, YqTar_raw;
    vector<array<double,3>> errXqTar_raw, errYqTar_raw;

    // LG charges
    vector<double> QLG, errQLG;

    // X target charge (raw and calib) as a function of periods
    vector<double> QXTot_target_raw, QXTot_target_raw_err;
    vector<double> QXTot_target_cal, QXTot_target_cal_err;
    // ratios of target X-Target charge to LG charge
    vector<double> QXratio_raw, QXratio_raw_err;
    vector<double> QXratio_cal, QXratio_cal_err;

    // Y target charge raw as a function of periods
    vector<double> QYTot_target_raw, QYTot_target_raw_err;
    // ratios of target Y-Target charge to LG charge
    vector<double> QYratio_raw, QYratio_raw_err;

    // Raw X and Y + Calibrated X fit results
    vector<array<double,3>> coeffsX_raw, coeffsY_raw;
    vector<array<double,3>> coeffsX_raw_err, coeffsY_raw_err;
    vector<array<double,3>> coeffsX; 
    vector<array<double,3>> coeffsX_err; 
    vector<GoodnessOfFit> gofX; 
    vector<GoodnessOfFit> gofX_raw, gofY_raw;
    
    // weighted-mean X position (mm) per period, raw+cal
    vector<double> Xpos_wmean_raw, Xpos_wmean_raw_err;
    vector<double> Xpos_wmean_cal, Xpos_wmean_cal_err;

    // weighted-mean Y position (mm) per period, raw
    vector<double> Ypos_wmean_raw, Ypos_wmean_raw_err;
    
    // total charge entire run 
    double totalQLG, totalQLG_err;
    // total charge entire run - X
    array<double,3> totalQXTar_cal, totalQXTar_cal_err;
    array<double,3> totalQXTar_raw, totalQXTar_raw_err;
    array<double,3> coeffXTot_raw, coeffXTot_raw_err;
    array<double,3> coeffXTot_cal, coeffXTot_cal_err;
    GoodnessOfFit gofTotX_raw, gofTotX_cal;

    // total charge entire run - Y
    array<double,3> totalQYTar_raw, totalQYTar_raw_err;
    array<double,3> coeffYTot_raw, coeffYTot_raw_err;
    GoodnessOfFit gofTotY_raw;

};


// Helper: mean and unbiased sample stddev
void mean_and_std(const vector<double>& v, double &mean, double &sigma) {
    int n = v.size();
    if (n==0) { mean=0; sigma=1; return; }
    double s=0;
    for(double x:v) s+=x;
    mean=s/n;
    double ss=0;
    for(double x:v) ss+=(x-mean)*(x-mean);
    sigma = (n>1) ? sqrt(ss/(n-1))/sqrt(n) : 1.0;  
}

// Helper: TGraphErrors cosmetics
int TGraphAttribute(TGraphErrors *Graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
    Graph->SetTitle(title);
    Graph->SetName(title);
    Graph->GetXaxis()->SetTitle(xlabel);
    Graph->GetYaxis()->SetTitle(ylabel);
    Graph->SetMarkerStyle(markerstyle);
    Graph->SetMarkerColor(color);
    return 0;
}

// globals for Minuit
static int g_nominal_strip;
static array<double,3> g_qTar;
static array<double,3> g_qTar_err;

//  FCN 
void fcn(int &npar, double *grad, double &fval, double *par, int iflag){

    double C2 = 0.0;

    // int i = g_nominal_strip; 
    auto &q = g_qTar; // [qm1, q0, qp1]
    auto &q_err = g_qTar_err; // [err_qm1, err_q0, err_qp1]
    for(int iq = 0; iq < 3; iq++){
        // cout << "iq: " << iq << " q: " << q[iq] << " q_err: " << q_err[iq] << endl;
        double z_left = ((-1 + iq - delta_strip/2) - par[1])/par[2];
        double z_right = ((-1 + iq + delta_strip/2) - par[1])/par[2];
        double model = par[0]*(TMath::Erf(z_right/sqrt(2)) - TMath::Erf(z_left/sqrt(2)))/2;
        double sigma = q_err[iq]; 
        if(sigma<=0) sigma=1.0;
        double diff =  model - q[iq];
        C2 += diff*diff/(sigma*sigma);

    }

    fval = C2;
}

//  Do the Fit in one direction
void fitBeamraw(const int &nominal, const array<double,3> &qTar, const array<double,3> &qTar_err, 
                array<double,3> &coeffs, array<double,3> &coeffs_err, GoodnessOfFit &gof,
                const array<double,3>* initPars = nullptr)
{
    g_nominal_strip = nominal;
    g_qTar = qTar;
    g_qTar_err = qTar_err;

    // default starting values
    array<double,3> start = {300.0, 0., 0.5}; // A, mu, sigma

    // if external parameters provided, use them
    if(initPars) start = *initPars;

    TMinuit minuit(3); // A + mu + sigma
    minuit.SetPrintLevel(0);
    minuit.SetFCN(fcn);

    minuit.DefineParameter(0,     "A", start[0],    1,  0.0, 1000.0);
    minuit.DefineParameter(1,    "mu", start[1], 0.01, -8.0,    8.0);
    minuit.DefineParameter(2, "sigma", start[2], 0.01,    0,    5.0);

    minuit.Migrad();

    minuit.mnstat(gof.chi2, gof.edm, gof.errdef, gof.nvpar, gof.nparx, gof.status);

    // covariance matrix
    TMatrixD covMatrix(3,3);
    minuit.mnemat(covMatrix.GetMatrixArray(),3);

    for(int i=0;i<3;i++){
        minuit.GetParameter(i, coeffs[i], coeffs_err[i]);
    }
}

void fitBeamcal(const int &nominal, const array<double,3> &qTar, const array<double,3> &qTar_err, 
                array<double,3> &coeffs, array<double,3> &coeffs_err, GoodnessOfFit &gof,
                const array<double,3>* initPars = nullptr) 
{
    g_nominal_strip = nominal;
    g_qTar = qTar;
    g_qTar_err = qTar_err;

    // default starting values
    array<double,3> start = {800.0, 0., 1.5};

    // if external parameters provided, use them
    if(initPars) start = *initPars;
    
    TMinuit minuit(3); // A + mu + sigma
    minuit.SetPrintLevel(0);
    minuit.SetFCN(fcn);
    minuit.DefineParameter(0,     "A", start[0],    1, 0.0, 1800.0);
    minuit.DefineParameter(1,    "mu", start[1], 0.01, -8.0,   8.0);
    minuit.DefineParameter(2, "sigma", start[2], 0.01,    0,   5.0);

    minuit.Migrad();

    minuit.mnstat(gof.chi2, gof.edm, gof.errdef, gof.nvpar, gof.nparx, gof.status);

    // covariance matrix
    TMatrixD covMatrix(3,3);
    minuit.mnemat(covMatrix.GetMatrixArray(),3);

    for(int i=0;i<3;i++){
        minuit.GetParameter(i, coeffs[i], coeffs_err[i]);
    } 
}

// Check if channel is on edge
bool isEdgeChannel(int Phys_Channel) {
    return (Phys_Channel == 0 || Phys_Channel == 15 || Phys_Channel == 16 || Phys_Channel == 31);
}

// NEW: weighted-mean estimator using the 3 strip charges q[-1], q[0], q[1]
// mu = (-1*q[-1] + 0*q[0] + 1*q[+1]) / (q[-1]+q[0]+q[+1]) = (q[+1]-q[-1]) / sum
// error propagated with standard Gaussian propagation, assuming uncorrelated dq
bool weighted_mean_mu_3strips(const array<double,3>& q,
                              const array<double,3>& dq,
                              double &mu, double &dmu)
{
    const double qm1 = q[0], q0 = q[1], qp1 = q[2];
    const double D = qm1 + q0 + qp1;
    if (D <= 0) { mu = 0.0; dmu = 0.0; return false; }

    const double N = (-1.0)*qm1 + (1.0)*qp1; // q(+1)-q(-1)
    mu = N / D;

    // derivatives
    const double D2 = D*D;
    const double dmu_dqm1 = (-D - N) / D2;
    const double dmu_dq0  = (   - N) / D2;
    const double dmu_dqp1 = ( D - N) / D2;

    dmu = sqrt( dmu_dqm1*dmu_dqm1*dq[0]*dq[0]
              + dmu_dq0 *dmu_dq0 *dq[1]*dq[1]
              + dmu_dqp1*dmu_dqp1*dq[2]*dq[2] );
    return true;
}

double BeamPosXmm(double center_strip_phys, double mu) { 
    return (center_strip_phys + mu) * 1.0; // each strip = 1 mm
}

double BeamPosYmm(double center_strip_phys, double mu) {
    return (center_strip_phys - 16 + mu) * 1.0; // Y strips start at phys 16
}

bool LoadCalibrationConstants(const string &filename) {

    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "Error: Cannot open calibration file " << filename << endl;
        return false;
    }

    for (int strip = 0; strip < 16; strip++) {
        if (!(infile >> XcalibConst[strip] >> XcalibErr[strip])) {
            cerr << "Error: Calibration file too short — expected 16 lines, got " 
                 << strip << endl;
            return false;
        }
    }

    infile.close();
    cout << "Calibration constants successfully loaded from " << filename << endl;
    return true;
}


// Calibrate one file
bool TargetChargePosition(int runNumber, const string &filePathPattern, RunEventCharge &RunEvQ){
    
    RunEvQ.run = runNumber;
    cout << "RunNumber: " << runNumber << endl;
    
    char fname[256];
    sprintf(fname, filePathPattern.c_str(), runNumber);
    TFile *f = TFile::Open(fname,"READ");
    if(!f || f->IsZombie()) { cerr << "Cannot open file " << fname << endl; return false; }

    TTree* t = (TTree*)f->Get("NTU");
    if(!t) { cerr << "Tree NTU not found in file " << fname << endl; f->Close(); return false; }

    static Int_t Nevent; 
    static Double_t QChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
    static Double_t QChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
    Int_t TrigMask;
    t->SetBranchStatus("*",0);
    t->SetBranchStatus("Nevent",1);
    t->SetBranchStatus("QChOld",1);
    t->SetBranchStatus("QChCumFixed",1);
    t->SetBranchStatus("TrigMask",1);
    
    t->SetBranchAddress("Nevent", &Nevent);
    t->SetBranchAddress("QChOld", QChOld);
    t->SetBranchAddress("QChCumFixed", QChCumFixed);
    t->SetBranchAddress("TrigMask", &TrigMask);

    Long64_t nentries = t->GetEntries();
    if(nentries <= 0) {
        cerr << "No entries in tree for run " << runNumber << endl;
        f->Close();
        return false;
    } 

    // accumulate total integrated charge per physics strip separately for X (phys 0..15) and Y (16..31)
    vector<double> totalX(16,0.0), totalY(16,0.0);
    for(Long64_t ev=0; ev<nentries; ++ev){
        t->GetEntry(ev);
        for(int adc=0; adc<NTUPLE_N_CHANNELS; ++adc) {
            int phys = ADCsToPhysics[adc];
            if(phys < 16) totalX[phys] += QChCumFixed[1][adc];
            else totalY[phys-16] += QChCumFixed[1][adc];
        }
    }

    // find maxima (physics indices)
    int centerX_phys = (int)(max_element(totalX.begin(), totalX.end()) - totalX.begin());
    int centerY_phys = (int)(max_element(totalY.begin(), totalY.end()) - totalY.begin()) + 16;

    // identify center position by finding max integrated charge in X and Y
    if(isEdgeChannel(centerX_phys) || isEdgeChannel(centerY_phys)) {
        cerr << "Edge channel: X=" << centerX_phys << " Y=" << centerY_phys << endl;
        return true;
    }

    // map to ADC
    int adc_X0 = PhysicsToADCs[centerX_phys];
    int adc_Y0 = PhysicsToADCs[centerY_phys];
    int adc_Xm1 = PhysicsToADCs[centerX_phys-1];
    int adc_Xp1 = PhysicsToADCs[centerX_phys+1];
    int adc_Ym1 = PhysicsToADCs[centerY_phys-1];
    int adc_Yp1 = PhysicsToADCs[centerY_phys+1];

    RunEvQ.centerXphys = centerX_phys;
    RunEvQ.centerYphys = centerY_phys;
    RunEvQ.centerXadc = adc_X0;
    RunEvQ.centerYadc = adc_Y0;
    cout << "Target (X,Y) position: (" << centerX_phys << ", " << centerY_phys << ")" << endl;
    cout << "Target (X_minus,X_plus) : (" << centerX_phys-1 << ", " << centerX_phys+1 << ")" << " - Target (Y_minus,Y_plus) : (" << centerY_phys-1 << ", " << centerY_phys+1 << ")" << endl;
    cout << "Target (X,Y) ADCs: (" << adc_X0 << ", " << adc_Y0 << ")" << endl;
    cout << "ADCs (X_minus,X_plus) : (" << adc_Xm1 << ", " << adc_Xp1 << ")" << " - Target (Y_minus,Y_plus) : (" << adc_Ym1 << ", " << adc_Yp1 << ")" << endl;

    vector<int> vNeve;
    vector<double> cur_QLG; 
    vector<double> blkQLG, blkQLG_err;
    vector<double> QLG, QLG_err;
    vector<array<double,3>> blkQX, blkQY;
    vector<array<double,3>> X_qTar_raw, err_X_qTar_raw;
    vector<array<double,3>> X_qTar, err_X_qTar;

    vector<array<double,3>> Y_qTar_raw, err_Y_qTar_raw;

    vector<array<double,3>> coeffX, coeffX_err;
    vector<array<double,3>> coeffX_raw, coeffX_raw_err;

    vector<array<double,3>> coeffY_raw, coeffY_raw_err;

    vector<double> QXTot_target_raw, QXTot_target_cal;
    vector<double> QXTot_target_raw_err, QXTot_target_cal_err;

    vector<double> QYTot_target_raw, QYTot_target_raw_err;
    
    vector<double> QXratio_raw, QXratio_cal;
    vector<double> QXratio_raw_err, QXratio_cal_err;

    vector<double> QYratio_raw, QYratio_raw_err;

    vector<GoodnessOfFit> gofX, gofX_raw; 
    vector<GoodnessOfFit> gofY_raw;
    // NEW: weighted-mean X position per period
    vector<double> Xpos_wmean_raw, Xpos_wmean_raw_err;
    vector<double> Xpos_wmean_cal, Xpos_wmean_cal_err;

    vector<double> Ypos_wmean_raw, Ypos_wmean_raw_err;

    double mQ = 0.;
    double sQ = 0.; 
    double qLG, dqLG;

    int chunk = 0;
    static int NCHUNKS = 10; // number of blocks to average over

    for(Long64_t ev=0; ev<nentries; ev++){
        t->GetEntry(ev);
        vNeve.push_back(Nevent);
        // cout << "iteration = " << ev << " - Nevent = " << Nevent << endl;
        if(TrigMask != 1) continue;
        // append to current block
        
        // append to current block (only LG charge)
        cur_QLG.push_back(QChOld[LG_BRD][LG_CH]);

        // when block full, compute block means and push to accumulated blocks
        if ((int)cur_QLG.size() == BLOCKSIZE) {

            chunk++;

            mean_and_std(cur_QLG, mQ, sQ);
            blkQLG.push_back(mQ);
            blkQLG_err.push_back(sQ);

            // x strips
            double rawXm1 = (QChCumFixed[1][adc_Xm1] > 0 ? QChCumFixed[1][adc_Xm1] : 0);
            double rawX0  = (QChCumFixed[1][adc_X0]  > 0 ? QChCumFixed[1][adc_X0]  : 0);
            double rawXp1 = (QChCumFixed[1][adc_Xp1] > 0 ? QChCumFixed[1][adc_Xp1] : 0);
            // y strips
            double rawYm1 = (QChCumFixed[1][adc_Ym1] > 0 ? QChCumFixed[1][adc_Ym1] : 0);
            double rawY0  = (QChCumFixed[1][adc_Y0]  > 0 ? QChCumFixed[1][adc_Y0]  : 0);
            double rawYp1 = (QChCumFixed[1][adc_Yp1] > 0 ? QChCumFixed[1][adc_Yp1] : 0);

            // store RAW in per-block container
            blkQX.push_back({rawXm1, rawX0, rawXp1});
            blkQY.push_back({rawYm1, rawY0, rawYp1});

            // Every NCHUNKS blocks -> compute averaged charge & fit
            if (chunk % NCHUNKS == 0) {

                size_t start = blkQLG.size() - NCHUNKS;

                vector<double> lastLG(blkQLG.begin() + start, blkQLG.end());
                vector<double> lastQXm1, lastQX0, lastQXp1;
                vector<double> lastQYm1, lastQY0, lastQYp1;

                for (size_t i = start; i < blkQX.size(); ++i) {
                    lastQXm1.push_back(blkQX[i][0]);
                    lastQX0 .push_back(blkQX[i][1]);
                    lastQXp1.push_back(blkQX[i][2]);
                    lastQYm1.push_back(blkQY[i][0]);
                    lastQY0 .push_back(blkQY[i][1]);
                    lastQYp1.push_back(blkQY[i][2]);

                }

                // LG block average
                mean_and_std(lastLG, qLG, dqLG);

                // RAW X & Y block averages (NO calibration yet)
                double qXm1_raw, qX0_raw, qXp1_raw;
                double dqXm1_raw, dqX0_raw, dqXp1_raw;
                double qYm1_raw, qY0_raw, qYp1_raw;
                double dqYm1_raw, dqY0_raw, dqYp1_raw;

                mean_and_std(lastQXm1, qXm1_raw, dqXm1_raw);
                mean_and_std(lastQX0,  qX0_raw,  dqX0_raw);
                mean_and_std(lastQXp1, qXp1_raw, dqXp1_raw);
                mean_and_std(lastQYm1, qYm1_raw, dqYm1_raw);
                mean_and_std(lastQY0,  qY0_raw,  dqY0_raw);
                mean_and_std(lastQYp1, qYp1_raw, dqYp1_raw);

                // --- X calibration constants for strips (phys indices 0..15)
                double C_m1   = XcalibConst[centerX_phys - 1];
                double C_0    = XcalibConst[centerX_phys];
                double C_p1   = XcalibConst[centerX_phys + 1];
                double dC_m1  = XcalibErr[centerX_phys - 1];
                double dC_0   = XcalibErr[centerX_phys];
                double dC_p1  = XcalibErr[centerX_phys + 1];

                // --- CALIBRATED means
                double qXm1_cal = qXm1_raw * C_m1;
                double qX0_cal  = qX0_raw  * C_0;
                double qXp1_cal = qXp1_raw * C_p1;

                // --- Propagate uncertainties (stat ⊕ calibration)
                double dqXm1_cal = sqrt( pow(dqXm1_raw * C_m1, 2) + pow(qXm1_raw * dC_m1, 2) );
                double dqX0_cal  = sqrt( pow(dqX0_raw  * C_0,  2) + pow(qX0_raw  * dC_0,  2) );
                double dqXp1_cal = sqrt( pow(dqXp1_raw * C_p1, 2) + pow(qXp1_raw * dC_p1, 2) );

                // --- Store RAW X and Y for later comparison
                array<double,3> qx_raw     = {qXm1_raw, qX0_raw, qXp1_raw};
                array<double,3> qx_raw_err = {dqXm1_raw, dqX0_raw, dqXp1_raw};
                array<double,3> qy_raw     = {qYm1_raw, qY0_raw, qYp1_raw};
                array<double,3> qy_raw_err = {dqYm1_raw, dqY0_raw, dqYp1_raw};

                // --- Store Calibrated X for later comparison
                array<double,3> qx_cal     = {qXm1_cal, qX0_cal, qXp1_cal};
                array<double,3> qx_cal_err = {dqXm1_cal, dqX0_cal, dqXp1_cal};

                X_qTar_raw.push_back(qx_raw);
                err_X_qTar_raw.push_back(qx_raw_err);
                Y_qTar_raw.push_back(qy_raw);
                err_Y_qTar_raw.push_back(qy_raw_err);

                X_qTar.push_back(qx_cal);
                err_X_qTar.push_back(qx_cal_err);

                 // --- NEW: weighted-mean position (raw & calibrated)
                double muX_w_raw=0.0, dmuX_w_raw=0.0;
                double muX_w_cal=0.0, dmuX_w_cal=0.0;

                double muY_w_raw=0.0, dmuY_w_raw=0.0;

                // if sum is 0, function returns false -> keep mu=0, dmu=0
                weighted_mean_mu_3strips(qx_raw, qx_raw_err, muX_w_raw, dmuX_w_raw);
                weighted_mean_mu_3strips(qx_cal, qx_cal_err, muX_w_cal, dmuX_w_cal);

                weighted_mean_mu_3strips(qy_raw, qy_raw_err, muY_w_raw, dmuY_w_raw);

                // convert to absolute X in mm, consistent with fit convention:
                // X(mm) = Xstrip + mu  (with 1 mm pitch)
                Xpos_wmean_raw.push_back( BeamPosXmm(centerX_phys, muX_w_raw) );
                Xpos_wmean_raw_err.push_back( fabs(dmuX_w_raw) * 1.0 );

                Xpos_wmean_cal.push_back( BeamPosXmm(centerX_phys, muX_w_cal) );
                Xpos_wmean_cal_err.push_back( fabs(dmuX_w_cal) * 1.0 );

                Ypos_wmean_raw.push_back( BeamPosYmm(centerY_phys, muY_w_raw) );
                Ypos_wmean_raw_err.push_back( fabs(dmuY_w_raw) * 1.0 );

                QLG.push_back(qLG);
                QLG_err.push_back(dqLG);

                // Total target charge RAW
                double QXTar_raw = qXm1_raw + qX0_raw + qXp1_raw;
                double dQXTar_raw = sqrt( dqXm1_raw*dqXm1_raw + dqX0_raw*dqX0_raw + dqXp1_raw*dqXp1_raw);

                double QYTar_raw = qYm1_raw + qY0_raw + qYp1_raw;
                double dQYTar_raw = sqrt( dqYm1_raw*dqYm1_raw + dqY0_raw*dqY0_raw + dqYp1_raw*dqYp1_raw);

                // Total target charge CAL
                double QXTar_cal = qXm1_cal + qX0_cal + qXp1_cal;
                double dQXTar_cal = sqrt( dqXm1_cal*dqXm1_cal + dqX0_cal*dqX0_cal + dqXp1_cal*dqXp1_cal);

                // Ratios
                double RX_raw = (QLG.back() > 0 ? QXTar_raw / QLG.back() : 0);
                double dRX_raw = (RX_raw > 0 ? RX_raw * sqrt( pow(dQXTar_raw/QXTar_raw, 2) + pow(dqLG/QLG.back(), 2) ) : 0);

                double RY_raw = (QLG.back() > 0 ? QYTar_raw / QLG.back() : 0);
                double dRY_raw = (RY_raw > 0 ? RY_raw * sqrt( pow(dQYTar_raw/QYTar_raw, 2) + pow(dqLG/QLG.back(), 2) ) : 0);

                double RX_cal = (QLG.back() > 0 ? QXTar_cal / QLG.back() : 0);
                double dRX_cal = (RX_cal > 0 ? RX_cal * sqrt( pow(dQXTar_cal/QXTar_cal, 2) + pow(dqLG/QLG.back(), 2) ) : 0);
                
                QXTot_target_raw.push_back(QXTar_raw);
                QXTot_target_raw_err.push_back(dQXTar_raw);
                QYTot_target_raw.push_back(QYTar_raw);
                QYTot_target_raw_err.push_back(dQYTar_raw);
                QXTot_target_cal.push_back(QXTar_cal);
                QXTot_target_cal_err.push_back(dQXTar_cal);
                // Store
                QXratio_raw.push_back(RX_raw);
                QXratio_raw_err.push_back(dRX_raw);
                QYratio_raw.push_back(RY_raw);
                QYratio_raw_err.push_back(dRY_raw);
                QXratio_cal.push_back(RX_cal);
                QXratio_cal_err.push_back(dRX_cal);

                // --- Two fits in X: RAW and CAL
                array<double,3> lastCX_raw,  lastCX_raw_err;
                array<double,3> lastCY_raw,  lastCY_raw_err;
                array<double,3> lastCX_cal,  lastCX_cal_err;
                GoodnessOfFit lastGX_raw, lastGX_cal; 
                GoodnessOfFit lastGY_raw;

                // cout << "charge qx_raw " << qXm1_raw << " " << qX0_raw << " " << qXp1_raw << endl;
                // cout << "charge qx_cal " << qXm1_cal << " " << qX0_cal << " " << qXp1_cal << endl; 
                // cout << "charge qx_raw_err " << dqXm1_raw << " " << dqX0_raw << " " << dqXp1_raw << endl;
                // cout << "charge qx_cal_err " << dqXm1_cal << " " << dqX0_cal << " " << dqXp1_cal << endl; 

                // ---------- RAW FIT ----------
                fitBeamraw(centerX_phys, qx_raw, qx_raw_err, lastCX_raw, lastCX_raw_err, lastGX_raw);
                if(lastGX_raw.status > 2){
                    if(coeffX_raw.empty()){
                        fitBeamraw(centerX_phys, qx_raw, qx_raw_err, lastCX_raw, lastCX_raw_err, lastGX_raw);
                    }
                    else{
                        fitBeamraw(centerX_phys, qx_raw, qx_raw_err, lastCX_raw, lastCX_raw_err, lastGX_raw, &coeffX_raw.back());
                        if(lastGX_raw.status > 3){
                            lastCX_raw = {0.,10.,10.};
                            lastCX_raw_err = {0.,10.,10.};
                        }
                    }
                } 
                fitBeamraw(centerY_phys, qy_raw, qy_raw_err, lastCY_raw, lastCY_raw_err, lastGY_raw);
                if(lastGY_raw.status > 2){
                    if(coeffY_raw.empty()){
                        fitBeamraw(centerY_phys, qy_raw, qy_raw_err, lastCY_raw, lastCY_raw_err, lastGY_raw);
                    }
                    else{
                        fitBeamraw(centerY_phys, qy_raw, qy_raw_err, lastCY_raw, lastCY_raw_err, lastGY_raw, &coeffY_raw.back());
                        if(lastGY_raw.status > 3){
                            lastCY_raw = {0.,10.,10.};
                            lastCY_raw_err = {0.,10.,10.};
                        }
                    }
                }   
                          
                // ---------- CAL FIT ----------
                fitBeamcal(centerX_phys, qx_cal, qx_cal_err, lastCX_cal, lastCX_cal_err, lastGX_cal);
                if(lastGX_cal.status > 2){
                    if(coeffX.empty()) {
                        fitBeamcal(centerX_phys, qx_cal, qx_cal_err, lastCX_cal, lastCX_cal_err, lastGX_cal);
                    }
                    else{
                        fitBeamcal(centerX_phys, qx_cal, qx_cal_err, lastCX_cal, lastCX_cal_err, lastGX_cal, &coeffX.back());
                        if(lastGX_cal.status > 3){
                            lastCX_cal = {0.,10.,10.};
                            lastCX_cal_err = {0.,10.,10.};
                        }
                    }
                }   

                coeffX_raw.push_back(lastCX_raw);
                coeffX_raw_err.push_back(lastCX_raw_err);
                gofX_raw.push_back(lastGX_raw);

                coeffY_raw.push_back(lastCY_raw);
                coeffY_raw_err.push_back(lastCY_raw_err);
                gofY_raw.push_back(lastGY_raw);

                coeffX.push_back(lastCX_cal);
                coeffX_err.push_back(lastCX_cal_err);
                gofX.push_back(lastGX_cal);

            }
            cur_QLG.clear();
        }
    }
    double QLG_tot, dQLG_tot;
    mean_and_std(blkQLG, QLG_tot, dQLG_tot);

    array<double,3> QXTARtot_raw, dQXTARtot_raw;
    vector<double> QXm1_raw(X_qTar_raw.size()), QX0_raw(X_qTar_raw.size()), QXp1_raw(X_qTar_raw.size());
    transform(X_qTar_raw.begin(), X_qTar_raw.end(), QXm1_raw.begin(), [](const array<double,3>& a){ return a[0]; });
    transform(X_qTar_raw.begin(), X_qTar_raw.end(), QX0_raw.begin(), [](const array<double,3>& a){ return a[1]; });
    transform(X_qTar_raw.begin(), X_qTar_raw.end(), QXp1_raw.begin(), [](const array<double,3>& a){ return a[2]; });

    mean_and_std(QXm1_raw, QXTARtot_raw[0], dQXTARtot_raw[0]);
    mean_and_std(QX0_raw, QXTARtot_raw[1], dQXTARtot_raw[1]);
    mean_and_std(QXp1_raw, QXTARtot_raw[2], dQXTARtot_raw[2]);

    array<double,3> QYTARtot_raw, dQYTARtot_raw;
    vector<double> QYm1_raw(Y_qTar_raw.size()), QY0_raw(Y_qTar_raw.size()), QYp1_raw(Y_qTar_raw.size());
    transform(Y_qTar_raw.begin(), Y_qTar_raw.end(), QYm1_raw.begin(), [](const array<double,3>& a){ return a[0]; });
    transform(Y_qTar_raw.begin(), Y_qTar_raw.end(), QY0_raw.begin(), [](const array<double,3>& a){ return a[1]; });
    transform(Y_qTar_raw.begin(), Y_qTar_raw.end(), QYp1_raw.begin(), [](const array<double,3>& a){ return a[2]; });

    mean_and_std(QYm1_raw, QYTARtot_raw[0], dQYTARtot_raw[0]);
    mean_and_std(QY0_raw, QYTARtot_raw[1], dQYTARtot_raw[1]);
    mean_and_std(QYp1_raw, QYTARtot_raw[2], dQYTARtot_raw[2]);

    array<double,3> QXTARtot_cal, dQXTARtot_cal;
    vector<double> qXm1_cal(X_qTar.size()), qX0_cal(X_qTar.size()), qXp1_cal(X_qTar.size());
    transform(X_qTar.begin(), X_qTar.end(), qXm1_cal.begin(), [](const array<double,3>& a){ return a[0]; });
    transform(X_qTar.begin(), X_qTar.end(), qX0_cal.begin(), [](const array<double,3>& a){ return a[1]; });
    transform(X_qTar.begin(), X_qTar.end(), qXp1_cal.begin(), [](const array<double,3>& a){ return a[2]; });

    mean_and_std(qXm1_cal, QXTARtot_cal[0], dQXTARtot_cal[0]);
    mean_and_std(qX0_cal, QXTARtot_cal[1], dQXTARtot_cal[1]);
    mean_and_std(qXp1_cal, QXTARtot_cal[2], dQXTARtot_cal[2]);

    // Fit total charge with global fit to check consistency and extract global fit parameters
    array<double,3> CXTot_raw,  CXTot_raw_err;
    array<double,3> CYTot_raw,  CYTot_raw_err;
    array<double,3> CXTot_cal,  CXTot_cal_err;
    GoodnessOfFit GXTot_raw, GXTot_cal; 
    GoodnessOfFit GYTot_raw;
    fitBeamraw(centerX_phys, QXTARtot_raw, dQXTARtot_raw, CXTot_raw, CXTot_raw_err, GXTot_raw);
    fitBeamraw(centerY_phys, QYTARtot_raw, dQYTARtot_raw, CYTot_raw, CYTot_raw_err, GYTot_raw);
    fitBeamcal(centerX_phys, QXTARtot_cal, dQXTARtot_cal, CXTot_cal, CXTot_cal_err, GXTot_cal);

    // Store in struct
    RunEvQ.eventNumber = vNeve;
    //LG
    RunEvQ.QLG = QLG;
    RunEvQ.errQLG = QLG_err; 
    // raw X and Y
    RunEvQ.XqTar_raw = X_qTar_raw;       
    RunEvQ.errXqTar_raw = err_X_qTar_raw;
    RunEvQ.YqTar_raw = Y_qTar_raw;       
    RunEvQ.errYqTar_raw = err_Y_qTar_raw;
    // calibrated X
    RunEvQ.XqTar = X_qTar;               
    RunEvQ.errXqTar = err_X_qTar;
    // Q tot raw X and Y
    RunEvQ.QXTot_target_raw = QXTot_target_raw;
    RunEvQ.QXTot_target_raw_err = QXTot_target_raw_err;
    RunEvQ.QYTot_target_raw = QYTot_target_raw;
    RunEvQ.QYTot_target_raw_err = QYTot_target_raw_err;
    // Q tot calibrated X
    RunEvQ.QXTot_target_cal = QXTot_target_cal;
    RunEvQ.QXTot_target_cal_err = QXTot_target_cal_err;
    // charge ratio raw X and Y
    RunEvQ.QXratio_raw = QXratio_raw;
    RunEvQ.QXratio_raw_err = QXratio_raw_err;
    RunEvQ.QYratio_raw = QYratio_raw;
    RunEvQ.QYratio_raw_err = QYratio_raw_err;
    // charge ratio calib X 
    RunEvQ.QXratio_cal = QXratio_cal;
    RunEvQ.QXratio_cal_err = QXratio_cal_err;
    // fit coefficients raw X adn Y 
    RunEvQ.coeffsX_raw = coeffX_raw;      
    RunEvQ.coeffsX_raw_err = coeffX_raw_err;
    RunEvQ.coeffsY_raw = coeffY_raw;      
    RunEvQ.coeffsY_raw_err = coeffY_raw_err;
    // fit coefficients calib X  
    RunEvQ.coeffsX = coeffX;              
    RunEvQ.coeffsX_err = coeffX_err;

    //position with weighted mean 
    // raw X adnd Y
    RunEvQ.Xpos_wmean_raw = Xpos_wmean_raw;
    RunEvQ.Xpos_wmean_raw_err = Xpos_wmean_raw_err;
    RunEvQ.Ypos_wmean_raw = Ypos_wmean_raw;
    RunEvQ.Ypos_wmean_raw_err = Ypos_wmean_raw_err;
    //calib X
    RunEvQ.Xpos_wmean_cal = Xpos_wmean_cal;
    RunEvQ.Xpos_wmean_cal_err = Xpos_wmean_cal_err;
    // fit quality
    RunEvQ.gofX_raw = gofX_raw; // X - raw
    RunEvQ.gofY_raw = gofY_raw; // Y - raw
    RunEvQ.gofX = gofX;         // X - calibrated

    //storing global fit results at the end
    //LG
    RunEvQ.totalQLG = QLG_tot;
    RunEvQ.totalQLG_err = dQLG_tot;
    // raw X adn Y
    RunEvQ.totalQXTar_raw = QXTARtot_raw;
    RunEvQ.totalQXTar_raw_err = dQXTARtot_raw;
    RunEvQ.totalQYTar_raw = QYTARtot_raw;
    RunEvQ.totalQYTar_raw_err = dQYTARtot_raw;
    // calib X
    RunEvQ.totalQXTar_cal = QXTARtot_cal;
    RunEvQ.totalQXTar_cal_err = dQXTARtot_cal;
    //coefficient raw X and Y
    RunEvQ.coeffXTot_raw = CXTot_raw;
    RunEvQ.coeffXTot_raw_err = CXTot_raw_err;
    RunEvQ.coeffYTot_raw = CYTot_raw;
    RunEvQ.coeffYTot_raw_err = CYTot_raw_err;
    // coefficeint calib X
    RunEvQ.coeffXTot_cal = CXTot_cal;
    RunEvQ.coeffXTot_cal_err = CXTot_cal_err;
    // quality fit 
    RunEvQ.gofTotX_raw = GXTot_raw; // X - raw
    RunEvQ.gofTotY_raw = GYTot_raw; // Y - raw
    RunEvQ.gofTotX_cal = GXTot_cal; // X - calibrated

    f->Close();
    return true;
}

// --- Main
int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <runNumber> <ebeam>\n"  << "Example: " << argv[0] << " 80555 298.5\n";
        return 1;
    }

    char* end1 = nullptr;
    long runL = std::strtol(argv[1], &end1, 10);
    if (*end1 != '\0' || runL <= 0) {
        std::cerr << "Error: runNumber must be a positive integer. Got: " << argv[1] << "\n";
        return 1;
    }
    int runNumber = static_cast<int>(runL);

    char* end2 = nullptr;
    double ebeam = std::strtod(argv[2], &end2);
    if (*end2 != '\0') {
        std::cerr << "Error: ebeam must be a number. Got: " << argv[2] << "\n";
        return 1;
    }

    std::cout << "runNumber=" << runNumber << " ebeam=" << ebeam << "\n";

    double ecalib = 298.5; // MeV

    // string calibFile = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/CalibTarget/outputCalibration/TargetCalibrationConst_NewCharge2.txt";
    string calibFile = "/home/mancinima/BeamMonitorRun4/padme-fw/TargetReco/CalibTarget/outputCalibration/TargetCalibrationConst_NewCharge2.txt";
    bool calibLoaded = LoadCalibrationConstants(calibFile);
    if (!calibLoaded) {
        cerr << "Calibration constants missing — continuing with unity gains!" << endl;
    }
    
    // string rootPattern = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco_NewRun4_100files/Reco_run_00%d.root";
    string rootPattern = "/home/mancinima/BeamMonitorRun4/padme-fw/TargetReco/outputReco/Reco_run_00%d.root";

    // defining the output txt file
    // ofstream OutputTxt(Form("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/CalibTarget/outputCalibration/Run4Monitor/TargetAnalysis_run_00%i_NewReco_100files.txt", runNumber));
    ofstream OutputTxt(Form("/home/mancinima/BeamMonitorRun4/padme-fw/TargetReco/CalibTarget/outputCalibration/Run4Monitor/TargetAnalysis_run_00%i.txt", runNumber));
    // defining the output root file
    TString outputFileName = "/home/mancinima/BeamMonitorRun4/padme-fw/TargetReco/CalibTarget/outputCalibration/Run4Monitor/NUOVO_run_00" + to_string(runNumber) + ".root";
    TFile* histoFile = new TFile(outputFileName,"RECREATE");
    if(!histoFile) {
        fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
        exit(1);
    }
    histoFile->cd();

    vector<int> evt;
    int Xstrip, Ystrip;
    // int Xadc, Yadc;
    vector<array<double,3>> TargetQX, TargetQX_raw, TargetQY_raw;
    vector<array<double,3>> err_TargetQX, err_TargetQX_raw, err_TargetQY_raw;
    vector<double> LGQ, err_LGQ;
    vector<double> QXTot_target_raw, QXTot_target_raw_err;
    vector<double> QYTot_target_raw, QYTot_target_raw_err;
    vector<double> QXTot_target_cal, QXTot_target_cal_err;
    vector<double> QXratio_raw, QXratio_raw_err;
    vector<double> QYratio_raw, QYratio_raw_err;
    vector<double> QXratio_cal, QXratio_cal_err;
    vector<array<double,3>> cX, cXraw, cYraw;
    vector<array<double,3>> cX_err, cXraw_err, cYraw_err;
    // NEW
    vector<double> Xpos_wmean_raw, Xpos_wmean_raw_err;
    vector<double> Ypos_wmean_raw, Ypos_wmean_raw_err;
    vector<double> Xpos_wmean_cal, Xpos_wmean_cal_err;

    double QLGtot, QLGtot_err;
    array<double,3> QXTarTot_raw, QXTarTot_raw_err;
    array<double,3> QYTarTot_raw, QYTarTot_raw_err;
    array<double,3> QXTarTot_cal, QXTarTot_cal_err;
    array<double,3> cXTot_raw, cXTot_raw_err;
    array<double,3> cYTot_raw, cYTot_raw_err;
    array<double,3> cXTot_cal, cXTot_cal_err;
    GoodnessOfFit gofXTot_raw, gofXTot_cal, gofYTot_raw;

    RunEventCharge evCharge;
    bool ok = TargetChargePosition(runNumber, rootPattern, evCharge);
    if(!ok){
        cerr << "Processing failed for run " << runNumber << endl;
        return 1;
    }
    Xstrip = evCharge.centerXphys;
    Ystrip = evCharge.centerYphys;
    // Xadc = evCharge.centerXadc;
    // Yadc = evCharge.centerYadc;
    evt = evCharge.eventNumber;
    TargetQX_raw = evCharge.XqTar_raw;
    TargetQY_raw = evCharge.YqTar_raw;
    TargetQX = evCharge.XqTar;
    err_TargetQX_raw = evCharge.errXqTar_raw;
    err_TargetQY_raw = evCharge.errYqTar_raw;
    err_TargetQX = evCharge.errXqTar;
    LGQ = evCharge.QLG;
    err_LGQ = evCharge.errQLG;
    QXTot_target_raw = evCharge.QXTot_target_raw;
    QXTot_target_raw_err = evCharge.QXTot_target_raw_err;
    QYTot_target_raw = evCharge.QYTot_target_raw;
    QYTot_target_raw_err = evCharge.QYTot_target_raw_err;
    QXTot_target_cal = evCharge.QXTot_target_cal;
    QXTot_target_cal_err = evCharge.QXTot_target_cal_err;
    QXratio_raw = evCharge.QXratio_raw;
    QXratio_raw_err = evCharge.QXratio_raw_err;
    QYratio_raw = evCharge.QYratio_raw;
    QYratio_raw_err = evCharge.QYratio_raw_err;
    QXratio_cal = evCharge.QXratio_cal;
    QXratio_cal_err = evCharge.QXratio_cal_err;
    cXraw = evCharge.coeffsX_raw;
    cYraw = evCharge.coeffsY_raw;
    cX = evCharge.coeffsX;
    cXraw_err = evCharge.coeffsX_raw_err;
    cYraw_err = evCharge.coeffsY_raw_err;   
    cX_err = evCharge.coeffsX_err;
    // NEW
    Xpos_wmean_raw = evCharge.Xpos_wmean_raw;
    Xpos_wmean_raw_err = evCharge.Xpos_wmean_raw_err;
    Ypos_wmean_raw = evCharge.Ypos_wmean_raw;
    Ypos_wmean_raw_err = evCharge.Ypos_wmean_raw_err;
    Xpos_wmean_cal = evCharge.Xpos_wmean_cal;
    Xpos_wmean_cal_err = evCharge.Xpos_wmean_cal_err;
    QLGtot = evCharge.totalQLG;
    QLGtot_err = evCharge.totalQLG_err;
    QXTarTot_raw = evCharge.totalQXTar_raw;
    QXTarTot_raw_err = evCharge.totalQXTar_raw_err;
    QYTarTot_raw = evCharge.totalQYTar_raw;
    QYTarTot_raw_err = evCharge.totalQYTar_raw_err;
    QXTarTot_cal = evCharge.totalQXTar_cal;
    QXTarTot_cal_err = evCharge.totalQXTar_cal_err;
    cXTot_raw = evCharge.coeffXTot_raw;
    cXTot_raw_err = evCharge.coeffXTot_raw_err;
    cYTot_raw = evCharge.coeffYTot_raw;
    cYTot_raw_err = evCharge.coeffYTot_raw_err;
    cXTot_cal = evCharge.coeffXTot_cal;
    cXTot_cal_err = evCharge.coeffXTot_cal_err;
    gofXTot_raw = evCharge.gofTotX_raw;
    gofYTot_raw = evCharge.gofTotY_raw;
    gofXTot_cal = evCharge.gofTotX_cal;

    // Number of fits done
    int Nfits = cX.size();
    if (Nfits == 0 || cYraw.size() != Nfits) {
        cerr << "No fits were performed — nothing to plot." << endl;
    } else {
        vector<double> beamPosX_mm(Nfits), beamPosX_mm_raw(Nfits), beamPosY_mm_raw(Nfits);
        vector<double> beamPosX_mm_err(Nfits), beamPosX_mm_raw_err(Nfits), beamPosY_mm_raw_err(Nfits);
        vector<double> beamWidthX(Nfits), beamWidthX_raw(Nfits), beamWidthY_raw(Nfits);
        vector<double> beamWidthX_err(Nfits), beamWidthX_raw_err(Nfits), beamWidthY_raw_err(Nfits);
        vector<double> chi2X(Nfits), chi2X_raw(Nfits), chi2Y_raw(Nfits);
        vector<double> fitstatusX(Nfits), fitstatusX_raw(Nfits), fitstatusY_raw(Nfits);
        vector<double> A_X(Nfits),  A_X_raw(Nfits),  A_Y_raw(Nfits);
        vector<double> A_X_err(Nfits),  A_X_raw_err(Nfits),  A_Y_raw_err(Nfits);
        vector<double> idx(Nfits);
        // NEW: weighted mean arrays (ensure size matches Nfits)
        vector<double> beamPosX_wm_cal(Nfits), beamPosX_wm_cal_err(Nfits);
        vector<double> beamPosX_wm_raw(Nfits), beamPosX_wm_raw_err(Nfits);
        vector<double> beamPosY_wm_raw(Nfits), beamPosY_wm_raw_err(Nfits);

        // Fill plotting arrays
        for (int i = 0; i < Nfits; i++) {
            idx[i] = i;

            double muX   = cX[i][1] - expected_centerX_phys;
            double dmuX  = fabs(cX_err[i][1]);
            double muX_raw  = cXraw[i][1] - expected_centerX_phys;
            double dmuX_raw = fabs(cXraw_err[i][1]);
            double muY_raw  = cYraw[i][1] - expected_centerY_phys;
            double dmuY_raw = fabs(cYraw_err[i][1]);

            // mirror the X coordinate to match PADME Frame convention(X increases towards the BTF door, with X=0 at the center of the central strip
            // Y increases vertically the BTF door, beam seen from the back of ECal)
            beamPosX_mm[i] = -((Xstrip + muX) * 1.0);
            beamPosX_mm_err[i] = dmuX * 1.0;
            beamPosX_mm_raw[i] = -((Xstrip + muX_raw) * 1.0);
            beamPosX_mm_raw_err[i] = dmuX_raw * 1.0;
            beamPosY_mm_raw[i] = (Ystrip + muY_raw) * 1.0;
            beamPosY_mm_raw_err[i] = dmuY_raw * 1.0;

            // NEW: weighted-mean from TargetChargePosition
            beamPosX_wm_cal[i] = (i < (int)Xpos_wmean_cal.size()) ? -(Xpos_wmean_cal[i] - expected_centerX_phys) : 0.0;
            beamPosX_wm_cal_err[i] = (i < (int)Xpos_wmean_cal_err.size()) ? Xpos_wmean_cal_err[i] : 0.0;
            beamPosX_wm_raw[i] = (i < (int)Xpos_wmean_raw.size()) ? -(Xpos_wmean_raw[i] - expected_centerX_phys) : 0.0;
            beamPosX_wm_raw_err[i] = (i < (int)Xpos_wmean_raw_err.size()) ? Xpos_wmean_raw_err[i] : 0.0;
            beamPosY_wm_raw[i] = (i < (int)Ypos_wmean_raw.size()) ? Ypos_wmean_raw[i] - expected_centerY_phys : 0.0;
            beamPosY_wm_raw_err[i] = (i < (int)Ypos_wmean_raw_err.size()) ? Ypos_wmean_raw_err[i] : 0.0;
        
            beamWidthX[i] = (fabs(cX[i][2]) > 1/sqrt(12) ?  fabs(cX[i][2]) : 1/sqrt(12)); // if fit returns width smaller than strip pitch/sqrt(12), set it to that value (uniform distribution assumption)
            beamWidthX_err[i] = (fabs(cX_err[i][2]) > 1/sqrt(12) ?  fabs(cX_err[i][2]) : 0);
            beamWidthX_raw[i] = (fabs(cXraw[i][2]) > 1/sqrt(12) ?  fabs(cXraw[i][2]) : 1/sqrt(12));
            beamWidthX_raw_err[i] = (fabs(cXraw_err[i][2]) > 1/sqrt(12) ?  fabs(cXraw_err[i][2]) : 0);
            beamWidthY_raw[i] = (fabs(cYraw[i][2]) > 1/sqrt(12) ?  fabs(cYraw[i][2]) : 1/sqrt(12));
            beamWidthY_raw_err[i] = (fabs(cYraw_err[i][2]) > 1/sqrt(12) ?  fabs(cYraw_err[i][2]) : 0);

            chi2X[i] = evCharge.gofX[i].chi2;
            chi2X_raw[i] = evCharge.gofX_raw[i].chi2;
            chi2Y_raw[i] = evCharge.gofY_raw[i].chi2;
            fitstatusX[i] = evCharge.gofX[i].status;
            fitstatusX_raw[i] = evCharge.gofX_raw[i].status;
            fitstatusY_raw[i] = evCharge.gofY_raw[i].status;
            A_X[i] = cX[i][0];
            A_X_err[i] = fabs(cX_err[i][0]);
            A_X_raw[i] = cXraw[i][0];
            A_X_raw_err[i] = fabs(cXraw_err[i][0]);
            A_Y_raw[i] = cYraw[i][0];
            A_Y_raw_err[i] = fabs(cYraw_err[i][0]);
        }

        // Create ROOT graphs
        TGraphErrors *gXTar = new TGraphErrors(Nfits, idx.data(), beamPosX_mm.data(), nullptr, nullptr);
        TGraphErrors *gXTar_raw = new TGraphErrors(Nfits, idx.data(), beamPosX_mm_raw.data(), nullptr, nullptr);
        TGraphErrors *gYTar_raw = new TGraphErrors(Nfits, idx.data(), beamPosY_mm_raw.data(), nullptr, nullptr);

        // NEW: weighted mean graphs
        TGraphErrors *gXTar_wmean = new TGraphErrors(Nfits, idx.data(), beamPosX_wm_cal.data(), nullptr, beamPosX_wm_cal_err.data());
        TGraphErrors *gXTar_wmean_raw = new TGraphErrors(Nfits, idx.data(), beamPosX_wm_raw.data(), nullptr, beamPosX_wm_raw_err.data());
        TGraphErrors *gYTar_wmean_raw = new TGraphErrors(Nfits, idx.data(), beamPosY_wm_raw.data(), nullptr, beamPosY_wm_raw_err.data());

        // tgraphs for target strip charges as a function of the periods
        TGraphErrors *gQ0_Xtarget_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQm1_Xtarget_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQp1_Xtarget_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQ0_Ytarget_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQm1_Ytarget_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQp1_Ytarget_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQ0_Xtarget_cal = new TGraphErrors(Nfits);
        TGraphErrors *gQm1_Xtarget_cal = new TGraphErrors(Nfits);
        TGraphErrors *gQp1_Xtarget_cal = new TGraphErrors(Nfits);

        // tgraphs for Q0/QLG as a function of the periods
        TGraphErrors *gQX0_QLG_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQY0_QLG_raw = new TGraphErrors(Nfits);
        TGraphErrors *gQX0_QLG_cal = new TGraphErrors(Nfits);

        //graph ratio charge fitted and QLG
        TGraphErrors *gChargeXratio_raw = new TGraphErrors(Nfits);
        TGraphErrors *gChargeYratio_raw = new TGraphErrors(Nfits);
        TGraphErrors *gChargeXratio_cal = new TGraphErrors(Nfits);

        //graph with energy corrected QLG
        TGraphErrors *gQLG_ECorr = new TGraphErrors(Nfits);
        TGraphErrors *gQXratio_raw_ECorr = new TGraphErrors(Nfits);
        TGraphErrors *gQYratio_raw_ECorr = new TGraphErrors(Nfits);
        TGraphErrors *gQXratio_cal_ECorr = new TGraphErrors(Nfits);
        TGraphErrors *gChargeXratio_raw_ECorr = new TGraphErrors(Nfits);
        TGraphErrors *gChargeYratio_raw_ECorr = new TGraphErrors(Nfits);
        TGraphErrors *gChargeXratio_cal_ECorr = new TGraphErrors(Nfits);
        

        // TH1 charge distributions
        TH1D *hChargeX_raw = new TH1D("hChargeX_raw", "Q_{X} raw [pC]", 1200, 0, 1200);
        TH1D *hQTarX0_raw = new TH1D("hQTarX0_raw", "Q_{X strip 0} raw [pC]", 1200, 0, 1200);
        TH1D *hQTarXm1_raw = new TH1D("hQTarXm1_raw", "Q_{X strip -1} raw [pC]", 1200, 0, 1200);
        TH1D *hQTarXp1_raw = new TH1D("hQTarXp1_raw", "Q_{X strip +1} raw [pC]", 1200, 0, 1200);
        TH1D *hChargeY_raw = new TH1D("hChargeY_raw", "Q_{Y} raw [pC]", 1200, 0, 1200);
        TH1D *hQTarY0_raw = new TH1D("hQTarY0_raw", "Q_{Y strip 0} raw [pC]", 1200, 0, 1200);
        TH1D *hQTarYm1_raw = new TH1D("hQTarYm1_raw", "Q_{Y strip -1} raw [pC]", 1200, 0, 1200);
        TH1D *hQTarYp1_raw = new TH1D("hQTarYp1_raw", "Q_{Y strip +1} raw [pC]", 1200, 0, 1200);
        TH1D *hChargeX = new TH1D("hChargeX", "Q_{X} [pC]", 1200, 0, 1200);
        TH1D *hQTarX0 = new TH1D("hQTarX0", "Q_{X strip 0} [pC]", 1200, 0, 1200);
        TH1D *hQTarXm1 = new TH1D("hQTarXm1", "Q_{X strip -1} [pC]", 1200, 0, 1200);
        TH1D *hQTarXp1 = new TH1D("hQTarXp1", "Q_{X strip +1} [pC]", 1200, 0, 1200);

        //TH1 for fit quality
        TH1D *hChi2X_raw = new TH1D("hChi2X_raw", "#chi^{2} X fit raw", 1000, 0, 10);
        TH1D *hFitStatusX_raw = new TH1D("hFitStatusX_raw", "Fit status X fit raw", 5, -0.5, 4.5);
        TH1D *hChi2Y_raw = new TH1D("hChi2Y_raw", "#chi^{2} Y fit raw", 1000, 0, 10);
        TH1D *hFitStatusY_raw = new TH1D("hFitStatusY_raw", "Fit status Y fit raw", 5, -0.5, 4.5);
        TH1D *hChi2X = new TH1D("hChi2X", "#chi^{2} X fit", 1000, 0, 10);
        TH1D *hFitStatusX = new TH1D("hFitStatusX", "Fit status X fit", 5, -0.5, 4.5);

        vector<double> dX_cal(Nfits), dX_raw(Nfits), dY_raw(Nfits);
        for(int i=0;i<Nfits;i++){
            dX_raw[i] = beamPosX_mm_raw[i] - beamPosX_wm_raw[i];
            dY_raw[i] = beamPosY_mm_raw[i] - beamPosY_wm_raw[i];
            dX_cal[i] = beamPosX_mm[i] - beamPosX_wm_cal[i];
            
            hChargeX_raw->Fill(QXTot_target_raw[i]);
            hQTarX0_raw->Fill(TargetQX_raw[i][1]);
            hQTarXm1_raw->Fill(TargetQX_raw[i][0]);
            hQTarXp1_raw->Fill(TargetQX_raw[i][2]);
            hChargeY_raw->Fill(QYTot_target_raw[i]);
            hQTarY0_raw->Fill(TargetQY_raw[i][1]);
            hQTarYm1_raw->Fill(TargetQY_raw[i][0]);
            hQTarYp1_raw->Fill(TargetQY_raw[i][2]);

            hChargeX->Fill(QXTot_target_cal[i]);
            hQTarX0->Fill(TargetQX[i][1]);
            hQTarXm1->Fill(TargetQX[i][0]);
            hQTarXp1->Fill(TargetQX[i][2]);

            hChi2X_raw->Fill(chi2X_raw[i]);
            hFitStatusX_raw->Fill(fitstatusX_raw[i]);
            hChi2Y_raw->Fill(chi2Y_raw[i]);
            hFitStatusY_raw->Fill(fitstatusY_raw[i]);
            hChi2X->Fill(chi2X[i]);
            hFitStatusX->Fill(fitstatusX[i]);
            
            gQ0_Xtarget_raw->SetPoint(i, idx[i], TargetQX_raw[i][1]);
            gQ0_Xtarget_raw->SetPointError(i, 0, err_TargetQX_raw[i][1]);
            gQm1_Xtarget_raw->SetPoint(i, idx[i], TargetQX_raw[i][0]);
            gQm1_Xtarget_raw->SetPointError(i, 0, err_TargetQX_raw[i][0]);
            gQp1_Xtarget_raw->SetPoint(i, idx[i], TargetQX_raw[i][2]);
            gQp1_Xtarget_raw->SetPointError(i, 0, err_TargetQX_raw[i][2]);

            gQ0_Ytarget_raw->SetPoint(i, idx[i], TargetQY_raw[i][1]);
            gQ0_Ytarget_raw->SetPointError(i, 0, err_TargetQY_raw[i][1]);
            gQm1_Ytarget_raw->SetPoint(i, idx[i], TargetQY_raw[i][0]);
            gQm1_Ytarget_raw->SetPointError(i, 0, err_TargetQY_raw[i][0]);
            gQp1_Ytarget_raw->SetPoint(i, idx[i], TargetQY_raw[i][2]);
            gQp1_Ytarget_raw->SetPointError(i, 0, err_TargetQY_raw[i][2]);

            gQ0_Xtarget_cal->SetPoint(i, idx[i], TargetQX[i][1]);
            gQ0_Xtarget_cal->SetPointError(i, 0, err_TargetQX[i][1]);
            gQm1_Xtarget_cal->SetPoint(i, idx[i], TargetQX[i][0]);
            gQm1_Xtarget_cal->SetPointError(i, 0, err_TargetQX[i][0]);
            gQp1_Xtarget_cal->SetPoint(i, idx[i], TargetQX[i][2]);
            gQp1_Xtarget_cal->SetPointError(i, 0, err_TargetQX[i][2]);
            // Q0/QLG graphs
            gQX0_QLG_raw->SetPoint(i, idx[i], TargetQX_raw[i][1] / LGQ[i]);
            gQX0_QLG_raw->SetPointError(i, 0, TargetQX_raw[i][1] / LGQ[i] * sqrt( pow(err_TargetQX_raw[i][1]/TargetQX_raw[i][1], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gQY0_QLG_raw->SetPoint(i, idx[i], TargetQY_raw[i][1] / LGQ[i]);
            gQY0_QLG_raw->SetPointError(i, 0, TargetQY_raw[i][1] / LGQ[i] * sqrt( pow(err_TargetQY_raw[i][1]/TargetQY_raw[i][1], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gQX0_QLG_cal->SetPoint(i, idx[i], TargetQX[i][1] / LGQ[i]);
            gQX0_QLG_cal->SetPointError(i, 0, TargetQX[i][1] / LGQ[i]* sqrt( pow(err_TargetQX[i][1]/TargetQX[i][1], 2) + pow(err_LGQ[i]/LGQ[i], 2) ) );
            
            // fitted charge/QLG graphs
            gChargeXratio_raw->SetPoint(i, idx[i], A_X_raw[i]/LGQ[i]);
            gChargeXratio_raw->SetPointError(i, 0, A_X_raw[i]/LGQ[i]* sqrt( pow(A_X_raw_err[i]/A_X_raw[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gChargeYratio_raw->SetPoint(i, idx[i], A_Y_raw[i]/LGQ[i]);
            gChargeYratio_raw->SetPointError(i, 0, A_Y_raw[i]/LGQ[i]* sqrt( pow(A_Y_raw_err[i]/A_Y_raw[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gChargeXratio_cal->SetPoint(i, idx[i], A_X[i]/LGQ[i]);
            gChargeXratio_cal->SetPointError(i, 0, A_X_err[i]/LGQ[i]* sqrt( pow(A_X_err[i]/A_X[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));

            // fitted charge/QLG energy corrected graphs
            gQLG_ECorr->SetPoint(i, idx[i], LGQ[i] * (ecalib / ebeam) );
            gQLG_ECorr->SetPointError(i, 0, err_LGQ[i] * (ecalib / ebeam) );
            gQXratio_raw_ECorr->SetPoint(i, idx[i], QXratio_raw[i] * (ebeam / ecalib) );
            gQXratio_raw_ECorr->SetPointError(i, 0, QXratio_raw[i] * (ebeam / ecalib) * sqrt( pow(QXTot_target_raw_err[i]/QXTot_target_raw[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gQYratio_raw_ECorr->SetPoint(i, idx[i], QYratio_raw[i] * (ebeam / ecalib) );
            gQYratio_raw_ECorr->SetPointError(i, 0, QYratio_raw[i] * (ebeam / ecalib) * sqrt( pow(QYTot_target_raw_err[i]/QYTot_target_raw[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gQXratio_cal_ECorr->SetPoint(i, idx[i], QXratio_cal[i] * (ebeam / ecalib) );
            gQXratio_cal_ECorr->SetPointError(i, 0, QXratio_cal[i] * (ebeam / ecalib) * sqrt( pow(QXTot_target_cal_err[i]/QXTot_target_cal[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            
            gChargeXratio_raw_ECorr->SetPoint(i, idx[i], A_X_raw[i]/LGQ[i] * (ebeam / ecalib) );
            gChargeXratio_raw_ECorr->SetPointError(i, 0, A_X_raw[i]/LGQ[i] * (ebeam / ecalib) * sqrt( pow(A_X_raw_err[i]/A_X_raw[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gChargeYratio_raw_ECorr->SetPoint(i, idx[i], A_Y_raw[i]/LGQ[i] * (ebeam / ecalib) );
            gChargeYratio_raw_ECorr->SetPointError(i, 0, A_Y_raw[i]/LGQ[i] * (ebeam / ecalib) * sqrt( pow(A_Y_raw_err[i]/A_Y_raw[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
            gChargeXratio_cal_ECorr->SetPoint(i, idx[i], A_X[i]/LGQ[i] * (ebeam / ecalib) );
            gChargeXratio_cal_ECorr->SetPointError(i, 0, A_X_err[i]/LGQ[i] * (ebeam / ecalib) * sqrt( pow(A_X_err[i]/A_X[i], 2) + pow(err_LGQ[i]/LGQ[i], 2) ));
        }

        TGraphErrors *gDeltaX_fit_minus_wmean_raw = new TGraphErrors(Nfits, idx.data(), dX_raw.data());
        TGraphErrors *gDeltaY_fit_minus_wmean_raw = new TGraphErrors(Nfits, idx.data(), dY_raw.data());
        TGraphErrors *gDeltaX_fit_minus_wmean = new TGraphErrors(Nfits, idx.data(), dX_cal.data());

        TGraphErrors *gsigmaXtar_raw = new TGraphErrors(Nfits, idx.data(), beamWidthX_raw.data(), nullptr, beamWidthX_raw_err.data());
        TGraphErrors *gsigmaYtar_raw = new TGraphErrors(Nfits, idx.data(), beamWidthY_raw.data(), nullptr, beamWidthY_raw_err.data());
        TGraphErrors *gsigmaXtar = new TGraphErrors(Nfits, idx.data(), beamWidthX.data(), nullptr, beamWidthX_err.data());
        TGraphErrors *gQLG = new TGraphErrors(Nfits, idx.data(), LGQ.data(), nullptr, err_LGQ.data());

        TGraphErrors *gQXTot_target_raw = new TGraphErrors(Nfits, idx.data(), QXTot_target_raw.data(), nullptr, QXTot_target_raw_err.data());
        TGraphErrors *gQYTot_target_raw = new TGraphErrors(Nfits, idx.data(), QYTot_target_raw.data(), nullptr, QYTot_target_raw_err.data());
        TGraphErrors *gQXTot_target_cal = new TGraphErrors(Nfits, idx.data(), QXTot_target_cal.data(), nullptr, QXTot_target_cal_err.data());

        TGraphErrors *gQXratio_raw = new TGraphErrors(Nfits, idx.data(), QXratio_raw.data(), nullptr, QXratio_raw_err.data());
        TGraphErrors *gQYratio_raw = new TGraphErrors(Nfits, idx.data(), QYratio_raw.data(), nullptr, QYratio_raw_err.data());
        TGraphErrors *gQXratio_cal = new TGraphErrors(Nfits, idx.data(), QXratio_cal.data(), nullptr, QXratio_cal_err.data());
        
        TGraphErrors *gChi2_X_raw = new TGraphErrors(Nfits, idx.data(), chi2X_raw.data());
        TGraphErrors *gFitStatus_X_raw = new TGraphErrors(Nfits, idx.data(), fitstatusX_raw.data());
        TGraphErrors *gChi2_Y_raw = new TGraphErrors(Nfits, idx.data(), chi2Y_raw.data());
        TGraphErrors *gFitStatus_Y_raw = new TGraphErrors(Nfits, idx.data(), fitstatusY_raw.data());
        TGraphErrors *gChi2_X = new TGraphErrors(Nfits, idx.data(), chi2X.data());
        TGraphErrors *gFitStatus_X = new TGraphErrors(Nfits, idx.data(), fitstatusX.data());
        TGraphErrors *gChargeX_raw = new TGraphErrors(Nfits, idx.data(), A_X_raw.data(), nullptr, A_X_raw_err.data());
        TGraphErrors *gChargeY_raw = new TGraphErrors(Nfits, idx.data(), A_Y_raw.data(), nullptr, A_Y_raw_err.data());
        TGraphErrors *gChargeX = new TGraphErrors(Nfits, idx.data(), A_X.data(), nullptr, A_X_err.data());

        // Calculating totals and ratios for the entire run - reconstructed X raw charge
        double QLGtot_ECorr = QLGtot * (ecalib / ebeam);
        double QLGtot_ECorr_err = QLGtot_err * (ecalib / ebeam);
        double QXTot_raw = QXTarTot_raw[0] + QXTarTot_raw[1] + QXTarTot_raw[2];
        double QXTot_raw_err = sqrt( pow(QXTarTot_raw_err[0],2) + pow(QXTarTot_raw_err[1],2) + pow(QXTarTot_raw_err[2],2) );
        double QXratio_raw = QXTot_raw/QLGtot;
        double QXratio_raw_err = QXratio_raw * sqrt(pow(QXTot_raw_err/QXTot_raw,2) + pow(QLGtot_err/QLGtot,2));
        double QXratio_raw_ECorr = QXratio_raw * (ebeam / ecalib);
        double QXratio_raw_ECorr_err = QXratio_raw_ECorr * sqrt(pow(QXTot_raw_err/QXTot_raw,2) + pow(QLGtot_err/QLGtot,2));

        // Calculating totals and ratios for the entire run - reconstructed Y raw charge
        double QYTot_raw = QYTarTot_raw[0] + QYTarTot_raw[1] + QYTarTot_raw[2];
        double QYTot_raw_err = sqrt( pow(QYTarTot_raw_err[0],2) + pow(QYTarTot_raw_err[1],2) + pow(QYTarTot_raw_err[2],2) );
        double QYratio_raw = QYTot_raw/QLGtot;
        double QYratio_raw_err = QYratio_raw * sqrt(pow(QYTot_raw_err/QYTot_raw,2) + pow(QLGtot_err/QLGtot,2));
        double QYratio_raw_ECorr = QYratio_raw * (ebeam / ecalib);
        double QYratio_raw_ECorr_err = QYratio_raw_ECorr * sqrt(pow(QYTot_raw_err/QYTot_raw,2) + pow(QLGtot_err/QLGtot,2));

        // Calculating totals and ratios for the entire run - reconstructed X calibrated charge
        double QXTot_cal = QXTarTot_cal[0] + QXTarTot_cal[1] + QXTarTot_cal[2];
        double QXTot_cal_err = sqrt( pow(QXTarTot_cal_err[0],2) + pow(QXTarTot_cal_err[1],2) + pow(QXTarTot_cal_err[2],2) );
        double QXLGratio_cal = QXTot_cal/QLGtot;
        double QXLGratio_cal_err = QXLGratio_cal * sqrt(pow(QXTot_cal_err/QXTot_cal,2) + pow(QLGtot_err/QLGtot,2));
        double QXLGratio_cal_ECorr = QXLGratio_cal * (ebeam / ecalib);
        double QXLGratio_cal_ECorr_err = QXLGratio_cal_ECorr * sqrt(pow(QXTot_cal_err/QXTot_cal,2) + pow(QLGtot_err/QLGtot,2));

        // Calculating totals and ratios for the entire run - fitted X and Y raw and X calibrated charge
        double QXRatio_fitted_raw = cXTot_raw[0]/QLGtot;
        double QXRatio_fitted_raw_err = QXRatio_fitted_raw * sqrt( pow(cXTot_raw_err[0]/cXTot_raw[0],2) + pow(QLGtot_err/QLGtot,2) );
        double QXRatio_fitted_raw_ECorr = QXRatio_fitted_raw * (ebeam / ecalib);
        double QXRatio_fitted_raw_ECorr_err = QXRatio_fitted_raw_ECorr * sqrt( pow(cXTot_raw_err[0]/cXTot_raw[0],2) + pow(QLGtot_err/QLGtot,2) );
        double QYRatio_fitted_raw = cYTot_raw[0]/QLGtot;
        double QYRatio_fitted_raw_err = QYRatio_fitted_raw * sqrt( pow(cYTot_raw_err[0]/cYTot_raw[0],2) + pow(QLGtot_err/QLGtot,2) );
        double QYRatio_fitted_raw_ECorr = QYRatio_fitted_raw * (ebeam / ecalib);
        double QYRatio_fitted_raw_ECorr_err = QYRatio_fitted_raw_ECorr * sqrt( pow(cYTot_raw_err[0]/cYTot_raw[0],2) + pow(QLGtot_err/QLGtot,2) );
        double QXRatio_fitted_cal = cXTot_cal[0]/QLGtot;
        double QXRatio_fitted_cal_err = QXRatio_fitted_cal * sqrt( pow(cXTot_cal_err[0]/cXTot_cal[0],2) + pow(QLGtot_err/QLGtot,2) );
        double QXRatio_fitted_cal_ECorr = QXRatio_fitted_cal * (ebeam / ecalib);
        double QXRatio_fitted_cal_ECorr_err = QXRatio_fitted_cal_ECorr * sqrt( pow(cXTot_cal_err[0]/cXTot_cal[0],2) + pow(QLGtot_err/QLGtot,2) );

        // double xx = 0.0;
        double eexx = 0.0;
        // charges target and LG - entire run
        TGraphErrors *gQLGTot = new TGraphErrors(1, &ebeam, &QLGtot, &eexx, &QLGtot_err);
        TGraphErrors *gQLGTot_ECorr = new TGraphErrors(1, &ebeam, &QLGtot_ECorr, &eexx, &QLGtot_ECorr_err);
        TGraphErrors *gQXTot_raw = new TGraphErrors(1, &ebeam, &QXTot_raw, &eexx, &QXTot_raw_err);
        TGraphErrors *gQXTot_fitted_raw = new TGraphErrors(1, &ebeam, &cXTot_raw[0], &eexx, &cXTot_raw_err[0]);
        TGraphErrors *gQYTot_raw = new TGraphErrors(1, &ebeam, &QYTot_raw, &eexx, &QYTot_raw_err);
        TGraphErrors *gQYTot_fitted_raw = new TGraphErrors(1, &ebeam, &cYTot_raw[0], &eexx, &cYTot_raw_err[0]);
        TGraphErrors *gQXTot_cal = new TGraphErrors(1, &ebeam, &QXTot_cal, &eexx, &QXTot_cal_err);
        TGraphErrors *gQXTot_fitted_cal = new TGraphErrors(1, &ebeam, &cXTot_cal[0], &eexx, &cXTot_cal_err[0]);
        // average positions - entire run
        double meanXTot_raw, meanYTot_raw, meanXTot_cal;
        double meanXTot_raw_err, meanYTot_raw_err, meanXTot_cal_err;
        mean_and_std(Xpos_wmean_raw, meanXTot_raw, meanXTot_raw_err);
        mean_and_std(Ypos_wmean_raw, meanYTot_raw, meanYTot_raw_err); 
        mean_and_std(Xpos_wmean_cal, meanXTot_cal, meanXTot_cal_err);
        meanXTot_raw = -(meanXTot_raw - expected_centerX_phys);
        meanYTot_raw = meanYTot_raw - expected_centerY_phys;
        meanXTot_cal = -(meanXTot_cal - expected_centerX_phys);

        double meanXTot_fitted_raw, meanYTot_fitted_raw, meanXTot_fitted_cal;
        double meanXTot_fitted_raw_err, meanYTot_fitted_raw_err, meanXTot_fitted_cal_err;
        mean_and_std(beamPosX_mm_raw, meanXTot_fitted_raw, meanXTot_fitted_raw_err);
        mean_and_std(beamPosY_mm_raw, meanYTot_fitted_raw, meanYTot_fitted_raw_err);
        mean_and_std(beamPosX_mm, meanXTot_fitted_cal, meanXTot_fitted_cal_err);
        meanXTot_fitted_raw = meanXTot_fitted_raw;
        // meanYTot_fitted_raw = meanYTot_fitted_raw - expected_centerY_phys;
        meanXTot_fitted_cal = meanXTot_fitted_cal;

        TGraphErrors *gmeanXTot_raw = new TGraphErrors(1, &ebeam, &meanXTot_raw, &eexx, &meanXTot_raw_err);
        TGraphErrors *gmeanXTot_fitted_raw = new TGraphErrors(1, &ebeam, &meanXTot_fitted_raw, &eexx, &cXTot_raw_err[1]);
        TGraphErrors *gmeanYTot_raw = new TGraphErrors(1, &ebeam, &meanYTot_raw, &eexx, &meanYTot_raw_err);
        TGraphErrors *gmeanYTot_fitted_raw = new TGraphErrors(1, &ebeam, &meanYTot_fitted_raw, &eexx, &cYTot_raw_err[1]);
        TGraphErrors *gmeanXTot_cal = new TGraphErrors(1, &ebeam, &meanXTot_cal, &eexx, &meanXTot_cal_err);
        TGraphErrors *gmeanXTot_fitted_cal = new TGraphErrors(1, &ebeam, &meanXTot_fitted_cal, &eexx, &cXTot_cal_err[1]);
        // average sigmas - entire run
        double meanSigmaXTot_raw = cXTot_raw[2]>1/sqrt(12) ? cXTot_raw[2] : 1/sqrt(12);
        double meanSigmaXTot_raw_err = cXTot_raw[2]>1/sqrt(12) ? cXTot_raw_err[2] : 0;
        double meanSigmaYTot_raw = cYTot_raw[2]>1/sqrt(12) ? cYTot_raw[2] : 1/sqrt(12);
        double meanSigmaYTot_raw_err = cYTot_raw[2]>1/sqrt(12) ? cYTot_raw_err[2] : 0;
        double meanSigmaXTot_cal = cXTot_cal[2]>1/sqrt(12) ? cXTot_cal[2] : 1/sqrt(12);
        double meanSigmaXTot_cal_err = cXTot_cal[2]>1/sqrt(12) ? cXTot_cal_err[2] : 0;

        TGraphErrors *gsigmaXTot_fitted_raw = new TGraphErrors(1, &ebeam, &meanSigmaXTot_raw, &eexx, &meanSigmaXTot_raw_err);
        TGraphErrors *gsigmaYTot_fitted_raw = new TGraphErrors(1, &ebeam, &meanSigmaYTot_raw, &eexx, &meanSigmaYTot_raw_err);
        TGraphErrors *gsigmaXTot_fitted_cal = new TGraphErrors(1, &ebeam, &meanSigmaXTot_cal, &eexx, &meanSigmaXTot_cal_err);
        // ratios - entire run
        TGraphErrors *gXR_fitted_raw = new TGraphErrors(1, &ebeam, &QXRatio_fitted_raw, &eexx, &QXRatio_fitted_raw_err);
        TGraphErrors *gRX_raw = new TGraphErrors(1, &ebeam, &QXratio_raw, &eexx, &QXratio_raw_err);
        TGraphErrors *gYR_fitted_raw = new TGraphErrors(1, &ebeam, &QYRatio_fitted_raw, &eexx, &QYRatio_fitted_raw_err);
        TGraphErrors *gRY_raw = new TGraphErrors(1, &ebeam, &QYratio_raw, &eexx, &QYratio_raw_err);
        TGraphErrors *gXR_fitted_cal = new TGraphErrors(1, &ebeam, &QXRatio_fitted_cal, &eexx, &QXRatio_fitted_cal_err);
        TGraphErrors *gRX_cal = new TGraphErrors(1, &ebeam, &QXLGratio_cal, &eexx, &QXLGratio_cal_err);
        // ratios beam energy corrected - entire run
        TGraphErrors *gRX_raw_ECorr = new TGraphErrors(1, &ebeam, &QXratio_raw_ECorr, &eexx, &QXratio_raw_ECorr_err);
        TGraphErrors *gXR_fitted_raw_ECorr = new TGraphErrors(1, &ebeam, &QXRatio_fitted_raw_ECorr, &eexx, &QXRatio_fitted_raw_ECorr_err);
        TGraphErrors *gRY_raw_ECorr = new TGraphErrors(1, &ebeam, &QYratio_raw_ECorr, &eexx, &QYratio_raw_ECorr_err);
        TGraphErrors *gYR_fitted_raw_ECorr = new TGraphErrors(1, &ebeam, &QYRatio_fitted_raw_ECorr, &eexx, &QYRatio_fitted_raw_ECorr_err);
        TGraphErrors *gRX_cal_ECorr = new TGraphErrors(1, &ebeam, &QXLGratio_cal_ECorr, &eexx, &QXLGratio_cal_ECorr_err);
        TGraphErrors *gXR_fitted_cal_ECorr = new TGraphErrors(1, &ebeam, &QXRatio_fitted_cal_ECorr, &eexx, &QXRatio_fitted_cal_ECorr_err);

        // Set graph attributes
        TGraphAttribute(gXTar_raw, "gXTar_raw", "Period", "X_{fit} (raw) [mm]", 20, kRed+2);
        TGraphAttribute(gXTar_wmean_raw, "gXTar_wmean_raw", "Period", "X_{wmean} (raw) [mm]", 24, kRed+2);
        TGraphAttribute(gDeltaX_fit_minus_wmean_raw, "gDeltaX_fit_minus_wmean_raw", "Period", "X_{fit} - X_{wmean} (raw) [mm]", 20, kRed+2);
        TGraphAttribute(gYTar_raw, "gYTar_raw", "Period", "Y_{fit} (raw) [mm]", 20, kRed+2);
        TGraphAttribute(gYTar_wmean_raw, "gYTar_wmean_raw", "Period", "Y_{wmean} (raw) [mm]", 24, kRed+2);
        TGraphAttribute(gDeltaY_fit_minus_wmean_raw, "gDeltaY_fit_minus_wmean_raw", "Period", "Y_{fit} - Y_{wmean} (raw) [mm]", 20, kRed+2);
        
        TGraphAttribute(gXTar, "gXTar", "Period", "X_{fit} (cal) [mm]", 20, kBlue+2);
        TGraphAttribute(gXTar_wmean, "gXTar_wmean", "Period", "X_{wmean} (cal) [mm]", 24, kBlue+2);      
        TGraphAttribute(gDeltaX_fit_minus_wmean, "gDeltaX_fit_minus_wmean", "Period", "X_{fit} - X_{wmean} (cal) [mm]", 20, kBlue+2);
        
        TGraphAttribute(gQLG, "gQLG", "Period", "Q_{LG} [pC]", 20, kBlue+2);
        TGraphAttribute(gQLG_ECorr, "gQLG_ECorr", "Period", "Q_{LG} ECorr [pC]", 20, kBlue+2);

        TGraphAttribute(gQXTot_target_raw, "gQXTot_target_raw", "Period", "Q_{X-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQYTot_target_raw, "gQYTot_target_raw", "Period", "Q_{Y-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQXTot_target_cal, "gQXTot_target_cal", "Period", "Q_{X-Tar} cal [pC]", 20, kBlue+2);

        TGraphAttribute(gQ0_Xtarget_raw, "gQ0_Xtarget_raw", "Period", "Q_{X0-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQm1_Xtarget_raw, "gQm1_Xtarget_raw", "Period", "Q_{Xm1-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQp1_Xtarget_raw, "gQp1_Xtarget_raw", "Period", "Q_{Xp1-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQ0_Ytarget_raw, "gQ0_Ytarget_raw", "Period", "Q_{Y0-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQm1_Ytarget_raw, "gQm1_Ytarget_raw", "Period", "Q_{Ym1-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQp1_Ytarget_raw, "gQp1_Ytarget_raw", "Period", "Q_{Yp1-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gQ0_Xtarget_cal, "gQ0_Xtarget_cal", "Period", "Q_{X0-Tar} cal [pC]", 20, kBlue+2);
        TGraphAttribute(gQm1_Xtarget_cal, "gQm1_Xtarget_cal", "Period", "Q_{Xm1-Tar} cal [pC]", 20, kBlue+2);
        TGraphAttribute(gQp1_Xtarget_cal, "gQp1_Xtarget_cal", "Period", "Q_{Xp1-Tar} cal [pC]", 20, kBlue+2);

        TGraphAttribute(gQX0_QLG_raw, "gQX0_QLG_raw", "Period", "Q_{X0-Tar}/Q_{LG} raw", 20, kRed+2);
        TGraphAttribute(gQY0_QLG_raw, "gQY0_QLG_raw", "Period", "Q_{Y0-Tar}/Q_{LG} raw", 20, kRed+2);
        TGraphAttribute(gQX0_QLG_cal, "gQX0_QLG_cal", "Period", "Q_{X0-Tar}/Q_{LG} cal", 20, kBlue+2);

        TGraphAttribute(gChargeXratio_raw, "gChargeXratio_fitted_raw", "Period", "Q_{X-Tar}/Q_{LG} raw", 20, kRed+2);
        TGraphAttribute(gChargeYratio_raw, "gChargeYratio_fitted_raw", "Period", "Q_{Y-Tar}/Q_{LG} raw", 20, kRed+2);
        TGraphAttribute(gChargeXratio_cal, "gChargeXratio_fitted_cal", "Period", "Q_{X-Tar}/Q_{LG} cal", 20, kBlue+2);

        TGraphAttribute(gQXratio_raw_ECorr, "gQXratio_raw_ECorr", "Period", "Q_{X-Tar}/Q_{LG} raw ECorr", 20, kRed+2);
        TGraphAttribute(gQYratio_raw_ECorr, "gQYratio_raw_ECorr", "Period", "Q_{Y-Tar}/Q_{LG} raw ECorr", 20, kRed+2);
        TGraphAttribute(gQXratio_cal_ECorr, "gQXratio_cal_ECorr", "Period", "Q_{X-Tar}/Q_{LG} cal ECorr", 20, kBlue+2);
        
        TGraphAttribute(gChargeXratio_raw_ECorr, "gChargeXratio_fitted_raw_ECorr", "Period", "Q_{X-Tar}/Q_{LG} raw ECorr", 20, kRed+2);
        TGraphAttribute(gChargeYratio_raw_ECorr, "gChargeYratio_fitted_raw_ECorr", "Period", "Q_{Y-Tar}/Q_{LG} raw ECorr", 20, kRed+2);
        TGraphAttribute(gChargeXratio_cal_ECorr, "gChargeXratio_fitted_cal_ECorr", "Period", "Q_{X-Tar}/Q_{LG} cal ECorr", 20, kBlue+2);

        TGraphAttribute(gQXratio_raw, "gQXratio_raw", "Period", "Q_{X-Tar}/Q_{LG} Raw", 20, kRed+2);
        TGraphAttribute(gQYratio_raw, "gQYratio_raw", "Period", "Q_{Y-Tar}/Q_{LG} Raw", 20, kRed+2);
        TGraphAttribute(gQXratio_cal, "gQXratio_cal", "Period", "Q_{X-Tar}/Q_{LG} Cal", 20, kBlue+2);
        
        TGraphAttribute(gsigmaXtar_raw, "gsigmaXtar_raw", "Period", "#sigma_{X} raw [mm]", 20, kRed+2);
        TGraphAttribute(gsigmaYtar_raw, "gsigmaYtar_raw", "Period", "#sigma_{Y} raw [mm]", 20, kRed+2);
        TGraphAttribute(gsigmaXtar, "gsigmaXtar", "Period", "#sigma_{X} cal [mm]", 20, kBlue+2);
        
        TGraphAttribute(gChi2_X_raw, "gChi2_X_raw", "Period", "#chi^{2}_{X} raw", 20, kRed+2);
        TGraphAttribute(gChi2_Y_raw, "gChi2_Y_raw", "Period", "#chi^{2}_{Y} raw", 20, kRed+2);
        TGraphAttribute(gChi2_X, "gChi2_X", "Period", "#chi^{2}_{X} cal", 20, kBlue+2);
        TGraphAttribute(gFitStatus_X_raw, "gFitStatus_X_raw", "Period", "Fit Status X raw", 20, kRed+2);
        TGraphAttribute(gFitStatus_Y_raw, "gFitStatus_Y_raw", "Period", "Fit Status Y raw", 20, kRed+2);
        TGraphAttribute(gFitStatus_X, "gFitStatus_X", "Period", "Fit Status X cal", 20, kBlue+2);
        TGraphAttribute(gChargeX_raw, "gChargeX_raw_fitted", "Period", "Q_{X-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gChargeY_raw, "gChargeY_raw_fitted", "Period", "Q_{Y-Tar} raw [pC]", 20, kRed+2);
        TGraphAttribute(gChargeX, "gChargeX_fitted", "Period", "Q_{X-Tar} cal [pC]", 20, kBlue+2);

        TGraphAttribute(gQLGTot, "gQLGTot", "E_{beam} [MeV]", "Q_{LG} Total [pC]", 20, kBlue+2);
        TGraphAttribute(gQLGTot_ECorr, "gQLGTot_ECorr", "E_{beam} [MeV]", "Q_{LG} Total ECorr [pC]", 20, kBlue+2);
        TGraphAttribute(gQXTot_raw, "gQXTot_raw", "E_{beam} [MeV]", "Q_{X-Tar} Raw Total [pC]", 20, kRed+2);
        TGraphAttribute(gQYTot_raw, "gQYTot_raw", "E_{beam} [MeV]", "Q_{Y-Tar} Raw Total [pC]", 20, kRed+2);
        TGraphAttribute(gQXTot_cal, "gQXTot_cal", "E_{beam} [MeV]", "Q_{X-Tar} Cal Total [pC]", 20, kBlue+2);
        TGraphAttribute(gQXTot_fitted_raw, "gQXTot_fitted_raw", "E_{beam} [MeV]", "Q_{X-Tar} Fitted Raw Total [pC]", 20, kRed+2);
        TGraphAttribute(gQYTot_fitted_raw, "gQYTot_fitted_raw", "E_{beam} [MeV]", "Q_{Y-Tar} Fitted Raw Total [pC]", 20, kRed+2);
        TGraphAttribute(gQXTot_fitted_cal, "gQXTot_fitted_cal", "E_{beam} [MeV]", "Q_{X-Tar} Fitted Cal Total [pC]", 20, kBlue+2);

        TGraphAttribute(gmeanXTot_raw, "gmeanXTot_raw", "E_{beam} [MeV]", "<X_{wmean}> Raw [mm]", 20, kRed+2);
        TGraphAttribute(gmeanYTot_raw, "gmeanYTot_raw", "E_{beam} [MeV]", "<Y_{wmean}> Raw [mm]", 20, kRed+2);
        TGraphAttribute(gmeanXTot_cal, "gmeanXTot_cal", "E_{beam} [MeV]", "<X_{wmean}> Cal [mm]", 20, kBlue+2);
        TGraphAttribute(gmeanXTot_fitted_raw, "gmeanXTot_fitted_raw", "E_{beam} [MeV]", "<X_{fit}> Fitted Raw [mm]", 20, kRed+2);  
        TGraphAttribute(gmeanYTot_fitted_raw, "gmeanYTot_fitted_raw", "E_{beam} [MeV]", "<Y_{fit}> Fitted Raw [mm]", 20, kRed+2);
        TGraphAttribute(gmeanXTot_fitted_cal, "gmeanXTot_fitted_cal", "E_{beam} [MeV]", "<X_{fit}> Fitted Cal [mm]", 20, kBlue+2);

        TGraphAttribute(gsigmaXTot_fitted_raw, "gsigmaXTot_fitted_raw", "E_{beam} [MeV]", "<#sigma_{X}> Fitted Raw [mm]", 20, kRed+2);
        TGraphAttribute(gsigmaYTot_fitted_raw, "gsigmaYTot_fitted_raw", "E_{beam} [MeV]", "<#sigma_{Y}> Fitted Raw [mm]", 20, kRed+2);
        TGraphAttribute(gsigmaXTot_fitted_cal, "gsigmaXTot_fitted_cal", "E_{beam} [MeV]", "<#sigma_{X}> Fitted Cal [mm]", 20, kBlue+2);

        TGraphAttribute(gRX_raw, "gRX_raw", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Raw", 20, kRed+2);
        TGraphAttribute(gRY_raw, "gRY_raw", "E_{beam} [MeV]", "Q_{Y-Tar}/Q_{LG} Raw", 20, kRed+2);
        TGraphAttribute(gRX_cal, "gRX_cal", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Cal", 20, kBlue+2);
        TGraphAttribute(gXR_fitted_raw, "gXR_fitted_raw", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Fitted Raw", 20, kRed+2);
        TGraphAttribute(gYR_fitted_raw, "gYR_fitted_raw", "E_{beam} [MeV]", "Q_{Y-Tar}/Q_{LG} Fitted Raw", 20, kRed+2);
        TGraphAttribute(gXR_fitted_cal, "gXR_fitted_cal", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Fitted Cal", 20, kBlue+2); 

        TGraphAttribute(gRX_raw_ECorr, "gRX_raw_ECorr", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Raw ECorr", 20, kRed+2);
        TGraphAttribute(gRY_raw_ECorr, "gRY_raw_ECorr", "E_{beam} [MeV]", "Q_{Y-Tar}/Q_{LG} Raw ECorr", 20, kRed+2);
        TGraphAttribute(gRX_cal_ECorr, "gRX_cal_ECorr", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Cal ECorr", 20, kBlue+2);
        TGraphAttribute(gXR_fitted_raw_ECorr, "gXR_fitted_raw_ECorr", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Fitted Raw ECorr", 20, kGreen+2);
        TGraphAttribute(gYR_fitted_raw_ECorr, "gYR_fitted_raw_ECorr", "E_{beam} [MeV]", "Q_{Y-Tar}/Q_{LG} Fitted Raw ECorr", 20, kGreen+2);
        TGraphAttribute(gXR_fitted_cal_ECorr, "gXR_fitted_cal_ECorr", "E_{beam} [MeV]", "Q_{X-Tar}/Q_{LG} Fitted Cal ECorr", 20, kBlue+2);


        //adding plots for stability charge variations studies
        TF1 *p0LG = new TF1("p0LG", "pol0", 0, 100);
        TF1 *p0Xcal = new TF1("p0Xcal", "pol0", 0, 100);
        TF1 *p0Xraw = new TF1("p0Xraw", "pol0", 0, 100); 
        TF1 *p0TarLGRatio = new TF1("p0TarLGRatio", "pol0", 0, 100);
        gQLG->Fit("p0LG", "QR");
        gQXTot_target_cal->Fit("p0Xcal", "QR");
        gQXTot_target_raw->Fit("p0Xraw", "QR");
        gQXratio_cal->Fit("p0TarLGRatio", "QR");

        // TGraphErrors *gQLGNorm = new TGraphErrors(Nfits, idx.data(), LGQ.data()/p0LG->GetParameter(0), nullptr, (QLGtot_err_data.data()/p0LG->GetParameter(0) + LGQ.data()*p0LG->GetParError(0)/pow(p0LG->GetParameter(0),2)));
        // TGraphErrors *gQX_calNorm = new TGraphErrors(Nfits, idx.data(), QXTot_target_cal.data()/p0Xcal->GetParameter(0), nullptr, (QXTot_target_cal_err.data()/p0Xcal->GetParameter(0) + QXTot_target_cal.data()*p0Xcal->GetParError(0)/pow(p0Xcal->GetParameter(0),2)));
        // TGraphErrors *gQX_raw_Norm = new TGraphErrors(Nfits, idx.data(), QXTot_target_raw.data()/p0Xraw->GetParameter(0), nullptr, (QXTot_target_raw_err.data()/p0Xraw->GetParameter(0) + QXTot_target_raw.data()*p0Xraw->GetParError(0)/pow(p0Xraw->GetParameter(0),2)));
        // TGraphErrors *gQXLGRatio_Norm = new TGraphErrors(Nfits, idx.data(), QXratio_cal.data()/p0TarLGRatio->GetParameter(0), nullptr, (QXratio_cal_err.data()/p0TarLGRatio->GetParameter(0) + QXratio_cal.data()*p0TarLGRatio->GetParError(0)/pow(p0TarLGRatio->GetParameter(0),2)));

        TGraphErrors *gQLGNorm = new TGraphErrors(Nfits);
        TGraphErrors *gQX_calNorm = new TGraphErrors(Nfits);
        TGraphErrors *gQX_raw_Norm = new TGraphErrors(Nfits);
        TGraphErrors *gQXLGRatio_Norm = new TGraphErrors(Nfits);

        for(int i = 0; i < Nfits; i++){
            gQLGNorm->SetPoint(i, idx[i], LGQ[i]/p0LG->GetParameter(0));
            gQLGNorm->SetPointError(i, 0, err_LGQ[i]/p0LG->GetParameter(0));
            // gQLGNorm->SetPointError(i, 0, (err_LGQ[i]/p0LG->GetParameter(0) + LGQ[i]*p0LG->GetParError(0)/pow(p0LG->GetParameter(0),2)));
            gQX_calNorm->SetPoint(i, idx[i], QXTot_target_cal[i]/p0Xcal->GetParameter(0));
            gQX_calNorm->SetPointError(i, 0, QXTot_target_cal_err[i]/p0Xcal->GetParameter(0));
            // gQX_calNorm->SetPointError(i, 0, (QXTot_target_cal_err[i]/p0Xcal->GetParameter(0) + QXTot_target_cal[i]*p0Xcal->GetParError(0)/pow(p0Xcal->GetParameter(0),2)));
            gQX_raw_Norm->SetPoint(i, idx[i], QXTot_target_raw[i]/p0Xraw->GetParameter(0));
            gQX_raw_Norm->SetPointError(i, 0, QXTot_target_raw_err[i]/p0Xraw->GetParameter(0));
            // gQX_raw_Norm->SetPointError(i, 0, (QXTot_target_raw_err[i]/p0Xraw->GetParameter(0) + QXTot_target_raw[i]*p0Xraw->GetParError(0)/pow(p0Xraw->GetParameter(0),2)));
            gQXLGRatio_Norm->SetPoint(i, idx[i], QXratio_cal[i]/p0TarLGRatio->GetParameter(0));
            gQXLGRatio_Norm->SetPointError(i, 0, QXratio_cal_err[i]/p0TarLGRatio->GetParameter(0));
            // gQXLGRatio_Norm->SetPointError(i, 0, (QXratio_cal_err[i]/p0TarLGRatio->GetParameter(0) + QXratio_cal[i]*p0TarLGRatio->GetParError(0)/pow(p0TarLGRatio->GetParameter(0),2)));
        }
        
        TGraphAttribute(gQLGNorm, "Q_{LG}/<Q_{LG}>", "Period (200s)", "Q_{LG}/<Q_{LG}>", 20, kBlue+2);
        TGraphAttribute(gQX_calNorm, "Q_{X-Tar}/<Q_{X-Tar}> Calib", "Period (200s)", "Q_{X-Tar}/<Q_{X-Tar}> Calib", 20, kBlue+2);
        TGraphAttribute(gQX_raw_Norm, "Q_{X-Tar}/<Q_{X-Tar}> Raw", "Period (200s)", "Q_{X-Tar}/<Q_{X-Tar}> Raw", 20, kBlue+2);
        TGraphAttribute(gQXLGRatio_Norm, "Q_{X-Tar}/Q_{LG} / <Q_{X-Tar}/Q_{LG}>", "Period (200s)", "Q_{X-Tar}/Q_{LG} / <Q_{X-Tar}/Q_{LG}>", 20, kBlue+2);

        histoFile->cd();
        TDirectory *distrDir = histoFile->mkdir("distributions");
        TDirectory *trendsDir = histoFile->mkdir("trends");
        TDirectory *summaryDir = histoFile->mkdir("summary");
        histoFile->cd();
        trendsDir->cd();
        gXTar->Write();
        gXTar_raw->Write();
        gYTar_raw->Write();
        gXTar_wmean->Write();
        gXTar_wmean_raw->Write();
        gYTar_wmean_raw->Write();
        gDeltaX_fit_minus_wmean->Write();
        gDeltaX_fit_minus_wmean_raw->Write();        
        gDeltaY_fit_minus_wmean_raw->Write();        
        gQXTot_target_raw->Write();
        gQYTot_target_raw->Write();
        gQXTot_target_cal->Write();
        gQ0_Xtarget_raw->Write();
        gQ0_Ytarget_raw->Write();
        gQ0_Xtarget_cal->Write();
        gQm1_Xtarget_raw->Write();        
        gQm1_Ytarget_raw->Write();
        gQm1_Xtarget_cal->Write();
        gQp1_Xtarget_raw->Write();
        gQp1_Ytarget_raw->Write();
        gQp1_Xtarget_cal->Write();
        gQX0_QLG_raw->Write();
        gQY0_QLG_raw->Write();
        gQX0_QLG_cal->Write();
        gQLG->Write();
        gQLG_ECorr->Write();
        gQXratio_raw->Write();
        gQYratio_raw->Write();
        gQXratio_cal->Write();
        gQXratio_raw_ECorr->Write();
        gQYratio_raw_ECorr->Write();
        gQXratio_cal_ECorr->Write();
        gsigmaXtar->Write();
        gsigmaXtar_raw->Write();
        gsigmaYtar_raw->Write();
        gChi2_X->Write();
        gChi2_X_raw->Write();
        gChi2_Y_raw->Write();
        gFitStatus_X->Write();
        gFitStatus_X_raw->Write();
        gFitStatus_Y_raw->Write();
        gChargeX->Write();
        gChargeX_raw->Write();
        gChargeY_raw->Write();
        gChargeXratio_raw->Write();
        gChargeYratio_raw->Write();
        gChargeXratio_cal->Write();
        gChargeXratio_raw_ECorr->Write();
        gChargeYratio_raw_ECorr->Write();
        gChargeXratio_cal_ECorr->Write();
        gQLGNorm->Write();
        gQX_calNorm->Write();
        gQX_raw_Norm->Write();
        gQXLGRatio_Norm->Write();

        histoFile->cd();
        distrDir->cd();
        hChargeX->Write();
        hChargeX_raw->Write();
        hChargeY_raw->Write();
        hQTarX0->Write();
        hQTarX0_raw->Write();
        hQTarY0_raw->Write();
        hQTarXm1->Write();
        hQTarXm1_raw->Write();
        hQTarYm1_raw->Write();
        hQTarXp1->Write();
        hQTarXp1_raw->Write();
        hQTarYp1_raw->Write();
        hChi2X->Write();
        hChi2X_raw->Write();
        hChi2Y_raw->Write();
        hFitStatusX->Write();
        hFitStatusX_raw->Write();
        hFitStatusY_raw->Write();

        histoFile->cd();
        summaryDir->cd();

        gQLGTot->Write();
        gQLGTot_ECorr->Write();
        gQXTot_raw->Write(); 
        gQYTot_raw->Write(); 
        gQXTot_cal->Write();
        gQXTot_fitted_raw->Write();
        gQYTot_fitted_raw->Write();
        gQXTot_fitted_cal->Write();

        gmeanXTot_raw->Write();
        gmeanYTot_raw->Write();
        gmeanXTot_cal->Write();
        gmeanXTot_fitted_raw->Write();
        gmeanYTot_fitted_raw->Write();
        gmeanXTot_fitted_cal->Write();

        gsigmaXTot_fitted_raw->Write();
        gsigmaYTot_fitted_raw->Write();
        gsigmaXTot_fitted_cal->Write();

        gRX_raw->Write();
        gRY_raw->Write();
        gRX_cal->Write();
        gXR_fitted_raw->Write();
        gYR_fitted_raw->Write();
        gXR_fitted_cal->Write();
        gRX_raw_ECorr->Write();
        gRY_raw_ECorr->Write();
        gRX_cal_ECorr->Write(); 
        gXR_fitted_raw_ECorr->Write();
        gYR_fitted_raw_ECorr->Write();
        gXR_fitted_cal_ECorr->Write();

        cout << "Plots successfully stored in " << outputFileName << endl;

        OutputTxt << "Run Number: " << runNumber << " - Ebeam: " << ebeam << " MeV" << endl;
        OutputTxt << "QLG: " << QLGtot << " +/- " << QLGtot_err << " pC" << endl;
        OutputTxt << "QXTar raw(-1,0,1): (" << QXTarTot_raw[0] << " +/- " << QXTarTot_raw_err[0] << ", " << QXTarTot_raw[1] << " +/- " << QXTarTot_raw_err[1] << ", " << QXTarTot_raw[2] << " +/- " << QXTarTot_raw_err[2] << ") [pC]" << endl;
        OutputTxt << "QXTar cal(-1,0,1): (" << QXTarTot_cal[0] << " +/- " << QXTarTot_cal_err[0] << ", " << QXTarTot_cal[1] << " +/- " << QXTarTot_cal_err[1] << ", " << QXTarTot_cal[2] << " +/- " << QXTarTot_cal_err[2] << ") [pC]" << endl;    
        OutputTxt << "QXtotFitted raw: " << cXTot_raw[0] << " +/- " << cXTot_raw_err[0] << " pC" << endl;
        OutputTxt << "QYTar raw(-1,0,1): (" << QYTarTot_raw[0] << " +/- " << QYTarTot_raw_err[0] << ", " << QYTarTot_raw[1] << " +/- " << QYTarTot_raw_err[1] << ", " << QYTarTot_raw[2] << " +/- " << QYTarTot_raw_err[2] << ") [pC]" << endl;
        OutputTxt << "QYtotFitted raw: " << cYTot_raw[0] << " +/- " << cYTot_raw_err[0] << " pC" << endl;
        OutputTxt << "X raw: " << cXTot_raw[1] << " +/- " << cXTot_raw_err[1] << " mm" << endl;
        OutputTxt << "sigmaX raw: " << cXTot_raw[2] << " +/- " << cXTot_raw_err[2] << " mm" << endl;
        OutputTxt << "Y raw: " << cYTot_raw[1] << " +/- " << cYTot_raw_err[1] << " mm" << endl;
        OutputTxt << "sigmaY raw: " << cYTot_raw[2] << " +/- " << cYTot_raw_err[2] << " mm" << endl;
        OutputTxt << "QXtotFitted cal: " << cXTot_cal[0] << " +/- " << cXTot_cal_err[0] << " pC" << endl;
        OutputTxt << "X cal: " << cXTot_cal[1] << " +/- " << cXTot_cal_err[1] << " mm" << endl;
        OutputTxt << "sigmaX cal: " << cXTot_cal[2] << " +/- " << cXTot_cal_err[2] << " mm" << endl;
        OutputTxt << "QXtot/QLG raw: " << QXratio_raw << " +/- " << QXratio_raw_err << endl;
        OutputTxt << "QXtot/QLG cal: " << QXLGratio_cal << " +/- " << QXLGratio_cal_err << endl;
        OutputTxt << "QXtot/QLG fitted raw: " << QXRatio_fitted_raw << " +/- " << QXRatio_fitted_raw_err << endl;
        OutputTxt << "QXtot/QLG fitted cal: " << QXRatio_fitted_cal << " +/- " << QXRatio_fitted_cal_err << endl;
        OutputTxt << "QYtot/QLG raw: " << QYratio_raw << " +/- " << QYratio_raw_err << endl;
        OutputTxt << "QYtot/QLG fitted raw: " << QYRatio_fitted_raw << " +/- " << QYRatio_fitted_raw_err << endl;

        OutputTxt.close();

    }

    
    printf("Output file%s closed \n",outputFileName.Data());
    histoFile->Close();
    
    cout << "Target position, charge and radiation damage estiamate done." << endl;

    return 0;
}

