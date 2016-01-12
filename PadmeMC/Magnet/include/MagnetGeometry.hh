// MagnetGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-01-11
// --------------------------------------------------------------

#ifndef MagnetGeometry_H
#define MagnetGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class MagnetGeometry
{

public:

  ~MagnetGeometry();
  static MagnetGeometry* GetInstance();

private:

  static MagnetGeometry* fInstance;

protected:

  MagnetGeometry();

public:

  G4String GetMagnetSensitiveDetectorName() { return fMagnetSensitiveDetectorName; }

  // Size of gap between sections of Magnet yoke

  G4double GetMagnetGap() { return fMagnetGap; };

  // Size of Magnet yoke sections

  G4double GetMagnetUpSizeX() { return 0.5*(fMagnetSizeOuterX-fMagnetSizeInnerX); }
  G4double GetMagnetUpSizeY() { return fMagnetSizeOuterY; }
  G4double GetMagnetUpSizeZ() { return fMagnetSizeZ; }

  G4double GetMagnetDownSizeX() { return 0.5*(fMagnetSizeOuterX-fMagnetSizeInnerX); }
  G4double GetMagnetDownSizeY() { return fMagnetSizeOuterY; }
  G4double GetMagnetDownSizeZ() { return fMagnetSizeZ; }

  G4double GetMagnetRightSizeX() { return fMagnetSizeInnerX; }
  G4double GetMagnetRightSizeY() { return 0.5*(fMagnetSizeOuterY-fMagnetSizeInnerY); }
  G4double GetMagnetRightSizeZ() { return fMagnetSizeZ; }

  G4double GetMagnetLeftSizeX() { return fMagnetSizeInnerX; }
  G4double GetMagnetLeftSizeY() { return 0.5*(fMagnetSizeOuterY-fMagnetSizeInnerY); }
  G4double GetMagnetLeftSizeZ() { return fMagnetSizeZ; }

  // Positions of Magnet yoke sections

  G4double GetMagnetUpPosX() { return 0.25*(fMagnetSizeInnerX+fMagnetSizeOuterX); }
  G4double GetMagnetUpPosY() { return 0.; }
  G4double GetMagnetUpPosZ() { return fMagnetFrontFacePosZ+0.5*fMagnetSizeZ; }

  G4double GetMagnetDownPosX() { return -0.25*(fMagnetSizeInnerX+fMagnetSizeOuterX); }
  G4double GetMagnetDownPosY() { return 0.; }
  G4double GetMagnetDownPosZ() { return fMagnetFrontFacePosZ+0.5*fMagnetSizeZ; }

  G4double GetMagnetRightPosX() { return 0.; }
  G4double GetMagnetRightPosY() { return 0.25*(fMagnetSizeInnerY+fMagnetSizeOuterY); }
  G4double GetMagnetRightPosZ() { return fMagnetFrontFacePosZ+0.5*fMagnetSizeZ; }

  G4double GetMagnetLeftPosX() { return 0.; }
  G4double GetMagnetLeftPosY() { return -0.25*(fMagnetSizeInnerY+fMagnetSizeOuterY); }
  G4double GetMagnetLeftPosZ() { return fMagnetFrontFacePosZ+0.5*fMagnetSizeZ; }

  // Size of Magnet coil (half circle)

  G4double GetCoilInnerRadius() { return fCoilInnerRadius; }
  G4double GetCoilOuterRadius() { return fCoilOuterRadius; }
  G4double GetCoilLength()      { return fCoilLength; }
  G4double GetCoilSpanStart()   { return fCoilSpanStart; }
  G4double GetCoilSpan()        { return fCoilSpan; }

  // Rotations of Magnet coils

  G4double GetCoilFrontUpRotX()   { return   0.*deg; }
  G4double GetCoilFrontUpRotY()   { return -90.*deg; }
  G4double GetCoilFrontUpRotZ()   { return  90.*deg; }

  G4double GetCoilFrontDownRotX() { return   0.*deg; }
  G4double GetCoilFrontDownRotY() { return -90.*deg; }
  G4double GetCoilFrontDownRotZ() { return  90.*deg; }

  G4double GetCoilBackUpRotX()    { return   0.*deg; }
  G4double GetCoilBackUpRotY()    { return  90.*deg; }
  G4double GetCoilBackUpRotZ()    { return  90.*deg; }

  G4double GetCoilBackDownRotX()  { return   0.*deg; }
  G4double GetCoilBackDownRotY()  { return  90.*deg; }
  G4double GetCoilBackDownRotZ()  { return  90.*deg; }

  // Positions of Magnet coils

  G4double GetCoilFrontUpPosX() { return 0.5*fMagnetSizeInnerX+fCoilLength; }
  G4double GetCoilFrontUpPosY() { return 0.*cm; }
  G4double GetCoilFrontUpPosZ() { return fMagnetFrontFacePosZ; }

  G4double GetCoilFrontDownPosX() { return -0.5*fMagnetSizeInnerX-fCoilLength; }
  G4double GetCoilFrontDownPosY() { return 0.*cm; }
  G4double GetCoilFrontDownPosZ() { return fMagnetFrontFacePosZ; }

  G4double GetCoilBackUpPosX() { return 0.5*fMagnetSizeInnerX+fCoilLength; }
  G4double GetCoilBackUpPosY() { return 0.*cm; }
  G4double GetCoilBackUpPosZ() { return fMagnetFrontFacePosZ+fMagnetSizeZ; }

  G4double GetCoilBackDownPosX() { return -0.5*fMagnetSizeInnerX-fCoilLength; }
  G4double GetCoilBackDownPosY() { return 0.*cm; }
  G4double GetCoilBackDownPosZ() { return fMagnetFrontFacePosZ+fMagnetSizeZ; }

private:

  G4double fMagnetGap;

  G4double fMagnetSizeInnerX;
  G4double fMagnetSizeInnerY;
  G4double fMagnetSizeOuterX;
  G4double fMagnetSizeOuterY;
  G4double fMagnetSizeZ;

  G4double fCoilInnerRadius;
  G4double fCoilOuterRadius;
  G4double fCoilLength;
  G4double fCoilSpanStart;
  G4double fCoilSpan;

  G4double fMagnetFrontFacePosZ; // Front face of Magnet yoke

  G4String fMagnetSensitiveDetectorName;

};

#endif
