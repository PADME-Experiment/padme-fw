// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//
#ifndef TTargetRecoBeam_H
#define TTargetRecoBeam_H

#include "TObject.h"
//class TargetBeamCalibParams;

class TTargetRecoBeam : public TObject
{
public:

  TTargetRecoBeam();
  TTargetRecoBeam(TClass* hCls);
  virtual ~TTargetRecoBeam(){};
  TTargetRecoBeam(const TTargetRecoBeam& T);
  TTargetRecoBeam(double x, double y,double xerr, double yerr,
                 double XCharge, double YCharge, double XChargeE, double YChargeE, 
				         double eventXTime, double eventXTimeErr, double eventYTime, double eventYTimeErr,
				         double XWidth, double XWidthErr, double YWidth, double YWidthErr,
				         double XCfit, double XCfitErr, double YCfit, double YCfitErr,
				         double XWfit, double XWfitErr, double YWfit, double YWfitErr,
				         double XChi2, double XNdof,double YChi2, double YNdof, double nPOT, double nPOTErr);


  double      getX();
  double      getY();
  double      getXError();
  double      getYError();
  double      getXCharge();
  double      getYCharge();
  double      getXChargeErr();
  double      getYChargeErr();
  double      getXTime();
  double      getXTimeErr();
  double      getYTime();
  double      getYTimeErr();
  
  double      getXWidth();
  double      getXWidthError();
  double      getYWidth();
  double      getYWidthError();
  double      getXCfit();
  double      getXCfitError();
  double      getYCfit();
  double      getYCfitError();
  double      getXWfit();
  double      getXWfitError();
  double      getYWfit();
  double      getYWfitError();
  double      getXChi2();
  double      getXNdof();
  double      getYChi2();
  double      getYNdof();
  double      getnPOT();
  double      getnPOTError();
  

  /*void setX           (double x);
  void setY           (double y);
  void setXError      (double xerr);
  void setYError      (double yerr);
  void setXCharge     (double XCharge);
  void setYCharge     (double YCharge);
  void setXChargeErr  (double XChargeE);
  void setYChargeErr  (double YChargeE);
  void setXTime       (double eventXTime);
  void setXTimeErr    (double eventXTimeErr);
  void setYTime       (double eventYTime);
  void setYTimeErr    (double eventYTimeErr);*/
   
  void setCentroid     (double x, double xErr, double y, double yErr);
  void setWidth        (double xWidth, double xWidthErr, double yWidth, double yWidthErr); 
  void setCharge       (double xCharge, double xChargeErr, double yCharge, double yChargeErr);
  void setTime         (double xTime, double xTimeErr, double yTime, double yTimeErr);     ///da implementare
  void setFitCentroid  (double xCFit, double xCFitErr, double yCFit, double yCFitErr);
  void setFitWidth     (double xWFit,double xWFitErr, double yWFit, double yWFitErr);
  void setFitParameter (double xchi2,double ychi2, double xNdof, double yNdof);
  void setPOT          (double nPOT, double nPOTErr);

protected:
    double      fXCentroid;
    double      fXCentroidError;
    double      fYCentroid;
    double      fYCentroidError;
    double      fXWidth;
    double      fXWidthError;
    double      fYWidth;
    double      fYWidthError;
    double      fXCfit;
    double      fXCfitError;
    double      fYCfit;
    double      fYCfitError;
    double      fXWfit;
    double      fXWfitError;
    double      fYWfit;
    double      fYWfitError;
    double      fXChi2;
    double      fXNdof;
    double      fYChi2;
    double      fYNdof;
    double      fXCharge;
    double      fXChargeError;
    double      fYCharge;
    double      fYChargeError;
    double      fXTime;
    double      fXTimeError;
    double      fYTime;
    double      fYTimeError;
    double      fnPOT;
    double      fnPOTError;
    
   // TargetBeamCalibParams* fCalPar;


   ClassDef(TTargetRecoBeam,1);
    

};
#endif
