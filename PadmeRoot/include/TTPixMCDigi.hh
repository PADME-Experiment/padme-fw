// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------
#ifndef TTPixMCDigi_H
#define TTPixMCDigi_H

#include "TMCVDigi.hh"

#define TTPIXMCDIGI_MAX_NPIXELS 65536

class TTPixMCDigi : public TMCVDigi
{

public:

  TTPixMCDigi();
  ~TTPixMCDigi();

  Int_t AllocatePixels(Int_t);
  Int_t SetPixel(Int_t,UShort_t,UInt_t);

  Int_t GetNPixels() { return fNPixels; }
  UShort_t GetPixelAddress(Int_t i) { return (i<fNPixels)?fPixelAddress[i]:0; }
  UInt_t GetPixelValue(Int_t i) { return (i<fNPixels)?fPixelValue[i]:0; }

private:

  // One digi per TPix chip. Each digi is a list of pixels defined by
  // address (UShort_t: 16bits) and value (UInt_t: 32bits)
  Int_t fNPixels;
  UShort_t* fPixelAddress; //[fNPixels] Do not remove this comment: it is needed by the streamer
  UInt_t*   fPixelValue;   //[fNPixels] Do not remove this comment: it is needed by the streamer

  ClassDef(TTPixMCDigi,1);
};
#endif
