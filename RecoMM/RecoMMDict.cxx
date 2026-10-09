// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME RecoMMDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
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

// Header files passed as explicit arguments
#include "RecoMM.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *RecoMM_Dictionary();
   static void RecoMM_TClassManip(TClass*);
   static void delete_RecoMM(void *p);
   static void deleteArray_RecoMM(void *p);
   static void destruct_RecoMM(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::RecoMM*)
   {
      ::RecoMM *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::RecoMM));
      static ::ROOT::TGenericClassInfo 
         instance("RecoMM", "RecoMM.h", 53,
                  typeid(::RecoMM), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &RecoMM_Dictionary, isa_proxy, 0,
                  sizeof(::RecoMM) );
      instance.SetDelete(&delete_RecoMM);
      instance.SetDeleteArray(&deleteArray_RecoMM);
      instance.SetDestructor(&destruct_RecoMM);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::RecoMM*)
   {
      return GenerateInitInstanceLocal(static_cast<::RecoMM*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::RecoMM*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *RecoMM_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::RecoMM*>(nullptr))->GetClass();
      RecoMM_TClassManip(theClass);
   return theClass;
   }

   static void RecoMM_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_RecoMM(void *p) {
      delete (static_cast<::RecoMM*>(p));
   }
   static void deleteArray_RecoMM(void *p) {
      delete [] (static_cast<::RecoMM*>(p));
   }
   static void destruct_RecoMM(void *p) {
      typedef ::RecoMM current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::RecoMM

namespace {
  void TriggerDictionaryInitialization_RecoMMDict_Impl() {
    static const char* headers[] = {
"RecoMM.h",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.34.04/x86_64-almalinux9.5-gcc115-opt/include/",
"/home/mancinima/BeamMonitorRun4/padme-fw/RecoMM/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "RecoMMDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$RecoMM.h")))  RecoMM;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "RecoMMDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "RecoMM.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"RecoMM", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("RecoMMDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_RecoMMDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_RecoMMDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_RecoMMDict() {
  TriggerDictionaryInitialization_RecoMMDict_Impl();
}
