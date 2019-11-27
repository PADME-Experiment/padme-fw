#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>

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

  sprintf(sqlCode,"SELECT version,part,time_open,time_close,n_events,size FROM daq_file WHERE name = '%s'",filename.c_str());
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::GetFileInfo - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  res = mysql_store_result(fDBHandle);
  if (res == NULL) {
    printf("DBService::GetFileInfo - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
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

//int DBService::GetRunEvents(int& n_events, int run_nr)
//{
//
//  char sqlCode[10240];
//  MYSQL_RES* res;
//  MYSQL_ROW row;
//
//  sprintf(sqlCode,"SELECT total_events FROM run WHERE number = %d",run_nr);
//  if ( mysql_query(fDBHandle,sqlCode) ) {
//    printf("DBService::GetRunEvents - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
//    return DBSERVICE_SQLERROR;
//  }
//
//  res = mysql_store_result(fDBHandle);
//  if (res == NULL) {
//    printf("DBService::GetRunEvents - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
//    return DBSERVICE_SQLERROR;
//  }
//
//  int result = DBSERVICE_OK;
//  if (mysql_num_rows(res) == 0) {
//    printf("DBService::GetRunEvents ERROR - run %d not found in DB\n", run_nr);
//    result = DBSERVICE_ERROR;
//  } else if (mysql_num_rows(res) == 1) {
//    row = mysql_fetch_row(res);
//    n_events   = atoi(row[0]);
//  } else {
//    printf("DBService::GetRunEvents ERROR - run %d defined %d times in DB (?)\n", run_nr,(int)mysql_num_rows(res));
//    result = DBSERVICE_ERROR;
//  }
//
//  mysql_free_result(res);
//  res = NULL;
//
//  return result;
//
//}

int DBService::UpdateMergerInfo(unsigned int n_files, unsigned int n_events, unsigned long int size, int merger_id)
{

  char sqlCode[10240];
  //MYSQL_RES* res;

  sprintf(sqlCode,"UPDATE lvl1_process SET n_raw_files = %u, total_events = %u, total_size = %lu WHERE id = %d",n_files,n_events,size,merger_id);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::UpdateMergerInfo - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  return DBSERVICE_OK;

}

int DBService::GetMergerId(int& merger_id, int run_nr)
{

  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  sprintf(sqlCode,"SELECT id FROM lvl1_process WHERE run_number = %d",run_nr);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::GetMergerId - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  res = mysql_store_result(fDBHandle);
  if (res == NULL) {
    printf("DBService::GetMergerId - ERROR retrieving result: %s\n", mysql_error(fDBHandle));
    return DBSERVICE_SQLERROR;
  }

  int result = DBSERVICE_OK;
  if (mysql_num_rows(res) == 0) {
    printf("DBService::GetMergerId - ERROR - merger not found for run %d\n", run_nr);
    result = DBSERVICE_ERROR;
  } else if (mysql_num_rows(res) == 1) {
    row = mysql_fetch_row(res);
    merger_id = atoi(row[0]);
  } else {
    printf("DBService::GetMergerId ERROR - multiple mergers (n=%d) defined for run %d (?)\n",(int)mysql_num_rows(res),run_nr);
    result = DBSERVICE_ERROR;
  }

  mysql_free_result(res);
  res = NULL;

  return result;

}

int DBService::SetMergerStatus(int status, int merger_id)
{

  char sqlCode[10240];
  //MYSQL_RES* res;

  sprintf(sqlCode,"UPDATE lvl1_process SET status = %d WHERE id = %d",status,merger_id);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::SetMergerStatus - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  return DBSERVICE_OK;

}

int DBService::SetMergerTime(std::string sel, int merger_id)
{

  char sqlCode[10240];
  //MYSQL_RES* res;

  time_t t = time(0);
  struct tm* t_st = gmtime(&t);

  if (sel.compare("START") == 0) {

    sprintf(sqlCode,"UPDATE lvl1_process SET time_start = '%04d-%02d-%02d %02d:%02d:%02d' WHERE id = %d",
	    1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,merger_id);

  } else if (sel.compare("STOP") == 0) {

    sprintf(sqlCode,"UPDATE lvl1_process SET time_stop = '%04d-%02d-%02d %02d:%02d:%02d' WHERE id = %d",
	    1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,merger_id);

  } else {

    printf("DBService::SetMergerTime ERROR - unknown time type '%s' requested for merger id %d\n",
	   sel.c_str(),merger_id);
    return DBSERVICE_ERROR;

  }

  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::SetMergerTime - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  return DBSERVICE_OK;

}

int DBService::OpenRawFile(int& file_id,int merger_id,std::string name,int part)
{

  char sqlCode[10240];
  //MYSQL_RES* res;

  time_t t = time(0);
  struct tm* t_st = gmtime(&t);
  sprintf(sqlCode,"INSERT INTO raw_file (lvl1_process_id,name,part,status,time_open,n_events,size) VALUES (%d,'%s',%d,0,'%04d-%02d-%02d %02d:%02d:%02d',0,0)",
	  merger_id,name.c_str(),part,1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);

  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::OpenRawFile - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  file_id = mysql_insert_id(fDBHandle);
  if (file_id == 0) {
    printf("DBService::OpenRawFile - ERROR unable to get file id from DB\n");
    return DBSERVICE_ERROR;
  }

  return DBSERVICE_OK;

}

int DBService::CloseRawFile(int file_id,unsigned int nevents,unsigned long int size)
{

  char sqlCode[10240];
  //MYSQL_RES* res;

  time_t t = time(0);
  struct tm* t_st = gmtime(&t);
  sprintf(sqlCode,"UPDATE raw_file SET status = 1, time_close = '%04d-%02d-%02d %02d:%02d:%02d', n_events = %u, size = %lu WHERE id = %d",
	  1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,nevents,size,file_id);
  if ( mysql_query(fDBHandle,sqlCode) ) {
    printf("DBService::CloseRawFile - ERROR executing SQL query: %s\n%s\n", mysql_error(fDBHandle),sqlCode);
    return DBSERVICE_SQLERROR;
  }

  return DBSERVICE_OK;

}
