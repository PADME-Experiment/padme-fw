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

  // === Magnet yoke ===

  // Up (Y>0) section
  G4double GetYokeUpSizeX() { return fYokeSizeOuterX; }
  G4double GetYokeUpSizeY() { return 0.5*(fYokeSizeOuterY-fYokeSizeInnerY); }
  G4double GetYokeUpSizeZ() { return fYokeSizeZ; }
  G4double GetYokeUpPosX()  { return 0.*cm; }
  G4double GetYokeUpPosY()  { return 0.25*(fYokeSizeInnerY+fYokeSizeOuterY); }
  //G4double GetYokeUpPosZ()  { return fYokeFrontFacePosZ+0.5*fYokeSizeZ; }
  G4double GetYokeUpPosZ()  { return 0.*cm; }

  // Down (Y<0) section
  G4double GetYokeDownSizeX() { return fYokeSizeOuterX; }
  G4double GetYokeDownSizeY() { return 0.5*(fYokeSizeOuterY-fYokeSizeInnerY); }
  G4double GetYokeDownSizeZ() { return fYokeSizeZ; }
  G4double GetYokeDownPosX()  { return 0.*cm; }
  G4double GetYokeDownPosY()  { return -0.25*(fYokeSizeInnerY+fYokeSizeOuterY); }
  //G4double GetYokeDownPosZ()  { return fYokeFrontFacePosZ+0.5*fYokeSizeZ; }
  G4double GetYokeDownPosZ()  { return 0.*cm; }

  // Right (X<0) section
  G4double GetYokeRightSizeX() { return 0.5*(fYokeSizeOuterX-fYokeSizeInnerX); }
  G4double GetYokeRightSizeY() { return fYokeSizeInnerY; }
  G4double GetYokeRightSizeZ() { return fYokeSizeZ; }
  G4double GetYokeRightPosX()  { return -0.25*(fYokeSizeInnerX+fYokeSizeOuterX); }
  G4double GetYokeRightPosY()  { return 0.*cm; }
  //G4double GetYokeRightPosZ()  { return fYokeFrontFacePosZ+0.5*fYokeSizeZ; }
  G4double GetYokeRightPosZ()  { return 0.*cm; }

  // Left (X>0) section
  G4double GetYokeLeftSizeX() { return 0.5*(fYokeSizeOuterX-fYokeSizeInnerX); }
  G4double GetYokeLeftSizeY() { return fYokeSizeInnerY; }
  G4double GetYokeLeftSizeZ() { return fYokeSizeZ; }
  G4double GetYokeLeftPosX()  { return 0.25*(fYokeSizeInnerX+fYokeSizeOuterX); }
  G4double GetYokeLeftPosY()  { return 0.*cm; }
  //G4double GetYokeLeftPosZ()  { return fYokeFrontFacePosZ+0.5*fYokeSizeZ; }
  G4double GetYokeLeftPosZ()  { return 0.*cm; }

  // Up (Y>0) internal section
  G4double GetYokeUpBarSizeX() { return fYokeSizeInBarX; }
  G4double GetYokeUpBarSizeY() { return fYokeSizeInBarY; }
  G4double GetYokeUpBarSizeZ() { return fYokeSizeZ; }
  G4double GetYokeUpBarPosX()  { return 0.*cm; }
  G4double GetYokeUpBarPosY()  { return 0.5*fYokeSizeInnerY-0.5*fYokeSizeInBarY; }
  //G4double GetYokeUpBarPosZ()  { return fYokeFrontFacePosZ+0.5*fYokeSizeZ; }
  G4double GetYokeUpBarPosZ()  { return 0.*cm; }

  // Down (Y<0) internal section
  G4double GetYokeDownBarSizeX() { return fYokeSizeInBarX; }
  G4double GetYokeDownBarSizeY() { return fYokeSizeInBarY; }
  G4double GetYokeDownBarSizeZ() { return fYokeSizeZ; }
  G4double GetYokeDownBarPosX()  { return 0.*cm; }
  G4double GetYokeDownBarPosY()  { return -0.5*fYokeSizeInnerY+0.5*fYokeSizeInBarY; }
  //G4double GetYokeDownBarPosZ()  { return fYokeFrontFacePosZ+0.5*fYokeSizeZ; }
  G4double GetYokeDownBarPosZ()  { return 0.*cm; }

  // === Magnet coils ===

  G4double GetCoilInnerRadius() { return fCoilInnerRadius; }
  G4double GetCoilOuterRadius() { return fCoilOuterRadius; }
  G4double GetCoilSizeY() { return fCoilLengthY; }

  // Up (Y>0) Front (Z<0) straight section
  G4double GetCoilUFSizeX() { return fCoilLengthX; }
  G4double GetCoilUFSizeY() { return fCoilLengthY; }
  G4double GetCoilUFSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilUFPosX()  { return 0.*cm; }
  G4double GetCoilUFPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilUFPosZ()  { return fYokeFrontFacePosZ-0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  // Up (Y>0) Back (Z>0) straight section
  G4double GetCoilUBSizeX() { return fCoilLengthX; }
  G4double GetCoilUBSizeY() { return fCoilLengthY; }
  G4double GetCoilUBSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilUBPosX()  { return 0.*cm; }
  G4double GetCoilUBPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilUBPosZ()  { return fYokeFrontFacePosZ+fCoilLengthZ+0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  // Up (Y>0) Left (X>0) straight section
  G4double GetCoilULSizeX() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilULSizeY() { return fCoilLengthY; }
  G4double GetCoilULSizeZ() { return fCoilLengthZ; }
  G4double GetCoilULPosX()  { return 0.5*fCoilLengthX+0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilULPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilULPosZ()  { return fYokeFrontFacePosZ+0.5*fCoilLengthZ; }

  // Up (Y>0) Right (X<0) straight section
  G4double GetCoilURSizeX() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilURSizeY() { return fCoilLengthY; }
  G4double GetCoilURSizeZ() { return fCoilLengthZ; }
  G4double GetCoilURPosX()  { return -0.5*fCoilLengthX-0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilURPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilURPosZ()  { return fYokeFrontFacePosZ+0.5*fCoilLengthZ; }

  // Down (Y<0) Front (Z<0) straight section
  G4double GetCoilDFSizeX() { return fCoilLengthX; }
  G4double GetCoilDFSizeY() { return fCoilLengthY; }
  G4double GetCoilDFSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDFPosX()  { return 0.*cm; }
  G4double GetCoilDFPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDFPosZ()  { return fYokeFrontFacePosZ-0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  // Down (Y<0) Back (Z>0) straight section
  G4double GetCoilDBSizeX() { return fCoilLengthX; }
  G4double GetCoilDBSizeY() { return fCoilLengthY; }
  G4double GetCoilDBSizeZ() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDBPosX()  { return 0.*cm; }
  G4double GetCoilDBPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDBPosZ()  { return fYokeFrontFacePosZ+fCoilLengthZ+0.5*(fCoilOuterRadius+fCoilInnerRadius); }

  // Down (Y<0) Left (X>0) straight section
  G4double GetCoilDLSizeX() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDLSizeY() { return fCoilLengthY; }
  G4double GetCoilDLSizeZ() { return fCoilLengthZ; }
  G4double GetCoilDLPosX()  { return 0.5*fCoilLengthX+0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilDLPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDLPosZ()  { return fYokeFrontFacePosZ+0.5*fCoilLengthZ; }

  // Down (Y<0) Right (X<0) straight section
  G4double GetCoilDRSizeX() { return fCoilOuterRadius-fCoilInnerRadius; }
  G4double GetCoilDRSizeY() { return fCoilLengthY; }
  G4double GetCoilDRSizeZ() { return fCoilLengthZ; }
  G4double GetCoilDRPosX()  { return -0.5*fCoilLengthX-0.5*(fCoilOuterRadius+fCoilInnerRadius); }
  G4double GetCoilDRPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDRPosZ()  { return fYokeFrontFacePosZ+0.5*fCoilLengthZ; }

  // Up (Y>0) Front (Z<0) Left (X>0) curved section
  G4double GetCoilUFLAngle() { return 0.*deg; }
  G4double GetCoilUFLPosX()  { return 0.5*fCoilLengthX; }
  G4double GetCoilUFLPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilUFLPosZ()  { return fYokeFrontFacePosZ; }

  // Up (Y>0) Front (Z<0) Right (X<0) curved section
  G4double GetCoilUFRAngle() { return 90.*deg; }
  G4double GetCoilUFRPosX()  { return -0.5*fCoilLengthX; }
  G4double GetCoilUFRPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilUFRPosZ()  { return fYokeFrontFacePosZ; }

  // Up (Y>0) Back (Z>0) Left (X>0) curved section
  G4double GetCoilUBLAngle() { return 270.*deg; }
  G4double GetCoilUBLPosX()  { return 0.5*fCoilLengthX; }
  G4double GetCoilUBLPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilUBLPosZ()  { return fYokeFrontFacePosZ+fCoilLengthZ; }

  // Up (Y>0) Back (Z>0) Right (X<0) curved section
  G4double GetCoilUBRAngle() { return 180.*deg; }
  G4double GetCoilUBRPosX()  { return -0.5*fCoilLengthX; }
  G4double GetCoilUBRPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-0.5*fCoilLengthY; }
  G4double GetCoilUBRPosZ()  { return fYokeFrontFacePosZ+fCoilLengthZ; }

  // Down (Y<0) Front (Z<0) Left (X>0) curved section
  G4double GetCoilDFLAngle() { return 0.*deg; }
  G4double GetCoilDFLPosX()  { return 0.5*fCoilLengthX; }
  G4double GetCoilDFLPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDFLPosZ()  { return fYokeFrontFacePosZ; }

  // Down (Y<0) Front (Z<0) Right (X<0) curved section
  G4double GetCoilDFRAngle() { return 90.*deg; }
  G4double GetCoilDFRPosX()  { return -0.5*fCoilLengthX; }
  G4double GetCoilDFRPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDFRPosZ()  { return fYokeFrontFacePosZ; }

  // Down (Y<0) Back (Z>0) Left (X>0) curved section
  G4double GetCoilDBLAngle() { return 270.*deg; }
  G4double GetCoilDBLPosX()  { return 0.5*fCoilLengthX; }
  G4double GetCoilDBLPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDBLPosZ()  { return fYokeFrontFacePosZ+fCoilLengthZ; }

  // Down (Y<0) Back (Z>0) Right (X<0) curved section
  G4double GetCoilDBRAngle() { return 180.*deg; }
  G4double GetCoilDBRPosX()  { return -0.5*fCoilLengthX; }
  G4double GetCoilDBRPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+0.5*fCoilLengthY; }
  G4double GetCoilDBRPosZ()  { return fYokeFrontFacePosZ+fCoilLengthZ; }

  // === Slabs in front of coils ===

  // Up (Y>0) Left (X>0) slab
  G4double GetSlabULSizeX() { return fSlabWidth; }
  G4double GetSlabULSizeY() { return fSlabThick; }
  G4double GetSlabULSizeZ() { return fSlabLengthZ; }
  G4double GetSlabULPosX()  { return 0.5*fYokeSizeInnerX-0.5*fSlabWidth; }
  G4double GetSlabULPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-fCoilLengthY-fNeopreneThick-0.5*fSlabThick; }
  G4double GetSlabULPosZ()  { return fYokeFrontFacePosZ+0.5*fSlabLengthZ; }

  // Up (Y>0) Right (X<0) slab
  G4double GetSlabURSizeX() { return fSlabWidth; }
  G4double GetSlabURSizeY() { return fSlabThick; }
  G4double GetSlabURSizeZ() { return fSlabLengthZ; }
  G4double GetSlabURPosX()  { return -0.5*fYokeSizeInnerX+0.5*fSlabWidth; }
  G4double GetSlabURPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-fCoilLengthY-fNeopreneThick-0.5*fSlabThick; }
  G4double GetSlabURPosZ()  { return fYokeFrontFacePosZ+0.5*fSlabLengthZ; }

  // Down (Y<0) Left (X>0) slab
  G4double GetSlabDLSizeX() { return fSlabWidth; }
  G4double GetSlabDLSizeY() { return fSlabThick; }
  G4double GetSlabDLSizeZ() { return fSlabLengthZ; }
  G4double GetSlabDLPosX()  { return 0.5*fYokeSizeInnerX-0.5*fSlabWidth; }
  G4double GetSlabDLPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+fCoilLengthY+fNeopreneThick+0.5*fSlabThick; }
  G4double GetSlabDLPosZ()  { return fYokeFrontFacePosZ+0.5*fSlabLengthZ; }

  // Down (Y<0) Right (X<0) slab
  G4double GetSlabDRSizeX() { return fSlabWidth; }
  G4double GetSlabDRSizeY() { return fSlabThick; }
  G4double GetSlabDRSizeZ() { return fSlabLengthZ; }
  G4double GetSlabDRPosX()  { return -0.5*fYokeSizeInnerX+0.5*fSlabWidth; }
  G4double GetSlabDRPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+fCoilLengthY+fNeopreneThick+0.5*fSlabThick; }
  G4double GetSlabDRPosZ()  { return fYokeFrontFacePosZ+0.5*fSlabLengthZ; }

  /*
  // === Vacuum chamber plates ===

  // Up (Y>0)
  G4double GetVacuumUSizeX() { return fVacuumWidth; }
  G4double GetVacuumUSizeY() { return fVacuumThick; }
  G4double GetVacuumUSizeZ() { return fVacuumLengthZ; }
  G4double GetVacuumUPosX()  { return 0.*cm; }
  G4double GetVacuumUPosY()  { return 0.5*fYokeSizeInnerY-fYokeSizeInBarY-0.5*fVacuumThick; }
  G4double GetVacuumUPosZ()  { return fYokeFrontFacePosZ+0.5*fSlabLengthZ; }

  // Down (Y<0)
  G4double GetVacuumDSizeX() { return fVacuumWidth; }
  G4double GetVacuumDSizeY() { return fVacuumThick; }
  G4double GetVacuumDSizeZ() { return fVacuumLengthZ; }
  G4double GetVacuumDPosX()  { return 0.*cm; }
  G4double GetVacuumDPosY()  { return -0.5*fYokeSizeInnerY+fYokeSizeInBarY+0.5*fVacuumThick; }
  G4double GetVacuumDPosZ()  { return fYokeFrontFacePosZ+0.5*fSlabLengthZ; }
  */

  /*
  // === Rails inside magnet ===

  // Up (Y>0) Left (X>0) rail
  G4double GetRailULSizeX() { return fRailWidth; }
  G4double GetRailULSizeY() { return fRailThick; }
  G4double GetRailULSizeZ() { return fRailLengthZ; }
  G4double GetRailULPosX()  { return 0.5*fYokeSizeInBarX-0.5*fRailWidth; }
  G4double GetRailULPosY()  { return 0.5*fYokeSizeInnerY-fYokeSizeInBarY-0.5*fRailThick; }
  G4double GetRailULPosZ()  { return fYokeFrontFacePosZ+0.5*fRailLengthZ; }

  // Up (Y>0) Right (X<0) rail
  G4double GetRailURSizeX() { return fRailWidth; }
  G4double GetRailURSizeY() { return fRailThick; }
  G4double GetRailURSizeZ() { return fRailLengthZ; }
  G4double GetRailURPosX()  { return -0.5*fYokeSizeInBarX+0.5*fRailWidth; }
  G4double GetRailURPosY()  { return 0.5*fYokeSizeInnerY-fYokeSizeInBarY-0.5*fRailThick; }
  G4double GetRailURPosZ()  { return fYokeFrontFacePosZ+0.5*fRailLengthZ; }

  // Down (Y<0) Left (X>0) rail
  G4double GetRailDLSizeX() { return fRailWidth; }
  G4double GetRailDLSizeY() { return fRailThick; }
  G4double GetRailDLSizeZ() { return fRailLengthZ; }
  G4double GetRailDLPosX()  { return 0.5*fYokeSizeInBarX-0.5*fRailWidth; }
  G4double GetRailDLPosY()  { return -0.5*fYokeSizeInnerY+fYokeSizeInBarY+0.5*fRailThick; }
  G4double GetRailDLPosZ()  { return fYokeFrontFacePosZ+0.5*fRailLengthZ; }

  // Down (Y<0) Right (X<0) rail
  G4double GetRailDRSizeX() { return fRailWidth; }
  G4double GetRailDRSizeY() { return fRailThick; }
  G4double GetRailDRSizeZ() { return fRailLengthZ; }
  G4double GetRailDRPosX()  { return -0.5*fYokeSizeInBarX+0.5*fRailWidth; }
  G4double GetRailDRPosY()  { return -0.5*fYokeSizeInnerY+fYokeSizeInBarY+0.5*fRailThick; }
  G4double GetRailDRPosZ()  { return fYokeFrontFacePosZ+0.5*fRailLengthZ; }
  */
  // === Neoprene sheets over and under internal sections of coils ===

  // Up (Y>0) Left (X>0) Top (above coil) sheet
  G4double GetNeopreneULTSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneULTSizeY() { return fNeopreneThick; }
  G4double GetNeopreneULTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneULTPosX()  { return 0.5*fCoilLengthX+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneULTPosY()  { return 0.5*fYokeSizeInnerY-0.5*fNeopreneThick; }
  G4double GetNeopreneULTPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // Up (Y>0) Left (X>0) Bottom (below coil) sheet
  G4double GetNeopreneULBSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneULBSizeY() { return fNeopreneThick; }
  G4double GetNeopreneULBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneULBPosX()  { return 0.5*fCoilLengthX+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneULBPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-fCoilLengthY-0.5*fNeopreneThick; }
  G4double GetNeopreneULBPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // Up (Y>0) Right (X<0) Top (above coil) sheet
  G4double GetNeopreneURTSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneURTSizeY() { return fNeopreneThick; }
  G4double GetNeopreneURTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneURTPosX()  { return -0.5*fCoilLengthX-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneURTPosY()  { return 0.5*fYokeSizeInnerY-0.5*fNeopreneThick; }
  G4double GetNeopreneURTPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // Up (Y>0) Right (X<0) Bottom (below coil) sheet
  G4double GetNeopreneURBSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneURBSizeY() { return fNeopreneThick; }
  G4double GetNeopreneURBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneURBPosX()  { return -0.5*fCoilLengthX-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneURBPosY()  { return 0.5*fYokeSizeInnerY-fNeopreneThick-fCoilLengthY-0.5*fNeopreneThick; }
  G4double GetNeopreneURBPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // Down (Y<0) Left (X>0) Top (above coil) sheet
  G4double GetNeopreneDLTSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneDLTSizeY() { return fNeopreneThick; }
  G4double GetNeopreneDLTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDLTPosX()  { return 0.5*fCoilLengthX+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneDLTPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+fCoilLengthY+0.5*fNeopreneThick; }
  G4double GetNeopreneDLTPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // Down (Y<0) Left (X>0) Bottom (below coil) sheet
  G4double GetNeopreneDLBSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneDLBSizeY() { return fNeopreneThick; }
  G4double GetNeopreneDLBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDLBPosX()  { return 0.5*fCoilLengthX+fCoilInnerRadius+0.5*fNeopreneWidth; }
  G4double GetNeopreneDLBPosY()  { return -0.5*fYokeSizeInnerY+0.5*fNeopreneThick; }
  G4double GetNeopreneDLBPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // Down (Y<0) Right (X<0) Top (above coil) sheet
  G4double GetNeopreneDRTSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneDRTSizeY() { return fNeopreneThick; }
  G4double GetNeopreneDRTSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDRTPosX()  { return -0.5*fCoilLengthX-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneDRTPosY()  { return -0.5*fYokeSizeInnerY+fNeopreneThick+fCoilLengthY+0.5*fNeopreneThick; }
  G4double GetNeopreneDRTPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // Down (Y<0) Right (X<0) Bottom (below coil) sheet
  G4double GetNeopreneDRBSizeX() { return fNeopreneWidth; }
  G4double GetNeopreneDRBSizeY() { return fNeopreneThick; }
  G4double GetNeopreneDRBSizeZ() { return fNeopreneLengthZ; }
  G4double GetNeopreneDRBPosX()  { return -0.5*fCoilLengthX-fCoilInnerRadius-0.5*fNeopreneWidth; }
  G4double GetNeopreneDRBPosY()  { return -0.5*fYokeSizeInnerY+0.5*fNeopreneThick; }
  G4double GetNeopreneDRBPosZ()  { return fYokeFrontFacePosZ+0.5*fNeopreneLengthZ; }

  // === Magnetic Volume to host magnetic field generated by magnet (extends outside yoke) ===

  //G4double GetMagneticVolumeSizeX() { return fMagneticVolumeLengthX; }
  //G4double GetMagneticVolumeSizeY() { return fMagneticVolumeLengthY; }
  //G4double GetMagneticVolumeSizeZ() { return fMagneticVolumeLengthZ; }
  //G4double GetMagneticVolumePosX()  { return 0.*cm; }
  //G4double GetMagneticVolumePosY()  { return 0.*cm; }
  ////G4double GetMagneticVolumePosZ()  { return fMagnetFrontFacePosZ+0.5*fMagnetSizeZ; }
  //G4double GetMagneticVolumePosZ()  { return 0.*cm; }

  //void SetMagnetFrontFacePosZ(G4double z) { fMagnetFrontFacePosZ = z; }

  //void SetVacuumChamberWallThickness(G4double t) { fVacuumThick = t; }

  //void     SetMagneticFieldConstantValue(G4double v) { fMagneticFieldConstantValue = v; }
  //G4double GetMagneticFieldConstantValue()           { return fMagneticFieldConstantValue; }

private:

  G4double fMagnetGap;

  G4double fYokeSizeZ;
  G4double fYokeSizeOuterX;
  G4double fYokeSizeOuterY;
  G4double fYokeSizeInnerX;
  G4double fYokeSizeInnerY;
  G4double fYokeSizeInBarX;
  G4double fYokeSizeInBarY;

  G4double fCoilInnerRadius;
  G4double fCoilOuterRadius;
  G4double fCoilLengthX;
  G4double fCoilLengthY;
  G4double fCoilLengthZ;

  G4double fSlabThick;
  G4double fSlabWidth;
  G4double fSlabLengthZ;

  //G4double fVacuumThick;
  //G4double fVacuumWidth;
  //G4double fVacuumLengthZ;

  //G4double fRailThick;
  //G4double fRailWidth;
  //G4double fRailLengthZ;

  G4double fNeopreneThick;
  G4double fNeopreneWidth;
  G4double fNeopreneLengthZ;

  //G4double fMagneticVolumeLengthX;
  //G4double fMagneticVolumeLengthY;
  //G4double fMagneticVolumeLengthZ;

  G4double fYokeFrontFacePosZ; // Front face of Magnet yoke

  //G4double fMagneticFieldConstantValue; // Value of magnetic field in the constant zone

  G4String fMagnetSensitiveDetectorName;

};

#endif
