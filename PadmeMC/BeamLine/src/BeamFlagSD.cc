#include "BeamFlagSD.hh"

#include "G4UnitsTable.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamFlagSD::BeamFlagSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="BeamFlagCollection");
}

//....Ooooo0ooooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BeamFlagSD::~BeamFlagSD(){ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeamFlagSD::Initialize(G4HCofThisEvent* HCE)
{
  fBeamFlagCollection = new BeamFlagHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fBeamFlagCollection); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool BeamFlagSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{

  G4double edep = aStep->GetTotalEnergyDeposit();
  //  if (edep==0.) return false;

  BeamFlagHit* newHit = new BeamFlagHit();

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);


//  << " global " << G4BestUnit(worldPosPre,"Length")<< G4endl;
//  << " local " << G4BestUnit(localPosPre,"Length") << G4endl;
  
//  G4cout<<"NAME "<<touchHPre->GetVolume()->GetName()<<G4endl;

  // this way you only see primary particle there is a component coming form background that is killed here.
  if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary && aStep->GetTrack()->GetParentID()==0){
  //  if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary){
    newHit->SetEnergy(edep);
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
    newHit->SetTrackEnergy(aStep->GetPreStepPoint()->GetTotalEnergy()); //M. Raggi 2/04/2019
    newHit->SetPDir(aStep->GetTrack()->GetMomentumDirection());  //this is post step we want the exit direction
    newHit->SetPosition(worldPosPre);
    
    if(touchHPre->GetVolume()->GetName()=="BeamLineBeamFlag1")  newHit->SetChannelId(1); 
    if(touchHPre->GetVolume()->GetName()=="BeamLineBeamFlag2")  newHit->SetChannelId(2); 
    if(touchHPre->GetVolume()->GetName()=="BeamLineBeamFlag3")  newHit->SetChannelId(3); 
    if(touchHPre->GetVolume()->GetName()=="BeamLineBeamFlag4")  newHit->SetChannelId(4); 
    if(touchHPre->GetVolume()->GetName()=="BeamLineBeamFlag5")  newHit->SetChannelId(5); 
    newHit->SetLocalPosition(localPosPre);
    fBeamFlagCollection->insert(newHit);
  }
  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BeamFlagSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int NbHits = fBeamFlagCollection->entries();
    G4cout << "\n-- BeamFlag Hits Collection: " << NbHits << " hits --" << G4endl;
    for (G4int i=0;i<NbHits;i++) (*fBeamFlagCollection)[i]->Print();
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
