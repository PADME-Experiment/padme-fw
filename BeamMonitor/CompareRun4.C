// Compare LG, Target and TMM trends for every run listed in the Run4 database.
// Run with: root -l -b -q 'CompareRun4.C'

#include <TFile.h>
#include <TDirectory.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TF1.h>
#include <TFitResultPtr.h>
#include <TString.h>
#include <TColor.h>
#include <TSystem.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace {

// ============================= Configuration ==============================
const char* RUN_LIST = "/home/mancinima/BeamMonitorRun4/padme-fw/UserAnalysis/config/run_level_offline_db_2025_full_withTMM.txt";
const char* TARGET_LG_PATTERN = "/data9Vd1/padme/mancinima/RecoRun4_v0/TargetLG/RecoTarget_v0_%07d.root";
const char* TMM_PATTERN       = "/data9Vd1/padme/mancinima/RecoRun4_v0/TMM/RecoTMM_v0_%07d.root";
const char* OUTPUT_FILE       = "ComparisonRun4_v0.root";

// Absolute charge trends (UNIX time on X).
const char* LG_CHARGE_PATH     = "trends_time/gQLG_time";
const char* TARGET_CHARGE_PATH = "trends_time/gQXTot_target_cal_time";
const char* TMM_CHARGE_PATH    = "TimeTrends/Calib_external/Direct/X/g_ChargePerEvent";

// Match the old code's time-bin ratio criterion exactly.
constexpr double MIN_OVERLAP_FRACTION = 0.70;
constexpr double CENTER_TOL_SECONDS = 1.0;

// Fit pol0; fall back to arithmetic mean if disagreement is >2 combined SEMs.
constexpr double N_SIGMA_MAX = 2.0;
// ==========================================================================

struct Point { double x, y, ex, ey; };
using Points = map<double, Point>;

vector<int> readRunIDs(const char* filename) {
    ifstream in(filename);
    if (!in) throw runtime_error(string("Cannot open run list: ") + filename);

    vector<int> runs;
    set<int> seen;
    string line;
    while (getline(in, line)) {
        istringstream iss(line);
        string field;
        if (!(iss >> field)) continue;
        // Only read the first column; automatically skip the header, comments,
        // and any continuation lines that do not begin with a valid RunID.
        try {
            size_t consumed = 0;
            const int id = stoi(field, &consumed);
            if (consumed != field.size() || id <= 0) continue;
            if (seen.insert(id).second) runs.push_back(id);
        } catch (const exception&) {
            continue;
        }
    }
    if (runs.empty()) throw runtime_error("No valid RunID found in run list");
    return runs;
}

TGraph* findGraph(TFile* file, initializer_list<const char*> paths, const char* label) {
    for (const char* path : paths) {
        auto* g = dynamic_cast<TGraph*>(file->Get(path));
        if (g) return g;
    }
    cerr << "  WARNING: missing " << label << " in " << file->GetName() << '\n';
    return nullptr;
}

// Extract symmetric uncertainties from TGraphErrors / TGraphAsymmErrors.
Points readPoints(const TGraph* g) {
    Points result;
    if (!g) return result;
    auto* ge = dynamic_cast<const TGraphErrors*>(g);
    auto* ga = dynamic_cast<const TGraphAsymmErrors*>(g);
    for (int i = 0; i < g->GetN(); ++i) {
        Point p{g->GetPointX(i), g->GetPointY(i), 0., 0.};
        if (ga) {
            p.ex = (ga->GetErrorXlow(i) + ga->GetErrorXhigh(i)) / 2.;
            p.ey = (ga->GetErrorYlow(i) + ga->GetErrorYhigh(i)) / 2.;
        } else if (ge) {
            p.ex = ge->GetErrorX(i);
            p.ey = ge->GetErrorY(i);
        }
        if (!isfinite(p.x) || !isfinite(p.y) || !isfinite(p.ex) ||
            !isfinite(p.ey) || p.ex < 0. || p.ey < 0.) continue;
        if (!result.emplace(p.x, p).second) {
            cerr << "  WARNING: repeated UNIX timestamp " << setprecision(15) << p.x << " in " << g->GetName() << " (keeping first)\n" << setprecision(6);
        }
    }
    return result;
}

unique_ptr<TGraphErrors> makeGraph(const char* name, const Points& pts, const char* yTitle, int color) {
    auto g = make_unique<TGraphErrors>();
    g->SetName(name);
    g->SetTitle(Form("%s;UNIX time [s];%s", name, yTitle));
    g->SetMarkerStyle(20);
    g->SetMarkerColor(color);
    g->SetLineColor(color);
    for (const auto& kv : pts) {
        const Point& p = kv.second;
        const int i = g->GetN();
        g->SetPoint(i, p.x, p.y);
        g->SetPointError(i, p.ex, p.ey);
    }
    return g;
}

// Same pol0 + 2-sigma mean fallback used by the previous macro.
double fitNormalization(const char* label, const Points& pts) {
    if (pts.size() < 2) throw runtime_error("Not enough points for pol0");
    auto raw = makeGraph(Form("raw_%s", label), pts, "Raw", kBlack);

    double sum = 0., sumSq = 0.;
    int nZeroErrors = 0;
    for (const auto& kv : pts) {
        sum += kv.second.y;
        if (kv.second.ey == 0.) ++nZeroErrors;
    }
    const double n = static_cast<double>(pts.size());
    const double mean = sum / n;
    if (!isfinite(mean) || mean <= 0.)
        throw runtime_error(Form("Non-positive mean for %s", label));
    for (const auto& kv : pts) {
        const double d = kv.second.y - mean;
        sumSq += d * d;
    }
    const double sem = sqrt(max(0., sumSq / (n * (n - 1.))));

    TF1 fit(Form("pol0_%s", label), "pol0", pts.begin()->first - 1., pts.rbegin()->first + 1.);
    const TFitResultPtr result = raw->Fit(&fit, "Q0RNS");
    const int status = static_cast<int>(result);
    const double p0 = fit.GetParameter(0);
    const double dp0 = fit.GetParError(0);
    const bool good = status == 0 && isfinite(p0) && p0 > 0. && isfinite(dp0) && dp0 >= 0. && fit.GetNDF() > 0;
    const double discrepancy = good ? fabs(p0 - mean) : 0.;
    const double threshold = good ? N_SIGMA_MAX * hypot(sem, dp0) : 0.;
    const bool fallback = !good || discrepancy > threshold;
    const double norm = fallback ? mean : p0;

    cout << "    " << label << ": N=" << pts.size()
         << ", pol0=" << p0 << " +/- " << dp0
         << ", mean=" << mean << " +/- " << sem
         << ", chi2/ndf=" << fit.GetChisquare() << "/" << fit.GetNDF()
         << ", zero-err=" << nZeroErrors
         << ", norm=" << norm << (fallback ? " [MEAN]" : " [pol0]") << '\n';
    return norm;
}

unique_ptr<TGraphErrors> normalizedGraph(const char* name, const Points& pts, int color, const char* yTitle) {
    if (pts.size() < 2) return nullptr;
    const double norm = fitNormalization(name, pts);
    Points values;
    for (const auto& kv : pts) {
        Point p = kv.second;
        p.y /= norm;
        p.ey /= fabs(norm);
        values.emplace(p.x, p);
    }
    return makeGraph(name, values, yTitle, color);
}

// Best overlap relative to the *larger* time bin
const Point* findTimeBin(const Points& other, const Point& a) {
    const Point* best = nullptr;
    double bestFraction = 0.;
    double closestDt = numeric_limits<double>::infinity();
    for (const auto& kv : other) {
        const Point& b = kv.second;
        const double dt = fabs(a.x - b.x);
        double overlapFraction = 0.;
        if (a.ex > 0. && b.ex > 0.) {
            const double left = max(a.x - a.ex, b.x - b.ex);
            const double right = min(a.x + a.ex, b.x + b.ex);
            const double overlap = max(0., right - left);
            overlapFraction = overlap / max(2. * a.ex, 2. * b.ex);
            if (overlapFraction < MIN_OVERLAP_FRACTION) continue;
        } else {
            if (dt > CENTER_TOL_SECONDS) continue;
            overlapFraction = 1.;
        }
        if (!best || overlapFraction > bestFraction ||
            (overlapFraction == bestFraction && dt < closestDt)) {
            best = &b;
            bestFraction = overlapFraction;
            closestDt = dt;
        }
    }
    return best;
}

double ratioError(double a, double ea, double b, double eb) {
    if (b == 0.) return 0.;
    return hypot(ea / b, a * eb / (b * b));
}

unique_ptr<TGraphErrors> normalizedRatio(const char* name, const Points& numerator, const Points& denominator, int color) {
    Points ratios;
    set<double> usedBins;
    int unmatched = 0, reused = 0, zeroDenominator = 0;
    for (const auto& kv : numerator) {
        const Point& a = kv.second;
        const Point* b = findTimeBin(denominator, a);
        if (!b) { ++unmatched; continue; }
        if (b->y == 0.) { ++zeroDenominator; continue; }
        if (!usedBins.insert(b->x).second) ++reused;
        const double ratio = a.y / b->y;
        const double err = ratioError(a.y, a.ey, b->y, b->ey);
        if (!isfinite(ratio) || !isfinite(err)) continue;
        ratios.emplace(a.x, Point{a.x, ratio, a.ex, err});
    }
    cout << "    " << name << ": matched=" << ratios.size()
         << ", unmatched=" << unmatched
         << ", zero denominator=" << zeroDenominator
         << ", reused denominator bins=" << reused << '\n';
    if (reused)
        cout << "    WARNING: reused denominator bins; check time overlap.\n";
    if (ratios.size() < 2) {
        cerr << "    WARNING: insufficient matched bins; skipping " << name << '\n';
        return nullptr;
    }
    return normalizedGraph(name, ratios, color, "Normalized charge ratio");
}

// For Position and Sigma: clone the *original* TGraph, preserving its actual
// charge/position/sigma values, errors (including asymmetric errors), and X bins.
int writeUnnormalized(TDirectory* dir, TGraph* source, const char* name, const char* yTitle, int color) {
    if (!source || source->GetN() == 0) return 0;
    unique_ptr<TGraph> g(dynamic_cast<TGraph*>(source->Clone(name)));
    if (!g) return 0;
    g->SetTitle(Form("%s;UNIX time [s];%s", name, yTitle));
    g->SetMarkerColor(color);
    g->SetLineColor(color);
    g->SetMarkerStyle(20);
    dir->cd();
    g->Write();
    return 1;
}

int writeNormalized(TDirectory* dir, const char* name, const Points& pts, int color, const char* title) {
    try {
        auto g = normalizedGraph(name, pts, color, title);
        if (!g) return 0;
        dir->cd();
        g->Write();
        return 1;
    } catch (const exception& e) {
        cerr << "    WARNING: " << name << ": " << e.what() << '\n';
        return 0;
    }
}

int writeRatio(TDirectory* dir, const char* name, const Points& a, const Points& b, int color) {
    if (a.empty() || b.empty()) return 0;
    try {
        auto g = normalizedRatio(name, a, b, color);
        if (!g) return 0;
        dir->cd();
        g->Write();
        return 1;
    } catch (const exception& e) {
        cerr << "    WARNING: " << name << ": " << e.what() << '\n';
        return 0;
    }
}

struct Counts { int charge = 0, position = 0, sigma = 0; };

Counts processRun(int runId, TFile* out) {
    const TString targetName = Form(TARGET_LG_PATTERN, runId);
    const TString tmmName = Form(TMM_PATTERN, runId);

    // These files are local to the analysis machine; missing runs are skipped.
    if (gSystem->AccessPathName(targetName.Data()) ||
        gSystem->AccessPathName(tmmName.Data())) {
        cerr << "  SKIP missing inputs:" << (gSystem->AccessPathName(targetName.Data()) ? " Target/LG" : "") << (gSystem->AccessPathName(tmmName.Data()) ? " TMM" : "") << '\n';
        return {};
    }
    unique_ptr<TFile> fTarget(TFile::Open(targetName.Data(), "READ"));
    unique_ptr<TFile> fTMM(TFile::Open(tmmName.Data(), "READ"));
    if (!fTarget || fTarget->IsZombie() || !fTMM || fTMM->IsZombie()) {
        cerr << "  SKIP: failed to open ROOT input file(s)\n";
        return {};
    }

    TDirectory* runDir = out->mkdir(Form("run_%07d", runId));
    TDirectory* chargeDir = runDir->mkdir("Charge");
    TDirectory* positionDir = runDir->mkdir("Position");
    TDirectory* sigmaDir = runDir->mkdir("Sigma");
    Counts count;

    // ------------------------ Charge: normalized -------------------------
    const Points lg = readPoints(findGraph(fTarget.get(), {LG_CHARGE_PATH}, "LG charge"));
    const Points target = readPoints(findGraph(fTarget.get(), {TARGET_CHARGE_PATH}, "Target charge"));
    const Points tmm = readPoints(findGraph(fTMM.get(), {TMM_CHARGE_PATH}, "TMM charge"));

    count.charge += writeNormalized(chargeDir, "gLG_norm", lg, kBlue+2, "Q_{LG}/<Q_{LG}>");
    count.charge += writeNormalized(chargeDir, "gTargetX_norm", target, kGreen+2, "Q_{Target}/<Q_{Target}>");
    count.charge += writeNormalized(chargeDir, "gTMMX_norm", tmm, kMagenta+2, "Q_{TMM}/<Q_{TMM}>");
    count.charge += writeRatio(chargeDir, "gTMMoverLG_norm", tmm, lg, kRed+1);
    count.charge += writeRatio(chargeDir, "gTargetoverLG_norm", target, lg, kOrange+7);
    count.charge += writeRatio(chargeDir, "gTargetoverTMM_norm", target, tmm, kCyan+2);

    // ---------------------- Position: original Y -------------------------
    // The user's first paths are under trends_period; TargetAnalysis writes
    // *_time into trends_time. Try both, prioritizing the requested path.
    count.position += writeUnnormalized(positionDir,
        findGraph(fTarget.get(), {"trends_period/gXTar_wmean_time",
                                   "trends_time/gXTar_wmean_time"}, "Target X position"),
        "gTargetX_position", "Target X position [mm]", kGreen+2);
    count.position += writeUnnormalized(positionDir,
        findGraph(fTarget.get(), {"trends_period/gYTar_wmean_raw_time",
                                   "trends_time/gYTar_wmean_raw_time"}, "Target Y position"),
        "gTargetY_position", "Target Y position [mm]", kGreen+3);
    count.position += writeUnnormalized(positionDir,
        findGraph(fTMM.get(), {"TimeTrends/Calib_external/Direct/X/g_PositionMean"}, "TMM X position"),
        "gTMMX_position", "TMM X position [mm]", kMagenta+2);
    count.position += writeUnnormalized(positionDir,
        findGraph(fTMM.get(), {"TimeTrends/Calib_external/Direct/Y/g_PositionMean"}, "TMM Y position"),
        "gTMMY_position", "TMM Y position [mm]", kMagenta+3);

    // ------------------------- Sigma: original Y -------------------------
    count.sigma += writeUnnormalized(sigmaDir,
        findGraph(fTarget.get(), {"trends_period/gsigmaXtar_time",
                                   "trends_time/gsigmaXtar_time"}, "Target X sigma"),
        "gTargetX_sigma", "Target X sigma [mm]", kGreen+2);
    count.sigma += writeUnnormalized(sigmaDir,
        findGraph(fTarget.get(), {"trends_period/gsigmaYtar_raw_time",
                                   "trends_time/gsigmaYtar_raw_time"}, "Target Y sigma"),
        "gTargetY_sigma", "Target Y sigma [mm]", kGreen+3);
    count.sigma += writeUnnormalized(sigmaDir,
        findGraph(fTMM.get(), {"TimeTrends/Calib_external/Direct/X/g_SigmaRMS"}, "TMM X sigma"),
        "gTMMX_sigma", "TMM X sigma [mm]", kMagenta+2);
    count.sigma += writeUnnormalized(sigmaDir,
        findGraph(fTMM.get(), {"TimeTrends/Calib_external/Direct/Y/g_SigmaRMS"}, "TMM Y sigma"),
        "gTMMY_sigma", "TMM Y sigma [mm]", kMagenta+3);

    cout << "  Saved: Charge=" << count.charge << "/6, Position=" << count.position << "/4, Sigma=" << count.sigma << "/4\n";
    return count;
}

} // namespace

void CompareRun4() {
    try {
        const vector<int> runs = readRunIDs(RUN_LIST);
        cout << "RunIDs to process: " << runs.size() << '\n';
        unique_ptr<TFile> out(TFile::Open(OUTPUT_FILE, "RECREATE"));
        if (!out || out->IsZombie())
            throw runtime_error(string("Cannot create output file: ") + OUTPUT_FILE);

        int completed = 0, skipped = 0;
        Counts total;
        for (int runId : runs) {
            cout << "\n========== Run " << runId << " ==========" << '\n';
            try {
                Counts c = processRun(runId, out.get());
                if (c.charge + c.position + c.sigma == 0) ++skipped;
                else ++completed;
                total.charge += c.charge;
                total.position += c.position;
                total.sigma += c.sigma;
            } catch (const exception& e) {
                cerr << "  ERROR run " << runId << ": " << e.what() << " (continuing with next run)\n";
                ++skipped;
            }
        }
        out->Close();
        cout << "\nOUTPUT: " << OUTPUT_FILE
             << "\nRuns with graphs: " << completed
             << ", skipped: " << skipped
             << "\nGraphs: Charge=" << total.charge
             << ", Position=" << total.position
             << ", Sigma=" << total.sigma << '\n';
    } catch (const exception& e) {
        cerr << "ERROR: " << e.what() << '\n';
    }
}
