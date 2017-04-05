// SACGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-11
// --------------------------------------------------------------

#include "SACGeometry.hh"

SACGeometry* SACGeometry::fInstance = 0;

SACGeometry* SACGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new SACGeometry(); }
  return fInstance;
}

SACGeometry::SACGeometry()
{

  // Inizialize default parameters

  fCrystalSizeX =  3.*cm;
  fCrystalSizeY =  3.*cm;
  fCrystalSizeZ = 20.*cm;

  fSACNRows = 5;
  fSACNCols = 5;

  fCrystalGap = 50.*um;

  fCrystalCoating = 100.*um;

  fSACFrontFacePosZ = 300.*cm; // from center of yoke, i.e. 370cm from target, 70cm from front of ECal

  fDigiAvgLightSpeed = (2.998E8*m/s)/1.85; // Average light speed inside SAC crystal for Cherenkov spectrum

  // Number of photoelectrons produced by photocathode per MeV of hit energy
  fDigiEtoNPEConversion = 0.5/MeV; // Wild guess: fix it!!!

  fDigiPEtoSignalConversion = 1.; // Contribution of 1 p.e. to integral ADC signal
 
  // Relative collection efficiency as function of Z along the crystal (bin 0: front face, bin N: readout face)
  static const G4double cmap[] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  G4int nbins = 20;
  fDigiPECollectionMap.assign(cmap,cmap+nbins);

  fDigiPMTTransitTime = 9.1*ns; // Hamamatsu R13478 PMT transit time from photocathode to anode
  fDigiPMTCableDelay = 0.*ns; // Delay due to connection cables

  fSACSensitiveDetectorName = "SACSD";

}

SACGeometry::~SACGeometry()
{}

G4int SACGeometry::ExistsCrystalAt(G4int row, G4int col)
{

  // Verify we are within SAC box
  if ( row<0 || row>=fSACNRows || col<0 || col>=fSACNCols ) return 0;

  // Remove central crystal (assumes an odd number of crystals per row/column)
  //if (row==fSACNRows/2 && col==fSACNCols/2) return 0;

  // All other crystals are there
  return 1;

}

G4double SACGeometry::GetCrystalPosX(G4int row, G4int col)
{

  // Verify we are within SAC box
  if (!ExistsCrystalAt(row,col)) {
    printf("SACGeometry::GetCrystalPosX - ERROR - Requested crystal at row %d col %d\n",row,col);
    return 0.;
  }

  // Return X position of center of crystal
  return (GetCellSizeX()+fCrystalGap)*(-fSACNCols*0.5+col+0.5);

}

G4double SACGeometry::GetCrystalPosY(G4int row, G4int col)
{

  // Verify we are within SAC box
  if (!ExistsCrystalAt(row,col)) {
    printf("SACGeometry::GetCrystalPosY - ERROR - Requested crystal at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Y position of center of crystal
  return (GetCellSizeY()+fCrystalGap)*(-fSACNRows*0.5+row+0.5);

}

G4double SACGeometry::GetCrystalPosZ(G4int row, G4int col)
{

  // Verify we are within SAC box
  if (!ExistsCrystalAt(row,col)) {
    printf("SACGeometry::GetCrystalPosZ - ERROR - Requested crystal at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Z position of center of crystal in local SAC coordinate system
  return 0.*cm;

}

std::vector<G4String> SACGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fSACNRows " << fSACNRows;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fSACNCols " << fSACNCols;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fSACFrontFacePosZ " << fSACFrontFacePosZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fCrystalSizeX " << fCrystalSizeX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fCrystalSizeY " << fCrystalSizeY;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fCrystalSizeZ " << fCrystalSizeZ;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fCrystalGap " << fCrystalGap;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fCrystalCoating " << fCrystalCoating;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fDigiEtoNPEConversion " << fDigiEtoNPEConversion;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fDigiPEtoSignalConversion " << fDigiPEtoSignalConversion;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fDigiPMTTransitTime " << fDigiPMTTransitTime;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fDigiPMTCableDelay " << fDigiPMTCableDelay;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fDigiPECollectionMap";
  for(G4int i=0;i<fDigiPECollectionMap.size();i++) buffer << " " << fDigiPECollectionMap[i];
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;

}
