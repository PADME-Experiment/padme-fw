#ifndef OfflineServer_h
#define OfflineServer_h 1

#include <vector>
#define MAXRUNNR 500000
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#define kTimeBit 0;// bit to define availability of run start and stop times
#define kEnergy 1;// bit to define availability of run DHSTB01 and 02 energies
#define kNFiles 2;// bit to define availability of run number of files
#define kPOT 3;// bit to define availability of run nominal POT
#define kBunchLength 4;// bit to define availability of run bunch length and start time
#define kTargetAvg 5;// bit to define availability of run beam-average position at target
#define kCOG 6;// bit to define availability of run beam-average position at ECal
#define kCalibEnergy 7;// bit to define availability of run energy calibration factor
#define kCalibTimeEnergy 8;// bit to define availability of run energy time calibration factor


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
  float calibEnergyFactor; //a.u. calibration constant to be multiplied to the cluster energy, so to obtain the corrected energy
  float calibTimeEnergyFactor; //a.u. calibration factor to be multiplied to the fractional time of the cluster with respect to the burst start, so to obtain the corrected energy
  int retrieveStatus; // word with bits to signal availability of information
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
  float getCalibEnergyFactor(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).calibEnergyFactor;} //a.u.
  float getCalibTimeEnergyFactor(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).calibTimeEnergyFactor;} //a.u.

  bool isTimeAvailable(int runID){       if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kTimeBit);} 
  bool isEnergyAvailable(int runID){     if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kEnergy);} 
  bool isNFilesAvailable(int runID){     if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kNFiles);} 
  bool isPOTAvailable(int runID){        if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kPOT);} 
  bool isBunchLengthAvailable(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kBunchLength);} 
  bool isTargetAvgAvailable(int runID){  if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kTargetAvg);} 
  bool isCOGAvailable(int runID){        if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kCOG);} 
  bool isCalibEnergyAvailable(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kCalibEnergy);} 
  bool isCalibTimeEnergyAvailable(int runID){if (runIndex[runID] == -1) {return 0;} return runInfos.at(runIndex[runID]).retrieveStatus & (1<<kCalibTimeEnergy);} 

private:
  int* runIndex; // array to pass from RunID to the index of the vector of runInfo structures
  std::vector<RunInfo> runInfos; // vector of runInfo structures
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
