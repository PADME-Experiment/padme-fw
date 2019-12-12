#ifndef ECalCalibHandler_h
#define ECalCalibHandler_h

#include "TTimeStamp.h"

#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <fstream>
#include "cstdlib"

#define ECALCALIBHANDLER_NBOARD 32
#define ECALCALIBHANDLER_NCHAN 32


class ECalCalibHandler {

public :

  static ECalCalibHandler* GetInstance();
  ~ECalCalibHandler();

private:

  static ECalCalibHandler* fInstance;

protected:

  ECalCalibHandler();

public:

  // List of calib files
  void SetFileList(std::string list) {fList = list;}
  std::string GetFileList(){return fList;}

  void Initialise();

  // time interval in which the event falls
  void SetTimeInterval(TTimeStamp);

  // calibration constants for (BD_id, CH_id) from file
  void SetCalibVal(std::string);
  double GetCalibVal(int BD, int CH){return fCalibVal[BD][CH];}

private :

  std::string fList;
  TTimeStamp fCurTStart;
  TTimeStamp fCurTEnd;
  // < <start_interval,end_interval>, calib_file >
  std::map <std::pair<TTimeStamp,TTimeStamp>, std::string> fFileMap;
  double fCalibVal[ECALCALIBHANDLER_NBOARD][ECALCALIBHANDLER_NCHAN];


};
#endif
