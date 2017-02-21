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
class LAVDetector;
class PVetoDetector;
class EVetoDetector;
class HEPVetoDetector;
class TDumpDetector;
class TPixDetector;
class MagnetStructure;
class ChamberStructure;
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
     
  G4double GetWorldFullLength()    {return fWorldLength;}; 
  G4double GetCryPosX(G4int CryInd){return CryX[CryInd];}; 
  G4double GetCryPosY(G4int CryInd){return CryY[CryInd];}; 
  G4double GetCryPosZ(G4int CryInd){return CryZ[CryInd];}; 
     
  void setTargetMaterial(G4String);
  void SetupDetectors();
  //     void SetMagField(G4double);
  void SetMaxStep (G4double);

  void EnableSubDetector(G4String);
  void DisableSubDetector(G4String);

  void EnableStructure(G4String);
  void DisableStructure(G4String);

  void EnableMagneticField()  { fEnableMagneticField = 1; }
  void DisableMagneticField() { fEnableMagneticField = 0; }

  // Return some global information used by beam generator
  G4double GetECalFrontFaceZ();
  G4double GetTargetFrontFaceZ();
  G4double GetTargetThickness();

private:

  void DefineMaterials();

  ECalDetector*    fECalDetector;
  TargetDetector*  fTargetDetector;
  SACDetector*     fSACDetector;
  LAVDetector*     fLAVDetector;
  PVetoDetector*   fPVetoDetector;
  EVetoDetector*   fEVetoDetector;
  HEPVetoDetector* fHEPVetoDetector;
  TDumpDetector*   fTDumpDetector;
  TPixDetector*    fTPixDetector;

  MagnetStructure*  fMagnetStructure;
  ChamberStructure* fChamberStructure;
  HallStructure*    fHallStructure;

  MagneticFieldSetup* fEmFieldSetup;     

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

  G4Box*             solidWorld;    
  G4LogicalVolume*   logicWorld;    
  G4VPhysicalVolume* physiWorld;    

  G4Box*             solidWall;    
  G4LogicalVolume*   logicWall;    
  G4VPhysicalVolume* physiWall;    

  G4Box*             solidLeadBrick;
  G4LogicalVolume*   logicLeadBrick;
  G4VPhysicalVolume* physiLeadBrickL;
  G4VPhysicalVolume* physiLeadBrickR;
  G4VPhysicalVolume* physiLeadBrickT;
  G4VPhysicalVolume* physiLeadBrickB;

  G4Tubs*            solidTDump;  //Target DUMP
  G4LogicalVolume*   logicTDump;
  G4VPhysicalVolume* physiTDump;

  G4Cons*            solidDumpGFilt;  //Target DUMP gamma filter
  G4LogicalVolume*   logicDumpGFilt;
  G4VPhysicalVolume* physiDumpGFilt;

  // G4SubtractionSolid* solidGSubFilt;   
 
  //G4Box*             solidTXRod;   
  //G4LogicalVolume*   logicTXRod;   
  //G4VPhysicalVolume* physiTXRod;   

  //G4Box*             solidTYRod;   
  //G4LogicalVolume*   logicTYRod;   
  //G4VPhysicalVolume* physiTYRod;   

  G4Box*             solidMonitor;  
  G4LogicalVolume*   logicMonitor;  
  G4VPhysicalVolume* physiMonitor;  

  G4Box*             solidMXRod;   
  G4LogicalVolume*   logicMXRod;   
  G4VPhysicalVolume* physiMXRod;   

  G4Box*             solidMYRod;   
  G4LogicalVolume*   logicMYRod;   
  G4VPhysicalVolume* physiMYRod;   

  //CERN MAGNET MODEL

  G4Box*             solidMagUpJoke;  
  G4LogicalVolume*   logicMagUpJoke;  
  G4VPhysicalVolume* physiMagUpJoke;  
			     
  G4Box*             solidMagDwJoke;  
  G4LogicalVolume*   logicMagDwJoke;  
  G4VPhysicalVolume* physiMagDwJoke;  

  G4Box*             solidMagLfJoke;  
  G4LogicalVolume*   logicMagLfJoke;  
  G4VPhysicalVolume* physiMagLfJoke;  

  G4Box*             solidMagRgJoke;  
  G4LogicalVolume*   logicMagRgJoke;  
  G4VPhysicalVolume* physiMagRgJoke;  

  G4Tubs*            solidMagCoil;
  G4LogicalVolume*   logicMagCoil;
  G4VPhysicalVolume* physiMagUpCoil;
  G4VPhysicalVolume* physiMagDwCoil;
  G4VPhysicalVolume* physiMagUpBkCoil;
  G4VPhysicalVolume* physiMagDwBkCoil;

  //was BTF MAGNET model
  //     G4Tubs*            solidMagIron;
  //     G4LogicalVolume*   logicMagIron;
  //     G4VPhysicalVolume* physiMagIron;
  //
  //     G4Tubs*            solidMagInnJoke;
  //     G4LogicalVolume*   logicMagInnJoke;
  //     G4VPhysicalVolume* physiMagInnJoke;
  //
  //     G4Tubs*            solidMagOutJoke;
  //     G4LogicalVolume*   logicMagOutJoke;
  //     G4VPhysicalVolume* physiMagOutJoke;
  //
  //     G4Tubs*            solidMagBArea;
  //     G4LogicalVolume*   logicMagBArea;
  //     G4VPhysicalVolume* physiMagBArea;

  G4Box*             solidSwepMag;  // pointer to the solid Tracker
  G4LogicalVolume*   logicSwepMag;  // pointer to the logical Tracker
  G4VPhysicalVolume* physiSwepMag;  // pointer to the physical Tracker

  G4Tubs*            solidTracker[10];  // pointer to the solid Chamber
  G4LogicalVolume*   logicTracker[10];  // pointer to the logical Chamber
  G4VPhysicalVolume* physiTracker[10];  // pointer to the physical Chamber

  G4Box*              solidPGEM;
  G4LogicalVolume*    logicPGEM;
  G4VPhysicalVolume*  physiPGEM;
 
  //G4Material*         TargetMater;  // pointer to the target  material
  G4Material*         WorldMater; // pointer to the chamber material

  G4UserLimits* stepLimit;             // pointer to user step limits

  DetectorMessenger* fDetectorMessenger;  // pointer to the Messenger
       
  G4double fWorldLength;            // Full length of the world volume

private:

  G4double CryX[1000];
  G4double CryY[1000];
  G4double CryZ[1000];

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
