// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-12
//
// This class generates the ionizations from the Geant4 hit data in the drift volume
//
// --------------------------------------------------------------

#ifndef MMegaIonizations_h
#define MMegaIonizations_h 1

#include "MMegaGeometry.hh"

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "TRandom3.h"

class MMegaIonizations 
{
    public:

    MMegaIonizations(G4ThreeVector start, G4ThreeVector end, G4double dE);
    ~MMegaIonizations();

    G4double GetDriftVelocity(){return fvdrift;}
    void SetDriftVelocity(G4double v){fvdrift = v;}
    G4double GetCarrierCost(){return fWI;}
    void SetCarrierCost(G4double cost){fWI = cost;}
    G4double GetGain(){return fGain;}
    void SetGain(G4double g){fGain = g;}
    G4double GetFirstGain(){return fFirstGain;}
    void SetFirstGain(G4double g){fFirstGain = g;}
    G4double GetSecondGain(){return fSecondGain;}
    void SetSecondGain(G4double g){fSecondGain = g;}
    G4double GetFirstZoneRadius(){return fFirstZone;}
    void SetFirstZoneRadius(G4double r){fFirstZone = r;}
    G4double GetSecondZoneRadius(){return fSecondZone;}
    void SetSecondZoneRadius(G4double r){fSecondZone = r;}

    std::vector<G4int> GetIDs(){return fIDs;}
    std::vector<G4double> GetTimes(){return fTimes;}

    G4int GetNIonizations(){return fNionizations;}
    std::vector<G4double> GetRadii(){return fRadii;}
    G4double GetElectronCharge(){return feCharge;}


    private:   
    
    // https://pdg.lbl.gov/2023/web/viewer.html?file=../reviews/rpp2023-rev-particle-detectors-accel.pdf
    // Gas Mixture ArCF4Iso (88:10:2)
    G4double fNPrimary    = 0.1*(0.88*25 + 0.10*63 + 0.02*90)/mm;   //Primary mm^-1
    
    G4double fWI    = (0.88*26 + 0.10*26 + 0.02*43.5) * eV;          //Carrier cost (eV)
    G4double fvdrift = 44.7 * mm/us;
    G4double feCharge = e_SI;
    
    G4double fGain = 10000.;
    G4double fFirstZone = 60*cm;
    G4double fFirstGain = 0.001;
    G4double fSecondZone = 100*cm;
    G4double fSecondGain = 0.01;

    G4double fstepLength;
    G4double fdE;
    G4int fid;
    G4double ftime;

    // std::vector<G4ThreeVector> fIonizations;
    std::vector<G4int> fIDs;
    std::vector<G4double> fTimes;
    std::vector<G4double> fRadii;
    G4int fNionizations;

    TRandom3 r;

    void ComputeStripID(G4ThreeVector ionipos);
    void ComputePadID(G4ThreeVector ionipos);

    MMegaGeometry* geo;
    
};

#endif