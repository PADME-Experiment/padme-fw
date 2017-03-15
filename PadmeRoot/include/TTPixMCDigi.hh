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

  Int_t GetNPixels() { return fNPixels; }
  ULong_t GetPixel(Int_t i) { return (i<fNPixels)?fPixel[i]:0; }
  Int_t AllocatePixels(Int_t);
  Int_t SetPixel(Int_t,ULong_t);

private:

  // One digi per chip. Each pixel is a 64bits integers
  Int_t fNPixels;
  ULong_t* fPixel;

  ClassDef(TTPixMCDigi,1);
};
#endif
