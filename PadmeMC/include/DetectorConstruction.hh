#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4SubtractionSolid.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4PhysicalConstants.hh"
#include "G4GDMLParser.hh"

class G4Box;
class G4Tubs;
class G4Cons;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4VPVParameterisation;
class G4UserLimits;
class G4SubtractionSolid;

class DetectorMessenger;
class ECalDetector;
class TargetDetector;
class SACDetector;
class ETagDetector;
class MMegaDetector; //D. Quaranta 20/12/2023
class PVetoDetector;
class EVetoDetector;
class HEPVetoDetector;
class TDumpDetector;
class TPixDetector;
class TungstenDetector;
class MagnetStructure;
class ChamberStructure;
class BeamLineStructure; //M. Raggi 07/03/2019
class HallStructure;
class MagneticFieldSetup;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  DetectorConstruction();
  ~DetectorConstruction();

public:
  
  G4VPhysicalVolume* Construct();
     
  void SetVerboseLevel(G4int v) { fVerbose = v; }
  G4int GetVerboseLevel() { return fVerbose; }

  void SetDetectorSetup(G4int);
  G4int GetDetectorSetup() { return fDetectorSetup; }

  G4double GetWorldFullLength() {return fWorldLength;}
     
  void SetupDetectors();
  //void SetMaxStep(G4double);

  void EnableSubDetector(G4String);
  void DisableSubDetector(G4String);
  G4bool IsSubDetectorEnabled(G4String);

  void EnableStructure(G4String);
  void DisableStructure(G4String);
  G4bool IsStructureEnabled(G4String);

  void EnableMagneticField();
  void DisableMagneticField();
  G4bool IsMagneticFieldEnabled();

  void SetCrossMagneticVolume(G4String);

  void MagneticVolumeIsVisible();
  void MagneticVolumeIsInvisible();

  void SetMagFieldValue(G4double);
  G4double GetMagFieldValue();

  void MagnetIsVisible();
  void MagnetIsInvisible();

  void ChamberIsVisible();
  void ChamberIsInvisible();

  //M. Raggi 07/03/2019
  void BeamLineIsVisible();
  void BeamLineIsInvisible();

  //D.Quaranta 09/04/2024
  void SetMMegaReadoutType(G4String); 

  void WorldIsAir();
  void WorldIsVacuum();

  // Is this really needed?
  void SetTargetMaterial(G4String);

  // Return some global information used by beam generator
  G4double GetECalFrontFaceZ();
  G4double GetTargetFrontFaceZ();
  G4double GetTargetThickness();

  // Recursive method to check full geometry
  void CheckDaughters(G4LogicalVolume*,G4int);

private:

  void DefineMaterials();

  G4int fVerbose; // Verbose level

  G4int fDetectorSetup;

  G4GDMLParser fParser;

  ECalDetector*     fECalDetector;
  TargetDetector*   fTargetDetector;
  SACDetector*      fSACDetector;
  ETagDetector*     fETagDetector;
  MMegaDetector*    fMMegaDetector; //D. Quaranta 20/12/23
  PVetoDetector*    fPVetoDetector;
  EVetoDetector*    fEVetoDetector;
  HEPVetoDetector*  fHEPVetoDetector;
  TDumpDetector*    fTDumpDetector;
  TPixDetector*     fTPixDetector;
  TungstenDetector* fTungstenDetector;

  MagnetStructure*  fMagnetStructure;
  ChamberStructure* fChamberStructure;
  BeamLineStructure* fBeamLineStructure; //M. Raggi 07/03/2019
  HallStructure*    fHallStructure;

  MagneticFieldSetup* fMagneticFieldManager;

  G4int fEnableECal;
  G4int fEnableTarget;
  G4int fEnableSAC;
  G4int fEnableETag;
  G4int fEnableMMega; // D.Quaranta 20/12/23
  G4int fEnablePVeto;
  G4int fEnableEVeto;
  G4int fEnableHEPVeto;
  G4int fEnableTDump;
  G4int fEnableTPix;
  G4int fEnableTungsten;

  G4int fEnableWall;
  G4int fEnableChamber;
  G4int fEnableBeamLine;  //M. Raggi 07/03/2019
  G4int fEnableMagnet;

  G4int fEnableMagneticField;
  G4int fMagneticVolumeIsVisible;

  G4String fCrossMagneticVolume;

  G4int fMagnetIsVisible;
  G4int fChamberIsVisible;
  G4int fBeamLineIsVisible; //M. Raggi 07/03/2019

  G4String fMMegaReadoutType; //D.Quaranta 09/04/2024

  G4int fWorldIsFilledWithAir;

  G4double fWorldLength;

  //G4UserLimits* stepLimit; // pointer to user step limits

  DetectorMessenger* fDetectorMessenger;  // pointer to the Messenger
  /*
private:

  G4double CryX[1000];
  G4double CryY[1000];
  G4double CryZ[1000];
  */
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
