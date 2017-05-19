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

#include "CAENDigitizer.h"

#include "DB.h"
#include "Config.h"
#include "PEvent.h"

#include "DAQ.h"

#define TIME_TAG_LEN     20
#define MAX_FILENAME_LEN MAX_DATA_FILE_LEN+TIME_TAG_LEN

#define MAX_N_OUTPUT_FILES 10240

// Global variables

int Handle; // Handle for V1742 digitizer

int InBurst; // =0 DAQ not running, >0 DAQ running

int BreakSignal = 0; // If >0 an interrupt was received and DAQ should exit

// Function to handle interrupts
void termination_handler (int signum)
{
  if (InBurst > 0) {

    // If DAQ is running, don't stop abruptly
    // Just tell the main to exit when DAQ allows it
    printf ("\n### Interrupt Received while in burst mode - Signal: %d\n", signum);
    printf ("--- Waiting for DAQ to terminate gracefully... \n");
    if (signum!=0) {
      BreakSignal = signum;
    } else {
      BreakSignal = -1;
    }
    // Ignore additional interrupts
    signal (SIGINT,  SIG_IGN);
    signal (SIGHUP,  SIG_IGN);
    signal (SIGTERM, SIG_IGN);
    signal (SIGFPE,  SIG_DFL);

  } else {

    printf ("\n### Interrupt Received in idle mode - Signal: %d\n", signum);
    printf ("--- Terminating DAQ program... \n");
    signal (SIGINT,  SIG_DFL); // Reset interrupt handler
    signal (SIGHUP,  SIG_DFL);
    signal (SIGTERM, SIG_DFL);
    signal (SIGFPE,  SIG_DFL);
    DAQ_close ();
    //remove_lock(); // will leave dangling lock file
    printf("= Hope you enjoyed the ride. Bye!\n");
    exit(0);

  }
}

// Set interrupt handling functions
void set_signal_handlers()
{
  printf ("Trap signals:");
  signal (SIGINT, termination_handler);
  printf (" SIGINT %d", SIGINT);
  signal (SIGHUP, termination_handler);
  printf (" SIGHUP %d", SIGHUP);
  signal (SIGTERM, termination_handler);
  printf (" SIGTERM %d", SIGTERM);
  //signal (SIGUSR2, termination_handler); // do we need this?
  //printf (" %d", SIGUSR2);
  signal (SIGFPE, termination_handler);
  printf (" SIGFPE %d", SIGFPE);
  printf ("\n");
}

// Return file name given the file open time. Return 0 if OK, <>0 error
int generate_filename(char* name, const time_t time) {
  struct tm* t = localtime(&time);
  sprintf(name,"%s_%.4d_%.2d_%.2d_%.2d_%.2d_%.2d",
	  Config->data_file,
	  1900+t->tm_year, 1+t->tm_mon, t->tm_mday,
	  t->tm_hour,      t->tm_min,   t->tm_sec);
  return 0;
}

// Write time (in secs) to a string with standard formatting
char* format_time(const time_t time) {
  static char tform[20];
  struct tm* t = localtime(&time);
  sprintf(tform,"%.4d/%.2d/%.2d %.2d:%.2d:%.2d",
	  1900+t->tm_year, 1+t->tm_mon, t->tm_mday,
	  t->tm_hour,      t->tm_min,   t->tm_sec);
  return tform;
}

// Get LinkNum (A2818 board local address) from board_id
int get_LinkNum()
{
  // This algorithm will evolve when more A3818 boards will be used
  if (Config->board_id == 0) return 0;
  // Board 1 and board 2 currently share the same optical channel
  // Only one of the two can be used at the same time
  //if (Config->board_id == 1) return 1;
  //if (Config->board_id == 2) return 1;
  // Daisy chain test
  if (Config->board_id == 1) return 0;
  if (Config->board_id == 2) return 0;
  return -1;
}

// Get ConetNode (ADC board address within A3818 board) from board_id
int get_ConetNode()
{
  // This algorithm will evolve when more A3818 boards will be used
  //if ( (Config->board_id < 0) || (Config->board_id >= 8) )  { return -1; }
  //return Config->board_id;
  if (Config->board_id == 0) return 0;
  //if (Config->board_id == 1) return 0;
  //if (Config->board_id == 2) return 0;
  // Daisy chain test
  if (Config->board_id == 1) return 1;
  if (Config->board_id == 2) return 2;
  return -1;
}

// Handle initial connection to digitizer. Return 0 if OK, >0 if error
int DAQ_connect ()
{
  CAEN_DGTZ_ErrorCode ret;
  CAEN_DGTZ_BoardInfo_t boardInfo;
  int linkNum,conetNode;

  // Use board_id to get board address
  linkNum = get_LinkNum();
  //printf("LinkNum returned %d\n",linkNum);
  if (linkNum<0) {
    printf("Unable to obtain LinkNum from board id\n");
    return 1;
  }
  conetNode = get_ConetNode();
  //printf("ConetNode returned %d\n",conetNode);
  if (conetNode<0) {
    printf("Unable to obtain ConetNode from board id\n");
    return 1;
  }

  // Open connection to digitizer and initialize Handle global variable
  ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink,linkNum,conetNode,0,&Handle);
  if (ret != CAEN_DGTZ_Success) {
    printf("Unable to connect to digitizer. Error code: %d\n",ret);
    return 1;
  }

  // Set signal handlers to make sure digitizer is reset before exiting
  set_signal_handlers();

  // Show information on connected digitizer
  ret = CAEN_DGTZ_GetInfo(Handle, &boardInfo);
  if (ret != CAEN_DGTZ_Success) {
    printf("Unable to retrieve information about the digitizer. Error code: %d\n",ret);
    return 1;
  }
  printf("- Connected to CAEN Digitizer Model %s (model %d family %d)\n",
         boardInfo.ModelName,boardInfo.Model,boardInfo.FamilyCode);
  printf("- %u channels, %u bits ADC samples\n",boardInfo.Channels,boardInfo.ADC_NBits);
  printf("- Serial number: %u\n", boardInfo.SerialNumber);
  printf("- ROC FPGA Release: %s\n", boardInfo.ROC_FirmwareRel);
  printf("- AMC FPGA Release: %s\n", boardInfo.AMC_FirmwareRel);
  printf("- PCB revision number: %u\n", boardInfo.PCB_Revision);

  return 0;
}

// Handle initialization of the digitizer. Return 0 if OK, >0 if error
int DAQ_init ()
{
  CAEN_DGTZ_ErrorCode ret;
  CAEN_DGTZ_AcqMode_t mode;
  uint32_t reg,data;
  uint32_t tr_offset,tr_threshold;
  CAEN_DGTZ_TriggerPolarity_t tr_polarity;
  CAEN_DGTZ_DRS4Frequency_t sampfreq;

  uint32_t kk;

  InBurst = 0;

  // Reset digitizer to its default values
  printf("- Resetting digitizer\n");
  ret = CAEN_DGTZ_Reset(Handle);
  if (ret != CAEN_DGTZ_Success) {
    printf("ERROR - Unable to reset digitizer. Error code: %d\n",ret);
    return 1;
  }
  
  // Set sampling frequency to 1GHz (0=5GHz, 1=2.5GHz, 2=1GHz)
  /*
  reg = 0x80D8;
  //data = 2;
  data = CAEN_DGTZ_DRS4_1GHz;
  printf("- Setting sampling frequency to 1GHz (reg 0x%04x) write %d",reg,data);
  ret = CAEN_DGTZ_WriteRegister(Handle,reg,data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set sampling frequency. Error code: %d\n",ret);
    return 1;
  }
  // Read back per-group sampling frequency
  printf(" read");
  for(g=0;g<MAX_X742_GROUP_SIZE;g++) {
    reg = 0x10D8 + (g << 8); // 0x10D8, 0x11D8, 0x12D8, ox13D8
    ret = CAEN_DGTZ_ReadRegister(Handle,reg,&data);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nUnable to read sampling frequency for group %d (reg 0x%04x). Error code: %d\n",g,reg,ret);
      return 1;
    }
    printf(" %d",data);
  }
  printf("\n");
  */
  //data = CAEN_DGTZ_DRS4_1GHz;
  if (Config->drs4_sampfreq == 2) {
    printf("- Setting DRS4 sampling frequency to 1GHz. Write %d",CAEN_DGTZ_DRS4_1GHz);
    //data = CAEN_DGTZ_DRS4_1GHz;
    sampfreq = CAEN_DGTZ_DRS4_1GHz;
  } else if (Config->drs4_sampfreq == 1) {
    printf("- Setting DRS4 sampling frequency to 2.5GHz. Write %d",CAEN_DGTZ_DRS4_2_5GHz);
    //data = CAEN_DGTZ_DRS4_2_5GHz;
    sampfreq = CAEN_DGTZ_DRS4_2_5GHz;
  } else if  (Config->drs4_sampfreq == 0) {
    printf("- Setting DRS4 sampling frequency to 5GHz. Write %d",CAEN_DGTZ_DRS4_5GHz);
    //data = CAEN_DGTZ_DRS4_5GHz;
    sampfreq = CAEN_DGTZ_DRS4_5GHz;
  } else {
    printf("\nERROR - DRS4 sampling frequency configuration parameter set to %d\n",Config->drs4_sampfreq);
    return 1;
  }
  //ret = CAEN_DGTZ_SetDRS4SamplingFrequency(Handle,data);
  ret = CAEN_DGTZ_SetDRS4SamplingFrequency(Handle,sampfreq);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set sampling frequency. Error code: %d\n",ret);
    return 1;
  }
  //ret = CAEN_DGTZ_GetDRS4SamplingFrequency(Handle,&data);
  ret = CAEN_DGTZ_GetDRS4SamplingFrequency(Handle,&sampfreq);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read DRS4 sampling frequency. Error code: %d\n",ret);
    return 1;
  }
  //printf(" read %d\n",data);
  printf(" read %d\n",sampfreq);

  /*
  // Set number of samples to 1024 (0=1024, 1=520, 2=256, 3=136)
  reg = 0x8020;
  data = 0;
  printf("- Setting number of samples to 1024 (reg 0x%04x) write %d",reg,data);
  ret = CAEN_DGTZ_WriteRegister(Handle,reg,data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set number of samples. Error code: %d\n",ret);
    return 1;
  }
  ret = CAEN_DGTZ_ReadRegister(Handle,reg,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read number of samples. Error code: %d\n",ret);
    return 1;
  }
  printf(" read %d\n",data);
  */

  // Set record length (i.e. number of samples). Can be 1024, 520, 256, or 136
  ret = CAEN_DGTZ_GetRecordLength(Handle,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read record length. Error code: %d\n",ret);
    return 1;
  }
  printf("- Setting record length (number of samples). def %d",data);
  data = 1024;
  printf(" write %d",data);
  ret = CAEN_DGTZ_SetRecordLength(Handle,data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set record length. Error code: %d\n",ret);
    return 1;
  }
  ret = CAEN_DGTZ_GetRecordLength(Handle,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read record length. Error code: %d\n",ret);
    return 1;
  }
  printf(" read %d\n",data);

  // Enable group(s) of channels to read
  if (Config->group_enable_mask & 0x1) printf("- Enabling group 0, channels  0- 7\n");
  if (Config->group_enable_mask & 0x2) printf("- Enabling group 1, channels  8-15\n");
  if (Config->group_enable_mask & 0x4) printf("- Enabling group 2, channels 16-23\n");
  if (Config->group_enable_mask & 0x8) printf("- Enabling group 3, channels 24-31\n");

  reg = 0x8120;
  printf("- Setting group enable mask (reg 0x%04x) write 0x%01x",reg,Config->group_enable_mask);
  ret = CAEN_DGTZ_WriteRegister(Handle,reg,Config->group_enable_mask);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set group enable mask. Error code: %d\n",ret);
    return 1;
  }
  ret = CAEN_DGTZ_ReadRegister(Handle,reg,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read group enable maskd. Error code: %d\n",ret);
    return 1;
  }
  printf(" read 0x%01x\n",data);
  /*
  printf("- Setting group enable mask. Write 0x%01x",Config->group_enable_mask);
  ret = CAEN_DGTZ_SetGroupEnableMask(Handle,Config->group_enable_mask);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set group enable mask. Error code: %d\n",ret);
    return 1;
  }
  ret = CAEN_DGTZ_GetGroupEnableMask(Handle,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read group enable maskd. Error code: %d\n",ret);
    return 1;
  }
  printf(" read 0x%01x\n",data);
  */

  // Uncomment to enable ADC test mode
  // CAEN_DGTZ_WriteRegister(Handle,0x8004,8);
  // printf("WARNING SAWTHOOTH ACTIVE!!\n");
  // printf("WARNING SAWTHOOTH ACTIVE!!\n"); 
  // printf("WARNING SAWTHOOTH ACTIVE!!\n");  
  // printf("WARNING SAWTHOOTH ACTIVE!!\n");

  // Set DC offsets for enabled channels
  for(kk=0;kk<32;kk++){
    if (Config->group_enable_mask & (0x1 << (kk/8))) {
      printf("- Channel %2d offset -",kk);
      ret = CAEN_DGTZ_GetChannelDCOffset(Handle,kk,&data); 
      if (ret != CAEN_DGTZ_Success) {
	printf("\nERROR - Unable to read default offset for channel %d. Error code: %d\n",kk,ret);
	return 1;
      }
      printf(" default 0x%04x",data);
      printf(" write 0x%04x",Config->offset_ch[kk]);
      ret = CAEN_DGTZ_SetChannelDCOffset(Handle,kk,Config->offset_ch[kk]); 
      if (ret != CAEN_DGTZ_Success) {
	printf("\nERROR - Unable to set offset for channel %d. Error code: %d\n",kk,ret);
	return 1;
      }
      ret = CAEN_DGTZ_GetChannelDCOffset(Handle,kk,&data); 
      if (ret != CAEN_DGTZ_Success) {
	printf("\nERROR - Unable to read offset for channel %d. Error code: %d\n",kk,ret);
	return 1;
      }
      printf(" read 0x%04x\n",data);
    }
  }

  // Set post trigger size (0% - 100%)
  ret = CAEN_DGTZ_GetPostTriggerSize(Handle,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read post trigger size. Error code: %d\n",ret);
    return 1;
  }
  printf("- Setting post trigger size. def %d",data);
  data = Config->post_trigger_size;
  printf(" write %d",data);
  ret = CAEN_DGTZ_SetPostTriggerSize(Handle,data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set post trigger size. Error code: %d\n",ret);
    return 1;
  }
  ret = CAEN_DGTZ_GetPostTriggerSize(Handle,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read post trigger size. Error code: %d\n",ret);
    return 1;
  }
  printf(" read %d\n",data);

  // Configure trigger mode (0=external, 1=fast, 2=software)
  if ( Config->trigger_mode == 0 ) {

    // Disable software triggers
    printf("- Disabling software triggers\n");
    ret = CAEN_DGTZ_SetSWTriggerMode(Handle,CAEN_DGTZ_TRGMODE_DISABLED);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to disable software triggers. Error code: %d\n",ret);
      return 1;
    }

    // Disable fast trigger
    printf("- Disabling fast triggers\n");
    ret = CAEN_DGTZ_SetFastTriggerMode(Handle,CAEN_DGTZ_TRGMODE_DISABLED);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to disable fast trigger. Error code: %d\n",ret);
      return 1;
    }

    // Enable external trigger and transmit it to trigger output
    printf("- Enabling external triggers\n");
    ret = CAEN_DGTZ_SetExtTriggerInputMode(Handle,CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to enable external trigger. Error code: %d\n",ret);
      return 1;
    }

    // Set external trigger signals to NIM or TTL standard according to configuration
    printf("- Setting trigger IO level to %s\n",Config->trigger_iolevel);
    if ( strcmp(Config->trigger_iolevel,"NIM") == 0 ) {
      ret = CAEN_DGTZ_SetIOLevel(Handle,CAEN_DGTZ_IOLevel_NIM);
    } else if ( strcmp(Config->trigger_iolevel,"TTL") == 0 ) {
      ret = CAEN_DGTZ_SetIOLevel(Handle,CAEN_DGTZ_IOLevel_TTL);
    } else {
      printf("ERROR - trigger_iolevel is set to %s (this should not happen!)",Config->trigger_iolevel);
      return 1;
    }
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to set trigger signals IOlevel to %s. Error code: %d\n",Config->trigger_iolevel,ret);
      return 1;
    }

  } else if ( Config->trigger_mode == 1 ) {

    // Disable software triggers
    printf("- Disabling software triggers\n");
    ret = CAEN_DGTZ_SetSWTriggerMode(Handle,CAEN_DGTZ_TRGMODE_DISABLED);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to disable software triggers. Error code: %d\n",ret);
      return 1;
    }

    // Disable external triggers
    printf("- Disabling external triggers\n");
    ret = CAEN_DGTZ_SetExtTriggerInputMode(Handle,CAEN_DGTZ_TRGMODE_DISABLED);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to disable external trigger. Error code: %d\n",ret);
      return 1;
    }

    // Enable fast trigger
    printf("- Enabling fast triggers\n");
    ret = CAEN_DGTZ_SetFastTriggerMode(Handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to enable fast trigger. Error code: %d\n",ret);
      return 1;
    }

    // Enable fast trigger readout
    printf("- Enabling fast triggers readout\n");
    ret = CAEN_DGTZ_SetFastTriggerDigitizing(Handle,CAEN_DGTZ_ENABLE);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to enable fast trigger readout. Error code: %d\n",ret);
      return 1;
    }

    // Set fast trigger polarization (TTL -> rising edge, NIM -> falling edge)

    if ( strcmp(Config->trigger_iolevel,"NIM") == 0 ) {
      tr_polarity = CAEN_DGTZ_TriggerOnFallingEdge;
    } else if ( strcmp(Config->trigger_iolevel,"TTL") == 0 ) {
      tr_polarity = CAEN_DGTZ_TriggerOnRisingEdge;
    } else {
      printf("ERROR - trigger_iolevel is set to %s (this should not happen!)",Config->trigger_iolevel);
      return 1;
    }

    printf("- Setting fast trigger polarization for TR0 & TR1, write 0x%04x", tr_polarity);
    ret = CAEN_DGTZ_SetTriggerPolarity(Handle,0,tr_polarity);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nERROR - Unable to set fast trigger polarity for TR0 & TR1. Error code: %d\n",ret);
      return 1;
    }
    //ret = CAEN_DGTZ_GetTriggerPolarity(Handle,0,&data);
    ret = CAEN_DGTZ_GetTriggerPolarity(Handle,0,&tr_polarity);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nERROR - Unable to read fast trigger polarity for TR0 & TR1. Error code: %d\n",ret);
      return 1;
    }
    //printf(" read 0x%04x\n",data);
    printf(" read 0x%04x\n",tr_polarity);

    // Set fast trigger offset for TR0 and TR1 (see V1742 manual for details)

    if ( strcmp(Config->trigger_iolevel,"NIM") == 0 ) {
      tr_offset = 0x8000;
      tr_threshold = 0x51c6;
    } else if ( strcmp(Config->trigger_iolevel,"TTL") == 0 ) {
      tr_offset = 0xA800;
      tr_threshold = 0x6666;
    } else {
      printf("ERROR - trigger_iolevel is set to %s (this should not happen!)",Config->trigger_iolevel);
      return 1;
    }

    printf("- Setting fast trigger DC offset for TR0, write 0x%04x", tr_offset);
    ret = CAEN_DGTZ_SetGroupFastTriggerDCOffset(Handle,0,tr_offset);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nERROR - Unable to set fast trigger DC offset for TR0. Error code: %d\n",ret);
      return 1;
    }
    ret = CAEN_DGTZ_GetGroupFastTriggerDCOffset(Handle,0,&data);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nERROR - Unable to read fast trigger DC offset for TR0. Error code: %d\n",ret);
      return 1;
    }
    printf(" read 0x%04x\n",data);

    printf("- Setting fast trigger DC offset for TR1, write 0x%04x", tr_offset);
    ret = CAEN_DGTZ_SetGroupFastTriggerDCOffset(Handle,2,tr_offset);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nERROR - Unable to set fast trigger DC offset for TR1. Error code: %d\n",ret);
      return 1;
    }
    ret = CAEN_DGTZ_GetGroupFastTriggerDCOffset(Handle,2,&data);
    if (ret != CAEN_DGTZ_Success) {
      printf("\nERROR - Unable to read fast trigger DC offset for TR1. Error code: %d\n",ret);
      return 1;
    }
    printf(" read 0x%04x\n",data);

    // Set fast trigger threshold for TR0 and TR1 (see V1742 manual for details)

    printf("- Setting fast trigger threshold for TR0, write 0x%04x",tr_threshold);
    ret = CAEN_DGTZ_SetGroupFastTriggerThreshold(Handle,0,tr_threshold);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to set fast trigger threshold for TR0. Error code: %d\n",ret);
      return 1;
    }
    ret = CAEN_DGTZ_GetGroupFastTriggerThreshold(Handle,0,&data);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to read fast trigger threshold for TR0. Error code: %d\n",ret);
      return 1;
    }
    printf(" read 0x%04x\n",data);

    printf("- Setting fast trigger threshold for TR1, write 0x%04x",tr_threshold);
    ret = CAEN_DGTZ_SetGroupFastTriggerThreshold(Handle,2,tr_threshold);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to set fast trigger threshold for TR1. Error code: %d\n",ret);
      return 1;
    }
    ret = CAEN_DGTZ_GetGroupFastTriggerThreshold(Handle,2,&data);
    if (ret != CAEN_DGTZ_Success) {
      printf("ERROR - Unable to read fast trigger threshold for TR1. Error code: %d\n",ret);
      return 1;
    }
    printf(" read 0x%04x\n",data);

  } else if ( Config->trigger_mode == 2 ) {

    printf("- Software triggers not implemented, yet\n");
    return 1;

  }


  // Set max number of events to transfer in a single readout
  ret = CAEN_DGTZ_GetMaxNumEventsBLT(Handle,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("ERROR - Unable to read max number of events BLT. Error code: %d\n",ret);
    return 1;
  }
  printf("- Setting max number of events BLT. def %d",data);
  data = Config->max_num_events_blt;
  printf(" write %d",data);
  ret = CAEN_DGTZ_SetMaxNumEventsBLT(Handle,data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to set max number of events BLT. Error code: %d\n",ret);
    return 1;
  }
  ret = CAEN_DGTZ_GetMaxNumEventsBLT(Handle,&data);
  if (ret != CAEN_DGTZ_Success) {
    printf("\nERROR - Unable to read max number of events BLT. Error code: %d\n",ret);
    return 1;
  }
  printf(" read %d\n",data);

  if ( Config->drs4corr_enable ) {

    // Load DRS4 correction tables used to decode the X742 event
    if (Config->drs4_sampfreq == 0) {
      ret = CAEN_DGTZ_LoadDRS4CorrectionData(Handle,CAEN_DGTZ_DRS4_5GHz);
    } else if (Config->drs4_sampfreq == 1) {
      ret = CAEN_DGTZ_LoadDRS4CorrectionData(Handle,CAEN_DGTZ_DRS4_2_5GHz);
    } else if (Config->drs4_sampfreq == 2) {
      ret = CAEN_DGTZ_LoadDRS4CorrectionData(Handle,CAEN_DGTZ_DRS4_1GHz);
    }
    if (ret != CAEN_DGTZ_Success) {
      printf("Unable to load correction tables for DRS4 chip. Error code: %d\n",ret);
      return 1;
    }
    printf("- Loaded DRS4 correction tables from X742 flash memory\n");

    // Enable DRS4 corrections to sampled data
    ret = CAEN_DGTZ_EnableDRS4Correction(Handle);
    if (ret != CAEN_DGTZ_Success) {
      printf("Unable to enable DRS4 corrections to sampled data. Error code: %d\n",ret);
      return 1;
    }
    printf("- Enabled DRS4 corrections to sampled data\n");

  } else {

    // Disable DRS4 corrections to sampled data
    ret = CAEN_DGTZ_DisableDRS4Correction(Handle);
    if (ret != CAEN_DGTZ_Success) {
      printf("Unable to disable DRS4 corrections to sampled data. Error code: %d\n",ret);
      return 1;
    }
    printf("WARNING: DRS4 corrections to sampled data are disabled!\n");

  }

  // Set method used to start and stop data acquisition
  if (Config->startdaq_mode == 0) {
    mode = CAEN_DGTZ_SW_CONTROLLED; // Start and stop from software commands
  } else if (Config->startdaq_mode == 1) {
    mode = CAEN_DGTZ_S_IN_CONTROLLED; // Start and stop from signals on S_IN connector
  } else if (Config->startdaq_mode == 2) {
    //mode = CAEN_DGTZ_FIRST_TRG_CONTROLLED; // Start on first trigger, stop from sw command
    printf("ERROR - startdaq_mode parameter set to 2 (trigger controlled): not implemented\n");
    return 1;
  } else {
    printf("ERROR - startdaq_mode parameter set to %d (valid: 0,1,2)\n",Config->startdaq_mode);
    return 1;
  }
  ret = CAEN_DGTZ_SetAcquisitionMode(Handle,mode);
  if (ret != CAEN_DGTZ_Success) {
    printf("ERROR - Unable to set acquisition mode. Error code: %d\n",ret);
    return 1;
  }

  // Initializiation finished. Digitizer is ready for acquisition.
  printf("- Digitizer initialization successful\n");
  return 0;

}

// Handle data acquisition
int DAQ_readdata ()
{

  CAEN_DGTZ_ErrorCode ret;
  uint32_t status,grstatus;

  // File to handle DAQ interaction with GUI
  FILE* iokf; // InitOK file

  // Input data information
  char *buffer = NULL;
  uint32_t bufferSize,readSize,writeSize;
  uint32_t numEvents;
  CAEN_DGTZ_EventInfo_t eventInfo;
  char *eventPtr = NULL;

  // Decoded event information
  CAEN_DGTZ_X742_EVENT_t *event = NULL;
  uint32_t iEv, iGr;
  //uint32_t iCh, iSm;
  //uint32_t TT,old_TT,TTT,old_TTT;
  //float dtr,ftr;

  // Output event information
  char *outEvtBuffer = NULL;
  int maxPEvtSize, pEvtSize;
  unsigned int fHeadSize, fTailSize;

  // Global counters for input data
  uint64_t totalReadSize;
  uint64_t totalReadEvents;
  float evtReadPerSec, sizeReadPerSec;

  // Global counters for output data
  uint64_t totalSize;
  uint64_t totalEvents;
  float evtPerSec, sizePerSec;

  unsigned int fileIndex;
  int tooManyOutputFiles;
  //char fileName[MAX_N_OUTPUT_FILES][MAX_FILENAME_LEN];
  char tmpName[MAX_FILENAME_LEN];
  char* fileName[MAX_N_OUTPUT_FILES];
  //char fileFullName[MAX_DATA_DIR_LEN+MAX_FILENAME_LEN];
  unsigned long int fileSize[MAX_N_OUTPUT_FILES];
  unsigned int fileEvents[MAX_N_OUTPUT_FILES];
  time_t fileTOpen[MAX_N_OUTPUT_FILES];
  time_t fileTClose[MAX_N_OUTPUT_FILES];
  int fileHandle;

  time_t t_daqstart, t_daqstop, t_daqtotal;
  time_t t_now;

  unsigned int i;

  // If quit file is already there, assume this is a test run and do nothing
  if ( access(Config->quit_file,F_OK) != -1 ) {
    printf("DAQ_readdata - Quit file '%s' found: will not run acquisition\n",Config->quit_file);
    return 0;
  }

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

  // Allocate buffer to hold retrieved data
  ret = CAEN_DGTZ_MallocReadoutBuffer(Handle,&buffer,&bufferSize);
  if (ret != CAEN_DGTZ_Success) {
    printf("Unable to allocate data readout buffer. Error code: %d\n",ret);
    return 1;
  }
  printf("- Allocated data readout buffer with size %d\n",bufferSize);

  // Allocate buffer to hold decoded event
  ret = CAEN_DGTZ_AllocateEvent(Handle,(void**)&event);
  if (ret != CAEN_DGTZ_Success) {
    printf("Unable to allocate decoded event buffer. Error code: %d\n",ret);
    return 1;
  }
  printf("- Allocated decoded event buffer\n");

  // Allocate buffer to hold output event structure
  maxPEvtSize = (PEVT_HEADER_LEN + 4*(PEVT_GRPHEAD_LEN + 512 + PEVT_GRPTTT_LEN) + 32*512)*4;
  outEvtBuffer = (char *)malloc(maxPEvtSize);
  if (outEvtBuffer == NULL) {
    printf("Unable to allocate output event buffer of size %d\n",maxPEvtSize);
    return 1;
  }
  printf("- Allocated output event buffer with size %d\n",maxPEvtSize);

  // DAQ is now ready to start. Create InitOK file
  printf("- Creating InitOK file '%s'\n",Config->initok_file);
  if ( access(Config->initok_file,F_OK) == -1 ) {
    iokf = fopen(Config->initok_file,"w");
    fclose(iokf);
    printf("- InitOK file '%s' created\n",Config->initok_file);
  } else {
    printf("- InitOK file '%s' already exists (?)\n",Config->initok_file);
    return 1;
  }

  if (Config->startdaq_mode == 0) {

    // If SW controlled DAQ is selected, we need to wait for the appearance of
    // the start file and then give the SWStartAcquisition command

    // Wait either for Start DAQ file or for Quit file
    while(1){
      if ( access(Config->start_file,F_OK) != -1 ) {
	printf("- Start file '%s' found: starting DAQ\n",Config->start_file);
	break;
      }
      if ( access(Config->quit_file,F_OK) != -1 ) {
	printf("- Quit file '%s' found: exiting\n",Config->quit_file);
	return 3;
      }
      // Sleep for ~1ms before checking again
      usleep(1000);
    }

    // Start digitizer acquisition
    ret = CAEN_DGTZ_SWStartAcquisition(Handle);
    if (ret != CAEN_DGTZ_Success) {
      printf("Unable to start acquisition. Error code: %d\n",ret);
      return 2;
    }

  } else if (Config->startdaq_mode == 1) {

    // If S_IN controlled DAQ is selected, everything is automatic
    printf("- Actual DAQ will start on S_IN signal\n");

  }

  InBurst = 1;
  time(&t_daqstart);
  printf("%s - Acquisition started\n",format_time(t_daqstart));

  // Zero counters
  totalReadSize = 0;
  totalReadEvents = 0;
  totalSize = 0;
  totalEvents = 0;
  
  if ( Config->run_number ) {
    // Tell DB that the process has started
    if ( db_process_open(Config->process_id,t_daqstart) != DB_OK ) return 2;
  }

  // Start counting output files
  fileIndex = 0;
  tooManyOutputFiles = 0;

  // Generate name for initial output file and verify it does not exist
  //generate_filename(fileName[fileIndex],t_daqstart);
  generate_filename(tmpName,t_daqstart);
  fileName[fileIndex] = (char*)malloc(strlen(tmpName)+1);
  strcpy(fileName[fileIndex],tmpName);
  if ( Config->run_number ) {
    if ( db_file_check(fileName[fileIndex]) != DB_OK ) return 2;
  }

  // Open file
  //strcpy(fileFullName,Config->data_dir);
  //strcat(fileFullName,fileName[fileIndex]);
  //strcpy(fileFullName,fileName[fileIndex]);
  //printf("- Opening file '%s'\n",fileFullName);
  printf("- Opening file '%s'\n",fileName[fileIndex]);
  fileHandle = open(fileName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (fileHandle == -1) {
    //printf("ERROR - Unable to open file '%s' for writing.\n",fileFullName);
    printf("ERROR - Unable to open file '%s' for writing.\n",fileName[fileIndex]);
    return 2;
  }
  fileTOpen[fileIndex] = t_daqstart;
  fileSize[fileIndex] = 0;
  fileEvents[fileIndex] = 0;

  // Register file in the DB
  if ( Config->run_number ) {
    if ( db_file_open(fileName[fileIndex],PEVT_CURRENT_VERSION,fileTOpen[fileIndex],Config->process_id,fileIndex) != DB_OK ) return 2;
  }

  // Write header to file
  fHeadSize = create_file_head(fileIndex,Config->run_number,fileTOpen[fileIndex],(void *)outEvtBuffer);
  writeSize = write(fileHandle,outEvtBuffer,fHeadSize);
  if (writeSize != fHeadSize) {
    printf("ERROR - Unable to write file header to file. Header size: %d, Write result: %d\n",
	   fHeadSize,writeSize);
    return 2;
  }
  fileSize[fileIndex] += fHeadSize;

  // Main DAQ loop: wait for some data to be present and copy it to output file
  //old_TT =0;
  //old_TTT=0;
  while(1){

    // Read Acquisition Status register
    ret = CAEN_DGTZ_ReadRegister(Handle,CAEN_DGTZ_ACQ_STATUS_ADD,&status);

    //printf("Register 0x%04X Status 0x%04X\n",CAEN_DGTZ_ACQ_STATUS_ADD,status);
    // Check if at least one event is available
    if (status & 0x8) { // Bit 3: EVENT READY

      // Check if group data buffers are full (bit 0 of register 0x1n88)
      for(iGr=0;iGr<MAX_X742_GROUP_SIZE;iGr++){
	if ( Config->group_enable_mask & (1 << iGr) ) {
	  //printf("Checking buffer for group %d on register 0x%04X\n",iGr,CAEN_DGTZ_CHANNEL_STATUS_BASE_ADDRESS+(iGr<<8));
	  if (CAEN_DGTZ_ReadRegister(Handle,CAEN_DGTZ_CHANNEL_STATUS_BASE_ADDRESS+(iGr<<8),&grstatus) != CAEN_DGTZ_Success) {
	    printf("Cannot read group %d status. Error code: %d\n",iGr,ret);
	    return 2;
	  } else if (grstatus & 1) { // Bit 0: Memory full
	    printf("*** WARNING *** Group %d data buffer is full (!!!)\n",iGr);
	  }
	}
      }

      // Read the data from digitizer
      ret = CAEN_DGTZ_ReadData(Handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&readSize);
      if (ret != CAEN_DGTZ_Success) {
	printf("Unable to read data from digitizer. Error code: %d\n",ret);
	return 2;
      }
      ret = CAEN_DGTZ_GetNumEvents(Handle,buffer,readSize,&numEvents);
      if (ret != CAEN_DGTZ_Success) {
	printf("Unable to get number of events from read buffer. Error code: %d\n",ret);
	return 2;
      }
      //printf("Read %d event(s) with total size %d bytes\n",numEvents,readSize);

      // Update global counters
      totalReadSize += readSize;
      totalReadEvents += numEvents;

      // Loop over all events in data buffer
      for(iEv=0;iEv<numEvents;iEv++) {

	// Retrieve info and pointer for current event
	ret = CAEN_DGTZ_GetEventInfo(Handle,buffer,readSize,iEv,&eventInfo,&eventPtr);
	if (ret != CAEN_DGTZ_Success) {
	  printf("Unable to get event info from read buffer. Error code: %d\n",ret);
	  return 2;
	}

	// *** EventInfo data structure (from CAENDigitizerType.h) ***
	//
	//typedef struct 
	//{
	//    uint32_t EventSize;
	//    uint32_t BoardId;
	//    uint32_t Pattern;
	//    uint32_t ChannelMask;
	//    uint32_t EventCounter;
	//    uint32_t TriggerTimeTag;
	//} CAEN_DGTZ_EventInfo_t;
	//
	// ***********************************************************

	/*
	// Debug code for event timing
	TT = ((int*)eventPtr)[3]; // Event time tag
	TTT = ( ((int*)eventPtr)[4+1+3456] & 0x3FFFFFFF ); // Group 0 trigger time tag
	dtr = ((float)(TTT-old_TTT))*0.0085; // 8.5ns per count in us
	ftr = 1000000./dtr; // Frequency in Hz
	printf("Evt# %6u t %10u 0x%08X dt %10u 0x%08X tt %10u dtt %10u %10.3fus %8.3fHz\n",
	       eventInfo.EventCounter,
	       TT,TT,TT-old_TT,TT-old_TT,TTT,TTT-old_TTT,dtr,ftr
	      );
	old_TT = TT;
	old_TTT = TTT;
	*/

	// Decode (and apply DRS4 corrections to) event
	ret = CAEN_DGTZ_DecodeEvent(Handle,eventPtr,(void**)&event);
	if (ret != CAEN_DGTZ_Success) {
	  printf("Unable to decode event. Error code: %d\n",ret);
	  return 2;
	}

	// *** Event data structures (from CAENDigitizerType.h) ***
	//
	//typedef struct 
	//{
	//    uint8_t                GrPresent[MAX_X742_GROUP_SIZE]; // If the group has data the value is 1 otherwise is 0  
	//    CAEN_DGTZ_X742_GROUP_t DataGroup[MAX_X742_GROUP_SIZE]; // the array of ChSize samples (meaning "Groups" :)
	//} CAEN_DGTZ_X742_EVENT_t;
	//
	//typedef struct 
	//{
	//    uint32_t ChSize[MAX_X742_CHANNEL_SIZE];      // the number of samples stored in DataChannel array  
	//    float   *DataChannel[MAX_X742_CHANNEL_SIZE]; // the array of ChSize samples
	//    uint32_t TriggerTimeTag;
	//    uint16_t StartIndexCell;
	//} CAEN_DGTZ_X742_GROUP_t;
	//
	// *********************************************************

	// Print output once in a while (can become a run-time monitor)
	if ( (eventInfo.EventCounter % 100) == 0 ) {

	  // Print event header
	  printf("- Evt# %u time %u size %u board 0x%02x pattern 0x%08x chmsk 0x%08x\n",
		 eventInfo.EventCounter   & 0x003FFFFF,
		 eventInfo.TriggerTimeTag & 0x7FFFFFFF,
		 eventInfo.EventSize      & 0x0FFFFFFF,
		 eventInfo.BoardId        & 0x0000001F,
		 eventInfo.Pattern        & 0x00003FFF,
		 eventInfo.ChannelMask
		);

	  // Print some group info
	  for(iGr=0;iGr<MAX_X742_GROUP_SIZE;iGr++){
	    if (event->GrPresent[iGr]) {
	      printf("  Group %d TTT %d SIC %d\n",iGr,event->DataGroup[iGr].TriggerTimeTag,event->DataGroup[iGr].StartIndexCell);
	      //for(iCh=0;iCh<MAX_X742_CHANNEL_SIZE;iCh++){
	        //printf("    Channel %d size %d\n",iCh,event->DataGroup[iGr].ChSize[iCh]);
	        //for(iSm=0;iSm<event->DataGroup[iGr].ChSize[iCh];iSm++){
	        //  printf(" %6.1f",event->DataGroup[iGr].DataChannel[iCh][iSm]);
	        //}
		//printf("\n");
	      //}
	    } else {
	      //printf("  Group %d not present\n",iGr);
	    }
	  }

	}

	// Copy decoded event to output event buffer applying zero-suppression
	// Return event size in bytes (0: event rejected, <0: error)
	pEvtSize = create_pevent((void *)eventPtr,event,(void *)outEvtBuffer);
	if (pEvtSize<0){
	  printf("ERROR - Unable to copy decoded event to output event buffer. RC %d\n",pEvtSize);
	  return 2;
	}
	
	// If event is accepted, write it to file and update counters
	if (pEvtSize > 0) {
	  
	  // Write data to output file
	  writeSize = write(fileHandle,outEvtBuffer,pEvtSize);
	  if (writeSize != pEvtSize) {
	    printf("ERROR - Unable to write read data to file. Event size: %d, Write result: %d\n",
		   pEvtSize,writeSize);
	    return 2;
	  }
	  
	  // Update file counters
	  fileSize[fileIndex] += pEvtSize;
	  fileEvents[fileIndex]++;
	  
	  // Update global counters
	  totalSize += pEvtSize;
	  totalEvents++;
	  
	}

      }

    }

    // Check if we need a new data file (required time elapsed or file size/events threshold exceeded)
    time(&t_now);
    if (
	(t_now-fileTOpen[fileIndex] >= Config->file_max_duration) ||
	(fileSize[fileIndex]        >= Config->file_max_size    ) ||
	(fileEvents[fileIndex]      >= Config->file_max_events  )
       ) {

      // Register file closing time
      fileTClose[fileIndex] = t_now;

      // Write tail to file
      fTailSize = create_file_tail(fileEvents[fileIndex],fileSize[fileIndex],fileTClose[fileIndex],(void *)outEvtBuffer);
      writeSize = write(fileHandle,outEvtBuffer,fTailSize);
      if (writeSize != fTailSize) {
	printf("ERROR - Unable to write file header to file. Tail size: %d, Write result: %d\n",
	       fTailSize,writeSize);
	return 2;
      }
      fileSize[fileIndex] += fTailSize;

      // Close old output file and show some info about counters
      if (close(fileHandle) == -1) {
	printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
	return 2;
      };
      //printf("%s - Closed file '%s' after %d secs with %u events and size %lu bytes\n",
      //	     format_time(fileTClose[fileIndex]),fileFullName,(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
      //	     fileEvents[fileIndex],fileSize[fileIndex]);
      printf("%s - Closed file '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(fileTClose[fileIndex]),fileName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);

      // Close file in DB
      if ( Config->run_number ) {
	if ( db_file_close(fileName[fileIndex],fileTClose[fileIndex],fileSize[fileIndex],fileEvents[fileIndex],Config->process_id) != DB_OK ) return 2;
      }

      // Update file counter
      fileIndex++;

      if ( fileIndex<MAX_N_OUTPUT_FILES ) {

	// Open new output file and reset all counters
	//generate_filename(fileName[fileIndex],t_now);
	generate_filename(tmpName,t_now);
	fileName[fileIndex] = (char*)malloc(strlen(tmpName)+1);
	strcpy(fileName[fileIndex],tmpName);
	if ( Config->run_number ) {
	  if ( db_file_check(fileName[fileIndex]) != DB_OK ) return 2;
	}
	//strcpy(fileFullName,Config->data_dir);
	//strcat(fileFullName,fileName[fileIndex]);
	//strcpy(fileFullName,fileName[fileIndex]);
	//printf("- Opening file %d with name '%s'\n",fileIndex,fileFullName);
	printf("- Opening file %d with name '%s'\n",fileIndex,fileName[fileIndex]);
	//fileHandle = open(fileFullName,O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	fileHandle = open(fileName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if (fileHandle == -1) {
	  //printf("ERROR - Unable to open file '%s' for writing.\n",fileFullName);
	  printf("ERROR - Unable to open file '%s' for writing.\n",fileName[fileIndex]);
	  return 2;
	}
	fileTOpen[fileIndex] = t_now;
	fileSize[fileIndex] = 0;
	fileEvents[fileIndex] = 0;

	// Register file in the DB
	if ( Config->run_number ) {
	  if ( db_file_open(fileName[fileIndex],PEVT_CURRENT_VERSION,fileTOpen[fileIndex],Config->process_id,fileIndex) != DB_OK ) return 2;
	}

	// Write header to file
	fHeadSize = create_file_head(fileIndex,Config->run_number,fileTOpen[fileIndex],(void *)outEvtBuffer);
	writeSize = write(fileHandle,outEvtBuffer,fHeadSize);
	if (writeSize != fHeadSize) {
	  printf("ERROR - Unable to write file header to file. Header size: %d, Write result: %d\n",
		 fHeadSize,writeSize);
	  return 2;
	}
	fileSize[fileIndex] += fHeadSize;

      } else {

	tooManyOutputFiles = 1;

      }

    }

    // Check if it is time to stop DAQ (user interrupt, quit file, time elapsed, too many output files)
    if (
	 BreakSignal || tooManyOutputFiles ||
	 (access(Config->quit_file,F_OK) != -1) ||
	 ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
       ) break;

    // Sleep for a while before continuing
    usleep(Config->daq_loop_delay);

  }

  // Tell user what stopped DAQ
  if ( BreakSignal ) printf("=== Stopping DAQ on interrupt %d ===\n",BreakSignal);
  if ( tooManyOutputFiles ) printf("=== Stopping DAQ after writing %d data files ===\n",fileIndex);
  if ( access(Config->quit_file,F_OK) != -1 )
    printf("=== Stopping DAQ on quit file '%s' ===\n",Config->quit_file);
  if ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
    printf("=== Stopping DAQ after %d secs of run (requested %d) ===\n",(int)(t_now-t_daqstart),Config->total_daq_time);

  // Register file closing time
  fileTClose[fileIndex] = t_now;

  // If DAQ was stopped for writing too many output files, we do not have
  // to close the last file
  if ( ! tooManyOutputFiles ) {

    // Write tail to file
    fTailSize = create_file_tail(fileEvents[fileIndex],fileSize[fileIndex],fileTClose[fileIndex],(void *)outEvtBuffer);
    writeSize = write(fileHandle,outEvtBuffer,fTailSize);
    if (writeSize != fTailSize) {
      printf("ERROR - Unable to write file tail to file. Tail size: %d, Write result: %d\n",
	     fTailSize,writeSize);
      return 2;
    }
    fileSize[fileIndex] += fTailSize;

    // Close output file and show some info about counters
    if (close(fileHandle) == -1) {
      printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
      return 2;
    };
    printf("%s - Closed file '%s' after %d secs with %u events and size %lu bytes\n",
	   format_time(t_now),fileName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	   fileEvents[fileIndex],fileSize[fileIndex]);

    // Close file in DB
    if ( Config->run_number ) {
      if ( db_file_close(fileName[fileIndex],fileTClose[fileIndex],fileSize[fileIndex],fileEvents[fileIndex],Config->process_id) != DB_OK ) return 2;
    }

    // Update file counter
    fileIndex++;

  }

  // Stop digitizer acquisition
  ret = CAEN_DGTZ_SWStopAcquisition(Handle);
  if (ret != CAEN_DGTZ_Success) {
    printf("Unable to stop acquisition. Error code: %d\n",ret);
    return 2;
  }
  InBurst = 0; // Signal DAQ has stopped
  time(&t_daqstop);
  printf("%s - Acquisition stopped\n",format_time(t_daqstop));

  // Deallocate data buffer
  ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  if (ret != CAEN_DGTZ_Success) {
    printf("Unable to free allocated data readout buffer. Error code: %d\n",ret);
    return 2;
  }
  printf("- Deallocated data readout buffer\n");

  // Deallocate event buffer
  ret = CAEN_DGTZ_FreeEvent(Handle,(void**)&event);
  if (ret != CAEN_DGTZ_Success) {
    printf("Unable to free allocated event buffer. Error code: %d\n",ret);
    return 2;
  }
  printf("- Deallocated event buffer\n");

  // Deallocate output event buffer
  free(outEvtBuffer);

  // Tell DB that the run has ended
  if ( Config->run_number ) {
    if ( db_process_close(Config->process_id,t_daqstop) != DB_OK ) return 2;
  }

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
  printf("=== Files created =======================================\n");
  for(i=0;i<fileIndex;i++) {
    printf("'%s' %u %lu",fileName[i],fileEvents[i],fileSize[i]);
    printf(" %s",format_time(fileTOpen[i])); // Optimizer effect! :)
    printf(" %s\n",format_time(fileTClose[i]));
  }
  printf("=========================================================\n");

  // Close DB file
  if ( Config->run_number ) {
    if ( db_end() != DB_OK ) return 2;
  }

  // Free space allocated for file names
  for(i=0;i<fileIndex;i++) free(fileName[i]);

  return 0;

}

// Function to handle final reset of the digitizer
int DAQ_close ()
{

  CAEN_DGTZ_ErrorCode ret;
  uint32_t reg,data;

  // Check if DAQ is running and stop it (should never happen!)
  reg = 0x8104; // Acquisition Status
  ret = CAEN_DGTZ_ReadRegister(Handle,reg,&data);
  if (ret == CAEN_DGTZ_Success) {
    if ( (data >> 2) & 0x1 ) { // bit 2: RUN on/off
      printf ("WARNING!!! DAQ is active (should not happen): stopping... ");
      ret = CAEN_DGTZ_SWStopAcquisition(Handle);
      if (ret != CAEN_DGTZ_Success) {
	printf("\nWARNING!!! Unable to stop data acquisition. Error code: %d\n",ret);
      } else {
	printf(" done\n");
	ret = CAEN_DGTZ_ClearData(Handle); // Clear digitizer buffers
      }
    }
  } else {
    printf("WARNING!!! Unable to read Acquisition Status register 0x%04x. Error code: %d\n",reg,ret);
  }

  // Reset
  printf ("- Resetting digitizer... ");
  ret = CAEN_DGTZ_Reset (Handle);
  if (ret == CAEN_DGTZ_Success) {
    printf ("done!\n");
  } else {
    printf("\nWARNING!!! Unable to reset digitizer. Error code: %d\n",ret);
  }

  printf ("- Flushing all output files... ");
  fflush (NULL);
  printf ("done!\n");

  printf ("- Closing connection to digitizer... ");
  ret = CAEN_DGTZ_CloseDigitizer (Handle);
  if (ret == CAEN_DGTZ_Success) {
    printf ("done!\n");
  } else {
    printf("\nWARNING!!! Unable to close digitizer connection. Error code: %d\n",ret);
  }

  return 0;

}
