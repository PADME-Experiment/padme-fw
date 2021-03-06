#include "TargetSD.hh"

#include "G4UnitsTable.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TargetSD::TargetSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="TargetCollection");
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TargetSD::~TargetSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TargetSD::Initialize(G4HCofThisEvent* HCE)
{
  fTargetCollection = new TargetHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fTargetCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TargetSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{

  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep==0.) return false;

  TargetHit* newHit = new TargetHit();

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);
  
  G4ThreeVector worldPosPost = aStep->GetPostStepPoint()->GetPosition();
  G4TouchableHandle touchHPost = aStep->GetPostStepPoint()->GetTouchableHandle();
  G4ThreeVector localPosPost = touchHPost->GetHistory()->GetTopTransform().TransformPoint(worldPosPost);

  //G4cout << "PreStepPoint in " << touchHPre->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPre,"Length")
  //	 << " local " << G4BestUnit(localPosPre,"Length") << G4endl;
  if(aStep->GetTrack()->GetParentID()==0){ 
    newHit->SetPrimary();
  }else{
    newHit->SetNotPrimary();
  }
  if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary){
    newHit->SetEnergy(edep);
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    newHit->SetTrackEnergy(aStep->GetPreStepPoint()->GetTotalEnergy()); //M. Raggi 2/04/2019
    newHit->SetPDir(aStep->GetTrack()->GetMomentumDirection());  //this is post step we want the exit direction
    newHit->SetPosition(worldPosPre);
    newHit->SetLocalPosition(localPosPre);
    newHit->SetGlobalPositionPost(worldPosPost);
    newHit->SetLocalPositionPost(localPosPost);

    //    std::cout<<"Evento strano "<<localPosPre<<std::endl;
    //    if(abs(localPosPre.y())>0.00001){ 
      //      std::cout<<"Evento strano "<<aStep->GetTrack()->GetParentID()<<" "<<std::endl; 
    //    }
    fTargetCollection->insert(newHit);
  }
  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TargetSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int NbHits = fTargetCollection->entries();
    G4cout << "\n-- Target Hits Collection: " << NbHits << " hits --" << G4endl;
    for (G4int i=0;i<NbHits;i++) (*fTargetCollection)[i]->Print();
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
