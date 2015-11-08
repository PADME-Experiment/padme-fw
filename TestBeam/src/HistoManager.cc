#include "HistoManager.hh"

#include "ECALHisto.hh"
#include "TargetHisto.hh"

HistoManager* HistoManager::fInstance = 0;

HistoManager::HistoManager()
{

  // Create ECAL histos
  fHistoList.push_back(new ECALHisto());

  // Create Target histos
  fHistoList.push_back(new TargetHisto());

}

HistoManager::~HistoManager()
{
  for ( std::vector<VHisto*>::iterator it = fHistoList.begin() ; it != fHistoList.end(); ++it )
    delete *it;
}

HistoManager* HistoManager::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HistoManager(); }
  return fInstance;
}

void HistoManager::Reset()
{
  for ( std::vector<VHisto*>::iterator it = fHistoList.begin() ; it != fHistoList.end(); ++it ) (*it)->Reset();
}

VHisto* HistoManager::Histo(TString name)
{
  for ( std::vector<VHisto*>::iterator it = fHistoList.begin() ; it != fHistoList.end(); ++it ) {
    if ((*it)->GetName() == name) return *it;
  }
  return 0;
}
