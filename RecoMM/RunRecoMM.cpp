#include "RecoMM.h"

#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

bool IsRemoteFile(const TString &name)
{
  return name.BeginsWith("root://") ||
         name.BeginsWith("http://") ||
         name.BeginsWith("https://") ||
         name.BeginsWith("xroot://") ||
         name.BeginsWith("davs://");
}

bool FileExists(const TString &name)
{
  if (IsRemoteFile(name)) return true;

  struct stat filestat;
  return stat(name.Data(), &filestat) == 0;
}

void PrintUsage(const char *progname)
{
  cout << endl;
  cout << "Usage:" << endl;
  cout << "  " << progname << " -i input.root -r RunID -d DetRunID [options]" << endl;
  cout << "  " << progname << " -l filelist.list -r RunID -d DetRunID [options]" << endl;
  cout << endl;
  cout << "Options:" << endl;
  cout << "  -i <file>     Add one input ROOT file. Can be repeated." << endl;
  cout << "  -l <list>     Text file containing one input ROOT file per line." << endl;
  cout << "  -o <file>     Output ROOT file. Default: RecoMM.root" << endl;
  cout << "  -n <N>        Maximum events. Default: 0 = all." << endl;
  cout << "  -r <RunID>    PADME run ID." << endl;
  cout << "  -d <DetRunID> MM detector run ID." << endl;
  cout << "  -b <NBlock>   Block size in event IDs. Default: 1000." << endl;
  cout << endl;
}

int main(int argc, char *argv[])
{
  int RunID = 0;
  int DetRunID = 0;
  int maxEvents = 0;
  int NevtBlock = 1000;

  TString inputFileName;
  TString outputFileName = "RecoMM.root";

  TObjArray inputFileNameList;
  inputFileNameList.SetOwner(kTRUE);

  int opt = 0;
  while ((opt = getopt(argc, argv, "i:l:o:n:r:d:b:")) != -1) {
    switch (opt) {
      case 'i': {
        TString fname = optarg;

        if (!FileExists(fname)) {
          cerr << "WARNING: input file is not accessible: " << fname << endl;
          break;
        }

        inputFileNameList.Add(new TObjString(fname));
        cout << "Added input data file: " << fname << endl;
        break;
      }

      case 'l': {
        TString listName = optarg;

        if (!FileExists(listName)) {
          cerr << "ERROR: input list is not accessible: " << listName << endl;
          return 1;
        }

        cout << "Reading list of input files from: " << listName << endl;

        ifstream inputList(listName.Data());
        if (!inputList.is_open()) {
          cerr << "ERROR: cannot open input list: " << listName << endl;
          return 1;
        }

        while (inputFileName.ReadLine(inputList)) {
          TString fname = inputFileName.Strip(TString::kBoth);

          if (fname == "" || fname.BeginsWith("#")) continue;

          if (!FileExists(fname)) {
            cerr << "WARNING: file is not accessible: " << fname << endl;
            continue;
          }

          inputFileNameList.Add(new TObjString(fname));
          cout << "Added input data file: " << fname << endl;
        }

        inputList.close();
        break;
      }

      case 'o':
        outputFileName = optarg;
        cout << "Output ROOT file set to: " << outputFileName << endl;
        break;

      case 'n':
        maxEvents = atoi(optarg);
        if (maxEvents < 0) {
          cerr << "ERROR: maxEvents must be >= 0" << endl;
          return 1;
        }
        break;

      case 'r':
        RunID = atoi(optarg);
        break;

      case 'd':
        DetRunID = atoi(optarg);
        break;

      case 'b':
        NevtBlock = atoi(optarg);
        if (NevtBlock <= 0) {
          cerr << "ERROR: NevtBlock must be > 0" << endl;
          return 1;
        }
        break;

      default:
        PrintUsage(argv[0]);
        return 1;
    }
  }

  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR: input file list is empty" << endl;
    PrintUsage(argv[0]);
    return 1;
  }

  cout << endl;
  cout << "=== === === MM reconstruction settings === === ===" << endl;
  cout << "RunID          = " << RunID << endl;
  cout << "DetRunID       = " << DetRunID << endl;
  cout << "maxEvents      = " << maxEvents << endl;
  cout << "NevtBlock      = " << NevtBlock << endl;
  cout << "outputFileName = " << outputFileName << endl;
  cout << endl;

  RecoMM reco(&inputFileNameList, RunID, DetRunID, maxEvents, outputFileName);

  if (!reco.fTree) {
    cerr << "ERROR: RecoMM initialization failed" << endl;
    return 1;
  }

  reco.LoopFileList(inputFileNameList, NevtBlock);

  cout << endl << "Done." << endl;
  return 0;
}
