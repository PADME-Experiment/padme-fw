#ifndef ChamberMonitor_H
#define ChamberMonitor_H

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TString.h"

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
  Int_t OutputBeam();

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

  // Histograms
  //TH1D* fHLGPedestalBM;

  // Trend vectors
  //std::vector<Double_t> fVLGTimeBM;

  // Trend support file
  TString fTFChTrendsBM;

  // Range for NPoTs and Bunch Length trend plots
  //Double_t fNPoTsRangeMin;
  //Double_t fNPoTsRangeMax;

  // Time fit objects
  //std::vector<double> t_mean0, t_mean1, t_mean2, t_mean3, t_mean4, t_mean5, t_mean6, t_mean7;
  //std::vector<double> z_mean0, z_mean1, z_mean2, z_mean3, z_mean4, z_mean5, z_mean6, z_mean7;
  //std::vector<double> x_mean0, x_mean1, x_mean2, x_mean3, x_mean4, x_mean5, x_mean6, x_mean7;
  //std::vector<double> q_mean0, q_mean1, q_mean2, q_mean3, q_mean4, q_mean5, q_mean6, q_mean7;
  std::vector<double> t_mean[MMCH_N_LAYERS];
  std::vector<double> z_mean[MMCH_N_LAYERS];
  std::vector<double> x_mean[MMCH_N_LAYERS];
  std::vector<double> q_mean[MMCH_N_LAYERS];

  TString mmch_tag[MMCH_N_LAYERS] = { "P1YR","P1YL","P1XT","P1XB","P2YR","P2YL","P2XT","P2XB" };

  // Histograms
  TH1D* h_occupancy[MMCH_N_LAYERS];
  TH1D* hw_occupancy[MMCH_N_LAYERS];
  TH1D* hqmax_totevent[MMCH_N_LAYERS];
  TH1D* htmax_totevent[MMCH_N_LAYERS];
  //TH2D* hqmax_strip[MMCH_N_LAYERS];
  //TH2D* htmax_strip[MMCH_N_LAYERS];
  TH1D* hqmax_perevent[MMCH_N_LAYERS];
  TH1D* htmax_perevent[MMCH_N_LAYERS];

  // Signal conversion parameters
  float pitch=1.2; //mm

  //float geo_hole[8] = {2.4,2.4,8.4,8.4,8.4,8.4,2.4,2.4};//mm
  float geo_hole[8] = {8.4,8.4,2.4,2.4,2.4,2.4,8.4,8.4};//mm
                                                                                
  int maxStrip=512, maxStripApv=128;                                                                                      
  float xmax = maxStrip*pitch;

  float z0=0, zm=50., z2=100.; //mm          z coord of the chamber starting from bottom to top 

  //float vd=0.105; //mm/ns in place of 0.105      
  float vd=0.1002; //mm/ns for run 2292 and 2271
  //float vd=0.1210; //mm/ns for run 2417 
  
  float clock = 25., ncamp=27; //with the 675ns time window (for the 450 one ncamp is 18)                               
  double qnoisecut = 10000;

  int maxholes=2;
  double xnstrip=3;
  
  double sigma_x = 0.5; //mm (per hit adjoint)

};
#endif
