// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2017-03-15
//
// --------------------------------------------------------------

#include "TTPixMCDigi.hh"

ClassImp(TTPixMCDigi)

TTPixMCDigi::TTPixMCDigi() : TMCVDigi()
{
  fNPixels = 0;
  fPixelAddress = 0;
  fPixelValue = 0;
}

TTPixMCDigi::~TTPixMCDigi()
{;}

// Allocate pixels array for this digi. Return number of allocated pixels or -1 if error
Int_t TTPixMCDigi::AllocatePixels(Int_t n)
{
  if (fNPixels!=0) {
    printf("TTPixMCDigi::AllocatePixels - ERROR: multiple allocation of pixels for the same digi\n");
    return -1;
  } else if (n>TTPIXMCDIGI_MAX_NPIXELS) {
    printf("TTPixMCDigi::AllocatePixels - ERROR: trying to allocate %d pixels. Max is %d\n",n,TTPIXMCDIGI_MAX_NPIXELS);
    return -1;
  } else if (n>0) {
      fNPixels = n;
      fPixelAddress = new UShort_t[fNPixels];
      fPixelValue = new UInt_t[fNPixels];
  }
  return n;
}

// Assign 64bits value to pixel i. Return i if success, -1 if error 
Int_t TTPixMCDigi::SetPixel(Int_t i,UShort_t a,UInt_t v)
{
  if (i<fNPixels) {
    fPixelAddress[i] = a;
    fPixelValue[i] = v;
  } else {
    printf("TTPixMCDigi::SetPixel - ERROR: trying to assign value to pixel %d. Max pixel index is %d\n",i,fNPixels-1);
    return -1;
  }
  return i;
}
