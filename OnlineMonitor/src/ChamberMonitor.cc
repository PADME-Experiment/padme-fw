#include <stdio.h>
#include <sys/stat.h>
#include <fstream>

#include "ChamberMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

ChamberMonitor::ChamberMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

ChamberMonitor::~ChamberMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
  //if (fHLGPedestalBM) { delete fHLGPedestalBM; fHLGPedestalBM = 0; }
}

void ChamberMonitor::Initialize()
{

  // Get output rates from config file
  fBeamOutputRate = fConfigParser->HasConfig("RECO","BeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate")):500;
  fOffBeamOutputRate = fConfigParser->HasConfig("RECO","OffBeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate")):100;
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):0;
  fRandomOutputRate = fConfigParser->HasConfig("RECO","RandomOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate")):100;

  // Define trend support file for this run
  fTFChTrendsBM = fConfig->TrendDirectory()+"/"+fConfig->RunName()+"_ChTrendsBM.trend";

  /*
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
  */

  // Create histograms
  //fHLGPedestalBM = new TH1D("LG_PedestalBM","LG_PedestalBM",120,3500.,4100.);

  // Reset global counters
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

}

void ChamberMonitor::StartOfEvent()
{
  /*
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
  */

  // Chamber rawdata file has no trigger info: assume all events are beam events
  fIsBeam = true;
  fIsCosmics = false;
  fIsRandom = false;
  fIsOffBeam = false;
  fBeamEventCount++;

}

void ChamberMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      /*
      // Check if current data is new
      if ( (fVLGTimeBM.size() == 0) || (fConfig->GetEventAbsTime().AsDouble() > fVLGTimeBM.back()) ) {

	// Update trend vectors
	fVLGTimeBM.push_back(fConfig->GetEventAbsTime().AsDouble());

	// Update trends file
	FILE* tf = fopen(fTFLGTrendsBM.Data(),"a");
	fprintf(tf,"%f %f %f %f %f %f\n",fVLGTimeBM.back(),fVLGNPoTsBM.back(),fVLGNPoTsTotBM.back(),fVLGBunchLengthBM.back(),fVLGBunchBBQBM.back(),fVLGBunchDensityBM.back());
	fclose(tf);

      }
      */

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset histograms
      //fHLGPedestalBM->Reset();

    }

  } // End of beam output

}

void ChamberMonitor::Finalize()
{
  printf("ChamberMonitor::Finalize - Total number of events: %d\n",fBeamEventCount);
  //printf("ChamberMonitor::Finalize - Total number of beam     events: %d\n",fBeamEventCount);
  //printf("ChamberMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  //printf("ChamberMonitor::Finalize - Total number of cosmics  events: %d\n",fCosmicsEventCount);
  //printf("ChamberMonitor::Finalize - Total number of random   events: %d\n",fRandomEventCount);
}

void ChamberMonitor::AnalyzeEvent(ChamberEvent* rawEv)
{
  /*
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
  */
}

/*
void ChamberMonitor::ComputeTotalCharge(Short_t* samples)
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

void ChamberMonitor::ComputeBunchLength(Short_t* samples)
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
    printf("ChamberMonitor::ComputeBunchLength - WARNING - Bunch length longer than signal interval\n");
    bunchEnd = fSignalSamplesEnd;
  }

  // Convert sample interval to ns. DAQ is assumed at 1GHz.
  fBunchLength = (Double_t)(bunchEnd-bunchStart)*1.;

  // Compute bunch quality parameter BBQ
  fBunchBBQ = sqrt(((Double_t)sum2 - (Double_t)sum*(Double_t)sum/(Double_t)(bunchEnd-bunchStart))/(Double_t)(bunchEnd-bunchStart-1));

}
*/

Int_t ChamberMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("ChamberMonitor::OutputBeam - Writing beam output files\n");

  // Write Chamber histograms
  TString ftname = fConfig->TmpDirectory()+"/ChamberMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ChamberMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"w");
  /*
  // Pedestal
  fprintf(outf,"PLOTID ChamberMon_beampedestal\n");
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

  // Bunch Density trend plot
  fprintf(outf,"PLOTID ChamberMon_trendbunchdensity\n");
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
  */
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ChamberMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}
