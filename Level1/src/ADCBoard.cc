#include <stdlib.h>
#include <stdexcept>

#include "Configuration.hh"

#include "ADCBoard.hh"

ADCBoard::ADCBoard(int board)
{

  // Connect to configuration handler
  fCfg = Configuration::GetInstance();

  // DO NOT FORGET to update this part when more formats are defined
  UInt_t max_event_len_v01 = 4*(ADCEVENT_V01_EVENTHEAD_LEN+ADCEVENT_NTRIGGERS*(ADCEVENT_V01_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_V01_GRTAIL_LEN)+ADCEVENT_NCHANNELS*ADCEVENT_NSAMPLES/2);
  UInt_t max_event_len_v02 = 4*(ADCEVENT_V02_EVENTHEAD_LEN+ADCEVENT_NTRIGGERS*(ADCEVENT_V02_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_V02_GRTAIL_LEN)+ADCEVENT_NCHANNELS*ADCEVENT_NSAMPLES/2);
  UInt_t max_event_len_v03 = 4*(ADCEVENT_V03_EVENTHEAD_LEN+ADCEVENT_NTRIGGERS*(ADCEVENT_V03_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_V03_GRTAIL_LEN)+ADCEVENT_NCHANNELS*ADCEVENT_NSAMPLES/2);

  UInt_t max_event_len = std::max(max_event_len_v01,std::max(max_event_len_v02,max_event_len_v03));

  //printf("max_event_len v01 %d v02 %d v03 %d max %d\n",max_event_len_v01,max_event_len_v02,max_event_len_v03,max_event_len);

  fBuffer = malloc(max_event_len);
  fBoardId = board;
  fADCEvent = new ADCEvent();
  Reset();
}

ADCBoard::~ADCBoard()
{
  free(fBuffer);
  delete fADCEvent;
}

void ADCBoard::Reset()
{
  //printf("Calling ADCBoard::Reset\n");
  fFiles.clear();
  if (fFileHandle && fFileHandle.is_open()) fFileHandle.close();
  fCurrentFile = 0;
  fNewFile = 0;
  fFinished = 0;
  fEventSize = 0;
  fVersion = 0;
  fClockCounter = 0;
  fTotalClockCounter = 0;
  fADCEvent->Reset();
}

void ADCBoard::Init()
{
  // Initialize board after configuration

  // Check if at least one file is in the input list
  if (fFiles.size() == 0) {
    printf("ADCBoard::Init - ERROR - Trying to initialize board with no input files. Aborting\n");
    exit(1);
  }

  // Open first input file in list
  fCurrentFile = 0;
  if (OpenFile() != 0) {
    printf("ADCBoard::Init - ERROR - Unable to open input file. Aborting\n");
    exit(1);
  }

  fFirstEvent = 1;
  fMissingEvent = 0;

}

ADCFile* ADCBoard::AddFile(std::string fileName)
{
  ADCFile* newFile = new ADCFile(fileName);
  fFiles.push_back(*newFile);
  return newFile;
}

ADCFile* ADCBoard::File(int i)
{
  try {
    return &fFiles.at(i);
  }
  catch (const std::out_of_range& err) {
    std::cout << "ADCBoard::File WARNING - File index out of range: " << err.what() << std::endl;
    return 0;
  }

}

std::string ADCBoard::GetFileName(int i)
{
  try {
    return fFiles.at(i).GetPath();
  }
  catch (const std::out_of_range& err) {
    std::cout << "ADCBoard::GetFileName WARNING - File index out of range: " << err.what() << std::endl;
    return "";
  }

}

int ADCBoard::OpenFile()
{
  // Will add some error checking later
  printf("Opening file %s\n",fFiles[fCurrentFile].GetPath().c_str());
  fFileHandle.open(fFiles[fCurrentFile].GetPath().c_str(), std::ios::in | std::ios::binary);
  fNewFile = 1;
  return 0;
}

int ADCBoard::NextEvent()
{

  int ret;
  unsigned char grMsk;
  unsigned int TT_evt = 0;
  unsigned int TT_grp[ADCEVENT_NTRIGGERS]; // Clock counters for event and for each group
  int dT; // Delta time between two clock counters

  if (fFinished) return 0;

  // If current event was not used in previous trigger, use it again
  if (fMissingEvent) {
    printf("ADCBoard::NextEvent - WARNING - Current event was not used by previous trigger: see if it is usable now\n");
    fMissingEvent = 0;
    return 1;
  }

  unsigned int board_in_time = 0;
  while (! board_in_time) {

    if (fNewFile == 1) {

      // Read file head. Returns 0:OK 1:Error
      if ( ReadFileHead() ) {
	printf("ERROR while reading header of file %s. Aborting\n",fFiles[fCurrentFile].GetPath().c_str());
	exit(1);
      }

      fVersion = fFiles[fCurrentFile].GetVersion();
      printf("File %s opened with format version %d\n",fFiles[fCurrentFile].GetPath().c_str(),fVersion);

      fNewFile = 0;

    }

    // Read next event (also handle file tail record). Returns 0:OK 1:Error 2:EOF
    ret = ReadNextEvent();
    if (ret == 1) {
      printf("ERROR while reading event from file %s. Aborting\n",fFiles[fCurrentFile].GetPath().c_str());
      exit(1);
    }
    if (ret == 2) {
      // EndOfFile reached: close current file
      fFileHandle.close();
      // Check if more files are present, if not we end here
      fCurrentFile++;
      if ( fCurrentFile >= fFiles.size() ) {
	// No more files: we are done
	fFinished = 1;
	return 0;
      }
      // More files are available: open the next one
      if (OpenFile() != 0) {
	printf("ADCBoard::NextEvent - ERROR - Unable to open next input file. Aborting\n");
	exit(1);
      }
      fNewFile = 1;
      continue; // Read next event from new file
    }

    // Check if current event is correctly time aligned
    board_in_time = 1;
    grMsk = GetGroupMask();
    if ( GetTriggerTimeTags(TT_grp) ) {
	printf("ADCBoard::NextEvent - FATAL ERROR - Unable to get board group time tags. Aborting\n");
	exit(1);
    }
    unsigned int first_active_group = 1;
    for (unsigned int g=0; g<ADCEVENT_NTRIGGERS; g++) {
      if (grMsk & 1<<g) {
	if (first_active_group) { // This is the first active group: get its time as reference for event
	  TT_evt = TT_grp[g];
	  first_active_group = 0;
	} else {
	  // Other groups are present: verify they are all in time (0x0002 clock cycles tolerance i.e. ~17ns)
	  dT = TT_evt-TT_grp[g];
	  if ( std::abs(dT) >= 0x0002 ) {
	    if (fFirstEvent) { // If this happens on first event stop the run
	      printf("*** FATAL ERROR - Board %2d - Internal time mismatch %032x %032x at Start of Run\n",GetBoardId(),TT_evt,TT_grp[g]);
	      exit(1);
	    } else {
	      board_in_time = 0;
	      printf("*** Board %2d - Internal time mismatch %032x %032x - Skipping event %8u\n",GetBoardId(),TT_evt,TT_grp[g],GetEventCounter());
	      break; // Break loop on trigger groups
	    }
	  }
	}
      }
    }

  }

  // Board is internally in time: update its time counters
  if (fFirstEvent) {
    fTotalClockCounter = 0;
    fClockCounter = TT_evt;
    fFirstEvent = 0;
  } else {
    dT = TT_evt-fClockCounter;
    if (dT<0) dT += (1<<30); // Check if clock counter rolled over
    fTotalClockCounter += dT;
    fClockCounter = TT_evt;
  }

  if (fCfg->Verbose()>=2) printf("- Board %d size %u clock %u total clock %llu\n",fBoardId,fEventSize,fClockCounter,fTotalClockCounter);

  fMissingEvent = 0;

  return 1; // An in-time event was read

}

int ADCBoard::ReadFileHead()
{

  // Read first line (4 bytes) of file header into buffer
  // NB: format of this line MUST NOT change with version!
  fFileHandle.read((char*)fBuffer,4);

  // Check if file header tag is correct
  //UChar_t tag = (UChar_t)( ( ((UInt_t*)fBuffer)[ADCEVENT_TAG_LIN] & ADCEVENT_TAG_BIT ) >> ADCEVENT_TAG_POS );
  //if (tag != ADCEVENT_FHEAD_TAG) {
  UChar_t tag = (UChar_t)( ( ((UInt_t*)fBuffer)[EVENT_TAG_LIN] & EVENT_TAG_BIT ) >> EVENT_TAG_POS );
  if (tag != EVENT_FILE_HEAD_TAG) {
    printf("ERROR - File does not start with the right tag - Expected 0x%1X - Found 0x%1X\n",EVENT_FILE_HEAD_TAG,tag);
    return 1;
  }

  // Get data format version
  //fFiles[fCurrentFile].SetVersion( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_VERSION_LIN] & ADCEVENT_VERSION_BIT) >> ADCEVENT_VERSION_POS) );
  fFiles[fCurrentFile].SetVersion( (Int_t)((((UInt_t*)fBuffer)[EVENT_VERSION_LIN] & EVENT_VERSION_BIT) >> EVENT_VERSION_POS) );

  // Unpack rest of file header according to data format version

  if (fFiles[fCurrentFile].GetVersion() == 1) {

    // Version 1
    fFileHandle.read((char*)fBuffer+4,4*(ADCEVENT_V01_FHEAD_LEN-1));
    fFiles[fCurrentFile].SetIndex( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_FILEINDEX_LIN] & ADCEVENT_V01_FILEINDEX_BIT) >> ADCEVENT_V01_FILEINDEX_POS) );
    fFiles[fCurrentFile].SetRunNumber( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_RUNNUMBER_LIN] & ADCEVENT_V01_RUNNUMBER_BIT) >> ADCEVENT_V01_RUNNUMBER_POS) );
    fFiles[fCurrentFile].SetStartTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_TTAGSOF_LIN] & ADCEVENT_V01_TTAGSOF_BIT) >> ADCEVENT_V01_TTAGSOF_POS) );

  } else if (fFiles[fCurrentFile].GetVersion() == 2) {

    // Version 2
    fFileHandle.read((char*)fBuffer+4,4*(ADCEVENT_V02_FHEAD_LEN-1));
    fFiles[fCurrentFile].SetIndex( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_FILEINDEX_LIN] & ADCEVENT_V02_FILEINDEX_BIT) >> ADCEVENT_V02_FILEINDEX_POS) );
    fFiles[fCurrentFile].SetRunNumber( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_RUNNUMBER_LIN] & ADCEVENT_V02_RUNNUMBER_BIT) >> ADCEVENT_V02_RUNNUMBER_POS) );
    fFiles[fCurrentFile].SetStartTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_TTAGSOF_LIN] & ADCEVENT_V02_TTAGSOF_BIT) >> ADCEVENT_V02_TTAGSOF_POS) );

  } else if (fFiles[fCurrentFile].GetVersion() == 3) {

    // Version 3
    fFileHandle.read((char*)fBuffer+4,4*(ADCEVENT_V03_FHEAD_LEN-1));
    fFiles[fCurrentFile].SetIndex( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_FILEINDEX_LIN] & ADCEVENT_V03_FILEINDEX_BIT) >> ADCEVENT_V03_FILEINDEX_POS) );
    fFiles[fCurrentFile].SetRunNumber( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_RUNNUMBER_LIN] & ADCEVENT_V03_RUNNUMBER_BIT) >> ADCEVENT_V03_RUNNUMBER_POS) );
    fFiles[fCurrentFile].SetBoardId( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_HBOARDID_LIN] & ADCEVENT_V03_HBOARDID_BIT) >> ADCEVENT_V03_HBOARDID_POS) );
    fFiles[fCurrentFile].SetBoardSN( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_BOARDSN_LIN] & ADCEVENT_V03_BOARDSN_BIT) >> ADCEVENT_V03_BOARDSN_POS) );
    fFiles[fCurrentFile].SetStartTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_TTAGSOF_LIN] & ADCEVENT_V03_TTAGSOF_BIT) >> ADCEVENT_V03_TTAGSOF_POS) );

  } else {

    printf("ERROR - Incompatible file version found: %d\n",fFiles[fCurrentFile].GetVersion());
    return 1;

  }

  return 0;

}

int ADCBoard::ReadNextEvent()
{

  // Read first line (4 bytes) of event header or file tail into buffer
  // Check tag to decide if it is an event header or a file tail
  fFileHandle.read((char*)fBuffer,4);
  //UChar_t tag = (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_TAG_LIN] & ADCEVENT_TAG_BIT ) >> ADCEVENT_TAG_POS);
  //if (tag == ADCEVENT_EVENT_TAG) {
  UChar_t tag = (UChar_t)((((UInt_t*)fBuffer)[EVENT_TAG_LIN] & EVENT_TAG_BIT ) >> EVENT_TAG_POS);
  if (tag == EVENT_ADCBOARD_INFO_TAG) {

    // This is an event block: get its size and read the remaining part to buffer
    //fEventSize = (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_EVENTSIZE_LIN] & ADCEVENT_EVENTSIZE_BIT ) >> ADCEVENT_EVENTSIZE_POS);
    fEventSize = (UInt_t)((((UInt_t*)fBuffer)[EVENT_EVENTSIZE_LIN] & EVENT_EVENTSIZE_BIT ) >> EVENT_EVENTSIZE_POS);
    fFileHandle.read((char*)fBuffer+4,4*(fEventSize-1));

    //} else if (tag == ADCEVENT_FTAIL_TAG) {
  } else if (tag == EVENT_FILE_TAIL_TAG) {

    // This is a file tail block
    // Read the remaining part to buffer and unpack it according to data format version

    if (fFiles[fCurrentFile].GetVersion() == 1) {

      // Version 1
      fFileHandle.read((char*)fBuffer+4,4*(ADCEVENT_V01_FTAIL_LEN-1));
      fFiles[fCurrentFile].SetNEvents( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_NEVENTS_LIN] & ADCEVENT_V01_NEVENTS_BIT) >> ADCEVENT_V01_NEVENTS_POS) );
      fFiles[fCurrentFile].SetEndTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_TTAGEOF_LIN] & ADCEVENT_V01_TTAGEOF_BIT) >> ADCEVENT_V01_TTAGEOF_POS) );
      fFiles[fCurrentFile].SetSize( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_V01_FILESIZELO_LIN]+( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_V01_FILESIZEHI_LIN] << 32 ) );

    } else if (fFiles[fCurrentFile].GetVersion() == 2) {

      // Version 2
      fFileHandle.read((char*)fBuffer+4,4*(ADCEVENT_V02_FTAIL_LEN-1));
      fFiles[fCurrentFile].SetNEvents( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_NEVENTS_LIN] & ADCEVENT_V02_NEVENTS_BIT) >> ADCEVENT_V02_NEVENTS_POS) );
      fFiles[fCurrentFile].SetEndTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_TTAGEOF_LIN] & ADCEVENT_V02_TTAGEOF_BIT) >> ADCEVENT_V02_TTAGEOF_POS) );
      fFiles[fCurrentFile].SetSize( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_V02_FILESIZELO_LIN]+( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_V02_FILESIZEHI_LIN] << 32 ) );

    } else if (fFiles[fCurrentFile].GetVersion() == 3) {

      // Version 3
      fFileHandle.read((char*)fBuffer+4,4*(ADCEVENT_V03_FTAIL_LEN-1));
      fFiles[fCurrentFile].SetNEvents( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_NEVENTS_LIN] & ADCEVENT_V03_NEVENTS_BIT) >> ADCEVENT_V03_NEVENTS_POS) );
      fFiles[fCurrentFile].SetEndTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_TTAGEOF_LIN] & ADCEVENT_V03_TTAGEOF_BIT) >> ADCEVENT_V03_TTAGEOF_POS) );
      fFiles[fCurrentFile].SetSize( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_V03_FILESIZELO_LIN]+( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_V03_FILESIZEHI_LIN] << 32 ) );

    }

    // Return EOF condition
    return 2;

  } else {

    printf("ERROR - Unknown tag found in file structure: 0x%1x\n",tag);
    return 1;
    
  }

  return 0;

}

unsigned char ADCBoard::GetGroupMask()
{
  // Get group mask
  if (fFiles[fCurrentFile].GetVersion() == 1) {
    return ((((UInt_t*)fBuffer)[ADCEVENT_V01_GRMASK_LIN] & ADCEVENT_V01_GRMASK_BIT) >> ADCEVENT_V01_GRMASK_POS);
  } else if (fFiles[fCurrentFile].GetVersion() == 2) {
    return ((((UInt_t*)fBuffer)[ADCEVENT_V02_GRMASK_LIN] & ADCEVENT_V02_GRMASK_BIT) >> ADCEVENT_V02_GRMASK_POS);
  } else if (fFiles[fCurrentFile].GetVersion() == 3) {
    return ((((UInt_t*)fBuffer)[ADCEVENT_V03_GRMASK_LIN] & ADCEVENT_V03_GRMASK_BIT) >> ADCEVENT_V03_GRMASK_POS);
  }
  printf("ADCBoard::GetGroupMask - ERROR - Invalid event version %d\n",fFiles[fCurrentFile].GetVersion());
  return 0;
}

UInt_t ADCBoard::GetEventCounter()
{
  // Get event counter
  if (fFiles[fCurrentFile].GetVersion() == 1) {
    return ((((UInt_t*)fBuffer)[ADCEVENT_V01_EVENTNUMBER_LIN] & ADCEVENT_V01_EVENTNUMBER_BIT) >> ADCEVENT_V01_EVENTNUMBER_POS);
  } else if (fFiles[fCurrentFile].GetVersion() == 2) {
    return ((((UInt_t*)fBuffer)[ADCEVENT_V02_EVENTNUMBER_LIN] & ADCEVENT_V02_EVENTNUMBER_BIT) >> ADCEVENT_V02_EVENTNUMBER_POS);
  } else if (fFiles[fCurrentFile].GetVersion() == 3) {
    return ((((UInt_t*)fBuffer)[ADCEVENT_V03_EVENTNUMBER_LIN] & ADCEVENT_V03_EVENTNUMBER_BIT) >> ADCEVENT_V03_EVENTNUMBER_POS);
  }
  printf("ADCBoard::GetGroupMask - ERROR - Invalid event version %d\n",fFiles[fCurrentFile].GetVersion());
  return 0;
}

int ADCBoard::GetTriggerTimeTags(UInt_t ttt[ADCEVENT_NTRIGGERS])
{
  // Extract all trigger time tags from buffer
  int rc = 0;
  if (fFiles[fCurrentFile].GetVersion() == 1) {
    rc = GetTriggerTimeTags_v01(ttt);
  } else if (fFiles[fCurrentFile].GetVersion() == 2) {
    rc = GetTriggerTimeTags_v02(ttt);
  } else if (fFiles[fCurrentFile].GetVersion() == 3) {
    rc = GetTriggerTimeTags_v03(ttt);
  }
  if (rc) {
    printf("ERROR while extracting trigger time tags from event buffer\n");
    return 1;
  }
  return 0;
}

int ADCBoard::GetTriggerTimeTags_v01(UInt_t ttt[ADCEVENT_NTRIGGERS])
{
  // Get group mask
  UInt_t grmsk = ((((UInt_t*)fBuffer)[ADCEVENT_V01_GRMASK_LIN] & ADCEVENT_V01_GRMASK_BIT) >> ADCEVENT_V01_GRMASK_POS);
  // Skip event header and go to first trigger structure
  UInt_t cursor = ADCEVENT_V01_EVENTHEAD_LEN;
  // Loop over all triggers
  for(UInt_t i=0;i<ADCEVENT_NTRIGGERS;i++){
    // If group is active get its TTT
    if (grmsk & (1 << i)) {
      UInt_t trsize = ((((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_GRSIZE_LIN] & ADCEVENT_V01_GRSIZE_BIT) >> ADCEVENT_V01_GRSIZE_POS);
      // TTT is last word at end of trigger structure
      ttt[i] = ((((UInt_t*)fBuffer)[cursor+trsize-1] & ADCEVENT_V01_TTAGTRIG_BIT) >> ADCEVENT_V01_TTAGTRIG_POS);
      // Skip to next trigger structure
      cursor += trsize;
    } else {
      // Trigger group is not there: set TTT to 0
      ttt[i] = 0;
    }
  }
  return 0; // All good
}

int ADCBoard::GetTriggerTimeTags_v02(UInt_t ttt[ADCEVENT_NTRIGGERS])
{
  // Get group mask
  UInt_t grmsk = ((((UInt_t*)fBuffer)[ADCEVENT_V02_GRMASK_LIN] & ADCEVENT_V02_GRMASK_BIT) >> ADCEVENT_V02_GRMASK_POS);
  // Skip event header and go to first trigger structure
  UInt_t cursor = ADCEVENT_V02_EVENTHEAD_LEN;
  // Loop over all triggers
  for(UInt_t i=0;i<ADCEVENT_NTRIGGERS;i++){
    // If group is active get its TTT
    if (grmsk & (1 << i)) {
      UInt_t trsize = ((((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_GRSIZE_LIN] & ADCEVENT_V02_GRSIZE_BIT) >> ADCEVENT_V02_GRSIZE_POS);
      // TTT is last word at end of trigger structure
      ttt[i] = ((((UInt_t*)fBuffer)[cursor+trsize-1] & ADCEVENT_V02_TTAGTRIG_BIT) >> ADCEVENT_V02_TTAGTRIG_POS);
      // Skip to next trigger structure
      cursor += trsize;
    } else {
      // Trigger group is not there: set TTT to 0
      ttt[i] = 0;
    }
  }
  return 0; // All good
}

int ADCBoard::GetTriggerTimeTags_v03(UInt_t ttt[ADCEVENT_NTRIGGERS])
{
  // Get group mask
  UInt_t grmsk = ((((UInt_t*)fBuffer)[ADCEVENT_V03_GRMASK_LIN] & ADCEVENT_V03_GRMASK_BIT) >> ADCEVENT_V03_GRMASK_POS);
  // Skip event header and go to first trigger structure
  UInt_t cursor = ADCEVENT_V03_EVENTHEAD_LEN;
  // Loop over all triggers
  for(UInt_t i=0;i<ADCEVENT_NTRIGGERS;i++){
    // If group is active get its TTT
    if (grmsk & (1 << i)) {
      UInt_t trsize = ((((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_GRSIZE_LIN] & ADCEVENT_V03_GRSIZE_BIT) >> ADCEVENT_V03_GRSIZE_POS);
      // TTT is last word at end of trigger structure
      ttt[i] = ((((UInt_t*)fBuffer)[cursor+trsize-1] & ADCEVENT_V03_TTAGTRIG_BIT) >> ADCEVENT_V03_TTAGTRIG_POS);
      // Skip to next trigger structure
      cursor += trsize;
    } else {
      // Trigger group is not there: set TTT to 0
      ttt[i] = 0;
    }
  }
  return 0; // All good
}

UInt_t ADCBoard::GetEventSize()
{
  // Return size of ADC event structure in 4-bytes words
  //return ((((UInt_t*)fBuffer)[ADCEVENT_EVENTSIZE_LIN] & ADCEVENT_EVENTSIZE_BIT) >> ADCEVENT_EVENTSIZE_POS);
  return (UInt_t)((((UInt_t*)fBuffer)[EVENT_EVENTSIZE_LIN] & EVENT_EVENTSIZE_BIT) >> EVENT_EVENTSIZE_POS);
}

UInt_t ADCBoard::GetSerialNumber()
{
  try {
    return fFiles.at(0).GetBoardSN();
  }
  catch (const std::out_of_range& err) {
    std::cout << "ADCBoard::GetSeralNumber WARNING - No files connected to board: " << err.what() << std::endl;
    return 0;
  }
}

int ADCBoard::UnpackEvent()
{

  // Get event size for final consistency check (does not depend on version)
  //fEventSize = (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_EVENTSIZE_LIN] & ADCEVENT_EVENTSIZE_BIT ) >> ADCEVENT_EVENTSIZE_POS);
  fEventSize = (UInt_t)((((UInt_t*)fBuffer)[EVENT_EVENTSIZE_LIN] & EVENT_EVENTSIZE_BIT ) >> EVENT_EVENTSIZE_POS);

  // Decode event structure according to data format version
  int rc = 0;
  if (fVersion == 1) {
    rc = UnpackEvent_v01();
  } else if (fVersion == 2) {
    rc = UnpackEvent_v02();
  } else if (fVersion == 3) {
    rc = UnpackEvent_v03();
  } else {
    printf("ADCBoard::UnpackEvent - ERROR - Data format version %u is unknown\n",fVersion);
    return 1;
  }
  if (rc) {
    printf("ADCBoard::UnpackEvent - ERROR while unpacking event\n");
    return 1;
  }
  return 0;
}

int ADCBoard::UnpackEvent_v01()
{

  // Decode event header
  fADCEvent->SetBoardId( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_BOARDID_LIN] & ADCEVENT_V01_BOARDID_BIT) >> ADCEVENT_V01_BOARDID_POS) );
  fADCEvent->SetLVDSPattern( (UShort_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_PATTERN_LIN] & ADCEVENT_V01_PATTERN_BIT ) >> ADCEVENT_V01_PATTERN_POS) );
  fADCEvent->SetStatus( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_STATUS_LIN] & ADCEVENT_V01_STATUS_BIT ) >> ADCEVENT_V01_STATUS_POS) );
  fADCEvent->SetGroupMask( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_GRMASK_LIN] & ADCEVENT_V01_GRMASK_BIT ) >> ADCEVENT_V01_GRMASK_POS) );
  fADCEvent->SetEventCounter( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_EVENTNUMBER_LIN] & ADCEVENT_V01_EVENTNUMBER_BIT ) >> ADCEVENT_V01_EVENTNUMBER_POS) );
  fADCEvent->SetEventTimeTag( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_TTAGEVENT_LIN] & ADCEVENT_V01_TTAGEVENT_BIT ) >> ADCEVENT_V01_TTAGEVENT_POS) );
  fADCEvent->SetActiveChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_CHMASKACTIVE_LIN] & ADCEVENT_V01_CHMASKACTIVE_BIT ) >> ADCEVENT_V01_CHMASKACTIVE_POS) );
  fADCEvent->SetAcceptedChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V01_CHMASKACCEPTED_LIN] & ADCEVENT_V01_CHMASKACCEPTED_BIT ) >> ADCEVENT_V01_CHMASKACCEPTED_POS) );

  // Decode trigger information
  UInt_t cursor = ADCEVENT_V01_EVENTHEAD_LEN;
  for(int ig=0;ig<ADCEVENT_NTRIGGERS;ig++){

    if (fADCEvent->GetGroupMask() & (0x1 << ig)) {

      fADCEvent->SetTriggerStartIndexCell( ig,(UShort_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_STARTIDXCELL_LIN] & ADCEVENT_V01_STARTIDXCELL_BIT) >> ADCEVENT_V01_STARTIDXCELL_POS) );
      fADCEvent->SetTriggerFrequency( ig,(UChar_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_FREQUENCY_LIN] & ADCEVENT_V01_FREQUENCY_BIT ) >> ADCEVENT_V01_FREQUENCY_POS) );
      fADCEvent->SetTriggerHasSignal( ig,(Bool_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_TRHASDATA_LIN] & ADCEVENT_V01_TRHASDATA_BIT ) >> ADCEVENT_V01_TRHASDATA_POS) );

      if (fADCEvent->GetTriggerHasSignal(ig)) {
	for(int is=0;is<ADCEVENT_NSAMPLES;is++){
	  fADCEvent->SetADCTriggerSample( ig,is,(Short_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_GRHEAD_LEN+is/2] >> (16*(is%2))) & 0xFFFF) );
	}
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_NSAMPLES%2]);
      } else {
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_GRHEAD_LEN]);
      }

      // Shift cursor to next trigger
      cursor += (UInt_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V01_GRSIZE_LIN] & ADCEVENT_V01_GRSIZE_BIT) >> ADCEVENT_V01_GRSIZE_POS);

    }

  }

  // Decode signal information
  for(int ic=0;ic<ADCEVENT_NCHANNELS;ic++){
    if ( fADCEvent->GetAcceptedChannelMask() & (0x1 << ic) ) {
      for(int is=0;is<ADCEVENT_NSAMPLES;is++){
	fADCEvent->SetADCChannelSample( ic,is,(Short_t)((((UInt_t*)fBuffer)[cursor+is/2] >> (16*(is%2))) & 0xFFFF) );
      }
      // Shift cursor to next channel
      cursor += (ADCEVENT_NSAMPLES/2 + ADCEVENT_NSAMPLES%2);
    }
  }

  // Verify event size consistency
  if (cursor != fEventSize) {
    printf("ERROR - Inconsistent event structure found for event %d: event size is %d (expected %d)\n",
	   fADCEvent->GetEventCounter(),cursor,fEventSize);
    return 1;
  }

  // All ok: return 0
  return 0;

}

int ADCBoard::UnpackEvent_v02()
{

  // Decode event header
  fADCEvent->SetBoardId( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_BOARDID_LIN] & ADCEVENT_V02_BOARDID_BIT) >> ADCEVENT_V02_BOARDID_POS) );
  fADCEvent->SetLVDSPattern( (UShort_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_PATTERN_LIN] & ADCEVENT_V02_PATTERN_BIT ) >> ADCEVENT_V02_PATTERN_POS) );
  fADCEvent->SetStatus( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_STATUS_LIN] & ADCEVENT_V02_STATUS_BIT ) >> ADCEVENT_V02_STATUS_POS) );
  fADCEvent->SetGroupMask( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_GRMASK_LIN] & ADCEVENT_V02_GRMASK_BIT ) >> ADCEVENT_V02_GRMASK_POS) );
  fADCEvent->SetEventCounter( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_EVENTNUMBER_LIN] & ADCEVENT_V02_EVENTNUMBER_BIT ) >> ADCEVENT_V02_EVENTNUMBER_POS) );
  fADCEvent->SetEventTimeTag( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_TTAGEVENT_LIN] & ADCEVENT_V02_TTAGEVENT_BIT ) >> ADCEVENT_V02_TTAGEVENT_POS) );
  fADCEvent->SetActiveChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_CHMASKACTIVE_LIN] & ADCEVENT_V02_CHMASKACTIVE_BIT ) >> ADCEVENT_V02_CHMASKACTIVE_POS) );
  fADCEvent->SetAcceptedChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V02_CHMASKACCEPTED_LIN] & ADCEVENT_V02_CHMASKACCEPTED_BIT ) >> ADCEVENT_V02_CHMASKACCEPTED_POS) );

  // Decode trigger information
  UInt_t cursor = ADCEVENT_V02_EVENTHEAD_LEN;
  for(int ig=0;ig<ADCEVENT_NTRIGGERS;ig++){

    if (fADCEvent->GetGroupMask() & (0x1 << ig)) {

      fADCEvent->SetTriggerStartIndexCell( ig,(UShort_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_STARTIDXCELL_LIN] & ADCEVENT_V02_STARTIDXCELL_BIT) >> ADCEVENT_V02_STARTIDXCELL_POS) );
      fADCEvent->SetTriggerFrequency( ig,(UChar_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_FREQUENCY_LIN] & ADCEVENT_V02_FREQUENCY_BIT ) >> ADCEVENT_V02_FREQUENCY_POS) );
      fADCEvent->SetTriggerHasSignal( ig,(Bool_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_TRHASDATA_LIN] & ADCEVENT_V02_TRHASDATA_BIT ) >> ADCEVENT_V02_TRHASDATA_POS) );

      if (fADCEvent->GetTriggerHasSignal(ig)) {
	for(int is=0;is<ADCEVENT_NSAMPLES;is++){
	  fADCEvent->SetADCTriggerSample( ig,is,(Short_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_GRHEAD_LEN+is/2] >> (16*(is%2))) & 0xFFFF) );
	}
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_NSAMPLES%2]);
      } else {
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_GRHEAD_LEN]);
      }

      // Shift cursor to next trigger
      cursor += (UInt_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V02_GRSIZE_LIN] & ADCEVENT_V02_GRSIZE_BIT) >> ADCEVENT_V02_GRSIZE_POS);

    }

  }

  // Decode signal information
  for(int ic=0;ic<ADCEVENT_NCHANNELS;ic++){
    if ( fADCEvent->GetAcceptedChannelMask() & (0x1 << ic) ) {
      for(int is=0;is<ADCEVENT_NSAMPLES;is++){
	fADCEvent->SetADCChannelSample( ic,is,(Short_t)((((UInt_t*)fBuffer)[cursor+is/2] >> (16*(is%2))) & 0xFFFF) );
      }
      // Shift cursor to next channel
      cursor += (ADCEVENT_NSAMPLES/2 + ADCEVENT_NSAMPLES%2);
    }
  }

  // Verify event size consistency
  if (cursor != fEventSize) {
    printf("ERROR - Inconsistent event structure found for event %d: event size is %d (expected %d)\n",
	   fADCEvent->GetEventCounter(),cursor,fEventSize);
    return 1;
  }

  // All ok: return 0
  return 0;

}

int ADCBoard::UnpackEvent_v03()
{

  UInt_t cursor = 0;

  // Decode event header
  fADCEvent->SetBoardId( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_BOARDID_LIN] & ADCEVENT_V03_BOARDID_BIT) >> ADCEVENT_V03_BOARDID_POS) );
  fADCEvent->SetLVDSPattern( (UShort_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_PATTERN_LIN] & ADCEVENT_V03_PATTERN_BIT ) >> ADCEVENT_V03_PATTERN_POS) );
  fADCEvent->Set0SuppAlgrtm( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_0SUPALG_LIN] & ADCEVENT_V03_0SUPALG_BIT ) >> ADCEVENT_V03_0SUPALG_POS) );
  fADCEvent->SetGroupMask( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_GRMASK_LIN] & ADCEVENT_V03_GRMASK_BIT ) >> ADCEVENT_V03_GRMASK_POS) );
  fADCEvent->SetBoardStatus( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_STATUS_LIN] & ADCEVENT_V03_STATUS_BIT ) >> ADCEVENT_V03_STATUS_POS) );
  fADCEvent->SetEventCounter( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_EVENTNUMBER_LIN] & ADCEVENT_V03_EVENTNUMBER_BIT ) >> ADCEVENT_V03_EVENTNUMBER_POS) );
  fADCEvent->SetEventTimeTag( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_TTAGEVENT_LIN] & ADCEVENT_V03_TTAGEVENT_BIT ) >> ADCEVENT_V03_TTAGEVENT_POS) );
  fADCEvent->SetActiveChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_CHMASKACTIVE_LIN] & ADCEVENT_V03_CHMASKACTIVE_BIT ) >> ADCEVENT_V03_CHMASKACTIVE_POS) );
  fADCEvent->SetAcceptedChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_V03_CHMASKACCEPTED_LIN] & ADCEVENT_V03_CHMASKACCEPTED_BIT ) >> ADCEVENT_V03_CHMASKACCEPTED_POS) );

//printf("Board Id %d LVDS %08x 0SuppAlg %u GrpMsk %1x Status %03x Event %u Time %u Active %08x Accept %08x\n",
//	 fADCEvent->GetBoardId(),fADCEvent->GetLVDSPattern(),fADCEvent->Get0SuppAlgrtm(),fADCEvent->GetGroupMask(),
//	 fADCEvent->GetBoardStatus(),fADCEvent->GetEventCounter(),fADCEvent->GetEventTimeTag(),
//	 fADCEvent->GetActiveChannelMask(),fADCEvent->GetAcceptedChannelMask());

  cursor += ADCEVENT_V03_EVENTHEAD_LEN;

  // Decode trigger information
  for(int ig=0;ig<ADCEVENT_NTRIGGERS;ig++){

    if (fADCEvent->GetGroupMask() & (0x1 << ig)) {

      fADCEvent->SetTriggerStartIndexCell( ig,(UShort_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_STARTIDXCELL_LIN] & ADCEVENT_V03_STARTIDXCELL_BIT) >> ADCEVENT_V03_STARTIDXCELL_POS) );
      fADCEvent->SetTriggerFrequency( ig,(UChar_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_FREQUENCY_LIN] & ADCEVENT_V03_FREQUENCY_BIT ) >> ADCEVENT_V03_FREQUENCY_POS) );
      fADCEvent->SetTriggerHasSignal( ig,(Bool_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_TRHASDATA_LIN] & ADCEVENT_V03_TRHASDATA_BIT ) >> ADCEVENT_V03_TRHASDATA_POS) );

      if (fADCEvent->GetTriggerHasSignal(ig)) {
	for(int is=0;is<ADCEVENT_NSAMPLES;is++){
	  fADCEvent->SetADCTriggerSample( ig,is,(Short_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_GRHEAD_LEN+is/2] >> (16*(is%2))) & 0xFFFF) );
	}
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_NSAMPLES%2]);
      } else {
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_GRHEAD_LEN]);
      }

      // Shift cursor to next trigger
      cursor += (UInt_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_V03_GRSIZE_LIN] & ADCEVENT_V03_GRSIZE_BIT) >> ADCEVENT_V03_GRSIZE_POS);

    }

  }

  // Decode signal information
  // Mask of stored channels depends on 0-suppression mode
  // - flagging (bit=1): use ActiveChannelMask
  // - rejection (bit=0): use AcceptedChannelMask
  UInt_t channel_mask = 0;
  if ( fADCEvent->GetBoardStatus() & (0x1 << ADCEVENT_V03_STATUS_ZEROSUPP_BIT) ) {
    channel_mask = fADCEvent->GetActiveChannelMask(); // Flagging
  } else {
    channel_mask = fADCEvent->GetAcceptedChannelMask(); // Rejection
  }
  for(int ic=0;ic<ADCEVENT_NCHANNELS;ic++){
    if ( channel_mask & (0x1 << ic) ) {
      for(int is=0;is<ADCEVENT_NSAMPLES;is++){
	fADCEvent->SetADCChannelSample( ic,is,(Short_t)((((UInt_t*)fBuffer)[cursor+is/2] >> (16*(is%2))) & 0xFFFF) );
	//Short_t sample = (Short_t)((((UInt_t*)fBuffer)[cursor+is/2] >> (16*(is%2))) & 0xFFFF); // DEBUG
	//if (sample<0 || sample>4095) printf("\tsample %d\n",sample); // DEBUG
	//fADCEvent->SetADCChannelSample(ic,is,sample); // DEBUG
      }
      // Shift cursor to next channel
      cursor += (ADCEVENT_NSAMPLES/2 + ADCEVENT_NSAMPLES%2);
    }
  }

  // Verify event size consistency
  if (cursor != fEventSize) {
    printf("ERROR - Inconsistent event structure found for event %d: event size is %d (expected %d)\n",
	   fADCEvent->GetEventCounter(),cursor,fEventSize);
    return 1;
  }

  // All ok: return 0
  return 0;

}
