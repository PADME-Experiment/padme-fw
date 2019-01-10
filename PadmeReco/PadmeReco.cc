#include "Riostream.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "PadmeReconstruction.hh"

#include <signal.h>
#include <fcntl.h>

#include "RecoRootIOManager.hh"
#include "PadmePerfUtils.hh"


PadmeReconstruction* PadmeReco; 
                              
void usage(char* name){
  std::cout << "Usage: "<< name << " [-h] [-b/-B #MaxFiles] [-i InputFile.root] [-l InputListFile.txt] [-n #MaxEvents] [-o OutputFile.root] [-s seed] [-c ConfigFileName.conf]" 
	    << std::endl;
}

void sighandler(int sig){
    std::cerr << std::endl << "********************************************************************************" << std::endl;
    std::cerr << "Killed with Signal " << sig << std::endl << "Closing ROOT files ..." << std::endl;

    //PadmeReco->EndProcessing();
    RecoRootIOManager::GetInstance()->EndRun();
    RecoRootIOManager::GetInstance()->Close();

    std::cerr << "... Done" << std::endl;
    std::cerr << std::endl << "********************************************************************************" << std::endl;

    exit(0);
}

int main(Int_t argc, char **argv)
{
    signal(SIGXCPU,sighandler);
    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);
    signal(127,sighandler);
    
    extern char *optarg;
    int opt;
    TString OutputFileName("OutputFile.root");
    TString InputFileName("InputFile.root");
    TString InputListFileName("InputListFile.txt");
    TString ConfFileName("config/PadmeReconstruction.conf");
    Int_t iFile = 0, NFiles = 100000, NEvt = 0;
    UInt_t Seed = 4357;
    struct stat filestat;

    Int_t n_options_read = 0;
    Int_t nb=0, nc=0, ni=0, nl=0, nn=0, no=0, ns=0;
    while ((opt = getopt(argc, argv, "b:B:c:h:i:l:n:o:s:")) != -1) {
      n_options_read++;
      switch (opt) {
      case 'b':
      case 'B':
	nb++;
	NFiles = TString(optarg).Atoi();
	break;
      case 'c':
	nc++;
	ConfFileName = TString(optarg);
	break;
      case 'h':
	usage(argv[0]);
	return 0;
      case 'i':
	ni++;
	InputFileName = TString(optarg);
	break;
      case 'l':
	nl++;
	InputListFileName = TString(optarg);
	break;
      case 'n':
	nn++;
	NEvt = TString(optarg).Atoi();
	break;
      case 'o':
	no++;
	OutputFileName = TString(optarg);
	break;
      case 's':
	ns++;
	Seed = (UInt_t)TString(optarg).Atoi();
	break;
      default:
	usage(argv[0]);
	return 0;
      }
    }

    // Sanity checks on the input
    if (!n_options_read || NEvt<0 || NFiles<=0) {
      usage(argv[0]);
      return 0;
    }
    if (nb>1 || nc>1 || ni>1 || nl>1 || nn>1 || no>1 || ns>0) {
      std::cerr << "[PadmeReco] Multiple arguments of the same type are not allowed" << std::endl;
      return 0;
    }

    // Protection against potentially incorrect output filenames
    struct stat buffer;
    if (!OutputFileName.EndsWith(".root") && !stat(OutputFileName.Data(), &buffer)) {
      std::cout << " [PadmeReco] Output file exists and is not *.root: potentially a destructive call" << std::endl;
      return 0;
    }

    TObjArray InputFileNameList;
    if(stat(Form(InputListFileName.Data()), &filestat) == 0) { //-l option used
        std::ifstream InputList(InputListFileName.Data());
        while(InputFileName.ReadLine(InputList) && iFile < NFiles){
//            if(stat(Form(InputFileName.Data()), &filestat) == 0)
          InputFileNameList.Add(new TObjString(InputFileName.Data()));
          iFile++;
        }
    } else if(InputFileName.CompareTo("")) //-i option used
//        if(stat(Form(InputFileName.Data()), &filestat) == 0)
      InputFileNameList.Add(new TObjString(InputFileName.Data()));

    if(InputFileNameList.GetEntries() == 0) {
        perror(Form("No Input File"));
        exit(1);
    }
    
    ////// init here memory allocated and cpu time
    int mem = 0;
    float cpu = 0;
    int umem = 0;
    float ucpu = 0;
    umem = PadmePerfUtils::getMem();
    ucpu = float(PadmePerfUtils::getCpu()/100.);
    mem = umem;
    cpu = ucpu;
    std::cout <<"***** PadmeReco MAIN                     \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    mem = umem;
    cpu = ucpu;
    

    //Perform the output initialization
    RecoRootIOManager *RecoIO = RecoRootIOManager::GetInstance();
    RecoIO->SetFileName(OutputFileName);
    RecoIO->NewRun(1);

    umem = PadmePerfUtils::getMem();
    ucpu = float(PadmePerfUtils::getCpu()/100.);
    std::cout <<"***** PadmeReco MAIN recoIO init         \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    mem = umem;
    cpu = ucpu;

    //    TFile* OutputFile = TFile::Open(OutputFileName.Data(),"RECREATE");

    //    PadmeReco = new PadmeReconstruction(&InputFileNameList, ConfFileName, OutputFile, NEvt, Seed);
    PadmeReco = new PadmeReconstruction(&InputFileNameList, ConfFileName, RecoIO->GetFile(), NEvt, Seed);

    umem = PadmePerfUtils::getMem();
    ucpu = float(PadmePerfUtils::getCpu()/100.);
    std::cout <<"***** PadmeReco MAIN Reconstruction init \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    mem = umem;
    cpu = ucpu;

    // memory before event loop
    int memBeforeEvLoop = mem;
    int cpuBeforeEvLoop = cpu;
    int niter=0;
    
    RecoIO->SetReconstruction(PadmeReco);
    
    std::cout<<"\n\n======================================================================================="<<std::endl;
    std::cout<<"======= PadmeReco: Initialization of the reconstruction is complete .... start processing events ................."<<std::endl;
    std::cout<<"=======================================================================================\n\n"<<std::endl;
    while(PadmeReco->NextEvent()) {
      RecoIO->SaveEvent();

      niter++;
      umem = PadmePerfUtils::getMem();
      ucpu = float(PadmePerfUtils::getCpu()/100.);
      if (niter<20 || niter%500==0) {
	std::cout <<"***** PadmeReco MAIN after this event    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	if (niter==1)
	  {
	    memBeforeEvLoop = umem;
	    cpuBeforeEvLoop = ucpu;
	    std::cout <<"***** PadmeReco MAIN AFTER FIRST EVENT      \t SZ= " << umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	  }
      }
      mem = umem;
      cpu = ucpu;
    }
    std::cout <<"***** PadmeReco MAIN after event loop    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM in the loop = "<<umem-memBeforeEvLoop<<" \t Delta T ="<<ucpu - cpuBeforeEvLoop <<" Events processed = "<<niter<< std::endl;
    std::cout <<"***** PadmeReco MAIN AVERAGE mem leak/events    \t SZ= " << (umem-memBeforeEvLoop)/float(niter-1) << " Kb/event \t average total_cpuTime/events = " << (ucpu - cpuBeforeEvLoop)/float(niter-1) << " s/event  "<< std::endl;
    
    PadmeReco->EndProcessing();
    RecoIO->EndRun();
    RecoIO->Close();
    std::cout <<"***** PadmeReco MAIN after finalizing    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;

    
    delete RecoIO;
    delete PadmeReco;
}
