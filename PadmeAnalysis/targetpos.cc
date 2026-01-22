#include "TMinuit.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TLorentzVector.h"
#include "TVector3.h"
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

TLorentzVector ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt,
                                  Double_t PosX, Double_t PosY, Double_t PosZ)
{
  Double_t fMe = 0.511;
  Double_t fBeamMomentum = 431.6;
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
  COGPos.SetXYZ(2.1,0.67,2508.31);
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

double Chi2(const double *params)
{
    double xt = params[0];
    double yt = params[1];
    double zt = params[2];

    double chi2 = 0;

    for (auto &p : pairs) {
        auto p1 = ComputeCMMomentum(xt, yt, zt, p.x1, p.y1, p.z1);
        auto p2 = ComputeCMMomentum(xt, yt, zt, p.x2, p.y2, p.z2);

        double S = p1.Vect().Theta() + p2.Vect().Theta();
        double diff = S - M_PI;
        chi2 += diff * diff;
    }

    return chi2;
}

double Chi2Gaus(const double *params)
{
    double xt = params[0];
    double yt = params[1];
    double zt = params[2];

    double chi2 = 0;
    double sum = 0;
    double sumsq = 0;
    int N=0;

    for (auto &p : pairs) {
      auto p1 = ComputeCMMomentum(xt, yt, zt, p.x1, p.y1, p.z1);
      auto p2 = ComputeCMMomentum(xt, yt, zt, p.x2, p.y2, p.z2);

      double S = p1.Vect().Theta() + p2.Vect().Theta();

      sum+=S;
      sumsq+=S*S;
      N++;
    }
    double mu = M_PI;
    double ss = sumsq-2*mu*sum+N*mu*mu;
    double var = std::max(ss/N,1e-6);
    chi2 = N*log(var) + ss/var;

    return chi2;
}

void FitTarget()
{
  
    std::ifstream in("gamma_gamma_allcoord.txt");

    ClusterPair cp;
    while (in >> cp.e1 >> cp.x1 >> cp.y1 >> cp.z1 >> cp.e2 >> cp.x2 >> cp.y2 >> cp.z2)
      pairs.push_back(cp);

    std::cout << "Loaded " << pairs.size() << " cluster pairs\n";
    
    std::vector<ClusterPair> cleaned;
    cleaned.reserve(pairs.size());

    double xt0 = 0.0;      
    double yt0 = 0.0;
    double zt0 = -1028.0;  

    for (auto &p : pairs) {
      auto c1 = ComputeCMMomentum(xt0, yt0, zt0, p.x1, p.y1, p.z1);
      auto c2 = ComputeCMMomentum(xt0, yt0, zt0, p.x2, p.y2, p.z2);

      double th1 = c1.Vect().Theta();
      double th2 = c2.Vect().Theta();
      //std::cout << "Theta1: " << th1 << " Theta2: " << th2 << std::endl;

      if (th1+th2 >= 3.0 && th1+th2 <= 3.3)
        {
	  cleaned.push_back(p);
        }
    }

    pairs = cleaned;

    std::cout << "After theta filter: " << pairs.size()
              << " pairs survive.\n";
    
    ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

    ROOT::Math::Functor f(&Chi2Gaus, 3);
    min->SetFunction(f);


    min->SetLimitedVariable(
			    0,             // parameter index
			    "xt",          // name
			    0.0,           // initial 
			    0.5,           // step 
			    -50.0,         // lower 
			    50.0           // upper 
			    );

    
    min->SetLimitedVariable(
			    1, "yt",
			    0.0,
			    0.5,
			    -50.0,
			    50.0
			    );

    
    min->SetLimitedVariable(
			    2, "zt",
			    -1028.0,
			    1,
			    -1040.0,
			    -1020.0
			    );



    min->Minimize();

    const double *xs = min->X();
  
    std::cout << "xt = " << xs[0] << "\n";
    std::cout << "yt = " << xs[1] << "\n";
    std::cout << "zt = " << xs[2] << "\n";

    std::cout << "\nChi2 = " << min->MinValue() << "\n";

    delete min;
}

void MinimizeTargetPosition()
{
  
    ifstream fin("gamma_gamma_allcoord.txt");

    

    vector<double> x1, y1, z1, x2, y2, z2;

    double cx1, cy1, cz1, cx2, cy2, cz2, ce1, ce2;

    while (fin >> ce1 >> cx1 >> cy1 >> cz1 >> ce2 >> cx2 >> cy2 >> cz2) {
        x1.push_back(cx1);
        y1.push_back(cy1);
        z1.push_back(cz1);
        x2.push_back(cx2);
        y2.push_back(cy2);
        z2.push_back(cz2);
    }

    fin.close();

    int N = x1.size();
    cout << "Loaded " << N << " gamma-gamma pairs" << endl;


    
    int xt_min = -50, xt_max = 50;
    int yt_min = -50, yt_max = 50;
    int zt_min = -1029, zt_max = -1027;

    double bestChi = 1e99;
    int best_xt = 0, best_yt = 0, best_zt = 0;

 
    for (int xt = xt_min; xt <= xt_max; xt++) {
      cout << "Scanning xt = " << xt << endl;

        for (int yt = yt_min; yt <= yt_max; yt++) {
	  
            for (int zt = zt_min; zt <= zt_max; zt++) {
	      //cout << "Scanning xt = " << xt << " yt = " << yt << " zt = " << zt << endl;

                double chi = 0;

                for (int i = 0; i < N; i++) {

                    TLorentzVector p1 =
                        ComputeCMMomentum(xt, yt, zt, x1[i], y1[i], 2508.31);

                    TLorentzVector p2 =
                        ComputeCMMomentum(xt, yt, zt, x2[i], y2[i], 2508.31);

                    double thetaSum =
                        p1.Vect().Theta() + p2.Vect().Theta();
		    if (thetaSum>3 && thetaSum <3.3) chi += fabs(thetaSum - TMath::Pi());
                }

                if (chi < bestChi) {
                    bestChi = chi;
                    best_xt = xt;
                    best_yt = yt;
                    best_zt = zt;
                }
            }
        }
    }

  
    cout << "Target positions" << endl;
    cout << "xt = " << best_xt << endl;
    cout << "yt = " << best_yt << endl;
    cout << "zt = " << best_zt << endl;
    cout << "Minimum χ = " << bestChi << endl;
}
