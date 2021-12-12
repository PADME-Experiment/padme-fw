#include "Riostream.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>

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

#include <TObjString.h>


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


int run_name(std::string lin, std::string& name);

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
    TString ConfFileName("config/PadmeReconstruction.cfg");
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

    std::vector<string> file_names;
    TObjArray InputFileNameList;
    if(stat(Form(InputListFileName.Data()), &filestat) == 0) { //-l option used
      std::ifstream InputList(InputListFileName.Data());
      while(InputFileName.ReadLine(InputList) && iFile < NFiles){
	//if(stat(Form(InputFileName.Data()), &filestat) == 0)
	/*std::cout<<"InputFileName "<<InputFileName.Data()<<" <====================="<<std::endl;
	std::string name_ = "";
	run_name(InputFileName.Data(),name_);
	std::cout<<name_<<std::endl;
	if( std::find(file_names.begin(),file_names.end(),name_) == file_names.end() ){
	  file_names.push_back(name_);
	}
	*/
	InputFileNameList.Add(new TObjString(InputFileName.Data()));
	iFile++;
      }
    } else if(InputFileName.CompareTo("")) { //-i option used
      //        if(stat(Form(InputFileName.Data()), &filestat) == 0)
      /*std::cout<<"InputFileName "<<InputFileName.Data()<<" <====================="<<std::endl;
      std::string name_ = "";
      run_name(InputFileName.Data(),name_);
      std::cout<<name_<<std::endl;
      if( std::find(file_names.begin(),file_names.end(),name_) == file_names.end() ){
	file_names.push_back(name_);
      }
      */
      InputFileNameList.Add(new TObjString(InputFileName.Data()));
    }

    if(InputFileNameList.GetEntries() == 0) {
        perror(Form("No Input File"));
        exit(1);
    }

    for(unsigned int ij=0;ij<file_names.size();++ij){
      std::cout<<file_names[ij]<<std::endl;
    }
    ////// init here memory allocated and cpu time
    int mem = 0;
    double cpu = 0.;
    double run = 0.;
    int umem = 0;
    double ucpu = 0.;
    double urun = 0.;
    umem = PadmePerfUtils::getMem();
    ucpu = double(PadmePerfUtils::getCpu()/100.);
    urun = double(PadmePerfUtils::getRunTime()/100.);
    mem = umem;
    cpu = ucpu;
    run = urun;
    //std::cout <<"***** PadmeReco MAIN                     \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    printf("***** PadmeReco MAIN --- Size %d kB (%+d kB) --- CPU %.2f s (%+.2f s) --- Run %.2f s (%+.2f s) *****\n",umem,umem-mem,ucpu,ucpu-cpu,urun,urun-run);

    double cpuAtStart = ucpu;
    double runAtStart = urun;

    //Perform the output initialization
    RecoRootIOManager *RecoIO = RecoRootIOManager::GetInstance(ConfFileName);
    RecoIO->SetFileName(OutputFileName);
    //RecoIO->NewRun(1);

    umem = PadmePerfUtils::getMem();
    ucpu = double(PadmePerfUtils::getCpu()/100.);
    urun = double(PadmePerfUtils::getRunTime()/100.);
    //std::cout <<"***** PadmeReco MAIN recoIO init         \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    printf("***** PadmeReco MAIN recoIO init --- Size %d kB (%+d kB) --- CPU %.2f s (%+.2f s) --- Run %.2f s (%+.2f s) *****\n",umem,umem-mem,ucpu,ucpu-cpu,urun,urun-run);
    mem = umem;
    cpu = ucpu;
    run = urun;

    PadmeReco = new PadmeReconstruction(&InputFileNameList, ConfFileName, RecoIO->GetFile(), NEvt, Seed);
    RecoIO->SetReconstruction(PadmeReco);
    RecoIO->NewRun(1);

    umem = PadmePerfUtils::getMem();
    ucpu = double(PadmePerfUtils::getCpu()/100.);
    urun = double(PadmePerfUtils::getRunTime()/100.);
    //std::cout <<"***** PadmeReco MAIN Reconstruction init \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    printf("***** PadmeReco MAIN Reconstruction init --- Size %d kB (%+d kB) --- CPU %.2f s (%+.2f s) --- Run %.2f s (%+.2f s) *****\n",umem,umem-mem,ucpu,ucpu-cpu,urun,urun-run);
    mem = umem;
    cpu = ucpu;
    run = urun;

    int memAfterFirstEvent = umem;
    int memBeforeEvLoop = umem;
    double cpuBeforeEvLoop = ucpu;
    double runBeforeEvLoop = urun;
    int niter=0;
    
    std::cout<<"\n\n======================================================================================="<<std::endl;
    std::cout<<"======= PadmeReco: Initialization of the reconstruction is complete .... start processing events ................."<<std::endl;
    std::cout<<"=======================================================================================\n\n"<<std::endl;
    while(PadmeReco->NextEvent()) {
      RecoIO->SaveEvent();

      niter++;
      umem = PadmePerfUtils::getMem();
      ucpu = double(PadmePerfUtils::getCpu()/100.);
      urun = double(PadmePerfUtils::getRunTime()/100.);
      if (niter<20 || niter%500==0) {
	if (niter==1) memAfterFirstEvent = umem;
	printf("***** PadmeReco MAIN after event %6d --- Size %d kB (%+d kB) --- CPU %.2f s (%+.2f s) --- Run %.2f s (%+.2f s) *****\n",niter,umem,umem-mem,ucpu,ucpu-cpu,urun,urun-run);
	//std::cout <<"***** PadmeReco MAIN after this event    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	//if (niter==1)
	//  {
	//    memBeforeEvLoop = umem;
	//    cpuBeforeEvLoop = ucpu;
	//    std::cout <<"***** PadmeReco MAIN AFTER FIRST EVENT      \t SZ= " << umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
	//  }
      }
      mem = umem;
      cpu = ucpu;
      run = urun;
    }

    umem = PadmePerfUtils::getMem();
    ucpu = double(PadmePerfUtils::getCpu()/100.);
    urun = double(PadmePerfUtils::getRunTime()/100.);
    //std::cout <<"***** PadmeReco MAIN after event loop    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM in the loop = "<<umem-memBeforeEvLoop<<" \t Delta T ="<<ucpu - cpuBeforeEvLoop <<" Events processed = "<<niter<< std::endl;
    //std::cout <<"***** PadmeReco MAIN AVERAGE mem leak/events    \t SZ= " << (umem-memBeforeEvLoop)/float(niter-1) << " Kb/event \t average total_cpuTime/events = " << (ucpu - cpuBeforeEvLoop)/float(niter-1) << " s/event  "<< std::endl;
    printf("***** PadmeReco MAIN after event loop --- Size %d kB (%+d kB) --- CPU %.2f s (%+.2f s) --- Run %.2f s (%+.2f s) *****\n",umem,umem-memBeforeEvLoop,ucpu,ucpu-cpuBeforeEvLoop,urun,urun-runBeforeEvLoop);
    printf("***** PadmeReco MAIN AVERAGE --- Memory Leak %.3f kB/evt (total %d kB) --- CPU %.3f s/evt --- Run %.3f s/evt *****\n",float(umem-memAfterFirstEvent)/(niter-1),umem-memAfterFirstEvent,(ucpu-cpuBeforeEvLoop)/niter,(urun-runBeforeEvLoop)/niter);
    mem = umem;
    cpu = ucpu;
    run = urun;

    int memAfterEvLoop = umem;
    double cpuAfterEvLoop = ucpu;
    double runAfterEvLoop = urun;

    PadmeReco->EndProcessing();
    RecoIO->EndRun();
    RecoIO->Close();

    umem = PadmePerfUtils::getMem();
    ucpu = double(PadmePerfUtils::getCpu()/100.);
    urun = double(PadmePerfUtils::getRunTime()/100.);
    //std::cout <<"***** PadmeReco MAIN after finalizing    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    printf("***** PadmeReco MAIN after finalizing --- Size %d kB (%+d kB) --- CPU %.2f s (%+.2f s) --- Run %.2f s (%+.2f s) *****\n",umem,umem-mem,ucpu,ucpu-cpu,urun,urun-run);
    mem = umem;
    cpu = ucpu;
    run = urun;

    delete RecoIO;
    delete PadmeReco;

    umem = PadmePerfUtils::getMem();
    ucpu = double(PadmePerfUtils::getCpu()/100.);
    urun = double(PadmePerfUtils::getRunTime()/100.);
    //std::cout <<"***** PadmeReco MAIN after clean up    \t SZ= " <<umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = "<<umem-mem<<" \t Delta T ="<<ucpu - cpu << std::endl;
    printf("***** PadmeReco MAIN after clean up --- Size %d kB (%+d kB) --- CPU %.2f s (%+.2f s) --- Run %.2f s (%+.2f s) *****\n",umem,umem-mem,ucpu,ucpu-cpu,urun,urun-run);

    double cpuAtEnd = ucpu;
    double runAtEnd = urun;

    // Show final run statistics before exiting
    printf("\n");
    printf ("RecoInfo - Processed Events %d\n",niter);
    printf("\n");
    printf ("RecoInfo - Total CPU time %.2f s\n",cpuAtEnd-cpuAtStart);
    printf ("RecoInfo - Total Run time %.2f s\n",runAtEnd-runAtStart);
    printf("\n");
    printf ("RecoInfo - Total Event Processing CPU time %.2f s\n",cpuAfterEvLoop-cpuBeforeEvLoop);
    printf ("RecoInfo - Total Event Processing Run time %.2f s\n",runAfterEvLoop-runBeforeEvLoop);
    if (niter>0) {
      printf("\n");
      printf ("RecoInfo - Average Event Processing CPU time %.3f s/evt\n",(cpuAfterEvLoop-cpuBeforeEvLoop)/niter);
      printf ("RecoInfo - Average Event Processing Run time %.3f s/evt\n",(runAfterEvLoop-runBeforeEvLoop)/niter);
    }
    printf("\n");
    printf ("RecoInfo - Memory Allocation before Event Processing %d kB\n",memBeforeEvLoop);
    printf ("RecoInfo - Memory Allocation after First Event       %d kB\n",memAfterFirstEvent);
    printf ("RecoInfo - Memory Allocation after Event Processing  %d kB\n",memAfterEvLoop);
    if (niter>1) {
      printf("\n");
      printf ("RecoInfo - Total Processing Memory Leak  %d kB\n",memAfterEvLoop-memAfterFirstEvent);
      printf ("RecoInfo - Average Memory Leak per Event %.3f kB/evt\n",float(memAfterEvLoop-memAfterFirstEvent)/(niter-1));
    }

}




int run_name(std::string lin, std::string& name){

  std::size_t begin = lin.find_last_of("/\\");

  name = lin.substr(begin+5,23);

  return 0;
}


