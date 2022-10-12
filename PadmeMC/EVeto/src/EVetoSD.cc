// EVetoSD.cc
// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2105-12-14
// --------------------------------------------------------------

#include "EVetoSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

EVetoSD::EVetoSD(G4String name)
:G4VSensitiveDetector(name)
{
  G4String HCname;
  collectionName.insert(HCname="EVetoCollection"); //crea il collection name
}

EVetoSD::~EVetoSD(){}

void EVetoSD::Initialize(G4HCofThisEvent* HCE)
{
  fEVetoCollection = new EVetoHitsCollection(SensitiveDetectorName,collectionName[0]); 
  static G4int HCID = -1;
  if (HCID<0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  HCE->AddHitsCollection(HCID,fEVetoCollection); 
}

G4bool EVetoSD::ProcessHits(G4Step* aStep,G4TouchableHistory*)
{

  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  G4TouchableHandle touchHPre = aStep->GetPreStepPoint()->GetTouchableHandle();

  EVetoHit* newHit = new EVetoHit();

  newHit->SetChannelId(touchHPre->GetCopyNumber());
  newHit->SetEnergy(edep);

  G4ThreeVector worldPosPre = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector localPosPre = touchHPre->GetHistory()->GetTopTransform().TransformPoint(worldPosPre);

  //Beth 12/10/22:
  //distance from end in y = distance from sipm
  //distance = length/2 - localpospre.y()
  double FingerSizeY=179;//mm
  double SiPMPos = -1*FingerSizeY/2;//SiPM is at bottom of bar
  double HitSiPMDistance = localPosPre.y()-SiPMPos;

  //speed of light in 1.5% POPUP = 1/(6ns/m) = 15cm/ns (trovato da Tommaso...non sono sicura dove?)
  double speed = 150; //mm/ns
  double PathTime = HitSiPMDistance/speed;

  //make sure that the hit is within the length of the finger
  if(0<=HitSiPMDistance && HitSiPMDistance<=FingerSizeY){
    if(HitSiPMDistance==0){
      PathTime = 0;
      std::cout<<"localPosPre.y() "<<localPosPre.y()<<" HitSiPMDistance "<<HitSiPMDistance<<" setting PathTime to 0"<<std::endl;
    }
    else if(HitSiPMDistance==FingerSizeY){
      PathTime = FingerSizeY/speed;
      std::cout<<"localPosPre.y() "<<localPosPre.y()<<" HitSiPMDistance "<<HitSiPMDistance<<" setting PathTime to FingerSizeY/speed = "<<FingerSizeY/speed<<std::endl;
    }
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime()+PathTime);
  }
  else{
    std::cout<<"Hit outside PVetoVolume, not using PathTime"<<std::endl;
    std::cout<<"localPosPre.y() "<<localPosPre.y()<<" HitSiPMDistance "<<HitSiPMDistance<<std::endl;
    newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());
  }


  newHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime());

  //G4cout << "PreStepPoint in " << touchHPre->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPre,"Length")
  //	 << " local " << G4BestUnit(localPosPre,"Length") << G4endl;

  //G4TouchableHandle touchHPost = aStep->GetPostStepPoint()->GetTouchableHandle();
  //G4ThreeVector worldPosPost = aStep->GetPostStepPoint()->GetPosition();
  //G4ThreeVector localPosPost = touchHPost->GetHistory()->GetTopTransform().TransformPoint(worldPosPost);
  //G4cout << "PostStepPoint in " << touchHPost->GetVolume()->GetName()
  //	 << " global " << G4BestUnit(worldPosPost,"Length")
  //	 << " local " << G4BestUnit(localPosPost,"Length") << G4endl;

  newHit->SetPosition(worldPosPre);
  newHit->SetLocalPosition(localPosPre);

  newHit -> SetTrackEnergy( aStep->GetPreStepPoint()->GetTotalEnergy());
  newHit -> SetTrackID(aStep->GetTrack()->GetTrackID());

  //record track only the first time it enters the volume M. Raggi 30/05/2018
  //it is not immune from tracks spinning around and entering a volume twice
  if(aStep->GetPreStepPoint()->GetStepStatus()==fGeomBoundary){
    newHit -> SetTrackID(aStep->GetTrack()->GetTrackID());
  }else{
    newHit -> SetTrackID(0);
  }

  fEVetoCollection->insert(newHit);
  return true;

}

void EVetoSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verboseLevel>0) { 
    G4int NbHits = fEVetoCollection->entries();
    G4cout << "\n-- EVeto Hits Collection: " << NbHits << " hits --" << G4endl;
    for (G4int i=0;i<NbHits;i++) (*fEVetoCollection)[i]->Print();
  } 
}
