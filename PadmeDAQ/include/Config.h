#ifndef _CONFIG_H_
#define _CONFIG_H_

// Needed for special data formats (uint32_t)
#include "CAENDigitizer.h"

#define MAX_RUN_COMMENT_LEN 1024
#define MAX_DATA_FILE_LEN   1024
#define MAX_FILE_LEN        1024

// We store the board_id in 8 bits
#define MAX_N_BOARDS         256

typedef struct config_s {

  // File used to read configuration
  char config_file[MAX_FILE_LEN];

  // File to signal that board should reset its trigger timer and start taking data
  char start_file[MAX_FILE_LEN];

  // File to request DAQ stop: if this file exists, DAQ will immediately exit
  char quit_file[MAX_FILE_LEN];

  // File to signal that board has been initialized and is ready to take data
  char initok_file[MAX_FILE_LEN];

  // File to signal that board initialization failed
  char initfail_file[MAX_FILE_LEN];

  // File to use for locking (".<PID>" will be appended)
  char lock_file[MAX_FILE_LEN];

  // DB file (sqlite3)
  //char db_file[MAX_FILE_LEN];
  
  // Run number
  int run_number;

  // Run type (can be "DAQ", "COSMIC", or "TEST")
  char run_type[16];

  // Process id
  int process_id;

  // File name template for data files.
  // The date/time string "_YYYY_MM_DD_hh_mm_ss" will be appended to each generated file
  // so the actual filename will have a maximum length of (MAX_DATA_FILE_LEN+20)
  char data_file[MAX_DATA_FILE_LEN];

  // Total time (secs) to run DAQ. DAQ program will stop and exit after this time. If 0: run indefinitely
  int total_daq_time;

  // ID of board connected to this process
  int board_id;

  // Choose start DAQ mode (0 = SW control, 1 = S_IN control, 2 = Frist trg)
  int startdaq_mode;

  // Choose DRS4 sampling frequency (0 = 5GHz, 1 = 2.5GHz, 2 = 1GHz)
  int drs4_sampfreq;

  // Choose trigger mode (0 = common trigger, 1 = fast trigger, 2 = software trigger)
  int trigger_mode;

  // Choose trigger signal IO level between NIM and TTL
  char trigger_iolevel[4];

  // Mask to choose will channel groups will be enabled
  // bit 0: group 0 (ch 0-7)
  // bit 1: group 1 (ch 8-15)
  // bit 2: group 2 (ch 16-23)
  // bit 3: group 3 (ch 24-31)
  uint32_t group_enable_mask;

  // Mask to choose which channel will be processed and saved to output file
  // N.B. if group is OFF, channel will not be written even if it is ON
  // bit n: channel n (n=0-31)
  uint32_t channel_enable_mask;

  // Global DC offset (common to all channels unless individual values are given)
  uint32_t offset_global;

  // Individual DC offsets (values for disabled channels are ignored)
  uint32_t offset_ch[32];

  // Post trigger size (see V1742 manual for definition)
  uint32_t post_trigger_size;

  // Maximum number of events to transfer in a single readout (used to set buffer size)
  uint32_t max_num_events_blt;

  // Enable/disable application of DRS4 corrections to sampled data
  int drs4corr_enable;

  // Delay in the DAQ main loop (usecs)
  useconds_t daq_loop_delay;

  // Zero-suppression (0:OFF, 1-n:ON with choice of the algorithm)
  int zero_suppression;

  // Zero-suppression algorithm parameters
  int zs1_head; // Number of samples to use to compute mean and rms
  int zs1_tail; // Number of samples to reject at the end (see V1742 manual)
  float zs1_nsigma; // Number of sigmas around mean to set the threshold
  int zs1_nabovethr; // Number of consecutive above-threshold samples to accept the eventchannel
  float zs1_badrmsthr; // RMS threshold above which channel is deemed problematic and is accepted

  // Set maximum per-file DAQ duration (secs).
  // After this time the output file will be closed and a new one will be opened
  int file_max_duration;

  // Set maximum size of a single DAQ file (bytes)
  // Output file will be close when its size exceeds this value and a new one will be opened
  long file_max_size;

  // Set maximum number of events to store in a single DAQ file
  // After writing this number of events, output file will be closed and a new one will be opened
  int file_max_events;

} config_t;

extern config_t* Config; // Declare pointer to common configuration structure

int init_config();      // Initialiaze configuration and set to default
int reset_config();     // Set configuration to default
int read_config(char*); // Read configuration file
int print_config();     // Print current configuration
int save_config();      // Save config parameters into DB
int end_config();       // Clear configuration

#endif
