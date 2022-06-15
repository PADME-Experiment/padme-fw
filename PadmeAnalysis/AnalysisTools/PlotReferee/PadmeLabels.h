//
//   @file    AtlasLabels.h         
//   
//   @author M.Sutton
// 
//   Copyright (C) 2010 Atlas Collaboration
//
//   $Id: AtlasLabels.h, v0.0   Thu 25 Mar 2010 10:34:20 CET $


#ifndef __PADMELABELS_H
#define __PADMELABELS_H

#include "Rtypes.h"

void PADMELabel(Double_t x,Double_t y,const char* text=NULL,Color_t color=1); 

void PADMELabelOld(Double_t x,Double_t y,bool Preliminary=false,Color_t color=1); 

void PADMEVersion(const char* version=NULL,Double_t x=0.88,Double_t y=0.975,Color_t color=1); 



#endif // __PADMELABELS_H
