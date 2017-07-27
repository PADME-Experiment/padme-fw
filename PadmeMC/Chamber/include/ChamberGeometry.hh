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

private:

  G4double fVCBackFacePosZ; // Z position of the VC flange face towards the thin window
  G4double fVCBackFaceAngle; // Angle of TPix/HEPVeto face of chamber
  G4double fVCBackFaceThick; // Thickness of back faces
  G4double fVCBackFaceCornerX; // X coordinate of the corner at the back face
  G4double fVCBackFaceCornerZ; // Z coordinate of the corner at the back face

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

};

#endif
