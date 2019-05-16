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
  
    void Print(Option_t* option="") const;

public:
 
  Int_t GetDNumber()      const { return fDNumber;   };
  Double_t GetChargeT()      const { return fChargeT;   };
  Double_t GetCharge()      const { return fCharge;   };
  Double_t GetNoiseCharge()      const { return fNCharge;   };
  const std::vector<Double_t>& GetWaveformRaw()  const { return fWaveformRaw;   };
  const std::vector<Double_t>& GetWaveform()  const { return fWaveform;   };

  void  SetDNumber  (Int_t    value) { fDNumber = value;   };
  void  SetChargeT  (Double_t    value) { fChargeT = value;   };
  void  SetCharge  (Double_t    value) { fCharge = value;   };
  void  SetNoiseCharge  (Double_t    value) { fNCharge = value;   };
  void  SetWaveformRaw   (const std::vector<double>& value) { fWaveformRaw = value;      };
  void  SetWaveform   (const std::vector<double>& value) { fWaveform = value;      };
 
private:
  
  Int_t    fDNumber;
  Double_t    fChargeT;
  Double_t    fCharge;
  Double_t    fNCharge;
  std::vector<double> fWaveformRaw;
  std::vector<double> fWaveform;

  ClassDef(TTargetMCDigi,1);
};
#endif
