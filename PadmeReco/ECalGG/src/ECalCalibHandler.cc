#include "ECalCalibHandler.hh"
#include "TTimeStamp.h"

#include <iostream>
#include <utility>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include "cstdlib"

ECalCalibHandler* ECalCalibHandler::fInstance = 0;

ECalCalibHandler* ECalCalibHandler::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalCalibHandler(); }
  return fInstance;
}




ECalCalibHandler::ECalCalibHandler()
{
  // default list file
  fList = "config/Calibration/ECalEnergyCalibTimeIntervals.txt";

  // set to 0 all the calib constants
  for (size_t ii = 0; ii < ECALCALIBHANDLER_NBOARD; ++ii){
    for (size_t jj = 0; jj < ECALCALIBHANDLER_NCHAN; ++jj){

      fCalibVal[ii][jj] = 0.;
    }
  }

  // set to 0 start and end point
  fCurTStart.Set(0,0,0,1,0);
  fCurTEnd.Set(0,0,0,1,0);

}




ECalCalibHandler::~ECalCalibHandler()
{;} // do I have to free fFileMap and fCalibVal?




void ECalCalibHandler::Initialise()
{

  std::string line;
  std::ifstream listfile(fList);

  if(listfile.is_open()){ // open list file and fill corresponding map
    while(getline(listfile,line)){
      int Ti_d, Ti_h, Te_d, Te_h;
      TTimeStamp Ti, Te;
      std::string file;
      std::stringstream ss(line);
      ss>>Ti_d>>Ti_h>>Te_d>>Te_h>>file;

      Ti.Set(Ti_d,Ti_h,0,1,0);
      Te.Set(Te_d,Te_h,0,1,0);
      fFileMap[std::make_pair(Ti,Te)] = file;
    }

    listfile.close();

  } else { // list file not found
    std::cout<<"\nWARNING!!! File "<<fList<<" with ECal energy calibration "
	     <<"files and corresponding time intervals does not exist."
	     <<"\nCalibration constants will be set to 0"<<std::endl;
  }

  // initialise all the calib constants to 0
  for (size_t ii = 0; ii < ECALCALIBHANDLER_NBOARD; ++ii){
    for (size_t jj = 0; jj < ECALCALIBHANDLER_NCHAN; ++jj){
      
      fCalibVal[ii][jj] = 0.;
    }
  }

}




void ECalCalibHandler::SetTimeInterval(TTimeStamp T_ev)
{

  if( (fCurTStart <= T_ev) && (T_ev <= fCurTEnd) ){ // current time interval is ok
    return;

  } else { // current time interval must be changed

    // initialise all the calib constants to 0
    for (size_t ii = 0; ii < ECALCALIBHANDLER_NBOARD; ++ii){
      for (size_t jj = 0; jj < ECALCALIBHANDLER_NCHAN; ++jj){
	
	fCalibVal[ii][jj] = 0.;
      }
    }

    // look for the new time interval
    bool found = false;
    std::map<std::pair<TTimeStamp,TTimeStamp>, std::string>::iterator FM_it;
    for(FM_it = fFileMap.begin();FM_it!=fFileMap.end();++FM_it){
      if( ((FM_it->first).first <= T_ev) && (T_ev <= (FM_it->first).second) ){

	// load calib const
	SetCalibVal(FM_it->second);


	// set new interval start and end points 
	fCurTStart = (FM_it->first).first;	
	fCurTEnd = (FM_it->first).second;

	// found interval
	found = true;

	std::cout<<"Setting ECal energy calibration file to "<<FM_it->second
		 <<". Valid from "<<fCurTStart<<" to "<<fCurTEnd<<std::endl;


	break;		
      } // end of time interval check
      
    } // end of loop on possible time intervals

    if(!found){ // time interval not found
      std::cout<<"WARNING!!! time interval for event time "<<T_ev<<" not found."
	       <<" Values are set to 0."<<std::endl;
 
    } // end of not found

  } // end of time interval changing

}




void ECalCalibHandler::SetCalibVal(std::string filename)
{

  char path[256];
  sprintf(path,"config/Calibration/%s",filename.c_str());
  std::ifstream calibfile(path);

  std::string line;
  if(calibfile.is_open()){
    while(getline(calibfile,line)){
      int row, col, BD, CH;
      double QMIP;
      std::stringstream ss(line);
      ss>> row >> col >> BD >> CH >> QMIP;

      // set the values
      fCalibVal[BD][CH] = 1./QMIP;
    }

  } else { // impossible to open calibration file
    std::cout<<"WARNING!!! Impossible to open ECal energy calibration file "<<filename
	     <<". Calibration constants will be set to 0"<<std::endl;

    // initialise all the calib constants to 0
    for (size_t ii = 0; ii < ECALCALIBHANDLER_NBOARD; ++ii){
      for (size_t jj = 0; jj < ECALCALIBHANDLER_NCHAN; ++jj){
	
	fCalibVal[ii][jj] = 0.;	  
      }
    }

  } // end of else
  
}
