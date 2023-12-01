// LeadGlassBlock.cc
// -------------------------------------------------------------------------
// History:
//
// 2009-03-02 Emanuele Leonardi (Emanuele.Leonardi@roma1.infn.it)
//   - New class to create PbGl blocks (basic components of LAVDetector)
// 2010-11-03 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Blocks geometry is now correct
//   - Added (part of) block support structure
// 2010-11-23 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Improved naming conventions
//   - Used more realistic colors for block components
// 2011-01-24 Domenico Di Filippo (difilippo@na.infn.it)
//   - Contruct the sensitive detector
// 2015-05-01 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Fixed positioning of mu-metal
// 2020-02-10 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Added G4Trap parameters adjustment method to comply with new
//     planarity tolerances (GEANT4 v.10.4)
// 2023-09-20 Emanuele Leonardi (emanuele.leonardi@roma1.infn.it)
//   - Adapted for PADME
//
// -------------------------------------------------------------------------

#include "LeadGlassBlock.hh"

#include "LeadGlassGeometry.hh"
//#include "LeadGlassSD.hh"

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"

LeadGlassBlock::LeadGlassBlock(G4int blockId)
{

  // Set to true to check for overlapping volumes
  G4bool checkOverlaps = false;

  // Get access to LAV geometrical parameters
  LeadGlassGeometry* geoParams = LeadGlassGeometry::GetInstance();

  // Check if block id is valid
  if(blockId < 0 || blockId >= geoParams->GetTotalNumberOfBlockShapes()) {
    G4cerr << "LeadGlassBlock ERROR: created with invalid block shape id " << blockId << G4endl;
    return;
  }
  fBlockId = blockId;
  fBlockOpalId = geoParams->GetBlockOpalId(blockId);

  // Dimensions of lead glass block (see OPAL document for details)
  fBlockL1 = geoParams->GetBlockL1Length(fBlockId);   // Long base of back face
  fBlockL2 = geoParams->GetBlockL2Length(fBlockId);   // Short base of back face
  fBlockL3 = geoParams->GetBlockL3Length(fBlockId);   // Long base of front face
  fBlockL4 = geoParams->GetBlockL4Length(fBlockId);   // Short base of front face
  fBlockW1 = geoParams->GetBlockW1Length(fBlockId);   // Height of back face
  fBlockW2 = geoParams->GetBlockW2Length(fBlockId);   // Height of front face
  fBlockZLen = geoParams->GetBlockZLength(fBlockId);  // Z length

  // Dimensions of lightguide
  fLightGuideZLen = geoParams->GetLightGuideZLength(fBlockId);
  fLightGuideDiam = geoParams->GetLightGuideDiameter(fBlockId);

  // Dimensions of u-metal cylinder
  fMuMetalZLen = geoParams->GetMuMetalZLength(fBlockId);     // u-metal cylinder Z length
  fMuMetalDiam = geoParams->GetMuMetalDiameter(fBlockId);    // u-metal cylinder external diameter
  fMuMetalThick = geoParams->GetMuMetalThickness(fBlockId);  // u-metal cylinder thickness

  // Dimensions of steel slab
  fSteelSlabThick = geoParams->GetSteelSlabThickness(fBlockId);  // Steel slab thickness

  // Create solid for PbGl block
  G4Trap *solidPbGlBlock =
    PbGlShape("pbglblock", fBlockL1, fBlockL2, fBlockL3, fBlockL4, fBlockW1, fBlockW2, fBlockZLen);

  // Create solid for steel slab shape (no hole inside)
  G4Trap *solidSteelSlabShape = PbGlShape("steelslabshape", fBlockL1, fBlockL2, fBlockL1, fBlockL2,
                                          fBlockW1, fBlockW1, fSteelSlabThick);

  // Create solid for mu-metal protruding part (i.e. out of the steel slab)
  G4Tubs *solidMuMetalProt =
    new G4Tubs("mumetalshape", 0. * cm, 0.5 * fMuMetalDiam, 0.5 * (fMuMetalZLen - fSteelSlabThick),
               0. * rad, 2. * M_PI * rad);

  //
  // Join shapes to create PbGlBlock logical volume
  //
  // Compute coordinates of center of back face of block in block reference system
  G4ThreeVector pbglCenterOfBackFace(0., 0.25 * (fBlockW1 - fBlockW2), -0.5 * fBlockZLen);

  // Compute displacement for steel slab
  G4ThreeVector posSteelSlab = pbglCenterOfBackFace + G4ThreeVector(0., 0., -0.5 * fSteelSlabThick);

  // Compute displacement for protruding mu-metal cylinder
  //G4ThreeVector posMuMetalProt = pbglCenterOfBackFace+G4ThreeVector(0.,0.,-0.5*(fMuMetalZLen-fSteelSlabThick));
  G4ThreeVector posMuMetalProt =
    pbglCenterOfBackFace
    + G4ThreeVector(0., 0., -fSteelSlabThick - 0.5 * (fMuMetalZLen - fSteelSlabThick));

  // Join pbgl block to steel slab and protruding mu metal cylinder
  G4UnionSolid *solidPbGl1 =
    new G4UnionSolid("pbgl1", solidPbGlBlock, solidSteelSlabShape, 0, posSteelSlab);
  G4UnionSolid *solidPbGl =
    new G4UnionSolid("pbgl", solidPbGl1, solidMuMetalProt, 0, posMuMetalProt);

  // Greate main logical volume for PbGl structure
  fLogicalVolume =
    new G4LogicalVolume(solidPbGl, G4Material::GetMaterial("Air"), "PbGl", 0, 0, 0);

  // Create PbGl block logical volume
  //G4LogicalVolume *logicPbGlBlock = new G4LogicalVolume(
  fPbGlBlockLogicalVolume = new G4LogicalVolume(
    solidPbGlBlock, G4Material::GetMaterial("LAV_PbGl_SF57"), "PbGlBlock", 0, 0, 0);

  // Create LightGuide logical volume
  G4Tubs *solidLightGuide = new G4Tubs("lightguide", 0. * cm, 0.5 * fLightGuideDiam,
                                       0.5 * fLightGuideZLen, 0. * rad, 2. * M_PI * rad);
  //G4LogicalVolume *logicLightGuide = new G4LogicalVolume(
  fLightGuideLogicalVolume = new G4LogicalVolume(
    solidLightGuide, G4Material::GetMaterial("LAV_PbGl_SF57"), "LightGuide", 0, 0, 0);

  // Create steel slab with hole for mu-metal
  G4Trap *solidSteelSlabFull = PbGlShape("steelslabfull", fBlockL1, fBlockL2, fBlockL1, fBlockL2,
                                         fBlockW1, fBlockW1, fSteelSlabThick);
  G4Tubs *solidSteelSlabHole = new G4Tubs("steelslabhole", 0. * cm, 0.5 * fMuMetalDiam,
                                          0.5 * fSteelSlabThick, 0. * rad, 2. * M_PI * rad);
  G4SubtractionSolid *solidSteelSlab =
    new G4SubtractionSolid("steelslab", solidSteelSlabFull, solidSteelSlabHole);
  G4LogicalVolume *logicSteelSlab =
    new G4LogicalVolume(solidSteelSlab, G4Material::GetMaterial("G4_Fe"), "SteelSlab", 0, 0, 0);

  // Create mu-metal cylinder
  G4Tubs *solidMuMetal =
    new G4Tubs("mumetal", 0.5 * fMuMetalDiam - fMuMetalThick, 0.5 * fMuMetalDiam,
               0.5 * fMuMetalZLen, 0. * rad, 2. * M_PI * rad);
  G4LogicalVolume *logicMuMetal =
    new G4LogicalVolume(solidMuMetal, G4Material::GetMaterial("G4_Fe"), "MuMetal", 0, 0, 0);

  //
  // Position block, lightguide, steel slab, and mu-metal cylinder inside main logical volume
  //

  // Block has same coordinate system as main logical volume
  //new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicPbGlBlock, "LG_PbGlBlock", fLogicalVolume,
  //                  false, 0, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), fPbGlBlockLogicalVolume, "LG_PbGlBlock", fLogicalVolume,
                    false, 0, checkOverlaps);

  // Position of lightguide cylinder
  G4ThreeVector posLightGuide =
    pbglCenterOfBackFace + G4ThreeVector(0., 0., -0.5 * fLightGuideZLen);
  //new G4PVPlacement(0, posLightGuide, logicLightGuide, "LG_PbGlLightGuide", fLogicalVolume, false,
  //                  0, checkOverlaps);
  new G4PVPlacement(0, posLightGuide, fLightGuideLogicalVolume, "LG_PbGlLightGuide", fLogicalVolume, false,
                    0, checkOverlaps);

  // Position of steel slab (already computed)
  new G4PVPlacement(0, posSteelSlab, logicSteelSlab, "LG_SteelSlab", fLogicalVolume, false, 0,
                    checkOverlaps);

  // Position of full u-metal cylinder
  G4ThreeVector posMuMetal = pbglCenterOfBackFace + G4ThreeVector(0., 0., -0.5 * fMuMetalZLen);
  new G4PVPlacement(0, posMuMetal, logicMuMetal, "LG_MuMetal", fLogicalVolume, false, 0,
                    checkOverlaps);

  //--------- Visualization attributes -----------------------

  // Main volume is transparent
  fLogicalVolume->SetVisAttributes(G4VisAttributes::GetInvisible());

  // PbGl is white
  G4VisAttributes *PbGlVisAtt = new G4VisAttributes(G4Colour(1., 1., 1.));
  //logicPbGlBlock->SetVisAttributes(PbGlVisAtt);
  //logicLightGuide->SetVisAttributes(PbGlVisAtt);
  fPbGlBlockLogicalVolume->SetVisAttributes(PbGlVisAtt);
  fLightGuideLogicalVolume->SetVisAttributes(PbGlVisAtt);

  // Steel slab is dark grey
  G4VisAttributes *SteelVisAtt = new G4VisAttributes(G4Colour(0.3, 0.3, 0.3));
  logicSteelSlab->SetVisAttributes(SteelVisAtt);

  // Mu-Metal is almost black
  G4VisAttributes *MuMetalVisAtt = new G4VisAttributes(G4Colour(0.1, 0.1, 0.1));
  logicMuMetal->SetVisAttributes(MuMetalVisAtt);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Trap *LeadGlassBlock::PbGlShape(const G4String &name, G4double l1,
                                G4double l2,               // bases of back face
                                G4double l3, G4double l4,  // bases of front face
                                G4double w1, G4double w2,  // heights of back and front faces
                                G4double z                 // Z-length of trapezoid
) {

  //--------------------------------------
  // Create trapezoid solid for PbGl block
  //--------------------------------------

  G4Trap *thisTrap;

  // Get half length of sides
  G4double hl1 = 0.5 * l1;
  G4double hl2 = 0.5 * l2;
  G4double hl3 = 0.5 * l3;
  G4double hl4 = 0.5 * l4;
  G4double hw1 = 0.5 * w1;
  G4double hw2 = 0.5 * w2;
  G4double hz = 0.5 * z;

  // Compute angle parameters
  G4double theta = atan2(0.5 * (w1 - w2), z);
  G4double phi = -90. * deg;
  G4double alp1 = 0. * deg;
  G4double alp2 = 0. * deg;

  // Adjust parameters for planarity
  double in[11] = {hz, theta, phi, hw1, hl1, hl2, alp1, hw2, hl3, hl4, alp2};
  double out[11];
  TrapParamAdjustment(in, out);
  hz = out[0];
  theta = out[1];
  phi = out[2];
  hw1 = out[3];
  hl1 = out[4];
  hl2 = out[5];
  alp1 = out[6];
  hw2 = out[7];
  hl3 = out[8];
  hl4 = out[9];
  alp2 = out[10];

  thisTrap = new G4Trap(
    name,
    hz,     // Half z length
    theta,  // Polar angle of the line joining the centres of the faces at -/+pDz
    phi,  // Azimuthal angle of the line joining the centre of the face at -pDz to the centre of the face at +pDz
    hw1,  // Half y length at -pDz
    hl1,  // Half x length of the side at y=-pDy1 of the face at -pDz
    hl2,  // Half x length of the side at y=+pDy1 of the face at -pDz
    alp1,  // Angle with respect to the y axis from the centre of the side (lower endcap)
    hw2,   // Half y length at +pDz
    hl3,   // Half x length of the side at y=-pDy2 of the face at +pDz
    hl4,   // Half x length of the side at y=+pDy2 of the face at +pDz
    alp2   // Angle with respect to the y axis from the centre of the side (upper endcap)
  );

  // Trapezoid
  return thisTrap;
}

void LeadGlassBlock::TrapParamAdjustment(const double (&in)[11], double (&out)[11]) {

  // 11 parameters from G4Trap constructor (same order)

  // get input
  double dy1 = in[3], dx1 = in[4], dx2 = in[5], alp1 = in[6];
  double dy2 = in[7], dx3 = in[8], dx4 = in[9], alp2 = in[10];

  // adjust alpha
  //alp1 = alp2 = (alp1 + alp2)/2.;
  alp1 = alp2 = std::atan((std::tan(alp1) + std::tan(alp2)) / 2.);

  // adjust dx
  double k1 = (dx1 - dx2) / (dy1 + dy1);
  double k2 = (dx3 - dx4) / (dy2 + dy2);
  double k = (k1 + k2) / 2.;
  double mn1 = (dx1 + dx2) / 2.;
  double mn2 = (dx3 + dx4) / 2.;
  dx1 = mn1 + k * dy1;
  dx2 = mn1 - k * dy1;
  dx3 = mn2 + k * dy2;
  dx4 = mn2 - k * dy2;

  // set output
  out[0] = in[0];
  out[1] = in[1];
  out[2] = in[2];
  out[3] = in[3];
  out[4] = dx1;
  out[5] = dx2;
  out[6] = alp1;
  out[7] = in[7];
  out[8] = dx3;
  out[9] = dx4;
  out[10] = alp2;
}
