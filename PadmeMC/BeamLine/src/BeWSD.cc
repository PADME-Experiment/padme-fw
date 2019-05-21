#include "BeWSD.hh"

#include "G4UnitsTable.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeWSD::BeWSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="BeWCollection");
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeWSD::~BeWSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeWSD::Initialize(G4HCofThisEvent* HCE)
{
  fBeWCollection = new BeWHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fBeWCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool BeWSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{

  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep==0.) return false;

  BeWHit* newHit = new BeWHit();

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);

  //  G4cout << "PreStepPoint in BeW " << touchHPre->GetVolume()->GetName()<<std::endl;
  //	 << " global " << G4BestUnit(worldPosPre,"Length")
  //	 << " local " << G4BestUnit(localPosPre,"Length") << G4endl;

  // this way you only see primary particle there is a component coming form background that is killed here.
  if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary && aStep->GetTrack()->GetParentID()==0){
    //if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary){
    newHit->SetEnergy(edep);
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    newHit->SetTrackEnergy(aStep->GetPreStepPoint()->GetTotalEnergy()); //M. Raggi 2/04/2019
    newHit->SetPDir(aStep->GetTrack()->GetMomentumDirection());  //this is post step we want the exit direction
    newHit->SetPosition(worldPosPre);
    newHit->SetLocalPosition(localPosPre);
    fBeWCollection->insert(newHit);
  }
  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeWSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int NbHits = fBeWCollection->entries();
    G4cout << "\n-- BeW Hits Collection: " << NbHits << " hits --" << G4endl;
    for (G4int i=0;i<NbHits;i++) (*fBeWCollection)[i]->Print();
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
