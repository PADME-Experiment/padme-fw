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

  // Show welcome message
  printf("=======================================\n");
  printf("=== Welcome to the PADME DAQ system ===\n");
  printf("=======================================\n");

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
    create_initfail_file();
    exit(1);
  }


  // Update process status
  printf("- Setting process status to INITIALIZING (%d)\n",DB_STATUS_INITIALIZING);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INITIALIZING);

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
      printf("- Setting process status to INIT_FAIL (%d)\n",DB_STATUS_INIT_FAIL);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INIT_FAIL);
      remove_lock();
      exit(1);
    }

    // Initialize and configure digitizer
    printf("\n=== Initialize digitizer ===\n");
    if ( DAQ_init() ) {
      printf("*** ERROR *** Problem while initializing V1742 digitizer. Exiting.\n");
      create_initfail_file();
      printf("- Setting process status to INIT_FAIL (%d)\n",DB_STATUS_INIT_FAIL);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INIT_FAIL);
      remove_lock();
      exit(1);
    }

    // Handle data acquisition
    printf("\n=== Starting data acquisition ===\n");
    rc = DAQ_readdata();
    if ( rc == 0 ) {
      printf("=== Run finished ===\n");
      printf("- Setting process status to FINISHED (%d) in DB\n",DB_STATUS_FINISHED);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_FINISHED);
    } else if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing DAQ process. Exiting.\n");
      create_initfail_file();
      printf("- Setting process status to INIT_FAIL (%d)\n",DB_STATUS_INIT_FAIL);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INIT_FAIL);
      remove_lock();
      exit(1);
    } else if ( rc == 2 ) {
      printf("*** ERROR *** Data acquistion ended with an error. Please check log file for details. Exiting.\n");
      printf("- Setting process status to RUN_FAIL (%d)\n",DB_STATUS_RUN_FAIL);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_RUN_FAIL);
      remove_lock();
      exit(1);
    } else if ( rc == 3 ) {
      printf("=== Run aborted before starting DAQ ===\n");
      printf("- Setting process status to ABORTED (%d)\n",DB_STATUS_ABORTED);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_ABORTED);
    } else {
      printf("=== DAQ reported unknown return code %d ===\n",rc);
      printf("- Setting process status to UNKNOWN (%d)\n",DB_STATUS_UNKNOWN);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_UNKNOWN);
    }

    // Final reset of the digitizer
    printf("\n=== Reset digitizer and close connection ===\n");
    if ( DAQ_close() ) {
      printf("*** ERROR *** Final reset of digitizer ended with an error. Exiting.\n");
      printf("- Setting process status to CLOSE_FAIL (%d)\n",DB_STATUS_CLOSE_FAIL);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_CLOSE_FAIL);
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
      printf("- Setting process status to FINISHED (%d)\n",DB_STATUS_FINISHED);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_FINISHED);
    } else if ( rc == 1 ) {
      printf("*** ERROR *** Problem while initializing ZSUP process. Exiting.\n");
      create_initfail_file();
      printf("- Setting process status to INIT_FAIL (%d)\n",DB_STATUS_INIT_FAIL);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INIT_FAIL);
      remove_lock();
      exit(1);
    } else if ( rc == 2 ) {
      printf("*** ERROR *** Zero suppression ended with an error. Please check log file for details. Exiting.\n");
      printf("- Setting process status to RUN_FAIL (%d)\n",DB_STATUS_RUN_FAIL);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_RUN_FAIL);
      remove_lock();
      exit(1);
    } else if ( rc == 3 ) {
      printf("=== Run aborted before starting ZSUP ===\n");
      printf("- Setting process status to ABORTED (%d)\n",DB_STATUS_ABORTED);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_ABORTED);
    } else {
      printf("=== ZSUP reported unknown return code %d ===\n",rc);
      printf("- Setting process status to UNKNOWN (%d)\n",DB_STATUS_UNKNOWN);
      printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_UNKNOWN);
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
