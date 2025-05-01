#include "PadmeVNamedModule.hh"

PadmeVNamedModule::PadmeVNamedModule(TString name) : fName(name)
{;}

PadmeVNamedModule* PadmeVNamedModule::Find(TString Name)
{
  for(UInt_t iLib = 0; iLib < fLibrary.size(); iLib++)
    if(fLibrary[iLib]->GetName().CompareTo(Name) == 0)
      return fLibrary[iLib];
  return 0;
}
