// HallGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-11
// --------------------------------------------------------------

#include "HallGeometry.hh"

#include "globals.hh"
#include "G4SystemOfUnits.hh"

HallGeometry* HallGeometry::fInstance = 0;

HallGeometry* HallGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HallGeometry(); }
  return fInstance;
}

HallGeometry::HallGeometry()
{

  // Inizialize default parameters

  fBlockGap = 0.1*mm;

  fBlockNominalSide = 1.*m;

  fBlockPosition[ 0] = G4ThreeVector( 100.*cm,-100.*cm,430.*cm);
  fBlockPosition[ 1] = G4ThreeVector(   0.*cm,-100.*cm,430.*cm);
  fBlockPosition[ 2] = G4ThreeVector(-100.*cm,-100.*cm,430.*cm);
  fBlockPosition[ 3] = G4ThreeVector(-200.*cm,-100.*cm,430.*cm);
  fBlockPosition[ 4] = G4ThreeVector(-200.*cm,-100.*cm,330.*cm);
  fBlockPosition[ 5] = G4ThreeVector(-200.*cm,-100.*cm,230.*cm);
  fBlockPosition[ 6] = G4ThreeVector(-200.*cm,-100.*cm,130.*cm);
  fBlockPosition[ 7] = G4ThreeVector(-200.*cm,-100.*cm, 30.*cm);
  fBlockPosition[ 8] = G4ThreeVector( 100.*cm,   0.*cm,430.*cm);
  fBlockPosition[ 9] = G4ThreeVector(   0.*cm,   0.*cm,430.*cm);
  fBlockPosition[10] = G4ThreeVector(-100.*cm,   0.*cm,430.*cm);
  fBlockPosition[11] = G4ThreeVector(-200.*cm,   0.*cm,430.*cm);
  fBlockPosition[12] = G4ThreeVector(-200.*cm,   0.*cm,330.*cm);
  fBlockPosition[13] = G4ThreeVector(-200.*cm,   0.*cm,230.*cm);
  fBlockPosition[14] = G4ThreeVector(-200.*cm,   0.*cm,130.*cm);
  fBlockPosition[15] = G4ThreeVector(-200.*cm,   0.*cm, 30.*cm);

}

HallGeometry::~HallGeometry()
{}
