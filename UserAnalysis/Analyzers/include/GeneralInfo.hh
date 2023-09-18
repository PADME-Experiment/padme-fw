//
// Event-level information
//
#ifndef GeneralInfo_h
#define GeneralInfo_h 1

#include "PadmeAnalysisEvent.hh"
#include "OfflineServer.hh"
#include "TVector3.h"

using namespace std;

class TRecoEvent;

class GeneralInfo : public TObject
{

protected:
  GeneralInfo();

public:
  ~GeneralInfo();
  static GeneralInfo* GetInstance();

private:
  static GeneralInfo* fInstance;

public:
  virtual Bool_t Init(PadmeAnalysisEvent* event);
  virtual Bool_t Process();

  long long int GetPeriodStartTime(){return fPeriodStart;} // tim of start of the period (2020, 2022, etc.)
  double GetBeamMomentum(){return fBeamMomentum;}
  double GetBunchLength(){return fBunchLength;}
  double GetBeamStart(){return fBeamStart;}
  inline TVector3 GetTargetPos() const {return fRTarg;}
  inline TVector3 GetCOG() const {return fCOGAtECal;}
  inline double GetCalibEnergyFactor(){return fCalibEnergyFactor;}
  inline double GetCalibTimeEnergyFactor(){return fCalibTimeEnergyFactor;}

  // methods giving availability of original information

  bool IsEnergyAvailable(){return fIsEnergyAvailable;}
  bool IsTargetAvgAvailable(){return fIsTargetAvgAvailable;} 
  bool IsCOGAvailable(){return fIsCOGAvailable;} 
  bool IsBunchLengthAvailable(){return fIsBunchLengthAvailable;} 
  bool IsCalibEnergyAvailable(){return fIsCalibEnergyAvailable;} 
  bool IsCalibTimeEnergyAvailable(){return fIsCalibTimeEnergyAvailable;} 


  // derived beam kinematics
  inline double GetSqrts() const {return fSqrts;}
  inline double GetGam() const {return fGam;}
  inline double GetBG() const {return fBG;}
  inline double GetBeta() const {return fBeta;}
  inline TVector3 GetBoost() const {return fBoostMom;}
  inline double GetRadiusMax() const { return fRadiusMax;}
  inline double GetRadiusMin() const { return fRadiusMin;}
  inline double GetEnergyMin() const { return fEnergyMin;}
  inline double GetEnergyMax() const { return fEnergyMax;}

  // global quantities
  inline double GetGlobalESlope() const { return fGlobalESlope;}
  inline double GetGlobalTimeESlope() const { return fGlobalTimeESlope;} //default time-energy slope
  inline double GetGlobalBunchTimeStart() const { return fGlobalBunchTimeStart;} // default bunch time start
  inline double GetGlobalBunchTimeLength() const { return fGlobalBunchTimeLength;} // default bunch time length
protected:
  TRecoEvent* fRecoEvent;

private:
  double fMe;
  OfflineServer* fOfflineServerDB;
  void EvalBeamProperties();
  void PrintBeamProperties(int);

  // event-by-event quantities

  Int_t fRunOld;
  long long int fPeriodStart; // time of start of the period (2020, 2022, etc.)
  double fBeamMomentum;
  double fBunchLength;
  double fBeamStart; 
  
  double fXTarg;
  double fYTarg;

  double fXCOG;
  double fYCOG;

  double fCalibEnergyFactor;
  double fCalibTimeEnergyFactor;

  // availability of original information

  bool fIsEnergyAvailable;
  bool fIsTargetAvgAvailable;
  bool fIsCOGAvailable;
  bool fIsBunchLengthAvailable;
  bool fIsCalibEnergyAvailable;
  bool fIsCalibTimeEnergyAvailable; 

  // global quantities

  double fZTarg;
  double fZECal;

  TVector3 fRTarg;
  TVector3 fCOGAtECal;
  double fGlobalESlope;
  double fGlobalTimeESlope;
  double fGlobalBunchTimeStart;
  double fGlobalBunchTimeLength;

  // derived quantities

  double fSqrts;
  double fBG;
  double fGam;
  double fBeta;

  TVector3 fBoostMom;
  double fRadiusMax;
  double fRadiusMin;
  double fEnergyMin;
  double fEnergyMax;

};
#endif
