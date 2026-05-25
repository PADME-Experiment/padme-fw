// MMDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Davide Quaranta (quaranta.1895475@studenti.uniroma1.it) 2023-19-12
// --------------------------------------------------------------

#ifndef MMDetector_H
#define MMDetector_H 1
#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "MMMessenger.hh"

class MMDetector
{

public:

  ~MMDetector();
  MMDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetMMLogicalVolume() { return fMMVolume; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fMMVolume;       // container volume for MM
  G4LogicalVolume* fDriftVolume;       // drift volume for MM
  G4LogicalVolume* fMMPanelVolume;  // readout plane volume for MM
  G4LogicalVolume* fAmpMeshVolume;     // amplification meshes volume
  G4LogicalVolume* fCathodeMeshVolume; // center cathode mesh volume


  MMMessenger* fMMMessenger;

};

#endif
