// LeadGlassGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-20
// --------------------------------------------------------------

#ifndef LeadGlassGeometry_H
#define LeadGlassGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

// Number of different block shapes
#define NUMBER_OF_BLOCK_SHAPES 9

class G4LogicalVolume;

class LeadGlassGeometry
{

public:

  ~LeadGlassGeometry();
  static LeadGlassGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static LeadGlassGeometry* fInstance;

protected:

  LeadGlassGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  G4int GetTotalNumberOfBlockShapes() { return NUMBER_OF_BLOCK_SHAPES; }

  G4int GetBlockId() { return fBlockId; }

  G4int GetBlockOpalId(const G4int id) { return fBlockOpalId[id]; }

  G4double GetBlockZLength(const G4int id) { return fBlockZLength[id]; }
  G4double GetBlockL1Length(const G4int id) { return fBlockL1Length[id]; }
  G4double GetBlockL2Length(const G4int id) { return fBlockL2Length[id]; }
  G4double GetBlockL3Length(const G4int id) { return fBlockL3Length[id]; }
  G4double GetBlockL4Length(const G4int id) { return fBlockL4Length[id]; }
  G4double GetBlockW1Length(const G4int id) { return fBlockW1Length[id]; }
  G4double GetBlockW2Length(const G4int id) { return fBlockW2Length[id]; }

  G4double GetLightGuideZLength(const G4int id) { return fLightGuideZLength[id]; }
  G4double GetLightGuideDiameter(const G4int id) { return fLightGuideDiameter[id]; }

  G4double GetCathodeDiameter(const G4int id) { return GetLightGuideDiameter(id) * 0.95; }
  G4double GetWrapFrontHole(const G4int) { return fWrapFrontHole; }
  G4double GetWrapThick(const G4int) { return fWrapThick; }
  G4double GetGlueThick(const G4int) { return fGlueThick; }
  G4double GetAirThick(const G4int) { return fAirThick; }

  G4double GetMuMetalZLength(const G4int id) { return fMuMetalZLength[id]; }
  G4double GetMuMetalDiameter(const G4int id) { return fMuMetalDiameter[id]; }
  G4double GetMuMetalThickness(const G4int id) { return fMuMetalThickness[id]; }

  G4double GetSteelSlabThickness(const G4int id) { return fSteelSlabThickness[id]; }

  // Position along Z of LeadGlass front face
  G4double GetLeadGlassFrontFacePosZ() { return fLeadGlassFrontFacePosZ; }
  void SetLeadGlassFrontFacePosZ(G4double z) { fLeadGlassFrontFacePosZ = z; }
  
  // Get name of LeadGlass digitizer
  G4String GetLeadGlassDigitizerName() { return fLeadGlassDigitizerName; }

  // Get name of LeadGlass sensitive detector
  G4String GetLeadGlassSensitiveDetectorName() { return fLeadGlassSensitiveDetectorName; }

private:

  G4int fBlockId; // Id (0-8) of block type used for PADME

  // Geometrical parameters for PbGl blocks
  G4int fBlockOpalId[NUMBER_OF_BLOCK_SHAPES];            // Opal Id of block
  G4double fBlockZLength[NUMBER_OF_BLOCK_SHAPES];        // Z length (PbGl only)
  G4double fBlockL1Length[NUMBER_OF_BLOCK_SHAPES];       // L1 length (long base of back face)
  G4double fBlockL2Length[NUMBER_OF_BLOCK_SHAPES];       // L2 length (short base of back face)
  G4double fBlockL3Length[NUMBER_OF_BLOCK_SHAPES];       // L3 length (long base of front face)
  G4double fBlockL4Length[NUMBER_OF_BLOCK_SHAPES];       // L4 length (short base of front face)
  G4double fBlockW1Length[NUMBER_OF_BLOCK_SHAPES];       // W1 length (height of back face)
  G4double fBlockW2Length[NUMBER_OF_BLOCK_SHAPES];       // W2 length (height of front face)
  G4double fLightGuideZLength[NUMBER_OF_BLOCK_SHAPES];   // Light Guide Z length
  G4double fLightGuideDiameter[NUMBER_OF_BLOCK_SHAPES];  // Light Guide diameter
  G4double fMuMetalZLength[NUMBER_OF_BLOCK_SHAPES];      // u-metal cylinder Z length
  G4double fMuMetalDiameter[NUMBER_OF_BLOCK_SHAPES];     // u-metal cylinder external diameter
  G4double fMuMetalThickness[NUMBER_OF_BLOCK_SHAPES];    // u-metal cylinder thickness
  G4double fSteelSlabThickness[NUMBER_OF_BLOCK_SHAPES];  // Steel slab thickness
  G4double fWrapFrontHole;  // diameter of the hole in the small wrapping face
  G4double fWrapThick;      // wrap thickness
  G4double fAirThick;       // air thickness between leadglass and wrapping
  G4double fGlueThick;      // glue thickness betwen leadglass and metal plate

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  G4double fLeadGlassFrontFacePosZ; // Position along Z axis of LeadGlass front face

  G4String fLeadGlassSensitiveDetectorName;
  G4String fLeadGlassDigitizerName;

};

#endif
