#ifndef ChamberMonitor_H
#define ChamberMonitor_H

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TString.h"
#include "TF1.h"

#include "utlConfigParser.hh"

#define MMCH_N_LAYERS 8

class Configuration;
class ChamberEvent;

class ChamberMonitor
{

public:

  ChamberMonitor(TString);
  ~ChamberMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void AnalyzeEvent(ChamberEvent*);

private:

  void CoordinateFinder(int, int, std::vector<double>, double &, double &, double &, double &);
  Int_t ComputeBeamSpot();
  Int_t ComputeBeamSpot_v2();
  Int_t OutputBeam();
  Int_t ClearBeamAccumulators();
  Int_t Compute2DOccupancy();

  Configuration* fConfig;
  utl::ConfigParser* fConfigParser;

  Bool_t fIsBeam;
  Bool_t fIsOffBeam;
  Bool_t fIsCosmics;
  Bool_t fIsRandom;

  UInt_t fBeamOutputRate;
  UInt_t fOffBeamOutputRate;
  UInt_t fCosmicsOutputRate;
  UInt_t fRandomOutputRate;

  UInt_t fBeamEventCount;
  UInt_t fOffBeamEventCount;
  UInt_t fCosmicsEventCount;
  UInt_t fRandomEventCount;

  // Beamspot position and spread
  UInt_t   fNEventsBeam;

  Double_t fP1_BeamX;
  UInt_t fP1_BeamX_N;
  Double_t fP1_BeamXSpread;
  UInt_t fP1_BeamXSpread_N;
  Double_t fP1_BeamXTCharge;
  UInt_t fP1_BeamXTCharge_N;
  Double_t fP1_BeamXBCharge;
  UInt_t fP1_BeamXBCharge_N;

  Double_t fP1_BeamY;
  UInt_t fP1_BeamY_N;
  Double_t fP1_BeamYSpread;
  UInt_t fP1_BeamYSpread_N;
  Double_t fP1_BeamYLCharge;
  UInt_t fP1_BeamYLCharge_N;
  Double_t fP1_BeamYRCharge;
  UInt_t fP1_BeamYRCharge_N;

  Double_t fP2_BeamX;
  UInt_t fP2_BeamX_N;
  Double_t fP2_BeamXSpread;
  UInt_t fP2_BeamXSpread_N;
  Double_t fP2_BeamXTCharge;
  UInt_t fP2_BeamXTCharge_N;
  Double_t fP2_BeamXBCharge;
  UInt_t fP2_BeamXBCharge_N;

  Double_t fP2_BeamY;
  UInt_t fP2_BeamY_N;
  Double_t fP2_BeamYSpread;
  UInt_t fP2_BeamYSpread_N;
  Double_t fP2_BeamYLCharge;
  UInt_t fP2_BeamYLCharge_N;
  Double_t fP2_BeamYRCharge;
  UInt_t fP2_BeamYRCharge_N;


  // Trend vectors
  std::vector<Double_t> fVTime_Beam;

  std::vector<Double_t> fVP1_BeamX;
  std::vector<Double_t> fVP1_BeamY;
  std::vector<Double_t> fVP2_BeamX;
  std::vector<Double_t> fVP2_BeamY;

  std::vector<Double_t> fVP1_BeamXSpread;
  std::vector<Double_t> fVP1_BeamYSpread;
  std::vector<Double_t> fVP2_BeamXSpread;
  std::vector<Double_t> fVP2_BeamYSpread;

  std::vector<Double_t> fVP1_BeamXTCharge;
  std::vector<Double_t> fVP1_BeamXBCharge;
  std::vector<Double_t> fVP1_BeamYLCharge;
  std::vector<Double_t> fVP1_BeamYRCharge;
  std::vector<Double_t> fVP2_BeamXTCharge;
  std::vector<Double_t> fVP2_BeamXBCharge;
  std::vector<Double_t> fVP2_BeamYLCharge;
  std::vector<Double_t> fVP2_BeamYRCharge;

  // Trend support file
  TString fTFChTrendsBM;

  // Time fit objects
  std::vector<Double_t> t_mean[MMCH_N_LAYERS]; // Time
  std::vector<Double_t> z_mean[MMCH_N_LAYERS]; // Z coordinate
  std::vector<Double_t> x_mean[MMCH_N_LAYERS]; // X coordinate
  std::vector<Double_t> q_mean[MMCH_N_LAYERS]; // Charge
  std::vector<Int_t>    c_mean[MMCH_N_LAYERS]; // Channel

  TString fRunString; // String to hold the run name to show on histograms
  TString mmch_tag[MMCH_N_LAYERS] = { "P1YR","P1YL","P1XT","P1XB","P2YR","P2YL","P2XT","P2XB" };

  
   // Histograms
  TH1D* h_occupancy[MMCH_N_LAYERS];
  TH1D* hw_occupancy[MMCH_N_LAYERS];
  TH1D* hqmax_totevent[MMCH_N_LAYERS];
  TH1D* htmax_totevent[MMCH_N_LAYERS];
  TH1D* hqmax_perevent[MMCH_N_LAYERS];
  TH1D* htmax_perevent[MMCH_N_LAYERS];
  TH1D* hxq_beam[MMCH_N_LAYERS];
  
  TH2D* hxyq_P1;
  TH2D* hxyq_P2;

  // Signal conversion parameters
  Double_t fStripPitch=1.2; //mm - Pitch between two consecutive strips
  
  Double_t fGeometryHole[MMCH_N_LAYERS] = {7.2,7.2,1.2,1.2,1.2,1.2,7.2,7.2};//mm
  Double_t fGeometryHoleNStrips[MMCH_N_LAYERS] = {6,6,1,1,1,1,6,6};//mm
  
  UInt_t fNStrips=512;
  Int_t fNStripsHalf=256;
  UInt_t fNStripsApv=128;
  
  Double_t fxmax = (fNStrips-2)*fStripPitch;
  Double_t fGlobalShift[MMCH_N_LAYERS];
  
  Int_t n_bins6 = 86; //binning 6 strips at once
  Int_t n_bins_beam = 100; //beam region binning
  
  Double_t mean_t_beam0 = 349; //ns
  Double_t sigma_t_beam0 = 18; //ns
  Double_t mean_t_beam3 = 328; //ns
  Double_t sigma_t_beam3 = 36; //ns
  Double_t mean_t_beam4 = 375; //ns default
  Double_t sigma_t_beam4 = 25; //ns default
  Double_t mean_t_beam7 = 400; //ns 
  Double_t sigma_t_beam7 = 25; //ns

  Double_t z0=0, zm=50., z2=100.; //mm          z coord of the chamber starting from bottom to top 

  //Double_t vd=0.105; //mm/ns in place of 0.105      
  Double_t vd=0.1002; //mm/ns for run 2292 and 2271
  //Double_t vd=0.1210; //mm/ns for run 2417 
  
  Double_t clock = 25.;
  Int_t ncamp=27.; //with the 675ns time window (for the 450 one ncamp is 18)

  // Position of central "Low HV" square to use for beam studies (mm)
  Double_t fLowHV_MeshRadius = 60.; // Radius of the low HV mesh area
  Double_t fLowHV_MeshGap[MMCH_N_LAYERS] = {7.2,7.2,0.,0.,0.,0.,7.2,7.2};
  Double_t fLowHV_XMin[MMCH_N_LAYERS];
  Double_t fLowHV_XMax[MMCH_N_LAYERS];

};
#endif
