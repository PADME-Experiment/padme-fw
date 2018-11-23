#include "TTargetFitEvent.hh"

#include "Riostream.h"

ClassImp(TTargetFitEvent)

TTargetFitEvent::TTargetFitEvent(){
    fNSetPar      = 0;
    //fFitParams    = 0;
    //fFitParams  = new TClonesArray();
    TClass* hCls = TTargetSignalFitParams::Class();
    fFitParams  = new TClonesArray(hCls,1000);
    ;
}

TTargetFitEvent::TTargetFitEvent(TClass* hCls) {
  fNSetPar = 0;
  fFitParams  = new TClonesArray(hCls,1000);
}

TTargetFitEvent::~TTargetFitEvent(){
  if (fFitParams) delete fFitParams;
}

void TTargetFitEvent::Clear(Option_t * option)
{

  fNSetPar = 0;
  if(fFitParams) fFitParams->Clear(option);

}

TTargetSignalFitParams* TTargetFitEvent::AddFitParSet()
{
  return (TTargetSignalFitParams*)(fFitParams->New(fNSetPar++));
}

TTargetSignalFitParams* TTargetFitEvent::AddFitParSet(TTargetSignalFitParams* mcPar)
{
  //  std::cout<<"Ciao in TTargetFitEvent::AddFitParSet;  pointer to the TClonesArray=<"<<fFitParams<<">"<<std::endl;
  TTargetSignalFitParams* Par = (TTargetSignalFitParams*)(fFitParams->New(fNSetPar++));
  //  std::cout<<"Ciao in TTargetFitEvent::AddFitParSet - done"<<std::endl;
  *Par = *mcPar;
  return Par;
}

TTargetSignalFitParams* TTargetFitEvent::GetFitParSet(Int_t iSetPar)
{
  return (TTargetSignalFitParams*)fFitParams->At(iSetPar);
}

TTargetSignalFitParams* TTargetFitEvent::GetLastFitParSet()
{
  return (TTargetSignalFitParams*)fFitParams->At(fNSetPar-1);
}

void TTargetFitEvent::RemoveFitParSet(Int_t iSetPar)
{
    fFitParams->RemoveAt(iSetPar);
    fFitParams->Compress();
    fNSetPar--;
}
