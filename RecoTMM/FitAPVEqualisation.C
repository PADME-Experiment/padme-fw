// FitAPVEqualisation.C

#include <TFile.h>
#include <TH2.h>
#include <TProfile.h>
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <TGraphErrors.h>

using namespace std;

const double APV_BOUNDARY = 14.25; // mm

// ------------------------------------------------------------
struct APVCalib {
  double A1 = 1.;
  double A2 = 1.;
  double A3 = 1.;

  double c1 = 1.;
  double c2 = 1.;
  double c3 = 1.;
};

// ------------------------------------------------------------
// APV strip boundaries
// ------------------------------------------------------------
const int APV1_LAST  = 121;
const int APV2_FIRST = 122;
const int APV2_LAST  = 234;
const int APV3_FIRST = 235;

// ------------------------------------------------------------
// Fit function versus physics strip index
// ------------------------------------------------------------
Double_t fitf(Double_t *x, Double_t *par)
{
  double strip = x[0];

  double xnorm = par[0]; // APV2

  if (strip < APV2_FIRST)
    xnorm = par[3];      // APV1

  if (strip >= APV3_FIRST)
    xnorm = par[4];      // APV3

  double arg = 0.;

  if (par[2] != 0.)
    arg = (strip - par[1]) / par[2];

  return xnorm * TMath::Exp(-0.5 * arg * arg);
}

// ------------------------------------------------------------
// ------------------------------------------------------------
APVCalib FitOneAPV(TH2 *h2, TString tag)
{
  APVCalib calib;

  if (!h2) {
    cout << "ERROR: null histogram for " << tag << endl;
    return calib;
  }

  TProfile *p = h2->ProfileX(Form("prof_%s", tag.Data()));

  const int nstrip = 359; // strips 0...358

  TGraphErrors *g = new TGraphErrors();

  int ip = 0;

  for (int bx = 1; bx <= p->GetNbinsX(); bx++) {

    int iStrip = bx - 1;   // ROOT bin 1 -> strip 0

    if (iStrip < 0 || iStrip > 358)
      continue;

    double q  = p->GetBinContent(bx);
    double eq = p->GetBinError(bx);

    if (q <= 0.)
      continue;

    g->SetPoint(ip, iStrip, q);
    g->SetPointError(ip, 0., eq);

    ip++;
  }

  TF1 *f = new TF1(Form("fit_%s", tag.Data()), fitf, 80., 270, 5);

  f->SetParNames(
    "A_APV2",
    "Mean_strip",
    "Sigma_strip",
    "A_APV1",
    "A_APV3"
  );

  double Ainit = p->GetMaximum();

  f->SetParameters(
    Ainit,  // A_APV2
    179.,   // mean strip
    40.,    // sigma in strips
    Ainit,  // A_APV1
    Ainit   // A_APV3
  );

  f->SetParLimits(0, 0., 1.e9);
  f->SetParLimits(2, 1., 500.);
  f->SetParLimits(3, 0., 1.e9);
  f->SetParLimits(4, 0., 1.e9);


  g->Fit(f, "R");

  calib.A2 = f->GetParameter(0);
  calib.A1 = f->GetParameter(3);
  calib.A3 = f->GetParameter(4);

  calib.c1 = 1.;
  calib.c2 = 1.;
  calib.c3 = 1.;

  if (calib.A1 > 0.) calib.c1 = calib.A2 / calib.A1;
  if (calib.A3 > 0.) calib.c3 = calib.A2 / calib.A3;

  cout << endl;
  cout << "======================================" << endl;
  cout << "APV equalisation constants for " << tag << endl;
  cout << "======================================" << endl;
  cout << "A_APV1 = " << calib.A1 << endl;
  cout << "A_APV2 = " << calib.A2 << endl;
  cout << "A_APV3 = " << calib.A3 << endl;
  cout << "APV1_fq" << tag << " = " << calib.c1 << ";" << endl;
  cout << "APV2_fq" << tag << " = " << calib.c2 << ";" << endl;
  cout << "APV3_fq" << tag << " = " << calib.c3 << ";" << endl;

  TCanvas *c = new TCanvas(
    Form("c_%s", tag.Data()),
    Form("APV fit %s", tag.Data()),
    900,
    700
  );

  g->SetTitle(Form("APV equalisation %s;strip index;q_{max}", tag.Data()));
  g->SetMarkerStyle(20);
  g->Draw("AP");
  f->Draw("same");

  return calib;
}

// ------------------------------------------------------------
void WriteConstantsToTxt(
  const char *txtFileName,
  int RunID,
  const APVCalib &calibX,
  const APVCalib &calibY
)
{
  ofstream out(txtFileName);

  if (!out.is_open()) {
    cout << "ERROR: cannot open output txt file " << txtFileName << endl;
    return;
  }

  out << fixed << setprecision(8);

  out << "# APV fine equalisation constants" << endl;
  out << "#RunID,APV1_fqx,APV2_fqx,APV3_fqx,APV1_fqy,APV2_fqy,APV3_fqy" << endl;
  out << RunID << "," << calibX.c1 << "," << calibX.c2 << "," << calibX.c3 << "," << calibY.c1 << "," << calibY.c2 << "," << calibY.c3 << endl;
  out.close();

  cout << endl;
  cout << "Calibration constants written to: " << txtFileName << endl;
}

// ------------------------------------------------------------
void FitAPVEqualisation(
  const TString inputFileName,
  const int RunID)
{
  TString path = "/home/mancinima/BeamMonitorRun4/padme-fw/TMM_Calibration/outputTMM/";
  TFile *fin = TFile::Open((path + inputFileName).Data(), "READ");

  if (!fin || fin->IsZombie()) {
    cout << "ERROR: cannot open file " << inputFileName << endl;
    return;
  }

  TH2 *h2x = (TH2*)fin->Get("Raw/hqmaxstrip0");
  TH2 *h2y = (TH2*)fin->Get("Raw/hqmaxstrip1");

  if (!h2x) {
    cout << "ERROR: cannot find X histogram " << endl;
    return;
  }

  if (!h2y) {
    cout << "ERROR: cannot find Y histogram " << endl;
    return;
  }

  APVCalib calibX = FitOneAPV(h2x, "x");
  APVCalib calibY = FitOneAPV(h2y, "y");

  char *txtFileName = Form("APV_equalisation_constants_%d_even1.txt", RunID);

  WriteConstantsToTxt(txtFileName, RunID, calibX, calibY);
}