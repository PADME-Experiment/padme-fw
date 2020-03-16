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
#include "Tools.h"
#include "PEvent.h"
#include "Signal.h"

#include "DAQ.h"

//#define TIME_TAG_LEN     20
//#define MAX_FILENAME_LEN MAX_DATA_FILE_LEN+TIME_TAG_LEN

//#define MAX_N_OUTPUT_FILES 10240

// Global variables

int Handle; // Handle for V1742 digitizer

extern int InBurst;
extern int BreakSignal;

// Get LinkNum (link address of port on A3818 boards)
int get_LinkNum()
{
  if (Config->conet2_link == -1) {
    // Link is not defined: extract from board_id
    printf("WARNING - CONET2 link set from board_id\n");
    return Config->board_id/8;
  }
  return Config->conet2_link;
}

// Get ConetNode (ADC board address, 0-7, within its A3818 link)
int get_ConetNode()
{
  if (Config->conet2_slot == -1) {
    // Slot is not defined: extract from board_id
    printf("WARNING - CONET2 slot set from board_id\n");
    return Config->board_id%8;
  }
  return Config->conet2_slot;
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

  printf("- Connecting to CAEN digitizer board %d on link %d slot %d\n",Config->board_id,linkNum,conetNode);

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

  // Get board serial number and save it to DB
  Config->board_sn = boardInfo.SerialNumber;
  printf("DBINFO - %s - add_proc_config_para %s %d\n",format_time(time(0)),"board_sn",Config->board_sn);

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
  //FILE* iokf; // InitOK file

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
  uint32_t fHeadSize, fTailSize;

  // Global counters for input data
  uint64_t totalReadSize;
  uint32_t totalReadEvents;
  float evtReadPerSec, sizeReadPerSec;

  // Global counters for output data
  uint64_t totalWriteSize;
  uint32_t totalWriteEvents;
  float evtWritePerSec, sizeWritePerSec;

  // Information about output files
  unsigned int fileIndex = 0;
  int tooManyOutputFiles;
  char tmpName[MAX_FILENAME_LEN];
  char* fileName[MAX_N_OUTPUT_FILES];
  char* pathName[MAX_N_OUTPUT_FILES];
  uint64_t fileSize[MAX_N_OUTPUT_FILES];
  uint32_t fileEvents[MAX_N_OUTPUT_FILES];
  time_t fileTOpen[MAX_N_OUTPUT_FILES];
  time_t fileTClose[MAX_N_OUTPUT_FILES];
  int fileHandle = 0;

  // Flag to end run on ADC read error
  int adcError;

  time_t t_daqstart, t_daqstop, t_daqtotal;
  time_t t_now;

  unsigned int i;

  // If quit file is already there, assume this is a test run and do nothing
  if ( access(Config->quit_file,F_OK) != -1 ) {
    printf("DAQ_readdata - Quit file '%s' found: will not run acquisition\n",Config->quit_file);
    return 3;
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

  // Initialize output files counter
  //fileIndex = 0;

  // If we use STREAM output, the output stream must be initialized here
  if ( strcmp(Config->output_mode,"STREAM")==0 ) {

    pathName[fileIndex] = (char*)malloc(strlen(Config->output_stream)+1);
    strcpy(pathName[fileIndex],Config->output_stream);

    printf("- Opening output stream '%s'\n",pathName[fileIndex]);
    fileHandle = open(pathName[fileIndex],O_WRONLY);
    if (fileHandle == -1) {
      printf("ERROR - Unable to open file '%s' for writing.\n",pathName[fileIndex]);
      return 2;
    }

    // Increase stream buffer to 128MB (~1800evts)
    //long pipe_size = (long)fcntl(fileHandle,F_GETPIPE_SZ);
    long pipe_size = (long)fcntl(fileHandle,1024+8);
    if (pipe_size == -1) { perror("get pipe size failed."); }
    printf("Default pipe size: %ld\n", pipe_size);

    //int ret = fcntl(fileHandle,F_SETPIPE_SZ,128*1024*1024);
    int ret = fcntl(fileHandle,1024+7,128*1024*1024);
    if (ret < 0) { perror("set pipe size failed."); }

    //pipe_size = (long)fcntl(fileHandle,F_GETPIPE_SZ);
    pipe_size = (long)fcntl(fileHandle,1024+8);
    if (pipe_size == -1) { perror("get pipe size 2 failed."); }
    printf("Pipe size: %ld\n", pipe_size);

  }

  // DAQ is now ready to start. Create InitOK file and set status to INITIALIZED
  if ( create_initok_file() ) return 1;
  printf("- Setting process status to INITIALIZED (%d)\n",DB_STATUS_INITIALIZED);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_INITIALIZED);

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
  
  printf("- Setting process status to RUNNING (%d)\n",DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_status %d\n",format_time(time(0)),DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_time_start %s\n",format_time(time(0)),format_time(t_daqstart));

  // Zero counters
  totalReadSize = 0;
  totalReadEvents = 0;
  totalWriteSize = 0;
  totalWriteEvents = 0;

  // Start counting output files
  //fileIndex = 0;
  //tooManyOutputFiles = 0;

  if ( strcmp(Config->output_mode,"FILE")==0 ) {

    // Generate name for initial output file and verify it does not exist
    generate_filename(tmpName,t_daqstart);
    fileName[fileIndex] = (char*)malloc(strlen(tmpName)+1);
    strcpy(fileName[fileIndex],tmpName);
    pathName[fileIndex] = (char*)malloc(strlen(Config->data_dir)+strlen(fileName[fileIndex])+1);
    strcpy(pathName[fileIndex],Config->data_dir);
    strcat(pathName[fileIndex],fileName[fileIndex]);

//  } else {
//
//    // Use only one virtual file for streaming out all data
//    pathName[fileIndex] = (char*)malloc(strlen(Config->output_stream)+1);
//    strcpy(pathName[fileIndex],Config->output_stream);
//
//  }
//
//  // Open file
//  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("- Opening output file %d with path '%s'\n",fileIndex,pathName[fileIndex]);
    fileHandle = open(pathName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
//  } else {
//
//    printf("- Opening output stream '%s'\n",pathName[fileIndex]);
//    fileHandle = open(pathName[fileIndex],O_WRONLY);
//
//    // Increase stream buffer to 128MB (~1800evts)
//    //long pipe_size = (long)fcntl(fileHandle,F_GETPIPE_SZ);
//    long pipe_size = (long)fcntl(fileHandle,1024+8);
//    if (pipe_size == -1) { perror("get pipe size failed."); }
//    printf("Default pipe size: %ld\n", pipe_size);
//
//    //int ret = fcntl(fileHandle,F_SETPIPE_SZ,128*1024*1024);
//    int ret = fcntl(fileHandle,1024+7,128*1024*1024);
//    if (ret < 0) { perror("set pipe size failed."); }
//
//    //pipe_size = (long)fcntl(fileHandle,F_GETPIPE_SZ);
//    pipe_size = (long)fcntl(fileHandle,1024+8);
//    if (pipe_size == -1) { perror("get pipe size 2 failed."); }
//    printf("Pipe size: %ld\n", pipe_size);
//
//  }
    if (fileHandle == -1) {
      printf("ERROR - Unable to open file '%s' for writing.\n",pathName[fileIndex]);
      return 2;
    }
  }
  fileTOpen[fileIndex] = t_daqstart;
  fileSize[fileIndex] = 0;
  fileEvents[fileIndex] = 0;

  // Register file in the DB
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("DBINFO - %s - file_create %s %s %d %d\n",format_time(time(0)),fileName[fileIndex],"DAQDATA",PEVT_CURRENT_VERSION,fileIndex);
    printf("DBINFO - %s - file_set_time_open %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTOpen[fileIndex]));
  }

  // Write header to file
  fHeadSize = create_file_head(fileIndex,Config->run_number,Config->board_id,Config->board_sn,fileTOpen[fileIndex],(void *)outEvtBuffer);
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
  adcError = 0;
  tooManyOutputFiles = 0;
  while(1){

    // Read Acquisition Status register
    ret = CAEN_DGTZ_ReadRegister(Handle,CAEN_DGTZ_ACQ_STATUS_ADD,&status);
    if (ret != CAEN_DGTZ_Success) {
      printf("Cannot read acquisition status. Error code: %d\n",ret);
      adcError = 1;
      break; // Exit from main DAQ loop
    }

    //printf("Register 0x%04X Status 0x%04X\n",CAEN_DGTZ_ACQ_STATUS_ADD,status);
    // Check if at least one event is available
    if (status & 0x8) { // Bit 3: EVENT READY

      // Check if group data buffers are full (bit 0 of register 0x1n88)
      for(iGr=0;iGr<MAX_X742_GROUP_SIZE;iGr++){
	if ( Config->group_enable_mask & (1 << iGr) ) {
	  //printf("Checking buffer for group %d on register 0x%04X\n",iGr,CAEN_DGTZ_CHANNEL_STATUS_BASE_ADDRESS+(iGr<<8));
	  //if (CAEN_DGTZ_ReadRegister(Handle,CAEN_DGTZ_CHANNEL_STATUS_BASE_ADDRESS+(iGr<<8),&grstatus) != CAEN_DGTZ_Success) {
	  ret = CAEN_DGTZ_ReadRegister(Handle,CAEN_DGTZ_CHANNEL_STATUS_BASE_ADDRESS+(iGr<<8),&grstatus);
	  if (ret != CAEN_DGTZ_Success) {
	    printf("Cannot read group %d status. Error code: %d\n",iGr,ret);
	    //return 2;
	    adcError = 1;
	    break; // Exit from loop over groups
	  } else if (grstatus & 1) { // Bit 0: Memory full
	    printf("*** WARNING *** Group %d data buffer is full (!!!)\n",iGr);
	  }
	}
      }
      if (adcError) break; // Exit from main DAQ loop

      // Read the data from digitizer
      ret = CAEN_DGTZ_ReadData(Handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&readSize);
      if (ret != CAEN_DGTZ_Success) {
	printf("Unable to read data from digitizer. Error code: %d\n",ret);
	//return 2;
	adcError = 1;
	break; // Exit from main DAQ loop
      }
      ret = CAEN_DGTZ_GetNumEvents(Handle,buffer,readSize,&numEvents);
      if (ret != CAEN_DGTZ_Success) {
	printf("Unable to get number of events from read buffer. Error code: %d\n",ret);
	//return 2;
	adcError = 1;
	break; // Exit from main DAQ loop
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
	  //return 2;
	  adcError = 1;
	  break; // Exit from loop over events
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
	  //return 2;
	  adcError = 1;
	  break; // Exit from loop over events
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
	//if ( (eventInfo.EventCounter % 100) == 0 ) {
	if ( (eventInfo.EventCounter % Config->debug_scale) == 0 ) {

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
	  printf("  Group(TTT,SIC)");
	  for(iGr=0;iGr<MAX_X742_GROUP_SIZE;iGr++){
	    if (event->GrPresent[iGr]) {
	      printf(" %1d(%04x,%4d)",iGr,event->DataGroup[iGr].TriggerTimeTag,event->DataGroup[iGr].StartIndexCell);
	    }
	  }
	  printf("\n");

	}

	// Copy decoded event to output event buffer applying zero-suppression
	// Return event size in bytes (0: event rejected, <0: error)
	pEvtSize = create_pevent((void *)eventPtr,event,(void *)outEvtBuffer);
	if (pEvtSize<0){
	  printf("ERROR - Unable to copy decoded event to output event buffer. RC %d\n",pEvtSize);
	  //return 2;
	  adcError = 1;
	  break; // Exit from loop over events
	}
	
	// If event is accepted, write it to file and update counters
	if (pEvtSize > 0) {
	  
	  // Write event header to debug info once in a while
	  //if ( (eventInfo.EventCounter % 100) == 0 ) {
	  if ( (eventInfo.EventCounter % Config->debug_scale) == 0 ) {
	    unsigned char i,j;
	    printf("  Header");
	    for (i=0;i<6;i++) {
	      printf(" %1d(",i);
	      for (j=0;j<4;j++) { printf("%02x",(unsigned char)(outEvtBuffer[i*4+3-j])); }
	      printf(")");
	    }
	    printf("\n");
	  }

	  // Write data to output file
	  writeSize = write(fileHandle,outEvtBuffer,pEvtSize);
	  if (writeSize != pEvtSize) {
	    printf("ERROR - Unable to write read data to file. Event size: %d, Write result: %d\n",
		   pEvtSize,writeSize);
	    return 2; // As this is an error while writing data to output file, no point in sending file tail
	  }
	  
	  // Update file counters
	  fileSize[fileIndex] += pEvtSize;
	  fileEvents[fileIndex]++;
	  
	  // Update global counters
	  totalWriteSize += pEvtSize;
	  totalWriteEvents++;
	  
	}

      }
      if (adcError) break; // Exit from main DAQ loop

    }

    // Save current time
    time(&t_now);

    // If we are running in FILE output mode, check if we need a new data file
    // i.e. required time elapsed or file size/events threshold exceeded
    if ( strcmp(Config->output_mode,"FILE")==0 ) {

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
	  return 2; // As this is an error while writing data to output file, no point in sending file tail
	}
	fileSize[fileIndex] += writeSize;
	totalWriteSize += writeSize;

	// Close old output file and show some info about counters
	if (close(fileHandle) == -1) {
	  printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
	  return 2; // As this is an error while writing data to output file, no point in sending file tail
	};
	printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	       format_time(fileTClose[fileIndex]),pathName[fileIndex],
	       (int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	       fileEvents[fileIndex],fileSize[fileIndex]);

	// Close file in DB
	printf("DBINFO - %s - file_set_time_close %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTClose[fileIndex]));
	printf("DBINFO - %s - file_set_size %s %lu\n",format_time(time(0)),fileName[fileIndex],fileSize[fileIndex]);
	printf("DBINFO - %s - file_set_n_events %s %u\n",format_time(time(0)),fileName[fileIndex],fileEvents[fileIndex]);

	// Update file counter
	fileIndex++;

	if ( fileIndex<MAX_N_OUTPUT_FILES ) {

	  // Open new output file and reset all counters
	  generate_filename(tmpName,t_now);
	  fileName[fileIndex] = (char*)malloc(strlen(tmpName)+1);
	  strcpy(fileName[fileIndex],tmpName);
	  pathName[fileIndex] = (char*)malloc(strlen(Config->data_dir)+strlen(fileName[fileIndex])+1);
	  strcpy(pathName[fileIndex],Config->data_dir);
	  strcat(pathName[fileIndex],fileName[fileIndex]);
	  printf("- Opening output file %d with path '%s'\n",fileIndex,pathName[fileIndex]);
	  fileHandle = open(pathName[fileIndex],O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	  if (fileHandle == -1) {
	    printf("ERROR - Unable to open file '%s' for writing.\n",pathName[fileIndex]);
	    return 2; // No output file currently open: no point in sending file tail
	  }
	  fileTOpen[fileIndex] = t_now;
	  fileSize[fileIndex] = 0;
	  fileEvents[fileIndex] = 0;

	  // Register file in the DB
	  printf("DBINFO - %s - file_create %s %s %d %d\n",format_time(time(0)),fileName[fileIndex],"DAQDATA",PEVT_CURRENT_VERSION,fileIndex);
	  printf("DBINFO - %s - file_set_time_open %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTOpen[fileIndex]));

	  // Write header to file
	  fHeadSize = create_file_head(fileIndex,Config->run_number,Config->board_id,Config->board_sn,fileTOpen[fileIndex],(void *)outEvtBuffer);
	  writeSize = write(fileHandle,outEvtBuffer,fHeadSize);
	  if (writeSize != fHeadSize) {
	    printf("ERROR - Unable to write file header to file. Header size: %d, Write result: %d\n",
		   fHeadSize,writeSize);
	    return 2; // As this is an error while writing data to output file, no point in sending file tail
	  }
	  fileSize[fileIndex] += writeSize;
	  totalWriteSize += writeSize;

	} else {

	  tooManyOutputFiles = 1;

	}

      }

    }

    // Check if it is time to stop DAQ (user interrupt, quit file, time elapsed, too many output files)
    //printf("Checking stop run condition %s %d\n",Config->quit_file,access(Config->quit_file,F_OK));
    if (
	 BreakSignal || tooManyOutputFiles ||
	 (access(Config->quit_file,F_OK) != -1) ||
	 ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
       ) break;

    // Sleep for a while before continuing
    usleep(Config->daq_loop_delay);

  }

  // Get time when processing ends
  time(&t_now);

  // Tell user what stopped DAQ
  if ( adcError ) printf("=== Stopping DAQ on ADC access or data handling ERROR ===\n");
  if ( BreakSignal ) printf("=== Stopping DAQ on interrupt %d ===\n",BreakSignal);
  if ( tooManyOutputFiles ) printf("=== Stopping DAQ after writing %d data files ===\n",fileIndex);
  if ( access(Config->quit_file,F_OK) != -1 )
    printf("=== Stopping DAQ on quit file '%s' ===\n",Config->quit_file);
  if ( Config->total_daq_time && ( t_now-t_daqstart >= Config->total_daq_time ) )
    printf("=== Stopping DAQ after %d secs of run (requested %d) ===\n",(int)(t_now-t_daqstart),Config->total_daq_time);

  // If DAQ was stopped for writing too many output files, we do not have to close the last file
  if ( ! tooManyOutputFiles ) {

    // Register file closing time
    fileTClose[fileIndex] = t_now;

    // Write tail to file
    fTailSize = create_file_tail(fileEvents[fileIndex],fileSize[fileIndex],fileTClose[fileIndex],(void *)outEvtBuffer);
    writeSize = write(fileHandle,outEvtBuffer,fTailSize);
    if (writeSize != fTailSize) {
      printf("ERROR - Unable to write file tail to file. Tail size: %d, Write result: %d\n",
	     fTailSize,writeSize);
      return 2;
    }
    fileSize[fileIndex] += writeSize;
    totalWriteSize += writeSize;

    // Close output file and show some info about counters
    if (close(fileHandle) == -1) {
      printf("ERROR - Unable to close output file '%s'.\n",fileName[fileIndex]);
      return 2;
    };
    if ( strcmp(Config->output_mode,"FILE")==0 ) {
      printf("%s - Closed output file '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(fileTClose[fileIndex]),pathName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
      printf("DBINFO - %s - file_set_time_close %s %s\n",format_time(time(0)),fileName[fileIndex],format_time(fileTClose[fileIndex]));
      printf("DBINFO - %s - file_set_size %s %lu\n",format_time(time(0)),fileName[fileIndex],fileSize[fileIndex]);
      printf("DBINFO - %s - file_set_n_events %s %u\n",format_time(time(0)),fileName[fileIndex],fileEvents[fileIndex]);
    } else {
      printf("%s - Closed output stream '%s' after %d secs with %u events and size %lu bytes\n",
	     format_time(fileTClose[fileIndex]),pathName[fileIndex],(int)(fileTClose[fileIndex]-fileTOpen[fileIndex]),
	     fileEvents[fileIndex],fileSize[fileIndex]);
    }

    // Update file counter
    fileIndex++;

  }

  if (adcError) {
    printf("DAQ was stopped because of an error related to ADC access or data handling: aborting\n");
    return 2;
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

  // Tell DB that the process has ended
  printf("DBINFO - %s - process_set_time_stop %s\n",format_time(time(0)),format_time(t_daqstop));
  printf("DBINFO - %s - process_set_total_events %d\n",format_time(time(0)),totalWriteEvents);
  printf("DBINFO - %s - process_set_total_size %ld\n",format_time(time(0)),totalWriteSize);
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("DBINFO - %s - process_set_n_files %d\n",format_time(time(0)),fileIndex);
  }

  // Give some final report
  evtReadPerSec = 0.;
  sizeReadPerSec = 0.;
  evtWritePerSec = 0.;
  sizeWritePerSec = 0.;
  t_daqtotal = t_daqstop-t_daqstart;
  if (t_daqtotal>0) {
    evtReadPerSec = 1.*totalReadEvents/t_daqtotal;
    sizeReadPerSec = totalReadSize/(t_daqtotal*1024.);
    evtWritePerSec = 1.*totalWriteEvents/t_daqtotal;
    sizeWritePerSec = totalWriteSize/(t_daqtotal*1024.);
  }
  printf("\n=== DAQ ending on %s ===\n",format_time(t_daqstop));
  printf("Total running time: %d secs\n",(int)t_daqtotal);
  printf("Total number of events acquired: %u - %6.2f events/s\n",totalReadEvents,evtReadPerSec);
  printf("Total size of data acquired: %lu B - %6.2f KB/s\n",totalReadSize,sizeReadPerSec);
  printf("Total number of events written: %u - %6.2f events/s\n",totalWriteEvents,evtWritePerSec);
  printf("Total size of data written: %lu B - %6.2f KB/s\n",totalWriteSize,sizeWritePerSec);
  if ( strcmp(Config->output_mode,"FILE")==0 ) {
    printf("=== %2d Files created =======================================\n",fileIndex);
    for(i=0;i<fileIndex;i++) {
      printf("'%s' %u %lu",fileName[i],fileEvents[i],fileSize[i]);
      printf(" %s",format_time(fileTOpen[i])); // Optimizer effect! :)
      printf(" %s\n",format_time(fileTClose[i]));
    }
  }
  printf("=========================================================\n");

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
  if (ret != CAEN_DGTZ_Success) {
    printf("*** ERROR *** Unable to read Acquisition Status register 0x%04x. Error code: %d\n",reg,ret);
    return 1;
  }
  if ( (data >> 2) & 0x1 ) { // bit 2: RUN on/off
    printf ("WARNING!!! DAQ is active (should not happen): stopping... ");
    ret = CAEN_DGTZ_SWStopAcquisition(Handle);
    if (ret != CAEN_DGTZ_Success) {
      printf("\n*** ERROR *** Unable to stop data acquisition. Error code: %d\n",ret);
      return 1;
    }
    printf(" done\n");
    ret = CAEN_DGTZ_ClearData(Handle); // Clear digitizer buffers
    if (ret != CAEN_DGTZ_Success) {
      printf("*** ERROR *** Unable to clear data after stopping data acquisition. Error code: %d\n",ret);
      return 1;
    }
  }

  // Reset
  printf ("- Resetting digitizer... ");
  ret = CAEN_DGTZ_Reset (Handle);
  if (ret != CAEN_DGTZ_Success) {
    printf("*** ERROR *** Unable to reset digitizer. Error code: %d\n",ret);
    return 1;
  }
  printf ("done!\n");

  printf ("- Flushing all output files... ");
  fflush (NULL);
  printf ("done!\n");

  printf ("- Closing connection to digitizer... ");
  ret = CAEN_DGTZ_CloseDigitizer (Handle);
  if (ret != CAEN_DGTZ_Success) {
    printf("*** ERROR *** Unable to close digitizer connection. Error code: %d\n",ret);
    return 1;
  }
  printf ("done!\n");

  return 0;

}
