#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TApplication.h"

#include "HistoManager.hh"
#include "Grapher.hh"

int main(int argc, char* argv[])
{

  int c;
  int verbose = 0;

  UInt_t nevents=0;
  TString infile = "rawdata.root";

  // Parse options
  while ((c = getopt (argc, argv, "i:n:v:h")) != -1) {
    switch (c)
      {
      case 'i':
        infile = optarg;
        fprintf(stdout,"Set input data file to '%s'\n",infile.Data());
        break;
      case 'n':
        if ( sscanf(optarg,"%d",&nevents) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        //if (nevents<0) {
        //  fprintf (stderr, "Error while processing option '-n'. Required %d events (must be >=0).\n", nevents);
        //  exit(1);
        //}
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

  // Define the ROOT application to use
  TApplication* app = new TApplication("App", &argc, argv);

  // Initilize histograms
  HistoManager::GetInstance();

  // Initialize graphics
  Grapher* grapher = Grapher::GetInstance();
  grapher->SetApplication(app);

  // Fill histograms for this slice
  grapher->SetFileName(infile);
  grapher->SetNEvents(nevents);
  //grapher->AnalyzeEvents();
  //grapher->Initialize();

  // Run application
  //app->Run(kTRUE);
  //app->Terminate(0);
  app->Run(kFALSE);
  // Pass kFALSE if you want application to terminate by itself.
  // Then you just need "return 0;" below (to avoid compiler warnings).

  return 0;

}
