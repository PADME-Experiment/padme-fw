#include <stdio.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

#include "ChamberMonitor.hh"
#include "ChamberEvent.hh"

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
  for(int i=0; i<MMCH_N_LAYERS; i++) {
    h_occupancy[i] = new TH1D(TString::Format("h_occupancy%d",i),TString::Format("Occupancy Layer %d",i),maxStrip,0,xmax);
    hw_occupancy[i] = new TH1D(TString::Format("hw_occupancy%d",i),TString::Format("Weighted Occupancy Layer %d",i),maxStrip,0,xmax);
    hqmax_totevent[i] = new TH1D(TString::Format("hqmax_totevent%d",i),TString::Format("hqmax_totevent%d",i),1000,10,2500);
    htmax_totevent[i] = new TH1D(TString::Format("htmax_totevent%d",i),TString::Format("htmax_totevent%d",i),1000,10,2500);
    //hqmax_strip[i] = new TH2D(TString::Format("hqmax_strip%d",i),TString::Format("hqmax_strip%d",i),maxStrip,0,xmax,1000,10,2500);;
    //htmax_strip[i] = new TH2D(TString::Format("htmax_strip%d",i),TString::Format("htmax_strip%d",i),maxStrip,0,xmax,1000,10,2500);
    hqmax_perevent[i] = new TH1D(TString::Format("hqmax_perevent%d",i),TString::Format("Q_max Layer %d; # strip ; Charge [ADC counts]",i),maxStrip,0,xmax);
    htmax_perevent[i] = new TH1D(TString::Format("htmax_perevent%d",i),TString::Format("Time Layer %d; # strip ; Time [ns]",i),maxStrip,0,xmax);
  }

  // Reset global counters
  fBeamEventCount = 0;
  fOffBeamEventCount = 0;
  fCosmicsEventCount = 0;
  fRandomEventCount = 0;

}

void ChamberMonitor::StartOfEvent()
{

  // Chamber rawdata file has no trigger info: assume all events are beam events
  fIsBeam = true;
  fIsCosmics = false;
  fIsRandom = false;
  fIsOffBeam = false;
  fBeamEventCount++;

  // Reset Time fit objects
  for(int i=0; i<MMCH_N_LAYERS; i++) {
    t_mean[i].clear(); z_mean[i].clear(); x_mean[i].clear(); q_mean[i].clear();
  }

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
      for(int i=0; i<MMCH_N_LAYERS; i++) {
	h_occupancy[i]->Reset();
	hw_occupancy[i]->Reset();
	hqmax_totevent[i]->Reset();
	htmax_totevent[i]->Reset();
	//hqmax_strip[i]->Reset();
	//htmax_strip[i]->Reset();
	hqmax_perevent[i]->Reset();
	htmax_perevent[i]->Reset();
      }

    }

  } // End of beam output

}

void ChamberMonitor::Finalize()
{
  printf("ChamberMonitor::Finalize - Total number of events: %d\n",fBeamEventCount);
}

void ChamberMonitor::AnalyzeEvent(ChamberEvent* rawEv)
{

  int layer, channel;
  std::vector<double> camp;   

  for (UInt_t j=0; j<rawEv->mmLayer->size(); j++){  // loop on fired strips

    layer = rawEv->mmLayer->at(j);
    channel = rawEv->mmStrip->at(j);

    // Find hit coordinates by looping on 25ns bin sampling
    double t_strip=0,x_strip=0,z_strip=0, q_strip=0;
    camp.clear();
    for(UInt_t n=0; n<rawEv->raw_q->at(j).size(); n++) camp.push_back(rawEv->raw_q->at(j).at(n));  
    CoordinateFinder(channel, layer, camp, t_strip, x_strip, z_strip, q_strip);

    // Fill summary vectors
    //std::cout <<"Layer "<<layer<<" channel "<<channel<<" : x="<<x_strip<<" z="<<z_strip<<" q_max="<<q_strip<<" time="<<t_strip<<std::endl;
    t_mean[layer].push_back(t_strip);
    x_mean[layer].push_back(x_strip);
    z_mean[layer].push_back(z_strip);
    q_mean[layer].push_back(q_strip);

    // Fill event histograms
    //hqmax_strip[layer]->Fill(channel,q_strip);
    //htmax_strip[layer]->Fill(channel,t_strip); 
    htmax_totevent[layer]->Fill(t_strip);
    hqmax_totevent[layer]->Fill(q_strip);
    h_occupancy[layer]->Fill(channel);
    hw_occupancy[layer]->Fill(channel,q_strip);
    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {
      hqmax_perevent[layer]->Fill(channel,q_strip);
      htmax_perevent[layer]->Fill(channel,t_strip);
    }
 
  }

}
void ChamberMonitor::CoordinateFinder(int iStrip, int iLayer, std::vector<double> camp, double &t_strip, double &x_strip, double &z_strip, double &q_strip) 
{

  int Nbins = camp.size();
  double qtot = std::accumulate(camp.begin(),camp.end(),0);
  double qtotT = 0;
  double threshold = 0.2 * qtot / Nbins;
  double qmax = -1000;      
  double t0 = 0.; //offset tempo in ns DA FITTARE!

  for (int ibin = 0; ibin < Nbins; ibin++) {
    double qbin = camp.at(ibin); // Charge in the bin
    double tbin = ibin*clock+clock/2.;  // Time (center of the bin)

	
    if (qbin > qmax) {
      qmax = qbin;
    }
    
    if (qbin < threshold) continue;
    
    t_strip += qbin * tbin;
    qtotT += qbin;
  }

  t_strip /= qtotT;
  
  if(iStrip <=256) x_strip = iStrip * pitch + pitch / 2;
  else x_strip = iStrip * pitch + pitch / 2 + geo_hole[iLayer];

  double z_ion = 2;//mm first signal from ionization

  if(iLayer>3) z_strip = zm+z_ion-(t_strip-t0)*vd;//plane 1 (close to the beam)
  else z_strip = (t_strip-t0)*vd-zm-z_ion;//plane 2 (far from to the beam)

  q_strip = qmax;

}

Int_t ChamberMonitor::OutputBeam()
{
  if (fConfig->Verbose()>0) printf("ChamberMonitor::OutputBeam - Writing beam output files\n");

  // Write Chamber histograms
  TString ftname = fConfig->TmpDirectory()+"/ChamberMon_Beam.txt";
  TString ffname = fConfig->OutputDirectory()+"/ChamberMon_Beam.txt";
  FILE* outf = fopen(ftname.Data(),"w");

  for(int i=0; i<MMCH_N_LAYERS; i++) {

    // Layer Occupancy
    fprintf(outf,"PLOTID MMCh_occupancy%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME MMCh %s Occupancy - Run %s - %s\n",mmch_tag[i].Data(),fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    fprintf(outf,"PLOTID MMCh_woccupancy%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME MMCh %s Weighted Occupancy - Run %s - %s\n",mmch_tag[i].Data(),fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    fprintf(outf,"PLOTID MMCh_qmax_totevent%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME MMCh %s QMax Total Event - Run %s - %s\n",mmch_tag[i].Data(),fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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

    // Layer TMax Total events
    fprintf(outf,"PLOTID MMCh_tmax_totevent%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME MMCh %s TMax Total Event - Run %s - %s\n",mmch_tag[i].Data(),fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",htmax_totevent[i]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",htmax_totevent[i]->GetXaxis()->GetXmin(),htmax_totevent[i]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X TMax\n");
    fprintf(outf,"TITLE_Y Strips\n");
    fprintf(outf,"DATA [[");
    for(Int_t b = 1; b <= htmax_totevent[i]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",htmax_totevent[i]->GetBinContent(b));
    }
    fprintf(outf,"]]\n\n");

    // Layer QMax Single event
    fprintf(outf,"PLOTID MMCh_qmax_perevent%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME MMCh %s QMax Single Event - Run %s Event %d - %s\n",mmch_tag[i].Data(),fConfig->RunName().Data(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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

    // Layer TMax Single event
    fprintf(outf,"PLOTID MMCh_tmax_perevent%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME MMCh %s TMax Single Event - Run %s Event %d - %s\n",mmch_tag[i].Data(),fConfig->RunName().Data(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",htmax_perevent[i]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",htmax_perevent[i]->GetXaxis()->GetXmin(),htmax_perevent[i]->GetXaxis()->GetXmax());
    fprintf(outf,"TITLE_X Channel\n");
    fprintf(outf,"TITLE_Y TMax\n");
    fprintf(outf,"DATA [[");
    for(Int_t b = 1; b <= htmax_perevent[i]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",htmax_perevent[i]->GetBinContent(b));
    }
    fprintf(outf,"]]\n\n");

  }
  
  /* Trend plot example
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
