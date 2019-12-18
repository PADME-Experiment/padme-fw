#ifndef DBService_H
#define DBService_H

//#include <vector>
//#include <string>
//#include <mysql/mysql.h>

//// Return codes for DB service calls
//#define DBSERVICE_OK           0
//#define DBSERVICE_ERROR        1
//#define DBSERVICE_SQLERROR     2
//#define DBSERVICE_CONNECTERROR 3

// Definition of process status values in DB
#define DB_STATUS_IDLE         0
#define DB_STATUS_INITIALIZING 1
#define DB_STATUS_INIT_FAIL    2
#define DB_STATUS_INITIALIZED  3
#define DB_STATUS_ABORTED      4
#define DB_STATUS_RUNNING      5
#define DB_STATUS_RUN_FAIL     6
#define DB_STATUS_FINISHED     7
#define DB_STATUS_CLOSE_FAIL   8
#define DB_STATUS_UNKNOWN      9
/*
class DBService
{

public:

  ~DBService();
  static DBService* GetInstance();

private:

  static DBService* fInstance;

protected:

  DBService();

  int Connect(); // Connects to current DB file
  int Disconnect(); // Disconnects from DB

public:

  int GetBoardList(std::vector<int>&,int); // (Board list, Run nr). Returns array of board ids for given run number
  int GetFileList(std::vector<std::string>&,int,int); // (File list, Run nr, Board id). Returns array of files

  int GetFileInfo(int&,int&,std::string&,std::string&,int&,long int&,std::string); // (Version, Index, Start Time, End Time, N Events, Size, File Name). Return info about raw file

  // Store number of events written and size of output file to DB
  int UpdateMergerInfo(unsigned int,unsigned int,unsigned long int,int); // (N Files, N Events, Size, Merger Id).

  int GetMergerId(int&,int); // (Merger id, Run nr). Returns id of merger for this run

  int SetMergerStatus(int,int); // (Status, Merger Id)
  int SetMergerTime(std::string,int); // ("START"|"STOP", Merger Id)

  int OpenRawFile(int&,int,std::string,int); // (File id, Merger id, Filename, Part)
  int CloseRawFile(int,unsigned int,unsigned long int); // (File id,N Events,Size)

private:

  MYSQL* fDBHandle;

};
*/
#endif
