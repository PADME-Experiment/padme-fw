#ifndef DBService_H
#define DBService_H

#include <vector>
#include <string>
#include <mysql/mysql.h>

#define DBSERVICE_OK           0
#define DBSERVICE_ERROR        1
#define DBSERVICE_SQLERROR     2
#define DBSERVICE_CONNECTERROR 3

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

  int GetRunEvents(int&,int); // (N Events, Run nr). Returns number of events currently associated to run
  int UpdateRunEvents(int,int); // (N Events, Run nr). Store number of events associated to run

 private:

  MYSQL* fDBHandle;

};
#endif
