// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//

#include "TargetRecoHit.hh"


TargetRecoHit::TargetRecoHit()
{
  fChannelId=-1;
  fTime     =-1.;
  fTimeError     =-1.;
  fCharge   =-1.;
  fChargeError=-1.;
}

TargetRecoHit::~TargetRecoHit()
{
}

TargetRecoHit::TargetRecoHit(const TargetRecoHit& T)
{
  fChannelId=T.fChannelId;
  fTime     =T.fTime;
  fTimeError     =T.fTimeError;
  fCharge   =T.fCharge;
  fChargeError=T.fChargeError;
}
TargetRecoHit::TargetRecoHit(int chid, double t, double et, double charge, double echarge)
{
  fChannelId=chid;
  fTime     =t;
  fTimeError     =et;
  fCharge   =charge;
  fChargeError=echarge;
}

int    TargetRecoHit::getChannelId(){return fChannelId;}
double TargetRecoHit::getTime(){return fTime;}
double TargetRecoHit::getTimeError(){return fTimeError;}
double TargetRecoHit::getCharge(){return fCharge;}
double TargetRecoHit::getChargeError(){return fChargeError;}
void TargetRecoHit::setChannelId  (int chid)           {fChannelId   = chid;}
void TargetRecoHit::setTime       (double hitTime)     {fTime        = hitTime;}
void TargetRecoHit::setTimeError  (double hitTimeErr)  {fTimeError   = hitTimeErr;}
void TargetRecoHit::setCharge     (double charge)      {fCharge      = charge;}
void TargetRecoHit::setChargeError(double chargeError) {fChargeError = chargeError;}
