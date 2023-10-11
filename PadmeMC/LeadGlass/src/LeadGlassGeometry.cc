// LeadGlassGeometry.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-20
// --------------------------------------------------------------

#include "LeadGlassGeometry.hh"

LeadGlassGeometry* LeadGlassGeometry::fInstance = 0;

LeadGlassGeometry* LeadGlassGeometry::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new LeadGlassGeometry(); }
  return fInstance;
}

LeadGlassGeometry::LeadGlassGeometry()
{

  fVerbose = 0; // Do not show debug output

  fDetectorSetup = 10; // Default to 2019 setup

  fBlockId = 1; // OPAL block type 8

  fLeadGlassFrontFacePosZ = 3100.*mm; // Position of block front face

  fLeadGlassDigitizerName = "LeadGlassDigitizer";
  fLeadGlassSensitiveDetectorName = "LeadGlassSD";

  // Opal id of different blocks
  fBlockOpalId[0] = 7;
  fBlockOpalId[1] = 8;
  fBlockOpalId[2] = 9;
  fBlockOpalId[3] = 10;
  fBlockOpalId[4] = 11;
  fBlockOpalId[5] = 12;
  fBlockOpalId[6] = 13;
  fBlockOpalId[7] = 14;
  fBlockOpalId[8] = 15;

  // Block OPAL 7
  fBlockZLength[0] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[0] = 109.526 * mm;      // L1 length (long base of back face)
  fBlockL2Length[0] = 107.003 * mm;      // L2 length (short base of back face)
  fBlockL3Length[0] = 97.575 * mm;       // L3 length (long base of front face)
  fBlockL4Length[0] = 95.318 * mm;       // L4 length (short base of front face)
  fBlockW1Length[0] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[0] = 98.250 * mm;       // W2 length (height of front face)
  fLightGuideZLength[0] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[0] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[0] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[0] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[0] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[0] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 8
  fBlockZLength[1] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[1] = 109.824 * mm;      // L1 length (long base of back face)
  fBlockL2Length[1] = 107.536 * mm;      // L2 length (short base of back face)
  fBlockL3Length[1] = 97.346 * mm;       // L3 length (long base of front face)
  fBlockL4Length[1] = 95.310 * mm;       // L4 length (short base of front face)
  fBlockW1Length[1] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[1] = 97.740 * mm;       // W2 length (height of front face)
  fLightGuideZLength[1] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[1] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[1] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[1] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[1] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[1] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 9
  fBlockZLength[2] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[2] = 110.073 * mm;      // L1 length (long base of back face)
  fBlockL2Length[2] = 108.040 * mm;      // L2 length (short base of back face)
  fBlockL3Length[2] = 97.103 * mm;       // L3 length (long base of front face)
  fBlockL4Length[2] = 95.302 * mm;       // L4 length (short base of front face)
  fBlockW1Length[2] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[2] = 97.261 * mm;       // W2 length (height of front face)
  fLightGuideZLength[2] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[2] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[2] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[2] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[2] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[2] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 10
  fBlockZLength[3] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[3] = 110.266 * mm;      // L1 length (long base of back face)
  fBlockL2Length[3] = 108.506 * mm;      // L2 length (short base of back face)
  fBlockL3Length[3] = 96.846 * mm;       // L3 length (long base of front face)
  fBlockL4Length[3] = 95.294 * mm;       // L4 length (short base of front face)
  fBlockW1Length[3] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[3] = 96.820 * mm;       // W2 length (height of front face)
  fLightGuideZLength[3] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[3] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[3] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[3] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[3] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[3] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 11
  fBlockZLength[4] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[4] = 110.391 * mm;      // L1 length (long base of back face)
  fBlockL2Length[4] = 108.923 * mm;      // L2 length (short base of back face)
  fBlockL3Length[4] = 96.577 * mm;       // L3 length (long base of front face)
  fBlockL4Length[4] = 95.288 * mm;       // L4 length (short base of front face)
  fBlockW1Length[4] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[4] = 96.426 * mm;       // W2 length (height of front face)
  fLightGuideZLength[4] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[4] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[4] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[4] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[4] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[4] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 12
  fBlockZLength[5] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[5] = 110.439 * mm;      // L1 length (long base of back face)
  fBlockL2Length[5] = 109.281 * mm;      // L2 length (short base of back face)
  fBlockL3Length[5] = 96.298 * mm;       // L3 length (long base of front face)
  fBlockL4Length[5] = 95.284 * mm;       // L4 length (short base of front face)
  fBlockW1Length[5] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[5] = 96.086 * mm;       // W2 length (height of front face)
  fLightGuideZLength[5] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[5] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[5] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[5] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[5] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[5] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 13
  fBlockZLength[6] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[6] = 110.366 * mm;      // L1 length (long base of back face)
  fBlockL2Length[6] = 109.572 * mm;      // L2 length (short base of back face)
  fBlockL3Length[6] = 96.010 * mm;       // L3 length (long base of front face)
  fBlockL4Length[6] = 95.317 * mm;       // L4 length (short base of front face)
  fBlockW1Length[6] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[6] = 95.808 * mm;       // W2 length (height of front face)
  fLightGuideZLength[6] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[6] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[6] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[6] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[6] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[6] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 14
  fBlockZLength[7] = 370.000 * mm;   // Z length (PbGl only)
  fBlockL1Length[7] = 110.204 * mm;  // L1 length (long base of back face)
  fBlockL2Length[7] = 109.786 * mm;  // L2 length (short base of back face)
  fBlockL3Length[7] = 95.717 * mm;   // L3 length (long base of front face)
  fBlockL4Length[7] = 95.353 * mm;   // L4 length (short base of front face)
  fBlockW1Length[7] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[7] = 95.598 * mm;       // W2 length (height of front face)
  fLightGuideZLength[7] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[7] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[7] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[7] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[7] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[7] = 15.000 * mm;  // Steel slab thickness

  // Block OPAL 15
  fBlockZLength[8] = 370.000 * mm;       // Z length (PbGl only)
  fBlockL1Length[8] = 109.952 * mm;      // L1 length (long base of back face)
  fBlockL2Length[8] = 109.918 * mm;      // L2 length (short base of back face)
  fBlockL3Length[8] = 95.420 * mm;       // L3 length (long base of front face)
  fBlockL4Length[8] = 95.391 * mm;       // L4 length (short base of front face)
  fBlockW1Length[8] = 109.952 * mm;      // W1 length (height of back face)
  fBlockW2Length[8] = 95.461 * mm;       // W2 length (height of front face)
  fLightGuideZLength[8] = 60.000 * mm;   // Light Guide Z length
  fLightGuideDiameter[8] = 73.000 * mm;  // Light Guide diameter
  fMuMetalZLength[8] = 120.800 * mm;     // u-metal cylinder Z length
  fMuMetalDiameter[8] = 83.800 * mm;     // u-metal cylinder external diameter
  fMuMetalThickness[8] = 2.100 * mm;     // u-metal cylinder thickness
  fSteelSlabThickness[8] = 15.000 * mm;  // Steel slab thickness

}

std::vector<G4String> LeadGlassGeometry::GetHashTable()
{
  std::vector<G4String> hash;
  std::ostringstream buffer;
  return hash;
}

LeadGlassGeometry::~LeadGlassGeometry()
{}
