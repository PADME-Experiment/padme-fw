#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4SubtractionSolid.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4PhysicalConstants.hh"

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
class PVetoDetector;
class EVetoDetector;
class HEPVetoDetector;
class TDumpDetector;
class TPixDetector;
class MagnetStructure;
class ChamberStructure;
class HallStructure;
class MagneticFieldSetup;
class LAVDetector;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  DetectorConstruction();
  ~DetectorConstruction();

public:
  
  G4VPhysicalVolume* Construct();
     
  G4double GetWorldFullLength() {return fWorldLength;}
     
  void SetupDetectors();
  //void SetMaxStep(G4double);

  void EnableSubDetector(G4String);
  void DisableSubDetector(G4String);

  void EnableStructure(G4String);
  void DisableStructure(G4String);

  void EnableMagneticField();
  void DisableMagneticField();

  void MagneticVolumeIsVisible();
  void MagneticVolumeIsInvisible();

  void SetMagFieldValue(G4double v);
  G4double GetMagFieldValue();

  void ChamberIsVisible();
  void ChamberIsInvisible();

  void WorldIsAir();
  void WorldIsVacuum();

  // Is this really needed?
  void SetTargetMaterial(G4String);

  // Return some global information used by beam generator
  G4double GetECalFrontFaceZ();
  G4double GetTargetFrontFaceZ();
  G4double GetTargetThickness();

private:

  void DefineMaterials();

  ECalDetector*    fECalDetector;
  TargetDetector*  fTargetDetector;
  SACDetector*     fSACDetector;
  PVetoDetector*   fPVetoDetector;
  EVetoDetector*   fEVetoDetector;
  HEPVetoDetector* fHEPVetoDetector;
  TDumpDetector*   fTDumpDetector;
  TPixDetector*    fTPixDetector;
  LAVDetector*     fLAVDetector;

  MagnetStructure*  fMagnetStructure;
  ChamberStructure* fChamberStructure;
  HallStructure*    fHallStructure;

  MagneticFieldSetup* fMagneticFieldManager;

  G4int fEnableECal;
  G4int fEnableTarget;
  G4int fEnableSAC;
  G4int fEnableLAV;
  G4int fEnablePVeto;
  G4int fEnableEVeto;
  G4int fEnableHEPVeto;
  G4int fEnableTDump;
  G4int fEnableTPix;

  G4int fEnableWall;
  G4int fEnableChamber;
  G4int fEnableMagnet;

  G4int fEnableMagneticField;
  G4int fMagneticVolumeIsVisible;

  G4int fChamberIsVisible;

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
