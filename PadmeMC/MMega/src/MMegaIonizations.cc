// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-12
//
// This class generates the ionizations from the Geant4 hit data in the drift volume
//
// --------------------------------------------------------------

#include "MMegaIonizations.hh"
#include "G4ThreeVector.hh"
#include "MMegaGeometry.hh"


MMegaIonizations::MMegaIonizations(G4ThreeVector start, G4ThreeVector end, G4double dE)
{
    fstepLength = (end-start).mag();
    fdE = dE;
    fNionizations = r.Poisson(fdE/fWI);

    MMegaGeometry* geo = MMegaGeometry::GetInstance();

    for(int i = 0; i < fNionizations; i++){
        double randomPos = r.Uniform(0, fstepLength);
        G4ThreeVector fionPos = start + ((end-start)/fstepLength)*randomPos;
        fhitStripsX.push_back((fionPos.x() + 0.5*geo->GetMMegaDriftSizeX())/fStripPitch);
        fhitStripsY.push_back((fionPos.y() + 0.5*geo->GetMMegaDriftSizeY())/fStripPitch);
        if(fionPos.z() <= 0){
            fisRear.push_back(0);
            double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+fionPos.z();
            fStripTimes.push_back(s/fvdrift);
        }

        else if(fionPos.z() > 0){
            fisRear.push_back(1);
            double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-fionPos.z();
            fStripTimes.push_back(s/fvdrift);
        }
    }

}

MMegaIonizations::~MMegaIonizations()
{}