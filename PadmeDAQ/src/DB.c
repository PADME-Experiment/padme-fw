#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sqlite3.h>

#include "DB.h"

sqlite3* DBHandle = NULL;

// Create a connection to the SQLite database
int db_init(char* db_file)
{
  int rc;

  // Check if this is the first time we are called
  if (DBHandle != NULL) return DB_OK;

  rc = sqlite3_open(db_file,&DBHandle);
  if ( rc == SQLITE_OK ) {
    printf("- Connected to DB file '%s'\n",db_file);
  } else {
    printf("Unable to connect to '%s' database file. Erro code: %d\n",db_file,rc);
    return DB_CONNECTERROR;
  }
  return DB_OK;
}

int db_end()
{
  int rc;
  rc = sqlite3_close(DBHandle);
  if ( rc == SQLITE_OK ) {
    printf("- DB was disconnected\n");
  } else {
    printf("Unable to disconnect database. Error code: %d\n",rc);
    return DB_CONNECTERROR;
  }
  return DB_OK;
}

// Check if run already exists (0: no, 1: yes, -1: error)
int db_run_check(int run_nr)
{

  int result;
  char sqlCode[10240];
  sqlite3_stmt* res;

  result = 0;

  strcpy(sqlCode,"SELECT number FROM run WHERE number = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_int(res, 1, run_nr) == SQLITE_OK ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_ROW:
	result = 1;
	break;
      case SQLITE_DONE:
	result = 0;
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	result = -1;
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind_int error: %s\n", sqlite3_errmsg(DBHandle));
      result = -1;
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
    result = -1;
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = -1; }

  return result;

}

// Create a new process in the database
// N.B. Default process status is 0 (i.e. DAQ not started)
int db_process_create(int run_nr,int board_id)
{

  char sqlCode[10240];
  int result;
  sqlite3_stmt* res;

  // Insert run into DB and initialize to 0 all parameters
  result = DB_SQLERROR;
  strcpy(sqlCode,"INSERT INTO process (run_number,board_id,status,time_start,time_stop,n_raw_files,total_events,total_size) VALUES (?,?,?,?,?,?,?,?)");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_int (res,  1, run_nr)   == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  2, board_id) == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  3, 0)        == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  4, 0)        == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  5, 0)        == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  6, 0)        == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  7, 0)        == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  8, 0)        == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }

  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }

  return result;

}

// Get process id from run_number and board_id
int db_get_process_id(int run_nr,int board_id)
{

  int result;
  char sqlCode[10240];
  sqlite3_stmt* res;

  result = 0;

  strcpy(sqlCode,"SELECT id FROM process WHERE run_number = ? AND board_id = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_int(res, 1, run_nr)   == SQLITE_OK ) &&
	( sqlite3_bind_int(res, 2, board_id) == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_ROW: // One row was returned: get process_id
	result = sqlite3_column_int(res,0);
	break;
      case SQLITE_DONE: // No rows returned: process does not exist
	result = 0;
	break;
      default: // Anything else is an error
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	result = -1;
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind_int error: %s\n", sqlite3_errmsg(DBHandle));
      result = -1;
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
    result = -1;
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = -1; }

  return result;

}

int db_process_open(int proc_id,int t)
{

  char sqlCode[10240];
  int result;
  sqlite3_stmt* res;

  result = DB_SQLERROR;
  strcpy(sqlCode,"UPDATE process SET status = ?, time_start = ? WHERE id = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_int (res,  1, 1)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  2, t)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  3, proc_id) == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }

  return result;

}

int db_process_close(int proc_id,int t)
{

  char sqlCode[10240];
  int result;
  sqlite3_stmt* res;

  result = DB_SQLERROR;
  strcpy(sqlCode,"UPDATE process SET status = ?, time_stop = ? WHERE id = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_int (res,  1, 2)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  2, t)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  3, proc_id) == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }

  return result;

}

// Check if file already exists (0: no, 1: yes, -1: error)
int db_file_check(char* file_name)
{

  int result;
  char sqlCode[10240];
  sqlite3_stmt* res;

  result = 0;
  strcpy(sqlCode,"SELECT name FROM raw_file WHERE name = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_text(res, 1, file_name, -1, SQLITE_TRANSIENT) == SQLITE_OK ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_ROW:
	result = 1;
	break;
      case SQLITE_DONE:
	result = 0;
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	result = -1;
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind_text error: %s\n", sqlite3_errmsg(DBHandle));
      result = -1;
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
    result = -1;
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = -1; }

  return result;

}

int db_file_open(char* file_name,int t,int proc_id,int part)
{

  char sqlCode[10240];
  int result;
  sqlite3_stmt* res;

  // Insert file into DB and initialize to 0 all parameters
  result = DB_SQLERROR;
  strcpy(sqlCode,"INSERT INTO raw_file (name,version,process_id,part,time_open,time_close,n_events,size) VALUES (?,?,?,?,?,?,?,?)");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_text(res,  1, file_name, -1, SQLITE_TRANSIENT) == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  2, 1)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  3, proc_id) == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  4, part)    == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  5, t)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  6, 0)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  7, 0)       == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  8, 0)       == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }

  if (result == DB_SQLERROR) return result;

  // Update file counter for this process
  result = DB_SQLERROR;
  strcpy(sqlCode,"UPDATE process SET n_raw_files = n_raw_files + 1 WHERE id = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_int (res,  1, proc_id) == SQLITE_OK ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }

  return result;

}

int db_file_close(char* file_name,int t,int size,int n_events, int proc_id)
{

  char sqlCode[10240];
  int result;
  sqlite3_stmt* res;

  // Set time of close and record size of file
  result = DB_SQLERROR;
  strcpy(sqlCode,"UPDATE file SET time_close = ?, size = ?, n_events = ? WHERE name = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_int (res,  1, t)        == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  2, size)     == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  3, n_events) == SQLITE_OK ) &&
	( sqlite3_bind_text(res,  4, file_name, -1, SQLITE_TRANSIENT) == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }

  if ( result == DB_SQLERROR ) return result;

  // Update size/events counters for this run
  result = DB_SQLERROR;
  strcpy(sqlCode,"UPDATE process SET total_events = total_events + ?, total_size = total_size + ? WHERE id = ?;");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_int (res,  1, n_events)  == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  2, size)      == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  3, proc_id)   == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }  
  } else {
    fprintf(stderr, "sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }
  
  return result;

}

int db_add_cfg_para(int proc_id, char* para_name, char* para_val)
{

  char sqlCode[10240];
  int result;
  sqlite3_stmt* res;

  int para_id = db_get_para_id(para_name);
  if (para_id == -1) {
    fprintf(stderr, "db_add_cfg_para error: db_get_para_id returned -1\n");
    return DB_SQLERROR;
  }

  // Update size/events counters for this run
  result = DB_SQLERROR;
  strcpy(sqlCode,"INSERT INTO proc_config_para (process_id,config_para_name_id,value) VALUES (?,?,?);");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if (
	( sqlite3_bind_int (res,  1, proc_id)  == SQLITE_OK ) &&
	( sqlite3_bind_int (res,  2, para_id)  == SQLITE_OK ) &&
	( sqlite3_bind_text(res,  3, para_val, -1, SQLITE_TRANSIENT) == SQLITE_OK )
       ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = DB_OK;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "db_add_cfg_para error - sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "db_add_cfg_para - sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "db_add_cfg_para - sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }  
  } else {
    fprintf(stderr, "db_add_cfg_para - sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = DB_SQLERROR; }
  
  return result;

}

int db_get_para_id(char* para_name)
{

  char sqlCode[10240];
  int result;
  sqlite3_stmt* res;

  // Get id of parameter if it exists
  result = -1;
  strcpy(sqlCode,"SELECT id FROM config_para_name WHERE name = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_text(res, 1, para_name, -1, SQLITE_TRANSIENT) == SQLITE_OK ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_ROW:
	result = sqlite3_column_int(res,0);
	break;
      case SQLITE_DONE:
	result = 0;
	break;
      default:
	fprintf(stderr, "db_get_para_id - sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "db_get_para_id - sqlite3_bind_text error: %s\n", sqlite3_errmsg(DBHandle));
    }
  } else {
    fprintf(stderr, "db_get_para_id - sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = -1; }

  // Check if we got the parameter id or an error
  if (result != 0) { return result; }

  // Parameter does not exist: create it
  result = -1;
  strcpy(sqlCode,"INSERT INTO config_para_name (name) VALUES (?)");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_text(res, 1, para_name, -1, SQLITE_TRANSIENT) == SQLITE_OK ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_DONE:
	result = 0;
	break;
      case SQLITE_ROW:
	fprintf(stderr, "db_get_para_id error - sqlite3_step returned SQLITE_ROW (what?)\n");
	break;
      default:
	fprintf(stderr, "db_get_para_id - sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "db_get_para_id - sqlite3_bind error: %s\n", sqlite3_errmsg(DBHandle));
    }  
  } else {
    fprintf(stderr, "db_get_para_id - sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = -1; }
  
  if (result != 0) { return result; }

  // Get id of newly created parameter
  result = -1;
  strcpy(sqlCode,"SELECT id FROM config_para_name WHERE name = ?");
  if ( sqlite3_prepare_v2(DBHandle, sqlCode, -1, &res, NULL) == SQLITE_OK ) {
    if ( sqlite3_bind_text(res, 1, para_name, -1, SQLITE_TRANSIENT) == SQLITE_OK ) {
      switch ( sqlite3_step(res) ) {
      case SQLITE_ROW:
	result = sqlite3_column_int(res,0);
	break;
      case SQLITE_DONE:
	fprintf(stderr, "db_get_para_id - sqlite3_step error: parameter not found after creation\n");
	break;
      default:
	fprintf(stderr, "db_get_para_id - sqlite3_step error: %s\n", sqlite3_errmsg(DBHandle));
	break;
      }
    } else {
      fprintf(stderr, "db_get_para_id - sqlite3_bind_text error: %s\n", sqlite3_errmsg(DBHandle));
    }
  } else {
    fprintf(stderr, "db_get_para_id - sqlite3_prepare_v2 error: %s\n", sqlite3_errmsg(DBHandle));
  }
  if ( sqlite3_finalize(res) != SQLITE_OK ) { result = -1; }

  return result;

}
