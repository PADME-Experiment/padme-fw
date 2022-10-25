#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "TFile.h"
#include "TTree.h"
#include "TError.h"

#define SIZE_THRESHOLD 10000

int main(int argc, char* argv[])
{

  int c;

  TString inputFileName = "";

  // Parse options
  while ((c = getopt(argc, argv, "i:h")) != -1) {
    switch (c)
      {
      case 'i':
	inputFileName = optarg;
	break;
      case 'h':
        fprintf(stdout,"\nOnlineRecoMonitor -i inputfile [-o outdir] [-s stopfile] [-c configfile] [-d debug] [-v] [-h]\n\n");
        fprintf(stdout,"  -i: input rawdata file\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(EXIT_SUCCESS);
      case '?':
	if (optopt == 'i')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        exit(EXIT_FAILURE);
      default:
        abort();
      }

  }

  // Check if input file was defined
  if (inputFileName.IsNull()) {
    fprintf (stderr,"ERROR - No input file defined with -i option.\n");
    exit(EXIT_FAILURE);
  }

  // Check if input file exists
  struct stat buffer;
  if (stat(inputFileName.Data(),&buffer) != 0) {
    fprintf (stderr,"ERROR - Input file %s does not exist.\n",inputFileName.Data());
    exit(EXIT_FAILURE);
  }
  Int_t fileSize = buffer.st_size;

  Int_t nEvents = 0;
  if (fileSize > SIZE_THRESHOLD) { // Check if file is large enough to contain at least one event
    gErrorIgnoreLevel = kBreak; // Do not report "open file" errors
    TFile* inputFile = new TFile(inputFileName,"READ"); // Open TFile
    if (! inputFile->IsZombie()) { // Check if root structure is good
      // File is good: get number of events in RawEvents tree
      TTree* tree;
      gDirectory->GetObject("RawEvents",tree);
      if (tree) nEvents = tree->GetEntries();
    }
  }
  printf("%d\n",nEvents);

}
