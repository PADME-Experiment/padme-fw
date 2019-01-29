#include <stdlib.h>
#include <stdexcept>

#include "EventTags.hh"
#include "Configuration.hh"

#include "Trigger.hh"

Trigger::Trigger()
{

  // Connect to configuration handler
  fCfg = Configuration::GetInstance();

  UInt_t max_event_len = 16; // Header 12, Event 12, Tail 16
  fBuffer = malloc(max_event_len);

  Reset();

}

Trigger::~Trigger()
{
  free(fBuffer);
}

void Trigger::Reset()
{
  //printf("Calling Trigger::Reset\n");
  fFiles.clear();
  if (fFileHandle && fFileHandle.is_open()) fFileHandle.close();
  fCurrentFile = 0;
  fNewFile = 0;
  fFinished = 0;
  fEventSize = 0;
  fVersion = 0;
  fTriggerMask = 0;
  fTriggerCounter = 0;
  fClockCounter = 0;
  fTotalClockCounter = 0;
}

void Trigger::Init()
{
  // Initialize trigger after configuration

  // Check if at least one file is in the input list
  if (fFiles.size() == 0) {
    printf("Trigger::Init - ERROR - Trying to initialize trigger with no input files. Aborting\n");
    exit(1);
  }

  // Open first input file in list
  fCurrentFile = 0;
  if (OpenFile() != 0) {
    printf("Trigger::Init - ERROR - Unable to open input file. Aborting\n");
    exit(1);
  }

  fFirstEvent = 1;

}

TrigFile* Trigger::AddFile(std::string fileName)
{
  TrigFile* newFile = new TrigFile(fileName);
  fFiles.push_back(*newFile);
  return newFile;
}

TrigFile* Trigger::File(int i)
{
  try {
    return &fFiles.at(i);
  }
  catch (const std::out_of_range& err) {
    std::cout << "Trigger::File WARNING - File index out of range: " << err.what() << std::endl;
    return 0;
  }

}

std::string Trigger::GetFileName(int i)
{
  try {
    return fFiles.at(i).GetPath();
  }
  catch (const std::out_of_range& err) {
    std::cout << "Trigger::GetFileName WARNING - File index out of range: " << err.what() << std::endl;
    return "";
  }

}

int Trigger::OpenFile()
{
  // Will add some error checking later
  printf("Opening file %s\n",fFiles[fCurrentFile].GetPath().c_str());
  fFileHandle.open(fFiles[fCurrentFile].GetPath().c_str(), std::ios::in | std::ios::binary);
  fNewFile = 1;
  return 0;
}

int Trigger::NextEvent()
{

  int ret;

  if (fFinished) return 0;

  // Save clock counter of previous event
  ULong64_t old_clock_counter = fClockCounter;

  while(1){

    if (fNewFile == 1) {

      // Read file head. Returns 0:OK 1:Error
      ret = ReadFileHead();
      if ( ret == 1 ) {
	printf("ERROR while reading header of file %s. Aborting\n",fFiles[fCurrentFile].GetPath().c_str());
	exit(1);
      }

      fVersion = fFiles[fCurrentFile].GetVersion();
      printf("File %s opened with format version %d\n",
	     fFiles[fCurrentFile].GetPath().c_str(),fVersion);

      fNewFile = 0;

    }

    // Read next event (also handle file tail record). Returns 0:OK 1:Error 2:EOF
    ret = ReadNextEvent();
    if (ret == 1){
      printf("ERROR while reading event from file %s. Aborting\n",fFiles[fCurrentFile].GetPath().c_str());
      exit(1);
    }
    if (ret == 2){
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
	printf("Trigger::NextEvent - ERROR - Unable to open next input file. Aborting\n");
	exit(1);
      }
      fNewFile = 1;
      continue;
    }

    break; // We now have an event to process: exit while loop

  }

  // Update trigger time counters
  if (fFirstEvent) {
    //printf("evt 0 old %llu new %llu\n",old_clock_counter,fClockCounter);
    fTotalClockCounter = 0; // Reset global time counter on first trigger
    fFirstEvent = 0;
  } else {
    Long64_t dT = fClockCounter - old_clock_counter;
    if (dT<0) dT += (1LL<<40); // Compensate for clock counter roll over (NEED VERIFICATION!)
    fTotalClockCounter += dT;
    //printf("evt N old %llu new %llu dT %lld TCC %llu\n",old_clock_counter,fClockCounter,dT,fTotalClockCounter);
  }

  if (fCfg->Verbose()>=2) printf("- Trigger size %u clock %llu total clock %llu\n",fEventSize,fClockCounter,fTotalClockCounter);

  return 1;

}

int Trigger::ReadFileHead()
{

  // Read first line (4 bytes) of file header into buffer
  // NB: format of this line MUST NOT change with version!
  fFileHandle.read((char*)fBuffer,4);
  if (!fFileHandle) {
    printf("ERROR - Error while reading file header first word\n");
    return 1;
  }

  // Check if file header tag is correct
  UChar_t tag = (UChar_t)( ( ((UInt_t*)fBuffer)[EVENT_TAG_LIN] & EVENT_TAG_BIT ) >> EVENT_TAG_POS );
  if (tag != EVENT_FILE_HEAD_TAG) {
    printf("ERROR - File does not start with the right tag - Expected 0x%1X - Found 0x%1X\n",EVENT_FILE_HEAD_TAG,tag);
    return 1;
  }

  // Get data format version
  fFiles[fCurrentFile].SetVersion( (Int_t)((((UInt_t*)fBuffer)[EVENT_VERSION_LIN] & EVENT_VERSION_BIT) >> EVENT_VERSION_POS) );

  // Unpack rest of file header according to data format version

  if (fFiles[fCurrentFile].GetVersion() == 3) {

    // Version 3
    fFileHandle.read((char*)fBuffer+4,4*(TRIGEVENT_V03_FHEAD_LEN-1));
    if (!fFileHandle) {
      printf("ERROR - Error while reading file header\n");
      return 1;
    }
    fFiles[fCurrentFile].SetIndex( (Int_t)((((UInt_t*)fBuffer)[TRIGEVENT_V03_FILEINDEX_LIN] & TRIGEVENT_V03_FILEINDEX_BIT) >> TRIGEVENT_V03_FILEINDEX_POS) );
    fFiles[fCurrentFile].SetRunNumber( (Int_t)((((UInt_t*)fBuffer)[TRIGEVENT_V03_RUNNUMBER_LIN] & TRIGEVENT_V03_RUNNUMBER_BIT) >> TRIGEVENT_V03_RUNNUMBER_POS) );
    fFiles[fCurrentFile].SetStartTime( (UInt_t)((((UInt_t*)fBuffer)[TRIGEVENT_V03_TTAGSOF_LIN] & TRIGEVENT_V03_TTAGSOF_BIT) >> TRIGEVENT_V03_TTAGSOF_POS) );

  } else {

    printf("ERROR - Incompatible file version found: %d\n",fFiles[fCurrentFile].GetVersion());
    return 1;

  }

  return 0;

}

int Trigger::ReadNextEvent()
{

  // Read first line (4 bytes) of event header or file tail into buffer
  // Check tag to decide if it is an event header or a file tail
  fFileHandle.read((char*)fBuffer,4);
  if (!fFileHandle) {
    printf("ERROR - Error while reading event header first word\n");
    return 1;
  }
  UChar_t tag = (UChar_t)((((UInt_t*)fBuffer)[EVENT_TAG_LIN] & EVENT_TAG_BIT ) >> EVENT_TAG_POS);
  if (tag == EVENT_TRIGGER_INFO_TAG) {

    // This is a trigger block: get its size and read the remaining part to buffer
    fEventSize = (UInt_t)((((UInt_t*)fBuffer)[EVENT_EVENTSIZE_LIN] & EVENT_EVENTSIZE_BIT ) >> EVENT_EVENTSIZE_POS);
    fFileHandle.read((char*)fBuffer+4,4*(fEventSize-1));
    if (!fFileHandle) {
      printf("ERROR - Error while reading event header\n");
      return 1;
    }
    //printf("Event size %u Buffer 0x",fEventSize);
    //for(unsigned int i=0;i<fEventSize*4;i++) printf("%02x",((unsigned char*)fBuffer)[i]);
    //printf("\n");

    return UnpackEvent();

  } else if (tag == EVENT_FILE_TAIL_TAG) {

    // This is a file tail block
    // Read the remaining part to buffer and unpack it according to data format version

    if (fFiles[fCurrentFile].GetVersion() == 3) {

      // Version 3
      fFileHandle.read((char*)fBuffer+4,4*(TRIGEVENT_V03_FTAIL_LEN-1));
      if (!fFileHandle) {
	printf("ERROR - Error while reading file tail\n");
	return 1;
      }
      fFiles[fCurrentFile].SetNEvents( (Int_t)((((UInt_t*)fBuffer)[TRIGEVENT_V03_NEVENTS_LIN] & TRIGEVENT_V03_NEVENTS_BIT) >> TRIGEVENT_V03_NEVENTS_POS) );
      fFiles[fCurrentFile].SetEndTime( (UInt_t)((((UInt_t*)fBuffer)[TRIGEVENT_V03_TTAGEOF_LIN] & TRIGEVENT_V03_TTAGEOF_BIT) >> TRIGEVENT_V03_TTAGEOF_POS) );
      fFiles[fCurrentFile].SetSize( (ULong_t)((UInt_t*)fBuffer)[TRIGEVENT_V03_FILESIZELO_LIN]+( (ULong_t)((UInt_t*)fBuffer)[TRIGEVENT_V03_FILESIZEHI_LIN] << 32 ) );

    }

    // Return EOF condition
    return 2;

  } else {

    printf("ERROR - Unknown tag found in file structure: 0x%1x\n",tag);
    return 1;
    
  }

  return 0;

}

int Trigger::UnpackEvent()
{

  // Get event size for final consistency check (does not depend on version)
  fEventSize = (UInt_t)((((UInt_t*)fBuffer)[EVENT_EVENTSIZE_LIN] & EVENT_EVENTSIZE_BIT ) >> EVENT_EVENTSIZE_POS);

  // Decode event structure according to data format version
  int rc = 0;
  if (fVersion == 3) {
    rc = UnpackEvent_v03();
  } else {
    printf("Trigger::UnpackEvent - ERROR - Data format version %u is unknown\n",fVersion);
    return 1;
  }
  if (rc) {
    printf("Trigger::UnpackEvent - ERROR while unpacking event\n");
    return 1;
  }
  return 0;
}

int Trigger::UnpackEvent_v03()
{

  // Get full trigger data info (64bits)
  ULong64_t data = (ULong64_t)((UInt_t*)fBuffer)[TRIGEVENT_V03_TRIGDATALO_LIN]+( (ULong64_t)((UInt_t*)fBuffer)[TRIGEVENT_V03_TRIGDATAHI_LIN] << 32 );

  // Extract information from data word
  fTriggerMask = (UChar_t)( (data & TRIGEVENT_V03_TRIGGERMASK_BIT) >> TRIGEVENT_V03_TRIGGERMASK_POS );
  fTriggerCounter = (UInt_t)( (data & TRIGEVENT_V03_TRIGGERCOUNT_BIT) >> TRIGEVENT_V03_TRIGGERCOUNT_POS );
  fClockCounter = (ULong64_t)( (data & TRIGEVENT_V03_CLOCKCOUNT_BIT) >> TRIGEVENT_V03_CLOCKCOUNT_POS );
  fTriggerFifo = (UChar_t)( (data & TRIGEVENT_V03_TRIGGERFIFO_BIT) >> TRIGEVENT_V03_TRIGGERFIFO_POS );
  fTriggerAuto = (UChar_t)( (data & TRIGEVENT_V03_TRIGGERAUTO_BIT) >> TRIGEVENT_V03_TRIGGERAUTO_POS );

  // All ok: return 0
  return 0;

}
