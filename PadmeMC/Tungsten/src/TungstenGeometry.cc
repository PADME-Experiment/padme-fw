// TungstenGeometry.cc
// --------------------------------------------------------------
// History:
//
// 2017-10-03 Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
// --------------------------------------------------------------

#include "TungstenGeometry.hh"

#include "G4SystemOfUnits.hh"

TungstenGeometry* TungstenGeometry::fInstance = 0;

TungstenGeometry* TungstenGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new TungstenGeometry(); }
  return fInstance;
}

TungstenGeometry::TungstenGeometry()
{

  // Inizialize default parameters

  fDetectorSetup = 1; // Default to 2019 setup

  fTungstenRadius = 5.*cm-2.*um; // Fill junction pipe taking into account magnetic volume tolerances
  fTungstenSizeZ = 10.*cm;

  fTungstenFrontFacePosZ = -80.*cm; // Relative to center of magnet

}

TungstenGeometry::~TungstenGeometry()
{}

std::vector<G4String> TungstenGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fTungstenRadius " << fTungstenRadius;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTungstenSizeZ " << fTungstenSizeZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fTungstenFrontFacePosZ " << fTungstenFrontFacePosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;

}
