#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "DB.h"
#include "Config.h"
#include "PEvent.h"

#include "ZSUP.h"

// Handle zero suppression
int ZSUP_readdata ()
{

  // Input/Output event buffers
  int maxPEvtSize;
  char *inEvtBuffer = NULL;
  char *outEvtBuffer = NULL;

  // Input/Output file handles
  int inFileHandle;
  int outFileHandle;

  // If this is a real run...
  if ( Config->run_number ) {

    //... connect to DB and create new process
    //if ( db_init(Config->db_file) != DB_OK ) return 1;
    if ( db_init() != DB_OK ) return 1;
    //if ( db_process_create(Config->run_number,Config->board_id) != DB_OK ) return 1;
    //Config->process_id = db_get_process_id(Config->run_number,Config->board_id);
    //if ( Config->process_id <= 0 ) return 1;

    // Save configuration to DB (by now Config knows the process_id)
    save_config();

  }

  // Allocate buffer to hold input and output event structures (same max size)

  maxPEvtSize = (PEVT_HEADER_LEN + 4*(PEVT_GRPHEAD_LEN + 512 + PEVT_GRPTTT_LEN) + 32*512)*4;

  inEvtBuffer = (char *)malloc(maxPEvtSize);
  if (inEvtBuffer == NULL) {
    printf("Unable to allocate input event buffer of size %d\n",maxPEvtSize);
    return 1;
  }
  printf("- Allocated input event buffer with size %d\n",maxPEvtSize);

  outEvtBuffer = (char *)malloc(maxPEvtSize);
  if (outEvtBuffer == NULL) {
    printf("Unable to allocate output event buffer of size %d\n",maxPEvtSize);
    return 1;
  }
  printf("- Allocated output event buffer with size %d\n",maxPEvtSize);

  // Open virtual file for input data stream
  printf("- Opening input stream from file '%s'\n",Config->input_stream);
  inFileHandle = open(Config->input_stream,O_RDONLY, S_IRUSR | S_IWUSR);
  if (inFileHandle == -1) {
    printf("ERROR - Unable to open input stream '%s' for reading.\n",Config->input_stream);
    return 2;
  }

  // Here goes the main code //

  // Close input stream file
  if (close(inFileHandle) == -1) {
    printf("ERROR - Unable to close input stream '%s'.\n",Config->input_stream);
    return 2;
  };

  // Deallocate input/output event buffer
  free(inEvtBuffer);
  free(outEvtBuffer);

  /*
  // Tell DB that the process has ended
  if ( Config->run_number ) {
    if ( db_process_close(Config->process_id,t_daqstop) != DB_OK ) return 2;
  }
  */

  /*
  // Give some final report
  evtReadPerSec = 0.;
  sizeReadPerSec = 0.;
  evtPerSec = 0.;
  sizePerSec = 0.;
  t_daqtotal = t_daqstop-t_daqstart;
  if (t_daqtotal>0) {
    evtReadPerSec = 1.*totalReadEvents/t_daqtotal;
    sizeReadPerSec = totalReadSize/(t_daqtotal*1024.);
    evtPerSec = 1.*totalEvents/t_daqtotal;
    sizePerSec = totalSize/(t_daqtotal*1024.);
  }
  printf("\n=== DAQ ending on %s ===\n",format_time(t_daqstop));
  printf("Total running time: %d secs\n",(int)t_daqtotal);
  printf("Total number of events acquired: %lu - %6.2f events/s\n",totalReadEvents,evtReadPerSec);
  printf("Total size of data acquired: %lu B - %6.2f KB/s\n",totalReadSize,sizeReadPerSec);
  printf("Total number of events written: %lu - %6.2f events/s\n",totalEvents,evtPerSec);
  printf("Total size of data written: %lu B - %6.2f KB/s\n",totalSize,sizePerSec);
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("=== Files created =======================================\n");
    for(i=0;i<fileIndex;i++) {
      printf("'%s' %u %lu",fileName[i],fileEvents[i],fileSize[i]);
      printf(" %s",format_time(fileTOpen[i])); // Optimizer effect! :)
      printf(" %s\n",format_time(fileTClose[i]));
    }
  }
  printf("=========================================================\n");
  */

  // Close DB file
  if ( Config->run_number ) {
    if ( db_end() != DB_OK ) return 2;
  }

  return 0;

}
