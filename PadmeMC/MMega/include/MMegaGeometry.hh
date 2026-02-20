// MMegaGeometry.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#ifndef MMegaGeometry_H
#define MMegaGeometry_H 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4LogicalVolume;

class MMegaGeometry
{

public:

  ~MMegaGeometry();
  static MMegaGeometry* GetInstance();
  std::vector<G4String> GetHashTable();

private:

  static MMegaGeometry* fInstance;

protected:

  MMegaGeometry();

public:

  void SetDetectorSetup(G4int s) { fDetectorSetup = s; }
  G4int GetDetectorSetup() { return fDetectorSetup; }

  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  void SetReadoutType(G4String type) {fReadoutType = type;}
  G4String GetReadoutType() {return fReadoutType;}

   // Position of MMega Volume
  G4double GetMMegaPosX() { return fMMegaDisplacementX; }
  G4double GetMMegaPosY() { return fMMegaDisplacementY; }
  G4double GetMMegaPosZ() { return fMMegaFrontFacePosZ+fMMegaSizeZ*0.5; }

  // Size of MMega
  G4double GetMMegaSizeX() { return fMMegaSizeX; }
  G4double GetMMegaSizeY() { return fMMegaSizeY; }
  G4double GetMMegaSizeZ() { return fMMegaSizeZ; }

  G4double GetMMegaCopperSizeZ(){ return fMMegaCopperSizeZ; }
  G4double GetMMegaFR4SizeZ() { return fMMegaFR4SizeZ;  }
  G4double GetMMegaFaradayFR4SizeZ() { return fMMegaFaradayFR4SizeZ;  }
  G4double GetMMegaFaradayCopperSizeZ() { return fMMegaFaradayCopperSizeZ;  }
  G4double GetMMegaNomexSizeZ() { return fMMegaNomexSizeZ;  }
  G4double GetMMegaKaptonSizeZ(){ return fMMegaKaptonSizeZ; }
  G4double GetMMegaFR4ReadoutSizeZ(){ return fMMegaFR4ReadoutSizeZ; }
  G4double GetMMegaCarbonSizeZ(){ return fMMegaCarbonSizeZ; }
  G4double GetMMegaLayerGap()   { return fMMegaLayerGap;    }
  
  G4double GetMMegaPanelSizeX() { return fMMegaPanelSizeX; }
  G4double GetMMegaPanelSizeY() { return fMMegaPanelSizeY; }
  G4double GetMMegaPanelSizeZ() { return fMMegaPanelSizeZ; }

  G4double GetMMegaFaradayPanelSizeX() { return fMMegaFaradayPanelSizeX; }
  G4double GetMMegaFaradayPanelSizeY() { return fMMegaFaradayPanelSizeY; }
  G4double GetMMegaFaradayPanelSizeZ() { return fMMegaFaradayPanelSizeZ; }

  G4double GetMMegaDriftSizeX() { return fMMegaDriftSizeX; }
  G4double GetMMegaDriftSizeY() { return fMMegaDriftSizeY; }
  G4double GetMMegaDriftSizeZ() { return fMMegaDriftSizeZ; }
  
  G4double GetMMegaAmpGapSizeX() { return fMMegaAmpGapSizeX; }
  G4double GetMMegaAmpGapSizeY() { return fMMegaAmpGapSizeY; }
  G4double GetMMegaAmpGapSizeZ() { return fMMegaAmpGapSizeZ; }

  G4double GetMMegaStripPitch() { return fMMegaStripPitch;}
  G4double GetMMegaStripWidth() { return fMMegaStripWidth;}
  
  G4double GetMMegaPanelSizeV1()  { return fMMegaPanelSizeV1;}
  G4double GetMMegaPanelSizeV2()  { return fMMegaPanelSizeV2;}
  
  G4double GetV1StripStartPos()  { return fV1StripStartPos;}
  G4double GetV2StripStartPos()  { return fV2StripStartPos;} 
  G4bool   GetisChV1Active(G4int iCh)  { 
    if(iCh<0 && iCh>512) return false;
    else return isChV1Active[iCh];} 
  G4bool   GetisChV2Active(G4int iCh)  { 
    if(iCh<0 && iCh>512+5) return false;
    else return isChV2Active[iCh];} 

  G4double GetXPadArea()    { return fMMegaXPadArea;   }
  G4double GetYPadArea()    { return fMMegaYPadArea;   }
  G4double GetPadDistance() { return fMMegaPadDistance;}
  G4double GetXPadStartPos(){ return fXPadStartPos;}
  G4double GetYPadStartPos(){ return fYPadStartPos;}

  G4double GetMMegaAmpMeshSizeZ()   { return fMMegaAmpMeshSizeZ;}
  G4double GetMMegaCathodeMeshSizeZ()   { return fMMegaCathodeMeshSizeZ;}

  // Set position along Z of MMega front face
  void SetMMegaFrontFacePosZ(G4double z) { fMMegaFrontFacePosZ = z; }
  G4double GetMMegaFrontFacePosZ() { return fMMegaFrontFacePosZ; }
  G4double GetMMegaDisplacementX() { return fMMegaDisplacementX; }
  G4double GetMMegaDisplacementY() { return fMMegaDisplacementY; }
  G4double GetMMegaRotationAngle() { return fMMegaRotationAngle; }
  
  // Get name of MMega digitizer
  G4String GetMMegaDigitizerName() { return fMMegaDigitizerName; }

  // Get name of MMega sensitive detector
  G4String GetMMegaSensitiveDetectorName() { return fMMegaSensitiveDetectorName; }

private:

  G4int fVerbose; // Verbose level

  G4String fReadoutType;
  G4int fDetectorSetup;

  //mmega dimensions

  G4double fMMegaSizeX;
  G4double fMMegaSizeY;
  G4double fMMegaSizeZ;

  G4double fMMegaDriftSizeX;
  G4double fMMegaDriftSizeY;
  G4double fMMegaDriftSizeZ;

  G4double fMMegaAmpGapSizeX;
  G4double fMMegaAmpGapSizeY;
  G4double fMMegaAmpGapSizeZ;

  //readout plane composition
  G4double fMMegaPanelSizeX;
  G4double fMMegaPanelSizeY;
  G4double fMMegaPanelSizeZ;
  
  G4double fMMegaFaradayPanelSizeX;
  G4double fMMegaFaradayPanelSizeY;
  G4double fMMegaFaradayPanelSizeZ;
  
  G4double fMMegaCopperSizeZ;
  G4double fMMegaFR4SizeZ;
  G4double fMMegaFaradayFR4SizeZ;
  G4double fMMegaFaradayCopperSizeZ;
  G4double fMMegaNomexSizeZ;
  G4double fMMegaKaptonSizeZ;
  G4double fMMegaCarbonSizeZ; //thickness of carbon resistive layer
  G4double fMMegaLayerGap;
  G4double fMMegaFR4ReadoutSizeZ;

  //geometry of strips readout design (same for x and y)
  G4double fMMegaStripPitch;
  G4double fMMegaStripWidth;
  G4double fMMegaPanelSizeV1;
  G4double fMMegaPanelSizeV2;
  G4double fV1StripStartPos;
  G4double fV2StripStartPos;
  G4bool isChV1Active[512+1];
  
  G4bool isChV2Active[512+6];
  
  //geometry of pads readout design
  G4double fMMegaXPadArea;
  G4double fMMegaYPadArea;
  G4double fMMegaPadDistance;
  G4double fXPadStartPos;
  G4double fYPadStartPos;

  //geometry of meshes
  G4double fMMegaAmpMeshSizeZ;
  G4double fMMegaCathodeMeshSizeZ;

  G4double fMMegaFrontFacePosZ; // Position along Z axis of MMega front face
  G4double fMMegaRotationAngle; // Rotation angle of the whole MMega system (surveys)
  G4double fMMegaDisplacementX; // Displacement wrt 0 in X (surveys)
  G4double fMMegaDisplacementY; // Displacement wrt 0 in Y (surveys)
  G4String fMMegaSensitiveDetectorName;
  G4String fMMegaDigitizerName;
 

};

#endif
