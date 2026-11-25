#include "MylarWSD.hh"

#include "G4UnitsTable.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MylarWSD::MylarWSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="MylarWCollection");
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MylarWSD::~MylarWSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MylarWSD::Initialize(G4HCofThisEvent* HCE)
{
  fMylarWCollection = new MylarWHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fMylarWCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MylarWSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{

  // The SD is attached to the vacuum volume containing the window: edep is ~0 there,
  // so do not cut on it (same as BeamFlagSD). M. Raggi 20/11/2026
  G4double edep = aStep->GetTotalEnergyDeposit();

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);

  //G4cout << "PreStepPoint in MylarW " << touchHPre->GetVolume()->GetName()<<std::endl;
  //	 << " global " << G4MylarstUnit(worldPosPre,"Length")
  //	 << " local " << G4MylarstUnit(localPosPre,"Length") << G4endl;

  // this way you only see primary particle there is a component coming form background that is killed here.
  // Keep only the crossing just downstream of the Mylar foil (local z>0): the entry face of the
  // container volume (local z=-FlangeThick/2) would give a second hit upstream of the window.
  if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary && aStep->GetTrack()->GetParentID()==0
     && localPosPre.z()>0.){
    //if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary){
    MylarWHit* newHit = new MylarWHit();
    newHit->SetEnergy(edep);
    newHit->SetTrackId(aStep->GetTrack()->GetTrackID()); // MR 20/11/2026
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    newHit->SetTrackEnergy(aStep->GetPreStepPoint()->GetTotalEnergy()); //M. Raggi 2/04/2019
    newHit->SetPDir(aStep->GetTrack()->GetMomentumDirection());  //this is post step we want the exit direction
    newHit->SetPosition(worldPosPre);
    newHit->SetLocalPosition(localPosPre);
    fMylarWCollection->insert(newHit);
  }
  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MylarWSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int NbHits = fMylarWCollection->entries();
    G4cout << "\n-- MylarW Hits Collection: " << NbHits << " hits --" << G4endl;
    for (G4int i=0;i<NbHits;i++) (*fMylarWCollection)[i]->Print();
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
