// ECalGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2015-12-11
// --------------------------------------------------------------

#include "ECalGeometry.hh"

ECalGeometry* ECalGeometry::fInstance = 0;

ECalGeometry* ECalGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalGeometry(); }
  return fInstance;
}

ECalGeometry::ECalGeometry()
{

  // Inizialize default parameters

  fCrystalSizeX =  2.1*cm;
  fCrystalSizeY =  2.1*cm;
  fCrystalSizeZ = 23.0*cm;

  fECalNRows = 29;
  fECalNCols = 29;

  fCrystalGap = 0.050*mm;

  fCrystalCoating = 0.100*mm;

  fECalFrontFacePosZ = 230.*cm; // From center of magnet yoke, i.e. 3m to target

  // Geometry parameters for plastic panel in front of ECal
  fECalPanelEnable = true;
  fECalPanelThickness = 0.5*cm;
  fECalPanelSizeX = 62.*cm; // Slightly larger than ECal (61.625cm)
  fECalPanelSizeY = 62.*cm; // Slightly larger than ECal (61.625cm)
  fECalPanelGap = 0.100*mm;

  // Map of ECal crystals
  // Y grows from top to bottom
  // X grows from left to right
  G4int tmpMap[29*29] = {
  0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
  0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
  0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
  0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
  0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
  0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0
  };
  for (G4int r=0; r<fECalNRows; r++) {
    for (G4int c=0; c<fECalNCols; c++) {
      fECalCrystalMap[r][c] = tmpMap[r*fECalNCols+c];
    }
  }

  // Number of photoelectrons produced by photocathode per MeV of hit energy
  // sqrt(N(E))/N(E) = sigma(E)/E = 2%/sqrt(E(GeV)) -> N(E) = 2.5*E(MeV)
  fDigiEtoNPEConversion = 2.5/MeV;

  fDigiPEtoSignalConversion = 1.; // Contribution of 1 p.e. to integral ADC signal
 
  // Relative collection efficiency as function of Z along the crystal (bin 0: front face, bin N: readout face)
  static const G4double cmap[] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.};
  G4int nbins = 23;
  fDigiPECollectionMap.assign(cmap,cmap+nbins);

  fDigiPMTTransitTime = 23.*ns; // HZC XP1911 PMT transit time from photocathode to anode
  fDigiPMTCableDelay = 0.*ns; // Delay due to connection cables

  fECalSensitiveDetectorName = "ECalSD";

}

ECalGeometry::~ECalGeometry()
{}

G4int ECalGeometry::ExistsCrystalAt(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) return 0;

  return fECalCrystalMap[row][col];

  // *** Replaced with explicit map ***
  //// Compute X/Y position of center of crystal
  //G4double posX = (fCrystalSizeX+fCrystalGap)*(-fECalNCols*0.5+col+0.5);
  //G4double posY = (fCrystalSizeY+fCrystalGap)*(-fECalNRows*0.5+row+0.5);
  //
  //// See if center of crystal falls inside inner-outer radius range
  //G4double r2 = posX*posX+posY*posY;
  //if (r2<fECalInnerRadius*fECalInnerRadius || r2>fECalOuterRadius*fECalOuterRadius) return 0;
  //
  //return 1;

}

G4double ECalGeometry::GetCrystalPosX(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetCrystalPosX - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return X position of center of crystal in local coordinate system
  //return (fCrystalSizeX+fCrystalGap)*(-fECalNCols*0.5+col+0.5);
  return (GetCellSizeX()+fCrystalGap)*(-fECalNCols*0.5+col+0.5);

}

G4double ECalGeometry::GetCrystalPosY(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetCrystalPosY - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return Y position of center of crystal in local coordinate system
  //return (fCrystalSizeY+fCrystalGap)*(-fECalNRows*0.5+row+0.5);
  return (GetCellSizeY()+fCrystalGap)*(-fECalNRows*0.5+row+0.5);

}

G4double ECalGeometry::GetCrystalPosZ(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetCrystalPosZ - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return Z position of center of crystal in local coordinate system
  return 0.;

}

std::vector<G4String> ECalGeometry::GetHashTable()
{

  std::vector<G4String> hash;
  std::ostringstream buffer;

  buffer << "fECalNRows " << fECalNRows;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fECalNCols " << fECalNCols;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fECalFrontFacePosZ " << fECalFrontFacePosZ;
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
