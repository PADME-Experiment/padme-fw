// ============================================================================
// AnalysisTMM.C - TIME-BINNED MICROMEGAS RECONSTRUCTION ANALYSIS
// ============================================================================
// PURPOSE
//   Read the reconstructed 3D charge histograms and extract the beam position,
//   width and charge using both direct histogram statistics and spatial fits.
//   Produce time trends, distributions, correlations and comparisons
//   between RAW, external- and run-calibrated reconstructions.
//   Print whole-run results to the console instead of storing ROOT scalars.
//
// EXPECTED INPUT ROOT FILE
//   Overall/Raw/hqmaxstripFullX, hqmaxstripFullY
//   Overall/Calib_external/hqmaxstripFull_extcalX, ...Y
//   Overall/Calib_run/hqmaxstripFull_runcalX, ...Y
//   Overall/g_DaqTime_iev : tree-entry index versus absolute DAQ time [s]
//
// INPUT TH3F AXES
//   X = strip position [mm]  (already converted during reconstruction)
//   Y = DAQ timestamp [s]  (defines time-bin boundaries)
//   Z = peak charge [ADC]  (converted to pC only in the analysis)
//
// ANALYSIS FLOW
//   1) Read common axes from RAW-X TH3F and count DAQ events per time bin
//   2) For each calibration mode and readout, select each TH3F time bin
//   3) Project3D("zx") -> TH2D(horizontal=position, vertical=charge)
//   4) Direct method: hit-weighted position/RMS and charge sum per DAQ event
//   5) FitSlicesY: Gaussian charge fit at each strip -> mean-charge profile
//   6) Fit the profile with a Gaussian, double Gaussian and Voigt model
//   7) Repeat measurements on the whole-run projection and print a summary
//   8) Write time trends, distributions, correlations and comparisons
//      FitDiagnostics contains slice histograms only (no fitted TF1 objects)
// ============================================================================

#include "TFile.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TAxis.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TMatrixDSym.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TParameter.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <utility>
#include <iostream>
#include <iomanip>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

namespace AnalysisTMMDetail {

  // ---- Configuration: detector conversion, measurement modes and fit cuts ----
  // ADC_TO_PC = 4.8e-5 pC/ADC. The TH3 charge axis and slice means remain in ADC.
  constexpr double ADC_TO_PC = 300. * 1.6e-19 * 1.e12;
  constexpr int N_MODE = 3;
  constexpr int N_VIEW = 2;
  constexpr int N_MODEL = 3;

  // Minimum strip-hit count used to accept a profile bin AFTER FitSlicesY.
  // CHECK: lowering this to 1 does not remove ROOT FitSlicesY fit requirements.
  constexpr int MIN_SLICE_HITS = 1;
  constexpr int MIN_PROFILE_STRIPS = 8;
  constexpr int MIN_FIT_COV_STATUS = 2;
  constexpr int MAX_FIT_STATUS = 1;
  constexpr bool WRITE_SLICE_HISTOGRAMS = true;

  const char *MODE_NAME[N_MODE] = {"Raw", "Calib_external", "Calib_run"};
  const char *VIEW_NAME[N_VIEW] = {"X", "Y"};
  const char *MODEL_NAME[N_MODEL] = {"SingleGaussian", "DoubleGaussian", "Voigt"};

  // Non-finite placeholder. IsGood() suppresses unavailable results in trends.
  const double kNaN = numeric_limits<double>::quiet_NaN();

  // ---- Direct observables, without fitting the spatial charge profile ----
  // Position/RMS = strip-hit weighted; chargePosition/chargeRMS = charge weighted.
  // charge and chargeErr are expressed in pC per DAQ event.
  struct DirectResult {
    bool valid = false;
    Long64_t events = 0;
    double stripHits = 0.;
    double position = kNaN;
    double positionErr = kNaN;
    double rms = kNaN;
    double rmsErr = kNaN;
    double chargePosition = kNaN;
    double chargePositionErr = kNaN;
    double chargeRMS = kNaN;
    double chargeRMSErr = kNaN;
    double charge = kNaN;
    double chargeErr = kNaN;
  }; // end struct DirectResult

  // ---- Observables from a fitted mean-charge-versus-position profile ----
  // Width conventions depend on the model (documented in FitModel).
  // profileArea = fitted integral [pC mm]; modelCharge = occupancy-based estimate [pC/event].
  struct ModelResult {
    bool valid = false;
    int status = -9999;
    int covariance = -9999;
    int nProfileStrips = 0;
    double position = kNaN;
    double positionErr = kNaN;
    double sigma = kNaN;
    double sigmaErr = kNaN;
    double sigmaCore = kNaN;
    double sigmaCoreErr = kNaN;
    double sigmaTail = kNaN;
    double sigmaTailErr = kNaN;
    double sigmaSingleCore = kNaN;
    double sigmaSingleCoreErr = kNaN;
    double sigmaFWHM = kNaN;
    double sigmaFWHMErr = kNaN;
    double sigmaTruncated = kNaN;
    double sigmaTruncatedErr = kNaN;
    double profileArea = kNaN;
    double profileAreaErr = kNaN;
    double modelCharge = kNaN;
    double modelChargeErr = kNaN;
  }; // end struct ModelResult

  // A single measurement combines direct histogram statistics and three fits.
  struct Measurement {
    DirectResult direct;
    ModelResult fit[N_MODEL];
  }; // end struct Measurement

  // Cache a time-trend graph and the ROOT directory in which to write it.
  struct OutputGraph {
    unique_ptr<TGraphErrors> graph;
    TDirectory *directory = nullptr;
  }; // end struct OutputGraph

  // Graph registry keyed by full ROOT path; graphs are filled incrementally.
  map<string, OutputGraph> outputGraphs;

  // Return true for a usable finite observable; reject NaN and infinities.
  bool IsGood(double x) {
    return std::isfinite(x);
  } // end function IsGood()

  // ---- ROOT output utilities ----
  // Create (if necessary) each directory along a slash-delimited ROOT path.
  // Return the final directory or nullptr if it cannot be created.
  TDirectory *EnsureDirectory(TDirectory *root, const TString &relative) {
    if (!root) return nullptr;

    TDirectory *dir = root;
    TString path = relative;
    TObjArray *pieces = path.Tokenize("/");
    if (!pieces) return dir;

    // Walk from the output ROOT file to the requested leaf directory.
    for (int i = 0; i < pieces->GetEntriesFast(); ++i) {
      TString part = pieces->At(i)->GetName();
      if (part.IsNull()) continue;

      TDirectory *next = dir->GetDirectory(part.Data());
      if (!next) next = dir->mkdir(part.Data());
      if (!next) {
        delete pieces;
        return nullptr;
      } // end if (!next)

      dir = next;
    } // end for i

    delete pieces;
    return dir;
  } // end function EnsureDirectory()

  // Add one valid (time,observable) point, including both x and y error bars.
  // The graph is created lazily and retained in outputGraphs until file writing.
  void AddTrend(TFile *output, const TString &folder, const TString &observable, const TString &yTitle, double time, double timeErr, double value, double valueErr) {
    if (!output || !IsGood(time) || !IsGood(value)) return;

    TString path = "TimeTrends/" + folder;
    TString objectName = "g_" + observable;
    string key = string(path.Data()) + "/" + objectName.Data();
    auto it = outputGraphs.find(key);

    // First point for this observable: allocate its graph and output folder.
    if (it == outputGraphs.end()) {
      TDirectory *dir = EnsureDirectory(output, path);
      if (!dir) return;

      TGraphErrors *g = new TGraphErrors();
      g->SetName(objectName.Data());
      g->SetTitle(Form("%s;Time [s];%s", observable.Data(), yTitle.Data()));
      g->SetMarkerStyle(20);
      outputGraphs[key].graph.reset(g);
      outputGraphs[key].directory = dir;
      it = outputGraphs.find(key);
    } // end if (it == outputGraphs.end())

    TGraphErrors *g = it->second.graph.get();
    int n = g->GetN();
    g->SetPoint(n, time, value);
    g->SetPointError(n, max(0., timeErr), IsGood(valueErr) && valueErr >= 0. ? valueErr : 0.);
  } // end function AddTrend()

  // Add a time-dependent comparison (difference or ratio), without a y error.
  // CAUTION: comparison-graph uncertainties are not propagated in this version.
  void AddComparison(TFile *output, const TString &folder, const TString &observable, const TString &yTitle, double time, double timeErr, double value) {
    if (!output || !IsGood(value) || !IsGood(time)) return;

    TString path = "Comparisons/" + folder;
    TString objectName = "g_" + observable;
    string key = string(path.Data()) + "/" + objectName.Data();
    auto it = outputGraphs.find(key);

    // First point for this observable: allocate its graph and output folder.
    if (it == outputGraphs.end()) {
      TDirectory *dir = EnsureDirectory(output, path);
      if (!dir) return;

      TGraphErrors *g = new TGraphErrors();
      g->SetName(objectName.Data());
      g->SetTitle(Form("%s;Time [s];%s", observable.Data(), yTitle.Data()));
      g->SetMarkerStyle(20);
      outputGraphs[key].graph.reset(g);
      outputGraphs[key].directory = dir;
      it = outputGraphs.find(key);
    } // end if (it == outputGraphs.end())

    TGraphErrors *g = it->second.graph.get();
    int n = g->GetN();
    g->SetPoint(n, time, value);
    g->SetPointError(n, max(0., timeErr), 0.);
  } // end function AddComparison()

  // ============================================================================
  // DIRECT CHARGE: THE REQUESTED CHARGE-WEIGHTED HISTOGRAM INTEGRAL
  // ============================================================================
  // h->X = strip position [mm], h->Y = strip charge [ADC].
  // charge = sum_over_bins(Nhits * charge_bin_center) * ADC_TO_PC [pC].
  // chargeErr = sqrt(sum_over_bins(Nhits * charge_bin_center^2)) * ADC_TO_PC.
  // There is NO /Nevents here: that normalization is applied in MeasureDirect().
  // TH2 base type accepts the TH2D produced by Project3D("zx").
  void TH2WeightedCharge(const TH2 *h, double &charge, double &chargeErr) {
    charge = 0.;
    chargeErr = 0.;
    if (!h) return;

    double err2 = 0.;

    // Integrate only regular position bins (exclude underflow and overflow).
    for (int bx = 1; bx <= h->GetNbinsX(); ++bx) {
      // Sum all regular charge bins at the current strip position.
      for (int by = 1; by <= h->GetNbinsY(); ++by) {
        const double n = h->GetBinContent(bx, by);
        if (n == 0.) continue;

        const double q = h->GetYaxis()->GetBinCenter(by);
        charge += n * q;
        err2 += fabs(n) * q * q;
      } // end for by
    } // end for bx

    charge *= ADC_TO_PC;
    chargeErr = sqrt(max(0.0, err2)) * ADC_TO_PC;
  } // end function TH2WeightedCharge()

  // ============================================================================
  // DIRECT PHYSICS ESTIMATORS FROM ONE POSITION-CHARGE TH2D
  // ============================================================================
  // Calculate the collected charge per DAQ event (all strips), and two distinct
  // position/width pairs: hit-weighted and charge-weighted.
  // hitsByStrip stores strip occupancy for the subsequent model-charge estimate.
  DirectResult MeasureDirect(const TH2D *h, Long64_t events, vector<double> *hitsByStrip = nullptr) {
    DirectResult result;
    result.events = events;
    if (hitsByStrip && h) hitsByStrip->assign(h->GetNbinsX() + 1, 0.);
    if (!h || events <= 0) return result;

    double charge = 0.;
    double chargeErr = 0.;

    // Total collected charge [pC]; normalize by ALL readable DAQ events.
    TH2WeightedCharge(h, charge, chargeErr);
    result.charge = charge / (double)events;
    result.chargeErr = chargeErr / (double)events;

    double nHits = 0.;
    double sumX = 0.;
    double sumX2 = 0.;
    double sumQ = 0.;
    double sumQ2 = 0.;
    double sumQX = 0.;
    double sumQX2 = 0.;

    // Integrate only regular position bins (exclude underflow and overflow).
    for (int bx = 1; bx <= h->GetNbinsX(); ++bx) {
      const double x = h->GetXaxis()->GetBinCenter(bx);

      // Sum all regular charge bins at the current strip position.
      for (int by = 1; by <= h->GetNbinsY(); ++by) {
        const double n = h->GetBinContent(bx, by);
        if (n <= 0.) continue;
        if (hitsByStrip) (*hitsByStrip)[bx] += n;

        const double q = h->GetYaxis()->GetBinCenter(by);
        nHits += n;
        sumX += n * x;
        sumX2 += n * x * x;
        if (q <= 0.) continue;

        const double w = n * q;
        sumQ += w;
        sumQ2 += n * q * q;
        sumQX += w * x;
        sumQX2 += w * x * x;
      } // end for by
    } // end for bx

    result.stripHits = nHits/events;
    if (nHits <= 0.) return result;
    result.valid = true;

    // Direct position/RMS: every recorded strip hit has equal statistical weight.
    result.position = sumX / nHits;
    result.rms = sqrt(max(0., sumX2 / nHits - result.position * result.position));
    result.positionErr = result.rms / sqrt(nHits);
    result.rmsErr = nHits > 1. ? result.rms / sqrt(2. * (nHits - 1.)) : kNaN;

    // Alternative beam position and width, weighted by strip charge.
    if (sumQ > 0.) {
      result.chargePosition = sumQX / sumQ;
      result.chargeRMS = sqrt(max(0., sumQX2 / sumQ - result.chargePosition * result.chargePosition));

      // Effective number of weighted observations for approximate uncertainties.
      const double nEffective = sumQ2 > 0. ? sumQ * sumQ / sumQ2 : 0.;
      result.chargePositionErr = nEffective > 0. ? result.chargeRMS / sqrt(nEffective) : kNaN;
      result.chargeRMSErr = nEffective > 1. ? result.chargeRMS / sqrt(2. * (nEffective - 1.)) : kNaN;
    } // end if (sumQ > 0.)

    return result;
  } // end function MeasureDirect()

  // ============================================================================
  // SPATIAL PROFILE MODELS: x is position [mm], f(x) has units ADC
  // ============================================================================
  // Single Gaussian: p = [area (ADC mm), mean (mm), sigma (mm)].
  double GaussianPDF(double *x, double *p) {
    if (p[0] < 0. || p[2] <= 0.) return 0.;

    const double u = (x[0] - p[1]) / p[2];
    return p[0] * exp(-0.5 * u * u) / (sqrt(2. * TMath::Pi()) * p[2]);
  } // end function GaussianPDF()

  // Two Gaussians with a common mean: [area1, mean, sigma1, area2, sigma2].
  // Core and tail are identified later by ordering the fitted widths.
  double DoubleGaussianPDF(double *x, double *p) {
    if (p[0] < 0. || p[2] <= 0. || p[3] < 0. || p[4] <= 0.) return 0.;

    const double u1 = (x[0] - p[1]) / p[2];
    const double u2 = (x[0] - p[1]) / p[4];
    const double f1 = p[0] * exp(-0.5 * u1 * u1) / (sqrt(2. * TMath::Pi()) * p[2]);
    const double f2 = p[3] * exp(-0.5 * u2 * u2) / (sqrt(2. * TMath::Pi()) * p[4]);
    return f1 + f2;
  } // end function DoubleGaussianPDF()

  // Voigt model: [area, mean, Gaussian sigma, Lorentzian HWHM].
  // ROOT TMath::Voigt expects the Lorentzian full width = 2 * HWHM.
  double VoigtPDF(double *x, double *p) {
    if (p[0] < 0. || p[2] <= 0. || p[3] <= 0.) return 0.;
    return p[0] * TMath::Voigt(x[0] - p[1], p[2], 2. * p[3]);
  } // end function VoigtPDF()

  // A fit is accepted only with a ROOT result, a valid minimization result,
  // acceptable fit status, and an adequately estimated covariance matrix.
  bool FitOK(TFitResultPtr &result) {
    if (!result.Get()) return false;
    if (!result->IsValid()) return false;
    if ((int)result > MAX_FIT_STATUS) return false;
    if (result->CovMatrixStatus() < MIN_FIT_COV_STATUS) return false;
    return true;
  } // end function FitOK()

  // Olivero-Longbothum approximation: Voigt FWHM converted to sigma-equivalent
  // via FWHM / 2.35482; this is not a true RMS.
  double VoigtEquivalentSigma(const TF1 *f) {
    if (!f) return kNaN;

    const double sigmaG = fabs(f->GetParameter(2));
    const double gammaL = fabs(f->GetParameter(3));
    const double fg = 2.354820045 * sigmaG;
    const double fl = 2. * gammaL;
    const double fv = 0.5346 * fl + sqrt(0.2166 * fl * fl + fg * fg);
    return fv / 2.354820045;
  } // end function VoigtEquivalentSigma()

  // Compute the Voigt RMS on a finite, parameter-dependent integration window.
  // A full Voigt (nonzero Lorentzian width) does not have a finite global RMS.
  double VoigtTruncatedSigma(TF1 *f) {
    if (!f) return kNaN;

    const double mu = f->GetParameter(1);
    const double sigma = fabs(f->GetParameter(2));
    const double gamma = fabs(f->GetParameter(3));

    // Finite truncation range is clipped to the TF1 spatial fit domain below.
    const double halfWidth = 6. * sigma + 20. * gamma;
    const double xmin = max(f->GetXmin(), mu - halfWidth);
    const double xmax = min(f->GetXmax(), mu + halfWidth);
    if (xmax <= xmin) return kNaN;
    // The Voigt RMS is a rough beam-size estimator: 1e-4 numerical
    // tolerance is sufficient and avoids unnecessarily strict GSL integration.
    const double integrationTolerance = 1.e-4;
    if (f->Integral(xmin, xmax, integrationTolerance) <= 0.) return kNaN;

    const double mean = f->Mean(xmin, xmax, nullptr, integrationTolerance);
    const double second = f->Moment(2, xmin, xmax, nullptr, integrationTolerance);
    return sqrt(max(0., second - mean * mean));
  } // end function VoigtTruncatedSigma()

  // Returns a model-derived spatial sigma in mm or a model-predicted charge in pC/event.

  // ============================================================================
  // DERIVED FIT OBSERVABLES
  // ============================================================================
  // observable = 0 -> spatial sigma: Gaussian sigma, double-Gaussian mixture
  //                   width, or truncated Voigt RMS [mm].
  // observable = 1 -> FWHM-based sigma-equivalent for the Voigt [mm].
  // observable = 2 -> model-predicted charge using measured occupancy [pC/event].
  // Here f(x) is the fitted mean strip charge [ADC], not an ADC/mm density.
  // Its integral is [ADC mm]; modelCharge samples the mean charge at each strip.
  // Direct charge from TH2WeightedCharge is unaffected.
  double ModelDerived(TF1 *f, int model, int observable, const vector<double> &hitsByStrip, const TAxis *xAxis, Long64_t events) {
    if (!f) return kNaN;

    // Spatial width selected according to the model.
    if (observable == 0) {
      if (model == 0) return fabs(f->GetParameter(2));
      if (model == 2) return VoigtTruncatedSigma(f);

      const double a = f->GetParameter(0);
      const double b = f->GetParameter(3);
      const double s1 = f->GetParameter(2);
      const double s2 = f->GetParameter(4);
      if (a + b <= 0.) return kNaN;
      return sqrt(max(0., (a * s1 * s1 + b * s2 * s2) / (a + b)));
    } // end if (observable == 0)

    if (observable == 1) return model == 2 ? VoigtEquivalentSigma(f) : ModelDerived(f, model, 0, hitsByStrip, xAxis, events);

    // Estimate charge per DAQ event by weighting the fitted strip response.
    if (observable == 2) {
      if (!xAxis || events <= 0) return kNaN;
      double sumChargeADC = 0.;

      // Iterate over ROOT regular strip bins; hitsByStrip[0] is unused.
      for (int ix = 1; ix < (int)hitsByStrip.size(); ++ix) {
        const double hits = hitsByStrip[ix];
        if (hits <= 0.) continue;
        const double x = xAxis->GetBinCenter(ix);
        sumChargeADC += hits * f->Eval(x);
      } // end for ix

      return sumChargeADC * ADC_TO_PC / (double)events;
    } // end if (observable == 2)

    return kNaN;
  } // end function ModelDerived()

  // Propagate fit-parameter errors to derived observables numerically.
  // Approximate gradient with finite differences; variance = grad^T * Cov * grad.
  // Only fitted-parameter uncertainty enters here (not occupancy fluctuations).
  double DerivedError(TF1 *f, TFitResultPtr &fitResult, int model, int observable, const vector<double> &hitsByStrip, const TAxis *xAxis, Long64_t events) {
    if (!f || !FitOK(fitResult)) return kNaN;

    const int n = f->GetNpar();
    TMatrixDSym cov = fitResult->GetCovarianceMatrix();
    vector<double> gradient(n, 0.);

    // Numerical derivative with respect to each fit parameter.
    for (int i = 0; i < n; ++i) {
      const double p = f->GetParameter(i);
      const double step = max(1.e-5, fabs(p) * 1.e-4);
      f->SetParameter(i, p + step);
      const double upper = ModelDerived(f, model, observable, hitsByStrip, xAxis, events);
      f->SetParameter(i, p - step);
      const double lower = ModelDerived(f, model, observable, hitsByStrip, xAxis, events);
      f->SetParameter(i, p);
      if (!IsGood(upper) || !IsGood(lower)) return kNaN;
      gradient[i] = (upper - lower) / (2. * step);
    } // end for i

    double variance = 0.;

    // Numerical derivative with respect to each fit parameter.
    for (int i = 0; i < n; ++i) {
      // Include off-diagonal parameter correlations in propagated variance.
      for (int j = 0; j < n; ++j) {
        variance += gradient[i] * cov(i, j) * gradient[j];
      } // end for j
    } // end for i

    return sqrt(max(0., variance));
  } // end function DerivedError()

  // Construct the requested model and set initial parameters and fit limits.
  // model index: 0=single Gaussian, 1=double Gaussian, 2=Voigt.
  TF1 *CreateProfileFit(int model, const TString &name, double minX, double maxX, double areaSeed, double muSeed, double sigmaSeed) {
    TF1 *f = nullptr;
    if (model == 0) f = new TF1(name.Data(), GaussianPDF, minX, maxX, 3);
    if (model == 1) f = new TF1(name.Data(), DoubleGaussianPDF, minX, maxX, 5);
    if (model == 2) f = new TF1(name.Data(), VoigtPDF, minX, maxX, 4);
    if (!f) return nullptr;

    const double span = maxX - minX;
    const double highArea = max(100., 100. * areaSeed);
    const double sigmaLow = 0.1;
    const double sigmaHigh = max(1., span);
    const double gammaLow = 0.01;
    const double gammaHigh = max(0.02, span);
    const double area = max(1.e-4, areaSeed);
    const double mu = max(minX + 1.e-3, min(maxX - 1.e-3, muSeed));
    const double sigma = max(sigmaLow * 1.01, min(sigmaHigh * 0.9, sigmaSeed));

    // MODEL 0: one area-normalized Gaussian.
    if (model == 0) {
      f->SetParNames("I", "mean", "sigma");
      f->SetParameters(area, mu, sigma);
      f->SetParLimits(0, 0., highArea);
      f->SetParLimits(1, minX, maxX);
      f->SetParLimits(2, sigmaLow, sigmaHigh);
    } // end if (model == 0)

    // MODEL 1: two area-normalized Gaussians sharing one mean.
    if (model == 1) {
      f->SetParNames("Icore", "mean", "sigma1", "Itail", "sigma2");
      f->SetParameters(0.75 * area, mu, max(sigmaLow * 1.01, 0.7 * sigma), 0.25 * area, min(2. * sigma, sigmaHigh * 0.9));
      f->SetParLimits(0, 0., highArea);
      f->SetParLimits(1, minX, maxX);
      f->SetParLimits(2, sigmaLow, sigmaHigh);
      f->SetParLimits(3, 0., highArea);
      f->SetParLimits(4, sigmaLow, sigmaHigh);
    } // end if (model == 1)

    // MODEL 2: area-normalized Voigt profile.
    if (model == 2) {
      f->SetParNames("I", "mean", "sigmaG", "gammaL_HWHM");
      f->SetParameters(area, mu, sigma, max(gammaLow * 1.01, 0.5 * sigma));
      f->SetParLimits(0, 0., highArea);
      f->SetParLimits(1, minX, maxX);
      f->SetParLimits(2, sigmaLow, sigmaHigh);
      f->SetParLimits(3, gammaLow, gammaHigh);
    } // end if (model == 2)

    return f;
  } // end function CreateProfileFit()

  // ============================================================================
  // SECOND-STAGE FIT: FIT THE STRIP-BY-STRIP MEAN-CHARGE SPATIAL PROFILE
  // ============================================================================
  // profile was obtained from the FitSlicesY Gaussian mean (ADC vs mm).
  // Reject unusable strips, seed spatial fit, check covariance, extract values.
  // A separate copy of the profile is used for each model to avoid side effects.
  ModelResult FitModel(TH1D *profile, const TH2D *h2, const vector<double> &hitsByStrip, Long64_t events, int model, const TString &tag) {
    ModelResult o;
    if (!profile || !h2 || events <= 0) return o;

    const double minX = profile->GetXaxis()->GetXmin();
    const double maxX = profile->GetXaxis()->GetXmax();
    const double binWidth = profile->GetXaxis()->GetBinWidth(1);
    double weighted = 0.;
    double weightedX = 0.;
    double weightedX2 = 0.;
    double amplitudeMax = 0.;

    // Reject charge slices that lack enough hits or valid Gaussian-fit errors.
    for (int b = 1; b <= profile->GetNbinsX(); ++b) {
      const double nHits = hitsByStrip[b];
      const double q = profile->GetBinContent(b);
      const double err = profile->GetBinError(b);
      if (nHits < MIN_SLICE_HITS || !IsGood(q) || !IsGood(err) || err <= 0.) {
        profile->SetBinContent(b, 0.);
        profile->SetBinError(b, 0.);
        continue;
      } // end if (condition)

      ++o.nProfileStrips;
      if (q <= 0.) continue;

      const double x = profile->GetXaxis()->GetBinCenter(b);
      weighted += q;
      weightedX += q * x;
      weightedX2 += q * x * x;
      amplitudeMax = max(amplitudeMax, q);
    } // end for b

    if (o.nProfileStrips < MIN_PROFILE_STRIPS || weighted <= 0. || amplitudeMax <= 0.) return o;

    // Use moments of surviving mean-charge bins for initial fit parameters.
    const double mu = weightedX / weighted;
    const double sigma = max(0.5, sqrt(max(0., weightedX2 / weighted - mu * mu)));
    const double area = max(1., weighted * binWidth);
    unique_ptr<TF1> f(CreateProfileFit(model, Form("f_%s", tag.Data()), minX, maxX, area, mu, sigma));
    if (!f) return o;

    // R=fit range; Q=quiet; S=return result; N=do not attach; 0=no drawing.
    TFitResultPtr result = profile->Fit(f.get(), "RQSN0");
    o.status = (int)result;
    if (result.Get()) o.covariance = result->CovMatrixStatus();
    o.valid = FitOK(result);
    if (!o.valid) return o;

    TMatrixDSym cov = result->GetCovarianceMatrix();
    o.position = f->GetParameter(1);
    o.positionErr = sqrt(max(0., cov(1, 1)));

    // The spatial profile area is NOT the total collected charge per event.
    const double areaADCmm = model == 1 ? f->GetParameter(0) + f->GetParameter(3) : f->GetParameter(0);
    const double areaVariance = model == 1 ? cov(0, 0) + cov(3, 3) + 2. * cov(0, 3) : cov(0, 0);
    o.profileArea = areaADCmm * ADC_TO_PC;
    o.profileAreaErr = sqrt(max(0., areaVariance)) * ADC_TO_PC;
    o.sigma = ModelDerived(f.get(), model, 0, hitsByStrip, h2->GetXaxis(), events);
    o.sigmaErr = DerivedError(f.get(), result, model, 0, hitsByStrip, h2->GetXaxis(), events);
    o.modelCharge = ModelDerived(f.get(), model, 2, hitsByStrip, h2->GetXaxis(), events);
    o.modelChargeErr = DerivedError(f.get(), result, model, 2, hitsByStrip, h2->GetXaxis(), events);

    // MODEL 0: one area-normalized Gaussian.
    if (model == 0) {
      o.sigmaCore = o.sigma;
      o.sigmaCoreErr = o.sigmaErr;
      o.sigmaFWHM = o.sigma;
      o.sigmaFWHMErr = o.sigmaErr;
    } // end if (model == 0)

    // MODEL 1: two area-normalized Gaussians sharing one mean.
    if (model == 1) {
      const bool firstCore = fabs(f->GetParameter(2)) <= fabs(f->GetParameter(4));
      const int coreIndex = firstCore ? 2 : 4;
      const int tailIndex = firstCore ? 4 : 2;
      o.sigmaCore = fabs(f->GetParameter(coreIndex));
      o.sigmaCoreErr = sqrt(max(0., cov(coreIndex, coreIndex)));
      o.sigmaTail = fabs(f->GetParameter(tailIndex));
      o.sigmaTailErr = sqrt(max(0., cov(tailIndex, tailIndex)));
    } // end if (model == 1)

    // MODEL 2: area-normalized Voigt profile.
    if (model == 2) {
      o.sigmaCore = fabs(f->GetParameter(2));
      o.sigmaCoreErr = sqrt(max(0., cov(2, 2)));
      o.sigmaTail = fabs(f->GetParameter(3));
      o.sigmaTailErr = sqrt(max(0., cov(3, 3)));
      o.sigmaTruncated = o.sigma;
      o.sigmaTruncatedErr = o.sigmaErr;
      o.sigmaFWHM = ModelDerived(f.get(), model, 1, hitsByStrip, h2->GetXaxis(), events);
      o.sigmaFWHMErr = DerivedError(f.get(), result, model, 1, hitsByStrip, h2->GetXaxis(), events);
    } // end if (model == 2)

    // Independent Gaussian-core cross-check within +/-2 fitted effective widths.
    if (model != 0 && IsGood(o.sigma) && o.sigma > 0.) {
      const double lowCore = max(minX, o.position - 2. * o.sigma);
      const double highCore = min(maxX, o.position + 2. * o.sigma);
      if (highCore > lowCore) {
        TF1 core(Form("fCore_%s", tag.Data()), "gaus", lowCore, highCore);
        TFitResultPtr rc = profile->Fit(&core, "RQSN0");
        if (FitOK(rc)) {
          o.sigmaSingleCore = fabs(core.GetParameter(2));
          o.sigmaSingleCoreErr = core.GetParError(2);
        } // end if (FitOK(rc))
      } // end if (highCore > lowCore)
    } // end if (model != 0 && IsGood(o.sigma) && o.sigma > 0.)

    return o;
  } // end function FitModel()

  // ============================================================================
  // COMPLETE MEASUREMENT OF ONE TIME BIN (OR THE ENTIRE RUN)
  // ============================================================================
  // Step A: direct histogram quantities and strip-hit occupancies.
  // Step B: FitSlicesY -> charge mean/sigma/amplitude/chi2 for each strip.
  // Step C: fit the mean-charge spatial profile with all three models.
  Measurement Measure(TH2D *h2, Long64_t events, const TString &tag, TDirectory *diagDir) {
    Measurement measurement;
    vector<double> hitsByStrip;

    // First extract all quantities that need no model fit.
    measurement.direct = MeasureDirect(h2, events, &hitsByStrip);
    if (!h2 || !measurement.direct.valid) return measurement;

    TObjArray slices;
    slices.SetOwner(kTRUE);

    // At every strip (TH2 X), fit the ADC charge distribution (TH2 Y).
    // Array entries: 0=amplitude, 1=charge mean, 2=charge sigma, 3=chi2/ndf.
    h2->FitSlicesY(nullptr, 1, h2->GetNbinsX(), 0, "QNR", &slices);
    TH1D *mean = dynamic_cast<TH1D*>(slices.At(1));
    if (!mean) return measurement;

    if (diagDir && WRITE_SLICE_HISTOGRAMS) {
      const char *names[4] = {"hSliceAmplitude", "hSliceChargeMeanADC", "hSliceChargeSigmaADC", "hSliceChi2NDF"};
      for (int i = 0; i < 4; ++i) {
        TH1D *h = dynamic_cast<TH1D*>(slices.At(i));
        if (h) diagDir->WriteTObject(h, names[i], "Overwrite");
      } // end for i
    } // end if (diagDir && WRITE_SLICE_HISTOGRAMS)

    for (int model = 0; model < N_MODEL; ++model) {
      unique_ptr<TH1D> profile(dynamic_cast<TH1D*>(mean->Clone(Form("hTemporary_%s_%d", tag.Data(), model))));
      if (!profile) continue;
      profile->SetDirectory(nullptr);
      measurement.fit[model] = FitModel(profile.get(), h2, hitsByStrip, events, model, tag + Form("_%s", MODEL_NAME[model]));
    } // end for model

    return measurement;
  } // end function Measure()

  // ============================================================================
  // WHOLE-RUN CONSOLE SUMMARY (NO ROOT OUTPUT DIRECTORY)
  // ============================================================================
  // The whole-run projections still supply reference values for time trends.
  // Print the main results of every method, keeping all physics units explicit.
  // No TF1 objects or whole-run scalar results are written to the ROOT file.
  void PrintOverallSummary(const Measurement overall[N_MODE][N_VIEW], int nTimeBins, Long64_t totalEvents, double tMin, double tMax) {
    const ios::fmtflags oldFlags = cout.flags();
    const streamsize oldPrecision = cout.precision();
    cout << fixed << setprecision(3);
    cout << "\n============================================================" << endl;
    cout << "                   ANALYSISTMM - WHOLE RUN" << endl;
    cout << "============================================================" << endl;
    cout << "Input: X = position [mm], Y = Time [s], Z = charge [ADC]" << endl;
    cout << "Processed: " << totalEvents << " DAQ events in " << nTimeBins << " time bins (" << tMin << " - " << tMax << " s)" << endl;
    cout << "Methods: direct moments/charge integral; FitSlicesY followed by single Gaussian, double Gaussian and Voigt fits" << endl;
    cout << "Direct Q = weighted histogram charge / DAQ events; model Q = occupancy-weighted fitted charge / DAQ events" << endl;

    // One compact group for each correction method and readout coordinate.
    for (int mode = 0; mode < N_MODE; ++mode) {
      for (int view = 0; view < N_VIEW; ++view) {
        const Measurement &m = overall[mode][view];
        const DirectResult &d = m.direct;
        cout << "\n--- " << MODE_NAME[mode] << " / " << VIEW_NAME[view] << " ---" << endl;

        if (d.valid) {
          cout << "  Direct:    mean = " << d.position << " +/- " << d.positionErr << " mm; RMS = " << d.rms << " +/- " << d.rmsErr << " mm; Q = " << d.charge << " +/- " << d.chargeErr << " pC/event" << endl;
          cout << "  Q-weighted: mean = " << d.chargePosition << " mm; RMS = " << d.chargeRMS << " mm; strip hits = " << d.stripHits << endl;
        } else {
          cout << "  Direct: no valid strip-hit measurement" << endl;
        } // end if (d.valid)

        // Report the main fit quantities and the model-specific sigma estimates.
        for (int model = 0; model < N_MODEL; ++model) {
          const ModelResult &r = m.fit[model];
          if (!r.valid) {
            cout << "  " << MODEL_NAME[model] << ": fit unavailable/rejected (status = " << r.status << ", covariance = " << r.covariance << ")" << endl;
            continue;
          } // end if (!r.valid)

          cout << "  " << MODEL_NAME[model] << ": mean = " << r.position << " +/- " << r.positionErr << " mm; sigma = " << r.sigma << " +/- " << r.sigmaErr << " mm; Q(model) = " << r.modelCharge << " +/- " << r.modelChargeErr << " pC/event" << endl;
          if (model == 1) cout << "    Gaussian widths: core = " << r.sigmaCore << " mm; tail = " << r.sigmaTail << " mm" << endl;
          if (model == 2) cout << "    Voigt widths: sigmaG = " << r.sigmaCore << " mm; gammaL(HWHM) = " << r.sigmaTail << " mm; FWHM-equivalent sigma = " << r.sigmaFWHM << " mm" << endl;
        } // end for model
      } // end for view
    } // end for mode

    cout << "============================================================\n" << endl;
    cout.flags(oldFlags);
    cout.precision(oldPrecision);
  } // end function PrintOverallSummary()

  // ============================================================================
  // TIME TRENDS: one point per available TH3 time bin
  // ============================================================================
  // For each method, write physics observables plus fit diagnostics.
  // Physical quantities from rejected fits are skipped.
  void TrendMeasurement(TFile *output, const TString &mode, const TString &view, const Measurement &m, double time, double timeErr) {
    // Direct observables: hit-based and charge-weighted physics measurements.
    const TString directPath = mode + "/Direct/" + view;
    AddTrend(output, directPath, "PositionMean", Form("Mean strip-hit %s [mm]", view.Data()), time, timeErr, m.direct.position, m.direct.positionErr);
    AddTrend(output, directPath, "SigmaRMS", Form("Strip-hit RMS-%s [mm]", view.Data()), time, timeErr, m.direct.rms, m.direct.rmsErr);
    AddTrend(output, directPath, "ChargeWeightedPosition", Form("Charge-weighted %s [mm]", view.Data()), time, timeErr, m.direct.chargePosition, m.direct.chargePositionErr);
    AddTrend(output, directPath, "ChargeWeightedSigma", Form("Charge-weighted #sigma_{%s} [mm]", view.Data()), time, timeErr, m.direct.chargeRMS, m.direct.chargeRMSErr);
    AddTrend(output, directPath, "ChargePerEvent", Form("Q_{%s} [pC/event]", view.Data()), time, timeErr, m.direct.charge, m.direct.chargeErr);
    AddTrend(output, directPath, "StripHits", Form("# of %s-strip hits",view.Data()), time, timeErr, m.direct.stripHits, 0.);
    AddTrend(output, directPath, "NEvents", "# events", time, timeErr, m.direct.events, 0.);

    for (int model = 0; model < N_MODEL; ++model) {
      const ModelResult &r = m.fit[model];
      const TString path = mode + "/" + MODEL_NAME[model] + "/" + view;
      AddTrend(output, path, "FitStatus", "ROOT fit status", time, timeErr, r.status, 0.);
      AddTrend(output, path, "CovarianceStatus", "Covariance status", time, timeErr, r.covariance, 0.);
      AddTrend(output, path, "NProfileStrips", "Fitted strip slices", time, timeErr, r.nProfileStrips, 0.);

      // Keep failure diagnostics, but suppress unphysical fit-derived trends.
      if (!r.valid) continue;
      AddTrend(output, path, "Position", "Fitted beam position [mm]", time, timeErr, r.position, r.positionErr);
      AddTrend(output, path, "Sigma", "Effective beam width [mm]", time, timeErr, r.sigma, r.sigmaErr);
      AddTrend(output, path, "SigmaCore", "Gaussian core width [mm]", time, timeErr, r.sigmaCore, r.sigmaCoreErr);
      AddTrend(output, path, "SigmaTail", model == 2 ? "Lorentzian HWHM [mm]" : "Gaussian tail width [mm]", time, timeErr, r.sigmaTail, r.sigmaTailErr);
      AddTrend(output, path, "SigmaFWHM", "FWHM / 2.35482 [mm]", time, timeErr, r.sigmaFWHM, r.sigmaFWHMErr);
      AddTrend(output, path, "SigmaTruncated", "Truncated Voigt RMS [mm]", time, timeErr, r.sigmaTruncated, r.sigmaTruncatedErr);
      AddTrend(output, path, "SigmaSingleCore", "Gaussian core cross-check [mm]", time, timeErr, r.sigmaSingleCore, r.sigmaSingleCoreErr);
      AddTrend(output, path, "ProfileArea", "Spatial profile area [pC mm]", time, timeErr, r.profileArea, r.profileAreaErr);
      AddTrend(output, path, "ModelChargePerEvent", "Model occupancy-weighted charge [pC/event]", time, timeErr, r.modelCharge, r.modelChargeErr);
      AddComparison(output, mode + "/" + view + "/" + MODEL_NAME[model], "ModelChargeOverDirectCharge", "Q_model / Q_direct", time, timeErr, m.direct.charge > 0. ? r.modelCharge / m.direct.charge : kNaN);
    } // end for model
  } // end function TrendMeasurement()

  // Verify that every TH3 uses the same strip, DAQ time and charge bin edges.
  // Essential for meaningful comparisons across modes and readouts.
  bool SameAxis(const TAxis *a, const vector<double> &edges) {
    if (!a) return false;
    if (a->GetNbins() + 1 != (int)edges.size()) return false;

    // Compare every bin edge, including the final upper edge.
    for (int i = 0; i <= a->GetNbins(); ++i) {
      const double edge = i == a->GetNbins() ? a->GetBinUpEdge(i) : a->GetBinLowEdge(i + 1);
      const double tol = max(1.e-6, 1.e-10 * fabs(edges.back() - edges.front()));
      if (fabs(edge - edges[i]) > tol) return false;
    } // end for i

    return true;
  } // end function SameAxis()

  // Common selector for cross-method comparisons.
  // method 0=Direct, 1=Gaussian, 2=DoubleGaussian, 3=Voigt.
  // observable 0=position, 1=width, 2=charge per event.
  double PhysicalMetric(const Measurement &m, int method, int observable) {
    if (method == 0) {
      if (!m.direct.valid) return kNaN;
      if (observable == 0) return m.direct.position;
      if (observable == 1) return m.direct.rms;
      if (observable == 2) return m.direct.charge;
      return kNaN;
    } // end if (method == 0)

    const ModelResult &r = m.fit[method - 1];
    if (!r.valid) return kNaN;
    if (observable == 0) return r.position;
    if (observable == 1) return r.sigma;
    if (observable == 2) return r.modelCharge;
    return kNaN;
  } // end function PhysicalMetric()

  // Convert accepted pairs of time-bin measurements into a TH2D correlation.
  // Axis limits are determined from observed minimum and maximum values.
  void WriteCorrelation(TFile *output, const TString &folder, const TString &name, const TString &xTitle, const TString &yTitle, const vector<pair<double, double>> &samples) {
    if (!output || samples.empty()) return;

    double minX = numeric_limits<double>::infinity();
    double maxX = -numeric_limits<double>::infinity();
    double minY = numeric_limits<double>::infinity();
    double maxY = -numeric_limits<double>::infinity();

    // Only finite pairs contribute to the dynamic correlation axes.
    for (const auto &p : samples) {
      if (!IsGood(p.first) || !IsGood(p.second)) continue;
      minX = min(minX, p.first);
      maxX = max(maxX, p.first);
      minY = min(minY, p.second);
      maxY = max(maxY, p.second);
    } // end for loop

    if (!IsGood(minX) || !IsGood(minY)) return;

    const double dx = max(1.e-5, maxX - minX);
    const double dy = max(1.e-5, maxY - minY);
    TH2D hist(name.Data(), Form("%s;%s;%s", name.Data(), xTitle.Data(), yTitle.Data()), 80, minX - 0.1 * dx, maxX + 0.1 * dx, 80, minY - 0.1 * dy, maxY + 0.1 * dy);

    // Only finite pairs contribute to the dynamic correlation axes.
    for (const auto &p : samples) {
      if (IsGood(p.first) && IsGood(p.second)) hist.Fill(p.first, p.second);
    } // end for loop

    TDirectory *dir = EnsureDirectory(output, "Correlations/" + folder);
    if (dir) dir->WriteTObject(&hist, name.Data(), "Overwrite");
  } // end function WriteCorrelation()

  // ============================================================================
  // TIME-BIN CORRELATIONS: within-view and X-versus-Y comparisons
  // ============================================================================
  // For each calibration and analysis method, correlate position/width/charge.
  // Pair X and Y measurements only when they come from the SAME time bin.
  void BuildCorrelationHistograms(TFile *output, vector<Measurement> results[N_MODE][N_VIEW], int nTimeBins) {
    for (int mode = 0; mode < N_MODE; ++mode) {
      // Compare direct method with the three spatial-profile fit methods.
      for (int method = 0; method <= N_MODEL; ++method) {
        const TString methodName = method == 0 ? "Direct" : MODEL_NAME[method - 1];

        for (int view = 0; view < N_VIEW; ++view) {
          vector<pair<double, double>> positionSigma;
          vector<pair<double, double>> positionCharge;
          vector<pair<double, double>> sigmaCharge;

          // One result per native DAQ time bin; invalid values are omitted.
          for (int i = 0; i < nTimeBins; ++i) {
            const Measurement &m = results[mode][view][i];
            const double position = PhysicalMetric(m, method, 0);
            const double sigma = PhysicalMetric(m, method, 1);
            const double charge = PhysicalMetric(m, method, 2);
            if (IsGood(position) && IsGood(sigma)) positionSigma.emplace_back(position, sigma);
            if (IsGood(position) && IsGood(charge)) positionCharge.emplace_back(position, charge);
            if (IsGood(sigma) && IsGood(charge)) sigmaCharge.emplace_back(sigma, charge);
          } // end for i

          const TString folder = TString(MODE_NAME[mode]) + "/" + methodName + "/" + VIEW_NAME[view];
          WriteCorrelation(output, folder, "hPositionVsSigma", "Position [mm]", "Sigma [mm]", positionSigma);
          WriteCorrelation(output, folder, "hPositionVsCharge", "Position [mm]", "Charge [pC/event]", positionCharge);
          WriteCorrelation(output, folder, "hSigmaVsCharge", "Sigma [mm]", "Charge [pC/event]", sigmaCharge);
        } // end for view

        vector<pair<double, double>> positionXY;
        vector<pair<double, double>> sigmaXY;
        vector<pair<double, double>> chargeXY;

        // One result per native DAQ time bin; invalid values are omitted.
        for (int i = 0; i < nTimeBins; ++i) {
          const Measurement &x = results[mode][0][i];
          const Measurement &y = results[mode][1][i];
          for (int metric = 0; metric < 3; ++metric) {
            const double vx = PhysicalMetric(x, method, metric);
            const double vy = PhysicalMetric(y, method, metric);
            if (!IsGood(vx) || !IsGood(vy)) continue;
            if (metric == 0) positionXY.emplace_back(vx, vy);
            if (metric == 1) sigmaXY.emplace_back(vx, vy);
            if (metric == 2) chargeXY.emplace_back(vx, vy);
          } // end for metric
        } // end for i

        const TString folderXY = TString(MODE_NAME[mode]) + "/" + methodName + "/XY";
        WriteCorrelation(output, folderXY, "hXvsY", "X position [mm]", "Y position [mm]", positionXY);
        WriteCorrelation(output, folderXY, "hSigmaXvsSigmaY", "Sigma X [mm]", "Sigma Y [mm]", sigmaXY);
        WriteCorrelation(output, folderXY, "hChargeXvsChargeY", "Charge X [pC/event]", "Charge Y [pC/event]", chargeXY);
      } // end for method
    } // end for mode
  } // end function BuildCorrelationHistograms()

  // ============================================================================
  // OUTPUT FINALIZATION
  // ============================================================================
  // Write cached TGraphErrors objects to their matching ROOT folders.
  // Build 1D distributions of graph Y values (one entry per valid time bin).
  void FinalizeGraphs(TFile *output) {
    // Write graph, then (when applicable) make its distribution over time.
    for (auto &p : outputGraphs) {
      TGraphErrors *g = p.second.graph.get();
      TDirectory *dir = p.second.directory;
      if (!g || !dir) continue;

      dir->WriteTObject(g, g->GetName(), "Overwrite");
      const string key = p.first;
      if (key.find("TimeTrends/") != 0) continue;
      if (key.find("/FitStatus") != string::npos || key.find("/CovarianceStatus") != string::npos || key.find("/NEvents") != string::npos || key.find("/StripHits") != string::npos || key.find("/NProfileStrips") != string::npos) continue;
      const size_t slash = key.rfind('/');
      if (slash == string::npos) continue;

      const TString relative = ("Distributions/" + key.substr(strlen("TimeTrends/"), slash - strlen("TimeTrends/"))).c_str();
      TDirectory *distDir = EnsureDirectory(output, relative);
      if (!distDir || g->GetN() <= 0) continue;

      double ymin = numeric_limits<double>::infinity();
      double ymax = -numeric_limits<double>::infinity();

      // Determine range (or fill distribution) using the finite graph points.
      for (int i = 0; i < g->GetN(); ++i) {
        double x = 0.;
        double y = 0.;
        g->GetPoint(i, x, y);
        if (!IsGood(y)) continue;
        ymin = min(ymin, y);
        ymax = max(ymax, y);
      } // end for i

      if (!IsGood(ymin) || !IsGood(ymax)) continue;

      const double span = max(1.e-6, ymax - ymin);
      const double lower = ymin - 0.1 * span;
      const double upper = ymax + 0.1 * span;
      const TString distName = TString(g->GetName()).ReplaceAll("g_", "h_");
      TH1D dist(distName.Data(), Form("Distribution of %s;%s;Time bins", g->GetName(), g->GetYaxis()->GetTitle()), 100, lower, upper);

      // Determine range (or fill distribution) using the finite graph points.
      for (int i = 0; i < g->GetN(); ++i) {
        double x = 0.;
        double y = 0.;
        g->GetPoint(i, x, y);
        if (IsGood(y)) dist.Fill(y);
      } // end for i

      distDir->WriteTObject(&dist, dist.GetName(), "Overwrite");
    } // end for loop

    // Release any graphs retained from a previous invocation.
    outputGraphs.clear();
  } // end function FinalizeGraphs()

} // end namespace AnalysisTMMDetail

// ============================================================================
// MAIN ENTRY POINT: ROOT MACRO AnalysisTMM("reconstruction.root", "output.root")
// ============================================================================
// This is the only function a user normally calls. It runs the entire
// histogram-based analysis in five phases (marked BEGIN/END below).
void AnalysisTMM(const char *InputFileName, const char *OutputFileName = "", int ExpectedNBins = 0) {
  using namespace AnalysisTMMDetail;

  // Release any graphs retained from a previous invocation.
  outputGraphs.clear();
  unique_ptr<TFile> fin(TFile::Open(InputFileName, "READ"));
  if (!fin || fin->IsZombie()) {
    cerr << "ERROR: cannot open input ROOT file: " << InputFileName << endl;
    return;
  } // end if (!fin || fin->IsZombie())

  // Production input: RecoTMM_v0_0090032.root -> AnalysisTMM_v0_0090032.root.
  // A second argument overrides the destination (for manual tests or reprocessing).
  TString inName = gSystem->BaseName(InputFileName);
  if (inName.EndsWith(".root")) inName.Remove(inName.Length() - 5);
  TString outName;
  if (inName.BeginsWith("RecoTMM_")) {
    outName = TString("AnalysisTMM_") + TString(inName(8, inName.Length() - 8)) + ".root";
  } else {
    outName = TString("AnalysisTMM_") + inName + ".root";
  }
  TString outputFile = (OutputFileName && OutputFileName[0])
      ? TString(OutputFileName)
      : TString("/data9Vd1/padme/mancinima/RecoRun4_v0/AnalysisTMM/") + outName;
  const TString outputDirectory = gSystem->DirName(outputFile.Data());
  if (gSystem->mkdir(outputDirectory.Data(), kTRUE) != 0 &&
      gSystem->AccessPathName(outputDirectory.Data())) {
    cerr << "ERROR: cannot create output directory: " << outputDirectory << endl;
    return;
  }
  unique_ptr<TFile> fout(TFile::Open(outputFile.Data(), "RECREATE"));
  if (!fout || fout->IsZombie()) {
    cerr << "ERROR: cannot create analysis ROOT file: " << outputFile << endl;
    return;
  } // end if (!fout || fout->IsZombie())

  TDirectory *rawDir = fin->GetDirectory("Overall/Raw");
  if (!rawDir) {
    cerr << "ERROR: missing Overall/Raw in input" << endl;
    return;
  } // end if (!rawDir)

  TH3F *reference = dynamic_cast<TH3F*>(rawDir->Get("hqmaxstripFullX"));
  if (!reference) {
    cerr << "ERROR: missing Overall/Raw/hqmaxstripFullX (expected TH3F)" << endl;
    return;
  } // end if (!reference)

  // PHASE 1 / BEGIN: read the master X / time / ADC axes from RAW X TH3F.
  const int nTimeBins = reference->GetNbinsY();
  const int nXBins = reference->GetNbinsX();
  const int nChargeBins = reference->GetNbinsZ();
  if (nTimeBins <= 0 || nXBins <= 0 || nChargeBins <= 0) {
    cerr << "ERROR: invalid TH3 axis bin count" << endl;
    return;
  } // end if (condition)
  if (ExpectedNBins > 0 && nTimeBins != ExpectedNBins) {
    cerr << "ERROR: expected " << ExpectedNBins << " time bins, input TH3 contains " << nTimeBins << endl;
    return;
  }

  // Save every native DAQ time-bin boundary; do not construct new blocks.
  vector<double> timeEdges(nTimeBins + 1, 0.);
  vector<double> stripEdges(nXBins + 1, 0.);
  vector<double> chargeEdges(nChargeBins + 1, 0.);
  // ROOT axes have independent bin counts. Record all edges separately.
  for (int i = 0; i < nTimeBins; ++i)
    timeEdges[i] = reference->GetYaxis()->GetBinLowEdge(i + 1);
  for (int i = 0; i < nXBins; ++i)
    stripEdges[i] = reference->GetXaxis()->GetBinLowEdge(i + 1);
  for (int i = 0; i < nChargeBins; ++i)
    chargeEdges[i] = reference->GetZaxis()->GetBinLowEdge(i + 1);
  timeEdges[nTimeBins] = reference->GetYaxis()->GetBinUpEdge(nTimeBins);
  stripEdges[nXBins] = reference->GetXaxis()->GetBinUpEdge(nXBins);
  chargeEdges[nChargeBins] = reference->GetZaxis()->GetBinUpEdge(nChargeBins);

  // PHASE 1 / END: common binning established.
  // Release the large reference histogram before loading the next TH3F.
  delete reference;

  // PHASE 2 / BEGIN: count DAQ events in each histogram time interval.
  // sum of each time inside each bin wrt to t_start
  TGraphErrors *tsumGraph = dynamic_cast<TGraphErrors*>(fin->Get("Overall/g_vtsum"));
  if (!tsumGraph) {
    cerr << "ERROR: missing Overall/g_vtsum;" << endl;
    return;
  } // end if (!tsumGraph)

  // sum of each time^2 inside each bin wrt to t_start
  TGraphErrors *tsum2Graph = dynamic_cast<TGraphErrors*>(fin->Get("Overall/g_vtsum2"));
  if (!tsum2Graph) {
    cerr << "ERROR: missing Overall/g_vtsum2;" << endl;
    return;
  } // end if (!tsum2Graph)

  // n_event inside each bin - to normalize the 2 tgraphs above
  TGraphErrors *vnnGraph = dynamic_cast<TGraphErrors*>(fin->Get("Overall/g_vnn"));
  if (!vnnGraph) {
    cerr << "ERROR: missing Overall/g_vnn;" << endl;
    return;
  } // end if (!vnnGraph)

  // Use the ROOT parameters introduced in RecoTMM_v0. Older files retain
  // g_TStart_Tstop, which is accepted as a backward-compatible fallback.
  const auto *pStart = dynamic_cast<TParameter<double>*>(fin->Get("tStartUnix"));
  const auto *pStop = dynamic_cast<TParameter<double>*>(fin->Get("tStopUnix"));
  const auto *pBins = dynamic_cast<TParameter<int>*>(fin->Get("nBins"));
  const auto *pTruncated = dynamic_cast<TParameter<int>*>(fin->Get("inputTruncated"));
  TGraphErrors *boundsGraph = dynamic_cast<TGraphErrors*>(fin->Get("Overall/g_TStart_Tstop"));
  if ((!pStart || !pStop) && (!boundsGraph || boundsGraph->GetN() < 2)) {
    cerr << "ERROR: missing tStartUnix/tStopUnix parameters and Overall/g_TStart_Tstop" << endl;
    return;
  }
  if (pBins && pBins->GetVal() != nTimeBins) {
    cerr << "ERROR: metadata nBins=" << pBins->GetVal()
         << " disagrees with TH3 time bins=" << nTimeBins << endl;
    return;
  }
  if (tsumGraph->GetN() != nTimeBins || tsum2Graph->GetN() != nTimeBins ||
      vnnGraph->GetN() != nTimeBins) {
    cerr << "ERROR: time-moment graph point count differs from TH3 time-bin count "
         << nTimeBins << endl;
    return;
  }
  const double T_Start = pStart ? pStart->GetVal() : boundsGraph->GetPointY(0);
  const double T_Stop  = pStop ? pStop->GetVal() : boundsGraph->GetPointY(1);
  if (!IsGood(T_Start) || !IsGood(T_Stop) || T_Stop <= T_Start) {
    cerr << "ERROR: invalid reconstructed UNIX interval" << endl;
    return;
  }
  const double axisDuration = timeEdges.back() - timeEdges.front();
  if (fabs(axisDuration - (T_Stop - T_Start)) >
      max(1.e-6, 1.e-9 * (T_Stop - T_Start))) {
    cerr << "ERROR: TH3 time-axis duration disagrees with UNIX metadata" << endl;
    return;
  }
  if (pTruncated && pTruncated->GetVal() != 0)
    cerr << "WARNING: input reconstruction was truncated; empty time bins may be unprocessed" << endl;

  // The three graphs store S1=sum(tRel), S2=sum(tRel^2), N per ROOT bin.
  // Compute the actual mean UNIX time and its standard error ONCE per bin.
  // Unlike the former code, this handles N=0/1 and never accesses graph[-1].
  vector<Long64_t> eventCounts(nTimeBins, 0);
  vector<double> meanUnix(nTimeBins, kNaN);
  vector<double> meanUnixErr(nTimeBins, 0.);
  Long64_t totalEvents = 0;
  for (int bin = 0; bin < nTimeBins; ++bin) {
    const double rawN = vnnGraph->GetPointY(bin);
    const double s1 = tsumGraph->GetPointY(bin);
    const double s2 = tsum2Graph->GetPointY(bin);
    if (!IsGood(rawN) || !IsGood(s1) || !IsGood(s2) || rawN < 0. ||
        rawN > static_cast<double>(numeric_limits<Long64_t>::max()) ||
        fabs(rawN - round(rawN)) > 1.e-5) {
      cerr << "ERROR: invalid time moments in bin " << bin + 1 << endl;
      return;
    }
    const Long64_t n = static_cast<Long64_t>(llround(rawN));
    eventCounts[bin] = n;
    totalEvents += n;
    if (n == 0) continue;
    const double meanRel = s1 / static_cast<double>(n);
    meanUnix[bin] = T_Start + meanRel;
    if (n > 1) {
      const double N = static_cast<double>(n);
      const double sem2 = (s2 - s1 * s1 / N) / (N * (N - 1.));
      meanUnixErr[bin] = sqrt(max(0., sem2));
    }
    if (meanRel < timeEdges[bin] - 1.e-4 ||
        meanRel >= timeEdges[bin + 1] + 1.e-4) {
      cerr << "WARNING: actual event mean lies outside TH3 bin " << bin + 1 << endl;
    }
  }

  cout << "AnalysisTMM: DAQ time bins = " << nTimeBins
       << ", UNIX interval [" << setprecision(17) << T_Start << ", " << T_Stop << ")"
       << ", total counted events = " << totalEvents << endl;
  if (totalEvents <= 0) {
    cerr << "ERROR: no DAQ events in the TH3 time interval" << endl;
    return;
  }
  fout->cd();
  TParameter<double>("tStartUnix", T_Start).Write();
  TParameter<double>("tStopUnix", T_Stop).Write();
  TParameter<int>("nBins", nTimeBins).Write();
  TParameter<double>("binWidthSeconds", (T_Stop - T_Start) / nTimeBins).Write();
  TParameter<Long64_t>("nEventsInTimeRange", totalEvents).Write();
  if (pTruncated) TParameter<int>("inputTruncated", pTruncated->GetVal()).Write();

  // PHASE 2 / END: denominators for per-event charge are now available.
  // PHASE 3 / BEGIN: per-time-bin measurements plus whole-run reference.
  // results[calibration][readout][timeBin] stores all four methods.
  vector<Measurement> results[N_MODE][N_VIEW];
  Measurement globalResults[N_MODE][N_VIEW];

  // Loop over RAW, external calibration and run calibration.
  for (int mode = 0; mode < N_MODE; ++mode) {
    // Compare calibration modes separately in each coordinate view.
    for (int view = 0; view < N_VIEW; ++view) {
      results[mode][view].resize(nTimeBins);
      const TString folder = Form("Overall/%s", MODE_NAME[mode]);
      TDirectory *inputDir = fin->GetDirectory(folder.Data());
      if (!inputDir) {
        cerr << "ERROR: missing input directory " << folder << endl;
        return;
      } // end if (!inputDir)

      TString histName;
      if (mode == 0) histName = Form("hqmaxstripFull%s", VIEW_NAME[view]);
      if (mode == 1) histName = Form("hqmaxstripFull_extcal%s", VIEW_NAME[view]);
      if (mode == 2) histName = Form("hqmaxstripFull_runcal%s", VIEW_NAME[view]);

      // Load only one very large TH3F at a time to control memory usage.
      TH3F *h3 = dynamic_cast<TH3F*>(inputDir->Get(histName.Data()));
      if (!h3) {
        cerr << "ERROR: missing or invalid TH3F: " << folder << "/" << histName << endl;
        return;
      } // end if (!h3)

      // Stop if modes/readouts have incompatible coordinate, time or ADC bins.
      if (!SameAxis(h3->GetXaxis(), stripEdges) || !SameAxis(h3->GetYaxis(), timeEdges) ||
          !SameAxis(h3->GetZaxis(), chargeEdges)) {
        cerr << "ERROR: inconsistent X/time/charge binning in " << histName << endl;
        delete h3;
        return;
      }

      cout << "Processing " << MODE_NAME[mode] << " / " << VIEW_NAME[view] << endl;

      // Input-quality checks: warn about underflow/overflow without saving metadata.
      const double nChargeOverflow = h3->Integral(1, nXBins, 1, nTimeBins, nChargeBins + 1, nChargeBins + 1);
      const double nPositionUnderflow = h3->Integral(0, 0, 1, nTimeBins, 1, nChargeBins);
      const double nPositionOverflow = h3->Integral(nXBins + 1, nXBins + 1, 1, nTimeBins, 1, nChargeBins);
      if (nChargeOverflow > 0.){
        cerr << "WARNING: charge-axis overflow hits in " << histName << " = " << nChargeOverflow << endl;
      } // end if (nChargeOverflow > 0.)

      if (nPositionUnderflow > 0. || nPositionOverflow > 0.){
        cerr << "WARNING: position overflow hits in " << histName << " : low=" << nPositionUnderflow << " high=" << nPositionOverflow << endl;
      } // end if (condition)

      // TIME-BIN LOOP / BEGIN: analyze one common histogram time interval.
      for (int timeBin = 1; timeBin <= nTimeBins; ++timeBin) {
        const Long64_t nEvents = eventCounts[timeBin - 1];
        if (nEvents <= 0) continue;
        const double timeCenter = meanUnix[timeBin - 1];
        const double timeHalfWidth = meanUnixErr[timeBin - 1];

        // Exclude time overflow, since g_vnn counts only regular TH3 time bins.
        const int lastBin = timeBin;

        // Select the time slice on TH3 Y before projecting.
        h3->GetYaxis()->SetRange(timeBin, lastBin);
        fout->cd();

        // Project TH3 (Z=charge, X=position) -> TH2 (X=position, Y=charge).
        // This orientation is required by TH2WeightedCharge and FitSlicesY.
        TH2D *h2 = dynamic_cast<TH2D*>(h3->Project3D("zx"));
        if (!h2) {
          cerr << "WARNING: Project3D(zx) failed for " << histName << " timeBin " << timeBin << endl;
          continue;
        } // end if (!h2)

        h2->SetDirectory(nullptr);
        const TString diagName = Form("FitDiagnostics/%s/%s/bin_%04d", MODE_NAME[mode], VIEW_NAME[view], timeBin);
        TDirectory *diagDir = EnsureDirectory(fout.get(), diagName);
        const TString tag = Form("%s_%s_bin_%04d", MODE_NAME[mode], VIEW_NAME[view], timeBin);

        // One direct measurement + FitSlicesY + three spatial-model fits.
        Measurement m = Measure(h2, nEvents, tag, diagDir);
        results[mode][view][timeBin - 1] = m;

        // Add all accepted physics observables at the native time-bin center.
        TrendMeasurement(fout.get(), MODE_NAME[mode], VIEW_NAME[view], m, timeCenter, timeHalfWidth);
        delete h2;
      } // end for timeBin

      // TIME-BIN LOOP / END.
      // WHOLE-RUN LOOP / BEGIN: compute reference results for comparisons and console summary.
      // Entire-run evaluation uses the same selection and measurement methods.
      h3->GetYaxis()->SetRange(1, nTimeBins);
      fout->cd();
      TH2D *hOverall = dynamic_cast<TH2D*>(h3->Project3D("zx"));
      if (!hOverall) {
        cerr << "ERROR: cannot create whole-run projection for " << histName << endl;
        delete h3;
        return;
      } // end if (!hOverall)

      hOverall->SetDirectory(nullptr);
      const TString globalTag = Form("%s_%s_Overall", MODE_NAME[mode], VIEW_NAME[view]);

      // Keep the full-run measurement for reference comparisons, but produce no overall ROOT output.
      globalResults[mode][view] = Measure(hOverall, totalEvents, globalTag, nullptr);
      delete hOverall;

      // WHOLE-RUN LOOP / END: release the large histogram before the next view.
      // The TH3F alone can be hundreds of MiB; release it now.
      delete h3;
    } // end for view
  } // end for mode

  // PHASE 3 / END: all time-bin and whole-run measurements are available.
  // Print the complete whole-run summary once, after processing all modes/views.
  PrintOverallSummary(globalResults, nTimeBins, totalEvents, T_Start, T_Stop);

  // PHASE 4 / BEGIN: time-dependent stability and calibration comparisons.
  // Global-reference trends: position difference and sigma/charge ratios.
  // Loop over RAW, external calibration and run calibration.
  for (int mode = 0; mode < N_MODE; ++mode) {
    // Compare calibration modes separately in each coordinate view.
    for (int view = 0; view < N_VIEW; ++view) {
      for (int bin = 0; bin < nTimeBins; ++bin) {
        if (eventCounts[bin] <= 0) continue;
        const double timeCenter = meanUnix[bin];
        const double timeHalfWidth = meanUnixErr[bin];
        const Measurement &point = results[mode][view][bin];
        const Measurement &overall = globalResults[mode][view];

        // Compare direct method with the three spatial-profile fit methods.
        for (int method = 0; method <= N_MODEL; ++method) {
          const TString methodName = method == 0 ? "Direct" : MODEL_NAME[method - 1];
          const TString path = Form("VersusOverall/%s/%s/%s", MODE_NAME[mode], methodName.Data(), VIEW_NAME[view]);
          const double position = PhysicalMetric(point, method, 0);
          const double sigma = PhysicalMetric(point, method, 1);
          const double charge = PhysicalMetric(point, method, 2);
          const double positionGlobal = PhysicalMetric(overall, method, 0);
          const double sigmaGlobal = PhysicalMetric(overall, method, 1);
          const double chargeGlobal = PhysicalMetric(overall, method, 2);

          if (IsGood(position) && IsGood(positionGlobal)) AddComparison(fout.get(), path, "PositionMinusGlobal", "Position - whole-run position [mm]", timeCenter, timeHalfWidth, position - positionGlobal);
          if (IsGood(sigma) && IsGood(sigmaGlobal) && sigmaGlobal > 0.) AddComparison(fout.get(), path, "SigmaOverGlobal", "Sigma / whole-run sigma", timeCenter, timeHalfWidth, sigma / sigmaGlobal);
          if (IsGood(charge) && IsGood(chargeGlobal) && chargeGlobal > 0.) AddComparison(fout.get(), path, "ChargeOverGlobal", "Q / whole-run Q", timeCenter, timeHalfWidth, charge / chargeGlobal);
        } // end for method
      } // end for bin
    } // end for view
  } // end for mode

  // CALIBRATION COMPARISONS: RAW versus external/run calibrations.
  // Compare matched time bins and readout coordinates for each method.
  // Comparisons are only made between measurements that use the same time bins.
  // Compare calibration modes separately in each coordinate view.
  for (int view = 0; view < N_VIEW; ++view) {
    for (int bin = 0; bin < nTimeBins; ++bin) {
      if (eventCounts[bin] <= 0) continue;
      const double timeCenter = meanUnix[bin];
      const double timeHalfWidth = meanUnixErr[bin];
        
      // Same time bin, same readout; only correction method changes.
      const Measurement &raw = results[0][view][bin];
      const Measurement &ext = results[1][view][bin];
      const Measurement &run = results[2][view][bin];
      if (raw.direct.valid) {
        if (ext.direct.valid && raw.direct.charge > 0.) AddComparison(fout.get(), Form("Calibration/%s", VIEW_NAME[view]), "ExtOverRawCharge", "Q_ext / Q_raw", timeCenter, timeHalfWidth, ext.direct.charge / raw.direct.charge);
        if (run.direct.valid && raw.direct.charge > 0.) AddComparison(fout.get(), Form("Calibration/%s", VIEW_NAME[view]), "RunOverRawCharge", "Q_run / Q_raw", timeCenter, timeHalfWidth, run.direct.charge / raw.direct.charge);
        if (ext.direct.valid) AddComparison(fout.get(), Form("Calibration/%s", VIEW_NAME[view]), "ExtMinusRawPosition", "Position_ext - Position_raw [mm]", timeCenter, timeHalfWidth, ext.direct.position - raw.direct.position);
        if (run.direct.valid) AddComparison(fout.get(), Form("Calibration/%s", VIEW_NAME[view]), "RunMinusRawPosition", "Position_run - Position_raw [mm]", timeCenter, timeHalfWidth, run.direct.position - raw.direct.position);
      } // end if (raw.direct.valid)

      // Model-dependent observables, including several definitions of sigma.
      for (int model = 0; model < N_MODEL; ++model) {
        const ModelResult &r0 = raw.fit[model];
        const ModelResult &r1 = ext.fit[model];
        const ModelResult &r2 = run.fit[model];
        const TString path = Form("Calibration/%s/%s", VIEW_NAME[view], MODEL_NAME[model]);
        if (r0.valid && r1.valid) AddComparison(fout.get(), path, "ExtMinusRawPosition", "Position_ext - Position_raw [mm]", timeCenter, timeHalfWidth, r1.position - r0.position);
        if (r0.valid && r2.valid) AddComparison(fout.get(), path, "RunMinusRawPosition", "Position_run - Position_raw [mm]", timeCenter, timeHalfWidth, r2.position - r0.position);
        if (r0.valid && r1.valid && r0.sigma > 0.) AddComparison(fout.get(), path, "ExtOverRawSigma", "Sigma_ext / Sigma_raw", timeCenter, timeHalfWidth, r1.sigma / r0.sigma);
        if (r0.valid && r2.valid && r0.sigma > 0.) AddComparison(fout.get(), path, "RunOverRawSigma", "Sigma_run / Sigma_raw", timeCenter, timeHalfWidth, r2.sigma / r0.sigma);
      } // end for model
    } // end for bin
  } // end for view

  // Loop over RAW, external calibration and run calibration.
  for (int mode = 0; mode < N_MODE; ++mode) {
    for (int bin = 0; bin < nTimeBins; ++bin) {
      if (eventCounts[bin] <= 0) continue;
      const double timeCenter = meanUnix[bin];
      const double timeHalfWidth = meanUnixErr[bin];
        
      // X and Y are aligned using the same DAQ time bin.
      const Measurement &x = results[mode][0][bin];
      const Measurement &y = results[mode][1][bin];
      const TString path = Form("XY/%s", MODE_NAME[mode]);
      if (x.direct.valid && y.direct.valid) {
        AddComparison(fout.get(), path + "/Direct", "XMinusYPosition", "X - Y [mm]", timeCenter, timeHalfWidth, x.direct.position - y.direct.position);
        if (y.direct.rms > 0.) AddComparison(fout.get(), path + "/Direct", "SigmaXOverSigmaY", "RMS_X / RMS_Y", timeCenter, timeHalfWidth, x.direct.rms / y.direct.rms);
        if (y.direct.charge > 0.) AddComparison(fout.get(), path + "/Direct", "ChargeXOverChargeY", "Q_X / Q_Y", timeCenter, timeHalfWidth, x.direct.charge / y.direct.charge);
      } // end if (x.direct.valid && y.direct.valid)

      // Model-dependent observables, including several definitions of sigma.
      for (int model = 0; model < N_MODEL; ++model) {
        const ModelResult &fx = x.fit[model];
        const ModelResult &fy = y.fit[model];
        if (!fx.valid || !fy.valid) continue;
        const TString fitPath = path + "/" + MODEL_NAME[model];
        AddComparison(fout.get(), fitPath, "XMinusYPosition", "X - Y [mm]", timeCenter, timeHalfWidth, fx.position - fy.position);
        if (fy.sigma > 0.) AddComparison(fout.get(), fitPath, "SigmaXOverSigmaY", "Sigma_X / Sigma_Y", timeCenter, timeHalfWidth, fx.sigma / fy.sigma);
      } // end for model
    } // end for bin
  } // end for mode

  // PHASE 4 / END: correlations between observables and detector views.
  // PHASE 5 / BEGIN: correlations, graphs, distributions and closing files.
  BuildCorrelationHistograms(fout.get(), results, nTimeBins);

  // Flush cached time trends and form their 1D time-bin distributions.
  FinalizeGraphs(fout.get());
  fout->cd();
  TParameter<int>("analysisComplete", 1).Write();
  fout->Write();

  // PHASE 5 / END: ROOT output file has been written.
  fout->Close();
  fin->Close();
  cout << "ANALYSISTMM_SUCCESS: " << outputFile << endl;
} // end function AnalysisTMM()
