// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-17
//
// --------------------------------------------------------------

#include "TSACMCHit.hh"

#include <iostream>
#include <algorithm>

ClassImp(TSACMCHit)

TSACMCHit::TSACMCHit() : TMCVHit()
{
  // Initialize histo limits to somewhat reasonable values
  fTHistoStart = 10.; // Start at 10ns, i.e. 3m from target
  fTHistoStep  = 0.1; // 100ps x 400bins = 40ns, i.e. 1 bunch length
  ResetTHisto();

  //fEnergyHisto = 0;
}

TSACMCHit::~TSACMCHit()
{
  //if (fEnergyHisto) delete fEnergyHisto;
}

void TSACMCHit::ResetTHisto()
{
  for(Int_t i=0;i<TSACMCHIT_N_BINS;i++) fTHisto[i] = 0.;
}

void TSACMCHit::PrintTHisto()
{
  std::cout << "NBins " << GetTHistoNBins() << " Start " << GetTHistoStart() << " End " << GetTHistoEnd() << " Step " << GetTHistoStep() << std::endl;
  for(Int_t i=0;i<TSACMCHIT_N_BINS;i++)
    printf("%3.3d %9.3f\n",i,fTHisto[i]);
}

/*
void TSACMCHit::CreateEnergyHisto(Int_t n_bin, Double_t t_start, Double_t t_end)
{
  if (fChannelId == -1) {
    printf("TSACMCHit::CreateEnergyHisto ERROR: Channel Id was not set\n");
    return;
  }
  char name[8], title[26];
  sprintf(name,"SACMCE%2.2d",fChannelId);
  sprintf(title,"SAC MC Energy - Channel %2.2d",fChannelId);
  //fEnergyHisto = new TH1D(name,title,n_bin,t_start,t_end);
  fEnergyHisto.Reset();
  fEnergyHisto.SetNameTitle(name,title);
  fEnergyHisto.SetBins(n_bin,t_start,t_end);
}
*/

void TSACMCHit::AddEnergyAtTime(Double_t e, Double_t t)
{

  //if (!fEnergyHisto) {
  //  printf("TSACMCHit::AddEnergyAtTime ERROR: Energy histogram was not created\n");
  //  return;
  //}
  //fEnergyHisto->Fill(t,e);

  //fEnergyHisto.Fill(t,e);

  // Bin 0 and TSACMCHIT_N_BINS-1 (399) act as under- and over-flow counters
  Int_t nbin = std::max(0,std::min(TSACMCHIT_N_BINS-1,(int)((t-fTHistoStart)/fTHistoStep)));
  fTHisto[nbin] += e;
}

Double_t TSACMCHit::GetEnergyAtTime(Double_t t)
{
  Int_t nbin = std::max(0,std::min(TSACMCHIT_N_BINS-1,(int)((t-fTHistoStart)/fTHistoStep)));
  return fTHisto[nbin];
}
