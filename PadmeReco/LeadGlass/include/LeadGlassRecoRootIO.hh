#ifndef LeadGlassRecoRootIO_h
#define LeadGlassRecoRootIO_h 1

#include "RecoVRootIO.hh"

class LeadGlassReconstruction;

class LeadGlassRecoRootIO : public RecoVRootIO
{
public:

  LeadGlassRecoRootIO();
  ~LeadGlassRecoRootIO();

  void SaveEvent();

private:

  LeadGlassReconstruction* fLGReco;

};
#endif // LeadGlassRecoRootIO_h
