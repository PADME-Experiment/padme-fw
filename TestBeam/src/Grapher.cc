#include "Grapher.hh"

#include "TEveManager.h"
#include "TEveBrowser.h"

#include "ECALOnlineMonitor.hh"

Grapher* Grapher::fInstance = 0;

Grapher::Grapher()
{

  // Create main EVE window with no interactive section
  TEveManager::Create(kTRUE,"VF");
  gEve->GetBrowser()->HideBottomTab();

  // Create tab for ECAL
  fOnlineMonitorList.push_back(new ECALOnlineMonitor());

}

Grapher::~Grapher()
{
  for ( std::vector<VOnlineMonitor*>::iterator it = fOnlineMonitorList.begin() ; it != fOnlineMonitorList.end(); ++it )
    delete *it;
}

Grapher* Grapher::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new Grapher(); }
  return fInstance;
}

VOnlineMonitor* Grapher::OnlineMonitor(TString name)
{
  for ( std::vector<VOnlineMonitor*>::iterator it = fOnlineMonitorList.begin() ; it != fOnlineMonitorList.end(); ++it ) {
    if ((*it)->GetName() == name) return *it;
  }
  return 0;
}
