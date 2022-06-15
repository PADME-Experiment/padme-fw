// ChamberGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#ifndef ChamberGeometry_H
#define ChamberGeometry_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#define CHAMBERGEOMETRY_N_PORTHOLES 8

class ChamberGeometry
{

public:

  ~ChamberGeometry();
  static ChamberGeometry* GetInstance();

private:

  static ChamberGeometry* fInstance;

protected:

  ChamberGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  // Info on the back face of the chamber
  // Needed to correctly position exit hole, HEPVeto and TimePix
  G4double GetVCBackFacePosZ() { return fVCBackFacePosZ; }
  G4double GetVCBackFaceAngle() { return fVCBackFaceAngle; }
  G4double GetVCBackFaceThickness() { return fVCBackFaceThick; }
  G4ThreeVector GetVCBackFaceCorner() { return G4ThreeVector(fVCBackFaceCornerX,0.,fVCBackFaceCornerZ); }

  // Info on section of the vacuum chamber inside magnet
  // Used to define the magnetic volume
  G4double GetVCInnerSizeX() { return fVCInnerSizeX; }
  G4double GetVCInnerSizeY() { return fVCInnerSizeY; }
  G4double GetVCInnerFacePosZ() { return fVCInnerFacePosZ; }
  G4double GetVCOuterFacePosZ() { return fVCOuterFacePosZ; }

  // Position of front face of target cross
  // Used to create magnetic volume in the cross region
  G4double GetCrossFrontFacePosZ() { return fCPZPosZ-0.5*fCPZLength; }

  // Properties of aluminum|carbon thin window and its flange

  G4double GetEWAlThick() { return fEWAlThick; }
  G4double GetEWAlConvexity() { return fEWAlConvexity; }
  G4double GetEWAlRadius() { return fEWAlFlangeRIn; }

  G4double GetEWCarbonThick() { return fEWCarbonThick; }
  G4double GetEWCarbonConvexity() { return fEWCarbonConvexity; }
  G4double GetEWCarbonRadius() { return fEWAlFlangeRIn; }

  G4double GetEWAlFlangeRIn() { return fEWAlFlangeRIn; }
  G4double GetEWAlFlangeROut() { return fEWAlFlangeROut; }
  G4double GetEWAlFlangeThick() { return fEWAlFlangeThick; }
  G4double GetEWAlFrontFacePosZ() { return fEWAlFrontFacePosZ; }

  // Properties of crossed pipes at target position

  G4double GetCPZRIn() { return fCPZRIn; }
  G4double GetCPZROut() { return fCPZRIn+fCPZThick; }
  G4double GetCPZThick() { return fCPZThick; }
  G4double GetCPZLength() { return fCPZLength; }
  G4double GetCPZPosZ() { return fCPZPosZ; }
  G4double GetCPZFlangeRIn() { return GetCPZROut(); }
  G4double GetCPZFlangeROut() { return fCPZFlangeR; }
  G4double GetCPZFlangeThick() { return fCPZFlangeThick; }

  G4double GetCPXRIn() { return fCPXRIn; }
  G4double GetCPXROut() { return fCPXRIn+fCPXThick; }
  G4double GetCPXThick() { return fCPXThick; }
  G4double GetCPXLength() { return fCPXLength; }
  G4double GetCPXPosZ() { return fCPXPosZ; }
  G4double GetCPXFlangeRIn() { return GetCPXROut(); }
  G4double GetCPXFlangeROut() { return fCPXFlangeR; }
  G4double GetCPXFlangeThick() { return fCPXFlangeThick; }

  // Properties of junction pipe between cross and chamber

  G4double GetJunRIn() { return fJunRIn; }
  G4double GetJunROut() { return fJunRIn+fJunThick; }
  G4double GetJunThick() { return fJunThick; }
  G4double GetJunLength() { return fJunLength; }
  G4double GetJunPosZ() { return fJunPosZ; }
  G4double GetJunFlangeRIn() { return GetJunROut(); }
  G4double GetJunFlangeROut() { return fJunFlangeR; }
  G4double GetJunFlangeThick() { return fJunFlangeThick; }
  G4double GetJunFrontFacePosZ() { return fJunFrontFacePosZ; }
  G4double GetJunBackFacePosZ() { return fJunBackFacePosZ; }

  // Properties of junction pipe between cross and BTF pipes

  G4double GetBTFJunRIn()    { return fBTFJunRIn; }
  G4double GetBTFJunROut()   { return fBTFJunROut; }
  G4double GetBTFJunLength() { return fBTFJunLength; }
  G4double GetBTFJunPosZ()   { return fBTFJunPosZ; }
  G4double GetBTFJunFFlgRIn()   { return fBTFJunFFlgRIn; }
  G4double GetBTFJunFFlgROut()  { return fBTFJunFFlgROut; }
  G4double GetBTFJunFFlgThick() { return fBTFJunFFlgThick; }
  G4double GetBTFJunFFlgPosZ()  { return fBTFJunFFlgPosZ; }
  G4double GetBTFJunBFlgRIn()   { return fBTFJunBFlgRIn; }
  G4double GetBTFJunBFlgROut()  { return fBTFJunBFlgROut; }
  G4double GetBTFJunBFlgThick() { return fBTFJunBFlgThick; }
  G4double GetBTFJunBFlgPosZ()  { return fBTFJunBFlgPosZ; }

  // Properties of bellow pipe between BTF junction and dipole magnet

  G4double GetBellowRIn()    { return fBellowRIn; }
  G4double GetBellowROut()   { return fBellowROut; }
  G4double GetBellowLength() { return fBellowLength; }
  G4double GetBellowPosZ()   { return fBellowPosZ; }
  G4double GetBellowFFlgRIn()   { return fBellowFFlgRIn; }
  G4double GetBellowFFlgROut()  { return fBellowFFlgROut; }
  G4double GetBellowFFlgThick() { return fBellowFFlgThick; }
  G4double GetBellowFFlgPosZ()  { return fBellowFFlgPosZ; }
  G4double GetBellowBFlgRIn()   { return fBellowBFlgRIn; }
  G4double GetBellowBFlgROut()  { return fBellowBFlgROut; }
  G4double GetBellowBFlgThick() { return fBellowBFlgThick; }
  G4double GetBellowBFlgPosZ()  { return fBellowBFlgPosZ; }

  // Properties of porthole caps

  G4int GetPHCapNumber() { return CHAMBERGEOMETRY_N_PORTHOLES; }
  G4double GetPHCapRadius(G4int c) { return fPHCapRadius[c]; }
  G4double GetPHCapThick(G4int c) { return fPHCapThick[c]; }
  G4double GetPHCapFlangeThick(G4int c) { return fPHCapFlangeThick[c]; }
  G4ThreeVector GetPHCapPos(G4int c) { return fPHCapPos[c]; }

  // Properties of large TPix porthole

  // Steel flange for rectangular porthole
  G4double GetTPPHCapThick() { return fTPPHCapThick; }
  G4double GetTPPHCapWidth() { return fTPPHCapWidth; }
  G4double GetTPPHCapHeight() { return fTPPHCapHeight; }

  // Beam exit hole
  G4double GetTPPHHoleRadius() { return fTPPHHoleRadius; }
  G4double GetTPPHHoleDist() { return fTPPHHoleDist; }

  // Service hole
  G4double GetTPPHHoleSRadius() { return fTPPHHoleSRadius; }
  G4double GetTPPHHoleSDist() { return fTPPHHoleSDist; }
 
  // Aluminum rectangular flange hosting the thin window
  G4double GetTPPHFlangeWidth() { return fTPPHFlangeWidth; }
  G4double GetTPPHFlangeHeight() { return fTPPHFlangeHeight; }
  G4double GetTPPHFlangeThick() { return fTPPHFlangeThick; }
 
  //G4double GetTPPHCircRadius() { return fTPPHCircRadius; }
  //G4double GetTPPHCircThick() { return fTPPHCircThick; }

  // Thin window dimensions (rectangle with two semicircles at sides)
  G4double GetTPPHWindRadius() { return fTPPHWindRadius; }
  G4double GetTPPHWindWidth() { return fTPPHWindWidth; }
  G4double GetTPPHWindThick() { return fTPPHWindThick; }

  // Radius and thickness of steel flange in front of service hole
  G4double GetTPPHSFlangeRadius() { return fTTPHSFlangeRadius; }
  G4double GetTPPHSFlangeThick() { return fTTPHSFlangeThick; }

  //G4double GetTPPHStopRadius() { return fTPPHStopRadius; }
  //G4double GetTPPHStopWidth() { return fTPPHStopWidth; }
  //G4double GetTPPHStopThick() { return fTPPHStopThick; }

private:

  G4int fDetectorSetup;

  G4double fVCBackFacePosZ; // Z position of the VC flange face towards the thin window
  G4double fVCBackFaceAngle; // Angle of TPix/HEPVeto face of chamber
  G4double fVCBackFaceThick; // Thickness of back faces
  G4double fVCBackFaceCornerX; // X coordinate of the corner at the back face
  G4double fVCBackFaceCornerZ; // Z coordinate of the corner at the back face

  G4double fVCInnerSizeX; // Internal size along X of the rectangular section inside the magnet
  G4double fVCInnerSizeY; // Internal size along Y of the rectangular section inside the magnet
  G4double fVCInnerFacePosZ; // Position along Z of the internal face of the rectangular section inside the magnet
  G4double fVCOuterFacePosZ; // Position along Z of the external face of the rectangular section inside the magnet

  // Aluminum|Carbon thin window and aluminum flange

  G4double fEWAlThick;
  G4double fEWAlConvexity;

  G4double fEWCarbonThick;
  G4double fEWCarbonConvexity;

  G4double fEWAlFlangeRIn;
  G4double fEWAlFlangeROut;
  G4double fEWAlFlangeThick;
  G4double fEWAlFrontFacePosZ;

  // Crossed pipes (one parallel to Z, one parallel to X) at target position

  G4double fCPZRIn;
  G4double fCPZThick;
  G4double fCPZLength;
  G4double fCPZPosZ;
  G4double fCPZFlangeR;
  G4double fCPZFlangeThick;

  G4double fCPXRIn;
  G4double fCPXThick;
  G4double fCPXLength;
  G4double fCPXPosZ;
  G4double fCPXFlangeR;
  G4double fCPXFlangeThick;

  // Junction pipe

  G4double fJunRIn;
  G4double fJunThick;
  G4double fJunFrontFacePosZ;
  G4double fJunBackFacePosZ;
  G4double fJunLength;
  G4double fJunPosZ;
  G4double fJunFlangeR;
  G4double fJunFlangeThick;

  // BTF Junction pipes

  G4double fBTFJunRIn;
  G4double fBTFJunROut;
  G4double fBTFJunLength;
  G4double fBTFJunPosZ;
  // Flange towards cross
  G4double fBTFJunFFlgRIn;
  G4double fBTFJunFFlgROut;
  G4double fBTFJunFFlgThick;
  G4double fBTFJunFFlgPosZ;
  // Flange towards bellow
  G4double fBTFJunBFlgRIn;
  G4double fBTFJunBFlgROut;
  G4double fBTFJunBFlgThick;
  G4double fBTFJunBFlgPosZ;

  // Bellow pipe between junction and magnet
  G4double fBellowRIn;
  G4double fBellowROut;
  G4double fBellowLength;
  G4double fBellowPosZ;
  // Flange towards juction
  G4double fBellowFFlgRIn;
  G4double fBellowFFlgROut;
  G4double fBellowFFlgThick;
  G4double fBellowFFlgPosZ;
  // Flange towards magnet
  G4double fBellowBFlgRIn;
  G4double fBellowBFlgROut;
  G4double fBellowBFlgThick;
  G4double fBellowBFlgPosZ;

  // Porthole caps

  G4double fPHCapRadius[CHAMBERGEOMETRY_N_PORTHOLES];
  G4double fPHCapThick[CHAMBERGEOMETRY_N_PORTHOLES];
  G4double fPHCapFlangeThick[CHAMBERGEOMETRY_N_PORTHOLES];
  G4ThreeVector fPHCapPos[CHAMBERGEOMETRY_N_PORTHOLES];

  // TPix porthole cap

  G4double fTPPHCapThick; // Thickness of rectangular porthole cap
  G4double fTPPHCapWidth; // Width of rectangular porthole cap
  G4double fTPPHCapHeight; // Height of rectangular porthole cap

  G4double fTPPHHoleRadius; // Radius of TPix beam exit hole in cap
  G4double fTPPHHoleDist; // Distance of center of beam exit hole from border of cap

  G4double fTPPHHoleSRadius; // Radius of TPix service hole in cap
  G4double fTPPHHoleSDist; // Distance of center of service hole from border of cap

  //G4double fTPPHCircRadius; // Radius of circular cap in front of hole
  //G4double fTPPHCircThick; // Thickness of circular cap in front of hole

  G4double fTPPHFlangeWidth; // Width of rectangular flange with thin window
  G4double fTPPHFlangeHeight; // Height of rectangular flange with thin window
  G4double fTPPHFlangeThick; // Thickness of rectangular flange with thin window

  G4double fTPPHWindRadius; // Radius of thin window
  G4double fTPPHWindWidth; // Width of thin window
  G4double fTPPHWindThick; // Thickness of thin window

  G4double fTTPHSFlangeRadius; // Radius of steel flange in front of service hole
  G4double fTTPHSFlangeThick; // Thickness of steel flange in front of service hole

  //G4double fTPPHStopRadius; // Radius of stop flange for Mylar window
  //G4double fTPPHStopWidth; // Width of stop flange for Mylar window
  //G4double fTPPHStopThick; // Thickness of stop flange for Mylar window

};

#endif
