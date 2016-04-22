#include <stdlib.h>
#include <stdexcept>

#include "ADCEventTags.hh"

#include "ADCBoard.hh"

ADCBoard::ADCBoard(int board)
{
  UInt_t max_event_len = 4*(ADCEVENT_EVENTHEAD_LEN+ADCEVENT_NTRIGGERS*(ADCEVENT_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_GRTAIL_LEN)+ADCEVENT_NCHANNELS*ADCEVENT_NSAMPLES/2);
  //printf("max_event_len %d\n",max_event_len);
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
  fFiles.clear();
  if (fFileHandle && fFileHandle.is_open()) fFileHandle.close();
  fCurrentFile = 0;
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

ADCEvent* ADCBoard::NextEvent()
{

  int ret;

  while(1){

    // Starting a new file
    if ( (! fFileHandle) || (! fFileHandle.is_open()) ) {

      // Open next file in list
      if ( fCurrentFile >= fFiles.size() ) {
	// No more files: we are done
	fADCEvent = 0;
	return fADCEvent;
      }
      printf("Opening file %s\n",fFiles[fCurrentFile].GetPath().c_str());
      fFileHandle.open(fFiles[fCurrentFile].GetPath().c_str(), std::ios::in | std::ios::binary);

      // Read file head. Returns 0:OK 1:Error
      ret = ReadFileHead();
      if ( ret == 1 ) {
	printf("ERROR while reading header of file %s. Aborting\n",fFiles[fCurrentFile].GetPath().c_str());
	exit(1);
      }

    }

    // Read next event (also handle file tail record). Returns 0:OK 1:Error 2:EOF
    ret = ReadNextEvent();
    if (ret == 1){
      printf("ERROR while reading event from file %s. Aborting\n",fFiles[fCurrentFile].GetPath().c_str());
      exit(1);
    }
    if (ret == 2){
      fFileHandle.close();
      fCurrentFile++;
      continue;
    }
    return fADCEvent;

  }

}

int ADCBoard::ReadFileHead()
{

  // Read file header into buffer
  fFileHandle.read((char*)fBuffer,ADCEVENT_FHEAD_LEN*4);

  // Check if file header tag is correct
  UChar_t tag = (UChar_t)( ( ((Int_t*)fBuffer)[ADCEVENT_TAG_LIN] & ADCEVENT_TAG_BIT ) >> ADCEVENT_TAG_POS );
  if (tag != ADCEVENT_FHEAD_TAG) {
    printf("ERROR - File does not start with the right tag - Expected 0x%1X - Found 0x%1X\n",ADCEVENT_FHEAD_TAG,tag);
    return 1;
  }

  if (UnpackFileHead()) {
    printf("ERROR while unpacking file head (?)\n");
    return 1;
  }

  return 0;

}

int ADCBoard::ReadNextEvent()
{

  // Read first word of event, check its tag
  fFileHandle.read((char*)fBuffer,4);
  UChar_t tag = (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_TAG_LIN] & ADCEVENT_TAG_BIT ) >> ADCEVENT_TAG_POS);
  if (tag == ADCEVENT_EVENT_TAG) {

    // This is an event block: get its size and read the remaining part to buffer
    UInt_t size = (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_EVENTSIZE_LIN] & ADCEVENT_EVENTSIZE_BIT ) >> ADCEVENT_EVENTSIZE_POS);
    fFileHandle.read((char*)fBuffer+4,4*(size-1));

    if (UnpackEvent(size)) {
      printf("ERROR while unpacking event\n");
      return 1;
    }
    
  } else if (tag == ADCEVENT_FTAIL_TAG) {

    // This is a file tail block: read the remaining part to buffer
    fFileHandle.read((char*)fBuffer+4,4*(ADCEVENT_FTAIL_LEN-1));

    // Unpack tail of file
    if (UnpackFileTail()) {
      printf("ERROR while unpacking file tail (?)\n");
      return 1;
    }

    // Return EOF condition
    return 2;

  } else {

    printf("ERROR - Unknown tag found in file structure: 0x%1x\n",tag);
    return 1;
    
  }

  return 0;

}

int ADCBoard::UnpackFileHead()
{

  // Unpack file header
  fFiles[fCurrentFile].SetVersion( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_VERSION_LIN] & ADCEVENT_VERSION_BIT) >> ADCEVENT_VERSION_POS) );
  fFiles[fCurrentFile].SetIndex( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_FILEINDEX_LIN] & ADCEVENT_FILEINDEX_BIT) >> ADCEVENT_FILEINDEX_POS) );
  fFiles[fCurrentFile].SetRunNumber( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_RUNNUMBER_LIN] & ADCEVENT_RUNNUMBER_BIT) >> ADCEVENT_RUNNUMBER_POS) );
  fFiles[fCurrentFile].SetStartTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_TTAGSOF_LIN] & ADCEVENT_TTAGSOF_BIT) >> ADCEVENT_TTAGSOF_POS) );

  // Verify if file has correct version
  if (fFiles[fCurrentFile].GetVersion() != ADCEVENT_FORMAT_VERSION) {
    printf("ERROR - File incompatible file version: expected %d found %d\n",ADCEVENT_FORMAT_VERSION,fFiles[fCurrentFile].GetVersion());
    return 1;
  }

  return 0;

}

int ADCBoard::UnpackFileTail()
{

  // Unpack file tail
  fFiles[fCurrentFile].SetNEvents( (Int_t)((((UInt_t*)fBuffer)[ADCEVENT_NEVENTS_LIN] & ADCEVENT_NEVENTS_BIT) >> ADCEVENT_NEVENTS_POS) );
  fFiles[fCurrentFile].SetEndTime( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_TTAGEOF_LIN] & ADCEVENT_TTAGEOF_BIT) >> ADCEVENT_TTAGEOF_POS) );
  //ULong_t filesize = (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_FILESIZELO_LIN]+( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_FILESIZEHI_LIN] << 32 );
  //fFiles[fCurrentFile].SetSize(filesize);
  fFiles[fCurrentFile].SetSize( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_FILESIZELO_LIN]+( (ULong_t)((UInt_t*)fBuffer)[ADCEVENT_FILESIZEHI_LIN] << 32 ) );

  return 0;

}

int ADCBoard::UnpackEvent(UInt_t size)
{

  // Decode event header
  fADCEvent->SetBoardId( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_BOARDID_LIN] & ADCEVENT_BOARDID_BIT) >> ADCEVENT_BOARDID_POS) );
  fADCEvent->SetLVDSPattern( (UShort_t)((((UInt_t*)fBuffer)[ADCEVENT_PATTERN_LIN] & ADCEVENT_PATTERN_BIT ) >> ADCEVENT_PATTERN_POS) );
  fADCEvent->SetStatus( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_STATUS_LIN] & ADCEVENT_STATUS_BIT ) >> ADCEVENT_STATUS_POS) );
  fADCEvent->SetGroupMask( (UChar_t)((((UInt_t*)fBuffer)[ADCEVENT_GRMASK_LIN] & ADCEVENT_GRMASK_BIT ) >> ADCEVENT_GRMASK_POS) );
  fADCEvent->SetEventCounter( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_EVENTNUMBER_LIN] & ADCEVENT_EVENTNUMBER_BIT ) >> ADCEVENT_EVENTNUMBER_POS) );
  fADCEvent->SetEventTimeTag( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_TTAGEVENT_LIN] & ADCEVENT_TTAGEVENT_BIT ) >> ADCEVENT_TTAGEVENT_POS) );
  fADCEvent->SetActiveChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_CHMASKACTIVE_LIN] & ADCEVENT_CHMASKACTIVE_BIT ) >> ADCEVENT_CHMASKACTIVE_POS) );
  fADCEvent->SetAcceptedChannelMask( (UInt_t)((((UInt_t*)fBuffer)[ADCEVENT_CHMASKACCEPTED_LIN] & ADCEVENT_CHMASKACCEPTED_BIT ) >> ADCEVENT_CHMASKACCEPTED_POS) );

  // Decode trigger information
  UInt_t cursor = ADCEVENT_EVENTHEAD_LEN;
  for(int ig=0;ig<ADCEVENT_NTRIGGERS;ig++){

    if (fADCEvent->GetGroupMask() & (0x1 << ig)) {

      fADCEvent->SetTriggerStartIndexCell( ig,(UShort_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_STARTIDXCELL_LIN] & ADCEVENT_STARTIDXCELL_BIT) >> ADCEVENT_STARTIDXCELL_POS) );
      fADCEvent->SetTriggerFrequency( ig,(UChar_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_FREQUENCY_LIN] & ADCEVENT_FREQUENCY_BIT ) >> ADCEVENT_FREQUENCY_POS) );
      fADCEvent->SetTriggerHasSignal( ig,(Bool_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_TRHASDATA_LIN] & ADCEVENT_TRHASDATA_BIT ) >> ADCEVENT_TRHASDATA_POS) );

      if (fADCEvent->GetTriggerHasSignal(ig)) {
	for(int is=0;is<ADCEVENT_NSAMPLES;is++){
	  fADCEvent->SetADCTriggerSample( ig,is,(Short_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_GRHEAD_LEN+is/2] >> (16*(is%2))) & 0xFFFF) );
	}
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_GRHEAD_LEN+ADCEVENT_NSAMPLES/2+ADCEVENT_NSAMPLES%2]);
      } else {
	fADCEvent->SetTriggerTimeTag(ig,(UInt_t)((UInt_t*)fBuffer)[cursor+ADCEVENT_GRHEAD_LEN]);
      }

      // Shift cursor to next trigger
      cursor += (UInt_t)((((UInt_t*)fBuffer)[cursor+ADCEVENT_GRSIZE_LIN] & ADCEVENT_GRSIZE_BIT) >> ADCEVENT_GRSIZE_POS);

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
