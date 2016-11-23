// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-22
//
// --------------------------------------------------------------

#include "SACDigi.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<SACDigi> SACDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SACDigi::SACDigi()
{

  // Initialize digi
  fChannelId = -1;
  fEnergy = 0.;
  fTime = 1.E9;

  // Initialize histo limits to some default values
  fEHistoStart = 12.; // Start at 12ns, i.e. 3.7m from target
  fEHistoStep  = 0.1; // 100ps x 500bins = 50ns, i.e. >1 bunch length
  ResetEHisto();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SACDigi::~SACDigi() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SACDigi::SACDigi(const SACDigi& right)
  : G4VDigi()
{

  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;

  fEHistoStart = right.fEHistoStart;
  fEHistoStep = right.fEHistoStep;
  for (G4int i=0;i<SACDIGI_N_BINS;i++) fEHisto[i] = right.fEHisto[i];

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const SACDigi& SACDigi::operator=(const SACDigi& right)
{

  fChannelId = right.fChannelId;
  fTime = right.fTime;
  fEnergy = right.fEnergy;

  fEHistoStart = right.fEHistoStart;
  fEHistoStep = right.fEHistoStep;
  for (G4int i=0;i<SACDIGI_N_BINS;i++) fEHisto[i] = right.fEHisto[i];

  return *this;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int SACDigi::operator==(const SACDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SACDigi::Draw()
{
  G4VVisManager* visManager = G4VVisManager::GetConcreteInstance();
  if(visManager)
  {
    //G4Circle circle(fPosition);
    //circle.SetScreenSize(2.);
    //circle.SetFillStyle(G4Circle::filled);
    //G4Colour colour(0.,1.,0.);
    //G4VisAttributes attribs(colour);
    //circle.SetVisAttributes(attribs);
    //visManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SACDigi::Print()
{
  G4cout << "- channel: " << fChannelId
	 << " time: " << G4BestUnit(fTime,"Time")
         << " energy: " << G4BestUnit(fEnergy,"Energy")
	 << G4endl;
  PrintEHisto();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SACDigi::ResetEHisto()
{
  for(G4int i=0;i<SACDIGI_N_BINS;i++) fEHisto[i] = 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SACDigi::PrintEHisto()
{
  std::cout << "NBins " << GetEHistoNBins() << " Start " << GetEHistoStart() << " End " << GetEHistoEnd() << " Step " << GetEHistoStep() << std::endl;
  for(G4int i=0;i<SACDIGI_N_BINS;i++)
    printf("%3.3d %9.3f\n",i,fEHisto[i]);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SACDigi::AddEnergyAtTime(G4double e, G4double t)
{
  // Bin 0 and SACDIGI_N_BINS-1 (399) act as under- and over-flow counters
  G4int nbin = std::max(0,std::min(SACDIGI_N_BINS-1,(int)((t-fEHistoStart)/fEHistoStep)));
  fEHisto[nbin] += e;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double SACDigi::GetEnergyAtTime(G4double t)
{
  G4int nbin = std::max(0,std::min(SACDIGI_N_BINS-1,(int)((t-fEHistoStart)/fEHistoStep)));
  return fEHisto[nbin];
}
