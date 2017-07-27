// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//

#include "TargetRecoEvent.hh"


TargetRecoEvent::TargetRecoEvent()
{
     fX=0.;
     fY=0.;
     fXError=0.;
     fYError=0.;
     fChargeX=0.;
     fChargeY=0.;
     fChargeXErr=0.;
     fChargeYErr=0.;
     fTime=0.;
     fTimeError=0.;
}

TargetRecoEvent::~TargetRecoEvent()
{
}

TargetRecoEvent::TargetRecoEvent(const TargetRecoEvent& T)
{
     fX=T.fX;
     fY=T.fY;
     fXError=T.fXError;
     fYError=T.fYError;
     fChargeX=T.fChargeX;
     fChargeY=T.fChargeY;
     fChargeXErr=T.fChargeXErr;
     fChargeYErr=T.fChargeYErr;
     fTime=T.fTime;
     fTimeError=T.fTimeError;
}
TargetRecoEvent::TargetRecoEvent(double x, double y,double xerr, double yerr,
                                 double chargex, double chargey,
                                 double chargexE, double chargeyE, 
				 double eventTime, double eventTimeErr)
{
     fX=x;
     fY=y;
     fXError=xerr;
     fYError=yerr;
     fChargeX=chargex;
     fChargeY=chargey;
     fChargeXErr=chargexE;
     fChargeYErr=chargeyE;
     fTime=eventTime;
     fTimeError=eventTimeErr;
}

double TargetRecoEvent::getX(){return fX;}
double TargetRecoEvent::getY(){return fY;}
double TargetRecoEvent::getXError(){return fXError;}
double TargetRecoEvent::getYError(){return fYError;}
double TargetRecoEvent::getChargeX(){return fChargeX;}
double TargetRecoEvent::getChargeY(){return fChargeY;}
double TargetRecoEvent::getChargeXErr(){return fChargeXErr;}
double TargetRecoEvent::getChargeYErr(){return fChargeYErr;}
double TargetRecoEvent::getTime(){return fTime;}
double TargetRecoEvent::getTimeErr(){return fTimeError;}

void TargetRecoEvent::setX         (double x)           {fX = x;}
void TargetRecoEvent::setY         (double y)           {fY = y;}
void TargetRecoEvent::setXError    (double Xerr)        {fXError = Xerr;}
void TargetRecoEvent::setYError    (double Yerr)        {fYError = Yerr;}
void TargetRecoEvent::setChargeX   (double ChargeX)     {fChargeX = ChargeX;}
void TargetRecoEvent::setChargeY   (double ChargeY)     {fChargeY = ChargeY;}
void TargetRecoEvent::setChargeXErr(double ChargeXErr)  {fChargeXErr = ChargeXErr;}
void TargetRecoEvent::setChargeYErr(double ChargeYErr)  {fChargeYErr = ChargeYErr;}
void TargetRecoEvent::setTime(double eventTime)  {fTime = eventTime;}
void TargetRecoEvent::setTimeErr(double eventTimeErr)  {fTimeError = eventTimeErr;}
