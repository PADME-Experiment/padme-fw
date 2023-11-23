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
const double dt = 1e-12;       // Time step (s)
const int nElectrons = 1000;

// Class definition for generating a random unit vector
class RandomGenerator {
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

    TVector3 randomVector; // Random unit vector
};

void Orbitals() {
    
//random generator
TRandom3 *r1=new TRandom3();
unsigned int seed = 0;

// Initial conditions
double r_sphere = 1.0;   // Radius of the sphere (m)
double v_tan = sqrt((Z * e * e) / (4 * M_PI * m_e * epsilon * r_sphere));  // Tangential velocity

// Arrays to store position coordinates and tangential versor modifications
double x[numSteps];
double y[numSteps];
double z[numSteps];
double tangentialVersorX[numSteps];
double tangentialVersorY[numSteps];
double tangentialVersorZ[numSteps];

// Initial position
double theta = 0.0;
double phi = 0.0;

// Initial velocity components in spherical coordinates
double v_r = 0.0;
double v_theta = 0.0;
double v_phi = v_tan / (r_sphere * sin(theta));

//initializing histograms
TH1D *hzProjection = new TH1D("hzProjection", "hzProjection", nElectrons, -1, 1);

//cresting Outputfile
TFile *FileOut = new TFile("Motion.root", "RECREATE");

vector<TVector3> s1Electron;

int ie = 0;
double zProjection = 0.;

for(ie = 0; ie < nElectrons; ie++){
    seed = r1->Uniform(0, 8739573778973);

    // Update position using velocity and time step
    x[ie] = r_sphere * sin(theta) * cos(phi);
    y[ie] = r_sphere * sin(theta) * sin(phi);
    z[ie] = r_sphere * cos(theta);

    // Calculate the tangential unit vector in spherical coordinates
    tangentialVersorX[ie] = sin(theta) * cos(phi);
    tangentialVersorY[ie] = sin(theta) * sin(phi);
    tangentialVersorZ[ie] = cos(theta);

    // Update velocity components based on centripetal acceleration
    v_r = 0.0;  // Motion is along the sphere's surface
    v_theta = -v_tan / (r_sphere * sin(theta));
    v_phi = v_tan / (r_sphere * sin(theta));

    // Update spherical coordinates for the next iteration
    theta += v_theta * dt;
    phi += v_phi * dt;

    // Ensure theta stays within [0, pi] range
    if (theta < 0.0) theta = 0.0;
    if (theta > M_PI) theta = M_PI;

    RandomGenerator randomVectorGenerator(seed);
    s1Electron.push_back(randomVectorGenerator.getRandomVector());
    s1Electron[ie].SetX() = s1Electron[ie].X() + tangentialVersorX[ie];
    s1Electron[ie].SetY() = s1Electron[ie].Y() + tangentialVersorY[ie];
    s1Electron[ie].SetZ() = s1Electron[ie].Z() + tangentialVersorZ[ie];
    s1Electron[ie].SetMag(1.0;)
    zProjection = s1Electron[ie].Mag() * cos(s1Electron[ie].Angle(zDir));
    hzProjection->Fill(zProjection);
}

FileOut->cd();
hzProjection->Write();
FileOut->Close();


}