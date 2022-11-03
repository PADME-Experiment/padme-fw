#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cmath>

#include "ADCBoard.hh"
#include "Configuration.hh"
#include "DBService.hh"
#include "RootIO.hh"
#include "EventTags.hh"

int main(int argc, char* argv[])
{

  //int rc; // DB library return code

  // Set standard output/error in unbuffered mode
  setbuf(stdout,NULL);
  setbuf(stderr,NULL);

  // Make sure we are on a sane machine (int: 32bits, long int: 64bits)
  if (sizeof(int) < 4 || sizeof(long int) < 8) {
    printf("*** ERROR *** On this machine int is %lu bytes and long int is %lu bytes. Aborting.\n",sizeof(int),sizeof(long int));
    exit(1);
  }

  // Connect to configuration handler
  Configuration* cfg = Configuration::GetInstance();

  // Get default parameters from configurator
  int runnr = cfg->RunNumber();
  unsigned int nevts = cfg->NEventsPerFile();
  unsigned int verbose = cfg->Verbose();
  std::string rawhead = cfg->RawFileHeader();
  std::string instream = cfg->InputStream();

  // Parse options
  int c;
  while ((c = getopt (argc, argv, "n:r:i:o:v:h")) != -1) {
    switch (c)
      {
      case 'r':
	if (runnr!=cfg->RunNumber()) {
          fprintf (stderr, "Error while processing option '-r'. Multiple runs specified.\n");
          exit(1);
	}
        if ( sscanf(optarg,"%d",&runnr) != 1 ) {
          fprintf (stderr, "Error while processing option '-r'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (runnr<0) {
          fprintf (stderr, "Error while processing option '-r'. Run number set to %d (must be >=0).\n", runnr);
          exit(1);
        }
        fprintf(stdout,"Filtering files from run %d\n",runnr);
	cfg->SetRunNumber(runnr);
        break;
      case 'n':
        if ( sscanf(optarg,"%u",&nevts) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        fprintf(stdout,"Writing up to %u events per output file\n",nevts);
	cfg->SetNEventsPerFile(nevts);
        break;
      case 'o':
        rawhead = optarg;
        fprintf(stdout,"Set output rawdata file header to '%s'\n",rawhead.c_str());
	cfg->SetRawFileHeader(rawhead);
        break;
      case 'i':
        instream = optarg;
        fprintf(stdout,"Data will be read from stream '%s'\n",instream.c_str());
	cfg->SetInputStream(instream);
        break;
      case 'v':
        if ( sscanf(optarg,"%u",&verbose) != 1 ) {
          fprintf (stderr, "Error while processing option '-v'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        fprintf(stdout,"Set verbose level to %u\n",verbose);
	cfg->SetVerbose(verbose);
        break;
      case 'h':
        fprintf(stdout,"\nPadmeLevel1 -i input_stream [-o rawfile_head] [-r run_number] [-n events] [-v level] [-h]\n\n");
        fprintf(stdout,"  -i: define input stream FIFO file\n");
        fprintf(stdout,"  -o: define rawdata output files header. Includes path. (default: %s)\n",cfg->RawFileHeader().c_str());
        fprintf(stdout,"  -r: define run number being processes (default: %d)\n",cfg->RunNumber());
        fprintf(stdout,"  -n: define max number of events per output file (0=no limit, default: %u)\n",cfg->NEventsPerFile());
        fprintf(stdout,"  -v: define verbose level (default: %u)\n",cfg->Verbose());
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        exit(0);
      case '?':
        if (optopt == 'v') {
          // verbose with no argument: increas verbose level by 1
          cfg->SetVerbose(cfg->Verbose()+1);
          break;
        } else if (optopt == 'r' || optopt == 'i' || optopt == 'o' || optopt == 'n')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        exit(1);
      default:
        abort();
      }
  }

  // Check if input stream was defined
  if (cfg->InputStream().compare("")==0) {
    printf("ERROR no input stream defined with -i. Aborting\n");
    exit(1);
  }

  // Connect to root services
  RootIO* root = new RootIO();
  if ( root->Init() != ROOTIO_OK ) {
    printf("ERROR while initializing root output. Aborting\n");
    exit(1);
  }

  // Open input stream
  std::ifstream input_stream_handle;
  printf("Opening stream %s\n",cfg->InputStream().c_str());
  input_stream_handle.open(cfg->InputStream().c_str(), std::ios::in | std::ios::binary);

  // We are now ready to process data: get start time
  time_t time_start;
  time(&time_start);
  //printf("=== PadmeLevel1 starting on %s UTC ===\n",format_time(time_start));
  printf("=== PadmeLevel1 starting on %s UTC ===\n",cfg->FormatTime(time_start));

  printf("DBINFO - %s - process_set_status %d\n",cfg->FormatTime(time(0)),DB_STATUS_RUNNING);
  printf("DBINFO - %s - process_set_time_start %s\n",cfg->FormatTime(time(0)),cfg->FormatTime(time_start));

  // Define counters for input stream size and number of events
  unsigned long int input_size = 0;
  unsigned int number_of_events = 0;

  // Define 4B buffer used to read single lines from input file
  unsigned int buff;

  // First line: tag,version,index
  input_stream_handle.read((char*)&buff,4);
  unsigned int tag = ((buff & 0xf0000000) >> 28);

  if (tag != EVENT_FILE_HEAD_TAG) {
    printf("PadmeLevel1 - ERROR - Wrong file header tag: 0x%1x (expected 0x%1x)\n",tag,EVENT_FILE_HEAD_TAG);
    input_stream_handle.close();
    root->Exit();
    exit(1);
  }
  unsigned int version = ((buff & 0x0fff0000) >> 16);
  if (version != 3) {
    printf("PadmeLevel1 - ERROR - Wrong data format version: %u (allowed version: 3)\n",version);
    input_stream_handle.close();
    root->Exit();
    exit(1);
  }
  unsigned int findex = ((buff & 0x0000ffff) >> 0);
  if (findex > 0) {
    printf("PadmeLevel1 - WARNING - Input file is part %u of a multi-file batch. Is this correct?\n",findex);
  }

  // Second line: run number
  int run_number;
  input_stream_handle.read((char*)&run_number,4);
  if (run_number != cfg->RunNumber()) {
    printf("PadmeLevel1 - ERROR - Wrong run number found in data: %d (expected %d)\n",run_number,cfg->RunNumber());
    input_stream_handle.close();
    root->Exit();
    exit(1);
  }

  // Third line: number of ADC boards
  unsigned int number_of_boards;
  input_stream_handle.read((char*)&number_of_boards,4);
  if (number_of_boards == 0) {
    printf("PadmeLevel1 - WARNING - DAQ stream contains 0 ADC boards. Is this correct?\n");
  }
  if (number_of_boards > 32) {
    printf("PadmeLevel1 - WARNING - DAQ stream contains %u ADC boards. Is this correct?\n",number_of_boards);
  }

  printf("Run number %d with %u ADC boards\n",run_number,number_of_boards);

  // Read board serial numbers and allocate board structures
  ADCBoard* boards[number_of_boards];
  unsigned int board_id[number_of_boards];
  unsigned int board_sn[number_of_boards];
  unsigned int expected_adcboards = 0;
  for (unsigned int i=0; i<number_of_boards; i++) {
    input_stream_handle.read((char*)&buff,4);
    board_id[i] = ((buff & 0xff000000) >> 24);
    board_sn[i] = ((buff & 0x00ffffff) >>  0);
    boards[i] = new ADCBoard(i);
    // Tell board which data format to handle
    boards[i]->SetVersion(version);
    expected_adcboards |= (1 << board_id[i]); // Add current board to mask of expected ADC boards
    printf("- Board %u Id %u SN %u\n",i,board_id[i],board_sn[i]);
  }
  printf("Expected ADC boards mask is 0x%08x\n",expected_adcboards);

  // Last line of header contains the start of file time tag
  input_stream_handle.read((char*)&buff,4);
  //printf("Time tag size %lu value %ld\n",sizeof(time_t),start_time_tag);
  time_t start_time_tag = buff;
  tm* stt = gmtime(&start_time_tag);
  printf("- Input stream was created on %04d-%02d-%02d %02d:%02d:%02d\n",1900+stt->tm_year,stt->tm_mon+1,stt->tm_mday,stt->tm_hour,stt->tm_min,stt->tm_sec);

  // Add size of file header to input size counter
  input_size += 4*(4+number_of_boards);

  // Define some flags/counters to verify event structure consistency
  unsigned int header_found = 0;
  unsigned int trigger_found = 0;
  unsigned int adcboard_counter = 0;
  unsigned int event_size = 0;

  // Define variables to hold event header information
  unsigned int total_event_size = 0;
  unsigned int event_number = 0;
  struct timespec event_time;
  unsigned long int event_run_time = 0;
  unsigned int event_trigger_mask = 0;
  unsigned int event_status = 0;
  unsigned int missing_adcboards = 0;

  // Define variables to hold trigger information
  unsigned long int trigger_data = 0;
  unsigned int trigger_mask = 0;
  unsigned int trigger_counter = 0;
  unsigned char trigger_fifo = 0;
  unsigned char trigger_auto = 0;
  unsigned long int trigger_clock = 0;

  // Main loop on input events
  while(1) {

    input_stream_handle.read((char*)&buff,4);
    unsigned int tag = ((buff & 0xf0000000) >> 28);
    //printf("buff 0x%08x tag 0x%01x\n",buff,tag);

    // Check if tag is one of those allowed here
    if (tag != EVENT_FILE_TAIL_TAG && tag != EVENT_HEADER_TAG && tag != EVENT_TRIGGER_INFO_TAG && tag != EVENT_ADCBOARD_INFO_TAG) {
      printf("PadmeLevel1 - ERROR - Unknown tag 0x%1x found. Aborting\n",tag);
    }

    // If we found a new event header or the file tail, save previous event (if any) before proceeding
    if ( (tag == EVENT_HEADER_TAG) || (tag == EVENT_FILE_TAIL_TAG) ) {

      if (header_found) {

	// Check if event is complete and size is consistent. Issue warning if they are not
	if (! trigger_found) printf("PadmeLevel1 - WARNING - Event %u - No Trigger information found\n",event_number);
	if (adcboard_counter != number_of_boards) {
	  printf("PadmeLevel1 - WARNING - Event %u - Expected data from %u ADC boards, only %u received\n",event_number,number_of_boards,adcboard_counter);
	  printf("                        Expected ADC boards mask 0x%08x\n",expected_adcboards);
	  printf("                        Missing  ADC boards mask 0x%08x\n",missing_adcboards);
	}
	if (event_size != 4*total_event_size) printf("PadmeLevel1 - WARNING - Event %u - Expected %u bytes and received %u bytes\n",event_number,total_event_size,event_size);

	// Unpack info in all ADC boards
	//for (unsigned int i=0; i<number_of_boards; i++) {
	for (unsigned int i=0; i<adcboard_counter; i++) {
	  unsigned int rc = boards[i]->UnpackEvent();
	  if (rc) {
	    printf("PadmeLevel1 - ERROR - Event %u - Problem while unpacking event (rc=%u). Aborting\n",event_number,rc);
	    input_stream_handle.close();
	    root->Exit();
	    exit(1);
	  }
	  // Add board serial number
	  boards[i]->Event()->SetBoardSN(board_sn[i]);
	}

	// Send event to ROOT
	//if (root->FillRawEvent(run_number,event_number,event_time,event_run_time,event_trigger_mask,event_status,trigger_mask,trigger_counter,trigger_clock,number_of_boards,boards) != ROOTIO_OK) {
	if (root->FillRawEvent(run_number,event_number,event_time,event_run_time,event_trigger_mask,event_status,missing_adcboards,trigger_mask,trigger_counter,trigger_clock,trigger_fifo,trigger_auto,adcboard_counter,boards) != ROOTIO_OK) {
	  printf("PadmeLevel1 - ERROR while writing event %u (evt nr %u) to root file. Aborting\n",number_of_events,event_number);
	  input_stream_handle.close();
	  root->Exit();
	  exit(1);
	}

	// Show some debug info once in a while
	if (number_of_events%cfg->DebugScale() == 0) {
	  printf("%7u Run %7u Event %7u Time %9lu.%09lus Clock %12lu TrigMask 0x%04x Status 0x%04x Boards %2u Missing boards 0x%08x\n",number_of_events,run_number,event_number,event_time.tv_sec,event_time.tv_nsec,event_run_time,event_trigger_mask,event_status,adcboard_counter,missing_adcboards);
	}

	// Increase event counter
	number_of_events++;

	// Reset counters for next event
	header_found = 0;
	trigger_found = 0;
	adcboard_counter = 0;
	event_size = 0;

      }

    }

    // End of File procedure
    if (tag == EVENT_FILE_TAIL_TAG) {

      // Check if previous event was complete and issue warning if not
      if (header_found || trigger_found || adcboard_counter || event_size) {
	printf("PadmeLevel1 - WARNING - End of File tag 0x%1x found but previous event was not complete: H %d T %d B %d S %d\n",EVENT_FILE_TAIL_TAG,header_found,trigger_found,adcboard_counter,event_size);
      }

      // File tail tag found: decode final data and exit main loop
      printf("- End of File tag 0x%1x received: finalizing Level1 procedure and exiting.\n",EVENT_FILE_TAIL_TAG);

      // Check if total number of events is consistent
      unsigned int eof_nevents = ((buff & 0x0fffffff) >> 0);
      if (eof_nevents != number_of_events) {
	printf("PadmeLevel1 - WARNING - Input stream reports %u events but %u were received.\n",eof_nevents,number_of_events);
      }

      // Get total size of input stream
      unsigned long eof_size;
      input_stream_handle.read((char*)&eof_size,8);

      // Get end of stream time tag and show it
      input_stream_handle.read((char*)&buff,4);
      time_t eof_time_tag = buff;
      tm* ett = gmtime(&eof_time_tag);
      printf("- Input stream was finalized on %04d-%02d-%02d %02d:%02d:%02d\n",1900+ett->tm_year,ett->tm_mon+1,ett->tm_mday,ett->tm_hour,ett->tm_min,ett->tm_sec);

      // Add size of file tail and check if total input size is consistent with that reported by file tail
      input_size += 4*4;
      if (eof_size != input_size) {
	printf("PadmeLevel1 - WARNING - Input stream reports %lu size but %lu bytes were received.\n",eof_size,input_size);
      }

      break;

    }

    // Event Header procedure
    if (tag == EVENT_HEADER_TAG) {

      // Check if previous event was correctly completed
      //if (header_found) {
      //	printf("PadmeLevel1 - ERROR - Event is not complete but new Event Header found. Aborting\n");
      //	input_stream_handle.close();
      //	exit(1);
      //}
      header_found = 1;

      // First word: total size of this event (4-bytes words!)
      total_event_size = ((buff & EVENT_EVENTSIZE_BIT) >> EVENT_EVENTSIZE_POS);

      // Second word: event number
      input_stream_handle.read((char*)&event_number,4);

      // Third+fourth words: event time (timespec structure: sec+nsec)
      input_stream_handle.read((char*)&buff,4);
      event_time.tv_sec = buff;
      input_stream_handle.read((char*)&buff,4);
      event_time.tv_nsec = buff;
      
      // Fifth+sixth words: run time of event
      input_stream_handle.read((char*)&event_run_time,8);

      // Seventh word: event status+trigger mask
      input_stream_handle.read((char*)&buff,4);
      event_status = ( (buff & EVENT_V03_EVENTSTATUS_BIT) >> EVENT_V03_EVENTSTATUS_POS );
      event_trigger_mask = ( (buff & EVENT_V03_TRIGMASK_BIT) >> EVENT_V03_TRIGMASK_POS );

      // Eighth word: missing adc boards bit mask
      input_stream_handle.read((char*)&missing_adcboards,4);

      // Add header size to event size and to total input size
      event_size += 32;
      input_size += 4*8;

      //printf("Header found. Event size %u number %u time %lu.%09lu run_time %lu status %04x trig_mask %04x miss_mask %08x\n",total_event_size,event_number,event_time.tv_sec,event_time.tv_nsec,event_run_time,event_status,event_trigger_mask,missing_adcboards);

    }

    // Trigger Info procedure
    if (tag == EVENT_TRIGGER_INFO_TAG) {

      // Check if a header was previously found
      if (! header_found) {
	printf("PadmeLevel1 - ERROR - Trigger Info found but no Event Header. Aborting\n");
	input_stream_handle.close();
	root->Exit();
	exit(1);
      }

      // Check if this is the first trigger found in this event
      if (trigger_found) {
	printf("PadmeLevel1 - ERROR - Multiple Trigger Info structures found in event. Aborting\n");
	input_stream_handle.close();
	root->Exit();
	exit(1);
      }
      trigger_found = 1;

      // First word: tag + size of trigger event (currently fixed to 3 words)
      unsigned int trigger_size = 4*((buff & 0x0fffffff) >> 0);
      if (trigger_size != 4*3) {
	printf("PadmeLevel1 - ERROR - Trigger Info structures with anomalous size (%u) found in event. Aborting\n",trigger_size);
	input_stream_handle.close();
	root->Exit();
	exit(1);
      }

      // Second+third word: 64bits trigger data
      input_stream_handle.read((char*)&trigger_data,8);

      // Extract trigger mask, trigger counter and clock counter from trigger data
      trigger_mask = ((trigger_data & TRIGEVENT_V03_TRIGGERMASK_BIT) >> TRIGEVENT_V03_TRIGGERMASK_POS);
      trigger_counter = ((trigger_data & TRIGEVENT_V03_TRIGGERCOUNT_BIT) >> TRIGEVENT_V03_TRIGGERCOUNT_POS);
      trigger_clock =  ((trigger_data & TRIGEVENT_V03_CLOCKCOUNT_BIT) >> TRIGEVENT_V03_CLOCKCOUNT_POS);
      trigger_fifo = ((trigger_data & TRIGEVENT_V03_TRIGGERFIFO_BIT) >> TRIGEVENT_V03_TRIGGERFIFO_POS);
      trigger_auto = ((trigger_data & TRIGEVENT_V03_TRIGGERAUTO_BIT) >> TRIGEVENT_V03_TRIGGERAUTO_POS);
      //printf("Trigger found. Data 0x%016lx Mask 0x%03x Count 0x%04x Clock 0x%lu\n",trigger_data,trigger_mask,trigger_counter,trigger_clock);

      // Add trigger info size to event size and total input size
      event_size += 12;
      input_size += 4*3;

    }

    // ADC Board procedure
    if (tag == EVENT_ADCBOARD_INFO_TAG) {

      // Check if a header was previously found
      if (! header_found) {
	printf("PadmeLevel1 - ERROR - ADC Board found but no Event Header. Aborting\n");
	input_stream_handle.close();
	root->Exit();
	exit(1);
      }

      // Check if too many ADC boards were found
      if (adcboard_counter >= number_of_boards) {
	printf("PadmeLevel1 - ERROR - %u ADC boards found. Expected %u. Aborting\n",adcboard_counter,number_of_boards);
	input_stream_handle.close();
	root->Exit();
	exit(1);
      }

      // First line: get total size (in bytes) of ADC board info
      unsigned int adcboard_size = 4*((buff & 0x0fffffff) >> 0);

      //printf("adcboard_size %d\n",adcboard_size);

      // Get buffer address for current board and copy info into it
      void* adc_data = boards[adcboard_counter]->Buffer();
      //printf("Addresses 0x%016x 0x%016x\n",*(boards[adcboard_counter]->Buffer()),*adc_data);

      // The first line was already read
      memcpy(adc_data,&buff,4);
      // Now read the rest of the data
      input_stream_handle.read((char*)(adc_data)+4,adcboard_size-4);

      /*
      memcpy(&buff,(char*)adc_data,4);
      printf("Buffer line 0 0x%08x\n",buff);
      memcpy(&buff,(char*)adc_data+4,4);
      printf("Buffer line 1 0x%08x\n",buff);
      memcpy(&buff,(char*)adc_data+8,4);
      printf("Buffer line 2 0x%08x\n",buff);
      memcpy(&buff,(char*)adc_data+12,4);
      printf("Buffer line 3 0x%08x\n",buff);
      memcpy(&buff,(char*)adc_data+16,4);
      printf("Buffer line 4 0x%08x\n",buff);
      memcpy(&buff,(char*)adc_data+20,4);
      printf("Buffer line 5 0x%08x\n",buff);
      */

      adcboard_counter++;

      // Add ADC board data to total size of event and of input data
      event_size += adcboard_size;
      input_size += adcboard_size;

    }

    /*
    // Check if event is complete
    if (header_found && trigger_found && (adcboard_counter == number_of_boards)) {

      // Check event size for consistency
      if (event_size != 4*total_event_size) {
	printf("PadmeLevel1 - WARNING - Expected %u bytes and received %u bytes in event %u",total_event_size,event_size,event_number);
      }

      // Event is complete: unpack info in all ADC boards
      for (unsigned int i=0; i<number_of_boards; i++) {
	unsigned int rc = boards[i]->UnpackEvent();
	if (rc) {
	  printf("PadmeLevel1 - ERROR - Problem while unpacking event %u (rc=%u). Aborting\n",event_number,rc);
	  input_stream_handle.close();
	  root->Exit();
	  exit(1);
	}
	// Add board serial number
	boards[i]->Event()->SetBoardSN(board_sn[i]);
      }

      // Send event to ROOT
      if (root->FillRawEvent(run_number,event_number,event_time,event_run_time,event_trigger_mask,event_status,trigger_mask,trigger_counter,trigger_clock,number_of_boards,boards) != ROOTIO_OK) {
	printf("PadmeLevel1 - ERROR while writing event %u (evt nr %u) to root file. Aborting\n",number_of_events,event_number);
	input_stream_handle.close();
	root->Exit();
	exit(1);
      }

      // Increase event counter
      number_of_events++;
      if (number_of_events%100 == 0) {
	printf("- Written event %u - Run %d Event %u\n",number_of_events,run_number,event_number);
      }

      // Reset counters for next event
      header_found = 0;
      trigger_found = 0;
      adcboard_counter = 0;
      event_size = 0;

    }
    */

  }

  // Data processing in over: get stop time
  time_t time_stop;
  time(&time_stop);

  // Finalize root file
  if (root->Exit() != ROOTIO_OK) {
      printf("PadmeLevel1 - ERROR while finalizing root file. Aborting\n");
      exit(1);
  }

  // Verify that all events sent to ROOT were written to file
  if (number_of_events != root->GetTotalEvents()) {
    printf("PadmeLevel1 - WARNING - ROOT inconsistency: %u events received, %u events written. This should never happen!",number_of_events,root->GetTotalEvents());
  }

  double dt = time_stop-time_start;
  double evtpsec = 0.; if (dt>0.) evtpsec = root->GetTotalEvents()/dt;
  double bytepsec = 0.; if (dt>0.) bytepsec = root->GetTotalSize()/dt;
  printf("Level1 processing for Run %d done.\n",cfg->RunNumber());
  printf("Total processing time: %ld secs\n",time_stop-time_start);
  printf("Total files created: %u\n",root->GetTotalFiles());
  printf("Events written: %u (%6.1f events/sec)\n",root->GetTotalEvents(),evtpsec);
  printf("Bytes written: %llu (%10.1f bytes/sec)\n",root->GetTotalSize(),bytepsec);

  printf("DBINFO - %s - process_set_status %d\n",cfg->FormatTime(time(0)),DB_STATUS_FINISHED);
  printf("DBINFO - %s - process_set_time_stop %s\n",cfg->FormatTime(time(0)),cfg->FormatTime(time_stop));
  printf("DBINFO - %s - process_set_n_files %d\n",cfg->FormatTime(time(0)),root->GetTotalFiles());
  printf("DBINFO - %s - process_set_total_events %d\n",cfg->FormatTime(time(0)),root->GetTotalEvents());
  printf("DBINFO - %s - process_set_total_size %lld\n",cfg->FormatTime(time(0)),root->GetTotalSize());

  // Show exit time
  printf("=== PadmeLevel1 exiting on %s UTC ===\n",cfg->FormatTime(time(0)));

  exit(0);

}
