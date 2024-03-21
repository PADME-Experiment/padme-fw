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

#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "TRandom3.h"

class MMegaIonizations 
{
    public:

    MMegaIonizations(G4ThreeVector start, G4ThreeVector end, G4double dE);
    ~MMegaIonizations();

    std::vector<G4int> GetHitStripsX(){return fhitStripsX;};
    std::vector<G4int> GetHitStripsY(){return fhitStripsY;};
    std::vector<G4int> GetisRear(){return fisRear;};
    std::vector<G4double> GetStripTimes(){return fStripTimes;};
    G4int GetNIonizations(){return fNionizations;};

    private:   
    // https://pdg.lbl.gov/2023/web/viewer.html?file=../reviews/rpp2023-rev-particle-detectors-accel.pdf
    // Gas Mixture ArCF4Iso (88:10:2)
    //Double_t NPrimary    = (0.88*25 + 0.10*63 + 0.02*90)/10;   //Primary mm^-1
    G4double fWI    = (0.88*26 + 0.10*26 + 0.02*43.5) * eV;          //Carrier cost (eV)
    G4double fvdrift = 44.7 * mm/us;
    G4double feCharge = -1*e_SI;
    G4int NStrips = 545;
    G4double fStripPitch = 1.1 * mm;

    
    G4double fstepLength;
    G4double fdE;
    G4ThreeVector fionPos;

    std::vector<G4int> fhitStripsX;
    std::vector<G4int> fhitStripsY;
    std::vector<G4int> fisRear;
    std::vector<G4double> fStripTimes;
    G4int fNionizations;

    TRandom3 r;
    
};

#endif