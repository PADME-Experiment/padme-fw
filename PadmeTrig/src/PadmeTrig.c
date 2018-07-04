#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <libgen.h>

#include "Config.h"
#include "Trigger.h"

// Return pid of locking process or 0 if no other DAQ is running
pid_t create_lock()
{
  FILE* lock_handle;
  pid_t pid;
  struct stat sb;

  // Check if lock file exists and return its pid
  if ( access(Config->lock_file,F_OK) != -1 ) {
    lock_handle = fopen(Config->lock_file,"r");
    if ( fscanf(lock_handle,"%d",&pid) == 0 ) {
      printf("PadmeTrig::create_lock - ERROR - Lock file '%s' found but could not read the PID\n",Config->lock_file);
      pid = -1;
    }
    fclose(lock_handle);
    return pid;
  }

  // Check if directory for lock file exists
  char* path = strdup(Config->lock_file);
  char* lock_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(lock_dir,&sb) == 0 && S_ISDIR(sb.st_mode) ) {

    // Create lock file and write own pid in it
    lock_handle = fopen(Config->lock_file,"w");
    fprintf(lock_handle,"%d",getpid());
    fclose(lock_handle);
    printf("PadmeTrig::create_lock - Lock file '%s' created for PID %d\n",Config->lock_file,getpid());
    return 0;

  }

  printf("PadmeTrig::create_lock - ERROR - Directory '%s' does not exist: cannot create lock file '%s'\n",lock_dir,Config->lock_file);
  return -1;

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
      printf("PadmeTrig::remove_lock - ERROR - Problem while removing lock file '%s'\n",Config->lock_file);
    } else {
      printf("PadmeTrig::remove_lock - Lock file '%s' removed\n",Config->lock_file);
    }
  } else {
    printf("PadmeTrig::remove_lock - ERROR - Directory '%s' does not exist: cannot remove lock file '%s'.\n",lock_dir,Config->lock_file);
  }

  return;

}

// Handle failure in initialization phase
// If rmv_lock<>0 the lock file will also be removed
void init_fail(int rmv_lock)
{
  FILE* iff;
  struct stat sb;

  // Check if directory for initfail file exists
  char* path = strdup(Config->initfail_file);
  char* iff_dir = dirname(path); // N.B. dirname modifies its argument!
  if ( stat(iff_dir,&sb) == 0 && S_ISDIR(sb.st_mode) ) {
    if ( access(Config->initfail_file,F_OK) == -1 ) {
      iff = fopen(Config->initfail_file,"w");
      fclose(iff);
      printf("PadmeTrig::init_fail - InitFail file '%s' created\n",Config->initfail_file);
    } else {
      printf("PadmeTrig::init_fail - InitFail file '%s' already exists.\n",Config->initfail_file);
    }
  } else {
    printf("PadmeTirg::init_fail - Directory '%s' does not exist: cannot create InitFail file '%s'.\n",iff_dir,Config->initfail_file);
  }
  if (rmv_lock) remove_lock();
  exit(1);
}

int main(int argc, char *argv[]) {

  pid_t pid;
  int c;
  char mask[4];

  // Use line buffering for stdout
  setlinebuf(stdout);

  // Show welcome message
  printf("===========================================\n");
  printf("=== Welcome to the PADME Trigger system ===\n");
  printf("===========================================\n");

  // Initialize run configuration
  if ( init_config() ) {
    printf("*** ERROR *** Problem initializing run configuration.\n");
    exit(1);
  }

  // Parse options
  while ((c = getopt (argc, argv, "c:h")) != -1)
    switch (c)
      {
      case 'c':
	// Check if another configuration file was specified
	if ( strcmp(Config->config_file,"")!=0 ) {
	  printf("*** ERROR *** Multiple configuration files specified: '%s' and '%s'.\n",Config->config_file,optarg);
	  exit(1);
	}
	// Read configuration parameters from configuration file
	if ( read_config(optarg) ) {
	  printf("*** ERROR *** Problem while reading configuration file '%s'.\n",optarg);
	  exit(1);
	}
        break;
      case 'h':
	fprintf(stdout,"\nPadmeTrig [-c cfg_file] [-h]\n\n");
	fprintf(stdout,"  -c: use file 'cfg_file' to set configuration parameters for this process\n");
	fprintf(stdout,"     If no file is specified, use default settings\n");
	fprintf(stdout,"  -h: show this help message and exit\n\n");
	exit(0);
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option '-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character '\\x%x'.\n",optopt);
        exit(1);
      default:
        abort ();
      }

  // Show configuration
  print_config();

  // Check if another PadmeTrig program is running
  printf("\n=== Verifying that no other PadmeTrig instances are running ===\n");
  if ( (pid = create_lock()) ) {
    if (pid > 0) {
      printf("*** ERROR *** Another PadmeTrig is running with PID %d. Exiting.\n",pid);
    } else {
      printf("*** ERROR *** Problems while creating lock file '%s'. Exiting.\n",Config->lock_file);
    }
    init_fail(0);
  }

  // Connect to Trigger board
  printf("\n=== Connect to Trigger board ===\n");
  if ( trig_init() != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while connecting to Trigger board. Exiting.\n");
    init_fail(1);
  }

  // Show current masks
  if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while readying trigger and busy masks. Exiting.\n");
    init_fail(1);
  }
  printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

  // Disable all triggers
  if ( trig_set_trigmask(0) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    init_fail(1);
  }

  // Disable busy
  if ( trig_set_busymask(0) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting busy mask. Exiting.\n");
    init_fail(1);
  }

  // Show current masks
  if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger and busy masks. Exiting.\n");
    init_fail(1);
  }
  printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

  // Enable all triggers
  if ( trig_set_trigmask(0x3f) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while resetting trigger mask. Exiting.\n");
    init_fail(1);
  }

  // Show current masks
  if ( trig_get_trigbusymask(mask) != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while reading trigger and busy masks. Exiting.\n");
    init_fail(1);
  }
  printf("Current masks: trig 0x%02x busy 0x%02x dummy 0x%02x 0x%02x\n",mask[3],mask[2],mask[1],mask[0]);

  // Run some data acquisition
  unsigned char data[100000];
  unsigned int data_len;
  while(1){
    if ( trig_get_data((void*)data,&data_len) != TRIG_OK ) {
      printf("PadmeTrig *** ERROR *** Problem while reading data. Exiting.\n");
      init_fail(1);
    }
    printf("New packet of length %u bytes received\n",data_len);
    unsigned int p;
    for(p=0;p<data_len;p+=8){
      printf("0x%02x%02x%02x%02x%02x%02x%02x%02x\n",data[p+0],data[p+1],data[p+2],data[p+3],data[p+4],data[p+5],data[p+6],data[p+7]);
    }
  }

  if ( trig_end() != TRIG_OK ) {
    printf("PadmeTrig *** ERROR *** Problem while disconnecting from trigger board. Exiting.\n");
    init_fail(1);
  }

  // Remove lock file
  remove_lock();

  // Clean up configuration
  end_config();

  // All is done: exit
  printf("\n=== May the force be with you. Bye! ===\n");
  exit(0);

}
