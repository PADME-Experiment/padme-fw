#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <time.h>

#include "Config.h"

#define MAX_PARAM_NAME_LEN  128
#define MAX_PARAM_VALUE_LEN 1024

config_t* Config; // Allocate pointer to common configuration structure

// Initialiaze configuration structure and set it to default
int init_config()
{

  Config = (config_t*)malloc(sizeof(config_t));
  if (Config == NULL) {
    printf("*** ERROR *** Memory allocation for configuration structure failed.\n");
    return 1;
  }
  memset(Config,0,sizeof(config_t));
  if ( reset_config() ) {
    printf("*** ERROR *** Problem setting run configuration to default values.\n");
    return 1;
  }
  return 0;

}

// Set configuration parameters to the default values
int reset_config()
{

  //Config->process_id = 0;

  strcpy(Config->config_file,"");

  strcpy(Config->start_file,"run/start");
  strcpy(Config->quit_file,"run/quit");
  strcpy(Config->initok_file,"run/initok.trigger");
  strcpy(Config->initfail_file,"run/initfail.trigger");
  strcpy(Config->lock_file,"run/lock.trigger");

  // Default trigger board IP configuration
  strcpy(Config->trigger_addr,"192.168.60.100");
  Config->trigger_port = 7;

  Config->run_number = 0; // Dummy run

  Config->trigger_mask = 0x01; // Only BTF trigger active

  Config->busy_mask = 0x10; // Only CPU busy active

  // Set timepix shutter delay to 2 (25ns) (WARNING: do not set it to 0 as delay will become 25us!)
  Config->timepix_shutter_delay = 0x02;

  // Leave timepix shutter width to its default value (0x64 = 10us)
  Config->timepix_shutter_width = 0x64;

  // Leave delay to trigger 0 (BTF) to its default value (0x7E = 126 = 1.575us)
  Config->trigger0_delay = 0x7E;

  // Leave correlated trigger delay to its firmware default values (500us)
  Config->correlated_trigger_delay = 0x01f4;

  // Set all triggers downscaling to 1 (no downscaling)
  //Config->trig0_scale_global   = 1;
  Config->trig1_scale_global   = 1;
  Config->trig2_scale_global   = 1;
  Config->trig3_scale_global   = 1;
  Config->trig4_scale_global   = 1;
  Config->trig5_scale_global   = 1;
  Config->trig6_scale_global   = 1;
  Config->trig7_scale_global   = 1;

  // Disable autopass for all external triggers
  Config->trig0_scale_autopass = 0;
  Config->trig1_scale_autopass = 0;
  Config->trig2_scale_autopass = 0;
  Config->trig3_scale_autopass = 0;
  Config->trig4_scale_autopass = 0;
  Config->trig5_scale_autopass = 0;
  // Enable autopass for all (un)correlated triggers
  Config->trig6_scale_autopass = 1;
  Config->trig7_scale_autopass = 1;

  strcpy(Config->output_mode,"STREAM"); // Default single stream mode

  strcpy(Config->output_stream,""); // No default

  // All data files written to subdirectory "data" of current directory
  strcpy(Config->data_dir,"data/");
  strcpy(Config->data_file,"trig"); // Data filename template

  Config->total_daq_time = 0; // Run forever
  //Config->total_daq_time = 2; // While testing use a 2s run time

  // Default to localhost
  Config->node_id = 0;

  // Add a delay between successive polls to the board
  Config->daq_loop_delay = 10000; // wait 10msec after each iteration

  // Ouput file limits
  Config->file_max_duration = 3600; // 1 hour
  Config->file_max_size = 1024*1024*1024; // 1GiB
  Config->file_max_events = 1000000; // 1E6 events

  // Rate of debug output (1=all events)
  Config->debug_scale = 10; // Info about one event on 10 is written to debug output

  return 0;

}

int read_config(char *cfgfile)
{

  FILE *fin;
  char *line = NULL;
  size_t len = 0;
  ssize_t lsize;
  char param[MAX_PARAM_NAME_LEN];
  char value[MAX_PARAM_VALUE_LEN];
  size_t plen,vlen;
  unsigned short int su;
  int v;
  //long int vl;
  unsigned int vu;
  unsigned long int vul;

  regex_t rex_empty;
  regex_t rex_comment;
  regex_t rex_setting;
  int rex_err;
  size_t nm = 3;
  regmatch_t rm[3];

  // Define needed regular expressions
  if ( (rex_err = regcomp(&rex_empty,"^[[:blank:]]*$",REG_EXTENDED|REG_NEWLINE)) )
    printf("ERROR %d compiling regex rex_empty\n",rex_err);
  if ( (rex_err = regcomp(&rex_comment,"^[[:blank:]]*#",REG_EXTENDED|REG_NEWLINE)) )
    printf("ERROR %d compiling regex rex_comment\n",rex_err);
  if ( (rex_err = regcomp(&rex_setting,"^[[:blank:]]*([[:alnum:]_]+)[[:blank:]]+([[:graph:]]+)[[:blank:]]*$",REG_EXTENDED|REG_NEWLINE)) )
    printf("ERROR %d compiling regex rex_setting\n",rex_err);

  // See if a configuration file was given
  if (strcmp(cfgfile,"")==0) {
    printf("read_config - No config file specified. Will use defaults.\n");
    return 0;
  }

  // Read configuration from file
  printf("\n=== Reading configuration from file %s ===\n",cfgfile);
  if ( strlen(cfgfile)>=MAX_FILE_LEN ) {
    printf("ERROR - Configuration file name too long (%lu characters): %s\n",strlen(cfgfile),cfgfile);
    return 1;
  }
  strcpy(Config->config_file,cfgfile);
  if( (fin = fopen(Config->config_file,"r")) == NULL) {
    printf("ERROR - Cannot open configuration file %s for reading\n",Config->config_file);
    return 1;
  }
  while( (lsize=getline(&line,&len,fin)) != -1 ) {

    if ( line[lsize-1] == '\n' ) line[--lsize] = '\0'; // Remove newline
    //printf("Read line '%s'\n",line);

    // Ignore blank and comment (#) lines
    if ( regexec(&rex_empty,line,0,NULL,0) == 0 ) continue;
    if ( regexec(&rex_comment,line,0,NULL,0) == 0 ) continue;

    // Global parameters
    if ( regexec(&rex_setting,line,nm,rm,0) == 0 ) {

      // Extract parameter name
      plen = rm[1].rm_eo-rm[1].rm_so;
      if (plen>=MAX_PARAM_NAME_LEN) {
	printf("WARNING - Parameter too long (%lu characters) in line:\n%s\n",plen,line);
      } else {
	strncpy(param,line+rm[1].rm_so,plen);
	param[plen] = '\0';
      }

      // Extract parameter value
      vlen = rm[2].rm_eo-rm[2].rm_so;
      if (vlen>=MAX_PARAM_VALUE_LEN) {
	printf("WARNING - Value too long (%lu characters) in line:\n%s\n",vlen,line);
      } else {
	strncpy(value,line+rm[2].rm_so,vlen);
	value[vlen] = '\0';
      }

      //printf("Parameter <%s> Value <%s>\n",param,value);

      // Interpret parameter
      //if ( strcmp(param,"process_id")==0 ) {
      //	if ( sscanf(value,"%d",&v) ) {
      //	  Config->process_id = v;
      //	  printf("Parameter %s set to %d\n",param,v);
      //	} else {
      //	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
      //	}
      //} else if ( strcmp(param,"start_file")==0 ) {
      if ( strcmp(param,"start_file")==0 ) {
	if ( strlen(value)<MAX_FILE_LEN ) {
	  strcpy(Config->start_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - start_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"quit_file")==0 ) {
	if ( strlen(value)<MAX_FILE_LEN ) {
	  strcpy(Config->quit_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - quit_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"initok_file")==0 ) {
	if ( strlen(value)<MAX_FILE_LEN ) {
	  strcpy(Config->initok_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - initok_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"initfail_file")==0 ) {
	if ( strlen(value)<MAX_FILE_LEN ) {
	  strcpy(Config->initfail_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - initfail_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"lock_file")==0 ) {
	if ( strlen(value)<MAX_FILE_LEN ) {
	  strcpy(Config->lock_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - lock_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"trigger_addr")==0 ) {
	if ( strlen(value)<16 ) {
	  strcpy(Config->trigger_addr,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - trigger_addr string too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"trigger_port")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->trigger_port = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"run_number")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->run_number = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trigger_mask")==0 ) {
	if ( sscanf(value,"%hx",&su) ) {
	  Config->trigger_mask = (su & 0x00ff);
	  printf("Parameter %s set to 0x%02x\n",param,Config->trigger_mask);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"busy_mask")==0 ) {
	if ( sscanf(value,"%hx",&su) ) {
	  Config->busy_mask = (su & 0x001f); // Only use bits 4:0
	  printf("Parameter %s set to 0x%02x\n",param,Config->busy_mask);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"timepix_shutter_delay")==0 ) {
	if ( sscanf(value,"%hx",&su) ) {
	  Config->timepix_shutter_delay = (su & 0x00ff);
	  printf("Parameter %s set to 0x%02x (%u)\n",param,Config->timepix_shutter_delay,Config->timepix_shutter_delay);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"timepix_shutter_width")==0 ) {
	if ( sscanf(value,"%hx",&su) ) {
	  Config->timepix_shutter_width = (su & 0x00ff);
	  printf("Parameter %s set to 0x%02x (%u)\n",param,Config->timepix_shutter_width,Config->timepix_shutter_width);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trigger0_delay")==0 ) {
	if ( sscanf(value,"%hx",&su) ) {
	  Config->trigger0_delay = (su & 0x00ff);
	  printf("Parameter %s set to 0x%02x (%u)\n",param,Config->trigger0_delay,Config->trigger0_delay);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"correlated_trigger_delay")==0 ) {
	if ( sscanf(value,"%hx",&su) ) {
	  Config->correlated_trigger_delay = su;
	  printf("Parameter %s set to 0x%04x (%u)\n",param,Config->correlated_trigger_delay,Config->correlated_trigger_delay);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
/*
      } else if ( strcmp(param,"trig0_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig0_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig0_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
*/
      } else if ( strcmp(param,"trig0_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig0_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig0_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig1_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig1_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig1_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig1_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig1_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig1_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig2_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig2_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig2_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig2_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig2_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig2_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig3_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig3_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig3_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig3_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig3_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig3_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig4_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig4_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig4_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig4_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig4_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig4_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig5_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig5_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig5_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig5_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig5_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig5_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig6_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig6_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig6_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig6_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig6_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig6_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig7_scale_global")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig7_scale_global = su;
	  printf("Parameter %s set to %u\n",param,Config->trig7_scale_global);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trig7_scale_autopass")==0 ) {
	if ( sscanf(value,"%hu",&su) ) {
	  Config->trig7_scale_autopass = su;
	  printf("Parameter %s set to %u\n",param,Config->trig7_scale_autopass);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"output_mode")==0 ) {
	if ( strcmp(value,"FILE")==0 || strcmp(value,"STREAM")==0 ) {
	  strcpy(Config->output_mode,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - Unknown output mode '%s' selected: ignoring\n",value);
	}
      } else if ( strcmp(param,"output_stream")==0 ) {
	if ( strlen(value)<MAX_DATA_FILE_LEN ) {
	  strcpy(Config->output_stream,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - output_stream name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"data_dir")==0 ) {
	if ( strlen(value)<MAX_DATA_DIR_LEN ) {
	  strcpy(Config->data_dir,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - data_dir name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"data_file")==0 ) {
	if ( strlen(value)<MAX_DATA_FILE_LEN ) {
	  strcpy(Config->data_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - data_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"total_daq_time")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->total_daq_time = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"node_id")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->node_id = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"daq_loop_delay")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->daq_loop_delay = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"file_max_duration")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->file_max_duration = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"file_max_size")==0 ) {
	if ( sscanf(value,"%lu",&vul) ) {
	  Config->file_max_size = vul;
	  printf("Parameter %s set to %lu\n",param,vul);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"file_max_events")==0 ) {
	if ( sscanf(value,"%u",&vu) ) {
	  Config->file_max_events = vu;
	  printf("Parameter %s set to %d\n",param,vu);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"debug_scale")==0 ) {
	if ( sscanf(value,"%u",&vu) ) {
	  Config->debug_scale = vu;
	  printf("Parameter %s set to %d\n",param,vu);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else {
	printf("WARNING - Unknown parameter %s from line:\n%s\n",param,line);
      }
    } else {
      printf("WARNING - Could not parse line in confg file:\n%s\n",line);
    }

  }

  free(line);
  fclose(fin);

  // Release memory used be regular expressions
  regfree(&rex_empty);
  regfree(&rex_comment);
  regfree(&rex_setting);

  return 0;
}

int print_config(){

  printf("\n=== Configuration parameters for this run ===\n");
  //printf("process_id\t\t%d\t\tDB id for this process\n",Config->process_id);
  printf("config_file\t\t'%s'\tname of configuration file (can be empty)\n",Config->config_file);

  printf("start_file\t\t'%s'\tname of start file. DAQ will start when this file is created\n",Config->start_file);
  printf("quit_file\t\t'%s'\tname of quit file. DAQ will exit when this file is created\n",Config->quit_file);

  printf("initok_file\t\t'%s'\tname of initok file. Created when board is correctly initialized and ready fo DAQ\n",Config->initok_file);
  printf("initfail_file\t\t'%s'\tname of initfail file. Created when board initialization failed\n",Config->initfail_file);
  printf("lock_file\t\t'%s'\tname of lock file. Contains PID of locking process\n",Config->lock_file);

  printf("trigger_addr\t\t'%s'\tIP address of trigger board.\n",Config->trigger_addr);
  printf("trigger_port\t\t%u\t\t\tIP port number of trigger board.\n",Config->trigger_port);

  printf("run_number\t\t%d\t\trun number (0: dummy run, not saved to DB)\n",Config->run_number);

  printf("trigger_mask\t\t0x%02x\t\ttrigger mask (8 bits)\n",Config->trigger_mask);
  printf("busy_mask\t\t0x%02x\t\tbusy mask (5 bits)\n",Config->busy_mask);

  printf("timepix_shutter_delay\t0x%02x (%u)\tDelay in clock counts from BTF trigger to start of timepix readout procedure\n",Config->timepix_shutter_delay,Config->timepix_shutter_delay);
  printf("timepix_shutter_width\t0x%02x (%u)\tWidth in 100ns counts of timepix shutter\n",Config->timepix_shutter_width,Config->timepix_shutter_width);

  printf("trigger0_delay\t\t0x%02x (%u)\tTrigger 0 (BTF) distribution delay in clock counts\n",Config->trigger0_delay,Config->trigger0_delay);

  printf("correlated_trigger_delay\t0x%04x (%u)\t\tDelay in us from BTF trigger for correlated trigger\n",Config->correlated_trigger_delay,Config->correlated_trigger_delay);

  //printf("trig0_scale_global\t%u\t\tTrigger 0 downscale factor\n",Config->trig0_scale_global);
  printf("trig1_scale_global\t%u\t\tTrigger 1 downscale factor\n",Config->trig1_scale_global);
  printf("trig2_scale_global\t%u\t\tTrigger 2 downscale factor\n",Config->trig2_scale_global);
  printf("trig3_scale_global\t%u\t\tTrigger 3 downscale factor\n",Config->trig3_scale_global);
  printf("trig4_scale_global\t%u\t\tTrigger 4 downscale factor\n",Config->trig4_scale_global);
  printf("trig5_scale_global\t%u\t\tTrigger 5 downscale factor\n",Config->trig5_scale_global);
  printf("trig6_scale_global\t%u\t\tTrigger 6 downscale factor\n",Config->trig6_scale_global);
  printf("trig7_scale_global\t%u\t\tTrigger 7 downscale factor\n",Config->trig7_scale_global);

  printf("trig0_scale_autopass\t%u\t\tTrigger 0 autopass downscale factor\n",Config->trig0_scale_autopass);
  printf("trig1_scale_autopass\t%u\t\tTrigger 1 autopass downscale factor\n",Config->trig1_scale_autopass);
  printf("trig2_scale_autopass\t%u\t\tTrigger 2 autopass downscale factor\n",Config->trig2_scale_autopass);
  printf("trig3_scale_autopass\t%u\t\tTrigger 3 autopass downscale factor\n",Config->trig3_scale_autopass);
  printf("trig4_scale_autopass\t%u\t\tTrigger 4 autopass downscale factor\n",Config->trig4_scale_autopass);
  printf("trig5_scale_autopass\t%u\t\tTrigger 5 autopass downscale factor\n",Config->trig5_scale_autopass);
  printf("trig6_scale_autopass\t%u\t\tTrigger 6 autopass downscale factor\n",Config->trig6_scale_autopass);
  printf("trig7_scale_autopass\t%u\t\tTrigger 7 autopass downscale factor\n",Config->trig7_scale_autopass);

  printf("output_mode\t\t%s\t\toutput mode (FILE or STREAM)\n",Config->output_mode);
  if (strcmp(Config->output_mode,"STREAM")==0) {
    printf("output_stream\t\t'%s'\tname of virtual file used as output stream\n",Config->output_stream);
  } else {
    printf("data_dir\t\t'%s'\t\tdirectory where output files will be stored\n",Config->data_dir);
    printf("data_file\t\t'%s'\ttemplate name for data files: <date/time> string will be appended\n",Config->data_file);
  }

  printf("node_id\t\t\t%d\t\tDB id of node running the process\n",Config->node_id);

  printf("total_daq_time\t\t%d\t\ttime (secs) after which daq will stop. 0=run forever\n",Config->total_daq_time);
  printf("daq_loop_delay\t\t%d\t\twait time inside daq loop in usecs\n",Config->daq_loop_delay);

  // These are only relevant for FILE output mode as in STREAM mode the output file never changes
  if (strcmp(Config->output_mode,"FILE")==0) {
    printf("file_max_duration\t%d\t\tmax time to write data before changing output file\n",Config->file_max_duration);
    printf("file_max_size\t\t%lu\tmax size of output file before changing it\n",Config->file_max_size);
    printf("file_max_events\t\t%u\t\tmax number of events to write before changing output file\n",Config->file_max_events);
  }

  printf("debug_scale\t\t%u\t\tDebug output downscale factor\n",Config->debug_scale);

  printf("=== End of configuration parameters ===\n\n");

  return 0;

}

/*
// Save configuration parameters to DB
int save_config()
{

  //int i;
  //char line[2048];


  //db_add_cfg_para(Config->process_id,"config_file",Config->config_file);
  printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"config_file",Config->config_file);

  //db_add_cfg_para(Config->process_id,"start_file",Config->start_file);
  //db_add_cfg_para(Config->process_id,"quit_file",Config->quit_file);
  //db_add_cfg_para(Config->process_id,"initok_file",Config->initok_file);
  //db_add_cfg_para(Config->process_id,"initfail_file",Config->initfail_file);
  //db_add_cfg_para(Config->process_id,"lock_file",Config->lock_file);
  printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"start_file",Config->start_file);
  printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"quit_file",Config->quit_file);
  printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"initok_file",Config->initok_file);
  printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"initfail_file",Config->initfail_file);
  printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"lock_file",Config->lock_file);

  //sprintf(line,"%d",Config->run_number);
  //db_add_cfg_para(Config->process_id,"run_number",line);
  //printf("DBINFO - add_proc_config_para %d %s %d\n",Config->process_id,"run_number",Config->run_number);

  printf("DBINFO - add_proc_config_para %d %s 0x%02x\n",Config->process_id,"trigger_mask",Config->trigger_mask);
  printf("DBINFO - add_proc_config_para %d %s 0x%02x\n",Config->process_id,"busy_mask",Config->busy_mask);

  printf("DBINFO - add_proc_config_para %d %s 0x%02x\n",Config->process_id,"timepix_shutter_delay",Config->timepix_shutter_delay);
  printf("DBINFO - add_proc_config_para %d %s 0x%02x\n",Config->process_id,"timepix_shutter_width",Config->timepix_shutter_width);

  printf("DBINFO - add_proc_config_para %d %s 0x%02x\n",Config->process_id,"trigger0_delay",Config->trigger0_delay);

  printf("DBINFO - add_proc_config_para %d %s 0x%04x\n",Config->process_id,"correlated_trigger_delay",Config->correlated_trigger_delay);

  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig1_scale_global",Config->trig1_scale_global);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig2_scale_global",Config->trig2_scale_global);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig3_scale_global",Config->trig3_scale_global);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig4_scale_global",Config->trig4_scale_global);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig5_scale_global",Config->trig5_scale_global);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig6_scale_global",Config->trig6_scale_global);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig7_scale_global",Config->trig7_scale_global);

  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig0_scale_autopass",Config->trig0_scale_autopass);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig1_scale_autopass",Config->trig1_scale_autopass);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig2_scale_autopass",Config->trig2_scale_autopass);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig3_scale_autopass",Config->trig3_scale_autopass);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig4_scale_autopass",Config->trig4_scale_autopass);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig5_scale_autopass",Config->trig5_scale_autopass);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig6_scale_autopass",Config->trig6_scale_autopass);
  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"trig7_scale_autopass",Config->trig7_scale_autopass);

  //db_add_cfg_para(Config->process_id,"output_mode",Config->output_mode);
  printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"output_mode",Config->output_mode);
  if (strcmp(Config->output_mode,"STREAM")==0) {
    //db_add_cfg_para(Config->process_id,"output_stream",Config->output_stream);
    printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"output_stream",Config->output_stream);
  } else {
    //db_add_cfg_para(Config->process_id,"data_dir",Config->data_dir);
    //db_add_cfg_para(Config->process_id,"data_file",Config->data_file);
    printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"data_dir",Config->data_dir);
    printf("DBINFO - add_proc_config_para %d %s %s\n",Config->process_id,"data_file",Config->data_file);
  }

  //sprintf(line,"%d",Config->node_id);
  //db_add_cfg_para(Config->process_id,"node_id",line);
  //printf("DBINFO - add_proc_config_para %d %s %d\n",Config->process_id,"node_id",Config->node_id);

  //sprintf(line,"%d",Config->total_daq_time);
  //db_add_cfg_para(Config->process_id,"total_daq_time",line);
  printf("DBINFO - add_proc_config_para %d %s %d\n",Config->process_id,"total_daq_time",Config->total_daq_time);

  //sprintf(line,"%u",Config->daq_loop_delay);
  //db_add_cfg_para(Config->process_id,"daq_loop_delay",line);
  printf("DBINFO - add_proc_config_para %d %s %d\n",Config->process_id,"daq_loop_delay",Config->daq_loop_delay);

  // In STREAM mode the output file never changes
  if (strcmp(Config->output_mode,"FILE")==0) {

    //sprintf(line,"%u",Config->file_max_duration);
    //db_add_cfg_para(Config->process_id,"file_max_duration",line);
    printf("DBINFO - add_proc_config_para %d %s %d\n",Config->process_id,"file_max_duration",Config->file_max_duration);

    //sprintf(line,"%lu",Config->file_max_size);
    //db_add_cfg_para(Config->process_id,"file_max_size",line);
    printf("DBINFO - add_proc_config_para %d %s %lu\n",Config->process_id,"file_max_size",Config->file_max_size);

    //sprintf(line,"%u",Config->file_max_events);
    //db_add_cfg_para(Config->process_id,"file_max_events",line);
    printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"file_max_events",Config->file_max_events);

  }

  printf("DBINFO - add_proc_config_para %d %s %u\n",Config->process_id,"debug_scale",Config->debug_scale);

  return 0;

}
*/

int end_config()
{
  free(Config);
  return 0;
}

// Return file name given the file open time. Return 0 if OK, <>0 error
int generate_filename(char* name, const time_t time) {
  struct tm* t = gmtime(&time);
  sprintf(name,"%s_%.4d_%.2d_%.2d_%.2d_%.2d_%.2d",
	  Config->data_file,
	  1900+t->tm_year, 1+t->tm_mon, t->tm_mday,
	  t->tm_hour,      t->tm_min,   t->tm_sec);
  return 0;
}

// Write time (in secs) to a string with standard formatting
char* format_time(const time_t time) {
  static char tform[20];
  //struct tm* t = localtime(&time);
  struct tm* t = gmtime(&time);
  sprintf(tform,"%.4d/%.2d/%.2d %.2d:%.2d:%.2d",
	  1900+t->tm_year, 1+t->tm_mon, t->tm_mday,
	  t->tm_hour,      t->tm_min,   t->tm_sec);
  return tform;
}
