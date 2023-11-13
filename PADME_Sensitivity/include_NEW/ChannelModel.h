#ifndef CHANNELMODEL
#define CHANNELMODEL

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooGaussian.h"
#include "RooLognormal.h"
#include "RooProdPdf.h"
#include "RooProduct.h"
#include "RooRealVar.h"
#include "TString.h"

#include <map>

using namespace std;

enum signal_types {
  kGaussian = 0,
};
enum background_types {
  kLandau = 0, 
  kBernstein = 1
}; //just one bkg bhabha + AA -> one bkg shape, using ETag BKG shape must be modified

class ChannelModel {
public:
  ChannelModel(double Nom_BeamEnergy, RooRealVar &_N2Cluster, RooRealVar &_X17Mass, RooRealVar &_NPoT, RooRealvar &_SigmaEB, RooRealVar &_EffSignal,
               RooRealVar &_NPoT_true, RooRealVar &_Mu_test, signal_types sig_type,
               background_types bkg_type, double Bkg_mean, double Sigma_bkg); //background_type scritto così gli passa un nome che corrisponde ad un numero, per cui posso riferirmi al tipo di bkg sia con un indice che con un nome (credo)
  ~ChannelModel();

public:
  vector<double> fEBeam;
  shared_ptr<RooRealVar> N2Cluster; //utilizzare shared_ptr serve per deallocare automaticamente la memoria quando si chiude questa classe
  shared_ptr<RooRealVar> NPoT;
  shared_ptr<RooRealVar> SigmaEB;
  shared_ptr<RooRealVar> EffSignal;
  shared_ptr<RooRealVar> X17Mass;
  shared_ptr<RooRealVar> NPoT_true;
  shared_ptr<RooRealVar> Mu_test; //PER QUESTI CREO DEI PUNTATORI ALLE VARIABILI (tipo mu_test per esempio) CHE POI QUANDO FINISCE LO SCOPE (ESCO DALL'AREA DI UTILIZZO) DEALLOCANO AUTOMATICAMENTE LA MEMORIA

  map<double, RooRealVar*> X17Mass_resolution; //sono mappe perché voglio creare puntatori uguali, che puntino a ciascun canale definito dalla Nominal Beam Energy
  map<double, RooRealVar*> Signal_shape;       
  map<double, RooRealVar*> Signal_yield; // from histo
  map<double, RooRealVar*> Expected_signal_yield;

  RooAbsPdf *Bkg_shape;

  RooRealVar *Bkg_yield_default;
  RooRealVar *Bkg_yield_scale;
  RooProduct *Bkg_yield_true;
  RooRealVar *Bkg_yield_sigma;
  RooAbsPdf *Bkg_yield_constraint;

  RooAddPdf *Extended_sbModel;
  RooExtendPdf *Extended_bkgModel;
  RooProdPdf *Full_sbModel;
  RooProdPdf *Full_bkgModel;

  // RooProduct *expected_bkg;

};

#endif