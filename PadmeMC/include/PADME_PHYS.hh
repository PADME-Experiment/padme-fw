//
// ********************************************************************
//  PADME modular physics list
//  Based on QGSP_BERT + OpticalPhotons
//
// ********************************************************************
//
#ifndef _PADME_PHYS_H_
#define _PADME_PHYS_H_


#include <CLHEP/Units/SystemOfUnits.h>

#include "globals.hh"
#include "G4VModularPhysicsList.hh"
#include "CompileTimeConstraints.hh"


class PADME_PHYS: public G4VModularPhysicsList
{
public:
					       
  PADME_PHYS(G4int ver = 1);
  virtual ~PADME_PHYS();
  
public:
  // SetCuts() 
  virtual void SetCuts();

private:
  enum {ok = CompileTimeConstraints::IsA<G4VModularPhysicsList, G4VModularPhysicsList>::ok };
};

#endif
