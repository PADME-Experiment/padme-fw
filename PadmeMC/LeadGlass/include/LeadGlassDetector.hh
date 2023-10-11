// LeadGlassDetector.hh
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2023-09-20
// --------------------------------------------------------------

#ifndef LeadGlassDetector_H
#define LeadGlassDetector_H 1

#include "globals.hh"

class G4LogicalVolume;
class LeadGlassMessenger;

#include "LeadGlassGeometry.hh"

class LeadGlassDetector
{

public:

  ~LeadGlassDetector();
  LeadGlassDetector(G4LogicalVolume*);

  void SetMotherVolume(G4LogicalVolume* v) { fMotherVolume = v; }
  void CreateGeometry();

  G4LogicalVolume* GetLeadGlassLogicalVolume() { return fLeadGlassVolume; }

  G4double GetLeadGlassFrontFaceZ() { return LeadGlassGeometry::GetInstance()->GetLeadGlassFrontFacePosZ(); }

private:

  G4LogicalVolume* fMotherVolume;
  G4LogicalVolume* fLeadGlassVolume;

  LeadGlassMessenger* fLeadGlassMessenger;

};

#endif
