#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
// #include <filesystem>  // C++17 standard library for file system operations
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TStyle.h> 
#include <TString.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TEfficiency.h>
#include <regex>
#include <sstream>

using namespace std;
// namespace fs = filesystem;

int TGraphAttribute(TGraphErrors *Graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
    Graph->SetTitle(title);
    Graph->SetName(title);
    Graph->GetXaxis()->SetTitle(xlabel);
    Graph->GetYaxis()->SetTitle(ylabel);
    Graph->SetMarkerStyle(markerstyle);
    Graph->SetMarkerColor(color);
    Graph->SetLineWidth(2);
    return 0;
}

static TFile* OpenRootOrThrow(const string& path) {
    TFile* f = TFile::Open(path.c_str(), "READ");
    if (!f || f->IsZombie()) {
        throw runtime_error("Cannot open ROOT file: " + path);
    }
    return f;
}

static void Require(bool cond, const string& msg) {
    if (!cond) throw runtime_error(msg);
}

void XSAnalyzer(const string& baba_ee_root,
                const string& baba_gg_root,
                const string& calc_ee_root,
                const string& calc_gg_root,
                const string& calc_sc_root,
                const string& out_root)
{

    double LumiPadme = 1.054e-14; //pb^-1 - l_tar = 0.100 mm
    vector<int> id;
    vector<double> SqrtS, SqrtSMeV, Ebeam;
    vector<double> XseeBaba, errXseeBaba, XseeBaba_tree, errXseeBaba_tree;
    vector<double> XsggBaba, errXsggBaba, XsggBaba_tree, errXsggBaba_tree;
    vector<double> XseeCalc, errXseeCalc, XsggCalc, errXsggCalc, XsscCalc, errXsscCalc;

    ifstream BabaInputtxtee("/home/mancinima/Babayagapadme-fw/LongBhabha/outputXS_sorted.txt");
    ifstream BabaInputtxtgg("/home/mancinima/Babayagapadme-fw/LongAA/outputXS_sorted.txt");
    ifstream CalcInputtxtee("/home/mancinima/Calchep/CalcHEPFiles/ScanCalchepBhabha/CrossSectionCalcHep.txt");
    ifstream CalcInputtxtgg("/home/mancinima/Calchep/CalcHEPFiles/ScanCalchepAA/outputXS.txt"); //to be checked after simulations
    ifstream CalcInputtxtsc("/home/mancinima/Calchep/CalcHEPFiles/ScanCalchepSChannel/outputXS.txt"); //to be checked after simulations

    Require(BabaInputtxtee.is_open(), "Error opening BabaInputtxtee file");
    Require(BabaInputtxtgg.is_open(), "Error opening BabaInputtxtgg file");
    Require(CalcInputtxtee.is_open(), "Error opening CalcInputtxtee file");
    Require(CalcInputtxtgg.is_open(), "Error opening CalcInputtxtgg file");
    Require(CalcInputtxtsc.is_open(), "Error opening CalcInputtxtsc file");

    string line;
    // Skip headers
    getline(BabaInputtxtee, line);
    getline(BabaInputtxtgg, line);
    getline(CalcInputtxtee, line);
    getline(CalcInputtxtgg, line);
    getline(CalcInputtxtsc, line);

    // ---- Read Babayaga ee
    while (getline(BabaInputtxtee, line)) {
        stringstream ss(line);
        string token;
        vector<double> row;
        while (getline(ss, token, ',')) row.push_back(stod(token));
        if (row.size() < 7) continue;
        if (row[0] < 6) continue;

        id.push_back((int)row[0]);
        SqrtS.push_back(row[2]);
        XseeBaba.push_back(row[3]*1e3);
        errXseeBaba.push_back(row[4]*1e3);
        XseeBaba_tree.push_back(row[5]*1e3);
        errXseeBaba_tree.push_back(row[6]*1e3);
    }
    BabaInputtxtee.close();

    // ---- Read Babayaga gg
    while (getline(BabaInputtxtgg, line)) {
        stringstream ss(line);
        string token;
        vector<double> row;
        while (getline(ss, token, ',')) row.push_back(stod(token));
        if (row.size() < 7) continue;
        if (row[0] < 6) continue;

        XsggBaba.push_back(row[3]*1e3);
        errXsggBaba.push_back(row[4]*1e3);
        XsggBaba_tree.push_back(row[5]*1e3);
        errXsggBaba_tree.push_back(row[6]*1e3);
    }
    BabaInputtxtgg.close();
    
    // ---- Read CalcHEP ee
    while (getline(CalcInputtxtee, line)) {
        stringstream ss(line);
        string token;
        vector<double> row;
        while (getline(ss, token, ',')) row.push_back(stod(token));
        if (row.size() < 5) continue;
        if (row[0] < 5) continue;

        Ebeam.push_back(row[1]);
        XseeCalc.push_back(row[3]);
        errXseeCalc.push_back(row[4]);
    }
    CalcInputtxtee.close();
    
    // ---- Read CalcHEP gg
    while (getline(CalcInputtxtgg, line)) {
        stringstream ss(line);
        string token;
        vector<double> row;
        while (getline(ss, token, ',')) row.push_back(stod(token));
        if (row.size() < 5) continue;
        if (row[0] < 5) continue;

        XsggCalc.push_back(row[3]);
        errXsggCalc.push_back(row[4]);
    }
    CalcInputtxtgg.close();  

    // ---- Read CalcHEP s-channel
    while (getline(CalcInputtxtsc, line)) {
        stringstream ss(line);
        string token;
        vector<double> row;
        while (getline(ss, token, ',')) row.push_back(stod(token));
        if (row.size() < 5) continue;
        if (row[0] < 5) continue;

        XsscCalc.push_back(row[3]);
        errXsscCalc.push_back(row[4]);
    }
    CalcInputtxtsc.close(); 

    Require(!SqrtS.empty(), "SqrtS is empty (check babayaga ee txt parsing)");
    Require(Ebeam.size() >= SqrtS.size(), "Ebeam vector shorter than SqrtS; inputs mismatched?");
    // Require(XsggBaba.size() >= SqrtS.size(), "Baba gg shorter than Baba ee; inputs mismatched?");
    // Require(XseeCalc.size() >= SqrtS.size(), "Calc ee shorter than Baba ee; inputs mismatched?");
    // Require(XsggCalc.size() >= SqrtS.size(), "Calc gg shorter than Baba ee; inputs mismatched?");
    // Require(XsscCalc.size() >= SqrtS.size(), "Calc sc shorter than Baba ee; inputs mismatched?");

    const double Nev_babaee = 2000000;
    const double Nev_babagg = 2000000;
    const double Nev_calcee = 100000;
    const double Nev_calcgg = 100000;
    const double Nev_calcsc = 100000;

    const int nbins = (int)SqrtS.size() - 1;
    Require(nbins > 0, "nbins <= 0: not enough points in SqrtS");

    
    TH1D *htotbabaee = new TH1D("htotbabaee", "htotbabaee", nbins, -0.5, nbins+0.5);
    TH1D *htotbabagg = new TH1D("htotbabagg", "htotbabagg", nbins, -0.5, nbins+0.5);
    TH1D *htotcalcee = new TH1D("htotcalcee", "htotcalcee", nbins, -0.5, nbins+0.5);
    TH1D *htotcalcgg = new TH1D("htotcalcgg", "htotcalcgg", nbins, -0.5, nbins+0.5);
    TH1D *htotcalcsc = new TH1D("htotcalcsc", "htotcalcsc", nbins, -0.5, nbins+0.5);

    TH1D *hNAcc_babaee = new TH1D("hNAcc_babaee", "hNAcc_babaee", nbins, -0.5, nbins+0.5);
    TH1D *hNAcc_babagg = new TH1D("hNAcc_babagg", "hNAcc_babagg", nbins, -0.5, nbins+0.5);
    TH1D *hNAcc_calcee = new TH1D("hNAcc_calcee", "hNAcc_calcee", nbins, -0.5, nbins+0.5);
    TH1D *hNAcc_calcgg = new TH1D("hNAcc_calcgg", "hNAcc_calcgg", nbins, -0.5, nbins+0.5);
    TH1D *hNAcc_calcsc = new TH1D("hNAcc_calcsc", "hNAcc_calcsc", nbins, -0.5, nbins+0.5);

    // Open ROOT inputs
    TFile *BabaeeRootFile = OpenRootOrThrow(baba_ee_root);
    TFile *BabaggRootFile = OpenRootOrThrow(baba_gg_root);
    TFile *CalceeRootFile = OpenRootOrThrow(calc_ee_root);
    TFile *CalcggRootFile = OpenRootOrThrow(calc_gg_root);
    TFile *CalcscRootFile = OpenRootOrThrow(calc_sc_root);

    // Loop energies
    for (int i = 0; i < nbins; ++i) {
        SqrtSMeV.push_back(SqrtS.at(i) * 1000.0);

        const int e10 = (int)lround(Ebeam.at(i) * 10.0);
        auto get_entries = [&](TFile* f) -> int {
            TDirectory *td = (TDirectory*)f->Get(Form("BhabhaFull_dir%i/labdir", e10));
            if (!td) return -1;

            TH1F* h = (TH1F*)td->Get(Form("hFEnAllCuts_P3_lab%.2f", Ebeam.at(i)));
            if (!h) return -1;

            return (int)h->GetEntries();
        };

        int acc_babaee = get_entries(BabaeeRootFile);
        int acc_babagg = get_entries(BabaggRootFile);
        int acc_calcee = get_entries(CalceeRootFile);
        int acc_calcgg = get_entries(CalcggRootFile);
        int acc_calcsc = get_entries(CalcscRootFile);

        Require(acc_babaee >= 0, "Missing histo/dir in baba ee ROOT at e10=" + to_string(e10));
        Require(acc_babagg >= 0, "Missing histo/dir in baba gg ROOT at e10=" + to_string(e10));
        Require(acc_calcee >= 0, "Missing histo/dir in calc ee ROOT at e10=" + to_string(e10));
        Require(acc_calcgg >= 0, "Missing histo/dir in calc gg ROOT at e10=" + to_string(e10));
        Require(acc_calcsc >= 0, "Missing histo/dir in calc sc ROOT at e10=" + to_string(e10));

        htotbabaee->SetBinContent(i+1, Nev_babaee);
        htotbabagg->SetBinContent(i+1, Nev_babagg);
        htotcalcee->SetBinContent(i+1, Nev_calcee);
        htotcalcgg->SetBinContent(i+1, Nev_calcgg);
        htotcalcsc->SetBinContent(i+1, Nev_calcsc);

        hNAcc_babaee->SetBinContent(i+1, acc_babaee);
        hNAcc_babagg->SetBinContent(i+1, acc_babagg);
        hNAcc_calcee->SetBinContent(i+1, acc_calcee);
        hNAcc_calcgg->SetBinContent(i+1, acc_calcgg);
        hNAcc_calcsc->SetBinContent(i+1, acc_calcsc);
    }

    // Efficiencies
    TEfficiency* pEff_babaee = new TEfficiency(*hNAcc_babaee, *htotbabaee);
    TEfficiency* pEff_babagg = new TEfficiency(*hNAcc_babagg, *htotbabagg);
    TEfficiency* pEff_calcee = new TEfficiency(*hNAcc_calcee, *htotcalcee);
    TEfficiency* pEff_calcgg = new TEfficiency(*hNAcc_calcgg, *htotcalcgg);
    TEfficiency* pEff_calcsc = new TEfficiency(*hNAcc_calcsc, *htotcalcsc);

    TGraphErrors* gAcc_babaee = new TGraphErrors(nbins);
    TGraphErrors* gAcc_babagg = new TGraphErrors(nbins);
    TGraphErrors* gAcc_calcee = new TGraphErrors(nbins);
    TGraphErrors* gAcc_calcgg = new TGraphErrors(nbins);
    TGraphErrors* gAcc_calcsc = new TGraphErrors(nbins);

    TGraphErrors* gdiffErrEff_babaee = new TGraphErrors(nbins);
    TGraphErrors* gdiffErrEff_babagg = new TGraphErrors(nbins);
    TGraphErrors* gdiffErrEff_calcee = new TGraphErrors(nbins);
    TGraphErrors* gdiffErrEff_calcgg = new TGraphErrors(nbins);
    TGraphErrors* gdiffErrEff_calcsc = new TGraphErrors(nbins);

    // Create TGraphErrors
    // keep working on schannel stuff from here
    TGraphErrors* gBabaTree_CalcHep_ee = new TGraphErrors(nbins);
    TGraphErrors* gBabaFull_CalcHep_ee = new TGraphErrors(nbins);
    TGraphErrors* gBabaFull_CalcHep_Eff_ee = new TGraphErrors(nbins);

    TGraphErrors* gBabaTree_CalcHep_gg = new TGraphErrors(nbins);
    TGraphErrors* gBabaFull_CalcHep_gg = new TGraphErrors(nbins);
    TGraphErrors* gBabaFull_CalcHep_Eff_gg = new TGraphErrors(nbins);

    TGraphErrors* gBabaFull_CalcHep_Eff_SM = new TGraphErrors(nbins);

    TGraphErrors* gXsBabaFullVSsm1_ee = new TGraphErrors(nbins);
    TGraphErrors* gXsBabaTreeVSsm1_ee = new TGraphErrors(nbins);
    TGraphErrors* gXsCalcVSsm1_ee = new TGraphErrors(nbins);

    TGraphErrors* gXsBabaFullVSsm1_gg = new TGraphErrors(nbins);
    TGraphErrors* gXsBabaTreeVSsm1_gg = new TGraphErrors(nbins);
    TGraphErrors* gXsCalcVSsm1_gg = new TGraphErrors(nbins);  
    
    TGraphErrors* gXsEvBabaFullVSsm1_ee = new TGraphErrors(nbins);
    TGraphErrors* gXsEvCalcVSsm1_ee = new TGraphErrors(nbins);

    TGraphErrors* gXsEvBabaFullVSsm1_gg = new TGraphErrors(nbins);
    TGraphErrors* gXsEvCalcVSsm1_gg = new TGraphErrors(nbins);

    TGraphErrors* geeYieldbaba = new TGraphErrors(nbins);
    TGraphErrors* geeYieldcalchep = new TGraphErrors(nbins);
    TGraphErrors* gggYieldbaba = new TGraphErrors(nbins);
    TGraphErrors* gggYieldcalchep = new TGraphErrors(nbins);
    TGraphErrors* gscYieldbaba = new TGraphErrors(nbins);
    TGraphErrors* gscYieldcalchep = new TGraphErrors(nbins);
    TGraphErrors* gbkgYieldbaba = new TGraphErrors(nbins);
    TGraphErrors* gbkgYieldcalchep = new TGraphErrors(nbins);

    TGraphErrors* gSignalEffOverBkg = new TGraphErrors(nbins);
    TGraphErrors* gSignalEffOverBkg_calchep = new TGraphErrors(nbins);

    
    vector<double> rBabaFull_Calc_ee, erBabaFull_Calc_ee;
    vector<double> rBabaTree_Calc_ee, erBabaTree_Calc_ee;
    vector<double> rBabaFull_CalcEffi_ee, erBabaFull_CalcEffi_ee;
    vector<double> rBabaFull_CalcEffi_SM;

    vector<double> rBabaFull_Calc_gg, erBabaFull_Calc_gg;
    vector<double> rBabaTree_Calc_gg, erBabaTree_Calc_gg;
    vector<double> rBabaFull_CalcEffi_gg, erBabaFull_CalcEffi_gg;
    vector<double> erBabaFull_CalcEffi_SM;

    
    for(int i = 1; i <= nbins; i++) {
        double effbabaee = pEff_babaee->GetEfficiency(i);
        double effbabagg = pEff_babagg->GetEfficiency(i);
        double effcalcee = pEff_calcee->GetEfficiency(i);
        double effcalcgg = pEff_calcgg->GetEfficiency(i);
        double effcalcsc = pEff_calcsc->GetEfficiency(i);
        double err_effbabaee = (pEff_babaee->GetEfficiencyErrorLow(i) + pEff_babaee->GetEfficiencyErrorUp(i))/2;
        double err_effbabagg = (pEff_babagg->GetEfficiencyErrorLow(i) + pEff_babagg->GetEfficiencyErrorUp(i))/2;
        double err_effcalcee = (pEff_calcee->GetEfficiencyErrorLow(i) + pEff_calcee->GetEfficiencyErrorUp(i))/2;
        double err_effcalcgg = (pEff_calcgg->GetEfficiencyErrorLow(i) + pEff_calcgg->GetEfficiencyErrorUp(i))/2;
        double err_effcalcsc = (pEff_calcsc->GetEfficiencyErrorLow(i) + pEff_calcsc->GetEfficiencyErrorUp(i))/2;

        gAcc_babaee->SetPoint(i-1, SqrtSMeV.at(i-1), effbabaee);
        gAcc_babaee->SetPointError(i-1, 0, err_effbabaee);
        gAcc_babagg->SetPoint(i-1, SqrtSMeV.at(i-1), effbabagg);
        gAcc_babagg->SetPointError(i-1, 0, err_effbabagg);
        gAcc_calcee->SetPoint(i-1, SqrtSMeV.at(i-1), effcalcee);
        gAcc_calcee->SetPointError(i-1, 0, err_effcalcee);
        gAcc_calcgg->SetPoint(i-1, SqrtSMeV.at(i-1), effcalcgg);
        gAcc_calcgg->SetPointError(i-1, 0, err_effcalcgg);
        gAcc_calcsc->SetPoint(i-1, SqrtSMeV.at(i-1), effcalcsc);
        gAcc_calcsc->SetPointError(i-1, 0, err_effcalcsc);

        gdiffErrEff_babaee->SetPoint(i-1, SqrtSMeV.at(i-1), abs(pEff_babaee->GetEfficiencyErrorLow(i) - pEff_babaee->GetEfficiencyErrorUp(i))/(pEff_babaee->GetEfficiencyErrorLow(i) + pEff_babaee->GetEfficiencyErrorUp(i)));
        gdiffErrEff_babaee->SetPointError(i-1, 0, 0);
        gdiffErrEff_babagg->SetPoint(i-1, SqrtSMeV.at(i-1), abs(pEff_babagg->GetEfficiencyErrorLow(i) - pEff_babagg->GetEfficiencyErrorUp(i))/(pEff_babagg->GetEfficiencyErrorLow(i) + pEff_babagg->GetEfficiencyErrorUp(i)));
        gdiffErrEff_babagg->SetPointError(i-1, 0, 0);
        gdiffErrEff_calcee->SetPoint(i-1, SqrtSMeV.at(i-1), abs(pEff_calcee->GetEfficiencyErrorLow(i) - pEff_calcee->GetEfficiencyErrorUp(i))/(pEff_calcee->GetEfficiencyErrorLow(i) + pEff_calcee->GetEfficiencyErrorUp(i)));
        gdiffErrEff_calcee->SetPointError(i-1, 0, 0);
        gdiffErrEff_calcgg->SetPoint(i-1, SqrtSMeV.at(i-1), abs(pEff_calcgg->GetEfficiencyErrorLow(i) - pEff_calcgg->GetEfficiencyErrorUp(i))/(pEff_calcgg->GetEfficiencyErrorLow(i) + pEff_calcgg->GetEfficiencyErrorUp(i)));
        gdiffErrEff_calcgg->SetPointError(i-1, 0, 0);
        gdiffErrEff_calcsc->SetPoint(i-1, SqrtSMeV.at(i-1), abs(pEff_calcsc->GetEfficiencyErrorLow(i) - pEff_calcsc->GetEfficiencyErrorUp(i))/(pEff_calcsc->GetEfficiencyErrorLow(i) + pEff_calcsc->GetEfficiencyErrorUp(i)));
        gdiffErrEff_calcsc->SetPointError(i-1, 0, 0);


        rBabaFull_Calc_ee.push_back(XseeBaba.at(i-1) / XseeCalc.at(i-1));
        erBabaFull_Calc_ee.push_back(errXseeBaba.at(i-1) / XseeCalc.at(i-1)); 
        rBabaTree_Calc_ee.push_back(XseeBaba_tree.at(i-1) / XseeCalc.at(i-1));
        erBabaTree_Calc_ee.push_back(errXseeBaba_tree.at(i-1) / XseeCalc.at(i-1)); 

        rBabaFull_Calc_gg.push_back(XsggBaba.at(i-1) / XsggCalc.at(i-1));
        erBabaFull_Calc_gg.push_back(errXsggBaba.at(i-1) / XsggCalc.at(i-1)); 
        rBabaTree_Calc_gg.push_back(XsggBaba_tree.at(i-1) / XsggCalc.at(i-1));
        erBabaTree_Calc_gg.push_back(errXsggBaba_tree.at(i-1) / XsggCalc.at(i-1)); 
        
        rBabaFull_CalcEffi_ee.push_back((XseeBaba.at(i-1)*effbabaee)/(XseeCalc.at(i-1)*effcalcee));
        // erBabaFull_CalcEffi_ee.push_back(((errXseeBaba.at(i-1)/XseeCalc.at(i-1))*(effbabaee/effcalcee)) + ((err_effbabaee*XseeBaba.at(i-1))/(XseeCalc.at(i-1)*effcalcee)) + 
        //                         ((err_effcalcee*XseeBaba.at(i-1)*effbabaee)/(XseeCalc.at(i-1)*effcalcee*effcalcee)));
        erBabaFull_CalcEffi_ee.push_back((XseeBaba.at(i-1)*effbabaee)/(XseeCalc.at(i-1)*effcalcee)*
                                        0.8*TMath::Sqrt(// errXseeBaba.at(i-1)*errXseeBaba.at(i-1)/XseeBaba.at(i-1)/XseeBaba.at(i-1)+
                                                    err_effbabaee*err_effbabaee/effbabaee/effbabaee+
                                                    // errXseeCalc.at(i-1)*errXseeCalc.at(i-1)/XseeCalc.at(i-1)/XseeCalc.at(i-1)+
                                                    err_effcalcee*err_effcalcee/effcalcee/effcalcee)
                                        );

        rBabaFull_CalcEffi_gg.push_back((XsggBaba.at(i-1)*effbabagg)/(XsggCalc.at(i-1)*effcalcgg));
        // erBabaFull_CalcEffi_gg.push_back(((errXsggBaba.at(i-1)/XsggCalc.at(i-1))*(effbabagg/effcalcgg)) + ((err_effbabagg*XsggBaba.at(i-1))/(XsggCalc.at(i-1)*effcalcgg)) + 
        //                         ((err_effcalcgg*XsggBaba.at(i-1)*effbabagg)/(XsggCalc.at(i-1)*effcalcgg*effcalcgg)));
        erBabaFull_CalcEffi_gg.push_back((XsggBaba.at(i-1)*effbabagg)/(XsggCalc.at(i-1)*effcalcgg)*
                                        TMath::Sqrt(// errXsggBaba.at(i-1)*errXsggBaba.at(i-1)/XsggBaba.at(i-1)/XsggBaba.at(i-1)+
                                                    err_effbabagg*err_effbabagg/effbabagg/effbabagg+
                                                    // errXsggCalc.at(i-1)*errXsggCalc.at(i-1)/XsggCalc.at(i-1)/XsggCalc.at(i-1)+
                                                    err_effcalcgg*err_effcalcgg/effcalcgg/effcalcgg)
                                        );

        rBabaFull_CalcEffi_SM.push_back((XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)/(XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg));
        erBabaFull_CalcEffi_SM.push_back(((XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)/(XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg))*
                                        TMath::Sqrt(((pow(XseeBaba.at(i-1)*err_effbabaee,2)+pow(XsggBaba.at(i-1)*err_effbabagg,2))/pow(XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg,2)) +
                                                    ((pow(XseeCalc.at(i-1)*err_effcalcee,2)+pow(XsggCalc.at(i-1)*err_effcalcgg,2))/pow(XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg,2)))
                                        );                                

        // filling TGraphs
        gBabaFull_CalcHep_ee->SetPoint(i-1, SqrtSMeV.at(i-1), rBabaFull_Calc_ee.at(i-1));
        gBabaFull_CalcHep_ee->SetPointError(i-1, 0, erBabaFull_Calc_ee.at(i-1));  
        gBabaTree_CalcHep_ee->SetPoint(i-1, SqrtSMeV.at(i-1), rBabaTree_Calc_ee.at(i-1));
        gBabaTree_CalcHep_ee->SetPointError(i-1, 0, erBabaTree_Calc_ee.at(i-1)); 
        gBabaFull_CalcHep_Eff_ee->SetPoint(i-1, SqrtSMeV.at(i-1), rBabaFull_CalcEffi_ee.at(i-1));
        gBabaFull_CalcHep_Eff_ee->SetPointError(i-1, 0, erBabaFull_CalcEffi_ee.at(i-1));  

        gBabaFull_CalcHep_gg->SetPoint(i-1, SqrtSMeV.at(i-1), rBabaFull_Calc_gg.at(i-1));
        gBabaFull_CalcHep_gg->SetPointError(i-1, 0, erBabaFull_Calc_gg.at(i-1));  
        gBabaTree_CalcHep_gg->SetPoint(i-1, SqrtSMeV.at(i-1), rBabaTree_Calc_gg.at(i-1));
        gBabaTree_CalcHep_gg->SetPointError(i-1, 0, erBabaTree_Calc_gg.at(i-1)); 
        gBabaFull_CalcHep_Eff_gg->SetPoint(i-1, SqrtSMeV.at(i-1), rBabaFull_CalcEffi_gg.at(i-1));
        gBabaFull_CalcHep_Eff_gg->SetPointError(i-1, 0, erBabaFull_CalcEffi_gg.at(i-1));  

        gBabaFull_CalcHep_Eff_SM->SetPoint(i-1, SqrtSMeV.at(i-1), rBabaFull_CalcEffi_SM.at(i-1));
        gBabaFull_CalcHep_Eff_SM->SetPointError(i-1, 0, erBabaFull_CalcEffi_SM.at(i-1));

        gXsBabaFullVSsm1_ee->SetPoint(i-1, SqrtSMeV.at(i-1), XseeBaba.at(i-1));
        gXsBabaFullVSsm1_ee->SetPointError(i-1, 0, errXseeBaba.at(i-1)); 
        gXsBabaTreeVSsm1_ee->SetPoint(i-1, SqrtSMeV.at(i-1), XseeBaba_tree.at(i-1));
        gXsBabaTreeVSsm1_ee->SetPointError(i-1, 0, errXseeBaba_tree.at(i-1));  
        gXsCalcVSsm1_ee->SetPoint(i-1, SqrtSMeV.at(i-1), XseeCalc.at(i-1));
        gXsCalcVSsm1_ee->SetPointError(i-1, 0, 0);  // Error on calchep XS to be checked

        gXsBabaFullVSsm1_gg->SetPoint(i-1, SqrtSMeV.at(i-1), XsggBaba.at(i-1));
        gXsBabaFullVSsm1_gg->SetPointError(i-1, 0, errXsggBaba.at(i-1)); 
        gXsBabaTreeVSsm1_gg->SetPoint(i-1, SqrtSMeV.at(i-1), XsggBaba_tree.at(i-1));
        gXsBabaTreeVSsm1_gg->SetPointError(i-1, 0, errXsggBaba_tree.at(i-1));  
        gXsCalcVSsm1_gg->SetPoint(i-1, SqrtSMeV.at(i-1), XsggCalc.at(i-1));
        gXsCalcVSsm1_gg->SetPointError(i-1, 0, 0);  // Error on calchep XS to be checked
        
        gXsEvBabaFullVSsm1_ee->SetPoint(i-1, SqrtSMeV.at(i-1), XseeBaba.at(i-1)*effbabaee);
        gXsEvBabaFullVSsm1_ee->SetPointError(i-1, 0, errXseeBaba.at(i-1)*effbabaee + XseeBaba.at(i-1)*err_effbabaee);  
        gXsEvCalcVSsm1_ee->SetPoint(i-1, SqrtSMeV.at(i-1), XseeCalc.at(i-1)*effcalcee);
        gXsEvCalcVSsm1_ee->SetPointError(i-1, 0, XseeCalc.at(i-1)*err_effcalcee);  

        gXsEvBabaFullVSsm1_gg->SetPoint(i-1, 1/(SqrtSMeV.at(i-1)*SqrtSMeV.at(i-1)), XsggBaba.at(i-1)*effbabagg);
        gXsEvBabaFullVSsm1_gg->SetPointError(i-1, 0, errXsggBaba.at(i-1)*effbabagg + XsggBaba.at(i-1)*err_effbabagg);  
        gXsEvCalcVSsm1_gg->SetPoint(i-1, 1/(SqrtSMeV.at(i-1)*SqrtSMeV.at(i-1)), XsggCalc.at(i-1)*effcalcgg);
        gXsEvCalcVSsm1_gg->SetPointError(i-1, 0, XsggCalc.at(i-1)*err_effcalcgg);  

        geeYieldbaba->SetPoint(i-1, SqrtSMeV.at(i-1), XseeBaba.at(i-1)*effbabaee*LumiPadme);
        geeYieldbaba->SetPointError(i-1, 0, XseeBaba.at(i-1)*err_effbabaee*LumiPadme);
        geeYieldcalchep->SetPoint(i-1, SqrtSMeV.at(i-1), XseeCalc.at(i-1)*effcalcee*LumiPadme);
        geeYieldcalchep->SetPointError(i-1, 0, XseeCalc.at(i-1)*err_effcalcee*LumiPadme);
        gggYieldbaba->SetPoint(i-1, SqrtSMeV.at(i-1), XsggBaba.at(i-1)*effbabagg*LumiPadme);
        gggYieldbaba->SetPointError(i-1, 0, XsggBaba.at(i-1)*err_effbabagg*LumiPadme);
        gggYieldcalchep->SetPoint(i-1, SqrtSMeV.at(i-1), XsggCalc.at(i-1)*effcalcgg*LumiPadme);
        gggYieldcalchep->SetPointError(i-1, 0, XsggCalc.at(i-1)*err_effcalcgg*LumiPadme);
        gscYieldcalchep->SetPoint(i-1, SqrtSMeV.at(i-1), XsscCalc.at(i-1)*effcalcsc*LumiPadme);
        gscYieldcalchep->SetPointError(i-1, 0, XsscCalc.at(i-1)*err_effcalcsc*LumiPadme);

        gbkgYieldbaba->SetPoint(i-1, SqrtSMeV.at(i-1), (XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)*LumiPadme);
        gbkgYieldbaba->SetPointError(i-1, 0, TMath::Sqrt(XseeBaba.at(i-1)*XseeBaba.at(i-1)*err_effbabaee*err_effbabaee + XsggBaba.at(i-1)*XsggBaba.at(i-1)*err_effbabagg*err_effbabagg)*LumiPadme);

        gbkgYieldcalchep->SetPoint(i-1, SqrtSMeV.at(i-1), (XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg)*LumiPadme);
        gbkgYieldcalchep->SetPointError(i-1, 0, TMath::Sqrt(XseeCalc.at(i-1)*XseeCalc.at(i-1)*err_effcalcee*err_effcalcee + XsggCalc.at(i-1)*XsggCalc.at(i-1)*err_effcalcgg*err_effcalcgg)*LumiPadme);

        // gSignalEffOverBkg->SetPoint(i-1, SqrtSMeV.at(i-1), (XsscCalc.at(i-1)*effcalcsc)/(XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg));
        // gSignalEffOverBkg->SetPointError(i-1, 0, (XsscCalc.at(i-1)*effcalcsc)/(XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)*
        //                                 TMath::Sqrt(err_effcalcsc*err_effcalcsc/effcalcsc/effcalcsc+
        //                                             ((XseeBaba.at(i-1)*XseeBaba.at(i-1)*err_effbabaee*err_effbabaee + XsggBaba.at(i-1)*XsggBaba.at(i-1)*err_effbabagg*err_effbabagg)/
        //                                             (XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)/(XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg))));
    
        gSignalEffOverBkg->SetPoint(i-1, SqrtSMeV.at(i-1),(XsscCalc.at(i-1)*effcalcsc*LumiPadme) / 
                            TMath::Sqrt((XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)*LumiPadme));
        gSignalEffOverBkg->SetPointError(i-1, 0, ((XsscCalc.at(i-1)*effcalcsc*LumiPadme) /
                            TMath::Sqrt((XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)*LumiPadme)) *
                            TMath::Sqrt( (err_effcalcsc*err_effcalcsc)/(effcalcsc*effcalcsc) +  
                                0.25 * ((XseeBaba.at(i-1)*XseeBaba.at(i-1)*err_effbabaee*err_effbabaee +
                                        XsggBaba.at(i-1)*XsggBaba.at(i-1)*err_effbabagg*err_effbabagg) /
                                        ((XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg) *
                                        (XseeBaba.at(i-1)*effbabaee + XsggBaba.at(i-1)*effbabagg)))));

        gSignalEffOverBkg_calchep->SetPoint(i-1, SqrtSMeV.at(i-1),  (XsscCalc.at(i-1)*effcalcsc*LumiPadme) /
                            TMath::Sqrt((XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg)*LumiPadme));
        gSignalEffOverBkg_calchep->SetPointError( i-1, 0, ((XsscCalc.at(i-1)*effcalcsc*LumiPadme) /
                            TMath::Sqrt((XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg)*LumiPadme)) *
                            TMath::Sqrt((err_effcalcsc*err_effcalcsc)/(effcalcsc*effcalcsc) +
                                0.25 * (
                                    (XseeCalc.at(i-1)*XseeCalc.at(i-1)*err_effcalcee*err_effcalcee +
                                    XsggCalc.at(i-1)*XsggCalc.at(i-1)*err_effcalcgg*err_effcalcgg) /
                                    ((XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg) *
                                    (XseeCalc.at(i-1)*effcalcee + XsggCalc.at(i-1)*effcalcgg)))));
                       
    }

    // Customize graph
    TGraphAttribute(gAcc_babaee, "gAcc_babaee", "#sqrt{s} [MeV]", "#epsilon_{Bhabha}", 20, kTeal-7);
    TGraphAttribute(gAcc_babagg, "gAcc_babagg", "#sqrt{s} [MeV]", "#epsilon_{#gamma#gamma}", 20, kTeal-7);
    TGraphAttribute(gAcc_calcee, "gAcc_calcee", "#sqrt{s} [MeV]", "#epsilon_{Bhabha}", 20, kMagenta+1);
    TGraphAttribute(gAcc_calcgg, "gAcc_calcgg", "#sqrt{s} [MeV]", "#epsilon_{#gamma#gamma}", 20, kMagenta+1);
    TGraphAttribute(gAcc_calcsc, "gAcc_calcsc", "#sqrt{s} [MeV]", "#epsilon_{sig}", 20, kMagenta+1);
    TGraphAttribute(gdiffErrEff_babaee, "gdiffErrEff_babaee", "#sqrt{s} [MeV]", "abs(#Delta Err)/#Sigma Err_{TEff}", 20, kTeal-7);
    TGraphAttribute(gdiffErrEff_babagg, "gdiffErrEff_babagg", "#sqrt{s} [MeV]", "abs(#Delta Err)/#Sigma Err_{TEff}", 20, kTeal-7);
    TGraphAttribute(gdiffErrEff_calcee, "gdiffErrEff_calcee", "#sqrt{s} [MeV]", "abs(#Delta Err)/#Sigma Err_{TEff}", 20, kMagenta+1);
    TGraphAttribute(gdiffErrEff_calcgg, "gdiffErrEff_calcgg", "#sqrt{s} [MeV]", "abs(#Delta Err)/#Sigma Err_{TEff}", 20, kMagenta+1);
    TGraphAttribute(gdiffErrEff_calcsc, "gdiffErrEff_calcsc", "#sqrt{s} [MeV]", "abs(#Delta Err)/#Sigma Err_{TEff}", 20, kMagenta+1);
    TGraphAttribute(gBabaFull_CalcHep_ee, "ee_BabayagaFull_CalcHep", "#sqrt{s} [MeV]", "#sigma_{BabaYaga}/#sigma_{CalcHep}", 20, kBlue+2);
    TGraphAttribute(gBabaTree_CalcHep_ee, "ee_BabayagaTree_CalcHep", "#sqrt{s} [MeV]", "#sigma_{BabaYaga}/#sigma_{CalcHep}", 20, kBlue+2);
    TGraphAttribute(gBabaFull_CalcHep_Eff_ee, "ee_BabayagaFull_CalcHepEffi", "#sqrt{s} [MeV]", "#sigma_{BabaYaga}/#sigma_{CalcHep}", 20, kBlue+2);
    TGraphAttribute(gBabaFull_CalcHep_gg, "gg_BabayagaFull_CalcHep", "#sqrt{s} [MeV]", "#sigma_{BabaYaga}/#sigma_{CalcHep}", 21, kRed+2);
    TGraphAttribute(gBabaTree_CalcHep_gg, "gg_BabayagaTree_CalcHep", "#sqrt{s} [MeV]", "#sigma_{BabaYaga}/#sigma_{CalcHep}", 21, kRed+2);
    TGraphAttribute(gBabaFull_CalcHep_Eff_gg, "gg_BabayagaFull_CalcHepEffi", "#sqrt{s} [MeV]", "#sigma_{BabaYaga}/#sigma_{CalcHep}", 21, kRed+2);
    TGraphAttribute(gBabaFull_CalcHep_Eff_SM, "FullSM_BabayagaFull_CalcHepEffi", "#sqrt{s} [MeV]", "#sigma_{BabaYaga}/#sigma_{CalcHep}", 21, kRed+2);
    TGraphAttribute(gXsBabaFullVSsm1_ee, "ee_XS_BabayagaVSs", "1/s [MeV^{-2}]", "#sigma [pb]", 20, kTeal-7);
    TGraphAttribute(gXsCalcVSsm1_ee, "ee_XS_CalchepVSs", "1/s [MeV^{-2}]", "#sigma [pb]", 20, kMagenta+1);
    TGraphAttribute(gXsBabaFullVSsm1_gg, "gg_XS_BabayagaVSs", "1/s [MeV^{-2}]", "#sigma [pb]", 20, kTeal-7);
    TGraphAttribute(gXsCalcVSsm1_gg, "gg_XS_CalchepVSs", "1/s [MeV^{-2}]", "#sigma [pb]", 20, kMagenta+1);
    TGraphAttribute(gXsEvBabaFullVSsm1_ee, "ee_XS_BabayagaVSs_Effi", "1/s [MeV^{-2}]", "#sigma#timesN_{ev} [pb]", 20, kTeal-7);
    TGraphAttribute(gXsEvCalcVSsm1_ee, "ee_XS_CalchepVSs_Effi", "1/s [MeV^{-2}]", "#sigma#timesN_{ev} [pb]", 20, kMagenta+1);
    TGraphAttribute(gXsEvBabaFullVSsm1_gg, "gg_XS_BabayagaVSs_Effi", "1/s [MeV^{-2}]", "#sigma#timesN_{ev} [pb]", 20, kTeal-7);
    TGraphAttribute(gXsEvCalcVSsm1_gg, "gg_XS_CalchepVSs_Effi", "1/s [MeV^{-2}]", "#sigma#timesN_{ev} [pb]", 20, kMagenta+1);
    TGraphAttribute(geeYieldbaba, "BkgYieldbaba_ee", "#sqrt{s} [MeV]", "Bhabha Yield", 20, kTeal-7);
    TGraphAttribute(geeYieldcalchep, "BkgYieldcalchep_ee", "#sqrt{s} [MeV]", "Bhabha Yield", 20, kMagenta+1);
    TGraphAttribute(gggYieldbaba, "BkgYieldbaba_gg", "#sqrt{s} [MeV]", "#gamma#gamma Yield", 20, kTeal-7);
    TGraphAttribute(gggYieldcalchep, "BkgYieldcalchep_gg", "#sqrt{s} [MeV]", "#gamma#gamma Yield", 20, kMagenta+1);
    TGraphAttribute(gscYieldcalchep, "SChannelEfficiency", "#sqrt{s} [MeV]", "SChannel Yield", 20, kMagenta+1);
    TGraphAttribute(gbkgYieldbaba, "BkgYieldbaba", "#sqrt{s} [MeV]", "B Yield", 20, kTeal-7);
    TGraphAttribute(gbkgYieldcalchep, "BkgYieldcalchep", "#sqrt{s} [MeV]", "B Yield", 20, kMagenta+1);
    TGraphAttribute(gSignalEffOverBkg, "gSignalEffOverBkg", "#sqrt{s} [MeV]", "#epsilon_{sig}/#sqrt{B} a.u.", 20, kBlue);
    TGraphAttribute(gSignalEffOverBkg_calchep, "gSignalEffOverBkg_calchepOnly", "#sqrt{s} [MeV]", "#epsilon_{sig}/#sqrt{B} a.u.", 20, kGreen);


    TFile *OutFile = TFile::Open(out_root.c_str(), "RECREATE");
    Require(OutFile && !OutFile->IsZombie(), "Cannot create output ROOT: " + out_root);

    OutFile->cd();

    htotbabaee->Write();
    htotbabagg->Write();
    htotcalcee->Write();
    htotcalcgg->Write();
    htotcalcsc->Write();
    hNAcc_babaee->Write();
    hNAcc_babagg->Write();
    hNAcc_calcee->Write();
    hNAcc_calcgg->Write();
    hNAcc_calcsc->Write();
    pEff_babaee->Write();
    pEff_babagg->Write();
    pEff_calcee->Write();
    pEff_calcgg->Write();
    pEff_calcsc->Write();
    gAcc_babaee->Write();
    gAcc_babagg->Write();
    gAcc_calcee->Write();
    gAcc_calcgg->Write();
    gAcc_calcsc->Write();
    gdiffErrEff_babaee->Write();
    gdiffErrEff_babagg->Write();
    gdiffErrEff_calcee->Write();
    gdiffErrEff_calcgg->Write();
    gdiffErrEff_calcsc->Write();
    gBabaTree_CalcHep_ee->Write();
    gBabaFull_CalcHep_ee->Write();
    gBabaFull_CalcHep_Eff_ee->Write();
    gBabaTree_CalcHep_gg->Write();
    gBabaFull_CalcHep_gg->Write();
    gBabaFull_CalcHep_Eff_gg->Write();
    gBabaFull_CalcHep_Eff_SM->Write();
    gXsBabaFullVSsm1_ee->Write();
    gXsCalcVSsm1_ee->Write();
    gXsBabaFullVSsm1_gg->Write();
    gXsCalcVSsm1_gg->Write();
    gXsEvBabaFullVSsm1_ee->Write();
    gXsEvCalcVSsm1_ee->Write();
    gXsEvBabaFullVSsm1_gg->Write();
    gXsEvCalcVSsm1_gg->Write();
    geeYieldbaba->Write();
    geeYieldcalchep->Write();
    gggYieldbaba->Write();
    gggYieldcalchep->Write();
    gscYieldcalchep->Write();
    gbkgYieldbaba->Write();
    gbkgYieldcalchep->Write();
    gSignalEffOverBkg->Write();
    gSignalEffOverBkg_calchep->Write();

    OutFile->Close();

    // Close inputs
    BabaeeRootFile->Close();
    BabaggRootFile->Close();
    CalceeRootFile->Close();
    CalcggRootFile->Close();
    CalcscRootFile->Close();

    cout << "[INFO] XSAnalyzer finished successfully." << endl;
}

int main(int argc, char** argv) {
    if (argc != 7) {
        cerr
          << "Usage:\n  " << argv[0]
          << "  <baba_ee> <baba_gg> <calc_ee> <calc_gg> <calc_sig>\n"
          << "  <out_root>\n";
        return 1;
    }

    // XSAnalyzer(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

    try { XSAnalyzer(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
    } catch (const exception& e) {
        cerr << "[FATAL] " << e.what() << "\n";
        return 2;
    }

    return 0;
}