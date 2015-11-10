#include "VHisto.hh"


VHisto::VHisto(TString name)
{
  fName = name;
}

VHisto::~VHisto()
{}

void VHisto::Reset()
{
  TIter next(fHistoList);
  while ( TObject* obj = next() ) ((TH1*)obj)->Reset();
}

Int_t VHisto::Fill1DHisto(TString name,Float_t var)
{
  TH1D* histo = (TH1D*)fHistoList->FindObject(name);
  if (histo == 0) {
    printf("WARNING - Request to fill non-existing %s 1D histo. Ignored.\n",name.Data());
    return 1;
  }
  histo->Fill(var);
  //printf("VHisto::Fill1DHisto filling %s with %f\n",name.Data(),var);
  return 0;
}

Int_t VHisto::Fill2DHisto(TString name,Float_t var1,Float_t var2)
{
  TH2D* histo = (TH2D*)fHistoList->FindObject(name);
  if (histo == 0) {
    printf("WARNING - Request to fill non-existing %s 2D histo. Ignored.\n",name.Data());
    return 1;
  }
  histo->Fill(var1,var2);
  //printf("VHisto::Fill2DHisto filling %s with %f %f\n",name.Data(),var1,var2);
  return 0;
}
