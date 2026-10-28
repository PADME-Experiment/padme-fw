#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"
#include "TParameter.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TDirectory.h"

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
#include <stdexcept>
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
// Output tree only Block-based 
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
  Long64_t blockId = -1;
  Int_t nEvents = 0;            // nEvents == 0 identifies an empty bin.
  Int_t firstEvent = 0;
  Int_t lastEvent = 0;

  // Seconds relative to the file-level tStartUnix parameter.
  // For empty bins, firstTime/meanTime/lastTime are the nominal bin centre.
  Double_t firstTime = 0.0;
  Double_t meanTime = 0.0;
  Double_t lastTime = 0.0;
  Double_t meanTimeInBinErr = 0.0; // SEM of event timestamps within bin [s].

  Double_t targetQ[kNTargetChannels];
  Double_t targetVMax[kNTargetChannels];
  Double_t targetTMax[kNTargetChannels];
  // Sufficient event-charge statistics to reproduce uncertainties without an Events tree.
  Double_t targetQEventMean[kNTargetChannels];
  Double_t targetQEventM2[kNTargetChannels];  // SEM derivable from M2 and nEvents.
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
    blockId = -1;
    nEvents = 0;
    firstEvent = 0;
    lastEvent = 0;
    firstTime = 0.0;
    meanTime = 0.0;
    lastTime = 0.0;
    meanTimeInBinErr = 0.0;
    fill_n(targetQ, kNTargetChannels, 0.0);
    fill_n(targetVMax, kNTargetChannels, 0.0);
    fill_n(targetTMax, kNTargetChannels, 0.0);
    fill_n(targetQEventMean, kNTargetChannels, 0.0);
    fill_n(targetQEventM2, kNTargetChannels, 0.0);
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
  Int_t nEvents = 0;
  Int_t firstEvent = 0;
  Int_t lastEvent = 0;
  Double_t binStart = 0.0;       // Relative to tStartUnix.
  Double_t firstTime = 0.0;      // Relative to tStartUnix.
  Double_t lastTime = 0.0;       // Relative to tStartUnix.
  Double_t sumTimeInBin = 0.0;   // Sum of (eventTime - binStart), each <= binWidth.
  Double_t sumTimeInBinSq = 0.0; // Sum of squared offsets within the bin [s^2].
  array<array<double, kNSamples>, kNTargetChannels> targetWaveSum{};
  array<double, kNTargetChannels> targetQEventMean{};
  array<double, kNTargetChannels> targetQEventM2{};
  double lgQSum = 0.0;
  double lgQSumSq = 0.0;
  double lgVMaxSum = 0.0;
  double lgVMaxSumSq = 0.0;
  double lgTMaxSum = 0.0;
  double lgTMaxSumSq = 0.0;

  void Reset(Long64_t newBlockId, Double_t newBinStart){
    blockId = newBlockId;
    nEvents = 0;
    firstEvent = 0;
    lastEvent = 0;
    binStart = newBinStart;
    firstTime = 0.0;
    lastTime = 0.0;
    sumTimeInBin = 0.0;
    sumTimeInBinSq = 0.0;
    for (auto& channel : targetWaveSum) channel.fill(0.0);
    targetQEventMean.fill(0.0);
    targetQEventM2.fill(0.0);
    lgQSum = 0.0;
    lgQSumSq = 0.0;
    lgVMaxSum = 0.0;
    lgVMaxSumSq = 0.0;
    lgTMaxSum = 0.0;
    lgTMaxSumSq = 0.0;
  }

  bool Empty() const { return nEvents == 0; }

  void AddEvent(Int_t eventNumber, Double_t timeFromStart, const array<array<double, kNSamples>, kNTargetChannels>& targetRaw, const double* eventTargetQ, const LeadGlassWaveformResult& lg){
    if (nEvents == 0) {
      firstEvent = eventNumber;
      firstTime = timeFromStart;
    }

    lastEvent = eventNumber;
    lastTime = timeFromStart;
    const double timeInBin = timeFromStart - binStart;
    sumTimeInBin += timeInBin;
    sumTimeInBinSq += timeInBin * timeInBin;

    for (int ch = 0; ch < kNTargetChannels; ++ch) {
      for (int s = 0; s < kNSamples; ++s) targetWaveSum[ch][s] += targetRaw[ch][s];
    }

// nEvents is the number BEFORE adding this event.
const double nextCount = static_cast<double>(nEvents + 1);
for (int ch = 0; ch < kNTargetChannels; ++ch) {
      const double delta = eventTargetQ[ch] - targetQEventMean[ch];
      targetQEventMean[ch] += delta / nextCount;
      targetQEventM2[ch] += delta * (eventTargetQ[ch] - targetQEventMean[ch]);
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
TTree* CreateBlockTree(BlockRecord& block){
  TTree* tree = new TTree("Blocks", "Fixed UNIX-time-bin cumulative Target and Lead Glass reconstruction");
  tree->Branch("blockId", &block.blockId, "blockId/L");
  tree->Branch("nEvents", &block.nEvents, "nEvents/I");
  tree->Branch("firstEvent", &block.firstEvent, "firstEvent/I");
  tree->Branch("lastEvent", &block.lastEvent, "lastEvent/I");
  tree->Branch("firstTime", &block.firstTime, "firstTime/D");
  tree->Branch("meanTime", &block.meanTime, "meanTime/D");
  tree->Branch("lastTime", &block.lastTime, "lastTime/D");
  tree->Branch("meanTimeInBinErr", &block.meanTimeInBinErr, "meanTimeInBinErr/D");
  tree->Branch("targetQ", block.targetQ, Form("targetQ[%d]/D", kNTargetChannels));
  tree->Branch("targetVMax", block.targetVMax, Form("targetVMax[%d]/D", kNTargetChannels));
  tree->Branch("targetTMax", block.targetTMax, Form("targetTMax[%d]/D", kNTargetChannels));
  tree->Branch("targetQEventMean", block.targetQEventMean, Form("targetQEventMean[%d]/D", kNTargetChannels));
  tree->Branch("targetQEventM2", block.targetQEventM2, Form("targetQEventM2[%d]/D", kNTargetChannels));
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
// Event based monitoring - th1-th2 e poi TGraphs--> to be optimized still!
// -----------------------------------------------------------------------------
struct EventMonitoring {
  TDirectory* directory = nullptr;
  TH2D* qOldVsChannel = nullptr;
  TH2D* qVsChannel = nullptr;
  TH2D* vMaxVsChannel = nullptr;
  TH2D* tMaxVsChannel = nullptr;
  TH1D* lgQ = nullptr;
  TH1D* lgVMax = nullptr;
  TH1D* lgTMax = nullptr;
  TH1D* time = nullptr;
  TH1D* triggerMask = nullptr;
  TH2D* lgQVsTime = nullptr;
  TH2D* lgQVsTargetQSum = nullptr;
  TH2D* triggerMaskVsTime = nullptr;

  EventMonitoring(TFile* file, int nBins, double duration) {
    TDirectory* dir = file->mkdir("monitoring");
    if (!dir) throw runtime_error("Cannot create ROOT monitoring directory");
    directory = dir;
    dir->cd();
    qOldVsChannel = new TH2D("hTargetQOldVsChannel", "Event Target legacy charge;ADC channel;Q_{old} [pC]", 32, -0.5, 31.5, 600, -300., 300.);
    qVsChannel = new TH2D("hTargetQVsChannel", "Event Target charge;ADC channel;Q [pC]", 32, -0.5, 31.5, 600, 0., 600.);
    vMaxVsChannel = new TH2D("hTargetVMaxVsChannel", "Event Target maximum signal;ADC channel;V_{max} [mV]", 32, -0.5, 31.5, 500, -50., 450.);
    tMaxVsChannel = new TH2D("hTargetTMaxVsChannel", "Event Target peak time;ADC channel;t_{max} [ns]", 32, -0.5, 31.5, kNSamples, 0., kNSamples * kSamplePeriodNs);
    lgQ = new TH1D("hLGQ", "Event Lead Glass charge;Q_{LG} [pC];Events", 600, -100., 2900.);
    lgVMax = new TH1D("hLGVMax", "Event Lead Glass maximum;V_{max,LG} [a.u.];Events", 500, -0.1, 1.1);
    lgTMax = new TH1D("hLGTMax", "Event Lead Glass peak time;t_{max,LG} [ns];Events", kNSamples, 0., kNSamples * kSamplePeriodNs);
    time = new TH1D("hEventTimeFromStart", "Accepted events vs time;t-t_{start} [s];Events", nBins, 0., duration);
    triggerMask = new TH1D("hTriggerMask", "Accepted event trigger mask;Trigger mask;Events", 256, -0.5, 255.5);
    // Event-level correlations remain available as bounded-size TH2D histograms.
    // The time axis is the same set of externally defined bins as the Blocks tree.
    lgQVsTime = new TH2D("hLGQVsTime", "Lead Glass event charge vs time;t-t_{start} [s];Q_{LG} [pC]", nBins, 0., duration, 300, -100., 2900.);
    lgQVsTargetQSum = new TH2D("hLGQVsTargetQSum", "Lead Glass vs total Target event charge;Q_{Target,sum} [pC];Q_{LG} [pC]", 300, 0., 3000., 300, -100., 2900.);
    triggerMaskVsTime = new TH2D("hTriggerMaskVsTime", "Accepted event trigger mask vs time;t-t_{start} [s];Trigger mask", nBins, 0., duration, 256, -0.5, 255.5);
    file->cd(); // Keep Blocks tree in the file's top-level directory.
  }

  void Write() {
    directory->cd();
    qOldVsChannel->Write();
    qVsChannel->Write();
    vMaxVsChannel->Write();
    tMaxVsChannel->Write();
    lgQ->Write();
    lgVMax->Write();
    lgTMax->Write();
    time->Write();
    triggerMask->Write();
    lgQVsTime->Write();
    lgQVsTargetQSum->Write();
    triggerMaskVsTime->Write();
    directory->cd("..");
  }

  // Target-charge sum is calculated only for the LG/Target correlation histogram.
  void Fill(const EventRecord& event) {
    double totalQ = 0.0;
    for (int ch = 0; ch < kNTargetChannels; ++ch) {
      qOldVsChannel->Fill(ch, event.targetQOld[ch]);
      qVsChannel->Fill(ch, event.targetQ[ch]);
      totalQ += event.targetQ[ch];
      vMaxVsChannel->Fill(ch, event.targetVMax[ch]);
      tMaxVsChannel->Fill(ch, event.targetTMax[ch]);
    }
    lgQ->Fill(event.lgQ);
    lgVMax->Fill(event.lgVMax);
    lgTMax->Fill(event.lgTMax);
    time->Fill(event.timeFromStart);
    triggerMask->Fill(static_cast<double>(event.triggerMask));
    lgQVsTime->Fill(event.timeFromStart, event.lgQ);
    lgQVsTargetQSum->Fill(totalQ, event.lgQ);
    triggerMaskVsTime->Fill(event.timeFromStart, static_cast<double>(event.triggerMask));
  }
};

// Two event/time TGraphs; ALL accepted events by default.
// TGraphs filled in the event loop 
struct EventTimeGraphs {
  TDirectory* directory = nullptr;
  TGraph eventNumberVsTime;
  TGraph lgQVsTime;
  Long64_t recorded = 0;
  const int stride;

  EventTimeGraphs(TFile* file, int graphStride) : stride(graphStride) {
    directory = file->mkdir("event_trends");
    if (!directory) throw runtime_error("Cannot create ROOT event_trends directory");
    eventNumberVsTime.SetName("gEventNumberVsTime");
    eventNumberVsTime.SetTitle("Event number vs accepted event time;t-t_{start} [s];Event number");
    lgQVsTime.SetName("gLGQVsTime");
    lgQVsTime.SetTitle("Lead Glass charge vs accepted event time;t-t_{start} [s];Q_{LG} [pC]");
  }

  void Fill(const EventRecord& event, Long64_t acceptedIndex) {
    if (acceptedIndex % stride != 0) return;
    // TGraph::SetPoint takes Int_t indices, so guard against integer overflow.
    if (recorded >= numeric_limits<int>::max()) {
      throw runtime_error("Too many TGraph points; rerun with a larger -g stride");
    }
    const int i = static_cast<int>(recorded++);
    eventNumberVsTime.SetPoint(i, event.timeFromStart, static_cast<double>(event.event));
    lgQVsTime.SetPoint(i, event.timeFromStart, event.lgQ);
  }

  void Write() {
    directory->cd();
    eventNumberVsTime.Write();
    lgQVsTime.Write();
    directory->cd("..");
  }
};

// -----------------------------------------------------------------------------
// Finalize a block and write it
// -----------------------------------------------------------------------------
// Bin edges are computed from the run's fixed time interval and are not stored.
void FinalizeTimeBin(const BlockAccumulator& accumulator, BlockRecord& block, TTree* blockTree, Double_t binStart, Double_t binStop){
  block.Reset();
  block.blockId = accumulator.blockId;
  block.nEvents = accumulator.nEvents;

  // Empty bins use the nominal bin centre as a time placeholder.
  // nEvents == 0 distinguishes placeholders from measured event timestamps.
  if (block.nEvents == 0) {
    const double binCenter = binStart + 0.5 * (binStop - binStart);
    block.firstTime = binCenter;
    block.meanTime = binCenter;
    block.lastTime = binCenter;
    blockTree->Fill();
    return;
  }

  block.firstEvent = accumulator.firstEvent;
  block.lastEvent = accumulator.lastEvent;
  block.firstTime = accumulator.firstTime;
  block.lastTime = accumulator.lastTime;
  const double n = static_cast<double>(accumulator.nEvents);
  const double meanTimeInBin = accumulator.sumTimeInBin / n;
  block.meanTime = binStart + meanTimeInBin;
  // SEM = sqrt((<tau^2> - <tau>^2)/(N-1)); tau = event.timeFromStart - binStart.
  // For N<=1 the SEM is stored as zero by convention.
  if (accumulator.nEvents > 1) {
    const double meanTimeInBinSq = accumulator.sumTimeInBinSq / n;
    const double varianceOfMean = (meanTimeInBinSq - meanTimeInBin * meanTimeInBin) / (n - 1.0);
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

  for (int ch = 0; ch < kNTargetChannels; ++ch) {
    block.targetQEventMean[ch] = accumulator.targetQEventMean[ch];
    block.targetQEventM2[ch] = accumulator.targetQEventM2[ch];
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
  cout << "\nUsage:\n  " << executable << " [-i <file>] [-l <list>] [-o <file>] -s <tStartUnix> -t <tStopUnix> -b <nBins> [-g <stride>] [-n <n>] [-e <event>] [-v] [-h]\n\n"
       << "  -s <time>   Inclusive UNIX start time (seconds). Required.\n"
       << "  -t <time>   Exclusive UNIX stop time (seconds). Required.\n"
       << "  -b <n>      Number of equal-width time bins. Required.\n"
       << "  -i <file>   Add input ROOT file; repeatable.\n"
       << "  -l <list>   Input ROOT file list.\n"
       << "  -o <file>   Output ROOT file (default: RecoTarget.root).\n"
       << "  -n <n>      Stop after N accepted events (TEST ONLY: remaining bins then appear empty).\n"
       << "  -e <event>  Reconstruct a selected event number; repeatable (TEST ONLY).\n"
       << "  -g <stride> Record every Nth accepted event in the two TGraphs (default: 1 = all).\n"
       << "  -v          Increase verbosity; repeatable.\n"
       << "  -h          Print this help.\n\n"
       << "All per-entry times are seconds relative to tStartUnix.\n"
       << "The file stores tStartUnix/tStopUnix once as ROOT TParameter<double>.\n"
       << "Blocks always has exactly nBins entries, including zero-valued empty bins.\n"
       << "No Events tree is written; run scalars are TParameters (GetVal()),\n"
       << "event distributions are TH1/TH2, event time correlations are TGraphs.\n\n";
}

} // namespace


// =============================================================================
// main
// =============================================================================

int main(int argc, char* argv[]){
  int verbose = 0;
  int maxAcceptedEvents = 0;
  int graphStride = 1;
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
  while ((option = getopt(argc, argv, "i:l:o:n:e:s:t:b:g:vh")) != -1) {
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

      case 'g': {
        char* end = nullptr;
        const long parsed = strtol(optarg, &end, 10);
        if (end == optarg || *end != '\0' || parsed < 1 || parsed > numeric_limits<int>::max()) {
          cerr << "ERROR - invalid -g graph sampling stride: " << optarg << endl;
          return 1;
        }
        graphStride = static_cast<int>(parsed);
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
  TTree* blockTree = CreateBlockTree(blockRecord);
  EventMonitoring monitoring(outputFile, nBins, duration);
  EventTimeGraphs eventGraphs(outputFile, graphStride);

  array<array<double, kNSamples>, kNTargetChannels> targetRaw{};
  array<double, kNSamples> lgRaw{};
  BlockAccumulator block;
  int currentBinId = 0;
  block.Reset(currentBinId, 0.0);

  Int_t observedRun = 0;
  bool haveObservedRun = false;
  Int_t firstAcceptedEvent = -1;
  Int_t lastAcceptedEvent = -1;
  ULong64_t firstAcceptedEventRunTime = 0;
  ULong64_t lastAcceptedEventRunTime = 0;
  double firstAcceptedTime = kInvalid;
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

    // An event contributes to monitoring AND Blocks only if both detector data exist.
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
      FinalizeTimeBin(block, blockRecord, blockTree, binStart, binStop);
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

    monitoring.Fill(eventRecord);
    eventGraphs.Fill(eventRecord, acceptedEvents);
    if (acceptedEvents == 0) {
      firstAcceptedEvent = eventNumber;
      firstAcceptedEventRunTime = eventRecord.eventRunTime;
      firstAcceptedTime = timeFromStart;
    }
    lastAcceptedEvent = eventNumber;
    lastAcceptedEventRunTime = eventRecord.eventRunTime;
    block.AddEvent(eventNumber, timeFromStart, targetRaw, eventRecord.targetQ, lgReco);
    lastAcceptedTime = timeFromStart;
    ++acceptedEvents;

    if (acceptedEvents % 1000 == 0) cout << "Processed " << acceptedEvents << " accepted events [raw entry " << entry + 1 << "/" << inputEntries << "]" << endl;
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
    FinalizeTimeBin(block, blockRecord, blockTree, binStart, binStop);
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
  outputFile->cd();
  TParameter<int>("inputTruncated", (inputTruncated || !selectedEvents.empty()) ? 1 : 0).Write();
  // Scalars formerly implicit in event-level rows are stored once per run.
  // These can be read with static_cast<TParameter<T>*>(file->Get(name))->GetVal().
  TParameter<int>("runNumber", haveObservedRun ? observedRun : -1).Write();
  TParameter<Long64_t>("inputEntries", inputEntries).Write();
  TParameter<Long64_t>("consumedInputEntries", consumedInputEntries).Write();
  TParameter<Long64_t>("acceptedEvents", acceptedEvents).Write();
  TParameter<Long64_t>("rejectedTrigger", rejectedTrigger).Write();
  TParameter<Long64_t>("rejectedOutsideTime", rejectedOutsideTime).Write();
  TParameter<Long64_t>("rejectedMissingDetector", rejectedMissingDetector).Write();
  TParameter<Long64_t>("emptyBins", emptyBins).Write();
  TParameter<int>("firstAcceptedEvent", firstAcceptedEvent).Write();
  TParameter<int>("lastAcceptedEvent", lastAcceptedEvent).Write();
  TParameter<Long64_t>("firstAcceptedEventRunTime", static_cast<Long64_t>(firstAcceptedEventRunTime)).Write();
  TParameter<Long64_t>("lastAcceptedEventRunTime", static_cast<Long64_t>(lastAcceptedEventRunTime)).Write();
  TParameter<double>("firstAcceptedTimeFromStart", firstAcceptedTime).Write();
  TParameter<double>("lastAcceptedTimeFromStart", acceptedEvents ? lastAcceptedTime : kInvalid).Write();
  TParameter<int>("eventGraphStride", graphStride).Write();
  TParameter<Long64_t>("eventGraphPoints", eventGraphs.recorded).Write();
  blockTree->Write();
  monitoring.Write();
  eventGraphs.Write();
  const Long64_t nBlockTreeEntries = blockTree->GetEntries();
  outputFile->Close();

  cout << "\n=== Reconstruction summary ===\n"
       << "Input entries                     : " << inputEntries << "\n"
       << "Input entries examined            : " << consumedInputEntries << "\n"
       << "Accepted events                   : " << acceptedEvents << "\n"
       << "Rejected by trigger               : " << rejectedTrigger << "\n"
       << "Rejected outside [tStart,tStop)   : " << rejectedOutsideTime << "\n"
       << "Rejected: missing detector data   : " << rejectedMissingDetector << "\n"
       << "Accepted events in histograms     : " << acceptedEvents << "\n"
       << "Accepted events in TGraphs       : " << eventGraphs.recorded << " (stride " << graphStride << ")\n"
       << "Blocks tree entries               : " << nBlockTreeEntries << "\n"
       << "Empty bins                        : " << emptyBins << "\n"
       << "Output ROOT file                  : " << outputFileName << "\n";
  if (inputTruncated || !selectedEvents.empty()) cout << "WARNING: input partially examined; empty bins may mean unprocessed data\n";
  cout << endl;

  delete rawMergedEvent;
  delete outputFile;
  return 0;
}
