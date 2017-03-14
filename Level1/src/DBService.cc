#include <string.h>
#include <stdio.h>
#include <cstdlib>

#include "DBService.hh"

DBService* DBService::fInstance = 0;

DBService::DBService()
{
  fDBHandle = NULL;
  Connect();
}

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

  // Check if this is the first time we are called
  if (fDBHandle != NULL) return DBSERVICE_OK;

  // Create handle to MySQL
  fDBHandle = mysql_init(NULL);
  if (fDBHandle == NULL) {
    printf("Unable to connect to MySQL database server.\n");
    return DBSERVICE_CONNECTERROR;
  }

  // Get MySQL server connection parameters from environment variables
  printf("Reading env variables\n");
  char* db_host = getenv("PADME_DB_HOST");
  if (db_host == NULL) db_host = (char*)"localhost";
  char* db_user = getenv("PADME_DB_USER");
  if (db_user == NULL) db_user = (char*)"padme";
  char* db_passwd = getenv("PADME_DB_PASSWD");
  if (db_passwd == NULL) db_passwd = (char*)"unknown";
  char* db_name = getenv("PADME_DB_NAME");
  if (db_name == NULL) db_name = (char*)"PadmeDB";
  int db_port = 5501;
  char* db_port_s = getenv("PADME_DB_PORT");
  if (db_port_s != NULL) db_port = atoi(db_port_s);
  //printf("host %s user %s passwd %s name %s port %d\n",db_host,db_user,db_passwd,db_name,db_port);

  // Connect to MySQL server
  if (mysql_real_connect(fDBHandle,db_host,db_user,db_passwd,db_name,db_port,NULL,0) == NULL) {
    fprintf(stderr, "%s\n", mysql_error(fDBHandle));
    mysql_close(fDBHandle);
    return DBSERVICE_CONNECTERROR;
  }  

  printf("- Connected to MySQL database server\n");
  return DBSERVICE_OK;

}

int DBService::Disconnect()
{
  mysql_close(fDBHandle);
  printf("- Disconnected from database server\n");
  return DBSERVICE_OK;
}

// Return list of files for given run number/board id
int DBService::GetFileList(std::vector<std::string>& fileList,int run_nr,int board_id)
{

  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  // Make sure the vector of file names is empty
  fileList.clear();

  sprintf(sqlCode,"SELECT daq_file.name FROM daq_file INNER JOIN process ON process.id = daq_file.process_id WHERE process.run_number = %d AND process.board_id = %d",run_nr,board_id);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::GetFileList - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  res = mysql_store_result(fDBHandle);
  if (res == NULL) {
    printf("DBService::GetFileList - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
    return DBSERVICE_SQLERROR;
  }

  while ( (row = mysql_fetch_row(res)) != NULL ) {
    if (row[0] != NULL) {
      fileList.push_back(std::string(row[0]));
    } else {
      printf("DBService::GetFileList - WARNING found daq_file with NULL name\n");
    }
  }

  mysql_free_result(res);
  res = NULL;

  return DBSERVICE_OK;

}

// Return list of boards for given run number
int DBService::GetBoardList(std::vector<int>& boardList,int run_nr)
{

  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  // Make sure the vector of board ids is empty
  boardList.clear();

  sprintf(sqlCode,"SELECT board_id FROM process WHERE run_number = %d",run_nr);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::GetBoardList - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  res = mysql_store_result(fDBHandle);
  if (res == NULL) {
    printf("DBService::GetBoardList - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
    return DBSERVICE_SQLERROR;
  }

  while ( (row = mysql_fetch_row(res)) != NULL ) {
    if (row[0] != NULL) {
      boardList.push_back(atoi(row[0]));
    } else {
      printf("DBService::GetBoardList - WARNING: found process with NULL board_id\n");
    }
  }

  mysql_free_result(res);
  res = NULL;

  return DBSERVICE_OK;

}

int DBService::GetFileInfo(int& version,int& part,std::string& time_open, std::string& time_close, int& n_events, long int& size, std::string filename)
{

  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  sprintf(sqlCode,"SELECT version,part,time_open,time_close,n_events,size FROM raw_file WHERE name = '%s'",filename.c_str());
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::GetBoardList - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  res = mysql_store_result(fDBHandle);
  if (res == NULL) {
    printf("DBService::GetBoardList - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
    return DBSERVICE_SQLERROR;
  }

  int result = DBSERVICE_OK;
  if (mysql_num_rows(res) == 0) {
    printf("DBService::GetFileInfo ERROR - file %s not found in DB\n", filename.c_str());
    result = DBSERVICE_ERROR;
  } else if (mysql_num_rows(res) == 1) {
    row = mysql_fetch_row(res);
    version    = atoi(row[0]);
    part       = atoi(row[1]);
    time_open  = row[2];
    time_close = row[3];
    n_events   = atoi(row[4]);
    size       = atoi(row[5]);
  } else {
    printf("DBService::GetFileInfo ERROR - file %s defined %d times in DB (?)\n", filename.c_str(),(int)mysql_num_rows(res));
    result = DBSERVICE_ERROR;
  }

  mysql_free_result(res);
  res = NULL;

  return result;

}

int DBService::GetRunEvents(int& n_events, int run_nr)
{

  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  sprintf(sqlCode,"SELECT total_events FROM run WHERE number = %d",run_nr);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::GetRunEvents - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  res = mysql_store_result(fDBHandle);
  if (res == NULL) {
    printf("DBService::GetRunEvents - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
    return DBSERVICE_SQLERROR;
  }

  int result = DBSERVICE_OK;
  if (mysql_num_rows(res) == 0) {
    printf("DBService::GetRunEvents ERROR - run %d not found in DB\n", run_nr);
    result = DBSERVICE_ERROR;
  } else if (mysql_num_rows(res) == 1) {
    row = mysql_fetch_row(res);
    n_events   = atoi(row[0]);
  } else {
    printf("DBService::GetRunEvents ERROR - run %d defined %d times in DB (?)\n", run_nr,(int)mysql_num_rows(res));
    result = DBSERVICE_ERROR;
  }

  mysql_free_result(res);
  res = NULL;

  return result;

}

int DBService::UpdateRunEvents(int n_events, int run_nr)
{

  char sqlCode[10240];
  MYSQL_RES* res;

  sprintf(sqlCode,"UPDATE run SET total_events = %d WHERE number = %d",n_events,run_nr);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::UpdateRunEvents - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  res = mysql_store_result(fDBHandle);
  if (res == NULL) {
    printf("DBService::UpdateRunEvents - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
    return DBSERVICE_SQLERROR;
  }

  int result = DBSERVICE_OK;
  if (mysql_num_rows(res) == 0) {
    printf("DBService::UpdateRunEvents ERROR - run %d not found in DB\n", run_nr);
    result = DBSERVICE_ERROR;
  } else if (mysql_num_rows(res) != 1) {
    printf("DBService::UpdateRunEvents ERROR - run %d defined %d times in DB (?)\n", run_nr,(int)mysql_num_rows(res));
    result = DBSERVICE_ERROR;
  }

  mysql_free_result(res);
  res = NULL;

  return result;

}
