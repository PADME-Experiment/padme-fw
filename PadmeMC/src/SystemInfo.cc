#include "SystemInfo.hh"

#include <time.h>
#include <fstream>
#include <sstream>

SystemInfo* SystemInfo::fInstance = 0;

SystemInfo* SystemInfo::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new SystemInfo; }
  return fInstance;
}

void SystemInfo::timefmt_gm(char* now_str)
{
  time_t now = time(NULL);
  tm* ptm = gmtime(&now);
  strftime(now_str,20,"%Y-%m-%d %H:%M:%S",ptm);
}

void SystemInfo::timefmt_local(char* now_str)
{
  time_t now = time(NULL);
  tm* ptm = localtime(&now);
  strftime(now_str,20,"%Y-%m-%d %H:%M:%S",ptm);
}

int SystemInfo::vmsize()
{
  fCurrentVmSize = 0;
  std::ifstream memfile("/proc/self/status");
  char line[256];
  while ((memfile >> line)) {
    if (std::string(line)=="VmSize:") {
      memfile >> line;
      std::istringstream istream(line);
      istream >> fCurrentVmSize;
    }
  }
  return fCurrentVmSize;
}

int SystemInfo::store_vmsize()
{
  fCurrentVmSize = vmsize();
  fStoredVmSize = fCurrentVmSize;
  return fCurrentVmSize;
}


int SystemInfo::delta_vmsize()
{
  return fCurrentVmSize-fStoredVmSize;
}

