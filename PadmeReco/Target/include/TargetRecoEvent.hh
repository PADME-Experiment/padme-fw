// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//
#ifndef TargetRecoEvent_H
#define TargetRecoEvent_H

class TargetRecoEvent
{
public:

  TargetRecoEvent();
  ~TargetRecoEvent();
  TargetRecoEvent(const TargetRecoEvent& T);
  TargetRecoEvent(double x, double y,double xerr, double yerr, double chargex, double chargey, double chargexE, double chargeyE, double eventTime, double eventTimeErr);


  double      getX();
  double      getY();
  double      getXError();
  double      getYError();
  double      getChargeX();
  double      getChargeY();
  double      getChargeXErr();
  double      getChargeYErr();
  double      getTime();
  double      getTimeErr();

  void setX           (double x);
  void setY           (double y);
  void setXError      (double xerr);
  void setYError      (double yerr);
  void setChargeX     (double chargex);
  void setChargeY     (double chargey);
  void setChargeXErr  (double chargexE);
  void setChargeYErr  (double chargeyE);
  void setTime        (double eventTime);
  void setTimeErr        (double eventTimeErr);

private:
    double      fX;
    double      fY;
    double      fXError;
    double      fYError;
    double      fChargeX;
    double      fChargeY;
    double      fChargeXErr;
    double      fChargeYErr;
    double      fTime;
    double      fTimeError;

};
#endif
