#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TH1S.h"

#include "ADCBoard.hh"

TH1S* GetHisto(TString name, TObjArray histos) {
  for (Int_t iH = 0; iH < histos.GetEntries(); iH++) {
    if ( ((TH1S*)histos.At(iH))->GetName() == name ) return (TH1S*)histos.At(iH);
  }
  return 0;
}

int main(int argc, char* argv[])
{
  
  int c;
  int verbose = 0;
  int nevents = 0;

  TString inputFileName;
  TObjArray inputFileNameList;
  struct stat filestat;

  TString outputFileName = "RawHisto.root";
  Int_t event;
  std::vector<UInt_t> events;
  Int_t board_id = -1;

  // Parse options
  while ((c = getopt (argc, argv, "i:l:o:n:e:b:vh")) != -1) {
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
      case 'o':
        outputFileName = optarg;
        fprintf(stdout,"Output histogram file set to '%s'\n",optarg);
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
      case 'e':
        if ( sscanf(optarg,"%d",&event) != 1 ) {
          fprintf (stderr, "Error while processing option '-e'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (event<0) {
          fprintf (stderr, "Error while processing option '-e'. Required event %d (must be >=0).\n", event);
          exit(1);
        }
	events.push_back(event);
        fprintf(stdout,"Added event %d to list\n",event);
        break;
      case 'b':
        if ( sscanf(optarg,"%d",&board_id) != 1 ) {
          fprintf (stderr, "Error while processing option '-b'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (board_id<0 || board_id>31) {
          fprintf (stderr, "Error while processing option '-b'. Required event %d (must be 0<=id< 32).\n", event);
          exit(1);
        }
        fprintf(stdout,"Creating histograms for board %d\n",board_id);
        break;
      case 'v':
	verbose++;
        break;
      case 'h':
        fprintf(stdout,"\nADCHisto -b <board_id> [-i <file>] [-l <list>] [-o <file>] [-n <n>] [-e <event>] [-v <level>] [-h]\n\n");
        fprintf(stdout,"  -b: define <board_id> of board contained in the input file (mandatory)\n");
        fprintf(stdout,"  -i: add a file to list of input files (can be repeated)\n");
        fprintf(stdout,"  -l: specify text file with list of input files (one file per line)\n");
        fprintf(stdout,"  -o: define the name of the output file (default: %s)\n",outputFileName.Data());
        fprintf(stdout,"  -n: total number of events to save to output file (default: 0 i.e. no limit)\n");
        fprintf(stdout,"  -e: add <event> to list of events to save to output file (can be repeated)\n");
        fprintf(stdout,"  -v: enable verbose output (repeat for more output)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        fprintf(stdout,"N.B. if no -e option is specified, all events will be saved to the output file\n\n");
        exit(0);
      case '?':
	if (optopt == 'i' || optopt == 'l' || optopt == 'o' || optopt == 'n' || optopt == 'e' || optopt == 'b')
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

  if (board_id == -1) {
    fprintf(stderr,"ERROR - Board id was not specified\n");
    exit(1);
  }
  printf("Reading data for board %d\n",board_id);

  if ( inputFileNameList.GetEntries() == 0 ) {
    fprintf(stderr,"ERROR - Input file list is empty\n");
    exit(1);
  }

  if (verbose) fprintf(stdout,"Set verbose level to %d\n",verbose);

  // Create histograms to use (4 triggers and 32 channels)
  TObjArray hList(0);
  TH1S* h;
  for(UChar_t t=0;t<4;t++){
    h = new TH1S(Form("T%d",t),"",1024,-0.5,1023.5);
    hList.Add(h);
  }
  for(UChar_t c=0;c<32;c++){
    h = new TH1S(Form("C%02d",c),"",1024,-0.5,1023.5);
    hList.Add(h);
  }

  // Create output file for histograms
  TFile* histoFile = new TFile(outputFileName,"RECREATE");
  if(!histoFile) {
    fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
    exit(1);
  }

  // Create board structure
  ADCBoard* board = new ADCBoard(board_id);

  // Add input files to board
  fprintf(stdout,"=== === === Chain of input files === === ===\n");
  for (Int_t iFile = 0; iFile < inputFileNameList.GetEntries(); iFile++) {
    std::string filePath = ((TObjString*)inputFileNameList.At(iFile))->GetString().Data();
    fprintf(stdout,"%4d %s\n",iFile,filePath.c_str());
    board->AddFile(filePath);
  }

  // Read events
  Int_t NumberOfEvents = 0;
  while ( board->NextEvent() ) {

    board->UnpackEvent();

    // Check if event can be accepted
    Int_t evtNumber = board->Event()->GetEventCounter();
    if ( (events.size() > 0) && (std::count(events.begin(),events.end(),evtNumber) == 0) ) continue;

    // Event is accepted: count it
    NumberOfEvents++;
    if ( nevents && (NumberOfEvents>nevents) ) {
      printf("- Read %d event(s): stopping here\n",nevents);
      break;
    }

    // Create directory for this event in output file
    TString evtDir = Form("%d/",evtNumber);
    histoFile->mkdir(evtDir);
    histoFile->cd(evtDir);

    // Show event header
    if (verbose>0) {
      printf("N %7d EvtCnt %7d Time %10u BId %2u S/N %3u LVDS 0x%04x Status 0x%03x GMsk 0x%1x 0Supp %d ActMsk %04x AccMsk %04x\n",
	     NumberOfEvents,evtNumber,board->Event()->GetEventTimeTag(),
	     board->Event()->GetBoardId(),board->Event()->GetBoardSN(),
	     board->Event()->GetLVDSPattern(),board->Event()->GetBoardStatus(),
	     board->Event()->GetGroupMask(),board->Event()->Get0SuppAlgrtm(),
	     board->Event()->GetActiveChannelMask(),board->Event()->GetAcceptedChannelMask());
    }

    // Save triggers information
    UChar_t groupmask = board->Event()->GetGroupMask();
    for(UInt_t t=0; t<ADCEVENT_NTRIGGERS; t++) {
      if ( groupmask & (0x1 << t) ) {
        if (verbose>1) {
	  printf("Trigger %u SIC %u Freq 0x%1x HasData 0x%1x TTT 0x%08x\n",
		 t,board->Event()->GetTriggerStartIndexCell(t),board->Event()->GetTriggerFrequency(t),
		 board->Event()->GetTriggerHasSignal(t),board->Event()->GetTriggerTimeTag(t));
	}
	TString hName = Form("T%d",t);
	h = GetHisto(hName,hList);
	h->Reset();
	h->SetTitle(Form("Event %d Board %d Trigger %d",evtNumber,board_id,t));
	for(UShort_t s=0;s<ADCEVENT_NSAMPLES;s++) h->Fill(s,board->Event()->GetADCTriggerSample(t,s));
      }
    }

    // Save channels information
    UInt_t channelmask;
    if ( board->Event()->GetBoardStatus() & (0x1 << 2) ) {
      channelmask = board->Event()->GetActiveChannelMask(); // Zero suppression is in flagging mode
    } else {
      channelmask = board->Event()->GetAcceptedChannelMask(); // Zero suppression is in rejection mode
    }
    for(UInt_t c=0; c<ADCEVENT_NCHANNELS; c++) {
      if ( channelmask & (0x1 << c) ) {
	TString hName = Form("C%02d",c);
	h = GetHisto(hName,hList);
	h->Reset();
	h->SetTitle(Form("Event %d Board %d Channel %d",evtNumber,board_id,c));
	for(UShort_t s=0;s<ADCEVENT_NSAMPLES;s++) h->Fill(s,board->Event()->GetADCChannelSample(c,s));
      }
    }
      
    // Write histograms to file
    hList.Write();

  }

  // Save and close output file
  printf("Closing output file\n");
  histoFile->Close();

  exit(0);

}
