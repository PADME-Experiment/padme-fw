// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PersistencyDICT

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "/home/mancinima/padme-fw/PADME_Sensitivity/include/ChannelModel.h"
#include "/home/mancinima/padme-fw/PADME_Sensitivity/include/GlobalParams.h"
#include "/home/mancinima/padme-fw/PADME_Sensitivity/include/MuMuBkgShape.h"
#include "/home/mancinima/padme-fw/PADME_Sensitivity/include/MuMuSignalShape.h"
#include "/home/mancinima/padme-fw/PADME_Sensitivity/include/StatisticsModel.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_MuMuBkgShape(void *p = 0);
   static void *newArray_MuMuBkgShape(Long_t size, void *p);
   static void delete_MuMuBkgShape(void *p);
   static void deleteArray_MuMuBkgShape(void *p);
   static void destruct_MuMuBkgShape(void *p);
   static void streamer_MuMuBkgShape(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MuMuBkgShape*)
   {
      ::MuMuBkgShape *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MuMuBkgShape >(0);
      static ::ROOT::TGenericClassInfo 
         instance("MuMuBkgShape", ::MuMuBkgShape::Class_Version(), "", 158,
                  typeid(::MuMuBkgShape), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MuMuBkgShape::Dictionary, isa_proxy, 16,
                  sizeof(::MuMuBkgShape) );
      instance.SetNew(&new_MuMuBkgShape);
      instance.SetNewArray(&newArray_MuMuBkgShape);
      instance.SetDelete(&delete_MuMuBkgShape);
      instance.SetDeleteArray(&deleteArray_MuMuBkgShape);
      instance.SetDestructor(&destruct_MuMuBkgShape);
      instance.SetStreamerFunc(&streamer_MuMuBkgShape);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MuMuBkgShape*)
   {
      return GenerateInitInstanceLocal((::MuMuBkgShape*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MuMuBkgShape*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MuMuSignalShape(void *p = 0);
   static void *newArray_MuMuSignalShape(Long_t size, void *p);
   static void delete_MuMuSignalShape(void *p);
   static void deleteArray_MuMuSignalShape(void *p);
   static void destruct_MuMuSignalShape(void *p);
   static void streamer_MuMuSignalShape(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MuMuSignalShape*)
   {
      ::MuMuSignalShape *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MuMuSignalShape >(0);
      static ::ROOT::TGenericClassInfo 
         instance("MuMuSignalShape", ::MuMuSignalShape::Class_Version(), "", 202,
                  typeid(::MuMuSignalShape), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MuMuSignalShape::Dictionary, isa_proxy, 16,
                  sizeof(::MuMuSignalShape) );
      instance.SetNew(&new_MuMuSignalShape);
      instance.SetNewArray(&newArray_MuMuSignalShape);
      instance.SetDelete(&delete_MuMuSignalShape);
      instance.SetDeleteArray(&deleteArray_MuMuSignalShape);
      instance.SetDestructor(&destruct_MuMuSignalShape);
      instance.SetStreamerFunc(&streamer_MuMuSignalShape);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MuMuSignalShape*)
   {
      return GenerateInitInstanceLocal((::MuMuSignalShape*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MuMuSignalShape*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr MuMuBkgShape::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MuMuBkgShape::Class_Name()
{
   return "MuMuBkgShape";
}

//______________________________________________________________________________
const char *MuMuBkgShape::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MuMuBkgShape*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MuMuBkgShape::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MuMuBkgShape*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MuMuBkgShape::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MuMuBkgShape*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MuMuBkgShape::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MuMuBkgShape*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MuMuSignalShape::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MuMuSignalShape::Class_Name()
{
   return "MuMuSignalShape";
}

//______________________________________________________________________________
const char *MuMuSignalShape::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MuMuSignalShape*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MuMuSignalShape::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MuMuSignalShape*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MuMuSignalShape::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MuMuSignalShape*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MuMuSignalShape::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MuMuSignalShape*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void MuMuBkgShape::Streamer(TBuffer &R__b)
{
   // Stream an object of class MuMuBkgShape.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      RooAbsPdf::Streamer(R__b);
      x.Streamer(R__b);
      coeffs.Streamer(R__b);
      xMin.Streamer(R__b);
      xMax.Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, MuMuBkgShape::IsA());
   } else {
      R__c = R__b.WriteVersion(MuMuBkgShape::IsA(), kTRUE);
      RooAbsPdf::Streamer(R__b);
      x.Streamer(R__b);
      coeffs.Streamer(R__b);
      xMin.Streamer(R__b);
      xMax.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MuMuBkgShape(void *p) {
      return  p ? new(p) ::MuMuBkgShape : new ::MuMuBkgShape;
   }
   static void *newArray_MuMuBkgShape(Long_t nElements, void *p) {
      return p ? new(p) ::MuMuBkgShape[nElements] : new ::MuMuBkgShape[nElements];
   }
   // Wrapper around operator delete
   static void delete_MuMuBkgShape(void *p) {
      delete ((::MuMuBkgShape*)p);
   }
   static void deleteArray_MuMuBkgShape(void *p) {
      delete [] ((::MuMuBkgShape*)p);
   }
   static void destruct_MuMuBkgShape(void *p) {
      typedef ::MuMuBkgShape current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_MuMuBkgShape(TBuffer &buf, void *obj) {
      ((::MuMuBkgShape*)obj)->::MuMuBkgShape::Streamer(buf);
   }
} // end of namespace ROOT for class ::MuMuBkgShape

//______________________________________________________________________________
void MuMuSignalShape::Streamer(TBuffer &R__b)
{
   // Stream an object of class MuMuSignalShape.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      RooAbsPdf::Streamer(R__b);
      x.Streamer(R__b);
      mean.Streamer(R__b);
      sigma.Streamer(R__b);
      xMin.Streamer(R__b);
      xMax.Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, MuMuSignalShape::IsA());
   } else {
      R__c = R__b.WriteVersion(MuMuSignalShape::IsA(), kTRUE);
      RooAbsPdf::Streamer(R__b);
      x.Streamer(R__b);
      mean.Streamer(R__b);
      sigma.Streamer(R__b);
      xMin.Streamer(R__b);
      xMax.Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MuMuSignalShape(void *p) {
      return  p ? new(p) ::MuMuSignalShape : new ::MuMuSignalShape;
   }
   static void *newArray_MuMuSignalShape(Long_t nElements, void *p) {
      return p ? new(p) ::MuMuSignalShape[nElements] : new ::MuMuSignalShape[nElements];
   }
   // Wrapper around operator delete
   static void delete_MuMuSignalShape(void *p) {
      delete ((::MuMuSignalShape*)p);
   }
   static void deleteArray_MuMuSignalShape(void *p) {
      delete [] ((::MuMuSignalShape*)p);
   }
   static void destruct_MuMuSignalShape(void *p) {
      typedef ::MuMuSignalShape current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_MuMuSignalShape(TBuffer &buf, void *obj) {
      ((::MuMuSignalShape*)obj)->::MuMuSignalShape::Streamer(buf);
   }
} // end of namespace ROOT for class ::MuMuSignalShape

namespace {
  void TriggerDictionaryInitialization_libPersistency_Impl() {
    static const char* headers[] = {
0    };
    static const char* includePaths[] = {
"/cvmfs/sft.cern.ch/lcg/releases/LCG_87/ROOT/6.08.02/x86_64-centos7-gcc62-opt/include",
"/home/mancinima/padme-fw/PADME_Sensitivity/include",
"/cvmfs/sft.cern.ch/lcg/releases/ROOT/6.08.02-be67f/x86_64-centos7-gcc62-opt/include",
"/home/mancinima/padme-fw/PADME_Sensitivity/build/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPersistency dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(Your description goes here...)ATTRDUMP"))) MuMuBkgShape;
class __attribute__((annotate(R"ATTRDUMP(Your description goes here...)ATTRDUMP"))) MuMuSignalShape;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPersistency dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
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

enum signal_types {
  kGaussian = 0,
};
enum background_types { kLandau = 0, kBernstein };

class ChannelModel {
public:
  ChannelModel(TString channelName, RooRealVar &_invMass, RooRealVar &_dpMass,
               RooRealVar &_pot_true, RooRealVar &_mu_test, signal_types sig_type,
               background_types bkg_type, double bkg_mean, double sigma_bkg);
  ~ChannelModel();

public:
  TString fChannelName;
  std::shared_ptr<RooRealVar> invariantMass;
  std::shared_ptr<RooRealVar> dpMass;
  std::shared_ptr<RooRealVar> pot_true;
  std::shared_ptr<RooRealVar> mu_test;

  std::map<TString, RooRealVar*> dpMass_resolution;
  std::map<TString, RooAbsPdf*> signal_shape;
  std::map<TString, RooRealVar*> signal_yeild; // from histo
  std::map<TString, RooProduct*> expected_signal_yield;

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

#endif#ifndef GLOBALPARAMS
#define GLOBALPARAMS

#include "TH2D.h"

#include "RooRealVar.h"
#include <vector>

namespace GlobalParams{
  // extern RooRealVar mInv;
  // extern RooRealVar dpMass;
  // extern RooRealVar pot_true;
  
  extern std::vector<TString> signal_sources; 
};

enum e_runType{
  kDebugRun = 0,
  kExclusionRun,
  kExpectedBandRun
};

enum e_statistic_type{
  kTevatron=0,
  kLHC,
  kCLSB
};

enum e_tossing_type{
  kCousinsHighland=0,
  kFixToFit
};


class global_config {
public:
  global_config()
      : hYield(), hMSigma(),
        mass_points(0), n_toys(100), log_coupling_points(0),
        useEE(false), useMuMu(true), runType(kExclusionRun), statType(kTevatron),cl(0.9){}
  ~global_config() {
    for (auto p : hYield){
      for (auto q : p.second){
        delete q.second;
      }
      p.second.clear();
    }
    hYield.clear();
    for (auto p : hMSigma){
      for (auto q : p.second){
        delete q.second;
      }
      p.second.clear();
    }
    hMSigma.clear();
    
    mass_points.clear();
    log_coupling_points.clear();
  }

  void retrieve_input();
  void configure_run(const int &argc, char **argv);
  void print_config();

public:
  std::map<TString, std::map<TString, TH2D*>> hYield;
  std::map<TString, std::map<TString, TH2D*>> hMSigma;
  std::vector<double> mass_points;
  int n_toys;
  std::vector<double> log_coupling_points;
  bool useEE;
  bool useMuMu;
  e_runType runType;
  e_statistic_type statType;
  e_tossing_type tossingType;
  double cl;
};

#endif/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * This code was autogenerated by RooClassFactory                            *
 *****************************************************************************/

#ifndef MUMUBKGSHAPE
#define MUMUBKGSHAPE

#include "RooAbsCategory.h"
#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooCategoryProxy.h"
#include "RooListProxy.h"
#include "RooRealProxy.h"

class MuMuBkgShape : public RooAbsPdf {
public:
  MuMuBkgShape(){};
  MuMuBkgShape(const char *name, const char *title, RooAbsReal &_x,
               const RooArgList &_coeffs, RooAbsReal &_xMin, RooAbsReal &_xMax);
  MuMuBkgShape(const MuMuBkgShape &other, const char *name = 0);
  virtual TObject *clone(const char *newname) const {
    return new MuMuBkgShape(*this, newname);
  }
  inline virtual ~MuMuBkgShape() {}

  Int_t getAnalyticalIntegral(RooArgSet &allVars, RooArgSet &analVars,
                              const char * /*rangeName*/) const;
  double analyticalIntegral(Int_t code, const char *rangeName) const;

protected:
  RooRealProxy x;
  RooListProxy coeffs;
  RooRealProxy xMin;
  RooRealProxy xMax;

  Double_t evaluate() const;

private:
  ClassDef(MuMuBkgShape, 1) // Your description goes here...
};

#endif
/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef MUMUSIGNALSHAPE
#define MUMUSIGNALSHAPE

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
 
class MuMuSignalShape : public RooAbsPdf {
public:
  MuMuSignalShape() {} ; 
  MuMuSignalShape(const char *name, const char *title,
	      RooAbsReal& _x,
	      RooAbsReal& _mean,
	      RooAbsReal& _sigma,
	      RooAbsReal& _xMin,
	      RooAbsReal& _xMax);
  MuMuSignalShape(const MuMuSignalShape& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new MuMuSignalShape(*this,newname); }
  inline virtual ~MuMuSignalShape() { }

  Int_t getAnalyticalIntegral(RooArgSet &allVars, RooArgSet& analVars, const char* /*rangeName*/) const;
  double analyticalIntegral(Int_t code, const char *rangeName) const;

  static double getGaussIntegral(double a);
protected:

  RooRealProxy x ;
  RooRealProxy mean ;
  RooRealProxy sigma ;
  RooRealProxy xMin ;
  RooRealProxy xMax ;
  
  Double_t evaluate() const ;

private:

  ClassDef(MuMuSignalShape,1) // Your description goes here...
};
 
#endif
#ifndef STATISTICSMODEL
#define STATISTICSMODEL

#include "ChannelModel.h"
#include "GlobalParams.h"

#include "RooArgSet.h"
#include "RooCategory.h"
#include "RooLognormal.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"

#include "TString.h"

#include <map>

class global_config;
class RooAbsPdf;

class StatisticsModel {
public:
  StatisticsModel(global_config &cfg);
  ~StatisticsModel();

  void AddChannel(TString name, signal_types st, background_types bt,
                  double bkg_mean, double sigma_bkg);
  void BuildModel();
  void Update(global_config &cfg, double mass, double log_coupling);

  double GetTotalYield();
  Bool_t IsLowYield();
  Bool_t IsHighYield();

  RooDataSet *Generate(global_config &cfg, Double_t mu, RooAbsPdf *pdf,
                       RooArgList &fitParams);

  double ComputeQstatistic(global_config &cfg, RooDataSet &ds,
                           RooArgSet *paramsInit);

  void ComputeQDistribution(global_config &cfg, RooAbsPdf *genPdf,
                            RooArgList &paramsGen, RooArgSet *paramsInit,
                            std::vector<double> &qDistribution);

  void SetDefaultAuxiliaryObservation() {
    pot_default->setVal(1.0);
    for (std::size_t i = 0; i < channels.size(); i++) {
      channelModels[i]->bkg_yield_default->setVal(default_bkgYield_MC[i]);
    }
  }

public:
  RooRealVar *invariantMass;
  RooRealVar *dpMass;
  RooRealVar *mu_test;

  std::vector<ChannelModel *> channelModels;
  std::vector<TString> channels;
  std::vector<double> default_bkgYield_MC;

  RooCategory *pid_categories;

  RooRealVar *pot_default;
  RooRealVar *pot_sigma;
  RooRealVar *pot_true;
  RooAbsPdf *pot_constraint;

  RooSimultaneous *sbModel_simultaneous;
  RooSimultaneous *bkgModel_simultaneous;

  RooProdPdf *sbModel_full;
  RooProdPdf *bkgModel_full;

  RooSimultaneous *sbModel_simultaneous_noConstraints;
  RooSimultaneous *bkgModel_simultaneous_noConstraints;

  RooArgSet *global_observarbles;
  RooArgSet *fit_parameters;
  RooArgSet
      *constrained_parameters; // needed because bkg does not depend on pot_true

  std::map<RooRealVar *, RooAbsPdf *> constraints_map;
};

#endif
#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"MuMuBkgShape", payloadCode, "@",
"MuMuSignalShape", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPersistency",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPersistency_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPersistency_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPersistency() {
  TriggerDictionaryInitialization_libPersistency_Impl();
}
