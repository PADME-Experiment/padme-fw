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
  kBernstein = 1 }; //due pdf diverse per la shape del fondo, landau per elettroni - bernstein per muoni (tipo)

class ChannelModel {
public:
  ChannelModel(TString channelName, RooRealVar &_invMass, RooRealVar &_dpMass,
               RooRealVar &_pot_true, RooRealVar &_mu_test, signal_types sig_type,
               background_types bkg_type, double bkg_mean, double sigma_bkg); //background_type scritto così gli passa un nome che corrisponde ad un numero, per cui posso riferirmi al tipo di bkg sia con un indice che con un nome (credo)
  ~ChannelModel();

public:
  TString fChannelName;
  shared_ptr<RooRealVar> invariantMass; //utilizzare shared_ptr serve per deallocare automaticamente la memoria quando si chiude questa classe
  shared_ptr<RooRealVar> dpMass;
  shared_ptr<RooRealVar> pot_true;
  shared_ptr<RooRealVar> mu_test; //PER QUESTI CREO DEI PUNTATORI ALLE VARIABILI (tipo mu_test per esempio) CHE POI QUANDO FINISCE LO SCOPE (ESCO DALL'AREA DI UTILIZZO) DEALLOCANO AUTOMATICAMENTE LA MEMORIA

  map<TString, RooRealVar*> dpMass_resolution; //sono mappe perché voglio creare due puntatori uguali, uno che punti alle variabili relative al cananle dell'esperimento con i "muon"
  map<TString, RooAbsPdf*> signal_shape;       // uno al canale con gli "electrons"
  map<TString, RooRealVar*> signal_yield; // from histo
  map<TString, RooProduct*> expected_signal_yield;

  RooAbsPdf *bkg_shape;

  RooRealVar *bkg_yield_default;
  RooRealVar *bkg_yield_scale;
  RooProduct *bkg_yield_true;
  RooRealVar *bkg_yield_sigma;
  RooAbsPdf *bkg_yield_constraint;

  RooAddPdf *extended_sbModel;
  RooExtendPdf *extended_bkgModel;
  RooProdPdf *full_sbModel;
  RooProdPdf *full_bkgModel;

  // RooProduct *expected_bkg;

};

#endif