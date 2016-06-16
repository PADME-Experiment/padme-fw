#ifndef DatacardManager_H
#define DatacardManager_H 1

#include "globals.hh"

#include "DatacardMessenger.hh"

class DatacardManager
{
public:

  virtual ~DatacardManager();
  static DatacardManager* GetInstance();

private:

  static DatacardManager* fInstance;
  DatacardMessenger* fDatacardMessenger;

protected:

  DatacardManager();

public:

  void     SetMessenger();

  G4String GetOutputFileName()             { return fOutputFileName; }
  void     SetOutputFileName(G4String val) { fOutputFileName = val;  }

  G4String GetHistoFileName()             { return fHistoFileName; }
  void     SetHistoFileName(G4String val) { fHistoFileName = val;  }

private:

  G4String fOutputFileName;
  G4String fHistoFileName;

};
#endif // DatacardManager_H
