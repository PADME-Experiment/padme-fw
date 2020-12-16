#ifndef EventCopier_H
#define EventCopier_H

class Configuration;

class TRawEvent;

class EventCopier
{

public:

  EventCopier();
  ~EventCopier();

public:

  Int_t Initialize();
  Int_t Finalize();

  Int_t CopyEvent(TRawEvent*, TRawEvent*);

private:

  // Connection to configuration class
  Configuration* fConfig;

};
#endif
