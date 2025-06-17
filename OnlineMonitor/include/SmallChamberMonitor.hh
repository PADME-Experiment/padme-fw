#ifndef SmallChamberMonitor_H
#define SmallChamberMonitor_H

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TString.h"

#include "utlConfigParser.hh"

#define MMCH_N_LAYERS 2

class Configuration;
class ChamberEvent;

class SmallChamberMonitor
{

public:

  SmallChamberMonitor(TString);
  ~SmallChamberMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void AnalyzeEvent(ChamberEvent*);

private:

  //void CoordinateFinder(int, int, std::vector<double>, double &, double &, double &, double &);
  void CoordinateFinder(Int_t, Int_t, std::vector<Double_t>, Double_t &, Double_t &);
  void ComputeBeamSpot();
  Int_t OutputBeam();
  Int_t ClearBeamAccumulators();

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

  // Trend vectors
  std::vector<Double_t> fVTime_Beam;

  std::vector<Double_t> fVP1_BeamX;
  std::vector<Double_t> fVP1_BeamY;

  std::vector<Double_t> fVP1_BeamXSpread;
  std::vector<Double_t> fVP1_BeamYSpread;

  std::vector<Double_t> fVP1_BeamXCharge;
  std::vector<Double_t> fVP1_BeamYCharge;

  // Trend support file
  TString fTFChTrendsBM;

  // Range for NPoTs and Bunch Length trend plots
  //Double_t fNPoTsRangeMin;
  //Double_t fNPoTsRangeMax;

  // Time fit objects
  std::vector<Double_t> x_mean[MMCH_N_LAYERS]; // X coordinate
  std::vector<Double_t> q_mean[MMCH_N_LAYERS]; // Charge
  std::vector<Int_t>    c_mean[MMCH_N_LAYERS]; // Channel

  TString fRunString; // String to hold the run name to show on histograms
  TString mmch_tag[MMCH_N_LAYERS] = { "TMMX","TMMY" };

  // Histograms
  TH1D* h_occupancy[MMCH_N_LAYERS];
  TH1D* hw_occupancy[MMCH_N_LAYERS];
  TH1D* hqmax_totevent[MMCH_N_LAYERS];
  //TH2D* hqmax_strip[MMCH_N_LAYERS];
  //TH2D* htmax_strip[MMCH_N_LAYERS];
  TH1D* hqmax_perevent[MMCH_N_LAYERS];

  // Signal conversion parameters
  Double_t fStripPitch=0.25; //mm - Pitch between two consecutive strips
                                                                                
  UInt_t fNStrips=358;

  //float xmax = fNStrips*fStripPitch;

  Double_t d_firstcross_lastcross_strips = 89.262; //mm
  Double_t GLOBAL_X_TRANSLATION = -d_firstcross_lastcross_strips/2; //mm
  Double_t GLOBAL_Y_TRANSLATION = -d_firstcross_lastcross_strips/2; //mm

  //Double_t z0=0, zm=50., z2=100.; //mm          z coord of the chamber starting from bottom to top 

  //Double_t vd=0.105; //mm/ns in place of 0.105      
  Double_t vd=0.1002; //mm/ns for run 2292 and 2271
  //Double_t vd=0.1210; //mm/ns for run 2417 
  
  //POSITION REFERENCE POINT 1  
  Double_t xP1_cad = 101.074, yP1_cad = 129.949; //mm    CAD MEASUREMENTs
  Double_t xP1_met = 101.69 , yP1_met = 130.26;  //mm    METROLOGY MEASUREMENTs (Report PADME 23-05-2025)
  //POSITION REFERENCE POINT 2
  Double_t xP2_cad = 127.174, yP2_cad = -98.791; //mm    CAD MEASUREMENTs
  Double_t xP2_met = 131.48 , yP2_med = -97.78;  //mm    METROLOGY MEASUREMENTs (Report PADME 23-05-2025)

  Double_t clock = 25.;
  Int_t ncamp=27.; //with the 675ns time window (for the 450 one ncamp is 18)

  Double_t APV1_fqx = 0.6355*0.8710;
  Double_t APV2_fqx = 1;
  Double_t APV3_fqx = 0.3194*0.8261;

  Double_t APV1_fqy = 0.7271*0.9437;
  Double_t APV2_fqy = 1;
  Double_t APV3_fqy = 0.9104*0.9574;

  Double_t beamspot_x[MMCH_N_LAYERS];
  Int_t beamspot_x_n[MMCH_N_LAYERS];
  Double_t beamspot_rms[MMCH_N_LAYERS];
  Int_t beamspot_rms_n[MMCH_N_LAYERS];
  Double_t beamspot_q[MMCH_N_LAYERS];
  Int_t beamspot_q_n[MMCH_N_LAYERS];

};
#endif
