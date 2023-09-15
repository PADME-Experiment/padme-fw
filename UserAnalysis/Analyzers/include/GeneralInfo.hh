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

  long long int GetStartTime(){return fStartTime;}
  double GetBeamMomentum(){return fBeamMomentum;}
  inline TVector3 GetTargetPos() const {return fRTarg;}
  inline TVector3 GetCOG() const {return fCOGAtECal;}
  inline double GetSqrts() const {return fSqrts;}
  inline double GetGam() const {return fGam;}
  inline double GetBG() const {return fBG;}
  inline double GetBeta() const {return fBeta;}
  inline TVector3 GetBoost() const {return fBoostMom;}
  inline double GetRadiusMax() const { return fRadiusMax;}
  inline double GetRadiusMin() const { return fRadiusMin;}
  inline double GetEnergyMin() const { return fEnergyMin;}
  inline double GetEnergyMax() const { return fEnergyMax;}
  inline double GetGlobalESlope() const { return fGlobalESlope;}
  inline double GetGlobalTimeESlope() const { return fGlobalTimeESlope;}
protected:
  TRecoEvent* fRecoEvent;

private:
  double fMe;
  OfflineServer* fOfflineServerDB;
  void EvalBeamProperties();
  void PrintBeamProperties(int);

  Int_t fRunOld;
  double fBeamMomentum;
  long long int fStartTime;
  
  double fXTarg;
  double fYTarg;
  double fZTarg;

  double fXCOG;
  double fYCOG;
  double fZECal;

  TVector3 fRTarg;
  TVector3 fCOGAtECal;

  double fSqrts;
  double fBG;
  double fGam;
  double fBeta;

  TVector3 fBoostMom;
  double fRadiusMax;
  double fRadiusMin;
  double fEnergyMin;
  double fEnergyMax;
  double fGlobalESlope;
  double fGlobalTimeESlope;
};
#endif
