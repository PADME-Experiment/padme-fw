#ifndef OfflineServer_h
#define OfflineServer_h 1

#include <vector>
#define MAXRUNNR 500000
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


struct RunInfo{
  int RunID;
  long long int runStartTime; //run start time in seconds UTC
  long long int runStopTime;  //run end time in seconds UTC
  float DHSTB01Energy; //MeV
  float DHSTB02Energy; //MeV
  int nFiles; // number of files for the run
  double runPOT; //number of POTs, rough evaluation
  float bunchLength; //ns, still dummy
  float beamStart; //ns, still dummy
  float tgx; // mm, average x position at target
  float tgy; // mm, average y position at target
  float cogx; // mm, average x position of COG at ECal
  float cogy; // mm, average y position of COG at ECal
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
  long long int getRunStartTime(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).runStartTime;} //seconds
  long long int getRunStopTime(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).runStopTime;} //seconds
  float getDHSTB01Energy(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).DHSTB01Energy;} //MeV
  float getDHSTB02Energy(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).DHSTB02Energy;} //MeV
  int getNFiles(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).nFiles;}
  double getRunPOT(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).runPOT;} //1E9
  float getBunchLength(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).bunchLength;} //ns
  float getBeamStart(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).beamStart;} //ns
  float getTargetXAvg(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).tgx;} //mm
  float getTargetYAvg(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).tgy;} //mm
  float getCOGXAvg(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).cogx;} //mm
  float getCOGYAvg(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).cogy;} //mm

  
private:
  int* runIndex; // array to pass from RunID to the index of the vector of runInfo structures
  std::vector<RunInfo> runInfos; // vector of runInfo structures
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
