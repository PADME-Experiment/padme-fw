// TMMGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.dimeco@lnf.infn.it) 2026-05-27
// --------------------------------------------------------------

#ifndef TMMGeometry_H
#define TMMGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class TMMGeometry
{

public:

  ~TMMGeometry();
  static TMMGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static TMMGeometry* fInstance;

protected:

  TMMGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  void SetReadoutType(G4String type) {fReadoutType = type;}
  G4String GetReadoutType() {return fReadoutType;}

   // Position of TMM Volume
  G4double GetTMMPosX() { return fTMMDisplacementX; }
  G4double GetTMMPosY() { return fTMMDisplacementY; }
  G4double GetTMMPosZ() { return fTMMFrontFacePosZ+fTMMSizeZ*0.5; }

  // Size of TMM
  G4double GetTMMSizeX() { return fTMMSizeX; }
  G4double GetTMMSizeY() { return fTMMSizeY; }
  G4double GetTMMSizeZ() { return fTMMSizeZ; }

  G4double GetTMMCopperSizeZ(){ return fTMMCopperSizeZ; }
  G4double GetTMMFR4SizeZ() { return fTMMFR4SizeZ;  }
  G4double GetTMMFaradayFR4SizeZ() { return fTMMFaradayFR4SizeZ;  }
  G4double GetTMMFaradayCopperSizeZ() { return fTMMFaradayCopperSizeZ;  }
  G4double GetTMMNomexSizeZ() { return fTMMNomexSizeZ;  }
  G4double GetTMMKaptonSizeZ(){ return fTMMKaptonSizeZ; }
  G4double GetTMMFR4ReadoutSizeZ(){ return fTMMFR4ReadoutSizeZ; }
  G4double GetTMMLateralFR4(){ return fTMMLateralFR4; }
  G4double GetTMMLateralBrass(){ return fTMMLateralBrass; }
  
  
  G4double GetTMMCarbonSizeZ(){ return fTMMCarbonSizeZ; }
  G4double GetTMMLayerGap()   { return fTMMLayerGap;    }
  
  G4double GetTMMPanelSizeX() { return fTMMPanelSizeX; }
  G4double GetTMMPanelSizeY() { return fTMMPanelSizeY; }
  G4double GetTMMPanelSizeZ() { return fTMMPanelSizeZ; }

  G4double GetTMMFaradayPanelSizeX() { return fTMMFaradayPanelSizeX; }
  G4double GetTMMFaradayPanelSizeY() { return fTMMFaradayPanelSizeY; }
  G4double GetTMMFaradayPanelSizeZ() { return fTMMFaradayPanelSizeZ; }

  G4double GetTMMDriftSizeX() { return fTMMDriftSizeX; }
  G4double GetTMMDriftSizeY() { return fTMMDriftSizeY; }
  G4double GetTMMDriftSizeZ() { return fTMMDriftSizeZ; }
  
  G4double GetTMMAmpGapSizeX() { return fTMMAmpGapSizeX; }
  G4double GetTMMAmpGapSizeY() { return fTMMAmpGapSizeY; }
  G4double GetTMMAmpGapSizeZ() { return fTMMAmpGapSizeZ; }
  G4double GetTMMMylarClosingZ() { return fTMMMylarClosingZ; }
  G4double GetTMMExtFrameZ() { return fTMMExtFrameZ; }

  G4double GetTMMStripPitch() { return fTMMStripPitch;}
  G4double GetTMMStripWidth() { return fTMMStripWidth;}
  
  // G4double GetTMMPanelSizeV1()  { return fTMMPanelSizeV1;}
  // G4double GetTMMPanelSizeV2()  { return fTMMPanelSizeV2;}
  // G4int    GetVXStripHole()     { return fVXStripHole;}
  // G4int    GetVYStripHole()     { return fVYStripHole;}
  G4double GetVXStripStartPos()  { return fVXStripStartPos;}
  G4double GetVYStripStartPos()  { return fVYStripStartPos;} 
  //G4bool   GetisChVXActive(G4int iCh)  { 
  //   if(iCh<0 || iCh>512+fVXStripHole-1) return false;
  //   else return isChV1Active[iCh];
  //   } 
  // G4bool   GetisChVYActive(G4int iCh)  { 
  //   if(iCh<0 || iCh>512+fV2StripHole-1) return false;
  //   else return isChV2Active[iCh];} 

  G4double GetXPadArea()    { return fTMMXPadArea;   }
  G4double GetYPadArea()    { return fTMMYPadArea;   }
  G4double GetPadDistance() { return fTMMPadDistance;}
  G4double GetXPadStartPos(){ return fXPadStartPos;}
  G4double GetYPadStartPos(){ return fYPadStartPos;}

  G4double GetTMMAmpMeshSizeZ()   { return fTMMAmpMeshSizeZ;}
  G4double GetTMMCathodeMeshSizeZ()   { return fTMMCathodeMeshSizeZ;}

  // Set position along Z of TMM front face
  void SetTMMFrontFacePosZ(G4double z) { fTMMFrontFacePosZ = z; }
  G4double GetTMMFrontFacePosZ() { return fTMMFrontFacePosZ; }
  G4double GetTMMDisplacementX() { return fTMMDisplacementX; }
  G4double GetTMMDisplacementY() { return fTMMDisplacementY; }
  G4double GetTMMRotationAngle() { return fTMMRotationAngle; }
  
  // Get name of TMM digitizer
  G4String GetTMMDigitizerName() { return fTMMDigitizerName; }

  // Get name of TMM sensitive detector
  G4String GetTMMSensitiveDetectorName() { return fTMMSensitiveDetectorName; }

private:

  G4int fVerbose; // Verbose level

  G4String fReadoutType;
  G4int fDetectorSetup;

  //TMM dimensions

  G4double fTMMSizeX;
  G4double fTMMSizeY;
  G4double fTMMSizeZ;

  G4double fTMMDriftSizeX;
  G4double fTMMDriftSizeY;
  G4double fTMMDriftSizeZ;

  G4double fTMMAmpGapSizeX;
  G4double fTMMAmpGapSizeY;
  G4double fTMMAmpGapSizeZ;

  //readout plane composition
  G4double fTMMPanelSizeX;
  G4double fTMMPanelSizeY;
  G4double fTMMPanelSizeZ;
  
  G4double fTMMFaradayPanelSizeX;
  G4double fTMMFaradayPanelSizeY;
  G4double fTMMFaradayPanelSizeZ;
  
  G4double fTMMCopperSizeZ;
  G4double fTMMFR4SizeZ;
  G4double fTMMFaradayFR4SizeZ;
  G4double fTMMFaradayCopperSizeZ;
  G4double fTMMNomexSizeZ;
  G4double fTMMKaptonSizeZ;
  G4double fTMMCarbonSizeZ; //thickness of carbon resistive layer
  G4double fTMMLayerGap;
  G4double fTMMFR4ReadoutSizeZ;
  G4double fTMMLateralFR4;
  G4double fTMMLateralBrass;
  G4double fTMMMylarClosingZ;
  G4double fTMMExtFrameZ;

  //geometry of strips readout design (same for x and y)
  G4double fTMMStripPitch;
  G4double fTMMStripWidth;
  // G4double fTMMPanelSizeV1;
  // G4double fTMMPanelSizeV2;
  G4double fVXStripStartPos;
  G4double fVYStripStartPos;
  // static const G4int fVXStripHole=1;
  // static const G4int fVYStripHole=6;
  // G4bool isChVXActive[512+fVXStripHole];
  
  // G4bool isChVYActive[512+fVYStripHole];
  
  //geometry of pads readout design
  G4double fTMMXPadArea;
  G4double fTMMYPadArea;
  G4double fTMMPadDistance;
  G4double fXPadStartPos;
  G4double fYPadStartPos;

  //geometry of meshes
  G4double fTMMAmpMeshSizeZ;
  G4double fTMMCathodeMeshSizeZ;

  G4double fTMMFrontFacePosZ; // Position along Z axis of TMM front face
  G4double fTMMRotationAngle; // Rotation angle of the whole TMM system (surveys)
  G4double fTMMDisplacementX; // Displacement wrt 0 in X (surveys)
  G4double fTMMDisplacementY; // Displacement wrt 0 in Y (surveys)
  G4String fTMMSensitiveDetectorName;
  G4String fTMMDigitizerName;
 

};

#endif
