#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "regex.h"

#include "DB.h"

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

  int ch;

  strcpy(Config->config_file,"");

  strcpy(Config->quit_file,"run/quit");
  strcpy(Config->start_file,"run/start");
  strcpy(Config->initok_file,"run/initok.b00"); // InitOK file for default board 0
  strcpy(Config->initfail_file,"run/initfail.b00"); // InitFail file for default board 0
  strcpy(Config->lock_file,"run/lock.b00"); // Lock file for default board 0
  //strcpy(Config->db_file,"db/PadmeDAQ.db");

  // All data files written to subdirectory "data" of current directory
  strcpy(Config->data_file,"data/daq_b00"); // Data filename template for default board 0

  // Dummy test run
  Config->run_number = 0;
  strcpy(Config->run_type,"TEST");
  Config->process_id = 0; // Id of process in the DB

  Config->total_daq_time = 0; // Run forever
  //Config->total_daq_time = 2; // While testing use a 2s run time

  Config->board_id = 0; // Board 0 is the default board used for testing

  Config->startdaq_mode = 0; // Default to SW controlled start/stop

  Config->drs4_sampfreq = 2; // Default to 1GHz sampling frequency

  Config->trigger_mode = 1; // Use fast trigger mode (0:ext trigger, 1: fast trigger, 2:sw trigger)

  strcpy(Config->trigger_iolevel,"NIM"); // Triggers expect NIM levels (NIM or TTL)

  Config->group_enable_mask = 0xf; // All groups are ON
  //Config->group_enable_mask = 0x1; // Only group 0 is ON (ch 0-7)
  //Config->group_enable_mask = 0x3; // Only groups 0 and 1 are ON (ch 0-7 and 8-15)

  Config->channel_enable_mask = 0xffffffff; // All channels are ON

  // All offsets are at default value of 0x5600
  Config->offset_global = 0x5600;
  for(ch=0;ch<32;ch++) Config->offset_ch[ch] = Config->offset_global;

  // Delay of trigger wrt start of sample
  Config->post_trigger_size = 65;

  // Use digitizer buffer up to its full size (128 events for V1742, 1024 for V1742B)
  Config->max_num_events_blt = 128;

  // Do not apply zero-suppression (test phase, this default will change in production)
  Config->zero_suppression = 0;

  // Set default parameters for zero-suppression algorithm
  Config->zs1_head = 80; // Use first 80 samples to compute mean and rms
  Config->zs1_tail = 30; // Do not use final 30 samples for zero suppression
  Config->zs1_nsigma = 3.; // Threshold set a mean +/- 3*rms
  Config->zs1_nabovethr = 4; // Require at least 4 consecutive samples above threshold to accept the channel
  Config->zs1_badrmsthr = 15.; // If rms is above 15 counts, channel has problems and is accepted

  // Enable DRS4 corrections to sampled data
  Config->drs4corr_enable = 1;

  // Add a delay between successive polls to the board
  Config->daq_loop_delay = 100000; // wait 0.1 sec after each iteration

  // Ouput file limits
  Config->file_max_duration = 900; // 15 min
  Config->file_max_size = 1024*1024*1024; // 1GiB
  Config->file_max_events = 100000; // 1E5 events

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
  uint32_t v;
  float vf;
  int ch;

  regex_t rex_empty;
  regex_t rex_comment;
  regex_t rex_setting;
  regex_t rex_chsetting;
  int rex_err;
  size_t nm = 3;
  regmatch_t rm[3];
  size_t ncm = 4;
  regmatch_t rcm[4];

  // Define needed regular expressions
  if ( (rex_err = regcomp(&rex_empty,"^[[:blank:]]*$",REG_EXTENDED|REG_NEWLINE)) )
    printf("ERROR %d compiling regex rex_empty\n",rex_err);
  if ( (rex_err = regcomp(&rex_comment,"^[[:blank:]]*#",REG_EXTENDED|REG_NEWLINE)) )
    printf("ERROR %d compiling regex rex_comment\n",rex_err);
  if ( (rex_err = regcomp(&rex_setting,"^[[:blank:]]*([[:alnum:]_]+)[[:blank:]]+([[:graph:]]+)[[:blank:]]*$",REG_EXTENDED|REG_NEWLINE)) )
    printf("ERROR %d compiling regex rex_setting\n",rex_err);
  if ( (rex_err = regcomp(&rex_chsetting,"^[[:blank:]]*([[:alnum:]_]+)[[:blank:]]+([[:digit:]]+)[[:blank:]]+([[:graph:]]+)[[:blank:]]*$",REG_EXTENDED|REG_NEWLINE)) )
    printf("ERROR %d compiling regex rex_chsetting\n",rex_err);

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
      if ( strcmp(param,"quit_file")==0 ) {
	if ( strlen(value)<MAX_FILE_LEN ) {
	  strcpy(Config->quit_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - quit_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"start_file")==0 ) {
	if ( strlen(value)<MAX_FILE_LEN ) {
	  strcpy(Config->start_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - start_file name too long (%lu characters): %s\n",strlen(value),value);
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
	//} else if ( strcmp(param,"db_file")==0 ) {
	//	if ( strlen(value)<MAX_FILE_LEN ) {
	//	  strcpy(Config->db_file,value);
	//	  printf("Parameter %s set to '%s'\n",param,value);
	//	} else {
	//	  printf("WARNING - db_file name too long (%lu characters): %s\n",strlen(value),value);
	//	}
      } else if ( strcmp(param,"data_file")==0 ) {
	if ( strlen(value)<MAX_DATA_FILE_LEN ) {
	  strcpy(Config->data_file,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - data_file name too long (%lu characters): %s\n",strlen(value),value);
	}
      } else if ( strcmp(param,"run_number")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->run_number = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"run_type")==0 ) {
	if ( strcmp(value,"DAQ")==0 || strcmp(value,"COSMIC")==0 || strcmp(value,"TEST")==0 ) {
	  strcpy(Config->run_type,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - Unknown run type '%s' selected: ignoring\n",value);
	}
      } else if ( strcmp(param,"total_daq_time")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->total_daq_time = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"board_id")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  if (v<MAX_N_BOARDS) {
	    Config->board_id = v;
	    printf("Parameter %s set to %d\n",param,v);
	  } else {
	    printf("WARNING - board_id set to %d, must be < %d\n",v,MAX_N_BOARDS);
	  }
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"startdaq_mode")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  if ( v>2 ) {
	    printf("WARNING - Invalid value for startdaq_mode: %d. Accepted: 0,1,2\n",v);
	  } else {
	    Config->startdaq_mode = v;
	    printf("Parameter %s set to %d\n",param,v);
	  }
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"drs4_sampfreq")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  if ( v>2 ) {
	    printf("WARNING - Invalid value for drs4_sampfreq: %d. Accepted: 0,1,2\n",v);
	  } else {
	    Config->drs4_sampfreq = v;
	    printf("Parameter %s set to %d\n",param,v);
	  }
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trigger_mode")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  if ( v>2 ) {
	    printf("WARNING - Invalid value for trigger_mode: %d. Accepted: 0,1,2\n",v);
	  } else {
	    Config->trigger_mode = v;
	    printf("Parameter %s set to %d\n",param,v);
	  }
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"trigger_iolevel")==0 ) {
	if ( strcmp(value,"NIM")==0 || strcmp(value,"TTL")==0 ) {
	  strcpy(Config->trigger_iolevel,value);
	  printf("Parameter %s set to '%s'\n",param,value);
	} else {
	  printf("WARNING - Value %s not valid for parameter %s: use NIM or TTL\n",value,param);
	}
      } else if ( strcmp(param,"group_enable_mask")==0 ) {
	if ( sscanf(value,"%x",&v) ) {
	  Config->group_enable_mask = v;
	  printf("Parameter %s set to 0x%1x\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"channel_enable_mask")==0 ) {
	if ( sscanf(value,"%x",&v) ) {
	  Config->channel_enable_mask = v;
	  printf("Parameter %s set to 0x%08x\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"offset_global")==0 ) {
	if ( sscanf(value,"%x",&v) ) {
	  Config->offset_global = v;
	  for(ch=0;ch<32;ch++) Config->offset_ch[ch] = Config->offset_global;
	  printf("Parameter %s set to 0x%04x\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"post_trigger_size")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->post_trigger_size = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"max_num_events_blt")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->max_num_events_blt = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"drs4corr_enable")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->drs4corr_enable = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"zero_suppression")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->zero_suppression = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"zs1_head")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->zs1_head = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"zs1_tail")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->zs1_tail = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"zs1_nsigma")==0 ) {
	if ( sscanf(value,"%f",&vf) ) {
	  Config->zs1_nsigma = vf;
	  printf("Parameter %s set to %f\n",param,vf);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"zs1_nabovethr")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->zs1_nabovethr = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"zs1_badrmsthr")==0 ) {
	if ( sscanf(value,"%f",&vf) ) {
	  Config->zs1_badrmsthr = vf;
	  printf("Parameter %s set to %f\n",param,vf);
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
	if ( sscanf(value,"%d",&v) ) {
	  Config->file_max_size = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else if ( strcmp(param,"file_max_events")==0 ) {
	if ( sscanf(value,"%d",&v) ) {
	  Config->file_max_events = v;
	  printf("Parameter %s set to %d\n",param,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else {
	printf("WARNING - Unknown parameter %s from line:\n%s\n",param,line);
      }

    // Channel parameters
    } else if ( regexec(&rex_chsetting,line,ncm,rcm,0) == 0 ) {

      //printf("Matches rex_chsetting\n");
      //printf("0 %d %d\n",rcm[0].rm_so,rcm[0].rm_eo);
      //printf("1 %d %d\n",rcm[1].rm_so,rcm[1].rm_eo);
      //printf("2 %d %d\n",rcm[2].rm_so,rcm[2].rm_eo);
      //printf("3 %d %d\n",rcm[3].rm_so,rcm[3].rm_eo);

      // Extract parameter name
      plen = rcm[1].rm_eo-rcm[1].rm_so;
      if (plen>=MAX_PARAM_NAME_LEN) {
	printf("WARNING - Parameter too long (%lu characters) in line:\n%s\n",plen,line);
      } else {
	strncpy(param,line+rcm[1].rm_so,plen);
	param[plen] = '\0';
      }

      // Extract channel number
      ch = -1;
      vlen = rcm[2].rm_eo-rcm[2].rm_so;
      if (vlen>=3) {
	printf("WARNING - Character number too long (%lu characters) in line:\n%s\n",vlen,line);
      } else {
	strncpy(value,line+rcm[2].rm_so,vlen);
	value[vlen] = '\0';
	if ( sscanf(value,"%d",&ch) ) {
	  if (ch>=32) printf("WARNING - Wrong channel number %d in line:\n%s\n",ch,line);
	} else {
	  printf("WARNING - Could not parse value %s to channel number in line:\n%s\n",value,line);
	}
      }
      if (ch<0 || ch>31) {
	printf("WARNING - Invalid channel number %d in line:\n%s\n",ch,line);
	continue;
      }

      // Extract parameter value
      vlen = rcm[3].rm_eo-rcm[3].rm_so;
      if (vlen>=MAX_PARAM_VALUE_LEN) {
	printf("WARNING - Value too long (%lu characters) in line:\n%s\n",vlen,line);
      } else {
	strncpy(value,line+rcm[3].rm_so,vlen);
	value[vlen] = '\0';
      }

      //printf("Parameter <%s> Channel <%d> Value <%s>\n",param,ch,value);

      if ( strcmp(param,"offset_ch")==0 ) {
	if ( sscanf(value,"%x",&v) ) {
	  Config->offset_ch[ch] = v;
	  printf("Parameter %s for channel %d set to 0x%04x\n",param,ch,v);
	} else {
	  printf("WARNING - Could not parse value %s to number in line:\n%s\n",value,line);
	}
      } else {
	printf("WARNING - Unknown channel parameter %s from line:\n%s\n",param,line);
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
  regfree(&rex_chsetting);

  return 0;
}

int print_config(){

  int i;

  printf("\n=== Configuration parameters for this run ===\n");
  printf("config_file\t\t'%s'\tname of configuration file (can be empty)\n",Config->config_file);
  printf("start_file\t\t'%s'\tname of start file. DAQ will start when this file is created\n",Config->start_file);
  printf("quit_file\t\t'%s'\tname of quit file. DAQ will exit when this file is created\n",Config->quit_file);
  printf("initok_file\t\t'%s'\tname of initok file. Created when board is correctly initialized and ready fo DAQ\n",Config->initok_file);
  printf("initfail_file\t\t'%s'\tname of initfail file. Created when board initialization failed\n",Config->initfail_file);
  printf("lock_file\t\t'%s'\tname of lock file. Contains PID of locking process\n",Config->lock_file);
  //printf("db_file\t\t\t'%s'\tname of db file (sqlite3)\n",Config->db_file);
  //printf("data_dir\t\t'%s'\t\tpath where data files will be written\n",Config->data_dir);
  printf("data_file\t\t'%s'\ttemplate name for data files: <date/time> string will be appended\n",Config->data_file);
  printf("run_number\t\t%d\t\trun number (0: dummy run, not saved to DB)\n",Config->run_number);
  printf("run_type\t\t'%s'\t\trun type (DAQ, COSMIC, TEST)\n",Config->run_type);
  printf("board_id\t\t%d\t\tboard ID\n",Config->board_id);
  printf("startdaq_mode\t\t%d\t\tstart/stop daq mode (0:SW, 1:S_IN, 2:trg)\n",Config->startdaq_mode);
  printf("drs4_sampfreq\t\t%d\t\tDRS4 sampling frequency (0:5GHz, 1:2.5GHz, 2:1GHz)\n",Config->drs4_sampfreq);
  printf("trigger_mode\t\t%d\t\ttrigger mode (0:ext, 1:fast, 2:sw)\n",Config->trigger_mode);
  printf("trigger_iolevel\t\t'%s'\t\ttrigger signal IO level (NIM or TTL)\n",Config->trigger_iolevel);
  printf("group_enable_mask\t0x%1x\t\tmask to enable groups of channels\n",Config->group_enable_mask);
  printf("channel_enable_mask\t0x%08x\tmask to enable individual channels\n",Config->channel_enable_mask);
  printf("offset_global\t\t0x%04x\t\tglobal DC offset\n",Config->offset_global);
  for(i=0;i<32;i++) {
    if (Config->offset_ch[i] != Config->offset_global) printf("offset_ch\t%.2d\t0x%04x\n",i,Config->offset_ch[i]);
  }
  printf("post_trigger_size\t%d\t\tpost trigger size\n",Config->post_trigger_size);
  printf("max_num_events_blt\t%d\t\tmax number of events to transfer in a single readout\n",Config->max_num_events_blt);
  printf("drs4corr_enable\t\t%d\t\tenable (1) or disable (0) DRS4 corrections to sampled data\n",Config->drs4corr_enable);
  printf("zero_suppression\t%d\t\tdisable (0) or enable/choose (>0) zero-suppression algorithm\n",Config->zero_suppression);
  if (Config->zero_suppression == 1) {
    printf("zs1_head\t\t%d\t\tnumber of samples to use to compute mean and rms\n",Config->zs1_head);
    printf("zs1_tail\t\t%d\t\tnumber of samples to reject at the end\n",Config->zs1_tail);
    printf("zs1_nsigma\t\t%5.3f\t\tnumber of sigmas around mean used to set the threshold\n",Config->zs1_nsigma);
    printf("zs1_nabovethr\t\t%d\t\tnumber of consecutive above-threshold samples required to accept the channel\n",Config->zs1_nabovethr);
    printf("zs1_badrmsthr\t\t%5.1f\t\trms value above which channel is accepted as problematic\n",Config->zs1_badrmsthr);
  }
  printf("total_daq_time\t\t%d\t\ttime (secs) after which daq will stop. 0=run forever\n",Config->total_daq_time);
  printf("daq_loop_delay\t\t%d\t\twait time inside daq loop in usecs\n",Config->daq_loop_delay);
  printf("file_max_duration\t%d\t\tmax time to write data before changing output file\n",Config->file_max_duration);
  printf("file_max_size\t\t%lu\tmax size of output file before changing it\n",Config->file_max_size);
  printf("file_max_events\t\t%d\t\tmax number of events to write before changing output file\n",Config->file_max_events);
  printf("=== End of configuration parameters ===\n\n");

  return 0;

}

// Save configuration parameters to DB
int save_config()
{

  int i;
  char line[2048];

  db_add_cfg_para(Config->process_id,"config_file",Config->config_file);

  db_add_cfg_para(Config->process_id,"start_file",Config->start_file);

  db_add_cfg_para(Config->process_id,"quit_file",Config->quit_file);

  db_add_cfg_para(Config->process_id,"initok_file",Config->initok_file);

  db_add_cfg_para(Config->process_id,"initfail_file",Config->initfail_file);

  db_add_cfg_para(Config->process_id,"lock_file",Config->lock_file);

  //db_add_cfg_para(Config->process_id,"db_file",Config->db_file);

  sprintf(line,"%d",Config->run_number);
  db_add_cfg_para(Config->process_id,"run_number",line);

  db_add_cfg_para(Config->process_id,"run_type",Config->run_type);

  sprintf(line,"%d",Config->process_id);
  db_add_cfg_para(Config->process_id,"process_id",line);

  //db_add_cfg_para(Config->process_id,"data_dir",Config->data_dir);

  db_add_cfg_para(Config->process_id,"data_file",Config->data_file);

  sprintf(line,"%d",Config->total_daq_time);
  db_add_cfg_para(Config->process_id,"total_daq_time",line);

  sprintf(line,"%d",Config->board_id);
  db_add_cfg_para(Config->process_id,"board_id",line);

  sprintf(line,"%d",Config->startdaq_mode);
  db_add_cfg_para(Config->process_id,"startdaq_mode",line);

  sprintf(line,"%d",Config->drs4_sampfreq);
  db_add_cfg_para(Config->process_id,"drs4_sampfreq",line);

  sprintf(line,"%d",Config->trigger_mode);
  db_add_cfg_para(Config->process_id,"trigger_mode",line);

  db_add_cfg_para(Config->process_id,"trigger_iolevel",Config->trigger_iolevel);

  sprintf(line,"0x%1x",Config->group_enable_mask);
  db_add_cfg_para(Config->process_id,"group_enable_mask",line);

  sprintf(line,"0x%08x",Config->channel_enable_mask);
  db_add_cfg_para(Config->process_id,"channel_enable_mask",line);

  sprintf(line,"0x%04x",Config->offset_global);
  db_add_cfg_para(Config->process_id,"offset_global",line);

  for(i=0;i<32;i++) {
    if (Config->offset_ch[i] != Config->offset_global) {
      sprintf(line,"%02d 0x%04x",i,Config->offset_ch[i]);
      db_add_cfg_para(Config->process_id,"offset_ch",line);
    }
  }

  sprintf(line,"%d",Config->post_trigger_size);
  db_add_cfg_para(Config->process_id,"post_trigger_size",line);

  sprintf(line,"%d",Config->max_num_events_blt);
  db_add_cfg_para(Config->process_id,"max_num_events_blt",line);

  sprintf(line,"%d",Config->drs4corr_enable);
  db_add_cfg_para(Config->process_id,"drs4corr_enable",line);

  sprintf(line,"%d",Config->daq_loop_delay);
  db_add_cfg_para(Config->process_id,"daq_loop_delay",line);

  sprintf(line,"%d",Config->zero_suppression);
  db_add_cfg_para(Config->process_id,"zero_suppression",line);

  if (Config->zero_suppression == 1) {
    sprintf(line,"%d",Config->zs1_head);
    db_add_cfg_para(Config->process_id,"zs1_head",line);
    sprintf(line,"%d",Config->zs1_tail);
    db_add_cfg_para(Config->process_id,"zs1_tail",line);
    sprintf(line,"%f",Config->zs1_nsigma);
    db_add_cfg_para(Config->process_id,"zs1_nsigma",line);
    sprintf(line,"%d",Config->zs1_nabovethr);
    db_add_cfg_para(Config->process_id,"zs1_nabovethr",line);
    sprintf(line,"%f",Config->zs1_badrmsthr);
    db_add_cfg_para(Config->process_id,"zs1_badrmsthr",line);
  }

  sprintf(line,"%d",Config->file_max_duration);
  db_add_cfg_para(Config->process_id,"file_max_duration",line);

  sprintf(line,"%lu",Config->file_max_size);
  db_add_cfg_para(Config->process_id,"file_max_size",line);

  sprintf(line,"%d",Config->file_max_events);
  db_add_cfg_para(Config->process_id,"file_max_events",line);

  return 0;

}

int end_config()
{
  free(Config);
  return 0;
}
