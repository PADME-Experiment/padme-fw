#include "G4VUserPrimaryParticleInformation.hh"

class MyParticleInfo : public G4VUserPrimaryParticleInformation {
public:
  MyParticleInfo(G4String processID, G4ThreeVector parentMomentum, G4double parentEnergy) 
    : fProcessID(processID), fParentMomentum(parentMomentum), fParentEnergy(parentEnergy) {}
  virtual ~MyParticleInfo() {}
  
  // Required pure virtual method to print your data
  virtual void Print() const override {
    G4cout << "Process ID: " << fProcessID <<
      " parent P = " << fParentMomentum.x() << " " << fParentMomentum.y() << " " << fParentMomentum.z() << " E = " << fParentEnergy << G4endl;
  }

  G4String GetProcessID() const { return fProcessID; }
  G4ThreeVector GetParentMomentum() const { return fParentMomentum; }
  G4double GetParentEnergy() const { return fParentEnergy; }

private:
  G4String fProcessID;
  G4ThreeVector fParentMomentum;
  G4double fParentEnergy;
};
