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
  fTPixCollection = new TPixHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if(HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fTPixCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TPixSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{

  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  TPixHit* newHit = new TPixHit();

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);
  //G4cout << "PreStepPoint in " << touchHPre->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPre,"Length")
  //	 << " local " << G4BestUnit(localPosPre,"Length") << G4endl;

  // devi usare la formula giusta non e' questa anche se quasi funziona per hit singoli
  G4ThreeVector worldPosPost   = aStep->GetPostStepPoint()->GetPosition();
  G4TouchableHandle touchHPost = aStep->GetPostStepPoint()->GetTouchableHandle();
  G4ThreeVector localPosPost   = touchHPost->GetHistory()->GetTopTransform().TransformPoint(worldPosPost);  //Uncommented M. Raggi 26/03/2019
  //G4cout << "PostStepPoint in " << touchHPost->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPost,"Length")
  //	 << " local " << G4BestUnit(localPosPost,"Length") << G4endl;
  //  newHit->SetLocalPosition(localPosPre);


  // Saving only hits exiting the TPix not correct if you want to study clustering
  //  if(aStep->GetPostStepPoint()->GetStepStatus()==fGeomBoundary){
  if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary && aStep->GetTrack()->GetParentID()==0){
    newHit->SetChannelId(touchHPre->GetCopyNumber());
    newHit->SetEnergy(edep);
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    newHit->SetTrackEnergy( aStep->GetPreStepPoint()->GetTotalEnergy()); //M. Raggi 27/03/2019
    newHit->SetPosition(worldPosPre); 
    //    newHit->SetLocalPosition(localPosPost); //M Raggi 26/03/2019
    newHit->SetLocalPosition(localPosPre); //M Raggi 30/09/2019 to check
    fTPixCollection->insert(newHit);
  }
  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TPixSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int nHits = fTPixCollection->entries();
    G4cout << "\n-- TPix Hits Collection: " << nHits << " hits --" << G4endl;
    for (G4int i=0;i<nHits;i++) (*fTPixCollection)[i]->Print();
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
