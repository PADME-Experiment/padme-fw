// ChamberGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-06-10
// --------------------------------------------------------------

#ifndef ChamberGeometry_H
#define ChamberGeometry_H 1

#define CHAMBERGEOMETRY_VCINMAGWALL_NVERTICES 14
#define CHAMBERGEOMETRY_VCINMAGPLATE_NVERTICES 6
#define CHAMBERGEOMETRY_VCOUTMAGWALL_NVERTICES 6

#include "globals.hh"
#include "G4TwoVector.hh"
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

  // This is the maximum Z reached by the whole VC structure
  // Needed to define the main magnetic volume
  G4double GetVCMostExternalX() { return fVCMostExternalX; }
  G4double GetVCMostAdvancedZ() { return fEWBackFacePosZ+fEWF1Thick; }

  G4double GetVCInMagThick() { return fVCInMagThick; }

  G4double GetVCInMagWallSizeY() { return fVCInMagWallSizeY; }
  G4int GetVCInMagWallNVertices() { return CHAMBERGEOMETRY_VCINMAGWALL_NVERTICES; }
  G4TwoVector GetVCInMagWallVertex(G4int p) { return fVCInMagWallVertex[p]; }

  G4double GetVCInMagWallHoleRadius() { return fVCInMagWallHoleRadius; }
  G4double GetVCInMagWallHolePosZ() { return fVCInMagWallHolePosZ; }

  G4int GetVCInMagPlateNVertices() { return CHAMBERGEOMETRY_VCINMAGPLATE_NVERTICES; }
  G4TwoVector GetVCInMagPlateVertex(G4int p) { return fVCInMagPlateVertex[p]; }

  G4double GetVCOutMagWallSizeY() { return fVCInMagWallSizeY; }
  G4int GetVCOutMagWallNVertices() { return CHAMBERGEOMETRY_VCOUTMAGWALL_NVERTICES; }
  G4TwoVector GetVCOutMagWallVertex(G4int p) { return fVCOutMagWallVertex[p]; }

  G4double GetVCBackFacePosZ() { return fVCBackFacePosZ; }

  // Properties of the big vacuum chamber cylinder

  // Flange to connect ECal thin window
  G4double GetVCCFPosZ() { return fVCBackFacePosZ-0.5*fVCCFThick; }
  G4double GetVCCFThick() { return fVCCFThick; }
  G4double GetVCCFRIn() { return fEWF3RIn; }
  G4double GetVCCFROut() { return fEWF2ROut; }

  // Big cylinder
  G4double GetVCCPosZ() { return fVCBackFacePosZ-fVCCFThick-0.5*fVCCLength; }
  G4double GetVCCLength() { return fVCCLength; }
  G4double GetVCCThick() { return fVCCThick; }
  G4double GetVCCRIn() { return fVCCRIn; }
  G4double GetVCCROut() { return fVCCRIn+fVCCThick; }

  // Properties of thin window

  G4double GetEWRadius() { return fEWRadius; }
  G4double GetEWConvexity() { return fEWConvexity; }
  G4double GetEWFrontMylarThick() { return fEWFrontMylarThick; }
  G4double GetEWKevlarThick() { return fEWKevlarThick; }
  G4double GetEWBackMylarThick() { return fEWBackMylarThick; }

  // Properties of thin window flange

  G4double GetEWF1PosZ() { return fEWBackFacePosZ+0.5*fEWF1Thick; }
  G4double GetEWF1Thick() { return fEWF1Thick; }
  G4double GetEWF1RIn() { return fEWRadius; }
  G4double GetEWF1ROut() { return fEWF1ROut; }

  G4double GetEWF2PosZ() { return fEWBackFacePosZ-fEWFrontMylarThick-fEWKevlarThick-fEWBackMylarThick-0.5*fEWF2Thick; }
  G4double GetEWF2Thick() { return fEWF2Thick; }
  G4double GetEWF2RIn() { return fEWRadius; }
  G4double GetEWF2ROut() { return fEWF2ROut; }

  G4double GetEWF3PosZ() { return fEWBackFacePosZ-fEWFrontMylarThick-fEWKevlarThick-fEWBackMylarThick-fEWF2Thick-0.5*fEWF3Thick; }
  G4double GetEWF3Thick() { return fEWF3Thick; }
  G4double GetEWF3RIn() { return fEWF3RIn; }
  G4double GetEWF3ROut() { return fEWF2ROut; }

private:

  G4double fVCInMagThick;

  G4double fVCInMagWallSizeY;
  G4TwoVector fVCInMagWallVertex[CHAMBERGEOMETRY_VCINMAGWALL_NVERTICES];
  G4double fVCInMagWallHoleRadius;
  G4double fVCInMagWallHolePosZ;

  G4TwoVector fVCInMagPlateVertex[CHAMBERGEOMETRY_VCINMAGPLATE_NVERTICES];

  G4TwoVector fVCOutMagWallVertex[CHAMBERGEOMETRY_VCOUTMAGWALL_NVERTICES];

  G4double fVCMostExternalX; // Absolute value of largest X coordinate of the chamber

  G4double fVCCFThick; // Thickness of the VC flange toward thin window flange

  G4double fVCCThick; // Thickness of the big cylinder
  G4double fVCCRIn; // Inner radius of the big cylinder
  G4double fVCCLength; // Length of the big cylinder

  G4double fVCBackFacePosZ; // Z position of the VC flange face towards the thin window

  //G4double fVCEWGap; // Gap for o-ring between main VC body and thin window flange

  // Thin window in front of ECal (EW)

  G4double fEWBackFacePosZ; // Z position of the external thin window face

  G4double fEWRadius; // Radius of window
  G4double fEWConvexity; // Convexity of window at its center

  G4double fEWFrontMylarThick; // Thickness of mylar layer inside VC
  G4double fEWKevlarThick; // Thickness of kevlar layer between mylar layers
  G4double fEWBackMylarThick;  // Thickness of mylar layer outside VC

  // Steel flange around thin window (three steel rings)

  G4double fEWF1Thick;
  G4double fEWF2Thick;
  G4double fEWF3Thick;

  G4double fEWF1ROut;
  G4double fEWF2ROut;
  G4double fEWF3RIn;

};

#endif
