#include "RecoTMM.h"

#include "TChain.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <cmath>
#include <cerrno>
#include <iomanip>

using namespace std;

bool IsRemoteFile(const TString& name)
{
  return name.BeginsWith("root://") || name.BeginsWith("http://") || name.BeginsWith("https://") || name.BeginsWith("xroot://") || name.BeginsWith("davs://");
}

bool FileExists(const TString& name)
{
  // if (IsRemoteFile(name)) return true;

  struct stat filestat;
  return stat(name.Data(), &filestat) == 0;
}

void PrintUsage(const char* progname)
{
  cout << endl;
  cout << "Usage:" << endl;
  cout << "  " << progname << " -i input.root -r RunID -d DetRunID [options]" << endl;
  cout << "  " << progname << " -l filelist.list -r RunID -d DetRunID [options]" << endl;
  cout << endl;
  cout << "Options:" << endl;
  cout << "  -i <file>     Add one input ROOT file. Can be repeated." << endl;
  cout << "  -l <list>     Text file containing one input ROOT file per line." << endl;
  cout << "  -o <file>     Output ROOT file name. Default: RecoTMM_run0000000.root" << endl;
  cout << "  -n <N>        Maximum number of events to reconstruct. Default: 0 = all." << endl;
  cout << "  -r <RunID>    PADME run ID, used as label/output metadata." << endl;
  cout << "  -d <DetRunID> Detector run ID, used as label/output metadata." << endl;
  cout << "  -s <time>     External inclusive UNIX start time [s]; requires -t." << endl;
  cout << "  -t <time>     External exclusive UNIX stop time [s]; requires -s." << endl;
  cout << "  -b <N>        Number of time bins. Default: 100." << endl;
  cout << "  -h            Show this help." << endl;
  cout << endl;
}

int main(int argc, char* argv[])
{
  
  int opt;

  int RunID = 0;
  int DetRunID = 0;
  int maxEvents = 0;
  int nBins = 100;
  double tStartUnix = std::numeric_limits<double>::quiet_NaN();
  double tStopUnix = std::numeric_limits<double>::quiet_NaN();
  bool haveTStart = false;
  bool haveTStop = false;

  TString inputFileName;
  TString outputFileName = "RecoTMM_run0000000.root";

  TObjArray inputFileNameList;
  inputFileNameList.SetOwner(kTRUE);

  while ((opt = getopt(argc, argv, "i:l:o:n:r:d:s:t:b:h")) != -1) {

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
          TString fname = inputFileName;
          fname = fname.Strip(TString::kBoth);
          if (fname == "") continue;
          if (fname.BeginsWith("#")) continue;
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

      case 'o': {
        outputFileName = optarg;
        cout << "Output ROOT file set to: " << outputFileName << endl;
        break;
      }

      case 'n': {
        maxEvents = atoi(optarg);
        if (maxEvents < 0) {
          cerr << "ERROR: maxEvents must be >= 0" << endl;
          return 1;
        }
        if (maxEvents > 0) {
          cout << "Will reconstruct first " << maxEvents << " events" << endl;
        } else {
          cout << "Will reconstruct all events" << endl;
        }
        break;
      }

      case 'r': {
        RunID = atoi(optarg);
        cout << "RunID set to: " << RunID << endl;
        break;
      }

      case 'd': {
        DetRunID = atoi(optarg);
        cout << "DetRunID set to: " << DetRunID << endl;
        break;
      }

      case 's': {
        char* end = nullptr;
        tStartUnix = strtod(optarg, &end);
        if (end == optarg || *end != '\0' || !std::isfinite(tStartUnix)) {
          cerr << "ERROR: invalid -s UNIX timestamp: " << optarg << endl;
          return 1;
        }
        haveTStart = true;
        break;
      }

      case 't': {
        char* end = nullptr;
        tStopUnix = strtod(optarg, &end);
        if (end == optarg || *end != '\0' || !std::isfinite(tStopUnix)) {
          cerr << "ERROR: invalid -t UNIX timestamp: " << optarg << endl;
          return 1;
        }
        haveTStop = true;
        break;
      }

      case 'b': {
        char* end = nullptr;
        errno = 0;
        const long parsed = strtol(optarg, &end, 10);
        if (errno == ERANGE || end == optarg || *end != '\0' ||
            parsed < 1 || parsed > std::numeric_limits<int>::max()) {
          cerr << "ERROR: invalid -b number of time bins: " << optarg << endl;
          return 1;
        }
        nBins = static_cast<int>(parsed);
        break;
      }

      case 'h': {
        PrintUsage(argv[0]);
        return 0;
      }

      default: {
        PrintUsage(argv[0]);
        return 1;
      }
    }
  }

  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR: input file list is empty" << endl;
    PrintUsage(argv[0]);
    return 1;
  }

  if (RunID < 0) {
    cerr << "ERROR: RunID was not provided or is invalid" << endl;
    PrintUsage(argv[0]);
    return 1;
  }

  if (DetRunID < 0) {
    cerr << "ERROR: DetRunID was not provided or is invalid" << endl;
    PrintUsage(argv[0]);
    return 1;
  }

  if (haveTStart != haveTStop) {
    cerr << "ERROR: -s and -t must be supplied together" << endl;
    return 1;
  }

  if (haveTStart && tStopUnix <= tStartUnix) {
    cerr << "ERROR: -t must be greater than -s" << endl;
    return 1;
  }

  cout << endl;
  cout << "=== === === Reconstruction settings === === ===" << endl;
  cout << "RunID          = " << RunID << endl;
  cout << "DetRunID       = " << DetRunID << endl;
  cout << "maxEvents      = " << maxEvents << endl;
  cout << "nBins          = " << nBins << endl;
  if (haveTStart) {
    cout << setprecision(17) << "tStartUnix     = " << tStartUnix << endl;
    cout << setprecision(17) << "tStopUnix      = " << tStopUnix << endl;
  } else {
    cout << "tStartUnix/tStopUnix = TMM chain-derived bounds (legacy fallback)" << endl;
  }
  cout << "outputFileName = " << outputFileName << endl;
  cout << endl;

  {
    RecoTMM reco(&inputFileNameList, RunID, DetRunID, maxEvents, outputFileName, tStartUnix, tStopUnix, nBins);
    reco.LoopFileList(inputFileNameList);    
  }

  cout << endl;
  cout << "Done." << endl;

  return 0;
}