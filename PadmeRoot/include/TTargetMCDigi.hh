// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-21
//
// --------------------------------------------------------------
#ifndef TTargetMCDigi_H
#define TTargetMCDigi_H

#include "TMCVDigi.hh"

class TTargetMCDigi : public TMCVDigi
{

public:

  TTargetMCDigi();
  ~TTargetMCDigi();

public:
 
  Int_t GetDNumber()      const { return fDNumber;   };
  Double_t GetChargeT()      const { return fChargeT;   };
  Double_t GetCharge()      const { return fCharge;   };
  const std::vector<Double_t>& GetTimeTrace()  const { return fTimeTrace;   };
  const std::vector<Double_t>& GetTimeTraceV()  const { return fTimeTraceV;   };

  void  SetDNumber  (Int_t    value) { fDNumber = value;   };
  void  SetChargeT  (Double_t    value) { fChargeT = value;   };
  void  SetCharge  (Double_t    value) { fCharge = value;   };
  void  SetTimeTrace   (const std::vector<double>& value) { fTimeTrace = value;      };
  void  SetTimeTraceV   (const std::vector<double>& value) { fTimeTraceV = value;      };
 
protected:
  
  Int_t    fDNumber;
  Double_t    fChargeT;
  Double_t    fCharge;
  std::vector<double> fTimeTrace;
  std::vector<double> fTimeTraceV;


  ClassDef(TTargetMCDigi,1);
};
#endif
