#define RecoTMM_cxx
#include "RecoTMM.h"

using namespace std;

constexpr double ADC_TO_PC = 300. * 1.6e-19 * 1.e12;  // pC / ADC

// helper not in the class
int TGraphAttribute(TGraphErrors *Graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
  Graph->SetTitle(title);
  Graph->SetName(title);
  Graph->GetXaxis()->SetTitle(xlabel);
  Graph->GetYaxis()->SetTitle(ylabel);
  Graph->SetMarkerStyle(markerstyle);
  Graph->SetMarkerColor(color);
  return 0;
}

double ClampToLimits(double x, double low, double up){
  if (x < low) return 0.5 * (low + up);
  if (x > up) return 0.5 * (low + up);
  return x;
}

// Clamp x to [xmin, xmax] and safely evaluate a TGraphErrors used as a
// per-strip correction factor. Returns 1.0 if the graph is missing, empty,
// or the evaluation is non-finite / non-positive.
static double SafeGraphEval(TGraphErrors *g, double x, double xmin, double xmax){
  if (!g) return 1.;
  if (g->GetN() <= 0) return 1.;
  double xc = x;
  if (xc < xmin) xc = xmin;
  if (xc > xmax) xc = xmax;
  double v = g->Eval(xc);
  if (!TMath::Finite(v) || v <= 0.) return 1.;
  return v;
}

// Sum the charge represented by the full TH2: sum_{x,y} N(x,y) * q_y.
// The uncertainty assumes Poisson fluctuations of the bin populations.
static void TH2WeightedCharge(const TH2D *h, double &charge, double &chargeErr){
  charge = 0.;
  chargeErr = 0.;
  if (!h) return;
  double err2 = 0.;
  for (int bx = 1; bx <= h->GetNbinsX(); ++bx) {
    for (int by = 1; by <= h->GetNbinsY(); ++by) {
      const double n = h->GetBinContent(bx, by);
      if (n == 0.) continue;
      const double q = h->GetYaxis()->GetBinCenter(by);
      charge += n * q;
      err2 += fabs(n) * q * q;
    }
  }
  charge *= ADC_TO_PC;
  chargeErr = sqrt(max(0.0, err2)) * ADC_TO_PC;
}

// class members

void RecoTMM::StripFinder(int &iReadout, int &iStrip) { 
  // ireadout: 0/1 from tree (x or y), istrip: strip number
  // remap to PADME local reference: x --> y ; y --> -x
  if(iReadout == 0) {
    iReadout = 1; // x  -->  y
    iStrip = iStrip;
  }
  else if(iReadout == 1) {
    iReadout = 0; // y  -->  -x
    iStrip = MAXSTRIP - iStrip - 1;
  }
}

void RecoTMM::GlobalCoordinate(int iReadout, double &x_strip){ 
  // reference system at chamber center, PADME frame
  if(iReadout == 0) { //X
    x_strip = x_strip + GLOBAL_X_TRANSLATION; 
  }
  else if(iReadout == 1) { //Y
    x_strip = x_strip + GLOBAL_Y_TRANSLATION;
  }
}

void RecoTMM::CoordinateFinder(int iStrip, const vector<short> &camp, int &iReadout, double &x_strip, double &q_strip, int &iStripLocal){

  if (camp.empty()) {
    x_strip = -999;
    q_strip = 0;
    iStripLocal = -1;
    return;
  }
  int Nbins = camp.size();
  double qmax = -1000;
  
  for (int ibin = 0; ibin < Nbins; ibin++) {
    double qbin = camp.at(ibin);
    if (qbin > qmax) qmax = qbin;
  }

  StripFinder(iReadout, iStrip); // remap to local PADME frame
  iStripLocal = iStrip;          // expose the remapped strip ID for calibration arrays

  // APV charge correction (after StripFinder so channels are mapped correctly)
  x_strip = iStrip * pitch;
  float APV_q_corr = 1.;
  if(iReadout == 0) {
    if (iStrip < 122) APV_q_corr = APV1_fqx;
    if (iStrip >= 122+113) APV_q_corr = APV3_fqx;
  }
  if(iReadout == 1) {
    if (iStrip < 122) APV_q_corr = APV1_fqy;
    if (iStrip >= 122+113) APV_q_corr = APV3_fqy;
  }
  q_strip = qmax*APV_q_corr;
  
  GlobalCoordinate(iReadout, x_strip);
}

bool RecoTMM::LoadCalibrationConstants(const string &filename){
  
  // Default: unity calibration
  for (int is = 0; is < MAXSTRIP; ++is) {
    XcalibConst[is] = 1.;
    XcalibErr[is]   = 0.;

    YcalibConst[is] = 1.;
    YcalibErr[is]   = 0.;
  }

  ifstream infile(filename.c_str());
  if (!infile.is_open()) {
    cerr << "Error: Cannot open calibration file " << filename << " - using unity calibration" << endl;
    return false;
  }

  int nX = 0;
  int nY = 0;

  string line;

  while (getline(infile, line)) {

    if (line.empty()) continue;
    if (line[0] == '#') continue;

    stringstream ss(line);

    int runID = -1;
    string view = "";
    int strip = -1;
    double corr = 1.;
    double ecorr = 0.;

    if (!(ss >> runID >> view >> strip >> corr >> ecorr)) {
      cerr << "WARNING: cannot parse calibration line: " << line << endl;
      continue;
    }

    if (strip < 0 || strip >= MAXSTRIP) {
      cerr << "WARNING: calibration strip out of range: " << "view = " << view << " - strip = " << strip  << " - line = " << line << endl;
      continue;
    }

    if (!TMath::Finite(corr) || corr <= 0.) {
      cerr << "WARNING: invalid calibration constant: " << "view = " << view << " - strip = " << strip << " - corr = " << corr << " ; using 1" << endl;
      corr = 1.;
      ecorr = 0.;
    }

    if (!TMath::Finite(ecorr) || ecorr < 0.) {
      cerr << "WARNING: invalid calibration error: " << "view = " << view << " - strip = " << strip << " - ecorr = " << ecorr << " ; using 0" << endl;
      ecorr = 0.;
    }

    if (view == "X") {
      XcalibConst[strip] = corr;
      XcalibErr[strip]   = ecorr;
      nX++;
    }
    else if (view == "Y") {
      YcalibConst[strip] = corr;
      YcalibErr[strip]   = ecorr;
      nY++;
    }
    else {
      cerr << "WARNING: unknown calibration view: " << view << " - line = " << line << endl;
    }
  }

  infile.close();

  if (nX != MAXSTRIP) {
    cerr << "WARNING: loaded " << nX << " X calibration constants instead of " << MAXSTRIP << ". Missing strips kept at 1." << endl;
  }

  if (nY != MAXSTRIP) {
    cerr << "WARNING: loaded " << nY << " Y calibration constants instead of " << MAXSTRIP << ". Missing strips kept at 1." << endl;
  }

  cout << "Calibration constants successfully loaded from " << filename << endl;
  cout << "Loaded X constants: " << nX << " / " << MAXSTRIP << endl;
  cout << "Loaded Y constants: " << nY << " / " << MAXSTRIP << endl;

  return true;
}

SliceFitResult RecoTMM::RunFitSlicesY(TH2D *H, TString tag){
  
  SliceFitResult out;
  if (!H) return out;

  TString oldName = H->GetName();
  TString uniqueName = Form("%s_%s", oldName.Data(), tag.Data());

  H->SetName(uniqueName);

  H->FitSlicesY();
  TH1D *h0 = (TH1D*)gDirectory->Get(Form("%s_0", H->GetName()));
  TH1D *h1 = (TH1D*)gDirectory->Get(Form("%s_1", H->GetName()));
  TH1D *h2 = (TH1D*)gDirectory->Get(Form("%s_2", H->GetName()));
  TH1D *h3 = (TH1D*)gDirectory->Get(Form("%s_chi2", H->GetName()));

  if (!h0 || !h1 || !h2 || !h3) {
    cerr << "ERROR: FitSlicesY failed for " << H->GetName() << endl;
    H->SetName(oldName);
    return out; 
  }

  out.amp   = (TH1D*)h0->Clone(Form("hAmpSlice_%s", tag.Data()));
  out.mean  = (TH1D*)h1->Clone(Form("hMeanSlice_%s", tag.Data()));
  out.sigma = (TH1D*)h2->Clone(Form("hSigmaSlice_%s", tag.Data()));
  out.chi2  = (TH1D*)h3->Clone(Form("hChi2Slice_%s", tag.Data()));

  out.amp->SetDirectory(0);
  out.mean->SetDirectory(0);
  out.sigma->SetDirectory(0);
  out.chi2->SetDirectory(0);

  gDirectory->Delete(Form("%s_0;*",    H->GetName()));
  gDirectory->Delete(Form("%s_1;*",    H->GetName()));
  gDirectory->Delete(Form("%s_2;*",    H->GetName()));
  gDirectory->Delete(Form("%s_chi2;*", H->GetName()));

  H->SetName(oldName);

  return out;
}

bool RecoTMM::IsFitAccepted(TFitResultPtr fitResult, int maxFitStatusAccepted, int minCovMatrixStatusAccepted){
  if (!fitResult.Get()) return false;
  if (!fitResult->IsValid()) return false;

  const int status = (int)fitResult;
  const int covStatus = fitResult->CovMatrixStatus();

  if (status > maxFitStatusAccepted) return false;
  if (covStatus < minCovMatrixStatusAccepted) return false;

  return true;
}

double RecoTMM::VoigtIntegralPDF(double *x, double *par){
  const double I      = par[0];          // total integral
  const double mu     = par[1];
  const double sigmaG = fabs(par[2]);    // Gaussian sigma
  const double gammaL = fabs(par[3]);    // Lorentzian HWHM

  if (I <= 0.) return 0.;
  if (sigmaG <= 0.) return 0.;
  if (gammaL <= 0.) return 0.;

  const double xx = x[0] - mu;

  // Gaussian FWHM
  const double kFWHM = 2. * sqrt(2. * log(2.));
  const double fG = kFWHM * sigmaG;

  // Lorentzian FWHM
  const double fL = 2. * gammaL;

  // Olivero-Longbothum approximation for the Voigt FWHM
  const double fV = 0.5346 * fL + sqrt(0.2166 * pow(fL, 2) + pow(fG, 2));

  if (fV <= 0.) return 0.;

  // Pseudo-Voigt mixing fraction
  const double r = fL / fV;

  double eta = 1.36603 * r - 0.47719 * pow(r, 2) + 0.11116 * pow(r, 3);

  eta = ClampToLimits(eta, 0., 1.);

  // Pseudo-Voigt uses Gaussian and Lorentzian with same FWHM = fV
  const double sigmaPV = fV / kFWHM;
  const double gammaPV = fV / 2.;

  if (sigmaPV <= 0.) return 0.;
  if (gammaPV <= 0.) return 0.;

  // Unit-area Gaussian
  const double G = 1. / (sqrt(2. * TMath::Pi()) * sigmaPV) * exp(-0.5 * pow(xx / sigmaPV, 2));

  // Unit-area Lorentzian
  const double L = (1. / TMath::Pi()) * gammaPV / (pow(xx, 2) + pow(gammaPV, 2));

  // Integral-normalized pseudo-Voigt
  return I * ((1. - eta) * G + eta * L);
}

TF1* RecoTMM::FitVoigt(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult){
  fitResult = TFitResultPtr();

  if (!h) {
    cerr << "ERROR: FitVoigt received null histogram: " << name << endl;
    return nullptr;
  }

  if (h->GetEntries() <= 0 || h->Integral() <= 0.) {
    cerr << "WARNING: FitVoigt skipped empty histogram: " << h->GetName() << " view = " << name << endl;
    return nullptr;
  }

  if (xmax <= xmin) {
    cerr << "ERROR: FitVoigt invalid fit range for " << h->GetName() << " tag = " << name << " xmin = " << xmin << " xmax = " << xmax << endl;
    return nullptr;
  }

  const int maxBin = h->GetMaximumBin();
  const double maxContent = h->GetBinContent(maxBin);

  if (maxContent <= 0.) {
    cerr << "WARNING: FitVoigt skipped histogram with non-positive maximum: " << h->GetName() << " view = " << name << endl;
    return nullptr;
  }

  TF1 *prefit = new TF1( Form("prefitVoigt_%s", name.Data()), "gaus", xmin, xmax);
  TFitResultPtr prefitResult = h->Fit(prefit, "RQSN");

  double amp0 = prefit->GetParameter(0);
  double mu0  = prefit->GetParameter(1);
  double s0   = fabs(prefit->GetParameter(2));

  // cout << "DEBUG: FitVoigt prefit results for " << h->GetName() << " view = " << name << " : amp0 = " << amp0 << ", mu0 = " << mu0 << ", s0 = " << s0 << endl;

  if ((int)prefitResult != 0) {
    cerr << "WARNING: Gaussian prefit failed for " << h->GetName() << " view = " << name << " status = " << (int)prefitResult << ". Using histogram maximum/RMS seeds." << endl;
    amp0 = maxContent;
    mu0  = h->GetBinCenter(maxBin);
    s0   = h->GetRMS();
  }

  if (amp0 <= 0.) amp0 = maxContent;
  if (mu0 < xmin || mu0 > xmax) mu0 = h->GetBinCenter(maxBin);
  if (s0 <= 0.) s0 = 0.1 * (xmax - xmin);

  const double xrange = xmax - xmin;

  if (s0 <= 0. || xrange <= 0.) {
    cerr << "ERROR: FitVoigt invalid seed/range for " << h->GetName() << " tag = " << name << " s0 = " << s0 << " xrange = " << xrange << endl;
    delete prefit;
    return nullptr;
  }

  const double area0 = max(1.0, amp0 * sqrt(2. * TMath::Pi()) * s0);

  TF1 *fit = new TF1(Form("fVoigt_%s", name.Data()), this, &RecoTMM::VoigtIntegralPDF, xmin, xmax, 4, "RecoTMM", "VoigtIntegralPDF");

  fit->SetParNames("I", "mean", "sigmaG", "gammaL");

  const double I_low = 0.;
  const double I_up  = max(1.0, 10. * area0);

  const double s_low = 0.1;
  const double s_up  = max(2. * s_low, xrange);

  const double g_low = 0.001;
  const double g_up  = max(2. * g_low, xrange);

  double I_0 = ClampToLimits(area0, I_low, I_up);
  double mu_0 = ClampToLimits(mu0, xmin, xmax);
  double s_0 = ClampToLimits(s0, s_low, s_up);
  double g_0 = ClampToLimits(0.5 * s0, g_low, g_up);

  fit->SetParameters(I_0, mu_0, s_0, g_0);

  fit->SetParLimits(0, I_low, I_up);
  fit->SetParLimits(1, xmin, xmax);
  fit->SetParLimits(2, s_low, s_up);
  fit->SetParLimits(3, g_low, g_up);

  fitResult = h->Fit(fit, "RQSN");

  if (!fitResult.Get()) {
    cerr << "WARNING: Voigt fit returned null result for " << h->GetName() << " view = " << name << endl;
    delete prefit;
    delete fit;
    fitResult = TFitResultPtr();
    return nullptr;
  }

  const int status = (int)fitResult;
  const int covStatus = fitResult->CovMatrixStatus();

  const bool accepted = IsFitAccepted(fitResult, 1, 2);

  if (!accepted) {
    cerr << "WARNING: Voigt fit NOT accepted for " << h->GetName() << " view = " << name << " fitStatus = " << status << " covStatus = " << covStatus << " isValid = " << fitResult->IsValid() << endl;
  }
  else {
    if (status == 1) {
      cerr << "WARNING: Voigt fit accepted with status = 1 for " << h->GetName() << " view = " << name << " ; covariance matrix may be non-ideal" << endl;
    }

    if (covStatus == 2) {
      cerr << "WARNING: Voigt covariance matrix accepted with CovMatrixStatus = 2 for " << h->GetName() << " view = " << name << " ; forced positive definite covariance" << endl;
    }
  }

  delete prefit;
  return fit;
}

TF1* RecoTMM::FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax, TFitResultPtr &fitResult){
  fitResult = TFitResultPtr();

  if (!h) {
    cerr << "ERROR: FitDoubleGaussian received null histogram: " << name << endl;
    return nullptr;
  }

  if (h->GetEntries() <= 0 || h->Integral() <= 0.) {
    cerr << "WARNING: FitDoubleGaussian skipped empty histogram: " << h->GetName() << " view = " << name << endl;
    return nullptr;
  }

  if (xmax <= xmin) {
    cerr << "ERROR: FitDoubleGaussian invalid fit range for " << h->GetName() << " tag = " << name << " xmin = " << xmin << " xmax = " << xmax << endl;
    return nullptr;
  }

  const int maxBin = h->GetMaximumBin();
  const double maxContent = h->GetBinContent(maxBin);

  if (maxContent <= 0.) {
    cerr << "WARNING: FitDoubleGaussian skipped histogram with non-positive maximum: " << h->GetName() << " view = " << name << endl;
    return nullptr;
  }

  TF1 *prefit = new TF1(Form("prefit_%s", name.Data()), "gaus", xmin, xmax);

  TFitResultPtr prefitResult = h->Fit(prefit, "RQSN");

  double amp0 = prefit->GetParameter(0);
  double mu0  = prefit->GetParameter(1);
  double s0   = fabs(prefit->GetParameter(2));

  // cout << "DEBUG: FitDoubleGaussian prefit results for " << h->GetName() << " view = " << name << " : amp0 = " << amp0 << ", mu0 = " << mu0 << ", s0 = " << s0 << endl;

  if ((int)prefitResult != 0) {
    cerr << "WARNING: Gaussian prefit failed for " << h->GetName() << " view =" << name << " status =" << (int)prefitResult << ". Using histogram maximum/RMS seeds." << endl;
    amp0 = maxContent;
    mu0  = h->GetBinCenter(maxBin);
    s0   = h->GetRMS();
  }

  if (amp0 <= 0.) amp0 = maxContent;
  if (mu0 < xmin || mu0 > xmax) mu0 = h->GetBinCenter(maxBin);
  if (s0 <= 0.) s0 = 0.1 * (xmax - xmin);

  const double xrange = xmax - xmin;

  if (s0 <= 0. || xrange <= 0.) {
    cerr << "ERROR: FitDoubleGaussian invalid seed/range for " << h->GetName() << " tag=" << name << " s0=" << s0 << " xrange=" << xrange << endl;
    delete prefit;
    return nullptr;
  }

  const double area0 = max(1.0, amp0 * sqrt(2. * TMath::Pi()) * s0);

  TF1 *fit = new TF1(
    Form("f_%s", name.Data()),
    "[0]/(sqrt(2*TMath::Pi())*[2])*exp(-0.5*((x-[1])/[2])^2)"
    "+[3]/(sqrt(2*TMath::Pi())*[4])*exp(-0.5*((x-[1])/[4])^2)",
    xmin,
    xmax
  );

  fit->SetParNames("I_1", "mean", "sigma_1", "I_2", "sigma_2");

  const double I_low = 0.;
  const double I_up  = max(1.0, 10. * area0);

  const double s_low = 0.1;
  const double s1_up = max(s_low * 2., xrange);
  const double s2_up = max(s_low * 2., 5. * xrange);

  double I1_0 = ClampToLimits(0.7 * area0, I_low, I_up);
  double I2_0 = ClampToLimits(0.3 * area0, I_low, I_up);
  double s1_0 = ClampToLimits(0.7 * s0,   s_low, s1_up);
  double s2_0 = ClampToLimits(2.0 * s0,   s_low, s2_up);
  double mu_0 = ClampToLimits(mu0,        xmin,  xmax);

  fit->SetParameters(I1_0, mu_0, s1_0, I2_0, s2_0);

  fit->SetParLimits(0, I_low, I_up);
  fit->SetParLimits(1, xmin, xmax);
  fit->SetParLimits(2, s_low, s1_up);
  fit->SetParLimits(3, I_low, I_up);
  fit->SetParLimits(4, s_low, s2_up);

  fitResult = h->Fit(fit, "RQSN");

  if (!fitResult.Get()) {
    cerr << "WARNING: Double Gaussian fit returned null result for " << h->GetName() << " view = " << name << endl;
    delete prefit;
    delete fit;
    fitResult = TFitResultPtr();
    return nullptr;
  }

  const int status = (int)fitResult;
  const int covStatus = fitResult->CovMatrixStatus();
  const bool accepted = IsFitAccepted(fitResult, 1, 2);

  if (!accepted) {
    cerr << "WARNING: Double Gaussian fit NOT accepted for "  << h->GetName() << " view = " << name << " fitStatus = " << status << " covStatus = " << covStatus << " isValid = " << fitResult->IsValid()  << endl;
  } else {
    if (status == 1) {
      cerr << "WARNING: Double Gaussian fit accepted with status = 1 for " << h->GetName() << " view = " << name << " ; covariance matrix may be non-ideal" << endl;
    }

    if (covStatus == 2) {
      cerr << "WARNING: Double Gaussian covariance matrix accepted with CovMatrixStatus = 2 for " << h->GetName() << " view = " << name << " ; forced positive definite covariance" << endl;
    }
  }
  delete prefit;
  return fit;
}

void RecoTMM::BuildFitRatio(TH1D *hMeanFull, TH1D *hSigmaFull, TF1 *fit, TGraphErrors *gRatio, TGraphErrors *gDiff){
  
  for (int bx = 1; bx <= hMeanFull->GetNbinsX(); bx++) {

    double x  = hMeanFull->GetBinCenter(bx);
    double q  = hMeanFull->GetBinContent(bx);
    double eq = hMeanFull->GetBinError(bx);
    double s  = hSigmaFull->GetBinContent(bx);
    double qfit = fit->Eval(x);

    if (q <= 0 || qfit <= 0 || s <= 0) continue;

    double ratio = qfit / q;
    double diff  = qfit - q;

    int ip = gRatio->GetN();
    gRatio->SetPoint(ip, x, ratio);
    gRatio->SetPointError(ip, 0., eq > 0 ? ratio * eq / q : 0.);

    int id = gDiff->GetN();
    gDiff->SetPoint(id, x, diff);
    gDiff->SetPointError(id, 0., eq);
  }
}

void RecoTMM::LoopFileList(TObjArray &inputFileNameList) {

  // ------------------------------------------------------------
  // check existence of input files, tree and number of events to process
  // ------------------------------------------------------------

  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR: empty input file list" << endl;
    return;
  }

  cout << "Number of input files: " << inputFileNameList.GetEntries() << endl;

  if (maxEvents > 0) {
    cout << "N entries requested: " << maxEvents << endl;
  } else {
    cout << "N entries requested: all available entries" << endl;
  }

  if (!fTree) {
    cerr << "ERROR: fTree is null in LoopFileList" << endl;
    return;
  }
  Long64_t runNEntries = fTree->GetEntries();
  if (runNEntries <= 0) {
    cerr << "ERROR: input chain has zero entries" << endl;
    return;
  }
  cout << "Found Tree 'apv_raw' with " << runNEntries << " entries" << endl;
  Long64_t nToProcess = runNEntries;

  if (maxEvents > 0 && maxEvents < runNEntries) {
    nToProcess = maxEvents;
  }

  cout << "Will process " << nToProcess << " entries" << endl;

  // ------------------------------------------------------------
  // DAQ time range and common time binning
  // ------------------------------------------------------------

  if (fTree->GetEntry(0) <= 0) {
    cerr << "ERROR: cannot read first entry" << endl;
    return;
  }

  // Observed full-chain limits, rounded to whole DAQ seconds.
  // The observed stop is EXCLUSIVE: max(daqTimeSec) + 1 includes its last second.
  // As before, these extrema refer to the whole chain, even in maxEvents test mode.
  const double observedTStartUnix = fTree->GetMinimum("daqTimeSec");
  const double observedTStopUnix  = fTree->GetMaximum("daqTimeSec") + 1.;
  if (!std::isfinite(observedTStartUnix) || !std::isfinite(observedTStopUnix) || observedTStopUnix <= observedTStartUnix) {
    cerr << "ERROR: invalid observed TMM DAQ time interval" << endl;
    return;
  }

  // RecoTarget convention: use externally provided absolute UNIX-time boundaries.
  // Both must be supplied together. A missing pair allows the old default for now,
  // so the existing launch scripts work until they are updated.
  const bool missingStart = std::isnan(tStartUnix);
  const bool missingStop = std::isnan(tStopUnix);
  if (missingStart != missingStop) {
    cerr << "ERROR: tStartUnix and tStopUnix must be supplied together" << endl;
    return;
  }
  const bool externalTimeBoundsUsed = !missingStart;
  if (externalTimeBoundsUsed && (!std::isfinite(tStartUnix) || !std::isfinite(tStopUnix) || tStopUnix <= tStartUnix)) {
    cerr << "ERROR: invalid external time boundaries: tStartUnix=" << tStartUnix << " tStopUnix=" << tStopUnix << endl;
    return;
  }

  // Always use the external boundaries when provided, even for subsecond differences.
  // This is necessary to keep the TH3 bins aligned with Target reconstruction.
  const double T_start = externalTimeBoundsUsed ? tStartUnix : observedTStartUnix;
  const double T_stop  = externalTimeBoundsUsed ? tStopUnix  : observedTStopUnix;
  const double startDifference = T_start - observedTStartUnix;
  const double stopDifference = T_stop - observedTStopUnix;
  const double timeCheckTolerance = 1.0; // seconds
  const bool startMismatch = externalTimeBoundsUsed && fabs(startDifference) > timeCheckTolerance;
  const bool stopMismatch  = externalTimeBoundsUsed && fabs(stopDifference) > timeCheckTolerance;

  if (!externalTimeBoundsUsed) {
    cerr << "WARNING: no external tStartUnix/tStopUnix supplied; using full-chain TMM extrema (legacy mode)" << endl;
  }
  if (startMismatch) {
    cerr << setprecision(17) << "WARNING: external tStartUnix (" << T_start
         << ") differs from TMM observed start (" << observedTStartUnix
         << ") by " << startDifference << " s; using external start" << endl;
  }
  if (stopMismatch) {
    cerr << setprecision(17) << "WARNING: external tStopUnix (" << T_stop
         << ") differs from TMM observed exclusive stop (" << observedTStopUnix
         << ") by " << stopDifference << " s; using external stop" << endl;
  }

  if (nBins <= 0) {
    cerr << "ERROR: number of time bins must be positive (nBins=" << nBins << ")" << endl;
    return;
  }

  // All TH3F time axes and time moments are relative to the EFFECTIVE T_start.
  // Events outside [T_start,T_stop) go into the histogram under/overflow bins;
  // they are also counted in nTimeUnderflow/nTimeOverflow, as in the old code.
  const double timeBinWidth = (T_stop - T_start) / nBins;
  if (!std::isfinite(timeBinWidth) || timeBinWidth <= 0.) {
    cerr << "ERROR: invalid histogram time-bin width" << endl;
    return;
  }
  const double T_max = T_stop;

  cout << setprecision(17);
  cout << "Observed TMM time boundaries: [" << observedTStartUnix << ", " << observedTStopUnix << ") s UNIX" << endl;
  cout << "Time boundaries used: [" << T_start << ", " << T_stop << ") s UNIX" << endl;
  cout << "Source: " << (externalTimeBoundsUsed ? "external (RecoTarget-aligned)" : "observed TMM (legacy)") << endl;
  cout << "Histogram duration: " << T_stop - T_start << " s" << endl;
  cout << "Number of time bins: " << nBins << endl;
  cout << "Time-bin width: " << timeBinWidth << " s" << endl;
  cout << "Histogram time interval: " << T_start << " - " << T_max << endl;

  // Allocate zero-initialized per-bin accumulators for the requested number of bins.
  // All three are reused for the two-pass reconstruction; filled only in the first pass.
  vtsum.reset(new double[nBins]());
  vtsum2.reset(new double[nBins]());
  vnn.reset(new int[nBins]());
  nTimeUnderflow = 0;
  nTimeOverflow = 0;

  g_TStart_Tstop = new TGraphErrors(2);
  TGraphAttribute(g_TStart_Tstop, "g_TStart_Tstop", "Boundary index", "UNIX Time [s]", 22, kBlack);
  g_TStart_Tstop->SetPoint(0,0,T_start);
  g_TStart_Tstop->SetPoint(1,1,T_stop);
  
  // ------------------------------------------------------------
  // Histograms and graphs: created once, before looping on events
  // ------------------------------------------------------------
  for (int l = 0; l < TMMCH_N_Readout; l++) {

    // --- Overall RAW (mode 1) ---
    hqmaxstrip[l] = new TH3F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, nBins, 0, T_stop-T_start,  1000, 0, 2000);
    hqmaxstrip[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip[l]->SetYTitle("Time [s]");
    hqmaxstrip[l]->SetZTitle("q_{max} [ADC counts]");

    hqmaxstripFull[l] = new TH3F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, nBins, 0, T_stop-T_start,  1000, 0, 2000);
    hqmaxstripFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull[l]->SetYTitle("Time [s]");
    hqmaxstripFull[l]->SetZTitle("q_{max} [ADC counts]");

    // --- Overall EXT-CAL (mode 2): external txt ---
    // hqmaxstrip_extcal[l] = new TH3F(Form("hqmaxstrip_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, nBins, 0, T_stop-T_start,  1000, 0, 2000);
    // hqmaxstrip_extcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    // hqmaxstrip_extcal[l]->SetYTitle("Time [s]");
    // hqmaxstrip_extcal[l]->SetZTitle("q_{max} calib [ADC counts]");

    hqmaxstripFull_extcal[l] = new TH3F(Form("hqmaxstripFull_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, nBins, 0, T_stop-T_start,  1000, 0, 2000);
    hqmaxstripFull_extcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_extcal[l]->SetYTitle("Time [s]");
    hqmaxstripFull_extcal[l]->SetZTitle("q_{max} calib [ADC counts]");

    // --- Overall RUN-CAL (mode 3): from g_RawCalibFullRatio ---
    // hqmaxstrip_runcal[l] = new TH3F(Form("hqmaxstrip_runcal%s", tmm_tag[l].Data()), TString("q_{max-runcal} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, nBins, 0, T_stop-T_start,  1000, 0, 2000);
    // hqmaxstrip_runcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    // hqmaxstrip_runcal[l]->SetYTitle("Time [s]");
    // hqmaxstrip_runcal[l]->SetZTitle("q_{max} runcal [ADC counts]");

    hqmaxstripFull_runcal[l] = new TH3F(Form("hqmaxstripFull_runcal%s", tmm_tag[l].Data()), TString("q_{max-runcal} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, nBins, 0, T_stop-T_start,  1000, 0, 2000);
    hqmaxstripFull_runcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_runcal[l]->SetYTitle("Time [s]");
    hqmaxstripFull_runcal[l]->SetZTitle("q_{max} runcal [ADC counts]");

    // --- overall run-based calibration graphs ---
    g_RawCalibFullDiff[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullDiff[l], Form("g_RawCalibFullDiff%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} - Q_{raw} [pC]", 22, kBlack);

    g_RawCalibFullRatio[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullRatio[l], Form("g_RawCalibFullRatio%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} / Q_{raw}", 22, kBlack);
  }

  // --- time distribution histograms ---
  // time association with Tree Index
  // g_SrsTimeStamp_iev = new TGraphErrors();
  // TGraphAttribute(g_SrsTimeStamp_iev, "g_SrsTimeStamp_iev", "Tree Index", "SrsTimeStamp", 22, kBlack);

  // g_DaqTimeSec_iev = new TGraphErrors();
  // TGraphAttribute(g_DaqTimeSec_iev, "g_DaqTimeSec_iev", "Tree Index", "DaqTimeSec [s]", 22, kBlack);

  // g_DaqTimeMicroSec_iev = new TGraphErrors();
  // TGraphAttribute(g_DaqTimeMicroSec_iev, "g_DaqTimeMicroSec_iev", "Tree Index", "DaqTimeMicroSec [#mus]", 22, kBlack);

  g_DaqTime_iev = new TGraphErrors();
  TGraphAttribute(g_DaqTime_iev, "g_DaqTime_iev", "Tree Index", "DaqTime [s]", 22, kBlack);

  // time association with tree event 
  // g_SrsTimeStamp_evt = new TGraphErrors();
  // TGraphAttribute(g_SrsTimeStamp_evt, "g_SrsTimeStamp_evt", "EvtID", "SrsTimeStamp", 22, kBlack);

  // g_DaqTimeSec_evt = new TGraphErrors();
  // TGraphAttribute(g_DaqTimeSec_evt, "g_DaqTimeSec_evt", "EvtID", "DaqTimeSec [s]", 22, kBlack);

  // g_DaqTimeMicroSec_evt = new TGraphErrors();
  // TGraphAttribute(g_DaqTimeMicroSec_evt, "g_DaqTimeMicroSec_evt", "EvtID", "DaqTimeMicroSec [#mus]", 22, kBlack);

  g_DaqTime_evt = new TGraphErrors();
  TGraphAttribute(g_DaqTime_evt, "g_DaqTime_evt", "EvtID", "DaqTime [s]", 22, kBlack);

  g_evt_vs_iev = new TGraphErrors();
  TGraphAttribute(g_evt_vs_iev, "g_evt_vs_iev", "Tree Index", "EvtID", 22, kBlack);

  // Persist the sufficient statistics of the actual event-time distribution.
  // Graph X = native TH3F time-bin center (relative to T_start); EX = half-bin width.
  // Graph Y = accumulated relative time [s], its square [s^2], or event count.
  // These are raw sums: no derived mean/error is calculated in reconstruction.
  g_vtsum = new TGraphErrors(nBins);
  TGraphAttribute(g_vtsum, "g_vtsum", "Time - T_{start} [s]", "#sum t_{i} [s]", 22, kBlack);
  g_vtsum2 = new TGraphErrors(nBins);
  TGraphAttribute(g_vtsum2, "g_vtsum2", "Time - T_{start} [s]", "#sum t_{i}^{2} [s^{2}]", 22, kBlack);
  g_vnn = new TGraphErrors(nBins);
  TGraphAttribute(g_vnn, "g_vnn", "Time - T_{start} [s]", "N_{events}", 22, kBlack);

  // ------------------------------------------------------------
  // global accumulators
  // ------------------------------------------------------------

  Long64_t RecoEntry = 0;

  // ------------------------------------------------------------
  // First pass: fill RAW histograms
  // ------------------------------------------------------------
  
  cout << "#### Creating output file " << outputFileName << endl;

  TFile *outfile = new TFile(outputFileName.Data(), "RECREATE");

  if (!outfile || outfile->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outputFileName << endl;
    if (outfile) delete outfile;
    return;
  }

  // Same ROOT-level time metadata as RecoTarget: can be retrieved by AnalysisTMM.
  // These are the EFFECTIVE external (or legacy) bounds, not the observed extrema.
  outfile->cd();
  TParameter<double>("tStartUnix", T_start).Write();
  TParameter<double>("tStopUnix", T_stop).Write();
  TParameter<int>("nBins", nBins).Write();
  TParameter<double>("binWidthSeconds", timeBinWidth).Write();
  // Validation/audit information, kept separate from the common time anchor.
  TParameter<double>("observedTStartUnix", observedTStartUnix).Write();
  TParameter<double>("observedTStopUnix", observedTStopUnix).Write();
  TParameter<int>("externalTimeBoundsUsed", externalTimeBoundsUsed ? 1 : 0).Write();
  TParameter<int>("timeBoundsMismatch", (startMismatch || stopMismatch) ? 1 : 0).Write();
  TParameter<int>("inputTruncated", nToProcess < runNEntries ? 1 : 0).Write();

  // Main output structure
  TDirectory *overalldir       = outfile->mkdir("Overall");
  TDirectory *rawoveralldir    = overalldir->mkdir("Raw");
  TDirectory *extcaloveralldir = overalldir->mkdir("Calib_external");
  TDirectory *runcaloveralldir = overalldir->mkdir("Calib_run");

  // event loop!
  for (Long64_t iev = 0; iev < nToProcess; ++iev) {

    Long64_t nb = fTree->GetEntry(iev);    
    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }

    if (RecoEntry % 1000 == 0) {
      float progress = (float)(RecoEntry) / nToProcess;
      cout << "Processed " << RecoEntry << " out of " << nToProcess << " entries (" << fixed << setprecision(2) << progress * 100 << "%)" << endl;
    }

    // fill the overall time-event association TGraphs
    // cout << "DEBUG: is iev equal to evt and RecoEntry? iev=" << iev << ", evt=" << evt << ", RecoEntry=" << RecoEntry << endl;
    double daqTime = (double)daqTimeSec + 1.e-6 * (double)daqTimeMicroSec;
    // Subtract whole seconds first to preserve microsecond resolution in the offset.
    const double tRel = ((double)daqTimeSec - T_start) + 1.e-6 * (double)daqTimeMicroSec;

    // Findfixbin retrieve the bin id when passing the variable of the axis.
    const int timeBin = hqmaxstripFull[0]->GetYaxis()->FindFixBin(tRel);
    if (timeBin >= 1 && timeBin <= nBins) {
      const size_t k = (size_t)(timeBin - 1);
      vtsum[k] += tRel;
      vtsum2[k] += tRel * tRel;
      ++vnn[k];
    } else if (timeBin == 0) {
      ++nTimeUnderflow;
    } else {
      ++nTimeOverflow;
    } // end time-bin accumulation

    int ipTime = g_DaqTime_iev->GetN();

    // g_SrsTimeStamp_iev->SetPoint(ipTime, iev, srsTimeStamp);
    // g_DaqTimeSec_iev->SetPoint(ipTime, iev, daqTimeSec);
    // g_DaqTimeMicroSec_iev->SetPoint(ipTime, iev, daqTimeMicroSec);
    g_DaqTime_iev->SetPoint(ipTime, iev, daqTime);

    // The SrsTimeStamp event graph is disabled; use the active DAQ event graph.
    ipTime = g_DaqTime_evt->GetN();
    // g_SrsTimeStamp_evt->SetPoint(ipTime, evt, srsTimeStamp);
    // g_DaqTimeSec_evt->SetPoint(ipTime, evt, daqTimeSec);
    // g_DaqTimeMicroSec_evt->SetPoint(ipTime, evt, daqTimeMicroSec);
    g_DaqTime_evt->SetPoint(ipTime, evt, daqTime);

    g_evt_vs_iev->SetPoint(iev, iev, evt);

    if (!mmLayer || !mmReadout || !mmStrip || !raw_q) {
      cerr << "WARNING: null branch pointer at event " << iev << endl;
      continue;
    }

    int firedstrip_size = min(
      min((int)mmLayer->size(),   (int)mmReadout->size()),
      min((int)mmStrip->size(),  (int)raw_q->size())
    );

    for (int j = 0; j < firedstrip_size; j++) {
      double x_strip = 0;
      double q_strip = 0;
      int    iStripLocal = -1;

      int channel = mmStrip->at(j);

      int iReadout = (int)mmReadout->at(j);
      int iReadout_new = iReadout - 88; // 'X'->0, 'Y'->1

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) continue;

      CoordinateFinder(channel, raw_q->at(j), iReadout_new, x_strip, q_strip, iStripLocal);

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) continue;

      if (channel % 2 == 0) {
        hqmaxstrip[iReadout_new]->Fill(x_strip, tRel, q_strip);
      }
      hqmaxstripFull[iReadout_new]->Fill(x_strip, tRel, q_strip);
    }

    RecoEntry++;
  }

  cout << "################################ " << endl;
  cout << "#### Total reconstructed events: " << RecoEntry << endl;
  cout << "################################ " << endl;
  cout << "#### Slice fit procedure to equalise channels + baseline removal" << endl;
  cout << "################################ " << endl;

  // ------------------------------------------------------------
  // Build overall calibration graphs from RAW
  // ------------------------------------------------------------
  for (int iR = 0; iR < TMMCH_N_Readout; iR++) {

    TH2D *h2 = nullptr;
    TH2D *h2Full = nullptr;
    h2 = (TH2D*)(hqmaxstrip[iR]->Project3D("zx"));
    h2Full = (TH2D*)(hqmaxstripFull[iR]->Project3D("zx"));

    SliceFitResult even = RunFitSlicesY(h2,     Form("%s_even", tmm_tag[iR].Data()));
    SliceFitResult full = RunFitSlicesY(h2Full, Form("%s_full", tmm_tag[iR].Data()));

    hAmpslice[iR]   = even.amp;
    hMeanslice[iR]  = even.mean;
    hSigmaslice[iR] = even.sigma;
    hChi2slice[iR]  = even.chi2;

    hAmpsliceFull[iR]   = full.amp;
    hMeansliceFull[iR]  = full.mean;
    hSigmasliceFull[iR] = full.sigma;
    hChi2sliceFull[iR]  = full.chi2;

    TFitResultPtr fitResultOverall;
    // TF1 *fit = FitDoubleGaussian(
    //   hMeanslice[iR], tmm_tag[iR],
    //   StripMin * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION),
    //   StripMax * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION),
    //   fitResultOverall
    // );

    TF1 *fit = FitVoigt(
      hMeanslice[iR], tmm_tag[iR],
      StripMin * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION),
      StripMax * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION),
      fitResultOverall
    );

    if (!fit || !fitResultOverall.Get()) {
      cerr << "WARNING: overall fit failed for readout=" << tmm_tag[iR] << endl;
      if (fit) {
        delete fit;
        fit = nullptr;
      }
      continue;
    }

    bool approved = IsFitAccepted(fitResultOverall, 1, 2 );
    if (approved) {
        BuildFitRatio(hMeansliceFull[iR], hSigmasliceFull[iR], fit, g_RawCalibFullRatio[iR], g_RawCalibFullDiff[iR] );
    } else {
      cerr << "WARNING: overall calibration fit rejected" << " readout = " << tmm_tag[iR] << " fitStatus = " << (int)fitResultOverall << " covStatus = " << fitResultOverall->CovMatrixStatus() << endl;
    }
    delete fit;
    fit = nullptr;
  }

  // ------------------------------------------------------------
  // Load external calibration constants ONCE before the second pass
  // ------------------------------------------------------------
  bool extCalibOk = LoadCalibrationConstants(externalCalibFile.Data());
  if (!extCalibOk) {
    cerr << "WARNING: external calibration file not loaded; mode 2 (extcal) histograms will use unity correction." << endl;
  }

  // Physical domain of the run-based calibration graph (mm), used to clamp Eval().
  // x_strip = iStripLocal + GLOBAL_*_TRANSLATION; valid strip IDs are [StripMin, StripMax].
  double xCalMin[TMMCH_N_Readout] = {0., 0.};
  double xCalMax[TMMCH_N_Readout] = {0., 0.};
  for (int iR = 0; iR < TMMCH_N_Readout; iR++) {
    double tr = (iR == 0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION;
    xCalMin[iR] = StripMin * pitch + tr;
    xCalMax[iR] = StripMax * pitch + tr;
  }

  // ------------------------------------------------------------
  // Second pass: fill calibrated histograms (modes 2, 3 overall; 2, 3)
  // ------------------------------------------------------------

  for (Long64_t iev = 0; iev < nToProcess; ++iev) {

    Long64_t nb = fTree->GetEntry(iev);    
    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }
    if (!mmLayer || !mmReadout || !mmStrip || !raw_q) {
      cerr << "WARNING: null branch pointer at event " << iev << endl;
      continue;
    }

    const double tRel = ((double)daqTimeSec - T_start) + 1.e-6 * (double)daqTimeMicroSec;

    int firedstrip_size = min(
      min((int)mmLayer->size(),   (int)mmReadout->size()),
      min((int)mmStrip->size(),  (int)raw_q->size())
    );

    for (int j = 0; j < firedstrip_size; j++) {
      double x_strip = 0;
      double q_strip = 0;
      int    iStripLocal = -1;

      int channel = mmStrip->at(j);

      int iReadout = (int)mmReadout->at(j);
      int iReadout_new = iReadout - 88;

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) continue;

      CoordinateFinder(channel, raw_q->at(j), iReadout_new, x_strip, q_strip, iStripLocal);

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) continue;

      if (channel > MAXSTRIP) cout << "ERROR channel bigger than RawCalibFullRatio vector - channel = " << channel << endl;

      // ---- Mode 2: external txt (indexed by REMAPPED local strip ID) ----
      double extcorr = 1.;
      if (iStripLocal >= 0 && iStripLocal < MAXSTRIP) {
        if (iReadout_new == 0) extcorr = XcalibConst[iStripLocal];
        if (iReadout_new == 1) extcorr = YcalibConst[iStripLocal];
      }

      // ---- Mode 3: overall run-based (clamped Eval on g_RawCalibFullRatio) ----
      double runcorr = SafeGraphEval(
        g_RawCalibFullRatio[iReadout_new],
        x_strip,
        xCalMin[iReadout_new],
        xCalMax[iReadout_new]
      );

      // ---- Overall fills (3 modes; mode 1 was filled in the first pass) ----
      // if (channel % 2 == 0) {
      //   hqmaxstrip_extcal[iReadout_new]->Fill(x_strip, q_strip * extcorr, daqTime-T_start);
      //   hqmaxstrip_runcal[iReadout_new]->Fill(x_strip, q_strip * runcorr, daqTime-T_start);
      // }
      hqmaxstripFull_extcal[iReadout_new]->Fill(x_strip, tRel, q_strip * extcorr);
      hqmaxstripFull_runcal[iReadout_new]->Fill(x_strip, tRel, q_strip * runcorr);
    }
  }

  // filling the vtsum, vtsum2, vnn graphs
  for (int k = 0; k < nBins; ++k) {
    const double center = hqmaxstripFull[0]->GetYaxis()->GetBinCenter(k + 1);
    const double halfWidth = 0.5 * hqmaxstripFull[0]->GetYaxis()->GetBinWidth(k + 1);
    g_vtsum->SetPoint(k, center, vtsum[k]);
    g_vtsum->SetPointError(k, halfWidth, 0.);
    g_vtsum2->SetPoint(k, center, vtsum2[k]);
    g_vtsum2->SetPointError(k, halfWidth, 0.);
    g_vnn->SetPoint(k, center, (double)vnn[k]);
    g_vnn->SetPointError(k, halfWidth, 0.);
  } // end time-bin output loop
  
  // --- Overall RAW + slice histograms + calibration graphs ---
  outfile->cd();
  overalldir->cd();

  // if(g_SrsTimeStamp_iev) g_SrsTimeStamp_iev->Write();
  // if(g_DaqTimeSec_iev) g_DaqTimeSec_iev->Write();
  // if(g_DaqTimeMicroSec_iev) g_DaqTimeMicroSec_iev->Write();
  if(g_DaqTime_iev) g_DaqTime_iev->Write();
  // if(g_SrsTimeStamp_evt) g_SrsTimeStamp_evt->Write();
  // if(g_DaqTimeSec_evt) g_DaqTimeSec_evt->Write();
  // if(g_DaqTimeMicroSec_evt) g_DaqTimeMicroSec_evt->Write();
  if(g_DaqTime_evt) g_DaqTime_evt->Write();
  if(g_evt_vs_iev) g_evt_vs_iev->Write();
  if(g_TStart_Tstop) g_TStart_Tstop->Write();

  if(g_vtsum) g_vtsum->Write();
  if(g_vtsum2) g_vtsum2->Write();
  if(g_vnn) g_vnn->Write();
  const Long64_t nCounted = accumulate(vnn.get(), vnn.get() + nBins, 0LL);
  cout << "Time-bin event statistics: " << nCounted << " counted, " << nTimeUnderflow << " underflow, " << nTimeOverflow << " overflow" << endl;
  // Time-range diagnostics for the downstream analysis, saved at ROOT file level.
  outfile->cd();
  TParameter<int>("nTimeUnderflow", nTimeUnderflow).Write();
  TParameter<int>("nTimeOverflow", nTimeOverflow).Write();
  
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    overalldir->cd();
    rawoveralldir->cd();
    hqmaxstrip[l]->Write();
    hqmaxstripFull[l]->Write();
    if (hAmpslice[l])           hAmpslice[l]->Write();
    if (hMeanslice[l])          hMeanslice[l]->Write();
    if (hSigmaslice[l])         hSigmaslice[l]->Write();
    if (hChi2slice[l])          hChi2slice[l]->Write();
    if (hAmpsliceFull[l])       hAmpsliceFull[l]->Write();
    if (hMeansliceFull[l])      hMeansliceFull[l]->Write();
    if (hSigmasliceFull[l])     hSigmasliceFull[l]->Write();
    if (hChi2sliceFull[l])      hChi2sliceFull[l]->Write();
    if (g_RawCalibFullDiff[l])  g_RawCalibFullDiff[l]->Write();
    if (g_RawCalibFullRatio[l]) g_RawCalibFullRatio[l]->Write();
  }

  // --- Overall EXT-CAL (mode 2) ---
  outfile->cd();
  overalldir->cd();
  extcaloveralldir->cd();
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    // hqmaxstrip_extcal[l]->Write();
    hqmaxstripFull_extcal[l]->Write();
  }

  // --- Overall RUN-CAL (mode 3) ---
  outfile->cd();
  overalldir->cd();
  runcaloveralldir->cd();
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    // hqmaxstrip_runcal[l]->Write();
    hqmaxstripFull_runcal[l]->Write();
  }

  outfile->Write();
  outfile->Close();
  delete outfile;
  outfile = nullptr;

  cout << "Output written to " << outputFileName << endl;
}
