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

// class members

void RecoTMM::StripFinder(int &iReadout, int &iStrip) { 
  // ireadout è la variabile del tree che ti dice se la strip sia x o y, istrip è la variabile del tree che ti dice il numero di strip 
  // il tree che sto analizzando è mappato da mmqad 
  // voglio ruotare le x e le y nel sistema di riferimento PADME
  // from the electronics maps to our local reference system, there are
  // old     new 
  // x  -->  y
  // y  --> -x
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
  // sistema di riferimento rispetto al centro della camera con padme reference frame
  // mancano traslazioni e rotazioni metrologia
  if(iReadout == 0) { //X
    x_strip = x_strip + GLOBAL_X_TRANSLATION; 
  }
  else if(iReadout == 1) { //Y
    x_strip = x_strip + GLOBAL_Y_TRANSLATION;
  }
}

void RecoTMM::CoordinateFinder(int iStrip, const vector<short> &camp, int &iReadout, double &x_strip, double &q_strip){

  if (camp.empty()) {
    x_strip = -999;
    q_strip = 0;
    return;
  }
  int Nbins = camp.size();
  double qmax = -1000;
  
  for (int ibin = 0; ibin < Nbins; ibin++) {
    double qbin = camp.at(ibin); // Charge in the bin
    if (qbin > qmax) {
      qmax = qbin;
    }
  } //massimo assoluto in carica trovato

  StripFinder(iReadout, iStrip); //rimappa istrip nelle posizioni fisiche corrette con sistema di riferimento locale

  // correzione carica in base alla calibrazione APV, da fare dopo la strip finder per mappare correttamente i canali
  //x_strip = iStrip * pitch - pitch;
  x_strip = iStrip;
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
  // mappatura corretta delle strip con coordinate globali e carica corretta, dopo calibrazione apv
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
    // Keep returning the fit.
    // The caller can decide whether to use it or reject it.
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
    double xini = sFull.mean->GetBinCenter(StripMin);
    double xfin = sFull.mean->GetBinCenter(StripMax);
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
    g_BlockBeamSpread[iR]->SetPointError(p1, 0., 0); //errors randomly selected

    int p2 = g_BlockBeamCharge[iR]->GetN();
    g_BlockBeamCharge[iR]->SetPoint(p2, xblock, charge_proxy);
    g_BlockBeamCharge[iR]->SetPointError(p2, 0., sqrt(charge_proxy)); //errors randomly selected

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

  // cout << "DEBUG LoopFileList: start" << endl;

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
  // Histograms and graphs: created once, before creating TChain and looping on events
  // ------------------------------------------------------------
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    // charge distribution histograms
    hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500);
    // hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstrip[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip[l]->SetYTitle("q_{max} [ADC counts]");

    hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500);
    // hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstripFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull[l]->SetYTitle("q_{max} [ADC counts]");

    // FitSlicesY() histograms
    hAmpslice[l] = new TH1D(Form("hAmpslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2);
    // hAmpslice[l] = new TH1D(Form("hAmpslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hAmpslice[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hAmpslice[l]->SetYTitle("AMP (q_{max} [ADC counts])");

    hMeanslice[l] = new TH1D(Form("hMeanslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2);
    // hMeanslice[l] = new TH1D(Form("hMeanslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hMeanslice[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hMeanslice[l]->SetYTitle("q_{max} [ADC counts]");

    hSigmaslice[l] = new TH1D(Form("hSigmaslice%s", tmm_tag[l].Data()), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2);
    // hSigmaslice[l] = new TH1D(Form("hSigmaslice%s", tmm_tag[l].Data()), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hSigmaslice[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hSigmaslice[l]->SetYTitle("#sigma_{q_{max}} [ADC counts]");
    
    hAmpsliceFull[l] = new TH1D(Form("hAmpsliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2);
    // hAmpsliceFull[l] = new TH1D(Form("hAmpsliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hAmpsliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hAmpsliceFull[l]->SetYTitle("AMP (q_{max} [ADC counts])");

    hMeansliceFull[l] = new TH1D(Form("hMeansliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2);
    // hMeansliceFull[l] = new TH1D(Form("hMeansliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hMeansliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hMeansliceFull[l]->SetYTitle("q_{max} [ADC counts]");

    hSigmasliceFull[l] = new TH1D(Form("hSigmasliceFull%s", tmm_tag[l].Data()), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2);
    // hSigmasliceFull[l] = new TH1D(Form("hSigmasliceFull%s", tmm_tag[l].Data()), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5);
    hSigmasliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hSigmasliceFull[l]->SetYTitle("#sigma_{q_{max}} [ADC counts]");

    //calibrated plots
    hqmaxstrip_cal[l] = new TH2F(Form("hqmaxstrip_cal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500);
    // hqmaxstrip_cal[l] = new TH2F(Form("hqmaxstrip_cal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstrip_cal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip_cal[l]->SetYTitle("q_{max} calib [ADC counts]");

    hqmaxstripFull_cal[l] = new TH2F(Form("hqmaxstripFull_cal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500);
    // hqmaxstripFull_cal[l] = new TH2F(Form("hqmaxstripFull_cal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500);
    hqmaxstripFull_cal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_cal[l]->SetYTitle("q_{max} calib [ADC counts]");

    // block by block tgrapherrors
    g_BlockBeamSpot[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpot[l], Form("g_BlockBeamSpot_%s", tmm_tag[l].Data()), "entry", Form("%s_{Beam} strip", tmm_tag[l].Data()), 20, kBlue + l);

    g_BlockBeamSpread[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpread[l], Form("g_BlockBeamSpread_%s", tmm_tag[l].Data()), "entry", Form("#sigma %s_{Beam} strip", tmm_tag[l].Data()), 21, kRed + l);

    g_BlockBeamCharge[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamCharge[l], Form("g_BlockBeamCharge_%s", tmm_tag[l].Data()), "entry", "q_{Beam} [ADC counts]", 22, kGreen + 2 + l);
  
    //TGraph for strip calibration and equalization
    g_RawCalibFullDiff[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullDiff[l], Form("g_RawCalibFullDiff%s", tmm_tag[l].Data()), Form("%s strip", tmm_tag[l].Data()), "Q_{calib} - Q_{raw} [ADC counts]", 22, kBlack);

    g_RawCalibFullRatio[l] = new TGraphErrors();
    TGraphAttribute(g_RawCalibFullRatio[l], Form("g_RawCalibFullRatio%s", tmm_tag[l].Data()), Form("%s strip", tmm_tag[l].Data()), "Q_{calib} / Q_{raw}", 22, kBlack);
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

        // charge distribution histograms
        hBlockqmaxstrip[l].push_back(new TH2F(Form("hBlockqmaxstrip%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500));
        // hBlockqmaxstrip[l].push_back(new TH2F(Form("hBlockqmaxstrip%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstrip[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstrip[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull[l].push_back(new TH2F(Form("hBlockqmaxstripFull%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500));
        // hBlockqmaxstripFull[l].push_back(new TH2F(Form("hBlockqmaxstripFull%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstripFull[l].back()->SetYTitle("q_{max} [ADC counts]");

        // charge distribution histograms calibrated block by block
        hBlockqmaxstrip_cal[l].push_back(new TH2F(Form("hBlockqmaxstrip_cal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500));
        // hBlockqmaxstrip_cal[l].push_back(new TH2F(Form("hBlockqmaxstrip_cal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstrip_cal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstrip_cal[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull_cal[l].push_back(new TH2F(Form("hBlockqmaxstripFull_cal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500));
        // hBlockqmaxstripFull_cal[l].push_back(new TH2F(Form("hBlockqmaxstripFull_cal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull_cal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstripFull_cal[l].back()->SetYTitle("q_{max} [ADC counts]");

        // charge distribution histograms overall calibrated
        hBlockqmaxstrip_Overallcal[l].push_back(new TH2F(Form("hBlockqmaxstrip_Overallcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500));
        // hBlockqmaxstrip_Overallcal[l].push_back(new TH2F(Form("hBlockqmaxstrip_Overallcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstrip_Overallcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstrip_Overallcal[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull_Overallcal[l].push_back(new TH2F(Form("hBlockqmaxstripFull_Overallcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -xmax/2, +xmax/2, 1000, 0, 2500));
        // hBlockqmaxstripFull_Overallcal[l].push_back(new TH2F(Form("hBlockqmaxstripFull_Overallcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), MAXSTRIP, -0.5, MAXSTRIP-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull_Overallcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockqmaxstripFull_Overallcal[l].back()->SetYTitle("q_{max} [ADC counts]");

        // FitSlicesY() histograms
        hBlockAmpslice[l].push_back(new TH1D(Form("hBlockAmpslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockAmpslice[l].push_back(new TH1D(Form("hBlockAmpslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockAmpslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockAmpslice[l].back()->SetYTitle("AMP (q_{max} [ADC counts])");

        hBlockMeanslice[l].push_back(new TH1D(Form("hBlockMeanslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockMeanslice[l].push_back(new TH1D(Form("hBlockMeanslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockMeanslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockMeanslice[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockSigmaslice[l].push_back(new TH1D(Form("hBlockSigmaslice%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockSigmaslice[l].push_back(new TH1D(Form("hBlockSigmaslice%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockSigmaslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockSigmaslice[l].back()->SetYTitle("#sigma_{q_{max}} [ADC counts]");

        hBlockChi2slice[l].push_back(new TH1D(Form("hBlockChi2slice%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockChi2slice[l].push_back(new TH1D(Form("hBlockChi2slice%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockChi2slice[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockChi2slice[l].back()->SetYTitle("#Chi^{2}_{q_{max}} [ADC counts]");
        
        hBlockAmpsliceFull[l].push_back(new TH1D(Form("hBlockAmpsliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockAmpsliceFull[l].push_back(new TH1D(Form("hBlockAmpsliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockAmpsliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockAmpsliceFull[l].back()->SetYTitle("AMP (q_{max} [ADC counts])");

        hBlockMeansliceFull[l].push_back(new TH1D(Form("hBlockMeansliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockMeansliceFull[l].push_back(new TH1D(Form("hBlockMeansliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockMeansliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockMeansliceFull[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockSigmasliceFull[l].push_back(new TH1D(Form("hBlockSigmasliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockSigmasliceFull[l].push_back(new TH1D(Form("hBlockSigmasliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockSigmasliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockSigmasliceFull[l].back()->SetYTitle("#sigma_{q_{max}} [ADC counts]");

        hBlockChi2sliceFull[l].push_back(new TH1D(Form("hBlockChi2sliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -xmax/2, +xmax/2));
        // hBlockChi2sliceFull[l].push_back(new TH1D(Form("hBlockChi2sliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution d [") + tmm_tag[l] + TString("]"), MAXSTRIP, -0.5, MAXSTRIP-0.5));
        hBlockChi2sliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
        hBlockChi2sliceFull[l].back()->SetYTitle("#Chi^{2}_{q_{max}} [ADC counts]");

        //TGraph for strip calibration and equalization
        g_BlockRawCalibFullDiff[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockRawCalibFullDiff[l].back(), Form("g_BlockRawCalibFullDiff%s", tmm_tag[l].Data()), "strip", "Q_{calib} - Q_{raw} [ADC counts]", 22, kBlack);

        g_BlockRawCalibFullRatio[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockRawCalibFullRatio[l].back(), Form("g_BlockRawCalibFullRatio%s", tmm_tag[l].Data()), "strip", "Q_{calib} / Q_{raw}", 22, kBlack);
      }
    }
  };

  // ------------------------------------------------------------
  // Create chain of input files
  // ------------------------------------------------------------
  // cout << "DEBUG LoopFileList: after creating TChain, before checking entries" << endl;

  if (!fTree) {
    cerr << "ERROR: fTree is null in LoopFileList" << endl;
    return;
  }
  // fTree->Print();
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
    // cout << "Event " << iev << ": firedstrip_size = " << firedstrip_size << endl;

    for (int j = 0; j < firedstrip_size; j++) {
      double x_strip = 0;
      double q_strip = 0;

      int channel = mmStrip->at(j);

      // mmReadout is vector<char>. Expected values appear to be 'X' = 88 and 'Y' = 89.
      int iReadout = (int)mmReadout->at(j);
      int iReadout_new = iReadout - 88; // Map 'X'->0, 'Y'->1 to be compatible with TMMCH_N_Readout

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

      CoordinateFinder(channel, raw_q->at(j), iReadout_new, x_strip, q_strip);

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

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

  auto IsValidBlockForCalibration = [&](int iR, int iblock) -> bool {

    if (iR < 0 || iR >= TMMCH_N_Readout) return false;
    if (iblock < 0) return false;

    if (iblock >= (int)hBlockqmaxstrip_cal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstripFull_cal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstrip_Overallcal[iR].size()) return false;
    if (iblock >= (int)hBlockqmaxstripFull_Overallcal[iR].size()) return false;

    if (!hBlockqmaxstrip_cal[iR][iblock]) return false;
    if (!hBlockqmaxstripFull_cal[iR][iblock]) return false;
    if (!hBlockqmaxstrip_Overallcal[iR][iblock]) return false;
    if (!hBlockqmaxstripFull_Overallcal[iR][iblock]) return false;

    if (iblock >= (int)g_BlockRawCalibFullRatio[iR].size()) return false;
    if (!g_BlockRawCalibFullRatio[iR][iblock]) return false;

    return true;
  };

  for (int iR = 0; iR < TMMCH_N_Readout; iR++) {

    SliceFitResult even = RunFitSlicesY(hqmaxstrip[iR], Form("%s_even", tmm_tag[iR].Data()));
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
    TF1 *fit = FitDoubleGaussian(hMeanslice[iR], tmm_tag[iR], StripMin, StripMax, fitResultOverall);

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

  // cout << "DEBUG: arriva alla fine del primo loop sugli eventi" << endl;
  
  //calibrated plots
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

      int channel = mmStrip->at(j);
      // cout << "DEBUG: channel = " << channel << endl;

      // mmReadout is vector<char>. Expected values appear to be 'X' = 88 and 'Y' = 89.
      int iReadout = (int)mmReadout->at(j);
      int iReadout_new = iReadout - 88; // Map 'X'->0, 'Y'->1 to be compatible with TMMCH_N_Readout

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

      CoordinateFinder(channel, raw_q->at(j), iReadout_new, x_strip, q_strip);
      // cout << "DEBUG: CoordinateFinder correctly applied and xstrip found = " << x_strip << endl;

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

      if (channel > MAXSTRIP) {
        cout << "ERROR channel bigger than RawCalibFullRatio vector - channel = " << channel << endl;
      }

      double chargecorrection = 1;
      //hardcoded with the precise path to the calibration constants
      LoadCalibrationConstants("/home/mancinima/BeamMonitorRun4/padme-fw/TMM_Calibration/outputTMM/TMMCalibration_run677_CalibrationConstant.txt");

      if (x_strip >= 0 && x_strip < MAXSTRIP) {
        int istrip = (int)x_strip;

        if (iReadout_new == 0) chargecorrection = XcalibConst[istrip];
        if (iReadout_new == 1) chargecorrection = YcalibConst[istrip];
      }
      if (channel % 2 == 0) {
        hqmaxstrip_cal[iReadout_new]->Fill(x_strip, q_strip*chargecorrection);
      }
      hqmaxstripFull_cal[iReadout_new]->Fill(x_strip, q_strip*chargecorrection);
    
      int ic = iev / NevtBlock;

      if (IsValidBlockForCalibration(iReadout_new, ic)) {

        double Blockchargecorrection = 1.;

        if (x_strip > StripMin && x_strip < StripMax) {

          if (g_BlockRawCalibFullRatio[iReadout_new][ic]->GetN() > 0) {
            Blockchargecorrection = g_BlockRawCalibFullRatio[iReadout_new][ic]->Eval(x_strip);
          } else {
            cerr << "WARNING: empty g_BlockRawCalibFullRatio for readout " << tmm_tag[iReadout_new] << " block " << ic << " ; using Blockchargecorrection = 1" << endl;
          }
        }
        if (channel % 2 == 0) {
          hBlockqmaxstrip_cal[iReadout_new][ic]->Fill(x_strip, q_strip * Blockchargecorrection);
          hBlockqmaxstrip_Overallcal[iReadout_new][ic]->Fill(x_strip,q_strip * chargecorrection);
        }
        hBlockqmaxstripFull_cal[iReadout_new][ic]->Fill(x_strip,q_strip * Blockchargecorrection);
        hBlockqmaxstripFull_Overallcal[iReadout_new][ic]->Fill(x_strip,q_strip * chargecorrection);
      } else {
        cerr << "WARNING: invalid block calibration index"
            << " event = " << iev << " ic = " << ic
            << " readout = " << iReadout_new << " tag = " << tmm_tag[iReadout_new]
            << " hBlockqmaxstrip_cal size = " << hBlockqmaxstrip_cal[iReadout_new].size()
            << " g_BlockRawCalibFullRatio size = " << g_BlockRawCalibFullRatio[iReadout_new].size()
            << endl;
      }
    }
  }
  // cout << "DEBUG: hBlockqmaxstripFull_cal are filled" << endl;

  // ------------------------------------------------------------
  // Write output ROOT file
  // ------------------------------------------------------------
  cout << "#### Creating output file " << outputFileName << endl;
  TFile *outfile = new TFile(outputFileName.Data(), "recreate");
  TDirectory *overalldir = outfile->mkdir("Overall");
  TDirectory *rawoveralldir = overalldir->mkdir("Raw");
  TDirectory *caloveralldir = overalldir->mkdir("Calib");
  TDirectory *blockdir = outfile->mkdir("Blocks");

  if (!outfile || outfile->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outputFileName << endl;
    if (outfile) delete outfile;
    return;
  }
  outfile->cd();
  overalldir->cd();
  rawoveralldir->cd();
  // cout << "DEBUG: Created output file " << outputFileName << endl;
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip[l]->Write();
    hqmaxstripFull[l]->Write();
    hAmpslice[l]->Write();
    hMeanslice[l]->Write();
    hSigmaslice[l]->Write();
    hChi2slice[l]->Write();
    hAmpsliceFull[l]->Write();
    hMeansliceFull[l]->Write();
    hSigmasliceFull[l]->Write();
    hChi2sliceFull[l]->Write();
    g_RawCalibFullDiff[l]->Write();
    g_RawCalibFullRatio[l]->Write();
    // f[l]->Write();
  }
  outfile->cd();
  overalldir->cd();  
  caloveralldir->cd();
  // cout << "DEBUG: Created output file " << outputFileName << endl;
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip_cal[l]->Write();
    hqmaxstripFull_cal[l]->Write();
  }

  // Write block histograms in subdirectories
  outfile->cd();
  blockdir->cd();
  for (int l = 0; l < TMMCH_N_Readout; l++) {  
    g_BlockBeamSpot[l]->Write();
    g_BlockBeamSpread[l]->Write();
    g_BlockBeamCharge[l]->Write();
  }
  for (int b = 0; b < (int)g_BlockBeamSpot[0]->GetN(); b++) {
    TDirectory *thisblockdir = blockdir->mkdir(Form("block_%04d", b));
    TDirectory *rawblockdir = thisblockdir->mkdir("Raw");
    TDirectory *calblockdir = thisblockdir->mkdir("Calib");
    TDirectory *caltotblockdir = thisblockdir->mkdir("Calib_overall");
    thisblockdir->cd();

    for (int l = 0; l < TMMCH_N_Readout; l++) {
      thisblockdir->cd();
      rawblockdir->cd();
      if (b < (int)hBlockqmaxstrip[l].size()) {
        hBlockqmaxstrip[l][b]->Write();
        hBlockAmpslice[l][b]->Write();
        hBlockMeanslice[l][b]->Write();
        hBlockSigmaslice[l][b]->Write();
        hBlockChi2slice[l][b]->Write();
      }

      if (b < (int)hBlockqmaxstripFull[l].size()) {
        hBlockqmaxstripFull[l][b]->Write();
        hBlockAmpsliceFull[l][b]->Write();
        hBlockMeansliceFull[l][b]->Write();
        hBlockSigmasliceFull[l][b]->Write();
        hBlockChi2sliceFull[l][b]->Write();
        g_BlockRawCalibFullDiff[l][b]->Write();
        g_BlockRawCalibFullRatio[l][b]->Write();
      }
      thisblockdir->cd();
      calblockdir->cd();
      if (b < (int)hBlockqmaxstrip_cal[l].size()) {
        hBlockqmaxstrip_cal[l][b]->Write();
      }
      if (b < (int)hBlockqmaxstripFull_cal[l].size()) {
        hBlockqmaxstripFull_cal[l][b]->Write();
      }
      thisblockdir->cd();
      caltotblockdir->cd();
      if (b < (int)hBlockqmaxstrip_Overallcal[l].size()) {
        hBlockqmaxstrip_Overallcal[l][b]->Write();
      }
      if (b < (int)hBlockqmaxstripFull_Overallcal[l].size()) {
        hBlockqmaxstripFull_Overallcal[l][b]->Write();
      }
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
