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
// $Id: SteppingAction.hh,v 1.3 2014/04/30 08:57:38 veni Exp $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4ThreeVector.hh"
#include "BeamParameters.hh"

#include <unordered_map>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class EventAction;
class G4Track;
class G4VPhysicalVolume;
class HistoManager;
class MCTruthManager;
class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction();
  ~SteppingAction(){};
  
  void UserSteppingAction(const G4Step*);
  double GetPhysProc()   {return ProcID;};
  void   SetPhysProc(float value);
  
  double GetPositronE()  {return BeamPartE;}; //energy of the primary after the first interaction
  double GetPrimE()      {return PrimE;};     //original energy of the primary
  double GetPositronX()  {return VertexPos[0];};
  double GetPositronY()  {return VertexPos[1];};
  double GetPositronZ()  {return VertexPos[2];};
  
  double GetGammaEnergy(){return GammaE;};
  double SetGammaAngle(G4ThreeVector GammaDir,G4ThreeVector BeamDir);
  double GetGammaAngle(){return ThetaGamma;};

  void EnableSACAnalysis()  { fEnableSACAnalysis = 1; }
  void DisableSACAnalysis() { fEnableSACAnalysis = 0; }

  void EnableECalAnalysis()  { fEnableECalAnalysis = 1; }
  void DisableECalAnalysis() { fEnableECalAnalysis = 0; }

  void EnableMMAnalysis()  { fEnableMMAnalysis = 1; } //EDM from D.Quaranta
  void DisableMMAnalysis() { fEnableMMAnalysis = 0; }

  void EnableTMMAnalysis()  { fEnableTMMAnalysis = 1; } //EDM 
  void DisableTMMAnalysis() { fEnableTMMAnalysis = 0; }


  G4ThreeVector & GetPositronMomentum(){ return PositronMomentum;  };

  // MR 26/11/2026 forget the volume classification (called at the start of each event,
  // in case the geometry is rebuilt between runs)
  void ResetVolumeCache() { fVolumeFlags.clear(); fLastVolume = 0; fLastVolumeFlags = 0; }


private:
  BeamParameters* bpar;
  EventAction* fEventAction;
  HistoManager *fHistoManager;
  MCTruthManager* fMCTruthManager;

  G4int fEnableSACAnalysis;
  G4int fEnableECalAnalysis;
  G4int fEnableMMAnalysis;
  G4int fEnableTMMAnalysis;

  double ProcID;
  double PositronE;
  double ElectronE;	
  double BeamPartE;
  double GammaE;
  double PrimE;
  double ThetaGamma;
  G4ThreeVector GammaDir;
  G4ThreeVector VertexPos;
  G4ThreeVector PositronMomentum;
  G4int NChild;

  G4int ClassifyTrack(G4Track* );

  // MR 26/11/2026 the volume names used in UserSteppingAction were compared as strings
  // at every step (~7% of the total CPU time in RunIV production). Each physical volume
  // is now classified once, the first time it is seen, and the flags are cached.
  enum VolumeFlag : unsigned int {
    kVolTarget        = 1u<<0, // "Target"
    kVolTargetSupport = 1u<<1, // "TargetSupport"
    kVolECal          = 1u<<2, // "ECal"
    kVolWorldPV       = 1u<<3, // "world_volume_PV"
    kVolWorld         = 1u<<4, // "World"
    kVolSACCry        = 1u<<5, // "SACCry"
    kVolKill          = 1u<<6  // BTF collimators SLTB2-5, LINAC wall and pipes: tracks killed on entry
  };
  unsigned int VolumeFlags(const G4VPhysicalVolume*);
  std::unordered_map<const G4VPhysicalVolume*,unsigned int> fVolumeFlags;
  const G4VPhysicalVolume* fLastVolume = 0;
  unsigned int fLastVolumeFlags = 0;

private:
  double fSACEnergyThr;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
