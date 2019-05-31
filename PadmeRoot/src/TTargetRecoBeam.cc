// --------------------------------------------------------------
// History:
//
// Created by Lecce Team () 2017-6-27
//
// --------------------------------------------------------------
//

#include "TTargetRecoBeam.hh"

ClassImp(TTargetRecoBeam)

TTargetRecoBeam::TTargetRecoBeam()
{
     fXCentroid=0.;
     fYCentroid=0.;
     fXCentroidError=0.;
     fYCentroidError=0.;
     fXCharge=0.;
     fYCharge=0.;
     fXChargeError=0.;
     fYChargeError=0.;
     fXTime=0.;
     fXTimeError=0.;
     fYTime=0.;
     fYTimeError=0.;
     fXWidth=0.;
     fXWidthError=0.;
     fYWidth=0.;
     fYWidthError=0.;
     fXCfit=0.;
     fXCfitError=0.;
     fYCfit=0.;
     fYCfitError=0.;
     fXWfit=0.;
     fXWfitError=0.;
     fYWfit=0.;
     fYWfitError=0.;
     fXChi2=0.;
     fXNdof=0.;
     fYChi2=0.;
     fYNdof=0.;
     fnPOT=0.;
     fnPOTError=0.;
}

TTargetRecoBeam::TTargetRecoBeam(TClass* hCls)
{
     fXCentroid=0.;
     fYCentroid=0.;
     fXCentroidError=0.;
     fYCentroidError=0.;
     fXCharge=0.;
     fYCharge=0.;
     fXChargeError=0.;
     fYChargeError=0.;
     fXTime=0.;
     fXTimeError=0.;
     fYTime=0.;
     fYTimeError=0.;
     fXWidth=0.;
     fXWidthError=0.;
     fYWidth=0.;
     fYWidthError=0.;
     fXCfit=0.;
     fXCfitError=0.;
     fYCfit=0.;
     fYCfitError=0.;
     fXWfit=0.;
     fXWfitError=0.;
     fYWfit=0.;
     fYWfitError=0.;
     fXChi2=0.;
     fXNdof=0.;
     fYChi2=0.;
     fYNdof=0.;
     fnPOT=0.;
     fnPOTError=0.;
}

/*TTargetRecoBeam::~TTargetRecoBeam()
{
}*/

TTargetRecoBeam::TTargetRecoBeam(const TTargetRecoBeam& T)
{
     fXCentroid=T.fXCentroid;
     fYCentroid=T.fYCentroid;
     fXCentroidError=T.fXCentroidError;
     fYCentroidError=T.fYCentroidError;
     fXCharge=T.fXCharge;
     fYCharge=T.fYCharge;
     fXChargeError=T.fXChargeError;
     fYChargeError=T.fYChargeError;
     fXTime=T.fXTime;
     fXTimeError=T.fXTimeError;
     fYTime=T.fYTime;
     fYTimeError=T.fYTimeError;
     fXWidth=T.fXWidth;
     fXWidthError=T.fXWidthError;
     fYWidth=T.fYWidth;
     fYWidthError=T.fYWidthError;
     fXCfit=T.fXCfit;
     fXCfitError=T.fXCfitError;
     fYCfit=T.fYCfit;
     fYCfitError=T.fYCfitError;
     fXWfit=T.fXWfit;
     fXWfitError=T.fXWfitError;
     fYWfit=T.fYWfit;
     fYWfitError=T.fYWfitError;
     fXChi2=T.fXChi2;
     fXNdof=T.fXNdof;
     fYChi2=T.fYChi2;
     fYNdof=T.fYNdof;
     fnPOT=T.fnPOT;
     fnPOTError=T.fnPOTError;
}
TTargetRecoBeam::TTargetRecoBeam(double x, double y,double xerr, double yerr,
                               double XCharge, double YCharge, double XChargeE, double YChargeE, 
				                       double eventXTime, double eventXTimeErr, double eventYTime, double eventYTimeErr,
				                       double XWidth, double XWidthErr, double YWidth, double YWidthErr,
				                       double XCfit, double XCfitErr, double YCfit, double YCfitErr,
				                       double XWfit, double XWfitErr, double YWfit, double YWfitErr,
				                       double XChi2, double XNdof,double YChi2, double YNdof, double nPOT, double nPOTErr )
{
     fXCentroid=x;
     fYCentroid=y;
     fXCentroidError=xerr;
     fYCentroidError=yerr;
     fXCharge=XCharge;
     fYCharge=YCharge;
     fXChargeError=XChargeE;
     fYChargeError=YChargeE;
     fXTime=eventXTime;
     fXTimeError=eventXTimeErr;
     fYTime=eventYTime;
     fYTimeError=eventYTimeErr;
     fXWidth=XWidth;
     fXWidthError=XWidthErr;
     fYWidth=YWidth;
     fYWidthError=YWidthErr;
     fXCfit=XCfit;
     fXCfitError=XCfitErr;
     fYCfit=YCfit;
     fYCfitError=YCfitErr;
     fXWfit=XWfit;
     fXWfitError=XWfitErr;
     fYWfit=YWfit;
     fYWfitError=YWfitErr;
     fXChi2=XChi2;
     fXNdof=XNdof;
     fYChi2=YChi2;
     fYNdof=YNdof;
     fnPOT=nPOT;
     fnPOTError=nPOTErr;
}

double TTargetRecoBeam::getX(){return fXCentroid;}
double TTargetRecoBeam::getY(){return fYCentroid;}
double TTargetRecoBeam::getXError(){return fXCentroidError;}
double TTargetRecoBeam::getYError(){return fYCentroidError;}
double TTargetRecoBeam::getXCharge(){return fXCharge;}
double TTargetRecoBeam::getYCharge(){return fYCharge;}
double TTargetRecoBeam::getXChargeErr(){return fXChargeError;}
double TTargetRecoBeam::getYChargeErr(){return fYChargeError;}
double TTargetRecoBeam::getXTime(){return fXTime;}
double TTargetRecoBeam::getXTimeErr(){return fXTimeError;}
double TTargetRecoBeam::getYTime(){return fYTime;}
double TTargetRecoBeam::getYTimeErr(){return fYTimeError;}
double TTargetRecoBeam::getXWidth(){return fXWidth;}
double TTargetRecoBeam::getXWidthError(){return fXWidthError;}
double TTargetRecoBeam::getYWidth(){return fYWidth;}
double TTargetRecoBeam::getYWidthError(){return fYWidthError;}
double TTargetRecoBeam::getXCfit(){return fXCfit;}
double TTargetRecoBeam::getXCfitError(){return fXCfitError;}
double TTargetRecoBeam::getYCfit(){return fYCfit;}
double TTargetRecoBeam::getYCfitError(){return fYCfitError;}
double TTargetRecoBeam::getXWfit(){return fXWfit;}
double TTargetRecoBeam::getXWfitError(){return fXWfitError;}
double TTargetRecoBeam::getYWfit(){return fYWfit;}
double TTargetRecoBeam::getYWfitError(){return fYWfitError;}
double TTargetRecoBeam::getXChi2(){return fXChi2;}
double TTargetRecoBeam::getXNdof(){return fXNdof;}
double TTargetRecoBeam::getYChi2(){return fYChi2;}
double TTargetRecoBeam::getYNdof(){return fYNdof;}
double TTargetRecoBeam::getnPOT(){return fnPOT;}
double TTargetRecoBeam::getnPOTError(){return fnPOTError;}
/*
void TTargetRecoBeam::setX         (double x)           {fXCentroid = x;}
void TTargetRecoBeam::setY         (double y)           {fYCentroid = y;}
void TTargetRecoBeam::setXError    (double Xerr)        {fXCentroidError = Xerr;}
void TTargetRecoBeam::setYError    (double Yerr)        {fYCentroidError = Yerr;}
void TTargetRecoBeam::setXCharge   (double XCharge)     {fXCharge = XCharge;}
void TTargetRecoBeam::setYCharge   (double YCharge)     {fYCharge = YCharge;}
void TTargetRecoBeam::setXChargeErr(double XChargeErr)  {fXChargeError = XChargeErr;}
void TTargetRecoBeam::setYChargeErr(double YChargeErr)  {fYChargeError = YChargeErr;}
void TTargetRecoBeam::setXTime(double eventXTime)  {fXTime = eventXTime;}
void TTargetRecoBeam::setXTimeErr(double eventXTimeErr)  {fXTimeError = eventXTimeErr;}
void TTargetRecoBeam::setYTime(double eventYTime)  {fYTime = eventYTime;}
void TTargetRecoBeam::setYTimeErr(double eventYTimeErr)  {fYTimeError = eventYTimeErr;}
*/

void TTargetRecoBeam::setCentroid     (double x, double xErr, double y, double yErr) {fXCentroid=x, fXCentroidError=xErr,fYCentroid=y, fYCentroidError=yErr;}
void TTargetRecoBeam::setWidth        (double xWidth, double xWidthErr, double yWidth, double yWidthErr){fXWidth=xWidth, fXWidthError=xWidthErr, fYWidth=yWidth, fYWidthError=yWidthErr;}
void TTargetRecoBeam::setCharge       (double xCharge, double xChargeErr, double yCharge, double yChargeErr){fXCharge=xCharge, fXChargeError=xChargeErr,       fYCharge=yCharge, fYChargeError=yChargeErr;}
void TTargetRecoBeam::setTime         (double xTime, double xTimeErr, double yTime, double yTimeErr){fXTime=xTime, fXTimeError=xTimeErr, fYTime=yTime, fYTimeError=yTimeErr;}
void TTargetRecoBeam::setFitCentroid  (double xCFit, double xCFitErr, double yCFit, double yCFitErr ){fXCfit=xCFit, fXCfitError=xCFitErr, fYCfit=yCFit, fYCfitError=yCFitErr;}
void TTargetRecoBeam::setFitWidth     (double xWFit, double xWFitErr, double yWFit, double yWFitErr){fXWfit=xWFit, fXWfitError=xWFitErr, fYWfit=yWFit, fYWfitError=yWFitErr;}
void TTargetRecoBeam::setFitParameter (double xchi2, double ychi2, double xNdof, double yNdof){fXChi2=xchi2, fYChi2=ychi2, fXNdof=xNdof, fYNdof=yNdof;}
void TTargetRecoBeam::setPOT          (double nPOT, double nPOTErr){fnPOT=nPOT, fnPOTError=nPOTErr;}

