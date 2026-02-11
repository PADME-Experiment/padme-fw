// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-12
//
// This class generates the ionizations from the Geant4 hit data in the drift volume
//
// Note: each readout layer is identified by the first digit of the strip id
//
// xfront : 1, yfront : 2, yrear : 3, xrear : 4
//
// --------------------------------------------------------------

#include "MMegaIonizations.hh"

#include "Randomize.hh"
#include "G4Poisson.hh"
#include "G4UnitsTable.hh"
#include <cmath>


MMegaIonizations::MMegaIonizations(G4ThreeVector start, G4ThreeVector end, G4double dE)
{

    geo = MMegaGeometry::GetInstance();
    
    // r = new TRandom3();
    // r->SetSeed(0);
    
    InitializeChargeDistribution();
    InitializeTimeSpread();

    fstepLength = (end-start).mag();
    fdE = dE;
    // fNionizations = r.Poisson(fdE/fWI);
    fNionizations = G4Poisson(fNPrimary*fstepLength);
    
    // G4cout << "MMegaIonizations.cc : dE = " << fdE << G4endl;
    // G4cout << "MMegaIonizations.cc : steplength = " << fstepLength << G4endl;
    // G4cout << "MMegaIonizations.cc : fNionizations = " << fNionizations << G4endl;
    

    for(short i = 0; i < fNionizations; i++){ // generate drift electrons in the step
        
        double randomPos = G4UniformRand()*fstepLength;
        // G4cout << "MMegaIonizations.cc : randomPos = " << randomPos << G4endl;
        
        G4ThreeVector fionPos = start + ((end-start)/fstepLength)*randomPos;

        if(geo->GetReadoutType() == "strips"){
            ComputeStripID(fionPos);
            // here i could also push back the neighbouring strips and the times for induction
            // something like ftime + (distance/induction speed)
            // by looking at test beam data induction speed is around 4.7 × 10−3 mm/ns 
            // fIDs.push_back(fid); 
            // fTimes.push_back(ftime);
            // fRadii.push_back(sqrt(fionPos.x()*fionPos.x() + fionPos.y()*fionPos.y()));
        }
        else if(geo->GetReadoutType() == "pads"){
            ComputePadID(fionPos);
            // fIDs.push_back(fid);
            // fTimes.push_back(ftime);
            // fRadii.push_back(sqrt(fionPos.x()*fionPos.x() + fionPos.y()*fionPos.y()));
        } 

    }
}

MMegaIonizations::~MMegaIonizations(){
}


// Define and initialize the static member variable to nullptr
TF1* MMegaIonizations::charge_dist = nullptr;
TF1* MMegaIonizations::time_spread = nullptr;

void MMegaIonizations::InitializeChargeDistribution(){
    
    if (!charge_dist) {
        
        // Initialize function according to parameters fitted from real data
        // coming from may 2024 Test Beam at LNF 
        charge_dist = new TF1("charge distribution", "landau", 0.01, 2500);  // [0, 2500] Charge in ADC Counts
        charge_dist->SetParameter(0, 0.0134);
        charge_dist->SetParameter(1, 249.3);
        charge_dist->SetParameter(2, 70.92);

    }
}

void MMegaIonizations::InitializeTimeSpread(){
    
    if (!time_spread) {
        
        // Initialize function according to parameters fitted from real data
        // coming from may 2024 Test Beam at LNF 
        time_spread = new TF1("time_spread", "[0] + [1]/x + [2]/x^2", 0.01, 2500);  // starting from 0.01 to avoid divergence
        time_spread->SetParameter(0, 6.5);
        time_spread->SetParameter(1, 904.8);
        time_spread->SetParameter(2, 1905);

    }
}

G4double MMegaIonizations::GetChargeFromDistribution(){
    return charge_dist->GetRandom();
}

G4double MMegaIonizations::GetTimeSpread(G4double charge){
    // G4cout << "MMegaIonizations TimeSpread : " << time_spread->Eval(charge) << G4endl;
    // G4cout << "MMegaIonizations TimeSpread : " << time_spread->Eval(charge) * ns<< G4endl;
    return G4RandGauss::shoot(0, time_spread->Eval(charge))*ns;
}

void MMegaIonizations::ComputeStripID(G4ThreeVector ionipos){

    G4double x0 = geo->GetXStripStartPos();
    G4double y0 = geo->GetYStripStartPos();
    G4double w = geo->GetMMegaStripWidth();
    G4double pitch = geo->GetMMegaStripPitch();
        
    G4double x = ionipos.x();
    G4double y = ionipos.y();
    G4double z = ionipos.z();

    G4int ix = (x-(x0-0.5*w))/pitch;
    G4int iy = (y-(y0-0.5*w))/pitch;

    if (ix >= 0 && ix < 2000){
        if((x-(x0-0.5*w)) - ix * pitch < w){ // if electron hits the strip
            if(z<=0){
                fid = ix + 10000;
                G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
                // G4cout << "MMegaIonizations::ComputeStripID : s = " << s << G4endl;
                ftime = s/fvdrift;
            }
            else if(z>0){
                fid = ix + 40000;
                G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
                // G4cout << "MMegaIonizations::ComputeStripID : s = " << s << G4endl;
                ftime = s/fvdrift;
            }
        
            fIDs.push_back(fid);
            fTimes.push_back(ftime);
            fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
            // G4cout << "MMegaIonizations::ComputeStripID : time = " << ftime << G4endl;
        }
    }

    if (iy >= 0 && iy < 2000){
        if((y-(y0-0.5*w)) - iy * pitch < w){ // if electron hits the strip
            if(z<=0){
                fid = iy+20000;
                G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
                // G4cout << "MMegaIonizations::ComputeStripID : s = " << s << G4endl;
                ftime = s/fvdrift;
            }
            else if(z>0){
                fid = iy+30000;
                G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
                // G4cout << "MMegaIonizations::ComputeStripID : s = " << s << G4endl;
                ftime = s/fvdrift;
            }

            fIDs.push_back(fid);
            fTimes.push_back(ftime);
            fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
            // G4cout << "MMegaIonizations::ComputeStripID : time = " << ftime/ns << G4endl;
            
        }
    }
}

void MMegaIonizations::ComputePadID(G4ThreeVector ionipos){

    G4double x0 = geo->GetXPadStartPos();
    G4double y0 = geo->GetYPadStartPos();
    G4double shift = 0.5*geo->GetPadDistance(); //accounts for the fact that x and y pads are shifted
    G4double wx = sqrt(0.5*geo->GetXPadArea());
    G4double wy = sqrt(0.5*geo->GetYPadArea());
    G4double pitch = geo->GetPadDistance();

    G4double x = ionipos.x();
    G4double y = ionipos.y();
    G4double z = ionipos.z();

    G4int ix_x = (x-(x0-wx))/pitch;
    G4int iy_x = (y-(y0-wx))/pitch;

    G4int ix_y = (x-(x0+shift-wy))/pitch;
    G4int iy_y = (y-(y0+shift-wy))/pitch;

    // shift coordinates in the center of the pad
    G4double x1_x = (x-(x0-wx)) - (ix_x*pitch+wx);
    G4double y1_x = (y-(y0-wy)) - (iy_x*pitch+wy);

    G4double x1_y = (x-(x0+shift-wx)) - (ix_y*pitch+wx);
    G4double y1_y = (y-(y0+shift-wy)) - (iy_y*pitch+wy);

    //since pads are rotated we need to rotate the coordinate system by 45 degrees

    G4double xprime_x = sqrt(0.5)*(x1_x - y1_x);
    G4double yprime_x = sqrt(0.5)*(x1_x + y1_x);
    G4double xprime_y = sqrt(0.5)*(x1_y - y1_y);
    G4double yprime_y = sqrt(0.5)*(x1_y + y1_y);

    //now check if hit point lies inside pad area

    if(abs(xprime_x)<=0.5*sqrt(geo->GetXPadArea()) && abs(yprime_x)<=0.5*sqrt(geo->GetXPadArea())){ // pads for the x coordinate
        if(z<=0){
            fid = ix_x + 1000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
            ftime = s/fvdrift;
        }
        else if(z>0){
            fid = ix_x + 4000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
            ftime = s/fvdrift;
        }

        fIDs.push_back(fid);
        fTimes.push_back(ftime);
        fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));

    }
    
    if(abs(xprime_y)<=0.5*sqrt(geo->GetYPadArea()) && abs(yprime_y)<=0.5*sqrt(geo->GetYPadArea())){ // pads for the y coordinate
        if(z<=0){
            fid = iy_y + 2000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
            ftime = s/fvdrift;
        }
        else if(z>0){
            fid = iy_y + 3000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
            ftime = s/fvdrift;
        }

        fIDs.push_back(fid);
        fTimes.push_back(ftime);
        fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
    }
}