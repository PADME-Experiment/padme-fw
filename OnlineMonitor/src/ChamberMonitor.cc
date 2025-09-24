
#include <stdio.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

#include "ChamberMonitor.hh"
#include "ChamberEvent.hh"

#include "Configuration.hh"

#include "TMath.h"
#include "TPRegexp.h"

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
    if (hxq_beam[i])       { delete hxq_beam[i];       hxq_beam[i] = 0; }
  }
  if (hxyq_P1) { delete hxyq_P1; }
  if (hxyq_P2) { delete hxyq_P2; }
}

void ChamberMonitor::Initialize()
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
  //printf("Run string is %s\n",fRunString.Data());
  
  // Define derived quantities from fixed parameters
  for(UInt_t i=0; i<MMCH_N_LAYERS; i++) {
    fGlobalShift[i] = 0.5*(fxmax+fGeometryHole[i]+fStripPitch);
    
    fLowHV_XMin[i] = -fLowHV_MeshRadius;
    fLowHV_XMax[i] = +fLowHV_MeshRadius;
    
    //fLowHV_XMin[i] = 0.5*(fNStrips*fStripPitch+fGeometryHole[i])-0.5*fLowHV_MeshGap[i]-fLowHV_MeshRadius;
    //fLowHV_XMax[i] = 0.5*(fNStrips*fStripPitch+fGeometryHole[i])+0.5*fLowHV_MeshGap[i]+fLowHV_MeshRadius;
    
  }
  
  
  ClearBeamAccumulators();
  
  // Get output rates from config file
  fBeamOutputRate = fConfigParser->HasConfig("RECO","BeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","BeamOutputRate")):500;
  fOffBeamOutputRate = fConfigParser->HasConfig("RECO","OffBeamOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","OffBeamOutputRate")):100;
  fCosmicsOutputRate = fConfigParser->HasConfig("RECO","CosmicsOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","CosmicsOutputRate")):0;
  fRandomOutputRate = fConfigParser->HasConfig("RECO","RandomOutputRate")?std::stoi(fConfigParser->GetSingleArg("RECO","RandomOutputRate")):100;
  
  // Define trend support file for this run
  fTFChTrendsBM = fConfig->TrendDirectory()+"/"+fConfig->RunName()+"_MMTrendsBM.trend";
  
  // If trend file exists, recover the data
  struct stat buffer;
  if (stat(fTFChTrendsBM.Data(),&buffer) == 0) {
    printf("- Reading trend file %s\n",fTFChTrendsBM.Data());
    std::ifstream tf(fTFChTrendsBM.Data());
    Double_t abstime,p1bx,p1by,p2bx,p2by,p1bsx,p1bsy,p2bsx,p2bsy,p1bxtq,p1bxbq,p1bylq,p1byrq,p2bxtq,p2bxbq,p2bylq,p2byrq;
    while (tf >> abstime >> p1bx >> p1by >> p2bx >> p2by >> p1bsx >> p1bsy >> p2bsx >> p2bsy >> p1bxtq >> p1bxbq >> p1bylq >> p1byrq >> p2bxtq >> p2bxbq >> p2bylq >> p2byrq) {
      
      fVTime_Beam.push_back(abstime);
      
      fVP1_BeamX.push_back(p1bx);
      fVP1_BeamY.push_back(p1by);
      fVP2_BeamX.push_back(p2bx);
      fVP2_BeamY.push_back(p2by);
      
      fVP1_BeamXSpread.push_back(p1bsx);
      fVP1_BeamYSpread.push_back(p1bsy);
      fVP2_BeamXSpread.push_back(p2bsx);
      fVP2_BeamYSpread.push_back(p2bsy);
      
      fVP1_BeamXTCharge.push_back(p1bxtq);
      fVP1_BeamXBCharge.push_back(p1bxbq);
      fVP1_BeamYLCharge.push_back(p1bylq);
      fVP1_BeamYRCharge.push_back(p1byrq);
      fVP2_BeamXTCharge.push_back(p2bxtq);
      fVP2_BeamXBCharge.push_back(p2bxbq);
      fVP2_BeamYLCharge.push_back(p2bylq);
      fVP2_BeamYRCharge.push_back(p2byrq);
      
    }
  }
  
  // Create histograms
  for(int i=0; i<MMCH_N_LAYERS; i++) {
    h_occupancy[i] = new TH1D(TString::Format("h_occupancy%d",i),TString::Format("Occupancy Layer %d",i),fNStrips,0,fNStrips);
    hw_occupancy[i] = new TH1D(TString::Format("hw_occupancy%d",i),TString::Format("Weighted Occupancy Layer %d",i),fNStrips,0,fNStrips);
    hqmax_totevent[i] = new TH1D(TString::Format("hqmax_totevent%d",i),TString::Format("hqmax_totevent%d",i),1000,10,2500);
    htmax_totevent[i] = new TH1D(TString::Format("htmax_totevent%d",i),TString::Format("htmax_totevent%d",i),100,0,700);
    hqmax_perevent[i] = new TH1D(TString::Format("hqmax_perevent%d",i),TString::Format("Q_max Layer %d; # strip ; Charge [ADC counts]",i),fNStrips,0,fNStrips);
    htmax_perevent[i] = new TH1D(TString::Format("htmax_perevent%d",i),TString::Format("Time Layer %d; # strip ; Time [ns]",i),fNStrips,0,fNStrips);
    hxq_beam[i] = new TH1D(TString::Format("hxq_beam%d",i),TString::Format("Beam pos q_weight %d; x [mm] ; #",i),n_bins_beam+fGeometryHoleNStrips[i],-n_bins_beam*fStripPitch*0.5-fGeometryHole[i]*0.5,n_bins_beam*fStripPitch*0.5+fGeometryHole[i]*0.5);
  }
  hxyq_P1 = new TH2D("hxyq_P1","Weighted Occupancy P1",n_bins6, -fGlobalShift[2]-fStripPitch*0.5,+fGlobalShift[2]+fStripPitch*0.5,n_bins6,-fGlobalShift[0]-fStripPitch*0.5,+fGlobalShift[0]+fStripPitch*0.5);
  hxyq_P2 = new TH2D("hxyq_P2","Weighted Occupancy P2",n_bins6, -fGlobalShift[6]-fStripPitch*0.5,+fGlobalShift[6]+fStripPitch*0.5,n_bins6,-fGlobalShift[4]-fStripPitch*0.5,+fGlobalShift[4]+fStripPitch*0.5);
  
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
	
	if (fP1_BeamX_N) {
	  fVP1_BeamX.push_back(fP1_BeamX/fP1_BeamX_N);
	} else {
	  fVP1_BeamX.push_back(0.);
	}
	if (fP1_BeamY_N) {
	  fVP1_BeamY.push_back(fP1_BeamY/fP1_BeamY_N);
	} else {
	  fVP1_BeamY.push_back(0.);
	}
	if (fP2_BeamX_N) {
	  fVP2_BeamX.push_back(fP2_BeamX/fP2_BeamX_N);
	} else {
	  fVP2_BeamX.push_back(0.);
	}
	if (fP2_BeamY_N) {
	  fVP2_BeamY.push_back(fP2_BeamY/fP1_BeamY_N);
	} else {
	  fVP2_BeamY.push_back(0.);
	}
	
	if (fP1_BeamXSpread_N) {
	  fVP1_BeamXSpread.push_back(fP1_BeamXSpread/fP1_BeamXSpread_N);
	} else {
	  fVP1_BeamXSpread.push_back(0.);
	}
	if (fP1_BeamYSpread_N) {
	  fVP1_BeamYSpread.push_back(fP1_BeamYSpread/fP1_BeamYSpread_N);
	} else {
	  fVP1_BeamYSpread.push_back(0.);
	}
	if (fP2_BeamXSpread_N) {
	  fVP2_BeamXSpread.push_back(fP2_BeamXSpread/fP2_BeamXSpread_N);
	} else {
	  fVP2_BeamXSpread.push_back(0.);
	}
	if (fP2_BeamYSpread_N) {
	  fVP2_BeamYSpread.push_back(fP2_BeamYSpread/fP2_BeamYSpread_N);
	} else {
	  fVP2_BeamYSpread.push_back(0.);
	}
	
	if (fP1_BeamXTCharge_N) {
	  fVP1_BeamXTCharge.push_back(fP1_BeamXTCharge/fP1_BeamXTCharge_N);
	} else {
	  fVP1_BeamXTCharge.push_back(0.);
	}
	if (fP1_BeamXBCharge_N) {
	  fVP1_BeamXBCharge.push_back(fP1_BeamXBCharge/fP1_BeamXBCharge_N);
	} else {
	  fVP1_BeamXBCharge.push_back(0.);
	}
	if (fP1_BeamYLCharge_N) {
	  fVP1_BeamYLCharge.push_back(fP1_BeamYLCharge/fP1_BeamYLCharge_N);
	} else {
	  fVP1_BeamYLCharge.push_back(0.);
	}
	if (fP1_BeamYRCharge_N) {
	  fVP1_BeamYRCharge.push_back(fP1_BeamYRCharge/fP1_BeamYRCharge_N);
	} else {
	  fVP1_BeamYRCharge.push_back(0.);
	}
	if (fP2_BeamXTCharge_N) {
	  fVP2_BeamXTCharge.push_back(fP2_BeamXTCharge/fP2_BeamXTCharge_N);
	} else {
	  fVP2_BeamXTCharge.push_back(0.);
	}
	if (fP2_BeamXBCharge_N) {
	  fVP2_BeamXBCharge.push_back(fP2_BeamXBCharge/fP2_BeamXBCharge_N);
	} else {
	  fVP2_BeamXBCharge.push_back(0.);
	}
	if (fP2_BeamYLCharge_N) {
	  fVP2_BeamYLCharge.push_back(fP2_BeamYLCharge/fP2_BeamYLCharge_N);
	} else {
	  fVP2_BeamYLCharge.push_back(0.);
	}
	if (fP2_BeamYRCharge_N) {
	  fVP2_BeamYRCharge.push_back(fP2_BeamYRCharge/fP2_BeamYRCharge_N);
	} else {
	  fVP2_BeamYRCharge.push_back(0.);
	}
	
	//++++++William
	/*for(Int_t i=0; i<MMCH_N_LAYERS; i++) {
	  TF1* fgaus = new TF1("fgaus","gaus",-50,50);
	  fgaus->SetParameters(hxq_beam[i]->GetSum(),hxq_beam[i]->GetMean(),hxq_beam[i]->GetRMS());
	  hxq_beam[i]->Fit(fgaus,"RQ");
	  double norm_temp = fgaus->GetParameter(0);
	  double mean_temp = fgaus->GetParameter(1);
	  double sigma_temp = fgaus->GetParameter(2);
	  double chi2_temp = fgaus->GetChisquare();
	  double ndf_temp = fgaus->GetNDF();
	  
	  if(chi2_temp/ndf_temp > 10.) {
	    norm_temp = hxq_beam[i]->GetSum();
	    mean_temp = hxq_beam[i]->GetMean();
	    sigma_temp = hxq_beam[i]->GetRMS();
	  }
	  
	  fgaus->SetParameters(norm_temp,mean_temp,sigma_temp);
	  
	  if(i==0){ 
	    double xmin_zoom = mean_temp-2*sigma_temp;
	    double xmax_zoom = mean_temp+2*sigma_temp;
	    hxq_beam[i]->Fit(fgaus,"Q","",xmin_zoom,xmax_zoom);
	    
	    double mean = fgaus->GetParameter(1);
	    double sigma = fgaus->GetParameter(2);
	    double chi2 = fgaus->GetChisquare();
	    double ndf = fgaus->GetNDF();
	    
	    if(chi2/ndf > 10.) {
	      mean = hxq_beam[i]->GetMean();
	      sigma = hxq_beam[i]->GetRMS();
	    }
	    
	    //fVbeam_meanYP1.push_back(fgaus->GetParameter(1));
	    //fVbeam_spreadYP1.push_back(fgaus->GetParameter(2));
	    fVP1_BeamY.push_back(mean);
	    fVP1_BeamYSpread.push_back(sigma);
	    
	  }
	  if(i==3){ 
	    double xmin_zoom = mean_temp-2*sigma_temp;
	    double xmax_zoom = mean_temp+1.5*sigma_temp;
	    hxq_beam[i]->Fit(fgaus,"Q","",xmin_zoom,xmax_zoom);
	    
	    double mean = fgaus->GetParameter(1);
	    double sigma = fgaus->GetParameter(2);
	    double chi2 = fgaus->GetChisquare();
	    double ndf = fgaus->GetNDF();
	    
	    if(chi2/ndf > 10.) {
	      mean = hxq_beam[i]->GetMean();
	      sigma = hxq_beam[i]->GetRMS();
	    }
	    
	    //fVbeam_meanXP1.push_back(fgaus->GetParameter(1));
	    //fVbeam_spreadXP1.push_back(fgaus->GetParameter(2));
	    fVP1_BeamX.push_back(mean);
	    fVP1_BeamXSpread.push_back(sigma);
	    
	  }
	  if(i==4){ 
	    double xmin_zoom = mean_temp-2*sigma_temp;
	    double xmax_zoom = mean_temp+2*sigma_temp;
	    hxq_beam[i]->Fit(fgaus,"Q","",xmin_zoom,xmax_zoom);
	    
	    double mean = fgaus->GetParameter(1);
	    double sigma = fgaus->GetParameter(2);
	    double chi2 = fgaus->GetChisquare();
	    double ndf = fgaus->GetNDF();
	    
	    if(chi2/ndf > 10.) {
	      mean = hxq_beam[i]->GetMean();
	      sigma = hxq_beam[i]->GetRMS();
	    }
	    
	    //fVbeam_meanYP2.push_back(fgaus->GetParameter(1));
	    //fVbeam_spreadYP2.push_back(fgaus->GetParameter(2));
	    fVP2_BeamX.push_back(mean);
	    fVP2_BeamYSpread.push_back(sigma);
	    
	  }
	  if(i==7){ 
	    double xmin_zoom = mean_temp-2*sigma_temp;
	    double xmax_zoom = mean_temp+2*sigma_temp;
	    hxq_beam[i]->Fit(fgaus,"Q","",xmin_zoom,xmax_zoom);
	    
	    double mean = fgaus->GetParameter(1);
	    double sigma = fgaus->GetParameter(2);
	    double chi2 = fgaus->GetChisquare();
	    double ndf = fgaus->GetNDF();
	    
	    if(chi2/ndf > 10.) {
	      mean = hxq_beam[i]->GetMean();
	      sigma = hxq_beam[i]->GetRMS();
	    }
	    
	    //fVbeam_meanXP2.push_back(fgaus->GetParameter(1));
	    //fVbeam_spreadXP2.push_back(fgaus->GetParameter(2));
	    fVP2_BeamX.push_back(mean);
	    fVP2_BeamXSpread.push_back(sigma);
	    
	  }
	  delete fgaus;
	  }*/
	//++++++William
	
	// Update trends file
	FILE* tf = fopen(fTFChTrendsBM.Data(),"a");
	fprintf(tf,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
		fVTime_Beam.back(),fVP1_BeamX.back(),fVP1_BeamY.back(),fVP2_BeamX.back(),fVP2_BeamY.back(),
		fVP1_BeamXSpread.back(),fVP1_BeamYSpread.back(),fVP2_BeamXSpread.back(),fVP2_BeamYSpread.back(),
		fVP1_BeamXTCharge.back(),fVP1_BeamXBCharge.back(),fVP1_BeamYLCharge.back(),fVP1_BeamYRCharge.back(),
		fVP2_BeamXTCharge.back(),fVP2_BeamXBCharge.back(),fVP2_BeamYLCharge.back(),fVP2_BeamYRCharge.back());
	fclose(tf);
	
	ClearBeamAccumulators();
	
      }

      // Write beam events data to output PadmeMonitor file
      OutputBeam();

      // Reset histograms
      for(Int_t i=0; i<MMCH_N_LAYERS; i++) {
	h_occupancy[i]->Reset();
	hw_occupancy[i]->Reset();
	hqmax_totevent[i]->Reset();
	htmax_totevent[i]->Reset();
	hqmax_perevent[i]->Reset();
	htmax_perevent[i]->Reset();
	hxq_beam[i]->Reset();
      }
      hxyq_P1->Reset();
      hxyq_P1->Reset();
      
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
    t_mean[layer].push_back(t_strip);
    x_mean[layer].push_back(x_strip);
    z_mean[layer].push_back(z_strip);
    q_mean[layer].push_back(q_strip);
    c_mean[layer].push_back(channel);
    
    // Fill event histograms
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
  //ComputeBeamSpot();
  ComputeBeamSpot_v2();
  
  // Compute 2D occupancy histogram for the two Planes (disabled for the moment)
  Compute2DOccupancy();
  
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
  
  
  x_strip = (1.*iStrip-1.)*fStripPitch;
  if (iStrip>256) x_strip += fGeometryHole[iLayer]; // Take into account small gap for second half of layer
  
  x_strip -= fGlobalShift[iLayer];//Global coordinate translation
  
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
  
  Double_t expected_spread = 10.; // Expected spread around estimated x in mm
  
  Double_t sum_x1_q = 0.;
  Double_t sum_x1_qx = 0.;
  Double_t sum_x1_qx2 = 0.;
  UInt_t sum_x1_used = 0;
  Double_t sum_y1_q = 0.;
  Double_t sum_y1_qx = 0.;
  Double_t sum_y1_qx2 = 0.;
  UInt_t sum_y1_used = 0;
  Double_t sum_x2_q = 0.;
  Double_t sum_x2_qx = 0.;
  Double_t sum_x2_qx2 = 0.;
  UInt_t sum_x2_used = 0;
  Double_t sum_y2_q = 0.;
  Double_t sum_y2_qx = 0.;
  Double_t sum_y2_qx2 = 0.;
  UInt_t sum_y2_used = 0;

  Double_t x,rms;
  Double_t sum_q,sum_qx,sum_qx2;
  UInt_t used;
  
  // Compute weighted average position for each layer. Position of each channel is weighted with the corresponding qmax
  for(Int_t i=0; i<MMCH_N_LAYERS; i++) {
    
    x = 0.;
    sum_q = 0.;
    sum_qx = 0.;
    sum_qx2 = 0.;
    used = 0;
    
    for(UInt_t h=0; h<x_mean[i].size(); h++) {
      if (x_mean[i][h]>fLowHV_XMin[i] && x_mean[i][h]<fLowHV_XMax[i]) { // Only use strips in the low HV zone
	sum_q += q_mean[i][h];
	sum_qx += q_mean[i][h]*x_mean[i][h];
	used++;
      }
    }
    
    if (used>=1) {
      
      x = sum_qx/sum_q; // First estimate of average beam position on this layer
      
      sum_q = 0.;
      sum_qx = 0.;
      sum_qx2 = 0.;
      used = 0;
      for(UInt_t h=0; h<x_mean[i].size(); h++) {
	if (x_mean[i][h]>std::max(fLowHV_XMin[i],x-expected_spread) && x_mean[i][h]<std::min(fLowHV_XMax[i],x+expected_spread)) { // Only use strips close to the estimated average beam position
	  used++;
	  sum_q += q_mean[i][h];
	  sum_qx += q_mean[i][h]*x_mean[i][h];
	  sum_qx2 += q_mean[i][h]*x_mean[i][h]*x_mean[i][h];
	}
      }
      
    }
    
    // Save cumulative values to compute beam position from a full layer
    if (i == 0 ){//|| i == 1) { // P1Y
      sum_y1_q = sum_q;
      sum_y1_qx = sum_qx;
      sum_y1_qx2 = sum_qx2;
      sum_y1_used = used;
    } else if (i == 3 ){//|| i == 3) { // P1X
      sum_x1_q = sum_q;
      sum_x1_qx = sum_qx;
      sum_x1_qx2 = sum_qx2;
      sum_x1_used = used;
    } else if (i == 4 ){//|| i == 5) { // P2Y
      sum_y2_q = sum_q;
      sum_y2_qx = sum_qx;
      sum_y2_qx2 = sum_qx2;
      sum_y2_used = used;
    } else if (i == 7 ){//|| i == 7) { // P2X
      sum_x2_q = sum_q;
      sum_x2_qx = sum_qx;
      sum_x2_qx2 = sum_qx2;
      sum_x2_used = used;
    }
    
    // Save total charge for each layer
    if (used>=1) {
      switch(i) {
      case 0:          // P1YR
	fP1_BeamYRCharge += sum_q;
	fP1_BeamYRCharge_N++;
	break;
      case 1:          // P1YL
	fP1_BeamYLCharge += sum_q;
	fP1_BeamYLCharge_N++;
	break;
      case 2:          // P1XT
	fP1_BeamXTCharge += sum_q;
	fP1_BeamXTCharge_N++;
	break;
      case 3:          // P1XB
	fP1_BeamXBCharge += sum_q;
	fP1_BeamXBCharge_N++;
	break;
      case 4:          // P2YR
	fP2_BeamYRCharge += sum_q;
	fP2_BeamYRCharge_N++;
	break;
      case 5:          // P2YL
	fP2_BeamYLCharge += sum_q;
	fP2_BeamYLCharge_N++;
	break;
      case 6:          // P2XT
	fP2_BeamXTCharge += sum_q;
	fP2_BeamXTCharge_N++;
	break;
      case 7:          // P2XB
	fP2_BeamXBCharge += sum_q;
	fP2_BeamXBCharge_N++;
	break;
      }
    }
    
  }
  
  // Compute X/Y coordinates for each plane
  
  if (sum_y1_used >= 1) {
    x = sum_y1_qx/sum_y1_q;
    fP1_BeamY += x;
    fP1_BeamY_N++;
    if (sum_y1_used >= 2) {
      rms = sqrt(sum_y1_qx2/sum_y1_q-x*x);
      fP1_BeamYSpread += rms;
      fP1_BeamYSpread_N++;
    }
  }
  
  if (sum_x1_used >= 1) {
    x = sum_x1_qx/sum_x1_q;
    fP1_BeamX += x;
    fP1_BeamX_N++;
    if (sum_x1_used >= 2) {
      rms = sqrt(sum_x1_qx2/sum_x1_q-x*x);
      fP1_BeamXSpread += rms;
      fP1_BeamXSpread_N++;
    }
  }
  
  if (sum_y2_used >= 1) {
    x = sum_y2_qx/sum_y2_q;
    fP2_BeamY += x;
    fP2_BeamY_N++;
    if (sum_y2_used >= 2) {
      rms = sqrt(sum_y2_qx2/sum_y2_q-x*x);
      fP2_BeamYSpread += rms;
      fP2_BeamYSpread_N++;
    }
  }
  
  if (sum_x2_used >= 1) {
    x = sum_x2_qx/sum_x2_q;
    fP2_BeamX += x;
    fP2_BeamX_N++;
    if (sum_x2_used >= 2) {
      rms = sqrt(sum_x2_qx2/sum_x2_q-x*x);
      fP2_BeamXSpread += rms;
      fP2_BeamXSpread_N++;
    }
  }
  
  fNEventsBeam++;
  
  return 0;
}

Int_t ChamberMonitor::ComputeBeamSpot_v2()
{

  Double_t expected_spread = 20.; // Expected spread around estimated x in mm
  
  Double_t sum_x1_q = 0.;
  Double_t sum_x1_qx = 0.;
  Double_t sum_x1_qx2 = 0.;
  UInt_t sum_x1_used = 0;
  Double_t sum_y1_q = 0.;
  Double_t sum_y1_qx = 0.;
  Double_t sum_y1_qx2 = 0.;
  UInt_t sum_y1_used = 0;
  Double_t sum_x2_q = 0.;
  Double_t sum_x2_qx = 0.;
  Double_t sum_x2_qx2 = 0.;
  UInt_t sum_x2_used = 0;
  Double_t sum_y2_q = 0.;
  Double_t sum_y2_qx = 0.;
  Double_t sum_y2_qx2 = 0.;
  UInt_t sum_y2_used = 0;
  
  Double_t x,rms;
  Double_t sum_q,sum_qx,sum_qx2;
  UInt_t used;
  
  // Compute weighted average position for each layer. Position of each channel is weighted with the corresponding qmax
  for(Int_t i=0; i<MMCH_N_LAYERS; i++) {
    
    x = 0.;
    sum_q = 0.;
    sum_qx = 0.;
    sum_qx2 = 0.;
    used = 0;
    
    for(UInt_t h=0; h<x_mean[i].size(); h++) {
      if (x_mean[i][h]>fLowHV_XMin[i] && x_mean[i][h]<fLowHV_XMax[i]) { // Only use strips in the low HV zone
	if(i == 0 && t_mean[i][h]>(mean_t_beam0-2*sigma_t_beam0) && t_mean[i][h]<(mean_t_beam0+2*sigma_t_beam0)) { //time cut layer 0
	  sum_q += q_mean[i][h];
	  sum_qx += q_mean[i][h]*x_mean[i][h];
	  used++;
	}
	else if(i == 3 && t_mean[i][h]>(mean_t_beam3-2*sigma_t_beam3) && t_mean[i][h]<(mean_t_beam3+2*sigma_t_beam3)) { //time cut layer 3
	  sum_q += q_mean[i][h];
	  sum_qx += q_mean[i][h]*x_mean[i][h];
	  used++;
	}
	else if(i == 4 && t_mean[i][h]>(mean_t_beam4-2*sigma_t_beam4) && t_mean[i][h]<(mean_t_beam4+2*sigma_t_beam4)) { //time cut layer 4
	  sum_q += q_mean[i][h];
	  sum_qx += q_mean[i][h]*x_mean[i][h];
	  used++;
	}
	else if(i == 7 && t_mean[i][h]>(mean_t_beam7-2*sigma_t_beam7) && t_mean[i][h]<(mean_t_beam7+2*sigma_t_beam7)) { //time cut layer 7
	  sum_q += q_mean[i][h];
	  sum_qx += q_mean[i][h]*x_mean[i][h];
	  used++;
	}
	else {
	  sum_q += q_mean[i][h];
	  sum_qx += q_mean[i][h]*x_mean[i][h];
	  used++;
	}
      }
    }
    
    if (used>=1) {
      
      x = sum_qx/sum_q; // First estimate of average beam position on this layer
      
      sum_q = 0.;
      sum_qx = 0.;
      sum_qx2 = 0.;
      used = 0;
      for(UInt_t h=0; h<x_mean[i].size(); h++) {
	if (x_mean[i][h]>std::max(fLowHV_XMin[i],x-expected_spread) && x_mean[i][h]<std::min(fLowHV_XMax[i],x+expected_spread)) { // Only use strips close to the estimated average beam position
	  if(i == 0 && t_mean[i][h]>(mean_t_beam0-2*sigma_t_beam0) && t_mean[i][h]<(mean_t_beam0+2*sigma_t_beam0)) { //time cut layer 0
	    used++;
	    sum_q += q_mean[i][h];
	    sum_qx += q_mean[i][h]*x_mean[i][h];
	    sum_qx2 += q_mean[i][h]*x_mean[i][h]*x_mean[i][h];
	    hxq_beam[i]->Fill(x_mean[i][h],q_mean[i][h]);
	  }
	  else if(i == 3 && t_mean[i][h]>(mean_t_beam3-2*sigma_t_beam3) && t_mean[i][h]<(mean_t_beam3+2*sigma_t_beam3)) { //time cut layer 3
	    used++;
	    sum_q += q_mean[i][h];
	    sum_qx += q_mean[i][h]*x_mean[i][h];
	    sum_qx2 += q_mean[i][h]*x_mean[i][h]*x_mean[i][h];
	    hxq_beam[i]->Fill(x_mean[i][h],q_mean[i][h]);
	  }
	  else if(i == 4 && t_mean[i][h]>(mean_t_beam4-2*sigma_t_beam4) && t_mean[i][h]<(mean_t_beam4+2*sigma_t_beam4)) { //time cut layer 4
	    used++;
	    sum_q += q_mean[i][h];
	    sum_qx += q_mean[i][h]*x_mean[i][h];
	    sum_qx2 += q_mean[i][h]*x_mean[i][h]*x_mean[i][h];
	    hxq_beam[i]->Fill(x_mean[i][h],q_mean[i][h]);
	  }
	  else if(i == 7 && t_mean[i][h]>(mean_t_beam7-2*sigma_t_beam7) && t_mean[i][h]<(mean_t_beam7+2*sigma_t_beam7)) { //time cut layer 7
	    used++;
	    sum_q += q_mean[i][h];
	    sum_qx += q_mean[i][h]*x_mean[i][h];
	    sum_qx2 += q_mean[i][h]*x_mean[i][h]*x_mean[i][h];
	    hxq_beam[i]->Fill(x_mean[i][h],q_mean[i][h]);
	  }
	  else {
	    used++;
	    sum_q += q_mean[i][h];
	    sum_qx += q_mean[i][h]*x_mean[i][h];
	    sum_qx2 += q_mean[i][h]*x_mean[i][h]*x_mean[i][h];
	    hxq_beam[i]->Fill(x_mean[i][h],q_mean[i][h]);
	  }
	}
      }
      
    }
    
    // Save cumulative values to compute beam position from a full layer
    if (i == 0 ){//|| i == 1) { // P1Y
      sum_y1_q += sum_q;
      sum_y1_qx += sum_qx;
      sum_y1_qx2 += sum_qx2;
      sum_y1_used += used;
    } else if (i == 3 ){//|| i == 3) { // P1X
      sum_x1_q += sum_q;
      sum_x1_qx += sum_qx;
      sum_x1_qx2 += sum_qx2;
      sum_x1_used += used;
    } else if (i == 4 ){//|| i == 5) { // P2Y
      sum_y2_q += sum_q;
      sum_y2_qx += sum_qx;
      sum_y2_qx2 += sum_qx2;
      sum_y2_used += used;
    } else if (i == 7 ){//|| i == 7) { // P2X
      sum_x2_q += sum_q;
      sum_x2_qx += sum_qx;
      sum_x2_qx2 += sum_qx2;
      sum_x2_used += used;
    }
    
    // Save total charge for each layer
    if (used>=1) {
      switch(i) {
      case 0:          // P1YR
	fP1_BeamYRCharge += sum_q;
	fP1_BeamYRCharge_N++;
	break;
      case 1:          // P1YL
	fP1_BeamYLCharge += sum_q;
	fP1_BeamYLCharge_N++;
	break;
      case 2:          // P1XT
	fP1_BeamXTCharge += sum_q;
	fP1_BeamXTCharge_N++;
	break;
      case 3:          // P1XB
	fP1_BeamXBCharge += sum_q;
	fP1_BeamXBCharge_N++;
	break;
      case 4:          // P2YR
	fP2_BeamYRCharge += sum_q;
	fP2_BeamYRCharge_N++;
	break;
      case 5:          // P2YL
	fP2_BeamYLCharge += sum_q;
	fP2_BeamYLCharge_N++;
	break;
      case 6:          // P2XT
	fP2_BeamXTCharge += sum_q;
	fP2_BeamXTCharge_N++;
	break;
      case 7:          // P2XB
	fP2_BeamXBCharge += sum_q;
	fP2_BeamXBCharge_N++;
	break;
      }
    }
    
  }
  
  // Compute X/Y coordinates for each plane

  if (sum_y1_used >= 1) {
    x = sum_y1_qx/sum_y1_q;
    fP1_BeamY += x;
    fP1_BeamY_N++;
    if (sum_y1_used >= 2) {
      rms = sqrt(sum_y1_qx2/sum_y1_q-x*x);
      fP1_BeamYSpread += rms;
      fP1_BeamYSpread_N++;
    }
  }
  
  if (sum_x1_used >= 1) {
    x = sum_x1_qx/sum_x1_q;
    fP1_BeamX += x;
    fP1_BeamX_N++;
    if (sum_x1_used >= 2) {
      rms = sqrt(sum_x1_qx2/sum_x1_q-x*x);
      fP1_BeamXSpread += rms;
      fP1_BeamXSpread_N++;
    }
  }
  
  if (sum_y2_used >= 1) {
    x = sum_y2_qx/sum_y2_q;
    fP2_BeamY += x;
    fP2_BeamY_N++;
    if (sum_y2_used >= 2) {
      rms = sqrt(sum_y2_qx2/sum_y2_q-x*x);
      fP2_BeamYSpread += rms;
      fP2_BeamYSpread_N++;
    }
  }
  
  if (sum_x2_used >= 1) {
    x = sum_x2_qx/sum_x2_q;
    fP2_BeamX += x;
    fP2_BeamX_N++;
    if (sum_x2_used >= 2) {
      rms = sqrt(sum_x2_qx2/sum_x2_q-x*x);
      fP2_BeamXSpread += rms;
      fP2_BeamXSpread_N++;
    }
  }
  
  fNEventsBeam++;
  
  return 0;
}


Int_t ChamberMonitor::Compute2DOccupancy(){
  for(int yl=0; yl<6; yl++) {
    if(yl == 2 || yl == 3) continue;
    for(int ihit=0; ihit<(int)x_mean[yl].size(); ihit++){ //Layer 0                                                                          
      double yy = x_mean[yl][ihit];
      double qy = q_mean[yl][ihit];
      int cy = c_mean[yl][ihit];
      int xl, couple;
      if(yl == 0 || yl == 1){           // P1 Y layers                                                                              
	if(cy <= fNStripsHalf) xl = 3; // X bottom layer                                                                           
	else xl = 2;                    // X top layer                                                                              
	couple = yl;
      }
      if(yl == 4 || yl == 5) {          // P2 Y layers                                                                              
	if(cy <= fNStripsHalf) xl = 7; // X bottom layer                                                                           
	else xl = 6;                    // X top layer                                                                              
	couple = yl-4;
      }
      for(int jhit=0; jhit<(int)x_mean[xl].size(); jhit++) {
	int cx = c_mean[xl][jhit];
	if(cx <= fNStripsHalf*(couple+1) && cx> fNStripsHalf*couple) {
	  double xx = x_mean[xl][jhit];
	  double qx = q_mean[xl][jhit];
	  if(yl < 2) {
	    hxyq_P1->Fill(xx,yy,qx*qy);
	  }
	  else {
	    hxyq_P2->Fill(xx,yy,qx*qy);
	  }
	}
      }
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
    fprintf(outf,"PLOTNAME MMCh %s Occupancy - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    fprintf(outf,"PLOTNAME MMCh %s Weighted Occupancy - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    fprintf(outf,"PLOTNAME MMCh %s QMax Total Event - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    fprintf(outf,"PLOTNAME MMCh %s TMax Total Event - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    fprintf(outf,"PLOTNAME MMCh %s QMax Single Event - %s Event %d - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    fprintf(outf,"PLOTNAME MMCh %s TMax Single Event - %s Event %d - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->GetEventNumber(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
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
    
    // Layer Weighted Occupancy Beam
    fprintf(outf,"PLOTID MMCh_xq_beam%d\n",i);
    fprintf(outf,"PLOTTYPE histo1d\n");
    fprintf(outf,"PLOTNAME MMCh %s Weighted Occupancy - %s - %s\n",mmch_tag[i].Data(),fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
    fprintf(outf,"CHANNELS %d\n",hxq_beam[i]->GetNbinsX());
    fprintf(outf,"RANGE_X %.3f %.3f\n",hxq_beam[i]->GetXaxis()->GetXmin(),hxq_beam[i]->GetXaxis()->GetXmax());
    if((i/2)%2 == 0) {
      fprintf(outf,"TITLE_X Y [mm]\n");
      fprintf(outf,"TITLE_Y W_Hits\n");
      fprintf(outf,"LEGEND [ \"Y: %.1f\", \"RMS:%.2f\" ]\n", hxq_beam[i]->GetMean(), hxq_beam[i]->GetRMS());
    }
    else {
      fprintf(outf,"TITLE_X X [mm]\n");
      fprintf(outf,"TITLE_Y W_Hits\n");
      fprintf(outf,"LEGEND [ \"X: %.1f\", \"RMS:%.2f\" ]\n", hxq_beam[i]->GetMean(), hxq_beam[i]->GetRMS());
    }
    fprintf(outf,"DATA [[");
    for(Int_t b = 1; b <= hxq_beam[i]->GetNbinsX(); b++) {
      if (b>1) fprintf(outf,",");
      fprintf(outf,"%.0f",hxq_beam[i]->GetBinContent(b));
    }
    fprintf(outf,"]]\n\n");
    
    
  }
  
  
  // Beam position trend plots
  
  fprintf(outf,"PLOTID ChamberMon_trendbeamposx\n");
  fprintf(outf,"PLOTNAME MMCh Beam X Position - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
    fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\", \"P1 fit\", \"P2 fit\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamX[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamX[j]);
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID ChamberMon_trendbeamposy\n");
  fprintf(outf,"PLOTNAME MMCh Beam Y Position - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamY[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamY[j]);
  }
  fprintf(outf,"] ]\n\n");

  //Beam trend x-y combined plots - MM 18.06
  
  fprintf(outf,"PLOTID ChamberMon_trendbeamposxy_P1\n");
  fprintf(outf,"PLOTNAME Beam pos at P1-MM - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y position [mm]\n");
  fprintf(outf,"LEGEND [ \"X\", \"Y\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamX[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamY[j]);
  }
  fprintf(outf,"] ]\n\n");


  // Beam spread trend plots

  fprintf(outf,"PLOTID ChamberMon_trendbeamspreadx\n");
  fprintf(outf,"PLOTNAME MMCh Beam X Spread - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP1_BeamXSpread[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP2_BeamXSpread[j]);
  }
  fprintf(outf,"] ]\n\n");
  
  fprintf(outf,"PLOTID ChamberMon_trendbeamspready\n");
  fprintf(outf,"PLOTNAME MMCh Beam Y Spread - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y [mm]\n");
  fprintf(outf,"LEGEND [ \"P1\", \"P2\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP1_BeamYSpread[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP2_BeamYSpread[j]);
  }
  fprintf(outf,"] ]\n\n");

  //Beam spread x-y combined trend plots - MM 18.06
  
  fprintf(outf,"PLOTID ChamberMon_trendbeamspreadxy_P1\n");
  fprintf(outf,"PLOTNAME Beam RMS at P1-MM - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"dc408b\", \"209b8e\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y RMS [mm]\n");
  fprintf(outf,"LEGEND [ \"RMS X\", \"RMS Y\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP1_BeamXSpread[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.2f]",fVTime_Beam[j],fVP1_BeamYSpread[j]);
  }
  fprintf(outf,"] ]\n\n");

  // Beam charge trend plots

  fprintf(outf,"PLOTID ChamberMon_trendbeamchargex\n");
  fprintf(outf,"PLOTNAME MMCh Beam X Charge - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\", \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\", \"00ff00\", \"ff00ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Charge\n");
  fprintf(outf,"LEGEND [ \"P1XT\", \"P1XB\", \"P2XT\", \"P2XB\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamXTCharge[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamXBCharge[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamXTCharge[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamXBCharge[j]);
  }
  fprintf(outf,"] ]\n\n");

  fprintf(outf,"PLOTID ChamberMon_trendbeamchargey\n");
  fprintf(outf,"PLOTNAME MMCh Beam Y Charge - %s - %s\n",fRunString.Data(),fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"PLOTTYPE timeline\n");
  fprintf(outf,"MODE [ \"lines\", \"lines\", \"lines\", \"lines\" ]\n");
  fprintf(outf,"COLOR [ \"ff0000\", \"0000ff\", \"00ff00\", \"ff00ff\" ]\n");
  fprintf(outf,"TITLE_X Time\n");
  fprintf(outf,"TITLE_Y Charge\n");
  fprintf(outf,"LEGEND [ \"P1YR\", \"P1YL\", \"P2YR\", \"P2YL\" ]\n");
  fprintf(outf,"DATA [ [");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamYRCharge[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP1_BeamYLCharge[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamYRCharge[j]);
  }
  fprintf(outf,"],[");
  for(UInt_t j = 0; j<fVTime_Beam.size(); j++) {
    if (j) fprintf(outf,",");
    fprintf(outf,"[\"%f\",%.1f]",fVTime_Beam[j],fVP2_BeamYLCharge[j]);
  }
  fprintf(outf,"] ]\n\n");
  
   
  //2D occupancy P1
  fprintf(outf,"PLOTID ChamberMon_2DOccupancy_P1\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME MM-P1 XY map - %s\n",fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 86 86\n");
  fprintf(outf,"RANGE_X %.1f %.1f\n",hxyq_P1->GetXaxis()->GetXmin(),hxyq_P1->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.1f %.1f\n",hxyq_P1->GetYaxis()->GetXmin(),hxyq_P1->GetYaxis()->GetXmax());
  fprintf(outf,"COLORSCALE Hot\n");
  fprintf(outf,"TITLE_X X (mm)\n");
  fprintf(outf,"TITLE_Y Y (mm)\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<n_bins6;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<n_bins6;x++) {
      if (x>0) fprintf(outf,",");   
      fprintf(outf,"%.3f",hxyq_P1->GetBinContent(x+1,y+1)/(Double_t)fBeamOutputRate);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");
  
  //2D Occupancy P2
  fprintf(outf,"PLOTID ChamberMon_2DOccupancy_P2\n");
  fprintf(outf,"PLOTTYPE heatmap\n");
  fprintf(outf,"PLOTNAME MM-P2 XY map - %s\n",fConfig->FormatTime(fConfig->GetEventAbsTime()));
  fprintf(outf,"CHANNELS 86 86\n");
  fprintf(outf,"RANGE_X %.1f %.1f\n",hxyq_P2->GetXaxis()->GetXmin(),hxyq_P2->GetXaxis()->GetXmax());
  fprintf(outf,"RANGE_Y %.1f %.1f\n",hxyq_P2->GetYaxis()->GetXmin(),hxyq_P2->GetYaxis()->GetXmax());
  fprintf(outf,"TITLE_X X\n");
  fprintf(outf,"TITLE_Y Y\n");
  fprintf(outf,"DATA [");
  for(UChar_t y = 0;y<n_bins6;y++) {
    if (y>0) fprintf(outf,",");
    fprintf(outf,"[");
    for(UChar_t x = 0;x<n_bins6;x++) {
      if (x>0) fprintf(outf,",");   
      fprintf(outf,"%.3f",hxyq_P2->GetBinContent(x+1,y+1)/(Double_t)fBeamOutputRate);
    }
    fprintf(outf,"]");
  }
  fprintf(outf,"]\n\n");


  
  fclose(outf);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ChamberMonitor::OutputBeam - ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    return 1;
  }

  return 0;
}

Int_t ChamberMonitor::ClearBeamAccumulators()
{

  // Clear beam related accumulators

  fNEventsBeam = 0;

  fP1_BeamX = 0.;
  fP1_BeamY = 0.;
  fP2_BeamX = 0.;
  fP2_BeamY = 0.;
  fP1_BeamX_N = 0;
  fP1_BeamY_N = 0;
  fP2_BeamX_N = 0;
  fP2_BeamY_N = 0;

  fP1_BeamXSpread = 0.;
  fP1_BeamYSpread = 0.;
  fP2_BeamXSpread = 0.;
  fP2_BeamYSpread = 0.;
  fP1_BeamXSpread_N = 0;
  fP1_BeamYSpread_N = 0;
  fP2_BeamXSpread_N = 0;
  fP2_BeamYSpread_N = 0;

  fP1_BeamXTCharge = 0.;
  fP1_BeamXBCharge = 0.;
  fP1_BeamYLCharge = 0.;
  fP1_BeamYRCharge = 0.;
  fP2_BeamXTCharge = 0.;
  fP2_BeamXBCharge = 0.;
  fP2_BeamYLCharge = 0.;
  fP2_BeamYRCharge = 0.;
  fP1_BeamXTCharge_N = 0;
  fP1_BeamXBCharge_N = 0;
  fP1_BeamYLCharge_N = 0;
  fP1_BeamYRCharge_N = 0;
  fP2_BeamXTCharge_N = 0;
  fP2_BeamXBCharge_N = 0;
  fP2_BeamYLCharge_N = 0;
  fP2_BeamYRCharge_N = 0;

  return 0;
}
