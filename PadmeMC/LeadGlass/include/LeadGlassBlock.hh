// LeadGlassBlock.hh
// -------------------------------------------------------------------------
// History:
//
// 2009-03-02 - Emanuele Leonardi (Emanuele.Leonardi@roma1.infn.it)
//   - New class to create PbGl blocks (basic components of LAVDetector)
// 2010-11-03 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Blocks geometry is now correct
//   - Added (part of) block support structure
// 2010-11-23 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Adapted to use LAVVPbGlBlock virtual class
// 2020-02-10 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Added G4Trap parameters adjustment method to comply with new
//     planarity tolerances (GEANT4 v.10.4)
// 2023-09-20 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Adapted for PADME
//
// -------------------------------------------------------------------------
//

#ifndef LeadGlassBlock_h
#define LeadGlassBlock_h 1

#include "globals.hh"
#include "G4LogicalVolume.hh"

class G4Trap;

class LAVGeometryParameters;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class LeadGlassBlock
{

public:
  LeadGlassBlock(G4int);
  ~LeadGlassBlock();

  G4LogicalVolume* GetBlockLogicalVolume() { return fLogicalVolume; } // Full block LV, used for positioning

  G4LogicalVolume* GetPbGlBlockLogicalVolume() { return fPbGlBlockLogicalVolume; } // PbGl LV, used for SD
  G4LogicalVolume* GetLightGuideLogicalVolume() { return fLightGuideLogicalVolume; } // LightGuide LV, used for SD

  G4int GetBlockOpalId() { return fBlockOpalId; };

  // Function to return block dimensions

  G4double GetBlockL1() { return fBlockL1; };
  G4double GetBlockL2() { return fBlockL2; };
  G4double GetBlockL3() { return fBlockL3; };
  G4double GetBlockL4() { return fBlockL4; };
  G4double GetBlockW1() { return fBlockW1; };
  G4double GetBlockW2() { return fBlockW2; };
  G4double GetBlockZLength() { return fBlockZLen; };  // From back of crystal to front of wrapping
  G4double GetBlockFullZLength() { return fBlockZLen + fSteelSlabThick; };  // From back of steel plate to front of wrapping

  // Abs value of the Z coordinate of the front (small) and back (large) face of the block
  // in the block coordinate system
  // This parameter is used to position the block on the banana structure so
  // back face means the steel slab back face and front face is that of the wrapping
  G4double GetBlockZofFrontFace() { return 0.5 * fBlockZLen; };
  G4double GetBlockZofBackFace() { return fSteelSlabThick + 0.5 * fBlockZLen; };

  G4double GetMuMetalDiameter() { return fMuMetalDiam; };
  G4double GetMuMetalExternalZLength() { return fMuMetalZLen - fSteelSlabThick; }  // From back of steel plate to end of mu-metal

  // Functions to return naked crystal dimensions

  G4double GetCrystalL1() { return fBlockL1; };
  G4double GetCrystalL2() { return fBlockL2; };
  G4double GetCrystalL3() { return fBlockL3; };
  G4double GetCrystalL4() { return fBlockL4; };
  G4double GetCrystalW1() { return fBlockW1; };
  G4double GetCrystalW2() { return fBlockW2; };
  G4double GetCrystalZLength() { return fBlockZLen; };

private:

  // Method to create PbGl trapezoid
  G4Trap* PbGlShape(const G4String &, G4double, G4double, G4double, G4double, G4double, G4double,
                    G4double);

  // Fix G4Trap parameters to guarantee planarity of side faces
  void TrapParamAdjustment(const double (&in)[11], double (&out)[11]);

  // Complete block logical volume
  G4LogicalVolume* fLogicalVolume;
  G4LogicalVolume* fPbGlBlockLogicalVolume;
  G4LogicalVolume* fLightGuideLogicalVolume;

  // Id of block (internal identifier)
  G4int fBlockId;

  // Opal Id of block (7-15)
  G4int fBlockOpalId;

  // Dimensions of lead glass block
  G4double fBlockL1;    // Long base of back face
  G4double fBlockL2;    // Short base of back face
  G4double fBlockL3;    // Long base of front face
  G4double fBlockL4;    // Short base of front face
  G4double fBlockW1;    // Height of back face
  G4double fBlockW2;    // Height of front face
  G4double fBlockZLen;  // Z length

  // Dimensions of lightguide
  G4double fLightGuideZLen;  // Z length
  G4double fLightGuideDiam;  // Diameter

  // Dimensions of u-metal cylinder
  G4double fMuMetalZLen;   // u-metal cylinder Z length
  G4double fMuMetalDiam;   // u-metal cylinder external diameter
  G4double fMuMetalThick;  // u-metal cylinder thickness

  // Dimensions of steel slab
  G4double fSteelSlabThick;  // Steel slab thickness

};

#endif
