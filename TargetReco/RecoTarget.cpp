#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"
#include "TParameter.h"

#include "TRawEvent.hh"
#include "TRawMergedEvent.hh"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
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

// Blocks are defined by external UNIX-time boundaries, not by an event count.

// Target charge integration window.
// The original code used: t > 200 && t < 700.
constexpr int kTargetIntegrationFirst = 200;
constexpr int kTargetIntegrationLast = 699;

// Lead Glass charge integration window.
// The original code used: t > 100 && t < 600.
// Katerina code uses: t > 70 && t < 600.

constexpr int kLeadGlassIntegrationFirst = 70; // 100;
constexpr int kLeadGlassIntegrationLast = 599;

constexpr double kVpp = 1.0;
constexpr double kADCCounts = 4096.0;
constexpr double kTargetImpedanceOhm = 50.0;

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
  Long64_t blockId = -1;           // Time-bin index, 0 ... nBins-1.
  Int_t indexInBlock = -1;         // Index among accepted events in this time bin.
  UInt_t triggerMask = 0;
  ULong64_t eventRunTime = 0;
  Double_t timeFromStart = 0.0;    // Seconds relative to externally supplied tStartUnix.

  Double_t targetQOld[kNTargetChannels];
  Double_t targetQ[kNTargetChannels];
  Double_t targetVMax[kNTargetChannels];
  Double_t targetTMax[kNTargetChannels];
  Double_t lgQ = kInvalid;
  Double_t lgVMax = kInvalid;
  Double_t lgTMax = kInvalid;

  void Reset(){
    run = -1;
    event = -1;
    blockId = -1;
    indexInBlock = -1;
    triggerMask = 0;
    eventRunTime = 0;
    timeFromStart = 0.0;
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
  Int_t run = 0;
  Long64_t blockId = -1;
  Bool_t hasEvents = false;
  Int_t nEvents = 0;
  Int_t firstEvent = 0;
  Int_t lastEvent = 0;
  ULong64_t firstEventRunTime = 0;
  ULong64_t lastEventRunTime = 0;

  // All times below are relative to tStartUnix.
  Double_t binStart = 0.0;
  Double_t binStop = 0.0;
  Double_t binCenter = 0.0;
  
  // hasEvents distinguishes a bin with no events from an event-derived measurement.
  Double_t firstTime = 0.0;
  Double_t meanTime = 0.0;
  Double_t lastTime = 0.0;
  Double_t meanTimeInBin = 0.0;
  Double_t meanTimeInBinErr = 0.0; // SEM of event times within this bin [s].

  Double_t targetQ[kNTargetChannels];
  Double_t targetVMax[kNTargetChannels];
  Double_t targetTMax[kNTargetChannels];
  Double_t lgQMean = 0.0;
  Double_t lgQStd = 0.0;
  Double_t lgQSem = 0.0;
  Double_t lgVMaxMean = 0.0;
  Double_t lgVMaxStd = 0.0;
  Double_t lgVMaxSem = 0.0;
  Double_t lgTMaxMean = 0.0;
  Double_t lgTMaxStd = 0.0;
  Double_t lgTMaxSem = 0.0;

  void Reset(){
    run = 0;
    blockId = -1;
    hasEvents = false;
    nEvents = 0;
    firstEvent = 0;
    lastEvent = 0;
    firstEventRunTime = 0;
    lastEventRunTime = 0;
    binStart = 0.0;
    binStop = 0.0;
    binCenter = 0.0;
    firstTime = 0.0;
    meanTime = 0.0;
    lastTime = 0.0;
    meanTimeInBin = 0.0;
    meanTimeInBinErr = 0.0;
    fill_n(targetQ, kNTargetChannels, 0.0);
    fill_n(targetVMax, kNTargetChannels, 0.0);
    fill_n(targetTMax, kNTargetChannels, 0.0);
    lgQMean = 0.0;
    lgQStd = 0.0;
    lgQSem = 0.0;
    lgVMaxMean = 0.0;
    lgVMaxStd = 0.0;
    lgVMaxSem = 0.0;
    lgTMaxMean = 0.0;
    lgTMaxStd = 0.0;
    lgTMaxSem = 0.0;
  }
};

// -----------------------------------------------------------------------------
// Block accumulator
// -----------------------------------------------------------------------------
struct BlockAccumulator {
  Long64_t blockId = 0;
  Int_t run = 0;
  Int_t nEvents = 0;
  Int_t firstEvent = 0;
  Int_t lastEvent = 0;
  ULong64_t firstEventRunTime = 0;
  ULong64_t lastEventRunTime = 0;
  Double_t binStart = 0.0;       // Relative to tStartUnix.
  Double_t firstTime = 0.0;      // Relative to tStartUnix.
  Double_t lastTime = 0.0;       // Relative to tStartUnix.
  Double_t sumTimeInBin = 0.0;   // Sum of (eventTime - binStart), each <= binWidth.
  Double_t sumTimeInBinSq = 0.0; // Sum of squared offsets within the bin [s^2].
  array<array<double, kNSamples>, kNTargetChannels> targetWaveSum{};
  double lgQSum = 0.0;
  double lgQSumSq = 0.0;
  double lgVMaxSum = 0.0;
  double lgVMaxSumSq = 0.0;
  double lgTMaxSum = 0.0;
  double lgTMaxSumSq = 0.0;

  void Reset(Long64_t newBlockId, Double_t newBinStart){
    blockId = newBlockId;
    run = 0;
    nEvents = 0;
    firstEvent = 0;
    lastEvent = 0;
    firstEventRunTime = 0;
    lastEventRunTime = 0;
    binStart = newBinStart;
    firstTime = 0.0;
    lastTime = 0.0;
    sumTimeInBin = 0.0;
    sumTimeInBinSq = 0.0;
    for (auto& channel : targetWaveSum) channel.fill(0.0);
    lgQSum = 0.0;
    lgQSumSq = 0.0;
    lgVMaxSum = 0.0;
    lgVMaxSumSq = 0.0;
    lgTMaxSum = 0.0;
    lgTMaxSumSq = 0.0;
  }

  bool Empty() const { return nEvents == 0; }

  void AddEvent(Int_t eventRun, Int_t eventNumber, ULong64_t eventRunTime, Double_t timeFromStart, const array<array<double, kNSamples>, kNTargetChannels>& targetRaw, const LeadGlassWaveformResult& lg){
    if (nEvents == 0) {
      run = eventRun;
      firstEvent = eventNumber;
      firstEventRunTime = eventRunTime;
      firstTime = timeFromStart;
    }

    lastEvent = eventNumber;
    lastEventRunTime = eventRunTime;
    lastTime = timeFromStart;
    const double timeInBin = timeFromStart - binStart;
    sumTimeInBin += timeInBin;
    sumTimeInBinSq += timeInBin * timeInBin;

    for (int ch = 0; ch < kNTargetChannels; ++ch) {
      for (int s = 0; s < kNSamples; ++s) targetWaveSum[ch][s] += targetRaw[ch][s];
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
  return name.BeginsWith("root://") || name.BeginsWith("http://") || name.BeginsWith("https://") || name.BeginsWith("xroot://") || name.BeginsWith("davs://");
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
      // integrate only positive charges
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
    // not converted directly to mV.
    const double signal = kVpp * (pedestal - samples[s]);

    if (signal > vMax) {
      vMax = signal;
      tMaxSample = s;
    }

    if (s >= kLeadGlassIntegrationFirst && s <= kLeadGlassIntegrationLast) {
      charge += signal * kLeadGlassChargeFactor; // this factor contains the proper rescaling to pC --> CHECK against Katerina's reco
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
TTree* CreateEventTree(EventRecord& event){
  TTree* tree = new TTree("Events", "Event-level Target and Lead Glass reconstruction");
  tree->Branch("run", &event.run, "run/I");
  tree->Branch("event", &event.event, "event/I");
  tree->Branch("blockId", &event.blockId, "blockId/L");
  tree->Branch("indexInBlock", &event.indexInBlock, "indexInBlock/I");
  tree->Branch("triggerMask", &event.triggerMask, "triggerMask/i");
  tree->Branch("eventRunTime", &event.eventRunTime, "eventRunTime/l");
  tree->Branch("timeFromStart", &event.timeFromStart, "timeFromStart/D");
  tree->Branch("targetQOld", event.targetQOld, Form("targetQOld[%d]/D", kNTargetChannels));
  tree->Branch("targetQ", event.targetQ, Form("targetQ[%d]/D", kNTargetChannels));
  tree->Branch("targetVMax", event.targetVMax, Form("targetVMax[%d]/D", kNTargetChannels));
  tree->Branch("targetTMax", event.targetTMax, Form("targetTMax[%d]/D", kNTargetChannels));
  tree->Branch("lgQ", &event.lgQ, "lgQ/D");
  tree->Branch("lgVMax", &event.lgVMax, "lgVMax/D");
  tree->Branch("lgTMax", &event.lgTMax, "lgTMax/D");
  return tree;
}


TTree* CreateBlockTree(BlockRecord& block){
  TTree* tree = new TTree("Blocks", "Fixed UNIX-time-bin cumulative Target and Lead Glass reconstruction");
  tree->Branch("run", &block.run, "run/I");
  tree->Branch("blockId", &block.blockId, "blockId/L");
  tree->Branch("hasEvents", &block.hasEvents, "hasEvents/O");
  tree->Branch("nEvents", &block.nEvents, "nEvents/I");
  tree->Branch("firstEvent", &block.firstEvent, "firstEvent/I");
  tree->Branch("lastEvent", &block.lastEvent, "lastEvent/I");
  tree->Branch("firstEventRunTime", &block.firstEventRunTime, "firstEventRunTime/l");
  tree->Branch("lastEventRunTime", &block.lastEventRunTime, "lastEventRunTime/l");
  tree->Branch("binStart", &block.binStart, "binStart/D");
  tree->Branch("binStop", &block.binStop, "binStop/D");
  tree->Branch("binCenter", &block.binCenter, "binCenter/D");
  tree->Branch("firstTime", &block.firstTime, "firstTime/D");
  tree->Branch("meanTime", &block.meanTime, "meanTime/D");
  tree->Branch("lastTime", &block.lastTime, "lastTime/D");
  tree->Branch("meanTimeInBin", &block.meanTimeInBin, "meanTimeInBin/D");
  tree->Branch("meanTimeInBinErr", &block.meanTimeInBinErr, "meanTimeInBinErr/D");
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
// Edges and bincenter are relative to tStartUnix
void FinalizeTimeBin(const BlockAccumulator& accumulator, BlockRecord& block, TTree* blockTree, Int_t expectedRun, Double_t binStart, Double_t binStop){
  block.Reset();
  block.run = expectedRun;
  block.blockId = accumulator.blockId;
  block.binStart = binStart;
  block.binStop = binStop;
  block.binCenter = binStart + 0.5 * (binStop - binStart);
  block.nEvents = accumulator.nEvents;
  block.hasEvents = (accumulator.nEvents > 0);

  // Empty bins have their nominal time coordinate - all physics values stay zero.
  // firstTime/lastTime use the centre as placeholders, NOT actual event timestamps.
  if (!block.hasEvents) {
    block.firstTime = block.binCenter;
    block.meanTime = block.binCenter;
    block.lastTime = block.binCenter;
    block.meanTimeInBin = block.binCenter - block.binStart;
    blockTree->Fill();
    return;
  }

  block.run = accumulator.run;
  block.firstEvent = accumulator.firstEvent;
  block.lastEvent = accumulator.lastEvent;
  block.firstEventRunTime = accumulator.firstEventRunTime;
  block.lastEventRunTime = accumulator.lastEventRunTime;
  block.firstTime = accumulator.firstTime;
  block.lastTime = accumulator.lastTime;
  const double n = static_cast<double>(accumulator.nEvents);
  block.meanTimeInBin = accumulator.sumTimeInBin / n;
  block.meanTime = binStart + block.meanTimeInBin;
  // SEM = sqrt((<tau^2> - <tau>^2)/(N-1)); tau = event.timeFromStart - binStart.
  // For N=0 the empty-bin branch above leaves zero; for N=1 the SEM is undefined and stored as zero.
  if (accumulator.nEvents > 1) {
    const double meanTimeInBinSq = accumulator.sumTimeInBinSq / n;
    const double varianceOfMean = (meanTimeInBinSq - block.meanTimeInBin * block.meanTimeInBin) / (n - 1.0);
    block.meanTimeInBinErr = sqrt(max(0.0, varianceOfMean));
  }

  array<double, kNSamples> averageWaveform{};
  for (int ch = 0; ch < kNTargetChannels; ++ch) {
    for (int s = 0; s < kNSamples; ++s) averageWaveform[s] = accumulator.targetWaveSum[ch][s] / static_cast<double>(accumulator.nEvents);
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
void PrintUsage(const char* executable){
  cout << "\nUsage:\n  " << executable << " [-i <file>] [-l <list>] [-o <file>] -s <tStartUnix> -t <tStopUnix> -b <nBins> [-n <n>] [-e <event>] [-v] [-h]\n\n"
       << "  -s <time>   Inclusive UNIX start time (seconds). Required.\n"
       << "  -t <time>   Exclusive UNIX stop time (seconds). Required.\n"
       << "  -b <n>      Number of equal-width time bins. Required.\n"
       << "  -i <file>   Add input ROOT file; repeatable.\n"
       << "  -l <list>   Input ROOT file list.\n"
       << "  -o <file>   Output ROOT file (default: RecoTarget.root).\n"
       << "  -n <n>      Stop after N accepted events (TEST ONLY: remaining bins then appear empty).\n"
       << "  -e <event>  Reconstruct a selected event number; repeatable (TEST ONLY).\n"
       << "  -v          Increase verbosity; repeatable.\n"
       << "  -h          Print this help.\n\n"
       << "All per-entry times are seconds relative to tStartUnix.\n"
       << "The file stores tStartUnix/tStopUnix once as ROOT TParameter<double>.\n"
       << "Blocks always has exactly nBins entries, including zero-valued empty bins.\n\n";
}

} // namespace


// =============================================================================
// main
// =============================================================================

int main(int argc, char* argv[]){
  int verbose = 0;
  int maxAcceptedEvents = 0;
  int nBins = 0;
  double tStartUnix = 0.0;
  double tStopUnix = 0.0;
  bool haveTStart = false;
  bool haveTStop = false;
  bool haveNBins = false;

  TString outputFileName = "RecoTarget.root";
  TObjArray inputFileNameList;
  vector<Int_t> selectedEvents;
  struct stat fileStat;

  int option = 0;
  while ((option = getopt(argc, argv, "i:l:o:n:e:s:t:b:vh")) != -1) {
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
          if (line.empty()) continue;
          TString fileName(line.c_str());
          if (IsRemoteFile(fileName) || stat(fileName.Data(), &fileStat) == 0) {
            inputFileNameList.Add(new TObjString(fileName));
            if (verbose) cout << "Added input data file '" << fileName << "'" << endl;
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
          cerr << "ERROR - invalid -n: " << optarg << endl;
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

      case 's': {
        char* end = nullptr;
        tStartUnix = strtod(optarg, &end);
        if (end == optarg || *end != '\0' || !isfinite(tStartUnix)) {
          cerr << "ERROR - invalid -s UNIX timestamp: " << optarg << endl;
          return 1;
        }
        haveTStart = true;
        break;
      }

      case 't': {
        char* end = nullptr;
        tStopUnix = strtod(optarg, &end);
        if (end == optarg || *end != '\0' || !isfinite(tStopUnix)) {
          cerr << "ERROR - invalid -t UNIX timestamp: " << optarg << endl;
          return 1;
        }
        haveTStop = true;
        break;
      }

      case 'b': {
        char* end = nullptr;
        const long parsed = strtol(optarg, &end, 10);
        if (end == optarg || *end != '\0' || parsed < 1 || parsed > numeric_limits<int>::max()) {
          cerr << "ERROR - invalid -b number of bins: " << optarg << endl;
          return 1;
        }
        nBins = static_cast<int>(parsed);
        haveNBins = true;
        break;
      }

      case 'v': ++verbose; break;
      case 'h': PrintUsage(argv[0]); return 0;
      default: PrintUsage(argv[0]); return 1;
    }
  }

  if (!haveTStart || !haveTStop || !haveNBins || tStopUnix <= tStartUnix) {
    cerr << "ERROR - required: -s tStartUnix -t tStopUnix -b nBins, with tStopUnix > tStartUnix" << endl;
    return 1;
  }
  const double duration = tStopUnix - tStartUnix;
  const double binWidth = duration / static_cast<double>(nBins);
  if (!isfinite(binWidth) || binWidth <= 0.0 || tStartUnix + binWidth == tStartUnix) {
    cerr << "ERROR - time bin is too narrow for the absolute UNIX timestamp precision" << endl;
    return 1;
  }
  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR - input file list is empty" << endl;
    return 1;
  }
  if (maxAcceptedEvents > 0 || !selectedEvents.empty()) {
    cerr << "WARNING - -n/-e are test modes: zero-entry bins can mean 'not processed', not necessarily 'no events'" << endl;
  }
  cout << setprecision(17) << "Time selection: [" << tStartUnix << ", " << tStopUnix << ") UNIX seconds, nBins=" << nBins << ", binWidth=" << binWidth << " s" << endl;

  // ---------------------------------------------------------------------------
  // Input chain and detector data.
  // ---------------------------------------------------------------------------
  TChain inputChain("RawMergedEvents");
  for (Int_t i = 0; i < inputFileNameList.GetEntries(); ++i) {
    const TString fileName = static_cast<TObjString*>(inputFileNameList.At(i))->GetString();
    cout << "  input " << i << ": " << fileName << endl;
    inputChain.AddFile(fileName);
  }
  const Long64_t inputEntries = inputChain.GetEntries();
  if (inputEntries <= 0) {
    cerr << "ERROR - RawMergedEvents tree is empty" << endl;
    return 1;
  }
  TRawMergedEvent* rawMergedEvent = new TRawMergedEvent();
  inputChain.SetBranchAddress("RawMergedEvent", &rawMergedEvent);

  // ---------------------------------------------------------------------------
  // Output ROOT file: store the absolute time anchor ONCE, not in every event/bin.
  // ---------------------------------------------------------------------------
  TFile* outputFile = TFile::Open(outputFileName, "RECREATE");
  if (!outputFile || outputFile->IsZombie()) {
    cerr << "ERROR - cannot create output file " << outputFileName << endl;
    delete rawMergedEvent;
    if (outputFile) delete outputFile;
    return 1;
  }
  outputFile->cd();
  TParameter<double>("tStartUnix", tStartUnix).Write();
  TParameter<double>("tStopUnix", tStopUnix).Write();
  TParameter<int>("nBins", nBins).Write();
  TParameter<double>("binWidthSeconds", binWidth).Write();

  EventRecord eventRecord;
  eventRecord.Reset();
  BlockRecord blockRecord;
  blockRecord.Reset();
  TTree* eventTree = CreateEventTree(eventRecord);
  TTree* blockTree = CreateBlockTree(blockRecord);

  array<array<double, kNSamples>, kNTargetChannels> targetRaw{};
  array<double, kNSamples> lgRaw{};
  BlockAccumulator block;
  int currentBinId = 0;
  block.Reset(currentBinId, 0.0);

  Int_t observedRun = 0;
  bool haveObservedRun = false;
  double lastAcceptedTime = -numeric_limits<double>::infinity();
  Long64_t acceptedEvents = 0;
  Long64_t rejectedTrigger = 0;
  Long64_t rejectedMissingDetector = 0;
  Long64_t rejectedOutsideTime = 0;
  Long64_t emptyBins = 0;
  Long64_t consumedInputEntries = 0;
  bool fatal = false;
  bool inputTruncated = false;

  // ---------------------------------------------------------------------------
  // Process accepted events chronologically, with ONE active bin accumulator.
  // ---------------------------------------------------------------------------
  for (Long64_t entry = 0; entry < inputEntries; ++entry) {
    consumedInputEntries = entry + 1;
    inputChain.GetEntry(entry);
    if (!rawMergedEvent) continue;
    TRawEvent* rawEvent = rawMergedEvent->GetTRawEvent();
    if (!rawEvent) continue;

    const Int_t runNumber = rawEvent->GetRunNumber();
    const Int_t eventNumber = rawEvent->GetEventNumber();
    if (!haveObservedRun) {
      observedRun = runNumber;
      haveObservedRun = true;
    } else if (runNumber != observedRun) {
      cerr << "ERROR - input contains multiple runs (" << observedRun << " and " << runNumber << "). One file/chain must describe one run." << endl;
      fatal = true;
      break;
    }

    const UInt_t triggerMask = rawEvent->GetEventTrigMask();
    if (!(triggerMask & 0x01)) {
      ++rejectedTrigger;
      continue;
    }
    if (!selectedEvents.empty() && count(selectedEvents.begin(), selectedEvents.end(), eventNumber) == 0) continue;

    const double absTimeUnix = rawEvent->GetEventAbsTime().AsDouble();
    const double timeFromStart = absTimeUnix - tStartUnix;
    if (!isfinite(timeFromStart) || timeFromStart < 0.0 || timeFromStart >= duration) {
      ++rejectedOutsideTime;
      continue;
    }

    // An event enters Events AND Blocks only if the same Target/LG detector data exist.
    const bool detectorsOK = ExtractDetectorWaveforms(rawEvent, targetRaw, lgRaw, verbose);
    if (!detectorsOK) {
      ++rejectedMissingDetector;
      continue;
    }

    // The single-accumulator approach requires accepted-event timestamps in order.
    if (timeFromStart < lastAcceptedTime) {
      cerr << setprecision(17) << "ERROR - out-of-order timestamp at raw entry " << entry << ": t=" << timeFromStart << " s, previous accepted t=" << lastAcceptedTime << " s" << endl;
      fatal = true;
      break;
    }

    // Finalize ALL elapsed bins. If a bin had no events, FinalizeTimeBin writes zeros
    // Calculate bin boundaries relative to tStartUnix
    while (currentBinId < nBins - 1 && timeFromStart >= (static_cast<double>(currentBinId + 1) * binWidth)) {
      const double binStart = static_cast<double>(currentBinId) * binWidth;
      const double binStop = static_cast<double>(currentBinId + 1) * binWidth;
      if (block.Empty()) ++emptyBins;
      FinalizeTimeBin(block, blockRecord, blockTree, observedRun, binStart, binStop);
      ++currentBinId;
      block.Reset(currentBinId, static_cast<double>(currentBinId) * binWidth);
    }

    const double activeBinStart = static_cast<double>(currentBinId) * binWidth;
    const double activeBinStop = (currentBinId == nBins - 1) ? duration : static_cast<double>(currentBinId + 1) * binWidth;
    if (timeFromStart < activeBinStart || timeFromStart >= activeBinStop) {
      cerr << setprecision(17) << "ERROR - event timestamp " << timeFromStart << " s does not belong to active bin " << currentBinId << " [" << activeBinStart << ", " << activeBinStop << ")" << endl;
      fatal = true;
      break;
    }

    const LeadGlassWaveformResult lgReco = ReconstructLeadGlassWaveform(lgRaw);
    eventRecord.Reset();
    eventRecord.run = runNumber;
    eventRecord.event = eventNumber;
    eventRecord.blockId = currentBinId;
    eventRecord.indexInBlock = block.nEvents;
    eventRecord.triggerMask = triggerMask;
    eventRecord.eventRunTime = rawEvent->GetEventRunTime();
    eventRecord.timeFromStart = timeFromStart;
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

    eventTree->Fill();
    block.AddEvent(runNumber, eventNumber, eventRecord.eventRunTime, timeFromStart, targetRaw, lgReco);
    lastAcceptedTime = timeFromStart;
    ++acceptedEvents;

    if (acceptedEvents % 10000 == 0) cout << "Processed " << acceptedEvents << " accepted events [raw entry " << entry + 1 << "/" << inputEntries << "]" << endl;
    if (maxAcceptedEvents > 0 && acceptedEvents >= maxAcceptedEvents) {
      inputTruncated = (entry + 1 < inputEntries);
      break;
    }
  }

  // Do not leave a misleading partially valid ROOT file on chronological/run errors.
  if (fatal) {
    outputFile->Close();
    delete outputFile;
    delete rawMergedEvent;
    std::remove(outputFileName.Data());
    return 2;
  }

  // ---------------------------------------------------------------------------
  // Always write exactly nBins Blocks entries, including empty bins.
  // ---------------------------------------------------------------------------
  for (int binId = currentBinId; binId < nBins; ++binId) {
    const double binStart = static_cast<double>(binId) * binWidth;
    const double binStop = (binId == nBins - 1) ? duration : static_cast<double>(binId + 1) * binWidth;
    if (block.Empty()) ++emptyBins;
    FinalizeTimeBin(block, blockRecord, blockTree, observedRun, binStart, binStop);
    if (binId + 1 < nBins) block.Reset(binId + 1, static_cast<double>(binId + 1) * binWidth);
  }

  if (blockTree->GetEntries() != nBins) {
    cerr << "ERROR - internal binning failure: expected " << nBins << " Blocks entries, got " << blockTree->GetEntries() << endl;
    outputFile->Close();
    delete outputFile;
    delete rawMergedEvent;
    std::remove(outputFileName.Data());
    return 2;
  }

  // The -n/-e options only reconstruct a subset; zero bins beyond it may be unexamined.
  TParameter<int>("inputTruncated", (inputTruncated || !selectedEvents.empty()) ? 1 : 0).Write();
  outputFile->cd();
  eventTree->Write();
  blockTree->Write();
  const Long64_t nEventTreeEntries = eventTree->GetEntries();
  const Long64_t nBlockTreeEntries = blockTree->GetEntries();
  outputFile->Close();

  cout << "\n=== Reconstruction summary ===\n"
       << "Input entries                     : " << inputEntries << "\n"
       << "Input entries examined            : " << consumedInputEntries << "\n"
       << "Accepted events                   : " << acceptedEvents << "\n"
       << "Rejected by trigger               : " << rejectedTrigger << "\n"
       << "Rejected outside [tStart,tStop)   : " << rejectedOutsideTime << "\n"
       << "Rejected: missing detector data   : " << rejectedMissingDetector << "\n"
       << "Events tree entries               : " << nEventTreeEntries << "\n"
       << "Blocks tree entries               : " << nBlockTreeEntries << "\n"
       << "Empty bins                        : " << emptyBins << "\n"
       << "Output ROOT file                  : " << outputFileName << "\n";
  if (inputTruncated || !selectedEvents.empty()) cout << "WARNING: input partially examined; empty bins may mean unprocessed data\n";
  cout << endl;

  delete rawMergedEvent;
  delete outputFile;
  return 0;
}
