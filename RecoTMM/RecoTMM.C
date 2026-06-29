#define RecoTMM_cxx
#include "RecoTMM.h"

using namespace std;

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

  ifstream infile(filename.c_str());

  if (!infile.is_open()) {
    cerr << "Error: Cannot open calibration file " << filename << endl;
    return false;
  }

  // Default: unity calibration
  for (int is = 0; is < MAXSTRIP; ++is) {
    XcalibConst[is] = 1.;
    XcalibErr[is]   = 0.;

    YcalibConst[is] = 1.;
    YcalibErr[is]   = 0.;
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

  H->FitSlicesY();
  TH1D *h0 = (TH1D*)gDirectory->Get(Form("%s_0", H->GetName()));
  TH1D *h1 = (TH1D*)gDirectory->Get(Form("%s_1", H->GetName()));
  TH1D *h2 = (TH1D*)gDirectory->Get(Form("%s_2", H->GetName()));
  TH1D *h3 = (TH1D*)gDirectory->Get(Form("%s_chi2", H->GetName()));

  if (!h0 || !h1 || !h2 || !h3) {
    cerr << "ERROR: FitSlicesY failed for " << H->GetName() << endl;
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

  TFitResultPtr prefitResult = h->Fit(prefit, "RQS0");

  double amp0 = prefit->GetParameter(0);
  double mu0  = prefit->GetParameter(1);
  double s0   = fabs(prefit->GetParameter(2));

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

  fitResult = h->Fit(fit, "RQS");

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

void RecoTMM::FillBlockGraphsFromSlices(int iR){

  for (size_t ib = 0; ib < hBlockqmaxstrip[iR].size(); ib++) {

    SliceFitResult s = RunFitSlicesY(hBlockqmaxstrip[iR][ib], Form("%s_block%zu", tmm_tag[iR].Data(), ib) );

    hBlockAmpslice[iR][ib]   = s.amp;
    hBlockMeanslice[iR][ib]  = s.mean;
    hBlockSigmaslice[iR][ib] = s.sigma;
    hBlockChi2slice[iR][ib]  = s.chi2;

    SliceFitResult sFull = RunFitSlicesY(hBlockqmaxstripFull[iR][ib], Form("%s_block%zu_full", tmm_tag[iR].Data(), ib));

    hBlockAmpsliceFull[iR][ib]   = sFull.amp;
    hBlockMeansliceFull[iR][ib]  = sFull.mean;
    hBlockSigmasliceFull[iR][ib] = sFull.sigma;
    hBlockChi2sliceFull[iR][ib]  = sFull.chi2;

    if (!s.mean) continue;
    
    TFitResultPtr fitResult;
    double xini = StripMin * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION);
    double xfin = StripMax * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION);

    TF1 *fb = FitDoubleGaussian(s.mean, Form("%s_block%zu", tmm_tag[iR].Data(), ib), xini, xfin, fitResult);
    
    if (!fb || !fitResult.Get()) {
      cerr << "WARNING: block fit failed for readout = " << tmm_tag[iR] << " block = " << ib << endl;
      continue;
    }

    double A1 = fb->GetParameter(0);
    double mu = fb->GetParameter(1);
    double s1 = fabs(fb->GetParameter(2));
    double A2 = fb->GetParameter(3);
    double s2 = fabs(fb->GetParameter(4));

    double e_mu = fb->GetParError(1);

    double norm = A1 + A2;
    if (norm <= 0) continue;

    double sigma_eff = sqrt((A1*s1*s1 + A2*s2*s2) / norm);
    double charge_proxy = A1 * s1 * sqrt(2. * TMath::Pi()) + A2 * s2 * sqrt(2. * TMath::Pi());
    double xblock = ib;

    int p0 = g_BlockBeamSpot[iR]->GetN();
    g_BlockBeamSpot[iR]->SetPoint(p0, xblock, mu);
    g_BlockBeamSpot[iR]->SetPointError(p0, 0., e_mu);

    int p1 = g_BlockBeamSpread[iR]->GetN();
    g_BlockBeamSpread[iR]->SetPoint(p1, xblock, sigma_eff);
    g_BlockBeamSpread[iR]->SetPointError(p1, 0., 0);

    int p2 = g_BlockBeamCharge[iR]->GetN();
    g_BlockBeamCharge[iR]->SetPoint(p2, xblock, charge_proxy);
    g_BlockBeamCharge[iR]->SetPointError(p2, 0., sqrt(charge_proxy));

    BuildFitRatio(
      hBlockMeansliceFull[iR][ib],
      hBlockSigmasliceFull[iR][ib],
      fb,
      g_BlockRawCalibFullRatio[iR][ib],
      g_BlockRawCalibFullDiff[iR][ib]
    );
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

  cout << "Block size: " << NevtBlock << " events" << endl;

  // ------------------------------------------------------------
  // Histograms and graphs: created once, before looping on events
  // ------------------------------------------------------------
  for (int l = 0; l < TMMCH_N_Readout; l++) {

    // --- Overall RAW (mode 1) ---
    hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500);
    // hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstrip[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip[l]->SetYTitle("q_{max} [ADC counts]");

    hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500);
    // hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstripFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull[l]->SetYTitle("q_{max} [ADC counts]");

    // --- FitSlicesY() result holders ---
    hAmpslice[l] = new TH1D(Form("hAmpslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2);
    // hAmpslice[l] = new TH1D(Form("hAmpslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hAmpslice[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hAmpslice[l]->SetYTitle("AMP (q_{max} [ADC counts])");

    hMeanslice[l] = new TH1D(Form("hMeanslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2);
    // hMeanslice[l] = new TH1D(Form("hMeanslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hMeanslice[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hMeanslice[l]->SetYTitle("q_{max} [ADC counts]");

    hSigmaslice[l] = new TH1D(Form("hSigmaslice%s", tmm_tag[l].Data()), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2);
    // hSigmaslice[l] = new TH1D(Form("hSigmaslice%s", tmm_tag[l].Data()), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hSigmaslice[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hSigmaslice[l]->SetYTitle("#sigma_{q_{max}} [ADC counts]");
    
    hAmpsliceFull[l] = new TH1D(Form("hAmpsliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2);
    // hAmpsliceFull[l] = new TH1D(Form("hAmpsliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hAmpsliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hAmpsliceFull[l]->SetYTitle("AMP (q_{max} [ADC counts])");

    hMeansliceFull[l] = new TH1D(Form("hMeansliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2);
    // hMeansliceFull[l] = new TH1D(Form("hMeansliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hMeansliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hMeansliceFull[l]->SetYTitle("q_{max} [ADC counts]");

    hSigmasliceFull[l] = new TH1D(Form("hSigmasliceFull%s", tmm_tag[l].Data()), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2);
    // hSigmasliceFull[l] = new TH1D(Form("hSigmasliceFull%s", tmm_tag[l].Data()), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hSigmasliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hSigmasliceFull[l]->SetYTitle("#sigma_{q_{max}} [ADC counts]");

    // --- Overall EXT-CAL (mode 2): external txt ---
    hqmaxstrip_extcal[l] = new TH2F(Form("hqmaxstrip_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500);
    // hqmaxstrip_extcal[l] = new TH2F(Form("hqmaxstrip_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstrip_extcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip_extcal[l]->SetYTitle("q_{max} calib [ADC counts]");

    hqmaxstripFull_extcal[l] = new TH2F(Form("hqmaxstripFull_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500);
    // hqmaxstripFull_extcal[l] = new TH2F(Form("hqmaxstripFull_extcal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstripFull_extcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_extcal[l]->SetYTitle("q_{max} calib [ADC counts]");

    // --- Overall RUN-CAL (mode 3): from g_RawCalibFullRatio ---
    hqmaxstrip_runcal[l] = new TH2F(Form("hqmaxstrip_runcal%s", tmm_tag[l].Data()), TString("q_{max-runcal} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500);
    hqmaxstrip_runcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip_runcal[l]->SetYTitle("q_{max} runcal [ADC counts]");

    hqmaxstripFull_runcal[l] = new TH2F(Form("hqmaxstripFull_runcal%s", tmm_tag[l].Data()), TString("q_{max-runcal} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500);
    hqmaxstripFull_runcal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_runcal[l]->SetYTitle("q_{max} runcal [ADC counts]");

    // --- block-level monitoring TGraphErrors ---
    g_BlockBeamSpot[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpot[l], Form("g_BlockBeamSpot_%s", tmm_tag[l].Data()), "entry", Form("%s [mm]", tmm_tag[l].Data()), 20, kBlue + l);

    g_BlockBeamSpread[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpread[l], Form("g_BlockBeamSpread_%s", tmm_tag[l].Data()), "entry", Form("#sigma_{%s} [mm]", tmm_tag[l].Data()), 21, kRed + l);

    g_BlockBeamCharge[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamCharge[l], Form("g_BlockBeamCharge_%s", tmm_tag[l].Data()), "entry", "q_{Beam} [ADC counts]", 22, kGreen + 2 + l);
  
    // --- overall run-based calibration graphs ---
    g_RawCalibFullDiff[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullDiff[l], Form("g_RawCalibFullDiff%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} - Q_{raw} [ADC counts]", 22, kBlack);

    g_RawCalibFullRatio[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullRatio[l], Form("g_RawCalibFullRatio%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} / Q_{raw}", 22, kBlack);
  }

  // ------------------------------------------------------------
  // Block/global accumulators
  // ------------------------------------------------------------

  Long64_t globalEntry = 0;
  int blockCounter = 0;

  auto EnsureBlockHistograms = [&](int iblock) {
    for (int l = 0; l < TMMCH_N_Readout; l++) {
      while ((int)hBlockqmaxstrip[l].size() <= iblock) {
        int b = hBlockqmaxstrip[l].size();

        // --- Block RAW (mode 1) ---
        hBlockqmaxstrip[l].push_back(new TH2F(Form("hBlockqmaxstrip%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        // hBlockqmaxstrip[l].push_back(new TH2F(Form("hBlockqmaxstrip%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstrip[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstrip[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull[l].push_back(new TH2F(Form("hBlockqmaxstripFull%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        // hBlockqmaxstripFull[l].push_back(new TH2F(Form("hBlockqmaxstripFull%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstripFull[l].back()->SetYTitle("q_{max} [ADC counts]");

        // --- Block EXT-CAL (mode 2): external txt ---
        hBlockqmaxstrip_extcal[l].push_back(new TH2F(Form("hBlockqmaxstrip_extcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max-extcal} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        hBlockqmaxstrip_extcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstrip_extcal[l].back()->SetYTitle("q_{max} extcal [ADC counts]");

        hBlockqmaxstripFull_extcal[l].push_back(new TH2F(Form("hBlockqmaxstripFull_extcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max-extcal} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        hBlockqmaxstripFull_extcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstripFull_extcal[l].back()->SetYTitle("q_{max} extcal [ADC counts]");

        // --- Block RUN-CAL (mode 3): overall run-based applied per block ---
        hBlockqmaxstrip_runcal[l].push_back(new TH2F(Form("hBlockqmaxstrip_runcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max-runcal} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        hBlockqmaxstrip_runcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstrip_runcal[l].back()->SetYTitle("q_{max} runcal [ADC counts]");

        hBlockqmaxstripFull_runcal[l].push_back(new TH2F(Form("hBlockqmaxstripFull_runcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max-runcal} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        hBlockqmaxstripFull_runcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstripFull_runcal[l].back()->SetYTitle("q_{max} runcal [ADC counts]");

        // --- Block BLOCK-CAL (mode 4): per-block calibration ---
        hBlockqmaxstrip_blockcal[l].push_back(new TH2F(Form("hBlockqmaxstrip_blockcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max-blockcal} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        hBlockqmaxstrip_blockcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstrip_blockcal[l].back()->SetYTitle("q_{max} blockcal [ADC counts]");

        hBlockqmaxstripFull_blockcal[l].push_back(new TH2F(Form("hBlockqmaxstripFull_blockcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max-blockcal} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xFullmm/2, +xFullmm/2, 1000, 0, 2500));
        hBlockqmaxstripFull_blockcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstripFull_blockcal[l].back()->SetYTitle("q_{max} blockcal [ADC counts]");

        // --- Per-block FitSlicesY() histograms ---
        hBlockAmpslice[l].push_back(new TH1D(Form("hBlockAmpslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockAmpslice[l].push_back(new TH1D(Form("hBlockAmpslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockAmpslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockAmpslice[l].back()->SetYTitle("AMP (q_{max} [ADC counts])");

        hBlockMeanslice[l].push_back(new TH1D(Form("hBlockMeanslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockMeanslice[l].push_back(new TH1D(Form("hBlockMeanslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockMeanslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockMeanslice[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockSigmaslice[l].push_back(new TH1D(Form("hBlockSigmaslice%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockSigmaslice[l].push_back(new TH1D(Form("hBlockSigmaslice%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockSigmaslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockSigmaslice[l].back()->SetYTitle("#sigma_{q_{max}} [ADC counts]");

        hBlockChi2slice[l].push_back(new TH1D(Form("hBlockChi2slice%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockChi2slice[l].push_back(new TH1D(Form("hBlockChi2slice%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockChi2slice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockChi2slice[l].back()->SetYTitle("#Chi^{2}_{q_{max}} [ADC counts]");
        
        hBlockAmpsliceFull[l].push_back(new TH1D(Form("hBlockAmpsliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockAmpsliceFull[l].push_back(new TH1D(Form("hBlockAmpsliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockAmpsliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockAmpsliceFull[l].back()->SetYTitle("AMP (q_{max} [ADC counts])");

        hBlockMeansliceFull[l].push_back(new TH1D(Form("hBlockMeansliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockMeansliceFull[l].push_back(new TH1D(Form("hBlockMeansliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockMeansliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockMeansliceFull[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockSigmasliceFull[l].push_back(new TH1D(Form("hBlockSigmasliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockSigmasliceFull[l].push_back(new TH1D(Form("hBlockSigmasliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockSigmasliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockSigmasliceFull[l].back()->SetYTitle("#sigma_{q_{max}} [ADC counts]");

        hBlockChi2sliceFull[l].push_back(new TH1D(Form("hBlockChi2sliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xFullmm/2, +xFullmm/2));
        // hBlockChi2sliceFull[l].push_back(new TH1D(Form("hBlockChi2sliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockChi2sliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockChi2sliceFull[l].back()->SetYTitle("#Chi^{2}_{q_{max}} [ADC counts]");

        // per-block calibration graphs
        g_BlockRawCalibFullDiff[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockRawCalibFullDiff[l].back(), Form("g_BlockRawCalibFullDiff%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} - Q_{raw} [ADC counts]", 22, kBlack);

        g_BlockRawCalibFullRatio[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockRawCalibFullRatio[l].back(), Form("g_BlockRawCalibFullRatio%s", tmm_tag[l].Data()), Form("%s [mm]", tmm_tag[l].Data()), "Q_{calib} / Q_{raw}", 22, kBlack);
      }
    }
  };

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

  for (Long64_t iev = 0; iev < nToProcess; ++iev) {

    Long64_t nb = fTree->GetEntry(iev);    
    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }
    EnsureBlockHistograms(blockCounter);

    if (globalEntry % 1000 == 0) {
      float progress = static_cast<float>(globalEntry) / nToProcess;
      cout << "Processed " << globalEntry << " out of " << nToProcess << " entries (" << fixed << setprecision(2) << progress * 100 << "%)" << endl;
    }

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
        hqmaxstrip[iReadout_new]->Fill(x_strip, q_strip);
        hBlockqmaxstrip[iReadout_new][blockCounter]->Fill(x_strip, q_strip);
      }
      hqmaxstripFull[iReadout_new]->Fill(x_strip, q_strip);
      hBlockqmaxstripFull[iReadout_new][blockCounter]->Fill(x_strip, q_strip);
    }

    globalEntry++;

    if (globalEntry % NevtBlock == 0) {
        blockCounter++;
    }
  }

  cout << "#### Total reconstructed events: " << globalEntry << endl;
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
    TF1 *fit = FitDoubleGaussian(
      hMeanslice[iR], tmm_tag[iR],
      StripMin * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION),
      StripMax * pitch + ((iR==0) ? GLOBAL_X_TRANSLATION : GLOBAL_Y_TRANSLATION),
      fitResultOverall
    );

    if (!fit || !fitResultOverall.Get()) {
      cerr << "WARNING: overall fit failed for readout=" << tmm_tag[iR] << endl;
      f.push_back(nullptr);
      continue;
    }

    f.push_back(fit);

    bool approved = IsFitAccepted(fitResultOverall, 1, 2);
    if(approved){
      BuildFitRatio(hMeansliceFull[iR], hSigmasliceFull[iR], fit, g_RawCalibFullRatio[iR], g_RawCalibFullDiff[iR]);  
    } else {
      cerr << "WARNING: overall calibration fit rejected for readout = " << tmm_tag[iR] << " fitStatus = " << (int)fitResultOverall << " covStatus = " << fitResultOverall->CovMatrixStatus() << endl;
    }
  }

  for (int iR = 0; iR < TMMCH_N_Readout; iR++) {
    FillBlockGraphsFromSlices(iR);
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

    if (iblock >= (int)hBlockqmaxstrip_blockcal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstripFull_blockcal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstrip_extcal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstripFull_extcal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstrip_runcal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstripFull_runcal[iR].size()) return false;

    if (!hBlockqmaxstrip_blockcal[iR][iblock]) return false;
    if (!hBlockqmaxstripFull_blockcal[iR][iblock]) return false;
    if (!hBlockqmaxstrip_extcal[iR][iblock]) return false;
    if (!hBlockqmaxstripFull_extcal[iR][iblock]) return false;
    if (!hBlockqmaxstrip_runcal[iR][iblock]) return false;
    if (!hBlockqmaxstripFull_runcal[iR][iblock]) return false;

    if (iblock >= (int)g_BlockRawCalibFullRatio[iR].size()) return false;
    if (!g_BlockRawCalibFullRatio[iR][iblock]) return false;

    return true;
  };

  // ------------------------------------------------------------
  // Second pass: fill calibrated histograms (modes 2, 3 overall; 2, 3, 4 block)
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
        xCalMax[iReadout_new]);

      int ic = iev / NevtBlock;

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
          hBlockqmaxstrip_extcal  [iReadout_new][ic]->Fill(x_strip, q_strip * extcorr);
          hBlockqmaxstrip_runcal  [iReadout_new][ic]->Fill(x_strip, q_strip * runcorr);
          hBlockqmaxstrip_blockcal[iReadout_new][ic]->Fill(x_strip, q_strip * blockcorr);
        }
        hBlockqmaxstripFull_extcal  [iReadout_new][ic]->Fill(x_strip, q_strip * extcorr);
        hBlockqmaxstripFull_runcal  [iReadout_new][ic]->Fill(x_strip, q_strip * runcorr);
        hBlockqmaxstripFull_blockcal[iReadout_new][ic]->Fill(x_strip, q_strip * blockcorr);
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

  // ------------------------------------------------------------
  // Write output ROOT file
  // ------------------------------------------------------------
  cout << "#### Creating output file " << outputFileName << endl;
  TFile *outfile = new TFile(outputFileName.Data(), "recreate");
  if (!outfile || outfile->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outputFileName << endl;
    if (outfile) delete outfile;
    return;
  }

  // Overall: 3 modes
  TDirectory *overalldir          = outfile->mkdir("Overall");
  TDirectory *rawoveralldir       = overalldir->mkdir("Raw");
  TDirectory *extcaloveralldir    = overalldir->mkdir("Calib_external");
  TDirectory *runcaloveralldir    = overalldir->mkdir("Calib_run");

  // Blocks
  TDirectory *blockdir = outfile->mkdir("Blocks");

  // --- Overall RAW + slice histograms + calibration graphs ---
  outfile->cd();
  overalldir->cd();
  rawoveralldir->cd();
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip[l]->Write();
    hqmaxstripFull[l]->Write();
    if (hAmpslice[l])       hAmpslice[l]->Write();
    if (hMeanslice[l])      hMeanslice[l]->Write();
    if (hSigmaslice[l])     hSigmaslice[l]->Write();
    if (hChi2slice[l])      hChi2slice[l]->Write();
    if (hAmpsliceFull[l])   hAmpsliceFull[l]->Write();
    if (hMeansliceFull[l])  hMeansliceFull[l]->Write();
    if (hSigmasliceFull[l]) hSigmasliceFull[l]->Write();
    if (hChi2sliceFull[l])  hChi2sliceFull[l]->Write();
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

  // --- Block summary graphs ---
  outfile->cd();
  blockdir->cd();
  for (int l = 0; l < TMMCH_N_Readout; l++) {  
    if (g_BlockBeamSpot[l])   g_BlockBeamSpot[l]->Write();
    if (g_BlockBeamSpread[l]) g_BlockBeamSpread[l]->Write();
    if (g_BlockBeamCharge[l]) g_BlockBeamCharge[l]->Write();
  }

  // --- Per-block subdirectories, 4 modes each ---
  for (int b = 0; b < (int)g_BlockBeamSpot[0]->GetN(); b++) {
    TDirectory *thisblockdir       = blockdir->mkdir(Form("block_%04d", b));
    TDirectory *rawblockdir        = thisblockdir->mkdir("Raw");
    TDirectory *extcalblockdir     = thisblockdir->mkdir("Calib_external");
    TDirectory *runcalblockdir     = thisblockdir->mkdir("Calib_run");
    TDirectory *blockcalblockdir   = thisblockdir->mkdir("Calib_block");

    for (int l = 0; l < TMMCH_N_Readout; l++) {

      // Mode 1: RAW + per-block slices + per-block calibration graphs
      rawblockdir->cd();
      if (b < (int)hBlockqmaxstrip[l].size()) {
        hBlockqmaxstrip[l][b]->Write();
        if (hBlockAmpslice[l][b])   hBlockAmpslice[l][b]->Write();
        if (hBlockMeanslice[l][b])  hBlockMeanslice[l][b]->Write();
        if (hBlockSigmaslice[l][b]) hBlockSigmaslice[l][b]->Write();
        if (hBlockChi2slice[l][b])  hBlockChi2slice[l][b]->Write();
      }
      if (b < (int)hBlockqmaxstripFull[l].size()) {
        hBlockqmaxstripFull[l][b]->Write();
        if (hBlockAmpsliceFull[l][b])   hBlockAmpsliceFull[l][b]->Write();
        if (hBlockMeansliceFull[l][b])  hBlockMeansliceFull[l][b]->Write();
        if (hBlockSigmasliceFull[l][b]) hBlockSigmasliceFull[l][b]->Write();
        if (hBlockChi2sliceFull[l][b])  hBlockChi2sliceFull[l][b]->Write();
        if (b < (int)g_BlockRawCalibFullDiff[l].size()  && g_BlockRawCalibFullDiff[l][b])  g_BlockRawCalibFullDiff[l][b]->Write();
        if (b < (int)g_BlockRawCalibFullRatio[l].size() && g_BlockRawCalibFullRatio[l][b]) g_BlockRawCalibFullRatio[l][b]->Write();
      }

      // Mode 2: EXT-CAL
      extcalblockdir->cd();
      if (b < (int)hBlockqmaxstrip_extcal[l].size())     hBlockqmaxstrip_extcal[l][b]->Write();
      if (b < (int)hBlockqmaxstripFull_extcal[l].size()) hBlockqmaxstripFull_extcal[l][b]->Write();

      // Mode 3: overall RUN-CAL applied per block
      runcalblockdir->cd();
      if (b < (int)hBlockqmaxstrip_runcal[l].size())     hBlockqmaxstrip_runcal[l][b]->Write();
      if (b < (int)hBlockqmaxstripFull_runcal[l].size()) hBlockqmaxstripFull_runcal[l][b]->Write();

      // Mode 4: per-block calibration
      blockcalblockdir->cd();
      if (b < (int)hBlockqmaxstrip_blockcal[l].size())     hBlockqmaxstrip_blockcal[l][b]->Write();
      if (b < (int)hBlockqmaxstripFull_blockcal[l].size()) hBlockqmaxstripFull_blockcal[l][b]->Write();
    }
    blockdir->cd();
  }
  outfile->cd();

  outfile->Write();
  outfile->Close();
  delete outfile;
  outfile = nullptr;

  cout << "Output written to " << outputFileName << endl;
}
