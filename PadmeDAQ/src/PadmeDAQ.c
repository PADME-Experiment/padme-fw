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
#include "Tools.h"
#include "DAQ.h"
#include "ZSUP.h"
#include "FAKE.h"

/*
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
    fprintf(lock_handle,"%d\n",getpid());
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
  if ( access(Config->initfail_file,F_OK) == -1 ) {
    printf("PadmeDAQ::create_initfail_file - InitFail file '%s' already exists.\n",Config->initfail_file);
    return 1;
  }

  // Create InitFail file
  iff = fopen(Config->initfail_file,"w");
  fclose(iff);
  printf("- InitFail file '%s' created\n",Config->initfail_file);
  return 0;

}
*/

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
	fprintf(stdout,"\nPadmeDAQ [-c cfg_file] [-h]\n\n");
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

  // Check if another PadmeDAQ program is running
  printf("\n=== Verifying that no other DAQ instances are running ===\n");
  if ( (pid = create_lock()) ) {
    if (pid > 0) {
      printf("*** ERROR *** Another PadmeDAQ is running with PID %d. Exiting.\n",pid);
    } else {
      printf("*** ERROR *** Problems while creating lock file '%s'. Exiting.\n",Config->lock_file);
    }
    //init_fail(0);
    create_initfail_file();
    exit(1);
  }

  if ( Config->run_number ) {

    // Connect to DB
    if ( db_init() != DB_OK ) {
      printf("*** ERROR *** Unable to initialize DB connection. Exiting.\n");
      create_initfail_file();
      remove_lock();
      exit(1);
    }

    // Verify if run number is valid
    rc = db_run_check(Config->run_number);
    if ( rc != 1 ) {
      if ( rc < 0 ) {
	printf("ERROR: DB check for run number %d returned an error\n",Config->run_number);
      } else if ( rc == 0 ) {
	printf("ERROR: run number %d does not exist in the DB\n",Config->run_number);
      }
      create_initfail_file();
      remove_lock();
      exit(1);
    }

    // Verify if process id is valid
    rc = db_process_check(Config->process_id);
    if ( rc != 1 ) {
      if ( rc < 0 ) {
	printf("ERROR: DB check for process id %d returned an error\n",Config->process_id);
      } else if ( rc == 0 ) {
	printf("ERROR: process id %d does not exist in the DB\n",Config->process_id);
      }
      create_initfail_file();
      remove_lock();
      exit(1);
    }
    int status = db_process_get_status(Config->process_id);
    if (status!=DB_STATUS_IDLE) {
      printf("ERROR: process id %d is not in IDLE (%d) status (status=%d)\n",Config->process_id,DB_STATUS_IDLE,status);
      create_initfail_file();
      remove_lock();
      exit(1);
    }

    // Save the process configuration to DB -> Now done by RunControl
    //save_config();

  }

  // Update process status
  if (Config->run_number) {
    printf("- Setting process status to INITIALIZING (%d) in DB\n",DB_STATUS_INITIALIZING);
    db_process_set_status(Config->process_id,DB_STATUS_INITIALIZING);
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
      create_initfail_file();
      if (Config->run_number) {
	printf("- Setting process status to INIT_FAIL (%d) in DB\n",DB_STATUS_INIT_FAIL);
	db_process_set_status(Config->process_id,DB_STATUS_INIT_FAIL);
      }
      remove_lock();
      exit(1);
    }

    // Initialize and configure digitizer
    printf("\n=== Initialize digitizer ===\n");
    if ( DAQ_init() ) {
      printf("*** ERROR *** Problem while initializing V1742 digitizer. Exiting.\n");
      create_initfail_file();
      if (Config->run_number) {
	printf("- Setting process status to INIT_FAIL (%d) in DB\n",DB_STATUS_INIT_FAIL);
	db_process_set_status(Config->process_id,DB_STATUS_INIT_FAIL);
      }
      remove_lock();
      exit(1);
    }

    // Handle data acquisition
    printf("\n=== Starting data acquisition ===\n");
    rc = DAQ_readdata();
    if ( rc == 0 ) {
      printf("=== Run finished ===\n");
      if (Config->run_number) {
	printf("- Setting process status to FINISHED (%d) in DB\n",DB_STATUS_FINISHED);
	db_process_set_status(Config->process_id,DB_STATUS_FINISHED);
      }
    } else if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing DAQ process. Exiting.\n");
      create_initfail_file();
      if (Config->run_number) {
	printf("- Setting process status to INIT_FAIL (%d) in DB\n",DB_STATUS_INIT_FAIL);
	db_process_set_status(Config->process_id,DB_STATUS_INIT_FAIL);
      }
      remove_lock();
      exit(1);
    } else if ( rc == 2 ) {
      printf("*** ERROR *** Data acquistion ended with an error. Please check log file for details. Exiting.\n");
      if (Config->run_number) {
	printf("- Setting process status to RUN_FAIL (%d) in DB\n",DB_STATUS_RUN_FAIL);
	db_process_set_status(Config->process_id,DB_STATUS_RUN_FAIL);
      }
      remove_lock();
      exit(1);
    } else if ( rc == 3 ) {
      printf("=== Run aborted before starting DAQ ===\n");
      if (Config->run_number) {
	printf("- Setting process status to ABORTED (%d) in DB\n",DB_STATUS_ABORTED);
	db_process_set_status(Config->process_id,DB_STATUS_ABORTED);
      }
    } else {
      printf("=== DAQ reported unknown return code %d ===\n",rc);
      if (Config->run_number) {
	printf("- Setting process status to UNKNOWN (%d) in DB\n",DB_STATUS_UNKNOWN);
	db_process_set_status(Config->process_id,DB_STATUS_UNKNOWN);
      }
    }

    // Final reset of the digitizer
    printf("\n=== Reset digitizer and close connection ===\n");
    if ( DAQ_close() ) {
      printf("*** ERROR *** Final reset of digitizer ended with an error. Exiting.\n");
      if (Config->run_number) {
	printf("- Setting process status to CLOSE_FAIL (%d) in DB\n",DB_STATUS_CLOSE_FAIL);
	db_process_set_status(Config->process_id,DB_STATUS_CLOSE_FAIL);
      }
      remove_lock();
      exit(1);
    }

  } else if ( strcmp(Config->process_mode,"FAKE")==0 ) {

    // Show some startup message
    if (Config->run_number == 0) {
      printf("\n=== Starting PadmeDAQ FAKE event generation for dummy run ===\n");
    } else {
      printf("\n*** ERROR *** Cannot run PadmeDAQ in FAKE mode for a real run. Exiting.\n");
      create_initfail_file();
      remove_lock();
      exit(1);
    }

    // Start generation of FAKE events
    rc = FAKE_readdata();
    if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing FAKE process. Exiting.\n");
      create_initfail_file();
      remove_lock();
      exit(1);
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

    // Handle zero suppression
    printf("\n=== Starting zero suppression ===\n");
    rc = ZSUP_readdata();
    if ( rc == 0 ) {
      printf("\n=== ZSUP process ended ===\n");
      if (Config->run_number) {
	printf("- Setting process status to FINISHED (%d) in DB\n",DB_STATUS_FINISHED);
	db_process_set_status(Config->process_id,DB_STATUS_FINISHED);
      }
    } else if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing ZSUP process. Exiting.\n");
      create_initfail_file();
      if (Config->run_number) {
	printf("- Setting process status to INIT_FAIL (%d) in DB\n",DB_STATUS_INIT_FAIL);
	db_process_set_status(Config->process_id,DB_STATUS_INIT_FAIL);
      }
      remove_lock();
      exit(1);
    } else if ( rc == 2 ) {
      printf("*** ERROR *** Zero suppression ended with an error. Please check log file for details. Exiting.\n");
      if (Config->run_number) {
	printf("- Setting process status to RUN_FAIL (%d) in DB\n",DB_STATUS_RUN_FAIL);
	db_process_set_status(Config->process_id,DB_STATUS_RUN_FAIL);
      }
      remove_lock();
      exit(1);
    } else if ( rc == 3 ) {
      printf("=== Run aborted before starting ZSUP ===\n");
      if (Config->run_number) {
	printf("- Setting process status to ABORTED (%d) in DB\n",DB_STATUS_ABORTED);
	db_process_set_status(Config->process_id,DB_STATUS_ABORTED);
      }
    } else {
      printf("=== ZSUP reported unknown return code %d ===\n",rc);
      if (Config->run_number) {
	printf("- Setting process status to UNKNOWN (%d) in DB\n",DB_STATUS_UNKNOWN);
	db_process_set_status(Config->process_id,DB_STATUS_UNKNOWN);
      }
    }

  }

  // Close DB connection
  if ( Config->run_number ) {
    if ( db_end() != DB_OK ) {
      printf("*** ERROR *** DB close procedure ended with an error. Please check log file for details. Exiting.\n");
      remove_lock();
      exit(1);
    }
  }

  // Remove lock file
  remove_lock();

  // Clean up configuration
  end_config();

  // All is done: exit
  printf("\n=== May the force be with you. Bye! ===\n");
  exit(0);

}
