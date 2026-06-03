// ============================================================
// TargetCalib.cpp  - Multi-algorithm calibration
// MODIFIED: txt output now has 32 rows x 19 columns:
// ch | (Q, C)_algo1 | (Q, C)_algo2 | ... | (Q, C)_algo9
// ============================================================

#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TSystem.h>
#include <TString.h>

#include <iostream>
#include <cstdio>

using namespace std;

void TargetCalib(const char* filename)
{
    // ================= OPEN FILE =================
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        cerr << "ERROR: Cannot open file " << filename << endl;
        return;
    }

    TTree *tree = (TTree*)f->Get("NTU");
    if (!tree) {
        cerr << "ERROR: Tree NTU not found" << endl;
        f->Close();
        return;
    }

    // ================= BRANCH VARIABLES =================
    Int_t Nevent = 0;
    Int_t TrigMask = 0;

    Double_t QChOld[2][32];
    Double_t QCh1WfSliding[2][32];
    Double_t QCh1WfSlidingCNCorr[2][32];
    Double_t QCh1WfFixed[2][32];
    Double_t QCh1WfFixedCNCorr[2][32];
    Double_t QChCumSliding[2][32];
    Double_t QChCumSlidingCNCorr[2][32];
    Double_t QChCumFixed[2][32];
    Double_t QChCumFixedCNCorr[2][32];

    if (tree->GetBranch("Nevent"))                         tree->SetBranchAddress("Nevent", &Nevent);
    if (tree->GetBranch("TrigMask"))                       tree->SetBranchAddress("TrigMask", &TrigMask);

    if (tree->GetBranch("QChOld"))                         tree->SetBranchAddress("QChOld", QChOld);
    if (tree->GetBranch("QCh1WfSliding"))                  tree->SetBranchAddress("QCh1WfSliding", QCh1WfSliding);
    if (tree->GetBranch("QCh1WfSlidingCNCorr"))            tree->SetBranchAddress("QCh1WfSlidingCNCorr", QCh1WfSlidingCNCorr);
    if (tree->GetBranch("QCh1WfFixed"))                    tree->SetBranchAddress("QCh1WfFixed", QCh1WfFixed);
    if (tree->GetBranch("QCh1WfFixedCNCorr"))              tree->SetBranchAddress("QCh1WfFixedCNCorr", QCh1WfFixedCNCorr);
    if (tree->GetBranch("QChCumSliding"))                  tree->SetBranchAddress("QChCumSliding", QChCumSliding);
    if (tree->GetBranch("QChCumSlidingCNCorr"))            tree->SetBranchAddress("QChCumSlidingCNCorr", QChCumSlidingCNCorr);
    if (tree->GetBranch("QChCumFixed"))                    tree->SetBranchAddress("QChCumFixed", QChCumFixed);
    if (tree->GetBranch("QChCumFixedCNCorr"))               tree->SetBranchAddress("QChCumFixedCNCorr", QChCumFixedCNCorr);

    Long64_t nEntries = tree->GetEntries();
    cout << "Processing " << nEntries << " events..." << endl;

    // ================= STORAGE =================
    double Qref = 0.0;               // board0 ch31
    double QsumTar[9][32] = {{0.0}};

    // ================= EVENT LOOP =================
    for (Long64_t ev = 0; ev < nEntries; ev++){
        tree->GetEntry(ev);

        Qref += QChOld[0][31];

        for(int ch = 0; ch < 32; ch++){
            if (QChOld[1][ch] > 0)              QsumTar[0][ch] += QChOld[1][ch];
            if (QCh1WfSliding[1][ch] > 0)       QsumTar[1][ch] += QCh1WfSliding[1][ch];
            if (QCh1WfSlidingCNCorr[1][ch] > 0) QsumTar[2][ch] += QCh1WfSlidingCNCorr[1][ch];
            if (QCh1WfFixed[1][ch] > 0)         QsumTar[3][ch] += QCh1WfFixed[1][ch];
            if (QCh1WfFixedCNCorr[1][ch] > 0)   QsumTar[4][ch] += QCh1WfFixedCNCorr[1][ch];
            if (QChCumSliding[1][ch] > 0)       QsumTar[5][ch] += QChCumSliding[1][ch];
            if (QChCumSlidingCNCorr[1][ch] > 0) QsumTar[6][ch] += QChCumSlidingCNCorr[1][ch];
            if (QChCumFixed[1][ch] > 0)         QsumTar[7][ch] += QChCumFixed[1][ch];
            if (QChCumFixedCNCorr[1][ch] > 0)   QsumTar[8][ch] += QChCumFixedCNCorr[1][ch];
        }
    }

    // ================= OUTPUT FILE NAMES =================
    TString inName = gSystem->BaseName(filename);
    TString base = inName;
    if (base.EndsWith(".root")) base.ReplaceAll(".root", "");
    TString outRootName = Form("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/calibration/fileroot_calib/calib_%s.root", base.Data());
    TString outTxtName  = Form("/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/calibration/txt_calib/calib_%s.txt",  base.Data());

    const char* AlgoNames[9] = {
        "Old",
        "1WfSliding",
        "1WfSlidingCMN",
        "1WfFixed",
        "1WfFixedCMN",
        "CumSliding",
        "CumSlidingCMN",
        "CumFixed",
        "CumFixedCMN"
    };

    // ================= ROOT OUTPUT =================
    TFile *out = new TFile(outRootName.Data(), "RECREATE");
    TGraphErrors *gCalib[9];

    // ================= TXT OUTPUT =================
    FILE *txt = fopen(outTxtName.Data(), "w");
    if (!txt) {
        cerr << "ERROR: cannot open output text file" << endl;
        out->Close();
        f->Close();
        return;
    }

    // Header
    fprintf(txt, "# Channel ");
    for(int algo = 0; algo < 9; algo++){
        fprintf(txt, "%s_Q %s_C ", AlgoNames[algo], AlgoNames[algo]);
    }
    fprintf(txt, "\n");

    // ================= WRITE TABLE =================
    for(int ch = 0; ch < 32; ch++){

        fprintf(txt, "%2d ", ch);

        for(int algo = 0; algo < 9; algo++){
            double Q = QsumTar[algo][ch];
            double C = (Qref != 0) ? Q / Qref : 0.0;

            fprintf(txt, "%.6e %.6e ", Q, C);
        }
        fprintf(txt, "\n");
    }

    // ================= ALSO WRITE ROOT GRAPHS =================
    for(int algo = 0; algo < 9; algo++){

        gCalib[algo] = new TGraphErrors(32);
        gCalib[algo]->SetName(Form("gCalib_%s", AlgoNames[algo]));
        gCalib[algo]->SetTitle(Form("Calibration %s;Channel;C", AlgoNames[algo]));

        for(int ch = 0; ch < 32; ch++){
            double C = (Qref != 0) ? QsumTar[algo][ch] / Qref : 0.0;
            gCalib[algo]->SetPoint(ch, ch, C);
            gCalib[algo]->SetPointError(ch, 0, 0);
        }
        gCalib[algo]->Write();
    }

    fclose(txt);
    out->Close();
    f->Close();

    cout << "=============================================" << endl;
    cout << "✅ Calibration completed" << endl;
    cout << "TXT table format: 32 rows x 19 columns" << endl;
    cout << "Outputs:" << endl;
    cout << " - " << outRootName.Data() << endl;
    cout << " - " << outTxtName.Data() << endl;
    cout << "=============================================" << endl;
}
