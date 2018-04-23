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

#include "DB.h"
#include "Config.h"
#include "DAQ.h"
#include "ZSUP.h"
#include "FAKE.h"

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
      printf("PadmeDAQ::create_lock - ERROR - Lock file '%s' found but could not read the PID\n",Config->lock_file);
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
    printf("PadmeDAQ::create_lock - Lock file '%s' created for PID %d\n",Config->lock_file,getpid());
    return 0;

  }

  printf("PadmeDAQ::create_lock - ERROR - Directory '%s' does not exist: cannot create lock file '%s'\n",lock_dir,Config->lock_file);
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
      printf("PadmeDAQ::remove_lock - ERROR - Problem while removing lock file '%s'\n",Config->lock_file);
    } else {
      printf("PadmeDAQ::remove_lock - Lock file '%s' removed\n",Config->lock_file);
    }
  } else {
    printf("PadmeDAQ::remove_lock - ERROR - Directory '%s' does not exist: cannot remove lock file '%s'.\n",lock_dir,Config->lock_file);
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
      printf("PadmeDAQ::init_fail - InitFail file '%s' created\n",Config->initfail_file);
    } else {
      printf("PadmeDAQ::init_fail - InitFail file '%s' already exists.\n",Config->initfail_file);
    }
  } else {
    printf("PadmeDAQ::init_fail - Directory '%s' does not exist: cannot create InitFail file '%s'.\n",iff_dir,Config->initfail_file);
  }
  if (rmv_lock) remove_lock();
  exit(1);
}

// Start of main program
int main(int argc, char*argv[])
{

  pid_t pid;
  int c;
  int rc;

  //printf("Size of int %lu long int %lu long long int %lu\n",sizeof(int),sizeof(long int),sizeof(long long int));

  // Make sure local data types are correct for us
  if (sizeof(int)<4) {
    printf("*** On this system sizeof(int) is %lu bytes while we need at least 4 bytes. Aborting ***\n",sizeof(int));
    exit(1);
  }
  if (sizeof(long)<8) {
    printf("*** On this system sizeof(long) is %lu bytes while we need at least 8 bytes. Aborting ***\n",sizeof(long));
    exit(1);
  }

  // Use line buffering for stdout
  setlinebuf(stdout);

  // Show welcome message
  printf("=======================================\n");
  printf("=== Welcome to the PADME DAQ system ===\n");
  printf("=======================================\n");

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
	fprintf(stdout,"\nPadmeDAQ [-c cfg_file] [-r run_nr] [-R] [-m comment] [-h]\n\n");
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

  // Check if another DAQ is running
  printf("\n=== Verifying that no other DAQ instances are running ===\n");
  if ( (pid = create_lock()) ) {
    if (pid > 0) {
      printf("*** ERROR *** Another DAQ is running with PID %d. Exiting.\n",pid);
    } else {
      printf("*** ERROR *** Problems while creating lock file '%s'. Exiting.\n",Config->lock_file);
    }
    init_fail(0);
  }

  if ( Config->run_number ) {

    // Connect to DB
    if ( db_init() != DB_OK ) init_fail(1);

    // Verify if run number is valid
    rc = db_run_check(Config->run_number);
    if ( rc == -1 ) {
      printf("ERROR: DB check for run number %d returned an error\n",Config->run_number);
      init_fail(1);
    } else if ( rc == 0 ) {
      printf("ERROR: run number %d does not exist in the DB\n",Config->run_number);
      init_fail(1);
    }

    // Verify if process id is valid
    rc = db_process_check(Config->process_id);
    if ( rc == -1 ) {
      printf("ERROR: DB check for process id %d returned an error\n",Config->process_id);
      init_fail(1);
    } else if ( rc == 0 ) {
      printf("ERROR: process id %d does not exist in the DB\n",Config->process_id);
      init_fail(1);
    }

    // Now we can save the process configuration to DB
    save_config();

  }

  // Check current running mode (DAQ, ZSUP, FAKE)

  if ( strcmp(Config->process_mode,"DAQ")==0 ) {

    // Show some startup message
    if (Config->run_number == 0) {
      printf("\n=== Starting PadmeDAQ acquisition for dummy run ===\n");
      printf("- WARNING: data will not be registered in the DB!\n");
    } else {
      printf("\n=== Starting PadmeDAQ acquisition for run %d ===\n",Config->run_number);
    }
    //printf("- Run type: '%s'\n",Config->run_type);

    // Connect to digitizer
    printf("\n=== Connect to digitizer ===\n");
    if ( DAQ_connect() ) {
      printf("*** ERROR *** Problem while connecting to V1742 digitizer. Exiting.\n");
      init_fail(1);
    }

    // Initialize and configure digitizer
    printf("\n=== Initialize digitizer ===\n");
    if ( DAQ_init() ) {
      printf("*** ERROR *** Problem while initializing V1742 digitizer. Exiting.\n");
      init_fail(1);
    }

    // Handle data acquisition
    printf("\n=== Starting data acquisition ===\n");
    rc = DAQ_readdata();
    if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing DAQ process. Exiting.\n");
      init_fail(1);
    } else if ( rc == 2 ) {
      printf("*** ERROR *** Data acquistion ended with an error. Please check log file for details. Exiting.\n");
      remove_lock();
      exit(1);
    } else if ( rc == 3 ) {
      printf("=== Run aborted before starting DAQ ===\n");
    }

    // Final reset of the digitizer
    printf("\n=== Reset digitizer and close connection ===\n");
    if ( DAQ_close() ) {
      printf("*** ERROR *** Final reset of digitizer ended with an error. Exiting.\n");
      remove_lock();
      exit(1);
    }

  } else if ( strcmp(Config->process_mode,"FAKE")==0 ) {

    // Show some startup message
    if (Config->run_number == 0) {
      printf("\n=== Starting PadmeDAQ FAKE event generation for dummy run ===\n");
      printf("- WARNING: data will not be registered in the DB!\n");
    } else {
      printf("\n=== Starting PadmeDAQ FAKE event generation for run %d ===\n",Config->run_number);
    }

    // Start generation of FAKE events
    rc = FAKE_readdata();
    if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing FAKE process. Exiting.\n");
      init_fail(1);
    } else if ( rc == 2 ) {
      printf("*** ERROR *** FAKE event generation ended with an error. Please check log file for details. Exiting.\n");
      remove_lock();
      exit(1);
    }

    printf("\n=== FAKE event generation process ended ===\n");


  } else if ( strcmp(Config->process_mode,"ZSUP")==0 ) {

    // Show some startup message
    if (Config->run_number == 0) {
      printf("\n=== Starting PadmeDAQ zero suppression for dummy run ===\n");
      printf("- WARNING: data will not be registered in the DB!\n");
    } else {
      printf("\n=== Starting PadmeDAQ zero suppression for run %d ===\n",Config->run_number);
    }
    //printf("- Run type: '%s'\n",Config->run_type);

    // Handle zero suppression
    printf("\n=== Starting zero suppression ===\n");
    rc = ZSUP_readdata();
    if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing ZSUP process. Exiting.\n");
      init_fail(1);
    } else if ( rc == 2 ) {
      printf("*** ERROR *** Zero suppression ended with an error. Please check log file for details. Exiting.\n");
      remove_lock();
      exit(1);
    }

    printf("\n=== Zero suppression process ended ===\n");

  }

  // Remove lock file
  remove_lock();

  // Clean up configuration
  end_config();

  // All is done: exit
  printf("\n=== May the force be with you. Bye! ===\n");
  exit(0);

}
