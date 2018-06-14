#include <stdlib.h>
#include <stdexcept>

#include "ADCEventTags.hh"

#include "ADCBoard.hh"

ADCBoard::ADCBoard(int board)
{

  UInt_t max_event_len_v01 = 4*(ADCEVENT_V01_EVENTHEAD_LEN+ADCEVENT_NTRIGGERS*(ADCEVENT_V01_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_V01_GRTAIL_LEN)+ADCEVENT_NCHANNELS*ADCEVENT_NSAMPLES/2);
  UInt_t max_event_len_v02 = 4*(ADCEVENT_V02_EVENTHEAD_LEN+ADCEVENT_NTRIGGERS*(ADCEVENT_V02_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_V02_GRTAIL_LEN)+ADCEVENT_NCHANNELS*ADCEVENT_NSAMPLES/2);
  UInt_t max_event_len_v03 = 4*(ADCEVENT_V03_EVENTHEAD_LEN+ADCEVENT_NTRIGGERS*(ADCEVENT_V03_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_V03_GRTAIL_LEN)+ADCEVENT_NCHANNELS*ADCEVENT_NSAMPLES/2);

  UInt_t max_event_len = std::max(max_event_len_v01,std::max(max_event_len_v02,max_event_len_v03));

  //printf("max_event_len v01 %d v02 %d v03 %d max %d\n",max_event_len_v01,max_event_len_v02,max_event_len_v03,max_event_len);

  fBuffer = malloc(max_event_len);
  fBoardId = board;
  fADCEvent = new ADCEvent();
  Reset();
  //fDB = DBService::GetInstance();
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

ADCEvent* ADCBoard::NextEvent()
{

  int ret;

  while(1){

    // Starting a new file
    //if ( (! fFileHandle) || (! fFileHandle.is_open()) ) {

      // Open next file in list
      //printf ("Current file %d List size %lu\n",fCurrentFile,fFiles.size());
      //if ( fCurrentFile >= fFiles.size() ) {
      //	// No more files: we are done
      //	fADCEvent = 0;
      //	return fADCEvent;
      //}
      //printf("Opening file %s\n",fFiles[fCurrentFile].GetPath().c_str());
      //fFileHandle.open(fFiles[fCurrentFile].GetPath().c_str(), std::ios::in | std::ios::binary);

    if (fNewFile == 1) {

      // Read file head. Returns 0:OK 1:Error
      ret = ReadFileHead();
      if ( ret == 1 ) {
	printf("ERROR while reading header of file %s. Aborting\n",fFiles[fCurrentFile].GetPath().c_str());
	exit(1);
      }

      printf("File %s opened with format version %d\n",
	     fFiles[fCurrentFile].GetPath().c_str(),fFiles[fCurrentFile].GetVersion());

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
	fADCEvent = 0;
	return fADCEvent;
      }
      // More files are available: open the next one
      if (OpenFile() != 0) {
	printf("ADCBoard::NextEvent - ERROR - Unable to open next input file. Aborting\n");
	exit(1);
      }
      fNewFile = 1;
      continue;
    }
    return fADCEvent;

  }

}

int ADCBoard::ReadFileHead()
{

  // Read first line (4 bytes) of file header into buffer
  // NB: format of this line MUST NOT change with version!
  fFileHandle.read((char*)fBuffer,4);

  // Check if file header tag is correct
  UChar_t tag = (UChar_t)( ( ((UInt_t*)fBuffer)[ADCEVENT_TAG_LIN] & ADCEVENT_TAG_BIT ) >> ADCEVENT_TAG_POS );
  if (tag != ADCEVENT_FHEAD_TAG) {
    printf("ERROR - File does not start with the right tag - Expected 0x%1X - Found 0x%1X\n",ADCEVENT_FHEAD_TAG,tag);
    return 1;
  }

  // Get data format version
  fFiles[fCurrentFile].SetVersion( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_VERSION_LIN] & ADCEVENT_VERSION_BIT) >> ADCEVENT_VERSION_POS) );

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
  UChar_t tag = (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_TAG_LIN] & ADCEVENT_TAG_BIT ) >> ADCEVENT_TAG_POS);
  if (tag == ADCEVENT_EVENT_TAG) {

    // This is an event block: get its size and read the remaining part to buffer
    UInt_t size = (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_EVENTSIZE_LIN] & ADCEVENT_EVENTSIZE_BIT ) >> ADCEVENT_EVENTSIZE_POS);
    fFileHandle.read((char*)fBuffer+4,4*(size-1));

    // Decode event structure according to data format version
    int rc = 0;
    if (fFiles[fCurrentFile].GetVersion() == 1) {
      rc = UnpackEvent_v01(size);
    } else if (fFiles[fCurrentFile].GetVersion() == 2) {
      rc = UnpackEvent_v02(size);
    } else if (fFiles[fCurrentFile].GetVersion() == 3) {
      rc = UnpackEvent_v03(size);
    }
    if (rc) {
      printf("ERROR while unpacking event\n");
      return 1;
    }
    
  } else if (tag == ADCEVENT_FTAIL_TAG) {

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

int ADCBoard::UnpackEvent_v01(UInt_t size)
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
  if (cursor != size) {
    printf("ERROR - Inconsistent event structure found for event %d: event size is %d (expected %d)\n",
	   fADCEvent->GetEventCounter(),cursor,size);
    return 1;
  }

  // All ok: return 0
  return 0;

}

int ADCBoard::UnpackEvent_v02(UInt_t size)
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
  if (cursor != size) {
    printf("ERROR - Inconsistent event structure found for event %d: event size is %d (expected %d)\n",
	   fADCEvent->GetEventCounter(),cursor,size);
    return 1;
  }

  // All ok: return 0
  return 0;

}

int ADCBoard::UnpackEvent_v03(UInt_t size)
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

//printf("Board Id %d LVDS %08x 0SuppAlg %d GrpMsk %1x Status %03x Event %d Time %d Active %08x Accept %08x\n",
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
      }
      // Shift cursor to next channel
      cursor += (ADCEVENT_NSAMPLES/2 + ADCEVENT_NSAMPLES%2);
    }
  }

  // Verify event size consistency
  if (cursor != size) {
    printf("ERROR - Inconsistent event structure found for event %d: event size is %d (expected %d)\n",
	   fADCEvent->GetEventCounter(),cursor,size);
    return 1;
  }

  // All ok: return 0
  return 0;

}
