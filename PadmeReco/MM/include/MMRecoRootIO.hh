#ifndef MMRecoRootIO_h
#define MMRecoRootIO_h 1

#include "RecoVRootIO.hh"

class MMReconstruction;

class MMRecoRootIO : public RecoVRootIO
{
public:

  MMRecoRootIO();
  ~MMRecoRootIO();

  void SaveEvent();

private:

  MMReconstruction* fMMReco;

};
#endif // MMRecoRootIO_h
