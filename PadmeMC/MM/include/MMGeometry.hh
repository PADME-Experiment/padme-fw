// MMGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#ifndef MMGeometry_H
#define MMGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class MMGeometry
{

public:

  ~MMGeometry();
  static MMGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static MMGeometry* fInstance;

protected:

  MMGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  void SetReadoutType(G4String type) {fReadoutType = type;}
  G4String GetReadoutType() {return fReadoutType;}

   // Position of MM Volume
  G4double GetMMPosX() { return fMMDisplacementX; }
  G4double GetMMPosY() { return fMMDisplacementY; }
  G4double GetMMPosZ() { return fMMFrontFacePosZ+fMMSizeZ*0.5; }

  // Size of MM
  G4double GetMMSizeX() { return fMMSizeX; }
  G4double GetMMSizeY() { return fMMSizeY; }
  G4double GetMMSizeZ() { return fMMSizeZ; }

  G4double GetMMCopperSizeZ(){ return fMMCopperSizeZ; }
  G4double GetMMFR4SizeZ() { return fMMFR4SizeZ;  }
  G4double GetMMFaradayFR4SizeZ() { return fMMFaradayFR4SizeZ;  }
  G4double GetMMFaradayCopperSizeZ() { return fMMFaradayCopperSizeZ;  }
  G4double GetMMNomexSizeZ() { return fMMNomexSizeZ;  }
  G4double GetMMKaptonSizeZ(){ return fMMKaptonSizeZ; }
  G4double GetMMFR4ReadoutSizeZ(){ return fMMFR4ReadoutSizeZ; }
  G4double GetMMLateralFR4(){ return fMMLateralFR4; }
  G4double GetMMLateralBrass(){ return fMMLateralBrass; }
  
  
  G4double GetMMCarbonSizeZ(){ return fMMCarbonSizeZ; }
  G4double GetMMLayerGap()   { return fMMLayerGap;    }
  
  G4double GetMMPanelSizeX() { return fMMPanelSizeX; }
  G4double GetMMPanelSizeY() { return fMMPanelSizeY; }
  G4double GetMMPanelSizeZ() { return fMMPanelSizeZ; }

  G4double GetMMFaradayPanelSizeX() { return fMMFaradayPanelSizeX; }
  G4double GetMMFaradayPanelSizeY() { return fMMFaradayPanelSizeY; }
  G4double GetMMFaradayPanelSizeZ() { return fMMFaradayPanelSizeZ; }

  G4double GetMMDriftSizeX() { return fMMDriftSizeX; }
  G4double GetMMDriftSizeY() { return fMMDriftSizeY; }
  G4double GetMMDriftSizeZ() { return fMMDriftSizeZ; }
  
  G4double GetMMAmpGapSizeX() { return fMMAmpGapSizeX; }
  G4double GetMMAmpGapSizeY() { return fMMAmpGapSizeY; }
  G4double GetMMAmpGapSizeZ() { return fMMAmpGapSizeZ; }

  G4double GetMMStripPitch() { return fMMStripPitch;}
  G4double GetMMStripWidth() { return fMMStripWidth;}
  
  G4double GetMMPanelSizeV1()  { return fMMPanelSizeV1;}
  G4double GetMMPanelSizeV2()  { return fMMPanelSizeV2;}
  G4int    GetV1StripHole()     { return fV1StripHole;}
  G4int    GetV2StripHole()     { return fV2StripHole;}
  G4double GetV1StripStartPos()  { return fV1StripStartPos;}
  G4double GetV2StripStartPos()  { return fV2StripStartPos;} 
  G4bool   GetisChV1Active(G4int iCh)  { 
    if(iCh<0 || iCh>512+fV1StripHole-1) return false;
    else return isChV1Active[iCh];
    } 
  G4bool   GetisChV2Active(G4int iCh)  { 
    if(iCh<0 || iCh>512+fV2StripHole-1) return false;
    else return isChV2Active[iCh];} 

  G4double GetXPadArea()    { return fMMXPadArea;   }
  G4double GetYPadArea()    { return fMMYPadArea;   }
  G4double GetPadDistance() { return fMMPadDistance;}
  G4double GetXPadStartPos(){ return fXPadStartPos;}
  G4double GetYPadStartPos(){ return fYPadStartPos;}

  G4double GetMMAmpMeshSizeZ()   { return fMMAmpMeshSizeZ;}
  G4double GetMMCathodeMeshSizeZ()   { return fMMCathodeMeshSizeZ;}

  // Set position along Z of MM front face
  void SetMMFrontFacePosZ(G4double z) { fMMFrontFacePosZ = z; }
  G4double GetMMFrontFacePosZ() { return fMMFrontFacePosZ; }
  G4double GetMMDisplacementX() { return fMMDisplacementX; }
  G4double GetMMDisplacementY() { return fMMDisplacementY; }
  G4double GetMMRotationAngle() { return fMMRotationAngle; }
  
  // Get name of MM digitizer
  G4String GetMMDigitizerName() { return fMMDigitizerName; }

  // Get name of MM sensitive detector
  G4String GetMMSensitiveDetectorName() { return fMMSensitiveDetectorName; }

private:

  G4int fVerbose; // Verbose level

  G4String fReadoutType;
  G4int fDetectorSetup;

  //MM dimensions

  G4double fMMSizeX;
  G4double fMMSizeY;
  G4double fMMSizeZ;

  G4double fMMDriftSizeX;
  G4double fMMDriftSizeY;
  G4double fMMDriftSizeZ;

  G4double fMMAmpGapSizeX;
  G4double fMMAmpGapSizeY;
  G4double fMMAmpGapSizeZ;

  //readout plane composition
  G4double fMMPanelSizeX;
  G4double fMMPanelSizeY;
  G4double fMMPanelSizeZ;
  
  G4double fMMFaradayPanelSizeX;
  G4double fMMFaradayPanelSizeY;
  G4double fMMFaradayPanelSizeZ;
  
  G4double fMMCopperSizeZ;
  G4double fMMFR4SizeZ;
  G4double fMMFaradayFR4SizeZ;
  G4double fMMFaradayCopperSizeZ;
  G4double fMMNomexSizeZ;
  G4double fMMKaptonSizeZ;
  G4double fMMCarbonSizeZ; //thickness of carbon resistive layer
  G4double fMMLayerGap;
  G4double fMMFR4ReadoutSizeZ;
  G4double fMMLateralFR4;
  G4double fMMLateralBrass;

  //geometry of strips readout design (same for x and y)
  G4double fMMStripPitch;
  G4double fMMStripWidth;
  G4double fMMPanelSizeV1;
  G4double fMMPanelSizeV2;
  G4double fV1StripStartPos;
  G4double fV2StripStartPos;
  static const G4int fV1StripHole=1;
  static const G4int fV2StripHole=6;
  G4bool isChV1Active[512+fV1StripHole];
  
  G4bool isChV2Active[512+fV2StripHole];
  
  //geometry of pads readout design
  G4double fMMXPadArea;
  G4double fMMYPadArea;
  G4double fMMPadDistance;
  G4double fXPadStartPos;
  G4double fYPadStartPos;

  //geometry of meshes
  G4double fMMAmpMeshSizeZ;
  G4double fMMCathodeMeshSizeZ;

  G4double fMMFrontFacePosZ; // Position along Z axis of MM front face
  G4double fMMRotationAngle; // Rotation angle of the whole MM system (surveys)
  G4double fMMDisplacementX; // Displacement wrt 0 in X (surveys)
  G4double fMMDisplacementY; // Displacement wrt 0 in Y (surveys)
  G4String fMMSensitiveDetectorName;
  G4String fMMDigitizerName;
 

};

#endif
