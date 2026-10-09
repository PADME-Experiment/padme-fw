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

static bool GetBlockGraphPoint(TGraphErrors *g, int iblock, double &value, double &error){
  if (!g) return false;
  int p = g->GetN();
  for (int i = 0; i < p; ++i) {
    double x, y;
    g->GetPoint(i, x, y);
    if ((int)x == iblock) {
      value = y;
      error = g->GetErrorY(i);
      return true;
    }
  }
  return false;
}

// Sum the charge represented by the full TH2: sum_{x,y} N(x,y) * q_y.
// The uncertainty assumes Poisson fluctuations of the bin populations.
static void TH2WeightedCharge(const TH2F *h, double &charge, double &chargeErr){
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

SliceFitResult RecoTMM::RunFitSlicesY(TH2F *H, TString tag){
  
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

void RecoTMM::LoopFileList(TObjArray &inputFileNameList, int NevtBlock) {

  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR: empty input file list" << endl;
    return;
  }

  if (NevtBlock <= 0) {
    cerr << "ERROR: NevtBlock must be > 0. Current value = " << NevtBlock << endl;
    return;
  }

  cout << "Number of input files: " << inputFileNameList.GetEntries() << endl;

  if (maxEvents > 0) {
    cout << "N entries requested: " << maxEvents << endl;
  } else {
    cout << "N entries requested: all available entries" << endl;
  }

  cout << "Block size: " << NevtBlock << " input-tree entries" << endl;

  // ------------------------------------------------------------
  // Histograms and graphs: created once, before looping on events
  // ------------------------------------------------------------
  for (int l = 0; l < TMMCH_N_Readout; l++) {

    // --- Overall RAW (mode 1) ---
    hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 2500, 0, 2500);
    // hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 2500, 0, 2500);
    hqmaxstrip[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip[l]->SetYTitle("q_{max} [ADC counts]");

    hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 2500, 0, 2500);
    // hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 2500, 0, 2500);
    hqmaxstripFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull[l]->SetYTitle("q_{max} [ADC counts]");

    // --- Overall EXT-CAL (mode 2): external txt ---
    hqmaxstrip_extcal[l] = new TH2F(Form("hqmaxstrip_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 2500, 0, 2500);
    // hqmaxstrip_extcal[l] = new TH2F(Form("hqmaxstrip_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 2500, 0, 2500);
    hqmaxstrip_extcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip_extcal[l]->SetYTitle("q_{max} calib [ADC counts]");

    hqmaxstripFull_extcal[l] = new TH2F(Form("hqmaxstripFull_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 2500, 0, 2500);
    // hqmaxstripFull_extcal[l] = new TH2F(Form("hqmaxstripFull_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 2500, 0, 2500);
    hqmaxstripFull_extcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_extcal[l]->SetYTitle("q_{max} calib [ADC counts]");

    // --- Overall RUN-CAL (mode 3): from g_RawCalibFullRatio ---
    hqmaxstrip_runcal[l] = new TH2F(Form("hqmaxstrip_runcal%s", tmm_tag[l].Data()), TString("q_{max-runcal} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 2500, 0, 2500);
    hqmaxstrip_runcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip_runcal[l]->SetYTitle("q_{max} runcal [ADC counts]");

    hqmaxstripFull_runcal[l] = new TH2F(Form("hqmaxstripFull_runcal%s", tmm_tag[l].Data()), TString("q_{max-runcal} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 2500, 0, 2500);
    hqmaxstripFull_runcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_runcal[l]->SetYTitle("q_{max} runcal [ADC counts]");

    // --- block-level monitoring TGraphErrors ---
    g_BlockBeamSpot[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpot[l], Form("g_BlockBeamSpot_%s", tmm_tag[l].Data()), "entry", Form("%s [mm]", tmm_tag[l].Data()), 20, kBlue + l);

    g_BlockBeamSpread[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpread[l], Form("g_BlockBeamSpread_%s", tmm_tag[l].Data()), "entry", Form("#sigma_{%s} [mm]", tmm_tag[l].Data()), 21, kRed + l);

    g_BlockBeamChargeFit[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamChargeFit[l], Form("g_BlockBeamChargeFit_%s", tmm_tag[l].Data()), "Block ID", "q_{Beam}^{fit} [pC]", 22, kGreen + 2 + l);

    g_BlockBeamChargeIntegral[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamChargeIntegral[l], Form("g_BlockBeamChargeIntegral_%s", tmm_tag[l].Data()), "Block ID", "q_{Beam}^{integral} [pC]", 22, kBlue + 2 + l);

    g_DaqTimeBeamSpot[l] = new TGraphErrors();
    TGraphAttribute(g_DaqTimeBeamSpot[l], Form("g_DaqTimeBeamSpot_%s", tmm_tag[l].Data()), "DAQ time [s]", Form("%s [mm]", tmm_tag[l].Data()), 20, kBlue + l);

    g_DaqTimeBeamSpread[l] = new TGraphErrors();
    TGraphAttribute(g_DaqTimeBeamSpread[l], Form("g_DaqTimeBeamSpread_%s", tmm_tag[l].Data()), "DAQ time [s]", Form("#sigma_{%s} [mm]", tmm_tag[l].Data()), 21, kRed + l);

    g_DaqTimeBeamChargeFit[l] = new TGraphErrors();
    TGraphAttribute(g_DaqTimeBeamChargeFit[l], Form("g_DaqTimeBeamChargeFit_%s", tmm_tag[l].Data()), "DAQ time [s]", Form("q_{%s}^{fit} [pC]", tmm_tag[l].Data()), 22, kGreen + 2 + l);

    g_DaqTimeBeamChargeIntegral[l] = new TGraphErrors();
    TGraphAttribute(g_DaqTimeBeamChargeIntegral[l], Form("g_DaqTimeBeamChargeIntegral_%s", tmm_tag[l].Data()), "DAQ time [s]", Form("q_{%s}^{integral} [pC]", tmm_tag[l].Data()), 22, kBlue + 2 + l);

    g_TreeIndexBeamChargeFit[l] = new TGraphErrors();
    TGraphAttribute(g_TreeIndexBeamChargeFit[l], Form("g_TreeIndexBeamChargeFit_%s", tmm_tag[l].Data()), "Mean tree index", Form("q_{%s}^{fit} [pC]", tmm_tag[l].Data()), 22, kGreen + 2 + l);

    g_TreeIndexBeamChargeIntegral[l] = new TGraphErrors();
    TGraphAttribute(g_TreeIndexBeamChargeIntegral[l], Form("g_TreeIndexBeamChargeIntegral_%s", tmm_tag[l].Data()), "Mean tree index", Form("q_{%s}^{integral} [pC]", tmm_tag[l].Data()), 22, kBlue + 2 + l);

    // --- overall run-based calibration graphs ---
    g_RawCalibFullDiff[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullDiff[l], Form("g_RawCalibFullDiff%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} - Q_{raw} [pC]", 22, kBlack);

    g_RawCalibFullRatio[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullRatio[l], Form("g_RawCalibFullRatio%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} / Q_{raw}", 22, kBlack);
  }

  // --- time distribution histograms ---
  // time association with Tree Index
  g_SrsTimeStamp_iev = new TGraphErrors();
  TGraphAttribute(g_SrsTimeStamp_iev, "g_SrsTimeStamp_iev", "Tree Index", "SrsTimeStamp", 22, kBlack);

  g_DaqTimeSec_iev = new TGraphErrors();
  TGraphAttribute(g_DaqTimeSec_iev, "g_DaqTimeSec_iev", "Tree Index", "DaqTimeSec [s]", 22, kBlack);

  g_DaqTimeMicroSec_iev = new TGraphErrors();
  TGraphAttribute(g_DaqTimeMicroSec_iev, "g_DaqTimeMicroSec_iev", "Tree Index", "DaqTimeMicroSec [#mus]", 22, kBlack);

  g_DaqTime_iev = new TGraphErrors();
  TGraphAttribute(g_DaqTime_iev, "g_DaqTime_iev", "Tree Index", "DaqTime [s]", 22, kBlack);

  // time association with tree event 
  g_SrsTimeStamp_evt = new TGraphErrors();
  TGraphAttribute(g_SrsTimeStamp_evt, "g_SrsTimeStamp_evt", "EvtID", "SrsTimeStamp", 22, kBlack);

  g_DaqTimeSec_evt = new TGraphErrors();
  TGraphAttribute(g_DaqTimeSec_evt, "g_DaqTimeSec_evt", "EvtID", "DaqTimeSec [s]", 22, kBlack);

  g_DaqTimeMicroSec_evt = new TGraphErrors();
  TGraphAttribute(g_DaqTimeMicroSec_evt, "g_DaqTimeMicroSec_evt", "EvtID", "DaqTimeMicroSec [#mus]", 22, kBlack);

  g_DaqTime_evt = new TGraphErrors();
  TGraphAttribute(g_DaqTime_evt, "g_DaqTime_evt", "EvtID", "DaqTime [s]", 22, kBlack);

  //block id - time association
  g_BlockMeanDaqSec = new TGraphErrors();
  TGraphAttribute(g_BlockMeanDaqSec, "g_BlockMeanDaqSec", "Block ID", "Mean DAQ Sec  [s]", 22, kBlack);

  g_BlockMeanDaqTime = new TGraphErrors();
  TGraphAttribute(g_BlockMeanDaqTime, "g_BlockMeanDaqTime", "Block ID", "Mean DAQ time [s]", 22, kBlack);

  g_BlockMeanSrsTime = new TGraphErrors();
  TGraphAttribute(g_BlockMeanSrsTime, "g_BlockMeanSrsTime", "Block ID", "Mean SRS timestamp", 22, kBlack);

  g_BlockMeanTreeIndex = new TGraphErrors();
  TGraphAttribute(g_BlockMeanTreeIndex, "g_BlockMeanTreeIndex", "Block ID", "Mean tree index", 22, kBlack);

  g_evt_vs_iev = new TGraphErrors();
  TGraphAttribute(g_evt_vs_iev, "g_evt_vs_iev", "Tree Index", "EvtID", 22, kBlack);

  g_DaqTimeBeamChargeFitTotal = new TGraphErrors();
  TGraphAttribute(g_DaqTimeBeamChargeFitTotal, "g_DaqTimeBeamChargeFitTotal", "DAQ time [s]", "Q_{tot}^{fit} [pC]", 22, kBlack);

  g_DaqTimeBeamChargeIntegralTotal = new TGraphErrors();
  TGraphAttribute(g_DaqTimeBeamChargeIntegralTotal, "g_DaqTimeBeamChargeIntegralTotal", "DAQ time [s]", "Q_{tot}^{integral} [pC]", 22, kBlack);

  g_TreeIndexBeamChargeFitTotal = new TGraphErrors();
  TGraphAttribute(g_TreeIndexBeamChargeFitTotal, "g_TreeIndexBeamChargeFitTotal", "Mean tree index", "Q_{tot}^{fit} [pC]", 22, kBlack);

  g_TreeIndexBeamChargeIntegralTotal = new TGraphErrors();
  TGraphAttribute(g_TreeIndexBeamChargeIntegralTotal, "g_TreeIndexBeamChargeIntegralTotal", "Mean tree index", "Q_{tot}^{integral} [pC]", 22, kBlack);

  // ------------------------------------------------------------
  // Block/global accumulators
  // ------------------------------------------------------------

  Long64_t RecoEntry = 0;

  // ------------------------------------------------------------
  // First pass: fill RAW histograms
  // ------------------------------------------------------------
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

  const int nBlocks = (int)((nToProcess + NevtBlock - 1) / NevtBlock);
  cout << "Number of tree-index blocks: " << nBlocks << endl;

  blockTimeInfo.clear();
  blockTimeInfo.resize(nBlocks);

  for (int l = 0; l < TMMCH_N_Readout; ++l) {
    g_BlockRawCalibFullRatio[l].clear();
    g_BlockRawCalibFullDiff[l].clear();
    g_BlockRawCalibFullRatio[l].resize(nBlocks, nullptr);
    g_BlockRawCalibFullDiff[l].resize(nBlocks, nullptr);
  }

  cout << "#### Creating output file " << outputFileName << endl;

  TFile *outfile = new TFile(outputFileName.Data(), "RECREATE");

  if (!outfile || outfile->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outputFileName << endl;
    if (outfile) delete outfile;
    return;
  }

  // Main output structure
  TDirectory *overalldir       = outfile->mkdir("Overall");
  TDirectory *rawoveralldir    = overalldir->mkdir("Raw");
  TDirectory *extcaloveralldir = overalldir->mkdir("Calib_external");
  TDirectory *runcaloveralldir = overalldir->mkdir("Calib_run");

  TDirectory *blockdir = outfile->mkdir("Blocks");

  // helper to create blockdir subdirectories
  auto GetBlockSubdir = [&](int iblock, const char *subdirName) -> TDirectory *{
    blockdir->cd();
    TString blockName = Form("block_%04d", iblock);
    TDirectory *thisblockdir = blockdir->GetDirectory(blockName);
    if (!thisblockdir) thisblockdir = blockdir->mkdir(blockName);
    TDirectory *subdir = thisblockdir->GetDirectory(subdirName);
    if (!subdir) subdir = thisblockdir->mkdir(subdirName);
    return subdir;
  };

  // histograms di appoggio
  TH2F *currentBlockRaw[TMMCH_N_Readout] = {nullptr};
  TH2F *currentBlockRawFull[TMMCH_N_Readout] = {nullptr}; 

  // creation histo lambda function
  auto CreateRawBlock = [&](int iblock) {
    for (int l = 0; l < TMMCH_N_Readout; ++l) {
      currentBlockRaw[l] = new TH2F(Form("hBlockqmaxstrip%s_block%04d", tmm_tag[l].Data(), iblock),
          TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentBlockRaw[l]->SetDirectory(nullptr);
      currentBlockRaw[l]->SetXTitle(TString(tmm_tag[l] + " [mm]").Data());
      currentBlockRaw[l]->SetYTitle("q_{max} [ADC counts]");

      currentBlockRawFull[l] = new TH2F(Form("hBlockqmaxstripFull%s_block%04d", tmm_tag[l].Data(), iblock),
          TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentBlockRawFull[l]->SetDirectory(nullptr);
      currentBlockRawFull[l]->SetXTitle(TString(tmm_tag[l] + " [mm]").Data());
      currentBlockRawFull[l]->SetYTitle("q_{max} [ADC counts]");
    }
  };

  auto FinalizeRawBlock = [&](int iblock) {
    cout << "Finalizing RAW block " << iblock << endl;

    TDirectory *rawblockdir = GetBlockSubdir(iblock, "Raw");

    for (int iR = 0; iR < TMMCH_N_Readout; ++iR) {
      // ------------------------------------------------
      // FitSlicesY
      // ------------------------------------------------
      SliceFitResult s = RunFitSlicesY(currentBlockRaw[iR],Form("%s_block%d", tmm_tag[iR].Data(), iblock));
      SliceFitResult sFull = RunFitSlicesY(currentBlockRawFull[iR],Form("%s_block%d_full", tmm_tag[iR].Data(), iblock));

      // ------------------------------------------------
      // Create the small calibration graphs
      // These MUST survive until second pass
      // ------------------------------------------------

      g_BlockRawCalibFullDiff[iR][iblock] = new TGraphErrors();
      TGraphAttribute(g_BlockRawCalibFullDiff[iR][iblock], Form("g_BlockRawCalibFullDiff%s", tmm_tag[iR].Data()), Form("%s [mm]", tmm_tag[iR].Data()), "Q_{calib} - Q_{raw} [ADC counts]", 22, kBlack);
      
      g_BlockRawCalibFullRatio[iR][iblock] = new TGraphErrors();
      TGraphAttribute(g_BlockRawCalibFullRatio[iR][iblock], Form("g_BlockRawCalibFullRatio%s", tmm_tag[iR].Data()), Form("%s [mm]", tmm_tag[iR].Data()), "Q_{calib} / Q_{raw} [ADC counts]", 22, kBlack);

      // ------------------------------------------------
      // Same beam-profile fit you currently perform
      // ------------------------------------------------

      if (s.mean) {
        TFitResultPtr fitResult;
        const double xini = StripMin * pitch + ((iR == 0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION);
        const double xfin = StripMax * pitch + ((iR == 0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION);

        TF1 *fb = FitVoigt(s.mean, Form("%s_block%d", tmm_tag[iR].Data(), iblock), xini, xfin, fitResult);

        if (!fb || !fitResult.Get()) {
          cerr << "WARNING: block fit failed" << " readout = " << tmm_tag[iR] << " block = " << iblock << endl;
        } else if (!IsFitAccepted(fitResult, 1, 2)) {
          cerr << "WARNING: block fit rejected" << " readout = " << tmm_tag[iR] << " block = " << iblock << endl;
          delete fb;
          fb = nullptr;
        } else {
          const double I = fb->GetParameter(0);
          const double mu = fb->GetParameter(1);
          const double s1 = fabs(fb->GetParameter(2));
          const double s2 = fabs(fb->GetParameter(3));
          const double e_mu = fb->GetParError(1);

          if (I > 0.) {
            const double sigma_eff = sqrt(s1 * s1 + s2 * s2);
            const double charge_proxy = I * ADC_TO_PC / (double)NevtBlock;
            const double charge_proxy_err = fb->GetParError(0) * ADC_TO_PC / (double)NevtBlock;

            int p = g_BlockBeamSpot[iR]->GetN();
            g_BlockBeamSpot[iR]->SetPoint(p, iblock, mu );
            g_BlockBeamSpot[iR]->SetPointError(p, 0., e_mu );

            p = g_BlockBeamSpread[iR]->GetN();
            g_BlockBeamSpread[iR]->SetPoint(p, iblock, sigma_eff );
            g_BlockBeamSpread[iR]->SetPointError(p, 0., 0. );

            p = g_BlockBeamChargeFit[iR]->GetN();
            g_BlockBeamChargeFit[iR]->SetPoint(p, iblock, charge_proxy);
            g_BlockBeamChargeFit[iR]->SetPointError( p,0.,charge_proxy_err);
          }

          if (sFull.mean && sFull.sigma) {
            BuildFitRatio(sFull.mean, sFull.sigma, fb, g_BlockRawCalibFullRatio[iR][iblock], g_BlockRawCalibFullDiff[iR][iblock] );
          }

          delete fb;
          fb = nullptr;
        }
      }

      // ------------------------------------------------
      // Additional charge proxy: weighted sum of all bins of qmaxstripFull
      // ------------------------------------------------
      double chargeTH2 = 0.;
      double chargeTH2Err = 0.;
      TH2WeightedCharge(currentBlockRawFull[iR], chargeTH2, chargeTH2Err);
      chargeTH2 /= (double)NevtBlock;
      chargeTH2Err /= (double)NevtBlock;
      int pChargeTH2 = g_BlockBeamChargeIntegral[iR]->GetN();
      g_BlockBeamChargeIntegral[iR]->SetPoint(pChargeTH2, iblock, chargeTH2);
      g_BlockBeamChargeIntegral[iR]->SetPointError(pChargeTH2, 0., chargeTH2Err);

      // ------------------------------------------------
      // WRITE EVERYTHING FROM THIS RAW BLOCK
      // ------------------------------------------------

      rawblockdir->cd();

      currentBlockRaw[iR]->Write();
      currentBlockRawFull[iR]->Write();

      if (s.amp)   s.amp->Write();
      if (s.mean)  s.mean->Write();
      if (s.sigma) s.sigma->Write();
      if (s.chi2)  s.chi2->Write();

      if (sFull.amp)   sFull.amp->Write();
      if (sFull.mean)  sFull.mean->Write();
      if (sFull.sigma) sFull.sigma->Write();
      if (sFull.chi2)  sFull.chi2->Write();

      g_BlockRawCalibFullDiff[iR][iblock]->Write();
      g_BlockRawCalibFullRatio[iR][iblock]->Write();

      // ------------------------------------------------
      // Dense/slice objects are no longer required
      // ------------------------------------------------

      delete currentBlockRaw[iR];
      currentBlockRaw[iR] = nullptr;

      delete currentBlockRawFull[iR];
      currentBlockRawFull[iR] = nullptr;

      delete s.amp;
      delete s.mean;
      delete s.sigma;
      delete s.chi2;

      delete sFull.amp;
      delete sFull.mean;
      delete sFull.sigma;
      delete sFull.chi2;

      // Diff graph is not needed in the second pass.
      // It is already safely stored in the ROOT file.
      delete g_BlockRawCalibFullDiff[iR][iblock];
      g_BlockRawCalibFullDiff[iR][iblock] = nullptr;

      // DO NOT delete g_BlockRawCalibFullRatio:
      // second pass still needs it.
    }

    outfile->Flush();
  };

  // Blocks are defined only by the input TTree index (iev).
  // evt remains available as DAQ metadata and may contain gaps.

  // counter for raw block id 
  int currentRawBlockId = -1;
  // event loop!
  for (Long64_t iev = 0; iev < nToProcess; ++iev) {

    Long64_t nb = fTree->GetEntry(iev);    
    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }
   
    // Fixed-size blocks in input-tree coordinates: [0,NevtBlock-1], [NevtBlock,2*NevtBlock-1], ...
    const int iblock = (int)(iev / (Long64_t)NevtBlock);

    if (iblock != currentRawBlockId) {
        // Previous block has just finished
        if (currentRawBlockId >= 0) FinalizeRawBlock(currentRawBlockId);
        // Start new block
        currentRawBlockId = iblock;
        CreateRawBlock(currentRawBlockId);
    }

    if (RecoEntry % 1000 == 0) {
      float progress = (float)(RecoEntry) / nToProcess;
      cout << "Processed " << RecoEntry << " out of " << nToProcess << " entries (" << fixed << setprecision(2) << progress * 100 << "%)" << endl;
    }

    if (!mmLayer || !mmReadout || !mmStrip || !raw_q) {
      cerr << "WARNING: null branch pointer at event " << iev << endl;
      continue;
    }

    // fill the overall time-event association TGraphs
    // cout << "DEBUG: is iev equal to evt and RecoEntry? iev=" << iev << ", evt=" << evt << ", RecoEntry=" << RecoEntry << endl;
    double daqTime = (double)daqTimeSec + 1.e-6 * (double)daqTimeMicroSec;
    
    BlockTimeInfo &bt = blockTimeInfo[iblock];
    if (bt.nEvents == 0) {
      bt.firstEvt = evt;
      bt.firstTreeIndex = iev;
      bt.firstSrs = (double)srsTimeStamp;
      bt.firstDaq = daqTime;
      bt.firstDaqSec = daqTimeSec;
    }
    bt.lastEvt = evt;
    bt.lastTreeIndex = iev;
    bt.lastSrs = (double)(srsTimeStamp);
    bt.lastDaq = daqTime;
    bt.lastDaqSec = daqTimeSec;
    bt.sumSrs += (double)srsTimeStamp;
    bt.sumDaq += (double)daqTime;
    bt.sumDaqSec += (double)daqTimeSec;
    bt.sumTreeIndex += (long double)iev;
    bt.nEvents++;

    int ipTime = g_DaqTime_iev->GetN();

    // g_SrsTimeStamp_iev->SetPoint(ipTime, iev, srsTimeStamp);
    // g_DaqTimeSec_iev->SetPoint(ipTime, iev, daqTimeSec);
    // g_DaqTimeMicroSec_iev->SetPoint(ipTime, iev, daqTimeMicroSec);
    g_DaqTime_iev->SetPoint(ipTime, iev, daqTime);

    ipTime = g_SrsTimeStamp_evt->GetN();
    g_SrsTimeStamp_evt->SetPoint(ipTime, evt, srsTimeStamp);
    g_DaqTimeSec_evt->SetPoint(ipTime, evt, daqTimeSec);
    g_DaqTimeMicroSec_evt->SetPoint(ipTime, evt, daqTimeMicroSec);
    g_DaqTime_evt->SetPoint(ipTime, evt, daqTime);

    g_evt_vs_iev->SetPoint(iev, iev, evt);

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
        hqmaxstrip[iReadout_new]->Fill(x_strip, q_strip);
        currentBlockRaw[iReadout_new]->Fill(x_strip, q_strip);
      }
      hqmaxstripFull[iReadout_new]->Fill(x_strip, q_strip);
      currentBlockRawFull[iReadout_new]->Fill(x_strip, q_strip);    
    }

    RecoEntry++;
  }

  if (currentRawBlockId >= 0) {
    FinalizeRawBlock(currentRawBlockId);
  }

  cout << "#### Total reconstructed events: " << RecoEntry << endl;
  cout << "################################ " << endl;
  cout << "#### Block time info: " << endl;
  for (size_t ib = 0; ib < blockTimeInfo.size(); ++ib) {

    BlockTimeInfo &bt = blockTimeInfo[ib];

    // No event existed in this entire event-ID block
    if (bt.nEvents <= 0) continue;

    bt.meanSrs = bt.sumSrs / (double)(bt.nEvents);
    bt.meanDaq = bt.sumDaq / (double)(bt.nEvents);
    bt.meanDaqSec = bt.sumDaqSec / (double)(bt.nEvents);
    bt.meanTreeIndex = (double)(bt.sumTreeIndex / (long double)(bt.nEvents));

    const Long64_t expectedFirstTreeIndex = (Long64_t)ib * (Long64_t)NevtBlock;
    const Long64_t expectedLastTreeIndex = min((Long64_t)nToProcess - 1, ((Long64_t)ib + 1) * (Long64_t)NevtBlock - 1);
    const Long64_t nUnreadEntries = (expectedLastTreeIndex - expectedFirstTreeIndex + 1) - bt.nEvents;

    const double daqSecHalfWidth = 0.5 * fabs(bt.lastDaqSec - bt.firstDaqSec);
    const double daqHalfWidth = 0.5 * fabs(bt.lastDaq - bt.firstDaq);
    const double srsHalfWidth = 0.5 * fabs(bt.lastSrs - bt.firstSrs);
    const double treeIndexHalfWidth = 0.5 * fabs((double)bt.lastTreeIndex - (double)bt.firstTreeIndex);

    // ------------------------------------------------------
    // Debug / QA
    // ------------------------------------------------------

    cout << "Block " << ib << " expected iev = [" << expectedFirstTreeIndex << ", " << expectedLastTreeIndex << "]" << " observed iev = [" << bt.firstTreeIndex << ", " << bt.lastTreeIndex << "]" << " observed evt = [" << bt.firstEvt << ", " << bt.lastEvt << "]" << " nEntries = " << bt.nEvents << " unread = " << nUnreadEntries << " meanDAQ = " << std::setprecision(16) << bt.meanDaq << endl;

    int p = g_BlockMeanDaqSec->GetN();
    g_BlockMeanDaqSec->SetPoint(p, (double)(ib), bt.meanDaqSec);
    g_BlockMeanDaqSec->SetPointError(p, 0., daqSecHalfWidth);

    p = g_BlockMeanDaqTime->GetN();
    g_BlockMeanDaqTime->SetPoint(p, (double)(ib), bt.meanDaq);
    g_BlockMeanDaqTime->SetPointError(p, 0., daqHalfWidth);

    p = g_BlockMeanSrsTime->GetN();
    g_BlockMeanSrsTime->SetPoint(p, (double)(ib), bt.meanSrs);
    g_BlockMeanSrsTime->SetPointError(p, 0., srsHalfWidth);

    p = g_BlockMeanTreeIndex->GetN();
    g_BlockMeanTreeIndex->SetPoint(p, (double)(ib), bt.meanTreeIndex);
    g_BlockMeanTreeIndex->SetPointError(p, 0., treeIndexHalfWidth);
}

  cout << "################################ " << endl;
  cout << "#### Slice fit procedure to equalise channels + baseline removal" << endl;

  // ------------------------------------------------------------
  // Build overall and per-block calibration graphs from RAW
  // ------------------------------------------------------------
  for (int iR = 0; iR < TMMCH_N_Readout; iR++) {

    SliceFitResult even = RunFitSlicesY(hqmaxstrip[iR],     Form("%s_even", tmm_tag[iR].Data()));
    SliceFitResult full = RunFitSlicesY(hqmaxstripFull[iR], Form("%s_full", tmm_tag[iR].Data()));

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

  for (int iR = 0; iR < TMMCH_N_Readout; ++iR) {

    const int nPoints = g_BlockBeamSpot[iR]->GetN();

    for (int ip = 0; ip < nPoints; ++ip) {

      // x coordinate of the block graph contains the REAL block number.
      double xBlock = 0.;
      double beamSpot = 0.;

      g_BlockBeamSpot[iR]->GetPoint(ip, xBlock, beamSpot);

      const int iblock = (int)std::lround(xBlock);

      if (iblock < 0 || iblock >= (int)blockTimeInfo.size()) {
        cerr << "WARNING: invalid time association for block " << iblock << endl;
        continue;
      }

      const BlockTimeInfo &bt = blockTimeInfo[iblock];

      if (bt.nEvents <= 0) continue;

      const double tDaq = bt.meanDaq;
      const double eTDaq = 0.5 * fabs(bt.lastDaq - bt.firstDaq);

      const double eSpot = g_BlockBeamSpot[iR]->GetErrorY(ip);

      const double beamSpread = g_BlockBeamSpread[iR]->GetPointY(ip);
      const double eSpread = g_BlockBeamSpread[iR]->GetErrorY(ip);

      const double beamCharge = g_BlockBeamChargeFit[iR]->GetPointY(ip);
      const double eCharge = g_BlockBeamChargeFit[iR]->GetErrorY(ip);

      // --------------------------
      // DAQ time
      // --------------------------

      int p = g_DaqTimeBeamSpot[iR]->GetN();

      g_DaqTimeBeamSpot[iR]->SetPoint(p, tDaq, beamSpot);
      g_DaqTimeBeamSpot[iR]->SetPointError(p, eTDaq, eSpot);

      p = g_DaqTimeBeamSpread[iR]->GetN();

      g_DaqTimeBeamSpread[iR]->SetPoint(p, tDaq, beamSpread);
      g_DaqTimeBeamSpread[iR]->SetPointError(p, eTDaq, eSpread);

      p = g_DaqTimeBeamChargeFit[iR]->GetN();

      g_DaqTimeBeamChargeFit[iR]->SetPoint(p, tDaq, beamCharge);
      g_DaqTimeBeamChargeFit[iR]->SetPointError(p, eTDaq, eCharge);

      const double tTree = bt.meanTreeIndex;
      const double eTTree = 0.5 * fabs((double)bt.lastTreeIndex - (double)bt.firstTreeIndex);
      p = g_TreeIndexBeamChargeFit[iR]->GetN();
      g_TreeIndexBeamChargeFit[iR]->SetPoint(p, tTree, beamCharge);
      g_TreeIndexBeamChargeFit[iR]->SetPointError(p, eTTree, eCharge);

      double blockIdTH2 = 0.;
      double beamChargeTH2 = 0.;
      for (int ipTH2 = 0; ipTH2 < g_BlockBeamChargeIntegral[iR]->GetN(); ++ipTH2) {
        g_BlockBeamChargeIntegral[iR]->GetPoint(ipTH2, blockIdTH2, beamChargeTH2);
        if ((int)llround(blockIdTH2) != iblock) continue;
        const double eChargeTH2 = g_BlockBeamChargeIntegral[iR]->GetErrorY(ipTH2);
        const int pTH2 = g_TreeIndexBeamChargeIntegral[iR]->GetN();
        const int pDaqTH2 = g_DaqTimeBeamChargeIntegral[iR]->GetN();
        g_TreeIndexBeamChargeIntegral[iR]->SetPoint(pTH2, tTree, beamChargeTH2);
        g_TreeIndexBeamChargeIntegral[iR]->SetPointError(pTH2, eTTree, eChargeTH2);
        g_DaqTimeBeamChargeIntegral[iR]->SetPoint(pDaqTH2, tDaq, beamChargeTH2);
        g_DaqTimeBeamChargeIntegral[iR]->SetPointError(pDaqTH2, eTDaq, eChargeTH2);
        break;
      }

    }
  }

  for (int iblock = 0; iblock < (int)blockTimeInfo.size(); ++iblock) {
    const BlockTimeInfo &bt = blockTimeInfo[iblock];
    if (bt.nEvents <= 0) continue;
    const double tDaq = bt.meanDaq;
    const double eTDaq = 0.5 * fabs(bt.lastDaq - bt.firstDaq);
    const double tTree = bt.meanTreeIndex;
    const double eTTree = 0.5 * fabs((double)bt.lastTreeIndex - (double)bt.firstTreeIndex);
  
    double qFitX = 0.;
    double qFitY = 0.;
    double eqFitX = 0.;
    double eqFitY = 0.;
    const bool hasFitX = GetBlockGraphPoint(g_BlockBeamChargeFit[0], iblock, qFitX, eqFitX);
    const bool hasFitY = GetBlockGraphPoint(g_BlockBeamChargeFit[1], iblock, qFitY, eqFitY);

    if (hasFitX && hasFitY) {
      const double qFitTot = 0.5 * (qFitX + qFitY);
      const double eFitTot = 0.5 * sqrt(eqFitX * eqFitX + eqFitY * eqFitY);

      int p = g_DaqTimeBeamChargeFitTotal->GetN();
      g_DaqTimeBeamChargeFitTotal->SetPoint(p, tDaq, qFitTot);
      g_DaqTimeBeamChargeFitTotal->SetPointError(p, eTDaq, eFitTot);

      p = g_TreeIndexBeamChargeFitTotal->GetN();
      g_TreeIndexBeamChargeFitTotal->SetPoint(p, tTree, qFitTot);
      g_TreeIndexBeamChargeFitTotal->SetPointError(p, eTTree, eFitTot);
    }

    double qIntX = 0.;
    double qIntY = 0.;
    double eqIntX = 0.;
    double eqIntY = 0.;
    const bool hasIntX = GetBlockGraphPoint(g_BlockBeamChargeIntegral[0], iblock, qIntX, eqIntX);
    const bool hasIntY = GetBlockGraphPoint(g_BlockBeamChargeIntegral[1], iblock, qIntY, eqIntY);

    if(hasIntX && hasIntY) {
      const double qIntTot = 0.5 * (qIntX + qIntY);
      const double eIntTot = 0.5 * sqrt(eqIntX * eqIntX + eqIntY * eqIntY);

      int p = g_DaqTimeBeamChargeIntegralTotal->GetN();
      g_DaqTimeBeamChargeIntegralTotal->SetPoint(p, tDaq, qIntTot);
      g_DaqTimeBeamChargeIntegralTotal->SetPointError(p, eTDaq, eIntTot);

      p = g_TreeIndexBeamChargeIntegralTotal->GetN();
      g_TreeIndexBeamChargeIntegralTotal->SetPoint(p, tTree, qIntTot);
      g_TreeIndexBeamChargeIntegralTotal->SetPointError(p, eTTree, eIntTot);
    }
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
  // Helper: is this block index valid across all calibrated containers?
  // ------------------------------------------------------------
  auto IsValidBlockForCalibration = [&](int iR, int iblock) -> bool {
    if (iR < 0 || iR >= TMMCH_N_Readout) return false;
    if (iblock < 0) return false;
    if (iblock >= (int)g_BlockRawCalibFullRatio[iR].size()) return false;
    if (!g_BlockRawCalibFullRatio[iR][iblock]) return false;

    return true;
  };

  // ------------------------------------------------------------
  // Second pass: fill calibrated histograms (modes 2, 3 overall; 2, 3, 4 block)
  // ------------------------------------------------------------

  // same logic with histo di appoggio per i calibrated plots
  TH2F *currentExtCal[TMMCH_N_Readout] = {nullptr};
  TH2F *currentExtCalFull[TMMCH_N_Readout] = {nullptr};

  TH2F *currentRunCal[TMMCH_N_Readout] = {nullptr};
  TH2F *currentRunCalFull[TMMCH_N_Readout] = {nullptr};

  TH2F *currentBlockCal[TMMCH_N_Readout] = {nullptr};
  TH2F *currentBlockCalFull[TMMCH_N_Readout] = {nullptr};

  auto CreateCalibratedBlock = [&](int iblock){
    for (int l = 0; l < TMMCH_N_Readout; ++l) {
      currentExtCal[l] = new TH2F(
          Form("hBlockqmaxstrip_extcal%s_block%04d",tmm_tag[l].Data(),iblock),
          TString("q_{max-extcal} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentExtCal[l]->SetDirectory(nullptr);

      currentExtCalFull[l] = new TH2F(
          Form("hBlockqmaxstripFull_extcal%s_block%04d",tmm_tag[l].Data(),iblock),
          TString("q_{max-extcal} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentExtCalFull[l]->SetDirectory(nullptr);

      currentRunCal[l] = new TH2F(
          Form("hBlockqmaxstrip_runcal%s_block%04d", tmm_tag[l].Data(), iblock),
          TString("q_{max-runcal} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentRunCal[l]->SetDirectory(nullptr);

      currentRunCalFull[l] = new TH2F(
          Form("hBlockqmaxstripFull_runcal%s_block%04d", tmm_tag[l].Data(), iblock),
          TString("q_{max-runcal} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentRunCalFull[l]->SetDirectory(nullptr);

      currentBlockCal[l] = new TH2F(
          Form("hBlockqmaxstrip_blockcal%s_block%04d", tmm_tag[l].Data(), iblock),
          TString("q_{max-blockcal} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentBlockCal[l]->SetDirectory(nullptr);

      currentBlockCalFull[l] = new TH2F(
          Form("hBlockqmaxstripFull_blockcal%s_block%04d", tmm_tag[l].Data(), iblock),
          TString("q_{max-blockcal} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", iblock),
          MAXSTRIP, -xFullmm / 2., +xFullmm / 2., 2500, 0., 2500.
      );
      currentBlockCalFull[l]->SetDirectory(nullptr);
    }
  };

  auto FinalizeCalibratedBlock = [&](int iblock) {
    TDirectory *extcalblockdir = GetBlockSubdir(iblock, "Calib_external");
    TDirectory *runcalblockdir = GetBlockSubdir(iblock, "Calib_run" );
    TDirectory *blockcalblockdir = GetBlockSubdir(iblock, "Calib_block");

    for (int l = 0; l < TMMCH_N_Readout; ++l) {
      extcalblockdir->cd();
      currentExtCal[l]->Write();
      currentExtCalFull[l]->Write();

      runcalblockdir->cd();
      currentRunCal[l]->Write();
      currentRunCalFull[l]->Write();

      blockcalblockdir->cd();
      currentBlockCal[l]->Write();
      currentBlockCalFull[l]->Write();

      delete currentExtCal[l];
      delete currentExtCalFull[l];
      delete currentRunCal[l];
      delete currentRunCalFull[l];
      delete currentBlockCal[l];
      delete currentBlockCalFull[l];

      currentExtCal[l] = nullptr;
      currentExtCalFull[l] = nullptr;
      currentRunCal[l] = nullptr;
      currentRunCalFull[l] = nullptr;
      currentBlockCal[l] = nullptr;
      currentBlockCalFull[l] = nullptr;
    }
    outfile->Flush();
  };
  
  int currentCalBlockId = -1;
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

    const int ic = (int)(iev / (Long64_t)NevtBlock);

    if (ic != currentCalBlockId) {
      if (currentCalBlockId >= 0) FinalizeCalibratedBlock(currentCalBlockId);
      currentCalBlockId = ic;
      CreateCalibratedBlock(currentCalBlockId);
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
      int iReadout_new = iReadout - 88;

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) continue;

      CoordinateFinder(channel, raw_q->at(j), iReadout_new, x_strip, q_strip, iStripLocal);

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) continue;

      if (channel > MAXSTRIP) {
        cout << "ERROR channel bigger than RawCalibFullRatio vector - channel = " << channel << endl;
      }

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
      
      // ---- Mode 4 (block only): per-block calibration ----
      double blockcorr = 1.;
      bool blockValid = IsValidBlockForCalibration(iReadout_new, ic);
      if (blockValid) {
        blockcorr = SafeGraphEval(
          g_BlockRawCalibFullRatio[iReadout_new][ic],
          x_strip,
          xCalMin[iReadout_new],
          xCalMax[iReadout_new]);
      }

      // ---- Overall fills (3 modes; mode 1 was filled in the first pass) ----
      if (channel % 2 == 0) {
        hqmaxstrip_extcal[iReadout_new]->Fill(x_strip, q_strip * extcorr);
        hqmaxstrip_runcal[iReadout_new]->Fill(x_strip, q_strip * runcorr);
      }
      hqmaxstripFull_extcal[iReadout_new]->Fill(x_strip, q_strip * extcorr);
      hqmaxstripFull_runcal[iReadout_new]->Fill(x_strip, q_strip * runcorr);

      // ---- Block fills (4 modes; mode 1 was filled in the first pass) ----
      if (blockValid) {
        if (channel % 2 == 0) {
          currentExtCal[iReadout_new]->Fill(x_strip, q_strip * extcorr);
          currentRunCal[iReadout_new]->Fill(x_strip, q_strip * runcorr);
          currentBlockCal[iReadout_new]->Fill(x_strip, q_strip * blockcorr);
        }
        currentExtCalFull[iReadout_new]->Fill(x_strip, q_strip * extcorr);
        currentRunCalFull[iReadout_new]->Fill(x_strip, q_strip * runcorr);
        currentBlockCalFull[iReadout_new]->Fill(x_strip, q_strip * blockcorr);
      } else {
        cerr << "WARNING: invalid block calibration index"
            << " event = " << iev << " ic = " << ic
            << " readout = " << iReadout_new << " tag = " << tmm_tag[iReadout_new]
            << " hBlockqmaxstrip_blockcal size = " << hBlockqmaxstrip_blockcal[iReadout_new].size()
            << " g_BlockRawCalibFullRatio size = " << g_BlockRawCalibFullRatio[iReadout_new].size()
            << endl;
      }
    }
  }

  if (currentCalBlockId >= 0) {
    FinalizeCalibratedBlock(currentCalBlockId);
  }

  // --- Overall RAW + slice histograms + calibration graphs ---
  outfile->cd();
  overalldir->cd();
  // if(g_SrsTimeStamp_iev) g_SrsTimeStamp_iev->Write();
  // if(g_DaqTimeSec_iev) g_DaqTimeSec_iev->Write();
  // if(g_DaqTimeMicroSec_iev) g_DaqTimeMicroSec_iev->Write();
  if(g_DaqTime_iev) g_DaqTime_iev->Write();
  if(g_SrsTimeStamp_evt) g_SrsTimeStamp_evt->Write();
  if(g_DaqTimeSec_evt) g_DaqTimeSec_evt->Write();
  if(g_DaqTimeMicroSec_evt) g_DaqTimeMicroSec_evt->Write();
  if(g_DaqTime_evt) g_DaqTime_evt->Write();
  if(g_evt_vs_iev) g_evt_vs_iev->Write();

  if (g_DaqTimeBeamChargeFitTotal) g_DaqTimeBeamChargeFitTotal->Write();
  if (g_DaqTimeBeamChargeIntegralTotal) g_DaqTimeBeamChargeIntegralTotal->Write();
  if (g_TreeIndexBeamChargeFitTotal) g_TreeIndexBeamChargeFitTotal->Write();
  if (g_TreeIndexBeamChargeIntegralTotal) g_TreeIndexBeamChargeIntegralTotal->Write();

  if (g_BlockMeanDaqSec) g_BlockMeanDaqSec->Write();
  if (g_BlockMeanDaqTime) g_BlockMeanDaqTime->Write();
  if (g_BlockMeanSrsTime) g_BlockMeanSrsTime->Write();
  if (g_BlockMeanTreeIndex) g_BlockMeanTreeIndex->Write();

  for (int l = 0; l < TMMCH_N_Readout; l++) {
    overalldir->cd();
    if (g_BlockBeamSpot[l])   g_BlockBeamSpot[l]->Write();
    if (g_BlockBeamSpread[l]) g_BlockBeamSpread[l]->Write();
    if (g_BlockBeamChargeFit[l]) g_BlockBeamChargeFit[l]->Write();
    if (g_BlockBeamChargeIntegral[l]) g_BlockBeamChargeIntegral[l]->Write();
    if (g_DaqTimeBeamSpot[l])   g_DaqTimeBeamSpot[l]->Write();
    if (g_DaqTimeBeamSpread[l]) g_DaqTimeBeamSpread[l]->Write();
    if (g_DaqTimeBeamChargeFit[l]) g_DaqTimeBeamChargeFit[l]->Write();
    if (g_DaqTimeBeamChargeIntegral[l]) g_DaqTimeBeamChargeIntegral[l]->Write();
    if (g_TreeIndexBeamChargeFit[l]) g_TreeIndexBeamChargeFit[l]->Write();
    if (g_TreeIndexBeamChargeIntegral[l]) g_TreeIndexBeamChargeIntegral[l]->Write();


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
    hqmaxstrip_extcal[l]->Write();
    hqmaxstripFull_extcal[l]->Write();
  }

  // --- Overall RUN-CAL (mode 3) ---
  outfile->cd();
  overalldir->cd();
  runcaloveralldir->cd();
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip_runcal[l]->Write();
    hqmaxstripFull_runcal[l]->Write();
  }

  outfile->Write();
  outfile->Close();
  delete outfile;
  outfile = nullptr;

  cout << "Output written to " << outputFileName << endl;
}
