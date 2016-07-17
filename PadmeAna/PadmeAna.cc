#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <signal.h>
#include <fcntl.h>

#include "PadmeAnalysis.hh"

using namespace std;

void sighandler(int sig){
    cerr << endl << "********************************************************************************" << endl;
    cerr << "Killed with Signal " << sig << endl << "Closing ROOT files ..." << endl;
    cerr << "... Done" << endl;
    cerr << endl << "********************************************************************************" << endl;

    exit(0);
}

void usage(){
  cout << "Usage" << endl;
}

int main(Int_t argc, char **argv) {

  signal(SIGXCPU,sighandler);
  signal(SIGINT,sighandler);
  signal(SIGTERM,sighandler);
  signal(127,sighandler);
  extern char *optarg;
  int opt;
  string InputFileName;
  string InputListFileName;
  vector<string> FileList;
  string ConfFileName;

  Int_t nb=0, nc=0, ni=0, nl=0, nn=0, no=0, ns=0;
  while ((opt = getopt(argc, argv, "b:c:h:i:l:n:o:s:")) != -1) {
    n_options_read++;
    switch (opt) {
      
    case 'h':
      usage(argv[0]);
      return 0;
    case 'i':
      ni++;
      InputFileName = optarg;
      break;
    case 'l':
      nl++;
      InputListFileName = optarg;
      break;
    case 'c':
      nc++;
      ConfFileName = optarg;
      break;
    default:
      usage(argv[0]);
      return 0;
    }
  }

  
  if (nb>1 || nc>1 || ni>1 || nl>1 || nn>1 || no>1 || ns>0) {
    cerr << "[PadmeAna] Multiple arguments of the same type are not allowed" << endl;
    return 0;
  }
  
  if( ni == 0 && nl == 0) {
    cerr <<"[PadmeAna] Please provide data file to analyse" << endl;
    return 0;
  }
  
  //Get the list of files to analyse:

  if(ni == 1) {
    FileList.push_back(InputFileName);
  } else if (nl == 1) {
    cout << "Opening a list file: " << InputListFileName << endl;
    ifstream listfile(InputListFileName);
    while (getline(listfile,InputFileName)) {
      FileList.push_back(InputFileName);
    }
  }
  cout << "[PadmeAna] will run on " <<  FileList.size() << " files" << endl;
  
  
  PadmeAnalysis PadmeAna();
  PadmeAna.ProcessInputList(FileList);
  PadmeAna.EndProcessing();

  return 0;
}
