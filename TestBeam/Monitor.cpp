#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "Histo.hh"

#include "TRawEvent.hh"
#include "TApplication.h"

void FillHisto(std::string name, int nevents){
  Histo* his = Histo::GetInstance();
  TFile* fRawEv = new TFile(name.c_str());
  TTree* tRawEv = (TTree*)fRawEv->Get("RawEvents");
  TBranch* bRawEv = tRawEv->GetBranch("RawEvent");
  TRawEvent* rawEv = new TRawEvent();
  bRawEv->SetAddress(&rawEv);
  Int_t NMaxSamp = 1024;
  Float_t SampInd[1024];
  for(int ll=0;ll<NMaxSamp;ll++) SampInd[ll]=ll*1.;

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
    // Show event header
    UChar_t nBoards = rawEv->GetNADCBoards();
    // Loop over boards
    for(UChar_t b=0;b<nBoards;b++){

      // Show board info
      TADCBoard* adcB = rawEv->ADCBoard(b);
      UChar_t nTrg = adcB->GetNADCTriggers();
      UChar_t nChn = adcB->GetNADCChannels();
      printf("Ntrg Nchn %d %d\n",nTrg,nChn);
      // Loop over triggers
      for(UChar_t t=0;t<nTrg;t++){
	TADCTrigger* trg = adcB->ADCTrigger(t);
	Float_t Sam[1024];
	for(Int_t tt=0;tt<trg->GetNSamples();tt++){
	  Sam[tt] = (Float_t) trg->GetSample(tt);
	}
	if(b==0) his->FillGraph("CaloTrig",t,trg->GetNSamples(),SampInd,Sam);
      }

      // Loop over channels
      Float_t Qtot[2]={0};

      for(UChar_t c=0;c<nChn;c++){
	TADCChannel* chn = adcB->ADCChannel(c);
	//	printf("\t\tChan %u Chn# %u\n",c,chn->GetChannelNumber());
	Float_t Sam[1024];
	Float_t SamRec[1024];
	Int_t NAvg=80;
	Float_t SumSam=0.;
	Float_t QCh=0.;
	Float_t Avg=0.;
       	for(UShort_t s=0;s<chn->GetNSamples();s++){
	  Sam[s]    = (Float_t) chn->GetSample(s);
	  if(s<NAvg) SumSam+=Sam[s];
	}
	Avg=SumSam/NAvg;
	
       	for(UShort_t s=0;s<chn->GetNSamples();s++){
	  SamRec[s] = (Float_t) (chn->GetSample(s)-Avg)/4096*1.;
	  QCh+=-SamRec[s]; //to be converted in coulomb
	}
	printf("ch %d AVG %f Q0 %f\n",c,Avg,QCh);
	if(b==0) his->Fill1D(Form("hPedCalo%d",c),Avg);
	if(b==0) his->FillGraph("Calo",c,chn->GetNSamples(),SampInd,Sam);
	if(b==0) his->FillGraph("CaloReco",c,chn->GetNSamples(),SampInd,SamRec);
	Qtot[b]+=QCh;
      } //end of loop on channels

      printf("Bd %d Qtot %f\n",b,Qtot[b]);
    }
    // Clear event
    rawEv->Clear("C");
  }
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
  Histo* his = Histo::GetInstance();
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

  TApplication theApp("App",&argc,argv);
  FillHisto(infile.c_str(),nevents);
  his->InitMonitor();
  //  his->DrawCalo(1,9);
  sleep(10);
  //  theApp.Run();
  exit(0);
}


