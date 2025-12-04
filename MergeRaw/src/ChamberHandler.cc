#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>

//#include "TFile.h"
//#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TError.h"
#include "TSystem.h"

#include "Configuration.hh"

#include "ChamberHandler.hh"

ChamberHandler::ChamberHandler()
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Disable WARNING reporting from ROOT
  // Can be set to kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal
  gErrorIgnoreLevel = kError; // Will only report kError, kBreak, kSysError, kFatal

}

ChamberHandler::~ChamberHandler()
{
  if (!fTChain) return;
  //delete fTChain->GetCurrentFile();
  delete fTChain;
}

Int_t ChamberHandler::Initialize()
{

  if (fConfig->Verbose()) printf("ChamberHandler::Initialize - Start initialization\n");

  // Initialize global event counter
  fCurrentEvent = 0;

  // Create TChain and add all files for this run
  fTChain = new TChain("apv_raw");
  fTotalFiles = 0;
  while(true) {

    // Get full path to next file
    TString nextFile = FormatFilename(fTotalFiles);
    
    // Check if file exists
    if (! FileExists(nextFile)) {
      if (fTotalFiles == 0) {
	// No files added to the chain: error
	printf("ChamberHandler::Initialize - ERROR - No files found\n");
	return -1;
      }
      // Next file does not exists: chain is complete
      break;
    }

    // Add file to the chain
    if (fConfig->Verbose()) printf("ChamberHandler::Initialize - Adding file %s to chain\n",nextFile.Data());
    fTChain->Add(nextFile);
    
    // Increase file counter
    fTotalFiles++;

  }

  //fTotalFiles = 1;
  //TString nextFile = FormatFilename(0);
  //if (fConfig->Verbose()) printf("ChamberHandler::Initialize - Adding file %s to chain\n",nextFile.Data());
  //TFile* f = TFile::Open(nextFile.Data(),"READ");
  //TTree* tree;
  //f->GetObject("apv_raw",tree);
  //if (!tree) {
  //  printf("ChamberHandler::Initialize - ERROR - Tree not initialized\n");
  //  return -1;
  //}
  //fTChain = tree;

  // Report about chain dimensions
  fTotalEvents = fTChain->GetEntries();
  if (fConfig->Verbose()) printf("ChamberHandler::Initialize - Chain completed with %d files and %lld events\n",fTotalFiles,fTotalEvents);

  // Initialize vector pointers to zero
  srsFec = 0;
  srsChip = 0;
  srsChan = 0;
  mmChamber = 0;
  mmLayer = 0;
  mmReadout = 0;
  mmStrip = 0;
  raw_q = 0;
  max_q = 0;
  t_max_q = 0;
  
  // Initialize branches associated to the chain
  fTChain->SetMakeClass(1);
  fTChain->SetBranchAddress("evt",             &evt,             &b_evt);
  fTChain->SetBranchAddress("error",           &error,           &b_error);
  fTChain->SetBranchAddress("daqTimeSec",      &daqTimeSec,      &b_daqTimeSec);
  fTChain->SetBranchAddress("daqTimeMicroSec", &daqTimeMicroSec, &b_daqTimeMicroSec);
  fTChain->SetBranchAddress("srsTimeStamp",    &srsTimeStamp,    &b_srsTimeStamp);
  fTChain->SetBranchAddress("srsTrigger",      &srsTrigger,      &b_srsTrigger);
  fTChain->SetBranchAddress("srsFec",          &srsFec,          &b_srsFec);
  fTChain->SetBranchAddress("srsChip",         &srsChip,         &b_srsChip);
  fTChain->SetBranchAddress("srsChan",         &srsChan,         &b_srsChan);
  fTChain->SetBranchAddress("mmChamber",       &mmChamber,       &b_mmChamber);
  fTChain->SetBranchAddress("mmLayer",         &mmLayer,         &b_mmLayer);
  fTChain->SetBranchAddress("mmReadout",       &mmReadout,       &b_mmReadout);
  fTChain->SetBranchAddress("mmStrip",         &mmStrip,         &b_mmStrip);
  fTChain->SetBranchAddress("raw_q",           &raw_q,           &b_raw_q);
  fTChain->SetBranchAddress("max_q",           &max_q,           &b_max_q);
  fTChain->SetBranchAddress("t_max_q",         &t_max_q,         &b_t_max_q);

  // Always start reading the chain from first event
  fCurrentEvent = -1;
  
  return 0;

}

Int_t ChamberHandler::Finalize()
{
  if (fConfig->Verbose()) printf("ChamberHandler::Finalize - Finalizing.\n");
  return 0;
}

Int_t ChamberHandler::NextEvent()
{

  fCurrentEvent++;

  if (fCurrentEvent >= fTotalEvents) {
    if (fConfig->Verbose()) printf("ChamberHandler::NextEvent - reached end of chain at event %lld: exiting.\n",fCurrentEvent);
    return 0;
  }

  // Read next event
  //printf("Reading event %lld\n",fCurrentEvent);
  if (!fTChain) {
    if (fConfig->Verbose()) printf("ChamberHandler::NextEvent - ERROR - Chain pointer is zero: exiting.\n");
    return 0;
  }
  fTChain->LoadTree(fCurrentEvent);  
  fTChain->GetEntry(fCurrentEvent);
  
  return 1;

}

TString ChamberHandler::FormatFilename(UInt_t filenr)
{
  if (filenr == 0) {
    return TString::Format("%s/%s.root",
			 fConfig->ChamberDataDirectory().Data(),
			 fConfig->ChamberRunName().Data());
  } else {
    return TString::Format("%s/%s_%d.root",
			   fConfig->ChamberDataDirectory().Data(),
			   fConfig->ChamberRunName().Data(),
			   filenr);
  }
}

Bool_t ChamberHandler::FileExists(TString fileName)
{

  if (fConfig->Verbose() > 2) printf("ChamberHandler::FileExists - Testing file %s\n",fileName.Data());

  Bool_t exists = true;
  if (fileName.BeginsWith("root:")) {

    // Remote file: try to open it and check if this succeeds
    Int_t oldErrorIgnoreLevel = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kBreak; // Do not report "open file" errors
    std::unique_ptr<TFile> testTF(TFile::Open(fileName.Data(),"READ"));
    if (testTF == 0) exists = false;
    gErrorIgnoreLevel = oldErrorIgnoreLevel; // Go back to previous error-reporting behaviour

  } else {

    // Local file: use standard 'stat' call
    struct stat filestat;
    if ( stat(Form(fileName.Data()),&filestat) != 0 ) exists = false;

  }

  if (fConfig->Verbose() > 2) {
    if (exists) {
      printf("ChamberHandler::FileExists - File %s exists\n",fileName.Data());
    } else {
      printf("ChamberHandler::FileExists - File %s does not exist\n",fileName.Data());
    }
  }

  return exists;

}
