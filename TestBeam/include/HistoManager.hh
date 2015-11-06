#ifndef HistoManager_h
#define HistoManager_h

#include "VHisto.hh"

class HistoManager {

public:

  ~HistoManager();
  static HistoManager* GetInstance();

  void Reset();

  VHisto* Histo(TString);

private:

  static HistoManager* fInstance;

protected:

  HistoManager();

  std::vector<VHisto*> fHistoList;

};
#endif
