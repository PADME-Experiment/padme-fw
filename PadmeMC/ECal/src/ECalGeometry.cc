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

  fCrystalGap = 100.*um;

  fCrystalCoating = 100.*um;

  fECalFrontFacePosZ = 230.*cm; // From center of magnet yoke, i.e. 3m to target

  fTedlarThickness = 50.*um; // Thickness of Tedlar slips

  // Geometry parameters for plastic panel in front of ECal
  fECalPanelEnable = false;
  fECalPanelThickness = 0.5*cm;
  fECalPanelSizeX = 62.*cm; // Slightly larger than ECal (61.625cm)
  fECalPanelSizeY = 62.*cm; // Slightly larger than ECal (61.625cm)
  fECalPanelGap = 0.100*mm;

  // Set default map of crystals in ECal to big (5x5) hole
  fCrystalMapId = 0;
  SetCrystalMap();

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

void ECalGeometry::SetCrystalMap()
{

  // Maps of ECal crystals
  // Y grows from top to bottom
  // X grows from left to right

  // Big (5x5) hole
  G4int tmpMap0[29*29] = {
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

  // Small (3x3) hole
  G4int tmpMap1[29*29] = {
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
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
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

  if ( fCrystalMapId == 0 ) {

    printf("ECalGeometry::SetCrystalMap - Using crystal map with 5x5 hole.\n");
    for (G4int r=0; r<fECalNRows; r++) {
      for (G4int c=0; c<fECalNCols; c++) {
	fECalCrystalMap[r][c] = tmpMap0[r*fECalNCols+c];
      }
    }

  } else if ( fCrystalMapId == 1 ) {

    printf("ECalGeometry::SetCrystalMap - Using crystal map with 3x3 hole.\n");
    for (G4int r=0; r<fECalNRows; r++) {
      for (G4int c=0; c<fECalNCols; c++) {
	fECalCrystalMap[r][c] = tmpMap1[r*fECalNCols+c];
      }
    }

  } else {

    printf("ECalGeometry::SetCrystalMap - ERROR - Requested map %d does not exist. Using empty map.\n",fCrystalMapId);
    for (G4int r=0; r<fECalNRows; r++) {
      for (G4int c=0; c<fECalNCols; c++) {
	fECalCrystalMap[r][c] = 0;
      }
    }

  }

}

G4int ECalGeometry::ExistsCrystalAt(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) return 0;

  return fECalCrystalMap[row][col];

}

G4int ECalGeometry::ExistsTedlarHAt(G4int row,G4int col)
{

  // Verify we are within ECal box (excluding last row)
  if ( row<0 || row>=fECalNRows-1 || col<0 || col>=fECalNCols ) return 0;

  // Verify that we have two adjacent crystals
  if ( fECalCrystalMap[row][col] && fECalCrystalMap[row+1][col] ) return 1;

  return 0;

}

G4int ECalGeometry::ExistsTedlarVAt(G4int row,G4int col)
{

  // Verify we are within ECal box (excluding last column)
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols-1 ) return 0;

  // Verify that we have two adjacent crystals
  if ( fECalCrystalMap[row][col] && fECalCrystalMap[row][col+1] ) return 1;

  return 0;

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

G4double ECalGeometry::GetTedlarHPosX(G4int row, G4int col)
{

  // Verify if horizontal Tedlar slip exists
  if ( ! ExistsTedlarHAt(row,col) ) {
    printf("ECalGeometry::GetTedlarHPosX - ERROR - Requested position of horizontal Tedlar slip at row %d col %d\n",row,col);
    return 0.;
  }

  // Return X position of center of slip in local coordinate system
  return (GetCellSizeX()+fCrystalGap)*(-fECalNCols*0.5+col+0.5);

}

G4double ECalGeometry::GetTedlarHPosY(G4int row, G4int col)
{

  // Verify if horizontal Tedlar slip exists
  if ( ! ExistsTedlarHAt(row,col) ) {
    printf("ECalGeometry::GetTedlarHPosY - ERROR - Requested position of horizontal Tedlar slip at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Y position of center of slip in local coordinate system
  return (GetCellSizeY()+fCrystalGap)*(-fECalNRows*0.5+row+1);

}

G4double ECalGeometry::GetTedlarHPosZ(G4int row, G4int col)
{

  // Verify if horizontal Tedlar slip exists
  if ( ! ExistsTedlarHAt(row,col) ) {
    printf("ECalGeometry::GetTedlarHPosZ - ERROR - Requested position of horizontal Tedlar slip at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Z position of center of slip in local coordinate system
  return 0.;

}

G4double ECalGeometry::GetTedlarVPosX(G4int row, G4int col)
{

  // Verify if vertical Tedlar slip exists
  if ( ! ExistsTedlarVAt(row,col) ) {
    printf("ECalGeometry::GetTedlarVPosX - ERROR - Requested position of vertical Tedlar slip at row %d col %d\n",row,col);
    return 0.;
  }

  // Return X position of center of slip in local coordinate system
  return (GetCellSizeX()+fCrystalGap)*(-fECalNCols*0.5+col+1);

}

G4double ECalGeometry::GetTedlarVPosY(G4int row, G4int col)
{

  // Verify if vertical Tedlar slip exists
  if ( ! ExistsTedlarVAt(row,col) ) {
    printf("ECalGeometry::GetTedlarVPosY - ERROR - Requested position of vertical Tedlar slip at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Y position of center of slip in local coordinate system
  return (GetCellSizeY()+fCrystalGap)*(-fECalNRows*0.5+row+0.5)-0.5*fCrystalCoating;

}

G4double ECalGeometry::GetTedlarVPosZ(G4int row, G4int col)
{

  // Verify if vertical Tedlar slip exists
  if ( ! ExistsTedlarVAt(row,col) ) {
    printf("ECalGeometry::GetTedlarVPosZ - ERROR - Requested position of vertical Tedlar slip at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Z position of center of slip in local coordinate system
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

  buffer << "fTedlarThickness " << fTedlarThickness;
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
