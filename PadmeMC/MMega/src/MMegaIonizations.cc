// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2024-03-12
//
// This class generates the ionizations from the Geant4 hit data in the drift volume
//
// Note: Readout Layers are in the order : x_front, y_front, y_back, x_back
// viewing from the target to the ECal
//
// --------------------------------------------------------------

#include "MMegaIonizations.hh"
#include "G4ThreeVector.hh"
#include <cmath>


MMegaIonizations::MMegaIonizations(G4ThreeVector start, G4ThreeVector end, G4double dE)
{

    geo = MMegaGeometry::GetInstance();

    fstepLength = (end-start).mag();
    fdE = dE;
    // fNionizations = r.Poisson(fdE/fWI);
    fNionizations = r.Poisson(fNPrimary*fstepLength);

    for(short i = 0; i < fNionizations; i++){ // generates drift electrons in the step
        double randomPos = r.Uniform(0, fstepLength);
        G4ThreeVector fionPos = start + ((end-start)/fstepLength)*randomPos;
        // fIonizations.push_back(fionPos);
        fRadii.push_back(sqrt(fionPos.x()*fionPos.x() + fionPos.y()*fionPos.y()));

        if(geo->GetReadoutType() == "strips"){
            ComputeStripID(fionPos);
            fIDs.push_back(fid);
            fTimes.push_back(ftime);
        }
        else if(geo->GetReadoutType() == "pads"){
            ComputePadID(fionPos);
            fIDs.push_back(fid);
            fTimes.push_back(ftime);
        } 

    }
}

MMegaIonizations::~MMegaIonizations()
{}

void MMegaIonizations::ComputeStripID(G4ThreeVector ionipos){

    G4double x0 = geo->GetXStripStartPos();
    G4double y0 = geo->GetYStripStartPos();
    G4double w = geo->GetMMegaStripWidth();
    G4double pitch = geo->GetMMegaStripPitch();

    // Reminder : add gaussian smearing for time and space resolution
        
    G4double x = ionipos.x();
    G4double y = ionipos.y();
    G4double z = ionipos.z();

    G4int ix = (x-(x0-0.5*w))/pitch;

    G4int iy = (y-(y0-0.5*w))/pitch;

    if((x-(x0-0.5*w)) - ix * pitch < w){ // if electron hits the strip
        if(z<=0){
            // fIDs.push_back(ix + 10000);
            fid = ix + 10000;
            G4cout << "print ix test : " << ix +10000 << G4endl;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);
        }
        else if(z>0){
            // fIDs.push_back(ix + 40000);
            fid = ix + 40000;
            G4cout << "print ix test : " << ix + 40000 << G4endl;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);
        }
    }
    
    if((y-(y0-0.5*w)) - iy * pitch < w){ // if electron hits the strip
        if(z<=0){
            // fIDs.push_back(iy + 20000);
            fid = iy+20000;
            G4cout << "print iy test : " << iy + 20000 << G4endl;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);
        }
        else if(z>0){
            // fIDs.push_back(iy + 30000);
            fid = iy+30000;
            G4cout << "print iy test : " << iy + 30000 << G4endl;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);
        }
        
    }
}

void MMegaIonizations::ComputePadID(G4ThreeVector ionipos){

    MMegaGeometry* geo = MMegaGeometry::GetInstance();

    G4double x0 = geo->GetXPadStartPos();
    G4double y0 = geo->GetYPadStartPos();
    G4double shift = 0.5*geo->GetPadDistance(); //acounts for the fact that x and y pads are shifted
    G4double wx = sqrt(0.5*geo->GetXPadArea());
    G4double wy = sqrt(0.5*geo->GetYPadArea());
    G4double pitch = geo->GetPadDistance();

    // REM: add gaussian smearing for time and space resolution

    G4double x = ionipos.x();
    G4double y = ionipos.y();
    G4double z = ionipos.z();

    G4int ix_x = (x-(x0-wx))/pitch;
    G4int iy_x = (y-(y0+shift-wx))/pitch;

    G4int ix_y = (x-(x0+shift-wy))/pitch;
    G4int iy_y = (y-(y0-wy))/pitch;

    // shift coordinates in the center of the pad
    G4double x1_x = (x-(x0-wx)) - ix_x*pitch;
    G4double y1_x = (y-(y0+shift-wy)) - iy_x*pitch;

    G4double x1_y = (x-(x0+shift-wx)) - ix_y*pitch;
    G4double y1_y = (y-(y0-wy)) - iy_y*pitch;

    //since pads are rotated we need to rotate the coordinate system by 45 degrees

    G4double xprime_x = sqrt(0.5)*(x1_x - y1_x);
    G4double yprime_x = sqrt(0.5)*(x1_x + y1_x);
    G4double xprime_y = sqrt(0.5)*(x1_y - y1_y);
    G4double yprime_y = sqrt(0.5)*(x1_y + y1_y);

    //now check if hit point lies inside pad area

    if(abs(xprime_x)<=sqrt(geo->GetXPadArea()) && abs(yprime_x)<=sqrt(geo->GetXPadArea())){ // pads for the x coordinate
        if(z<=0){
            // fIDs.push_back(ix_x + 1000);
            fid = ix_x + 1000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);
        }
        else if(z>0){
            // fIDs.push_back(ix_x + 4000);
            fid = ix_x + 4000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);
        }
    }
    
    if(abs(xprime_y)<=sqrt(geo->GetYPadArea()) && abs(yprime_y)<=sqrt(geo->GetYPadArea())){ // pads for the y coordinate
        if(z<=0){
            // fIDs.push_back(iy_y + 2000);
            fid = iy_y + 2000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())+z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);
        }
        else if(z>0){
            // fIDs.push_back(iy_y + 3000);
            fid = iy_y + 3000;
            G4double s = (0.5*geo->GetMMegaDriftSizeZ()+geo->GetMMegaAmpGapSizeZ())-z;
            ftime = s/fvdrift;
            // fTimes.push_back(s/fvdrift);

        }
    }
}