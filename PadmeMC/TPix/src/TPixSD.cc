#include "TPixSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TPixSD::TPixSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="TPixCollection"); //crea il collection name
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TPixSD::~TPixSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TPixSD::Initialize(G4HCofThisEvent* HCE)
{
  TPixCollection = new TPixHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0) { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,TPixCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TPixSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{

  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  TPixHit* newHit = new TPixHit();
  newHit->SetChannelId(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  newHit->SetEnergy(edep);
  newHit->SetTime(aStep->GetTrack()->GetGlobalTime())	;
  newHit->SetPosition(aStep->GetPostStepPoint()->GetPosition());
  TPixCollection->insert(newHit);

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TPixSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int nHits = TPixCollection->entries();
    G4cout << "\n-> TPix hits collection has " << nHits << " hits" << G4endl;
    for (G4int i=0;i<nHits;i++) (*TPixCollection)[i]->Print();
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
