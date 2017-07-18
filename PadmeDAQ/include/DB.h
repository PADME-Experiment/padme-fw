#ifndef _DB_H_
#define _DB_H_

#include <time.h>

#define DB_OK           0
#define DB_CONNECTERROR 1
#define DB_SQLERROR     2

int db_init();
int db_end();
int db_run_check(int); // run_nr
int db_process_create(int,int); // run_nr,board_id
int db_get_process_id(int,int); // run_nr,board_id (returns id or -1 for error)
int db_process_open(int,time_t); // process_id,time
int db_process_close(int,time_t); // process_id,time
int db_file_check(char*); // file_name
int db_file_open(char*,int,time_t,int,int); // file_name,file_version,time,process_id,part
int db_file_close(char*,time_t,unsigned long int,unsigned int,int); // file_name,time,size,nevents,process_id
int db_add_cfg_para(int,const char*,char*); // process_id,parameter_name,parameter_value
int db_get_para_id(const char*); // parameter_name (returns id or -1 for error)

#endif
