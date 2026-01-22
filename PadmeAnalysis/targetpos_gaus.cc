#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"
#include "TLegend.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

struct ClusterPair {
  double e1, x1, y1, z1;
  double e2, x2, y2, z2;
};

std::vector<ClusterPair> pairs;

double g_manual_xt = 0.0;
double g_manual_yt = 0.0;
bool   g_manual_valid = false;

TLorentzVector ComputeCMMomentum(double Energy,
                                 double xt, double yt,
                                 double PosX, double PosY, double PosZ)
{
  const double fMe = 0.511;
  const double fBeamMomentum = 431.6;
  const double zt = -1028.0;

  double sqrts = sqrt(2.*fMe*fMe + 2.*fBeamMomentum*fMe);
  double betagamma = fBeamMomentum / sqrts;
  double gamma = sqrt(betagamma*betagamma + 1.);
  double beta = betagamma / gamma;

  TVector3 cluPos(PosX, PosY, 2508.31);
  TVector3 targetPos(xt, yt, zt);

  TVector3 cluMom = cluPos - targetPos;
  cluMom *= 1.0 / cluMom.Mag();

  TVector3 COGPos(2.1, 0.67, 2508.31);
  TVector3 boost = COGPos - targetPos;
  boost *= beta / boost.Mag();

  double cosa = cluMom.Dot(boost);

  double Elab =
    0.5 * sqrts /
    sqrt(1. - cosa*cosa
         + pow(gamma*cosa,2)
         - 2.*betagamma*gamma*cosa
         + betagamma*betagamma);

  cluMom *= Elab;
  cluMom *= Energy;

  TLorentzVector p4;
  p4.SetVectM(cluMom, 0.);
  p4.Boost(-boost);

  return p4;
}

double NLL_Gaussian(const double *par)
{
  const double xt = par[0];
  const double yt = par[1];
  const double PI = TMath::Pi();

  double sum_theta2 = 0.0;
  double sum_phi2   = 0.0;
  int N = 0;

  for (const auto &p : pairs) {

    auto p1 = ComputeCMMomentum(p.e1, xt, yt, p.x1, p.y1, p.z1);
    auto p2 = ComputeCMMomentum(p.e2, xt, yt, p.x2, p.y2, p.z2);

    double thsum = p1.Vect().Theta() + p2.Vect().Theta();
    if (thsum < 3.0 || thsum > 3.3) continue;

    double dtheta = thsum - PI;

    double dphi = fabs(p1.Phi() - p2.Phi());
    //if (dphi > PI) dphi = 2.*PI - dphi;
    double dphi_shift = dphi - PI;

    sum_theta2 += dtheta * dtheta;
    sum_phi2   += dphi_shift * dphi_shift;
    N++;
  }

  if (N < 10) return 1e12;

  double var_theta = sum_theta2 / N;
  double var_phi   = sum_phi2   / N;

  if (var_theta <= 0 || var_phi <= 0) return 1e12;
  
  return var_theta + var_phi;
  //return 0.5 * N * (2.0 + log(var_theta) + log(var_phi));
}

double ComputeNLL_Minuit(double xt, double yt)
{
  double par[2] = {xt, yt};
  return NLL_Gaussian(par);
}

void FitTarget()
{
  pairs.clear();

  ifstream in("gamma_gamma_allcoord.txt");
  ClusterPair cp;

  while (in >> cp.e1 >> cp.x1 >> cp.y1 >> cp.z1
         >> cp.e2 >> cp.x2 >> cp.y2 >> cp.z2)
    pairs.push_back(cp);

  cout << "Loaded " << pairs.size() << " pairs\n";

  auto *min =
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

  ROOT::Math::Functor f(&NLL_Gaussian, 2);
  min->SetFunction(f);

  min->SetLimitedVariable(0, "xt", 0.0, 0.001, -20.0, 20.0);
  min->SetLimitedVariable(1, "yt", 0.0, 0.001, -20.0, 20.0);

  min->Minimize();

  const double *x = min->X();
  double NLL_min = min->MinValue();

  cout << "\n=== Minuit Gaussian NLL fit ===\n";
  cout << "xt = " << x[0] << "\n";
  cout << "yt = " << x[1] << "\n";
  cout << "zt = -1028 (fixed)\n";
  cout << "NLL_min = " << NLL_min << "\n";

  // 1σ uncertainty extraction via ΔNLL = 0.5

  double xt0 = x[0];
  double yt0 = x[1];

  double sigma_xt_low = 0, sigma_xt_high = 0;
  double sigma_yt_low = 0, sigma_yt_high = 0;

  const double scanStep = 0.0005;  // 0.5 micron
  const double targetDelta = 0.5;

  double par[2];

  // --- xt positive direction ---
  for (int i = 1; i < 2000; i++) {
    par[0] = xt0 + i * scanStep;
    par[1] = yt0;
    double dNLL = NLL_Gaussian(par) - NLL_min;
    if (dNLL >= targetDelta) {
      sigma_xt_high = i * scanStep;
      break;
    }
  }

  // --- xt negative direction ---
  for (int i = 1; i < 2000; i++) {
    par[0] = xt0 - i * scanStep;
    par[1] = yt0;
    double dNLL = NLL_Gaussian(par) - NLL_min;
    if (dNLL >= targetDelta) {
      sigma_xt_low = i * scanStep;
      break;
    }
  }

  // --- yt positive direction ---
  for (int i = 1; i < 2000; i++) {
    par[0] = xt0;
    par[1] = yt0 + i * scanStep;
    double dNLL = NLL_Gaussian(par) - NLL_min;
    if (dNLL >= targetDelta) {
      sigma_yt_high = i * scanStep;
      break;
    }
  }

  // --- yt negative direction ---
  for (int i = 1; i < 2000; i++) {
    par[0] = xt0;
    par[1] = yt0 - i * scanStep;
    double dNLL = NLL_Gaussian(par) - NLL_min;
    if (dNLL >= targetDelta) {
      sigma_yt_low = i * scanStep;
      break;
    }
  }

  cout << "\n=== 1σ uncertainties from ΔNLL = 0.5 ===\n";
  cout << "xt = " << xt0
       << " +" << sigma_xt_high
       << " -" << sigma_xt_low << "\n";

  cout << "yt = " << yt0
       << " +" << sigma_yt_high
       << " -" << sigma_yt_low << "\n";

  //double xt0 = x[0];
  // double yt0 = x[1];
  double NLL0 = NLL_min;

  cout << "\n=== Local ΔNLL contour scan (Minuit-based) ===\n";

  double best_xt = xt0;
  double best_yt = yt0;
  double best_dNLL = 1e99;

  const double step = 0.002;   // 2 micron scale
  const int    Nscan = 30;     // ±0.06 mm region

  for (int ix = -Nscan; ix <= Nscan; ix++) {
    for (int iy = -Nscan; iy <= Nscan; iy++) {

      double xt = xt0 + ix * step;
      double yt = yt0 + iy * step;

      double nll = ComputeNLL_Minuit(xt, yt);
      double dNLL = nll - NLL0;

      if (dNLL < best_dNLL) {
        best_dNLL = dNLL;
        best_xt = xt;
        best_yt = yt;
      }


      if (fabs(dNLL) < 0.01) {
        cout << "xt = " << xt
             << " yt = " << yt
             << " ΔNLL = " << dNLL << "\n";
      }
    }
  }

  cout << "\n[ΔNLL scan minimum]\n";
  cout << "xt = " << best_xt
       << " yt = " << best_yt
       << " ΔNLL = " << best_dNLL << "\n";

  // auto refine_parabolic = [&](double x0, double y0, double step, bool refineX)
  // {
  //   double f_m, f_0, f_p;

  //   if (refineX) {
  //     f_m = ComputeNLL_Minuit(x0 - step, y0);
  //     f_0 = ComputeNLL_Minuit(x0,        y0);
  //     f_p = ComputeNLL_Minuit(x0 + step, y0);
  //   } else {
  //     f_m = ComputeNLL_Minuit(x0, y0 - step);
  //     f_0 = ComputeNLL_Minuit(x0, y0);
  //     f_p = ComputeNLL_Minuit(x0, y0 + step);
  //   }

  //   double denom = (f_m - 2.0*f_0 + f_p);
  //   if (fabs(denom) < 1e-14) return 0.0;

  //   double delta = 0.5 * (f_m - f_p) / denom;
  //   return delta * step;
  // };

  // double refined_xt = best_xt;
  // double refined_yt = best_yt;

  // for (int iter = 0; iter < 6; iter++) {

  //   double dx = refine_parabolic(refined_xt, refined_yt, 0.002, true);
  //   refined_xt += dx;

  //   double dy = refine_parabolic(refined_xt, refined_yt, 0.002, false);
  //   refined_yt += dy;
  // }

  // double refinedNLL = ComputeNLL_Minuit(refined_xt, refined_yt);
  // double refined_dNLL = refinedNLL - NLL0;

  // cout << "\n=== FINAL REFINED (Minuit-based) RESULT ===\n";
  // cout << "xt = " << refined_xt << "\n";
  // cout << "yt = " << refined_yt << "\n";
  // cout << "zt = -1028 (fixed)\n";
  // cout << "NLL = " << refinedNLL << "\n";
  // cout << "ΔNLL = " << refined_dNLL << "\n";

    cout << "\n=== Building map ===\n";

  gStyle->SetOptStat(0);

const double rangeX = 7;  
const double rangeY = 7;
const int    nBinsX = 200;
const int    nBinsY = 200;

// TH2D *hNLL = new TH2D("hNLL", "2D #DeltaNLL; xt [mm]; yt [mm]",
//                       nBinsX, xt0 - rangeX, xt0 + rangeX,
//                       nBinsY, yt0 - rangeY, yt0 + rangeY);

TH2D *hNLL = new TH2D("hNLL", "2D #DeltaNLL; xt [mm]; yt [mm]",
                      nBinsX, -20, 20,
                      nBinsY, -20, 20);

//double best_dNLL = 1e99;
//double best_xt = xt0;
//double best_yt = yt0;

for (int ix = 1; ix <= nBinsX; ix++) {
  double xt = hNLL->GetXaxis()->GetBinCenter(ix);

  for (int iy = 1; iy <= nBinsY; iy++) {
    double yt = hNLL->GetYaxis()->GetBinCenter(iy);

    double par[2] = {xt, yt};
    double nll = NLL_Gaussian(par);
    double dNLL = nll - NLL_min;  // ALWAYS compute ΔNLL

    hNLL->SetBinContent(ix, iy, -(nll));

    if (dNLL < abs(best_dNLL)) {
      best_dNLL = dNLL;
      best_xt = xt;
      best_yt = yt;
    }
  }
}

 double xMin = hNLL->GetXaxis()->GetBinLowEdge(1);
double xMax = hNLL->GetXaxis()->GetBinUpEdge(hNLL->GetNbinsX());
double yMin = hNLL->GetYaxis()->GetBinLowEdge(1);
double yMax = hNLL->GetYaxis()->GetBinUpEdge(hNLL->GetNbinsY());



  cout << "\n[Best point from 2D scan]\n";
  cout << "xt = " << best_xt << "  yt = " << best_yt
       << "  ΔNLL = " << best_dNLL << "\n";

  TCanvas *c1 = new TCanvas("c1", "DeltaNLL Contour", 900, 800);

  hNLL->SetContour(500);
  hNLL->Draw("COLZ");

  // Draw contour lines for specific ΔNLL levels
  double levels[4] = {0.5, 1.0, 2.0, 4.0};
  //double levels[4] = {0.0001, 0.001, 0.01, 0.1};
  //hNLL->SetContour(4, levels);
  //hNLL->Draw("CONT3 SAME");

  // Mark Minuit point
  TLine *lx = new TLine(xt0, yMin, xt0, yMax);
  TLine *ly = new TLine(xMin, yt0, xMax, yt0);
  lx->SetLineColor(kRed);
  ly->SetLineColor(kRed);
  lx->SetLineWidth(1);
  ly->SetLineWidth(1);
  lx->Draw();
  ly->Draw();

  // Mark best grid point
  TLine *lx2 = new TLine(best_xt, yMin, best_xt, yMax);
  TLine *ly2 = new TLine(xMin, best_yt, xMax, best_yt);
  lx2->SetLineColor(kGreen+2);
  ly2->SetLineColor(kGreen+2);
  lx2->SetLineWidth(1);
  ly2->SetLineWidth(1);
  lx2->Draw();
  ly2->Draw();

  // Mark best grid point
  // TLine *lx3 = new TLine(refined_xt, yMin, refined_xt, yMax);
  // TLine *ly3 = new TLine(xMin, refined_yt, xMax, refined_yt);
  // lx3->SetLineColor(kBlue+2);
  // ly3->SetLineColor(kBlue+2);
  // lx3->SetLineWidth(1);
  // ly3->SetLineWidth(1);
  // lx3->Draw();
  // ly3->Draw();

  TLine *lxm = nullptr, *lym = nullptr;

  // Mark manual minimizer result (from MinimizeTargetPosition)
if (g_manual_valid) {

  lxm = new TLine(g_manual_xt, yMin, g_manual_xt, yMax);
  lym = new TLine(xMin, g_manual_yt, xMax, g_manual_yt);

  lxm->SetLineColor(kMagenta+2);
  lym->SetLineColor(kMagenta+2);
  lxm->SetLineWidth(2);
  lym->SetLineWidth(2);
  lxm->SetLineStyle(2);   // dashed
  lym->SetLineStyle(2);   // dashed

  lxm->Draw();
  lym->Draw();

  cout << "[Overlay] Manual minimizer drawn at xt = "
       << g_manual_xt << " yt = " << g_manual_yt << endl;
}

TLegend *leg = new TLegend(0.15, 0.75, 0.45, 0.90);
leg->SetBorderSize(0);
leg->SetFillStyle(0);
leg->SetTextSize(0.03);

leg->AddEntry(lx,  "Minuit minimum",        "l");
leg->AddEntry(lx2, "Grid best point",       "l");
//leg->AddEntry(lx3, "Parabolic refined",     "l");

if (g_manual_valid)
  leg->AddEntry(lxm, "Manual minimizer",    "l");

leg->Draw();

  c1->Update();

  delete min;
}


double ComputeNLL_At(double xt, double yt,
                     const std::vector<ClusterPair> &data)
{
  const double PI = TMath::Pi();

  double sum_theta2 = 0.0;
  double sum_phi2   = 0.0;
  int N = 0;

  for (const auto &p : data) {

    auto p1 = ComputeCMMomentum(p.e1, xt, yt, p.x1, p.y1, p.z1);
    auto p2 = ComputeCMMomentum(p.e2, xt, yt, p.x2, p.y2, p.z2);

    double thsum = p1.Vect().Theta() + p2.Vect().Theta();
    if (thsum < 3.0 || thsum > 3.3) continue;

    double dtheta = thsum - PI;

    double dphi = fabs(p1.Phi() - p2.Phi());
    double dphi_shift = dphi - PI;

    sum_theta2 += dtheta * dtheta;
    sum_phi2   += dphi_shift * dphi_shift;
    N++;
  }

  if (N < 10) return 1e12;

  double var_theta = sum_theta2 / N;
  double var_phi   = sum_phi2   / N;

  if (var_theta <= 0 || var_phi <= 0) return 1e12;

  return 0.5 * N * (2.0 + log(var_theta) + log(var_phi));
}

void MinimizeTargetPosition_dNLL()
{
  vector<ClusterPair> data;

  ifstream fin("gamma_gamma_allcoord.txt");
  ClusterPair cp;

  while (fin >> cp.e1 >> cp.x1 >> cp.y1 >> cp.z1
	 >> cp.e2 >> cp.x2 >> cp.y2 >> cp.z2)
    data.push_back(cp);

  double best_xt = 0.0, best_yt = 0.0;
  double bestNLL = 1e99;

  // -------------------------
  // COARSE SCAN
  // -------------------------
  for (double xt = -20; xt <= 20; xt += 0.5) {
    cout<< "Manual scan: xt = " << xt << endl;
    for (double yt = -20; yt <= 20; yt += 0.5) {

      double nll = ComputeNLL_At(xt, yt, data);
      if (nll < bestNLL) {
	bestNLL = nll;
	best_xt = xt;
	best_yt = yt;
      }
    }
  }

  cout << "[Coarse] xt = " << best_xt << " yt = " << best_yt
       << " NLL = " << bestNLL << endl;

  // -------------------------
  // FINE SCAN
  // -------------------------
  double fine_xt = best_xt;
  double fine_yt = best_yt;
  double fineNLL = bestNLL;

  const double fineStep = 0.05;

  for (int ix = -20; ix <= 20; ix++) {
    cout<< "Fine manual scan: ix = " << ix << endl;
    for (int iy = -20; iy <= 20; iy++) {

      double xt = best_xt + ix * fineStep;
      double yt = best_yt + iy * fineStep;

      double nll = ComputeNLL_At(xt, yt, data);
      if (nll < fineNLL) {
	fineNLL = nll;
	fine_xt = xt;
	fine_yt = yt;
      }
    }
  }

  cout << "[Fine]   xt = " << fine_xt << " yt = " << fine_yt
       << " NLL = " << fineNLL << endl;

  g_manual_xt = fine_xt;
g_manual_yt = fine_yt;
g_manual_valid = true;

cout << "\n[Stored manual minimizer result]\n";
cout << "xt = " << g_manual_xt << "  yt = " << g_manual_yt << endl;


  const double NLL0 = fineNLL;

  cout << "\n=== ΔNLL ultra-fine scan ===\n";

  double best2_xt = fine_xt;
  double best2_yt = fine_yt;
  double best2_dNLL = 1e99;

  const double ultraStep = 0.002;  // 2 micron scale
  const int ultraN = 25;           // ±0.05 region

  for (int ix = -ultraN; ix <= ultraN; ix++) {
    for (int iy = -ultraN; iy <= ultraN; iy++) {

      double xt = fine_xt + ix * ultraStep;
      double yt = fine_yt + iy * ultraStep;

      double nll = ComputeNLL_At(xt, yt, data);
      double dNLL = nll - NLL0;

      if (dNLL < best2_dNLL) {
	best2_dNLL = dNLL;
	best2_xt = xt;
	best2_yt = yt;
      }
    }
  }

  cout << "[Ultra]  xt = " << best2_xt << " yt = " << best2_yt
       << " ΔNLL = " << best2_dNLL << endl;


  // auto refine_parabolic = [&](double x0, double y0, double step, bool refineX)
  // {
  //   double f_m, f_0, f_p;

  //   if (refineX) {
  //     f_m = ComputeNLL_At(x0 - step, y0, data);
  //     f_0 = ComputeNLL_At(x0,        y0, data);
  //     f_p = ComputeNLL_At(x0 + step, y0, data);
  //   } else {
  //     f_m = ComputeNLL_At(x0, y0 - step, data);
  //     f_0 = ComputeNLL_At(x0, y0,        data);
  //     f_p = ComputeNLL_At(x0, y0 + step, data);
  //   }

  //   double denom = (f_m - 2*f_0 + f_p);
  //   if (fabs(denom) < 1e-12) return 0.0;

  //   double delta = 0.5 * (f_m - f_p) / denom;
  //   return delta * step;
  // };

  // double refined_xt = best2_xt;
  // double refined_yt = best2_yt;

  // for (int iter = 0; iter < 5; iter++) {

  //   double dx = refine_parabolic(refined_xt, refined_yt, 0.002, true);
  //   refined_xt += dx;

  //   double dy = refine_parabolic(refined_xt, refined_yt, 0.002, false);
  //   refined_yt += dy;
  // }

  // double finalNLL = ComputeNLL_At(refined_xt, refined_yt, data);

  // cout << "\n=== FINAL REFINED RESULT ===\n";
  // cout << "xt = " << refined_xt << endl;
  // cout << "yt = " << refined_yt << endl;
  // cout << "zt = -1028 (fixed)\n";
  // cout << "NLL = " << finalNLL << endl;
  // cout << "ΔNLL = " << finalNLL - NLL0 << endl;
}




void MinimizeTargetPosition()
{
  vector<ClusterPair> data;

  ifstream fin("gamma_gamma_allcoord.txt");
  ClusterPair cp;

  while (fin >> cp.e1 >> cp.x1 >> cp.y1 >> cp.z1
	 >> cp.e2 >> cp.x2 >> cp.y2 >> cp.z2)
    data.push_back(cp);

  const double PI = TMath::Pi();
  const double zt = -1028.0;

  auto ComputeNLL = [&](double xt, double yt) {

    double sum_theta2 = 0.0;
    double sum_phi2   = 0.0;
    int N = 0;

    for (const auto &p : data) {

      auto p1 = ComputeCMMomentum(p.e1, xt, yt,
                                  p.x1, p.y1, p.z1);
      auto p2 = ComputeCMMomentum(p.e2, xt, yt,
                                  p.x2, p.y2, p.z2);

      double thsum = p1.Vect().Theta() + p2.Vect().Theta();
      if (thsum < 3.0 || thsum > 3.3) continue;

      double dtheta = thsum - PI;

      double dphi = fabs(p1.Phi() - p2.Phi());
      //if (dphi > PI) dphi = 2.*PI - dphi;
      double dphi_shift = dphi - PI;

      sum_theta2 += dtheta * dtheta;
      sum_phi2   += dphi_shift * dphi_shift;
      N++;
    }

    if (N < 10) return 1e12;

    double var_theta = sum_theta2 / N;
    double var_phi   = sum_phi2   / N;
    if (var_theta <= 0 || var_phi <= 0) return 1e12;
    return var_theta + var_phi;
    //return 0.5 * N * (2.0 + log(var_theta) + log(var_phi));
  };


  double best_xt = 0, best_yt = 0;
  double bestNLL = 1e99;

  for (double xt = -20; xt <= 20; xt += 0.5) {
    std::cout<<"Initial scan: xt = "<<xt<<std::endl;
    for (double yt = -20; yt <= 20; yt += 0.5) {

      double nll = ComputeNLL(xt, yt);
      if (nll < bestNLL) {
        bestNLL = nll;
        best_xt = xt;
        best_yt = yt;
      }
    }
  }

  cout << "\n[Coarse scan]\n";
  cout << "xt = " << best_xt
       << " yt = " << best_yt
       << " NLL = " << bestNLL << "\n";



  double fine_xt = best_xt;
  double fine_yt = best_yt;
  double fineNLL = bestNLL;

  const double fineStep = 0.05;
  const int    fineN = 20;   // ±1.0 region

  for (int ix = -fineN; ix <= fineN; ix++) {
    std::cout<<"Fine scan: ix = "<<ix<<std::endl;
    for (int iy = -fineN; iy <= fineN; iy++) {
      
      double xt = best_xt + ix * fineStep;
      double yt = best_yt + iy * fineStep;

      double nll = ComputeNLL(xt, yt);
      if (nll < fineNLL) {
        fineNLL = nll;
        fine_xt = xt;
        fine_yt = yt;
      }
    }
  }

  cout << "\n[Fine scan]\n";
  cout << "xt = " << fine_xt
       << " yt = " << fine_yt
       << " NLL = " << fineNLL << "\n";

  g_manual_xt = fine_xt;
g_manual_yt = fine_yt;
g_manual_valid = true;

cout << "\n[Stored manual minimizer result]\n";
cout << "xt = " << g_manual_xt << "  yt = " << g_manual_yt << endl;

  cout << "\n=== ΔNLL (ultra-fine) ===\n";

  const double ultraStep = 0.005;
  const int    ultraN = 20;

  for (int ix = -ultraN; ix <= ultraN; ix++) {
    
    std::cout<<"Ultra fine scan: ix = "<<ix<<std::endl;
    for (int iy = -ultraN; iy <= ultraN; iy++) {

      double xt = fine_xt + ix * ultraStep;
      double yt = fine_yt + iy * ultraStep;

      double dNLL = ComputeNLL(xt, yt) - fineNLL;

      if (abs(dNLL) < 0.001) {
        cout << "xt = " << xt
             << " yt = " << yt
             << " ΔNLL = " << dNLL << "\n";
      }
    }
  }

  cout << "\nBest point:\n";
  cout << "xt = " << fine_xt << "\n";
  cout << "yt = " << fine_yt << "\n";
  cout << "zt = -1028 (fixed)\n";

  
}


void LoadSimulatedEvents(const std::string &fname)
{
  ifstream fin(fname);
  if (!fin.is_open()) {
    cerr << "ERROR: cannot open " << fname << endl;
    return;
  }

  cout << "Reading simulated events from " << fname << endl;

  // skip header
  std::string line;
  size_t header_skip = 12;
  for (size_t i = 0; i < header_skip && std::getline(fin, line); ++i) {
    cout << "Header: " << line << endl;
  }

  size_t parsed = 0;
  while (std::getline(fin, line)) {

    if (line.find("|") == std::string::npos) continue;
    if (line.find_first_not_of(" \t") == std::string::npos) continue;

    std::stringstream left( line.substr(0, line.find("|")) );
    std::stringstream right( line.substr(line.find("|")+1) );

    double ev, P1_3, P2_3;
    double P3_1,P3_2,P3_3;
    double P4_1,P4_2,P4_3;

    if (!(left >> ev >> P1_3 >> P2_3
	  >> P3_1 >> P3_2 >> P3_3
	  >> P4_1 >> P4_2 >> P4_3))
      continue;

    double Q_factor, alpha_QCD;
    if (!(right >> Q_factor >> alpha_QCD))
      continue;

    TVector3 v1(P3_1, P3_2, P3_3);
    TVector3 v2(P4_1, P4_2, P4_3);

    double zECal = 3536.0;

    double zt = 2508.31;
	
    ClusterPair sp;
    sp.e1 = 1000.0 * std::sqrt(P3_1*P3_1 + P3_2*P3_2 + P3_3*P3_3);
    sp.e2 = 1000.0 * std::sqrt(P4_1*P4_1 + P4_2*P4_2 + P4_3*P4_3);
    sp.x1 = P3_1 / (P3_3 * zECal);
    sp.x2 = P4_1 / (P4_3 * zECal);
    sp.y1 = P3_2 / (P3_3 * zECal);
    sp.y2 = P4_2 / (P4_3 * zECal);
    sp.z1 =  zt;
    sp.z2 =  zt;
	  

    pairs.push_back(sp);
    parsed++;
  }

  fin.close();
  cout << "Loaded " << parsed << " simulated photon pairs." << endl;
}

void FitSimulatedTarget()
{

  LoadSimulatedEvents("events_1.txt");

  auto *min =
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

  ROOT::Math::Functor f(&NLL_Gaussian, 2);
  min->SetFunction(f);

  min->SetLimitedVariable(0, "xt", 0.0, 0.001, -20.0, 20.0);
  min->SetLimitedVariable(1, "yt", 0.0, 0.001, -20.0, 20.0);

  min->Minimize();

  const double *x = min->X();
  double NLL_min = min->MinValue();

  cout << "\n=== Minuit Gaussian NLL fit ===\n";
  cout << "xt = " << x[0] << "\n";
  cout << "yt = " << x[1] << "\n";
  cout << "zt = -1028 (fixed)\n";
  cout << "NLL_min = " << NLL_min << "\n";


  cout << "\n--- Local ΔNLL scan (fine grid) ---\n";

  const double step = 0.001;   // 1 micron–level scan
  const int    Nscan = 10;     // ±10 steps around minimum

  for (int ix = -Nscan; ix <= Nscan; ix++) {
    for (int iy = -Nscan; iy <= Nscan; iy++) {

      double xt = x[0] + ix * step;
      double yt = x[1] + iy * step;

      double par[2] = {xt, yt};
      double nll = NLL_Gaussian(par);
      double dNLL = nll - NLL_min;

      // Print only interesting region
      if (abs(dNLL) < 0.001) {
        cout << "xt = " << xt
             << " yt = " << yt
             << " ΔNLL = " << dNLL << "\n";
      }
    }
  }

  delete min;
}
