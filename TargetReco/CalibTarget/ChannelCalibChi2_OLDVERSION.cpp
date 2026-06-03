// ChannelCalibChi2.cpp
// Calibrate one target channel using neighbor channels and LG reference.
// Usage:
//   ./ChannelCalib input.root 
//

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>

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
#include "TFitter.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TLine.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompSVD.h"

using namespace std;


// #define NTUPLE_N_BOARDS 2
// #define NTUPLE_N_CHANNELS 32
// #define VPP 1.
// #define BLOCKSIZE 100


// // --- ADC ↔ Physics mapping
// int ADCsToPhysics[NTUPLE_N_CHANNELS] = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
// int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// // --- LG reference
// const int LG_BRD = 0;
// const int LG_CH  = 31;

// // --- Global vectors for TMinuit FCN
// vector<double> g_qminus1, g_qnom, g_qplus1;
// vector<double> g_Qlg_avg, g_Qlg_err;

// // --- Helper: mean and unbiased sample stddev
// void mean_and_std(const vector<double>& v, double &mean, double &sigma) {
//     int n = v.size();
//     if (n==0) { mean=0; sigma=1; return; }
//     double s=0;
//     for(double x:v) s+=x;
//     mean=s/n;
//     double ss=0;
//     for(double x:v) ss+=(x-mean)*(x-mean);
//     sigma = (n>1) ? sqrt(ss/(n-1)) : 1.0;
// }

// // --- FCN for TMinuit
// void fcn(int &npar, double *grad, double &fval, double *par, int iflag) {
//     double chi2 = 0.0;
//     int nblocks = g_Qlg_avg.size();
//     for(int i=0;i<nblocks;i++){
//         double model = par[0]*g_qminus1[i] + par[1]*g_qnom[i] + par[2]*g_qplus1[i];
//         double sigma = g_Qlg_err[i];
//         if(sigma<=0) sigma=1.0;
//         double diff = model - g_Qlg_avg[i];
//         chi2 += diff*diff / (sigma*sigma);
//     }
//     fval = chi2;
// }

// // --- Struct to store results
// struct CalibResult {
//     int run;
//     int target_ch;
//     double c_minus1, c0, c_plus1;
//     double err_minus1, err0, err_plus1;
// };

// // --- Function to calibrate one file
// bool calibrateFile(int runNumber, int target_ch, CalibResult &result, const string &filePathPattern="/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco1/Reco_run_00%d.root") {
//     char fname[256];
//     sprintf(fname, filePathPattern.c_str(), runNumber);
//     cout << "Calibrating run " << runNumber << " ADC-channel " << target_ch << " - Physics strip " << ADCsToPhysics[target_ch] << "..." << endl;

//     TFile *f = TFile::Open(fname,"READ");
//     if(!f || f->IsZombie()) {
//         cerr << "Cannot open file " << fname << endl;
//         return false;
//     }

//     TTree* t = (TTree*)f->Get("NTU");
//     if(!t) { cerr << "Tree NTU not found in file " << fname << endl; f->Close(); return false; }

//     static Double_t QChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     static Double_t QChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     t->SetBranchAddress("QChOld", QChOld);
//     t->SetBranchAddress("QChCumFixed", QChCumFixed);

//     Long64_t nentries = t->GetEntries();
//     vector<double> block_Qlg, block_qminus1, block_qnom, block_qplus1;

//     g_Qlg_avg.clear();
//     g_Qlg_err.clear();
//     g_qminus1.clear();
//     g_qnom.clear();
//     g_qplus1.clear();

//     for(Long64_t ev=0; ev<nentries; ev++){
//         t->GetEntry(ev);

//         // --- map ADC channel to physical strip
//         int phys = ADCsToPhysics[target_ch];
//         int phys_m1 = (phys > 0) ? phys - 1 : -1;
//         int phys_p1 = (phys < NTUPLE_N_CHANNELS-1) ? phys + 1 : -1;

//         // convert back to ADC channels
//         int adc_m1 = (phys_m1 >= 0) ? PhysicsToADCs[phys_m1] : -1;
//         int adc_p1 = (phys_p1 >= 0) ? PhysicsToADCs[phys_p1] : -1;

//         double Qlg = QChOld[LG_BRD][LG_CH];
//         double q0  = QChCumFixed[1][target_ch];
//         double qm1 = (adc_m1 >= 0) ? QChCumFixed[1][adc_m1] : 0.0;
//         double qp1 = (adc_p1 >= 0) ? QChCumFixed[1][adc_p1] : 0.0;

//         block_Qlg.push_back(Qlg);
//         block_qnom.push_back(q0);
//         block_qminus1.push_back(qm1);
//         block_qplus1.push_back(qp1);

//         // --- if block full, compute averages
//         if(block_Qlg.size() == BLOCKSIZE){
//             double mQ,sQ; mean_and_std(block_Qlg,mQ,sQ);
//             double deltaQ = sQ/sqrt(block_Qlg.size());

//             double mm1,sm1,m0,s0,mp1,sp1;
//             mean_and_std(block_qminus1, mm1, sm1);
//             mean_and_std(block_qnom,    m0, s0);
//             mean_and_std(block_qplus1,  mp1, sp1);

//             g_Qlg_avg.push_back(mQ);
//             g_Qlg_err.push_back(deltaQ>0?deltaQ:1.0);
//             g_qminus1.push_back(mm1);
//             g_qnom.push_back(m0);
//             g_qplus1.push_back(mp1);

//             block_Qlg.clear();
//             block_qminus1.clear();
//             block_qnom.clear();
//             block_qplus1.clear();
//         }
//     }

//     // --- last partial block
//     if(!block_Qlg.empty()){
//         double mQ,sQ; mean_and_std(block_Qlg,mQ,sQ);
//         double deltaQ = sQ/sqrt(block_Qlg.size());

//         double mm1,sm1,m0,s0,mp1,sp1;
//         mean_and_std(block_qminus1, mm1, sm1);
//         mean_and_std(block_qnom,    m0, s0);
//         mean_and_std(block_qplus1,  mp1, sp1);

//         g_Qlg_avg.push_back(mQ);
//         g_Qlg_err.push_back(deltaQ>0?deltaQ:1.0);
//         g_qminus1.push_back(mm1);
//         g_qnom.push_back(m0);
//         g_qplus1.push_back(mp1);
//     }

//     // --- TMinuit
//     TMinuit minuit(3);
//     minuit.SetFCN(fcn);

//     double vstart[3] = {0.2,0.8,0.2};
//     double step[3]   = {0.002,0.002,0.002};

//     minuit.DefineParameter(0,"c_minus1",vstart[0],step[0],0,0);
//     minuit.DefineParameter(1,"c0",      vstart[1],step[1],0,0);
//     minuit.DefineParameter(2,"c_plus1", vstart[2],step[2],0,0);

//     minuit.Migrad();

//     double c_minus1,c0,c_plus1, err0,err1,err2;
//     minuit.GetParameter(0,c_minus1,err0);
//     minuit.GetParameter(1,c0,      err1);
//     minuit.GetParameter(2,c_plus1, err2);

//     result.run = runNumber;
//     result.target_ch = target_ch;
//     result.c_minus1 = c_minus1; result.err_minus1 = err0;
//     result.c0       = c0;       result.err0       = err1;
//     result.c_plus1  = c_plus1;  result.err_plus1  = err2;

//     f->Close();
//     return true;
// }

// // --- Main
// int main() {
//     // Run number and nominal channel
//     vector<pair<int,int>> runs_channels = {
//         {80166,4},{80167,11},{80168,5},{80169,10},{80170,6},{80171,9},{80173,7},
//         {80174,8},{80175,12},{80176,3},{80177,13},{80178,2},{80179,14},{80180,1},
//         {80181,15},{80507,19},{80508,26},{80509,29},{80510,25},{80511,30},{80512,31},
//         {80513,23},{80514,16},{80515,17},{80516,18},{80517,21}
//     };

//     vector<CalibResult> results;
//     for(auto &rc : runs_channels){
//         CalibResult res;
//         if(calibrateFile(rc.first, rc.second, res)) results.push_back(res);
//         else cerr << "Failed to calibrate run "<<rc.first<<endl;
//     }

//     // Write CSV
//     ofstream out("ChannelCalibrationResults1.csv");
//     out << "run,target_ch,c_minus1,err_minus1,c0,err0,c_plus1,err_plus1\n";
//     for(auto &r: results){
//         out << r.run << "," << r.target_ch << ","
//             << r.c_minus1 << "," << r.err_minus1 << ","
//             << r.c0 << "," << r.err0 << ","
//             << r.c_plus1 << "," << r.err_plus1 << "\n";
//     }
//     out.close();

//     cout << "Calibration done. Results written to ChannelCalibrationResults.csv" << endl;

//     return 0;
// }

////////////////////////// ChannelCalibChi2_XY_6coeff.cpp
// Y and Y strips in the same chi2 minimization.

// #define NTUPLE_N_BOARDS 2
// #define NTUPLE_N_CHANNELS 32
// #define BLOCKSIZE 100

// using namespace std;

// // --- ADC ↔ Physics mapping
// int ADCsToPhysics[NTUPLE_N_CHANNELS] = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
// int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// // --- LG reference
// const int LG_BRD = 0;
// const int LG_CH  = 31;

// // --- Global vectors for TMinuit FCN (6 constants)
// vector<double> g_qminus1_X, g_qnom_X, g_qplus1_X;
// vector<double> g_qminus1_Y, g_qnom_Y, g_qplus1_Y;
// vector<double> g_Qlg_avg, g_Qlg_err;

// // --- Helper: mean and unbiased sample stddev
// void mean_and_std(const vector<double>& v, double &mean, double &sigma) {
//     int n = v.size();
//     if (n==0) { mean=0; sigma=1; return; }
//     double s=0;
//     for(double x:v) s+=x;
//     mean=s/n;
//     double ss=0;
//     for(double x:v) ss+=(x-mean)*(x-mean);
//     sigma = (n>1) ? sqrt(ss/(n-1)) : 1.0;
// }

// // --- FCN for TMinuit (6 parameters: 3X + 3Y)
// void fcn(int &npar, double *grad, double &fval, double *par, int iflag) {
//     // par[0-2] -> X coefficients, par[3-5] -> Y coefficients
//     double chi2 = 0.0;
//     int nblocks = g_Qlg_avg.size();
//     for(int i=0;i<nblocks;i++){
//         double model_X = par[0]*g_qminus1_X[i] + par[1]*g_qnom_X[i] + par[2]*g_qplus1_X[i];
//         double model_Y = par[3]*g_qminus1_Y[i] + par[4]*g_qnom_Y[i] + par[5]*g_qplus1_Y[i];
//         double model = model_X + model_Y;
//         double sigma = g_Qlg_err[i];
//         if(sigma<=0) sigma=1.0;
//         double diff = model - g_Qlg_avg[i];
//         chi2 += diff*diff / (sigma*sigma);
//     }
//     fval = chi2;
// }

// // --- Struct to store results
// struct CalibResult {
//     int run;
//     int X_ch, Y_ch;
//     double c_minus1_X, c0_X, c_plus1_X;
//     double err_minus1_X, err0_X, err_plus1_X;
//     double c_minus1_Y, c0_Y, c_plus1_Y;
//     double err_minus1_Y, err0_Y, err_plus1_Y;
//     bool skipped = false;
// };

// // --- Check if channel is on edge
// bool isEdgeChannel(int adc_ch) {
//     return (adc_ch == 0 || adc_ch == 8 || adc_ch == 19 || adc_ch == 27);
// }

// // --- Function to calibrate one file
// bool calibrateFile(int runNumber, int X_ch, int Y_ch, CalibResult &result, const string &filePathPattern="/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco1/Reco_run_00%d.root") {
//     result.run = runNumber;
//     result.X_ch = X_ch;
//     result.Y_ch = Y_ch;

//     cout << "Calibrating run " << runNumber << " - ADC-channels: (" << X_ch << ", " << Y_ch << ") - Physics strips (" << ADCsToPhysics[X_ch] << ", " << ADCsToPhysics[Y_ch] << ") ..." << endl;

//     if(isEdgeChannel(X_ch) || isEdgeChannel(Y_ch)) {
//         cerr << "Run " << runNumber << " - Skipping edge channel X=" << X_ch << " Y=" << Y_ch << endl;
//         result.skipped = true;
//         return true;
//     }

//     char fname[256];
//     sprintf(fname, filePathPattern.c_str(), runNumber);

//     TFile *f = TFile::Open(fname,"READ");
//     if(!f || f->IsZombie()) { cerr << "Cannot open file " << fname << endl; return false; }

//     TTree* t = (TTree*)f->Get("NTU");
//     if(!t) { cerr << "Tree NTU not found in file " << fname << endl; f->Close(); return false; }

//     static Double_t QChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     static Double_t QChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     t->SetBranchAddress("QChOld", QChOld);
//     t->SetBranchAddress("QChCumFixed", QChCumFixed);

//     Long64_t nentries = t->GetEntries();
//     vector<double> block_Qlg, block_qminus1_X, block_qnom_X, block_qplus1_X;
//     vector<double> block_qminus1_Y, block_qnom_Y, block_qplus1_Y;

//     g_Qlg_avg.clear(); g_Qlg_err.clear();
//     g_qminus1_X.clear(); g_qnom_X.clear(); g_qplus1_X.clear();
//     g_qminus1_Y.clear(); g_qnom_Y.clear(); g_qplus1_Y.clear();

//     for(Long64_t ev=0; ev<nentries; ev++){
//         t->GetEntry(ev);

//         // --- map ADC -> physical
//         int phys_X = ADCsToPhysics[X_ch]; int phys_Y = ADCsToPhysics[Y_ch];

//         int phys_Xm1 = phys_X-1; int phys_Xp1 = phys_X+1;
//         int phys_Ym1 = phys_Y-1; int phys_Yp1 = phys_Y+1;

//         int adc_Xm1 = PhysicsToADCs[phys_Xm1]; int adc_Xp1 = PhysicsToADCs[phys_Xp1];
//         int adc_Ym1 = PhysicsToADCs[phys_Ym1]; int adc_Yp1 = PhysicsToADCs[phys_Yp1];

//         double Qlg = QChOld[LG_BRD][LG_CH];

//         double qnom_X = QChCumFixed[1][X_ch];
//         double qminus_X = QChCumFixed[1][adc_Xm1];
//         double qplus_X  = QChCumFixed[1][adc_Xp1];

//         double qnom_Y = QChCumFixed[1][Y_ch];
//         double qminus_Y = QChCumFixed[1][adc_Ym1];
//         double qplus_Y  = QChCumFixed[1][adc_Yp1];

//         block_Qlg.push_back(Qlg);
//         block_qnom_X.push_back(qnom_X); block_qminus1_X.push_back(qminus_X); block_qplus1_X.push_back(qplus_X);
//         block_qnom_Y.push_back(qnom_Y); block_qminus1_Y.push_back(qminus_Y); block_qplus1_Y.push_back(qplus_Y);

//         if(block_Qlg.size() == BLOCKSIZE){
//             double mQ,sQ, mm,sm,m0,s0,mp,sp;
//             mean_and_std(block_Qlg,mQ,sQ); g_Qlg_avg.push_back(mQ); g_Qlg_err.push_back(sQ/sqrt(block_Qlg.size()));
//             mean_and_std(block_qminus1_X, mm, sm); g_qminus1_X.push_back(mm);
//             mean_and_std(block_qnom_X, m0, s0); g_qnom_X.push_back(m0);
//             mean_and_std(block_qplus1_X, mp, sp); g_qplus1_X.push_back(mp);

//             mean_and_std(block_qminus1_Y, mm, sm); g_qminus1_Y.push_back(mm);
//             mean_and_std(block_qnom_Y, m0, s0); g_qnom_Y.push_back(m0);
//             mean_and_std(block_qplus1_Y, mp, sp); g_qplus1_Y.push_back(mp);

//             block_Qlg.clear();
//             block_qminus1_X.clear(); block_qnom_X.clear(); block_qplus1_X.clear();
//             block_qminus1_Y.clear(); block_qnom_Y.clear(); block_qplus1_Y.clear();
//         }
//     }

//     if(!block_Qlg.empty()){
//         double mQ,sQ, mm,sm,m0,s0,mp,sp;
//         mean_and_std(block_Qlg,mQ,sQ); g_Qlg_avg.push_back(mQ); g_Qlg_err.push_back(sQ/sqrt(block_Qlg.size()));
//         mean_and_std(block_qminus1_X, mm, sm); g_qminus1_X.push_back(mm);
//         mean_and_std(block_qnom_X, m0, s0); g_qnom_X.push_back(m0);
//         mean_and_std(block_qplus1_X, mp, sp); g_qplus1_X.push_back(mp);
//         mean_and_std(block_qminus1_Y, mm, sm); g_qminus1_Y.push_back(mm);
//         mean_and_std(block_qnom_Y, m0, s0); g_qnom_Y.push_back(m0);
//         mean_and_std(block_qplus1_Y, mp, sp); g_qplus1_Y.push_back(mp);
//     }

//     // --- TMinuit fit (6 parameters)
//     TMinuit minuit(6); minuit.SetFCN(fcn);
//     double vstart[6] = {0.2,0.6,0.2,0.2,0.6,0.2};
//     double step[6] = {0.002,0.002,0.002,0.002,0.002,0.002};

//     for(int i=0;i<6;i++) minuit.DefineParameter(i,Form("p%d",i),vstart[i],step[i],0,0);

//     minuit.Migrad();

//     double par[6], err[6];
//     for(int i=0;i<6;i++) minuit.GetParameter(i,par[i],err[i]);

//     result.c_minus1_X = par[0]; result.err_minus1_X = err[0];
//     result.c0_X       = par[1]; result.err0_X       = err[1];
//     result.c_plus1_X  = par[2]; result.err_plus1_X  = err[2];
//     result.c_minus1_Y = par[3]; result.err_minus1_Y = err[3];
//     result.c0_Y       = par[4]; result.err0_Y       = err[4];
//     result.c_plus1_Y  = par[5]; result.err_plus1_Y  = err[5];

//     f->Close();
//     return true;
// }

// // --- Main
// int main(){
//     vector<tuple<int,int,int>> runs_channels = {
//         //{80166,4,23},{80167,11,23},{80168,5,23},{80169,10,23},{80170,6,23},{80171,9,23},{80173,7,23},
//         //{80174,8,23},{80175,12,23},{80176,3,23},{80177,13,23},{80178,2,23},{80179,14,23},{80180,1,23},{80181,15,23},
//         {80507,4,19},{80508,4,26},{80509,4,29},{80510,4,25},{80511,4,30},{80512,4,31},
//         {80513,4,23},{80514,4,16},{80515,4,17},{80516,4,18},{80517,4,21}
//     };

//     vector<CalibResult> results;
//     for(auto &rc : runs_channels){
//         CalibResult res;
//         if(calibrateFile(get<0>(rc), get<1>(rc), get<2>(rc), res)) results.push_back(res);
//     }

//     ofstream out("ChannelCalibrationResults_XY_6coeff.csv");
//     out << "run,X_ch,Y_ch,"
//         << "c_minus1_X,err_minus1_X,c0_X,err0_X,c_plus1_X,err_plus1_X,"
//         << "c_minus1_Y,err_minus1_Y,c0_Y,err0_Y,c_plus1_Y,err_plus1_Y,skipped\n";
//     for(auto &r: results){
//         out << r.run << "," << r.X_ch << "," << r.Y_ch << ","
//             << r.c_minus1_X << "," << r.err_minus1_X << ","
//             << r.c0_X << "," << r.err0_X << ","
//             << r.c_plus1_X << "," << r.err_plus1_X << ","
//             << r.c_minus1_Y << "," << r.err_minus1_Y << ","
//             << r.c0_Y << "," << r.err0_Y << ","
//             << r.c_plus1_Y << "," << r.err_plus1_Y << "," << r.skipped << "\n";
//     }
//     out.close();

//     cout << "Calibration done. Results written to ChannelCalibrationResults_XY_6coeff.csv" << endl;
//     return 0;
// }

// //////////////////////////////// X-Y strips independently treated
// #define NTUPLE_N_BOARDS 2
// #define NTUPLE_N_CHANNELS 32
// #define BLOCKSIZE 100

// using namespace std;

// // --- ADC ↔ Physics mapping
// int ADCsToPhysics[NTUPLE_N_CHANNELS] = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
// int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// // --- LG reference
// const int LG_BRD = 0;
// const int LG_CH  = 31;

// // --- Global vectors for TMinuit FCN
// vector<double> g_qminus1, g_qnom, g_qplus1;
// vector<double> g_Qlg_avg, g_Qlg_err;

// // --- Helper: mean and unbiased sample stddev
// void mean_and_std(const vector<double>& v, double &mean, double &sigma) {
//     int n = v.size();
//     if (n==0) { mean=0; sigma=1; return; }
//     double s=0;
//     for(double x:v) s+=x;
//     mean=s/n;
//     double ss=0;
//     for(double x:v) ss+=(x-mean)*(x-mean);
//     sigma = (n>1) ? sqrt(ss/(n-1)) : 1.0;
// }

// // --- FCN for TMinuit (3 parameters)
// void fcn(int &npar, double *grad, double &fval, double *par, int iflag) {
//     double chi2 = 0.0;
//     int nblocks = g_Qlg_avg.size();
//     for(int i=0;i<nblocks;i++){
//         double model = par[0]*g_qminus1[i] + par[1]*g_qnom[i] + par[2]*g_qplus1[i];
//         double sigma = g_Qlg_err[i];
//         if(sigma<=0) sigma=1.0;
//         double diff = model - g_Qlg_avg[i];
//         chi2 += diff*diff / (sigma*sigma);
//     }
//     fval = chi2;
// }

// // --- Helper to fit one direction
// void fitDirection(const vector<double> &qminus, const vector<double> &q0, const vector<double> &qplus,
//                   const vector<double> &Qlg, double &c_minus1, double &c0, double &c_plus1,
//                   double &err_minus1, double &err0, double &err_plus1) 
// {
//     g_qminus1 = qminus;
//     g_qnom = q0;
//     g_qplus1 = qplus;
//     g_Qlg_avg = Qlg;
//     g_Qlg_err.resize(Qlg.size());
//     for(size_t i=0;i<Qlg.size();i++) g_Qlg_err[i]=1.0;

//     TMinuit minuit(3);
//     minuit.SetFCN(fcn);
//     double vstart[3] = {0.2,0.6,0.2};
//     double step[3] = {0.002,0.002,0.002};
//     minuit.DefineParameter(0,"c_minus1",vstart[0],step[0],0,0);
//     minuit.DefineParameter(1,"c0",vstart[1],step[1],0,0);
//     minuit.DefineParameter(2,"c_plus1",vstart[2],step[2],0,0);

//     minuit.Migrad();

//     minuit.GetParameter(0,c_minus1,err_minus1);
//     minuit.GetParameter(1,c0,err0);
//     minuit.GetParameter(2,c_plus1,err_plus1);
// }

// // --- Struct to store results
// struct CalibResult {
//     int run;
//     int X_ch, Y_ch;
//     double c_minus1_X, c0_X, c_plus1_X;
//     double err_minus1_X, err0_X, err_plus1_X;
//     double c_minus1_Y, c0_Y, c_plus1_Y;
//     double err_minus1_Y, err0_Y, err_plus1_Y;
//     bool skipped = false;
// };

// // --- Check if channel is on edge
// bool isEdgeChannel(int adc_ch) {
//     return (adc_ch == 0 || adc_ch == 8 || adc_ch == 19 || adc_ch == 27);
// }

// // --- Calibrate one file
// bool calibrateFile(int runNumber, int X_ch, int Y_ch, CalibResult &result,
//                    const string &filePathPattern="/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco1/Reco_run_00%d.root") 
// {
//     result.run = runNumber;
//     result.X_ch = X_ch;
//     result.Y_ch = Y_ch;

//     if(isEdgeChannel(X_ch) || isEdgeChannel(Y_ch)) {
//         cerr << "Skipping edge channel: X=" << X_ch << " Y=" << Y_ch << endl;
//         result.skipped = true;
//         return true;
//     }

//     char fname[256];
//     sprintf(fname, filePathPattern.c_str(), runNumber);
//     TFile *f = TFile::Open(fname,"READ");
//     if(!f || f->IsZombie()) { cerr << "Cannot open file " << fname << endl; return false; }

//     TTree* t = (TTree*)f->Get("NTU");
//     if(!t) { cerr << "Tree NTU not found in file " << fname << endl; f->Close(); return false; }

//     static Double_t QChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     static Double_t QChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     t->SetBranchAddress("QChOld", QChOld);
//     t->SetBranchAddress("QChCumFixed", QChCumFixed);

//     Long64_t nentries = t->GetEntries();
//     vector<double> block_Qlg, block_qminus1_X, block_qnom_X, block_qplus1_X;
//     vector<double> block_qminus1_Y, block_qnom_Y, block_qplus1_Y;

//     g_Qlg_avg.clear();
//     g_qminus1.clear(); g_qnom.clear(); g_qplus1.clear();

//     for(Long64_t ev=0; ev<nentries; ev++){
//         t->GetEntry(ev);

//         int phys_X = ADCsToPhysics[X_ch]; int phys_Y = ADCsToPhysics[Y_ch];
//         int adc_Xm1 = PhysicsToADCs[phys_X-1]; int adc_Xp1 = PhysicsToADCs[phys_X+1];
//         int adc_Ym1 = PhysicsToADCs[phys_Y-1]; int adc_Yp1 = PhysicsToADCs[phys_Y+1];

//         double Qlg = QChOld[LG_BRD][LG_CH];

//         double qnom_X = QChCumFixed[1][X_ch];
//         double qminus_X = QChCumFixed[1][adc_Xm1];
//         double qplus_X  = QChCumFixed[1][adc_Xp1];

//         double qnom_Y = QChCumFixed[1][Y_ch];
//         double qminus_Y = QChCumFixed[1][adc_Ym1];
//         double qplus_Y  = QChCumFixed[1][adc_Yp1];

//         block_Qlg.push_back(Qlg);
//         block_qminus1_X.push_back(qminus_X); block_qnom_X.push_back(qnom_X); block_qplus1_X.push_back(qplus_X);
//         block_qminus1_Y.push_back(qminus_Y); block_qnom_Y.push_back(qnom_Y); block_qplus1_Y.push_back(qplus_Y);

//         if(block_Qlg.size() == BLOCKSIZE){
//             double mQ,sQ, mm,sm,m0,s0,mp,sp;
//             mean_and_std(block_Qlg,mQ,sQ); g_Qlg_avg.push_back(mQ);
//             mean_and_std(block_qminus1_X, mm, sm); g_qminus1.push_back(mm);
//             mean_and_std(block_qnom_X, m0, s0); g_qnom.push_back(m0);
//             mean_and_std(block_qplus1_X, mp, sp); g_qplus1.push_back(mp);

//             mean_and_std(block_qminus1_Y, mm, sm); g_qminus1.push_back(mm);
//             mean_and_std(block_qnom_Y, m0, s0); g_qnom.push_back(m0);
//             mean_and_std(block_qplus1_Y, mp, sp); g_qplus1.push_back(mp);

//             block_Qlg.clear();
//             block_qminus1_X.clear(); block_qnom_X.clear(); block_qplus1_X.clear();
//             block_qminus1_Y.clear(); block_qnom_Y.clear(); block_qplus1_Y.clear();
//         }
//     }

//     // --- Fit X
//     fitDirection(block_qminus1_X, block_qnom_X, block_qplus1_X, block_Qlg,
//                  result.c_minus1_X, result.c0_X, result.c_plus1_X,
//                  result.err_minus1_X, result.err0_X, result.err_plus1_X);

//     // --- Fit Y
//     fitDirection(block_qminus1_Y, block_qnom_Y, block_qplus1_Y, block_Qlg,
//                  result.c_minus1_Y, result.c0_Y, result.c_plus1_Y,
//                  result.err_minus1_Y, result.err0_Y, result.err_plus1_Y);

//     f->Close();
//     return true;
// }

// // --- Main
// int main() {
//     vector<tuple<int,int,int>> runs_channels = {
//         {80507,4,19},{80508,4,26},{80509,4,29},{80510,4,25},{80511,4,30},{80512,4,31},
//         {80513,4,23},{80514,4,16},{80515,4,17},{80516,4,18},{80517,4,21}
//     };

//     vector<CalibResult> results;
//     for(auto &rc : runs_channels){
//         CalibResult res;
//         if(calibrateFile(get<0>(rc), get<1>(rc), get<2>(rc), res)) results.push_back(res);
//     }

//     ofstream out("ChannelCalibrationResults_XY_independent.csv");
//     out << "run,X_ch,Y_ch,"
//         << "c_minus1_X,err_minus1_X,c0_X,err0_X,c_plus1_X,err_plus1_X,"
//         << "c_minus1_Y,err_minus1_Y,c0_Y,err0_Y,c_plus1_Y,err_plus1_Y,skipped\n";
//     for(auto &r: results){
//         out << r.run << "," << r.X_ch << "," << r.Y_ch << ","
//             << r.c_minus1_X << "," << r.err_minus1_X << ","
//             << r.c0_X << "," << r.err0_X << ","
//             << r.c_plus1_X << "," << r.err_plus1_X << ","
//             << r.c_minus1_Y << "," << r.err_minus1_Y << ","
//             << r.c0_Y << "," << r.err0_Y << ","
//             << r.c_plus1_Y << "," << r.err_plus1_Y << "," << r.skipped << "\n";
//     }
//     out.close();

//     cout << "Calibration done. Results written to ChannelCalibrationResults_XY_independent.csv" << endl;
//     return 0;
// }



// /////////////// bare calibration constant introduction

// #define NTUPLE_N_BOARDS 2
// #define NTUPLE_N_CHANNELS 32
// #define BLOCKSIZE 100

// using namespace std;

// // --- ADC ↔ Physics mapping
// int ADCsToPhysics[NTUPLE_N_CHANNELS] = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
// int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// // --- LG reference
// const int LG_BRD = 0;
// const int LG_CH  = 31;

// // --- Global vectors for TMinuit FCN
// vector<double> g_qminus1, g_qnom, g_qplus1;
// vector<double> g_Qlg_avg, g_Qlg_err;

// // --- Helper: mean and unbiased sample stddev
// void mean_and_std(const vector<double>& v, double &mean, double &sigma) {
//     int n = v.size();
//     if (n==0) { mean=0; sigma=1; return; }
//     double s=0;
//     for(double x:v) s+=x;
//     mean=s/n;
//     double ss=0;
//     for(double x:v) ss+=(x-mean)*(x-mean);
//     sigma = (n>1) ? sqrt(ss/(n-1)) : 1.0;
// }

// // --- FCN for TMinuit (3 parameters)
// void fcn(int &npar, double *grad, double &fval, double *par, int iflag) {
//     double chi2 = 0.0;
//     int nblocks = g_Qlg_avg.size();
//     for(int i=0;i<nblocks;i++){
//         double model = par[0]*g_qminus1[i] + par[1]*g_qnom[i] + par[2]*g_qplus1[i];
//         double sigma = g_Qlg_err[i];
//         if(sigma<=0) sigma=1.0;
//         double diff = model - g_Qlg_avg[i];
//         chi2 += diff*diff / (sigma*sigma);
//     }
//     fval = chi2;
// }

// // --- Helper to fit one direction
// void fitDirection(const vector<double> &qminus, const vector<double> &q0, const vector<double> &qplus,
//                   const vector<double> &Qlg, double &c_minus1, double &c0, double &c_plus1,
//                   double &err_minus1, double &err0, double &err_plus1) 
// {
//     g_qminus1 = qminus;
//     g_qnom = q0;
//     g_qplus1 = qplus;
//     g_Qlg_avg = Qlg;
//     g_Qlg_err.resize(Qlg.size());
//     for(size_t i=0;i<Qlg.size();i++) g_Qlg_err[i]=1.0;

//     TMinuit minuit(3);
//     minuit.SetFCN(fcn);
//     double vstart[3] = {0.2,0.6,0.2};
//     double step[3] = {0.002,0.002,0.002};
//     minuit.DefineParameter(0,"c_minus1",vstart[0],step[0],0,0);
//     minuit.DefineParameter(1,"c0",vstart[1],step[1],0,0);
//     minuit.DefineParameter(2,"c_plus1",vstart[2],step[2],0,0);

//     minuit.Migrad();

//     minuit.GetParameter(0,c_minus1,err_minus1);
//     minuit.GetParameter(1,c0,err0);
//     minuit.GetParameter(2,c_plus1,err_plus1);
// }

// // --- Struct to store results
// struct CalibResult {
//     int run;
//     int X_ch, Y_ch;
//     double c_minus1_X, c0_X, c_plus1_X;
//     double err_minus1_X, err0_X, err_plus1_X;
//     double c_minus1_Y, c0_Y, c_plus1_Y;
//     double err_minus1_Y, err0_Y, err_plus1_Y;
//     bool skipped = false;
// };

// // --- Check if channel is on edge
// bool isEdgeChannel(int adc_ch) {
//     return (adc_ch == 0 || adc_ch == 8 || adc_ch == 19 || adc_ch == 27);
// }

// // --- Load bare calibration constants from file, using CCumFixed column
// bool loadBareCalibration(const string &filename, map<int,double> &bareCal) {
//     ifstream infile(filename);
//     if(!infile.is_open()) { 
//         cerr << "Cannot open bare calibration file " << filename << endl; 
//         return false; 
//     }

//     string line;
//     getline(infile,line); // read header
//     istringstream header(line);
//     string token;
//     int colIndex = -1;
//     int col=0;
//     while(header >> token){
//         if(token == "CCumFixed") { colIndex = col; break; }
//         col++;
//     }
//     if(colIndex < 0){ cerr << "CCumFixed column not found in header!" << endl; return false; }

//     while(getline(infile,line)){
//         if(line.empty() || line[0]=='#') continue;
//         istringstream ss(line);
//         int adc;
//         ss >> adc;
//         string temp;
//         for(int i=0;i<colIndex;i++) ss >> temp; // skip columns
//         double cal;
//         ss >> cal;
//         if(cal==0) cal=1.0;
//         bareCal[adc] = cal;
//     }

//     infile.close();
//     return true;
// }

// // --- Calibrate one file
// bool calibrateFile(int runNumber, int X_ch, int Y_ch, const map<int,double> &bareCal, CalibResult &result,
//                    const string &filePathPattern="/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco1/Reco_run_00%d.root") 
// {
//     result.run = runNumber;
//     result.X_ch = X_ch;
//     result.Y_ch = Y_ch;

//     if(isEdgeChannel(X_ch) || isEdgeChannel(Y_ch)) {
//         cerr << "Skipping edge channel: X=" << X_ch << " Y=" << Y_ch << endl;
//         result.skipped = true;
//         return true;
//     }

//     char fname[256];
//     sprintf(fname, filePathPattern.c_str(), runNumber);
//     TFile *f = TFile::Open(fname,"READ");
//     if(!f || f->IsZombie()) { cerr << "Cannot open file " << fname << endl; return false; }

//     TTree* t = (TTree*)f->Get("NTU");
//     if(!t) { cerr << "Tree NTU not found in file " << fname << endl; f->Close(); return false; }

//     static Double_t QChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     static Double_t QChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     t->SetBranchAddress("QChOld", QChOld);
//     t->SetBranchAddress("QChCumFixed", QChCumFixed);

//     Long64_t nentries = t->GetEntries();
//     vector<double> block_Qlg, block_qminus1_X, block_qnom_X, block_qplus1_X;
//     vector<double> block_qminus1_Y, block_qnom_Y, block_qplus1_Y;

//     g_Qlg_avg.clear();
//     g_qminus1.clear(); g_qnom.clear(); g_qplus1.clear();

//     double cal_X = bareCal.at(X_ch);
//     double cal_Y = bareCal.at(Y_ch);

//     for(Long64_t ev=0; ev<nentries; ev++){
//         t->GetEntry(ev);

//         int phys_X = ADCsToPhysics[X_ch]; int phys_Y = ADCsToPhysics[Y_ch];
//         int adc_Xm1 = PhysicsToADCs[phys_X-1]; int adc_Xp1 = PhysicsToADCs[phys_X+1];
//         int adc_Ym1 = PhysicsToADCs[phys_Y-1]; int adc_Yp1 = PhysicsToADCs[phys_Y+1];

//         double Qlg = QChOld[LG_BRD][LG_CH];

//         double qnom_X = QChCumFixed[1][X_ch]*cal_X;
//         double qminus_X = QChCumFixed[1][adc_Xm1]*cal_X;
//         double qplus_X  = QChCumFixed[1][adc_Xp1]*cal_X;

//         double qnom_Y = QChCumFixed[1][Y_ch]*cal_Y;
//         double qminus_Y = QChCumFixed[1][adc_Ym1]*cal_Y;
//         double qplus_Y  = QChCumFixed[1][adc_Yp1]*cal_Y;

//         block_Qlg.push_back(Qlg);
//         block_qminus1_X.push_back(qminus_X); block_qnom_X.push_back(qnom_X); block_qplus1_X.push_back(qplus_X);
//         block_qminus1_Y.push_back(qminus_Y); block_qnom_Y.push_back(qnom_Y); block_qplus1_Y.push_back(qplus_Y);

//         if(block_Qlg.size() == BLOCKSIZE){
//             g_Qlg_avg.push_back(Qlg);
//             g_qminus1.push_back(qminus_X); g_qnom.push_back(qnom_X); g_qplus1.push_back(qplus_X);
//             g_qminus1.push_back(qminus_Y); g_qnom.push_back(qnom_Y); g_qplus1.push_back(qplus_Y);
//             block_Qlg.clear();
//             block_qminus1_X.clear(); block_qnom_X.clear(); block_qplus1_X.clear();
//             block_qminus1_Y.clear(); block_qnom_Y.clear(); block_qplus1_Y.clear();
//         }
//     }

//     // --- Fit X
//     fitDirection(block_qminus1_X, block_qnom_X, block_qplus1_X, block_Qlg,
//                  result.c_minus1_X, result.c0_X, result.c_plus1_X,
//                  result.err_minus1_X, result.err0_X, result.err_plus1_X);

//     // --- Fit Y
//     fitDirection(block_qminus1_Y, block_qnom_Y, block_qplus1_Y, block_Qlg,
//                  result.c_minus1_Y, result.c0_Y, result.c_plus1_Y,
//                  result.err_minus1_Y, result.err0_Y, result.err_plus1_Y);

//     f->Close();
//     return true;
// }

// // --- Main
// int main() {
//     map<int,double> bareCal;
//     if(!loadBareCalibration("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/calibration/final_target_calibration.txt", bareCal)) return 1;

//     vector<tuple<int,int,int>> runs_channels = {
//         {80507,4,19},{80508,4,26},{80509,4,29},{80510,4,25},{80511,4,30},{80512,4,31},
//         {80513,4,23},{80514,4,16},{80515,4,17},{80516,4,18},{80517,4,21}
//     };

//     vector<CalibResult> results;
//     for(auto &rc : runs_channels){
//         CalibResult res;
//         if(calibrateFile(get<0>(rc), get<1>(rc), get<2>(rc), bareCal, res)) results.push_back(res);
//     }

//     ofstream out("ChannelCalibrationResults_XY_bare.csv");
//     out << "run,X_ch,Y_ch,"
//         << "c_minus1_X,err_minus1_X,c0_X,err0_X,c_plus1_X,err_plus1_X,"
//         << "c_minus1_Y,err_minus1_Y,c0_Y,err0_Y,c_plus1_Y,err_plus1_Y,skipped\n";
//     for(auto &r: results){
//         out << r.run << "," << r.X_ch << "," << r.Y_ch << ","
//             << r.c_minus1_X << "," << r.err_minus1_X << ","
//             << r.c0_X << "," << r.err0_X << ","
//             << r.c_plus1_X << "," << r.err_plus1_X << ","
//             << r.c_minus1_Y << "," << r.err_minus1_Y << ","
//             << r.c0_Y << "," << r.err0_Y << ","
//             << r.c_plus1_Y << "," << r.err_plus1_Y << "," << r.skipped << "\n";
//     }
//     out.close();

//     cout << "Calibration done. Results written to ChannelCalibrationResults_XY_bare.csv" << endl;
//     return 0;
// }


// ChannelCalib_AutoXY.cpp
// Auto-detect fired strip on X and Y (max integrated charge) and perform independent chi2 fits
// Usage: compile and run in environment with ROOT available
//
// Compile example:
//   g++ ChannelCalib_AutoXY.cpp $(root-config --cflags --libs) -O2 -o ChannelCalib_AutoXY
//

// #define NTUPLE_N_BOARDS 2
// #define NTUPLE_N_CHANNELS 32
// #define BLOCKSIZE 100

// // --- ADC <-> Physics mapping (user-provided)
// int ADCsToPhysics[NTUPLE_N_CHANNELS]  = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
// int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// // --- LG reference (as before)
// const int LG_BRD = 0;
// const int LG_CH  = 31;

// // --- Globals used by TMinuit FCN
// static vector<double> g_qminus1, g_qnom, g_qplus1;
// static vector<double> g_Qlg_avg, g_Qlg_err;

// // mean & unbiased stddev helper
// void mean_and_std(const vector<double>& v, double &mean, double &sigma) {
//     int n = (int)v.size();
//     if (n==0) { mean=0; sigma=1; return; }
//     double s=0;
//     for(double x:v) s+=x;
//     mean = s / n;
//     double ss=0;
//     for(double x:v) ss += (x-mean)*(x-mean);
//     sigma = (n>1) ? sqrt(ss/(n-1)) : 1.0;
// }

// // FCN for 3-parameter fit (c_minus1, c0, c_plus1)
// void fcn(int &npar, double *grad, double &fval, double *par, int iflag) {
//     double chi2 = 0.0;
//     size_t nblocks = g_Qlg_avg.size();
//     for(size_t i=0;i<nblocks;i++){
//         double model = par[0]*g_qminus1[i] + par[1]*g_qnom[i] + par[2]*g_qplus1[i];
//         double sigma = (i < g_Qlg_err.size() && g_Qlg_err[i]>0) ? g_Qlg_err[i] : 1.0;
//         double diff = model - g_Qlg_avg[i];
//         chi2 += diff*diff / (sigma*sigma);
//     }
//     fval = chi2;
// }

// // helper: perform one direction fit (fills c and errors)
// void fitDirection(const vector<double> &qminus_blocks, const vector<double> &q0_blocks, const vector<double> &qplus_blocks,
//                   const vector<double> &Qlg_blocks, const vector<double> &Qlg_err_blocks,
//                   double &c_minus1, double &c0, double &c_plus1,
//                   double &err_minus1, double &err0, double &err_plus1)
// {
//     // set globals for FCN
//     g_qminus1 = qminus_blocks;
//     g_qnom    = q0_blocks;
//     g_qplus1  = qplus_blocks;
//     g_Qlg_avg = Qlg_blocks;
//     g_Qlg_err = Qlg_err_blocks;

//     TMinuit minuit(3);
//     minuit.SetFCN(fcn);

//     double vstart[3] = {0.15, 0.7, 0.15};
//     double step[3]   = {0.005,0.005,0.005};
//     minuit.DefineParameter(0,"c_minus1", vstart[0], step[0], 0, 0);
//     minuit.DefineParameter(1,"c0",       vstart[1], step[1], 0, 0);
//     minuit.DefineParameter(2,"c_plus1",  vstart[2], step[2], 0, 0);

//     minuit.Migrad();

//     minuit.GetParameter(0, c_minus1, err_minus1);
//     minuit.GetParameter(1, c0,       err0);
//     minuit.GetParameter(2, c_plus1,  err_plus1);
// }

// // Load bare calibration file and pick CCumFixed column; zero -> 1.0
// bool loadBareCalibration(const string &filename, map<int,double> &bareCal) {
//     ifstream in(filename);
//     if(!in.is_open()) {
//         cerr << "Cannot open bare calibration file: " << filename << endl;
//         return false;
//     }

//     string header;
//     if(!getline(in, header)) {
//         cerr << "Empty calibration file: " << filename << endl;
//         return false;
//     }

//     // tokenize header to find CCumFixed index
//     vector<string> hdr_tokens;
//     {
//         istringstream hs(header);
//         string tk;
//         while(hs >> tk) hdr_tokens.push_back(tk);
//     }

//     int ccumIndex = -1;
//     for(size_t i=0;i<hdr_tokens.size();++i) {
//         if(hdr_tokens[i] == "CCumFixed") { ccumIndex = (int)i; break; }
//     }
//     if(ccumIndex < 0) {
//         cerr << "Header column 'CCumFixed' not found in file " << filename << endl;
//         return false;
//     }

//     string line;
//     while(getline(in, line)) {
//         if(line.empty()) continue;
//         if(line[0] == '#') continue;
//         istringstream ss(line);
//         vector<string> cols;
//         string token;
//         while(ss >> token) cols.push_back(token);
//         if(cols.size() <= (size_t)ccumIndex) {
//             cerr << "Malformed line in calibration file (too few columns): " << line << endl;
//             continue;
//         }
//         int adc = stoi(cols[0]);
//         double val = stod(cols[ccumIndex]);
//         if(val == 0.0) val = 1.0;
//         bareCal[adc] = val;
//     }

//     in.close();

//     // ensure all ADC channels have an entry (if not, default to 1)
//     for(int ch=0; ch<NTUPLE_N_CHANNELS; ++ch) {
//         if(bareCal.find(ch) == bareCal.end()) bareCal[ch] = 1.0;
//     }

//     return true;
// }

// // Return true if physics strip is edge for ±1 neighbor (phys==0 or phys==15 for X; phys==16 or 31 for Y)
// bool isEdgePhysics(int phys) {
//     return (phys==0 || phys==15 || phys==16 || phys==31);
// }

// struct ResultRow {
//     int run;
//     int centerX_phys, centerY_phys;
//     int centerX_adc, centerY_adc;
//     double cXm1, err_Xm1, cX0, err_X0, cXp1, err_Xp1;
//     double cYm1, err_Ym1, cY0, err_Y0, cYp1, err_Yp1;
//     bool skipped;
//     int nblocks;
// };

// // Main calibration routine for one run: Auto-detect center X and center Y and build block-averages
// bool processRun(int runNumber,
//                 const string &rootFilePattern,
//                 // const map<int,double> &bareCal,
//                 ResultRow &outRow)
// {
//     char fname[512];
//     sprintf(fname, rootFilePattern.c_str(), runNumber);

//     TFile *f = TFile::Open(fname, "READ");
//     if(!f || f->IsZombie()) {
//         cerr << "Cannot open ROOT file " << fname << endl;
//         return false;
//     }

//     TTree *t = (TTree*) f->Get("NTU");
//     if(!t) {
//         cerr << "Tree 'NTU' not found in file " << fname << endl;
//         f->Close();
//         return false;
//     }

//     static Double_t QChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     static Double_t QChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
//     Int_t TrigMask;
//     t->SetBranchAddress("QChOld", QChOld);
//     t->SetBranchAddress("QChCumFixed", QChCumFixed);
//     t->SetBranchAddress("TrigMask", &TrigMask);

//     Long64_t nentries = t->GetEntries();
//     if(nentries <= 0) {
//         cerr << "No entries in tree for run " << runNumber << endl;
//         f->Close();
//         return false;
//     }

//     // accumulate total integrated charge per physics strip separately for X (phys 0..15) and Y (16..31)
//     vector<double> totalX(16,0.0), totalY(16,0.0);
//     for(Long64_t ev=0; ev<nentries; ++ev){
//         t->GetEntry(ev);
//         for(int adc=0; adc<NTUPLE_N_CHANNELS; ++adc) {
//             int phys = ADCsToPhysics[adc];
//             if(phys < 16) totalX[phys] += QChCumFixed[1][adc];
//             else totalY[phys-16] += QChCumFixed[1][adc];
//         }
//     }

//     // find maxima (physics indices)
//     int centerX_phys = (int)(max_element(totalX.begin(), totalX.end()) - totalX.begin());
//     int centerY_phys = (int)(max_element(totalY.begin(), totalY.end()) - totalY.begin()) + 16;

//     // map to ADC
//     int adc_X0 = PhysicsToADCs[centerX_phys];
//     int adc_Y0 = PhysicsToADCs[centerY_phys];
//     int adc_Xm1 = PhysicsToADCs[centerX_phys-1];
//     int adc_Xp1 = PhysicsToADCs[centerX_phys+1];
//     int adc_Ym1 = PhysicsToADCs[centerY_phys-1];
//     int adc_Yp1 = PhysicsToADCs[centerY_phys+1];

//     outRow.run = runNumber;
//     outRow.centerX_phys = centerX_phys;
//     outRow.centerY_phys = centerY_phys;
//     outRow.centerX_adc = adc_X0;
//     outRow.centerY_adc = adc_Y0;
//     outRow.skipped = false;
//     outRow.nblocks = 0;

//     // If center is on physics edge (no ±1 neighbor), skip
//     if(isEdgePhysics(centerX_phys) || isEdgePhysics(centerY_phys)) {
//         cerr << "Run " << runNumber << ": center on edge (physX=" << centerX_phys << " physY=" << centerY_phys << ") -> skipping" << endl;
//         outRow.skipped = true;
//         f->Close();
//         return true;
//     }

//     // print out channel and nieghbours 
//     cout << "RunNumber: " << runNumber <<endl;
//     cout << "Target (X,Y) position: (" << centerX_phys << ", " << centerY_phys << ")" << endl;
//     cout << "Target (X_minus,X_plus) : (" << centerX_phys-1 << ", " << centerX_phys+1 << ")" << " - Target (Y_minus,Y_plus) : (" << centerY_phys-1 << ", " << centerY_phys+1 << ")" << endl;
//     cout << "Target (X,Y) ADCs: (" << adc_X0 << ", " << adc_Y0 << ")" << endl;
//     cout << "ADCs (X_minus,X_plus) : (" << PhysicsToADCs[centerX_phys-1] << ", " << PhysicsToADCs[centerX_phys+1] << ")" << " - Target (Y_minus,Y_plus) : (" << PhysicsToADCs[centerY_phys-1] << ", " << PhysicsToADCs[centerY_phys+1] << ")" << endl;


//     // block-averages using TargetCharge for block values (scaled by bareCal)
//     // LG charge event-by-event to compute standard errors and to compare to target's
//     vector<double> blk_Qlg_avg, blk_Qlg_err;
//     vector<double> blk_qXm1_avg, blk_qX0_avg, blk_qXp1_avg;
//     vector<double> blk_qYm1_avg, blk_qY0_avg, blk_qYp1_avg;

//     // temporary per-event buffers
//     vector<double> cur_Qlg;
//     double Qlg = 0.;

//     double mQ = 0.;
//     double sQ = 0.; 
//     double deltaQ = 0.;

//     double cum_qX0 = 0.;
//     double cum_qXm1 = 0.;
//     double cum_qXp1 = 0.;
//     double cum_qY0 = 0.;
//     double cum_qYm1 = 0.;
//     double cum_qYp1 = 0.;

//     // Reset tree to loop again for block-building
//     for(Long64_t ev=0; ev<nentries; ++ev){
//         t->GetEntry(ev);
//         if(TrigMask != 1) continue;

//         // LG
//         Qlg = QChOld[LG_BRD][LG_CH];

//         // append to current block
//         cur_Qlg.push_back(Qlg);
       
//         // when block full, compute block means and push to averaged-vectors 
//         if((int)cur_Qlg.size() == BLOCKSIZE) {
//             // cout << "ev= " << ev << endl;
//             if(QChCumFixed[1][adc_X0] < 0 ) cout << "QChCumFixed[1][adc_X0] = " << QChCumFixed[1][adc_X0] << endl;
//             mean_and_std(cur_Qlg, mQ, sQ);

//             // push to global block vectors (X)
//             blk_Qlg_avg.push_back(mQ);
//             // store standard error of Qlg for weighting
//             deltaQ = (cur_Qlg.size()>0) ? sQ / sqrt((double)cur_Qlg.size()) : 1.0;
//             blk_Qlg_err.push_back(deltaQ>0?deltaQ:1.0);
//             // cout << "mQ= " << mQ << " - deltaQ= " << deltaQ << endl; 

//             cum_qX0 = (QChCumFixed[1][adc_X0]>0) ? QChCumFixed[1][adc_X0] : 0;
//             cum_qXm1 = (QChCumFixed[1][adc_Xm1]>0) ? QChCumFixed[1][adc_Xm1] : 0;
//             cum_qXp1 = (QChCumFixed[1][adc_Xp1]>0) ? QChCumFixed[1][adc_Xp1] : 0;
//             // cout << "cum_qX0= " << cum_qX0 << " - cum_qXm1= " << cum_qXm1 << " - cum_qXp1= " << cum_qXp1 << endl;
            
//             cum_qY0 = (QChCumFixed[1][adc_Y0]>0) ? QChCumFixed[1][adc_Y0] : 0;
//             cum_qYm1 = (QChCumFixed[1][adc_Ym1]>0) ? QChCumFixed[1][adc_Ym1] : 0;
//             cum_qYp1 = (QChCumFixed[1][adc_Yp1]>0) ? QChCumFixed[1][adc_Yp1] : 0;   
//             // cout << "cum_qY0= " << cum_qY0 << " - cum_qYm1= " << cum_qYm1 << " - cum_qYp1= " << cum_qYp1 << endl;

//             blk_qXm1_avg.push_back(cum_qXm1);
//             blk_qX0_avg.push_back(cum_qX0);
//             blk_qXp1_avg.push_back(cum_qXp1);

//             blk_qYm1_avg.push_back(cum_qYm1);
//             blk_qY0_avg.push_back(cum_qY0);
//             blk_qYp1_avg.push_back(cum_qYp1);

//             cur_Qlg.clear();
//         }

//     }

//     // If no blocks formed, can't fit
//     if(blk_Qlg_avg.size() == 0) {
//         cerr << "No blocks formed for run " << runNumber << " -> skipping" << endl;
//         outRow.skipped = true;
//         f->Close();
//         return true;
//     }

//     // Now call fits independently for X and Y
//     double cXm1, cX0, cXp1, errXm1, errX0, errXp1;
//     double cYm1, cY0, cYp1, errYm1, errY0, errYp1;

//     fitDirection(blk_qXm1_avg, blk_qX0_avg, blk_qXp1_avg, blk_Qlg_avg, blk_Qlg_err,
//                  cXm1, cX0, cXp1, errXm1, errX0, errXp1);

//     fitDirection(blk_qYm1_avg, blk_qY0_avg, blk_qYp1_avg, blk_Qlg_avg, blk_Qlg_err,
//                  cYm1, cY0, cYp1, errYm1, errY0, errYp1);

//     // Fill output row
//     outRow.cXm1 = cXm1; outRow.err_Xm1 = errXm1;
//     outRow.cX0  = cX0;  outRow.err_X0  = errX0;
//     outRow.cXp1 = cXp1; outRow.err_Xp1 = errXp1;

//     outRow.cYm1 = cYm1; outRow.err_Ym1 = errYm1;
//     outRow.cY0  = cY0;  outRow.err_Y0  = errY0;
//     outRow.cYp1 = cYp1; outRow.err_Yp1 = errYp1;

//     outRow.skipped = false;
//     outRow.nblocks = (int)blk_Qlg_avg.size();

//     blk_qXm1_avg.clear();
//     blk_qX0_avg.clear();
//     blk_qXp1_avg.clear();
//     blk_qYm1_avg.clear();
//     blk_qY0_avg.clear();
//     blk_qYp1_avg.clear();
//     blk_Qlg_avg.clear();
//     blk_Qlg_err.clear();

//     f->Close();
//     return true;
// }

// int main(int argc, char** argv) {
//     // path to bare calibration file (user specified path)
//     // string bareCalFile = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/calibration/final_target_calibration.txt";
//     // root file pattern (as in user's code)
//     string rootPattern = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco2/Reco_run_00%d.root";

//     // list of runs to process (example set — change as needed)
//     vector<int> runs = {80507,80508,80509,80510,80511,80512,80513,80514,80515,80516,80517};
//     //vector<int> runs = {80509};

//     // load bare calibration
//     // map<int,double> bareCal;
//     // if(!loadBareCalibration(bareCalFile, bareCal)) {
//     //     cerr << "Failed to load bare calibration file: " << bareCalFile << endl;
//     //     return 1;
//     // }

//     vector<ResultRow> results;
//     for(int run : runs) {
//         ResultRow row;
//         bool ok = processRun(run, rootPattern, /*bareCal,*/ row);
//         if(!ok) {
//             cerr << "Processing failed for run " << run << " (file IO / tree problems)" << endl;
//             continue;
//         }
//         results.push_back(row);
//     }

//     // write CSV
//     ofstream out("AutoCenterCalibration_XY_full.csv");
//     out << "run,centerX_phys,centerX_adc,cXm1,err_Xm1,cX0,err_X0,cXp1,err_Xp1,centerY_phys,centerY_adc,cYm1,err_Ym1,cY0,err_Y0,cYp1,err_Yp1,skipped\n";
//     for(const auto &r : results) {
//         out << r.run << "," << r.centerX_phys << "," << r.centerX_adc << ","
//             << r.cXm1 << "," << r.err_Xm1 << "," << r.cX0 << "," << r.err_X0 << "," << r.cXp1 << "," << r.err_Xp1 << ","
//             << r.centerY_phys << "," << r.centerY_adc << ","
//             << r.cYm1 << "," << r.err_Ym1 << "," << r.cY0 << "," << r.err_Y0 << "," << r.cYp1 << "," << r.err_Yp1 << ","
//             << (r.skipped ? 1 : 0)
//             << "\n";
//     }
//     out.close();

//     cout << "Done. Results written to AutoCenterCalibration_XY_full.csv" << endl;
//     return 0;
// }

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

// total charge calibration
// 

#define NTUPLE_N_BOARDS 2
#define NTUPLE_N_CHANNELS 32
#define BLOCKSIZE 100

// ADC <-> Physics mapping (user-provided)
int ADCsToPhysics[NTUPLE_N_CHANNELS]  = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// LG reference
const int LG_BRD = 0;
const int LG_CH  = 31;

// Helper: mean and unbiased sample stddev
void mean_and_std(const vector<double>& v, double &mean, double &sigma) {
    int n = v.size();
    if (n==0) { mean=0; sigma=1; return; }
    double s=0;
    for(double x:v) s+=x;
    mean=s/n;
    double ss=0;
    for(double x:v) ss+=(x-mean)*(x-mean);
    sigma = (n>1) ? sqrt(ss/(n-1)) : 1.0;  //sqrt(n) trying to understand what could be the correct error estimate
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
static vector<int> g_nominal;
static vector<array<double,3>> g_qTar;
static vector<double> g_QLG;
static vector<double> g_QLGerr;
static vector<double> g_cMinus1;
static vector<double> g_cPlus1;

//  FCN 
void fcn(int &npar, double *grad, double &fval, double *par, int iflag){

    double chi2 = 0.0;

    g_cMinus1.clear();
    g_cPlus1.clear();

    for(size_t r=0; r<g_nominal.size(); r++){
        int i = g_nominal[r];
        auto &q = g_qTar[r]; // [qm1, q0, qp1]
        double model = par[i-1]*q[0] + par[i]*q[1] + par[i+1]*q[2];
        double diff = model - g_QLG[r];
        double sigma = g_QLGerr[r]; if(sigma<=0) sigma=1.0;
        chi2 += diff*diff/(sigma*sigma);

        if(i>0 && i<15){
            if(par[i-1] != 0) g_cMinus1.push_back(par[i-1]);
            if(par[i+1] != 0) g_cPlus1.push_back(par[i+1]);
        }
    }
    fval = chi2;
}

//  Do the Fit in one direction
// --- Helper to fit one direction void 
void fit_direction(const vector<int> &nominal, const vector<array<double,3>> &qTar, const vector<double> &Qlg, const vector<double> &Qlg_err, 
                    vector<double> &coeffs, vector<double> &coeffs_err, vector<double> &g_Minus, vector<double> &g_Plus,
                    ofstream &fitlog) {
     
    g_nominal = nominal; 
    g_qTar = qTar; 
    g_QLG = Qlg; 
    g_QLGerr = Qlg_err; 
    
    const int NCH = 16; 
    TMinuit minuit(NCH); 
    minuit.SetFCN(fcn); 
    double vstart = 1.0, 
    step = 0.01; 
    for(int i=1;i<NCH-1;i++){ 
        minuit.DefineParameter(i, Form("c_%d",i), vstart, step, 0, 0); 
    } 
    
    minuit.Migrad(); 
    double chi2, edm, errdef;
    int nvpar, nparx, status;
    minuit.mnstat(chi2, edm, errdef, nvpar, nparx, status);

    fitlog << "\n--- Fit Summary ---" << endl;
    fitlog << "Chi2: " << chi2 << endl; // if want to normalize for the NDF --> NDF = nruns-npar ()
    fitlog << "EDM: " << edm << endl;
    fitlog << "ErrDef: " << errdef << endl;
    fitlog << "Varying parameters: " << nvpar << endl;
    fitlog << "Total parameters: " << nparx << endl;
    fitlog << "Status: " << status << endl;

    // covariance matrix
    TMatrixD covMatrix(NCH, NCH);
    minuit.mnemat(covMatrix.GetMatrixArray(), NCH);
    fitlog << "=== Covariance Matrix ===\n";
    for(int i = 0; i < NCH; i++){
        for(int j = 0; j < NCH; j++){
            fitlog << std::setw(10) << std::setprecision(4) << covMatrix(i,j) << " ";
        }
        fitlog << "\n";
    }
    fitlog << "\n";
    
    coeffs.resize(NCH); 
    coeffs_err.resize(NCH); 
    for(int i=1;i<NCH-1;i++){ 
        minuit.GetParameter(i, coeffs[i], coeffs_err[i]); 
    } 
    fitlog << "\nCalibration Coefficients:\n";
    for(int i=1;i<NCH-1;i++){
        fitlog << "c["<<i<<"] = " << coeffs[i] << " +- " << coeffs_err[i] << endl;
    }
    fitlog << "--------------------------\n";

    g_Minus = g_cMinus1; 
    g_Plus = g_cPlus1; 
}

// Struct to store results
struct ChargeCalib {
    int run;
    int centerX_phys, centerY_phys;
    int centerX_adc, centerY_adc;
    double qXm1, err_qXm1, qX0, err_qX0, qXp1, err_qXp1;
    double qYm1, err_qYm1, qY0, err_qY0, qYp1, err_qYp1;
    double qLG, err_qLG; 
    bool skipped;
};

// Check if channel is on edge
bool isEdgeChannel(int Phys_Channel) {
    return (Phys_Channel == 0 || Phys_Channel == 15 || Phys_Channel == 16 || Phys_Channel == 31);
}

// Calibrate one file
bool calibrateFile(int runNumber, int X_ch, int Y_ch, const string &filePathPattern, ChargeCalib &result){

    //map adc physics
    int adc_X0 = PhysicsToADCs[X_ch];
    int adc_Y0 = PhysicsToADCs[Y_ch];
    int adc_Xm1 = PhysicsToADCs[X_ch-1];
    int adc_Xp1 = PhysicsToADCs[X_ch+1];
    int adc_Ym1 = PhysicsToADCs[Y_ch-1];
    int adc_Yp1 = PhysicsToADCs[Y_ch+1];
    
    result.run = runNumber;
    result.centerX_phys = X_ch;
    result.centerY_phys = Y_ch;
    result.centerX_adc = adc_X0;
    result.centerY_adc = adc_Y0;
    result.skipped = false;

    cout << "RunNumber: " << runNumber << endl;
    // cout << "Target (X,Y) position: (" << X_ch << ", " << Y_ch << ")" << endl;
    // cout << "Target (X_minus,X_plus) : (" << X_ch-1 << ", " << X_ch+1 << ")" << " - Target (Y_minus,Y_plus) : (" << Y_ch-1 << ", " << Y_ch+1 << ")" << endl;
    // cout << "Target (X,Y) ADCs: (" << adc_X0 << ", " << adc_Y0 << ")" << endl;
    // cout << "ADCs (X_minus,X_plus) : (" << PhysicsToADCs[X_ch-1] << ", " << PhysicsToADCs[X_ch+1] << ")" << " - Target (Y_minus,Y_plus) : (" << PhysicsToADCs[Y_ch-1] << ", " << PhysicsToADCs[Y_ch+1] << ")" << endl;


    if(isEdgeChannel(X_ch) || isEdgeChannel(Y_ch)) {
        cerr << "Skipping edge channel: X=" << X_ch << " Y=" << Y_ch << endl;
        result.skipped = true;
        return true;
    }

    char fname[256];
    sprintf(fname, filePathPattern.c_str(), runNumber);
    TFile *f = TFile::Open(fname,"READ");
    if(!f || f->IsZombie()) { cerr << "Cannot open file " << fname << endl; return false; }

    TTree* t = (TTree*)f->Get("NTU");
    if(!t) { cerr << "Tree NTU not found in file " << fname << endl; f->Close(); return false; }

    static Double_t QChOld[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
    static Double_t QChCumFixed[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
    Int_t TrigMask;
    t->SetBranchStatus("*",0);
    t->SetBranchStatus("QChOld",1);
    t->SetBranchStatus("QChCumFixed",1);
    t->SetBranchStatus("TrigMask",1);

    t->SetBranchAddress("QChOld", QChOld);
    t->SetBranchAddress("QChCumFixed", QChCumFixed);
    t->SetBranchAddress("TrigMask", &TrigMask);

    Long64_t nentries = t->GetEntries();
    if(nentries <= 0) {
        cerr << "No entries in tree for run " << runNumber << endl;
        f->Close();
        return false;
    } 

    vector<double> cur_Qlg;
    vector<double> blk_Qlg_avg, blk_Qlg_err;
    vector<double> blk_qXm1_avg, blk_qX0_avg, blk_qXp1_avg;
    vector<double> blk_qYm1_avg, blk_qY0_avg, blk_qYp1_avg;
    
    double mQ = 0.;
    double sQ = 0.; 
    // double deltaQ = 0.;
    double QLG, dQLG;
    double qXm1,qX0,qXp1;
    double dqXm1,dqX0,dqXp1;
    double qYm1,qY0,qYp1;
    double dqYm1,dqY0,dqYp1;

    // double cum_qX0 = 0.;
    // double cum_qXm1 = 0.;
    // double cum_qXp1 = 0.;
    // double cum_qY0 = 0.;
    // double cum_qYm1 = 0.;
    // double cum_qYp1 = 0.;

    for(Long64_t ev=0; ev<nentries; ev++){
        t->GetEntry(ev);
        if(TrigMask != 1) continue;
        // append to current block
        cur_Qlg.push_back(QChOld[LG_BRD][LG_CH]);
       
        // when block full, compute block means and push to averaged-vectors 
        if((int)cur_Qlg.size() == BLOCKSIZE) {
            // cout << "ev= " << ev << endl;
            if(QChCumFixed[1][adc_X0] < 0 ) cout << "QChCumFixed[1][adc_X0] = " << QChCumFixed[1][adc_X0] << endl;
            // if(QChCumFixed[1][adc_Y0] < 0 ) cout << "QChCumFixed[1][adc_Y0] = " << QChCumFixed[1][adc_Y0] << endl;
            
            mean_and_std(cur_Qlg, mQ, sQ);
            // mQ = TMath::Mean(cur_Qlg.size(), &cur_Qlg[0]);
            // sQ = TMath::StdDev(cur_Qlg.size(), &cur_Qlg[0])/TMath::Sqrt(cur_Qlg.size());

            // push to global block vectors (X)
            blk_Qlg_avg.push_back(mQ);
            // store standard error of Qlg for weighting
            // deltaQ = (cur_Qlg.size()>0) ? sQ / sqrt((double)cur_Qlg.size()) : 1.0;
            blk_Qlg_err.push_back(sQ);
            // cout << "mQ= " << mQ << " - deltaQ= " << deltaQ << endl; 

            // auto safeQ = [&](double v){ return v<0 ? 0.0 : v; };
            // cum_qX0 = QChCumFixed[1][adc_X0];
            // if(QChCumFixed[1][adc_X0] < 0) cum_qX0 = 0;
            // cum_qXm1 = QChCumFixed[1][adc_Xm1];
            // if(QChCumFixed[1][adc_Xm1] < 0) cum_qXm1 = 0;
            // cum_qXp1 = QChCumFixed[1][adc_Xp1];
            // if(QChCumFixed[1][adc_Xp1] < 0) cum_qXp1 = 0;
            // cout << "cum_qX0= " << cum_qX0 << " - cum_qXm1= " << cum_qXm1 << " - cum_qXp1= " << cum_qXp1 << endl;
            // cum_qY0 = QChCumFixed[1][adc_Y0];
            // if(QChCumFixed[1][adc_Y0] < 0) cum_qY0 = 0;
            // cum_qYm1 = QChCumFixed[1][adc_Ym1];
            // if(QChCumFixed[1][adc_Ym1] < 0) cum_qYm1 = 0;
            // cum_qYp1 = QChCumFixed[1][adc_Yp1];
            // if(QChCumFixed[1][adc_Yp1] < 0) cum_qYp1 = 0;
            // cout << "cum_qY0= " << cum_qY0 << " - cum_qYm1= " << cum_qYm1 << " - cum_qYp1= " << cum_qYp1 << endl;


            blk_qXm1_avg.push_back((QChCumFixed[1][adc_Xm1]>0 ? QChCumFixed[1][adc_Xm1] : 0));
            blk_qX0_avg.push_back((QChCumFixed[1][adc_X0]>0 ? QChCumFixed[1][adc_X0] : 0));
            blk_qXp1_avg.push_back((QChCumFixed[1][adc_Xp1]>0 ? QChCumFixed[1][adc_Xp1] : 0));

            blk_qYm1_avg.push_back((QChCumFixed[1][adc_Ym1]>0 ? QChCumFixed[1][adc_Ym1] : 0));
            blk_qY0_avg.push_back((QChCumFixed[1][adc_Y0]>0 ? QChCumFixed[1][adc_Y0] : 0));
            blk_qYp1_avg.push_back((QChCumFixed[1][adc_Yp1]>0 ? QChCumFixed[1][adc_Yp1] : 0));

            cur_Qlg.clear();
        }
    }

    // If no blocks formed, can't fit
    if(blk_Qlg_avg.size() == 0) {
        cerr << "No blocks formed for run " << runNumber << " -> skipping" << endl;
        result.skipped = true;
        f->Close();
        return true;
    }

    //DO WHAT MARIO SAYS
    if(blk_Qlg_avg.size()==blk_qX0_avg.size()){
        mean_and_std(blk_Qlg_avg, QLG, dQLG);
        // QLG = TMath::Mean(blk_Qlg_avg.size(), &blk_Qlg_avg[0]);
        // dQLG = TMath::StdDev(blk_Qlg_avg.size(), &blk_Qlg_avg[0])/TMath::Sqrt(blk_Qlg_avg.size());
        mean_and_std(blk_qX0_avg, qX0, dqX0);
        // qX0 = TMath::Mean(blk_qX0_avg.size(), &blk_qX0_avg[0]);
        // dqX0 = TMath::StdDev(blk_qX0_avg.size(), &blk_qX0_avg[0])/TMath::Sqrt(blk_qX0_avg.size());
        mean_and_std(blk_qXm1_avg, qXm1, dqXm1);
        // qXm1 = TMath::Mean(blk_qXm1_avg.size(), &blk_qXm1_avg[0]);
        // dqXm1 = TMath::StdDev(blk_qXm1_avg.size(), &blk_qXm1_avg[0])/TMath::Sqrt(blk_qXm1_avg.size());
        mean_and_std(blk_qXp1_avg, qXp1, dqXp1);
        // qXp1 = TMath::Mean(blk_qXp1_avg.size(), &blk_qXp1_avg[0]);
        // dqXp1 = TMath::StdDev(blk_qXp1_avg.size(), &blk_qXp1_avg[0])/TMath::Sqrt(blk_qXp1_avg.size());
        mean_and_std(blk_qY0_avg, qY0, dqY0);
        // qY0 = TMath::Mean(blk_qY0_avg.size(), &blk_qY0_avg[0]);
        // dqY0 = TMath::StdDev(blk_qY0_avg.size(), &blk_qY0_avg[0])/TMath::Sqrt(blk_qY0_avg.size());
        mean_and_std(blk_qYm1_avg, qYm1, dqYm1);
        // qYm1 = TMath::Mean(blk_qYm1_avg.size(), &blk_qYm1_avg[0]);
        // dqYm1 = TMath::StdDev(blk_qYm1_avg.size(), &blk_qYm1_avg[0])/TMath::Sqrt(blk_qYm1_avg.size());
        mean_and_std(blk_qYp1_avg, qYp1, dqYp1);
        // qYp1 = TMath::Mean(blk_qYp1_avg.size(), &blk_qYp1_avg[0]);
        // dqYp1 = TMath::StdDev(blk_qYp1_avg.size(), &blk_qYp1_avg[0])/TMath::Sqrt(blk_qYp1_avg.size());
        
    } else {
        cout << "wrong treatment over the BLOCKSIZE sum " << endl;
        return true;
    }

    result.qLG = QLG; result.err_qLG = dQLG;
    result.qXm1 = qXm1; result.err_qXm1 = dqXm1;
    result.qX0 = qX0; result.err_qX0 = dqX0;
    result.qXp1 = qXp1; result.err_qXp1 = dqXp1;
    result.qYm1 = qYm1; result.err_qYm1 = dqYm1;
    result.qY0 = qY0; result.err_qY0 = dqY0;
    result.qYp1 = qYp1; result.err_qYp1 = dqYp1;

    f->Close();
    return true;
}

// --- Main
int main(int argc, char** argv) {
    //root file pattern (as in user's code)
    string rootPattern = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco3/Reco_run_00%d.root";

    // defining the output txt file
    ofstream fitout("outputCalibration/ChCalib_XY.txt");
    fitout << "==== Calibration LogFile ChCalib ====" << endl;
    // defining the output root file
    TString outputFileName = "outputCalibration/ChCalib_XY.root";
    TFile* histoFile = new TFile(outputFileName,"RECREATE");
    if(!histoFile) {
        fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
        exit(1);
    }

    // run - X_Ch - Y_ch association (physics strips)
    vector<tuple<int,int,int>> runs_channels = {
        {80166,7,7},{80167,6,7},{80168,5,7},{80169,4,7},{80170,3,7},{80171,2,7},{80173,1,7},{80174,0,7},
        {80175,8,7},{80176,9,7},{80177,10,7},{80178,11,7},{80179,12,7},{80180,13,7},{80181,14,7},
        {80507,7,0},{80508,7,13},{80509,7,12},{80510,7,11},{80511,7,10},{80512,7,8},
        {80513,7,7},{80514,7,6},{80515,7,4},{80516,7,2},{80517,7,3}
    };

    //here do the minimisation process
    const int NRUNS = runs_channels.size();
    const int NCH_1Dir = NTUPLE_N_CHANNELS/2;
    
    vector<int> X_nom, Y_nom;
    vector<array<double,3>> X_qTar, Y_qTar;
    vector<double> X_QLG, Y_QLG;
    vector<double> X_err, Y_err;

    vector<TGraphErrors*> gQx;  
    vector<TGraphErrors*> gQy;  
    // TGraphErrors* gQ0X = new TGraphErrors(NCH_1Dir);    
    // TGraphErrors* gQm1X = new TGraphErrors(NCH_1Dir);    
    // TGraphErrors* gQp1X = new TGraphErrors(NCH_1Dir);    

    for(int irun = 0; irun < NRUNS; irun++){
        auto &rc = runs_channels[irun];
        int run = std::get<0>(rc);
        int ix  = std::get<1>(rc);
        int iy  = std::get<2>(rc);

        ChargeCalib res;
        bool ok = calibrateFile(run, ix, iy, rootPattern, res);
        if(!ok){
            cerr << "Processing failed for run " << run << endl;
            continue;
        }
        // Fill X strips
        if(ix > 0 && ix < NCH_1Dir-1){
            X_nom.push_back(ix);
            X_qTar.push_back({res.qXm1, res.qX0, res.qXp1});
            X_QLG.push_back(res.qLG);
            X_err.push_back(res.err_qLG);
        }
        // Fill Y strips
        if(iy > 0 && iy < NCH_1Dir-1){
            Y_nom.push_back(iy);
            Y_qTar.push_back({res.qYm1, res.qY0, res.qYp1});
            Y_QLG.push_back(res.qLG);
            Y_err.push_back(res.err_qLG);
        }       
        // Filling graphs for debug
        gQx.push_back(new TGraphErrors(3));
        gQx.at(irun)->SetPoint(0, ix-1, res.qXm1); gQx.at(irun)->SetPointError(0, 0, res.err_qXm1);
        gQx.at(irun)->SetPoint(1, ix, res.qX0); gQx.at(irun)->SetPointError(1, 0, res.err_qX0);
        gQx.at(irun)->SetPoint(2, ix+1, res.qXp1); gQx.at(irun)->SetPointError(2, 0, res.err_qXp1);
        
        gQy.push_back(new TGraphErrors(3));
        gQy.at(irun)->SetPoint(0, iy-1, res.qYm1); gQy.at(irun)->SetPointError(0, 0, res.err_qYm1);
        gQy.at(irun)->SetPoint(1, iy, res.qY0); gQy.at(irun)->SetPointError(1, 0, res.err_qY0);
        gQy.at(irun)->SetPoint(2, iy+1, res.qYp1); gQy.at(irun)->SetPointError(2, 0, res.err_qYp1);

        TGraphAttribute(gQx.at(irun), Form("gQx_run_%d",run), "X Strip", "Q [pC]", 20, kBlue+irun);
        TGraphAttribute(gQy.at(irun), Form("gQy_run_%d",run), "Y Strip", "Q [pC]", 20, kRed+irun);
    }



    // run fits
    vector<double> coeffX, coeffX_err;
    vector<double> Xcm1, Xcp1;
    vector<double> coeffY, coeffY_err;
    vector<double> Ycm1, Ycp1;

    fitout << "==== X - STRIPS ====" << endl;
    fit_direction(X_nom, X_qTar, X_QLG, X_err, coeffX, coeffX_err, Xcm1, Xcp1, fitout);
    fitout << "==== Y - STRIPS ====" << endl;
    fit_direction(Y_nom, Y_qTar, Y_QLG, Y_err, coeffY, coeffY_err, Ycm1, Ycp1, fitout);
    // for(int i=0;i<(int)coeffX.size();i++){
    //     cout << "index= " << i << " - Xcm1= " << Xcm1.at(i) << " - coeffX= " << coeffX.at(i) << " - Xcp1= " << Xcp1.at(i) << endl;
    // }
        
    // vector<double> xc_m1, xc_0, xc_p1;
    TH2D * hXC0Cm1 = new TH2D("hXC0Cm1","hXC0Cm1", 200, -10, 10, 200, -10, 10);
    TH2D * hXC0Cp1 = new TH2D("hXC0Cp1","hXC0Cp1", 200, -10, 10, 200, -10, 10);
    TGraphErrors* gXC = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gXCm1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gXCp1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gXCm1_over_XCp1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gXCm1_over_XC0 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gXCp1_over_XC0 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gPullXCm1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gPullXCp1 = new TGraphErrors(NCH_1Dir);

    TH2D * hYC0Cm1 = new TH2D("hYC0Cm1","hYC0Cm1", 200, -10, 10, 200, -10, 10);
    TH2D * hYC0Cp1 = new TH2D("hYC0Cp1","hYC0Cp1", 200, -10, 10, 200, -10, 10);
    TGraphErrors* gYC = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gYCm1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gYCp1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gYCm1_over_YCp1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gYCm1_over_YC0 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gYCp1_over_YC0 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gPullYCm1 = new TGraphErrors(NCH_1Dir);
    TGraphErrors* gPullYCp1 = new TGraphErrors(NCH_1Dir);
    
    for(int i=1;i<15;i++){
        gXC->SetPoint(i,i,coeffX.at(i));
        gXC->SetPointError(i,0,coeffX_err.at(i));           
        gXCm1->SetPoint(i,i,Xcm1.at(i));
        gXCm1->SetPointError(i,0,0);           
        gXCp1->SetPoint(i,i,Xcp1.at(i));
        gXCp1->SetPointError(i,0,0);
        gXCm1_over_XCp1->SetPoint(i,i,Xcm1.at(i)/Xcp1.at(i));
        gXCm1_over_XCp1->SetPointError(i,0,0);
        gXCm1_over_XC0->SetPoint(i,i,Xcm1.at(i)/coeffX.at(i));
        gXCm1_over_XC0->SetPointError(i,0,0);
        gXCp1_over_XC0->SetPoint(i,i,Xcp1.at(i)/coeffX.at(i));
        gXCp1_over_XC0->SetPointError(i,0,0);
        gPullXCm1->SetPoint(i,i,coeffX.at(i) - Xcm1.at(i));
        gPullXCm1->SetPointError(i,0,coeffX_err.at(i));
        gPullXCp1->SetPoint(i,i,coeffX.at(i) - Xcp1.at(i));
        gPullXCp1->SetPointError(i,0,coeffX_err.at(i));            

        hYC0Cm1->Fill(coeffY[i],Ycm1.at(i));
        hYC0Cp1->Fill(coeffY[i],Ycp1.at(i));

        gYC->SetPoint(i,i,coeffY.at(i));
        gYC->SetPointError(i,0,coeffY_err.at(i));           
        gYCm1->SetPoint(i,i,Ycm1.at(i));
        gYCm1->SetPointError(i,0,0);           
        gYCp1->SetPoint(i,i,Ycp1.at(i));
        gYCp1->SetPointError(i,0,0);
        gYCm1_over_YCp1->SetPoint(i,i,Ycm1.at(i)/Ycp1.at(i));
        gYCm1_over_YCp1->SetPointError(i,0,0);
        gYCm1_over_YC0->SetPoint(i,i,Ycm1.at(i)/coeffY.at(i));
        gYCm1_over_YC0->SetPointError(i,0,0);
        gYCp1_over_YC0->SetPoint(i,i,Ycp1.at(i)/coeffY.at(i));
        gYCp1_over_YC0->SetPointError(i,0,0);
        gPullYCm1->SetPoint(i,i,coeffY.at(i) - Ycm1.at(i));
        gPullYCm1->SetPointError(i,0,coeffY_err.at(i));
        gPullYCp1->SetPoint(i,i,coeffY.at(i) - Ycp1.at(i));
        gPullYCp1->SetPointError(i,0,coeffY_err.at(i));            

        hYC0Cm1->Fill(coeffY[i],Ycm1.at(i));
        hYC0Cp1->Fill(coeffY[i],Ycp1.at(i));

    }
    TGraphAttribute(gXC,"gXC","X", "coeff", 20, kBlue);
    TGraphAttribute(gXCm1,"gXCm1","X", "c_{i-1}", 20, kBlue);
    TGraphAttribute(gXCp1,"gXCp1","X", "c_{i+1}", 20, kBlue);
    TGraphAttribute(gXC,"gXC","X", "coeff", 20, kBlue);
    TGraphAttribute(gXCm1_over_XCp1,"gXCm1_over_XCp1","X", "c_{i-1}/c_{i+1}", 20, kBlue);
    TGraphAttribute(gXCm1_over_XC0,"gXCm1_over_XC0","X", "c_{i-1}/c_{i}", 20, kBlue);
    TGraphAttribute(gXCp1_over_XC0,"gXCp1_over_XC0","X", "c_{i+1}/c_{i}", 20, kBlue);
    TGraphAttribute(gPullXCm1,"gPullXCm1","X", "c_{i-1} - c_{i}", 20, kBlue);
    TGraphAttribute(gPullXCp1,"gPullXCp1","X", "c_{i+1} - c_{i}", 20, kBlue);

    TGraphAttribute(gYC,"gYC","Y", "coeff", 20, kBlue);
    TGraphAttribute(gYCm1,"gYCm1","Y", "c_{i-1}", 20, kBlue);
    TGraphAttribute(gYCp1,"gYCp1","Y", "c_{i+1}", 20, kBlue);
    TGraphAttribute(gYC,"gYC","Y", "coeff", 20, kBlue);
    TGraphAttribute(gYCm1_over_YCp1,"gYCm1_over_YCp1","Y", "c_{i-1}/c_{i+1}", 20, kBlue);
    TGraphAttribute(gYCm1_over_YC0,"gYCm1_over_YC0","Y", "c_{i-1}/c_{i}", 20, kBlue);
    TGraphAttribute(gYCp1_over_YC0,"gYCp1_over_YC0","Y", "c_{i+1}/c_{i}", 20, kBlue);
    TGraphAttribute(gPullYCm1,"gPullYCm1","Y", "c_{i-1} - c_{i}", 20, kBlue);
    TGraphAttribute(gPullYCp1,"gPullYCp1","Y", "c_{i+1} - c_{i}", 20, kBlue);

    //writing out the histrograms in a root file
    histoFile->cd();
    for(int irun = 0; irun < NRUNS; irun++){
        gQx.at(irun)->Write();
        gQy.at(irun)->Write();
    }
    gXC->Write();
    gXCm1->Write();
    gXCp1->Write();
    gXCm1_over_XCp1->Write();
    gXCm1_over_XC0->Write();
    gXCp1_over_XC0->Write();
    gPullXCm1->Write();
    gPullXCp1->Write();
    hXC0Cm1->Write();
    hXC0Cp1->Write();

    gYC->Write();
    gYCm1->Write();
    gYCp1->Write();
    gYCm1_over_YCp1->Write();
    gYCm1_over_YC0->Write();
    gYCp1_over_YC0->Write();
    gPullYCm1->Write();
    gPullYCp1->Write();
    hYC0Cm1->Write();
    hYC0Cp1->Write();

    printf("Output file%s closed \n",outputFileName.Data());
    histoFile->Close();
    
    cout << "Calibration done." << endl;
    return 0;
}

