#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>

#include "Config.h"

#include "Tools.h"

// Return pid of locking process or 0 if no other DAQ is running
pid_t create_lock()
{
  FILE* lock_handle;
  pid_t pid;
  struct stat sb;

  if ( strcmp(Config->lock_file,"none")==0 ) {
    printf("PadmeDAQ::create_lock - No lock file required\n");
    return 0;
  }

  // Check if directory for lock file exists
  char* path = strdup(Config->lock_file);
  char* lock_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(lock_dir,&sb) != 0 || (! S_ISDIR(sb.st_mode)) ) {
    printf("PadmeDAQ::create_lock - ERROR - Directory '%s' does not exist: cannot create lock file '%s'\n",lock_dir,Config->lock_file);
    return -1;
  }

  // Check if lock file exists and return its pid
  if ( access(Config->lock_file,F_OK) != -1 ) {
    lock_handle = fopen(Config->lock_file,"r");
    if ( fscanf(lock_handle,"%d",&pid) == 0 ) {
      printf("PadmeDAQ::create_lock - ERROR - Lock file '%s' found but could not read the PID\n",Config->lock_file);
      pid = -1;
    }
    fclose(lock_handle);
    return pid;
  }

  // Create lock file and write own pid in it
  lock_handle = fopen(Config->lock_file,"w");
  fprintf(lock_handle,"%d\n",getpid());
  fclose(lock_handle);
  printf("PadmeDAQ::create_lock - Lock file '%s' created for PID %d\n",Config->lock_file,getpid());
  return 0;

}

// Remove lock file (called just before exiting)
void remove_lock()
{
  struct stat sb;

  // Check if lock file directory exists
  char* path = strdup(Config->lock_file);
  char* lock_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(lock_dir,&sb) == 0 && S_ISDIR(sb.st_mode) ) {
    if ( unlink(Config->lock_file) ) {
      printf("PadmeDAQ::remove_lock - ERROR - Problem while removing lock file '%s'\n",Config->lock_file);
    } else {
      printf("PadmeDAQ::remove_lock - Lock file '%s' removed\n",Config->lock_file);
    }
  } else {
    printf("PadmeDAQ::remove_lock - ERROR - Directory '%s' does not exist: cannot remove lock file '%s'.\n",lock_dir,Config->lock_file);
  }

  return;

}

int create_initok_file()
{

  FILE* iff;
  struct stat sb;

  // Check if directory for initok file exists
  char* path = strdup(Config->initok_file);
  char* iff_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(iff_dir,&sb) != 0 || ! S_ISDIR(sb.st_mode) ) {
    printf("PadmeDAQ::create_initok_file - Directory '%s' does not exist: cannot create InitOK file '%s'.\n",iff_dir,Config->initok_file);
    return 1;
  }

  // Check if file is already there (not cleaned in previous run?)
  if ( access(Config->initok_file,F_OK) != -1 ) {
    printf("PadmeDAQ::create_initok_file - InitOK file '%s' already exists.\n",Config->initok_file);
    return 1;
  }

  // Create InitOK file
  iff = fopen(Config->initok_file,"w");
  fclose(iff);
  printf("- InitOK file '%s' created\n",Config->initok_file);
  return 0;

}

int create_initfail_file()
{

  FILE* iff;
  struct stat sb;

  // Check if directory for initfail file exists
  char* path = strdup(Config->initfail_file);
  char* iff_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(iff_dir,&sb) != 0 || ! S_ISDIR(sb.st_mode) ) {
    printf("PadmeDAQ::create_initfail_file - Directory '%s' does not exist: cannot create InitFail file '%s'.\n",iff_dir,Config->initfail_file);
    return 1;
  }

  // Check if file is already there (not cleaned in previous run?)
  if ( access(Config->initfail_file,F_OK) != -1 ) {
    printf("PadmeDAQ::create_initfail_file - InitFail file '%s' already exists.\n",Config->initfail_file);
    return 1;
  }

  // Create InitFail file
  iff = fopen(Config->initfail_file,"w");
  fclose(iff);
  printf("- InitFail file '%s' created\n",Config->initfail_file);
  return 0;

}

// Return file name given the file open time. Return 0 if OK, <>0 error
int generate_filename(char* name, const time_t time) {
  struct tm* t = gmtime(&time);
  sprintf(name,"%s_%.4d_%.2d_%.2d_%.2d_%.2d_%.2d",
	  Config->data_file,
	  1900+t->tm_year, 1+t->tm_mon, t->tm_mday,
	  t->tm_hour,      t->tm_min,   t->tm_sec);
  return 0;
}

// Write time (in secs) to a string with standard formatting
char* format_time(const time_t time) {
  static char tform[20];
  struct tm* t = gmtime(&time);
  sprintf(tform,"%.4d/%.2d/%.2d %.2d:%.2d:%.2d",
	  1900+t->tm_year, 1+t->tm_mon, t->tm_mday,
	  t->tm_hour,      t->tm_min,   t->tm_sec);
  return tform;
}
