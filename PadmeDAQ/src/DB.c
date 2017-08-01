#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <mysql/mysql.h>

#include "DB.h"

MYSQL* DBHandle = NULL;

// Create a connection to the SQLite database
int db_init()
{

  const char* db_host;
  const char* db_user;
  const char* db_passwd;
  const char* db_name;
  unsigned int db_port;

  // Check if this is the first time we are called
  if (DBHandle != NULL) return DB_OK;

  // Create handle to MySQL
  DBHandle = mysql_init(NULL);
  if (DBHandle == NULL) {
    printf("Unable to connect to MySQL database server.\n");
    return DB_CONNECTERROR;
  }

  // Get MySQL server connection parameters from environment variables
  printf("Reading env variables\n");
  db_host = getenv("PADME_DB_HOST");
  if (db_host == NULL) db_host = "localhost";
  db_user = getenv("PADME_DB_USER");
  if (db_user == NULL) db_user = "padme";
  db_passwd = getenv("PADME_DB_PASSWD");
  if (db_passwd == NULL) db_passwd = "unknown";
  db_name = getenv("PADME_DB_NAME");
  if (db_name == NULL) db_name = "PadmeDB";
  db_port = 5501;
  if (getenv("PADME_DB_PORT")) db_port = atoi(getenv("PADME_DB_PORT"));
  //printf("host %s user %s passwd %s name %s port %d\n",db_host,db_user,db_passwd,db_name,db_port);

  // Connect to MySQL server
  if (mysql_real_connect(DBHandle,db_host,db_user,db_passwd,db_name,db_port,NULL,0) == NULL) {
    fprintf(stderr, "%s\n", mysql_error(DBHandle));
    mysql_close(DBHandle);
    return DB_CONNECTERROR;
  }  

  printf("- Connected to MySQL database server\n");
  return DB_OK;
}

int db_end()
{
  mysql_close(DBHandle);
  printf("- Disconnected from database server\n");
  return DB_OK;
}

// Check if run already exists (0: no, 1: yes, -1: error)
int db_run_check(int run_nr)
{

  int result;
  char sqlCode[10240];
  MYSQL_RES* res;

  sprintf(sqlCode,"SELECT number FROM run WHERE number = %d",run_nr);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_run_check - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return -1;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_run_check - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return -1;
  }

  if (mysql_num_rows(res) == 0) {
    result = 0;
  } else if (mysql_num_rows(res) == 1) {
    result = 1;
  } else {
    printf("DB::db_run_check - ERROR multiple runs with same number (?)\n");
    result = -1;
  }

  mysql_free_result(res);
  res = NULL;

  return result;

}

// Create a new process in the database
// N.B. Default process status is 0 (i.e. DAQ not started)
int db_process_create(int run_nr,int board_id)
{

  char sqlCode[10240];

  // Insert run into DB and initialize to 0 all parameters
  //sprintf(sqlCode,"INSERT INTO process (run_number,board_id,status,time_start,time_stop,n_daq_files,total_events,total_size) VALUES (%d,%d,0,0,0,0,0,0)",run_nr,board_id);
  sprintf(sqlCode,"INSERT INTO process (run_number,board_id,status,n_daq_files,total_events,total_size) VALUES (%d,%d,0,0,0,0)",run_nr,board_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_process_create - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  return DB_OK;

}

// Get process id from run_number and board_id (0: not found, -1: error)
int db_get_process_id(int run_nr,int board_id)
{

  int result = 0;
  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  sprintf(sqlCode,"SELECT id FROM process WHERE run_number = %d AND board_id = %d",run_nr,board_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_get_process_id - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return -1;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_get_process_id - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return -1;
  }

  if (mysql_num_rows(res) == 0) {
    result = 0;
  } else if (mysql_num_rows(res) == 1) {
    row = mysql_fetch_row(res);
    result = atoi(row[0]);
  } else {
    printf("DB::db_get_process_id - ERROR multiple processes with same (run,board) pair (?)\n");
    result = -1;
  }

  mysql_free_result(res);
  res = NULL;

  return result;
}

int db_process_open(int proc_id,time_t t)
{

  struct tm* t_st;
  char sqlCode[10240];

  t_st = localtime(&t);
  sprintf(sqlCode,"UPDATE process SET status = 1, time_start = '%04d-%02d-%02d %02d:%02d:%02d' WHERE id = %d",
	  1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_process_open - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  return DB_OK;

}

int db_process_close(int proc_id,time_t t)
{

  struct tm* t_st;
  char sqlCode[10240];

  t_st = localtime(&t);
  sprintf(sqlCode,"UPDATE process SET status = 2, time_stop = '%04d-%02d-%02d %02d:%02d:%02d' WHERE id = %d",
	  1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_process_close - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  return DB_OK;

}

// Check if file already exists (0: no, 1: yes, -1: error)
int db_file_check(char* file_name)
{

  int result;
  char sqlCode[10240];
  MYSQL_RES* res;

  sprintf(sqlCode,"SELECT name FROM daq_file WHERE name = '%s'",file_name);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_run_check - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return -1;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_run_check - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return -1;
  }

  if (mysql_num_rows(res) == 0) {
    result = 0;
  } else if (mysql_num_rows(res) == 1) {
    result = 1;
  } else {
    printf("DB::db_run_check - ERROR multiple runs with same number (?)\n");
    result = -1;
  }

  mysql_free_result(res);
  res=NULL;

  return result;
  
}
 
int db_file_open(char* file_name,int file_version,time_t t,int proc_id,int part)
{

  struct tm* t_st;
  char sqlCode[10240];

  t_st = localtime(&t);
  sprintf(sqlCode,"INSERT INTO daq_file (name,version,process_id,part,time_open,n_events,size) VALUES ('%s',%d,%d,%d,'%04d-%02d-%02d %02d:%02d:%02d',0,0)",file_name,file_version,proc_id,part,1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_file_open - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  // Update file counter for this process
  sprintf(sqlCode,"UPDATE process SET n_daq_files = n_daq_files + 1 WHERE id = %d",proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_file_open - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  return DB_OK;

}

int db_file_close(char* file_name,time_t t,unsigned long int size,unsigned int n_events, int proc_id)
{

  struct tm* t_st;
  char sqlCode[10240];

  t_st = localtime(&t);
  sprintf(sqlCode,"UPDATE daq_file SET time_close = '%04d-%02d-%02d %02d:%02d:%02d', size = %lu, n_events = %u WHERE name = '%s'",1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,size,n_events,file_name);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_file_close - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  // Update size/events counters for this process
  sprintf(sqlCode,"UPDATE process SET total_events = total_events + %u, total_size = total_size + %lu WHERE id = %d;",n_events,size,proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_file_close - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  
  return DB_OK;

}

int db_add_cfg_para(int proc_id, const char* para_name, char* para_val)
{

  char sqlCode[10240];

  int para_id = db_get_para_id(para_name);
  if (para_id == -1) {
    printf("DB::db_add_cfg_para error: db_get_para_id returned -1\n");
    return DB_SQLERROR;
  }

  // Update size/events counters for this run
  sprintf(sqlCode,"INSERT INTO proc_config_para (process_id,config_para_name_id,value) VALUES (%d,%d,'%s');",proc_id,para_id,para_val);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_add_cfg_para - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  
  return DB_OK;

}

int db_get_para_id(const char* para_name)
{

  int result = 0;
  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  // Get id of parameter if it exists
  sprintf(sqlCode,"SELECT id FROM config_para_name WHERE name = '%s'",para_name);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_get_para_id - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return -1;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_get_para_id - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return -1;
  }

  if (mysql_num_rows(res) == 0) {

    // Parameter does not exist: create it
    sprintf(sqlCode,"INSERT INTO config_para_name (name) VALUES ('%s')",para_name);
    if ( mysql_query(DBHandle,sqlCode) ) {
      printf("DB::db_get_para_id - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
      result = -1;
    } else {
      result = mysql_insert_id(DBHandle);
    }

  } else if (mysql_num_rows(res) == 1) {

    row = mysql_fetch_row(res);
    result = atoi(row[0]);

  } else {

    printf("DB::db_get_para_id - ERROR multiple parameters same name (%s) pair (?)\n",para_name);
    result = -1;

  }

  mysql_free_result(res);
  res = NULL;

  return result;

}
