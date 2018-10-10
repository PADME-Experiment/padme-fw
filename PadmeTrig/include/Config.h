#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <unistd.h>

// Parameters used for configuration definitions
#define MAX_RUN_COMMENT_LEN 1024
#define MAX_DATA_DIR_LEN    1024
#define MAX_DATA_FILE_LEN    235
#define MAX_FILE_LEN        1024

// Parameters used for final filenames
#define TIME_TAG_LEN 20
#define MAX_FILENAME_LEN MAX_DATA_FILE_LEN+TIME_TAG_LEN
#define MAX_N_OUTPUT_FILES 10240

typedef struct config_s {

  // Process id in PadmeDAQ DB
  int process_id;

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

  // Run number
  int run_number;

  // Trigger mask
  unsigned char trigger_mask;

  // Output mode (can be "FILE" or "STREAM").
  char output_mode[16];

  // Name of the virtual file used for streaming out data. Only used when output_mode is "STREAM"
  char output_stream[MAX_DATA_FILE_LEN];

  // Directory path where data files will be written. Only used when output_mode is "FILE"
  char data_dir[MAX_DATA_DIR_LEN];

  // File name template for data files. Only used when output_mode is "FILE"
  // The date/time string "_YYYY_MM_DD_hh_mm_ss" will be appended to each generated file
  // so the actual filename will have a maximum length of (MAX_DATA_FILE_LEN+20)
  char data_file[MAX_DATA_FILE_LEN];

  // Total time (secs) to run DAQ. DAQ program will stop and exit after this time. If 0: run indefinitely
  int total_daq_time;

  // DB id of node where this process is running
  int node_id;

  // Delay in the Trigger main loop (usecs)
  useconds_t daq_loop_delay;

  // Set maximum per-file DAQ duration (secs).
  // After this time the output file will be closed and a new one will be opened
  int file_max_duration;

  // Set maximum size of a single DAQ file (bytes)
  // Output file will be close when its size exceeds this value and a new one will be opened
  unsigned long int file_max_size;

  // Set maximum number of events to store in a single DAQ file
  // After writing this number of events, output file will be closed and a new one will be opened
  unsigned int file_max_events;

} config_t;

extern config_t* Config; // Declare pointer to common configuration structure

int init_config();      // Initialiaze configuration and set to default
int reset_config();     // Set configuration to default
int read_config(char*); // Read configuration file
int print_config();     // Print current configuration
int save_config();      // Save config parameters into DB
int end_config();       // Clear configuration

int generate_filename(char*, const time_t); // Function to add time tag to file name
char* format_time(const time_t); // Function to format time tags

#endif
