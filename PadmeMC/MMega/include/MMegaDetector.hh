// MMegaDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#ifndef MMegaDetector_H
#define MMegaDetector_H 1
#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "MMegaMessenger.hh"

class MMegaDetector
{

public:

  ~MMegaDetector();
  MMegaDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void SetReadoutType(G4String type) {fReadoutType = type;}
  void CreateGeometry();

  G4LogicalVolume* GetMMegaLogicalVolume() { return fMMegaVolume; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fMMegaVolume;       // container volume for MMega
  G4LogicalVolume* fDriftVolume;       // drift volume for MMega
  G4LogicalVolume* fMMegaPanelVolume;  // readout plane volume for MMega
  G4LogicalVolume* fAmpMeshVolume;     // amplification meshes volume
  G4LogicalVolume* fCathodeMeshVolume; // center cathode mesh volume

  G4String fReadoutType;

  MMegaMessenger* fMMegaMessenger;

};

#endif
