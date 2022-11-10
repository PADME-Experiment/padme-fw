#include "OfflineServer.hh"
#include <iostream>
#include <fstream>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OfflineServer* OfflineServer::fInstance = 0;
OfflineServer* OfflineServer::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new OfflineServer(); }
  return fInstance;
}

OfflineServer::OfflineServer()
{
  std::cout << "Opening input OfflineDB file" << std::endl;
  // open input file
  std::ifstream inputDBFile("/home/spadaro/PADME/UserAnalysis/config/run_dhstb.txt");

  if(!inputDBFile) { // file couldn't be opened
    std::cerr << "Error: file could not be opened" << std::endl;
    exit(1);
  }
  runInfos.clear();
  runIndex = new int[MAXRUNNR];
  for (int i=0; i<MAXRUNNR; i++) runIndex[i] = -1;

  RunInfo rInfo;

  while ( !inputDBFile.eof() ) { // keep reading until end-of-file
    inputDBFile >> 
      rInfo.RunID >> rInfo.runTime >> rInfo.DHSTB01Energy >> 
      rInfo.DHSTB02Energy >> rInfo.nFiles >> rInfo.runPOT >> 
      rInfo.bunchLength >> rInfo.beamStart ; // sets EOF flag if no value found

  // populate structure
    runInfos.push_back(rInfo);
  // fill runIndex array
    runIndex[rInfo.RunID] = runInfos.size()-1;
  }
  
  inputDBFile.close();
  std::cout << "End-of-file OfflineDB reached.." << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OfflineServer::~OfflineServer(){
  delete runIndex;
}