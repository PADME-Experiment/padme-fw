#include "TLorentzVector.h"
#include <fstream>
#include <iostream>
#include <cmath>

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
  COGPos.SetXYZ(1.7,0.4,2508.31);
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

double RegisteredEnergy(double Etrue, double x, double y)
{
    
    double k0 = 0.95;        
    double k1 = 0.0005;      
    double kE = k0 + k1 * Etrue;

   
    double phi = std::atan2(y, x);  
    if (phi < 0) phi += 2*M_PI;   

    int sector = int( phi / (M_PI/3.0) );  
    if (sector < 0) sector = 0;
    if (sector > 5) sector = 5;

    double sectorCorr[6] = {
        1.02,  
        0.98,
        1.01, 
        0.99,  
        1.03,  
        1.00 
    };

    double kSector = sectorCorr[sector];
    return kE * kSector * Etrue;
}

void toyMC(){
  ifstream fin("events_1.txt");
    if (!fin.is_open()) {
    std::cerr << "ERROR: cannot open file. Check path.\n";
    return;
  }

     std::string line;
  size_t header_skip = 12;
  
  for (size_t i=0; i<header_skip && std::getline(fin, line); ++i) {

    std::cout << "Header: " << line << "\n";
  }

  std::vector<double> Enum, Ee, Epos;
  std::vector<double> P1x, P1y, P1z, P2x, P2y, P2z, Q, alpha;


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

  size_t totalLines = 0;
  size_t parsed = 0;
 

while (std::getline(fin, line)) {

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

    // Store values
    Enum.push_back(ev);
    Ee.push_back(P1_3);
    Epos.push_back(P2_3);

    P1x.push_back(P3_1);
    P1y.push_back(P3_2);
    P1z.push_back(P3_3);

    P2x.push_back(P4_1);
    P2y.push_back(P4_2);
    P2z.push_back(P4_3);

    Q.push_back(Q_factor);
    alpha.push_back(alpha_QCD);
}


  fin.close();

  std::cout << "File read complete. total non-empty lines checked: " << totalLines
            << ", parsed events: " << parsed << ", vectors size: " << P1x.size() << "\n";

 
  if (!P1x.empty()) {
    std::cout << "First parsed event: ev=" << Enum[0] 
              << " P1=( " << P1x[0] << ", " << P1y[0] << ", " << P1z[0] << " )"
              << " P2=( " << P2x[0] << ", " << P2y[0] << ", " << P2z[0] << " )\n";
  } else {
    std::cerr << "No events parsed. Check header skipping and file format.\n";
    return;
  }

  double zECal = 3536;
    
  std::vector<double> X1,X2,Y1,Y2;

  for (size_t i = 0; i < P1x.size(); i++) {

    double x1 = P1x[i] / P1z[i] * zECal;
    double y1 = P1y[i] / P1z[i] * zECal;
    double r1 = std::sqrt(x1*x1 + y1*y1);

    double x2 = P2x[i] / P2z[i] * zECal;
    double y2 = P2y[i] / P2z[i] * zECal;
    double r2 = std::sqrt(x2*x2 + y2*y2);

    
    //std::cout<< r1 << r2 << std::endl;
    if (r1 > 90.0 && r1 < 270.0 && r2 > 90.0 && r2 < 270.0 ) {

      double Etrue1 = std::sqrt(P1x[i]*P1x[i] + P1y[i]*P1y[i] + P1z[i]*P1z[i]);
      double Etrue2 = std::sqrt(P2x[i]*P2x[i] + P2y[i]*P2y[i] + P2z[i]*P2z[i]);

      double Ereg1 = RegisteredEnergy(Etrue1, x1, y1);
      double Ereg2 = RegisteredEnergy(Etrue2, x2, y2);
      
      X1.push_back(x1);
      Y1.push_back(y1);

      X2.push_back(x2);
      Y2.push_back(y2);

      TLorentzVector p1 = ComputeCMMomentum(0, 0, 0, x1, y1, zECal);
      TLorentzVector p2 = ComputeCMMomentum(0, 0, 0, x2, y2, zECal);
      double sumTheta = p1.Vect().Theta() + p2.Vect().Theta();      
      double deltaPhi = p1.Vect().Phi() - p2.Vect().Phi();

      //std::cout<< "Sum Theta: " << sumTheta <<std::endl;
	
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
      
    }
  }

  TCanvas *c1 = new TCanvas("ggX");
  c1->cd();
  ggX->Draw();

  TCanvas *c2 = new TCanvas("ggY");
  c2->cd();
  ggY->Draw();

  TCanvas *c3 = new TCanvas("ggXX");
  c3->cd();
  ggXX->Draw();

  TCanvas *c4 = new TCanvas("ggYY");
  c4->cd();
  ggYY->Draw();

  TCanvas *c5 = new TCanvas("ggXY");
  c5->cd();
  ggXY->Draw();

  TCanvas *c6 = new TCanvas("ggTheta");
  c6->cd();
  ggTheta->Draw();

  TCanvas *c7 = new TCanvas("ggSumTheta");
  c7->cd();
  ggSumTheta->Draw();

  TCanvas *c8 = new TCanvas("ggPhi");
  c8->cd();
  ggPhi->Draw();

  TCanvas *c9 = new TCanvas("ggDeltaPhi");
  c9->cd();
  ggDeltaPhi->Draw();

  TCanvas *c10 = new TCanvas("ggSumThetaDeltaPhi");
  c10->cd();
  ggSumThetaDeltaPhi->Draw();


  
}
