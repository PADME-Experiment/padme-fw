#include <stdio.h>
#include <sys/stat.h>
#include <fstream>

#include "CombinedMonitor.hh"

#include "Configuration.hh"
#include "ECalMonitor.hh"
#include "LeadGlassMonitor.hh"
#include "ETagMonitor.hh"
#include "EVetoMonitor.hh"
#include "PVetoMonitor.hh"
#include "HEPVetoMonitor.hh"
#include "SACMonitor.hh"
#include "TargetMonitor.hh"
#include "TriggerMonitor.hh"
#include "ADCMonitor.hh"

#include "TMath.h"

CombinedMonitor::CombinedMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Reset all individual monitors
  fECalMon = 0;
  fLeadGlassMon = 0;
  fETagMon = 0;
  fEVetoMon = 0;
  fPVetoMon = 0;
  fHEPVetoMon = 0;
  fSACMon = 0;
  fTargetMon = 0;
  fTriggerMon = 0;
  fADCMon = 0;

  // Initialize all counters
  Initialize();

}

CombinedMonitor::~CombinedMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
}

void CombinedMonitor::Initialize()
{

  // Get output rates from config file
  fBeamOutputRate = fConfigParser->HasConfig("RECO","BeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate")):200;
  fOffBeamOutputRate = fConfigParser->HasConfig("RECO","OffBeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate")):0;
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):0;
  fRandomOutputRate = fConfigParser->HasConfig("RECO","RandomOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate")):0;

  // Set acceptable range for ETot/NPoTs trend plot
  fETotOnNPoTsMin = 0.;
  if (fConfigParser->HasConfig("RECO","ETotOnNPoTsRangeMin")) {
    fETotOnNPoTsMin = std::stod(fConfigParser->GetSingleArg("RECO","ETotOnNPoTsRangeMin"));
  } else {
    printf("CombinedMonitor::Initialize - WARNING: ETotOnNPoTsRangeMin not set in config file. Using %f\n",fETotOnNPoTsMin);
  }
  fETotOnNPoTsMax = 2.;
  if (fConfigParser->HasConfig("RECO","ETotOnNPoTsRangeMax")) {
    fETotOnNPoTsMax = std::stod(fConfigParser->GetSingleArg("RECO","ETotOnNPoTsRangeMax"));
  } else {
    printf("CombinedMonitor::Initialize - WARNING: ETotOnNPoTsRangeMax not set in config file. Using %f\n",fETotOnNPoTsMax);
  }

  // Define trend support file for this run
  fTFCmbTrendsBM = fConfig->TrendDirectory()+"/"+fConfig->RunName()+"_CmbTrendsBM.trend";

  // If trend file exists, recover the data
  struct stat buffer;
  if (stat(fTFCmbTrendsBM.Data(),&buffer) == 0) {
    std::ifstream tf(fTFCmbTrendsBM.Data());
    Double_t abstime,etotonnpots;
    while (tf >> abstime >> etotonnpots) {
      //printf("%f %f %f\n",abstime,etotonnpots);
      fVCmbTimeBM.push_back(abstime);
      fVETotOnNPoTsBM.push_back(etotonnpots);
    }
  }

  // Create histograms
  fHETotOnNPoTsBM = new TH1D("Cmb_ETotOnNPoTsBM","Cmb_ETotOnNPoTsBM",100,0.,10.);

  // Reset global counters
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

}

void CombinedMonitor::StartOfEvent()
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

void CombinedMonitor::EndOfEvent()
{

  if (fIsBeam) {

    // Compute etot to npots ratio
    fETotOnNPoTsBM = 0.;
    if (fECalMon && fLeadGlassMon && fLeadGlassMon->GetLeadGlassNPoTs())
      fETotOnNPoTsBM = fECalMon->GetTotalEventEnergy()/fLeadGlassMon->GetLeadGlassNPoTs();
    //printf("%f %f %f\n",fECalMon->GetTotalEventEnergy(),fLeadGlassMon->GetLeadGlassNPoTs(),fETotOnNPoTsBM);

    // Populate histograms
    fHETotOnNPoTsBM->Fill(fETotOnNPoTsBM);

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Check if current data is new
      if ( (fVCmbTimeBM.size() == 0) || (fConfig->GetEventAbsTime().AsDouble() > fVCmbTimeBM.back()) ) {

	// Update trend vectors
	fVCmbTimeBM.push_back(fConfig->GetEventAbsTime().AsDouble());
	fVETotOnNPoTsBM.push_back(fHETotOnNPoTsBM->GetMean());

	// Update trends file
	FILE* tf = fopen(fTFCmbTrendsBM.Data(),"a");
	fprintf(tf,"%f %f\n",fVCmbTimeBM.back(),fVETotOnNPoTsBM.back());
	fclose(tf);

      }

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset histograms
      fHETotOnNPoTsBM->Reset();

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

void CombinedMonitor::Finalize()
{
  printf("CombinedMonitor::Finalize - Total number of beam     events: %d\n",fBeamEventCount);
  printf("CombinedMonitor::Finalize - Total number of off-beam events: %d\n",fOffBeamEventCount);
  printf("CombinedMonitor::Finalize - Total number of cosmics  events: %d\n",fCosmicsEventCount);
  printf("CombinedMonitor::Finalize - Total number of random   events: %d\n",fRandomEventCount);
}

Int_t CombinedMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("CombinedMonitor::OutputBeam - Writing beam output files\n");

  // Write Combined histograms
  TString ftname = fConfig->TmpDirectory()+"/CombinedMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/CombinedMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  // ETot from ECal divided by NPoTs from LeadGlass
  fprintf(outf,"PLOTID CombinedMon_etotonnpots\n");
  fprintf(outf,"PLOTTYPE histo1d\n");
  fprintf(outf,"PLOTNAME ECal Etot on LG NPoTs - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS %d\n",fHETotOnNPoTsBM->GetNbinsX());
  fprintf(outf,"RANGE_X %.3f %.3f\n",fHETotOnNPoTsBM->GetXaxis()->GetXmin(),fHETotOnNPoTsBM->GetXaxis()->GetXmax());
  fprintf(outf,"TITLE_X MeV/PoT\n");
  fprintf(outf,"TITLE_Y Bunches\n");
  fprintf(outf,"DATA [[");
  for(Int_t b = 1; b <= fHETotOnNPoTsBM->GetNbinsX(); b++) {
    if (b>1) fprintf(outf,",");
    fprintf(outf,"%.0f",fHETotOnNPoTsBM->GetBinContent(b));
  }
  fprintf(outf,"]]\n\n");

  // ETot/NPoTs trend plot with acceptable range
  fprintf(outf,"PLOTID CombinedMon_trendetotonnpots\n");
  fprintf(outf,"PLOTNAME ECal Etot on LG NPoTs  - Run %d - %s\n",fConfig->GetRunNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\",\"lines\",\"lines\" ]\n");
  if ( (fVETotOnNPoTsBM.back() < fETotOnNPoTsMin) || (fVETotOnNPoTsBM.back() > fETotOnNPoTsMax) ) {
    fprintf(outf,"COLOR [ \"ff0000\",\"00ff00\",\"aaaa00\" ]\n");
  } else {
    fprintf(outf,"COLOR [ \"0000ff\",\"00ff00\",\"aaaa00\" ]\n");
  }
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y MeV/PoT\n");
  fprintf(outf,"LEGEND [ \"Ratio\",\"min\",\"max\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVCmbTimeBM.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVCmbTimeBM[j],fVETotOnNPoTsBM[j]);
  }
  fprintf(outf,"]");
  fprintf(outf,",[[\"%f\",%.1f],[\"%f\",%.1f]]",fVCmbTimeBM[0],fETotOnNPoTsMin,fVCmbTimeBM.back(),fETotOnNPoTsMin);
  fprintf(outf,",[[\"%f\",%.1f],[\"%f\",%.1f]]",fVCmbTimeBM[0],fETotOnNPoTsMax,fVCmbTimeBM.back(),fETotOnNPoTsMax);
  fprintf(outf," ]\n\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("CombinedMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t CombinedMonitor::OutputOffBeam()
{
  return 0;
}

Int_t CombinedMonitor::OutputCosmics()
{
  return 0;
}

Int_t CombinedMonitor::OutputRandom()
{
  return 0;
}
