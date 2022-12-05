#include <stdio.h>
#include <sys/stat.h>
#include <fstream>

#include "LeadGlassMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

LeadGlassMonitor::LeadGlassMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

LeadGlassMonitor::~LeadGlassMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
  if (fHLGPedestalBM) { delete fHLGPedestalBM; fHLGPedestalBM = 0; }
  if (fHLGPedRMSBM) { delete fHLGPedRMSBM; fHLGPedRMSBM = 0; }
  if (fHLGTotChargeBM) { delete fHLGTotChargeBM; fHLGTotChargeBM = 0; }
  if (fHLGNPoTsBM) { delete fHLGNPoTsBM; fHLGNPoTsBM = 0; }
  if (fHLGNPoTsTotBM) { delete fHLGNPoTsTotBM; fHLGNPoTsTotBM = 0; }
  if (fHLGBunchLengthBM) { delete fHLGBunchLengthBM; fHLGBunchLengthBM = 0; }
  if (fHLGBunchLengthTotBM) { delete fHLGBunchLengthTotBM; fHLGBunchLengthTotBM = 0; }
  if (fHLGBunchBBQBM) { delete fHLGBunchBBQBM; fHLGBunchBBQBM = 0; }
  if (fHLGBunchBBQTotBM) { delete fHLGBunchBBQTotBM; fHLGBunchBBQTotBM = 0; }
  if (fHLGBunchDensityBM) { delete fHLGBunchDensityBM; fHLGBunchDensityBM = 0; }
  if (fHLGBunchDensityTotBM) { delete fHLGBunchDensityTotBM; fHLGBunchDensityTotBM = 0; }
}

void LeadGlassMonitor::Initialize()
{

  if (fConfig->Verbose() > 1)
    printf("--- LeadGlass on board %d channel %d ---\n",LEADGLASS_BOARD,LEADGLASS_CHANNEL);

  // Get output rates from config file
  fBeamOutputRate = fConfigParser->HasConfig("RECO","BeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate")):500;
  fOffBeamOutputRate = fConfigParser->HasConfig("RECO","OffBeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate")):100;
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):0;
  fRandomOutputRate = fConfigParser->HasConfig("RECO","RandomOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate")):100;

  // Get pedestal and charge reconstruction parameters from config file
  fPedestalSamples = fConfigParser->HasConfig("RECO","PedestalSamples")?std::stoi(fConfigParser->GetSingleArg("RECO","PedestalSamples")):100;
  fSignalSamplesStart = fConfigParser->HasConfig("RECO","SignalSamplesStart")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesStart")):200;
  fSignalSamplesEnd = fConfigParser->HasConfig("RECO","SignalSamplesEnd")?std::stoi(fConfigParser->GetSingleArg("RECO","SignalSamplesEnd")):600;

  // Get charge-to-NPoTs conversion factor from config file. Complain if not found
  fChargeToNPoTs = 0.239; // Value for 402MeV and LeadGlass at 650V
  if (fConfigParser->HasConfig("RECO","ChargeToNPoTs")) {
    fChargeToNPoTs = std::stod(fConfigParser->GetSingleArg("RECO","ChargeToNPoTs"));
  } else {
    printf("LeadGlassMonitor::Initialize - WARNING: ChargeToNPoTs not set in config file. Using %f\n",fChargeToNPoTs);
  }

  // Get streams correction factor from config file. Complain if not found
  fStreamsFactor = 5.; // Assume 1 stream used over 5 acquired
  if (fConfigParser->HasConfig("RECO","StreamsFactor")) {
    fStreamsFactor = std::stod(fConfigParser->GetSingleArg("RECO","StreamsFactor"));
  } else {
    printf("LeadGlassMonitor::Initialize - WARNING: StreamsFactor not set in config file. Using %f\n",fStreamsFactor);
  }

  // Get threshold for bunch length evaluation.
  fBunchLengthThreshold = fConfigParser->HasConfig("RECO","BunchLengthThreshold")?std::stod(fConfigParser->GetSingleArg("RECO","BunchLengthThreshold")):50.;

  // Set acceptable range for NPoTs trend plot
  fNPoTsRangeMin = 2000.;
  if (fConfigParser->HasConfig("RECO","NPoTsRangeMin")) {
    fNPoTsRangeMin = std::stod(fConfigParser->GetSingleArg("RECO","NPoTsRangeMin"));
  } else {
    printf("LeadGlassMonitor::Initialize - WARNING: NPoTsRangeMin not set in config file. Using %f\n",fNPoTsRangeMin);
  }
  fNPoTsRangeMax = 4000.;
  if (fConfigParser->HasConfig("RECO","NPoTsRangeMax")) {
    fNPoTsRangeMax = std::stod(fConfigParser->GetSingleArg("RECO","NPoTsRangeMax"));
  } else {
    printf("LeadGlassMonitor::Initialize - WARNING: NPoTsRangeMax not set in config file. Using %f\n",fNPoTsRangeMax);
  }

  // Set acceptable range for Bunch Length trend plot
  fBunchLengthRangeMin = 100.;
  if (fConfigParser->HasConfig("RECO","BunchLengthRangeMin")) {
    fBunchLengthRangeMin = std::stod(fConfigParser->GetSingleArg("RECO","BunchLengthRangeMin"));
  } else {
    printf("LeadGlassMonitor::Initialize - WARNING: BunchLengthRangeMin not set in config file. Using %f\n",fBunchLengthRangeMin);
  }
  fBunchLengthRangeMax = 300.;
  if (fConfigParser->HasConfig("RECO","BunchLengthRangeMax")) {
    fBunchLengthRangeMax = std::stod(fConfigParser->GetSingleArg("RECO","BunchLengthRangeMax"));
  } else {
    printf("LeadGlassMonitor::Initialize - WARNING: BunchLengthRangeMax not set in config file. Using %f\n",fBunchLengthRangeMax);
  }

  // Define trend support file for this run
  fTFLGTrendsBM = fConfig->TrendDirectory()+"/"+fConfig->RunName()+"_LGTrendsBM.trend";

  // If trend file exists, recover the data
  struct stat buffer;
  if (stat(fTFLGTrendsBM.Data(),&buffer) == 0) {
    std::ifstream tf(fTFLGTrendsBM.Data());
    Double_t abstime,npots,npotstot,bunchlen,bunchbbq,bunchdens;
    while (tf >> abstime >> npots >> npotstot >> bunchlen >> bunchbbq >> bunchdens) {
      //printf("%f %f %f\n",abstime,npots,npotstot,bunchlen,bunchbbq);
      fVLGTimeBM.push_back(abstime);
      fVLGNPoTsBM.push_back(npots);
      fVLGNPoTsTotBM.push_back(npotstot);
      fVLGBunchLengthBM.push_back(bunchlen);
      fVLGBunchBBQBM.push_back(bunchbbq);
      fVLGBunchDensityBM.push_back(bunchdens);
    }
  }

  // Create histograms
  fHLGPedestalBM = new TH1D("LG_PedestalBM","LG_PedestalBM",120,3500.,4100.);
  fHLGPedRMSBM = new TH1D("LG_PedRMSBM","LG_PedRMSBM",100,0.,50.);
  fHLGTotChargeBM = new TH1D("LG_TotChargeBM","LG_TotChargeBM",1000,0.,5000.);
  fHLGNPoTsBM = new TH1D("LG_NPoTsBM","LG_NPoTsBM",1000,0.,20000.);
  fHLGNPoTsTotBM = new TH1D("LG_NPoTsTotBM","LG_NPoTsTotBM",1000,0.,20000.);
  fHLGBunchLengthBM = new TH1D("LG_BunchLengthBM","LG_BunchLengthBM",1000,0.,1000.);
  fHLGBunchLengthTotBM = new TH1D("LG_BunchLengthTotBM","LG_BunchLengthTotBM",1000,0.,1000.);
  fHLGBunchBBQBM = new TH1D("LG_BunchBBQBM","LG_BunchBBQBM",1000,0.,1000.);
  fHLGBunchBBQTotBM = new TH1D("LG_BunchBBQTotBM","LG_BunchBBQTotBM",1000,0.,1000.);
  fHLGBunchDensityBM = new TH1D("LG_BunchDensityBM","LG_BunchBBQBM",1000,0.,100.);
  fHLGBunchDensityTotBM = new TH1D("LG_BunchDensityTotBM","LG_BunchBBQTotBM",1000,0.,100.);

  // Reset cumulative waveform
  for(UInt_t i = 0; i<1024; i++) fLGWaveSumBM[i] = 0;

  // Reset waveform saturation flag
  fWFSaturated = false;

  // Reset global counters
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

}

void LeadGlassMonitor::StartOfEvent()
{

  // Check if event was triggered by BTF beam
  if (fConfig->GetEventTrigMask() & 0x01) {
    fIsBeam = true;
    fBeamEventCount++;
  } else {
    fIsBeam = false;
  }

  // Check if event was triggered by cosmics
  if (fConfig->GetEventTrigMask() & 0x02) {
    fIsCosmics = true;
    fCosmicsEventCount++;
  } else {
    fIsCosmics = false;
  }
 
  // Check if event was a random trigger
  if (fConfig->GetEventTrigMask() & 0x40) {
    fIsRandom = true;
    fRandomEventCount++;
  } else {
    fIsRandom = false;
  }
  
  // Check if event was an off-beam trigger
  if (fConfig->GetEventTrigMask() & 0x80) {
    fIsOffBeam = true;
    fOffBeamEventCount++;
  } else {
    fIsOffBeam = false;
  }

}

void LeadGlassMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Check if current data is new
      if ( (fVLGTimeBM.size() == 0) || (fConfig->GetEventAbsTime().AsDouble() > fVLGTimeBM.back()) ) {

	// Update trend vectors
	fVLGTimeBM.push_back(fConfig->GetEventAbsTime().AsDouble());
	fVLGNPoTsBM.push_back(fHLGNPoTsBM->GetMean());
	fVLGBunchLengthBM.push_back(fHLGBunchLengthBM->GetMean());
	fVLGBunchBBQBM.push_back(fHLGBunchBBQBM->GetMean());
	fVLGBunchDensityBM.push_back(fHLGBunchDensityBM->GetMean());

	// Compute total number of PoTs and update trend vector
	if (fVLGNPoTsTotBM.size() == 0) {
	  fVLGNPoTsTotBM.push_back(fVLGNPoTsBM.back()*(Double_t)fBeamOutputRate*fStreamsFactor);
	} else {
	  fVLGNPoTsTotBM.push_back(fVLGNPoTsTotBM.back()+fVLGNPoTsBM.back()*(Double_t)fBeamOutputRate*fStreamsFactor);
	}

	// Update trends file
	FILE* tf = fopen(fTFLGTrendsBM.Data(),"a");
	fprintf(tf,"%f %f %f %f %f %f\n",fVLGTimeBM.back(),fVLGNPoTsBM.back(),fVLGNPoTsTotBM.back(),fVLGBunchLengthBM.back(),fVLGBunchBBQBM.back(),fVLGBunchDensityBM.back());
	fclose(tf);

      }

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset histograms
      fHLGPedestalBM->Reset();
      fHLGPedRMSBM->Reset();
      fHLGTotChargeBM->Reset();
      fHLGNPoTsBM->Reset();
      fHLGBunchLengthBM->Reset();
      fHLGBunchBBQBM->Reset();
      fHLGBunchDensityBM->Reset();

      // Reset cumulative waveform
      for(UInt_t i = 0; i<1024; i++) fLGWaveSumBM[i] = 0;

      // Reset waveform saturation flag
      fWFSaturated = false;

    }

  } // End of beam output

  if (fIsOffBeam) {

    if (fOffBeamOutputRate && (fOffBeamEventCount % fOffBeamOutputRate == 0)) {

      // Write off-beam events data to output PadmeMonitor file
      OutputOffBeam();

    }

  } // End of off-beam output

  if (fIsCosmics) {

    if (fCosmicsOutputRate && (fCosmicsEventCount % fCosmicsOutputRate == 0)) {

      OutputCosmics();

    }

  }

  if (fIsRandom) {

    if (fRandomOutputRate && (fRandomEventCount % fRandomOutputRate == 0)) {

      // Write random events data to output PadmeMonitor file
      OutputRandom();

    }

  } // End of random output

}

void LeadGlassMonitor::Finalize()
{
  printf("LeadGlassMonitor::Finalize - Total number of beam     events: %d\n",fBeamEventCount);
  printf("LeadGlassMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("LeadGlassMonitor::Finalize - Total number of cosmics  events: %d\n",fCosmicsEventCount);
  printf("LeadGlassMonitor::Finalize - Total number of random   events: %d\n",fRandomEventCount);
  if (fVLGNPoTsTotBM.size() == 0) {
    printf("LeadGlassMonitor::Finalize - Total number of PoTs: 0\n");
  } else {
    printf("LeadGlassMonitor::Finalize - Total number of PoTs: %.0f\n",fVLGNPoTsTotBM.back());
  }
}

void LeadGlassMonitor::AnalyzeBoard(UChar_t board)
{;}

void LeadGlassMonitor::AnalyzeChannel(UChar_t board,UChar_t channel,Short_t* samples)
{

  // Compute pedestal and total charge in leadglass and save them to histogram
  ComputeTotalCharge(samples);

  if (fIsBeam) {

    // Compute lenght of bunch (period above a given thershold) and bunch quality (BBQ)
    ComputeBunchLength(samples);

    // Compute number of positrons on target (NPoTs)
    fLGNPoTs = fChannelCharge/fChargeToNPoTs;

    // Compute bunch density
    if (fBunchLength) {
      fBunchDensity = fLGNPoTs/fBunchLength;
    } else {
      fBunchDensity = 0.;
    }

    fHLGPedestalBM->Fill(fChannelPedestal);
    fHLGPedRMSBM->Fill(fChannelPedRMS);
    fHLGTotChargeBM->Fill(fChannelCharge);
    fHLGNPoTsBM->Fill(fLGNPoTs);
    fHLGNPoTsTotBM->Fill(fLGNPoTs);
    fHLGBunchLengthBM->Fill(fBunchLength);
    fHLGBunchLengthTotBM->Fill(fBunchLength);
    fHLGBunchBBQBM->Fill(fBunchBBQ);
    fHLGBunchBBQTotBM->Fill(fBunchBBQ);
    fHLGBunchDensityBM->Fill(fBunchDensity);
    fHLGBunchDensityTotBM->Fill(fBunchDensity);

    // Add waveform to cumulative for bunch shape studies
    for(UInt_t i = 0; i<1024; i++) {
      fLGWaveSumBM[i] += samples[i];
      if (samples[i] < 10) fWFSaturated = true;
    }

    // Save waveform once every few events
    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0))
      for(UInt_t i = 0; i<1024; i++) fLGWaveformBM[i] = samples[i];

  }

}

void LeadGlassMonitor::ComputeTotalCharge(Short_t* samples)
{

  // Get total signal area using first fPedestalSamples samples as pedestal
  Int_t sum = 0;
  Int_t sum_ped = 0;
  ULong_t sum2_ped = 0;
  for(UInt_t s = 0; s<1024; s++) {
    if (s<fPedestalSamples) {
      sum_ped += samples[s];
      sum2_ped += samples[s]*samples[s];
    } else if (s >= fSignalSamplesStart) {
      if (s < fSignalSamplesEnd) {
	sum += samples[s];
      } else {
	break;
      }
    }
  }

  fChannelPedestal = (Double_t)sum_ped/(Double_t)fPedestalSamples;
  fChannelPedRMS = sqrt(((Double_t)sum2_ped - (Double_t)sum_ped*fChannelPedestal)/((Double_t)fPedestalSamples-1.));
  fChannelCharge = fChannelPedestal*(Double_t)(fSignalSamplesEnd-fSignalSamplesStart)-(Double_t)sum;
  // Convert counts to charge in pC
  //charge = counts/(4096.*50.)*(1.E-9/1.E-12);
  fChannelCharge *= 4.8828E-3;

}

void LeadGlassMonitor::ComputeBunchLength(Short_t* samples)
{

  // Get length of bunch using a given threshold.
  // N.B. Assumes that pedestal was already computed
  Bool_t bunch = false;
  UInt_t bunchStart =0.;
  UInt_t bunchEnd = 0.;
  Int_t sum = 0;
  ULong_t sum2 = 0;
  for(UInt_t s = fSignalSamplesStart; s<fSignalSamplesEnd; s++) {
    if (bunch) {
      if (fChannelPedestal-(Double_t)samples[s] < fBunchLengthThreshold) {
	if (s-bunchStart > 1) { // Ignore noise ffluctuations
	  bunchEnd = s;
	  break;
	}
	sum += samples[s];
	sum2 += samples[s]*samples[s];
      }
    } else {
      if (fChannelPedestal-(Double_t)samples[s] > fBunchLengthThreshold) {
	bunch = true;
	bunchStart = s;
	sum = samples[s];
	sum2 = samples[s]*samples[s];
      }
    }
  }
  if (bunch && bunchEnd == 0.) {
    printf("LeadGlassMonitor::ComputeBunchLength - WARNING - Bunch length longer than signal interval\n");
    bunchEnd = fSignalSamplesEnd;
  }

  // Convert sample interval to ns. DAQ is assumed at 1GHz.
  fBunchLength = (Double_t)(bunchEnd-bunchStart)*1.;

  // Compute bunch quality parameter BBQ
  fBunchBBQ = sqrt(((Double_t)sum2 - (Double_t)sum*(Double_t)sum/(Double_t)(bunchEnd-bunchStart))/(Double_t)(bunchEnd-bunchStart-1));

}

Int_t LeadGlassMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("LeadGlassMonitor::OutputBeam - Writing beam output files\n");

  // Write LeadGlass histograms
  TString ftname = fConfig->TmpDirectory()+"/LeadGlassMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/LeadGlassMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  // Pedestal
  fprintf(outf,"PLOTID LeadGlassMon_beampedestal\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BM Pedestal - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGPedestalBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGPedestalBM->GetXaxis()->GetXmin(),fHLGPedestalBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X Counts\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGPedestalBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGPedestalBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Pedestal RMS
  fprintf(outf,"PLOTID LeadGlassMon_beampedrms\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BM Pedestal RMS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGPedRMSBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGPedRMSBM->GetXaxis()->GetXmin(),fHLGPedRMSBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X Counts\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGPedRMSBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGPedRMSBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Bunch Length
  fprintf(outf,"PLOTID LeadGlassMon_beambunchlength\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BM Bunch Length - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGBunchLengthBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGBunchLengthBM->GetXaxis()->GetXmin(),fHLGBunchLengthBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X ns\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGBunchLengthBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGBunchLengthBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Total Bunch Length distribution for this run
  fprintf(outf,"PLOTID LeadGlassMon_beambunchlengthtot\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BM Bunch Length Total - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGBunchLengthTotBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGBunchLengthTotBM->GetXaxis()->GetXmin(),fHLGBunchLengthTotBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X ns\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGBunchLengthTotBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGBunchLengthTotBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Bunch BBQ
  fprintf(outf,"PLOTID LeadGlassMon_beambunchbbq\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BBQ - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGBunchBBQBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGBunchBBQBM->GetXaxis()->GetXmin(),fHLGBunchBBQBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X BBQ\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGBunchBBQBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGBunchBBQBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Total Bunch BBQ distribution for this run
  fprintf(outf,"PLOTID LeadGlassMon_beambunchbbqtot\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BBQ Total - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGBunchBBQTotBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGBunchBBQTotBM->GetXaxis()->GetXmin(),fHLGBunchBBQTotBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X BBQ\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGBunchBBQTotBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGBunchBBQTotBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Bunch Density
  fprintf(outf,"PLOTID LeadGlassMon_beambunchdensity\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG Density - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGBunchDensityBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGBunchDensityBM->GetXaxis()->GetXmin(),fHLGBunchDensityBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X PoTs/ns\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGBunchDensityBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGBunchDensityBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Total Bunch Density distribution for this run
  fprintf(outf,"PLOTID LeadGlassMon_beambunchdensitytot\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG Density Total - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGBunchDensityTotBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGBunchDensityTotBM->GetXaxis()->GetXmin(),fHLGBunchDensityTotBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X PoTs/ns\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGBunchDensityTotBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGBunchDensityTotBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Total Charge
  fprintf(outf,"PLOTID LeadGlassMon_beamtotcharge\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BM Total Charge - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGTotChargeBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGTotChargeBM->GetXaxis()->GetXmin(),fHLGTotChargeBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X pC\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGTotChargeBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGTotChargeBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Number of Positrons on Target (NPoTs)
  fprintf(outf,"PLOTID LeadGlassMon_beamnpots\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BM PoTs - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGNPoTsBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGNPoTsBM->GetXaxis()->GetXmin(),fHLGNPoTsBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X nPoTs\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGNPoTsBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGNPoTsBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Total number of Positrons on Target (NPoTs) for this run
  fprintf(outf,"PLOTID LeadGlassMon_beamnpotstot\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME LG BM PoTs Total - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHLGNPoTsTotBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHLGNPoTsTotBM->GetXaxis()->GetXmin(),fHLGNPoTsTotBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X nPoTs\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHLGNPoTsTotBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHLGNPoTsTotBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // Single event waveform
  fprintf(outf,"PLOTID LeadGlassMon_beamwaveform\n");
  fprintf(outf,"PLOTTYPE scatter\n");
  fprintf(outf,"PLOTNAME LG BM Waveform - Run %d Event %d - %s\n",fConfig->GetRunNumber(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"RANGE_X 0 1024\n");
  fprintf(outf,"TITLE_X Sample\n");
  fprintf(outf,"TITLE_Y Counts\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  //  fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  fprintf(outf,"DATA [ [");
  Bool_t saturated = false;
  for(UInt_t j = 0; j<1024; j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[%d,%d]",j,fLGWaveformBM[j]);
    if (fLGWaveformBM[j]<10) saturated = true;
  }
  fprintf(outf,"] ]\n");
  if (saturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"\n");

  // Beam bunch shape
  fprintf(outf,"PLOTID LeadGlassMon_beambunchshape\n");
  fprintf(outf,"PLOTTYPE scatter\n");
  fprintf(outf,"PLOTNAME LG BM Bunch Shape - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"RANGE_X 0 1024\n");
  fprintf(outf,"TITLE_X Sample\n");
  fprintf(outf,"TITLE_Y Counts\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  if (fWFSaturated) {
    fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  }
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<1024; j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[%d,%.1f]",j,(Double_t)fLGWaveSumBM[j]/(Double_t)fBeamOutputRate);
  }
  fprintf(outf,"] ]\n\n");

  // NPoTs trend plot with acceptable range
  fprintf(outf,"PLOTID LeadGlassMon_trendnpots\n");
  fprintf(outf,"PLOTNAME LG NPotS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\",\"lines\",\"lines\" ]\n");
  if ( (fVLGNPoTsBM[fVLGNPoTsBM.size()-1] < fNPoTsRangeMin) || (fVLGNPoTsBM[fVLGNPoTsBM.size()-1] > fNPoTsRangeMax) ) {
    fprintf(outf,"COLOR [ \"ff0000\",\"00ff00\",\"aaaa00\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\",\"00ff00\",\"aaaa00\" ]\n");
  }
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y NPoTs/Bunch\n");
  fprintf(outf,"LEGEND [ \"NPoTs\",\"min\",\"max\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVLGTimeBM.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVLGTimeBM[j],fVLGNPoTsBM[j]);
  }
  fprintf(outf,"]");
  fprintf(outf,",[[\"%f\",%.1f],[\"%f\",%.1f]]",fVLGTimeBM[0],fNPoTsRangeMin,fVLGTimeBM[fVLGTimeBM.size()-1],fNPoTsRangeMin);
  fprintf(outf,",[[\"%f\",%.1f],[\"%f\",%.1f]]",fVLGTimeBM[0],fNPoTsRangeMax,fVLGTimeBM[fVLGTimeBM.size()-1],fNPoTsRangeMax);
  fprintf(outf," ]\n\n");

  // Total NPoTs trend plot
  fprintf(outf,"PLOTID LeadGlassMon_trendnpotstot\n");
  fprintf(outf,"PLOTNAME LG Total NPotS - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Tot_NPoTs\n");
  fprintf(outf,"LEGEND [ \"Tot NPoTs\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVLGTimeBM.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVLGTimeBM[j],fVLGNPoTsTotBM[j]);
  }
  fprintf(outf,"] ]\n\n");

  // Bunch length trend plot with acceptable range
  fprintf(outf,"PLOTID LeadGlassMon_trendbunchlength\n");
  fprintf(outf,"PLOTNAME LG Bunch Length - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\",\"lines\",\"lines\" ]\n");
  if ( (fVLGBunchLengthBM[fVLGBunchLengthBM.size()-1] < fBunchLengthRangeMin) || (fVLGBunchLengthBM[fVLGBunchLengthBM.size()-1] > fBunchLengthRangeMax) ) {
    fprintf(outf,"COLOR [ \"ff0000\",\"00ff00\",\"aaaa00\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\",\"00ff00\",\"aaaa00\" ]\n");
  }
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y ns\n");
  fprintf(outf,"LEGEND [ \"BchLen\",\"min\",\"max\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVLGTimeBM.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVLGTimeBM[j],fVLGBunchLengthBM[j]);
  }
  fprintf(outf,"]");
  fprintf(outf,",[[\"%f\",%.1f],[\"%f\",%.1f]]",fVLGTimeBM[0],fBunchLengthRangeMin,fVLGTimeBM[fVLGTimeBM.size()-1],fBunchLengthRangeMin);
  fprintf(outf,",[[\"%f\",%.1f],[\"%f\",%.1f]]",fVLGTimeBM[0],fBunchLengthRangeMax,fVLGTimeBM[fVLGTimeBM.size()-1],fBunchLengthRangeMax);
  fprintf(outf," ]\n\n");

  // Bunch BBQ trend plot
  fprintf(outf,"PLOTID LeadGlassMon_trendbunchbbq\n");
  fprintf(outf,"PLOTNAME LG Bunch BBQ - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y BBQ\n");
  fprintf(outf,"LEGEND [ \"BBQ\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVLGTimeBM.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVLGTimeBM[j],fVLGBunchBBQBM[j]);
  }
  fprintf(outf,"] ]\n\n");

  // Bunch Density trend plot
  fprintf(outf,"PLOTID LeadGlassMon_trendbunchdensity\n");
  fprintf(outf,"PLOTNAME LG Bunch Density - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y PoTs/ns\n");
  fprintf(outf,"LEGEND [ \"PoTs/ns\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVLGTimeBM.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVLGTimeBM[j],fVLGBunchDensityBM[j]);
  }
  fprintf(outf,"] ]\n\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("LeadGlassMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t LeadGlassMonitor::OutputOffBeam()
{
  return 0;
}

Int_t LeadGlassMonitor::OutputCosmics()
{
  return 0;
}

Int_t LeadGlassMonitor::OutputRandom()
{
  return 0;
}
