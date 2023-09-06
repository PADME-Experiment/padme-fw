#include <TH1.h>
#include <TH2.h>
#include <TH3.h> 
#include <TStyle.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TRandom.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <regex>

class Particle{
private:
    double mass_;
    double px_;
    double py_;
    double pz_;

public:
    Particle(double mass, double px, double py, double pz)
        : mass_(mass), px_(px), py_(py), pz_(pz) {}

    // Calcola l'impulso trasverso (transverse momentum) della particella
    double TransverseMomentum() const {
        return sqrt(px_ * px_ + py_ * py_);
    }
    
    //Calcola il modulo del trimomento
    double TriMomentum() const {
        return sqrt(px_*px_ + py_*py_ + pz_*pz_);
    }

    // Calcola l'energia totale della particella
    double Energy() const {
        return sqrt(mass_*mass_ + TriMomentum()*TriMomentum());
    }


    // Calcola l'angolo di scattering della particella
    double ScatteringAngle() const {
        return atan2(TransverseMomentum(), pz_);
    }

    // Calcola l'angolo di scattering della particella nel CoM frame
    double CoMScatteringAngle(const Particle& targetParticle) const {
        double ptDiff = TransverseMomentum() - targetParticle.TransverseMomentum();
        double pzDiff = pz_ - targetParticle.pz_;

        return atan2(ptDiff, pzDiff);
    }

    //Calcola l'angolo tra l'impulso x e z not along beam axis
    double PhiX(double PhiXboost) const {
        return atan2(px_, pz_) + PhiXboost;
    }

    //Calcola l'angolo tra l'impulso y e z not along beam axis
    double PhiY(double PhiYboost) const {
        return atan2(py_, pz_) + PhiYboost;
    }

    //Calcola la coordinata x ad una distanza Delta dal bersaglio in mm
    double R_x(double DeltaZ, double PhiXboost, double TargetOffsetX) const{
        return TMath::Tan(PhiX(PhiXboost))*DeltaZ + TargetOffsetX;
    }

    //Calcola la coordinata y ad una distanza Delta dal bersaglio in mm
    double R_y(double DeltaZ, double PhiYboost, double TargetOffsetY) const{
        return TMath::Tan(PhiY(PhiYboost))*DeltaZ + TargetOffsetY;
    }

    // Calcola il raggio ad una distanza Delta dal bersaglio in mm
    double TransverseRadius(double DeltaZ, double PhiXboost, double PhiYboost, double TargetOffsetX, double TargetOffsetY) const {
        return (TMath::Sqrt(R_x(DeltaZ, PhiXboost, TargetOffsetX)*R_x(DeltaZ, PhiXboost, TargetOffsetX) + R_y(DeltaZ, PhiYboost, TargetOffsetY)*R_y(DeltaZ, PhiYboost, TargetOffsetY)));
    }
};

class EventAnalyzer {
private:
    TH2F* h1_;
    TH2F* h2_;
    TH2F* h3_;
    TH2F* h4_;
    TH2F* h5_;
    TH2F* h6_;
    double Radius_1_;
    double Radius_2_;
    double x_1_;
    double y_1_;
    double x_2_;
    double y_2_;
    double FinEnergy_1_;
    double FinEnergy_2_;
public:
    EventAnalyzer(TH2F* h1, TH2F* h2, TH2F* h3, TH2F* h4, TH2F* h5, TH2F* h6)
        : h1_(h1), h2_(h2), h3_(h3), h4_(h4), h5_(h5), h6_(h6) {}

    ~EventAnalyzer() {}

    double Radius_max;
    double Radius_min;
    double En_min;
    double Shadow;

    void AnalyzeEvents(const double Radius_max, double Radius_min, const double En_min, 
                        const double Radius_1, const double Radius_2,
                        const double x_1, const double y_1,
                        const double x_2, const double y_2,
                        const double FinEnergy_1, const double FinEnergy_2,
                        double Shadow) {
        Radius_1_ = Radius_1;
        Radius_2_ = Radius_2;
        x_1_ = x_1;
        y_1_ = y_1;
        x_2_ = x_2;
        y_2_ = y_2;
        FinEnergy_1_ = FinEnergy_1;
        FinEnergy_2_ = FinEnergy_2;
        bool radiusCut = (Radius_1_ > Radius_min && Radius_2_ > Radius_min &&
                        Radius_1_ < Radius_max && Radius_2_ < Radius_max);
        bool energyCut = (FinEnergy_1_ > En_min && FinEnergy_2_ > En_min);
        bool shadowCut = (abs(y_1_) < Shadow && abs(y_2_) < Shadow);
        // Analisi degli eventi con tutti i tagli
        if (radiusCut && energyCut && shadowCut) {
            h5_->Fill(x_1_,y_1_);
            h6_->Fill(x_2_,y_2_);
        }
    }

    void GeometricCutsOnly(const double Radius_max, const double Radius_min,
                        const double Radius_1, const double Radius_2,
                        const double x_1, const double y_1,
                        const double x_2, const double y_2,
                        double Shadow) {
        Radius_1_ = Radius_1;
        Radius_2_ = Radius_2;
        x_1_ = x_1;
        y_1_ = y_1;
        x_2_ = x_2;
        y_2_ = y_2;
        bool radiusCut = (Radius_1_ > Radius_min && Radius_2_ > Radius_min &&
                        Radius_1_ < Radius_max && Radius_2_ < Radius_max);
        bool shadowCut = (abs(y_1_) < Shadow && abs(y_2_) < Shadow);
        // Analisi degli eventi con solo i tagli geometrici (radiuscut e shadowcut)
        if (radiusCut && shadowCut) {
            h3_->Fill(x_1_, y_1_);
            h4_->Fill(x_2_, y_2_);
        }
    }

    void NoCuts() {
        // Analisi degli eventi senza nessun taglio
        h1_->Fill(x_1_, y_1_);
        h2_->Fill(x_2_, y_2_);
    }

    void AnalyzeEventsFull(const double Radius_max, double Radius_min, const double En_min, 
                        const double Radius_1, const double Radius_2,
                        const double x_1, const double y_1,
                        const double x_2, const double y_2,
                        const double FinEnergy_1, const double FinEnergy_2,
                        double Shadow) {
        Radius_1_ = Radius_1;
        Radius_2_ = Radius_2;
        x_1_ = x_1;
        y_1_ = y_1;
        x_2_ = x_2;
        y_2_ = y_2;
        FinEnergy_1_ = FinEnergy_1;
        FinEnergy_2_ = FinEnergy_2;
        bool radiusCut = (Radius_1_ > Radius_min && Radius_2_ > Radius_min &&
                        Radius_1_ < Radius_max && Radius_2_ < Radius_max);
        bool energyCut = (FinEnergy_1_ > En_min && FinEnergy_2_ > En_min);
        bool shadowCut = (abs(y_1_) < Shadow && abs(y_2_) < Shadow);
        // Analisi degli eventi con tutti i tagli
        h1_->Fill(x_1_, y_1_);
        h2_->Fill(x_2_, y_2_);
        if (radiusCut && shadowCut) {
            h3_->Fill(x_1_, y_1_);
            h4_->Fill(x_2_, y_2_);;
            if(energyCut){
                h5_->Fill(x_1_, y_1_);
                h6_->Fill(x_2_, y_2_);
            }
        }
    }
};