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

  // Properties of aluminum thin window and its flange

  G4double GetEWAlThick() { return fEWAlThick; }
  G4double GetEWAlConvexity() { return fEWAlConvexity; }
  G4double GetEWAlRadius() { return fEWAlFlangeRIn; }
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

  G4double GetCPXRIn() { return fCPXRIn; }
  G4double GetCPXROut() { return fCPXRIn+fCPXThick; }
  G4double GetCPXThick() { return fCPXThick; }
  G4double GetCPXLength() { return fCPXLength; }
  G4double GetCPXPosZ() { return fCPXPosZ; }

  // Properties of porthole caps

  G4int GetPHCapNumber() { return CHAMBERGEOMETRY_N_PORTHOLES; }
  G4double GetPHCapRadius(G4int c) { return fPHCapRadius[c]; }
  G4double GetPHCapThick(G4int c) { return fPHCapThick[c]; }
  G4double GetPHCapFlangeThick(G4int c) { return fPHCapFlangeThick[c]; }
  G4ThreeVector GetPHCapPos(G4int c) { return fPHCapPos[c]; }

  // Properties of large TPix porthole
  G4double GetTPPHCapThick() { return fTPPHCapThick; }
  G4double GetTPPHCapWidth() { return fTPPHCapWidth; }
  G4double GetTPPHCapHeight() { return fTPPHCapHeight; }

  G4double GetTPPHHoleRadius() { return fTPPHHoleRadius; }
  G4double GetTPPHHoleDist() { return fTPPHHoleDist; }

  G4double GetTPPHCircRadius() { return fTPPHCircRadius; }
  G4double GetTPPHCircThick() { return fTPPHCircThick; }

  G4double GetTPPHWindRadius() { return fTPPHWindRadius; }
  G4double GetTPPHWindWidth() { return fTPPHWindWidth; }
  G4double GetTPPHWindThick() { return fTPPHWindThick; }

  G4double GetTPPHStopRadius() { return fTPPHStopRadius; }
  G4double GetTPPHStopWidth() { return fTPPHStopWidth; }
  G4double GetTPPHStopThick() { return fTPPHStopThick; }

private:

  G4double fVCBackFacePosZ; // Z position of the VC flange face towards the thin window
  G4double fVCBackFaceAngle; // Angle of TPix/HEPVeto face of chamber
  G4double fVCBackFaceThick; // Thickness of back faces
  G4double fVCBackFaceCornerX; // X coordinate of the corner at the back face
  G4double fVCBackFaceCornerZ; // Z coordinate of the corner at the back face

  G4double fVCInnerSizeX; // Internal size along X of the rectangular section inside the magnet
  G4double fVCInnerSizeY; // Internal size along Y of the rectangular section inside the magnet
  G4double fVCInnerFacePosZ; // Position along Z of the internal face of the rectangular section inside the magnet

  // Aluminum thin window and aluminum flange

  G4double fEWAlThick;
  G4double fEWAlConvexity;
  G4double fEWAlFlangeRIn;
  G4double fEWAlFlangeROut;
  G4double fEWAlFlangeThick;
  G4double fEWAlFrontFacePosZ;

  // Crossed pipes (one parallel to Z, one parallel to X) at target position

  G4double fCPZRIn;
  G4double fCPZThick;
  G4double fCPZLength;
  G4double fCPZPosZ;

  G4double fCPXRIn;
  G4double fCPXThick;
  G4double fCPXLength;
  G4double fCPXPosZ;

  // Porthole caps
  G4double fPHCapRadius[CHAMBERGEOMETRY_N_PORTHOLES];
  G4double fPHCapThick[CHAMBERGEOMETRY_N_PORTHOLES];
  G4double fPHCapFlangeThick[CHAMBERGEOMETRY_N_PORTHOLES];
  G4ThreeVector fPHCapPos[CHAMBERGEOMETRY_N_PORTHOLES];

  // TPix porthole cap
  G4double fTPPHCapThick; // Thickness of rectangular porthole cap
  G4double fTPPHCapWidth; // Width of rectangular porthole cap
  G4double fTPPHCapHeight; // Height of rectangular porthole cap

  G4double fTPPHHoleRadius; // Radius of TPix hole in cap
  G4double fTPPHHoleDist; // Distance of center of hole from border of cap

  G4double fTPPHCircRadius; // Radius of circular cap in front of hole
  G4double fTPPHCircThick; // Thickness of circular cap in front of hole

  G4double fTPPHWindRadius; // Radius of Mylar window
  G4double fTPPHWindWidth; // Width of Mylar window
  G4double fTPPHWindThick; // Thickness of Mylar window

  G4double fTPPHStopRadius; // Radius of stop flange for Mylar window
  G4double fTPPHStopWidth; // Width of stop flange for Mylar window
  G4double fTPPHStopThick; // Thickness of stop flange for Mylar window

};

#endif
