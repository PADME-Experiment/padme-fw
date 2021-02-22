//
// PADME Style, based on a style file from BaBar
//

#include <iostream>

#include "PadmeStyle.h"

#include "TROOT.h"

void SetPadmeStyle ()
{
  static TStyle* padmeStyle = 0;
  std::cout << "\nApplying PADME style settings...\n" << std::endl ;
  if ( padmeStyle==0 ) padmeStyle = PadmeStyle();
  gROOT->SetStyle("PADME");
  gROOT->ForceStyle();
}

TStyle* PadmeStyle() 
{
  TStyle *padmeStyle = new TStyle("PADME","Padme style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  padmeStyle->SetFrameBorderMode(icol);
  padmeStyle->SetFrameFillColor(icol);
  padmeStyle->SetCanvasBorderMode(icol);
  padmeStyle->SetCanvasColor(icol);
  padmeStyle->SetPadBorderMode(icol);
  padmeStyle->SetPadColor(icol);
  padmeStyle->SetStatColor(icol);
  //padmeStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  padmeStyle->SetPaperSize(20,26);

  // set margin sizes
  padmeStyle->SetPadTopMargin(0.05);
  padmeStyle->SetPadRightMargin(0.05);
  padmeStyle->SetPadBottomMargin(0.16);
  padmeStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  padmeStyle->SetTitleXOffset(1.4);
  padmeStyle->SetTitleYOffset(1.4);

  // use large fonts
  Int_t font=132; // Helvetica italics
  Int_t Labelfont=62; // Helvetica
  Double_t tsize=0.055;
  //Double_t labelsize=0.045;
  padmeStyle->SetTextFont(font);

  padmeStyle->SetTextSize(tsize);
  padmeStyle->SetLabelFont(font,"x");
  padmeStyle->SetTitleFont(Labelfont,"x");
  padmeStyle->SetLabelFont(font,"y");
  padmeStyle->SetTitleFont(Labelfont,"y");
  padmeStyle->SetLabelFont(font,"z");
  padmeStyle->SetTitleFont(font,"z");
  
  padmeStyle->SetLabelSize(tsize,"x");
  padmeStyle->SetTitleSize(tsize,"x");
  padmeStyle->SetLabelSize(tsize,"y");
  padmeStyle->SetTitleSize(tsize,"y");
  padmeStyle->SetLabelSize(tsize,"z");
  padmeStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  padmeStyle->SetMarkerStyle(20);
  padmeStyle->SetMarkerSize(1.);
  padmeStyle->SetHistLineWidth(1.);
  padmeStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars (as recommended in PADME figure guidelines)
  padmeStyle->SetErrorX(0.0001);
  // get rid of error bar caps
  padmeStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  padmeStyle->SetOptTitle(0);
  //padmeStyle->SetOptStat(1111);
  padmeStyle->SetOptStat(0);
  //padmeStyle->SetOptFit(1111);
  padmeStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  padmeStyle->SetPadTickX(1);
  padmeStyle->SetPadTickY(1);

  return padmeStyle;

}

