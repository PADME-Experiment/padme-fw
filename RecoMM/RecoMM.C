#define RecoMM_cxx
#include "RecoMM.h"

using namespace std;

namespace {

int TGraphAttribute(TGraphErrors *graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
  if (!graph) return 1;

  graph->SetTitle(title);
  graph->SetName(title);
  graph->GetXaxis()->SetTitle(xlabel);
  graph->GetYaxis()->SetTitle(ylabel);
  graph->SetMarkerStyle(markerstyle);
  graph->SetMarkerColor(color);

  return 0;
}

} // namespace

// -----------------------------------------------------------------------------
// MM geometry and signal reconstruction
// -----------------------------------------------------------------------------

double RecoMM::StripToX(int iStrip, int iLayer) const{
  if (iLayer < 0 || iLayer >= MM_N_Layers) return -999.;
  if (iStrip < 0 || iStrip >= MAXSTRIP) return -999.;

  if (iStrip <= 256) return (iStrip - 256.) * pitch + pitch / 2. + shift_coord[iLayer] - geo_hole[iLayer] / 2.;
  return (iStrip - 256.) * pitch + pitch / 2. + shift_coord[iLayer] + geo_hole[iLayer] / 2.;
}

double RecoMM::LayerXMin(int iLayer) const{
  return StripToX(0, iLayer) - pitch / 2.;
}

double RecoMM::LayerXMax(int iLayer) const{
  return StripToX(MAXSTRIP - 1, iLayer) + pitch / 2.;
}

int RecoMM::LayerNBinsX(int iLayer) const{
  if (iLayer < 0 || iLayer >= MM_N_Layers) return MAXSTRIP;

  // The physical span is MAXSTRIP*pitch + geo_hole.
  // Because geo_hole/pitch is integer for the current geometry, this keeps the
  // physical-position histogram bin width exactly equal to one strip pitch and
  // leaves the central inactive region as empty physical bins.
  const int nGapBins = (int)std::lround(geo_hole[iLayer] / pitch);
  return MAXSTRIP + std::max(0, nGapBins);
}

void RecoMM::CoordinateFinder(int iStrip, int iLayer, const vector<short> &camp, double &t_strip, double &x_strip, double &z_strip, double &q_strip){
  t_strip = 0.;
  x_strip = -999.;
  z_strip = -999.;
  q_strip = 0.;

  if (iLayer < 0 || iLayer >= MM_N_Layers) return;
  if (iStrip < 0 || iStrip >= MAXSTRIP) return;
  if (camp.empty()) return;

  const int nBins = (int)camp.size();

  double qtot = 0.;
  double qmax = -1.e9;

  for (int ibin = 0; ibin < nBins; ++ibin) {
    const double q = (double)camp[ibin];
    qtot += q;
    if (q > qmax) qmax = q;
  }

  const double threshold = 0.2 * qtot / (double)nBins;

  double weightedTime = 0.;
  double qtotT = 0.;

  for (int ibin = 0; ibin < nBins; ++ibin) {
    const double qbin = (double)camp[ibin];
    if (qbin < threshold) continue;

    const double tbin = ibin * clock + clock / 2.;
    weightedTime += qbin * tbin;
    qtotT += qbin;
  }

  if (qtotT > 0.) t_strip = weightedTime / qtotT;

  x_strip = StripToX(iStrip, iLayer);

  const double t0 = 0.;
  const double z_ion = 2.;

  if (iLayer > 3) {
    z_strip = zm + z_ion - (t_strip - t0) * vd;
  } else {
    z_strip = (t_strip - t0) * vd - zm - z_ion;
  }

  q_strip = qmax;
}

// -----------------------------------------------------------------------------
// Main reconstruction
// -----------------------------------------------------------------------------

void RecoMM::LoopFileList(TObjArray &inputFileNameList, int NevtBlock){
  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR: empty input file list" << endl;
    return;
  }

  if (NevtBlock <= 0) {
    cerr << "ERROR: NevtBlock must be > 0. Current value = " << NevtBlock << endl;
    return;
  }

  if (!fTree) {
    cerr << "ERROR: fTree is null in RecoMM::LoopFileList" << endl;
    return;
  }

  const Long64_t runNEntries = fTree->GetEntries();
  if (runNEntries <= 0) {
    cerr << "ERROR: input chain has zero entries" << endl;
    return;
  }

  Long64_t nToProcess = runNEntries;
  if (maxEvents > 0 && maxEvents < runNEntries) {
    nToProcess = maxEvents;
  }

  cout << "Number of input files: " << inputFileNameList.GetEntries() << endl;
  cout << "Found Tree 'apv_raw' with " << runNEntries << " entries" << endl;
  cout << "Will process " << nToProcess << " entries" << endl;
  cout << "Block size: " << NevtBlock << " event IDs" << endl;
  cout << "No APV or strip calibration is applied in RecoMM" << endl;
  cout << "Saturation threshold for NoSat products: " << SaturationThreshold << " ADC counts" << endl;

  // ---------------------------------------------------------------------------
  // Output file and directory structure
  // ---------------------------------------------------------------------------
  cout << "#### Creating output file " << outputFileName << endl;

  TFile *outfile = new TFile(outputFileName.Data(), "RECREATE");
  if (!outfile || outfile->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outputFileName << endl;
    if (outfile) delete outfile;
    return;
  }

  TDirectory *overallDir = outfile->mkdir("Overall");
  TDirectory *rawOverallDir = overallDir->mkdir("Raw");
  TDirectory *timeOverallDir = overallDir->mkdir("Time");

  TDirectory *blockDir = outfile->mkdir("Blocks");
  TDirectory *blockMonitoringDir = blockDir->mkdir("Monitoring");

  auto GetBlockRawDir = [&](int iblock) -> TDirectory * {
    blockDir->cd();

    TString blockName = Form("block_%04d", iblock);
    TDirectory *thisBlockDir = blockDir->GetDirectory(blockName);
    if (!thisBlockDir) thisBlockDir = blockDir->mkdir(blockName);

    TDirectory *rawDir = thisBlockDir->GetDirectory("Raw");
    if (!rawDir) rawDir = thisBlockDir->mkdir("Raw");

    return rawDir;
  };

  // ---------------------------------------------------------------------------
  // Overall RAW products
  // ---------------------------------------------------------------------------
  const int nChargeBins = 2500;
  const int nNoSatBins = std::max(1, (int)std::lround(SaturationThreshold));

  for (int l = 0; l < MM_N_Layers; ++l) {
    const int nXBins = LayerNBinsX(l);
    const double xmin = LayerXMin(l);
    const double xmax = LayerXMax(l);

    hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", mm_tag[l].Data()), TString("q_{max} vs physical position [") + mm_tag[l] + "]", nXBins, xmin, xmax, nChargeBins, 0., 2500.);
    hqmaxstripFull[l]->SetDirectory(nullptr);
    hqmaxstripFull[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
    hqmaxstripFull[l]->SetYTitle("q_{max} [ADC counts]");

    hqmaxstripFull_NoSat[l] = new TH2F(Form("hqmaxstripFull_NoSat%s", mm_tag[l].Data()), TString("q_{max} vs physical position, no saturation [") + mm_tag[l] + "]", nXBins, xmin, xmax, nNoSatBins, 0., SaturationThreshold);
    hqmaxstripFull_NoSat[l]->SetDirectory(nullptr);
    hqmaxstripFull_NoSat[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
    hqmaxstripFull_NoSat[l]->SetYTitle("q_{max} [ADC counts]");

    // hQmaxSumNoSatStrip[l] = new TH1D(Form("hQmaxSumNoSatStrip%s", mm_tag[l].Data()), TString("sum of non-saturated q_{max} vs strip [") + mm_tag[l] + "]", MAXSTRIP, -0.5, MAXSTRIP - 0.5);
    // hQmaxSumNoSatStrip[l]->SetXTitle(Form("%s strip", mm_tag[l].Data()));
    // hQmaxSumNoSatStrip[l]->SetYTitle("#Sigma q_{max}, q_{max}<sat [ADC counts]");
    hQmaxSumNoSatStrip[l] = nullptr; // will be the hqmaxstripFull_NoSat->ProjectionX()

    htimePositionFull[l] = new TH2F(Form("htimePositionFull%s", mm_tag[l].Data()), TString("time vs physical position [") + mm_tag[l] + "]", nXBins, xmin, xmax, 750, -50., 700.);
    htimePositionFull[l]->SetDirectory(nullptr);
    htimePositionFull[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
    htimePositionFull[l]->SetYTitle("time [ns]");

    hzPositionFull[l] = new TH2F(Form("hzPositionFull%s", mm_tag[l].Data()), TString("z vs physical position [") + mm_tag[l] + "]", nXBins, xmin, xmax, 400, -100., 100.);
    hzPositionFull[l]->SetDirectory(nullptr);
    hzPositionFull[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
    hzPositionFull[l]->SetYTitle("z [mm]");

    g_BlockMaxChargeStrip[l] = new TGraphErrors();
    TGraphAttribute(g_BlockMaxChargeStrip[l], Form("g_BlockMaxChargeStrip_%s", mm_tag[l].Data()), "block ID", Form("%s strip at ProjectionX maximum (NoSat)", mm_tag[l].Data()), 20, kBlue + l);

    g_BlockMaxChargePosition[l] = new TGraphErrors();
    TGraphAttribute(g_BlockMaxChargePosition[l], Form("g_BlockMaxChargePosition_%s", mm_tag[l].Data()), "block ID", Form("%s position of max-charge strip [mm]", mm_tag[l].Data()), 20, kBlue + l);

    g_BlockMaxChargeValue[l] = new TGraphErrors();
    TGraphAttribute(g_BlockMaxChargeValue[l], Form("g_BlockMaxChargeValue_%s", mm_tag[l].Data()), "block ID", "ProjectionX maximum entries, NoSat [ADC counts]", 21, kRed + l);

    g_DaqTimeMaxChargeStrip[l] = new TGraphErrors();
    TGraphAttribute(g_DaqTimeMaxChargeStrip[l], Form("g_DaqTimeMaxChargeStrip_%s", mm_tag[l].Data()), "DAQ time [s]", Form("%s strip at ProjectionX maximum (NoSat)", mm_tag[l].Data()), 20, kBlue + l);

    g_DaqTimeMaxChargePosition[l] = new TGraphErrors();
    TGraphAttribute(g_DaqTimeMaxChargePosition[l], Form("g_DaqTimeMaxChargePosition_%s", mm_tag[l].Data()), "DAQ time [s]", Form("%s position of max-charge strip [mm]", mm_tag[l].Data()), 20, kBlue + l);

    g_DaqTimeMaxChargeValue[l] = new TGraphErrors();
    TGraphAttribute(g_DaqTimeMaxChargeValue[l], Form("g_DaqTimeMaxChargeValue_%s", mm_tag[l].Data()), "DAQ time [s]", "ProjectionX maximum entries, NoSat [ADC counts]", 21, kRed + l);
  }

  // Event/time association
  g_DaqTime_iev = new TGraphErrors();
  TGraphAttribute(g_DaqTime_iev,"g_DaqTime_iev", "Loop Index", "DAQ time [s]", 22, kBlack);

  g_SrsTimeStamp_evt = new TGraphErrors();
  TGraphAttribute(g_SrsTimeStamp_evt,"g_SrsTimeStamp_evt", "EvtID", "SRS timestamp", 22, kBlack);

  g_DaqTimeSec_evt = new TGraphErrors();
  TGraphAttribute(g_DaqTimeSec_evt,"g_DaqTimeSec_evt", "EvtID", "DAQ time sec [s]", 22, kBlack);

  g_DaqTimeMicroSec_evt = new TGraphErrors();
  TGraphAttribute(g_DaqTimeMicroSec_evt,"g_DaqTimeMicroSec_evt", "EvtID", "DAQ time microsec [#mus]", 22, kBlack);

  g_DaqTime_evt = new TGraphErrors();
  TGraphAttribute(g_DaqTime_evt,"g_DaqTime_evt", "EvtID", "DAQ time [s]", 22, kBlack);

  g_evt_vs_iev = new TGraphErrors();
  TGraphAttribute(g_evt_vs_iev,"g_evt_vs_iev", "Loop Index", "EvtID", 22, kBlack);

  g_BlockMeanDaqSec = new TGraphErrors();
  TGraphAttribute(g_BlockMeanDaqSec,"g_BlockMeanDaqSec", "Block ID", "Mean DAQ sec [s]", 22, kBlack);

  g_BlockMeanDaqTime = new TGraphErrors();
  TGraphAttribute(g_BlockMeanDaqTime, "g_BlockMeanDaqTime", "Block ID", "Mean DAQ time [s]", 22, kBlack);

  g_BlockMeanSrsTime = new TGraphErrors();
  TGraphAttribute(g_BlockMeanSrsTime, "g_BlockMeanSrsTime", "Block ID", "Mean SRS timestamp", 22, kBlack);

  blockTimeInfo.clear();

  // ---------------------------------------------------------------------------
  // Streaming block histograms
  // ---------------------------------------------------------------------------
  TH2F *currentBlockRawFull[MM_N_Layers] = {nullptr};
  TH2F *currentBlockRawFullNoSat[MM_N_Layers] = {nullptr};
  TH1D *currentBlockQmaxSumNoSatStrip[MM_N_Layers] = {nullptr};

  auto EnsureBlockContainers = [&](int iblock) {
    if ((int)blockTimeInfo.size() <= iblock) {
      blockTimeInfo.resize(iblock + 1);
    }
  };

  auto CreateRawBlock = [&](int iblock) {
    for (int l = 0; l < MM_N_Layers; ++l) {
      const int nXBins = LayerNBinsX(l);
      const double xmin = LayerXMin(l);
      const double xmax = LayerXMax(l);

      currentBlockRawFull[l] = new TH2F(Form("hBlockqmaxstripFull%s_block%04d", mm_tag[l].Data(), iblock), TString("q_{max} vs physical position [") + mm_tag[l] + Form("] block %04d", iblock), nXBins, xmin, xmax, nChargeBins, 0., 2500.);
      currentBlockRawFull[l]->SetDirectory(nullptr);
      currentBlockRawFull[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
      currentBlockRawFull[l]->SetYTitle("q_{max} [ADC counts]");

      currentBlockRawFullNoSat[l] = new TH2F(Form("hBlockqmaxstripFull_NoSat%s_block%04d", mm_tag[l].Data(), iblock), TString("q_{max} vs physical position, no saturation [") + mm_tag[l] + Form("] block %04d", iblock), nXBins, xmin, xmax, nNoSatBins, 0., SaturationThreshold);
      currentBlockRawFullNoSat[l]->SetDirectory(nullptr);
      currentBlockRawFullNoSat[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
      currentBlockRawFullNoSat[l]->SetYTitle("q_{max} [ADC counts]");

      // currentBlockQmaxSumNoSatStrip[l] = new TH1D(Form("hBlockQmaxSumNoSatStrip%s_block%04d", mm_tag[l].Data(), iblock), TString("sum of non-saturated q_{max} vs strip [") + mm_tag[l] + Form("] block %04d", iblock), MAXSTRIP, -0.5, MAXSTRIP - 0.5);
      // currentBlockQmaxSumNoSatStrip[l]->SetDirectory(nullptr);
      // currentBlockQmaxSumNoSatStrip[l]->SetXTitle(Form("%s strip", mm_tag[l].Data()));
      // currentBlockQmaxSumNoSatStrip[l]->SetYTitle("#Sigma q_{max}, q_{max}<sat [ADC counts]");
    }
  };

  auto FinalizeRawBlock = [&](int iblock) {
    cout << "Finalizing RAW block " << iblock << endl;

    TDirectory *rawBlockDir = GetBlockRawDir(iblock);

    for (int l = 0; l < MM_N_Layers; ++l) {
      // ------------------------------------------------------------
      // Seed for the later analysis:
      // ProjectionX of the NON-SATURATED q_max vs physical-position histogram.
      // No beam fit is performed in reconstruction.
      // ------------------------------------------------------------
      if (currentBlockRawFullNoSat[l]) {
        currentBlockQmaxSumNoSatStrip[l] = currentBlockRawFullNoSat[l]->ProjectionX(Form("hBlockQmaxSumNoSatStrip%s_block%04d", mm_tag[l].Data(), iblock));
        currentBlockQmaxSumNoSatStrip[l]->SetDirectory(nullptr);
        currentBlockQmaxSumNoSatStrip[l]->SetTitle(TString("ProjectionX of non-saturated q_{max} vs physical position [") + mm_tag[l] + Form("] block %04d", iblock));
        currentBlockQmaxSumNoSatStrip[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
        currentBlockQmaxSumNoSatStrip[l]->SetYTitle("entries");
      }
      if (currentBlockQmaxSumNoSatStrip[l] && currentBlockQmaxSumNoSatStrip[l]->GetMaximum() > 0.) {

        const int maxBin = currentBlockQmaxSumNoSatStrip[l]->GetMaximumBin();
        const double maxValue = currentBlockQmaxSumNoSatStrip[l]->GetBinContent(maxBin);

        if (maxValue > 0.) {
          const double maxPosition = currentBlockQmaxSumNoSatStrip[l]->GetXaxis()->GetBinCenter(maxBin);
          int maxStrip = -1;
          double minDistance = 1.e30;
          for (int iStrip = 0; iStrip < MAXSTRIP; ++iStrip) {
            const double distance = fabs(StripToX(iStrip, l) - maxPosition);
            if (distance < minDistance) {
              minDistance = distance;
              maxStrip = iStrip;
            }
          }
          int p = g_BlockMaxChargeStrip[l]->GetN();
          g_BlockMaxChargeStrip[l]->SetPoint(p, iblock, maxStrip);
          g_BlockMaxChargeStrip[l]->SetPointError(p, 0., 0.);

          p = g_BlockMaxChargePosition[l]->GetN();
          g_BlockMaxChargePosition[l]->SetPoint(p, iblock, maxPosition);
          g_BlockMaxChargePosition[l]->SetPointError(p, 0., 0.);

          p = g_BlockMaxChargeValue[l]->GetN();
          g_BlockMaxChargeValue[l]->SetPoint(p, iblock, maxValue);
          g_BlockMaxChargeValue[l]->SetPointError(p, 0., 0.);
        }
      }

      rawBlockDir->cd();

      if (currentBlockRawFull[l]) currentBlockRawFull[l]->Write();
      if (currentBlockRawFullNoSat[l]) currentBlockRawFullNoSat[l]->Write();
      if (currentBlockQmaxSumNoSatStrip[l]) currentBlockQmaxSumNoSatStrip[l]->Write();

      delete currentBlockRawFull[l];
      delete currentBlockRawFullNoSat[l];
      delete currentBlockQmaxSumNoSatStrip[l];

      currentBlockRawFull[l] = nullptr;
      currentBlockRawFullNoSat[l] = nullptr;
      currentBlockQmaxSumNoSatStrip[l] = nullptr;
    }

    outfile->Flush();
  };

  // ---------------------------------------------------------------------------
  // Single reconstruction pass
  // ---------------------------------------------------------------------------
  bool evtOriginSet = false;
  ULong64_t evtOrigin = 0;

  bool previousEvtSet = false;
  ULong64_t previousEvt = 0;

  int currentRawBlockId = -1;
  Long64_t recoEntry = 0;

  for (Long64_t iev = 0; iev < nToProcess; ++iev) {
    const Long64_t nb = fTree->GetEntry(iev);

    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }

    if (!mmLayer || !mmStrip || !raw_q) {
      cerr << "WARNING: null MM branch pointer at event " << iev << endl;
      continue;
    }

    if (!evtOriginSet) {
      evtOrigin = evt;
      evtOriginSet = true;
      cout << "Block event origin = " << evtOrigin << endl;
    }

    if (previousEvtSet && evt < previousEvt) {
      cerr << "ERROR: evt is not monotonic: previous=" << previousEvt << " current=" << evt  << " at tree entry=" << iev << endl;
      outfile->Close();
      delete outfile;
      return;
    }

    previousEvt = evt;
    previousEvtSet = true;

    const ULong64_t relativeEvt = evt - evtOrigin;
    const int iblock = (int)(relativeEvt / (ULong64_t)NevtBlock);

    EnsureBlockContainers(iblock);

    if (iblock != currentRawBlockId) {
      if (currentRawBlockId >= 0) FinalizeRawBlock(currentRawBlockId);

      currentRawBlockId = iblock;
      CreateRawBlock(currentRawBlockId);
    }

    if (recoEntry % 1000 == 0) {
      const double progress = nToProcess > 0 ? (double)recoEntry / (double)nToProcess : 0.;
      cout << "Processed " << recoEntry << " / " << nToProcess << " (" << fixed << setprecision(2) << 100. * progress << "%)" << endl;
    }

    const double daqTime = (double)daqTimeSec + 1.e-6 * (double)daqTimeMicroSec;

    BlockTimeInfo &bt = blockTimeInfo[iblock];

    if (bt.nEvents == 0) {
      bt.firstEvt = evt;
      bt.firstSrs = (double)srsTimeStamp;
      bt.firstDaq = daqTime;
      bt.firstDaqSec = (double)daqTimeSec;
    }

    bt.lastEvt = evt;
    bt.lastSrs = (double)srsTimeStamp;
    bt.lastDaq = daqTime;
    bt.lastDaqSec = (double)daqTimeSec;

    bt.sumSrs += (double)srsTimeStamp;
    bt.sumDaq += daqTime;
    bt.sumDaqSec += (double)daqTimeSec;
    bt.nEvents++;

    int pTime = g_DaqTime_iev->GetN();
    g_DaqTime_iev->SetPoint(pTime, iev, daqTime);

    pTime = g_SrsTimeStamp_evt->GetN();
    g_SrsTimeStamp_evt->SetPoint(pTime, evt, srsTimeStamp);
    g_DaqTimeSec_evt->SetPoint(pTime, evt, daqTimeSec);
    g_DaqTimeMicroSec_evt->SetPoint(pTime, evt, daqTimeMicroSec);
    g_DaqTime_evt->SetPoint(pTime, evt, daqTime);

    g_evt_vs_iev->SetPoint(iev, iev, evt);

    const int firedstrip_size = min(  (int)mmLayer->size(),  min((int)mmStrip->size(), (int)raw_q->size()));

    for (int j = 0; j < firedstrip_size; ++j) {
      const int layer = mmLayer->at(j);
      const int channel = mmStrip->at(j);

      if (layer < 0 || layer >= MM_N_Layers) continue;
      if (channel < 0 || channel >= MAXSTRIP) continue;

      double t_strip = 0.;
      double x_strip = 0.;
      double z_strip = 0.;
      double q_strip = 0.;

      CoordinateFinder(channel, layer, raw_q->at(j), t_strip, x_strip, z_strip, q_strip);

      // RAW always keeps the original q_max, including saturation.
      hqmaxstripFull[layer]->Fill(x_strip, q_strip);
      currentBlockRawFull[layer]->Fill(x_strip, q_strip);

      // Dedicated products for analyses that must reject saturation.
      if (q_strip < SaturationThreshold) {
        hqmaxstripFull_NoSat[layer]->Fill(x_strip, q_strip);
        currentBlockRawFullNoSat[layer]->Fill(x_strip, q_strip);

        // This profile is intentionally indexed by strip number, not physical x.
        // It is used only to identify a robust block-by-block beam seed.
        // hQmaxSumNoSatStrip[layer]->Fill(channel, q_strip);
      }

      htimePositionFull[layer]->Fill(x_strip, t_strip);
      hzPositionFull[layer]->Fill(x_strip, z_strip);
    }

    ++recoEntry;
  }

  if (currentRawBlockId >= 0) {
    FinalizeRawBlock(currentRawBlockId);
  }

  cout << "#### Total reconstructed events: " << recoEntry << endl;

  // ---------------------------------------------------------------------------
  // Block time summaries
  // ---------------------------------------------------------------------------
  for (size_t ib = 0; ib < blockTimeInfo.size(); ++ib) {
    BlockTimeInfo &bt = blockTimeInfo[ib];
    if (bt.nEvents <= 0) continue;

    bt.meanSrs = bt.sumSrs / (double)bt.nEvents;
    bt.meanDaq = bt.sumDaq / (double)bt.nEvents;
    bt.meanDaqSec = bt.sumDaqSec / (double)bt.nEvents;

    const double daqSecHalfWidth = 0.5 * fabs(bt.lastDaqSec - bt.firstDaqSec);
    const double daqHalfWidth = 0.5 * fabs(bt.lastDaq - bt.firstDaq);
    const double srsHalfWidth = 0.5 * fabs(bt.lastSrs - bt.firstSrs);

    int p = g_BlockMeanDaqSec->GetN();
    g_BlockMeanDaqSec->SetPoint(p, (double)ib, bt.meanDaqSec);
    g_BlockMeanDaqSec->SetPointError(p, 0., daqSecHalfWidth);

    p = g_BlockMeanDaqTime->GetN();
    g_BlockMeanDaqTime->SetPoint(p, (double)ib, bt.meanDaq);
    g_BlockMeanDaqTime->SetPointError(p, 0., daqHalfWidth);

    p = g_BlockMeanSrsTime->GetN();
    g_BlockMeanSrsTime->SetPoint(p, (double)ib, bt.meanSrs);
    g_BlockMeanSrsTime->SetPointError(p, 0., srsHalfWidth);
  }

  // ---------------------------------------------------------------------------
  // Convert max-charge-strip monitoring from block ID to DAQ time.
  // These are seeds/monitors only; final beam position and sigma stay in AnalysisMM.
  // ---------------------------------------------------------------------------
  for (int l = 0; l < MM_N_Layers; ++l) {
    const int nPoints = g_BlockMaxChargeStrip[l]->GetN();

    for (int ip = 0; ip < nPoints; ++ip) {
      double xBlock = 0.;
      double maxStrip = 0.;
      g_BlockMaxChargeStrip[l]->GetPoint(ip, xBlock, maxStrip);

      const int iblock = (int)std::lround(xBlock);
      if (iblock < 0 || iblock >= (int)blockTimeInfo.size()) continue;

      const BlockTimeInfo &bt = blockTimeInfo[iblock];
      if (bt.nEvents <= 0) continue;

      const double tDaq = bt.meanDaq;
      const double eTDaq = 0.5 * fabs(bt.lastDaq - bt.firstDaq);

      double dummyX = 0.;
      double maxPosition = 0.;
      double maxValue = 0.;

      g_BlockMaxChargePosition[l]->GetPoint(ip, dummyX, maxPosition);
      g_BlockMaxChargeValue[l]->GetPoint(ip, dummyX, maxValue);

      int p = g_DaqTimeMaxChargeStrip[l]->GetN();
      g_DaqTimeMaxChargeStrip[l]->SetPoint(p, tDaq, maxStrip);
      g_DaqTimeMaxChargeStrip[l]->SetPointError(p, eTDaq, 0.);

      p = g_DaqTimeMaxChargePosition[l]->GetN();
      g_DaqTimeMaxChargePosition[l]->SetPoint(p, tDaq, maxPosition);
      g_DaqTimeMaxChargePosition[l]->SetPointError(p, eTDaq, 0.);

      p = g_DaqTimeMaxChargeValue[l]->GetN();
      g_DaqTimeMaxChargeValue[l]->SetPoint(p, tDaq, maxValue);
      g_DaqTimeMaxChargeValue[l]->SetPointError(p, eTDaq, 0.);
    }
  }

  // ---------------------------------------------------------------------------
  // Write overall products
  // ---------------------------------------------------------------------------
  rawOverallDir->cd();

  for (int l = 0; l < MM_N_Layers; ++l) {
    if (hqmaxstripFull_NoSat[l]) {
      hQmaxSumNoSatStrip[l] = hqmaxstripFull_NoSat[l]->ProjectionX(Form("hQmaxSumNoSatStrip%s", mm_tag[l].Data()));
      hQmaxSumNoSatStrip[l]->SetDirectory(nullptr);
      hQmaxSumNoSatStrip[l]->SetTitle(TString("ProjectionX of non-saturated q_{max} vs physical position [") + mm_tag[l] + "]");
      hQmaxSumNoSatStrip[l]->SetXTitle(Form("%s position [mm]", mm_tag[l].Data()));
      hQmaxSumNoSatStrip[l]->SetYTitle("entries");
    }
    hqmaxstripFull[l]->Write();
    hqmaxstripFull_NoSat[l]->Write();
    if (hQmaxSumNoSatStrip[l]) hQmaxSumNoSatStrip[l]->Write();
    htimePositionFull[l]->Write();
    hzPositionFull[l]->Write();
  }

  timeOverallDir->cd();
  g_DaqTime_iev->Write();
  g_SrsTimeStamp_evt->Write();
  g_DaqTimeSec_evt->Write();
  g_DaqTimeMicroSec_evt->Write();
  g_DaqTime_evt->Write();
  g_evt_vs_iev->Write();

  blockMonitoringDir->cd();

  for (int l = 0; l < MM_N_Layers; ++l) {
    g_BlockMaxChargeStrip[l]->Write();
    g_BlockMaxChargePosition[l]->Write();
    g_BlockMaxChargeValue[l]->Write();

    g_DaqTimeMaxChargeStrip[l]->Write();
    g_DaqTimeMaxChargePosition[l]->Write();
    g_DaqTimeMaxChargeValue[l]->Write();
  }

  g_BlockMeanDaqSec->Write();
  g_BlockMeanDaqTime->Write();
  g_BlockMeanSrsTime->Write();

  outfile->Write();
  outfile->Close();
  delete outfile;
  outfile = nullptr;

  cout << "Output written to " << outputFileName << endl;
}
