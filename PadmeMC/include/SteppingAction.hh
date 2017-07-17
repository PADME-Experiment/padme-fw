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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class EventAction;
class G4Track;
class HistoManager;
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

  G4ThreeVector & GetPositronMomentum(){ return PositronMomentum;  };
  

private:
  BeamParameters* bpar;
  EventAction* fEventAction;
  HistoManager *fHistoManager;

  G4int fEnableSACAnalysis;
  G4int fEnableECalAnalysis;

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
  //   HistoManager* fHistoManager;
private:
  double fSACEnergyThr;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
