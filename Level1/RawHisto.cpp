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
#include "TProfile.h"

#include "TRawEvent.hh"

TH1S* GetHisto(TString name, TObjArray histos) {
  for (Int_t iH = 0; iH < histos.GetEntries(); iH++) {
    if ( ((TH1S*)histos.At(iH))->GetName() == name ) return (TH1S*)histos.At(iH);
  }
  return 0;
}

TProfile* GetProfile(TString name, TObjArray histos) {
  for (Int_t iH = 0; iH < histos.GetEntries(); iH++) {
    if ( ((TProfile*)histos.At(iH))->GetName() == name ) return (TProfile*)histos.At(iH);
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
  Int_t board;
  std::vector<UInt_t> boards;

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
        if ( sscanf(optarg,"%d",&board) != 1 ) {
          fprintf (stderr, "Error while processing option '-b'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (board<0 || board>31) {
          fprintf (stderr, "Error while processing option '-b'. Required board %d (must be 0<=board<=31).\n", board);
          exit(1);
        }
	boards.push_back(board);
        fprintf(stdout,"Added board %d to list\n",board);
        break;
      case 'v':
	verbose++;
        break;
      case 'h':
        fprintf(stdout,"\nRawHisto [-i <file>] [-l <list>] [-o <file>] [-n <n>] [-e <event>] [-b <board>] [-v <level>] [-h]\n\n");
        fprintf(stdout,"  -i: add a file to list of input files (can be repeated)\n");
        fprintf(stdout,"  -l: specify text file with list of input files (one file per line)\n");
        fprintf(stdout,"  -o: define the name of the output file (default: %s)\n",outputFileName.Data());
        fprintf(stdout,"  -n: total number of events to save to output file (default: 0 i.e. no limit)\n");
        fprintf(stdout,"  -e: add <event> to list of events to save to output file (can be repeated)\n");
        fprintf(stdout,"  -b: add <board> to list of boards to save to output file (can be repeated)\n");
        fprintf(stdout,"  -v: enable verbose output (repeat for more output)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        fprintf(stdout,"N.B. if no -e/-b options are specified, then all events/boards will be saved to the output file\n\n");
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

  if ( inputFileNameList.GetEntries() == 0 ) {
    fprintf(stderr,"ERROR - Input file list is empty\n");
    exit(1);
  }

  if (verbose) fprintf(stdout,"Set verbose level to %d\n",verbose);

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
  //for(Int_t i=0; i < inputChain->GetListOfBranches()->GetEntries(); i++) {
  //  std::cout << "Branch " << i << " is " << inputChain->GetListOfBranches()->At(i)->GetName() << std::endl;
  //}
  TRawEvent* rawEv = new TRawEvent();
  inputChain->SetBranchAddress("RawEvent",&rawEv);

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

  // Create summary profile histograms with means and RMS (32 boards)
  TObjArray hpList(0);
  TProfile* hp;
  for(UChar_t b=0;b<32;b++){
    hp = new TProfile(Form("B%02dM",b),Form("Board %d Mean",b),32,-0.5,31.5,-100.,4195.);
    hpList.Add(hp);
    hp = new TProfile(Form("B%02dR",b),Form("Board %d RMS",b),32,-0.5,31.5,-100.,4195.);
    hpList.Add(hp);
  }

  // Create output file for histograms
  TFile* histoFile = new TFile(outputFileName,"RECREATE");
  if(!histoFile) {
    fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
    exit(1);
  }

  Int_t readEvt = 0;
  for(Int_t iev=0;iev<runNEntries;iev++){

    // Read event
    inputChain->GetEntry(iev);

    // Check if this event number was selected
    Int_t runNumber = rawEv->GetRunNumber();
    Int_t evtNumber = rawEv->GetEventNumber();
    if ( (events.size() > 0) && (std::count(events.begin(),events.end(),evtNumber) == 0) ) continue;
    readEvt++;
    if ( nevents && (readEvt>nevents) ) {
      printf("- Read %d event(s): stopping here\n",nevents);
      break;
    }

    // Get event trigger mask
    UInt_t trigMask = rawEv->GetEventTrigMask();

    // Create directory for this event in output file
    TString evtDir = Form("E%d_%02x/",evtNumber,trigMask);
    histoFile->mkdir(evtDir);

    // Check event type accoding to trigger mask
    Bool_t isBTFEvent = false;
    Bool_t isCosmicsEvent = false;
    Bool_t isOffBeamEvent = false;
    Bool_t isRandomEvent = false;
    if (trigMask & 0x01) isBTFEvent = true;
    if (trigMask & 0x02) isCosmicsEvent = true;
    if (trigMask & 0x20) isOffBeamEvent = true;
    if (trigMask & 0x40) isRandomEvent = true;

    // Show event header
    if (verbose>0) {
      TTimeStamp tts = rawEv->GetEventAbsTime();
      printf("N %7d Run %7d Event %7d Time %8d-%06d.%09d RunTime %12llu TrigMask %08x EvtStatus %08x Boards %2d MissBoard %08x\n",
	     iev,rawEv->GetRunNumber(),rawEv->GetEventNumber(),tts.GetDate(),tts.GetTime(),tts.GetNanoSec(),
	     rawEv->GetEventRunTime(),rawEv->GetEventTrigMask(),rawEv->GetEventStatus(),
	     rawEv->GetNADCBoards(),rawEv->GetMissingADCBoards());
    }

    // Loop over boards
    for(UChar_t b=0;b<rawEv->GetNADCBoards();b++){

      // Check if this board was selected
      Int_t brdId = rawEv->ADCBoard(b)->GetBoardId();
      if ( (boards.size() > 0) && (std::count(boards.begin(),boards.end(),brdId) == 0) ) continue;

      // Show board info
      TADCBoard* adcB = rawEv->ADCBoard(b);
      UChar_t nTrg = adcB->GetNADCTriggers();
      UChar_t nChn = adcB->GetNADCChannels();
      if (verbose>1) {
	printf("\tBoard %2u Board Id %2u Board SN %3u LVDS 0x%04x Status 0x%03x GMsk 0x%1x EvtCnt %7u Time %10u ActMsk 0x%08x AccMsk 0x%08x #Trg %u #Chn %2u\n",
	       b,adcB->GetBoardId(),adcB->GetBoardSN(),adcB->GetLVDSPattern(),adcB->GetBoardStatus(),adcB->GetGroupMask(),adcB->GetEventCounter(),
	       adcB->GetEventTimeTag(),adcB->GetActiveChannelMask(),adcB->GetAcceptedChannelMask(),nTrg,nChn);
      }

      // Reset all histograms
      TIter iObj1(&hList);
      while (TObject* obj = iObj1()) {
	((TH1S*)obj)->Reset();
	((TH1S*)obj)->SetTitle(Form("Run %d Event %d TrigMask 0x%02x Board %d",
				    runNumber,evtNumber,trigMask,brdId));
      }

      // Loop over triggers
      for(UChar_t t=0;t<nTrg;t++){
	TADCTrigger* trg = adcB->ADCTrigger(t);
	if (verbose>2) {
	  printf("\t\tTrig %u Grp %u SIC %u Freq %u Sign %u Time %u\n",
		 t,trg->GetGroupNumber(),trg->GetStartIndexCell(),trg->GetFrequency(),trg->GetTriggerSignal(),
		 trg->GetTriggerTimeTag());
	}
	TString hName = Form("T%d",trg->GetGroupNumber());
	h = GetHisto(hName,hList);
	//h->Reset();
	h->SetTitle(Form("Run %d Event %d TrigMask 0x%02x Board %d TrigGroup %d",
			 runNumber,evtNumber,trigMask,brdId,trg->GetGroupNumber()));
	for(UShort_t s=0;s<trg->GetNSamples();s++) {
	  //h->Fill(s,trg->GetSample(s));
          h->SetBinContent(s+1,trg->GetSample(s));
	}
      }

      // Loop over channels
      for(UChar_t c=0;c<nChn;c++){

	TADCChannel* chn = adcB->ADCChannel(c);
	//if (verbose>2) {
	//  printf("\t\tChan %u Chn# %u\n",c,chn->GetChannelNumber());
	//}
	TString hName = Form("C%02d",chn->GetChannelNumber());
	h = GetHisto(hName,hList);
	//h->Reset();
	h->SetTitle(Form("Run %d Event %d TrigMask 0x%02x Board %d Channel %d",
			 runNumber,evtNumber,trigMask,brdId,chn->GetChannelNumber()));
	Double_t sx = 0.;
	Double_t sx2 = 0.;
	for(UShort_t s=0;s<chn->GetNSamples();s++) {
	  //h->Fill(s,chn->GetSample(s));
          h->SetBinContent(s+1,chn->GetSample(s));
 	  if (s<994) {
	    Double_t x = chn->GetSample(s);
	    sx += x;
	    sx2 += x*x;
	  }
	}

	// Save mean and RMS for this channel to board profile only if this is not a physics event (Random or Off-Beam)
	if (isRandomEvent || isOffBeamEvent) {
	  Double_t mean = sx/994.;
	  hp = GetProfile(Form("B%02dM",brdId),hpList);
	  hp->Fill(chn->GetChannelNumber(),mean,1.);
	  Double_t rms = std::sqrt((sx2-994.*mean*mean)/993.);
	  hp = GetProfile(Form("B%02dR",brdId),hpList);
	  hp->Fill(chn->GetChannelNumber(),rms,1.);
	}
      }
      
      // Write non-empty board histograms to file
      //hList.Write();
      TObjArray hListOut(0);
      TIter iObj2(&hList);
      while (TObject* obj = iObj2()) {
      	if ( ((TH1S*)obj)->GetEntries() != 0. ) hListOut.Add(obj);
      }
      // If any data are present, create subdirectory for this board in output file and write data there
      if (hListOut.GetEntries()) {
	TString brdDir = Form("%sB%d/",evtDir.Data(),brdId);
	histoFile->mkdir(brdDir);
	histoFile->cd(brdDir);
	hListOut.Write();
      }

    } // End loop over boards

    // Clear event
    rawEv->Clear("C");
    
  } // End loop over events

  // Create summary directory for this event in output file
  TString smrDir = "Summary";
  histoFile->mkdir(smrDir);
  histoFile->cd(smrDir);

  // Select non-empty profiles
  TObjArray hpListOut(0);
  TIter iObj(&hpList);
  while (TObject* obj = iObj()) {
    if ( ((TProfile*)obj)->GetEntries() != 0. ) hpListOut.Add(obj);
  }

  // Save non-empty summary profile histograms to output file
  //hpList.Write();
  hpListOut.Write();

  // Save and close output file
  printf("Closing output file\n");
  histoFile->Close();

  delete rawEv;

  exit(0);

}
