
// BeamLineGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by M. Raggi (mauro.raggi@roma1.infn.it) 2019-03-07
// --------------------------------------------------------------

#ifndef BeamLineGeometry_H
#define BeamLineGeometry_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#define CHAMBERGEOMETRY_N_PORTHOLES 8

class BeamLineGeometry
{

public:

  ~BeamLineGeometry();
  static BeamLineGeometry* GetInstance();

private:

  static BeamLineGeometry* fInstance;

protected:

  BeamLineGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  void EnableBeWindow() { fBeWindowEnable = true; }
  void DisableBeWindow() { fBeWindowEnable = false; }
  G4bool BeWindowIsEnabled() { return fBeWindowEnable; }

  void EnableMylarWindow() { fMylarWindowEnable = true; }
  void DisableMylarWindow() { fMylarWindowEnable = false; }
  G4bool MylarWindowIsEnabled() { return fMylarWindowEnable; }

  void EnableBeamFlag()  { fBeamFlagEnable = true; }
  void DisableBeamFlag() { fBeamFlagEnable = false; }
  G4bool BeamFlagIsEnabled() { return fBeamFlagEnable; } // M. Raggi 28/08/2019

  G4String fBeWSensitiveDetectorName; 
  G4String fMylarWSensitiveDetectorName; 

  G4String fBeamFlag1SensitiveDetectorName; 
  G4String fBeamFlag2SensitiveDetectorName;   
  G4String fBeamFlag3SensitiveDetectorName; 

  // Magnetic field inside DHSTB002
  void SetDHSTB002MagneticFieldY(G4double f) { fDHSTB002MagneticFieldY = f; }
  G4double GetDHSTB002MagneticFieldY() { return fDHSTB002MagneticFieldY; }

  // Magnetic field inside DHSTB002
  void SetDHSTB001MagneticFieldY(G4double f) { fDHSTB001MagneticFieldY = f; }
  G4double GetDHSTB001MagneticFieldY() { return fDHSTB001MagneticFieldY; }

  // Mylar or BeWthickness
  void SetWindowThickness(G4double f) { fWindowThickness = f; }
  G4double GetWindowThickness() { return fWindowThickness;}

  // Enable/disable quadrupoles
  void EnableQuadrupoles() { fQuadrupolesEnable = true; }
  void DisableQuadrupoles() { fQuadrupolesEnable = false; }
  G4bool QuadrupolesAreEnabled() { return fQuadrupolesEnable; }

  // Magnetic field gradient inside quadrupoles
  void SetQ1MagneticFieldGrad(G4double g) { fQ1MagneticFieldGrad = g*tesla/m; }
  G4double GetQ1MagneticFieldGrad() { return fQ1MagneticFieldGrad; }

  void SetQ2MagneticFieldGrad(G4double g) { fQ2MagneticFieldGrad = g*tesla/m; }
  G4double GetQ2MagneticFieldGrad() { return fQ2MagneticFieldGrad; }

  void SetQ3MagneticFieldGrad(G4double g) { fQ3MagneticFieldGrad = g*tesla/m; }
  G4double GetQ3MagneticFieldGrad() { return fQ3MagneticFieldGrad; }

  void SetQ4MagneticFieldGrad(G4double g) { fQ4MagneticFieldGrad = g*tesla/m; }
  G4double GetQ4MagneticFieldGrad() { return fQ4MagneticFieldGrad; }


  //Aperture of the collimators
  void SetSLTB2Aperture(G4double g) { fSLTB2Aperture = g; }
  G4double GetSLTB2Aperture() { return fSLTB2Aperture; }

  //Aperture of the collimators
  void SetSLTB3Aperture(G4double g) { fSLTB3Aperture = g; }
  G4double GetSLTB3Aperture() { return fSLTB3Aperture; }

  void SetSLTB4Aperture(G4double g) { fSLTB4Aperture = g; }
  G4double GetSLTB4Aperture() { return fSLTB4Aperture; }

  //setup line configuration M. Raggi 06/2020
  void SetBeamLineSetup(G4double g) { fBeamLineSetup = g; }
  G4double GetBeamLineSetup() { return fBeamLineSetup; }

  // Dimensions of DHSTB002 magnet yoke

  G4double GetDHSTB002CenterRadius() { return fDHSTB002CenterRadius; }
  G4double GetDHSTB002AngularSpan() { return fDHSTB002AngularSpan; }
  G4double GetDHSTB002ExitPosZ() { return fDHSTB002ExitPosZ; }

  G4double GetDHSTB002SizeX() { return fDHSTB002SizeX; }
  G4double GetDHSTB002SizeY() { return fDHSTB002SizeY; }
  G4double GetDHSTB002MinRadius() { return fDHSTB002MinRadius; }
  G4double GetDHSTB002MaxRadius() { return fDHSTB002MinRadius+fDHSTB002SizeX; }

  // Dimensions of the H-shaped hole inside magnet yoke

  G4double GetDHSTB002L1() { return fDHSTB002L1; }
  G4double GetDHSTB002L2() { return fDHSTB002L2; }
  G4double GetDHSTB002L3() { return fDHSTB002L3; }
  G4double GetDHSTB002L4() { return fDHSTB002L4; }

  // Dimensions of the magnetic field volume inside the magnet
  G4double GetMagVolSizeX() { return fMagVolSizeX; }
  G4double GetMagVolSizeY() { return fMagVolSizeY; }
  G4double GetMagVolMinRadius() { return fMagVolMinRadius; }
  G4double GetMagVolMaxRadius() { return fMagVolMaxRadius; }

  // Dimensions of beam pipe inside magnet

  G4double GetMagPipeSizeX() { return fMagPipeSizeX; }
  G4double GetMagPipeSizeY() { return fMagPipeSizeY; }
  G4double GetMagPipeMinRadius() { return fMagPipeMinRadius; }
  G4double GetMagPipeMaxRadius() { return fMagPipeMaxRadius; }

  G4double GetMagPipeHoleSizeX() { return fMagPipeHoleSizeX; }
  G4double GetMagPipeHoleSizeY() { return fMagPipeHoleSizeY; }
  G4double GetMagPipeHoleMinRadius() { return fMagPipeHoleMinRadius; }
  G4double GetMagPipeHoleMaxRadius() { return fMagPipeHoleMaxRadius; }

  G4double GetMagPipeStraightLength() { return fMagPipeStraightLength; }
  G4double GetMagPipeFlangeThick() { return fMagPipeFlangeThick; }
  G4double GetMagPipeFlangeRadius() { return fMagPipeFlangeRadius; }
  G4double GetMagPipeFlangePosZ() { return fMagPipeFlangePosZ; }

  G4double GetMagPipeStraightFrontPosX() { return fMagPipeStraightFrontPosX; }
  G4double GetMagPipeStraightFrontPosZ() { return fMagPipeStraightFrontPosZ; }
  G4double GetMagPipeStraightFrontRotY() { return fMagPipeStraightFrontRotY; }

  G4double GetMagPipeStraightBackPosX() { return fMagPipeStraightBackPosX; }
  G4double GetMagPipeStraightBackPosZ() { return fMagPipeStraightBackPosZ; }
  G4double GetMagPipeStraightBackRotY() { return fMagPipeStraightBackRotY; }

  G4double GetMagPipeStraightSidePosX() { return fMagPipeStraightSidePosX; }
  G4double GetMagPipeStraightSidePosZ() { return fMagPipeStraightSidePosZ; }
  G4double GetMagPipeStraightSideRotY() { return fMagPipeStraightSideRotY; }

  // Length of the straight section inside the magnet
  G4double GetMagPipeLineLength() { return fMagPipeLineLength; }

  // Entrance and exit coordinates of magnet pipe

  G4double GetMagPipeEnterPosX() { return fMagPipeEnterPosX; }
  G4double GetMagPipeEnterPosY() { return fMagPipeEnterPosY; }
  G4double GetMagPipeEnterPosZ() { return fMagPipeEnterPosZ; }

  G4double GetMagPipeExitPosX() { return fMagPipeExitPosX; }
  G4double GetMagPipeExitPosY() { return fMagPipeExitPosY; }
  G4double GetMagPipeExitPosZ() { return fMagPipeExitPosZ; }

  // Properties of junction between magnet pipe and long pipe to Be window

  G4double GetBeJunctionLength() { return fBeJunctionLength; }
  G4double GetBeJunctionRIn() { return fBeJunctionRIn; }
  G4double GetBeJunctionROut() { return fBeJunctionROut; }

  G4double GetBeJunctionFlangeFrontRadius() { return fBeJunctionFlangeFrontRadius; }
  G4double GetBeJunctionFlangeFrontThick() { return fBeJunctionFlangeFrontThick; }

  G4double GetBeJunctionFlangeBackRadius() { return fBeJunctionFlangeBackRadius; }
  G4double GetBeJunctionFlangeBackThick() { return fBeJunctionFlangeBackThick; }

  // Properties of long pipe to Be window

  G4double GetBePipeLength() { return fBePipeLength; }
  G4double GetBePipeRIn() { return fBePipeRIn; }
  G4double GetBePipeROut() { return fBePipeROut; }

  G4double GetBePipeFlangeFrontRadius() { return fBePipeFlangeFrontRadius; }
  G4double GetBePipeFlangeFrontThick() { return fBePipeFlangeFrontThick; }

  G4double GetBePipeFlangeBackRadius() { return fBePipeFlangeBackRadius; }
  G4double GetBePipeFlangeBackThick() { return fBePipeFlangeBackThick; }

  // Properties of Be thin window and its support flange

  G4double GetBeWindowRadius() { return fBeWindowRadius; }
  G4double GetBeWindowThick() { return fBeWindowThick; }
  G4double GetBeWindowFlangeRadius() { return fBeWindowFlangeRadius; }
  G4double GetBeWindowFlangeThick() { return fBeWindowFlangeThick; }

  // Properties of Mylar thin window and its support flange
  G4double GetMylarWindowRadius() { return fMylarWindowRadius; }
  G4double GetMylarWindowThick() { return fMylarWindowThick; }
  G4double GetMylarWindowFlangeRadius() { return fMylarWindowFlangeRadius; }
  G4double GetMylarWindowFlangeThick() { return fMylarWindowFlangeThick; }

  // Properties of the quadrupoles
  G4double  GetQuadBoxSizeX(){return fQuadBoxSizeX;}
  G4double  GetQuadBoxSizeY(){return fQuadBoxSizeY;}
  G4double  GetQuadBoxSizeZ(){return fQuadBoxSizeZ;}
     
  G4double GetQuadMagSizeX(){return fQuadMagSizeX;}
  G4double GetQuadMagSizeY(){return fQuadMagSizeY;}
  G4double GetQuadMagSizeZ(){return fQuadMagSizeZ;}

  G4double GetQ4DistFromDHSTB002(){return fQ4DistFromDHSTB002;} 
  G4double GetQ1Q2Dist(){return fQ1Q2Dist;} 
  G4double GetQ3Q4Dist(){return fQ3Q4Dist;} 

  // Get name of BeW sensitive detector
  G4String GetBeWSensitiveDetectorName() { return fBeWSensitiveDetectorName; }
  G4String GetMylarWSensitiveDetectorName() { return fMylarWSensitiveDetectorName; }
  G4String GetBeamFlag1SensitiveDetectorName() { return fBeamFlag1SensitiveDetectorName; }
  G4String GetBeamFlag2SensitiveDetectorName() { return fBeamFlag2SensitiveDetectorName; }
  G4String GetBeamFlag3SensitiveDetectorName() { return fBeamFlag3SensitiveDetectorName; }

  // BEAM line 2020 geometry parameters M. Raggi 02.2021 
  G4double GetDHSTB002WallDistance() { return fDHSTB002WallDistance;}
  G4double Get2020PipeOuterRadius()  { return f2020PipeOuterRadius; }
  G4double Get2020PipeInnerRadius()  { return f2020PipeInnerRadius; }

  G4double GetWallThickness() { return fWallThickness; }
  G4double GetWallPipeLen()   { return fWallPipeLen; }
  G4double GetWallHoleRadius(){ return fWallHoleRadius;}
  G4double GetWallMylarWinDistance(){ return fWallMylarWinDistance;}


private:

  G4int fDetectorSetup;

  //  Berillium thin window and stainless still flange

  G4bool fBeWindowEnable;
  G4bool fMylarWindowEnable;
  G4bool fBeamFlagEnable;

  // Magnetic field
  G4double fDHSTB002MagneticFieldY;
  G4double fDHSTB001MagneticFieldY;
  G4double fWindowThickness;

  // Gradient of Quadrupoles Magnetic field 
  //G4double fQ1_FieldGrad;  //from data cards

  G4bool fQuadrupolesEnable;
  G4double fQ1MagneticFieldGrad;
  G4double fQ2MagneticFieldGrad;
  G4double fQ3MagneticFieldGrad;
  G4double fQ4MagneticFieldGrad;


  G4double fSLTB4Aperture;
  G4double fSLTB3Aperture;
  G4double fSLTB2Aperture;

  G4double fBeamLineSetup;

  // Radius of magnet at center of beam line
  G4double fDHSTB002CenterRadius;

  // Z coordinate of magnet exit hole
  G4double fDHSTB002ExitPosZ;

  // Angular span of the magnet
  G4double fDHSTB002AngularSpan;

  // Transversal size of the yoke
  G4double fDHSTB002SizeX;
  G4double fDHSTB002SizeY;
  G4double fDHSTB002MinRadius; // Magnet yoke is not centered on beam pipe

  // Yoke has a H-shaped hole inside centered on beam radius
  // Y
  // ^ |-----------------------|
  // | +------+   L1    +------+
  // | |      |         |      |
  // | |      |         |      |
  // | |      |         |      |
  // | |   -  +---------+      |
  // | |   |                   |
  // | |   |L4     o         L3|
  // | |   |                   |
  // | |   -  +---------+      |
  // | |      |   L2    |      |
  // | |      |         |      |
  // | |      |         |      |
  // | +------+         +------+
  // +--------------------------->R                   
  G4double fDHSTB002L1;
  G4double fDHSTB002L2;
  G4double fDHSTB002L3;
  G4double fDHSTB002L4;

  // Dimensions of the magnetic field volume inside the magnet
  G4double fMagVolSizeX;
  G4double fMagVolSizeY;
  G4double fMagVolMinRadius;
  G4double fMagVolMaxRadius;

  // Dimensions of the beam pipe inside the magnet

  // Transverse section
  G4double fMagPipeSizeX;
  G4double fMagPipeSizeY;
  G4double fMagPipeMinRadius;
  G4double fMagPipeMaxRadius;
  G4double fMagPipeHoleSizeX;
  G4double fMagPipeHoleSizeY;
  G4double fMagPipeHoleMinRadius;
  G4double fMagPipeHoleMaxRadius;

  // Length of the straight section inside the magnet
  G4double fMagPipeLineLength;

  // Straight section with flange (outside magnet yoke)
  G4double fMagPipeStraightLength;
  G4double fMagPipeFlangeThick;
  G4double fMagPipeFlangeRadius;
  G4double fMagPipeFlangePosZ; // Local position

  G4double fMagPipeStraightFrontPosX;
  G4double fMagPipeStraightFrontPosZ;
  G4double fMagPipeStraightFrontRotY;

  G4double fMagPipeStraightBackPosX;
  G4double fMagPipeStraightBackPosZ;
  G4double fMagPipeStraightBackRotY;

  G4double fMagPipeStraightSidePosX;
  G4double fMagPipeStraightSidePosZ;
  G4double fMagPipeStraightSideRotY;

  // Entrance and exit coordinates of magnet pipe

  G4double fMagPipeEnterPosX;
  G4double fMagPipeEnterPosY;
  G4double fMagPipeEnterPosZ;

  G4double fMagPipeExitPosX;
  G4double fMagPipeExitPosY;
  G4double fMagPipeExitPosZ;

  // Properties of junction between magnet pipe and long pipe to Be window

  G4double fBeJunctionLength;
  G4double fBeJunctionRIn;
  G4double fBeJunctionROut;

  G4double fBeJunctionFlangeFrontRadius;
  G4double fBeJunctionFlangeFrontThick;

  G4double fBeJunctionFlangeBackRadius;
  G4double fBeJunctionFlangeBackThick;

  // Properties of long pipe to Be window

  G4double fBePipeLength;
  G4double fBePipeRIn;
  G4double fBePipeROut;

  G4double fBePipeFlangeFrontRadius;
  G4double fBePipeFlangeFrontThick;

  G4double fBePipeFlangeBackRadius;
  G4double fBePipeFlangeBackThick;

  // Properties of Be thin window and its support flange

  G4double fBeWindowRadius;
  G4double fBeWindowThick;
  G4double fBeWindowFlangeRadius;
  G4double fBeWindowFlangeThick;

  // Properties of Be thin window and its support flange

  G4double fMylarWindowRadius;
  G4double fMylarWindowThick;
  G4double fMylarWindowFlangeRadius;
  G4double fMylarWindowFlangeThick;

  // Properties of the quadrupoles
  G4double  fQuadBoxSizeX;  
  G4double  fQuadBoxSizeY;
  G4double  fQuadBoxSizeZ;
     
  G4double fQuadMagSizeX;
  G4double fQuadMagSizeY;
  G4double fQuadMagSizeZ;


  //
  // BEAM line 2020 geometry parameters M. Raggi 02.2021 
  //

  G4double fQ4DistFromDHSTB002;
  G4double fQ1Q2Dist;  //center to center in Linac
  G4double fQ3Q4Dist;  //center to center in BTF 

  G4double fWallThickness;
  G4double fWallPipeLen;
 
  G4double fWallHoleRadius;
  G4double fWallMylarWinDistance;

  G4double fDHSTB002WallDistance;
  G4double f2020PipeOuterRadius; 
  G4double f2020PipeInnerRadius; 


};

#endif
