#include <stdio.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

#include "SmallChamberMonitor.hh"
#include "ChamberEvent.hh"

#include "Configuration.hh"

#include "TMath.h"
#include "TPRegexp.h"

SmallChamberMonitor::SmallChamberMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>1) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

SmallChamberMonitor::~SmallChamberMonitor()
{
  if (fConfigParser) { delete fConfigParser; fConfigParser = 0; }
  for(UInt_t i=0; i<MMCH_N_LAYERS; i++) {
    if (h_occupancy[i])    { delete h_occupancy[i];    h_occupancy[i] = 0; }
    if (hw_occupancy[i])   { delete hw_occupancy[i];   hw_occupancy[i] = 0; }
    if (hqmax_totevent[i]) { delete hqmax_totevent[i]; hqmax_totevent[i] = 0; }
    if (hqmax_perevent[i]) { delete hqmax_perevent[i]; hqmax_perevent[i] = 0; }
  }
}

void SmallChamberMonitor::Initialize()
{

  // Parse run name to extract string to use in histograms
  fRunString = fConfig->RunName();
  TObjArray* runTags = TPRegexp("^run([0-9]+)_run_0*([0-9]+)_.*$").MatchS(fRunString);
  const Int_t nrTags = runTags->GetLast()+1;
  if (nrTags > 2) {
    const TString chRun = ((TObjString *)runTags->At(1))->GetString();
    const TString pdRun = ((TObjString *)runTags->At(2))->GetString();
    fRunString = "Run "+pdRun+" ("+chRun+")";
  }

  ClearBeamAccumulators();

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
  for(int i=0; i<MMCH_N_LAYERS; i++) {
    h_occupancy[i] = new TH1D(TString::Format("h_occupancy%d",i),TString::Format("Occupancy Layer %d",i),fNStrips,0,fNStrips);
    hw_occupancy[i] = new TH1D(TString::Format("hw_occupancy%d",i),TString::Format("Weighted Occupancy Layer %d",i),fNStrips,0,fNStrips);
    hqmax_totevent[i] = new TH1D(TString::Format("hqmax_totevent%d",i),TString::Format("hqmax_totevent%d",i),1000,10,2500);
    hqmax_perevent[i] = new TH1D(TString::Format("hqmax_perevent%d",i),TString::Format("Q_max Layer %d; # strip ; Charge [ADC counts]",i),fNStrips,0,fNStrips);
  }

  // Reset global counters
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

  // Define trend support file for this run
  fTFChTrendsBM = fConfig->TrendDirectory()+"/"+fConfig->RunName()+"_ChTrendsBM.trend";

  /*
  // If trend file exists, recover the data
  struct stat buffer;
  if (stat(fTFChTrendsBM.Data(),&buffer) == 0) {
    std::ifstream tf(fTFChTrendsBM.Data());
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

}

void SmallChamberMonitor::StartOfEvent()
{

  // Chamber rawdata file has no trigger info: assume all events are beam events
  fIsBeam = true;
  fIsCosmics = false;
  fIsRandom = false;
  fIsOffBeam = false;
  fBeamEventCount++;

  // Reset hits vectors
  for(int i=0; i<MMCH_N_LAYERS; i++) {
    x_mean[i].clear(); q_mean[i].clear(); c_mean[i].clear();
  }

}

void SmallChamberMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Check if current data is new
      if ( (fVTime_Beam.size() == 0) || (fConfig->GetEventAbsTime().AsDouble() > fVTime_Beam.back()) ) {

	// Update trend vectors
	fVTime_Beam.push_back(fConfig->GetEventAbsTime().AsDouble());

	if (beamspot_x_n[0]) {
	  fVP1_BeamX.push_back(beamspot_x[0]/beamspot_x_n[0]);
	} else {
	  fVP1_BeamX.push_back(0.);
	}
	if (beamspot_x_n[1]) {
	  fVP1_BeamY.push_back(beamspot_x[1]/beamspot_x_n[1]);
	} else {
	  fVP1_BeamY.push_back(0.);
	}

	if (beamspot_rms_n[0]) {
	  fVP1_BeamXSpread.push_back(beamspot_rms[0]/beamspot_rms_n[0]);
	} else {
	  fVP1_BeamXSpread.push_back(0.);
	}
	if (beamspot_rms_n[1]) {
	  fVP1_BeamYSpread.push_back(beamspot_rms[1]/beamspot_rms_n[1]);
	} else {
	  fVP1_BeamYSpread.push_back(0.);
	}

	if (beamspot_q_n[0]) {
	  fVP1_BeamXCharge.push_back(beamspot_q[0]/beamspot_q_n[0]);
	} else {
	  fVP1_BeamXCharge.push_back(0.);
	}

	if (beamspot_q_n[1]) {
	  fVP1_BeamYCharge.push_back(beamspot_q[1]/beamspot_q_n[1]);
	} else {
	  fVP1_BeamYCharge.push_back(0.);
	}

	/*
	// Update trends file
	FILE* tf = fopen(fTFLGTrendsBM.Data(),"a");
	fprintf(tf,"%f %f %f %f %f %f\n",fVLGTimeBM.back(),fVLGNPoTsBM.back(),fVLGNPoTsTotBM.back(),fVLGBunchLengthBM.back(),fVLGBunchBBQBM.back(),fVLGBunchDensityBM.back());
	fclose(tf);
	*/

	ClearBeamAccumulators();

      }

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset histograms
      for(Int_t i=0; i<MMCH_N_LAYERS; i++) {
	h_occupancy[i]->Reset();
	hw_occupancy[i]->Reset();
	hqmax_totevent[i]->Reset();
	hqmax_perevent[i]->Reset();
      }

    }

  } // End of beam output

}

void SmallChamberMonitor::Finalize()
{
  printf("SmallChamberMonitor::Finalize - Total number of events: %d\n",fBeamEventCount);
}

void SmallChamberMonitor::AnalyzeEvent(ChamberEvent* rawEv)
{

  Int_t layer, channel;
  std::vector<Double_t> camp;   

  for (UInt_t j=0; j<rawEv->mmLayer->size(); j++){  // loop on fired strips

    layer = rawEv->mmReadout->at(j)-88;
    if (layer<0 || layer>1) continue; // Skip pathologic strips
    channel = rawEv->mmStrip->at(j);

    // Fix layer and strip for TMM: x->y and y->Nstrips-x
    if (layer == 0) {
      layer = 1;
    } else {
      layer = 0;
      channel = fNStrips-channel;
    }

    // Find hit charge and coordinate by looping on 25ns bin sampling
    camp.clear();
    for(UInt_t n=0; n<rawEv->raw_q->at(j).size(); n++) camp.push_back(rawEv->raw_q->at(j).at(n));  
    Double_t x_strip=0., q_strip=0.;
    CoordinateFinder(layer, channel, camp, x_strip, q_strip);

    // Fill summary vectors
    //std::cout <<"Layer "<<layer<<" channel "<<channel<<" : x="<<x_strip<<" z="<<z_strip<<" q_max="<<q_strip<<" time="<<t_strip<<std::endl;
    x_mean[layer].push_back(x_strip);
    q_mean[layer].push_back(q_strip);
    c_mean[layer].push_back(channel);

    // Fill event histograms
    hqmax_totevent[layer]->Fill(q_strip);
    h_occupancy[layer]->Fill(channel);
    hw_occupancy[layer]->Fill(channel,q_strip);
    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {
      hqmax_perevent[layer]->Fill(channel,q_strip);
    }
 
  }

  // Compute beam spot properties (coordinates, rms, and total charge)
  ComputeBeamSpot();

}

void SmallChamberMonitor::CoordinateFinder(Int_t iReadout, Int_t iStrip, std::vector<Double_t> camp, Double_t &x_strip, Double_t &q_strip) 
{

  // Comput strip charge (max charge)
  Int_t Nbins = camp.size();
  Double_t qmax = -1000;
  for (Int_t ibin = 0; ibin < Nbins; ibin++) {
    Double_t qbin = camp.at(ibin); // Charge in the bin
    if (qbin > qmax) qmax = qbin;
  }

  // Correct charge for strip position
  double APV_q_corr = 1.;
  if (iReadout == 0) {
    if (iStrip < 122) APV_q_corr = APV1_fqx;
    if (iStrip > 122+113) APV_q_corr = APV3_fqx;
  } else if (iReadout == 1) {
    if (iStrip < 122) APV_q_corr = APV1_fqy;
    if (iStrip > 122+113) APV_q_corr = APV3_fqy;
  }
  q_strip = qmax*APV_q_corr;
  
  // Compute strip coordinate correcting for global position
  x_strip = (iStrip-1)*fStripPitch; // Coordinate 0. is at center of strip 1
  if(iReadout == 0) { //X
    x_strip = x_strip + GLOBAL_X_TRANSLATION;
  }
  else if(iReadout == 1) { //Y
    x_strip = x_strip + GLOBAL_Y_TRANSLATION;
  }

}

void SmallChamberMonitor::ComputeBeamSpot() {

  Double_t sum_q,sum_qx,sum_qx2,x_spot;

  for(UInt_t r=0; r<MMCH_N_LAYERS; r++) {

    sum_q = 0.;
    sum_qx = 0.;
    sum_qx2 = 0.;

    UInt_t used = 0;
    for(UInt_t h=0; h<x_mean[r].size(); h++) {
      used++;
      sum_q += q_mean[r][h];
      sum_qx += q_mean[r][h]*x_mean[r][h];
      sum_qx2 += q_mean[r][h]*x_mean[r][h]*x_mean[r][h];
    }

    if (used>0) {
      beamspot_q[r] += sum_q;
      beamspot_q_n[r]++;
      x_spot = sum_qx/sum_q;
      beamspot_x[r] += x_spot;
      beamspot_x_n[r]++;
      if (used>1) {
	beamspot_rms[r] += sqrt(sum_qx2/sum_q - x_spot*x_spot);
	beamspot_rms_n[r]++;
      }
    }

  }

}

Int_t SmallChamberMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("SmallChamberMonitor::OutputBeam - Writing beam output files\n");

  // Write Chamber histograms
  TString ftname = fConfig->TmpDirectory()+"/SmallChamberMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/SmallChamberMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  for(int i=0; i<MMCH_N_LAYERS; i++) {

    // Layer Occupancy
    fprintf(outf,"PLOTID TMMCh_occupancy%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME TMMCh %s Occupancy - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",h_occupancy[i]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",h_occupancy[i]->GetXaxis()->GetXmin(),h_occupancy[i]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X Channel\n");
    fprintf(outf,"TITLE_Y Hits\n");
    fprintf(outf,"DATA [[");
    for(Int_t b = 1; b <= h_occupancy[i]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",h_occupancy[i]->GetBinContent(b));
    }
    fprintf(outf,"]]\n\n");

    // Layer Weighted Occupancy
    fprintf(outf,"PLOTID TMMCh_woccupancy%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME TMMCh %s Weighted Occupancy - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",hw_occupancy[i]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",hw_occupancy[i]->GetXaxis()->GetXmin(),hw_occupancy[i]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X Channel\n");
    fprintf(outf,"TITLE_Y W_Hits\n");
    fprintf(outf,"DATA [[");
    for(Int_t b = 1; b <= hw_occupancy[i]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",hw_occupancy[i]->GetBinContent(b));
    }
    fprintf(outf,"]]\n\n");

    // Layer QMax Total events
    fprintf(outf,"PLOTID TMMCh_qmax_totevent%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME TMMCh %s QMax Total Event - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",hqmax_totevent[i]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",hqmax_totevent[i]->GetXaxis()->GetXmin(),hqmax_totevent[i]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X QMax\n");
    fprintf(outf,"TITLE_Y Strips\n");
    fprintf(outf,"DATA [[");
    for(Int_t b = 1; b <= hqmax_totevent[i]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",hqmax_totevent[i]->GetBinContent(b));
    }
    fprintf(outf,"]]\n\n");

    // Layer QMax Single event
    fprintf(outf,"PLOTID TMMCh_qmax_perevent%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME TMMCh %s QMax Single Event - %s Event %d - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",hqmax_perevent[i]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",hqmax_perevent[i]->GetXaxis()->GetXmin(),hqmax_perevent[i]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X Channel\n");
    fprintf(outf,"TITLE_Y QMax\n");
    fprintf(outf,"DATA [[");
    for(Int_t b = 1; b <= hqmax_perevent[i]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",hqmax_perevent[i]->GetBinContent(b));
    }
    fprintf(outf,"]]\n\n");

  }

  // Beam position trend plots

  fprintf(outf,"PLOTID TMMCh_trendbeamposx\n");
  fprintf(outf,"PLOTNAME TMMCh Beam X Position - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamX[j]);
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID TMMCh_trendbeamposy\n");
  fprintf(outf,"PLOTNAME TMMCh Beam Y Position - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamY[j]);
  }
  fprintf(outf,"] ]\n\n");

  // Beam spread trend plots

  fprintf(outf,"PLOTID TMMCh_trendbeamspreadx\n");
  fprintf(outf,"PLOTNAME TMMCh Beam X Spread - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP1_BeamXSpread[j]);
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID TMMCh_trendbeamspready\n");
  fprintf(outf,"PLOTNAME TMMCh Beam Y Spread - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP1_BeamYSpread[j]);
  }
  fprintf(outf,"] ]\n\n");

  // Beam charge trend plots

  fprintf(outf,"PLOTID TMMCh_trendbeamchargex\n");
  fprintf(outf,"PLOTNAME TMMCh Beam X Charge - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Charge\n");
  fprintf(outf,"LEGEND [ \"P1\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamXCharge[j]);
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID TMMCh_trendbeamchargey\n");
  fprintf(outf,"PLOTNAME TMMCh Beam Y Charge - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Charge\n");
  fprintf(outf,"LEGEND [ \"P1\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamYCharge[j]);
  }
  fprintf(outf,"] ]\n\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("SmallChamberMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t SmallChamberMonitor::ClearBeamAccumulators()
{
  // Clear beam related accumulators

  for(UInt_t i=0; i<MMCH_N_LAYERS; i++) {
    beamspot_x[i] = 0.;
    beamspot_x_n[i] = 0;
    beamspot_rms[i] = 0.;
    beamspot_rms_n[i] = 0;
    beamspot_q[i] = 0.;
    beamspot_q_n[i] = 0;
  }

  return 0;
}
