// TMMDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Elisa Di Meco (elisa.di.meco@lnf.infn.it) 2026-05-27
// --------------------------------------------------------------

#ifndef TMMDetector_H
#define TMMDetector_H 1
#include "globals.hh"
#include "G4LogicalVolume.hh"

#include "TMMMessenger.hh"

class TMMDetector
{

public:

  ~TMMDetector();
  TMMDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetTMMLogicalVolume() { return fTMMVolume; }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fTMMVolume;       // container volume for TMM
  G4LogicalVolume* fDriftVolume;       // drift volume for TMM
  G4LogicalVolume* fTMMPanelVolume;  // readout plane volume for TMM
  G4LogicalVolume* fAmpMeshVolume;     // amplification meshes volume
  G4LogicalVolume* fCathodeMeshVolume; // center cathode mesh volume


  TMMMessenger* fTMMMessenger;

};

#endif
