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

// Create a connection to MySQL database
int db_init()
{

  //const char* db_host;
  //const char* db_user;
  //const char* db_passwd;
  //const char* db_name;
  //unsigned int db_port;

  // Check if this is the first time we are called
  if (DBHandle != NULL) return DB_OK;

  // Create handle to MySQL
  DBHandle = mysql_init(NULL);
  if (DBHandle == NULL) {
    printf("Unable to create a MySQL database handle.\n");
    return DB_CONNECTERROR;
  }

  /*
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
  */

  return db_connect();

}

int db_connect()
{

  const char* db_host;
  const char* db_user;
  const char* db_passwd;
  const char* db_name;
  unsigned int db_port;

  // Get MySQL server connection parameters from environment variables
  //printf("Reading env variables\n");
  db_host = getenv("PADME_DB_HOST");     if (db_host == NULL) db_host = "localhost";
  db_user = getenv("PADME_DB_USER");     if (db_user == NULL) db_user = "padme";
  db_passwd = getenv("PADME_DB_PASSWD"); if (db_passwd == NULL) db_passwd = "unknown";
  db_name = getenv("PADME_DB_NAME");     if (db_name == NULL) db_name = "PadmeDB";
  db_port = 5501; if (getenv("PADME_DB_PORT")) db_port = atoi(getenv("PADME_DB_PORT"));
  //printf("host %s user %s passwd %s name %s port %d\n",db_host,db_user,db_passwd,db_name,db_port);

  // Connect to MySQL server
  if (mysql_real_connect(DBHandle,db_host,db_user,db_passwd,db_name,db_port,NULL,0) == NULL) {
    fprintf(stderr, "mysql_real_connect ERROR: %s\n", mysql_error(DBHandle));
    mysql_close(DBHandle);
    return DB_CONNECTERROR;
  }  

  printf("- Connected to MySQL database server\n");
  return DB_OK;

}

int db_check_conn()
{

  // Check status of DB connection
  const char* stat = mysql_stat(DBHandle);

  // If status is not NULL, everything is OK
  if (stat) return DB_OK;

  // DB is gone away: try to reconnect
  fprintf(stderr, "mysql_stat ERROR: %s\n", mysql_error(DBHandle));
  return db_connect();
  
}

int db_end()
{
  mysql_close(DBHandle);
  printf("- Disconnected from database server\n");
  return DB_OK;
}

// Check if run already exists (0: no, 1: yes, <0: error)
int db_run_check(int run_nr)
{

  int result;
  char sqlCode[10240];
  MYSQL_RES* res;

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_run_check - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  sprintf(sqlCode,"SELECT number FROM run WHERE number = %d",run_nr);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_run_check - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_run_check - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return DB_CONNECTERROR;
  }

  if (mysql_num_rows(res) == 0) {
    result = 0;
  } else if (mysql_num_rows(res) == 1) {
    result = 1;
  } else {
    printf("DB::db_run_check - ERROR multiple runs with same number (?)\n");
    result = DB_SQLERROR;
  }

  mysql_free_result(res);
  res = NULL;

  return result;

}

// Check if process already exists (0: no, 1: yes, <0: error)
int db_process_check(int proc_id)
{

  int result;
  char sqlCode[10240];
  MYSQL_RES* res;

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_process_check - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  sprintf(sqlCode,"SELECT id FROM daq_process WHERE id = %d",proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_process_check - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_process_check - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return DB_CONNECTERROR;
  }

  if (mysql_num_rows(res) == 0) {
    result = 0;
  } else if (mysql_num_rows(res) == 1) {
    result = 1;
  } else {
    printf("DB::db_process_check - ERROR multiple processes with same id (?)\n");
    result = DB_SQLERROR;
  }

  mysql_free_result(res);
  res = NULL;

  return result;

}

int db_process_open(int proc_id,time_t t)
{

  struct tm* t_st;
  char sqlCode[10240];

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_process_open - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  t_st = gmtime(&t);
  sprintf(sqlCode,"UPDATE daq_process SET status = 1, time_start = '%04d-%02d-%02d %02d:%02d:%02d' WHERE id = %d",
	  1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_process_open - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  return DB_OK;

}

int db_process_close(int proc_id,time_t t, unsigned long int size, unsigned int nevt)
{

  struct tm* t_st;
  char sqlCode[10240];

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_process_close - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  t_st = gmtime(&t);
  sprintf(sqlCode,"UPDATE daq_process SET status = 2, time_stop = '%04d-%02d-%02d %02d:%02d:%02d', total_events = %u, total_size = %lu WHERE id = %d",
	  1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,nevt,size,proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_process_close - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  return DB_OK;

}

int db_process_get_status(int proc_id)
{

  char sqlCode[10240];
  MYSQL_RES* res;
  MYSQL_ROW row;

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_file_check - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  // Get current status of process
  sprintf(sqlCode,"SELECT status FROM daq_process WHERE id = %d",proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_run_check - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_run_check - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return DB_CONNECTERROR;
  }

  if (mysql_num_rows(res) == 0) {
    printf("DB::process_get_status - ERROR process %d does not exist in DB (?)\n",proc_id);
    return DB_SQLERROR;
  } else if (mysql_num_rows(res) > 1) {
    printf("DB::process_get_status - ERROR multiple processes with same id %d (?)\n",proc_id);
    return DB_SQLERROR;
  }

  row = mysql_fetch_row(res);
  return atoi(row[0]);

}

int db_process_set_status(int proc_id,int status)
{

  char sqlCode[10240];

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_process_set_status - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  sprintf(sqlCode,"UPDATE daq_process SET status = %d WHERE id = %d",status,proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_process_set_status - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  return DB_OK;

}

// Check if file already exists (0: no, 1: yes, <0: error)
int db_file_check(char* file_name)
{

  int result;
  char sqlCode[10240];
  MYSQL_RES* res;

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_file_check - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  sprintf(sqlCode,"SELECT name FROM daq_file WHERE name = '%s'",file_name);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_run_check - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_run_check - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return DB_CONNECTERROR;
  }

  if (mysql_num_rows(res) == 0) {
    result = 0;
  } else if (mysql_num_rows(res) == 1) {
    result = 1;
  } else {
    printf("DB::db_run_check - ERROR multiple files with same name (?)\n");
    result = DB_SQLERROR;
  }

  mysql_free_result(res);
  res=NULL;

  return result;
  
}
 
int db_file_open(char* file_name,int file_version,time_t t,int proc_id,int part)
{

  struct tm* t_st;
  char sqlCode[10240];

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_file_open - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  //t_st = localtime(&t);
  t_st = gmtime(&t);
  sprintf(sqlCode,"INSERT INTO daq_file (name,version,daq_process_id,part,time_open,n_events,size) VALUES ('%s',%d,%d,%d,'%04d-%02d-%02d %02d:%02d:%02d',0,0)",file_name,file_version,proc_id,part,1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_file_open - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  // Update file counter for this process
  sprintf(sqlCode,"UPDATE daq_process SET n_daq_files = n_daq_files + 1 WHERE id = %d",proc_id);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_file_open - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  return DB_OK;

}

//int db_file_close(char* file_name,time_t t,unsigned long long int size,unsigned int n_events, int proc_id)
int db_file_close(char* file_name,time_t t,unsigned long int size,unsigned int n_events)
{

  struct tm* t_st;
  char sqlCode[10240];

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_file_close - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  //t_st = localtime(&t);
  t_st = gmtime(&t);
  sprintf(sqlCode,"UPDATE daq_file SET time_close = '%04d-%02d-%02d %02d:%02d:%02d', size = %lu, n_events = %u WHERE name = '%s'",1900+t_st->tm_year,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_hour,t_st->tm_min,t_st->tm_sec,size,n_events,file_name);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_file_close - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }

  return DB_OK;

}

int db_add_cfg_para(int proc_id, const char* para_name, char* para_val)
{

  char sqlCode[10240];

  // Check if DB is alive
  if (db_check_conn()!=DB_OK) {
    printf("DB::db_add_cfg_para - ERROR connecting to MySQL DB\n");
    return DB_CONNECTERROR;
  }

  int para_id = db_get_para_id(para_name);
  if (para_id == -1) {
    printf("DB::db_add_cfg_para error: db_get_para_id returned -1\n");
    return DB_SQLERROR;
  }

  // Update size/events counters for this run
  sprintf(sqlCode,"INSERT INTO daq_proc_config_para (daq_process_id,config_para_name_id,value) VALUES (%d,%d,'%s');",proc_id,para_id,para_val);
  if ( mysql_query(DBHandle,sqlCode) ) {
    printf("DB::db_add_cfg_para - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
    return DB_SQLERROR;
  }
  
  return DB_OK;

}

// Return id of parameter with given name (or <0 if error)
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
    return DB_SQLERROR;
  }

  res = mysql_store_result(DBHandle);
  if (res == NULL) {
    printf("DB::db_get_para_id - ERROR retrieving result: %s\n", mysql_error(DBHandle));
    return DB_CONNECTERROR;
  }

  if (mysql_num_rows(res) == 0) {

    // Parameter does not exist: create it
    sprintf(sqlCode,"INSERT INTO config_para_name (name) VALUES ('%s')",para_name);
    if ( mysql_query(DBHandle,sqlCode) ) {
      printf("DB::db_get_para_id - ERROR executing SQL query: %s\n%s\n", mysql_error(DBHandle),sqlCode);
      result = DB_SQLERROR;
    } else {
      result = mysql_insert_id(DBHandle);
    }

  } else if (mysql_num_rows(res) == 1) {

    row = mysql_fetch_row(res);
    result = atoi(row[0]);

  } else {

    printf("DB::db_get_para_id - ERROR multiple parameters same name (%s) pair (?)\n",para_name);
    result = DB_SQLERROR;

  }

  mysql_free_result(res);
  res = NULL;

  return result;

}
