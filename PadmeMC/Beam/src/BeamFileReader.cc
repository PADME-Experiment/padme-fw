// BeamFileReader.cc
// --------------------------------------------------------------
// See BeamFileReader.hh for description.
// --------------------------------------------------------------

#include "BeamFileReader.hh"
#include "BeamParameters.hh"

#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include "TFile.h"
#include "TTree.h"

#include <cstdlib>

BeamFileReader* BeamFileReader::fInstance = 0;

BeamFileReader* BeamFileReader::GetInstance()
{
  if (fInstance == 0) fInstance = new BeamFileReader();
  return fInstance;
}

BeamFileReader::BeamFileReader()
 : fLoaded(false), fPOTLoaded(false), fIndex(0), fNRecycles(0)
{}

BeamFileReader::~BeamFileReader()
{
  if (fPOTLoaded) delete fPOTProfile;
}

void BeamFileReader::LoadFile()
{
  BeamParameters* bpar = BeamParameters::GetInstance();

  G4String fileName = bpar->GetBeamInputFilename();
  G4String treeName = bpar->GetBeamInputTreeName();
  G4int    flag     = bpar->GetBeamInputFlag();

  if (fileName == "") {
    G4cerr << "BeamFileReader - ERROR - /beam/input_file_on is true but no file given with /beam/input_file" << G4endl;
    exit(1);
  }

  TFile* f = TFile::Open(fileName.c_str(),"READ");
  if (!f || f->IsZombie()) {
    G4cerr << "BeamFileReader - ERROR - cannot open file " << fileName << G4endl;
    exit(1);
  }
  TTree* t = (TTree*)f->Get(treeName.c_str());
  if (!t) {
    G4cerr << "BeamFileReader - ERROR - no TTree " << treeName << " in " << fileName << G4endl;
    exit(1);
  }

  Int_t    nFlag;
  Double_t p,px,py,pz,x,y,z;
  t->SetBranchStatus("*",0);
  const char* br[] = {"NFlag","p","PX","PY","PZ","x","y","z"};
  for (auto b : br) t->SetBranchStatus(b,1);
  t->SetBranchAddress("NFlag",&nFlag);
  t->SetBranchAddress("p",&p);
  t->SetBranchAddress("PX",&px);
  t->SetBranchAddress("PY",&py);
  t->SetBranchAddress("PZ",&pz);
  t->SetBranchAddress("x",&x);
  t->SetBranchAddress("y",&y);
  t->SetBranchAddress("z",&z);

  Long64_t nEnt = t->GetEntries();
  fPos.clear(); fMom.clear();
  Long64_t nBad = 0;
  for (Long64_t i=0; i<nEnt; i++) {
    t->GetEntry(i);
    if (nFlag != flag) continue;
    // Consistency check between p and |(PX,PY,PZ)| (1e-3 relative)
    G4double pmag = sqrt(px*px+py*py+pz*pz);
    if (p > 0. && fabs(pmag-p)/p > 1.e-3) nBad++;
    // z is the (constant) position of the scoring plane in the file frame:
    // the particle is placed on the plane through /beam/position_z
    fPos.push_back(G4ThreeVector(x*mm,y*mm,0.));
    fMom.push_back(G4ThreeVector(px*MeV,py*MeV,pz*MeV));
  }
  f->Close();
  delete f;

  if (fPos.empty()) {
    G4cerr << "BeamFileReader - ERROR - no entries with NFlag==" << flag << " in " << fileName << G4endl;
    exit(1);
  }

  fIndex = (size_t)bpar->GetBeamInputFirstEntry() % fPos.size();
  fLoaded = true;

  G4cout << "BeamFileReader - Loaded " << fPos.size() << " particles with NFlag==" << flag
         << " from " << fileName << ":" << treeName
         << " (|P| vs p mismatches: " << nBad << "), start index " << fIndex << G4endl;
}

void BeamFileReader::LoadPOTFile()
{
  BeamParameters* bpar = BeamParameters::GetInstance();

  G4String fileName = bpar->GetIntensityProfileFilename();
  G4int period = bpar->GetIntensityProfilePeriod();
  G4String histoName = Form("period_%d_nPoT",period);

  if (fileName == "") {
    G4cerr << "BeamFileReader - ERROR - /beam/intensity_profile_on is true but no file given with /beam/intensity_profile_file" << G4endl;
    exit(1);
  }

  if (period < 1 || period > 36) {
    G4cerr << "BeamFileReader - ERROR - /beam/intensity_profile_on is true but no/wrong period given with /beam/intensity_profile_period : " << period << G4endl;
    exit(1);
  }
  
  TFile* f = TFile::Open(fileName.c_str(),"READ");
  if (!f || f->IsZombie()) {
    G4cerr << "BeamFileReader - ERROR - cannot open intensity profile file " << fileName << G4endl;
    exit(1);
  }

  TH1D* h = (TH1D*)f->Get(histoName.c_str());
  if (!h) {
    G4cerr << "BeamFileReader - ERROR - no TH1D in intensity profile file " << histoName << " in " << fileName << G4endl;
    exit(1);
  }
  
  if (h->GetSum() == 0) {
    G4cerr << "BeamFileReader - ERROR - no entries in intensity profile histogram " << histoName << " in " << fileName << G4endl;
    exit(1);
  }
  h->SetDirectory(nullptr);
  f->Close();
  delete f;

  fPOTProfile = h;
  fPOTLoaded = true;

  G4cout << "BeamFileReader - POT Loaded " << fPOTProfile->GetMean() << " average POTs from " << fileName << ":" << histoName << " period " << period << G4endl;
}

TH1D* BeamFileReader::GetPOTProfileHistogram(){
  if (!fPOTLoaded) {
    G4cout << "BeamFileReader Loading POT file " << G4endl;
    LoadPOTFile();
  }
  return fPOTProfile;
}


void BeamFileReader::GetNextParticle(BeamFileParticle& part)
{
  if (!fLoaded) LoadFile();

  if (fIndex >= fPos.size()) {
    fIndex = 0;
    fNRecycles++;
    G4cout << "BeamFileReader - End of sample reached, restarting from first particle (recycle #"
           << fNRecycles << ")" << G4endl;
  }

  part.pos = fPos[fIndex];
  part.mom = fMom[fIndex];
  fIndex++;
}

