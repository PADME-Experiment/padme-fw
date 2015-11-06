#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TApplication.h"

#include "TRawEvent.hh"

#include "HistoManager.hh"
#include "Grapher.hh"
#include "ECALHisto.hh"
#include "ECALAnal.hh"

void AnalyzeEvents(std::string name, int nevents){

  // Prepare the analyses
  ECALAnal* ecalA = new ECALAnal();

  // Connect to raw events file
  TFile* fRawEv = new TFile(name.c_str());
  TTree* tRawEv = (TTree*)fRawEv->Get("RawEvents");
  TBranch* bRawEv = tRawEv->GetBranch("RawEvent");
  TRawEvent* rawEv = new TRawEvent();
  bRawEv->SetAddress(&rawEv);

  Int_t nevt = tRawEv->GetEntries();
  printf("TTree RawEvents contains %d events\n",nevt);

  // Set number of events to read
  Int_t ntoread = nevt;
  if (nevents && nevents<nevt) ntoread = nevents;
  printf("Reading the first %d events\n",ntoread);

  for(Int_t iev=0;iev<ntoread;iev++){

    // Read event
    printf("Reading event %d\n",iev);
    bRawEv->GetEntry(iev);

    // Do all analyses
    ecalA->SetEvent(rawEv);
    ecalA->AnalyzeCharge();
    ecalA->AnalyzePosition();

    // Clear event
    rawEv->Clear("C");

  }

  // Final cleanup
  delete rawEv;
  delete bRawEv;
  delete tRawEv;
  fRawEv->Close();
  delete fRawEv;

}

int main(int argc, char* argv[])
{
  int nevents=0;
  int c;
  std::string infile = "rawdata.root";
  int verbose = 0;
  //Histo* his = Histo::GetInstance();
  // Parse options
  while ((c = getopt (argc, argv, "i:n:v:h")) != -1) {
    switch (c)
      {
      case 'i':
        infile = optarg;
        fprintf(stdout,"Set input data file to '%s'\n",infile.c_str());
        break;
      case 'n':
        if ( sscanf(optarg,"%d",&nevents) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (nevents<0) {
          fprintf (stderr, "Error while processing option '-n'. Required %d events (must be >=0).\n", nevents);
          exit(1);
        }
	if (nevents) {
	  fprintf(stdout,"Will read first %d events in file\n",nevents);
	} else {
	  fprintf(stdout,"Will read all events in file\n");
	}
        break;
      case 'v':
        if ( sscanf(optarg,"%d",&verbose) != 1 ) {
          fprintf (stderr, "Error while processing option '-v'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (verbose<0) {
          fprintf (stderr, "Error while processing option '-v'. Verbose level set to %d (must be >=0).\n", verbose);
          exit(1);
        }
        fprintf(stdout,"Set verbose level to %d\n",verbose);
        break;
      case 'h':
        fprintf(stdout,"\nReadTest [-i input root file] [-v verbosity] [-h]\n\n");
        fprintf(stdout,"  -i: define an input file in root format\n");
        fprintf(stdout,"  -n: define number of events to read from input file (0: all events)\n");
        fprintf(stdout,"  -v: define verbose level\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(0);
      case '?':
        if (optopt == 'v') {
          // verbose with no argument: just enable at minimal level
          verbose = 1;
          break;
        } else if (optopt == 'i' || optopt == 'l' || optopt == 'o')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        exit(1);
      default:
        abort();
      }
  }

  // Use TApplication if you don't need prompt.
  TApplication* app = new TApplication("App", &argc, argv);

  // Initilize histograms
  HistoManager::GetInstance();

  // Initialize graphics
  Grapher::GetInstance();

  // Fill histograms for this slice
  AnalyzeEvents(infile.c_str(),nevents);

  // Run application
  app->Run(kTRUE);
  // Pass kFALSE if you want application to terminate by itself.
  // Then you just need "return 0;" below (to avoid compiler warnings).

  app->Terminate(0);
  return 0;

}
