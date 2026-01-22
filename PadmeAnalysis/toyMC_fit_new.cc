#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TFile.h"
#include "TRandom3.h"
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
#include <map>
#include <utility>
#include <string>
#include <algorithm>

double targetCalibMean = 432.0;

enum CalibMode { SECTOR_MODE = 0, GRID_MODE = 1 };

const CalibMode calibMode = SECTOR_MODE;

const int NradialSubsectors = 4;
const double rMinCalib = 90.0;
const double rMaxCalib = 270.0;

const int NphiSectors = 8;

const double gridSizeMM = 180.0;

const double gridXmin = -270.0;
const double gridXmax =  270.0;
const double gridYmin = -270.0;
const double gridYmax =  270.0;

const double radius_additional_factor = 0.98;

TLorentzVector ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt, Double_t PosX, Double_t PosY, Double_t PosZ)
{
  Double_t Me = 0.511;
  Double_t BeamMomentum = 432;
  Double_t sqrts = sqrt(2.*Me*Me + 2.*BeamMomentum*Me);
  Double_t betagamma=BeamMomentum/sqrts;
  Double_t gamma = sqrt(betagamma*betagamma+1.);
  Double_t beta = betagamma/gamma;

  TVector3 cluPos;
  cluPos.SetXYZ(PosX,PosY,3536.0);
  TVector3 targetPos;
  targetPos.SetXYZ(xt,yt,zt);
  TVector3 cluMom = cluPos - targetPos;
  cluMom *= (1. / cluMom.Mag());
  TVector3 COGPos;
  COGPos.SetXYZ(0,0,3536.0);
  TVector3 boost = COGPos - targetPos;
  boost*=(beta/boost.Mag());
  Double_t cosa = cluMom.Dot(boost)/(cluMom.Mag()*boost.Mag());

  Double_t Elab = 0.5 * sqrts / sqrt(1. - cosa * cosa + pow(gamma * cosa, 2) - 2. * betagamma * gamma * cosa + pow(betagamma, 2));
  cluMom *= Elab;
  TLorentzVector clu4Mom;
  clu4Mom.SetVectM(cluMom, 0.);
  TLorentzVector clu4MomCM;
  clu4MomCM.SetVectM(clu4Mom.Vect(), 0);
  clu4MomCM.Boost(-boost);
  return clu4MomCM;
}

int ComputeCellIndex(double x, double y) {
  if (calibMode == SECTOR_MODE) {
    double phi = std::atan2(y, x);
    if (phi < 0) phi += 2*M_PI;
    double sectorWidth = 2.0*M_PI / double(NphiSectors);
    int sphi = int(phi / sectorWidth);
    if (sphi < 0) sphi = 0;
    if (sphi >= NphiSectors) sphi = NphiSectors - 1;


    double r = std::sqrt(x*x + y*y);
    double rClamped = std::min(std::max(r, rMinCalib + 1e-9), rMaxCalib - 1e-9);
    int sr = int(std::floor(
			    (rClamped - rMinCalib) /
			    ((rMaxCalib - rMinCalib) / double(NradialSubsectors))
			    ));
    if (sr < 0) sr = 0;
    if (sr >= NradialSubsectors) sr = NradialSubsectors - 1;


    return sphi + NphiSectors * sr;
  
  } else {
    double xc = std::min(std::max(x, gridXmin + 1e-9), gridXmax - 1e-9);
    double yc = std::min(std::max(y, gridYmin + 1e-9), gridYmax - 1e-9);
    int nGridX = std::max(1, int(std::ceil((gridXmax - gridXmin) / gridSizeMM)));
    int nGridY = std::max(1, int(std::ceil((gridYmax - gridYmin) / gridSizeMM)));
    int sx = int(std::floor((xc - gridXmin) / gridSizeMM));
    int sy = int(std::floor((yc - gridYmin) / gridSizeMM));
    if (sx < 0) sx = 0; if (sx >= nGridX) sx = nGridX - 1;
    if (sy < 0) sy = 0; if (sy >= nGridY) sy = nGridY - 1;
    return sx + sy * nGridX;
  }
}

int ComputeSector(double x, double y) { return ComputeCellIndex(x,y); }

struct EventPairCalib {
  double x1, y1, Etrue1;
  double x2, y2, Etrue2;
  double Edec1, Edec2;
  int sector1;
};

struct GlobalFunctorCalib {
  const std::vector<EventPairCalib> *events;
  CalibMode mode;
  int nPhi;
  double gridStep;
  double xMin, xMax, yMin, yMax;
  int nGridX, nGridY;
  int nCells;

  GlobalFunctorCalib(const std::vector<EventPairCalib> *ev,
		     CalibMode m, int nPhiIn, double gridStepIn,
		     double xMinIn, double xMaxIn, double yMinIn, double yMaxIn)
    : events(ev), mode(m), nPhi(nPhiIn), gridStep(gridStepIn),
      xMin(xMinIn), xMax(xMaxIn), yMin(yMinIn), yMax(yMaxIn)
  {
    if (mode == SECTOR_MODE) {
      nCells = std::max(1, nPhi) * NradialSubsectors;
      nGridX = nGridY = 0;

    } else {
      nGridX = std::max(1, int(std::ceil((xMax - xMin) / gridStep)));
      nGridY = std::max(1, int(std::ceil((yMax - yMin) / gridStep)));
      nCells = nGridX * nGridY;
    }
  }

  int GetCellIndex(double x, double y) const {
    return ComputeCellIndex(x, y);
  }

  double operator()(const double *par) const {
    const int Nsec = nCells;
    std::vector<double> sum(Nsec,0.0);
    std::vector<double> sumsq(Nsec,0.0);
    std::vector<int> count(Nsec,0);

    double chi2 = 0.0;
    for (const auto &ev : *events) {
      int c1 = GetCellIndex(ev.x1, ev.y1);
      int c2 = GetCellIndex(ev.x2, ev.y2);

      double base1 =  par[0] + par[1]*ev.Edec1;
      double base2 =  par[0] + par[1]*ev.Edec2;
      double Epred1 = base1 * par[2 + c1] * ev.Edec1;
      double Epred2 = base2 * par[2 + c2] * ev.Edec2;

      // HARD constraint: calibrated energy within ±10% of true
      //if (std::abs(Epred1 - ev.Etrue1) > 0.05 * ev.Etrue1) return 1e12;
      //if (std::abs(Epred2 - ev.Etrue2) > 0.05 * ev.Etrue2) return 1e12;

      double frac1 = (Epred1 - ev.Etrue1) / ev.Etrue1;
      double frac2 = (Epred2 - ev.Etrue2) / ev.Etrue2;
      // double frac3 = (Epred1 + Epred2) / (ev.Etrue1 + ev.Etrue2);

      const double fracMax = 0.1;
      if (std::abs(frac1) > fracMax)
	chi2 += 1e5 * std::pow(std::abs(frac1) - fracMax, 2);

      if (std::abs(frac2) > fracMax)
	chi2 += 1e5 * std::pow(std::abs(frac2) - fracMax, 2);
      // --- soft lower bound on energy sum ---
      const double Smin = 300.0;
      const double lambdaSum = 1e3;  // tune strength

      double S = Epred1 + Epred2;
      double deficit = Smin - S;
      if (deficit > 0.0) {
      	chi2 += lambdaSum * deficit * deficit;
       }




      //double S = Epred1 + Epred2;
      int s = GetCellIndex(ev.x1, ev.y1);
      sum[s] += S;
      sumsq[s] += S*S;
      count[s] += 1;
    }


   
    for (int s=0;s<Nsec;++s) {
      if (count[s] == 0) {
	chi2 += 1e6;
	continue;
      }
      double N = double(count[s]);
      double sum_s = sum[s];
      double sumsq_s = sumsq[s];
      // sum_i (S_i - target)^2 = sumsq - 2*target*sum + N*target^2
      double sumsqdiff = sumsq_s - 2.0 * targetCalibMean * sum_s + N * (targetCalibMean*targetCalibMean);
      double sigma2 = sumsqdiff / N;
      const double minVar = 1e-6;
      if (sigma2 < minVar) sigma2 = minVar;
      chi2 += N * std::log(sigma2) + sumsqdiff / sigma2; // full NLL form (keeps scale)
    }

    chi2 += pow((par[0] - 0.0)/100.0, 2);
    chi2 += pow((par[1] - 1.0)/0.5, 2);
    for (int s=0;s<nCells;++s) chi2 += pow((par[2+s] - 1.0)/0.15, 2);

    return chi2;
  }
};

void toyMC_decalib_and_fit() {
  std::ifstream fin("/padmesf/PADME/2020/events_1.txt");

  std::string line;
  std::vector<double> P1x, P1y, P1z, P2x, P2y, P2z;
  size_t totalLines = 0;
  size_t parsed = 0;

  TH1F *ggX_true = new TH1F("ggX_true","Gamma Gamma X_true",1200,-600.0,600.0);
  TH1F *ggY_true = new TH1F("ggY_true","Gamma Gamma Y_true",1200,-600.0,600.0);
  TH2F *ggXX_true = new TH2F("ggXX_true","Gamma Gamma XX_true",1200,-600.0,600.0,1200,-600.0,600.0);
  TH2F *ggYY_true = new TH2F("ggYY_true","Gamma Gamma YY_true",1200,-600.0,600.0,1200,-600.0,600.0);
  TH2F *ggXY_true = new TH2F("ggXY_true","Gamma Gamma XY_true",1200,-600.0,600.0,1200,-600.0,600.0);
  TH1F *ggTheta_true = new TH1F("ggTheta_true","Gamma Gamma Theta_true",200,-5.0,5.0);
  TH1F *ggSumTheta_true = new TH1F("ggSumTheta_true","Gamma Gamma Sum Theta_true",200,-5.0,5.0);
  TH1F *ggPhi_true = new TH1F("ggPhi_true","Gamma Gamma Phi_true",200,-5.0,5.0);
  TH1F *ggDeltaPhi_true = new TH1F("ggDeltaPhi_true","Gamma Gamma Delta Phi_true",200,-5.0,5.0);
  TH2F *ggSumThetaDeltaPhi_true = new TH2F("ggSumThetaDeltaPhi_true","Gamma Gamma Sum Theta Delta Phi_true",200,2.8,3.4,200,2.8,3.4);

  TProfile *ggEsumX_true = new TProfile("ggEsumX_true","GammaGamma_EnergySum_X_true",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumY_true = new TProfile("ggEsumY_true","GammaGamma_EnergySum_Y_true",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumPhi_true = new TProfile("ggEsumPhi_true","GammaGamma_EnergySum_Phi_true",200,-10.0,10.0,0.0,500.0);
  TProfile *ggEsumE_true = new TProfile("ggEsumE_true","GammaGamma_EnergySum_E_true",500,0.0,500.0,0.0,500.0);

  TH1F *ggX_smear = new TH1F("ggX_smear","Gamma Gamma X_smear",1200,-600.0,600.0);
  TH1F *ggY_smear = new TH1F("ggY_smear","Gamma Gamma Y_smear",1200,-600.0,600.0);
  TH2F *ggXX_smear = new TH2F("ggXX_smear","Gamma Gamma XX_smear",1200,-600.0,600.0,1200,-600.0,600.0);
  TH2F *ggYY_smear = new TH2F("ggYY_smear","Gamma Gamma YY_smear",1200,-600.0,600.0,1200,-600.0,600.0);
  TH2F *ggXY_smear = new TH2F("ggXY_smear","Gamma Gamma XY_smear",1200,-600.0,600.0,1200,-600.0,600.0);
  TH1F *ggTheta_smear = new TH1F("ggTheta_smear","Gamma Gamma Theta_smear",200,-5.0,5.0);
  TH1F *ggSumTheta_smear = new TH1F("ggSumTheta_smear","Gamma Gamma Sum Theta_smear",200,-5.0,5.0);
  TH1F *ggPhi_smear = new TH1F("ggPhi_smear","Gamma Gamma Phi_smear",200,-5.0,5.0);
  TH1F *ggDeltaPhi_smear = new TH1F("ggDeltaPhi_smear","Gamma Gamma Delta Phi_smear",200,-5.0,5.0);
  TH2F *ggSumThetaDeltaPhi_smear = new TH2F("ggSumThetaDeltaPhi_smear","Gamma Gamma Sum Theta Delta Phi_smear",200,2.8,3.4,200,2.8,3.4);

  TProfile *ggEsumX_smear = new TProfile("ggEsumX_smear","GammaGamma_EnergySum_X_smear",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumY_smear = new TProfile("ggEsumY_smear","GammaGamma_EnergySum_Y_smear",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumPhi_smear = new TProfile("ggEsumPhi_smear","GammaGamma_EnergySum_Phi_smear",200,-10.0,10.0,0.0,500.0);
  TProfile *ggEsumE_smear = new TProfile("ggEsumE_smear","GammaGamma_EnergySum_E_smear",500,0.0,500.0,0.0,500.0);

  TProfile *ggEsumX_dec = new TProfile("ggEsumX_dec","GammaGamma_EnergySum_X_dec",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumY_dec = new TProfile("ggEsumY_dec","GammaGamma_EnergySum_Y_dec",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumPhi_dec = new TProfile("ggEsumPhi_dec","GammaGamma_EnergySum_Phi_dec",200,-10.0,10.0,0.0,500.0);
  TProfile *ggEsumE_dec = new TProfile("ggEsumE_dec","GammaGamma_EnergySum_E_dec",500,0.0,500.0,0.0,500.0);

  TProfile *ggEsumX_pred = new TProfile("ggEsumX_pred","GammaGamma_EnergySum_X_pred",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumY_pred = new TProfile("ggEsumY_pred","GammaGamma_EnergySum_Y_pred",1200,-600.0,600.0,0.0,500.0);
  TProfile *ggEsumPhi_pred = new TProfile("ggEsumPhi_pred","GammaGamma_EnergySum_Phi_pred",200,-10.0,10.0,0.0,500.0);
  TProfile *ggEsumE_pred = new TProfile("ggEsumE_pred","GammaGamma_EnergySum_E_pred",500,0.0,500.0,0.0,500.0);

   TH2F *ggEtrueEpred = new TH2F("ggEtrueEpred ","Gamma Gamma True vs. Predicted Energy",500,0.0,500.0,500,0.0,500.0);
   
   TH1F *ggSum_true = new TH1F("ggSum_true","Gamma Gamma True energy sum",500,0.0,500.0);
   TH1F *ggSum_smear = new TH1F("ggSum_smear","Gamma Gamma Smeared energy sum",500,0.0,500.0);
   TH1F *ggSum_decalib = new TH1F("ggSum_decalib","Gamma Gamma Decalibrated energy sum",500,0.0,500.0);
   TH1F *ggSum_calib = new TH1F("ggSum_calib","Gamma Gamma Ccalibrated energy sum",500,0.0,500.0);
 
  TProfile2D *hDecalibMap = new TProfile2D(
					   "hDecalibMap",
					   "Decalibration coefficient",
					   540, gridXmin, gridXmax,
					   540, gridYmin, gridYmax
					   );


  while (std::getline(fin, line)) {
    ++totalLines;
    if (line.find("|") == std::string::npos) continue;
    if (line.find_first_not_of(" \t") == std::string::npos) continue;
    std::stringstream left( line.substr(0, line.find("|")) );
    std::stringstream right( line.substr(line.find("|") + 1) );

    double ev, P1_3, P2_3;
    double P3_1, P3_2, P3_3;
    double P4_1, P4_2, P4_3;

    if (!(left >> ev >> P1_3 >> P2_3
	  >> P3_1 >> P3_2 >> P3_3
	  >> P4_1 >> P4_2 >> P4_3)) {
      continue;
    }

    double Q_factor, alpha_QCD;
    if (!(right >> Q_factor >> alpha_QCD)) {
      continue;
    }

    P1x.push_back(P3_1); P1y.push_back(P3_2); P1z.push_back(P3_3);
    P2x.push_back(P4_1); P2y.push_back(P4_2); P2z.push_back(P4_3);
    ++parsed;
  }
  fin.close();

  std::cout << "Read file. total lines scanned: " << totalLines << ", parsed events: " << parsed << "\n";

  const double zECal = 3536.0;

  const double energyResolutionSigma = 0.05;
  const double posResolutionXY = 0.5;
  TRandom3 rng(98765);

  std::vector<EventPairCalib> events;
  events.reserve(P1x.size());

  std::map<std::pair<int,int>, double> decalibMap;

  for (size_t i=0; i<P1x.size(); ++i) {
    double x1 = P1x[i] / P1z[i] * zECal;
    double y1 = P1y[i] / P1z[i] * zECal;
    double r1 = std::sqrt(x1*x1 + y1*y1);
    double x2 = P2x[i] / P2z[i] * zECal;
    double y2 = P2y[i] / P2z[i] * zECal;
    double r2 = std::sqrt(x2*x2 + y2*y2);
    if (r1 > 90.0 && r1 < 270.0 && r2 > 90.0 && r2 < 270.0) {
      double Etrue1_nom = 1000.0 * std::sqrt(P1x[i]*P1x[i] + P1y[i]*P1y[i] + P1z[i]*P1z[i]);
      double Etrue2_nom = 1000.0 * std::sqrt(P2x[i]*P2x[i] + P2y[i]*P2y[i] + P2z[i]*P2z[i]);

      ggX_true->Fill(x1);      ggX_true ->Fill(x2);
      ggY_true->Fill(y1);      ggX_true ->Fill(y2);
      ggXX_true->Fill(x1,x2);
      ggYY_true->Fill(y1,y2);	    
      ggXY_true->Fill(x1,y1);
      ggXY_true->Fill(x2,y2);

      TLorentzVector p1_true = ComputeCMMomentum(0, 0, 0, x1, y1, zECal);
      TLorentzVector p2_true = ComputeCMMomentum(0, 0, 0, x2, y2, zECal);
      double sumTheta_true = p1_true.Vect().Theta() + p2_true.Vect().Theta();
      double deltaPhi_true =abs( p1_true.Vect().Phi() - p2_true.Vect().Phi());

      ggTheta_true->Fill(p1_true.Vect().Theta());
      ggTheta_true->Fill(p2_true.Vect().Theta());
      ggSumTheta_true->Fill(sumTheta_true);
      ggPhi_true->Fill(p1_true.Vect().Phi());
      ggPhi_true->Fill(p2_true.Vect().Phi());
      ggDeltaPhi_true->Fill(deltaPhi_true);
      ggSumThetaDeltaPhi_true->Fill(sumTheta_true,deltaPhi_true);

      double Sum_true = Etrue1_nom + Etrue2_nom;

      // ggEsumX_true->Fill(x1,Sum_true);
      // ggEsumY_true->Fill(y1,Sum_true);
      // ggEsumPhi_true->Fill(p1_true.Vect().Phi(),Sum_true);
      // ggEsumE_true->Fill(Etrue1_nom,Sum_true);
	    

      double Etrue1 = rng.Gaus(Etrue1_nom, energyResolutionSigma * Etrue1_nom);
      double Etrue2 = rng.Gaus(Etrue2_nom, energyResolutionSigma * Etrue2_nom);
      double xs1 = rng.Gaus(x1, posResolutionXY);
      double ys1 = rng.Gaus(y1, posResolutionXY);
      double xs2 = rng.Gaus(x2, posResolutionXY);
      double ys2 = rng.Gaus(y2, posResolutionXY);

      ggX_smear->Fill(xs1);      ggX_smear ->Fill(xs2);
      ggY_smear->Fill(ys1);      ggX_smear ->Fill(ys2);
      ggXX_smear->Fill(xs1,xs2);
      ggYY_smear->Fill(ys1,ys2);	    
      ggXY_smear->Fill(xs1,ys1);
      ggXY_smear->Fill(xs2,ys2);

      TLorentzVector p1_smear = ComputeCMMomentum(0, 0, 0, xs1, ys1, zECal);
      TLorentzVector p2_smear = ComputeCMMomentum(0, 0, 0, xs2, ys2, zECal);
      double sumTheta_smear = p1_smear.Vect().Theta() + p2_smear.Vect().Theta();
      double deltaPhi_smear =abs( p1_smear.Vect().Phi() - p2_smear.Vect().Phi());

      ggTheta_smear->Fill(p1_smear.Vect().Theta());
      ggTheta_smear->Fill(p2_smear.Vect().Theta());
      ggSumTheta_smear->Fill(sumTheta_smear);
      ggPhi_smear->Fill(p1_smear.Vect().Phi());
      ggPhi_smear->Fill(p2_smear.Vect().Phi());
      ggDeltaPhi_smear->Fill(deltaPhi_smear);
      ggSumThetaDeltaPhi_smear->Fill(sumTheta_smear,deltaPhi_smear);

      double Sum_smear = Etrue1 + Etrue2;

      // ggEsumX_smear->Fill(xs1,Sum_smear);
      // ggEsumY_smear->Fill(ys1,Sum_smear);
      // ggEsumPhi_smear->Fill(p1_smear.Vect().Phi(),Sum_smear);
      // ggEsumE_smear->Fill(Etrue1,Sum_smear);
	    

      int ix1 = int(std::round(xs1));
      int iy1 = int(std::round(ys1));
      int ix2 = int(std::round(xs2));
      int iy2 = int(std::round(ys2));

      std::pair<int,int> key1 = std::make_pair(ix1, iy1);
      std::pair<int,int> key2 = std::make_pair(ix2, iy2);

      if (decalibMap.find(key1) == decalibMap.end()) {
	decalibMap[key1] = rng.Uniform(0.9, 1.0);
      }
      if (decalibMap.find(key2) == decalibMap.end()) {
	decalibMap[key2] = rng.Uniform(0.9, 1.0);
      }

      double m1 = decalibMap[key1];
      double m2 = decalibMap[key2];

      if ( (r1 >= 90.0 && r1 < 120.0) || (r1 > 240.0) ) {
	m1 *= radius_additional_factor;
      }
      if ( (r2 >= 90.0 && r2 < 120.0) || (r2 > 240.0) ) {
	m2 *= radius_additional_factor;
      }

      hDecalibMap->Fill(xs1, ys1, m1);
      hDecalibMap->Fill(xs2, ys2, m2);


      double Edec1 = Etrue1 * m1;
      double Edec2 = Etrue2 * m2;

      double Sum_dec = Edec1 + Edec2;

      // ggEsumX_dec->Fill(xs1,Sum_dec);
      // ggEsumY_dec->Fill(ys1,Sum_dec);
      // ggEsumPhi_dec->Fill(p1_smear.Vect().Phi(),Sum_dec);
      // ggEsumE_dec->Fill(Etrue1,Sum_dec);

      EventPairCalib ev;
      ev.x1 = xs1; ev.y1 = ys1; ev.Etrue1 = Etrue1; ev.Edec1 = Edec1;
      ev.x2 = xs2; ev.y2 = ys2; ev.Etrue2 = Etrue2; ev.Edec2 = Edec2;
      ev.sector1 = ComputeCellIndex(xs1, ys1);
      events.push_back(ev);
    }
  }

  std::cout << "Selected events after r cuts: " << events.size() << "\n";
  std::cout << "Unique decalibration cells: " << decalibMap.size() << "\n";

  int nCells = 0;
  int nPhi = 0;
  int nGridX = 0, nGridY = 0;
  if (calibMode == SECTOR_MODE) {
    nPhi = std::max(1, NphiSectors);
    nCells = nPhi * NradialSubsectors;
    
  } else {
    nGridX = std::max(1, int(std::ceil((gridXmax - gridXmin) / gridSizeMM)));
    nGridY = std::max(1, int(std::ceil((gridYmax - gridYmin) / gridSizeMM)));
    nCells = nGridX * nGridY;
  }

  std::cout << "Calibration mode: " << (calibMode==SECTOR_MODE ? "SECTOR" : "GRID") << "\n";
  std::cout << "Number of calibration cells: " << nCells << "\n";
  if (calibMode == SECTOR_MODE) {
    std::cout << "  Nphi = " << nPhi
          << ", Nradial = " << NradialSubsectors
          << "  (total cells = " << nCells << ")\n";

  } else {
    std::cout << "  Grid X cells = " << nGridX << ", Grid Y cells = " << nGridY
	      << "  (x range [" << gridXmin << "," << gridXmax << "), y range [" << gridYmin << "," << gridYmax << "))\n";
  }

  const int nParams = 2 + nCells;
  std::vector<double> initPars(nParams);
  initPars[0] = 1.0;
  initPars[1] = 0.01;  
  for (int i=0;i<nCells;++i) initPars[2+i] = 1.0;

  GlobalFunctorCalib gf(&events, calibMode, nPhi, gridSizeMM, gridXmin, gridXmax, gridYmin, gridYmax);
  ROOT::Math::Functor f(gf, nParams);

  std::unique_ptr<ROOT::Math::Minimizer> min(
					     ROOT::Math::Factory::CreateMinimizer("Minuit2","Migrad")
					     );

  min->SetMaxFunctionCalls(200000);
  min->SetMaxIterations(20000);
  min->SetTolerance(1e-6);
  min->SetFunction(f);
  min->SetVariable(0, "k0", initPars[0],0.1);
  min->SetFixedVariable(0, "k1", 0.0);
  //min->SetVariable(1, "k1", initPars[1], 0.001);
  for (int i=0;i<nCells;++i) {
    std::string name = std::string("kcell") + std::to_string(i);
    min->SetVariable(2 + i, name.c_str(), initPars[2 + i], 0.01);
  }

  min->Minimize();
  const double *best = min->X();

  std::vector<double> perr;
  int npar_free = min->NFree();
  try {
    const double* errs = min->Errors();
    if (errs != nullptr && npar_free > 0) {
      perr.resize(npar_free);
      for (int i=0;i<npar_free;++i) perr[i] = errs[i];
    } else {
      perr.assign(npar_free, 0.0);
    }
  } catch (...) {
    perr.assign(npar_free, 0.0);
    std::cerr << "Warning: could not retrieve parameter errors via min->Errors(). Errors set to 0.\n";
  }

  std::cout << "Calibration fit results (E_true = (k0 + k1 * E_decalib) * k_cell * E_decalib ):\n";
  std::cout << " k0 = " << best[0] << " ± " << (perr.size()>=1?perr[0]:0.0) << "\n";
  std::cout << " k1 = " << best[1] << " ± " << (perr.size()>=2?perr[1]:0.0) << "\n";
  
  for (int i=0;i<nCells;++i) {
    double err = (perr.size() >= (size_t)(2+i+1)) ? perr[2+i] : (perr.size() > (size_t)(2+i) ? perr[2+i] : 0.0);
    std::cout << " kcell[" << i << "] = " << best[2+i] << " ± " << err << "\n";
  }
  std::cout << "Minimized chi2 = " << min->MinValue() << "\n";
  std::cout << "Estimated edm = " << min->Edm() << "\n";
  std::cout << "Number of function calls: " << min->NCalls() << "\n";


  std::ifstream fin_test("/padmesf/PADME/2020/events_251218_2.txt");

  std::string line_test;
  std::vector<double> P1x_test, P1y_test, P1z_test, P2x_test, P2y_test, P2z_test;
  size_t totalLines_test = 0;
  size_t parsed_test = 0;

  while (std::getline(fin_test, line_test)) {
    ++totalLines_test;
    if (line_test.find("|") == std::string::npos) continue;
    if (line_test.find_first_not_of(" \t") == std::string::npos) continue;
    std::stringstream left_test( line_test.substr(0, line_test.find("|")) );
    std::stringstream right_test( line_test.substr(line_test.find("|") + 1) );

    double ev_test, P1_3_test, P2_3_test;
    double P3_1_test, P3_2_test, P3_3_test;
    double P4_1_test, P4_2_test, P4_3_test;

    if (!(left_test >> ev_test >> P1_3_test >> P2_3_test
	  >> P3_1_test >> P3_2_test >> P3_3_test
	  >> P4_1_test >> P4_2_test >> P4_3_test)) {
      continue;
    }

    double Q_factor_test, alpha_QCD_test;
    if (!(right_test >> Q_factor_test >> alpha_QCD_test)) {
      continue;
    }

    P1x_test.push_back(P3_1_test); P1y_test.push_back(P3_2_test); P1z_test.push_back(P3_3_test);
    P2x_test.push_back(P4_1_test); P2y_test.push_back(P4_2_test); P2z_test.push_back(P4_3_test);
    ++parsed_test;
  }
  fin_test.close();

  std::cout << "Read test file. total lines scanned: " << totalLines_test << ", parsed events: " << parsed_test << "\n";

   std::vector<EventPairCalib> events_test;
  events_test.reserve(P1x_test.size());

  for (size_t i=0; i<P1x_test.size(); ++i) {
    
    double x1_test = P1x_test[i] / P1z_test[i] * zECal;
    double y1_test = P1y_test[i] / P1z_test[i] * zECal;
    double r1_test = std::sqrt(x1_test*x1_test + y1_test*y1_test);
    double x2_test = P2x_test[i] / P2z_test[i] * zECal;
    double y2_test = P2y_test[i] / P2z_test[i] * zECal;
    double r2_test = std::sqrt(x2_test*x2_test + y2_test*y2_test);
    if (r1_test > 90.0 && r1_test < 270.0 && r2_test > 90.0 && r2_test < 270.0) {
      double Etrue1_nom_test = 1000.0 * std::sqrt(P1x_test[i]*P1x_test[i] + P1y_test[i]*P1y_test[i] + P1z_test[i]*P1z_test[i]);
      double Etrue2_nom_test = 1000.0 * std::sqrt(P2x_test[i]*P2x_test[i] + P2y_test[i]*P2y_test[i] + P2z_test[i]*P2z_test[i]);

      TLorentzVector p1_true_test = ComputeCMMomentum(0, 0, 0, x1_test, y1_test, zECal);
      TLorentzVector p2_true_test = ComputeCMMomentum(0, 0, 0, x2_test, y2_test, zECal);
      double sumTheta_true_test = p1_true_test.Vect().Theta() + p2_true_test.Vect().Theta();
      double deltaPhi_true_test =abs( p1_true_test.Vect().Phi() - p2_true_test.Vect().Phi());

      double Sum_true_test = Etrue1_nom_test + Etrue2_nom_test;
      
      ggEsumX_true->Fill(x1_test,Sum_true_test);
      ggEsumY_true->Fill(y1_test,Sum_true_test);
      ggEsumPhi_true->Fill(p1_true_test.Vect().Phi(),Sum_true_test);
      ggEsumE_true->Fill(Etrue1_nom_test,Sum_true_test);
      
      double Etrue1_test = rng.Gaus(Etrue1_nom_test, energyResolutionSigma * Etrue1_nom_test);
      double Etrue2_test = rng.Gaus(Etrue2_nom_test, energyResolutionSigma * Etrue2_nom_test);
      double xs1_test = rng.Gaus(x1_test, posResolutionXY);
      double ys1_test = rng.Gaus(y1_test, posResolutionXY);
      double xs2_test = rng.Gaus(x2_test, posResolutionXY);
      double ys2_test = rng.Gaus(y2_test, posResolutionXY);

      TLorentzVector p1_smear_test = ComputeCMMomentum(0, 0, 0, xs1_test, ys1_test, zECal);
      TLorentzVector p2_smear_test = ComputeCMMomentum(0, 0, 0, xs2_test, ys2_test, zECal);
      double sumTheta_smear_test = p1_smear_test.Vect().Theta() + p2_smear_test.Vect().Theta();
      double deltaPhi_smear_test =abs( p1_smear_test.Vect().Phi() - p2_smear_test.Vect().Phi());
      
      double Sum_smear_test = Etrue1_test + Etrue2_test;
      ggEsumX_smear->Fill(xs1_test,Sum_smear_test);
      ggEsumY_smear->Fill(ys1_test,Sum_smear_test);
      ggEsumPhi_smear->Fill(p1_smear_test.Vect().Phi(),Sum_smear_test);
      ggEsumE_smear->Fill(Etrue1_test,Sum_smear_test);
	     
      int ix1_test = int(std::round(xs1_test));
      int iy1_test = int(std::round(ys1_test));
      int ix2_test = int(std::round(xs2_test));
      int iy2_test = int(std::round(ys2_test));

      std::pair<int,int> key1_test = std::make_pair(ix1_test, iy1_test);
      std::pair<int,int> key2_test = std::make_pair(ix2_test, iy2_test);

      if (decalibMap.find(key1_test) == decalibMap.end()) {
	decalibMap[key1_test] = rng.Uniform(0.9, 1.0);
      }
      if (decalibMap.find(key2_test) == decalibMap.end()) {
	decalibMap[key2_test] = rng.Uniform(0.9, 1.0);
      }

      double m1_test = decalibMap[key1_test];
      double m2_test = decalibMap[key2_test];

      if ( (r1_test >= 90.0 && r1_test < 120.0) || (r1_test > 240.0) ) {
	m1_test *= radius_additional_factor;
      }
      if ( (r2_test >= 90.0 && r2_test < 120.0) || (r2_test > 240.0) ) {
	m2_test *= radius_additional_factor;
      }

      double Edec1_test = Etrue1_test * m1_test;
      double Edec2_test = Etrue2_test * m2_test;

      double Sum_dec_test = Edec1_test + Edec2_test;

      ggEsumX_dec->Fill(xs1_test,Sum_dec_test);
      ggEsumY_dec->Fill(ys1_test,Sum_dec_test);
      ggEsumPhi_dec->Fill(p1_smear_test.Vect().Phi(),Sum_dec_test);
      ggEsumE_dec->Fill(Etrue1_test,Sum_dec_test);

      EventPairCalib ev_test;
      ev_test.x1 = xs1_test; ev_test.y1 = ys1_test; ev_test.Etrue1 = Etrue1_test; ev_test.Edec1 = Edec1_test;
      ev_test.x2 = xs2_test; ev_test.y2 = ys2_test; ev_test.Etrue2 = Etrue2_test; ev_test.Edec2 = Edec2_test;
      ev_test.sector1 = ComputeCellIndex(xs1_test, ys1_test);
      events_test.push_back(ev_test);
    }
  }

  std::cout << "Selected test events after r cuts: " << events_test.size() << "\n";
 

  {
    std::vector<double> sumPred(nCells, 0.0), sumPred2(nCells, 0.0);
    std::vector<double> sumTrue(nCells, 0.0);
    std::vector<int> count(nCells, 0);

    std::vector<double> sumResid(nCells, 0.0), sumFracResid(nCells, 0.0), sumFracResid2(nCells, 0.0);

    double globalSumSqErr = 0.0;
    double globalSumErr = 0.0;
    long long globalN = 0;

    for (const auto &ev : events_test) {
      int c1 = ComputeCellIndex(ev.x1, ev.y1);
      int c2 = ComputeCellIndex(ev.x2, ev.y2);

      double base1 = best[0] + best[1]*ev.Edec1;
      double base2 = best[0] + best[1]*ev.Edec2;
      double Epred1 = base1 * best[2 + c1] * ev.Edec1;
      double Epred2 = base2 * best[2 + c2] * ev.Edec2;

      double Spred = Epred1 + Epred2;
      double Strue = ev.Etrue1 + ev.Etrue2;

      TLorentzVector p1_pred = ComputeCMMomentum(0, 0, 0, ev.x1, ev.y1, zECal);

      ggEsumX_pred->Fill(ev.x1,Spred);
      ggEsumY_pred->Fill(ev.y1,Spred);
      ggEsumPhi_pred->Fill(p1_pred.Vect().Phi(),Spred);
      ggEsumE_pred->Fill(Epred1,Spred);

      ggEtrueEpred->Fill(ev.Etrue1,Epred1);
      ggEtrueEpred->Fill(ev.Etrue2,Epred2);
      
      ggSum_true->Fill(Strue);
      ggSum_decalib->Fill(ev.Edec1+ev.Edec2);
      ggSum_calib->Fill(Spred);
	  
      int s = c1;
      if (s < 0) s = 0;
      if (s >= nCells) s = nCells-1;

      sumPred[s] += Spred;
      sumPred2[s] += Spred*Spred;
      sumTrue[s] += Strue;
      count[s] += 1;

      double resid1 = Epred1 - ev.Etrue1;
      double resid2 = Epred2 - ev.Etrue2;
      double frac1 = (ev.Etrue1 != 0.0) ? resid1 / ev.Etrue1 : 0.0;
      double frac2 = (ev.Etrue2 != 0.0) ? resid2 / ev.Etrue2 : 0.0;

      sumResid[s] += resid1 + resid2;
      sumFracResid[s] += frac1 + frac2;
      sumFracResid2[s] += frac1*frac1 + frac2*frac2;

      globalSumSqErr += resid1*resid1 + resid2*resid2;
      globalSumErr += resid1 + resid2;
      globalN += 2;
    }

    std::cout << "\nPer-cell (or per-sector) calibration performance:\n";
    std::cout << " cell  count   mean_pred_sum   mean_true_sum   mean_resid_per_event   mean_frac_resid   frac_rms\n";
    for (int c=0;c<nCells;++c) {
      if (count[c] == 0) {
	std::cout << " " << c << "    0\n";
	continue;
      }
      double meanPred = sumPred[c] / double(count[c]);
      double meanTrue = sumTrue[c] / double(count[c]);
      double meanResidEvent = (sumPred[c] - sumTrue[c]) / double(count[c]);
      double perPhotonMeanFrac = sumFracResid[c] / double(count[c]*2.0);
      double perPhotonMeanFrac2 = sumFracResid2[c] / double(count[c]*2.0);
      double perPhotonRMS = std::sqrt( std::max(0.0, perPhotonMeanFrac2 - perPhotonMeanFrac*perPhotonMeanFrac) );

      std::cout << " " << c
		<< "     " << count[c]
		<< "     " << meanPred
		<< "         " << meanTrue
		<< "         " << meanResidEvent
		<< "               " << perPhotonMeanFrac
		<< "          " << perPhotonRMS
		<< "\n";
    }

    double globalBias = (globalN>0) ? globalSumErr / double(globalN) : 0.0;
    double globalRMSE = (globalN>0) ? std::sqrt(globalSumSqErr / double(globalN)) : 0.0;

    std::cout << "\nGlobal metrics (per photon):\n";
    std::cout << "  Global bias (mean residual) = " << globalBias << " MeV\n";
    std::cout << "  Global RMSE (sqrt(mean residual^2)) = " << globalRMSE << " MeV\n";
    std::cout << "  Number of photons used = " << globalN << "\n";
  }

  new TCanvas("ggX_true"); ggX_true->Draw();
  new TCanvas("ggY_true"); ggY_true->Draw();
  new TCanvas("ggXX_true"); ggXX_true->Draw("COLZ");
  new TCanvas("ggYY_true"); ggYY_true->Draw("COLZ");
  new TCanvas("ggXY_true"); ggXY_true->Draw("COLZ");
  new TCanvas("ggTheta_true"); ggTheta_true->Draw();
  new TCanvas("ggSumTheta_true"); ggSumTheta_true->Draw();
  new TCanvas("ggPhi_true"); ggPhi_true->Draw();
  new TCanvas("ggDeltaPhi_true"); ggDeltaPhi_true->Draw();
  new TCanvas("ggSumThetaDeltaPhi_true"); ggSumThetaDeltaPhi_true->Draw("COLZ");
    
  new TCanvas("ggEsumX_true"); ggEsumX_true->Draw();
  new TCanvas("ggEsumY_true"); ggEsumY_true->Draw();
  new TCanvas("ggEsumPhi_true"); ggEsumPhi_true->Draw();
  new TCanvas("ggEsumE_true"); ggEsumE_true->Draw();

  new TCanvas("ggX_smear"); ggX_smear->Draw();
  new TCanvas("ggY_smear"); ggY_smear->Draw();
  new TCanvas("ggXX_smear"); ggXX_smear->Draw("COLZ");
  new TCanvas("ggYY_smear"); ggYY_smear->Draw("COLZ");
  new TCanvas("ggXY_smear"); ggXY_smear->Draw("COLZ");
  new TCanvas("ggTheta_smear"); ggTheta_smear->Draw();
  new TCanvas("ggSumTheta_smear"); ggSumTheta_smear->Draw();
  new TCanvas("ggPhi_smear"); ggPhi_smear->Draw();
  new TCanvas("ggDeltaPhi_smear"); ggDeltaPhi_smear->Draw();
  new TCanvas("ggSumThetaDeltaPhi_smear"); ggSumThetaDeltaPhi_smear->Draw("COLZ");
    
  new TCanvas("ggEsumX_smear"); ggEsumX_smear->Draw();
  new TCanvas("ggEsumY_smear"); ggEsumY_smear->Draw();
  new TCanvas("ggEsumPhi_smear"); ggEsumPhi_smear->Draw();
  new TCanvas("ggEsumE_smear"); ggEsumE_smear->Draw();
    
  new TCanvas("ggEsumX_dec"); ggEsumX_dec->Draw();
  new TCanvas("ggEsumY_dec"); ggEsumY_dec->Draw();
  new TCanvas("ggEsumPhi_dec"); ggEsumPhi_dec->Draw();
  new TCanvas("ggEsumE_dec"); ggEsumE_dec->Draw();
    
  new TCanvas("ggEsumX_pred"); ggEsumX_pred->Draw();
  new TCanvas("ggEsumY_pred"); ggEsumY_pred->Draw();
  new TCanvas("ggEsumPhi_pred"); ggEsumPhi_pred->Draw();
  new TCanvas("ggEsumE_pred"); ggEsumE_pred->Draw();

  ggEsumX_smear->SetLineColor(kRed);
  ggEsumY_smear->SetLineColor(kRed);
  ggEsumPhi_smear->SetLineColor(kRed);
  ggEsumE_smear->SetLineColor(kRed);

  ggEsumX_dec->SetLineColor(kGreen);
  ggEsumY_dec->SetLineColor(kGreen);
  ggEsumPhi_dec->SetLineColor(kGreen);
  ggEsumE_dec->SetLineColor(kGreen);

  ggEsumX_pred->SetLineColor(kMagenta);
  ggEsumY_pred->SetLineColor(kMagenta);
  ggEsumPhi_pred->SetLineColor(kMagenta);
  ggEsumE_pred->SetLineColor(kMagenta);
    
  new TCanvas("ggEsumX_all");
  ggEsumX_true->Draw();
  ggEsumX_smear->Draw("same");
  ggEsumX_dec->Draw("same");
  ggEsumX_pred->Draw("same");
    
  new TCanvas("ggEsumY_all");
  ggEsumY_true->Draw();
  ggEsumY_smear->Draw("same");
  ggEsumY_dec->Draw("same");
  ggEsumY_pred->Draw("same");
    
  new TCanvas("ggEsumPhi_all");
  ggEsumPhi_true->Draw();
  ggEsumPhi_smear->Draw("same");
  ggEsumPhi_dec->Draw("same");
  ggEsumPhi_pred->Draw("same");
    
  new TCanvas("ggEsumE_all");
  ggEsumE_true->Draw();
  ggEsumE_smear->Draw("same");
  ggEsumE_dec->Draw("same");
  ggEsumE_pred->Draw("same");

  new TCanvas("cDecalibMap");
  hDecalibMap->SetMinimum(0.9);
  hDecalibMap->SetMaximum(1.0);
  hDecalibMap->Draw("COLZ");

  new TCanvas("ggEtrueEpred");
  ggEtrueEpred->Draw("COLZ");

  
  new TCanvas("ggEsum_all");
  ggSum_true->Draw();
  //ggEsumE_smear->Draw("same");
  ggSum_decalib->Draw("same");
  ggSum_calib->Draw("same");

}
