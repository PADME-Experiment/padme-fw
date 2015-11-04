#ifndef DBService_H
#define DBService_H

#include <vector>
#include <string>
#include <sqlite3.h>

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

 public:

  int Connect(); // Connects to current DB file
  int Disconnect(); // Disconnects from DB

  int SetDBFilePath(std::string path) { fDBFilePath = path; return 0; }
  int SetDBFilePath(char* path) { return SetDBFilePath(std::string(path)); }

  int GetBoardList(std::vector<int>&,int); // (Board list, Run nr). Returns array of board ids for given run number
  int GetFileList(std::vector<std::string>&,int,int); // (File list, Run nr, Board id). Returns array of files

  int GetFileInfo(int&,int&,unsigned int&, unsigned int&, int&, long int&, std::string); // (Version, Index, Start Time, End Time, N Events, Size, File Name). Return info about raw file

 private:

  std::string fDBFilePath;
  sqlite3* fDBHandle;
  
};
#endif
