#include "MagnetSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MagnetSD::MagnetSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="MagnetCollection"); //crea il collection name
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MagnetSD::~MagnetSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MagnetSD::Initialize(G4HCofThisEvent* HCE)
{
  MagnetCollection = new MagnetHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,MagnetCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MagnetSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{

  // Create new hit only if some energy was release in the SD
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  MagnetHit* newHit = new MagnetHit();
  newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
  newHit->SetTime(aStep->GetTrack()->GetGlobalTime())	;
  newHit->SetEnergy(edep);
  newHit->SetPosition(aStep->GetPostStepPoint()->GetPosition());
  MagnetCollection->insert(newHit);

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MagnetSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
     G4int NbHits = MagnetCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
            << " hits in the Magnet : " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*MagnetCollection)[i]->Print();
    } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
