// FitVariance.C
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TH1D.h>

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <limits>

using namespace std;

struct ClusterPair {
    double x1, y1, z1;
    double x2, y2, z2;
};

static std::vector<ClusterPair> pairs;


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
  cluPos.SetXYZ(PosX,PosY,PosZ);
  TVector3 targetPos;
  targetPos.SetXYZ(xt,yt,zt);
  TVector3 cluMom = cluPos - targetPos;
  cluMom *= (1. / cluMom.Mag());
  TVector3 COGPos;
  COGPos.SetXYZ(1.7,0.4,PosZ);
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

struct VarianceFunctor {
   
    double th_min = 3.0;
    double th_max = 3.3;
    double posZ = 2508.31;

    double operator()(const double *par) const {
        const double xt = par[0];
        const double yt = par[1];
        const double zt = par[2];

        double sum = 0.0;
        double sumsq = 0.0;
        size_t count = 0;

        for (const auto &p : pairs) {
            TLorentzVector p1 = ComputeCMMomentum(xt, yt, zt, p.x1, p.y1, p.z1);
            TLorentzVector p2 = ComputeCMMomentum(xt, yt, zt, p.x2, p.y2, p.z2);

            double th1 = p1.Vect().Theta();
            double th2 = p2.Vect().Theta();

            if (th1+th2 < th_min) continue;
            if (th1+th2 > th_max) continue;

            double tsum = th1 + th2;
            sum += tsum;
            sumsq += tsum * tsum;
            ++count;
        }

        if (count < 2) {
            
            return 1e6;
        }

        double mean = sum / double(count);
        double variance = (sumsq / double(count)) - (mean * mean);

       
        if (variance < 0 && variance > -1e-12) variance = 0.0;
        if (variance < 0) {
            return 1e6;
        }
        return variance;
    }
};


void SaveThetaSumDistribution(double xt, double yt, double zt, const char *outname = "thetaSum_best.root") {
    vector<double> thetaSums;
    for (const auto &p : pairs) {
        TLorentzVector p1 = ComputeCMMomentum(xt, yt, zt, p.x1, p.y1, p.z1);
        TLorentzVector p2 = ComputeCMMomentum(xt, yt, zt, p.x2, p.y2, p.z2);

        double th1 = p1.Vect().Theta();
        double th2 = p2.Vect().Theta();

        if (th1 < 3.0 || th1 > 3.3) continue;
        if (th2 < 3.0 || th2 > 3.3) continue;

        thetaSums.push_back(th1 + th2);
    }

    if (thetaSums.empty()) {
        cout << "No theta-sum entries to save.\n";
        return;
    }

    double minv = *min_element(thetaSums.begin(), thetaSums.end());
    double maxv = *max_element(thetaSums.begin(), thetaSums.end());
    int nbins = max(10, int((maxv - minv) / 0.001)); 
    if (nbins > 1000) nbins = 1000;

    TH1D h("h_thetaSum", "theta1+theta2;#theta_{sum};counts", nbins, minv, maxv);
    for (double v : thetaSums) h.Fill(v);

    TFile fout(outname, "RECREATE");
    h.Write();
    fout.Close();

    cout << "Saved theta-sum histogram with " << thetaSums.size() << " entries to " << outname << "\n";
}


void FitVariance()
{
  
    pairs.clear();
    ifstream fin("gamma_gamma_coord.txt");
   
    ClusterPair cp;
    while (fin >> cp.x1 >> cp.y1 >> cp.z1 >> cp.x2 >> cp.y2 >> cp.z2) {
        pairs.push_back(cp);
    }
    fin.close();

    cout << "Loaded " << pairs.size() << " cluster pairs from file.\n";
    if (pairs.empty()) return;

    vector<ClusterPair> preclean;
    preclean.reserve(pairs.size());
    double xt0 = 0.0, yt0 = 0.0, zt0 = -1030.0;
    for (auto &p : pairs) {
        TLorentzVector c1 = ComputeCMMomentum(xt0, yt0, zt0, p.x1, p.y1, p.z1);
        TLorentzVector c2 = ComputeCMMomentum(xt0, yt0, zt0, p.x2, p.y2, p.z2);
        double th1 = c1.Vect().Theta();
        double th2 = c2.Vect().Theta();
        if (th1+th2 < 3.0) continue;
        if (th1+th2 > 3.3) continue;
        preclean.push_back(p);
    }
    pairs.swap(preclean);
    cout << "After prefilter (using nominal target): " << pairs.size() << " pairs remain.\n";
    if (pairs.size() < 2) {
        cout << "Not enough pairs to perform variance minimization.\n";
        return;
    }

   
    ROOT::Math::Minimizer *min =
        ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

   
    min->SetMaxFunctionCalls(100000); 
    min->SetTolerance(1e-6);

    VarianceFunctor varfun;
    ROOT::Math::Functor f(&varfun, &VarianceFunctor::operator(), 3);
    min->SetFunction(f);

    min->SetLimitedVariable(0, "xt", 0.0, 0.5, -50.0, 50.0);
    min->SetLimitedVariable(1, "yt", 0.0, 0.5, -50.0, 50.0);
    min->SetLimitedVariable(2, "zt", -1030.0, 0.5, -1031.0, -1025.0);

    std::cout << "Starting minimization...\n";
    min->Minimize();

    const double *xs = min->X();
    double varmin = min->MinValue();


    cout << "Minimization results (variance target):\n";
    cout << "xt = " << xs[0] << "\n";
    cout << "yt = " << xs[1] << "\n";
    cout << "zt = " << xs[2] << "\n";
    cout << "Minimized variance = " << varmin << "\n";

    SaveThetaSumDistribution(xs[0], xs[1], xs[2], "thetaSum_best.root");

    delete min;
}
