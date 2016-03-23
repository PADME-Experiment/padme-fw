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
// $Id: PhysicsList.hh,v 1.2 2014/01/22 17:21:10 veni Exp $
//
// 
#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4SystemOfUnits.hh"

//Optical Processes
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"

class PhysicsList: public G4VUserPhysicsList
{
public:
  PhysicsList();
  virtual ~PhysicsList();
  
protected:
  // Construct particle and physics
  virtual void ConstructParticle();
  virtual void ConstructProcess();
  
  // 
  virtual void SetCuts();
  
protected:
  // these methods Construct particles 
  virtual void ConstructBosons();
  virtual void ConstructLeptons();
  virtual void ConstructMesons();
  virtual void ConstructBaryons();
  virtual void ConstructIons();
  
protected:
  // these methods Construct physics processes and register them
  void AddParameterisation();

  virtual void ConstructGeneral();
  virtual void ConstructEM();
  virtual void ConstructNuclearProcesses();
  virtual void ConstructOpticalPhysics();
  virtual void AddTransportation();

private:
  int IsOpticsON;
  // Optical Processes
  G4Cerenkov*          fCerenkovProcess;
  G4Scintillation*     fScintillationProcess;
  G4OpAbsorption*      fAbsorptionProcess;
  G4OpRayleigh*        fRayleighScatteringProcess;
  G4OpBoundaryProcess* fBoundaryProcess;
  //  G4OpWLS*             fWLSProcess;
};

#endif



