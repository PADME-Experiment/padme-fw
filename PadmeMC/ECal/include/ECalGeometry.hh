// ECalGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef ECalGeometry_H
#define ECalGeometry_H 1

#include <vector>

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

#define ECALGEOMETRY_N_ROWS_MAX 100
#define ECALGEOMETRY_N_COLS_MAX 100

class ECalGeometry
{

public:

  ~ECalGeometry();
  static ECalGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

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
  G4double GetECalSizeX() { return (GetCellSizeX()+fCrystalGap)*fECalNCols; }
  G4double GetECalSizeY() { return (GetCellSizeY()+fCrystalGap)*fECalNRows; }
  G4double GetECalSizeZ() { return  GetCellSizeZ()+fCrystalGap; }

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
  G4double GetTedlarHSizeX() { return fCrystalSizeX+2.*fCrystalCoating+fCrystalGap-1.*um; } // Approximate a continuous sheet
  G4double GetTedlarHSizeY() { return fTedlarThickness; }
  G4double GetTedlarHSizeZ() { return fCrystalSizeZ; }

  // Position of center of horizontal Tedlar slip at given row/column
  G4double GetTedlarHPosX(G4int,G4int);
  G4double GetTedlarHPosY(G4int,G4int);
  G4double GetTedlarHPosZ(G4int,G4int);

  // Check if vertical Tedlar slip exists at given row/column
  G4int ExistsTedlarVAt(G4int,G4int);

  // Size of vertical Tedlar slip
  G4double GetTedlarVSizeX() { return fTedlarThickness; }
  G4double GetTedlarVSizeY() { return fCrystalSizeY+fCrystalCoating; }
  G4double GetTedlarVSizeZ() { return fCrystalSizeZ; }

  // Position of center of vertical Tedlar slip at given row/column
  G4double GetTedlarVPosX(G4int,G4int);
  G4double GetTedlarVPosY(G4int,G4int);
  G4double GetTedlarVPosZ(G4int,G4int);

  // Position along Z of ECal front face
  G4double GetECalFrontFacePosZ() { return fECalFrontFacePosZ; }
  void SetECalFrontFacePosZ(G4double z) { fECalFrontFacePosZ = z; }

  // Enable/Disable plastic panel in front of ECal
  void EnableECalPanel() { fECalPanelEnable = true; }
  void DisableECalPanel() { fECalPanelEnable = false; }
  G4bool ECalPanelIsEnabled() { return fECalPanelEnable; }

  // Position of plastic panel
  G4double GetECalPanelPosX() { return 0.*cm; }
  G4double GetECalPanelPosY() { return 0.*cm; }
  G4double GetECalPanelPosZ() { return fECalFrontFacePosZ-fECalPanelGap-0.5*fECalPanelThickness; }

  // Dimensions of plastic panel
  G4double GetECalPanelSizeX() { return fECalPanelSizeX; }
  G4double GetECalPanelSizeY() { return fECalPanelSizeY; }
  G4double GetECalPanelSizeZ() { return fECalPanelThickness; }

  // Thickness of the plastic panel
  G4double GetECalPanelThickness() { return fECalPanelThickness; }
  void SetECalPanelThickness(G4double t) { fECalPanelThickness = t; }

  // Air gap between plastic panel and ECal
  G4double GetECalPanelGap() { return fECalPanelGap; }
  void SetECalPanelGap(G4double g) { fECalPanelGap = g; }

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
  G4String GetECalSensitiveDetectorName() { return fECalSensitiveDetectorName; }

private:

  void SetCrystalMap();

  // Geometry parameters

  G4int fCrystalMapId; // Id of crystal map to use

  G4double fCrystalSizeX;
  G4double fCrystalSizeY;
  G4double fCrystalSizeZ;

  G4int fECalNRows;
  G4int fECalNCols;

  G4double fCrystalGap; // Air gap size between adjacent crystals+coating

  G4double fCrystalCoating; // Thickness of coating around crystals

  G4double fTedlarThickness;  // Thickness of Tedlar slips

  G4double fECalFrontFacePosZ; // Position along Z axis of ECal front face

  G4bool fECalPanelEnable; // Use (true) or do not use (false) the panel in front of ECal
  G4double fECalPanelSizeX; // Size of plastic panel in front of ECal along X
  G4double fECalPanelSizeY; // Size of plastic panel in front of ECal along Y
  G4double fECalPanelThickness; // Thickness of the plastic panel in front of ECal
  G4double fECalPanelGap; // Gap between back of plastic panel and front face of ECal

  G4int fECalCrystalMap[ECALGEOMETRY_N_ROWS_MAX][ECALGEOMETRY_N_COLS_MAX]; // Map of existing crystals

  // Digitization parameters

  G4double fDigiEtoNPEConversion; // Number of p.e. produced by photocathode per MeV of hit energy
  G4double fDigiPEtoSignalConversion; // Contribution of 1 p.e. to integral ADC signal
 
  std::vector<G4double> fDigiPECollectionMap; // Relative collection efficiency along crystal

  G4double fDigiPMTTransitTime; // PMT transit time from photocathode to anode
  G4double fDigiPMTCableDelay; // Delay due to connection cables

  // Other parameteres

  G4String fECalSensitiveDetectorName;

};

#endif
