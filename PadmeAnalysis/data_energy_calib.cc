#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TFile.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <algorithm>

double targetCalibMean = 431.4;

enum CalibMode { SECTOR_MODE = 0, GRID_MODE = 1 };
const CalibMode calibMode = SECTOR_MODE;

const int NradialSubsectors = 3;
const double rMinCalib = 90.0;
const double rMaxCalib = 270.0;
const int NphiSectors = 6;

const double gridSizeMM = 180.0;
const double gridXmin = -270.0;
const double gridXmax =  270.0;
const double gridYmin = -270.0;
const double gridYmax =  270.0;

int ComputeCellIndex(double x, double y)
{
  if (calibMode == SECTOR_MODE) {
    double phi = atan2(y,x);
    if (phi < 0) phi += 2*M_PI;
    int sphi = int(phi / (2*M_PI/NphiSectors));
    sphi = std::clamp(sphi,0,NphiSectors-1);

    double r = sqrt(x*x+y*y);
    double rC = std::clamp(r,rMinCalib+1e-9,rMaxCalib-1e-9);
    int sr = int((rC-rMinCalib)/((rMaxCalib-rMinCalib)/NradialSubsectors));
    sr = std::clamp(sr,0,NradialSubsectors-1);

    return sphi + NphiSectors*sr;
  }

  int nx = int(ceil((gridXmax-gridXmin)/gridSizeMM));
  int ny = int(ceil((gridYmax-gridYmin)/gridSizeMM));
  int ix = int((x-gridXmin)/gridSizeMM);
  int iy = int((y-gridYmin)/gridSizeMM);
  ix = std::clamp(ix,0,nx-1);
  iy = std::clamp(iy,0,ny-1);
  return ix + iy*nx;
}

struct EventPair {
  double x1,y1,E1;
  double x2,y2,E2;
};

struct GlobalFunctor {
  const std::vector<EventPair>* evs;
  int nCells;

  GlobalFunctor(const std::vector<EventPair>* e,int nc)
    : evs(e), nCells(nc) {}

  double operator()(const double* par) const
  {
    std::vector<double> sum(nCells,0), sumsq(nCells,0);
    std::vector<int> cnt(nCells,0);

    double chi2 = 0;

    for (auto& ev : *evs) {
      int c1 = ComputeCellIndex(ev.x1,ev.y1);
      int c2 = ComputeCellIndex(ev.x2,ev.y2);

      double Epred1 = (par[0] + par[1]*ev.E1) * par[2+c1] * ev.E1;
      double Epred2 = (par[0] + par[1]*ev.E2) * par[2+c2] * ev.E2;

      double frac1 = (Epred1-ev.E1)/ev.E1;
      double frac2 = (Epred2-ev.E2)/ev.E2;

      const double fmax = 0.1;
      if (fabs(frac1)>fmax) chi2 += 1e5*pow(fabs(frac1)-fmax,2);
      if (fabs(frac2)>fmax) chi2 += 1e5*pow(fabs(frac2)-fmax,2);

      double S = Epred1+Epred2;
      if (S<300) chi2 += 1e3*pow(300-S,2);

      int s = c1;
      sum[s]+=S;
      sumsq[s]+=S*S;
      cnt[s]++;
    }

    for (int c=0;c<nCells;c++) {
      if (!cnt[c]) { chi2+=1e6; continue; }
      double N = cnt[c];
      double mu = targetCalibMean;
      double ss = sumsq[c]-2*mu*sum[c]+N*mu*mu;
      double var = std::max(ss/N,1e-6);
      chi2 += N*log(var) + ss/var;
    }

    chi2 += pow(par[0]/100,2);
    chi2 += pow((par[1]-1)/0.5,2);
    for (int c=0;c<nCells;c++) chi2+=pow((par[2+c]-1)/0.15,2);

    return chi2;
  }
};

void data_calib_and_fit()
{

  TH1F *ggSum_true = new TH1F("ggSum_true","Gamma Gamma True energy sum",500,0.0,500.0);
  TH1F *ggSum_calib = new TH1F("ggSum_calib","Gamma Gamma Ccalibrated energy sum",500,0.0,500.0);
 
  std::ifstream fin("gamma_gamma_allcoord.txt");
  
  std::vector<EventPair> events;
  double E1,x1,y1,z1,E2,x2,y2,z2;

  while (fin>>E1>>x1>>y1>>z1>>E2>>x2>>y2>>z2) {
    double r1=sqrt(x1*x1+y1*y1);
    double r2=sqrt(x2*x2+y2*y2);
    if (r1<rMinCalib||r1>rMaxCalib) continue;
    if (r2<rMinCalib||r2>rMaxCalib) continue;
    events.push_back({x1,y1,E1,x2,y2,E2});
  }

  int nCells = (calibMode==SECTOR_MODE)
    ? NphiSectors*NradialSubsectors
    : int(ceil((gridXmax-gridXmin)/gridSizeMM))*int(ceil((gridYmax-gridYmin)/gridSizeMM));

  GlobalFunctor gf(&events,nCells);
  ROOT::Math::Functor f(gf,2+nCells);

  auto min = std::unique_ptr<ROOT::Math::Minimizer>(
    ROOT::Math::Factory::CreateMinimizer("Minuit2","Migrad"));

  min->SetFunction(f);
  min->SetVariable(0,"k0",1.0,0.1);
  //min->SetVariable(1,"k1",0.01,0.001);
  min->SetFixedVariable(0, "k1", 0.0);
  for (int c=0;c<nCells;c++)
    min->SetVariable(2+c,Form("kcell%d",c),1.0,0.01);

  min->Minimize();
  const double* best=min->X();

  std::cout<<"\nCalibration results:\n";
  std::cout<<"k0 = "<<best[0]<<"\n";
  std::cout<<"k1 = "<<best[1]<<"\n";
  for (int c=0;c<nCells;c++)
    std::cout<<"kcell["<<c<<"] = "<<best[2+c]<<"\n";

  std::vector<double> sumPred(nCells,0), sumTrue(nCells,0);
  std::vector<int> count(nCells,0);

  double globalBias=0, globalRMSE=0;
  long long N=0;

  for (const auto& ev : events) {

    int c1 = ComputeCellIndex(ev.x1,ev.y1);
    int c2 = ComputeCellIndex(ev.x2,ev.y2);

    double Epred1 = (best[0]+best[1]*ev.E1)*best[2+c1]*ev.E1;
    double Epred2 = (best[0]+best[1]*ev.E2)*best[2+c2]*ev.E2;

    double Strue = ev.E1 + ev.E2;
    double Spred = Epred1 + Epred2;

    ggSum_true->Fill(Strue);
    ggSum_calib->Fill(Spred);

    int s = c1;
    sumPred[s] += Spred;
    sumTrue[s] += Strue;
    count[s]++;

    globalBias += (Epred1-ev.E1)+(Epred2-ev.E2);
    globalRMSE += std::pow(Epred1-ev.E1,2)+std::pow(Epred2-ev.E2,2);
    N += 2;
  }

  std::cout << "\nPer-cell results:\n";
  for (int c=0;c<nCells;++c) {
    if (count[c]==0) continue;
    std::cout << " cell " << c
              << "  <Spred>=" << sumPred[c]/count[c]
              << "  <Strue>=" << sumTrue[c]/count[c]
              << "  bias/event=" << (sumPred[c]-sumTrue[c])/count[c]
              << "\n";
  }

  std::cout << "\nGlobal metrics:\n";
  std::cout << " Bias/photon = " << globalBias/N << " MeV\n";
  std::cout << " RMSE/photon = " << std::sqrt(globalRMSE/N) << " MeV\n";

  new TCanvas("ggEsum_all");
  ggSum_true->Draw();
  //ggEsumE_smear->Draw("same");
  //ggSum_decalib->Draw("same");
  ggSum_calib->Draw("same");
}
