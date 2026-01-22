#include "TMinuit.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

struct SimPair {
    TVector3 p1;  
    TVector3 p2;   
};

std::vector<SimPair> simPairs;


TLorentzVector ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt,
                                 Double_t PosX, Double_t PosY, Double_t PosZ)
{
    Double_t fMe = 0.511;
    Double_t fBeamMomentum = 432;
    Double_t sqrts = sqrt(2.*fMe*fMe + 2.*fBeamMomentum*fMe);
    Double_t betagamma=fBeamMomentum/sqrts;
    Double_t gamma = sqrt(betagamma*betagamma+1.);
    Double_t beta = betagamma/gamma;

    TVector3 cluPos;
    cluPos.SetXYZ(PosX,PosY,PosZ);

    TVector3 targetPos;
    targetPos.SetXYZ(xt,yt,zt);

    TVector3 cluMom = cluPos - targetPos;
    cluMom *= (1.0 / cluMom.Mag());

    TVector3 COGPos;
    COGPos.SetXYZ(0,0,3536);

    TVector3 boost = COGPos - targetPos;
    boost *= (beta / boost.Mag());

    Double_t cosa = cluMom.Dot(boost)/(cluMom.Mag()*boost.Mag());

    Double_t Elab = 0.5 * sqrts / sqrt(1. - cosa*cosa + pow(gamma*cosa,2)
                                      - 2.*betagamma*gamma*cosa + pow(betagamma,2));

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
  double zt = 0.0;
  const double PI = TMath::Pi();
  const double zcal = 3536.0;  
  //double chi2 = 0;
  double sum_theta2 = 0.0;
  double sum_phi2   = 0.0;
  int N = 0;
  for (auto &sp : simPairs) {

    const TVector3 &d1 = sp.p1;
    const TVector3 &d2 = sp.p2;

    double t1 = (zcal - zt) / d1.Z();
    double t2 = (zcal - zt) / d2.Z();

    double x1 = xt + t1 * d1.X();
    double y1 = yt + t1 * d1.Y();

    double x2 = xt + t2 * d2.X();
    double y2 = yt + t2 * d2.Y();

    TLorentzVector p1 = ComputeCMMomentum(xt, yt, zt, x1, y1, 3536.0);
    TLorentzVector p2 = ComputeCMMomentum(xt, yt, zt, x2, y2, 3536.0);

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
  
  return 0.5 * N * (2.0 + log(var_theta) + log(var_phi));
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

        SimPair sp;
        sp.p1 = v1.Unit();
        sp.p2 = v2.Unit();

        simPairs.push_back(sp);
        parsed++;
    }

    fin.close();
    cout << "Loaded " << parsed << " simulated photon pairs." << endl;
}

void FitSimulatedTarget()
{

    LoadSimulatedEvents("events_1.txt");

    if (simPairs.empty()) {
        cerr << "No events loaded – aborting." << endl;
        return;
    }


    ROOT::Math::Minimizer *min =
        ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

    ROOT::Math::Functor f(&Chi2, 3);
    min->SetFunction(f);

    min->SetLimitedVariable(0, "xt", 0.0, 0.01, -50.0, 50.0);
    min->SetLimitedVariable(1, "yt", 0.0, 0.01, -50.0, 50.0);
    min->SetLimitedVariable(2, "zt", 0.0, 1.0, -50.0, 50.0);

    min->Minimize();

    const double *xs = min->X();


    cout << "xt = " << xs[0] << endl;
    cout << "yt = " << xs[1] << endl;
    //cout << "zt = " << xs[2] << endl;
    cout << "Chi2 = " << min->MinValue() << endl;


    delete min;
}
