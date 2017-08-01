// SACGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#ifndef SACGeometry_H
#define SACGeometry_H 1

#include <vector>

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

#define SACGEOMETRY_N_ROWS_MAX 10
#define SACGEOMETRY_N_COLS_MAX 10

class SACGeometry
{

public:

  ~SACGeometry();
  static SACGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static SACGeometry* fInstance;

protected:

  SACGeometry();

public:

  // Position of center of SAC box
  G4double GetSACPosX() { return 0.*cm; }
  G4double GetSACPosY() { return 0.*cm; }
  G4double GetSACPosZ() { return fSACFrontFacePosZ+GetCellSizeZ()*0.5; }

  // Size of SAC box
  G4double GetSACSizeX() { return (GetCellSizeX()+fCrystalGap)*fSACNCols; }
  G4double GetSACSizeY() { return (GetCellSizeY()+fCrystalGap)*fSACNRows; }
  G4double GetSACSizeZ() { return  GetCellSizeZ()+fCrystalGap; }

  // Number of rows and columns of crystals in SAC
  G4int GetSACNRows() { return fSACNRows; }
  G4int GetSACNCols() { return fSACNCols; }
  void  SetSACNRows(G4int r) { fSACNRows = r; }
  void  SetSACNCols(G4int c) { fSACNCols = c; }

  // Check if crystal exists at given row/column
  G4int ExistsCrystalAt(G4int,G4int);

  // Position of center of crystal at given row/column
  G4double GetCrystalPosX(G4int,G4int);
  G4double GetCrystalPosY(G4int,G4int);
  G4double GetCrystalPosZ(G4int,G4int);

  // Size of SAC BaF2 crystal
  G4double GetCrystalSizeX() { return fCrystalSizeX; }
  G4double GetCrystalSizeY() { return fCrystalSizeY; }
  G4double GetCrystalSizeZ() { return fCrystalSizeZ; }

  // Set size of BGO crystal
  void SetCrystalSizeX(G4double s) { fCrystalSizeX = s; }
  void SetCrystalSizeY(G4double s) { fCrystalSizeY = s; }
  void SetCrystalSizeZ(G4double s) { fCrystalSizeZ = s; }

  // Size of SAC cell (crystal+coating)
  G4double GetCellSizeX() { return fCrystalSizeX+2.*fCrystalCoating; }
  G4double GetCellSizeY() { return fCrystalSizeY+2.*fCrystalCoating; }
  G4double GetCellSizeZ() { return fCrystalSizeZ+2.*fCrystalCoating; }

  // Size of gap between crystals
  G4double GetCrystalGap() { return fCrystalGap; }
  void SetCrystalGap(G4double g) { fCrystalGap = g; }

  // Thickness of paint coating around each crystal
  G4double GetCrystalCoating() { return fCrystalCoating; }
  void SetCrystalCoating(G4double c) { fCrystalCoating = c; }

  // Set position along Z of SAC front face
  G4double GetSACFrontFacePosZ()           { return fSACFrontFacePosZ; }
  void     SetSACFrontFacePosZ(G4double z) { fSACFrontFacePosZ = z; }

  // Digitization parameters
  G4double GetSACLightPropagationSpeed() { return fDigiAvgLightSpeed; }
  G4double GetDigiEtoNPEConversion() { return fDigiEtoNPEConversion; }
  G4double GetDigiPEtoSignalConversion() { return fDigiPEtoSignalConversion; }

  std::vector<G4double> GetDigiPECollectionMap() { return fDigiPECollectionMap; }
  G4int GetDigiPECollectionMapNBins() { return fDigiPECollectionMap.size(); }
  G4double  GetDigiPECollectionMapBinLength() { return fCrystalSizeZ/fDigiPECollectionMap.size(); }

  G4double GetDigiPMTTransitTime() { return fDigiPMTTransitTime; }
  G4double GetDigiPMTCableDelay() { return fDigiPMTCableDelay; }

  // Get name of SAC sensitive detector
  G4String GetSACSensitiveDetectorName() { return fSACSensitiveDetectorName; }

private:

  // Geometry parameters

  G4double fCrystalSizeX;
  G4double fCrystalSizeY;
  G4double fCrystalSizeZ;

  G4int fSACNRows;
  G4int fSACNCols;

  G4double fCrystalGap; // Air gap size between adjacent crystals

  G4double fCrystalCoating; // Thickness of coating around crystals

  G4double fSACFrontFacePosZ; // Position along Z axis of SAC front face

  // Digitization parameters

  G4double fDigiAvgLightSpeed; // Average light speed inside SAC crystal for Cherenkov spectrum

  G4double fDigiEtoNPEConversion; // Number of p.e. produced by photocathode per MeV of hit energy
  G4double fDigiPEtoSignalConversion; // Contribution of 1 p.e. to integral ADC signal
 
  std::vector<G4double> fDigiPECollectionMap; // Relative collection efficiency along crystal

  G4double fDigiPMTTransitTime; // PMT transit time from photocathode to anode
  G4double fDigiPMTCableDelay; // Delay due to connection cables

  // Other parameteres

  G4String fSACSensitiveDetectorName;

};

#endif
