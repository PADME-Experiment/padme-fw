#ifndef MagneticFieldSetup_H
#define MagneticFieldSetup_H

#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"

#include "MagneticFieldMap.hh"

class G4FieldManager;
class G4ChordFinder;
class G4Mag_UsualEqRhs;
class G4MagIntegratorStepper;

//class MagneticFieldMessenger;

///  A class for setting up the Magnetic Field

class MagneticFieldSetup
{

public:

  MagneticFieldSetup();               // A zero field
  ~MagneticFieldSetup();
      
  void SetStepperType(G4int i) { fStepperType = i; }

  void SetStepper();

  void SetMinStep(G4double s) { fMinStep = s; }

  void UpdateField();

  MagneticFieldMap* GetMagneticField() { return fMagneticField; }

protected:

  G4FieldManager* GetGlobalFieldManager();

private:

  G4FieldManager* fFieldManager;
  MagneticFieldMap* fMagneticField; 

  G4ChordFinder* fChordFinder;
  G4Mag_UsualEqRhs* fEquation;

  G4MagIntegratorStepper* fStepper;
  G4int fStepperType;
  G4double fMinStep;
 
  //MagneticFieldMessenger* fFieldMessenger;

};

#endif
