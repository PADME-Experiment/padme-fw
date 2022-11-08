#ifndef OfflineServer_h
#define OfflineServer_h 1

#include <vector>
#define MAXRUNNR 500000
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


struct RunInfo{
  int RunID;
  long int runTime; //seconds?
  float DHSTB01Energy; //MeV
  float DHSTB02Energy; //MeV
  int nFiles;
  double runPOT; //1E9
  float bunchLength; //ns
  float beamStart; //ns
};
  
class OfflineServer
{

protected:
  OfflineServer();
public:
  ~OfflineServer();
  static OfflineServer* GetInstance();
private:
  static OfflineServer* fInstance;

public:

  long int getRunTime(int runID){return runInfos.at(runIndex[runID]).runTime;} //seconds?
  float getDHSTB01Energy(int runID){return runInfos.at(runIndex[runID]).DHSTB01Energy;} //MeV
  float getDHSTB02Energy(int runID){return runInfos.at(runIndex[runID]).DHSTB02Energy;} //MeV
  int getNFiles(int runID){return runInfos.at(runIndex[runID]).nFiles;}
  double getRunPOT(int runID){return runInfos.at(runIndex[runID]).runPOT;} //1E9
  float getBunchLength(int runID){return runInfos.at(runIndex[runID]).bunchLength;} //ns
  float getBeamStart(int runID){return runInfos.at(runIndex[runID]).beamStart;} //ns
  
private:
  int* runIndex; // array to pass from RunID to the index of the vector of runInfo structures
  std::vector<RunInfo> runInfos; // vector of runInfo structures
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
