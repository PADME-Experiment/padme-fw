#ifndef Grapher_h
#define Grapher_h

#include "VOnlineMonitor.hh"

class Grapher {

public:

  ~Grapher();
  static Grapher* GetInstance();

  VOnlineMonitor* OnlineMonitor(TString);

private:

  static Grapher* fInstance;

protected:

  Grapher();

  std::vector<VOnlineMonitor*> fOnlineMonitorList;

};
#endif
