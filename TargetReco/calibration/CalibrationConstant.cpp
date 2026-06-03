// ============================================================
// BuildFinalTargetCalibration.cpp
//
// Reads multiple calib_runXXXXX.txt files and produces a final
// calibration table (32 rows) using ONLY:
//   - QCh1Wf
//   - QCh1WfCNCorr
//   - QChCum
// for the FIRED CHANNEL defined by your run->channel mapping
//
// Output format:
// Channel  C_QCh1Wf  C_QCh1WfCNCorr  C_QChCum  Entries
// ============================================================

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

struct CalibData {
    double sum1 = 0;  // COld
    double sum2 = 0;  // C1WfFixed
    double sum3 = 0;  // C1WfFixedCMN
    double sum4 = 0;  // C1WfSliding
    double sum5 = 0;  // C1WfSlidingCMN
    double sum6 = 0;  // CCumFixed
    double sum7 = 0;  // CCumFixedCMN
    double sum8 = 0;  // CCumSliding
    double sum9 = 0;  // CCumSlidingCMN
    int count = 0;
};

// RUN -> CHANNEL MAP =================
map<int, vector<int>> runToChannel = {
    {80166, {4}},
    {80167, {11}},
    {80168, {5}},
    {80169, {10}},
    {80170, {6}},
    {80171, {9}},
    {80173, {7}},
    {80174, {8}},
    {80175, {12}},
    {80176, {3}},
    {80177, {13}},
    {80178, {2}},
    {80179, {14}},
    {80180, {1}},
    {80181, {15}}
};

int main() {

    map<int, CalibData> finalCalib;

    // ================= LOOP OVER RUN MAP =================
    for (auto &entry : runToChannel) {

        int run = entry.first;
        vector<int> firedChannels = entry.second;

        string filename = "/home/mancinima/LeadGlassCalib2024/padme-fw/TargetRun4/calibration/txt_calib/calib_Reco_run_00" + to_string(run) + ".txt";
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Cannot open " << filename << endl;
            continue;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            stringstream ss(line);
            int ch;
            ss >> ch;

            // Table format:
            // ch | Q1 C1 | Q2 C2 | Q3 C3 | ... | Q9 C9
            // We want :
            // Algo 1 -> QChOld        -> (Q1 C1)
            // Algo 2 -> QCh1WfFixed  -> (Q2 C2)
            // ...

            double Q, C;
            double C1 = 0, C2 = 0, C3 = 0, C4 = 0, C5 = 0, C6 = 0, C7 = 0, C8 = 0, C9 = 0;

            for(int algo = 1; algo <= 9; algo++) {
                ss >> Q >> C;
              
                if (algo == 1) C1 = C; // COld
                if (algo == 2) C2 = C; // C1WfSliding
                if (algo == 3) C3 = C; // C1WfSlidingCMN
                if (algo == 4) C4 = C; // C1WfFixed
                if (algo == 5) C5 = C; // C1WfFixedCMN
                if (algo == 6) C6 = C; // CCumSliding
                if (algo == 7) C7 = C; // CCumSlidingCNCorr
                if (algo == 8) C8 = C; // CCumFixed
                if (algo == 9) C9 = C; // CCumFixedCnCorr
            }

            // Use only fired channels for this run
            for (int firedCh : firedChannels) {
                if (ch == firedCh) {
                    finalCalib[ch].sum1 += C1;
                    finalCalib[ch].sum2 += C2;
                    finalCalib[ch].sum3 += C3;
                    finalCalib[ch].sum4 += C4;
                    finalCalib[ch].sum5 += C5;
                    finalCalib[ch].sum6 += C6;
                    finalCalib[ch].sum7 += C7;
                    finalCalib[ch].sum8 += C8;
                    finalCalib[ch].sum9 += C9;
                    finalCalib[ch].count++;
                }
            }
        }
        file.close();
    }

    // ================= OUTPUT FINAL FILE =================
    ofstream out("final_target_calibration.txt");
    out << "# Ch COld C1WfSliding C1WfSlidingCMN C1WfFixed  C1WfFixedCMN CCumSliding CCumSlidingCMN CCumFixed CCumFixedCMN Entries" << endl;

    for(int ch = 0; ch < 32; ch++) {
        if (finalCalib[ch].count > 0) {
            CalibData &c = finalCalib[ch];
            out << ch << " "
                << c.sum1 / c.count << " "
                << c.sum2 / c.count << " "
                << c.sum3 / c.count << " "
                << c.sum4 / c.count << " "
                << c.sum5 / c.count << " "
                << c.sum6 / c.count << " "
                << c.sum7 / c.count << " "
                << c.sum8 / c.count << " "
                << c.sum9 / c.count << " "
                << c.count << endl;
        } else {
            out << ch << " 0 0 0 0 0 0 0 0 0" << endl;
        }
    }

    out.close();

    cout << "============================================\n";
    cout << "✅ Final calibration created: final_target_calibration.txt\n";
    cout << "============================================\n";

    return 0;
}
