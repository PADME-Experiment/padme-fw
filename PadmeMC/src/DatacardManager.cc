#include "DatacardManager.hh"

DatacardManager* DatacardManager::fInstance = 0;

DatacardManager::DatacardManager():fFileName("default.root")
{}

DatacardManager::~DatacardManager()
{
  delete fDatacardMessenger;
}

DatacardManager* DatacardManager::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new DatacardManager(); }
  return fInstance;
}

void DatacardManager::SetMessenger()
{
  fDatacardMessenger = new DatacardMessenger(this);
}
