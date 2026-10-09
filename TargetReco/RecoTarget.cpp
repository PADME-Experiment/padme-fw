#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"

#include "TRawEvent.hh"
#include "TRawMergedEvent.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using namespace std;
namespace {

// -----------------------------------------------------------------------------
// Detector / acquisition configuration
// -----------------------------------------------------------------------------

constexpr int kTargetBoardId = 28;
constexpr int kLeadGlassBoardId = 14;
constexpr int kLeadGlassChannel = 31;

constexpr int kNTargetChannels = 32;
constexpr int kNSamples = 1000;

constexpr int kTargetPedestalSamples = 100;
constexpr int kLeadGlassPedestalSamples = 50;

constexpr int kBlockSize = 100;

// Target charge integration window.
// The original code used: t > 200 && t < 700.
constexpr int kTargetIntegrationFirst = 200;
constexpr int kTargetIntegrationLast = 699;

// Lead Glass charge integration window.
// The original code used: t > 100 && t < 600.
constexpr int kLeadGlassIntegrationFirst = 100;
constexpr int kLeadGlassIntegrationLast = 599;

constexpr double kVpp = 1.0;
constexpr double kADCCounts = 4096.0;
constexpr double kTargetImpedanceOhm = 50.0;

// IMPORTANT:
// The original reconstruction used digiTime = 1 ns even though a comment said
// "sampled at 5 GS/s".  We intentionally preserve the numerical value here.
// Replace this constant with the proper value from the raw-data header once the
// corresponding PADME getter has been identified.
constexpr double kSamplePeriodNs = 1.0;
constexpr double kSamplePeriodSec = kSamplePeriodNs * 1.e-9;

// Legacy Target normalization retained only to allow comparison with QChOld.
constexpr double kLegacyTargetScale = 1.75;

// Original Lead Glass charge conversion.
constexpr double kLeadGlassChargeFactor = 4.8828e-3;

constexpr double kInvalid = -999.0;


// -----------------------------------------------------------------------------
// Small reconstruction result objects
// -----------------------------------------------------------------------------

struct TargetWaveformResult {
  double chargeLegacy = kInvalid;
  double charge = kInvalid;
  double vMax = kInvalid;
  double tMaxNs = kInvalid;
};

struct LeadGlassWaveformResult {
  double charge = kInvalid;
  double vMax = kInvalid;
  double tMaxNs = kInvalid;
};


// -----------------------------------------------------------------------------
// Output tree records
// -----------------------------------------------------------------------------

struct EventRecord {
  Int_t run = -1;
  Int_t event = -1;

  Long64_t blockId = -1;
  Int_t indexInBlock = -1;

  UInt_t triggerMask = 0;

  ULong64_t eventRunTime = 0;
  Double_t absTimeUnix = kInvalid;

  Double_t targetQOld[kNTargetChannels];
  Double_t targetQ[kNTargetChannels];
  Double_t targetVMax[kNTargetChannels];
  Double_t targetTMax[kNTargetChannels];

  Double_t lgQ = kInvalid;
  Double_t lgVMax = kInvalid;
  Double_t lgTMax = kInvalid;

  void Reset()
  {
    run = -1;
    event = -1;
    blockId = -1;
    indexInBlock = -1;
    triggerMask = 0;
    eventRunTime = 0;
    absTimeUnix = kInvalid;

    fill_n(targetQOld, kNTargetChannels, kInvalid);
    fill_n(targetQ, kNTargetChannels, kInvalid);
    fill_n(targetVMax, kNTargetChannels, kInvalid);
    fill_n(targetTMax, kNTargetChannels, kInvalid);

    lgQ = kInvalid;
    lgVMax = kInvalid;
    lgTMax = kInvalid;
  }
};


struct BlockRecord {
  Int_t run = -1;
  Long64_t blockId = -1;

  Bool_t complete = false;
  Int_t nEvents = 0;

  Int_t firstEvent = -1;
  Int_t lastEvent = -1;

  ULong64_t firstEventRunTime = 0;
  ULong64_t lastEventRunTime = 0;

  Double_t firstTimeUnix = kInvalid;
  Double_t meanTimeUnix = kInvalid;
  Double_t lastTimeUnix = kInvalid;

  Double_t targetQ[kNTargetChannels];
  Double_t targetVMax[kNTargetChannels];
  Double_t targetTMax[kNTargetChannels];

  Double_t lgQMean = kInvalid;
  Double_t lgQStd = kInvalid;
  Double_t lgQSem = kInvalid;

  Double_t lgVMaxMean = kInvalid;
  Double_t lgVMaxStd = kInvalid;
  Double_t lgVMaxSem = kInvalid;

  Double_t lgTMaxMean = kInvalid;
  Double_t lgTMaxStd = kInvalid;
  Double_t lgTMaxSem = kInvalid;

  void Reset()
  {
    run = -1;
    blockId = -1;
    complete = false;
    nEvents = 0;

    firstEvent = -1;
    lastEvent = -1;

    firstEventRunTime = 0;
    lastEventRunTime = 0;

    firstTimeUnix = kInvalid;
    meanTimeUnix = kInvalid;
    lastTimeUnix = kInvalid;

    fill_n(targetQ, kNTargetChannels, kInvalid);
    fill_n(targetVMax, kNTargetChannels, kInvalid);
    fill_n(targetTMax, kNTargetChannels, kInvalid);

    lgQMean = kInvalid;
    lgQStd = kInvalid;
    lgQSem = kInvalid;

    lgVMaxMean = kInvalid;
    lgVMaxStd = kInvalid;
    lgVMaxSem = kInvalid;

    lgTMaxMean = kInvalid;
    lgTMaxStd = kInvalid;
    lgTMaxSem = kInvalid;
  }
};


// -----------------------------------------------------------------------------
// Block accumulator
// -----------------------------------------------------------------------------

struct BlockAccumulator {
  Long64_t blockId = 0;
  Int_t run = -1;

  Int_t nEvents = 0;
  Int_t firstEvent = -1;
  Int_t lastEvent = -1;

  ULong64_t firstEventRunTime = 0;
  ULong64_t lastEventRunTime = 0;

  Double_t firstTimeUnix = kInvalid;
  Double_t lastTimeUnix = kInvalid;

  // To avoid loss of precision from repeatedly summing ~1.8e9 UNIX times,
  // accumulate offsets relative to the first event in the block.
  Double_t sumTimeOffset = 0.0;

  array<array<double, kNSamples>, kNTargetChannels> targetWaveSum{};

  double lgQSum = 0.0;
  double lgQSumSq = 0.0;

  double lgVMaxSum = 0.0;
  double lgVMaxSumSq = 0.0;

  double lgTMaxSum = 0.0;
  double lgTMaxSumSq = 0.0;

  void Reset(Long64_t newBlockId)
  {
    blockId = newBlockId;
    run = -1;

    nEvents = 0;
    firstEvent = -1;
    lastEvent = -1;

    firstEventRunTime = 0;
    lastEventRunTime = 0;

    firstTimeUnix = kInvalid;
    lastTimeUnix = kInvalid;
    sumTimeOffset = 0.0;

    for (auto& channel : targetWaveSum) {
      channel.fill(0.0);
    }

    lgQSum = 0.0;
    lgQSumSq = 0.0;

    lgVMaxSum = 0.0;
    lgVMaxSumSq = 0.0;

    lgTMaxSum = 0.0;
    lgTMaxSumSq = 0.0;
  }

  bool Empty() const
  {
    return nEvents == 0;
  }

  bool Full() const
  {
    return nEvents >= kBlockSize;
  }

  void AddEvent(Int_t eventRun, Int_t eventNumber, ULong64_t eventRunTime, Double_t absTimeUnix, const array<array<double, kNSamples>, kNTargetChannels>& targetRaw, const LeadGlassWaveformResult& lg){
    if (nEvents == 0) {
      run = eventRun;
      firstEvent = eventNumber;
      firstEventRunTime = eventRunTime;
      firstTimeUnix = absTimeUnix;
      sumTimeOffset = 0.0;
    } else {
      sumTimeOffset += absTimeUnix - firstTimeUnix;
    }

    lastEvent = eventNumber;
    lastEventRunTime = eventRunTime;
    lastTimeUnix = absTimeUnix;

    for (int ch = 0; ch < kNTargetChannels; ++ch) {
      for (int s = 0; s < kNSamples; ++s) {
        targetWaveSum[ch][s] += targetRaw[ch][s];
      }
    }

    lgQSum += lg.charge;
    lgQSumSq += lg.charge * lg.charge;

    lgVMaxSum += lg.vMax;
    lgVMaxSumSq += lg.vMax * lg.vMax;

    lgTMaxSum += lg.tMaxNs;
    lgTMaxSumSq += lg.tMaxNs * lg.tMaxNs;

    ++nEvents;
  }
};


// -----------------------------------------------------------------------------
// Utility functions
// -----------------------------------------------------------------------------

bool IsRemoteFile(const TString& name)
{
  return name.BeginsWith("root://") ||
         name.BeginsWith("http://") ||
         name.BeginsWith("https://") ||
         name.BeginsWith("xroot://") ||
         name.BeginsWith("davs://");
}


void MeanStdSem(double sum, double sumSq, int n, double& mean, double& stdev, double& sem){
  
  if (n <= 0) {
    mean = kInvalid;
    stdev = kInvalid;
    sem = kInvalid;
    return;
  }

  mean = sum / (double)(n);

  if (n == 1) {
    stdev = 0.0;
    sem = 0.0;
    return;
  }

  const double variance = max(0.0, (sumSq - (double)(n) * mean * mean) / (double)(n - 1));

  stdev = sqrt(variance);
  sem = stdev / sqrt((double)(n));
}


// -----------------------------------------------------------------------------
// Waveform reconstruction
// -----------------------------------------------------------------------------

TargetWaveformResult ReconstructTargetWaveform(const array<double, kNSamples>& samples){
  TargetWaveformResult result;

  double pedestal = 0.0;
  for (int s = 0; s < kTargetPedestalSamples; ++s) {
    pedestal += samples[s];
  }
  pedestal /= (double)(kTargetPedestalSamples);

  double vMax = -numeric_limits<double>::infinity();
  int tMaxSample = 0;

  double chargeLegacy = 0.0;
  double charge = 0.0;

  for (int s = 0; s < kNSamples; ++s) {
    const double signalMv = kVpp * (samples[s] - pedestal) / kADCCounts * 1000.0;

    if (signalMv > vMax) {
      vMax = signalMv;
      tMaxSample = s;
    }

    if (s >= kTargetIntegrationFirst && s <= kTargetIntegrationLast) {

      // Legacy Target charge definition retained for comparison.
      chargeLegacy += signalMv / kTargetImpedanceOhm * kSamplePeriodSec / 1.e-12 / 1000.0 / kLegacyTargetScale;

      // Main event/cumulative charge:
      // integrate only positive contributions, as intended in the old code.
      const double positiveSignalMv = max(0.0, signalMv);

      charge += positiveSignalMv * 1.e-3 / kTargetImpedanceOhm * kSamplePeriodSec / 1.e-12;
    }
  }

  result.chargeLegacy = chargeLegacy;
  result.charge = charge;
  result.vMax = vMax;
  result.tMaxNs = (double)(tMaxSample) * kSamplePeriodNs;

  return result;
}


LeadGlassWaveformResult ReconstructLeadGlassWaveform(const array<double, kNSamples>& samples){

  LeadGlassWaveformResult result;

  double pedestal = 0.0;
  for (int s = 0; s < kLeadGlassPedestalSamples; ++s) {
    pedestal += samples[s];
  }
  pedestal /= (double)(kLeadGlassPedestalSamples);

  double vMax = -numeric_limits<double>::infinity();
  int tMaxSample = 0;

  double charge = 0.0;

  for (int s = 0; s < kNSamples; ++s) {

    // LG pulse is negative-going.
    // Keep exactly the original convention: this is still in ADC counts,
    // not converted to mV.
    const double signal = kVpp * (pedestal - samples[s]);

    if (signal > vMax) {
      vMax = signal;
      tMaxSample = s;
    }

    if (s >= kLeadGlassIntegrationFirst && s <= kLeadGlassIntegrationLast) {
      charge += signal * kLeadGlassChargeFactor;
    }
  }

  result.charge = charge;
  result.vMax = vMax;
  result.tMaxNs = (double)(tMaxSample) * kSamplePeriodNs;

  return result;
}


// -----------------------------------------------------------------------------
// Raw detector extraction
// -----------------------------------------------------------------------------

bool ExtractDetectorWaveforms(TRawEvent* rawEvent, array<array<double, kNSamples>, kNTargetChannels>& targetRaw, array<double, kNSamples>& lgRaw, int verbose){
  
  for (auto& channel : targetRaw) {
    channel.fill(0.0);
  }
  lgRaw.fill(0.0);

  array<bool, kNTargetChannels> targetChannelSeen{};
  bool lgSeen = false;
  bool targetBoardSeen = false;
  bool lgBoardSeen = false;

  for (UChar_t brd = 0; brd < rawEvent->GetNADCBoards(); ++brd) {

    TADCBoard* adcBoard = rawEvent->ADCBoard(brd);
    if (!adcBoard) {
      if (verbose > 1) cerr << "WARNING - null ADC board pointer" << endl;
      continue;
    }

    const int boardId = adcBoard->GetBoardId();

    if (boardId != kTargetBoardId && boardId != kLeadGlassBoardId) continue;

    if (boardId == kTargetBoardId)    targetBoardSeen = true;
    if (boardId == kLeadGlassBoardId) lgBoardSeen = true;

    const UChar_t nChannels = adcBoard->GetNADCChannels();

    for (UChar_t ich = 0; ich < nChannels; ++ich) {

      TADCChannel* adcChannel = adcBoard->ADCChannel(ich);
      if (!adcChannel) {
        if (verbose > 1) cerr << "WARNING - null ADC channel pointer on board " << boardId << endl;
        continue;
      }

      const int channel = adcChannel->GetChannelNumber();

      if (boardId == kLeadGlassBoardId) {

        if (channel != kLeadGlassChannel) continue;

        for (int s = 0; s < kNSamples; ++s) {
          lgRaw[s] = (double)(adcChannel->GetSample(s));
        }

        lgSeen = true;
      }

      else if (boardId == kTargetBoardId) {
        if (channel < 0 || channel >= kNTargetChannels) {
          if (verbose) {
            cerr << "WARNING - unexpected Target channel " << channel << endl;
          }
          continue;
        }
        for (int s = 0; s < kNSamples; ++s) {
          targetRaw[channel][s] = (double)(adcChannel->GetSample(s));
        }
        targetChannelSeen[channel] = true;
      }
    }
  }

  if (!targetBoardSeen || !lgBoardSeen || !lgSeen) {
    return false;
  }

  for (int ch = 0; ch < kNTargetChannels; ++ch) {
    if (!targetChannelSeen[ch]) {
      if (verbose) {
        cerr  << "WARNING - Target channel " << ch << " missing: event rejected" << endl;
      }
      return false;
    }
  }

  return true;
}


// -----------------------------------------------------------------------------
// ROOT tree creation
// -----------------------------------------------------------------------------

TTree* CreateEventTree(EventRecord& event)
{
  TTree* tree = new TTree( "Events", "Event-level Target and Lead Glass reconstruction");

  tree->Branch("run", &event.run, "run/I");
  tree->Branch("event", &event.event, "event/I");
  tree->Branch("blockId", &event.blockId, "blockId/L");
  tree->Branch("indexInBlock", &event.indexInBlock, "indexInBlock/I");
  tree->Branch("triggerMask", &event.triggerMask, "triggerMask/i");
  tree->Branch("eventRunTime", &event.eventRunTime, "eventRunTime/l");
  tree->Branch("absTimeUnix", &event.absTimeUnix, "absTimeUnix/D");
  tree->Branch("targetQOld", event.targetQOld, Form("targetQOld[%d]/D", kNTargetChannels));
  tree->Branch("targetQ", event.targetQ, Form("targetQ[%d]/D", kNTargetChannels));
  tree->Branch("targetVMax", event.targetVMax, Form("targetVMax[%d]/D", kNTargetChannels));
  tree->Branch("targetTMax", event.targetTMax, Form("targetTMax[%d]/D", kNTargetChannels));
  tree->Branch("lgQ", &event.lgQ, "lgQ/D");
  tree->Branch("lgVMax", &event.lgVMax, "lgVMax/D");
  tree->Branch("lgTMax", &event.lgTMax, "lgTMax/D");

  return tree;
}


TTree* CreateBlockTree(BlockRecord& block)
{
  TTree* tree = new TTree("Blocks", "Block-level cumulative Target and Lead Glass reconstruction");

  tree->Branch("run", &block.run, "run/I");
  tree->Branch("blockId", &block.blockId, "blockId/L");
  tree->Branch("complete", &block.complete, "complete/O");
  tree->Branch("nEvents", &block.nEvents, "nEvents/I");
  tree->Branch("firstEvent", &block.firstEvent, "firstEvent/I");
  tree->Branch("lastEvent", &block.lastEvent, "lastEvent/I");
  tree->Branch("firstEventRunTime", &block.firstEventRunTime, "firstEventRunTime/l");
  tree->Branch("lastEventRunTime", &block.lastEventRunTime, "lastEventRunTime/l");
  tree->Branch("firstTimeUnix", &block.firstTimeUnix, "firstTimeUnix/D");
  tree->Branch("meanTimeUnix", &block.meanTimeUnix, "meanTimeUnix/D");
  tree->Branch("lastTimeUnix", &block.lastTimeUnix, "lastTimeUnix/D");
  tree->Branch("targetQ", block.targetQ, Form("targetQ[%d]/D", kNTargetChannels));
  tree->Branch("targetVMax", block.targetVMax, Form("targetVMax[%d]/D", kNTargetChannels));
  tree->Branch("targetTMax", block.targetTMax, Form("targetTMax[%d]/D", kNTargetChannels));
  tree->Branch("lgQMean", &block.lgQMean, "lgQMean/D");
  tree->Branch("lgQStd", &block.lgQStd, "lgQStd/D");
  tree->Branch("lgQSem", &block.lgQSem, "lgQSem/D");
  tree->Branch("lgVMaxMean", &block.lgVMaxMean, "lgVMaxMean/D");
  tree->Branch("lgVMaxStd", &block.lgVMaxStd, "lgVMaxStd/D");
  tree->Branch("lgVMaxSem", &block.lgVMaxSem, "lgVMaxSem/D");
  tree->Branch("lgTMaxMean", &block.lgTMaxMean, "lgTMaxMean/D");
  tree->Branch("lgTMaxStd", &block.lgTMaxStd, "lgTMaxStd/D");
  tree->Branch("lgTMaxSem", &block.lgTMaxSem, "lgTMaxSem/D");

  return tree;
}

// -----------------------------------------------------------------------------
// Finalize a block and write it
// -----------------------------------------------------------------------------

void FinalizeBlock(const BlockAccumulator& accumulator, BlockRecord& block, TTree* blockTree){

  if (accumulator.Empty()) {
    return;
  }

  block.Reset();

  block.run = accumulator.run;
  block.blockId = accumulator.blockId;

  block.nEvents = accumulator.nEvents;
  block.complete = (accumulator.nEvents == kBlockSize);

  block.firstEvent = accumulator.firstEvent;
  block.lastEvent = accumulator.lastEvent;

  block.firstEventRunTime = accumulator.firstEventRunTime;
  block.lastEventRunTime = accumulator.lastEventRunTime;

  block.firstTimeUnix = accumulator.firstTimeUnix;
  block.lastTimeUnix = accumulator.lastTimeUnix;

  block.meanTimeUnix = accumulator.firstTimeUnix + accumulator.sumTimeOffset / (double)(accumulator.nEvents);
  array<double, kNSamples> averageWaveform{};

  for (int ch = 0; ch < kNTargetChannels; ++ch) {

    for (int s = 0; s < kNSamples; ++s) {
      averageWaveform[s] = accumulator.targetWaveSum[ch][s] / (double)(accumulator.nEvents);
    }
    const TargetWaveformResult reco = ReconstructTargetWaveform(averageWaveform);
    block.targetQ[ch] = reco.charge;
    block.targetVMax[ch] = reco.vMax;
    block.targetTMax[ch] = reco.tMaxNs;
  }

  MeanStdSem(accumulator.lgQSum, accumulator.lgQSumSq, accumulator.nEvents, block.lgQMean, block.lgQStd, block.lgQSem);
  MeanStdSem(accumulator.lgVMaxSum, accumulator.lgVMaxSumSq, accumulator.nEvents, block.lgVMaxMean, block.lgVMaxStd, block.lgVMaxSem);
  MeanStdSem(accumulator.lgTMaxSum, accumulator.lgTMaxSumSq, accumulator.nEvents, block.lgTMaxMean, block.lgTMaxStd, block.lgTMaxSem);

  blockTree->Fill();
}


// -----------------------------------------------------------------------------
// Command-line help
// -----------------------------------------------------------------------------

void PrintUsage(const char* executable)
{
  cout
      << "\nUsage:\n"
      << "  " << executable
      << " [-i <file>] [-l <list>] [-o <file>]"
      << " [-n <n>] [-e <event>] [-v] [-h]\n\n"

      << "Options:\n"
      << "  -i <file>   Add one input ROOT file. Can be repeated.\n"
      << "  -l <list>   Text file containing one input ROOT file per line.\n"
      << "  -o <file>   Output ROOT file. Default: RecoTarget.root\n"
      << "  -n <n>      Maximum number of accepted events to write.\n"
      << "              0 means no limit.\n"
      << "  -e <event>  Process only this event number. Can be repeated.\n"
      << "  -v          Increase verbosity. Can be repeated.\n"
      << "  -h          Show this help.\n\n"

      << "Output trees:\n"
      << "  Events : one entry per accepted event\n"
      << "  Blocks : one entry per "
      << kBlockSize
      << " accepted events; final partial block is also written\n\n";
}

} // namespace


// =============================================================================
// main
// =============================================================================

int main(int argc, char* argv[]){
  
  int verbose = 0;
  int maxAcceptedEvents = 0;

  TString outputFileName = "RecoTarget.root";
  TObjArray inputFileNameList;
  vector<Int_t> selectedEvents;

  struct stat fileStat;

  int option = 0;

  while ((option = getopt(argc, argv, "i:l:o:n:e:vh")) != -1) {

    switch (option) {

      case 'i':
        inputFileNameList.Add(new TObjString(optarg));
        cout << "Added input data file '" << optarg << "'" << endl;
        break;

      case 'l': {
        if (stat(optarg, &fileStat) != 0) {
          cerr << "ERROR - file list '" << optarg << "' is not accessible" << endl;
          return 1;
        }

        ifstream inputList(optarg);

        if (!inputList) {
          cerr << "ERROR - cannot open file list '" << optarg << "'" << endl;
          return 1;
        }

        cout << "Reading list of input files from '" << optarg << "'" << endl;

        string line;

        while (getline(inputList, line)) {

          if (line.empty()) {
            continue;
          }

          TString fileName(line.c_str());

          if (IsRemoteFile(fileName) || stat(fileName.Data(), &fileStat) == 0) {
            inputFileNameList.Add( new TObjString(fileName));
            if (verbose) {
              cout << "Added input data file '" << fileName << "'" << endl;
            }
          } else {
            cerr << "WARNING - file '" << fileName << "' is not accessible" << endl;
          }
        }
        break;
      }

      case 'o':
        outputFileName = optarg;
        break;

      case 'n':
        if (sscanf(optarg, "%d", &maxAcceptedEvents) != 1 || maxAcceptedEvents < 0) {
          cerr << "ERROR - invalid value for -n: " << optarg << endl;
          return 1;
        }
        break;

      case 'e': {
        Int_t eventNumber = -1;
        if (sscanf(optarg, "%d", &eventNumber) != 1 || eventNumber < 0) {
          cerr << "ERROR - invalid event number: " << optarg << endl;
          return 1;
        }

        selectedEvents.push_back(eventNumber);
        break;
      }

      case 'v':
        ++verbose;
        break;

      case 'h':
        PrintUsage(argv[0]);
        return 0;

      default:
        PrintUsage(argv[0]);
        return 1;
    }
  }


  // ---------------------------------------------------------------------------
  // Validate input
  // ---------------------------------------------------------------------------

  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR - input file list is empty" << endl;
    return 1;
  }


  // ---------------------------------------------------------------------------
  // Build the input chain
  // ---------------------------------------------------------------------------

  const TString rawMergedTreeName = "RawMergedEvents";
  TChain inputChain(rawMergedTreeName);

  cout << "=== Input files ===" << endl;

  for (Int_t i = 0; i < inputFileNameList.GetEntries(); ++i) {
    const TString fileName = static_cast<TObjString*>(inputFileNameList.At(i))->GetString();
    cout << "  " << i << "  " << fileName  << endl;
    inputChain.AddFile(fileName);
  }

  const Long64_t inputEntries = inputChain.GetEntries();

  if (inputEntries <= 0) {
    cerr << "ERROR - tree '" << rawMergedTreeName << "' has no entries" << endl;
    return 1;
  }

  const int nBranches = inputChain.GetListOfBranches() ? inputChain.GetListOfBranches()->GetEntries() : 0;
  cout << "Found tree " << rawMergedTreeName << " with " << nBranches << " branches and " << inputEntries << " entries" << endl;

  // ---------------------------------------------------------------------------
  // Input branch
  // ---------------------------------------------------------------------------

  TRawMergedEvent* rawMergedEvent = new TRawMergedEvent();

  inputChain.SetBranchAddress("RawMergedEvent", &rawMergedEvent);

  // ---------------------------------------------------------------------------
  // Output file and trees
  // ---------------------------------------------------------------------------

  TFile* outputFile = TFile::Open(outputFileName, "RECREATE");

  if (!outputFile || outputFile->IsZombie()) {

    cerr << "ERROR - cannot create output file " << outputFileName << endl;
    delete rawMergedEvent;
    if (outputFile) delete outputFile;

    return 1;
  }

  outputFile->cd();

  EventRecord eventRecord;
  eventRecord.Reset();

  BlockRecord blockRecord;
  blockRecord.Reset();

  TTree* eventTree = CreateEventTree(eventRecord);
  TTree* blockTree = CreateBlockTree(blockRecord);

  // ---------------------------------------------------------------------------
  // Event-loop buffers
  // ---------------------------------------------------------------------------

  array<array<double, kNSamples>, kNTargetChannels> targetRaw{};
  array<double, kNSamples> lgRaw{};

  BlockAccumulator block;
  Long64_t nextBlockId = 0;
  block.Reset(nextBlockId);

  Long64_t acceptedEvents = 0;
  Long64_t rejectedMissingDetector = 0;
  Long64_t rejectedTrigger = 0;

  // ---------------------------------------------------------------------------
  // Main loop
  // ---------------------------------------------------------------------------

  cout << "=== Starting reconstruction ===" << endl;

  for (Long64_t entry = 0; entry < inputEntries; ++entry) {

    inputChain.GetEntry(entry);
    if (!rawMergedEvent) continue;

    TRawEvent* rawEvent = rawMergedEvent->GetTRawEvent();
    if (!rawEvent) continue;

    const UInt_t triggerMask = rawEvent->GetEventTrigMask();

    // Accept trigger bit 0, as in the original reconstruction.
    if (!(triggerMask & 0x01)) {
      ++rejectedTrigger;
      continue;
    }

    const Int_t runNumber = rawEvent->GetRunNumber();
    const Int_t eventNumber = rawEvent->GetEventNumber();

    // Optional explicit event-number selection.
    if (!selectedEvents.empty() && count(selectedEvents.begin(), selectedEvents.end(), eventNumber) == 0) {
      continue;
    }

    // Do not let one block span different runs.
    if (!block.Empty() && runNumber != block.run) {
      if (verbose) {
        cout << "Run changed from " << block.run << " to " << runNumber << ": writing partial block " << block.blockId << " with " << block.nEvents << " events" << endl;
      }
      FinalizeBlock(block, blockRecord, blockTree);

      ++nextBlockId;
      block.Reset(nextBlockId);
    }
    const ULong64_t eventRunTime = rawEvent->GetEventRunTime();
    const Double_t absTimeUnix = rawEvent->GetEventAbsTime().AsDouble();

    // -------------------------------------------------------------------------
    // Extract both detector waveforms.
    //
    // IMPORTANT:
    // The event is accepted only if both LG and all 32 Target channels are
    // available.  Therefore Events and Blocks always refer to the same event
    // population for the two detectors.
    // -------------------------------------------------------------------------

    const bool detectorsOK = ExtractDetectorWaveforms(rawEvent, targetRaw, lgRaw, verbose);

    if (!detectorsOK) {
      ++rejectedMissingDetector;
      continue;
    }

    // -------------------------------------------------------------------------
    // Reconstruct Lead Glass
    // -------------------------------------------------------------------------

    const LeadGlassWaveformResult lgReco = ReconstructLeadGlassWaveform(lgRaw);

    // -------------------------------------------------------------------------
    // Fill event-level Target quantities
    // -------------------------------------------------------------------------

    eventRecord.Reset();

    eventRecord.run = runNumber;
    eventRecord.event = eventNumber;

    eventRecord.blockId = block.blockId;
    eventRecord.indexInBlock = block.nEvents;
    eventRecord.triggerMask = triggerMask;
    eventRecord.eventRunTime = eventRunTime;
    eventRecord.absTimeUnix = absTimeUnix;

    for (int ch = 0; ch < kNTargetChannels; ++ch) {
      const TargetWaveformResult targetReco = ReconstructTargetWaveform(targetRaw[ch]);
      eventRecord.targetQOld[ch] = targetReco.chargeLegacy;
      eventRecord.targetQ[ch] = targetReco.charge;
      eventRecord.targetVMax[ch] = targetReco.vMax;
      eventRecord.targetTMax[ch] = targetReco.tMaxNs;
    }
    eventRecord.lgQ = lgReco.charge;
    eventRecord.lgVMax = lgReco.vMax;
    eventRecord.lgTMax = lgReco.tMaxNs;

    // -------------------------------------------------------------------------
    // The event is now officially accepted.
    // Add it to both the Events tree and the current block.
    // -------------------------------------------------------------------------

    eventTree->Fill();

    block.AddEvent(runNumber, eventNumber, eventRunTime, absTimeUnix, targetRaw, lgReco);
    ++acceptedEvents;

    // -------------------------------------------------------------------------
    // Close a complete block
    // -------------------------------------------------------------------------

    if (block.Full()) {
      FinalizeBlock(block, blockRecord, blockTree);
      if (verbose > 1) {
        cout << "Wrote complete block " << block.blockId << " with " << block.nEvents << " events" << endl;
      }
      ++nextBlockId;
      block.Reset(nextBlockId);
    }

    // -------------------------------------------------------------------------
    // Progress
    // -------------------------------------------------------------------------

    if (acceptedEvents % 1000 == 0) {
      cout << "Processed " << acceptedEvents << " accepted events" << "  [input entry " << entry + 1 << "/" << inputEntries << "]" << endl;
    }

    // -------------------------------------------------------------------------
    // Optional accepted-event limit
    // -------------------------------------------------------------------------

    if (maxAcceptedEvents > 0 && acceptedEvents >= maxAcceptedEvents) break;

  }


  // ---------------------------------------------------------------------------
  // Save the final partial block, if any.
  //
  // It has complete == false and nEvents < kBlockSize.
  // This preserves the event -> blockId relation for every written event.
  // Later physics analysis can simply require complete == true.
  // ---------------------------------------------------------------------------

  if (!block.Empty()) {
    if (verbose) cout << "Writing final partial block " << block.blockId << " with " << block.nEvents << " events" << endl;
    FinalizeBlock(block, blockRecord, blockTree);
  }

  // ---------------------------------------------------------------------------
  // Write output
  // ---------------------------------------------------------------------------

  outputFile->cd();

  eventTree->Write();
  blockTree->Write();

  const Long64_t nEventTreeEntries = eventTree->GetEntries();
  const Long64_t nBlockTreeEntries = blockTree->GetEntries();

  outputFile->Write();
  outputFile->Close();

  // ---------------------------------------------------------------------------
  // Summary
  // ---------------------------------------------------------------------------

  cout
      << "\n=== Reconstruction summary ===\n"
      << "Input entries                  : "
      << inputEntries << "\n"
      << "Accepted events                : "
      << acceptedEvents << "\n"
      << "Rejected by trigger            : "
      << rejectedTrigger << "\n"
      << "Rejected: missing detector data: "
      << rejectedMissingDetector << "\n"
      << "Event-tree entries             : "
      << nEventTreeEntries << "\n"
      << "Block-tree entries             : "
      << nBlockTreeEntries << "\n"
      << "Output file                    : "
      << outputFileName << "\n"
      << endl;


  delete rawMergedEvent;
  delete outputFile;

  return 0;
}
