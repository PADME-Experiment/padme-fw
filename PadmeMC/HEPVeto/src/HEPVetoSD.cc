#include "HEPVetoSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HEPVetoSD::HEPVetoSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="HEPVetoCollection"); //crea il collection name
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HEPVetoSD::~HEPVetoSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HEPVetoSD::Initialize(G4HCofThisEvent* HCE)
{
  HEPVetoCollection = new HEPVetoHitsCollection
                          (SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection(HCID,HEPVetoCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool HEPVetoSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4ThreeVector posix = aStep->GetPostStepPoint()->GetPosition();
  //  printf("Hey I'm processing a Hit %f\n",edep);
  if(edep==0.) return false;

  edep  = aStep->GetTrack()->GetTotalEnergy();
  HEPVetoHit* newHit = new HEPVetoHit();
  newHit -> SetTrackID(aStep->GetTrack()->GetTrackID());
  //newHit -> SetTrackCh(aStep->GetTrack()->GetParticleDefinition()->GetPDGCharge());
  newHit -> SetHEPVetoNb(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  newHit -> SetEdep(edep);
  newHit -> SetTime(aStep->GetTrack()->GetGlobalTime())	;
  newHit -> SetPos(aStep->GetPostStepPoint()->GetPosition());
  newHit -> SetHitE(aStep->GetTotalEnergyDeposit());
  HEPVetoCollection ->insert( newHit );
  // Totedep=ToTEDep+edep;
  // printf("EDep %f %d\n",edep,aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  //  newHit->Print();
  //  newHit->Draw();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HEPVetoSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
     G4int NbHits = HEPVetoCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
            << " hits in the HEPVeto : " << G4endl;
     for (G4int i=0;i<NbHits;i++) (*HEPVetoCollection)[i]->Print();
    } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
