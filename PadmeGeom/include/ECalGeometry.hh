// ECalGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ECalGeometry_H
#define ECalGeometry_H 1

#include <vector>
#include <string>


//#include "globals.hh"
#include "PadmeSystemOfUnits.hh"
// Include base types
#include "PadmeG4Types.hh"
// Get definition of G4String
//#include "G4String.hh"
// Includes some additional definitions: sqr, G4SwapPtr, G4SwapObj.
//#include "templates.hh"
// Includes Physical Constants and System of Units
//#include "G4PhysicalConstants.hh"

//class G4LogicalVolume;

#define ECALGEOMETRY_N_ROWS_MAX 100
#define ECALGEOMETRY_N_COLS_MAX 100

class ECalGeometry
{

public:

  ~ECalGeometry();
  static ECalGeometry* GetInstance();
  std::vector<std::string> GetHashTable();

private:

  static ECalGeometry* fInstance;

protected:

  ECalGeometry();

public:

  // Position of center of ECal box
  G4double GetECalPosX() { return 0.*cm; }
  G4double GetECalPosY() { return 0.*cm; }
  G4double GetECalPosZ() { return fECalFrontFacePosZ+GetECalSizeZ()*0.5; }

  // Size of ECal box
  //G4double GetECalSizeX() { return (GetCellSizeX()+fCrystalGap)*fECalNCols; }
  //G4double GetECalSizeY() { return (GetCellSizeY()+fCrystalGap)*fECalNRows; }
  //G4double GetECalSizeZ() { return  GetCellSizeZ()+fCrystalGap; }
  G4double GetECalSizeX() { return fECalSizeX; }
  G4double GetECalSizeY() { return fECalSizeY; }
  G4double GetECalSizeZ() { return fECalSizeZ; }

  // Id of crystal map to use
  G4int GetCrystalMapId() { return fCrystalMapId; }
  void SetCrystalMapId(G4int m) { fCrystalMapId = m; SetCrystalMap(); }

  // Number of rows and columns of crystals in ECAL
  G4int GetECalNRows()        { return fECalNRows; }
  G4int GetECalNCols()        { return fECalNCols; }
  void  SetECalNRows(G4int r) { fECalNRows = r; }
  void  SetECalNCols(G4int c) { fECalNCols = c; }

  // Check if crystal exists at given row/column
  G4int ExistsCrystalAt(G4int,G4int);

  // Position of center of crystal at given row/column
  G4double GetCrystalPosX(G4int,G4int);
  G4double GetCrystalPosY(G4int,G4int);
  G4double GetCrystalPosZ(G4int,G4int);

  // Size of BGO crystal
  G4double GetCrystalSizeX() { return fCrystalSizeX; }
  G4double GetCrystalSizeY() { return fCrystalSizeY; }
  G4double GetCrystalSizeZ() { return fCrystalSizeZ; }

  // Set size of BGO crystal
  void SetCrystalSizeX(G4double s) { fCrystalSizeX = s; }
  void SetCrystalSizeY(G4double s) { fCrystalSizeY = s; }
  void SetCrystalSizeZ(G4double s) { fCrystalSizeZ = s; }

  // Size of ECal cell (BGO crystal+coating)
  G4double GetCellSizeX() { return fCrystalSizeX+2.*fCrystalCoating; }
  G4double GetCellSizeY() { return fCrystalSizeY+2.*fCrystalCoating; }
  G4double GetCellSizeZ() { return fCrystalSizeZ+2.*fCrystalCoating; }

  // Size of gap between crystals
  G4double GetCrystalGap() { return fCrystalGap; }
  void SetCrystalGap(G4double g) { fCrystalGap = g; }

  // Thickness of paint coating around each crystal
  G4double GetCrystalCoating() { return fCrystalCoating; }
  void SetCrystalCoating(G4double c) { fCrystalCoating = c; }

  // Thickness of Tedlar slips
  G4double GetTedlarThickness() { return fTedlarThickness; }
  void SetTedlarThickness(G4double t) { fTedlarThickness = t; }

  // Check if horizontal Tedlar slip exists at given row/column
  G4int ExistsTedlarHAt(G4int,G4int);

  // Size of horizontal Tedlar slip
  // G4double GetTedlarHSizeX() { return fCrystalSizeX+2.*fCrystalCoating+fCrystalGap-1.*um; } // Approximate a continuous sheet (creates overlap with support)
  G4double GetTedlarHSizeX() { return fCrystalSizeX+2.*fCrystalCoating; }
  G4double GetTedlarHSizeY() { return fTedlarThickness; }
  G4double GetTedlarHSizeZ() { return GetCellSizeZ(); }

  // Position of center of horizontal Tedlar slip at given row/column
  G4double GetTedlarHPosX(G4int,G4int);
  G4double GetTedlarHPosY(G4int,G4int);
  G4double GetTedlarHPosZ(G4int,G4int);

  // Check if vertical Tedlar slip exists at given row/column
  G4int ExistsTedlarVAt(G4int,G4int);

  // Size of vertical Tedlar slip
  G4double GetTedlarVSizeX() { return fTedlarThickness; }
  //G4double GetTedlarVSizeY() { return fCrystalSizeY+fCrystalCoating; }
  G4double GetTedlarVSizeY() { return fCrystalSizeY+2.*fCrystalCoating; }
  G4double GetTedlarVSizeZ() { return GetCellSizeZ(); }

  // Position of center of vertical Tedlar slip at given row/column
  G4double GetTedlarVPosX(G4int,G4int);
  G4double GetTedlarVPosY(G4int,G4int);
  G4double GetTedlarVPosZ(G4int,G4int);

  // Position along Z of ECal front face
  G4double GetECalFrontFacePosZ() { return fECalFrontFacePosZ; }
  void SetECalFrontFacePosZ(G4double z) { fECalFrontFacePosZ = z; }

    ////////////////////////////////////
   // External ASA support structure //
  ////////////////////////////////////

  // Check if support structure exists at given row/column
  G4int ExistsExternalSupportAt(G4int,G4int);

  G4double GetExternalSupportCellSizeX() { return fCrystalSizeX+2.*fCrystalCoating+fCrystalGap; }
  G4double GetExternalSupportCellSizeY() { return fCrystalSizeY+2.*fCrystalCoating+fCrystalGap; }
  G4double GetExternalSupportCellSizeZ() { return fCrystalSizeZ+2.*fCrystalCoating; }

  G4double GetExternalSupportCellPosX(G4int,G4int);
  G4double GetExternalSupportCellPosY(G4int,G4int);
  G4double GetExternalSupportCellPosZ(G4int,G4int);

    //////////////////
   // Nomex panel //
  /////////////////

  // Enable/Disable Nomex panel in front of ECal
  void EnableECalPanel() { fECalPanelEnable = true; }
  void DisableECalPanel() { fECalPanelEnable = false; }
  G4bool ECalPanelIsEnabled() { return fECalPanelEnable; }

  // Position of Nomex panel
  G4double GetECalPanelPosX() { return 0.*cm; }
  G4double GetECalPanelPosY() { return 0.*cm; }
  G4double GetECalPanelPosZ() { return fECalFrontFacePosZ-fECalPanelGap-0.5*fECalPanelThickness; }

  // Dimensions of Nomex panel
  G4double GetECalPanelSizeX() { return fECalPanelSizeX; }
  G4double GetECalPanelSizeY() { return fECalPanelSizeY; }
  G4double GetECalPanelSizeZ() { return fECalPanelThickness; }

  // Total thickness of Nomex panel
  G4double GetECalPanelThickness() { return fECalPanelThickness; }
  void SetECalPanelThickness(G4double t) { fECalPanelThickness = t; }

  // Thickness of glass fiber foils enclosing honeycomb
  G4double GetECalPanelFoilThickness() { return fECalPanelFoilThickness; }
  void SetECalPanelFoilThickness(G4double t) { fECalPanelFoilThickness = t; }

  // Thickness of glue layers between glass fiber foils and honeycomb
  G4double GetECalPanelGlueThickness() { return fECalPanelGlueThickness; }
  void SetECalPanelGlueThickness(G4double t) { fECalPanelGlueThickness = t; }

  // Thickness of honeycomb inside panel (derived quantity: can only be read)
  G4double GetECalPanelHoneycombThickness() { return fECalPanelThickness - 2.*(fECalPanelFoilThickness+fECalPanelGlueThickness); }

  // Radius of the hole at center of Nomex panel
  G4double GetECalPanelHoleRadius() { return fECalPanelHoleRadius; }
  void SetECalPanelHoleRadius(G4double r) { fECalPanelHoleRadius = r; }

  // Air gap between Nomex panel and ECal
  G4double GetECalPanelGap() { return fECalPanelGap; }
  void SetECalPanelGap(G4double g) { fECalPanelGap = g; }

    ///////////////////////////////////////////
   // ASA support structure in central hole //
  ///////////////////////////////////////////

  // Enable/Disable ASA support in ECal hole
  void EnableECalSupport() { fECalSupportEnable = true; }
  void DisableECalSupport() { fECalSupportEnable = false; }
  G4bool ECalSupportIsEnabled() { return fECalSupportEnable; }

  // Position of ASA support in ECal hole (local main ECal box coordinates)
  G4double GetECalSupportPosX() { return 0.*cm; }
  G4double GetECalSupportPosY() { return 0.*cm; }
  G4double GetECalSupportPosZ() { return 0.5*(-fECalSizeZ+fCrystalGap+fECalSupportSizeZ); }

  // Dimensions of ASA support in ECal hole
  G4double GetECalSupportSizeX() { return fECalSupportSizeX; }
  G4double GetECalSupportSizeY() { return fECalSupportSizeY; }
  G4double GetECalSupportSizeZ() { return fECalSupportSizeZ; }

  // Radius of the hole at center of ASA support front face
  G4double GetECalSupportHoleRadius() { return fECalSupportHoleRadius; }
  void SetECalSupportHoleRadius(G4double r) { fECalSupportHoleRadius = r; }

  // Position of hole in fron face of ASA support (local ASA support coordinates)
  G4double GetECalSupportHolePosX() { return 0.; }
  G4double GetECalSupportHolePosY() { return 0.; }
  G4double GetECalSupportHolePosZ() { return 0.5*(-fECalSupportSizeZ+fECalSupportFrontThickness); }

  // Thickness of front wall of ASA support
  G4double GetECalSupportFrontThickness() { return fECalSupportFrontThickness; }

  // Size of cavity inside ASA support
  G4double GetECalSupportCavitySizeX() { return fECalSupportSizeX-2.*fECalSupportSideThickness; }
  G4double GetECalSupportCavitySizeY() { return fECalSupportSizeY-2.*fECalSupportSideThickness; }
  G4double GetECalSupportCavitySizeZ() { return fECalSupportSizeZ-fECalSupportFrontThickness-fECalSupportBackThickness; }

  // Position of cavity inside ASA support (local ASA support coordinates)
  G4double GetECalSupportCavityPosX() { return 0.; }
  G4double GetECalSupportCavityPosY() { return 0.; }
  G4double GetECalSupportCavityPosZ() { return 0.5*(fECalSupportFrontThickness-fECalSupportBackThickness); }

  // Parameters for lattice along internal faces of support structure

  G4int GetECalSupportLatticeNCellXY() { return fECalSupportLatticeNCellXY; }
  G4int GetECalSupportLatticeNCellZ() { return fECalSupportLatticeNCellZ; }

  G4double GetECalSupportLatticeThickness() { return fECalSupportLatticeThickness; }
  G4double GetECalSupportLatticeHoleWidth() { return fECalSupportLatticeHoleWidth; }
  G4double GetECalSupportLatticeHoleLength() { return fECalSupportLatticeHoleLength; }
  G4double GetECalSupportLatticeCellWidth() { return fECalSupportLatticeHoleWidth+2.*fECalSupportLatticeFrame; }
  G4double GetECalSupportLatticeCellLength() { return fECalSupportLatticeHoleLength+2.*fECalSupportLatticeFrame; }

  G4double GetECalSupportLatticeXSizeX() { return fECalSupportLatticeNCellXY*GetECalSupportLatticeCellWidth(); }
  G4double GetECalSupportLatticeXSizeY() { return fECalSupportLatticeThickness; }
  G4double GetECalSupportLatticeXSizeZ() { return fECalSupportLatticeNCellZ*GetECalSupportLatticeCellLength(); }
  G4double GetECalSupportLatticeXpPosX() { return 0.; }
  G4double GetECalSupportLatticeXpPosY() { return 0.5*fECalSupportSizeY-fECalSupportSideThickness-0.5*fECalSupportLatticeThickness-5.*um; }
  G4double GetECalSupportLatticeXpPosZ() { return 0.5*(fECalSupportFrontThickness-fECalSupportBackThickness); }
  G4double GetECalSupportLatticeXmPosX() { return 0.; }
  G4double GetECalSupportLatticeXmPosY() { return -0.5*fECalSupportSizeY+fECalSupportSideThickness+0.5*fECalSupportLatticeThickness+5.*um; }
  G4double GetECalSupportLatticeXmPosZ() { return 0.5*(fECalSupportFrontThickness-fECalSupportBackThickness); }

  G4double GetECalSupportLatticeXCellPosX(G4int x, G4int z) { return -0.5*GetECalSupportLatticeXSizeX()+(x+0.5)*GetECalSupportLatticeCellWidth(); }
  G4double GetECalSupportLatticeXCellPosY(G4int x, G4int z) { return 0.; }
  G4double GetECalSupportLatticeXCellPosZ(G4int x, G4int z) { return -0.5*GetECalSupportLatticeXSizeZ()+(z+0.5)*GetECalSupportLatticeCellLength(); }

  G4double GetECalSupportLatticeYSizeX() { return fECalSupportLatticeThickness; }
  G4double GetECalSupportLatticeYSizeY() { return fECalSupportLatticeNCellXY*GetECalSupportLatticeCellWidth(); }
  G4double GetECalSupportLatticeYSizeZ() { return fECalSupportLatticeNCellZ*GetECalSupportLatticeCellLength(); }
  G4double GetECalSupportLatticeYpPosX() { return 0.5*fECalSupportSizeX-fECalSupportSideThickness-0.5*fECalSupportLatticeThickness-5.*um; }
  G4double GetECalSupportLatticeYpPosY() { return 0.; }
  G4double GetECalSupportLatticeYpPosZ() { return 0.5*(fECalSupportFrontThickness-fECalSupportBackThickness); }
  G4double GetECalSupportLatticeYmPosX() { return -0.5*fECalSupportSizeX+fECalSupportSideThickness+0.5*fECalSupportLatticeThickness+5.*um; }
  G4double GetECalSupportLatticeYmPosY() { return 0.; }
  G4double GetECalSupportLatticeYmPosZ() { return 0.5*(fECalSupportFrontThickness-fECalSupportBackThickness); }

  G4double GetECalSupportLatticeYCellPosX(G4int y, G4int z) { return 0.; }
  G4double GetECalSupportLatticeYCellPosY(G4int y, G4int z) { return -0.5*GetECalSupportLatticeYSizeY()+(y+0.5)*GetECalSupportLatticeCellWidth(); }
  G4double GetECalSupportLatticeYCellPosZ(G4int y, G4int z) { return -0.5*GetECalSupportLatticeYSizeZ()+(z+0.5)*GetECalSupportLatticeCellLength(); }

  G4double GetECalSupportCornerSizeX() { return 0.5*(GetECalSupportCavitySizeX()-GetECalSupportLatticeXSizeX()-10.*um); }
  G4double GetECalSupportCornerSizeY() { return 0.5*(GetECalSupportCavitySizeY()-GetECalSupportLatticeYSizeY()-10.*um); }
  G4double GetECalSupportCornerSizeZ() { return GetECalSupportLatticeXSizeZ(); }

  G4double GetECalSupportCornerURPosX() { return 0.5*(GetECalSupportCavitySizeX()-GetECalSupportCornerSizeX()-10.*um); }
  G4double GetECalSupportCornerURPosY() { return 0.5*(GetECalSupportCavitySizeY()-GetECalSupportCornerSizeY()-10.*um); }
  G4double GetECalSupportCornerURPosZ() { return GetECalSupportCavityPosZ(); }

  G4double GetECalSupportCornerULPosX() { return -0.5*(GetECalSupportCavitySizeX()-GetECalSupportCornerSizeX()-10.*um); }
  G4double GetECalSupportCornerULPosY() { return 0.5*(GetECalSupportCavitySizeY()-GetECalSupportCornerSizeY()-10.*um); }
  G4double GetECalSupportCornerULPosZ() { return GetECalSupportCavityPosZ(); }

  G4double GetECalSupportCornerDRPosX() { return 0.5*(GetECalSupportCavitySizeX()-GetECalSupportCornerSizeX()-10.*um); }
  G4double GetECalSupportCornerDRPosY() { return -0.5*(GetECalSupportCavitySizeY()-GetECalSupportCornerSizeY()-10.*um); }
  G4double GetECalSupportCornerDRPosZ() { return GetECalSupportCavityPosZ(); }

  G4double GetECalSupportCornerDLPosX() { return -0.5*(GetECalSupportCavitySizeX()-GetECalSupportCornerSizeX()-10.*um); }
  G4double GetECalSupportCornerDLPosY() { return -0.5*(GetECalSupportCavitySizeY()-GetECalSupportCornerSizeY()-10.*um); }
  G4double GetECalSupportCornerDLPosZ() { return GetECalSupportCavityPosZ(); }

  // Digitization parameters
  G4double GetBGOLightPropagationSpeed() { return (2.998E8*m/s)/2.57; }
  G4double GetDigiEtoNPEConversion() { return fDigiEtoNPEConversion; }
  G4double GetDigiPEtoSignalConversion() { return fDigiPEtoSignalConversion; }

  std::vector<G4double> GetDigiPECollectionMap() { return fDigiPECollectionMap; }
  G4int GetDigiPECollectionMapNBins() { return fDigiPECollectionMap.size(); }
  G4double  GetDigiPECollectionMapBinLength() { return fCrystalSizeZ/fDigiPECollectionMap.size(); }

  G4double GetDigiPMTTransitTime() { return fDigiPMTTransitTime; }
  G4double GetDigiPMTCableDelay() { return fDigiPMTCableDelay; }

  // Get name of ECal sensitive detector
  std::string GetECalSensitiveDetectorName() { return fECalSensitiveDetectorName; }

private:

  void SetCrystalMap();

  // Geometry parameters

  G4int fCrystalMapId; // Id of crystal map to use

  G4double fCrystalSizeX;
  G4double fCrystalSizeY;
  G4double fCrystalSizeZ;

  G4int fECalNRows;
  G4int fECalNCols;

  G4double fECalSizeX;
  G4double fECalSizeY;
  G4double fECalSizeZ;

  G4double fCrystalGap; // Air gap size between adjacent crystals+coating

  G4double fCrystalCoating; // Thickness of coating around crystals

  G4double fTedlarThickness;  // Thickness of Tedlar slips

  G4double fECalFrontFacePosZ; // Position along Z axis of ECal front face

  G4bool fECalPanelEnable; // Use (true) or do not use (false) the panel in front of ECal
  G4double fECalPanelSizeX; // Size of Nomex panel in front of ECal along X
  G4double fECalPanelSizeY; // Size of Nomex panel in front of ECal along Y
  G4double fECalPanelThickness; // Total thickness of Nomex panel in front of ECal
  G4double fECalPanelFoilThickness; // Thickness of glass fiber foils enclosing honeycomb
  G4double fECalPanelGlueThickness; // Thickness of glue layers between glass fiber foils and honeycomb
  G4double fECalPanelHoleRadius; // Radius of hole at center of Nomex panel in front of ECal
  G4double fECalPanelGap; // Gap between back of Nomex panel and front face of ECal

  G4bool fECalSupportEnable; // Use (true) or do not use (false) the ASA support structure inside ECal hole
  G4double fECalSupportSizeX; // Size along X of ASA support structure inside ECal
  G4double fECalSupportSizeY; // Size along Y of ASA support structure inside ECal
  G4double fECalSupportSizeZ; // Size along Z of ASA support structure inside ECal
  G4double fECalSupportHoleRadius; // Radius of hole at center of ASA support structure inside ECal
  G4double fECalSupportFrontThickness; // Thickness of front face
  G4double fECalSupportSideThickness; // Thickness of side faces
  G4double fECalSupportBackThickness; // Thickness of back face
  G4double fECalSupportLatticeFrame; // Width of frame around lattice cell
  G4double fECalSupportLatticeHoleWidth; // Width (along X or Y) of hole inside lattice cell
  G4double fECalSupportLatticeHoleLength; // Length (along Z) of hole inside lattice cell
  G4double fECalSupportLatticeThickness; // Thickness of lattice
  G4int fECalSupportLatticeNCellXY; // Number of cells along X or Y
  G4int fECalSupportLatticeNCellZ; // Number of cells along Z

  G4int fECalCrystalMap[ECALGEOMETRY_N_ROWS_MAX][ECALGEOMETRY_N_COLS_MAX]; // Map of existing crystals

  // Digitization parameters

  G4double fDigiEtoNPEConversion; // Number of p.e. produced by photocathode per MeV of hit energy
  G4double fDigiPEtoSignalConversion; // Contribution of 1 p.e. to integral ADC signal
 
  std::vector<G4double> fDigiPECollectionMap; // Relative collection efficiency along crystal

  G4double fDigiPMTTransitTime; // PMT transit time from photocathode to anode
  G4double fDigiPMTCableDelay; // Delay due to connection cables

  // Other parameteres

  std::string fECalSensitiveDetectorName;

};

#endif
