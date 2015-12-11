//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: DetectorConstruction.hh,v 1.4 2014/06/23 13:44:14 veni Exp $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4SubtractionSolid.hh"
#include "MagneticField.hh"
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
     DetectorConstruction();
    ~DetectorConstruction();

  public:
  
     G4VPhysicalVolume* Construct();
     
     G4double GetWorldFullLength()   {return fWorldLength;}; 
     G4double GetCryPosX(G4int CryInd){return CryX[CryInd];}; 
     G4double GetCryPosY(G4int CryInd){return CryY[CryInd];}; 
     G4double GetCryPosZ(G4int CryInd){return CryZ[CryInd];}; 
     
     void setTargetMaterial(G4String);
     void SetupDetectors();
  //     void SetMagField(G4double);
     void SetMaxStep (G4double);     
  
  private:
     void DefineMaterials();

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

     G4Box*             solidTarget;   
     G4LogicalVolume*   logicTarget;   
     G4VPhysicalVolume* physiTarget;   

     G4Tubs*            solidTDump;  //Target DUMP
     G4LogicalVolume*   logicTDump;
     G4VPhysicalVolume* physiTDump;

     G4Cons*            solidDumpGFilt;  //Target DUMP gamma filter
     G4LogicalVolume*   logicDumpGFilt;
     G4VPhysicalVolume* physiDumpGFilt;

  // G4SubtractionSolid* solidGSubFilt;   
 
     G4Box*             solidTXRod;   
     G4LogicalVolume*   logicTXRod;   
     G4VPhysicalVolume* physiTXRod;   

     G4Box*             solidTYRod;   
     G4LogicalVolume*   logicTYRod;   
     G4VPhysicalVolume* physiTYRod;   

     G4Box*             solidMonitor;  
     G4LogicalVolume*   logicMonitor;  
     G4VPhysicalVolume* physiMonitor;  

     G4Box*             solidMXRod;   
     G4LogicalVolume*   logicMXRod;   
     G4VPhysicalVolume* physiMXRod;   

     G4Box*             solidMYRod;   
     G4LogicalVolume*   logicMYRod;   
     G4VPhysicalVolume* physiMYRod;   

     G4Box*             solidEcal;   // pointer to the solid Target
     G4LogicalVolume*   logicEcal;   // pointer to the logical Target
     G4VPhysicalVolume* physiEcal;   // pointer to the physical Target
 
     G4Box*             solidCry;   // pointer to the solid Target
     G4LogicalVolume*   logicCry;   // pointer to the logical Target
     G4VPhysicalVolume* physiCry;   // pointer to the physical Target

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

     G4VSolid*           solidEVeto;   
     G4LogicalVolume*    logicEVeto;   
     G4VPhysicalVolume*  physiEVeto;   

     G4VSolid*           solidVetoFinger;
     G4LogicalVolume*    logicVetoFinger;
     G4VPhysicalVolume*  physiVetoFinger;

     G4VSolid*           solidPosVeto;   
     G4LogicalVolume*    logicPosVeto;   
     G4VPhysicalVolume*  physiPosVeto;   

     G4VSolid*           solidPosVetoFinger;
     G4LogicalVolume*    logicPosVetoFinger;
     G4VPhysicalVolume*  physiPosVetoFinger;

     G4VSolid*           solidEleVeto;   
     G4LogicalVolume*    logicEleVeto;   
     G4VPhysicalVolume*  physiEleVeto;   

     G4VSolid*           solidEleVetoFinger;
     G4LogicalVolume*    logicEleVetoFinger;
     G4VPhysicalVolume*  physiEleVetoFinger;

     G4VSolid*           solidSAC;   
     G4LogicalVolume*    logicSAC;   
     G4VPhysicalVolume*  physiSAC;

     G4Tubs*            solidLAV;   
     G4LogicalVolume*    logicLAV;   
     G4VPhysicalVolume*  physiLAV;

     G4Box*              solidPGEM;
     G4LogicalVolume*    logicPGEM;
     G4VPhysicalVolume*  physiPGEM;
 
     G4Material*         TargetMater;  // pointer to the target  material
     G4Material*         WorldMater; // pointer to the chamber material

     G4UserLimits* stepLimit;             // pointer to user step limits

     G4UniformMagField*              fMagField;   // pointer to the magnetic field 
     F03FieldSetup*              fEmFieldSetup;     
     DetectorMessenger*      detectorMessenger;  // pointer to the Messenger
       
     G4double fWorldLength;            // Full length of the world volume

     private:
     G4double CryX[1000];
     G4double CryY[1000];
     G4double CryZ[1000];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
