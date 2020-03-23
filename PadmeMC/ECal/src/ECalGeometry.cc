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

  fVerbose = 0; // Do not show debug output

  fECalSizeX = 690.*mm; // Same as front panel
  fECalSizeY = 690.*mm; // Same as pront panel
  fECalSizeZ = 250.*mm; // Will become longer if we include the length of PMTs

  fCrystalSizeX =  2.1*cm;
  fCrystalSizeY =  2.1*cm;
  fCrystalSizeZ = 23.0*cm;

  fECalNRows = 29;
  fECalNCols = 29;

  fCrystalGap = 120.*um;

  fCrystalCoating = 115.*um;

  // Small displacement along X/Y from Jan 2020 survey
  fECalDisplacementX = 3.13*mm;
  fECalDisplacementY = 3.86*mm;

  // Back surface of thin window flange is at 2323mm
  //fECalFrontFacePosZ = 2330.*mm; // From center of magnet yoke, i.e. 3.33m to target
  //fECalFrontFacePosZ = 2340.*mm; // From center of magnet yoke, i.e. 3.34m to target
  //fECalFrontFacePosZ = 2440.*mm; // From center of magnet yoke, i.e. 3.44m to target includes calibration system M.Raggi 10/10/2018
  fECalFrontFacePosZ = 2435.51*mm; // From center of magnet yoke, from Jan 2020 survey (2550.51 - 230/2)

  fTedlarThickness = 50.*um; // Thickness of Tedlar slips

  // Geometry parameters for Alustep panel in front of ECal
  fECalPanelEnable = true;
  fECalPanelThickness = 15.*mm;
  fECalPanelFoilThickness = 0.3*mm;
  fECalPanelGlueThickness = 0.2*mm;
  fECalPanelSizeX = 690.*mm;
  fECalPanelSizeY = 690.*mm;
  fECalPanelHoleRadius = 48.*mm;
  fECalPanelGap = 0.1*mm;

  // Geometry parameters for ASA support structure inside ECal central hole
  fECalSupportEnable = true;

  fECalSupportSizeX = 106.85*mm;
  fECalSupportSizeY = 106.85*mm;
  fECalSupportSizeZ = 240.*mm;
  fECalSupportHoleRadius = 48.*mm;
  fECalSupportFrontThickness = 5.*mm;
  fECalSupportSideThickness = 2.*mm;
  fECalSupportBackThickness = 2.*mm;

  fECalSupportLatticeNCellXY = 4;
  fECalSupportLatticeNCellZ = 7;
  fECalSupportLatticeHoleWidth = 22.46*mm;
  fECalSupportLatticeHoleLength = 31.*mm;
  fECalSupportLatticeFrame = 1.*mm;
  fECalSupportLatticeThickness = 2.*mm;

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

G4int ECalGeometry::ExistsExternalSupportAt(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) return 0;

  // Check if a crystal exists at that position
  if ( fECalCrystalMap[row][col] ) return 0;

  // The central 5x5 hole has a special support structure
  G4int centralHoleXStart = fECalNCols/2-2;
  G4int centralHoleXEnd = fECalNCols/2+2;
  G4int centralHoleYStart = fECalNRows/2-2;
  G4int centralHoleYEnd = fECalNRows/2+2;
  //printf("%d %d %d %d %d %d\n",row,centralHoleXStart,centralHoleXEnd,col,centralHoleYStart,centralHoleYEnd);
  if ( row >= centralHoleYStart && row <= centralHoleYEnd && col >= centralHoleXStart && col <= centralHoleXEnd ) {
    //printf("- Inside central hole\n");
    return 0;
  }

  // No crystal and not in central hole: create external support structure
  return 1;

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

  // Return Z position of center of cell in local coordinate system
  return 0.5*(-fECalSizeZ+fCrystalGap+GetCellSizeZ());

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
  return 0.5*(-fECalSizeZ+fCrystalGap+GetCellSizeZ());

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
  //return (GetCellSizeY()+fCrystalGap)*(-fECalNRows*0.5+row+0.5)-0.5*fCrystalCoating;
  return (GetCellSizeY()+fCrystalGap)*(-fECalNRows*0.5+row+0.5);

}

G4double ECalGeometry::GetTedlarVPosZ(G4int row, G4int col)
{

  // Verify if vertical Tedlar slip exists
  if ( ! ExistsTedlarVAt(row,col) ) {
    printf("ECalGeometry::GetTedlarVPosZ - ERROR - Requested position of vertical Tedlar slip at row %d col %d\n",row,col);
    return 0.;
  }

  // Return Z position of center of slip in local coordinate system
  return 0.5*(-fECalSizeZ+fCrystalGap+GetCellSizeZ());

}

G4double ECalGeometry::GetExternalSupportCellPosX(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetExternalSupportCellPosX - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return X position of center of ASA cell in local coordinate system
  return GetExternalSupportCellSizeX()*(-fECalNCols*0.5+col+0.5);

}

G4double ECalGeometry::GetExternalSupportCellPosY(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetExternalSupportCellPosY - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return Y position of center of ASA cell in local coordinate system
  return GetExternalSupportCellSizeY()*(-fECalNRows*0.5+row+0.5);

}

G4double ECalGeometry::GetExternalSupportCellPosZ(G4int row, G4int col)
{

  // Verify we are within ECal box
  if ( row<0 || row>=fECalNRows || col<0 || col>=fECalNCols ) {
    printf("ECalGeometry::GetExternalSupportCellPosZ - ERROR - Requested position of crystal at row %d col %d (outside ECal box)\n",row,col);
    return 0.;
  }

  // Return Z position of center of cell in local coordinate system
  return 0.5*(-fECalSizeZ+fCrystalGap+GetExternalSupportCellSizeZ());

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

  buffer << "fECalDisplacementX " << fECalDisplacementX;
  hash.push_back(buffer.str());
  buffer.str("");

  buffer << "fECalDisplacementY " << fECalDisplacementY;
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
  for(G4int i=0;i<(G4int)fDigiPECollectionMap.size();i++) buffer << " " << fDigiPECollectionMap[i];
  hash.push_back(buffer.str());
  buffer.str("");

  return hash;
}
