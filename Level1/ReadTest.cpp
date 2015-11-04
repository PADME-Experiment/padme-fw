#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "TRawEvent.hh"

int main(int argc, char* argv[])
{
  
  int c;
  std::string infile = "rawdata.root";
  int verbose = 0;
  int nevents = 0;

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

  TFile* fRawEv = new TFile(infile.c_str());
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
    printf("Event %d read\n",iev);

    // Show event header
    UChar_t nBoards = rawEv->GetNADCBoards();
    printf("Event %d Run nr %d Event nr %d ADC boards %d\n",
	   iev,rawEv->GetRunNumber(),rawEv->GetEventNumber(),nBoards);

    // Loop over boards
    for(UChar_t b=0;b<nBoards;b++){

      // Show board info
      TADCBoard* adcB = rawEv->ADCBoard(b);
      UChar_t nTrg = adcB->GetNADCTriggers();
      UChar_t nChn = adcB->GetNADCChannels();
      printf("\tBoard %u Board Id %u LVDS %u Status %u GMsk 0x%1x EvtCnt %u Time %u ActMsk 0x%08x AccMsk 0x%08x #Trg %u #Chn %u\n",
	     b,adcB->GetBoardId(),adcB->GetLVDSPattern(),adcB->GetStatus(),adcB->GetGroupMask(),adcB->GetEventCounter(),
	     adcB->GetEventTimeTag(),adcB->GetActiveChannelMask(),adcB->GetAcceptedChannelMask(),nTrg,nChn);

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

    // Clear event
    rawEv->Clear("C");

  }

  delete rawEv;
  delete bRawEv;
  delete tRawEv;

  fRawEv->Close();
  delete fRawEv;

  exit(0);

}
