// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//
#ifndef TargetRecoHit_H
#define TargetRecoHit_H

class TargetRecoHit
{
public:

  TargetRecoHit();
  ~TargetRecoHit();
  TargetRecoHit(const TargetRecoHit& T);
  TargetRecoHit(int chid, double t, double et, double charge, double echarge);


  int         getChannelId();
  double      getTime();
  double      getTimeError();
  double      getCharge();
  double      getChargeError();

  void setChannelId  (int chid);
  void setTime       (double time);
  void setTimeError       (double timeerr);
  void setCharge     (double charge);
  void setChargeError(double chargeError);

private:
    int         fChannelId;
    double      fTime;
    double      fTimeError;
    double      fCharge;
    double      fChargeError;

};
#endif
