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

#include "MMIonizations.hh"

#include "Randomize.hh"
#include "G4Poisson.hh"
#include "G4UnitsTable.hh"
#include <cmath>


MMIonizations::MMIonizations(G4ThreeVector start, G4ThreeVector end, G4double dE)
{

    geo = MMGeometry::GetInstance();
    r = new TRandom3();
    r->SetSeed(0);
    
    InitializeChargeDistribution();
    InitializeTimeSpread();

    fstepLength = (end-start).mag();
    fdE = dE;
    fNionizations = fdE/fWI;
    ////G4cout << "r.Poisson ---- MMIonizations.cc : fNionizations = " << fNionizations<<" fdE "<<fdE<< "fWI "<<fWI << G4endl;
    //fNionizations = G4Poisson(fNPrimary*fstepLength);
    ////G4cout << "G4Poisson -- MMIonizations.cc : fNionizations = " << fNionizations << G4endl;

    // //G4cout << "MMIonizations.cc : dE = " << fdE << G4endl;
    ////G4cout << "MMIonizations.cc : steplength = " << fstepLength << G4endl;
    //G4cout << "MMIonizations.cc : fNionizations = " << fNionizations << G4endl;
    

    for(short i = 0; i < fNionizations; i++){ // generate drift electrons in the step
        
        double randomPos = G4UniformRand()*fstepLength;
        // //G4cout << "MMIonizations.cc : randomPos = " << randomPos << G4endl;
        
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

MMIonizations::~MMIonizations(){
    delete r;
}


// Define and initialize the static member variable to nullptr
TF1* MMIonizations::charge_dist = nullptr;
TF1* MMIonizations::time_spread = nullptr;

void MMIonizations::InitializeTimeSpread(){
    
    if (!time_spread) {
        
        // Initialize function according to parameters fitted from real data
        // coming from may 2024 Test Beam at LNF 
        time_spread = new TF1("time_spread", "[0] + [1]/x + [2]/x^2", 0.01, 2500);  // starting from 0.01 to avoid divergence
        time_spread->SetParameter(0, 6.5);
        time_spread->SetParameter(1, 904.8);
        time_spread->SetParameter(2, 1905);

    }
}
void MMIonizations::InitializeChargeDistribution(){
    
    if (!charge_dist) {
        
        // Initialize function according to parameters fitted from real data
        // coming from may 2024 Test Beam at LNF 
        charge_dist = new TF1("ChargeFluctuation", "landau", 0.0001, 100);  // [0, 2500] Charge in ADC Counts
        charge_dist->SetParameter(0, 0.0134);
        charge_dist->SetParameter(1, 10000.);
        charge_dist->SetParameter(2, 2600.);

    }
}
G4double MMIonizations::GetChargeFromDistribution(){
    return charge_dist->GetRandom();
}




G4double MMIonizations::GetTimeSpread(G4double charge){
    // //G4cout << "MMIonizations TimeSpread : " << time_spread->Eval(charge) << G4endl;
    // //G4cout << "MMIonizations TimeSpread : " << time_spread->Eval(charge) * ns<< G4endl;
    return G4RandGauss::shoot(0, time_spread->Eval(charge))*ns;
}

// void MMIonizations::ComputeStripID(G4ThreeVector ionipos){

//     G4double x0 = geo->GetXStripStartPos();
//     G4double y0 = geo->GetYStripStartPos();
//     G4double w = geo->GetMMStripWidth();
//     G4double pitch = geo->GetMMStripPitch();
        
//     G4double x = ionipos.x();
//     G4double y = ionipos.y();
//     G4double z = ionipos.z();

//     G4int ix = (x-(x0-0.5*w))/pitch;
//     G4int iy = (y-(y0-0.5*w))/pitch;

//     if (ix >= 0 && ix < 2000){
//         if((x-(x0-0.5*w)) - ix * pitch < w){ // if electron hits the strip
//             if(z<=0){
//                 fid = ix + 10000;
//                 G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())+z;
//                 // //G4cout << "MMIonizations::ComputeStripID : s = " << s << G4endl;
//                 ftime = s/fvdrift;
//             }
//             else if(z>0){
//                 fid = ix + 40000;
//                 G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())-z;
//                 // //G4cout << "MMIonizations::ComputeStripID : s = " << s << G4endl;
//                 ftime = s/fvdrift;
//             }
        
//             fIDs.push_back(fid);
//             fTimes.push_back(ftime);
//             fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
//             // //G4cout << "MMIonizations::ComputeStripID : time = " << ftime << G4endl;
//         }
//     }

//     if (iy >= 0 && iy < 2000){
//         if((y-(y0-0.5*w)) - iy * pitch < w){ // if electron hits the strip
//             if(z<=0){
//                 fid = iy+20000;
//                 G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())+z;
//                 // //G4cout << "MMIonizations::ComputeStripID : s = " << s << G4endl;
//                 ftime = s/fvdrift;
//             }
//             else if(z>0){
//                 fid = iy+30000;
//                 G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())-z;
//                 // //G4cout << "MMIonizations::ComputeStripID : s = " << s << G4endl;
//                 ftime = s/fvdrift;
//             }

//             fIDs.push_back(fid);
//             fTimes.push_back(ftime);
//             fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
//             // //G4cout << "MMIonizations::ComputeStripID : time = " << ftime/ns << G4endl;
            
//         }
//     }
// }

void MMIonizations::ComputeStripID(G4ThreeVector ionipos){

    G4double xP0 = geo->GetV1StripStartPos(); //plane 0 X readout
    G4double xP1 = geo->GetV2StripStartPos(); //plane 1 X readout
    G4double yP0 = geo->GetV2StripStartPos(); //plane 0 Y readout
    G4double yP1 = geo->GetV1StripStartPos(); //plane 1 Y readout
    G4double w = geo->GetMMStripWidth();
    G4double pitch = geo->GetMMStripPitch();
    G4double V1Panel= geo->GetMMPanelSizeV1(); 
    G4double V2Panel= geo->GetMMPanelSizeV2(); 
    G4int V1Hole = geo->GetV1StripHole();
    G4int V2Hole = geo->GetV2StripHole();
    G4double x = ionipos.x();
    G4double y = ionipos.y();
    G4double z = ionipos.z();
    // G4double xabsshift= geo->GetMMPosX();
    // G4double yabsshift= geo->GetMMPosY();

    G4int iChx; 
    G4int iChy;
    //G4double radii = sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y());
    //G4cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<G4endl;
    //G4cout<<"xP0: "<<xP0<<" xP0+V1Panel "<<xP0+V1Panel<<"yP0: "<<yP0<<" yP0+V2Panel "<<yP0+V2Panel<<G4endl;
    //G4cout<<"xP1: "<<xP1<<" xP1+V2Panel "<<xP1+V2Panel<<"yP1: "<<yP1<<" yP1+V1Panel "<<yP1+V1Panel<<G4endl;
    
    if(z<0){ //if the particle has not crossed half of the volume the readout is made at P0
        iChx = (x-(xP0))/pitch;
        iChy = (y-(yP0))/pitch;
        // iChx = (x-(xP0-0.5*w))/pitch;
        // iChy = (y-(yP0-0.5*w))/pitch;
        if((x<xP0 || x>xP0+V1Panel) && (y<yP0 || y>yP0+V2Panel)) return;
        ftime = ((0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())+z)/fvdrift;
        if(geo->GetisChV2Active(iChy) && (y>yP0 && y<yP0+V2Panel)){  //mask non active strips in Y
            if(x<0){
                if(y<0) fid = iChy; //brd 0
                else fid = iChy-V2Hole + 1000; //brd 1, -6 to account for the hole
            }else{
                if(y<0) fid = iChy + 2000; //brd 2
                else fid = iChy-V2Hole + 3000; //brd 3, -6 to exclude the hole      
            }
            fIDs.push_back(fid);
            fTimes.push_back(ftime);
            fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
        }
        if(geo->GetisChV1Active(iChx) && (x>xP0 && x<xP0+V1Panel)){  //mask non active strips in X
            if(y>0){
                if(x<0){ fid = iChx + 4000; //brd 4
                //    if(iChx==0) std::cout << "MMIonizations::ComputeStripID : fid = " << fid << " x "<< x - xP0<<" y "<< y << G4endl;
                //    if(iChx==1) std::cout << "MMIonizations::ComputeStripID : fid = " << fid << " x "<< x - xP0 <<" y "<< y << G4endl;
                }
                else fid = iChx-V1Hole + 5000; //brd 5, -1 to exclude the hole
            }else{
                if(x<0) fid = iChx + 6000; //brd 6
                else fid = iChx-V1Hole + 7000; //brd 7, -1 to exclude the hole
                
            }
            fIDs.push_back(fid);
            fTimes.push_back(ftime);
            fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
        }
    }else{ //if the particle has crossed half of the volume the readout is made at P1 
        // iChx = (x-(xP1-0.5*w))/pitch;
        // iChy = (y-(yP1-0.5*w))/pitch;
        iChx = (x-(xP1))/pitch;
        iChy = (y-(yP1))/pitch;
        if((x<xP1 || x>xP1+V2Panel) && (y<yP1 || y>yP1+V1Panel)) return;
        ftime = ((0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())-z)/fvdrift;
        if(geo->GetisChV1Active(iChy) && (y>yP1 && y<yP1+V1Panel)){ //mask non active strips
            if(x<0){
                if(y<0) fid = iChy  + 8000; //brd 8
                else fid = iChy-V1Hole + 9000; //brd 9, -1 to account for the hole
            }else{
                
                if(y<0) fid = iChy  + 10000; //brd 10
                else fid = iChy-V1Hole + 11000; //brd 11, -1 to exclude the hole      
            }
            fIDs.push_back(fid);
            fTimes.push_back(ftime);
            fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
        }
        if(geo->GetisChV2Active(iChx) && (x>xP1 && x<xP1+V2Panel)){ //mask non active strips
            if(y>0){
                if(x<0) fid = iChx + 12000; //brd 12
                else fid = iChx-V2Hole + 13000; //brd 13, -6 to exclude the hole
            }else{
                if(x<0) fid = iChx + 14000; //brd 14
                else fid = iChx-V2Hole + 15000; //brd 15, -6 to exclude the hole
            }
            fIDs.push_back(fid);
            fTimes.push_back(ftime);
            fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
        }

    }
    
}
void MMIonizations::ComputePadID(G4ThreeVector ionipos){

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
            G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())+z;
            ftime = s/fvdrift;
        }
        else if(z>0){
            fid = ix_x + 4000;
            G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())-z;
            ftime = s/fvdrift;
        }

        fIDs.push_back(fid);
        fTimes.push_back(ftime);
        fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));

    }
    
    if(abs(xprime_y)<=0.5*sqrt(geo->GetYPadArea()) && abs(yprime_y)<=0.5*sqrt(geo->GetYPadArea())){ // pads for the y coordinate
        if(z<=0){
            fid = iy_y + 2000;
            G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())+z;
            ftime = s/fvdrift;
        }
        else if(z>0){
            fid = iy_y + 3000;
            G4double s = (0.5*geo->GetMMDriftSizeZ()+geo->GetMMAmpGapSizeZ())-z;
            ftime = s/fvdrift;
        }

        fIDs.push_back(fid);
        fTimes.push_back(ftime);
        fRadii.push_back(sqrt(ionipos.x()*ionipos.x() + ionipos.y()*ionipos.y()));
    }
}