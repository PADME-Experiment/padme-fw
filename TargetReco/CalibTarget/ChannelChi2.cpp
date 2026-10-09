
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <vector>

#include "TDirectory.h"
#include "TFile.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TMath.h"
#include "TMinuit.h"
#include "TString.h"
#include "TTree.h"

using namespace std;

namespace {

// =============================================================================
// Configuration
// =============================================================================

constexpr int kNChannels = 32;
constexpr int kNStripsPerView = 16;
constexpr int kEventsPerBlock = 100;
constexpr int kBlocksPerPeriod = 100;

constexpr double kStripWidthMm = 0.850;
constexpr double kStripPitchMm = 1.0;

// Expected beam-centre strip in the local X/Y numbering.
// X physical strips are 0..15.
// Y physical strips are 16..31, so local Y = physicalY - 16.
constexpr int kExpectedXLocal = 8;
constexpr int kExpectedYLocal = 9;

constexpr double kCalibrationEnergyMeV = 298.5;

// ADC <-> physical-strip mapping inherited from the old analysis.
constexpr array<int, kNChannels> kADCToPhysics = {15, 13, 11,  9,  7,  5,  3,  1, 0,  2,  4,  6,  8, 10, 12, 14, 22, 20, 18, 16, 17, 19, 21, 23, 25, 27, 29, 31, 30, 28, 26, 24 };

constexpr array<int, kNChannels> kPhysicsToADC = {8,  7,  9,  6, 10,  5, 11,  4, 12,  3, 13,  2, 14,  1, 15,  0, 19, 20, 18, 21, 17, 22, 16, 23, 31, 24, 30, 25, 29, 26, 28, 27 };

array<double, kNStripsPerView> gXCalib{};
array<double, kNStripsPerView> gXCalibErr{};

// =============================================================================
// Data structures
// =============================================================================

struct GoodnessOfFit {
    double chi2 = numeric_limits<double>::quiet_NaN();
    double edm = numeric_limits<double>::quiet_NaN();
    double errdef = numeric_limits<double>::quiet_NaN();
    int nvpar = 0;
    int nparx = 0;

    // TMinuit::mnstat ISTAT:
    // 0 = covariance not calculated
    // 1 = approximate
    // 2 = full matrix, forced positive definite
    // 3 = full accurate covariance matrix
    int covStatus = 0;

    // Return value of TMinuit::Migrad().
    // 0 means MIGRAD completed successfully.
    int migradStatus = -999;
};

struct FitResult {
    array<double, 3> par{
        numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN() };

    array<double, 3> err{
        numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN() };

    GoodnessOfFit gof;
    bool usable = false;
};

struct BlockData {
    int run = -1;
    Long64_t blockId = -1;
    int nEvents = 0;

    double firstTimeUnix = 0.0;
    double meanTimeUnix = 0.0;
    double lastTimeUnix = 0.0;

    array<double, kNChannels> targetQ{};
    double lgQMean = 0.0;
};

struct PeriodResult {
    int periodId = -1;
    Long64_t firstBlockId = -1;
    Long64_t lastBlockId = -1;

    double firstTimeUnix = 0.0;
    double meanTimeUnix = 0.0;
    double lastTimeUnix = 0.0;
    double timeHalfWidth = 0.0;

    double qLG = 0.0;
    double dqLG = 0.0;

    array<double, 3> qXRaw{};
    array<double, 3> dqXRaw{};

    array<double, 3> qYRaw{};
    array<double, 3> dqYRaw{};

    array<double, 3> qXCal{};
    array<double, 3> dqXCal{};

    double qXTotRaw = 0.0;
    double dqXTotRaw = 0.0;

    double qYTotRaw = 0.0;
    double dqYTotRaw = 0.0;

    double qXTotCal = 0.0;
    double dqXTotCal = 0.0;

    double qXRatioRaw = 0.0;
    double dqXRatioRaw = 0.0;

    double qYRatioRaw = 0.0;
    double dqYRatioRaw = 0.0;

    double qXRatioCal = 0.0;
    double dqXRatioCal = 0.0;

    double muXWRaw = 0.0;
    double dMuXWRaw = 0.0;

    double muYWRaw = 0.0;
    double dMuYWRaw = 0.0;

    double muXWCal = 0.0;
    double dMuXWCal = 0.0;

    FitResult fitXRaw;
    FitResult fitYRaw;
    FitResult fitXCal;
};

struct RunSummary {
    double qLG = 0.0;
    double dqLG = 0.0;

    array<double, 3> qXRaw{};
    array<double, 3> dqXRaw{};

    array<double, 3> qYRaw{};
    array<double, 3> dqYRaw{};

    array<double, 3> qXCal{};
    array<double, 3> dqXCal{};

    double muXWRaw = 0.0;
    double dMuXWRaw = 0.0;

    double muYWRaw = 0.0;
    double dMuYWRaw = 0.0;

    double muXWCal = 0.0;
    double dMuXWCal = 0.0;

    FitResult fitXRaw;
    FitResult fitYRaw;
    FitResult fitXCal;
};

// =============================================================================
// Statistics / propagation helpers
// =============================================================================

void MeanAndSem(const vector<double>& values, double& mean, double& sem) {
    if (values.empty()) {
        mean = 0.0;
        sem = 0.0;
        return;
    }

    const double n = static_cast<double>(values.size());
    mean = accumulate(values.begin(), values.end(), 0.0) / n;

    if (values.size() == 1) {
        sem = 0.0;
        return;
    }

    double sumSq = 0.0;
    for (const double x : values) {
        const double dx = x - mean;
        sumSq += dx * dx;
    }

    const double sampleVariance = sumSq / static_cast<double>(values.size() - 1);
    sem = sqrt(max(0.0, sampleVariance)) / sqrt(n);
}

double Quadrature3(const array<double, 3>& values) {
    return sqrt( values[0] * values[0] + values[1] * values[1] + values[2] * values[2] );
}

double SafeRatio(double numerator, double denominator) {
    if (!isfinite(numerator) || !isfinite(denominator) || denominator == 0.0) {
        return 0.0;
    }
    return numerator / denominator;
}

double RatioError( double numerator, double numeratorError, double denominator, double denominatorError) {
    if (!isfinite(numerator) || !isfinite(numeratorError) || !isfinite(denominator) || !isfinite(denominatorError) || denominator == 0.0) {
        return 0.0;
    }

    const double termNum = numeratorError / denominator;
    const double termDen = numerator * denominatorError / (denominator * denominator);
    return hypot(termNum, termDen);
}

bool WeightedMeanMu3Strips( const array<double, 3>& q, const array<double, 3>& dq, double& mu, double& dmu) {
    const double qm1 = q[0];
    const double q0  = q[1];
    const double qp1 = q[2];

    const double denominator = qm1 + q0 + qp1;

    if (denominator <= 0.0) {
        mu = 0.0;
        dmu = 0.0;
        return false;
    }

    const double numerator = qp1 - qm1;
    mu = numerator / denominator;
    const double d2 = denominator * denominator;
    const double dMu_dQm1 = (-denominator - numerator) / d2;
    const double dMu_dQ0 = -numerator / d2;
    const double dMu_dQp1 = (denominator - numerator) / d2;
    dmu = sqrt( dMu_dQm1 * dMu_dQm1 * dq[0] * dq[0] + dMu_dQ0  * dMu_dQ0  * dq[1] * dq[1] + dMu_dQp1 * dMu_dQp1 * dq[2] * dq[2] );

    return true;
}

// =============================================================================
// Coordinate helpers
// =============================================================================

double PadmeXmm(int centerXPhys, double localMu) {
    return -( static_cast<double>(centerXPhys - kExpectedXLocal) + localMu ) * kStripPitchMm;
}

double PadmeYmm(int centerYPhys, double localMu) {
    return ( static_cast<double>(centerYPhys - 16 - kExpectedYLocal) + localMu ) * kStripPitchMm;
}

bool IsEdgePhysicalStrip(int physicalStrip) {
    return physicalStrip == 0 || physicalStrip == 15 || physicalStrip == 16 || physicalStrip == 31;
}

// =============================================================================
// Calibration
// =============================================================================

void SetUnityCalibration() {
    gXCalib.fill(1.0);
    gXCalibErr.fill(0.0);
}

bool LoadCalibrationConstants(const string& filename) {
    ifstream input(filename);

    if (!input.is_open()) {
        cerr << "WARNING: cannot open calibration file " << filename << ". Using unity calibration." << endl;
        return false;
    }

    array<double, kNStripsPerView> tempCalib{};
    array<double, kNStripsPerView> tempErr{};

    for (int strip = 0; strip < kNStripsPerView; ++strip) {
        if (!(input >> tempCalib[strip] >> tempErr[strip])) {
            cerr << "WARNING: calibration file " << filename << " is incomplete. Expected " << kNStripsPerView << " lines. Using unity calibration." << endl;
            return false;
        }
    }

    gXCalib = tempCalib;
    gXCalibErr = tempErr;
    cout << "Calibration constants loaded from " << filename << endl;

    return true;
}

// =============================================================================
// TMinuit three-strip integrated-Gaussian fit
// =============================================================================

static array<double, 3> gFitQ{};
static array<double, 3> gFitDQ{};

void FitFCN( int&, double*, double& fval, double* par, int) {
    const double amplitude = par[0];
    const double mu = par[1];
    const double sigma = par[2];

    if (sigma <= 0.0 || !isfinite(sigma)) {
        fval = 1.e30;
        return;
    }

    double chi2 = 0.0;
    for (int i = 0; i < 3; ++i) {

        const double stripCenter = static_cast<double>(i - 1);
        const double zLeft = (stripCenter - kStripWidthMm / 2.0 - mu) / sigma;
        const double zRight = (stripCenter + kStripWidthMm / 2.0 - mu) / sigma;
        const double model = amplitude * ( TMath::Erf(zRight / sqrt(2.0)) - TMath::Erf(zLeft  / sqrt(2.0)) ) / 2.0;
        double error = gFitDQ[i];
        if (error <= 0.0 || !isfinite(error)) error = 1.0;

        const double residual = model - gFitQ[i];
        chi2 += residual * residual / (error * error);
    }

    fval = chi2;
}

struct FitConfig {
    array<double, 3> start;
    double maxAmplitude;
};

const FitConfig kRawFitConfig{{300.0, 0.0, 0.5}, 1000.0 };
const FitConfig kCalFitConfig{{800.0, 0.0, 1.5}, 1800.0 };

FitResult FitBeamOnce( const array<double, 3>& q, const array<double, 3>& dq, const FitConfig& config, const array<double, 3>* initialParameters = nullptr) {
    gFitQ = q;
    gFitDQ = dq;

    const array<double, 3> start = initialParameters ? *initialParameters : config.start;

    TMinuit minuit(3);

    minuit.SetPrintLevel(-1);
    minuit.SetFCN(FitFCN);
    minuit.DefineParameter( 0, "A", max(0.0, start[0]), 1.0, 0.0, config.maxAmplitude );
    minuit.DefineParameter( 1, "mu", start[1], 0.01, -8.0, 8.0 );
    minuit.DefineParameter( 2, "sigma", max(1.e-3, fabs(start[2])), 0.01, 1.e-3, 5.0 );

    FitResult result;
    result.gof.migradStatus = minuit.Migrad();
    minuit.mnstat( result.gof.chi2, result.gof.edm, result.gof.errdef, result.gof.nvpar, result.gof.nparx, result.gof.covStatus );

    for (int i = 0; i < 3; ++i) {
        minuit.GetParameter( i, result.par[i], result.err[i] );
    }

    result.usable = result.gof.migradStatus == 0 && isfinite(result.par[0]) && isfinite(result.par[1]) && isfinite(result.par[2]) && result.par[2] > 0.0 && isfinite(result.gof.chi2);
    return result;
}

FitResult FitBeam( const array<double, 3>& q, const array<double, 3>& dq, const FitConfig& config, const array<double, 3>* previousParameters = nullptr) {
    FitResult result = FitBeamOnce( q, dq, config, nullptr );

    if (!result.usable && previousParameters != nullptr) {
        result = FitBeamOnce( q, dq, config, previousParameters );
    }

    return result;
}

// =============================================================================
// Input
// =============================================================================

bool ReadCompleteBlocks( const string& inputFile, int requestedRun, vector<BlockData>& blocks) {
    TFile* file = TFile::Open( inputFile.c_str(), "READ" );

    if (!file || file->IsZombie()) {
        cerr << "ERROR: cannot open reconstruction file " << inputFile << endl;

        if (file) {
            file->Close();
            delete file;
        }
        return false;
    }

    TTree* tree = dynamic_cast<TTree*>( file->Get("Blocks") );
    if (!tree) {
        cerr << "ERROR: tree 'Blocks' not found in " << inputFile << endl;

        file->Close();
        delete file;
        return false;
    }

    const vector<string> requiredBranches = {"run", "blockId", "complete", "nEvents", "firstTimeUnix", "meanTimeUnix", "lastTimeUnix", "targetQ", "lgQMean" };

    for (const string& branchName : requiredBranches) {
        if (!tree->GetBranch(branchName.c_str())) {
            cerr << "ERROR: required branch '" << branchName << "' not found in Blocks tree." << endl;

            file->Close();
            delete file;
            return false;
        }
    }

    Int_t run = -1;
    Long64_t blockId = -1;
    Bool_t complete = false;
    Int_t nEvents = 0;

    Double_t firstTimeUnix = 0.0;
    Double_t meanTimeUnix = 0.0;
    Double_t lastTimeUnix = 0.0;

    Double_t targetQ[kNChannels] = {0.0};
    Double_t lgQMean = 0.0;

    tree->SetBranchStatus("*", 0);

    for (const string& branchName : requiredBranches) {
        tree->SetBranchStatus( branchName.c_str(), 1 );
    }

    tree->SetBranchAddress("run", &run);
    tree->SetBranchAddress("blockId", &blockId);
    tree->SetBranchAddress("complete", &complete);
    tree->SetBranchAddress("nEvents", &nEvents);
    tree->SetBranchAddress( "firstTimeUnix", &firstTimeUnix );
    tree->SetBranchAddress( "meanTimeUnix", &meanTimeUnix );
    tree->SetBranchAddress( "lastTimeUnix", &lastTimeUnix );
    tree->SetBranchAddress( "targetQ", targetQ );
    tree->SetBranchAddress( "lgQMean", &lgQMean );

    const Long64_t nEntries = tree->GetEntries();

    blocks.clear();
    blocks.reserve( static_cast<size_t>(nEntries) );

    Long64_t nIncomplete = 0;
    Long64_t nWrongRun = 0;
    Long64_t nWrongSize = 0;

    for (Long64_t entry = 0; entry < nEntries; ++entry) {

        tree->GetEntry(entry);

        if (requestedRun > 0 && run != requestedRun) {
            ++nWrongRun;
            continue;
        }

        if (!complete) {
            ++nIncomplete;
            continue;
        }

        if (nEvents != kEventsPerBlock) {
            ++nWrongSize;
            continue;
        }

        BlockData block;

        block.run = run;
        block.blockId = blockId;
        block.nEvents = nEvents;

        block.firstTimeUnix = firstTimeUnix;
        block.meanTimeUnix = meanTimeUnix;
        block.lastTimeUnix = lastTimeUnix;

        for (int ch = 0; ch < kNChannels; ++ch) {
            block.targetQ[ch] = max( 0.0, static_cast<double>( targetQ[ch] ) );
        }

        block.lgQMean = lgQMean;
        blocks.push_back(block);
    }

    file->Close();
    delete file;

    cout << "Blocks tree: " << nEntries << " entries, " << blocks.size() << " complete usable blocks";

    if (nIncomplete > 0) {
        cout << ", skipped incomplete=" << nIncomplete;
    }

    if (nWrongRun > 0) {
        cout << ", skipped other-run=" << nWrongRun;
    }

    if (nWrongSize > 0) {
        cout << ", skipped invalid-size=" << nWrongSize;
    }

    cout << endl;

    return !blocks.empty();
}

// =============================================================================
// Centre-strip determination
// =============================================================================

bool DetermineCentralStrips( const vector<BlockData>& blocks, int& centerXPhys, int& centerYPhys) {
    array<double, kNStripsPerView> totalX{};
    array<double, kNStripsPerView> totalY{};

    for (const BlockData& block : blocks) {
        for (int adc = 0; adc < kNChannels; ++adc) {
            const int phys = kADCToPhysics[adc];
            const double q = block.targetQ[adc];
            if (phys < 16) totalX[phys] += q;
            else totalY[phys - 16] += q;
        }
    }

    centerXPhys = static_cast<int>( max_element( totalX.begin(), totalX.end() ) - totalX.begin() );
    centerYPhys = static_cast<int>( max_element( totalY.begin(), totalY.end() ) - totalY.begin() ) + 16;
    cout << "Central physical strips: X=" << centerXPhys << "  Y=" << centerYPhys << endl;
    if (IsEdgePhysicalStrip(centerXPhys) || IsEdgePhysicalStrip(centerYPhys)) {
        cerr << "ERROR: central strip is on an edge. " << "A three-strip (-1,0,+1) analysis is impossible." << endl;
        return false;
    }

    cout << "X ADCs (-1,0,+1): " << kPhysicsToADC[centerXPhys - 1] << ", " << kPhysicsToADC[centerXPhys] << ", " << kPhysicsToADC[centerXPhys + 1] << endl;
    cout << "Y ADCs (-1,0,+1): " << kPhysicsToADC[centerYPhys - 1] << ", " << kPhysicsToADC[centerYPhys] << ", " << kPhysicsToADC[centerYPhys + 1] << endl;

    return true;
}

// =============================================================================
// Build one period from 100 complete reconstruction blocks
// =============================================================================

PeriodResult BuildPeriod( const vector<BlockData>& blocks, size_t begin, int periodId, int centerXPhys, int centerYPhys, const array<double, 3>* previousXRaw, const array<double, 3>* previousYRaw, const array<double, 3>* previousXCal) {
    PeriodResult result;

    result.periodId = periodId;
    const size_t end = begin + kBlocksPerPeriod;
    result.firstBlockId = blocks[begin].blockId;
    result.lastBlockId = blocks[end - 1].blockId;
    result.firstTimeUnix = blocks[begin].firstTimeUnix;
    result.lastTimeUnix = blocks[end - 1].lastTimeUnix;

    vector<double> blockTimes;
    vector<double> lgValues;

    array<vector<double>, 3> xValues;
    array<vector<double>, 3> yValues;

    blockTimes.reserve(kBlocksPerPeriod);
    lgValues.reserve(kBlocksPerPeriod);

    for (auto& v : xValues) {
        v.reserve(kBlocksPerPeriod);
    }

    for (auto& v : yValues) {
        v.reserve(kBlocksPerPeriod);
    }

    const array<int, 3> xADC = { kPhysicsToADC[centerXPhys - 1], kPhysicsToADC[centerXPhys], kPhysicsToADC[centerXPhys + 1] };

    const array<int, 3> yADC = { kPhysicsToADC[centerYPhys - 1], kPhysicsToADC[centerYPhys], kPhysicsToADC[centerYPhys + 1] };

    for (size_t i = begin; i < end; ++i) {
        const BlockData& block = blocks[i];
        blockTimes.push_back( block.meanTimeUnix );
        lgValues.push_back( block.lgQMean );
        for (int j = 0; j < 3; ++j) {
            xValues[j].push_back( block.targetQ[xADC[j]] );
            yValues[j].push_back( block.targetQ[yADC[j]] );
        }
    }

    double dummyTimeSem = 0.0;
    MeanAndSem( blockTimes, result.meanTimeUnix, dummyTimeSem );
    result.timeHalfWidth = 0.5 * max( 0.0, result.lastTimeUnix - result.firstTimeUnix );
    MeanAndSem( lgValues, result.qLG, result.dqLG );

    for (int j = 0; j < 3; ++j) {
        MeanAndSem( xValues[j], result.qXRaw[j], result.dqXRaw[j] );
        MeanAndSem( yValues[j], result.qYRaw[j], result.dqYRaw[j] );
    }

    const array<int, 3> xPhys = { centerXPhys - 1, centerXPhys, centerXPhys + 1 };

    for (int j = 0; j < 3; ++j) {
        const int strip = xPhys[j];
        const double c = gXCalib[strip];
        const double dc = gXCalibErr[strip];
        result.qXCal[j] = result.qXRaw[j] * c;
        result.dqXCal[j] = hypot( result.dqXRaw[j] * c, result.qXRaw[j] * dc );
    }

    result.qXTotRaw = result.qXRaw[0] + result.qXRaw[1] + result.qXRaw[2];
    result.dqXTotRaw = Quadrature3( result.dqXRaw );
    result.qYTotRaw = result.qYRaw[0] + result.qYRaw[1] + result.qYRaw[2];
    result.dqYTotRaw = Quadrature3( result.dqYRaw );
    result.qXTotCal = result.qXCal[0] + result.qXCal[1] + result.qXCal[2];
    result.dqXTotCal = Quadrature3( result.dqXCal );
    result.qXRatioRaw = SafeRatio( result.qXTotRaw, result.qLG );
    result.dqXRatioRaw = RatioError( result.qXTotRaw, result.dqXTotRaw, result.qLG, result.dqLG );
    result.qYRatioRaw = SafeRatio( result.qYTotRaw, result.qLG );
    result.dqYRatioRaw = RatioError( result.qYTotRaw, result.dqYTotRaw, result.qLG, result.dqLG );
    result.qXRatioCal = SafeRatio( result.qXTotCal, result.qLG );
    result.dqXRatioCal = RatioError( result.qXTotCal, result.dqXTotCal, result.qLG, result.dqLG );

    WeightedMeanMu3Strips( result.qXRaw, result.dqXRaw, result.muXWRaw, result.dMuXWRaw );
    WeightedMeanMu3Strips( result.qYRaw, result.dqYRaw, result.muYWRaw, result.dMuYWRaw );
    WeightedMeanMu3Strips( result.qXCal, result.dqXCal, result.muXWCal, result.dMuXWCal );
   
    result.fitXRaw = FitBeam( result.qXRaw, result.dqXRaw, kRawFitConfig, previousXRaw );
    result.fitYRaw = FitBeam( result.qYRaw, result.dqYRaw, kRawFitConfig, previousYRaw );
    result.fitXCal = FitBeam( result.qXCal, result.dqXCal, kCalFitConfig, previousXCal );

    return result;
}

// =============================================================================
// Whole-run result from all complete reconstruction blocks
// =============================================================================

RunSummary BuildRunSummary( const vector<BlockData>& blocks, int centerXPhys, int centerYPhys) {
    RunSummary result;

    const array<int, 3> xADC = { kPhysicsToADC[centerXPhys - 1], kPhysicsToADC[centerXPhys], kPhysicsToADC[centerXPhys + 1] };

    const array<int, 3> yADC = { kPhysicsToADC[centerYPhys - 1], kPhysicsToADC[centerYPhys], kPhysicsToADC[centerYPhys + 1] };

    vector<double> lgValues;
    array<vector<double>, 3> xValues;
    array<vector<double>, 3> yValues;

    lgValues.reserve(blocks.size());

    for (auto& v : xValues) {
        v.reserve(blocks.size());
    }

    for (auto& v : yValues) {
        v.reserve(blocks.size());
    }

    for (const BlockData& block : blocks) {

        lgValues.push_back( block.lgQMean );
        for (int j = 0; j < 3; ++j) {
            xValues[j].push_back( block.targetQ[xADC[j]] );
            yValues[j].push_back( block.targetQ[yADC[j]] );
        }
    }

    MeanAndSem( lgValues, result.qLG, result.dqLG );

    for (int j = 0; j < 3; ++j) {
        MeanAndSem( xValues[j], result.qXRaw[j], result.dqXRaw[j] );
        MeanAndSem( yValues[j], result.qYRaw[j], result.dqYRaw[j] );
    }

    const array<int, 3> xPhys = { centerXPhys - 1, centerXPhys, centerXPhys + 1 };

    for (int j = 0; j < 3; ++j) {
        const int strip = xPhys[j];
        const double c = gXCalib[strip];
        const double dc = gXCalibErr[strip];
        result.qXCal[j] = result.qXRaw[j] * c;
        result.dqXCal[j] = hypot( result.dqXRaw[j] * c, result.qXRaw[j] * dc );
    }

    WeightedMeanMu3Strips( result.qXRaw, result.dqXRaw, result.muXWRaw, result.dMuXWRaw );
    WeightedMeanMu3Strips( result.qYRaw, result.dqYRaw, result.muYWRaw, result.dMuYWRaw );
    WeightedMeanMu3Strips( result.qXCal, result.dqXCal, result.muXWCal, result.dMuXWCal );

    result.fitXRaw = FitBeam( result.qXRaw, result.dqXRaw, kRawFitConfig );
    result.fitYRaw = FitBeam( result.qYRaw, result.dqYRaw, kRawFitConfig );
    result.fitXCal = FitBeam( result.qXCal, result.dqXCal, kCalFitConfig );

    return result;
}

// =============================================================================
// ROOT graph helpers
// =============================================================================

void SetGraphAttributes( TGraphErrors* graph, const TString& name, const TString& xLabel, const TString& yLabel, int markerStyle, int color) {
    graph->SetName(name);
    graph->SetTitle(name);
    graph->GetXaxis()->SetTitle( xLabel );
    graph->GetYaxis()->SetTitle( yLabel );
    graph->SetMarkerStyle( markerStyle );
    graph->SetMarkerColor( color );
    graph->SetLineColor( color );
}

TGraphErrors* MakeTrendGraph( const TString& name, const TString& yLabel, const vector<double>& periodIndex, const vector<double>& values, const vector<double>& errors, int markerStyle, int color) {
    const int n = static_cast<int>( values.size() );
    vector<double> xErrors( static_cast<size_t>(n), 0.0 );
    TGraphErrors* graph = new TGraphErrors( n, periodIndex.data(), values.data(), xErrors.data(), errors.data() );
    SetGraphAttributes( graph, name, "Period", yLabel, markerStyle, color );
    return graph;
}

TGraphErrors* CloneTrendVsTime( const TGraphErrors* source, const vector<double>& meanTimes, const vector<double>& timeHalfWidths) {
    const int n = source->GetN();
    TGraphErrors* graph = new TGraphErrors(n);
    TString name = source->GetName();
    name += "_time";
    
    graph->SetName(name);
    graph->SetTitle(name);
    graph->GetXaxis()->SetTitle( "Mean UNIX time [s]" );
    graph->GetYaxis()->SetTitle( source->GetYaxis()->GetTitle() );
    graph->SetMarkerStyle( source->GetMarkerStyle() );
    graph->SetMarkerColor( source->GetMarkerColor() );
    graph->SetLineColor( source->GetLineColor() );

    for (int i = 0; i < n; ++i) {
        double oldX = 0.0;
        double y = 0.0;
        source->GetPoint( i, oldX, y );
        graph->SetPoint( i, meanTimes[static_cast<size_t>(i)], y );
        graph->SetPointError( i, timeHalfWidths[static_cast<size_t>(i)], source->GetErrorY(i) );
    }

    return graph;
}

TGraphErrors* MakeOnePointGraph( const TString& name, const TString& yLabel, double x, double y, double ey, int markerStyle, int color) {
    const double ex = 0.0;
    TGraphErrors* graph = new TGraphErrors( 1, &x, &y, &ex, &ey );
    SetGraphAttributes( graph, name, "E_{beam} [MeV]", yLabel, markerStyle, color );
    return graph;
}

// =============================================================================
// Main analysis
// =============================================================================

bool AnalyseRun( int runNumber, double beamEnergyMeV, const string& inputRoot, const string& calibrationFile, const string& outputRoot, const string& outputTxt) {
    SetUnityCalibration();

    if (!calibrationFile.empty()) {
        LoadCalibrationConstants( calibrationFile );
    }

    vector<BlockData> blocks;

    if (!ReadCompleteBlocks( inputRoot, runNumber, blocks)) {
        return false;
    }

    int centerXPhys = -1;
    int centerYPhys = -1;

    if (!DetermineCentralStrips( blocks, centerXPhys, centerYPhys)) {
        return false;
    }
    const size_t nPeriods = blocks.size() / static_cast<size_t>( kBlocksPerPeriod );
    const size_t leftoverBlocks = blocks.size() % static_cast<size_t>( kBlocksPerPeriod );
    cout << "Complete blocks: " << blocks.size() << endl;
    cout << "Analysis periods (" << kBlocksPerPeriod << " blocks/period): " << nPeriods << endl;

    if (leftoverBlocks > 0) {
        cout << "Complete blocks not entering a trend period: " << leftoverBlocks << " (they ARE included in the whole-run summary)" << endl;
    }

    if (nPeriods == 0) {
        cerr << "ERROR: fewer than " << kBlocksPerPeriod << " complete blocks. " << "No trend point can be produced." << endl;

        return false;
    }

    vector<PeriodResult> periods;
    periods.reserve(nPeriods);

    array<double, 3> previousXRaw{};
    array<double, 3> previousYRaw{};
    array<double, 3> previousXCal{};

    bool havePreviousXRaw = false;
    bool havePreviousYRaw = false;
    bool havePreviousXCal = false;

    for (size_t period = 0; period < nPeriods; ++period) {
        const size_t begin = period * static_cast<size_t>( kBlocksPerPeriod );
        PeriodResult result = BuildPeriod( blocks, begin, static_cast<int>(period), centerXPhys, centerYPhys, havePreviousXRaw ? &previousXRaw : nullptr, havePreviousYRaw ? &previousYRaw : nullptr, havePreviousXCal ? &previousXCal : nullptr );

        if (result.fitXRaw.usable) {
            previousXRaw = result.fitXRaw.par;
            havePreviousXRaw = true;
        }

        if (result.fitYRaw.usable) {
            previousYRaw = result.fitYRaw.par;
            havePreviousYRaw = true;
        }

        if (result.fitXCal.usable) {
            previousXCal = result.fitXCal.par;
            havePreviousXCal = true;
        }

        periods.push_back(result);
    }

    const RunSummary summary = BuildRunSummary( blocks, centerXPhys, centerYPhys );

    // -------------------------------------------------------------------------
    // Convert PeriodResult objects into plotting arrays.
    // -------------------------------------------------------------------------

    const size_t n = periods.size();

    vector<double> idx(n);
    vector<double> meanTimes(n);
    vector<double> timeHalfWidths(n);

    vector<double> qLG(n), dqLG(n);
    vector<double> qLGEcorr(n), dqLGEcorr(n);

    vector<double> qXTotRaw(n), dqXTotRaw(n);
    vector<double> qYTotRaw(n), dqYTotRaw(n);
    vector<double> qXTotCal(n), dqXTotCal(n);

    vector<double> qXRatioRaw(n), dqXRatioRaw(n);
    vector<double> qYRatioRaw(n), dqYRatioRaw(n);
    vector<double> qXRatioCal(n), dqXRatioCal(n);

    vector<double> qXRatioRawEcorr(n), dqXRatioRawEcorr(n);
    vector<double> qYRatioRawEcorr(n), dqYRatioRawEcorr(n);
    vector<double> qXRatioCalEcorr(n), dqXRatioCalEcorr(n);

    vector<double> qXm1Raw(n), dqXm1Raw(n);
    vector<double> qX0Raw(n), dqX0Raw(n);
    vector<double> qXp1Raw(n), dqXp1Raw(n);

    vector<double> qYm1Raw(n), dqYm1Raw(n);
    vector<double> qY0Raw(n), dqY0Raw(n);
    vector<double> qYp1Raw(n), dqYp1Raw(n);

    vector<double> qXm1Cal(n), dqXm1Cal(n);
    vector<double> qX0Cal(n), dqX0Cal(n);
    vector<double> qXp1Cal(n), dqXp1Cal(n);

    vector<double> qX0OverLGGRaw(n), dqX0OverLGGRaw(n);
    vector<double> qY0OverLGGRaw(n), dqY0OverLGGRaw(n);
    vector<double> qX0OverLGGCal(n), dqX0OverLGGCal(n);

    vector<double> xFitRaw(n), dxFitRaw(n);
    vector<double> yFitRaw(n), dyFitRaw(n);
    vector<double> xFitCal(n), dxFitCal(n);

    vector<double> xWRaw(n), dxWRaw(n);
    vector<double> yWRaw(n), dyWRaw(n);
    vector<double> xWCal(n), dxWCal(n);

    vector<double> deltaXRaw(n), dDeltaXRaw(n);
    vector<double> deltaYRaw(n), dDeltaYRaw(n);
    vector<double> deltaXCal(n), dDeltaXCal(n);

    vector<double> sigmaXRaw(n), dSigmaXRaw(n);
    vector<double> sigmaYRaw(n), dSigmaYRaw(n);
    vector<double> sigmaXCal(n), dSigmaXCal(n);

    vector<double> chi2XRaw(n), chi2YRaw(n), chi2XCal(n);
    vector<double> covXRaw(n), covYRaw(n), covXCal(n);
    vector<double> migradXRaw(n), migradYRaw(n), migradXCal(n);

    vector<double> fittedQXRaw(n), dFittedQXRaw(n);
    vector<double> fittedQYRaw(n), dFittedQYRaw(n);
    vector<double> fittedQXCal(n), dFittedQXCal(n);

    vector<double> fittedRatioXRaw(n), dFittedRatioXRaw(n);
    vector<double> fittedRatioYRaw(n), dFittedRatioYRaw(n);
    vector<double> fittedRatioXCal(n), dFittedRatioXCal(n);

    vector<double> fittedRatioXRawEcorr(n), dFittedRatioXRawEcorr(n);
    vector<double> fittedRatioYRawEcorr(n), dFittedRatioYRawEcorr(n);
    vector<double> fittedRatioXCalEcorr(n), dFittedRatioXCalEcorr(n);

    const double widthFloor = kStripPitchMm / sqrt(12.0);

    for (size_t i = 0; i < n; ++i) {
        const PeriodResult& p = periods[i];

        idx[i] = static_cast<double>(i);
        meanTimes[i] = p.meanTimeUnix;
        timeHalfWidths[i] = p.timeHalfWidth;
        qLG[i] = p.qLG;
        dqLG[i] = p.dqLG;

        const double lgEnergyScale = kCalibrationEnergyMeV / beamEnergyMeV;
        const double ratioEnergyScale = beamEnergyMeV / kCalibrationEnergyMeV;

        qLGEcorr[i] = p.qLG * lgEnergyScale;
        dqLGEcorr[i] = p.dqLG * fabs(lgEnergyScale);
        qXTotRaw[i] = p.qXTotRaw;
        dqXTotRaw[i] = p.dqXTotRaw;
        qYTotRaw[i] = p.qYTotRaw;
        dqYTotRaw[i] = p.dqYTotRaw;
        qXTotCal[i] = p.qXTotCal;
        dqXTotCal[i] = p.dqXTotCal;
        qXRatioRaw[i] = p.qXRatioRaw;
        dqXRatioRaw[i] = p.dqXRatioRaw;
        qYRatioRaw[i] = p.qYRatioRaw;
        dqYRatioRaw[i] = p.dqYRatioRaw;
        qXRatioCal[i] = p.qXRatioCal;
        dqXRatioCal[i] = p.dqXRatioCal;
        qXRatioRawEcorr[i] = p.qXRatioRaw * ratioEnergyScale;
        dqXRatioRawEcorr[i] = p.dqXRatioRaw * fabs(ratioEnergyScale);
        qYRatioRawEcorr[i] = p.qYRatioRaw * ratioEnergyScale;
        dqYRatioRawEcorr[i] = p.dqYRatioRaw * fabs(ratioEnergyScale);
        qXRatioCalEcorr[i] = p.qXRatioCal * ratioEnergyScale;
        dqXRatioCalEcorr[i] = p.dqXRatioCal * fabs(ratioEnergyScale);

        qXm1Raw[i] = p.qXRaw[0];
        qX0Raw[i]  = p.qXRaw[1];
        qXp1Raw[i] = p.qXRaw[2];

        dqXm1Raw[i] = p.dqXRaw[0];
        dqX0Raw[i]  = p.dqXRaw[1];
        dqXp1Raw[i] = p.dqXRaw[2];

        qYm1Raw[i] = p.qYRaw[0];
        qY0Raw[i]  = p.qYRaw[1];
        qYp1Raw[i] = p.qYRaw[2];

        dqYm1Raw[i] = p.dqYRaw[0];
        dqY0Raw[i]  = p.dqYRaw[1];
        dqYp1Raw[i] = p.dqYRaw[2];

        qXm1Cal[i] = p.qXCal[0];
        qX0Cal[i]  = p.qXCal[1];
        qXp1Cal[i] = p.qXCal[2];

        dqXm1Cal[i] = p.dqXCal[0];
        dqX0Cal[i]  = p.dqXCal[1];
        dqXp1Cal[i] = p.dqXCal[2];

        qX0OverLGGRaw[i] = SafeRatio( p.qXRaw[1], p.qLG );
        dqX0OverLGGRaw[i] = RatioError( p.qXRaw[1], p.dqXRaw[1], p.qLG, p.dqLG );
        qY0OverLGGRaw[i] = SafeRatio( p.qYRaw[1], p.qLG );
        dqY0OverLGGRaw[i] = RatioError( p.qYRaw[1], p.dqYRaw[1], p.qLG, p.dqLG );
        qX0OverLGGCal[i] = SafeRatio( p.qXCal[1], p.qLG );
        dqX0OverLGGCal[i] = RatioError( p.qXCal[1], p.dqXCal[1], p.qLG, p.dqLG );
        xWRaw[i] = PadmeXmm( centerXPhys, p.muXWRaw );
        dxWRaw[i] = fabs(p.dMuXWRaw) * kStripPitchMm;
        yWRaw[i] = PadmeYmm( centerYPhys, p.muYWRaw );
        dyWRaw[i] = fabs(p.dMuYWRaw) * kStripPitchMm;
        xWCal[i] = PadmeXmm( centerXPhys, p.muXWCal );
        dxWCal[i] = fabs(p.dMuXWCal) * kStripPitchMm;
        xFitRaw[i] = PadmeXmm( centerXPhys, p.fitXRaw.par[1] );
        dxFitRaw[i] = fabs( p.fitXRaw.err[1] ) * kStripPitchMm;
        yFitRaw[i] = PadmeYmm( centerYPhys, p.fitYRaw.par[1] );
        dyFitRaw[i] = fabs( p.fitYRaw.err[1] ) * kStripPitchMm;
        xFitCal[i] = PadmeXmm( centerXPhys, p.fitXCal.par[1] );
        dxFitCal[i] = fabs( p.fitXCal.err[1] ) * kStripPitchMm;
        deltaXRaw[i] = xFitRaw[i] - xWRaw[i];
        dDeltaXRaw[i] = hypot( dxFitRaw[i], dxWRaw[i] );
        deltaYRaw[i] = yFitRaw[i] - yWRaw[i];
        dDeltaYRaw[i] = hypot( dyFitRaw[i], dyWRaw[i] );
        deltaXCal[i] = xFitCal[i] - xWCal[i];
        dDeltaXCal[i] = hypot( dxFitCal[i], dxWCal[i] );
        sigmaXRaw[i] = max( fabs(p.fitXRaw.par[2]), widthFloor );
        dSigmaXRaw[i] = fabs(p.fitXRaw.par[2]) > widthFloor ? fabs(p.fitXRaw.err[2]) : 0.0;
        sigmaYRaw[i] = max( fabs(p.fitYRaw.par[2]), widthFloor );
        dSigmaYRaw[i] = fabs(p.fitYRaw.par[2]) > widthFloor ? fabs(p.fitYRaw.err[2]) : 0.0;
        sigmaXCal[i] = max( fabs(p.fitXCal.par[2]), widthFloor );
        dSigmaXCal[i] = fabs(p.fitXCal.par[2]) > widthFloor ? fabs(p.fitXCal.err[2]) : 0.0;
        chi2XRaw[i] = p.fitXRaw.gof.chi2;
        chi2YRaw[i] = p.fitYRaw.gof.chi2;
        chi2XCal[i] = p.fitXCal.gof.chi2;
        covXRaw[i] = p.fitXRaw.gof.covStatus;
        covYRaw[i] = p.fitYRaw.gof.covStatus;
        covXCal[i] = p.fitXCal.gof.covStatus;
        migradXRaw[i] = p.fitXRaw.gof.migradStatus;
        migradYRaw[i] = p.fitYRaw.gof.migradStatus;
        migradXCal[i] = p.fitXCal.gof.migradStatus;
        fittedQXRaw[i] = p.fitXRaw.par[0];
        dFittedQXRaw[i] = fabs(p.fitXRaw.err[0]);
        fittedQYRaw[i] = p.fitYRaw.par[0];
        dFittedQYRaw[i] = fabs(p.fitYRaw.err[0]);
        fittedQXCal[i] = p.fitXCal.par[0];
        dFittedQXCal[i] = fabs(p.fitXCal.err[0]);
        fittedRatioXRaw[i] = SafeRatio( fittedQXRaw[i], p.qLG );
        dFittedRatioXRaw[i] = RatioError( fittedQXRaw[i], dFittedQXRaw[i], p.qLG, p.dqLG );
        fittedRatioYRaw[i] = SafeRatio( fittedQYRaw[i], p.qLG );
        dFittedRatioYRaw[i] = RatioError( fittedQYRaw[i], dFittedQYRaw[i], p.qLG, p.dqLG );
        fittedRatioXCal[i] = SafeRatio( fittedQXCal[i], p.qLG );
        dFittedRatioXCal[i] = RatioError( fittedQXCal[i], dFittedQXCal[i], p.qLG, p.dqLG );
        fittedRatioXRawEcorr[i] = fittedRatioXRaw[i] * ratioEnergyScale;
        dFittedRatioXRawEcorr[i] = dFittedRatioXRaw[i] * fabs(ratioEnergyScale);
        fittedRatioYRawEcorr[i] = fittedRatioYRaw[i] * ratioEnergyScale;
        dFittedRatioYRawEcorr[i] = dFittedRatioYRaw[i] * fabs(ratioEnergyScale);
        fittedRatioXCalEcorr[i] = fittedRatioXCal[i] * ratioEnergyScale;
        dFittedRatioXCalEcorr[i] = dFittedRatioXCal[i] * fabs(ratioEnergyScale);
    }

    // -------------------------------------------------------------------------
    // Output ROOT file
    // -------------------------------------------------------------------------

    TFile* output = TFile::Open( outputRoot.c_str(), "RECREATE" );

    if (!output || output->IsZombie()) {
        cerr << "ERROR: cannot create output ROOT file " << outputRoot << endl;
        if (output) {
            output->Close();
            delete output;
        }
        return false;
    }

    TDirectory* distributionsDir = output->mkdir( "distributions" );
    TDirectory* trendsPeriodDir = output->mkdir( "trends_period" );
    TDirectory* trendsTimeDir = output->mkdir( "trends_time" );
    TDirectory* summaryDir = output->mkdir( "summary" );

    // -------------------------------------------------------------------------
    // Distributions
    // -------------------------------------------------------------------------

    distributionsDir->cd();

    TH1D* hChargeXRaw = new TH1D( "hChargeX_raw", "Q_{X} raw [pC]", 1200, 0, 1200 );
    TH1D* hChargeYRaw = new TH1D( "hChargeY_raw", "Q_{Y} raw [pC]", 1200, 0, 1200 );
    TH1D* hChargeXCal = new TH1D( "hChargeX", "Q_{X} calibrated [pC]", 1200, 0, 1200 );
    TH1D* hQTarX0Raw = new TH1D( "hQTarX0_raw", "Q_{X strip 0} raw [pC]", 1200, 0, 1200 );
    TH1D* hQTarXm1Raw = new TH1D( "hQTarXm1_raw", "Q_{X strip -1} raw [pC]", 1200, 0, 1200 );
    TH1D* hQTarXp1Raw = new TH1D( "hQTarXp1_raw", "Q_{X strip +1} raw [pC]", 1200, 0, 1200 );
    TH1D* hQTarY0Raw = new TH1D( "hQTarY0_raw", "Q_{Y strip 0} raw [pC]", 1200, 0, 1200 );
    TH1D* hQTarYm1Raw = new TH1D( "hQTarYm1_raw", "Q_{Y strip -1} raw [pC]", 1200, 0, 1200 );
    TH1D* hQTarYp1Raw = new TH1D( "hQTarYp1_raw", "Q_{Y strip +1} raw [pC]", 1200, 0, 1200 );
    TH1D* hQTarX0Cal = new TH1D( "hQTarX0", "Q_{X strip 0} calibrated [pC]", 1200, 0, 1200 );
    TH1D* hQTarXm1Cal = new TH1D( "hQTarXm1", "Q_{X strip -1} calibrated [pC]", 1200, 0, 1200 );
    TH1D* hQTarXp1Cal = new TH1D( "hQTarXp1", "Q_{X strip +1} calibrated [pC]", 1200, 0, 1200 );
    TH1D* hChi2XRaw = new TH1D( "hChi2X_raw", "Minimized FCN X raw", 1000, 0, 10 );
    TH1D* hChi2YRaw = new TH1D( "hChi2Y_raw", "Minimized FCN Y raw", 1000, 0, 10 );
    TH1D* hChi2XCal = new TH1D( "hChi2X", "Minimized FCN X calibrated", 1000, 0, 10 );

    // Backward-compatible names: these contain covariance status.
    TH1D* hFitStatusXRaw = new TH1D( "hFitStatusX_raw", "Covariance status X raw", 5, -0.5, 4.5 );
    TH1D* hFitStatusYRaw = new TH1D( "hFitStatusY_raw", "Covariance status Y raw", 5, -0.5, 4.5 );
    TH1D* hFitStatusXCal = new TH1D( "hFitStatusX", "Covariance status X calibrated", 5, -0.5, 4.5 );
    TH1D* hMigradStatusXRaw = new TH1D( "hMigradStatusX_raw", "MIGRAD status X raw", 11, -0.5, 10.5 );
    TH1D* hMigradStatusYRaw = new TH1D( "hMigradStatusY_raw", "MIGRAD status Y raw", 11, -0.5, 10.5 );
    TH1D* hMigradStatusXCal = new TH1D( "hMigradStatusX", "MIGRAD status X calibrated", 11, -0.5, 10.5 );

    for (size_t i = 0; i < n; ++i) {
        hChargeXRaw->Fill( qXTotRaw[i] );
        hChargeYRaw->Fill( qYTotRaw[i] );
        hChargeXCal->Fill( qXTotCal[i] );
        hQTarX0Raw->Fill( qX0Raw[i] );
        hQTarXm1Raw->Fill( qXm1Raw[i] );
        hQTarXp1Raw->Fill( qXp1Raw[i] );
        hQTarY0Raw->Fill( qY0Raw[i] );
        hQTarYm1Raw->Fill( qYm1Raw[i] );
        hQTarYp1Raw->Fill( qYp1Raw[i] );
        hQTarX0Cal->Fill( qX0Cal[i] );
        hQTarXm1Cal->Fill( qXm1Cal[i] );
        hQTarXp1Cal->Fill( qXp1Cal[i] );
        hChi2XRaw->Fill( chi2XRaw[i] );
        hChi2YRaw->Fill( chi2YRaw[i] );
        hChi2XCal->Fill( chi2XCal[i] );
        hFitStatusXRaw->Fill( covXRaw[i] );
        hFitStatusYRaw->Fill( covYRaw[i] );
        hFitStatusXCal->Fill( covXCal[i] );
        hMigradStatusXRaw->Fill( migradXRaw[i] );
        hMigradStatusYRaw->Fill( migradYRaw[i] );
        hMigradStatusXCal->Fill( migradXCal[i] );
    }

    hChargeXRaw->Write();
    hChargeYRaw->Write();
    hChargeXCal->Write();

    hQTarX0Raw->Write();
    hQTarXm1Raw->Write();
    hQTarXp1Raw->Write();

    hQTarY0Raw->Write();
    hQTarYm1Raw->Write();
    hQTarYp1Raw->Write();

    hQTarX0Cal->Write();
    hQTarXm1Cal->Write();
    hQTarXp1Cal->Write();

    hChi2XRaw->Write();
    hChi2YRaw->Write();
    hChi2XCal->Write();

    hFitStatusXRaw->Write();
    hFitStatusYRaw->Write();
    hFitStatusXCal->Write();

    hMigradStatusXRaw->Write();
    hMigradStatusYRaw->Write();
    hMigradStatusXCal->Write();

    // -------------------------------------------------------------------------
    // Period trends
    // -------------------------------------------------------------------------

    vector<TGraphErrors*> periodTrends;

    auto AddTrend = [&](const TString& name, const TString& yLabel, const vector<double>& values, const vector<double>& errors, int markerStyle, int color) {
            TGraphErrors* graph = MakeTrendGraph( name, yLabel, idx, values, errors, markerStyle, color );
            periodTrends.push_back( graph );
            return graph;
        };

    AddTrend( "gXTar_raw", "X_{fit} raw [mm]", xFitRaw, dxFitRaw, 20, kRed + 2 );
    AddTrend( "gYTar_raw", "Y_{fit} raw [mm]", yFitRaw, dyFitRaw, 20, kRed + 2 );
    AddTrend( "gXTar", "X_{fit} calibrated [mm]", xFitCal, dxFitCal, 20, kBlue + 2 );
    AddTrend( "gXTar_wmean_raw", "X_{wmean} raw [mm]", xWRaw, dxWRaw, 24, kRed + 2 );
    AddTrend( "gYTar_wmean_raw", "Y_{wmean} raw [mm]", yWRaw, dyWRaw, 24, kRed + 2 );
    AddTrend( "gXTar_wmean", "X_{wmean} calibrated [mm]", xWCal, dxWCal, 24, kBlue + 2 );
    AddTrend( "gDeltaX_fit_minus_wmean_raw", "X_{fit}-X_{wmean} raw [mm]", deltaXRaw, dDeltaXRaw, 20, kRed + 2 );
    AddTrend( "gDeltaY_fit_minus_wmean_raw", "Y_{fit}-Y_{wmean} raw [mm]", deltaYRaw, dDeltaYRaw, 20, kRed + 2 );
    AddTrend( "gDeltaX_fit_minus_wmean", "X_{fit}-X_{wmean} calibrated [mm]", deltaXCal, dDeltaXCal, 20, kBlue + 2 );
    
    TGraphErrors* gQLG = AddTrend( "gQLG", "Q_{LG} [pC]", qLG, dqLG, 20, kBlue + 2 );
    
    AddTrend( "gQLG_ECorr", "Q_{LG} energy-corrected [pC]", qLGEcorr, dqLGEcorr, 20, kBlue + 2 );
    
    TGraphErrors* gQXTotRaw = AddTrend( "gQXTot_target_raw", "Q_{X-Tar} raw [pC]", qXTotRaw, dqXTotRaw, 20, kRed + 2 );
    
    AddTrend( "gQYTot_target_raw", "Q_{Y-Tar} raw [pC]", qYTotRaw, dqYTotRaw, 20, kRed + 2 );
    
    TGraphErrors* gQXTotCal = AddTrend( "gQXTot_target_cal", "Q_{X-Tar} calibrated [pC]", qXTotCal, dqXTotCal, 20, kBlue + 2 );

    AddTrend( "gQm1_Xtarget_raw", "Q_{X-1} raw [pC]", qXm1Raw, dqXm1Raw, 20, kRed + 2 );
    AddTrend( "gQ0_Xtarget_raw", "Q_{X0} raw [pC]", qX0Raw, dqX0Raw, 20, kRed + 2 );
    AddTrend( "gQp1_Xtarget_raw", "Q_{X+1} raw [pC]", qXp1Raw, dqXp1Raw, 20, kRed + 2 );
    AddTrend( "gQm1_Ytarget_raw", "Q_{Y-1} raw [pC]", qYm1Raw, dqYm1Raw, 20, kRed + 2 );
    AddTrend( "gQ0_Ytarget_raw", "Q_{Y0} raw [pC]", qY0Raw, dqY0Raw, 20, kRed + 2 );
    AddTrend( "gQp1_Ytarget_raw", "Q_{Y+1} raw [pC]", qYp1Raw, dqYp1Raw, 20, kRed + 2 );
    AddTrend( "gQm1_Xtarget_cal", "Q_{X-1} calibrated [pC]", qXm1Cal, dqXm1Cal, 20, kBlue + 2 );
    AddTrend( "gQ0_Xtarget_cal", "Q_{X0} calibrated [pC]", qX0Cal, dqX0Cal, 20, kBlue + 2 );
    AddTrend( "gQp1_Xtarget_cal", "Q_{X+1} calibrated [pC]", qXp1Cal, dqXp1Cal, 20, kBlue + 2 );
    AddTrend( "gQX0_QLG_raw", "Q_{X0}/Q_{LG} raw", qX0OverLGGRaw, dqX0OverLGGRaw, 20, kRed + 2 );
    AddTrend( "gQY0_QLG_raw", "Q_{Y0}/Q_{LG} raw", qY0OverLGGRaw, dqY0OverLGGRaw, 20, kRed + 2 );
    AddTrend( "gQX0_QLG_cal", "Q_{X0}/Q_{LG} calibrated", qX0OverLGGCal, dqX0OverLGGCal, 20, kBlue + 2 );

    TGraphErrors* gQXRatioCal = AddTrend( "gQXratio_cal", "Q_{X-Tar}/Q_{LG} calibrated", qXRatioCal, dqXRatioCal, 20, kBlue + 2 );

    AddTrend( "gQXratio_raw", "Q_{X-Tar}/Q_{LG} raw", qXRatioRaw, dqXRatioRaw, 20, kRed + 2 );
    AddTrend( "gQYratio_raw", "Q_{Y-Tar}/Q_{LG} raw", qYRatioRaw, dqYRatioRaw, 20, kRed + 2 );
    AddTrend( "gQXratio_raw_ECorr", "Q_{X-Tar}/Q_{LG} raw energy-corrected", qXRatioRawEcorr, dqXRatioRawEcorr, 20, kRed + 2 );
    AddTrend( "gQYratio_raw_ECorr", "Q_{Y-Tar}/Q_{LG} raw energy-corrected", qYRatioRawEcorr, dqYRatioRawEcorr, 20, kRed + 2 );
    AddTrend( "gQXratio_cal_ECorr", "Q_{X-Tar}/Q_{LG} calibrated energy-corrected", qXRatioCalEcorr, dqXRatioCalEcorr, 20, kBlue + 2 );
    AddTrend( "gsigmaXtar_raw", "#sigma_{X} raw [mm]", sigmaXRaw, dSigmaXRaw, 20, kRed + 2 );
    AddTrend( "gsigmaYtar_raw", "#sigma_{Y} raw [mm]", sigmaYRaw, dSigmaYRaw, 20, kRed + 2 );
    AddTrend( "gsigmaXtar", "#sigma_{X} calibrated [mm]", sigmaXCal, dSigmaXCal, 20, kBlue + 2 );

    const vector<double> zeroErrors( n, 0.0 );

    AddTrend( "gChi2_X_raw", "Minimized FCN X raw", chi2XRaw, zeroErrors, 20, kRed + 2 );
    AddTrend( "gChi2_Y_raw", "Minimized FCN Y raw", chi2YRaw, zeroErrors, 20, kRed + 2 );
    AddTrend( "gChi2_X", "Minimized FCN X calibrated", chi2XCal, zeroErrors, 20, kBlue + 2 );

    // Backward-compatible graph names: these are covariance statuses.
    AddTrend( "gFitStatus_X_raw", "Covariance status X raw", covXRaw, zeroErrors, 20, kRed + 2 );
    AddTrend( "gFitStatus_Y_raw", "Covariance status Y raw", covYRaw, zeroErrors, 20, kRed + 2 );
    AddTrend( "gFitStatus_X", "Covariance status X calibrated", covXCal, zeroErrors, 20, kBlue + 2 );
    AddTrend( "gMigradStatus_X_raw", "MIGRAD status X raw", migradXRaw, zeroErrors, 20, kRed + 2 );
    AddTrend( "gMigradStatus_Y_raw", "MIGRAD status Y raw", migradYRaw, zeroErrors, 20, kRed + 2 );
    AddTrend( "gMigradStatus_X", "MIGRAD status X calibrated", migradXCal, zeroErrors, 20, kBlue + 2 );
    AddTrend( "gChargeX_raw_fitted", "Q_{X-Tar} fitted raw [pC]", fittedQXRaw, dFittedQXRaw, 20, kRed + 2 );
    AddTrend( "gChargeY_raw_fitted", "Q_{Y-Tar} fitted raw [pC]", fittedQYRaw, dFittedQYRaw, 20, kRed + 2 );
    AddTrend( "gChargeX_fitted", "Q_{X-Tar} fitted calibrated [pC]", fittedQXCal, dFittedQXCal, 20, kBlue + 2 );
    AddTrend( "gChargeXratio_fitted_raw", "Q_{X-Tar}/Q_{LG} fitted raw", fittedRatioXRaw, dFittedRatioXRaw, 20, kRed + 2 );
    AddTrend( "gChargeYratio_fitted_raw", "Q_{Y-Tar}/Q_{LG} fitted raw", fittedRatioYRaw, dFittedRatioYRaw, 20, kRed + 2 );
    AddTrend( "gChargeXratio_fitted_cal", "Q_{X-Tar}/Q_{LG} fitted calibrated", fittedRatioXCal, dFittedRatioXCal, 20, kBlue + 2 );
    AddTrend( "gChargeXratio_fitted_raw_ECorr", "Q_{X-Tar}/Q_{LG} fitted raw energy-corrected", fittedRatioXRawEcorr, dFittedRatioXRawEcorr, 20, kRed + 2 );
    AddTrend( "gChargeYratio_fitted_raw_ECorr", "Q_{Y-Tar}/Q_{LG} fitted raw energy-corrected", fittedRatioYRawEcorr, dFittedRatioYRawEcorr, 20, kRed + 2 );
    AddTrend( "gChargeXratio_fitted_cal_ECorr", "Q_{X-Tar}/Q_{LG} fitted calibrated energy-corrected", fittedRatioXCalEcorr, dFittedRatioXCalEcorr, 20, kBlue + 2 );

    // -------------------------------------------------------------------------
    // Normalized stability trends.
    // Keep the old pol0-normalization concept.
    // -------------------------------------------------------------------------

    TF1 p0LG( "p0LG", "pol0", 0.0, static_cast<double>(n) );
    TF1 p0XCal( "p0XCal", "pol0", 0.0, static_cast<double>(n) );
    TF1 p0XRaw( "p0XRaw", "pol0", 0.0, static_cast<double>(n) );
    TF1 p0RatioCal( "p0RatioCal", "pol0", 0.0, static_cast<double>(n) );

    gQLG->Fit( &p0LG, "Q0R" );
    gQXTotCal->Fit( &p0XCal, "Q0R" );
    gQXTotRaw->Fit( &p0XRaw, "Q0R" );
    gQXRatioCal->Fit( &p0RatioCal, "Q0R" );

    vector<double> qLGNorm(n), dqLGNorm(n);
    vector<double> qXCalNorm(n), dqXCalNorm(n);
    vector<double> qXRawNorm(n), dqXRawNorm(n);
    vector<double> qXRatioCalNorm(n), dqXRatioCalNorm(n);

    const double normLG = p0LG.GetParameter(0);
    const double normXCal = p0XCal.GetParameter(0);
    const double normXRaw = p0XRaw.GetParameter(0);
    const double normRatioCal = p0RatioCal.GetParameter(0);

    for (size_t i = 0; i < n; ++i) {
        qLGNorm[i] = SafeRatio( qLG[i], normLG );
        dqLGNorm[i] = normLG != 0.0 ? dqLG[i] / fabs(normLG) : 0.0;
        qXCalNorm[i] = SafeRatio( qXTotCal[i], normXCal );
        dqXCalNorm[i] = normXCal != 0.0 ? dqXTotCal[i] / fabs(normXCal) : 0.0;
        qXRawNorm[i] = SafeRatio( qXTotRaw[i], normXRaw );
        dqXRawNorm[i] = normXRaw != 0.0 ? dqXTotRaw[i] / fabs(normXRaw) : 0.0;
        qXRatioCalNorm[i] = SafeRatio( qXRatioCal[i], normRatioCal );
        dqXRatioCalNorm[i] = normRatioCal != 0.0 ? dqXRatioCal[i] / fabs(normRatioCal) : 0.0;
    }

    AddTrend( "gQLGNorm", "Q_{LG}/<Q_{LG}>", qLGNorm, dqLGNorm, 20, kBlue + 2 );
    AddTrend( "gQX_calNorm", "Q_{X-Tar}^{cal}/<Q_{X-Tar}^{cal}>", qXCalNorm, dqXCalNorm, 20, kBlue + 2 );
    AddTrend( "gQX_raw_Norm", "Q_{X-Tar}^{raw}/<Q_{X-Tar}^{raw}>", qXRawNorm, dqXRawNorm, 20, kBlue + 2 );
    AddTrend( "gQXLGRatio_Norm", "(Q_{X-Tar}/Q_{LG})/<Q_{X-Tar}/Q_{LG}>", qXRatioCalNorm, dqXRatioCalNorm, 20, kBlue + 2 );

    trendsPeriodDir->cd();

    for (TGraphErrors* graph : periodTrends) {
        graph->Write();
    }

    // -------------------------------------------------------------------------
    // Timestamp trends: exactly the same Y values/errors as the period trends.
    // -------------------------------------------------------------------------

    trendsTimeDir->cd();

    vector<TGraphErrors*> timeTrends;
    timeTrends.reserve( periodTrends.size() );

    for (const TGraphErrors* graph : periodTrends) {
        TGraphErrors* timeGraph = CloneTrendVsTime( graph, meanTimes, timeHalfWidths );
        timeGraph->Write();
        timeTrends.push_back(timeGraph);
    }

    // -------------------------------------------------------------------------
    // Whole-run summary
    // -------------------------------------------------------------------------

    const double qXTotalRaw = summary.qXRaw[0] + summary.qXRaw[1] + summary.qXRaw[2];

    const double dqXTotalRaw = Quadrature3( summary.dqXRaw );

    const double qYTotalRaw = summary.qYRaw[0] + summary.qYRaw[1] + summary.qYRaw[2];

    const double dqYTotalRaw = Quadrature3( summary.dqYRaw );

    const double qXTotalCal = summary.qXCal[0] + summary.qXCal[1] + summary.qXCal[2];

    const double dqXTotalCal = Quadrature3( summary.dqXCal );

    const double qLGEcorrSummary = summary.qLG * kCalibrationEnergyMeV / beamEnergyMeV;

    const double dqLGEcorrSummary = summary.dqLG * fabs( kCalibrationEnergyMeV / beamEnergyMeV );

    const double qXRatioRawSummary = SafeRatio( qXTotalRaw, summary.qLG );

    const double dqXRatioRawSummary = RatioError( qXTotalRaw, dqXTotalRaw, summary.qLG, summary.dqLG );

    const double qYRatioRawSummary = SafeRatio( qYTotalRaw, summary.qLG );

    const double dqYRatioRawSummary = RatioError( qYTotalRaw, dqYTotalRaw, summary.qLG, summary.dqLG );

    const double qXRatioCalSummary = SafeRatio( qXTotalCal, summary.qLG );

    const double dqXRatioCalSummary = RatioError( qXTotalCal, dqXTotalCal, summary.qLG, summary.dqLG );

    const double qXFitRawSummary = summary.fitXRaw.par[0];

    const double dqXFitRawSummary = fabs( summary.fitXRaw.err[0] );

    const double qYFitRawSummary = summary.fitYRaw.par[0];

    const double dqYFitRawSummary = fabs( summary.fitYRaw.err[0] );

    const double qXFitCalSummary = summary.fitXCal.par[0];

    const double dqXFitCalSummary = fabs( summary.fitXCal.err[0] );
    const double qXFitRatioRawSummary = SafeRatio( qXFitRawSummary, summary.qLG );
    const double dqXFitRatioRawSummary = RatioError( qXFitRawSummary, dqXFitRawSummary, summary.qLG, summary.dqLG );
    const double qYFitRatioRawSummary = SafeRatio( qYFitRawSummary, summary.qLG );
    const double dqYFitRatioRawSummary = RatioError( qYFitRawSummary, dqYFitRawSummary, summary.qLG, summary.dqLG );
    const double qXFitRatioCalSummary = SafeRatio( qXFitCalSummary, summary.qLG );
    const double dqXFitRatioCalSummary = RatioError( qXFitCalSummary, dqXFitCalSummary, summary.qLG, summary.dqLG );
    const double ratioEnergyScale = beamEnergyMeV / kCalibrationEnergyMeV;
    const double xWRawSummary = PadmeXmm( centerXPhys, summary.muXWRaw );
    const double dxWRawSummary = fabs( summary.dMuXWRaw ) * kStripPitchMm;
    const double yWRawSummary = PadmeYmm( centerYPhys, summary.muYWRaw );
    const double dyWRawSummary = fabs( summary.dMuYWRaw ) * kStripPitchMm;
    const double xWCalSummary = PadmeXmm( centerXPhys, summary.muXWCal );
    const double dxWCalSummary = fabs( summary.dMuXWCal ) * kStripPitchMm;
    const double xFitRawSummary = PadmeXmm( centerXPhys, summary.fitXRaw.par[1] );
    const double dxFitRawSummary = fabs( summary.fitXRaw.err[1] ) * kStripPitchMm;
    const double yFitRawSummary = PadmeYmm( centerYPhys, summary.fitYRaw.par[1] );
    const double dyFitRawSummary = fabs( summary.fitYRaw.err[1] ) * kStripPitchMm;
    const double xFitCalSummary = PadmeXmm( centerXPhys, summary.fitXCal.par[1] );
    const double dxFitCalSummary = fabs( summary.fitXCal.err[1] ) * kStripPitchMm;
    const double sigmaXRawSummary = max( fabs(summary.fitXRaw.par[2]), widthFloor );
    const double dSigmaXRawSummary = fabs(summary.fitXRaw.par[2]) > widthFloor ? fabs(summary.fitXRaw.err[2]) : 0.0;
    const double sigmaYRawSummary = max( fabs(summary.fitYRaw.par[2]), widthFloor );
    const double dSigmaYRawSummary = fabs(summary.fitYRaw.par[2]) > widthFloor ? fabs(summary.fitYRaw.err[2]) : 0.0;
    const double sigmaXCalSummary = max( fabs(summary.fitXCal.par[2]), widthFloor );
    const double dSigmaXCalSummary = fabs(summary.fitXCal.par[2]) > widthFloor ? fabs(summary.fitXCal.err[2]) : 0.0;

    summaryDir->cd();

    vector<TGraphErrors*> summaryGraphs;

    auto AddSummary = [&](const TString& name, const TString& yLabel, double value, double error, int markerStyle, int color) {
            TGraphErrors* graph = MakeOnePointGraph( name, yLabel, beamEnergyMeV, value, error, markerStyle, color );
            summaryGraphs.push_back( graph );

            return graph;
        };

    AddSummary( "gQLGTot", "Q_{LG} whole run [pC]", summary.qLG, summary.dqLG, 20, kBlue + 2 );
    AddSummary( "gQLGTot_ECorr", "Q_{LG} whole run energy-corrected [pC]", qLGEcorrSummary, dqLGEcorrSummary, 20, kBlue + 2 );
    AddSummary( "gQXTot_raw", "Q_{X-Tar} raw whole run [pC]", qXTotalRaw, dqXTotalRaw, 20, kRed + 2 );
    AddSummary( "gQYTot_raw", "Q_{Y-Tar} raw whole run [pC]", qYTotalRaw, dqYTotalRaw, 20, kRed + 2 );
    AddSummary( "gQXTot_cal", "Q_{X-Tar} calibrated whole run [pC]", qXTotalCal, dqXTotalCal, 20, kBlue + 2 );
    AddSummary( "gQXTot_fitted_raw", "Q_{X-Tar} fitted raw whole run [pC]", qXFitRawSummary, dqXFitRawSummary, 20, kRed + 2 );
    AddSummary( "gQYTot_fitted_raw", "Q_{Y-Tar} fitted raw whole run [pC]", qYFitRawSummary, dqYFitRawSummary, 20, kRed + 2 );
    AddSummary( "gQXTot_fitted_cal", "Q_{X-Tar} fitted calibrated whole run [pC]", qXFitCalSummary, dqXFitCalSummary, 20, kBlue + 2 );
    AddSummary( "gmeanXTot_raw", "X_{wmean} raw whole run [mm]", xWRawSummary, dxWRawSummary, 20, kRed + 2 );
    AddSummary( "gmeanYTot_raw", "Y_{wmean} raw whole run [mm]", yWRawSummary, dyWRawSummary, 20, kRed + 2 );
    AddSummary( "gmeanXTot_cal", "X_{wmean} calibrated whole run [mm]", xWCalSummary, dxWCalSummary, 20, kBlue + 2 );
    AddSummary( "gmeanXTot_fitted_raw", "X_{fit} raw whole run [mm]", xFitRawSummary, dxFitRawSummary, 20, kRed + 2 );
    AddSummary( "gmeanYTot_fitted_raw", "Y_{fit} raw whole run [mm]", yFitRawSummary, dyFitRawSummary, 20, kRed + 2 );
    AddSummary( "gmeanXTot_fitted_cal", "X_{fit} calibrated whole run [mm]", xFitCalSummary, dxFitCalSummary, 20, kBlue + 2 );
    AddSummary( "gsigmaXTot_fitted_raw", "#sigma_{X} fitted raw whole run [mm]", sigmaXRawSummary, dSigmaXRawSummary, 20, kRed + 2 );
    AddSummary( "gsigmaYTot_fitted_raw", "#sigma_{Y} fitted raw whole run [mm]", sigmaYRawSummary, dSigmaYRawSummary, 20, kRed + 2 );
    AddSummary( "gsigmaXTot_fitted_cal", "#sigma_{X} fitted calibrated whole run [mm]", sigmaXCalSummary, dSigmaXCalSummary, 20, kBlue + 2 );
    AddSummary( "gRX_raw", "Q_{X-Tar}/Q_{LG} raw whole run", qXRatioRawSummary, dqXRatioRawSummary, 20, kRed + 2 );
    AddSummary( "gRY_raw", "Q_{Y-Tar}/Q_{LG} raw whole run", qYRatioRawSummary, dqYRatioRawSummary, 20, kRed + 2 );
    AddSummary( "gRX_cal", "Q_{X-Tar}/Q_{LG} calibrated whole run", qXRatioCalSummary, dqXRatioCalSummary, 20, kBlue + 2 );
    AddSummary( "gXR_fitted_raw", "Q_{X-Tar}/Q_{LG} fitted raw whole run", qXFitRatioRawSummary, dqXFitRatioRawSummary, 20, kRed + 2 );
    AddSummary( "gYR_fitted_raw", "Q_{Y-Tar}/Q_{LG} fitted raw whole run", qYFitRatioRawSummary, dqYFitRatioRawSummary, 20, kRed + 2 );
    AddSummary( "gXR_fitted_cal", "Q_{X-Tar}/Q_{LG} fitted calibrated whole run", qXFitRatioCalSummary, dqXFitRatioCalSummary, 20, kBlue + 2 );
    AddSummary( "gRX_raw_ECorr", "Q_{X-Tar}/Q_{LG} raw whole run energy-corrected", qXRatioRawSummary * ratioEnergyScale, dqXRatioRawSummary * fabs(ratioEnergyScale), 20, kRed + 2 );
    AddSummary( "gRY_raw_ECorr", "Q_{Y-Tar}/Q_{LG} raw whole run energy-corrected", qYRatioRawSummary * ratioEnergyScale, dqYRatioRawSummary * fabs(ratioEnergyScale), 20, kRed + 2 );
    AddSummary( "gRX_cal_ECorr", "Q_{X-Tar}/Q_{LG} calibrated whole run energy-corrected", qXRatioCalSummary * ratioEnergyScale, dqXRatioCalSummary * fabs(ratioEnergyScale), 20, kBlue + 2 );
    AddSummary( "gXR_fitted_raw_ECorr", "Q_{X-Tar}/Q_{LG} fitted raw whole run energy-corrected", qXFitRatioRawSummary * ratioEnergyScale, dqXFitRatioRawSummary * fabs(ratioEnergyScale), 20, kGreen + 2 );
    AddSummary( "gYR_fitted_raw_ECorr", "Q_{Y-Tar}/Q_{LG} fitted raw whole run energy-corrected", qYFitRatioRawSummary * ratioEnergyScale, dqYFitRatioRawSummary * fabs(ratioEnergyScale), 20, kGreen + 2 );
    AddSummary( "gXR_fitted_cal_ECorr", "Q_{X-Tar}/Q_{LG} fitted calibrated whole run energy-corrected", qXFitRatioCalSummary * ratioEnergyScale, dqXFitRatioCalSummary * fabs(ratioEnergyScale), 20, kBlue + 2 );

    for (TGraphErrors* graph : summaryGraphs) {
        graph->Write();
    }

    // -------------------------------------------------------------------------
    // Text summary
    // -------------------------------------------------------------------------

    ofstream textOutput( outputTxt );

    if (!textOutput.is_open()) {
        cerr << "WARNING: cannot create text output " << outputTxt << endl;
    }
    else {

        textOutput << fixed << setprecision(6);
        textOutput << "Run Number: " << runNumber << " - Ebeam: " << beamEnergyMeV << " MeV\n";
        textOutput << "Input ROOT: " << inputRoot << "\n";
        textOutput << "Complete reconstruction blocks: " << blocks.size() << "\n";
        textOutput << "Trend periods: " << nPeriods << " (" << kBlocksPerPeriod << " blocks/period)\n";
        textOutput << "Central physical strips: X=" << centerXPhys << " Y=" << centerYPhys << "\n";
        textOutput << "Run time interval UNIX: " << blocks.front().firstTimeUnix << " -> " << blocks.back().lastTimeUnix << "\n";
        textOutput << "QLG: " << summary.qLG << " +/- " << summary.dqLG << " pC\n";
        textOutput << "QXTar raw(-1,0,+1): (" << summary.qXRaw[0] << " +/- " << summary.dqXRaw[0] << ", " << summary.qXRaw[1] << " +/- " << summary.dqXRaw[1] << ", " << summary.qXRaw[2] << " +/- " << summary.dqXRaw[2] << ") pC\n";
        textOutput << "QYTar raw(-1,0,+1): (" << summary.qYRaw[0] << " +/- " << summary.dqYRaw[0] << ", " << summary.qYRaw[1] << " +/- " << summary.dqYRaw[1] << ", " << summary.qYRaw[2] << " +/- " << summary.dqYRaw[2] << ") pC\n";
        textOutput << "QXTar calibrated(-1,0,+1): (" << summary.qXCal[0] << " +/- " << summary.dqXCal[0] << ", " << summary.qXCal[1] << " +/- " << summary.dqXCal[1] << ", " << summary.qXCal[2] << " +/- " << summary.dqXCal[2] << ") pC\n";
        textOutput << "QX total raw: " << qXTotalRaw << " +/- " << dqXTotalRaw << " pC\n";
        textOutput << "QY total raw: " << qYTotalRaw << " +/- " << dqYTotalRaw << " pC\n";
        textOutput << "QX total calibrated: " << qXTotalCal << " +/- " << dqXTotalCal << " pC\n";
        textOutput << "QX fitted raw: " << qXFitRawSummary << " +/- " << dqXFitRawSummary << " pC\n";
        textOutput << "QY fitted raw: " << qYFitRawSummary << " +/- " << dqYFitRawSummary << " pC\n";
        textOutput << "QX fitted calibrated: " << qXFitCalSummary << " +/- " << dqXFitCalSummary << " pC\n";
        textOutput << "X weighted mean raw: " << xWRawSummary << " +/- " << dxWRawSummary << " mm\n";
        textOutput << "Y weighted mean raw: " << yWRawSummary << " +/- " << dyWRawSummary << " mm\n";
        textOutput << "X weighted mean calibrated: " << xWCalSummary << " +/- " << dxWCalSummary << " mm\n";
        textOutput << "X fitted raw: " << xFitRawSummary << " +/- " << dxFitRawSummary << " mm\n";
        textOutput << "Y fitted raw: " << yFitRawSummary << " +/- " << dyFitRawSummary << " mm\n";
        textOutput << "X fitted calibrated: " << xFitCalSummary << " +/- " << dxFitCalSummary << " mm\n";
        textOutput << "sigmaX raw: " << sigmaXRawSummary << " +/- " << dSigmaXRawSummary << " mm\n";
        textOutput << "sigmaY raw: " << sigmaYRawSummary << " +/- " << dSigmaYRawSummary << " mm\n";
        textOutput << "sigmaX calibrated: " << sigmaXCalSummary << " +/- " << dSigmaXCalSummary << " mm\n";
        textOutput << "QXtot/QLG raw: " << qXRatioRawSummary << " +/- " << dqXRatioRawSummary << "\n";
        textOutput << "QYtot/QLG raw: " << qYRatioRawSummary << " +/- " << dqYRatioRawSummary << "\n";
        textOutput << "QXtot/QLG calibrated: " << qXRatioCalSummary << " +/- " << dqXRatioCalSummary << "\n";
        textOutput << "QXfit/QLG raw: " << qXFitRatioRawSummary << " +/- " << dqXFitRatioRawSummary << "\n";
        textOutput << "QYfit/QLG raw: " << qYFitRatioRawSummary << " +/- " << dqYFitRatioRawSummary << "\n";
        textOutput << "QXfit/QLG calibrated: " << qXFitRatioCalSummary << " +/- " << dqXFitRatioCalSummary << "\n";
        textOutput << "Global fit status X raw: MIGRAD=" << summary.fitXRaw.gof.migradStatus << " covariance=" << summary.fitXRaw.gof.covStatus << "\n";
        textOutput << "Global fit status Y raw: MIGRAD=" << summary.fitYRaw.gof.migradStatus << " covariance=" << summary.fitYRaw.gof.covStatus << "\n";
        textOutput << "Global fit status X calibrated: MIGRAD=" << summary.fitXCal.gof.migradStatus << " covariance=" << summary.fitXCal.gof.covStatus << "\n";
    }

    output->Write();
    output->Close();
    delete output;

    cout << "Analysis ROOT file: " << outputRoot << endl;
    cout << "Analysis text file: " << outputTxt << endl;

    return true;
}

// =============================================================================
// CLI
// =============================================================================

void PrintUsage(const char* executable) {
    cerr << "Usage:\n  " << executable << " <runNumber> <ebeam_MeV>" << " <inputReco.root>" << " <calibration.txt>" << " <output.root>" << " <output.txt>\n\n" << "Example:\n  " << executable << " 80555 298.5 " << "outputReco/Reco_run_0080555.root " << "CalibTarget/outputCalibration/TargetCalibrationConst_NewCharge2.txt " << "CalibTarget/outputCalibration/Run4Monitor/TargetAnalysis_run_0080555_v2.root " << "CalibTarget/outputCalibration/Run4Monitor/TargetAnalysis_run_0080555_v2.txt\n";
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 7) {
        PrintUsage(argv[0]);
        return 1;
    }

    char* endRun = nullptr;
    const long runLong = strtol( argv[1], &endRun, 10 );
    if (!endRun || *endRun != '\0' || runLong <= 0) {
        cerr << "ERROR: runNumber must be a positive integer. Got '" << argv[1] << "'." << endl;
        return 1;
    }

    char* endEnergy = nullptr;
    const double beamEnergyMeV = strtod( argv[2], &endEnergy );

    if (!endEnergy || *endEnergy != '\0' || !isfinite(beamEnergyMeV) || beamEnergyMeV <= 0.0) {
        cerr << "ERROR: ebeam must be a positive number. Got '" << argv[2] << "'." << endl;
        return 1;
    }

    const int runNumber = static_cast<int>( runLong );
    const string inputRoot = argv[3];
    const string calibrationFile = argv[4];
    const string outputRoot = argv[5];
    const string outputTxt = argv[6];

    cout << "Run       : " << runNumber << "\nEbeam     : " << beamEnergyMeV << " MeV" << "\nInput     : " << inputRoot << "\nCalibration: " << calibrationFile << "\nOutput ROOT: " << outputRoot << "\nOutput TXT : " << outputTxt << endl;
    const bool ok = AnalyseRun( runNumber, beamEnergyMeV, inputRoot, calibrationFile, outputRoot, outputTxt );

    return ok ? 0 : 1;
}
