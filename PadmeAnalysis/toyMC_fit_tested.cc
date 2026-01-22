// toyMC_with_fit_updated.cpp
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TFile.h"
#include "TRandom3.h"

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

double targetMean[6] = {
    423.847, // sector 0
    423.061, // sector 1
    423.417, // sector 2
    423.734, // sector 3
    422.436, // sector 4
    421.502  // sector 5
};

TLorentzVector ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt,
                                  Double_t PosX, Double_t PosY, Double_t PosZ)
{
  Double_t fMe = 0.511;
  Double_t fBeamMomentum = 428.4;
  Double_t sqrts = sqrt(2.*fMe*fMe + 2.*fBeamMomentum*fMe);
  Double_t betagamma=fBeamMomentum/sqrts;
  Double_t gamma = sqrt(betagamma*betagamma+1.);
  Double_t beta = betagamma/gamma;

  TVector3 cluPos;
  cluPos.SetXYZ(PosX,PosY,2508.31);
  TVector3 targetPos;
  //targetPos.SetXYZ(0,0,-1028);
  targetPos.SetXYZ(xt,yt,zt);
  TVector3 cluMom = cluPos - targetPos;
  cluMom *= (1. / cluMom.Mag());
  TVector3 COGPos;
  COGPos.SetXYZ(0,0,2508.31);
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

int ComputeSector(double x, double y) {
    double phi = std::atan2(y, x);
    if (phi < 0) phi += 2*M_PI;
    int sector = int( phi / (M_PI/3.0) );
    if (sector < 0) sector = 0;
    if (sector > 5) sector = 5;
    return sector;
}

double RegisteredEnergy_param(double Etrue, double x, double y, const double *params) {
    double k0 = params[0];
    double k1 = params[1];
    double kE = k0 + k1 * Etrue;
    double phi = std::atan2(y, x);
    if (phi < 0) phi += 2*M_PI;
    int sector = int( phi / (M_PI/3.0) );
    if (sector < 0) sector = 0;
    if (sector > 5) sector = 5;
    double kSector = params[2 + sector];

    return kE * kSector * Etrue;
}


// Solve inverse of registered energy: given E_reg and sector multiplier kSec and k0,k1,
// return estimated E_true by solving k1*kSec*E^2 + k0*kSec*E - E_reg = 0
double InvertRegisteredEnergy(double Ereg, double k0, double k1, double kSec) {
    const double a = k1 * kSec;
    const double b = k0 * kSec;
    const double c = -Ereg;

    if (std::abs(a) < 1e-16) {
        // fall back to linear inversion E = Ereg / (b)
        if (std::abs(b) < 1e-16) return 0.0;
        return Ereg / b;
    }

    double disc = b*b - 4.0*a*c;
    if (disc < 0) disc = 0;
    double root = (-b + std::sqrt(disc)) / (2.0*a); // positive root expected
    if (root < 0) {
        // try the other root
        double root2 = (-b - std::sqrt(disc)) / (2.0*a);
        if (root2 > 0) return root2;
        return std::abs(root); // fallback, shouldn't happen normally
    }
    return root;
}


struct EventPair {
    double x1, y1, E1;
    double x2, y2, E2;
    int sector1;       
};


double ComputeChi2ForParams(const std::vector<EventPair>& events, const double *params)
{
  
    const int Nsec = 6;
    std::vector<double> sum(Nsec, 0.0);
    std::vector<double> sumsq(Nsec, 0.0);
    std::vector<int> count(Nsec, 0);

    for (const auto &ev : events) {

        auto regE = [&](double E, double x, double y)
        {
            double k0 = params[0];
            double k1 = params[1];

            double base = k0 + k1 * E;
            double phi = atan2(y, x);
            if (phi < 0) phi += 2*M_PI;
            int s = int(phi / (M_PI/3.0));
            if (s < 0) s = 0;
            if (s > 5) s = 5;
            double kSec = params[2 + s];

	    return base * kSec * E;
	};

        double S = regE(ev.E1, ev.x1, ev.y1) +
                   regE(ev.E2, ev.x2, ev.y2);

        int s = ev.sector1;
        sum[s]    += S;
        sumsq[s]  += S*S;
        count[s]  += 1;
    }

    double chi2 = 0.0;

    for (int s=0; s<6; ++s) {
        if (count[s] == 0) {
            chi2 += 1e6;
            continue;
        }

        double mean = sum[s] / count[s];
        double var = (count[s] > 1)
                   ? (sumsq[s] - sum[s]*sum[s]/count[s])/(count[s] - 1)
                   : 0.0;

        double sigma = (var > 0.0) ? sqrt(var)/sqrt(count[s]) : 10.0;

        double diff = mean - targetMean[s];
        chi2 += (diff * diff) / (sigma * sigma);
    }

    chi2 += pow((params[0] - 1.0)/0.3, 2);
    chi2 += pow(params[1]/0.001, 2);

    for (int s=0; s<6; ++s)
        chi2 += pow((params[2+s] - 1.0)/0.15, 2);

    return chi2;
}

struct GlobalFunctor {
    const std::vector<EventPair> *events;
    GlobalFunctor(const std::vector<EventPair> *ev): events(ev) {}
    double operator()(const double *par) const {
        return ComputeChi2ForParams(*events, par);
    }
};

void toyMC_with_fit() {
   
    std::ifstream fin("events_1.txt");

    std::string line;
    size_t header_skip = 12;
    std::vector<double> P1x, P1y, P1z, P2x, P2y, P2z;

    size_t totalLines = 0;
    size_t parsed = 0;

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
            std::cout << "Could not parse left block: " << line << std::endl;
            continue;
        }

        double Q_factor, alpha_QCD;
        if (!(right >> Q_factor >> alpha_QCD)) {
            std::cout << "Could not parse right block: " << line << std::endl;
            continue;
        }

        P1x.push_back(P3_1); P1y.push_back(P3_2); P1z.push_back(P3_3);
        P2x.push_back(P4_1); P2y.push_back(P4_2); P2z.push_back(P4_3);
        ++parsed;
    }
    fin.close();

    std::cout << "Read file. total lines scanned: " << totalLines << ", parsed events: " << parsed << "\n";

    const double zECal = 3536.0;
   
    std::vector<EventPair> events;
    events.reserve(P1x.size());

    // --- RESOLUTION SETTINGS (user-changeable) ---
    const double energyResolutionSigma = 0.02; // relative sigma (2%) for energy smear
    const double posResolutionXY = 0.5; // absolute sigma in mm for x,y smear
    TRandom3 rng(12345);

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

            // Apply gaussian smearing to energy (relative) and position (absolute)
            double Etrue1 = rng.Gaus(Etrue1_nom, energyResolutionSigma * Etrue1_nom);
            double Etrue2 = rng.Gaus(Etrue2_nom, energyResolutionSigma * Etrue2_nom);
            double xs1 = rng.Gaus(x1, posResolutionXY);
            double ys1 = rng.Gaus(y1, posResolutionXY);
            double xs2 = rng.Gaus(x2, posResolutionXY);
            double ys2 = rng.Gaus(y2, posResolutionXY);

            EventPair ev;
            ev.x1 = xs1; ev.y1 = ys1; ev.E1 = Etrue1;
            ev.x2 = xs2; ev.y2 = ys2; ev.E2 = Etrue2;
            ev.sector1 = ComputeSector(xs1, ys1);
            events.push_back(ev);
        }
    }
    std::cout << "Selected events after r cuts: " << events.size() << "\n";

    // k0 ~ 0.95, k1 ~ 0.0005, sector corrections ~ 1.0
    double initParams[8];
    initParams[0] = 0.95;   // k0
    initParams[1] = 0.0005; // k1
    for (int i=0;i<6;++i) initParams[2+i] = 1.0;

    std::unique_ptr<ROOT::Math::Minimizer> min(
        ROOT::Math::Factory::CreateMinimizer("Minuit2","Migrad")
    );

    min->SetMaxFunctionCalls(100000);
    min->SetMaxIterations(10000);
    min->SetTolerance(1e-6);

    GlobalFunctor gf(&events);
    ROOT::Math::Functor f(gf, 8);
    min->SetFunction(f);

    min->SetVariable(0, "k0", initParams[0], 1e-4);
    min->SetVariable(1, "k1", initParams[1], 1e-7);
    for (int s=0; s<6; ++s) {
        std::string name = "sec" + std::to_string(s);
        min->SetVariable(2 + s, name.c_str(), initParams[2 + s], 1e-3);
    }

    min->Minimize();
    const double *best = min->X();

    // Attempt to get errors from minimizer
    std::vector<double> perr;
    int npar = min->NFree();

    try {
      const double* errs = min->Errors();  // pointer to array of size npar

      perr.resize(npar);
      for (int i = 0; i < npar; ++i) {
	perr[i] = errs[i];
      }

    } catch (...) {
      perr.assign(8, 0.0);
        std::cerr << "Warning: could not retrieve parameter errors via min->Errors(). Errors set to 0.\n";
    }

    std::cout << "Best-fit parameters:\n";
    std::cout << " k0 = " << best[0] << "  ± " << (perr.size()>=1?perr[0]:0.0) << "\n";
    std::cout << " k1 = " << best[1] << "  ± " << (perr.size()>=2?perr[1]:0.0) << "\n";
    for (int s=0; s<6; ++s) {
        double err = (perr.size() >= (size_t)(2+s+1)) ? perr[2+s] : 0.0;
        std::cout << " sec[" << s << "] = " << best[2+s] << "  ± " << err << "\n";
    }
    std::cout << "Minimized chi2 = " << min->MinValue() << "\n";
    std::cout << "Estimated edm = " << min->Edm() << "\n";
    std::cout << "Number of function calls: " << min->NCalls() << "\n";

    // --- Compute reversed coefficients and evaluate recovery performance ---
    {
        std::vector<double> sum(6,0.0), sumsq(6,0.0);
        std::vector<int> count(6,0);
        for (const auto &ev : events) {
            double Sreg = RegisteredEnergy_param(ev.E1, ev.x1, ev.y1, best) +
                          RegisteredEnergy_param(ev.E2, ev.x2, ev.y2, best);

            // Now invert registered energies back to estimated true energies
            // per photon:
            double phi1 = atan2(ev.y1, ev.x1);
            if (phi1 < 0) phi1 += 2*M_PI;
            int s1 = int(phi1 / (M_PI/3.0));
            if (s1<0) s1=0; if (s1>5) s1=5;

            double phi2 = atan2(ev.y2, ev.x2);
            if (phi2 < 0) phi2 += 2*M_PI;
            int s2 = int(phi2 / (M_PI/3.0));
            if (s2<0) s2=0; if (s2>5) s2=5;

            double reg1 = RegisteredEnergy_param(ev.E1, ev.x1, ev.y1, best);
            double reg2 = RegisteredEnergy_param(ev.E2, ev.x2, ev.y2, best);

            double Erec1 = InvertRegisteredEnergy(reg1, best[0], best[1], best[2 + s1]);
            double Erec2 = InvertRegisteredEnergy(reg2, best[0], best[1], best[2 + s2]);

            double Srec = Erec1 + Erec2;

            int s = ev.sector1;
            sum[s] += Srec;
            sumsq[s] += Srec*Srec;
            count[s] += 1;
        }

        std::cout << "\nSector results AFTER inversion (recovered mean sum) vs target:\n";
        for (int s=0;s<6;++s) {
            if (count[s] == 0) {
                std::cout << " sector " << s << ": no events\n";
                continue;
            }
            double mean = sum[s]/double(count[s]);
            double var = (count[s]>1) ? (sumsq[s] - sum[s]*sum[s]/double(count[s]))/double(count[s]-1) : 0.0;
            double statErr = (var>0.0) ? std::sqrt(var)/std::sqrt(double(count[s])) : 1.0;
            std::cout << " sector " << s << ": recovered_mean=" << mean << " ± " << statErr
                      << "  target=" << targetMean[s] << "  N=" << count[s] << "\n";
        }
    }

    // The rest of your original histogramming/drawing code (unchanged, uses recovered k's)
    TH1F *ggX = new TH1F("ggX","Gamma Gamma X",1200,-600.0,600.0);
    TH1F *ggY = new TH1F("ggY","Gamma Gamma Y",1200,-600.0,600.0);
    TH2F *ggXX = new TH2F("ggXX","Gamma Gamma XX",1200,-600.0,600.0,1200,-600.0,600.0);
    TH2F *ggYY = new TH2F("ggYY","Gamma Gamma YY",1200,-600.0,600.0,1200,-600.0,600.0);
    TH2F *ggXY = new TH2F("ggXY","Gamma Gamma XY",1200,-600.0,600.0,1200,-600.0,600.0);
    TH1F *ggTheta = new TH1F("ggTheta","Gamma Gamma Theta",200,-5.0,5.0);
    TH1F *ggSumTheta = new TH1F("ggSumTheta","Gamma Gamma Sum Theta",200,-5.0,5.0);
    TH1F *ggPhi = new TH1F("ggPhi","Gamma Gamma Phi",200,-5.0,5.0);
    TH1F *ggDeltaPhi = new TH1F("ggDeltaPhi","Gamma Gamma Delta Phi",200,-5.0,5.0);
    TH2F *ggSumThetaDeltaPhi = new TH2F("ggSumThetaDeltaPhi","Gamma Gamma Sum Theta Delta Phi",200,-5.0,5.0,200,-5.0,5.0);

    TProfile *ggEsumX = new TProfile("ggEsumX","GammaGamma_EnergySum_X",1200,-600.0,600.0,0.0,500.0);
    TProfile *ggEsumY = new TProfile("ggEsumY","GammaGamma_EnergySum_Y",1200,-600.0,600.0,0.0,500.0);
    TProfile *ggEsumPhi = new TProfile("ggEsumPhi","GammaGamma_EnergySum_Phi",200,-10.0,10.0,0.0,500.0);
    TProfile *ggEsumE = new TProfile("ggEsumE","GammaGamma_EnergySum_E",500,0.0,500.0,0.0,500.0); 
    
    for (const auto &ev: events) {
      double x1 = ev.x1, y1 = ev.y1, x2 = ev.x2, y2 = ev.y2, e1 = ev.E1;
      double Sum = RegisteredEnergy_param(ev.E1, ev.x1, ev.y1, best) + RegisteredEnergy_param(ev.E2, ev.x2, ev.y2, best);
       
        TLorentzVector p1 = ComputeCMMomentum(0, 0, 0, x1, y1, zECal);
        TLorentzVector p2 = ComputeCMMomentum(0, 0, 0, x2, y2, zECal);
        double sumTheta = p1.Vect().Theta() + p2.Vect().Theta();
        double deltaPhi = p1.Vect().Phi() - p2.Vect().Phi();

        ggX->Fill(x1);
        ggX->Fill(x2);
        ggY->Fill(y1);
        ggY->Fill(y2);
        ggXX->Fill(x1,x2);
        ggYY->Fill(y1,y2);
        ggXY->Fill(x1,y1);
        ggXY->Fill(x2,y2);
        ggTheta->Fill( p1.Vect().Theta());
        ggTheta->Fill(p2.Vect().Theta());
        ggSumTheta->Fill(sumTheta);
        ggPhi->Fill(p1.Vect().Phi());
        ggPhi->Fill(p2.Vect().Phi());
        ggDeltaPhi->Fill(deltaPhi);
        ggSumThetaDeltaPhi->Fill(sumTheta,deltaPhi);

        ggEsumX->Fill(x1,Sum);
        ggEsumY->Fill(y1,Sum);
        ggEsumPhi->Fill(p1.Vect().Phi(),Sum);
        ggEsumE->Fill(e1,Sum);
    }

    new TCanvas("ggX"); ggX->Draw();
    new TCanvas("ggY"); ggY->Draw();
    new TCanvas("ggXX"); ggXX->Draw("COLZ");
    new TCanvas("ggYY"); ggYY->Draw("COLZ");
    new TCanvas("ggXY"); ggXY->Draw("COLZ");
    new TCanvas("ggTheta"); ggTheta->Draw();
    new TCanvas("ggSumTheta"); ggSumTheta->Draw();
    new TCanvas("ggPhi"); ggPhi->Draw();
    new TCanvas("ggDeltaPhi"); ggDeltaPhi->Draw();
    new TCanvas("ggSumThetaDeltaPhi"); ggSumThetaDeltaPhi->Draw("COLZ");
    
    new TCanvas("ggEsumX"); ggEsumX->Draw();
    new TCanvas("ggEsumY"); ggEsumY->Draw();
    new TCanvas("ggEsumPhi"); ggEsumPhi->Draw();
    new TCanvas("ggEsumE"); ggEsumE->Draw();
}
