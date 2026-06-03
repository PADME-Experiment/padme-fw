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

#define NTUPLE_N_BOARDS 2
#define NTUPLE_N_CHANNELS 32
#define BLOCKSIZE 100

// ADC <-> Physics mapping (user-provided)
int ADCsToPhysics[NTUPLE_N_CHANNELS]  = {15,13,11,9,7,5,3,1,0,2,4,6,8,10,12,14,22,20,18,16,17,19,21,23,25,27,29,31,30,28,26,24};
int PhysicsToADCs[NTUPLE_N_CHANNELS] = {8,7,9,6,10,5,11,4,12,3,13,2,14,1,15,0,19,20,18,21,17,22,16,23,31,24,30,25,29,26,28,27};

// LG reference
const int LG_BRD = 0;
const int LG_CH  = 31;
const int NCH = 16; // number of channels in one direction
const double delta_strip = 0.850; // size of strip in mm

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
static vector<int> g_nominal_strip;
static vector<array<double,3>> g_qTar;
static vector<double> g_QLG;
static vector<double> g_QLGerr;
static vector<double> g_cMinus1;
static vector<double> g_c0;
static vector<double> g_cPlus1;

//  FCN 
void fcn(int &npar, double *grad, double &fval, double *par, int iflag){

    double chi2 = 0.0;

    g_cMinus1.clear();
    g_c0.clear();
    g_cPlus1.clear();

    for(size_t r=0; r<g_nominal_strip.size(); r++){
        int i = g_nominal_strip[r]; // nominal channel fired [0,15]
        auto &q = g_qTar[r]; // [qm1, q0, qp1]
        double z_left[3] = {0.};
        double z_right[3] = {0.};
        for(int iq = 0; iq < 3; iq++){
            z_left[iq] = ((-1 + iq - delta_strip/2) - par[NCH])/par[NCH+1];
            z_right[iq] = ((-1 + iq + delta_strip/2) - par[NCH])/par[NCH+1];
            double diff =  par[i-1+iq]*q[iq] - g_QLG[r]*(TMath::Erf(z_right[iq]/sqrt(2)) - TMath::Erf(z_left[iq]/sqrt(2)))/2;
            double sigma = g_QLGerr[r]*(TMath::Erf(z_right[iq]/sqrt(2)) - TMath::Erf(z_left[iq]/sqrt(2)))/2;; if(sigma<=0) sigma=1.0;
            // cout << i << "," << iq << "," << diff/sigma << endl;
            chi2 += diff*diff/(sigma*sigma);
        }
        if(i>0 && i<15){
            if(par[i-1] != 0) g_cMinus1.push_back(par[i-1]);
            if(par[i] != 0) g_c0.push_back(par[i]);
            if(par[i+1] != 0) g_cPlus1.push_back(par[i+1]);
        }
    }

    fval = chi2;
}

//  Do the Fit in one direction
void fit_direction(const vector<int> &nominal, const vector<array<double,3>> &qTar, const vector<double> &Qlg, const vector<double> &Qlg_err, 
                    vector<double> &coeffs, vector<double> &coeffs_err, vector<double> &g_Minus, vector<double> &g_0, vector<double> &g_Plus,
                    ofstream &fitlog) {
     
    g_nominal_strip = nominal; 
    g_qTar = qTar; 
    g_QLG = Qlg; 
    g_QLGerr = Qlg_err; 

    TMinuit minuit(NCH+2); // NCH coeffs + mu + sigma
    minuit.SetFCN(fcn); 
    double vstart = 1.0, 
    step = 0.01; 
    for(int i=0;i<NCH+2;i++){ 
        if(i == NCH) minuit.DefineParameter(i, "mu", 0., 0.01, -10.0, 10.0); 
        if(i == NCH+1) minuit.DefineParameter(i, "sigma", 0.5, 0.01, 0.1, 10.0); 
        minuit.DefineParameter(i, Form("c_%d",i), vstart, step, -100, 100); 
    } 
    
    minuit.Migrad(); 
    double chi2, edm, errdef;
    int nvpar, nparx, status;
    minuit.mnstat(chi2, edm, errdef, nvpar, nparx, status);

    fitlog << "\n--- Fit Summary ---" << endl;
    fitlog << "Chi2: " << chi2 << endl; // if want to normalize for the NDF --> NDF = nruns-npar
    fitlog << "EDM: " << edm << endl;
    fitlog << "ErrDef: " << errdef << endl;
    fitlog << "Varying parameters: " << nvpar << endl;
    fitlog << "Total parameters: " << nparx << endl;
    fitlog << "Status: " << status << endl;

    // covariance matrix
    TMatrixD covMatrix(NCH+2, NCH+2); 
    minuit.mnemat(covMatrix.GetMatrixArray(), NCH+2);
    fitlog << "=== Covariance Matrix ===\n";
    for(int i = 0; i < NCH+2; i++){
        for(int j = 0; j < NCH+2; j++){
            fitlog << std::setw(10) << std::setprecision(4) << covMatrix(i,j) << " ";
        }
        fitlog << "\n";
    }
    fitlog << "\n";

    coeffs.resize(NCH +2); // +2 -->including mu and sigma of gaussian integral accounting for death areas 
    coeffs_err.resize(NCH +2); // +2 -->including mu and sigma of gaussian integral accounting for death areas 
    for(int i=0;i<NCH+2;i++){ 
        minuit.GetParameter(i, coeffs[i], coeffs_err[i]); 
    } 
    fitlog << "\nCalibration Coefficients:\n";
    for(int i=0;i<NCH+2;i++){
        fitlog << "c["<<i<<"] = " << coeffs[i] << " +- " << coeffs_err[i] << endl;
    }
    fitlog << "--------------------------\n";

    g_Minus = g_cMinus1; 
    g_0 = g_c0; 
    g_Plus = g_cPlus1; 
}

// Struct to store results
struct ChargeCalib {
    int run;
    vector<int> eventNumber;
    int centerX_phys, centerY_phys;
    int centerX_adc, centerY_adc;
    double qXm1, err_qXm1, qX0, err_qX0, qXp1, err_qXp1;
    double qYm1, err_qYm1, qY0, err_qY0, qYp1, err_qYp1;
    double qLG, err_qLG; 
    vector<double> qLg_mean, qLg_sigma;
    vector<double> qTar_m1, qTar_0, qTar_p1;
    bool skipped;
};

// Check if channel is on edge
bool isEdgeChannel(int Phys_Channel) {
    return (Phys_Channel == 0 || Phys_Channel == 15 || Phys_Channel == 16 || Phys_Channel == 31);
}

// Calibrate one file
bool calibrateFile(int runNumber, int X_ch, int Y_ch, const string &filePathPattern, ChargeCalib &result){

    // map adc physics
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

    vector<int> vNeve;
    vector<double> cur_Qlg;
    vector<tuple<double,double,double>> cur_qXTar; // (qm1, q0, qp1) 
    vector<tuple<double,double,double>> cur_qYTar; // (qm1, q0, qp1) 
    vector<double> blk_Qlg_avg, blk_Qlg_err;
    vector<double> blk_qXm1_avg, blk_qX0_avg, blk_qXp1_avg;
    vector<double> blk_qYm1_avg, blk_qY0_avg, blk_qYp1_avg;
    
    double mQ = 0.;
    double sQ = 0.; 
    double QLG, dQLG;
    double qXm1,qX0,qXp1;
    double dqXm1,dqX0,dqXp1;
    double qYm1,qY0,qYp1;
    double dqYm1,dqY0,dqYp1;

    for(Long64_t ev=0; ev<nentries; ev++){
        t->GetEntry(ev);
        vNeve.push_back(Nevent);
        // cout << "iteration = " << ev << " - Nevent = " << Nevent << endl;
        if(TrigMask != 1) continue;
        // append to current block
        cur_Qlg.push_back(QChOld[LG_BRD][LG_CH]);
       
        // when block full, compute block means and push to averaged-vectors 
        if((int)cur_Qlg.size() == BLOCKSIZE) {
            // cout << "ev= " << ev << endl;
            if(QChCumFixed[1][adc_X0] < 0 ) cout << "QChCumFixed[1][adc_X0] = " << QChCumFixed[1][adc_X0] << endl;
            // if(QChCumFixed[1][adc_Y0] < 0 ) cout << "QChCumFixed[1][adc_Y0] = " << QChCumFixed[1][adc_Y0] << endl;
            
            mean_and_std(cur_Qlg, mQ, sQ);
            blk_Qlg_avg.push_back(mQ);
            blk_Qlg_err.push_back(sQ);
            // cout << "mQ= " << mQ << " - deltaQ= " << deltaQ << endl; 

            blk_qXm1_avg.push_back((QChCumFixed[1][adc_Xm1]>0 ? QChCumFixed[1][adc_Xm1] : 0));
            blk_qX0_avg.push_back((QChCumFixed[1][adc_X0]>0 ? QChCumFixed[1][adc_X0] : 0));
            blk_qXp1_avg.push_back((QChCumFixed[1][adc_Xp1]>0 ? QChCumFixed[1][adc_Xp1] : 0));

            blk_qYm1_avg.push_back((QChCumFixed[1][adc_Ym1]>0 ? QChCumFixed[1][adc_Ym1] : 0));
            blk_qY0_avg.push_back((QChCumFixed[1][adc_Y0]>0 ? QChCumFixed[1][adc_Y0] : 0));
            blk_qYp1_avg.push_back((QChCumFixed[1][adc_Yp1]>0 ? QChCumFixed[1][adc_Yp1] : 0));

            cur_Qlg.clear();
            result.qLg_mean.push_back(mQ);
            result.qLg_sigma.push_back(sQ); 
            result.qTar_m1.push_back(QChCumFixed[1][adc_Xm1]);
            result.qTar_0.push_back(QChCumFixed[1][adc_X0]);
            result.qTar_p1.push_back(QChCumFixed[1][adc_Xp1]);
        }
        
    }

    // If no blocks formed, can't fit
    if(blk_Qlg_avg.size() == 0) {
        cerr << "No blocks formed for run " << runNumber << " -> skipping" << endl;
        result.skipped = true;
        f->Close();
        return true;
    }

    if(blk_Qlg_avg.size()==blk_qX0_avg.size()){
        mean_and_std(blk_Qlg_avg, QLG, dQLG);
        mean_and_std(blk_qX0_avg, qX0, dqX0);
        mean_and_std(blk_qXm1_avg, qXm1, dqXm1);
        mean_and_std(blk_qXp1_avg, qXp1, dqXp1);
        mean_and_std(blk_qY0_avg, qY0, dqY0);
        mean_and_std(blk_qYm1_avg, qYm1, dqYm1);
        mean_and_std(blk_qYp1_avg, qYp1, dqYp1);
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
    result.eventNumber = vNeve;
    f->Close();
    return true;
}

// --- Main
int main(int argc, char** argv) {
    //root file pattern (as in user's code)
    string rootPattern = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/outputReco_NewCharge/Reco_run_00%d.root";

    // defining the output txt file
    ofstream fitout("outputCalibration/TargetCalibration_NewCharge2.txt");
    fitout << "==== Calibration LogFile ChCalib ====" << endl;
    // defining the output root file
    TString outputFileName = "outputCalibration/TargetCalibration_NewCharge2.root";
    TFile* histoFile = new TFile(outputFileName,"RECREATE");
    if(!histoFile) {
        fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
        exit(1);
    }

    // run - X_Ch - Y_ch association (physics strips)
    vector<tuple<int,int,int>> runs_channels = {
        {80166,7,7},{80167,6,7},{80169,4,7},{80170,3,7},{80171,2,7},{80173,1,7},{80174,0,7}, // {80168,5,7}, 
        {80175,8,7},{80176,9,7},{80177,10,7},{80178,11,7},{80179,12,7},{80180,13,7},{80181,14,7}
        // {80507,7,0},{80508,7,13},{80509,7,12},{80510,7,11},{80511,7,10},{80512,7,8},
        // {80513,7,7},{80514,7,6},{80515,7,4},{80516,7,2},{80517,7,3}
    };

    //here do the minimisation process
    const int NRUNS = runs_channels.size();
    
    vector<int> X_nom; // Y_nom;
    vector<vector<int>> Nevt; // Nevt;
    vector<array<double,3>> X_qTar; //, Y_qTar;
    vector<array<double,3>> err_X_qTar; // , err_Y_qTar;
    vector<double> X_QLG; // , Y_QLG;
    vector<double> X_QLGerr; // , Y_QLGerr;
    vector<vector<double>> qLGmean, qLGsigma;
    vector<vector<double>> qTargetm1, qTarget0, qTargetp1;
    vector<TGraphErrors*> gQx;  
    // vector<TGraphErrors*> gQy;
    
    int Xruns = 14;
    cout << "NRUNS = " << NRUNS << " and Xruns = " << Xruns << endl;

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
        // if(irun < NRUNS){ // Fill X strips
            if(!isEdgeChannel(ix) && ix != 5 && ix != 15){
                X_nom.push_back(ix);
                X_qTar.push_back({res.qXm1, res.qX0, res.qXp1});
                err_X_qTar.push_back({res.err_qXm1, res.err_qX0, res.err_qXp1});
                X_QLG.push_back(res.qLG);
                X_QLGerr.push_back(res.err_qLG);
                qLGmean.push_back(res.qLg_mean);
                qLGsigma.push_back(res.qLg_sigma);
                qTargetm1.push_back(res.qTar_m1);
                qTarget0.push_back(res.qTar_0);
                qTargetp1.push_back(res.qTar_p1);
                Nevt.push_back(res.eventNumber);
            } else {
                cout << "Skipping edge channel or excluded channel in X: " << ix << endl;
            }
        // } else { // Fill Y strips
        //     continue;
            // if(iy > 0 && iy < NCH-1){
            //     Y_nom.push_back(iy);
            //     Y_qTar.push_back({res.qYm1, res.qY0, res.qYp1});
            //     err_Y_qTar.push_back({res.err_qYm1, res.err_qY0, res.err_qYp1});
            //     Y_QLG.push_back(res.qLG);
            //     Y_QLGerr.push_back(res.err_qLG);
            // }     
        // }  
        // Filling graphs for debug
        gQx.push_back(new TGraphErrors(3));
        gQx.at(irun)->SetPoint(0, ix+2-1, res.qXm1); gQx.at(irun)->SetPointError(0, 0, res.err_qXm1);
        gQx.at(irun)->SetPoint(1, ix+2, res.qX0); gQx.at(irun)->SetPointError(1, 0, res.err_qX0);
        gQx.at(irun)->SetPoint(2, ix+2+1, res.qXp1); gQx.at(irun)->SetPointError(2, 0, res.err_qXp1);
        
        // gQy.push_back(new TGraphErrors(3));
        // gQy.at(irun)->SetPoint(0, iy-1, res.qYm1); gQy.at(irun)->SetPointError(0, 0, res.err_qYm1);
        // gQy.at(irun)->SetPoint(1, iy, res.qY0); gQy.at(irun)->SetPointError(1, 0, res.err_qY0);
        // gQy.at(irun)->SetPoint(2, iy+1, res.qYp1); gQy.at(irun)->SetPointError(2, 0, res.err_qYp1);

        TGraphAttribute(gQx.at(irun), Form("gQx_run_%d",run), "X-Strip", "Q [pC]", 20, kBlue);
        // TGraphAttribute(gQy.at(irun), Form("gQy_run_%d",run), "Y Strip", "Q [pC]", 20, kRed+irun);
    }

    cout << "ci arrivi?" << endl;
    vector<TGraphErrors*> gqLG_mean, gqLG_sigma, gqLG_sigma_over_mean;
    vector<TGraphErrors*> gqtarget_m1, gqtarget_0, gqtarget_p1;
    for(int irun=0; irun<NRUNS-2; irun++){
        gqLG_mean.push_back(new TGraphErrors());
        gqLG_sigma.push_back(new TGraphErrors());
        gqLG_sigma_over_mean.push_back(new TGraphErrors());
        gqtarget_m1.push_back(new TGraphErrors());
        gqtarget_0.push_back(new TGraphErrors());
        gqtarget_p1.push_back(new TGraphErrors());
        // cout << "irun = " << irun << " strip = " << (double)std::get<1>(runs_channels[irun]) << " - qLGmean[irun].size()" << qLGmean[irun].size() << endl;
        // cout << "qLGsigma[irun].size()" << qLGsigma[irun].size() << endl;
        // cout << "qTargetm1[irun].size()" << qTargetm1[irun].size() << endl;
        // cout << "qTarget0[irun].size()" << qTarget0[irun].size() << endl;
        // cout << "qTargetp1[irun].size()" << qTargetp1[irun].size() << endl;
        for(size_t iblk=0; iblk<qLGmean[irun].size(); iblk++){
            gqLG_mean.at(irun)->SetPoint(iblk, Nevt[irun].at(iblk), qLGmean[irun].at(iblk));  
            gqLG_sigma.at(irun)->SetPoint(iblk, Nevt[irun].at(iblk), qLGsigma[irun].at(iblk));  
            gqLG_sigma_over_mean.at(irun)->SetPoint(iblk, Nevt[irun].at(iblk), qLGsigma[irun].at(iblk)/qLGmean[irun].at(iblk)); 
            gqtarget_m1.at(irun)->SetPoint(iblk, Nevt[irun].at(iblk), qTargetm1[irun].at(iblk));  
            gqtarget_0.at(irun)->SetPoint(iblk, Nevt[irun].at(iblk), qTarget0[irun].at(iblk));  
            gqtarget_p1.at(irun)->SetPoint(iblk, Nevt[irun].at(iblk), qTargetp1[irun].at(iblk)); 
        }
        TGraphAttribute(gqLG_mean.at(irun), Form("gqLGmean_run_%d_strip_%d", std::get<0>(runs_channels[irun]), std::get<1>(runs_channels[irun])), "index", "Mean(Q_{LG})", 20, kBlue);
        TGraphAttribute(gqLG_sigma.at(irun), Form("gqLGsigma_run_%d_strip_%d", std::get<0>(runs_channels[irun]), std::get<1>(runs_channels[irun])), "index", "#sigma(Q_{LG})", 20, kBlue);
        TGraphAttribute(gqLG_sigma_over_mean.at(irun), Form("gqLGsigma_over_mean_run_%d_strip_%d", std::get<0>(runs_channels[irun]), std::get<1>(runs_channels[irun])), "index", "#sigma(Q_{LG})/Q_{LG})", 20, kBlue);
        TGraphAttribute(gqtarget_m1.at(irun), Form("gqtarget_m1_run_%d_strip_%d", std::get<0>(runs_channels[irun]), std::get<1>(runs_channels[irun])), "index", "Q Target m1", 20, kBlue);
        TGraphAttribute(gqtarget_0.at(irun), Form("gqtarget_0_run_%d_strip_%d", std::get<0>(runs_channels[irun]), std::get<1>(runs_channels[irun])), "index", "Q Target 0", 20, kBlue);
        TGraphAttribute(gqtarget_p1.at(irun), Form("gqtarget_p1_run_%d_strip_%d", std::get<0>(runs_channels[irun]), std::get<1>(runs_channels[irun])), "index", "Q Target p1", 20, kBlue);
    }

    // run fits
    vector<double> coeffX, coeffX_err;
    vector<double> Xcm1, Xc0, Xcp1;
    // vector<double> coeffY, coeffY_err;
    // vector<double> Ycm1, Yc0, Ycp1;

    fitout << "==== X - STRIPS ====" << endl;
    fit_direction(X_nom, X_qTar, X_QLG, X_QLGerr, coeffX, coeffX_err, Xcm1, Xc0, Xcp1, fitout);
    // fitout << "==== Y - STRIPS ====" << endl;
    // fit_direction(Y_nom, Y_qTar, Y_QLG, Y_QLGerr, coeffY, coeffY_err, Ycm1, Yc0, Ycp1, fitout);
    cout << "==== Fit completed ====" << endl;
        
    // vector<double> xc_m1, xc_0, xc_p1;
    TH2D * hXC0Cm1 = new TH2D("hXC0Cm1","hXC0Cm1", 200, -10, 10, 200, -10, 10);
    TH2D * hXC0Cp1 = new TH2D("hXC0Cp1","hXC0Cp1", 200, -10, 10, 200, -10, 10);
    TGraphErrors* gXC = new TGraphErrors(coeffX.size());
    TGraphErrors* gXCm1 = new TGraphErrors(Xcm1.size());
    TGraphErrors* gXCp1 = new TGraphErrors(Xcp1.size());
    TGraphErrors* gXCm1_over_XCp1 = new TGraphErrors(Xcm1.size());
    TGraphErrors* gXCm1_over_XC0 = new TGraphErrors(Xcm1.size());
    TGraphErrors* gXCp1_over_XC0 = new TGraphErrors(Xcp1.size());
    TGraphErrors* gPullXCm1 = new TGraphErrors(Xcm1.size());
    TGraphErrors* gPullXCp1 = new TGraphErrors(Xcp1.size());
    cout << "==== X TGraph and histograms initialized ====" << endl;

    // TH2D * hYC0Cm1 = new TH2D("hYC0Cm1","hYC0Cm1", 200, -10, 10, 200, -10, 10);
    // TH2D * hYC0Cp1 = new TH2D("hYC0Cp1","hYC0Cp1", 200, -10, 10, 200, -10, 10);
    // TGraphErrors* gYC = new TGraphErrors(coeffY.size());
    // TGraphErrors* gYCm1 = new TGraphErrors(Ycm1.size());
    // TGraphErrors* gYCp1 = new TGraphErrors(Ycp1.size());
    // TGraphErrors* gYCm1_over_YCp1 = new TGraphErrors(Ycm1.size());
    // TGraphErrors* gYCm1_over_YC0 = new TGraphErrors(Ycm1.size());
    // TGraphErrors* gYCp1_over_YC0 = new TGraphErrors(Ycp1.size());
    // TGraphErrors* gPullYCm1 = new TGraphErrors(Ycm1.size());
    // TGraphErrors* gPullYCp1 = new TGraphErrors(Ycp1.size());
    // cout << "==== Y TGraph and histograms initialized ====" << endl;

    for(int i=0;i<NCH+2;i++){
        gXC->SetPoint(i,i+2,coeffX.at(i));
        gXC->SetPointError(i,0,coeffX_err.at(i));           
        // gYC->SetPoint(i,i,coeffY.at(i));
        // gYC->SetPointError(i,0,coeffY_err.at(i));          
    }
    for(int i=0;i<(int)Xcm1.size();i++){     
        gXCm1->SetPoint(i,i+2,Xcm1.at(i));
        gXCm1->SetPointError(i,0,0);           
        gXCp1->SetPoint(i,i+2,Xcp1.at(i));
        gXCp1->SetPointError(i,0,0);
        gXCm1_over_XCp1->SetPoint(i,i+2,Xcm1.at(i)/Xcp1.at(i));
        gXCm1_over_XCp1->SetPointError(i,0,0);
        gXCm1_over_XC0->SetPoint(i,i+2,Xcm1.at(i)/Xc0.at(i));
        gXCm1_over_XC0->SetPointError(i,0,0);
        gXCp1_over_XC0->SetPoint(i,i+2,Xcp1.at(i)/Xc0.at(i));
        gXCp1_over_XC0->SetPointError(i,0,0);
        gPullXCm1->SetPoint(i,i+2,Xc0.at(i) - Xcm1.at(i));
        gPullXCm1->SetPointError(i,0,0);
        gPullXCp1->SetPoint(i,i+2,Xc0.at(i) - Xcp1.at(i));
        gPullXCp1->SetPointError(i,0,0);            

        hXC0Cm1->Fill(Xc0.at(i),Xcm1.at(i));
        hXC0Cp1->Fill(Xc0.at(i),Xcp1.at(i));
    }
    // for(int i=0;i<(int)Ycm1.size();i++){              
    //     gYCm1->SetPoint(i,i,Ycm1.at(i));
    //     gYCm1->SetPointError(i,0,0);           
    //     gYCp1->SetPoint(i,i,Ycp1.at(i));
    //     gYCp1->SetPointError(i,0,0);
    //     gYCm1_over_YCp1->SetPoint(i,i,Ycm1.at(i)/Ycp1.at(i));
    //     gYCm1_over_YCp1->SetPointError(i,0,0);
    //     gYCm1_over_YC0->SetPoint(i,i,Ycm1.at(i)/Yc0.at(i));
    //     gYCm1_over_YC0->SetPointError(i,0,0);
    //     gYCp1_over_YC0->SetPoint(i,i,Ycp1.at(i)/Yc0.at(i));
    //     gYCp1_over_YC0->SetPointError(i,0,0);
    //     gPullYCm1->SetPoint(i,i,Yc0.at(i) - Ycm1.at(i));
    //     gPullYCm1->SetPointError(i,0,0);
    //     gPullYCp1->SetPoint(i,i,Yc0.at(i) - Ycp1.at(i));
    //     gPullYCp1->SetPointError(i,0,0);            

    //     hYC0Cm1->Fill(Yc0.at(i),Ycm1.at(i));
    //     hYC0Cp1->Fill(Yc0.at(i),Ycp1.at(i));
    // }

    cout << "==== TGraph and histograms Filled ====" << endl;

    //residuals of q_bare - q_reco
    double pullX[16]; // , pullY[16]; // total target vs LG charge
    double deltapullX[16]; // , deltapullY[16]; // total target vs LG charge
    double residualsX[16][3]; // , residualsY[16][3]; // fired strips vs reconstructed strips charge
    double deltaresidualsX[16][3]; // , deltaresidualsY[16][3]; // fired strips vs reconstructed strips charge

    TGraphErrors* gPullX = new TGraphErrors(16);
    // TGraphErrors* gPullY = new TGraphErrors(16);
    // vector<TH2D*> gResX; // 15 runs for X strips
    // vector<TGraphErrors*> gResY; // 11 runs for Y strips
    for(int i=0;i<16;i++){
       // gResY.push_back(new TGraphErrors(3));
    }
    TH2D * gResX = new TH2D("gResX", "gResX", 16, -0.5, 15.5, 1000, -50, 50);
    vector<TH2D *> gpullXi;
    for(int i = 0; i<3; i++){
        gpullXi.push_back(new TH2D(Form("gpullX%d",i), Form("gpullX%d",i), 16, -0.5, 15.5, 1000, -50, 50));
    }
    // int ChUp = NRUNS-1;
    int ChUp = NRUNS-2;
    for(int irun = 0; irun < NRUNS; irun++){
        auto &rc = runs_channels[irun];
        int run = std::get<0>(rc);
        int ix  = std::get<1>(rc);
        // int iy  = std::get<2>(rc);
        if(irun < NRUNS){ // pull wrt the LG charge - Fill X strips
        cout << "Processing residuals and pull for run " << std::get<0>(rc) << " - X strip " << ix << endl;
            if(ix > 0 && ix < ChUp){ 
                double qX_reco = coeffX.at(ix-1)*X_qTar.at(ix)[0] + coeffX.at(ix)*X_qTar.at(ix)[1] + coeffX.at(ix+1)*X_qTar.at(ix)[2];
                pullX[ix] = (X_QLG.at(ix) - qX_reco);
                double ep1 = sqrt( pow(coeffX.at(ix-1)*err_X_qTar.at(ix)[0],2) + pow(coeffX.at(ix)*err_X_qTar.at(ix)[1],2) + pow(coeffX.at(ix+1)*err_X_qTar.at(ix)[2],2)
                                + pow(coeffX_err.at(ix-1)*X_qTar.at(ix)[0],2) + pow(coeffX_err.at(ix)*X_qTar.at(ix)[1],2) + pow(coeffX_err.at(ix+1)*X_qTar.at(ix)[2],2));
                double ep2 = X_QLGerr.at(ix);
                deltapullX[ix] = sqrt(ep1*ep1 + ep2*ep2);
                gPullX->SetPoint(ix, ix+2, pullX[ix]); // I want to map the pull with the physics strip number
                gPullX->SetPointError(ix, 0, deltapullX[ix]);
                TGraphAttribute(gPullX, "gPullX", "X Strip", "Q Pull [pC]", 20, kBlue+irun);
                cout << "Run " << run << " - X Strip " << ix << " - qLG: " << X_QLG.at(ix) << " - qX_reco: " << qX_reco << " - Pull: " << pullX[ix] << endl;
            
                // residuals per strip
                double z_left[3] = {0.};
                double z_right[3] = {0.};
                double qix_reco[3] = {0.};
                // cout << "z_left, z_right, qix_reco and gResX initialized for X strip " << ix << endl;
                for(int iq = 0; iq < 3; iq++){
                    z_left[iq] = ((-1 + iq - delta_strip/2) - coeffX.at(NCH))/coeffX.at(NCH+1);
                    z_right[iq] = ((-1 + iq + delta_strip/2) - coeffX.at(NCH))/coeffX.at(NCH+1);
                    qix_reco[iq] = X_QLG.at(ix)*(TMath::Erf(z_right[iq]/sqrt(2)) - TMath::Erf(z_left[iq]/sqrt(2)))/2;
                    residualsX[ix][iq] = (X_qTar.at(ix)[iq]*coeffX.at(ix -1 +iq) - qix_reco[iq]);
                    double er1 = pow(err_X_qTar.at(ix)[iq]*coeffX.at(ix -1 +iq),2) + pow(X_qTar.at(ix)[iq]*coeffX_err.at(ix -1 +iq),2);
                    double er2 = pow(X_QLGerr.at(ix)*(TMath::Erf(z_right[iq]/sqrt(2)) - TMath::Erf(z_left[iq]/sqrt(2)))/2, 2);
                    deltaresidualsX[ix][iq] = sqrt(er1 + er2);
                    residualsX[ix][iq] /= deltaresidualsX[ix][iq];
                    gResX->Fill(ix -1 + iq, residualsX[ix][iq]);
                    gpullXi.at(iq)->Fill(ix -1 + iq, residualsX[ix][iq]);
                    
                }
                // TGraphAttribute(gResX.at(ix), Form("gResX_run_%d_strip_%d",std::get<0>(rc),ix), "X Strip", "Residuals [pC]", 20, kBlue);
            }
            cout << "Residuals filled for X strip " << ix << endl;
        } else { // pull wrt the LG charge - Fill Y strips
            continue;
            // cout << "Processing residuals and pull for run " << run << " - Y strip " << iy << endl;
            // if(iy > 0 && iy < NCH-2){
            //     cout << "Calculating qY_reco for Y strip " << iy << endl;
            //     double qY_reco = coeffY.at(iy-1)*Y_qTar.at(iy)[0] + coeffY.at(iy)*Y_qTar.at(iy)[1] + coeffY.at(iy+1)*Y_qTar.at(iy)[2];
            //     cout << "prova1 " << endl;
            //     pullY[iy] = (Y_QLG.at(iy) - qY_reco);
            //     cout << "prova2 " << endl;
            //     double ep1 = sqrt( pow(coeffY.at(iy-1)*err_Y_qTar.at(iy)[0],2) + pow(coeffY.at(iy)*err_Y_qTar.at(iy)[1],2) + pow(coeffY.at(iy+1)*err_Y_qTar.at(iy)[2],2)
            //                     + pow(coeffY_err.at(iy-1)*Y_qTar.at(iy)[0],2) + pow(coeffY_err.at(iy)*Y_qTar.at(iy)[1],2) + pow(coeffY_err.at(iy+1)*Y_qTar.at(iy)[2],2));
            //     cout << "prova3 " << endl;
            //     double ep2 = Y_QLGerr.at(iy);
            //     cout << "prova4 " << endl;
            //     deltapullY[iy] = sqrt(ep1*ep1 + ep2*ep2); 
            //     cout << "prova5 " << endl;
            //     gPullY->SetPoint(iy, iy, pullY[iy]);
            //     cout << "prova6 " << endl;
            //     gPullY->SetPointError(iy, 0, deltapullY[iy]);
            //     cout << "Run " << run << " - Y Strip " << iy << " - qLG: " << Y_QLG.at(iy) << " - qY_reco: " << qY_reco << " - Pull: " << pullY[iy] << endl;
            
            //     // residuals per strip
            //     double z_left[3] = {0.};
            //     double z_right[3] = {0.};
            //     double qiy_reco[3] = {0.};
            //     for(int iq = 0; iq < 3; iq++){
            //         z_left[iq] = ((iy -1 + iq - delta_strip/2) - coeffY.at(NCH))/coeffY.at(NCH+1);
            //         z_right[iq] = ((iy -1 + iq + delta_strip/2) - coeffY.at(NCH))/coeffY.at(NCH+1);
            //         qiy_reco[iq] = Y_QLG.at(iy)*coeffY.at(iy -1 + iq)*(TMath::Erf(z_right[iq]/sqrt(2)) - TMath::Erf(z_left[iq]/sqrt(2)))/2;
            //         residualsY[iy][iq] = Y_qTar.at(iy)[iq] - qiy_reco[iq];
            //         double e1 = err_Y_qTar.at(iy)[iq];
            //         double e2 = sqrt(pow(Y_QLGerr.at(iy)*coeffY.at(iy -1 +iq), 2) + pow(coeffY_err.at(iy -1 +iq)*Y_QLG.at(iy), 2))*(TMath::Erf(z_right[iq]/sqrt(2)) - TMath::Erf(z_left[iq]/sqrt(2)))/2;
            //         deltaresidualsY[iy][iq] = sqrt(e1*e1 + e2*e2);
            //         gResY[iy]->SetPoint(iq, iy -1 + iq, residualsY[iy][iq]);
            //         gResY[iy]->SetPointError(iq, 0, deltaresidualsY[iy][iq]);
            //     }
            // }
        }   
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

    // TGraphAttribute(gYC,"gYC","Y", "coeff", 20, kBlue);
    // TGraphAttribute(gYCm1,"gYCm1","Y", "c_{i-1}", 20, kBlue);
    // TGraphAttribute(gYCp1,"gYCp1","Y", "c_{i+1}", 20, kBlue);
    // TGraphAttribute(gYC,"gYC","Y", "coeff", 20, kBlue);
    // TGraphAttribute(gYCm1_over_YCp1,"gYCm1_over_YCp1","Y", "c_{i-1}/c_{i+1}", 20, kBlue);
    // TGraphAttribute(gYCm1_over_YC0,"gYCm1_over_YC0","Y", "c_{i-1}/c_{i}", 20, kBlue);
    // TGraphAttribute(gYCp1_over_YC0,"gYCp1_over_YC0","Y", "c_{i+1}/c_{i}", 20, kBlue);
    // TGraphAttribute(gPullYCm1,"gPullYCm1","Y", "c_{i-1} - c_{i}", 20, kBlue);
    // TGraphAttribute(gPullYCp1,"gPullYCp1","Y", "c_{i+1} - c_{i}", 20, kBlue);

    //writing out the histrograms in a root file
    // make directory in the output file
    histoFile->mkdir("Residuals");
    histoFile->mkdir("LeadGlassCharge");
    histoFile->mkdir("TargetCharge");
    histoFile->cd();
    for(int irun = 0; irun < NRUNS; irun++){
        if(irun<NRUNS) gQx.at(irun)->Write();
        // else gQy.at(irun)->Write();
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

    // gYC->Write();
    // gYCm1->Write();
    // gYCp1->Write();
    // gYCm1_over_YCp1->Write();
    // gYCm1_over_YC0->Write();
    // gYCp1_over_YC0->Write();
    // gPullYCm1->Write();
    // gPullYCp1->Write();
    // hYC0Cm1->Write();
    // hYC0Cp1->Write();

    histoFile->cd("Residuals");
    gResX->Write();
    for(auto gr : gpullXi){
        gr->Write();
    }
    gPullX->Write();

    histoFile->cd();
    histoFile->cd("LeadGlassCharge");
    for(int i = 0; i < gqLG_mean.size(); i++){
        gqLG_mean.at(i)->Write();
        gqLG_sigma.at(i)->Write();
        gqLG_sigma_over_mean.at(i)->Write();
    }
    histoFile->cd();
    histoFile->cd("TargetCharge");
    for(int i = 0; i < gqLG_mean.size(); i++){
        gqtarget_m1.at(i)->Write();
        gqtarget_0.at(i)->Write();
        gqtarget_p1.at(i)->Write();
    }

    printf("Output file%s closed \n",outputFileName.Data());
    histoFile->Close();
    
    cout << "Calibration done." << endl;
    return 0;
}

