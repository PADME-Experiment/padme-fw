#ifndef _DB_H_
#define _DB_H_

//#include <time.h>

//#define DB_OK            0
//#define DB_CONNECTERROR -1
//#define DB_SQLERROR     -2

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

//int db_init();
//int db_connect();
//int db_check_conn();
//int db_end();
//int db_run_check(int); // run_nr
//int db_process_check(int); // process_id
//int db_process_open(int,time_t); // process_id,start time
//int db_process_close(int,time_t,unsigned long int,unsigned int); // process_id,stop time,size,nevent
//int db_process_get_status(int); // process_id
//int db_process_set_status(int,int); // process_id,status
////int db_file_check(char*); // file_name
////int db_file_open(char*,int,time_t,int,int); // file_name,file_version,time,process_id,part
////int db_file_close(char*,time_t,unsigned long int,unsigned int); // file_name,time,size,nevents
////int db_add_cfg_para(int,const char*,char*); // process_id,parameter_name,parameter_value
////int db_get_para_id(const char*); // parameter_name (returns id or -1 for error)

#endif
