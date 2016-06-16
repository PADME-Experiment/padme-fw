#ifndef PadmeVNamedModule_H
#define PadmeVNamedModule_H

#include "TString.h"

class PadmeVNamedModule
{

public:
  
  PadmeVNamedModule(TString);
  ~PadmeVNamedModule(){};
  PadmeVNamedModule* Find(TString);

public:

  TString GetName() { return fName; };
  void    SetName(TString value) { fName = value; };
 
protected:

  TString fName;

  typedef std::vector<PadmeVNamedModule*> Library;
  Library fLibrary;

};
#endif
