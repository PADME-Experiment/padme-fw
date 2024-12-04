#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <vector>

#include "Riostream.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TTimeStamp.h"

#include "Configuration.hh"

#include "utlConfigParser.hh"

// Connect to configuration class
Configuration* cfg = Configuration::GetInstance();

void Get1DHisto(TFile* reco,FILE* fout,TString dirname,TString hname){

  TDirectory* tdir = (TDirectory*)reco->Get(dirname);
  TH1F* h1d = (TH1F*)tdir->Get(hname);
  if (h1d) {

    fprintf(fout,"PLOTID %s\n",hname.Data());
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME %s %s\n",hname.Data(),cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetXaxis()->GetXmin(),h1d->GetXaxis()->GetXmax());
    fprintf(fout,"DATA [");
    fprintf(fout,"[");
    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");

  }

}

void Get2DHisto(TFile* reco,FILE* fout,TString dirname,TString hname){

  TDirectory* tdir = (TDirectory*)reco->Get(dirname);
  TH2F* h2d = (TH2F*)tdir->Get(hname);
  if (h2d) {

    fprintf(fout,"PLOTID %s\n",hname.Data());
    fprintf(fout,"PLOTTYPE heatmap\n");
    fprintf(fout,"PLOTNAME %s %s\n",hname.Data(),cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d %d\n",h2d->GetNbinsX (),h2d->GetNbinsY ());
    fprintf(fout,"RANGE_X 0 %d\n",h2d->GetNbinsX ());
    fprintf(fout,"RANGE_Y 0 %d\n",h2d->GetNbinsY ());
    fprintf(fout,"TITLE_X X\n");
    fprintf(fout,"TITLE_Y Y\n");
    fprintf(fout,"DATA [");
    for(int ix = 1;ix<= h2d->GetNbinsX ();ix++) {
      fprintf(fout,"[");
      for(int iy = 1;iy<= h2d->GetNbinsY ();iy++) {
	if(iy <  h2d->GetNbinsY ()) {
	  fprintf(fout,"%f,",h2d->GetBinContent(iy,ix));
	} else {
	  fprintf(fout,"%f",h2d->GetBinContent(iy,ix));
	}
      }
      if(ix<h2d->GetNbinsX ()) {
	fprintf(fout,"],");
      } else {
	fprintf(fout,"]");
      }
    }
    fprintf(fout,"]\n\n");

  }

}

void GetECalOccupancy(TFile* reco,FILE* fout) {

  TDirectory* tdir = (TDirectory*)reco->Get("ECal");
  TH2F* h2d = (TH2F*)tdir->Get("ECalOccupancy");

  fprintf(fout,"PLOTID ECal_HeatMap\n");
  fprintf(fout,"PLOTTYPE heatmap\n");
  fprintf(fout,"PLOTNAME ECal Occupancy %s\n",cfg->FormatCurrentTime());
  fprintf(fout,"CHANNELS %d %d\n",h2d->GetNbinsX(),h2d->GetNbinsY());
  fprintf(fout,"RANGE_X 0 %d\n",h2d->GetNbinsX());
  fprintf(fout,"RANGE_Y 0 %d\n",h2d->GetNbinsY());
  fprintf(fout,"TITLE_X X\n");
  fprintf(fout,"TITLE_Y Y\n");
  fprintf(fout,"DATA [");

  for(int ix = 1;ix<= h2d->GetNbinsX ();ix++) {

    fprintf(fout,"[");
    for(int iy = 1;iy<= h2d->GetNbinsY ();iy++) {
      if(iy <  h2d->GetNbinsY ()) {
	fprintf(fout,"%f,",h2d->GetBinContent(iy,ix));
      } else {
	fprintf(fout,"%f",h2d->GetBinContent(iy,ix));
      }
    }

    if(ix<h2d->GetNbinsX ()) {
      fprintf(fout,"],");
    } else {
      fprintf(fout,"]");
    }
    
  }
  fprintf(fout,"]\n\n");
}

void GetECalEnergy(TFile* reco,FILE* fout,Double_t MeV_pC ) {

  TDirectory* tdir = (TDirectory*)reco->Get("ECal");
  TH2F* h2d = (TH2F*)tdir->Get("ECalCharge");

  fprintf(fout,"PLOTID ECal_Energy\n");
  fprintf(fout,"PLOTTYPE heatmap\n");
  fprintf(fout,"PLOTNAME ECal Energy map (MeV) %s\n",cfg->FormatCurrentTime());
  fprintf(fout,"CHANNELS %d %d\n",h2d->GetNbinsX (),h2d->GetNbinsY ());
  fprintf(fout,"RANGE_X 0 %d\n",h2d->GetNbinsX ());
  fprintf(fout,"RANGE_Y 0 %d\n",h2d->GetNbinsY ());
  fprintf(fout,"TITLE_X X\n");
  fprintf(fout,"TITLE_Y Y\n");
  fprintf(fout,"DATA [");

  for(int ix = 1;ix<= h2d->GetNbinsX ();ix++) {
    fprintf(fout,"[");
    for(int iy = 1;iy<= h2d->GetNbinsY ();iy++) {
      if(iy <  h2d->GetNbinsY ()) {
	fprintf(fout,"%f,",h2d->GetBinContent(iy,ix)/MeV_pC);
      } else {
	fprintf(fout,"%f",h2d->GetBinContent(iy,ix)/MeV_pC);
      }
    }

    if(ix<h2d->GetNbinsX ()) {
      fprintf(fout,"],");
    } else {
      fprintf(fout,"]");
    }
    
  }

  fprintf(fout,"]\n\n");
}
void GetTargetHistograms(TFile* reco,FILE* fout) {

  TH1F *h1d;
  TH2F *h2d;

  TDirectory* tdir = (TDirectory*)reco->Get("Target");

  //TargetBeamMultiplicity
  h1d = (TH1F*)tdir->Get("TargetBeamMultiplicity");
  if (h1d) {
  
    fprintf(fout,"PLOTID TargetBeamMultiplicity\n");
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME Target Beam Multiplicity %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetXaxis()->GetXmin(),h1d->GetXaxis()->GetXmax());
    fprintf(fout,"TITLE_X e+ multiplicity\n");
    fprintf(fout,"TITLE_Y Counts\n");
    fprintf(fout,"DATA [");
    fprintf(fout,"[");

    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");  

  }

  //TargetXYmap
  h2d = (TH2F*)tdir->Get("TargetXYmap");
  if (h2d) {
  
    fprintf(fout,"PLOTID %s\n","TargetXYmap");
    fprintf(fout,"PLOTTYPE heatmap\n");
    fprintf(fout,"PLOTNAME Target XY map %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d %d\n",h2d->GetNbinsX (),h2d->GetNbinsY ());
    fprintf(fout,"RANGE_X %f %f\n",h2d->GetXaxis()->GetBinLowEdge(1),h2d->GetXaxis()->GetBinLowEdge(h2d->GetNbinsX())+h2d->GetXaxis()->GetBinWidth(1));
    fprintf(fout,"RANGE_Y %f %f\n",h2d->GetYaxis()->GetBinLowEdge(1),h2d->GetYaxis()->GetBinLowEdge(h2d->GetNbinsX())+h2d->GetYaxis()->GetBinWidth(1));
    fprintf(fout,"TITLE_X X[mm]\n");
    fprintf(fout,"TITLE_Y Y[mm]\n");
    fprintf(fout,"DATA [");

    for(int ix = 1;ix<= h2d->GetNbinsX ();ix++) {
      fprintf(fout,"[");
      for(int iy = 1;iy<= h2d->GetNbinsY ();iy++) {
	if(iy <  h2d->GetNbinsY ()) {
	  fprintf(fout,"%f,",h2d->GetBinContent(iy,ix));
	} else {
	  fprintf(fout,"%f",h2d->GetBinContent(iy,ix));
	}
      }
      if(ix<h2d->GetNbinsX ()) {
	fprintf(fout,"],");
      } else {
	fprintf(fout,"]");
      }
    }
    fprintf(fout,"]\n\n");

  }

  //TargetBeamSpot
  h2d = (TH2F*)tdir->Get("TargetBeamSpot");
  if (h2d) {

    fprintf(fout,"PLOTID %s\n","TargetBeamSpot");
    fprintf(fout,"PLOTTYPE heatmap\n");
    fprintf(fout,"PLOTNAME TargetBeamSpot %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d %d\n",h2d->GetNbinsX (),h2d->GetNbinsY ());
    fprintf(fout,"RANGE_X %f %f\n",h2d->GetXaxis()->GetBinLowEdge(1),h2d->GetXaxis()->GetBinLowEdge(h2d->GetNbinsX())+h2d->GetXaxis()->GetBinWidth(1));
    fprintf(fout,"RANGE_Y %f %f\n",h2d->GetYaxis()->GetBinLowEdge(1),h2d->GetYaxis()->GetBinLowEdge(h2d->GetNbinsX())+h2d->GetYaxis()->GetBinWidth(1));
    fprintf(fout,"TITLE_X X[mm]\n");
    fprintf(fout,"TITLE_Y Y[mm]\n");
    fprintf(fout,"DATA [");
    for(int ix = 1;ix<= h2d->GetNbinsX ();ix++) {
      fprintf(fout,"[");
      for(int iy = 1;iy<= h2d->GetNbinsY ();iy++) {
	if(iy <  h2d->GetNbinsY ()) {
	  fprintf(fout,"%f,",h2d->GetBinContent(iy,ix));
	} else {
	  fprintf(fout,"%f",h2d->GetBinContent(iy,ix));
	}
      }
      
      if(ix<h2d->GetNbinsX ()) {
	fprintf(fout,"],");
      } else {
	fprintf(fout,"]");
      }
      
    }
    
    fprintf(fout,"]\n\n");

  }

  //TargetProfileX
  h1d = (TH1F*)tdir->Get("TargetProfileX");
  if (h1d) {

    fprintf(fout,"PLOTID TargetProfileX\n");
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME Target Profile X %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetBinLowEdge(1),h1d->GetBinLowEdge(h1d->GetNbinsX())+h1d->GetBinWidth(1));
    fprintf(fout,"TITLE_X X[mm]\n");
    fprintf(fout,"TITLE_Y MIPS/strip\n");
    fprintf(fout,"DATA [");
    fprintf(fout,"[");
    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");

  }


  //TargetProfileY
  h1d = (TH1F*)tdir->Get("TargetProfileY");
  if (h1d) {
  
    fprintf(fout,"PLOTID TargetProfileY\n");
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME Target Profile Y %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetBinLowEdge(1),h1d->GetBinLowEdge(h1d->GetNbinsX())+h1d->GetBinWidth(1));
    fprintf(fout,"TITLE_X Y[mm]\n");
    fprintf(fout,"TITLE_Y MIPS/strip\n");
    fprintf(fout,"DATA [");
    fprintf(fout,"[");
    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");

  }

  //TargetProfileLastX
  h1d = (TH1F*)tdir->Get("TargetProfileLastX");
  if (h1d) {
  
    fprintf(fout,"PLOTID TargetProfileLastX\n");
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME Target Profile Last X %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetBinLowEdge(1),h1d->GetBinLowEdge(h1d->GetNbinsX())+h1d->GetBinWidth(1));
    fprintf(fout,"TITLE_X X[mm]\n");
    fprintf(fout,"TITLE_Y MIPS/strip\n");
    fprintf(fout,"DATA [");
    fprintf(fout,"[");
    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");

  }

  //TargetProfileLastY
  h1d = (TH1F*)tdir->Get("TargetProfileLastY");
  if (h1d) {

    fprintf(fout,"PLOTID TargetProfileLastY\n");
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME Target Profile Last Y %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetBinLowEdge(1),h1d->GetBinLowEdge(h1d->GetNbinsX())+h1d->GetBinWidth(1));
    fprintf(fout,"TITLE_X Y[mm]\n");
    fprintf(fout,"TITLE_Y MIPS/strip\n");
    fprintf(fout,"DATA [");
    fprintf(fout,"[");
    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");

  }

  //TargetProfileCNSX
  h1d = (TH1F*)tdir->Get("TargetProfileCNSX");
  if (h1d) {
  
    fprintf(fout,"PLOTID TargetProfileCNSX\n");
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME Target Profile CNSX %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetBinLowEdge(1),h1d->GetBinLowEdge(h1d->GetNbinsX())+h1d->GetBinWidth(1));
    fprintf(fout,"TITLE_X X[mm]\n");
    fprintf(fout,"TITLE_Y MIPS/strip\n");
    fprintf(fout,"DATA [");
    fprintf(fout,"[");
    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");

  }

  //TargetProfileCNSY
  h1d = (TH1F*)tdir->Get("TargetProfileCNSY");
  if (h1d) {
  
    fprintf(fout,"PLOTID TargetProfileCNSY\n");
    fprintf(fout,"PLOTTYPE histo1d\n");
    fprintf(fout,"PLOTNAME Target Profile CNSY %s\n",cfg->FormatCurrentTime());
    fprintf(fout,"CHANNELS %d\n",h1d->GetNbinsX());
    fprintf(fout,"RANGE_X %f %f\n",h1d->GetBinLowEdge(1),h1d->GetBinLowEdge(h1d->GetNbinsX())+h1d->GetBinWidth(1));
    fprintf(fout,"TITLE_X Y[mm]\n");
    fprintf(fout,"TITLE_Y MIPS/strip\n");
    fprintf(fout,"DATA [");
    fprintf(fout,"[");
    for(int ix = 1;ix<= h1d->GetNbinsX ();ix++) {
      if(ix <  h1d->GetNbinsX ()) {
	fprintf(fout,"%f,",h1d->GetBinContent(ix));
      } else {
	fprintf(fout,"%f",h1d->GetBinContent(ix));
      }
    }
    fprintf(fout,"]]\n\n");

  }

}

void GetTargetWaveforms(TFile* recoFile,FILE* outFile){

  for(Int_t ch=0; ch<32; ch++) {
    Get1DHisto(recoFile,outFile,"Target",TString::Format("TargetCh%d",ch));
    Get1DHisto(recoFile,outFile,"Target",TString::Format("TargetLastCh%d",ch));
    Get1DHisto(recoFile,outFile,"Target",TString::Format("TargetNOCNSCh%d",ch));
    Get1DHisto(recoFile,outFile,"Target",TString::Format("TargetNOCNSLastCh%d",ch));
    //Get1DHisto(recoFile,outFile,"Target",TString::Format("TargetQCh%d",ch));
  }

}

void GetEvtTrend(TFile* tren, FILE* fout) {

  TGraph* tgra = (TGraph*)tren->Get("Ent");
  if(tgra == NULL) return;
 
  fprintf(fout,"PLOTID RecoEvt\n");
  fprintf(fout,"PLOTTYPE timeline\n");
  fprintf(fout,"PLOTNAME N Reco Events %s\n",cfg->FormatCurrentTime());
  fprintf(fout,"MODE [ \"lines+markers\" ]\n");
  fprintf(fout,"COLOR [ \"000000\" ]\n");
  fprintf(fout,"LEGEND [\"Reconstructed events\"]\n");
  fprintf(fout,"DATA [[");

  Double_t* xve = tgra->GetX();
  Double_t* yve = tgra->GetY();
  for(int ii = 0;ii<tgra->GetN();ii++) {
    if (ii) fprintf(fout,",");
    fprintf(fout,"[\"%f\",%f]",xve[ii],yve[ii]);
  }

  fprintf(fout,"]]\n\n");

}

void GetXYTrend(TFile* tren, FILE* fout) {

  TGraph* tgraX = (TGraph*)tren->Get("BeamX");
  if(tgraX == NULL) return;
  TGraph* tgraY = (TGraph*)tren->Get("BeamY");
  if(tgraY == NULL) return;

  fprintf(fout,"PLOTID BeamXY\n");
  fprintf(fout,"PLOTNAME Beam X and Y average %s\n",cfg->FormatCurrentTime());
  fprintf(fout,"PLOTTYPE timeline\n");
  fprintf(fout,"MODE [ \"lines+markers\", \"lines+markers\"]\n");
  fprintf(fout,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(fout,"TITLE_X timeline\n");
  fprintf(fout,"TITLE_Y [mm]\n");
  fprintf(fout,"LEGEND [\"X beam\",\"Y beam\"]\n");
  fprintf(fout,"DATA [[");

  Double_t* xve;
  Double_t* yve;

  xve = tgraX->GetX();
  yve = tgraX->GetY();
  for(int ii = 0;ii<tgraX->GetN();ii++) {
    if (ii) fprintf(fout,",");
    fprintf(fout,"[\"%f\",%f]",xve[ii],yve[ii]);
  }

  fprintf(fout,"],[");

  xve = tgraY->GetX();
  yve = tgraY->GetY();
  for(int ii = 0;ii<tgraY->GetN();ii++) {
    if (ii) fprintf(fout,",");
    fprintf(fout,"[\"%f\",%f]",xve[ii],yve[ii]);
  }

  fprintf(fout,"]]\n\n");

}

void GetSXYTrend(TFile* tren, FILE* fout) {

  TGraph* tgraX = (TGraph*)tren->Get("BeamSX");
  if(tgraX == NULL) return;
  TGraph* tgraY = (TGraph*)tren->Get("BeamSY");
  if(tgraY == NULL) return;

  fprintf(fout,"PLOTID BeamSXY\n");
  fprintf(fout,"PLOTNAME Beam X and Y RMS %s\n",cfg->FormatCurrentTime());
  fprintf(fout,"PLOTTYPE timeline\n");
  fprintf(fout,"MODE [ \"lines+markers\", \"lines+markers\"]\n");
  fprintf(fout,"COLOR [ \"ff0000\", \"0000ff\" ]\n");
  fprintf(fout,"TITLE_X timeline\n");
  fprintf(fout,"TITLE_Y [mm]\n");
  fprintf(fout,"LEGEND [\"sigma X beam\",\"sigma Y beam\"]\n");
  fprintf(fout,"DATA [[");

  Double_t* xve;
  Double_t* yve;

  xve = tgraX->GetX();
  yve = tgraX->GetY();
  for(int ii = 0;ii<tgraX->GetN();ii++) {
    if (ii) fprintf(fout,",");
    fprintf(fout,"[\"%f\",%f]",xve[ii],yve[ii]);
  }

  fprintf(fout,"],[");

  xve = tgraY->GetX();
  yve = tgraY->GetY();
  for(int ii = 0;ii<tgraY->GetN();ii++) {
    if (ii) fprintf(fout,",");
    fprintf(fout,"[\"%f\",%f]",xve[ii],yve[ii]);
  }

  fprintf(fout,"]]\n\n");

}

void GetPotTrend(TFile* tren, FILE* fout) {

  TGraph* tgra = (TGraph*)tren->Get("PotBunch");
  if(tgra == NULL) return;

  fprintf(fout,"PLOTID PotBunch\n");
  fprintf(fout,"PLOTNAME POT/pulse - target %s\n",cfg->FormatCurrentTime());
  fprintf(fout,"PLOTTYPE timeline\n");
  fprintf(fout,"MODE [ \"lines+markers\" ]\n");
  fprintf(fout,"COLOR [ \"ff0000\" ]\n");
  fprintf(fout,"TITLE_X timeline\n");
  fprintf(fout,"LEGEND [\"positrons on target\"]\n");
  fprintf(fout,"DATA [[");

  Double_t* xve = tgra->GetX();
  Double_t* yve = tgra->GetY();
  for(int ii = 0;ii<tgra->GetN();ii++) {
    if (ii) fprintf(fout,",");
    fprintf(fout,"[\"%f\",%f]",xve[ii],yve[ii]);
  }

  fprintf(fout,"]]\n\n");

}

void GetECalEtotTrend(TFile* tren, FILE* fout) {

  TGraph* hEtot = (TGraph*)tren->Get("ETotECal");
  if(hEtot == NULL) return;

  fprintf(fout,"PLOTID ETotECal\n");
  fprintf(fout,"PLOTNAME Total Energy in ECal %s\n",cfg->FormatCurrentTime());
  fprintf(fout,"PLOTTYPE timeline\n");
  fprintf(fout,"MODE [ \"lines+markers\"]\n");
  fprintf(fout,"COLOR [ \"ff0000\"]\n");
  fprintf(fout,"TITLE_X timeline\n");
  fprintf(fout,"TITLE_Y ECalEtot\n");
  fprintf(fout,"LEGEND [\"Ecal Total Energy (MeV)\"]\n");
  fprintf(fout,"DATA [[");

  Double_t* xve = hEtot->GetX();
  Double_t* yve = hEtot->GetY();
  for(int ii = 0;ii<hEtot->GetN();ii++) {
    if (ii) fprintf(fout,",");
    fprintf(fout,"[\"%f\",%f]",xve[ii],yve[ii]);
  }

  fprintf(fout,"]]\n\n");

}

int main(int argc, char* argv[])
{
  
  int c;

  TString inputFileName = "";
  TString trendFileName = "";
  TString outputDirectory = "";
  TString configFileName = "config/OnlineRecoMonitor.cfg";
  TString stopFileName = "";

  struct timespec now;

  // Parse options
  while ((c = getopt(argc, argv, "i:t:o:c:vh")) != -1) {
    switch (c)
      {
      case 'i':
	inputFileName = optarg;
	break;
      case 't':
	trendFileName = optarg;
	break;
      case 'o':
        outputDirectory = optarg;
	break;
      case 'c':
        configFileName = optarg;
	break;
      case 'v':
	cfg->SetVerbose(cfg->Verbose()+1);
        break;
      case 'h':
        fprintf(stdout,"\nOnlineRecoMonitor -i inputfile [-o outdir] [-s stopfile] [-c configfile] [-d debug] [-v] [-h]\n\n");
        fprintf(stdout,"  -i: define name of file with reconstruction histograms\n");
        fprintf(stdout,"  -t: define name of file with trend graphs\n");
        fprintf(stdout,"  -o: define directory where PadmeMonitor files will be written [default: '%s']\n",cfg->OutputDirectory().Data());
        fprintf(stdout,"  -c: define name of configuration file[default: '%s']\n",cfg->ConfigFile().Data());
        fprintf(stdout,"  -v: increase verbose level (can be repeated)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(EXIT_SUCCESS);
      case '?':
	if (optopt == 'i' || optopt == 't' || optopt == 'o' || optopt == 'c' || optopt == 'd')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        exit(EXIT_FAILURE);
      default:
        abort();
      }

  }

  // Check if input file was defined
  if (inputFileName.IsNull()) {
    fprintf (stderr,"ERROR - No input file defined with -i option.\n");
    exit(EXIT_FAILURE);
  }

  struct stat buffer;

  // Check if input file exists
  if (stat(inputFileName.Data(),&buffer) != 0) {
    fprintf (stderr,"ERROR - Input file %s does not exist.\n",inputFileName.Data());
    exit(EXIT_FAILURE);
  }

  //cfg->SetInputFileName(inputFileName);
  //printf("Reading histo file %s\n",cfg->InputFileName().Data());

  // Check if trend file was defined
  if (! trendFileName.IsNull()) {

    // Check if trend file exists
    if (stat(trendFileName.Data(),&buffer) != 0) {
      fprintf (stderr,"ERROR - Trend file %s does not exist.\n",trendFileName.Data());
      exit(EXIT_FAILURE);
    }

    //cfg->SetTrendFileName(trendFileName);
    //printf("Reading trend file %s\n",cfg->InputFileName().Data());

  }

  // Check if configuration file exists
  if (stat(configFileName.Data(),&buffer) != 0) {
    fprintf (stderr,"ERROR - Configuration file %s does not exist.\n",configFileName.Data());
    exit(EXIT_FAILURE);
  }

  // Save configuration parameters for this run
  if (! outputDirectory.IsNull()) cfg->SetOutputDirectory(outputDirectory);
  if (! configFileName.IsNull()) cfg->SetConfigFile(configFileName);
  if (! stopFileName.IsNull()) cfg->SetStopFile(stopFileName);

  // Create configuration file parser
  utl::ConfigParser* configParser = new utl::ConfigParser((const std::string)cfg->ConfigFile());
  if (cfg->Verbose()>1) configParser->Print();

  // Enable/disable detector plots according to defaults and configuration file

  Bool_t plotECal      = true;
  Bool_t plotETag      = false;
  Bool_t plotEVeto     = false;
  Bool_t plotPVeto     = false;
  Bool_t plotHEPVeto   = true;
  Bool_t plotSAC       = false;
  Bool_t plotLeadGlass = false;
  Bool_t plotTarget    = true;
  Bool_t plotTrends    = true;

  if ( configParser->HasConfig("PLOT","ECal") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","ECal") ) == 1)      { plotECal = true; }      else { plotECal = false; }
  }
  if ( configParser->HasConfig("PLOT","ETag") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","ETag") ) == 1)      { plotETag = true; }      else { plotETag = false; }
  }
  if ( configParser->HasConfig("PLOT","EVeto") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","EVeto") ) == 1)     { plotEVeto = true; }     else { plotEVeto = false; }
  }
  if ( configParser->HasConfig("PLOT","PVeto") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","PVeto") ) == 1)     { plotPVeto = true; }     else { plotPVeto = false; }
  }
  if ( configParser->HasConfig("PLOT","HEPVeto") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","HEPVeto") ) == 1)   { plotHEPVeto = true; }   else { plotHEPVeto = false; }
  }
  if ( configParser->HasConfig("PLOT","SAC") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","SAC") ) == 1)       { plotSAC = true; }       else { plotSAC = false; }
  }
  if ( configParser->HasConfig("PLOT","LeadGlass") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","LeadGlass") ) == 1) { plotLeadGlass = true; } else { plotLeadGlass = false; }
  }
  if ( configParser->HasConfig("PLOT","Target") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","Target") ) == 1)    { plotTarget = true; }    else { plotTarget = false; }
  }
  if ( configParser->HasConfig("PLOT","Trends") ) {
    if ( std::stoi(configParser->GetSingleArg("PLOT","Trends") ) == 1)    { plotTrends = true; }    else { plotTrends = false; }
  }

  if( clock_gettime(CLOCK_REALTIME,&now) == -1 ) {
    perror("- ERROR clock_gettime");
    exit(EXIT_FAILURE);
  }
  //TTimeStamp t_start = TTimeStamp(now.tv_sec,now.tv_nsec);
  printf("=== OnlineRecoMonitor starting on %s\n",cfg->FormatCurrentTime());

  // Show settings for this run
  fprintf(stdout,"- Input file: '%s'\n",inputFileName.Data());
  if (! trendFileName.IsNull()) fprintf(stdout,"- Trend file: '%s'\n",trendFileName.Data());
  fprintf(stdout,"- Output PadmeMonitor directory: '%s'\n",cfg->OutputDirectory().Data());
  fprintf(stdout,"- Configuration file: '%s'\n",cfg->ConfigFile().Data());
  if (cfg->Verbose()) fprintf(stdout,"- Verbose level: %u\n",cfg->Verbose());

  // Check if input file is ready to be processed
  TFile* recoFile = TFile::Open(inputFileName,"READ","data");
  if (recoFile == NULL) {
    printf("ERROR - Input file %s cannot be opened.\n",inputFileName.Data());
    exit(EXIT_FAILURE);
  }
  if (recoFile->IsZombie()) {
    printf("ERROR - Input file %s is a zombie.\n",inputFileName.Data());
    exit(EXIT_FAILURE);
  }
  if (recoFile->TestBit(TFile::kRecovered)) {
    printf("ERROR - Input file %s is still being written.\n",inputFileName.Data());
    exit(EXIT_FAILURE);
  }

  TString ftname,ffname; // Name of temporary and final output files
  FILE* outFile; // Output file for PadmeMonitor plots

  /*
  // Create general plots
  ftname = cfg->TmpDirectory()+"/RecoMon_General.txt";
  ffname = cfg->OutputDirectory()+"/RecoMon_General.txt";
  outFile = fopen(ftname.Data(),"w");
  if(outFile == NULL) {
    printf("ERROR - could not open temporary output file %s.\n",ftname.Data());
    exit(EXIT_FAILURE);
  }

  GetRunInfo(recoFile,outFile);

  fclose(outFile);
  if ( std::rename(ftname.Data(),ffname.Data()) ) {
    printf("ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
    exit(EXIT_FAILURE);
  }
  */

  // Create ECal plots
  if (plotECal) {

    ftname = cfg->TmpDirectory()+"/RecoMon_ECal.txt";
    ffname = cfg->OutputDirectory()+"/RecoMon_ECal.txt";
    outFile = fopen(ftname.Data(),"w");
    if(outFile == NULL) {
      printf("ERROR - could not open temporary output file %s.\n",ftname.Data());
      exit(EXIT_FAILURE);
    }

    GetECalOccupancy(recoFile,outFile);
    GetECalEnergy(recoFile,outFile,22.);
    Get2DHisto(recoFile,outFile,"ECal","ECalCharge");
    Get1DHisto(recoFile,outFile,"ECal","Etot");
    Get1DHisto(recoFile,outFile,"ECal","ECALClE");
    Get1DHisto(recoFile,outFile,"ECal","ECALTime");
    Get2DHisto(recoFile,outFile,"ECal","ECalOccupancyOffTime");

    fclose(outFile);
    if ( std::rename(ftname.Data(),ffname.Data()) ) {
      printf("ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
      exit(EXIT_FAILURE);
    }

  }

  // Create ETag plots
  if (plotETag) {;}

  // Create EVeto plots
  if (plotEVeto) {;}

  // Create PVeto plots
  if (plotPVeto) {;}

  // Create HEPVeto plots
  if (plotHEPVeto) {

    ftname = cfg->TmpDirectory()+"/RecoMon_HEPVeto.txt";
    ffname = cfg->OutputDirectory()+"/RecoMon_HEPVeto.txt";
    outFile = fopen(ftname.Data(),"w");
    if(outFile == NULL) {
      printf("ERROR - could not open temporary output file %s.\n",ftname.Data());
      exit(EXIT_FAILURE);
    }

    Get1DHisto(recoFile,outFile,"HEPVeto","HEPVetoOccupancy");

    fclose(outFile);
    if ( std::rename(ftname.Data(),ffname.Data()) ) {
      printf("ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
      exit(EXIT_FAILURE);
    }

  }

  // Create SAC plots
  if (plotSAC) {;}

  // Create LeadGlass plots
  if (plotLeadGlass) {;}

  // Create Target plots
  if (plotTarget) {

    // Create Target plots
    ftname = cfg->TmpDirectory()+"/RecoMon_Target.txt";
    ffname = cfg->OutputDirectory()+"/RecoMon_Target.txt";
    outFile = fopen(ftname.Data(),"w");
    if(outFile == NULL) {
      printf("ERROR - could not open temporary output file %s.\n",ftname.Data());
      exit(EXIT_FAILURE);
    }

    GetTargetHistograms(recoFile,outFile);
    GetTargetWaveforms(recoFile,outFile);

    fclose(outFile);
    if ( std::rename(ftname.Data(),ffname.Data()) ) {
      printf("ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
      exit(EXIT_FAILURE);
    }

  }

  // Close file with reconstructed events
  recoFile->Close();

  // If trend plots are active and trend file name was set, then create them
  if (plotTrends && ! trendFileName.IsNull()) {
    
    // Check if input file is ready to be processed
    TFile* trendFile = TFile::Open(trendFileName,"READ","data");
    if (trendFile) {
      if (! trendFile->IsZombie()) {
	if (! trendFile->TestBit(TFile::kRecovered)) {

	  // Now we can create the trend plots
	  ftname = cfg->TmpDirectory()+"/RecoMon_Trend.txt";
	  ffname = cfg->OutputDirectory()+"/RecoMon_Trend.txt";
	  outFile = fopen(ftname.Data(),"w");
	  if(outFile == NULL) {
	    printf("ERROR - could not open temporary output file %s.\n",ftname.Data());
	    exit(EXIT_FAILURE);
	  }

	  GetEvtTrend(trendFile,outFile);
	  GetXYTrend(trendFile,outFile);
	  GetSXYTrend(trendFile,outFile);
	  GetPotTrend(trendFile,outFile);
	  GetECalEtotTrend(trendFile,outFile);

	  fclose(outFile);
	  if ( std::rename(ftname.Data(),ffname.Data()) ) {
	    printf("ERROR - could not rename file from %s to %s\n",ftname.Data(),ffname.Data());
	    exit(EXIT_FAILURE);
	  }

	} else {
	  printf("WARNING - Trend file %s is still being written.\n",inputFileName.Data());
	}
      } else {
	printf("WARNING - Trend file %s is a zombie.\n",inputFileName.Data());
      }
      trendFile->Close();
    } else {
      printf("WARNING - Trend file %s cannot be opened.\n",trendFileName.Data());
    }

  }

}
