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
  for(UInt_t i=0; i<MMCH_N_LAYERS; i++) {
    if (h_occupancy[i])    { delete h_occupancy[i];    h_occupancy[i] = 0; }
    if (hw_occupancy[i])   { delete hw_occupancy[i];   hw_occupancy[i] = 0; }
    if (hqmax_totevent[i]) { delete hqmax_totevent[i]; hqmax_totevent[i] = 0; }
    if (htmax_totevent[i]) { delete htmax_totevent[i]; htmax_totevent[i] = 0; }
    if (hqmax_perevent[i]) { delete hqmax_perevent[i]; hqmax_perevent[i] = 0; }
    if (htmax_perevent[i]) { delete htmax_perevent[i]; htmax_perevent[i] = 0; }
  }
}

void ChamberMonitor::Initialize()
{

  // Define derived quantities from fixed parameters
  for(UInt_t i=0; i<MMCH_N_LAYERS; i++) {
    fLowHV_XMin[i] = 0.5*(fNStrips*fStripPitch+fGeometryHole[i])-0.5*fLowHV_MeshGap[i]-fLowHV_MeshRadius;
    fLowHV_XMax[i] = 0.5*(fNStrips*fStripPitch+fGeometryHole[i])+0.5*fLowHV_MeshGap[i]+fLowHV_MeshRadius;
  }

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
    htmax_totevent[i] = new TH1D(TString::Format("htmax_totevent%d",i),TString::Format("htmax_totevent%d",i),100,0,700);
    //hqmax_strip[i] = new TH2D(TString::Format("hqmax_strip%d",i),TString::Format("hqmax_strip%d",i),fNStrips,0,xmax,1000,10,2500);;
    //htmax_strip[i] = new TH2D(TString::Format("htmax_strip%d",i),TString::Format("htmax_strip%d",i),fNStrips,0,xmax,1000,10,2500);
    hqmax_perevent[i] = new TH1D(TString::Format("hqmax_perevent%d",i),TString::Format("Q_max Layer %d; # strip ; Charge [ADC counts]",i),fNStrips,0,fNStrips);
    htmax_perevent[i] = new TH1D(TString::Format("htmax_perevent%d",i),TString::Format("Time Layer %d; # strip ; Time [ns]",i),fNStrips,0,fNStrips);
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

void ChamberMonitor::StartOfEvent()
{

  // Chamber rawdata file has no trigger info: assume all events are beam events
  fIsBeam = true;
  fIsCosmics = false;
  fIsRandom = false;
  fIsOffBeam = false;
  fBeamEventCount++;

  // Reset hits vectors
  for(int i=0; i<MMCH_N_LAYERS; i++) {
    t_mean[i].clear(); z_mean[i].clear(); x_mean[i].clear(); q_mean[i].clear(); c_mean[i].clear();
  }

}

void ChamberMonitor::EndOfEvent()
{

  if (fIsBeam) {

    if (fBeamOutputRate && (fBeamEventCount % fBeamOutputRate == 0)) {

      // Check if current data is new
      if ( (fVTime_Beam.size() == 0) || (fConfig->GetEventAbsTime().AsDouble() > fVTime_Beam.back()) ) {

	// Update trend vectors

	fVTime_Beam.push_back(fConfig->GetEventAbsTime().AsDouble());

	fVP1_BeamX.push_back(fP1_BeamX);
	fVP1_BeamY.push_back(fP1_BeamY);
	fVP2_BeamX.push_back(fP2_BeamX);
	fVP2_BeamY.push_back(fP2_BeamY);

	fVP1_BeamXSpread.push_back(fP1_BeamXSpread);
	fVP1_BeamYSpread.push_back(fP1_BeamYSpread);
	fVP2_BeamXSpread.push_back(fP2_BeamXSpread);
	fVP2_BeamYSpread.push_back(fP2_BeamYSpread);

	fVP1_BeamXCharge.push_back(fP1_BeamXCharge);
	fVP1_BeamYCharge.push_back(fP1_BeamYCharge);
	fVP2_BeamXCharge.push_back(fP2_BeamXCharge);
	fVP2_BeamYCharge.push_back(fP2_BeamYCharge);

	/*
	// Update trends file
	FILE* tf = fopen(fTFLGTrendsBM.Data(),"a");
	fprintf(tf,"%f %f %f %f %f %f\n",fVLGTimeBM.back(),fVLGNPoTsBM.back(),fVLGNPoTsTotBM.back(),fVLGBunchLengthBM.back(),fVLGBunchBBQBM.back(),fVLGBunchDensityBM.back());
	fclose(tf);
	*/

      }

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset histograms
      for(Int_t i=0; i<MMCH_N_LAYERS; i++) {
	//h_occupancy[i]->Reset();
	//hw_occupancy[i]->Reset();
	//hqmax_totevent[i]->Reset();
	//htmax_totevent[i]->Reset();
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

  Int_t layer, channel;
  std::vector<Double_t> camp;   

  for (UInt_t j=0; j<rawEv->mmLayer->size(); j++){  // loop on fired strips

    layer = rawEv->mmLayer->at(j);
    channel = rawEv->mmStrip->at(j);

    // Find hit coordinates by looping on 25ns bin sampling
    Double_t t_strip=0,x_strip=0,z_strip=0, q_strip=0;
    camp.clear();
    for(UInt_t n=0; n<rawEv->raw_q->at(j).size(); n++) camp.push_back(rawEv->raw_q->at(j).at(n));  
    CoordinateFinder(channel, layer, camp, t_strip, x_strip, z_strip, q_strip);

    // Fill summary vectors
    //std::cout <<"Layer "<<layer<<" channel "<<channel<<" : x="<<x_strip<<" z="<<z_strip<<" q_max="<<q_strip<<" time="<<t_strip<<std::endl;
    t_mean[layer].push_back(t_strip);
    x_mean[layer].push_back(x_strip);
    z_mean[layer].push_back(z_strip);
    q_mean[layer].push_back(q_strip);
    c_mean[layer].push_back(channel);

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

  // Compute beam spot properties (coordinates and total charge) from low HV sector
  ComputeBeamSpot();

}
void ChamberMonitor::CoordinateFinder(int iStrip, int iLayer, std::vector<double> camp, double &t_strip, double &x_strip, double &z_strip, double &q_strip) 
{

  UInt_t Nbins = camp.size();
  Double_t qtot = std::accumulate(camp.begin(),camp.end(),0);
  Double_t qtotT = 0.;
  Double_t threshold = 0.2 * qtot / Nbins;
  Double_t qmax = -1000.;      
  Double_t t0 = 0.; //offset tempo in ns DA FITTARE!

  t_strip = 0.;
  for (UInt_t ibin = 0; ibin < Nbins; ibin++) {

    Double_t qbin = camp.at(ibin); // Charge in the bin
    Double_t tbin = ibin*clock+clock/2.;  // Time (center of the bin)

    if (qbin > qmax) qmax = qbin;
    
    if (qbin < threshold) continue;
    
    t_strip += qbin * tbin;
    qtotT += qbin;

  }

  if (qtotT != 0.) {
    t_strip /= qtotT;
  } else {
    t_strip = 0.;
  }
  
  x_strip = (1.*iStrip-0.5)*fStripPitch;
  if (iStrip>256) x_strip += fGeometryHole[iLayer]; // Take into account small gap for second half of layer

  Double_t z_ion = 2.; //mm first signal from ionization

  if(iLayer<=3) {
    z_strip =  zm+z_ion-(t_strip-t0)*vd; // Plane 1 (close to the beam), local z is positive
  } else {
    z_strip = -zm-z_ion+(t_strip-t0)*vd; // Plane 2 (far from to the beam), local z is negative
  }

  q_strip = qmax;

}

Int_t ChamberMonitor::ComputeBeamSpot()
{

  fP1_BeamX = 0.;
  fP1_BeamY = 0.;
  fP2_BeamX = 0.;
  fP2_BeamY = 0.;

  fP1_BeamXSpread = 0.;
  fP1_BeamYSpread = 0.;
  fP2_BeamXSpread = 0.;
  fP2_BeamYSpread = 0.;

  fP1_BeamXCharge = 0.;
  fP1_BeamYCharge = 0.;
  fP2_BeamXCharge = 0.;
  fP2_BeamYCharge = 0.;

  // Compute weighted average position for each layer 
  for(Int_t i=0; i<MMCH_N_LAYERS; i++) {

    Double_t sum_q = 0.;
    Double_t sum_qx = 0.;
    Double_t sum_qx2 = 0.;
    for(UInt_t h=0; h<x_mean[i].size(); h++) {
      // Only use strips in the low HV zone
      if (x_mean[i][h]>fLowHV_XMin[i] && x_mean[i][h]<fLowHV_XMax[i]) {
	sum_q += q_mean[i][h];
	sum_qx += q_mean[i][h]*x_mean[i][h];
	sum_qx2 += q_mean[i][h]*x_mean[i][h]*x_mean[i][h];
      }
    }
    Double_t x = 0.;
    Double_t rms = 0.;
    if (sum_q != 0.) {
      x = sum_qx/sum_q; // Average beam position on this layer. Position of each channel is weighted with the corresponding qmax
      rms = sqrt(sum_qx2/sum_q-x*x); // Weighted variance s_w^2 = Sum_i(w_i*(x_i-x_w)^2)/Sum_i(w_i) where x_w is the weighted average
    }

    // Save computed quantitites to corresponding layer
    // Here we assume that beam is impacting on the XB-YR sector and is not very large
    // This algorithm should be improved
    switch(i) {
    case 0:          // P1YR
      fP1_BeamY = x;
      fP1_BeamYSpread = rms;
      fP1_BeamYCharge += sum_q;
      break;
    case 1:          // P1YL
      fP1_BeamYCharge += sum_q;
      break;
    case 2:          // P1XT
      fP1_BeamXCharge += sum_q;
      break;
    case 3:          // P1XB
      fP1_BeamX = x;
      fP1_BeamXSpread = rms;
      fP1_BeamXCharge += sum_q;
      break;
    case 4:          // P2YR
      fP2_BeamY = x;
      fP2_BeamYSpread = rms;
      fP2_BeamYCharge += sum_q;
      break;
    case 5:          // P2YL
      fP2_BeamYCharge += sum_q;
      break;
    case 6:          // P2XT
      fP2_BeamXCharge += sum_q;
      break;
    case 7:          // P2XB
      fP2_BeamX = x;
      fP2_BeamXSpread = rms;
      fP2_BeamXCharge += sum_q;
      break;
    }

  }

  return 0;
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

  UInt_t jout;

  // Beam position trend plots

  fprintf(outf,"PLOTID ChamberMon_trendbeamposx\n");
  fprintf(outf,"PLOTNAME MMCh Beam X Position - Run %s - %s\n",fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines+markers\", \"lines+markers\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP1_BeamX[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamX[j]);
      jout++;
    }
  }
  fprintf(outf,"],[");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP2_BeamX[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamX[j]);
      jout++;
    }
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID ChamberMon_trendbeamposy\n");
  fprintf(outf,"PLOTNAME MMCh Beam Y Position - Run %s - %s\n",fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines+markers\", \"lines+markers\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP1_BeamY[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamY[j]);
      jout++;
    }
  }
  fprintf(outf,"],[");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP2_BeamY[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamY[j]);
      jout++;
    }
  }
  fprintf(outf,"] ]\n\n");

  // Beam spread trend plots

  fprintf(outf,"PLOTID ChamberMon_trendbeamspreadx\n");
  fprintf(outf,"PLOTNAME MMCh Beam X Spread - Run %s - %s\n",fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines+markers\", \"lines+markers\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP1_BeamXSpread[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamXSpread[j]);
      jout++;
    }
  }
  fprintf(outf,"],[");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP2_BeamXSpread[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamXSpread[j]);
      jout++;
    }
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID ChamberMon_trendbeamspready\n");
  fprintf(outf,"PLOTNAME MMCh Beam Y Spread - Run %s - %s\n",fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines+markers\", \"lines+markers\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP1_BeamYSpread[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamYSpread[j]);
      jout++;
    }
  }
  fprintf(outf,"],[");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP2_BeamYSpread[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamYSpread[j]);
      jout++;
    }
  }
  fprintf(outf,"] ]\n\n");

  // Beam charge trend plots

  fprintf(outf,"PLOTID ChamberMon_trendbeamchargex\n");
  fprintf(outf,"PLOTNAME MMCh Beam X Charge - Run %s - %s\n",fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines+markers\", \"lines+markers\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Charge\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP1_BeamXCharge[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamXCharge[j]);
      jout++;
    }
  }
  fprintf(outf,"],[");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP2_BeamXCharge[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamXCharge[j]);
      jout++;
    }
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID ChamberMon_trendbeamchargey\n");
  fprintf(outf,"PLOTNAME MMCh Beam Y Charge - Run %s - %s\n",fConfig->RunName().Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines+markers\", \"lines+markers\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Charge\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP1_BeamYCharge[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamYCharge[j]);
      jout++;
    }
  }
  fprintf(outf,"],[");
  jout = 0;
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (fVP2_BeamYCharge[j] != 0.) {
      if (jout) fprintf(outf,",");
      fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamYCharge[j]);
      jout++;
    }
  }
  fprintf(outf,"] ]\n\n");

  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ChamberMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}
