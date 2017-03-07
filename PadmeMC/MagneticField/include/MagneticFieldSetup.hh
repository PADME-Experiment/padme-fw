#ifndef MagneticFieldSetup_H
#define MagneticFieldSetup_H

#include "G4MagneticField.hh"

class G4FieldManager;
class G4MagneticField;
class G4ChordFinder;
class G4Mag_UsualEqRhs;
class G4MagIntegratorStepper;

class MagneticFieldMap;
//class MagneticFieldMessenger;

///  A class for setting up the Magnetic Field

class MagneticFieldSetup
{

public:

  MagneticFieldSetup();
  ~MagneticFieldSetup();
      
  void SetStepperType(G4int i) { fStepperType = i; }

  void SetStepper();

  void SetMinStep(G4double s) { fMinStep = s; }

  void UpdateField();

  G4FieldManager*   GetLocalFieldManager()  { return fLocalFieldManager; }
  MagneticFieldMap* GetMagneticField() { return fMagneticField; }

  void SetMagneticFieldValue(G4double);

protected:

  G4FieldManager* GetGlobalFieldManager();

private:

  G4FieldManager* fFieldManager;
  G4FieldManager* fLocalFieldManager;

  MagneticFieldMap* fMagneticField; 

  G4ChordFinder* fChordFinder;
  G4ChordFinder* fLocalChordFinder;

  G4Mag_UsualEqRhs* fEquation;
  G4Mag_UsualEqRhs* fLocalEquation; 

  G4MagIntegratorStepper* fStepper;
  G4MagIntegratorStepper* fLocalStepper;
  G4int fStepperType;
  G4double fMinStep;
 
  //MagneticFieldMessenger* fFieldMessenger;

};

#endif
