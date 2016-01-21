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

  G4double GetMagnetUpBarSizeX() { return fMagnetSizeInBarX; }
  G4double GetMagnetUpBarSizeY() { return fMagnetSizeInBarY; }
  G4double GetMagnetUpBarSizeZ() { return fMagnetSizeZ; }

  G4double GetMagnetDownBarSizeX() { return fMagnetSizeInBarX; }
  G4double GetMagnetDownBarSizeY() { return fMagnetSizeInBarY; }
  G4double GetMagnetDownBarSizeZ() { return fMagnetSizeZ; }

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

  G4double GetMagnetUpBarPosX() { return 0.5*fMagnetSizeInnerX-0.5*fMagnetSizeInBarX; }
  G4double GetMagnetUpBarPosY() { return 0.*cm; }
  G4double GetMagnetUpBarPosZ() { return fMagnetFrontFacePosZ+0.5*fMagnetSizeZ; }

  G4double GetMagnetDownBarPosX() { return -0.5*fMagnetSizeInnerX+0.5*fMagnetSizeInBarX; }
  G4double GetMagnetDownBarPosY() { return 0.*cm; }
  G4double GetMagnetDownBarPosZ() { return fMagnetFrontFacePosZ+0.5*fMagnetSizeZ; }

  // Size of Magnet coil

  G4double GetCoilInnerRadius() { return fCoilInnerRadius; }
  G4double GetCoilOuterRadius() { return fCoilOuterRadius; }
  G4double GetCoilSizeX() { return fCoilLengthX; }

  G4double GetCoilUFSizeX() { return fCoilLengthX; }
  G4double GetCoilUFSizeY() { return fCoilLengthY; }
  G4double GetCoilUFSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilUFPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilUFPosY()  { return 0.*cm; }
  G4double GetCoilUFPosZ()  { return fMagnetFrontFacePosZ-0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  G4double GetCoilUBSizeX() { return fCoilLengthX; }
  G4double GetCoilUBSizeY() { return fCoilLengthY; }
  G4double GetCoilUBSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilUBPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilUBPosY()  { return 0.*cm; }
  G4double GetCoilUBPosZ()  { return fMagnetFrontFacePosZ+fCoilLengthZ+0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  G4double GetCoilULSizeX() { return fCoilLengthX; }
  G4double GetCoilULSizeY() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilULSizeZ() { return fCoilLengthZ; }
  G4double GetCoilULPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilULPosY()  { return -0.5*fCoilLengthY-0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilULPosZ()  { return fMagnetFrontFacePosZ+0.5*fCoilLengthZ; }

  G4double GetCoilURSizeX() { return fCoilLengthX; }
  G4double GetCoilURSizeY() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilURSizeZ() { return fCoilLengthZ; }
  G4double GetCoilURPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilURPosY()  { return 0.5*fCoilLengthY+0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilURPosZ()  { return fMagnetFrontFacePosZ+0.5*fCoilLengthZ; }

  G4double GetCoilDFSizeX() { return fCoilLengthX; }
  G4double GetCoilDFSizeY() { return fCoilLengthY; }
  G4double GetCoilDFSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDFPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDFPosY()  { return 0.*cm; }
  G4double GetCoilDFPosZ()  { return fMagnetFrontFacePosZ-0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  G4double GetCoilDBSizeX() { return fCoilLengthX; }
  G4double GetCoilDBSizeY() { return fCoilLengthY; }
  G4double GetCoilDBSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDBPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDBPosY()  { return 0.*cm; }
  G4double GetCoilDBPosZ()  { return fMagnetFrontFacePosZ+fCoilLengthZ+0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  G4double GetCoilDLSizeX() { return fCoilLengthX; }
  G4double GetCoilDLSizeY() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDLSizeZ() { return fCoilLengthZ; }
  G4double GetCoilDLPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDLPosY()  { return -0.5*fCoilLengthY-0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilDLPosZ()  { return fMagnetFrontFacePosZ+0.5*fCoilLengthZ; }

  G4double GetCoilDRSizeX() { return fCoilLengthX; }
  G4double GetCoilDRSizeY() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDRSizeZ() { return fCoilLengthZ; }
  G4double GetCoilDRPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDRPosY()  { return 0.5*fCoilLengthY+0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilDRPosZ()  { return fMagnetFrontFacePosZ+0.5*fCoilLengthZ; }

  G4double GetCoilUFLAngle() { return 180.*deg; }
  G4double GetCoilUFLPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilUFLPosY()  { return -0.5*fCoilLengthY; }
  G4double GetCoilUFLPosZ()  { return fMagnetFrontFacePosZ; }

  G4double GetCoilUFRAngle() { return 90.*deg; }
  G4double GetCoilUFRPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilUFRPosY()  { return 0.5*fCoilLengthY; }
  G4double GetCoilUFRPosZ()  { return fMagnetFrontFacePosZ; }

  G4double GetCoilUBLAngle() { return 270.*deg; }
  G4double GetCoilUBLPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilUBLPosY()  { return -0.5*fCoilLengthY; }
  G4double GetCoilUBLPosZ()  { return fMagnetFrontFacePosZ+fCoilLengthZ; }

  G4double GetCoilUBRAngle() { return 0.*deg; }
  G4double GetCoilUBRPosX()  { return 0.5*fMagnetSizeInnerX-fNeopreneThick-0.5*fCoilLengthX; }
  G4double GetCoilUBRPosY()  { return 0.5*fCoilLengthY; }
  G4double GetCoilUBRPosZ()  { return fMagnetFrontFacePosZ+fCoilLengthZ; }

  G4double GetCoilDFLAngle() { return 180.*deg; }
  G4double GetCoilDFLPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDFLPosY()  { return -0.5*fCoilLengthY; }
  G4double GetCoilDFLPosZ()  { return fMagnetFrontFacePosZ; }

  G4double GetCoilDFRAngle() { return 90.*deg; }
  G4double GetCoilDFRPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDFRPosY()  { return 0.5*fCoilLengthY; }
  G4double GetCoilDFRPosZ()  { return fMagnetFrontFacePosZ; }

  G4double GetCoilDBLAngle() { return 270.*deg; }
  G4double GetCoilDBLPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDBLPosY()  { return -0.5*fCoilLengthY; }
  G4double GetCoilDBLPosZ()  { return fMagnetFrontFacePosZ+fCoilLengthZ; }

  G4double GetCoilDBRAngle() { return 0.*deg; }
  G4double GetCoilDBRPosX()  { return -0.5*fMagnetSizeInnerX+fNeopreneThick+0.5*fCoilLengthX; }
  G4double GetCoilDBRPosY()  { return 0.5*fCoilLengthY; }
  G4double GetCoilDBRPosZ()  { return fMagnetFrontFacePosZ+fCoilLengthZ; }

  G4double GetSlabULSizeX() { return fSlabThick; }
  G4double GetSlabULSizeY() { return fSlabWidth; }
  G4double GetSlabULSizeZ() { return fSlabLengthZ; }
  G4double GetSlabULPosX()  { return  0.5*fMagnetSizeInnerX-2.*fNeopreneThick-fCoilLengthX-0.5*fSlabThick; }
  G4double GetSlabULPosY()  { return -0.5*fMagnetSizeInnerY+0.5*fSlabWidth; }
  G4double GetSlabULPosZ()  { return fMagnetFrontFacePosZ+0.5*fSlabLengthZ; }

  G4double GetSlabURSizeX() { return fSlabThick; }
  G4double GetSlabURSizeY() { return fSlabWidth; }
  G4double GetSlabURSizeZ() { return fSlabLengthZ; }
  G4double GetSlabURPosX()  { return  0.5*fMagnetSizeInnerX-2.*fNeopreneThick-fCoilLengthX-0.5*fSlabThick; }
  G4double GetSlabURPosY()  { return  0.5*fMagnetSizeInnerY-0.5*fSlabWidth; }
  G4double GetSlabURPosZ()  { return fMagnetFrontFacePosZ+0.5*fSlabLengthZ; }

  G4double GetSlabDLSizeX() { return fSlabThick; }
  G4double GetSlabDLSizeY() { return fSlabWidth; }
  G4double GetSlabDLSizeZ() { return fSlabLengthZ; }
  G4double GetSlabDLPosX()  { return -0.5*fMagnetSizeInnerX+2.*fNeopreneThick+fCoilLengthX+0.5*fSlabThick; }
  G4double GetSlabDLPosY()  { return -0.5*fMagnetSizeInnerY+0.5*fSlabWidth; }
  G4double GetSlabDLPosZ()  { return fMagnetFrontFacePosZ+0.5*fSlabLengthZ; }

  G4double GetSlabDRSizeX() { return fSlabThick; }
  G4double GetSlabDRSizeY() { return fSlabWidth; }
  G4double GetSlabDRSizeZ() { return fSlabLengthZ; }
  G4double GetSlabDRPosX()  { return -0.5*fMagnetSizeInnerX+2.*fNeopreneThick+fCoilLengthX+0.5*fSlabThick; }
  G4double GetSlabDRPosY()  { return  0.5*fMagnetSizeInnerY-0.5*fSlabWidth; }
  G4double GetSlabDRPosZ()  { return fMagnetFrontFacePosZ+0.5*fSlabLengthZ; }

  G4double GetRailULSizeX() { return fRailThick; }
  G4double GetRailULSizeY() { return fRailWidth; }
  G4double GetRailULSizeZ() { return fRailLengthZ; }
  G4double GetRailULPosX()  { return  0.5*fMagnetSizeInnerX-fMagnetSizeInBarX-0.5*fRailThick; }
  G4double GetRailULPosY()  { return -0.5*fMagnetSizeInBarY+0.5*fRailWidth; }
  G4double GetRailULPosZ()  { return fMagnetFrontFacePosZ+0.5*fRailLengthZ; }

  G4double GetRailURSizeX() { return fRailThick; }
  G4double GetRailURSizeY() { return fRailWidth; }
  G4double GetRailURSizeZ() { return fRailLengthZ; }
  G4double GetRailURPosX()  { return  0.5*fMagnetSizeInnerX-fMagnetSizeInBarX-0.5*fRailThick; }
  G4double GetRailURPosY()  { return  0.5*fMagnetSizeInBarY-0.5*fRailWidth; }
  G4double GetRailURPosZ()  { return fMagnetFrontFacePosZ+0.5*fRailLengthZ; }

  G4double GetRailDLSizeX() { return fRailThick; }
  G4double GetRailDLSizeY() { return fRailWidth; }
  G4double GetRailDLSizeZ() { return fRailLengthZ; }
  G4double GetRailDLPosX()  { return -0.5*fMagnetSizeInnerX+fMagnetSizeInBarX+0.5*fRailThick; }
  G4double GetRailDLPosY()  { return -0.5*fMagnetSizeInBarY+0.5*fRailWidth; }
  G4double GetRailDLPosZ()  { return fMagnetFrontFacePosZ+0.5*fRailLengthZ; }

  G4double GetRailDRSizeX() { return fRailThick; }
  G4double GetRailDRSizeY() { return fRailWidth; }
  G4double GetRailDRSizeZ() { return fRailLengthZ; }
  G4double GetRailDRPosX()  { return -0.5*fMagnetSizeInnerX+fMagnetSizeInBarX+0.5*fRailThick; }
  G4double GetRailDRPosY()  { return  0.5*fMagnetSizeInBarY-0.5*fRailWidth; }
  G4double GetRailDRPosZ()  { return fMagnetFrontFacePosZ+0.5*fRailLengthZ; }

  G4double GetNeopreneULTSizeX() { return fNeopreneThick; }
  G4double GetNeopreneULTSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneULTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneULTPosX()  { return  0.5*fMagnetSizeInnerX-0.5*fNeopreneThick; }
  G4double GetNeopreneULTPosY()  { return -0.5*fCoilLengthY-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneULTPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetNeopreneULBSizeX() { return fNeopreneThick; }
  G4double GetNeopreneULBSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneULBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneULBPosX()  { return  0.5*fMagnetSizeInnerX-fCoilLengthX-1.5*fNeopreneThick; }
  G4double GetNeopreneULBPosY()  { return -0.5*fCoilLengthY-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneULBPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetNeopreneURTSizeX() { return fNeopreneThick; }
  G4double GetNeopreneURTSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneURTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneURTPosX()  { return  0.5*fMagnetSizeInnerX-0.5*fNeopreneThick; }
  G4double GetNeopreneURTPosY()  { return  0.5*fCoilLengthY+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneURTPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetNeopreneURBSizeX() { return fNeopreneThick; }
  G4double GetNeopreneURBSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneURBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneURBPosX()  { return  0.5*fMagnetSizeInnerX-fCoilLengthX-1.5*fNeopreneThick; }
  G4double GetNeopreneURBPosY()  { return  0.5*fCoilLengthY+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneURBPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetNeopreneDLTSizeX() { return fNeopreneThick; }
  G4double GetNeopreneDLTSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneDLTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDLTPosX()  { return -0.5*fMagnetSizeInnerX+fCoilLengthX+1.5*fNeopreneThick; }
  G4double GetNeopreneDLTPosY()  { return -0.5*fCoilLengthY-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneDLTPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetNeopreneDLBSizeX() { return fNeopreneThick; }
  G4double GetNeopreneDLBSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneDLBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDLBPosX()  { return -0.5*fMagnetSizeInnerX+0.5*fNeopreneThick; }
  G4double GetNeopreneDLBPosY()  { return -0.5*fCoilLengthY-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneDLBPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetNeopreneDRTSizeX() { return fNeopreneThick; }
  G4double GetNeopreneDRTSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneDRTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDRTPosX()  { return -0.5*fMagnetSizeInnerX+fCoilLengthX+1.5*fNeopreneThick; }
  G4double GetNeopreneDRTPosY()  { return  0.5*fCoilLengthY+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneDRTPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetNeopreneDRBSizeX() { return fNeopreneThick; }
  G4double GetNeopreneDRBSizeY() { return fNeopreneWidth; }
  G4double GetNeopreneDRBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDRBPosX()  { return -0.5*fMagnetSizeInnerX+0.5*fNeopreneThick; }
  G4double GetNeopreneDRBPosY()  { return  0.5*fCoilLengthY+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneDRBPosZ()  { return fMagnetFrontFacePosZ+0.5*fNeopreneLengthZ; }

  G4double GetMagneticVolumeSizeX() { return fMagneticVolumeLengthX; }
  G4double GetMagneticVolumeSizeY() { return fMagneticVolumeLengthY; }
  G4double GetMagneticVolumeSizeZ() { return fMagneticVolumeLengthZ; }
  G4double GetMagneticVolumePosX()  { return 0.; }
  G4double GetMagneticVolumePosY()  { return 0.; }
  G4double GetMagneticVolumePosZ()  { return fMagnetFrontFacePosZ+0.5*fMagneticVolumeLengthZ; }

  void SetMagnetFrontFacePosZ(G4double z) { fMagnetFrontFacePosZ = z; }

private:

  G4double fMagnetGap;

  G4double fMagnetSizeZ;
  G4double fMagnetSizeOuterX;
  G4double fMagnetSizeOuterY;
  G4double fMagnetSizeInnerX;
  G4double fMagnetSizeInnerY;
  G4double fMagnetSizeInBarX;
  G4double fMagnetSizeInBarY;

  G4double fCoilInnerRadius;
  G4double fCoilOuterRadius;
  G4double fCoilLengthX;
  G4double fCoilLengthY;
  G4double fCoilLengthZ;

  G4double fSlabThick;
  G4double fSlabWidth;
  G4double fSlabLengthZ;

  G4double fRailThick;
  G4double fRailWidth;
  G4double fRailLengthZ;

  G4double fNeopreneThick;
  G4double fNeopreneWidth;
  G4double fNeopreneLengthZ;

  G4double fMagneticVolumeLengthX;
  G4double fMagneticVolumeLengthY;
  G4double fMagneticVolumeLengthZ;

  G4double fMagnetFrontFacePosZ; // Front face of Magnet yoke

  G4String fMagnetSensitiveDetectorName;

};

#endif
