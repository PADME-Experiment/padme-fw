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

  // If this is a real run...
  if ( Config->run_number ) {

    //... connect to DB and create new process
    //if ( db_init(Config->db_file) != DB_OK ) return 1;
    if ( db_init() != DB_OK ) return 1;
    if ( db_process_create(Config->run_number,Config->board_id) != DB_OK ) return 1;
    Config->process_id = db_get_process_id(Config->run_number,Config->board_id);
    if ( Config->process_id <= 0 ) return 1;

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

  return 0;

}
