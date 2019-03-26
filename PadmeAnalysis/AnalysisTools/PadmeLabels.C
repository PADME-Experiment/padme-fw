#include "PadmeLabels.h"

#include "TLatex.h"
#include "TLine.h"
#include "TPave.h"
#include "TPad.h"
#include "TMarker.h"


void PADMELabel(Double_t x,Double_t y,const char* text,Color_t color) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetTextSize(0.09);
  l.SetNDC();
  l.SetTextFont(22);
  l.SetTextColor(color);

  //double delx = 0.115*696*gPad->GetWh()/(472*gPad->GetWw());
  double delx = 0.13*696*gPad->GetWh()/(472*gPad->GetWw());

  l.DrawLatex(x+0.02,y+0.65,"PADME");
  if (text) {
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(52);
    p.SetTextSize(0.07);
    p.SetTextColor(1);
    p.DrawLatex(x+0.36,y+ 0.65,text);
    //    p.DrawLatex(x,y,"#sqrt{s}=900GeV");
  }
}


void PADMELabelOld(Double_t x,Double_t y,bool Preliminary,Color_t color) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(82);
  l.SetTextColor(color);
  l.DrawLatex(x,y,"PADME");
  if (Preliminary) {
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(color);
    p.DrawLatex(x+0.215,y,"Preliminary");
  }
   
}



void PADMEVersion(const char* version,Double_t x,Double_t y,Color_t color) 
{

  if (version) {
    char versionString[100];
    sprintf(versionString,"Version %s",version);
    TLatex l;
    l.SetTextAlign(22); 
    l.SetTextSize(0.04); 
    l.SetNDC();
    l.SetTextFont(72);
    l.SetTextColor(color);
    l.DrawLatex(x,y,versionString);
  }
}




