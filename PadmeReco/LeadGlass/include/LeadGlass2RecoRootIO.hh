#ifndef LeadGlass2RecoRootIO_h
#define LeadGlass2RecoRootIO_h 1

#include "RecoVRootIO.hh"

class LeadGlassReconstruction;

class LeadGlass2RecoRootIO : public RecoVRootIO
{
public:

  LeadGlass2RecoRootIO();
  ~LeadGlass2RecoRootIO();

  void SaveEvent();

private:

  LeadGlassReconstruction* fLGReco;

};
#endif // LeadGlass2RecoRootIO_h
