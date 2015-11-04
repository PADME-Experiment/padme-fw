#include <string.h>
#include <stdio.h>

#include "DBService.hh"

DBService* DBService::fInstance = 0;

DBService::DBService()
{
  SetDBFilePath((std::string)"");
  fDBHandle = NULL;
}

//DBService::DBService(std::string path)
//{
//  SetDBFilePath(path);
//  Connect();
//}

//DBService::DBService(char* path)
//{
//  SetDBFilePath(path);
//  Connect();
//}

DBService::~DBService()
{
  Disconnect();
}

DBService* DBService::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new DBService(); }
  return fInstance;
}

int DBService::Connect()
{
  int rc;
  if (fDBFilePath.compare("")==0) {
    printf("DBService::Connect ERROR - DB file path is not defined\n");
    return DBSERVICE_ERROR;
  }
  rc = sqlite3_open(fDBFilePath.c_str(),&fDBHandle);
  if (rc != SQLITE_OK) {
    printf("DBService::Connect ERROR - Unable to connect to '%s' database file. Error code: %d\n",fDBFilePath.c_str(),rc);
    return DBSERVICE_CONNECTERROR;
  }
  return DBSERVICE_OK;
}

int DBService::Disconnect()
{
  int rc;
  rc = sqlite3_close(fDBHandle);
  if ( rc != SQLITE_OK ) {
    printf("DBService::Disconnect ERROR - Unable to disconnect database. Error code: %d\n",rc);
    return DBSERVICE_CONNECTERROR;
  }
  return DBSERVICE_OK;
}

// Return list of files for given run number/board id
int DBService::GetFileList(std::vector<std::string>& fileList,int run_nr,int board_id)
{

  char sqlCode[10240];
  sqlite3_stmt* res;

  // Make sure the vector of file names is empty
  fileList.clear();

  int result = DBSERVICE_OK;
  strcpy(sqlCode,"SELECT raw_file.name FROM raw_file INNER JOIN process ON process.id = raw_file.process_id WHERE process.run_number = ? AND process.board_id = ?");
  if ( sqlite3_prepare_v2(fDBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
        ( sqlite3_bind_int(res, 1, run_nr)   == SQLITE_OK ) &&
        ( sqlite3_bind_int(res, 2, board_id) == SQLITE_OK )
       ) {
      int rc;
      while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
	if (rc == SQLITE_ROW) { // A row was returned: get name, convert to string, and save in vector
	  const char* name = reinterpret_cast<const char*>(sqlite3_column_text(res,0));
	  if (name == NULL) name = "";
	  fileList.push_back(std::string(name));
	} else { // Anything else is an error
	  printf("DBService::GetFileList ERROR - sqlite3_step error: %s\n", sqlite3_errmsg(fDBHandle));
	  result = DBSERVICE_SQLERROR;
	  break; // Exit the while loop
	}
      }
    } else {
      printf("DBService::GetFileList ERROR - sqlite3_bind error: %s\n", sqlite3_errmsg(fDBHandle));
      result = DBSERVICE_SQLERROR;
    }
  } else {
    printf("DBService::GetFileList ERROR - sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(fDBHandle));
    result = DBSERVICE_SQLERROR;
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) {
    printf("DBService::GetFileList ERROR - sqlite3_finalize error: %s\n", sqlite3_errmsg(fDBHandle));
    result = DBSERVICE_SQLERROR;
  }

  return result;

}

// Return list of boards for given run number
int DBService::GetBoardList(std::vector<int>& boardList,int run_nr)
{

  char sqlCode[10240];
  sqlite3_stmt* res;

  // Make sure the vector of board ids is empty
  boardList.clear();

  int result = DBSERVICE_OK;
  strcpy(sqlCode,"SELECT board_id FROM process WHERE run_number = ?");
  if ( sqlite3_prepare_v2(fDBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_int(res, 1, run_nr) == SQLITE_OK ) {
      int rc;
      while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
	if (rc == SQLITE_ROW) { // A row was returned: get board id and save in vector
	  boardList.push_back(sqlite3_column_int(res,0));
	} else { // Anything else is an error
	  printf("DBService::GetBoardList ERROR - sqlite3_step error: %s\n", sqlite3_errmsg(fDBHandle));
	  result = DBSERVICE_SQLERROR;
	  break; // Exit the while loop
	}
      }
    } else {
      printf("DBService::GetBoardList ERROR - sqlite3_bind error: %s\n", sqlite3_errmsg(fDBHandle));
      result = DBSERVICE_SQLERROR;
    }
  } else {
    printf("DBService::GetBoardList ERROR - sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(fDBHandle));
    result = DBSERVICE_SQLERROR;
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) {
    printf("DBService::GetBoardList ERROR - sqlite3_finalize error: %s\n", sqlite3_errmsg(fDBHandle));
    result = DBSERVICE_SQLERROR;
  }

  return result;

}

int DBService::GetFileInfo(int& version,int& part,unsigned int& time_open, unsigned int& time_close, int& n_events, long int& size, std::string filename)
{

  char sqlCode[10240];
  sqlite3_stmt* res;

  int n_found = 0;
  int result = DBSERVICE_OK;
  strcpy(sqlCode,"SELECT version,part,time_open,time_close,n_events,size FROM raw_file WHERE name = ?");
  if ( sqlite3_prepare_v2(fDBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_text(res, 1, filename.c_str(), -1, SQLITE_TRANSIENT) == SQLITE_OK ) {
      int rc;
      while ((rc = sqlite3_step(res)) != SQLITE_DONE) {
	if (rc == SQLITE_ROW) { // A row was returned: get board id and save in vector
	  n_found++;
	  version    = sqlite3_column_int(res,0);
	  part       = sqlite3_column_int(res,1);
	  time_open  = sqlite3_column_int(res,2);
	  time_close = sqlite3_column_int(res,3);
	  n_events   = sqlite3_column_int(res,4);
	  size       = sqlite3_column_int(res,5);
	} else { // Anything else is an error
	  printf("DBService::GetBoardList ERROR - sqlite3_step error: %s\n", sqlite3_errmsg(fDBHandle));
	  result = DBSERVICE_SQLERROR;
	  break; // Exit the while loop
	}
      }
    } else {
      printf("DBService::GetBoardList ERROR - sqlite3_bind error: %s\n", sqlite3_errmsg(fDBHandle));
      result = DBSERVICE_SQLERROR;
    }
  } else {
    printf("DBService::GetBoardList ERROR - sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(fDBHandle));
    result = DBSERVICE_SQLERROR;
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) {
    printf("DBService::GetBoardList ERROR - sqlite3_finalize error: %s\n", sqlite3_errmsg(fDBHandle));
    result = DBSERVICE_SQLERROR;
  }
  if ( result == DBSERVICE_OK ) {
    if ( n_found == 0 ) {
      printf("DBService::GetFileInfo ERROR - file %s not found in DB\n", filename.c_str());
      result = DBSERVICE_ERROR;
    } else if ( n_found > 1 ) {
      printf("DBService::GetFileInfo ERROR - file %s defined %d times in DB (?)\n", filename.c_str(),n_found);
      result = DBSERVICE_ERROR;
    }
  }

  return result;

}
