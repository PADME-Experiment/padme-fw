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

  // Size of MMega
  G4double GetMMegaSizeX() { return fMMegaSizeX; }
  G4double GetMMegaSizeY() { return fMMegaSizeY; }
  G4double GetMMegaSizeZ() { return fMMegaSizeZ; }

  G4double GetMMegaCopperSizeZ(){ return fMMegaCopperSizeZ; }
  G4double GetMMegaFreonSizeZ() { return fMMegaFreonSizeZ;  }
  G4double GetMMegaNomexSizeZ() { return fMMegaNomexSizeZ;  }
  G4double GetMMegaKaptonSizeZ(){ return fMMegaKaptonSizeZ; }
  G4double GetMMegaCarbonSizeZ(){ return fMMegaCarbonSizeZ; }
  G4double GetMMegaLayerGap()   { return fMMegaLayerGap;    }
  
  G4double GetMMegaPanelSizeX() { return fMMegaPanelSizeX; }
  G4double GetMMegaPanelSizeY() { return fMMegaPanelSizeY; }
  G4double GetMMegaPanelSizeZ() { return fMMegaPanelSizeZ; }

  G4double GetMMegaDriftSizeX() { return fMMegaDriftSizeX; }
  G4double GetMMegaDriftSizeY() { return fMMegaDriftSizeY; }
  G4double GetMMegaDriftSizeZ() { return fMMegaDriftSizeZ; }
  
  G4double GetMMegaAmpGapSizeX() { return fMMegaAmpGapSizeX; }
  G4double GetMMegaAmpGapSizeY() { return fMMegaAmpGapSizeY; }
  G4double GetMMegaAmpGapSizeZ() { return fMMegaAmpGapSizeZ; } 

  G4double GetMMegaMeshSizeZ()   { return fMMegaMeshSizeZ;   }
  G4double GetMMegaPitch()       { return fMMegaMeshPitch;   }

  G4double GetXPadArea()   { return fMMegaXPadArea;   }
  G4double GetYPadArea()   { return fMMegaYPadArea;   }
  G4double GetPadDistance(){ return fMMegaPadDistance;}

  // Position of MMega Volume
  G4double GetMMegaPosX() { return 0.; }
  G4double GetMMegaPosY() { return 0.; }
  G4double GetMMegaPosZ() { return fMMegaFrontFacePosZ+fMMegaSizeZ*0.5; }


  // Set position along Z of MMega front face
  void SetMMegaFrontFacePosZ(G4double z) { fMMegaFrontFacePosZ = z; }
  
  // Get name of MMega digitizer
  G4String GetMMegaDigitizerName() { return fMMegaDigitizerName; }

  // Get name of MMega sensitive detector
  G4String GetMMegaSensitiveDetectorName() { return fMMegaSensitiveDetectorName; }

private:

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  G4double fMMegaSizeX;
  G4double fMMegaSizeY;
  G4double fMMegaSizeZ;
  
  //readout plane composition
  G4double fMMegaCopperSizeZ;
  G4double fMMegaFreonSizeZ;
  G4double fMMegaNomexSizeZ;
  G4double fMMegaKaptonSizeZ;
  G4double fMMegaCarbonSizeZ; //thickness of carbon resistive layer
  G4double fMMegaLayerGap;

  G4double fMMegaXPadArea;
  G4double fMMegaYPadArea;
  G4double fMMegaPadDistance;

  G4double fMMegaPanelSizeX;
  G4double fMMegaPanelSizeY;
  G4double fMMegaPanelSizeZ;

  G4double fMMegaDriftSizeX;
  G4double fMMegaDriftSizeY;
  G4double fMMegaDriftSizeZ;

  G4double fMMegaAmpGapSizeX;
  G4double fMMegaAmpGapSizeY;
  G4double fMMegaAmpGapSizeZ;

  G4double fMMegaMeshSizeZ;
  G4double fMMegaMeshPitch;


  G4double fMMegaFrontFacePosZ; // Position along Z axis of MMega front face
  G4String fMMegaSensitiveDetectorName;
  G4String fMMegaDigitizerName;

};

#endif
