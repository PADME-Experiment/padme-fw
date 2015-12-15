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
  G4String GetOutputFileName()             { return fFileName; }
  void     SetOutputFileName(G4String val) { fFileName = val;  }

private:

  G4String      fFileName;

};
#endif // DatacardManager_H
