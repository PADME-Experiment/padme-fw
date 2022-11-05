#include "PadmePerfUtils.hh"
#include <unistd.h>
#include <sys/types.h>
#include <sstream>
#include <fstream>
int PadmePerfUtils::getMem() {
#ifndef __APPLE__
  int memSize = 0;
 {
   pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/status";
    std::ifstream memfile(procstream.str().c_str());
   
   
    char line[256];
    while ((memfile >> line)) {
      if (std::string(line)=="VmSize:") {
        memfile >> line;
        std::istringstream istream(line);
        istream >> memSize;
      }
    }
  }
 #endif
  return memSize;
}

int PadmePerfUtils::getCpu() {

#ifndef __APPLE__
  pid_t pid = getpid();
    std::ostringstream procstream;
    procstream << "/proc/" << pid << "/stat";
    std::ifstream memfile(procstream.str().c_str());

    int nblancks=0;
    int uTime = 0;
    int sTime = 0;

    char line[256];
    while ((memfile >> line)) {
        nblancks++;

        if (nblancks==13) {
            memfile >> line;
            std::istringstream istream(line);
            istream >> uTime;

            memfile >> line;
            std::istringstream istream1(line);
            istream1 >> sTime;

            return uTime+sTime;
        }
    }   
#endif
    return 0;
}

int PadmePerfUtils::getRunTime() {
#ifndef __APPLE__
  // Get current time in ticks-since-boot
  FILE *procuptime;
  int sec, ssec;
  procuptime = fopen("/proc/uptime", "r");
  fscanf(procuptime, "%d.%ds", &sec, &ssec);
  fclose(procuptime);
  int sinceboot = sec*100+ssec;

  // Get process start time in ticks-since-boot
  pid_t pid = getpid();
  std::ostringstream procstream;
  procstream << "/proc/" << pid << "/stat";
  std::ifstream memfile(procstream.str().c_str());

  int nblancks=0;
  unsigned int start_time;

  char line[256];
  while ((memfile >> line)) {

    nblancks++;
    if (nblancks==21) {

      memfile >> line;
      std::istringstream istream(line);
      istream >> start_time;

      // Return total process run time in ticks
      return sinceboot-start_time;

    }
  }   
#endif
  return 0;
}
