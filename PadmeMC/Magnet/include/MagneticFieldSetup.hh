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
///
///  It also creates the necessary classes to control accuracy of propagation.
///  In this example
///    - There is a global field for most of the setup;
///    - A local  field overides it for some volume(s) and it assumed to be 
///      uniform.

class MagneticFieldSetup
{

public:

  MagneticFieldSetup();               // A zero field
  MagneticFieldSetup(G4ThreeVector);  // The value of the field
  ~MagneticFieldSetup();
      
  void SetStepperType(G4int i) { fStepperType = i; }

  void SetStepper();

  void SetMinStep(G4double s) { fMinStep = s; }

  void UpdateField();

  void SetFieldValue(G4ThreeVector fieldVector);
  void SetFieldValue(G4double      fieldValue);
  G4ThreeVector GetConstantFieldValue();

  G4FieldManager*   GetLocalFieldManager()  { return fLocalFieldManager; }
  MagneticFieldMap* GetLocalMagneticField() { return fLocalMagneticField; }

protected:

  G4FieldManager* GetGlobalFieldManager();

  G4FieldManager* fFieldManager;
  G4FieldManager* fLocalFieldManager;

  G4ChordFinder* fChordFinder;
  G4ChordFinder* fLocalChordFinder;

  G4Mag_UsualEqRhs* fEquation; 
  G4Mag_UsualEqRhs* fLocalEquation; 

  G4MagneticField*  fMagneticField; 
  MagneticFieldMap* fLocalMagneticField; 

  G4MagIntegratorStepper* fStepper;
  G4MagIntegratorStepper* fLocalStepper;
  G4int                   fStepperType;
  G4double                fMinStep;
 
  //MagneticFieldMessenger* fFieldMessenger;

};

#endif
