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

    // Calibrated X and raw Y (current)
    vector<array<double,3>> XqTar, YqTar;
    vector<array<double,3>> errXqTar, errYqTar;

    vector<double> QLG, errQLG;

    // Total X target charge as a function of periods
    vector<double> QTot_target_raw, QTot_target_raw_err;
    vector<double> QTot_target_cal, QTot_target_cal_err;
    // ratios of target X charge to LG charge
    vector<double> Qratio_raw, Qratio_raw_err;
    vector<double> Qratio_cal, Qratio_cal_err;


    // Calibrated X fit results
    vector<array<double,3>> coeffsX, coeffsY;
    vector<array<double,3>> coeffsX_err, coeffsY_err;
    vector<GoodnessOfFit> gofX, gofY;

    // NEW: RAW X charge and fit results (no calibration)
    vector<array<double,3>> XqTar_raw;
    vector<array<double,3>> errXqTar_raw;
    vector<array<double,3>> coeffsX_raw;
    vector<array<double,3>> coeffsX_raw_err;
    vector<GoodnessOfFit> gofX_raw;
    // NEW: simple weighted-mean X position (mm) per period, raw+cal
    vector<double> Xpos_wmean_raw, Xpos_wmean_raw_err;
    vector<double> Xpos_wmean_cal, Xpos_wmean_cal_err;

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
        double sigma = q_err[iq]; // error doubled trying to understand the error evaluation
        if(sigma<=0) sigma=1.0;
        double diff =  model - q[iq];
        C2 += diff*diff/(sigma*sigma);

    }

    fval = C2;
}

//  Do the Fit in one direction
void fitBeam(const int &nominal, const array<double,3> &qTar, const array<double,3> &qTar_err,
             array<double,3> &coeffs, array<double,3> &coeffs_err, GoodnessOfFit &gof) {
     
    g_nominal_strip = nominal; 
    g_qTar = qTar; 
    g_qTar_err = qTar_err;
    
    TMinuit minuit(3); // A + mu + sigma
    minuit.SetPrintLevel(0);  // Silence all Minuit output
    minuit.SetFCN(fcn); 
    minuit.DefineParameter(0, "A", 100.0, 0.01, 0.0, 300.0); 
    minuit.DefineParameter(1, "mu", 0., 0.01, -10, 10.0); 
    minuit.DefineParameter(2, "sigma",  0., 0.01, 0, 10.0); 
    
    minuit.Migrad(); 
    minuit.mnstat(gof.chi2, gof.edm, gof.errdef, gof.nvpar, gof.nparx, gof.status);

    // covariance matrix
    TMatrixD covMatrix(3, 3); 
    minuit.mnemat(covMatrix.GetMatrixArray(), 3);

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
    // int adc_Y0 = PhysicsToADCs[centerY_phys];
    int adc_Xm1 = PhysicsToADCs[centerX_phys-1];
    int adc_Xp1 = PhysicsToADCs[centerX_phys+1];
    // int adc_Ym1 = PhysicsToADCs[centerY_phys-1];
    // int adc_Yp1 = PhysicsToADCs[centerY_phys+1];

    RunEvQ.centerXphys = centerX_phys;
    RunEvQ.centerYphys = centerY_phys;
    // RunEvQ.centerXadc = adc_X0;
    // RunEvQ.centerYadc = adc_Y0;
    // cout << "Target (X,Y) position: (" << X_ch << ", " << Y_ch << ")" << endl;
    // cout << "Target (X_minus,X_plus) : (" << X_ch-1 << ", " << X_ch+1 << ")" << " - Target (Y_minus,Y_plus) : (" << Y_ch-1 << ", " << Y_ch+1 << ")" << endl;
    // cout << "Target (X,Y) ADCs: (" << adc_X0 << ", " << adc_Y0 << ")" << endl;
    // cout << "ADCs (X_minus,X_plus) : (" << PhysicsToADCs[X_ch-1] << ", " << PhysicsToADCs[X_ch+1] << ")" << " - Target (Y_minus,Y_plus) : (" << PhysicsToADCs[Y_ch-1] << ", " << PhysicsToADCs[Y_ch+1] << ")" << endl;


    vector<int> vNeve;
    vector<double> cur_QLG; 
    vector<double> blkQLG, blkQLG_err;
    vector<double> QLG, QLG_err;
    vector<array<double,3>> blkQX, blkQY;
    vector<array<double,3>> X_qTar_raw, err_X_qTar_raw;
    vector<array<double,3>> X_qTar, err_X_qTar;
    // vector<array<double,3>> Y_qTar, err_Y_qTar;

    vector<array<double,3>> coeffX, coeffX_err;
    vector<array<double,3>> coeffX_raw, coeffX_raw_err;
    // vector<array<double,3>> coeffY, coeffY_err;

    vector<double> QTot_target_raw, QTot_target_cal;
    vector<double> QTot_target_raw_err, QTot_target_cal_err;
    
    vector<double> Qratio_raw, Qratio_cal;
    vector<double> Qratio_raw_err, Qratio_cal_err;

    vector<GoodnessOfFit> gofX, gofX_raw; //, gofY;
    // NEW: weighted-mean X position per period
    vector<double> Xpos_wmean_raw, Xpos_wmean_raw_err;
    vector<double> Xpos_wmean_cal, Xpos_wmean_cal_err;

    double mQ = 0.;
    double sQ = 0.; 
    double qLG, dqLG;
    // double qXm1,qX0,qXp1;
    // double dqXm1,dqX0,dqXp1;
    // double qYm1,qY0,qYp1;
    // double dqYm1,dqY0,dqYp1;

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

            double rawXm1 = (QChCumFixed[1][adc_Xm1] > 0 ? QChCumFixed[1][adc_Xm1] : 0);
            double rawX0  = (QChCumFixed[1][adc_X0]  > 0 ? QChCumFixed[1][adc_X0]  : 0);
            double rawXp1 = (QChCumFixed[1][adc_Xp1] > 0 ? QChCumFixed[1][adc_Xp1] : 0);

            // store RAW in per-block container
            blkQX.push_back({rawXm1, rawX0, rawXp1});

            // blkQY.push_back({QChCumFixed[1][adc_Ym1], 
            //                 QChCumFixed[1][adc_Y0], 
            //                 QChCumFixed[1][adc_Yp1]});

            // Every NCHUNKS blocks → compute averaged charge & fit
            if (chunk % NCHUNKS == 0) {

                size_t start = blkQLG.size() - NCHUNKS;

                vector<double> lastLG(blkQLG.begin() + start, blkQLG.end());
                vector<double> lastQXm1, lastQX0, lastQXp1;
                // vector<double> lastQYm1, lastQY0, lastQYp1;

                for (size_t i = start; i < blkQX.size(); ++i) {
                    lastQXm1.push_back(blkQX[i][0]);
                    lastQX0 .push_back(blkQX[i][1]);
                    lastQXp1.push_back(blkQX[i][2]);

                    // lastQYm1.push_back(blkQY[i][0]);
                    // lastQY0 .push_back(blkQY[i][1]);
                    // lastQYp1.push_back(blkQY[i][2]);
                }

                // LG block average
                mean_and_std(lastLG, qLG, dqLG);

                // RAW X & Y block averages (NO calibration yet)
                double qXm1_raw, qX0_raw, qXp1_raw;
                double dqXm1_raw, dqX0_raw, dqXp1_raw;

                mean_and_std(lastQXm1, qXm1_raw, dqXm1_raw);
                mean_and_std(lastQX0,  qX0_raw,  dqX0_raw);
                mean_and_std(lastQXp1, qXp1_raw, dqXp1_raw);

                // mean_and_std(lastQYm1, qYm1, dqYm1);
                // mean_and_std(lastQY0,  qY0,  dqY0);
                // mean_and_std(lastQYp1, qYp1, dqYp1);

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

                // --- Store RAW and CALIBRATED X for later comparison
                array<double,3> x_raw     = {qXm1_raw, qX0_raw, qXp1_raw};
                array<double,3> x_raw_err = {dqXm1_raw, dqX0_raw, dqXp1_raw};

                array<double,3> x_cal     = {qXm1_cal, qX0_cal, qXp1_cal};
                array<double,3> x_cal_err = {dqXm1_cal, dqX0_cal, dqXp1_cal};

                X_qTar_raw.push_back(x_raw);
                err_X_qTar_raw.push_back(x_raw_err);

                X_qTar.push_back(x_cal);
                err_X_qTar.push_back(x_cal_err);

                 // --- NEW: weighted-mean position (raw & calibrated)
                double mu_w_raw=0.0, dmu_w_raw=0.0;
                double mu_w_cal=0.0, dmu_w_cal=0.0;

                // if sum is 0, function returns false -> keep mu=0, dmu=0
                weighted_mean_mu_3strips(x_raw, x_raw_err, mu_w_raw, dmu_w_raw);
                weighted_mean_mu_3strips(x_cal, x_cal_err, mu_w_cal, dmu_w_cal);

                // convert to absolute X in mm, consistent with your fit convention:
                // X(mm) = Xstrip + mu  (with 1 mm pitch)
                Xpos_wmean_raw.push_back( BeamPosXmm(centerX_phys, mu_w_raw) );
                Xpos_wmean_raw_err.push_back( fabs(dmu_w_raw) * 1.0 );

                Xpos_wmean_cal.push_back( BeamPosXmm(centerX_phys, mu_w_cal) );
                Xpos_wmean_cal_err.push_back( fabs(dmu_w_cal) * 1.0 );

                // Y remains uncalibrated
                // Y_qTar.push_back({qYm1, qY0, qYp1});
                // err_Y_qTar.push_back({dqYm1, dqY0, dqYp1});

                QLG.push_back(qLG);
                QLG_err.push_back(dqLG);

                // Total target charge RAW
                double QTar_raw = qXm1_raw + qX0_raw + qXp1_raw;
                double dQTar_raw = sqrt( dqXm1_raw*dqXm1_raw + dqX0_raw*dqX0_raw + dqXp1_raw*dqXp1_raw);

                // Total target charge CAL
                double QTar_cal = qXm1_cal + qX0_cal + qXp1_cal;
                double dQTar_cal = sqrt( dqXm1_cal*dqXm1_cal + dqX0_cal*dqX0_cal + dqXp1_cal*dqXp1_cal);

                // Ratios
                double R_raw = (QLG.back() > 0 ? QTar_raw / QLG.back() : 0);
                double dR_raw = (R_raw > 0 ? R_raw * sqrt( pow(dQTar_raw/QTar_raw, 2) + pow(dqLG/QLG.back(), 2) ) : 0);

                double R_cal = (QLG.back() > 0 ? QTar_cal / QLG.back() : 0);
                double dR_cal = (R_cal > 0 ? R_cal * sqrt( pow(dQTar_cal/QTar_cal, 2) + pow(dqLG/QLG.back(), 2) ) : 0);
                
                QTot_target_raw.push_back(QTar_raw);
                QTot_target_raw_err.push_back(dQTar_raw);
                QTot_target_cal.push_back(QTar_cal);
                QTot_target_cal_err.push_back(dQTar_cal);
                // Store
                Qratio_raw.push_back(R_raw);
                Qratio_raw_err.push_back(dR_raw);
                Qratio_cal.push_back(R_cal);
                Qratio_cal_err.push_back(dR_cal);

                // --- Two fits in X: RAW and CAL
                array<double,3> lastCX_raw,  lastCX_raw_err;
                array<double,3> lastCX_cal,  lastCX_cal_err;
                // array<double,3> lastCY,      lastCY_err;
                GoodnessOfFit lastGX_raw, lastGX_cal; //, lastGY;

                // cout << "charge x_raw " << qXm1_raw << " " << qX0_raw << " " << qXp1_raw << endl;
                // cout << "charge x_cal " << qXm1_cal << " " << qX0_cal << " " << qXp1_cal << endl; 
                // cout << "charge x_raw_err " << dqXm1_raw << " " << dqX0_raw << " " << dqXp1_raw << endl;
                // cout << "charge x_cal_err " << dqXm1_cal << " " << dqX0_cal << " " << dqXp1_cal << endl; 

                fitBeam(centerX_phys, x_raw, x_raw_err, lastCX_raw, lastCX_raw_err, lastGX_raw);
                fitBeam(centerX_phys, x_cal, x_cal_err, lastCX_cal, lastCX_cal_err, lastGX_cal);
                // fitBeam(centerY_phys, Y_qTar.back(), err_Y_qTar.back(), lastCY, lastCY_err, lastGY);

                coeffX_raw.push_back(lastCX_raw);
                coeffX_raw_err.push_back(lastCX_raw_err);
                gofX_raw.push_back(lastGX_raw);

                coeffX.push_back(lastCX_cal);
                coeffX_err.push_back(lastCX_cal_err);
                gofX.push_back(lastGX_cal);

                // coeffY.push_back(lastCY);
                // coeffY_err.push_back(lastCY_err);
                // gofY.push_back(lastGY);
            }

            cur_QLG.clear();
        }
    }
        
    RunEvQ.eventNumber = vNeve;
    RunEvQ.QLG = QLG;
    RunEvQ.errQLG = QLG_err; 
    RunEvQ.XqTar = X_qTar;               // calibrated
    RunEvQ.errXqTar = err_X_qTar;
    RunEvQ.XqTar_raw = X_qTar_raw;       // raw
    RunEvQ.errXqTar_raw = err_X_qTar_raw;
    // RunEvQ.YqTar = Y_qTar;
    // RunEvQ.errYqTar = err_Y_qTar;
    RunEvQ.QTot_target_raw = QTot_target_raw;
    RunEvQ.QTot_target_raw_err = QTot_target_raw_err;
    RunEvQ.QTot_target_cal = QTot_target_cal;
    RunEvQ.QTot_target_cal_err = QTot_target_cal_err;
    RunEvQ.Qratio_raw = Qratio_raw;
    RunEvQ.Qratio_raw_err = Qratio_raw_err;
    RunEvQ.Qratio_cal = Qratio_cal;
    RunEvQ.Qratio_cal_err = Qratio_cal_err;
    RunEvQ.coeffsX = coeffX;              // calibrated
    RunEvQ.coeffsX_err = coeffX_err;
    RunEvQ.coeffsX_raw = coeffX_raw;      // raw
    RunEvQ.coeffsX_raw_err = coeffX_raw_err;
    // RunEvQ.coeffsY = coeffY;
    // RunEvQ.coeffsY_err = coeffY_err;
    // NEW
    RunEvQ.Xpos_wmean_raw = Xpos_wmean_raw;
    RunEvQ.Xpos_wmean_raw_err = Xpos_wmean_raw_err;
    RunEvQ.Xpos_wmean_cal = Xpos_wmean_cal;
    RunEvQ.Xpos_wmean_cal_err = Xpos_wmean_cal_err;
    RunEvQ.gofX = gofX;                   // calibrated
    RunEvQ.gofX_raw = gofX_raw;           // raw
    // RunEvQ.gofY = gofY;

    f->Close();
    return true;
}

// --- Main
int main(int argc, char** argv) {
    //root file pattern (as in user's code)

    // run - X_Ch - Y_ch association (physics strips)
    int runNumber = 80677; // example run number

    string calibFile = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/CalibTarget/outputCalibration/TargetCalibrationConst1.txt";
    bool calibLoaded = LoadCalibrationConstants(calibFile);
    if (!calibLoaded) {
        cerr << "Calibration constants missing — continuing with unity gains!" << endl;
    }
    
    string rootPattern = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco_Scan1_Scan2/Reco_run_00%d.root";

    // defining the output txt file
    ofstream OutputFit(Form("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/output/TargetChargePosition_run_00%i_newPositioning.txt", runNumber));
    OutputFit << "==== Calibration LogFile ChCalib ====" << endl;
    // defining the output root file
    TString outputFileName = "outputCalibration/Run4Monitor/ChCalib_XY_run_00" + to_string(runNumber) + "_newPositioning_Jan26.root";
    TFile* histoFile = new TFile(outputFileName,"RECREATE");
    if(!histoFile) {
        fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
        exit(1);
    }

    vector<int> evt;
    int Xstrip; // , Ystrip;
    // int Xadc, Yadc;
    vector<array<double,3>> TargetQX, TargetQX_raw; //  , TargetQY;
    vector<array<double,3>> err_TargetQX, err_TargetQX_raw; // , err_TargetQY;
    vector<double> LGQ, err_LGQ;
    vector<double> QTot_target_raw, QTot_target_raw_err;
    vector<double> QTot_target_cal, QTot_target_cal_err;
    vector<double> Qratio_raw, Qratio_raw_err;
    vector<double> Qratio_cal, Qratio_cal_err;
    vector<array<double,3>> cX, cXraw; // , cY;
    vector<array<double,3>> cX_err, cXraw_err; // , cY_err;
    // NEW
    vector<double> Xpos_wmean_raw, Xpos_wmean_raw_err;
    vector<double> Xpos_wmean_cal, Xpos_wmean_cal_err;

    RunEventCharge evCharge;
    bool ok = TargetChargePosition(runNumber, rootPattern, evCharge);
    if(!ok){
        cerr << "Processing failed for run " << runNumber << endl;
        return 1;
    }
    Xstrip = evCharge.centerXphys;
    // Ystrip = evCharge.centerYphys;
    // Xadc = evCharge.centerXadc;
    // Yadc = evCharge.centerYadc;
    evt = evCharge.eventNumber;
    TargetQX = evCharge.XqTar;
    TargetQX_raw = evCharge.XqTar_raw;
    err_TargetQX = evCharge.errXqTar;
    err_TargetQX_raw = evCharge.errXqTar_raw;
    // TargetQY = evCharge.YqTar;
    // err_TargetQY = evCharge.errYqTar;
    LGQ = evCharge.QLG;
    err_LGQ = evCharge.errQLG;
    QTot_target_raw = evCharge.QTot_target_raw;
    QTot_target_raw_err = evCharge.QTot_target_raw_err;
    QTot_target_cal = evCharge.QTot_target_cal;
    QTot_target_cal_err = evCharge.QTot_target_cal_err;
    Qratio_raw = evCharge.Qratio_raw;
    Qratio_raw_err = evCharge.Qratio_raw_err;
    Qratio_cal = evCharge.Qratio_cal;
    Qratio_cal_err = evCharge.Qratio_cal_err;
    cX = evCharge.coeffsX;
    cXraw = evCharge.coeffsX_raw;
    cX_err = evCharge.coeffsX_err;
    cXraw_err = evCharge.coeffsX_raw_err;
    // NEW
    Xpos_wmean_raw = evCharge.Xpos_wmean_raw;
    Xpos_wmean_raw_err = evCharge.Xpos_wmean_raw_err;
    Xpos_wmean_cal = evCharge.Xpos_wmean_cal;
    Xpos_wmean_cal_err = evCharge.Xpos_wmean_cal_err;
    // cY = evCharge.coeffsY;
    // cY_err = evCharge.coeffsY_err;

    // Number of fits done
    int Nfits = cX.size();
    if (Nfits == 0) {
        cerr << "No fits were performed — nothing to plot." << endl;
    } else {
        vector<double> beamPosX_mm(Nfits), beamPosX_mm_raw(Nfits); // , beamPosY_mm(Nfits);
        vector<double> beamPosX_mm_err(Nfits), beamPosX_mm_raw_err(Nfits);
        vector<double> beamWidthX(Nfits), beamWidthX_raw(Nfits); // , beamWidthY(Nfits);
        vector<double> chi2X(Nfits), chi2X_raw(Nfits); // , chi2Y(Nfits);
        vector<double> A_X(Nfits),  A_X_raw(Nfits); // ,  A_Y(Nfits);
        vector<double> idx(Nfits);
        // NEW: weighted mean arrays (ensure size matches Nfits)
        vector<double> beamPosX_wm_cal(Nfits), beamPosX_wm_cal_err(Nfits);
        vector<double> beamPosX_wm_raw(Nfits), beamPosX_wm_raw_err(Nfits);

        // Fill plotting arrays
        for (int i = 0; i < Nfits; i++) {
            idx[i] = i;

            double muX   = cX[i][1];
            double muX_raw  = cXraw[i][1];
            double dmuX  = fabs(cX_err[i][1]);
            double dmuX_raw = fabs(cXraw_err[i][1]);
            // double muY   = cY[i][1];
            double sigX  = fabs(cX[i][2]);
            double sigX_raw = fabs(cXraw[i][2]);
            // double sigY  = fabs(cY[i][2]);
            double ampX  = cX[i][0];
            double ampX_raw = cXraw[i][0];
            // double ampY  = cY[i][0];

            beamPosX_mm[i] = (Xstrip + muX) * 1.0;
            beamPosX_mm_raw[i] = (Xstrip + muX_raw) * 1.0;
            beamPosX_mm_err[i] = dmuX * 1.0;
            beamPosX_mm_raw_err[i] = dmuX_raw * 1.0;

            // NEW: weighted-mean from TargetChargePosition
            beamPosX_wm_cal[i] = (i < (int)Xpos_wmean_cal.size()) ? Xpos_wmean_cal[i] : 0.0;
            beamPosX_wm_cal_err[i] = (i < (int)Xpos_wmean_cal_err.size()) ? Xpos_wmean_cal_err[i] : 0.0;
            beamPosX_wm_raw[i] = (i < (int)Xpos_wmean_raw.size()) ? Xpos_wmean_raw[i] : 0.0;
            beamPosX_wm_raw_err[i] = (i < (int)Xpos_wmean_raw_err.size()) ? Xpos_wmean_raw_err[i] : 0.0;
        
            // beamPosY_mm[i] = (Ystrip - NCH + muY) * 1.0;
            beamWidthX[i] = sigX;
            beamWidthX_raw[i] = sigX_raw;
            // beamWidthY[i] = sigY;

            chi2X[i] = evCharge.gofX[i].chi2;
            chi2X_raw[i] = evCharge.gofX_raw[i].chi2;
            // chi2Y[i] = evCharge.gofY[i].chi2;
            A_X[i] = ampX;
            A_X_raw[i] = ampX_raw;
            // A_Y[i] = ampY;
        }

        // Create ROOT graphs
        TGraphErrors *gXTar = new TGraphErrors(Nfits, idx.data(), beamPosX_mm.data(), nullptr, nullptr);
        TGraphErrors *gXTar_raw = new TGraphErrors(Nfits, idx.data(), beamPosX_mm_raw.data(), nullptr, nullptr);

        // NEW: weighted mean graphs
        TGraphErrors *gXTar_wmean = new TGraphErrors(Nfits, idx.data(), beamPosX_wm_cal.data(), nullptr, beamPosX_wm_cal_err.data());
        TGraphErrors *gXTar_wmean_raw = new TGraphErrors(Nfits, idx.data(), beamPosX_wm_raw.data(), nullptr, beamPosX_wm_raw_err.data());

        // NEW: difference (fit - wmean)
        vector<double> dX_cal(Nfits), dX_raw(Nfits);
        for(int i=0;i<Nfits;i++){
            dX_cal[i] = beamPosX_mm[i] - beamPosX_wm_cal[i];
            dX_raw[i] = beamPosX_mm_raw[i] - beamPosX_wm_raw[i];
        }
        TGraphErrors *gDeltaX_fit_minus_wmean = new TGraphErrors(Nfits, idx.data(), dX_cal.data());
        TGraphErrors *gDeltaX_fit_minus_wmean_raw = new TGraphErrors(Nfits, idx.data(), dX_raw.data());
        // TGraphErrors *gYTar = new TGraphErrors(Nfits, idx.data(), beamPosY_mm.data());
        TGraphErrors *gsigmaXtar = new TGraphErrors(Nfits, idx.data(), beamWidthX.data());
        TGraphErrors *gsigmaXtar_raw = new TGraphErrors(Nfits, idx.data(), beamWidthX_raw.data());
        // TGraphErrors *gsigmaYtar = new TGraphErrors(Nfits, idx.data(), beamWidthY.data());
        TGraphErrors *gQLG = new TGraphErrors(Nfits, idx.data(), LGQ.data(), nullptr, err_LGQ.data());

        TGraphErrors *gQTot_target_raw = new TGraphErrors(Nfits, idx.data(), QTot_target_raw.data(), nullptr, QTot_target_raw_err.data());
        TGraphErrors *gQTot_target_cal = new TGraphErrors(Nfits, idx.data(), QTot_target_cal.data(), nullptr, QTot_target_cal_err.data());
        TGraphErrors *gQratio_raw = new TGraphErrors(Nfits, idx.data(), Qratio_raw.data(), nullptr, Qratio_raw_err.data());
        TGraphErrors *gQratio_cal = new TGraphErrors(Nfits, idx.data(), Qratio_cal.data(), nullptr, Qratio_cal_err.data());
        TGraphErrors *gChi2_X = new TGraphErrors(Nfits, idx.data(), chi2X.data());
        TGraphErrors *gChi2_X_raw = new TGraphErrors(Nfits, idx.data(), chi2X_raw.data());
        // TGraphErrors *gChi2_Y = new TGraphErrors(Nfits, idx.data(), chi2Y.data());
        TGraphErrors *gChargeX = new TGraphErrors(Nfits, idx.data(), A_X.data());
        TGraphErrors *gChargeX_raw = new TGraphErrors(Nfits, idx.data(), A_X_raw.data());
        // TGraphErrors *gChargeY = new TGraphErrors(Nfits, idx.data(), A_Y.data());

        TGraphAttribute(gXTar, "gXTar", "Period", "X fit (cal) [mm]", 20, kRed);
        TGraphAttribute(gXTar_raw, "gXTar_raw", "Period", "X fit (raw) [mm]", 20, kBlack);
        TGraphAttribute(gXTar_wmean, "gXTar_wmean", "Period", "X w.mean (cal) [mm]", 24, kBlue);
        TGraphAttribute(gXTar_wmean_raw, "gXTar_wmean_raw", "Period", "X w.mean (raw) [mm]", 24, kGreen+2);
        TGraphAttribute(gDeltaX_fit_minus_wmean, "gDeltaX_fit_minus_wmean", "Period", "X_fit - X_wmean (cal) [mm]", 20, kMagenta+1);
        TGraphAttribute(gDeltaX_fit_minus_wmean_raw, "gDeltaX_fit_minus_wmean_raw", "Period", "X_fit - X_wmean (raw) [mm]", 20, kOrange+7);
        // TGraphAttribute(gYTar, "gYTar", "Period", "Y [mm]", 20, kRed);
        TGraphAttribute(gQLG, "gQLG", "Period", "LG Q [pC]", 20, kRed);
        TGraphAttribute(gQTot_target_raw, "gQTot_target_raw", "Period", "Q [pC]", 20, kRed);
        TGraphAttribute(gQTot_target_cal, "gQTot_target_cal", "Period", "Q [pC]", 20, kRed);
        TGraphAttribute(gQratio_raw, "gQratio_raw", "Period", "Q Ratio Raw", 20, kRed);
        TGraphAttribute(gQratio_cal, "gQratio_cal", "Period", "Q Ratio Cal", 20, kRed);
        TGraphAttribute(gsigmaXtar, "gsigmaXtar", "Period", "Sigma X [mm]", 20, kRed);
        TGraphAttribute(gsigmaXtar_raw, "gsigmaXtar_raw", "Period", "Sigma X Raw [mm]", 20, kRed);
        // TGraphAttribute(gsigmaYtar, "gsigmaYYtar", "Period", "Sigma Y [mm]", 20, kRed);
        TGraphAttribute(gChi2_X, "gChi2_X", "Period", "Chi2", 20, kRed);
        TGraphAttribute(gChi2_X_raw, "gChi2_X_raw", "Period", "Chi2 Raw", 20, kRed);
        // TGraphAttribute(gChi2_Y, "gChi2_Y", "Period", "Chi2", 20, kRed);
        TGraphAttribute(gChargeX, "gChargeX", "Period", "QX [pC]", 20, kRed);
        TGraphAttribute(gChargeX_raw, "gChargeX_raw", "Period", "QX Raw [pC]", 20, kRed);
        // TGraphAttribute(gChargeY, "gChargeY", "Period", "QY [pC]", 20, kRed);
        
        histoFile->cd();
        gXTar->Write();
        gXTar_raw->Write();
        gXTar_wmean->Write();
        gXTar_wmean_raw->Write();
        gDeltaX_fit_minus_wmean->Write();
        gDeltaX_fit_minus_wmean_raw->Write();        
        // gYTar->Write();
        gQTot_target_raw->Write();
        gQTot_target_cal->Write();
        gQLG->Write();
        gQratio_raw->Write();
        gQratio_cal->Write();
        gsigmaXtar->Write();
        gsigmaXtar_raw->Write();
        // gsigmaYtar->Write();
        gChi2_X->Write();
        gChi2_X_raw->Write();
        // gChi2_Y->Write();
        gChargeX->Write();
        gChargeX_raw->Write();
        // gChargeY->Write();

        cout << "Plots successfully stored in " << outputFileName << endl;
    }


    printf("Output file%s closed \n",outputFileName.Data());
    histoFile->Close();
    
    cout << "Target position, charge and radiation damage estiamate done." << endl;

    return 0;
}

