#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <sys/stat.h>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"

#include "TRawEvent.hh"

int main(int argc, char* argv[])
{
  
  int c;
  int verbose = 0;
  int nevents = 0;

  TString inputFileName;
  TObjArray inputFileNameList;
  struct stat filestat;

  // Parse options
  while ((c = getopt (argc, argv, "i:l:n:v:h")) != -1) {
    switch (c)
      {
      case 'i':
        inputFileNameList.Add(new TObjString(optarg));
        fprintf(stdout,"Added input data file '%s'\n",optarg);
	break;
      case 'l':
	if ( stat(Form(optarg),&filestat) == 0 ) {
	  fprintf(stdout,"Reading list of input files from '%s'\n",optarg);
	  std::ifstream inputList(optarg);
	  while( inputFileName.ReadLine(inputList) ){
	    if ( stat(Form(inputFileName.Data()),&filestat) == 0 ) {
	      inputFileNameList.Add(new TObjString(inputFileName.Data()));
	      fprintf(stdout,"Added input data file '%s'\n",inputFileName.Data());
	    } else {
	      fprintf(stdout,"WARNING: file '%s' is not accessible\n",inputFileName.Data());
	    }
	  }
	} else {
	  fprintf(stdout,"WARNING: file list '%s' is not accessible\n",optarg);
	}
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

  if ( inputFileNameList.GetEntries() == 0 ) {
    perror(Form("ERROR No Input File specified"));
    exit(1);
  }

  // Create chain of input files
  fprintf(stdout,"=== === === Chain of input files === === ===\n");
  TChain* inputChain = new TChain("RawEvents");
  for (Int_t iFile = 0; iFile < inputFileNameList.GetEntries(); iFile++) {
    fprintf(stdout,"%4d %s\n",iFile,((TObjString*)inputFileNameList.At(iFile))->GetString().Data());
    inputChain->AddFile(((TObjString*)inputFileNameList.At(iFile))->GetString());
  }
  if (inputChain->GetEntries() == 0) {
    perror(Form("ERROR No events found for tree 'RawEvents' in input chain"));
    exit(1);
  }

  // Get some info about the input chain
  Int_t runNEntries = inputChain->GetEntries();
  std::cout << "Found Tree 'RawEvents' with " << runNEntries << " entries" << std::endl;
  TRawEvent* rawEv = new TRawEvent();
  inputChain->SetBranchAddress("RawEvent",&rawEv);

  // Set number of events to read
  Int_t ntoread = runNEntries;
  if (nevents && nevents<runNEntries) {
    ntoread = nevents;
    printf("Reading first %d events\n",ntoread);
  }

  for(Int_t iev=0;iev<ntoread;iev++){

    // Read event
    //printf("Reading event %d\n",iev);
    inputChain->GetEntry(iev);
    //printf("Event %d read\n",iev);

    // Show event header
    UChar_t nBoards = rawEv->GetNADCBoards();
    printf("Event %d Run nr %d Event nr %d ADC boards %d\n",
	   iev,rawEv->GetRunNumber(),rawEv->GetEventNumber(),nBoards);

    if (verbose>0) {

      // Loop over boards
      for(UChar_t b=0;b<nBoards;b++){

	// Show board info
	TADCBoard* adcB = rawEv->ADCBoard(b);
	UChar_t nTrg = adcB->GetNADCTriggers();
	UChar_t nChn = adcB->GetNADCChannels();
	printf("\tBoard %u Board Id %u LVDS %u Status %u GMsk 0x%1x EvtCnt %u Time %u ActMsk 0x%08x AccMsk 0x%08x #Trg %u #Chn %u\n",
	       b,adcB->GetBoardId(),adcB->GetLVDSPattern(),adcB->GetStatus(),adcB->GetGroupMask(),adcB->GetEventCounter(),
	       adcB->GetEventTimeTag(),adcB->GetActiveChannelMask(),adcB->GetAcceptedChannelMask(),nTrg,nChn);

	if (verbose>1) {

	  // Loop over triggers
	  for(UChar_t t=0;t<nTrg;t++){
	    TADCTrigger* trg = adcB->ADCTrigger(t);
	    printf("\t\tTrig %u Grp %u SIC %u Freq %u Sign %u Time %u\n",
		   t,trg->GetGroupNumber(),trg->GetStartIndexCell(),trg->GetFrequency(),trg->GetTriggerSignal(),
		   trg->GetTriggerTimeTag());
	    for(UShort_t s=0;s<trg->GetNSamples();s++){
	      if ( (s%40)==0 ) printf("\t\t\t");
	      printf(" %04u",trg->GetSample(s));
	      if ( (s%40)==39 ) printf("\n");
	    }
	    printf("\n");
	  }

	  // Loop over channels
	  for(UChar_t c=0;c<nChn;c++){
	    TADCChannel* chn = adcB->ADCChannel(c);
	    printf("\t\tChan %u Chn# %u\n",c,chn->GetChannelNumber());
	    for(UShort_t s=0;s<chn->GetNSamples();s++){
	      if ( (s%40)==0 ) printf("\t\t\t");
	      printf(" %04u",chn->GetSample(s));
	      if ( (s%40)==39 ) printf("\n");
	    }
	    printf("\n");
	  }

	}

      } // End loop over boards
      
    }

    // Clear event
    rawEv->Clear("C");
    
  } // End loop over events

  delete rawEv;

  exit(0);

}
