#ifndef DarkPhoton_h
#define DarkPhoton_h

#include "PadmeVAnalyser.hh"
#include "TLorentzVector.h"
#include "TVector3.h"
#include <fstream>
#include <string>
class DarkPhoton: public  PadmeVAnalyser {

 private:
struct ECalClusterInfo {
    Float_t energy;
    Float_t time;
    Float_t x;
    Float_t y;
    Float_t z;
    Float_t radius;
    Float_t theta;
    Float_t phi;
    Float_t chX;
    Float_t chY;
    Float_t missingMass;
    Float_t missingMomentum;
    TVector3 missingMomentumV;
    Float_t missingTheta;
  };


public:
  DarkPhoton(){;};
  DarkPhoton(const char *s,int valid, int verb);
  DarkPhoton(const char *s);
  ~DarkPhoton(){;};

  virtual Bool_t InitHistos();

  virtual void BookTargetHistos();
  virtual void BookPVetoHistos();
  virtual void BookPVetoEVetoCheckHistos();
  virtual void BookEVetoHistos();
  virtual void BookHEPVetoHistos();
  virtual void BookSACHistos();
  virtual void BookSACPVetoBremsHistos();
  virtual void BookSACHEPVetoBremsHistos();
  virtual void BookSACEVetoCheckHistos();
  virtual void BookECalPVetoBremsHistos();
  virtual void BookECalHEPVetoBremsHistos();
  virtual void BookECalEVetoCheckHistos();
  virtual void BookECalSACCheckHistos();
  virtual void BookDarkPhotonHistos();
  virtual void BookDarkPhotonDebugHistos();
  virtual void BookUBosonDecayHistos();
  virtual void BookGGHistos();


  virtual double ComputeRadius(Double_t PosX1,Double_t PosY1);
  virtual double ComputeInvariantMass(Double_t E1, Double_t E2, Double_t PosX1, Double_t PosX2, Double_t PosY1, Double_t PosY2);
  virtual double ComputeMissingMass(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeMissingMomentum(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual TVector3 ComputeMissingMomentumVector(double BeamEnergy, Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeTheta(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputePhi(Double_t PosX, Double_t PosY, Double_t PosZ, Double_t E_gamma);
  virtual double ComputeCOG(Double_t E_gamma1,Double_t Pos1,Double_t E_gamma2,Double_t Pos2);
  virtual TLorentzVector ComputeCMMomentum(Double_t xt, Double_t yt, Double_t zt,Double_t PosX, Double_t PosY, Double_t PosZ);
  virtual int ComputeSector(Double_t x, Double_t y);
  virtual double CalibrateEnergy(Double_t E_gamma, Double_t x, Double_t y);
  virtual double ComputeExpectedEnergy(Double_t PosX, Double_t PosY);
  virtual double ComputeMomentumPositron(Double_t z,Double_t x);
  
  
  virtual void ProcessTarget();
  virtual void ProcessPVeto();
  virtual void PVetoEVetoCheck();
  virtual void ProcessEVeto();
  virtual void ProcessHEPVeto();
  virtual void ProcessSAC();
  virtual void SACPVetoBrems();
  virtual void SACHEPVetoBrems();
  virtual void SACEVetoCheck();
  virtual void ProcessECal();
  virtual void ECalPVetoBrems();
  virtual void ECalHEPVetoBrems();
  virtual void ECalEVetoCheck();
  virtual void ECalSACCheck();
  virtual void FillECalBasicHistos(const ECalClusterInfo &clu);
  virtual bool IsGG(const ECalClusterInfo& clu);
  virtual void ProcessGG();
  virtual void DarkPhotonHunt();
  virtual void UBosonDecayHunt();
  virtual bool Preselection();
  virtual Bool_t Process();  

  virtual Bool_t IsInGeometricCut(const ECalClusterInfo &clu);
  virtual bool IsECalIsolated(const ECalClusterInfo &clu, Float_t isolationWindow);
  virtual bool IsPVetoIsolated(const ECalClusterInfo &clu, Float_t isolationWindow);
  virtual bool IsPVetoBremsFiltered(const ECalClusterInfo &clu, Float_t isolationWindow);
  virtual bool IsHEPVetoIsolated(const ECalClusterInfo &clu, Float_t isolationWindow);
  virtual bool IsHEPVetoBremsFiltered(const ECalClusterInfo &clu, Float_t isolationWindow);
  virtual bool IsSACPVetoBremsFiltered(TRecoVCluster* clu, Float_t isolationWindow);

  virtual void FillMissingMassHistos(const ECalClusterInfo &clu, const std::string &suffix);
 virtual void FillMissingMassDebugHistos(const ECalClusterInfo &clu, const std::string &suffix);

  virtual Bool_t Finalize();

protected:

  Bool_t fProcessTarget;
  Bool_t fProcessPVeto;
  Bool_t fPVetoEVetoCheck;
  Bool_t fProcessEVeto;
  Bool_t fProcessHEPVeto;
  Bool_t fProcessSAC;
  Bool_t fSACPVetoBrems;
  Bool_t fSACHEPVetoBrems;
  Bool_t fSACEVetoCheck;
  Bool_t fECalPVetoBrems;
  Bool_t fECalHEPVetoBrems;
  Bool_t fECalEVetoCheck;
  Bool_t fECalSACCheck;
  Bool_t fDarkPhotonHunt;
  Bool_t fUBosonDecayHunt;
  Bool_t fProcessGG;

  
  Double_t PVetoIsolationWindow;
  Double_t HEPVetoIsolationWindow;

  Double_t SACPVetoIsolationWindow;

  Double_t SACPVetoTimeWindow;
  Double_t SACHEPVetoTimeWindow;
  Double_t SACEVetoTimeWindow;

  Double_t ECalPVetoInTimeWindow;
  Double_t ECalHEPVetoInTimeWindow;
  Double_t ECalEVetoInTimeWindow;

  Double_t MaxTimeDiffGG;

  Double_t ECalSACInTimeWindow;

  Double_t fECalIsolationWindow;
  Double_t fECalMinEnergy;
  Double_t ShiftECalSac;
  Double_t ShiftECalPVeto;
  Double_t shiftMomentumsacPVeto;

Double_t DebugMin;
Double_t DebugMax;
  
private:
  Int_t nGG_scan[41*41*41]={0};
  std::string filecoord;
  std::ofstream fcoordout;


  Int_t EcalShiftTime=100;
  Int_t minTimeBunch=-140;
  Int_t maxTimebunch=150;


  static constexpr Double_t Z_1_U = 3536.0;
  static constexpr Float_t BeamEnergy = 431.6;
  static constexpr Double_t fMe = 0.511;
  static constexpr Double_t fBeamMomentum = 431.2;
  static constexpr Double_t MinEnergy = 30.0;

  static constexpr Float_t eSumMClow = 399.7;
  static constexpr Float_t eSumMChigh = 454.9;
  static constexpr Float_t eSumDatalow = 382.5;
  static constexpr Float_t eSumDatahigh = 463.3;

  static constexpr Float_t sumThetaMClow = 0.124;
  static constexpr Float_t sumThetaMChigh = 0.157;
  static constexpr Float_t sumThetaDatalow = 3.07;
  static constexpr Float_t sumThetaDatahigh = 3.21;

  static constexpr Float_t deltaPhiMClow = 2.88;
  static constexpr Float_t deltaPhiMChigh = 3.38;
  static constexpr Float_t deltaPhiDatalow = 2.90;
  static constexpr Float_t deltaPhiDatahigh = 3.35;


  
  // Configuration parameters for veto cuts
  static constexpr Float_t alineECalCh = -0.00035;
  static constexpr Float_t blineECalCh = -0.01;
  static constexpr Float_t cgoreECalCh = 77.64;
  static constexpr Float_t cdoluECalCh = 64.07;
  static constexpr Float_t alineECalHEPCh = -0.00129;
  static constexpr Float_t blineECalHEPCh = -0.07;
  static constexpr Float_t cgoreECalHEPCh = 32.65;
  static constexpr Float_t cdoluECalHEPCh = 15.66;

  static constexpr Float_t alineSACCh = -0.00025;
  static constexpr Float_t blineSACCh = -0.07971;
  static constexpr Float_t cgoreSACCh = 105.0;
  static constexpr Float_t cdoluSACCh = 75.0;
  
  // Energy calibration coefficients
  static constexpr double calibrationCoefficients[19] = {
    1.00651, 1.02013, 1.0266, 1.01829, 1.01393, 1.02115, 
    1.02356, 1.00694, 1.01086, 1.00473, 1.00284, 1.0056, 
    1.00919, 0.998449, 0.9914, 1.00386, 0.991517, 0.985795, 0.990386
  };

  static constexpr double PVetoTimeCalib[90] = {
    39.815943, 39.741668, 40.0056915, 39.998239, 39.910826, 42.103284, 42.183914, 42.283804, 42.247380, 42.127884, 42.520957, 42.611348, 42.370419, 42.379331, 42.541509, 42.604503, 42.342282, 42.319263, 42.633682, 42.428726, 42.394504, 42.186876, 42.313951, 42.537322, 42.512878, 42.332085, 42.493889, 42.689779, 42.556729, 42.516648, 42.505646, 42.544033, 42.401379, 42.379903, 42.680627, 42.500516, 42.000818, 42.399425, 42.486720, 42.498553, 42.324297, 42.230548, 42.477100, 42.604390, 42.379047, 42.365893, 42.884562, 43.121920, 39.763999, 39.855322, 40.094321, 40.017549, 39.808740, 39.883138, 39.877237, 40.067838, 39.779709, 39.826122, 40.053293, 39.927473, 39.861128, 39.911666, 40.064611, 40.102640, 39.594062, 39.705577, 39.981060, 39.859490, 39.742498, 39.868403, 39.845057, 39.972684, 39.992295, 39.738745, 39.788163, 39.853616, 39.625895, 39.626586, 39.686472, 39.560914, 39.385894, 39.285565, 39.814462, 39.327550, 39.386270, 39.103682, 39.068841, 39.075710, 38.971075, 37.942386
  };

    

  static constexpr Float_t RADIUS_MIN = 90.0;
  static constexpr Float_t RADIUS_MAX = 258.0;

  static constexpr Float_t ftargetx = -14.3;
  static constexpr Float_t ftargety = 0.35;
  static constexpr Float_t ftargetz = -1028.0;

  static constexpr Float_t PI = 3.14159265358979323846;

  struct ECalGGClusterPair {
    Float_t x1;
    Float_t y1;
    Float_t z1;
    Float_t energy1;
    Float_t  time1;

    Float_t x2;
    Float_t y2;
    Float_t z2;
    Float_t energy2;
    Float_t time2;

    Float_t COGX;
    Float_t COGY;

    Float_t energySum;
    Float_t CMtheta1;
    Float_t CMtheta2;
    Float_t radius1;
    Float_t radius2;
    Float_t phi1;
    Float_t phi2;
    Float_t invMass;
  };
  
  std::vector<ECalClusterInfo> ecalAllClusters;

  std::vector<ECalGGClusterPair> ecalGGClusters;
  
};


#endif
