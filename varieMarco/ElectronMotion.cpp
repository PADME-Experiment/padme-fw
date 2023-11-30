// Electron motion toy analysis

#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h> 
#include <TString.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TGraph.h> 
#include <TMultiGraph.h> 
#include <TAxis.h>
#include <TRandom.h>
#include <TRandom3.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TVector3.h>
#include <regex>
#include <ctime>

using namespace std;
static const TVector3 zDir(0,0,1);
const double Z = 6;          // Atomic number
const double e = 1.602e-19;    // Elementary charge (Coulombs)
const double m_e = 9.109e-31;  // Electron mass (kg)
const double epsilon = 8.854e-12;  // Permittivity of free space (F/m)
// Simulation parameters
const double dt = 2e-6;       // Time step (s)
const int nElectrons = 100000;
const double Pi = TMath::Pi();

// Class definition for generating a random unit vector
class RandomVectorGenerator {
public:
    // Constructor: initializes the random number generator and generates a random unit vector
    RandomVectorGenerator(unsigned int seed) : randGen(seed) {
        generateRandomVector();
    }

    // Public method to get the generated random vector
    TVector3 getRandomVector() const {
        return randomVector;
    }

private:
    // Private members
    TRandom3 randGen;     // Random number generator
    TVector3 randomVector; // Random unit vector

    // Private method to generate a random unit vector
    void generateRandomVector() {
        // Generate random coordinates between -1.0 and 1.0
        double x = randGen.Uniform(-1.0, 1.0);
        double y = randGen.Uniform(-1.0, 1.0);
        double z = randGen.Uniform(-1.0, 1.0);

        // Set the coordinates of the random vector
        randomVector.SetXYZ(x, y, z);

        // Normalize the vector to have a magnitude of 1.0
        randomVector.SetMag(1.0);
    }
};

void Orbitals() {
    
//random generator
TRandom3 *r1=new TRandom3();
unsigned int seed = 0;

// Initial conditions
double r_sphere = 1.0;   // Radius of the sphere (m)
double v_tan = sqrt((Z * e * e) / (4 * Pi * m_e * epsilon * r_sphere));  // Tangential velocity

// Arrays to store position coordinates and tangential versor modifications
// double x[nElectrons];
double y[nElectrons];
double z[nElectrons];
// double tangentialVersorX[nElectrons];
double tangentialVersorY[nElectrons];
double tangentialVersorZ[nElectrons];

// Initial position
double theta = 0.001;
// double phi = 0.001;

// Initial velocity components in spherical coordinates
double v_r = 0.0;
double v_theta = 0.0;
// double v_phi = 0.0;  //v_tan / (r_sphere * sin(theta));

//initializing histograms
int nbins = 1000;
TH1D *hRNDVectorZ = new TH1D("hRNDVectorZ", "Unitary random vector - Z dir", nbins, -1, 1);
// TH1D *hVTanTheta = new TH1D("hVTanThetA", "Tangential velocity theta", nbins, -10, 10);
TH2D *hCircle = new TH2D("hCircle", "Circle", nbins, -2, 2, nbins, -2, 2);
// TH3D *hSphere = new TH3D("hCircle", "Circle", nbins, -2, 2, nbins, -2, 2, nbins, -2, 2);
// TH1D *hVTanPhi = new TH1D("hVTanPhi", "Tangential velocity phi", nbins, -10, 10);
TH1D *hzProjection = new TH1D("hzProjection", "momentum projection along z", nbins, -1, 1);
std::cout<<"histo definition ok "<<std::endl;

//cresting Outputfile
TFile *FileOut = new TFile("Motion.root", "RECREATE");

TVector3 s1Electron[nElectrons];
TVector3 vector;
double zProjection = 0.;

for(int ie = 0; ie < nElectrons; ie++){
    // std::cout<<"entering in the loop "<<std::endl;
    vector.SetXYZ(0,0,0);
    zProjection = 0.;
    seed = r1->Uniform(0, 8739573778973);
    // std::cout<<"seed per iteration: "<<seed<<std::endl;

    // Update position using velocity and time step
    // x[ie] = r_sphere * sin(theta) * cos(phi);
    y[ie] = r_sphere * sin(theta); // * sin(phi);
    z[ie] = r_sphere * cos(theta);
    // y[ie] = r_sphere * sin(theta);
    // z[ie] = r_sphere * cos(theta);
    hCircle->Fill(z[ie], y[ie]);

    // Calculate the tangential unit vector in spherical coordinates
    // tangentialVersorX[ie] = sin(theta) * cos(phi);
    tangentialVersorY[ie] = sin(theta); // * sin(phi);
    tangentialVersorZ[ie] = cos(theta);
    // std::cout<<"tangentialVersor X: "<<tangentialVersorX[ie]<<" Y: "<<tangentialVersorY[ie]<<" Z: "<<tangentialVersorZ[ie]<<std::endl;
    std::cout<<"tangentialVersor Y: "<<tangentialVersorY[ie]<<" Z: "<<tangentialVersorZ[ie]<<std::endl; 

    // Update velocity components based on centripetal acceleration
    v_r = 0;  // Motion is along the sphere's surface
    // std::cout<<"Random radial velocity: "<< v_r <<std::endl;
    v_theta = v_tan / r_sphere;
    // hVTanTheta->Fill(v_theta);
    // v_phi = 0;
    // hVTanPhi->Fill(v_phi);

    // Update spherical coordinates for the next iteration
    r_sphere = r_sphere + v_r * dt;
    theta = theta + v_theta * dt;
    
    RandomVectorGenerator randomVectorGenerator(seed);
    vector = randomVectorGenerator.getRandomVector();
    hRNDVectorZ->Fill(vector.Z());
    // std::cout<<"random vector X: "<<vector.X()<<" Y: "<<vector.Y()<<" Z: "<<vector.Z()<<std::endl;
    s1Electron[ie].SetX(vector.X());
    // std::cout<<"s1Electron X: "<<s1Electron[ie].X()<<std::endl;
    s1Electron[ie].SetY(vector.Y() + tangentialVersorY[ie]);
    // std::cout<<"s1Electron Y: "<<s1Electron[ie].Y()<<std::endl;
    s1Electron[ie].SetZ(vector.Z() + tangentialVersorZ[ie]);
    // std::cout<<"s1Electron Z: "<<s1Electron[ie].Z()<<std::endl; 
    s1Electron[ie].SetMag(1.0);
    // std::cout<<"s1Electron X: "<<s1Electron[ie].X()<<" Y: "<<s1Electron[ie].Y()<<" Z: "<<s1Electron[ie].Z()<<std::endl;
    zProjection = s1Electron[ie].Mag() * cos(s1Electron[ie].Angle(zDir));
    hzProjection->Fill(zProjection);

    // Ensure theta stays within [0, Pi] range
    // if (theta < 0.0) theta = 0.001;
    // phi = phi + Pi/nElectrons;
    if (theta > 2*Pi) {
        std::cout<<"a full circle is completed after n= "<<ie<<" iteration->Break and look histos"<<std::endl;
        break;
        theta = 0.001;
    }
}

hRNDVectorZ->Draw();
// hVTanTheta->Draw();
hCircle->Draw();
// hSphere->Draw();
hzProjection->Draw();

FileOut->cd();
hRNDVectorZ->Write();
// hVTanTheta->Write();
hCircle->Write();
// hSphere->Write();
// hVTanPhi->Write();
hzProjection->Write();
if (hzProjection) std::cout<<"Saving histos"<<std::endl;
FileOut->Close();

}