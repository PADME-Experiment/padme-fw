#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "CAENDigitizer.h"

#include "DB.h"
#include "Config.h"

#include "PEvent.h"

int create_pevent(void *evtPtr, CAEN_DGTZ_X742_EVENT_t *event, void *pEvt)
{

  int pEvtSize = 0;
  int pEvtStatus;
  int pEvtGroupSize;

  uint32_t pEvtChMaskActive = 0;
  uint32_t pEvtChMaskAccepted = 0;

  int nSm;
  int freq,tr;
  float sample; // Used to store corrected sample value from CAEN
  int16_t myshort; // Used to store rounded samples (can be negative)
  unsigned int n_samples_on; // Counter for trigger length evaluation (autopass)
  uint32_t line;

  int Ch,iGr,iCh,iSm;
  uint32_t bCh; // bit mask for channel

  // Pointer to move over the pEvt structure one byte at a time
  void *cursor = pEvt;

  // Extract 0-suppression configuration
  int pEvt0SupMode = Config->zero_suppression / 100; // 0=rejction, 1=flagging
  int pEvt0SupAlgr = Config->zero_suppression % 100; // 0=off, 1-15=algorithm code

  // Set autopass bit to 0. Will be set to 1 if trigger signal is long.
  int pEvtAutoPass = 0;

  // Define autopass trig ON duration taking into account sampling frequency
  unsigned int autopass_trig_duration;
  if (Config->drs4_sampfreq == 0) {
    autopass_trig_duration = 5*Config->auto_duration; // 1ns = 5 samples
  } else if (Config->drs4_sampfreq == 1) {
    autopass_trig_duration = 2.5*Config->auto_duration; // 1ns = 2.5 samples
  } else if (Config->drs4_sampfreq == 2) {
    autopass_trig_duration = Config->auto_duration; // 1ns = 1 sample
  } else {
    printf("PEvent ERROR - drs4_sampfreq set to %d\n",Config->drs4_sampfreq);
    return 0;
  }
  //printf("Autopass trigger ON duration set to %u samples\n",autopass_trig_duration);

  // Event header will be created at the end

  // Jump at beginning of group trigger section
  cursor += PEVT_HEADER_LEN*4;
  
  // Update total size of event
  pEvtSize += PEVT_HEADER_LEN;
  //  printf("1 - pEvtSize %d\n",pEvtSize);

  // Write group header and group trigger info
  for (iGr=0;iGr<MAX_X742_GROUP_SIZE;iGr++) {

    // We trust CAEN that event->GrPresent[iGr] is consistent with group mask from header
    if (event->GrPresent[iGr]) {

      // Group header: start index cell|freq|tr|size
      pEvtGroupSize = PEVT_GRPHEAD_LEN+PEVT_GRPTTT_LEN;
      freq = Config->drs4_sampfreq; // 0=5GHz, 1=2.5GHz, 2=1GHz, 3=0.7GHz(new)
      tr = 0; // trigger data - 0:no, 1:yes
      if ( (nSm = event->DataGroup[iGr].ChSize[8]) ) {
	tr = 1;
	pEvtGroupSize += (nSm/2 + nSm%2);
      }

      // pEvent group header includes:
      // Start Index Cell (bit 22-31)
      // ADC frequency (bit 20-21)
      // Trigger data flag (bit 19)
      // Size of group data in 4 bytes words (bit 0-11). Includes group header, trigger samples, and trigger time tag.
      line = ((event->DataGroup[iGr].StartIndexCell & 0x3FF)<<22)
	+ ((freq & 0x3)<<20) + ((tr & 0x1)<<19) + (pEvtGroupSize & 0xFFF);
      memcpy(cursor,&line,4);
      cursor += 4;

      // Copy trigger samples (if present)
      if (nSm) {
	n_samples_on = 0;
	for (iSm=0;iSm<nSm;iSm++) {
	  myshort = roundf(event->DataGroup[iGr].DataChannel[8][iSm]);
	  if (myshort < Config->auto_threshold) n_samples_on++;
	  memcpy(cursor,&myshort,2);
	  cursor += 2;
	  //printf("Tr %d Sm %d %f %d\n",iGr,iSm,event->DataGroup[iGr].DataChannel[8][iSm],myshort);
	  //printf("Tr %d Sm %d 0x%04x\n",iGr,iSm,myshort);
	}
	//printf("Trigger ON for %u samples\n",n_samples_on);
	if (n_samples_on > autopass_trig_duration) {
	  //printf("Autopass enabled: %u > %u\n",n_samples_on,autopass_trig_duration);
	  pEvtAutoPass = 1;
	}
      }

      // Copy trigger time tag
      memcpy(cursor,&event->DataGroup[iGr].TriggerTimeTag,4);
      cursor += 4;

      // Update total size of event
      pEvtSize += pEvtGroupSize;
      //      printf("2 - pEvtSize %d\n",pEvtSize);

    }

  }

  // Loop over 32 channels
  for (Ch=0;Ch<32;Ch++) {

    iGr = Ch/8;
    iCh = Ch%8;
    bCh = (1 << Ch);

    // Channel is active if: group is ON, user enabled it, and samples are present
    if (event->GrPresent[iGr] && (Config->channel_enable_mask & bCh) && (nSm=event->DataGroup[iGr].ChSize[iCh])) {

      // Tag channel as active
      pEvtChMaskActive |= bCh;
      pEvtChMaskAccepted |= bCh;

      // Copy the samples to output structure
      for (iSm=0;iSm<nSm;iSm++) {
	//myshort = roundf(event->DataGroup[iGr].DataChannel[iCh][iSm]);
	sample = event->DataGroup[iGr].DataChannel[iCh][iSm];
	//printf("g %1d c %2d s %4d v %f\n",iGr,iCh,iSm,sample); // DEBUG
	if (sample<-32767. || sample>32767.) { // Take into account CAEN library bug for underflow sample values
	  myshort = 0;
	} else {
	  myshort = roundf(sample);
	}
	//if (myshort<=0 || myshort>4095) printf("\tsample %d\n",myshort); // DEBUG
	memcpy(cursor,&myshort,2);
	cursor += 2;
      }

      // If number of samples is odd, pad last sample to full word (probably never used)
      if (nSm%2) cursor += 2;

      pEvtSize += (nSm/2 + nSm%2);

    }

  }
  //  printf("Final masks 0x%08X 0x%08X\n",pEvtChMaskActive,pEvtChMaskAccepted);

  // Create the event header

  // Line 0 of pEvent header: event tag (bit 28-31) + event size in 4bytes words (bit 0-27)
  //  printf("Final - pEvtSize %d\n",pEvtSize);
  line = (PEVT_EVENT_TAG << 28) + (pEvtSize & 0x0FFFFFFF);
  memcpy(pEvt,&line,4);

  // Prepare events status mask for this event
  pEvtStatus = 0; // Zero event status

  if (pEvtChMaskAccepted) { // Check if at least one channel was accepted
    pEvtStatus += (0x1 << PEVT_STATUS_HASDATA_BIT); // Event has data
  } else {
    pEvtStatus += (0x0 << PEVT_STATUS_HASDATA_BIT); // Event has no data
  }

  if (Config->drs4corr_enable) { // Check if DRS4 corrections were applied
    pEvtStatus += (0x1 << PEVT_STATUS_DRS4CORR_BIT); // Corrections applied
  } else {
    pEvtStatus += (0x0 << PEVT_STATUS_DRS4CORR_BIT); // Corrections not applied
  }

  if (pEvt0SupMode == 0) { // Save 0-suppression mode
    pEvtStatus += (0x0 << PEVT_STATUS_ZEROSUPP_BIT); // 0-suppression in rejection mode
  } else {
    pEvtStatus += (0x1 << PEVT_STATUS_ZEROSUPP_BIT); // 0-suppression in flagging mode
  }

  if (pEvtAutoPass == 0) {
    pEvtStatus += (0x0 << PEVT_STATUS_AUTOPASS_BIT); // No autopass
  } else {
    pEvtStatus += (0x1 << PEVT_STATUS_AUTOPASS_BIT); // Enable autopass for this even
  }

  // 1) Get line 1 of V1742 event header
  memcpy(&line,evtPtr+4,4);
  //printf("First line of header 0x%08x\n",line);
  // 2) Save Board Fail flag (bit 26) to event status. *** NOT USED ANYMORE: replaced by "missing event" bit ***
  //if (line & 0x04000000) pEvtStatus += (0x1 << PEVT_STATUS_BRDFAIL_BIT);
  // 3) Extract LVDS pattern (bit 8-23) and group mask (bit 0-3).
  // 4) Add our board id (bit 24-31) and 0-suppression algorithm code (bit 4-7).
  line = (line & 0x00FFFF0F) + ((Config->board_id & 0xFF) << 24) + ((pEvt0SupAlgr & 0xF) << 4);
  // 5) Copy result to line 1 of pEvent header
  memcpy(pEvt+4,&line,4);

  // 1) Get line 2 of V1742 event header
  memcpy(&line,evtPtr+8,4);
  // 2) Extract event counter (bit 0-21)
  // 3) Add event status pattern (bit 22-31)
  line = (line & 0x003FFFFF) + ((pEvtStatus & 0x03FF) << 22);
  // 4) Copy result to line 2 of pEvent header
  memcpy(pEvt+8,&line,4);

  // Copy line 3 of V1742 event header to line 3 of pEvent header (event time tag)
  memcpy(pEvt+12,evtPtr+12,4);

  // Copy channel masks to lines 4 and 5 of pEvent header
  memcpy(pEvt+PEVT_CHMASK_ACTIVE_LINE*4,  &pEvtChMaskActive,  4);
  memcpy(pEvt+PEVT_CHMASK_ACCEPTED_LINE*4,&pEvtChMaskAccepted,4);

  // Dump event header
  //for(i=0;i<PEVT_HEADER_LEN;i++){
  //  printf("%d\t%08x\n",i,((int*)pEvt)[i]);
  //}

  return pEvtSize*4; // Return total size of event in bytes

}

//unsigned int create_file_head(unsigned int fIndex, int runNr, uint32_t boardSN, time_t timeTag, void *fHead)
unsigned int create_file_head(unsigned int fIndex, int runNr, int boardId, uint32_t boardSN, time_t timeTag, void *fHead)
{

  uint32_t line;

  // First line: file head tag (4) + version (12) + file index (16)
  line = (PEVT_FHEAD_TAG << 28) + (PEVT_CURRENT_VERSION << 16) + (fIndex & 0xFFFF);
  memcpy(fHead,&line,4);

  // Second line: run number (32, signed int)
  memcpy(fHead+4,&runNr,4);

  // Third line: board id (8) + board serial number (24)
  line = ( (boardId & 0xff) << 24 ) + (boardSN & 0xffffff);
  memcpy(fHead+8,&line,4);

  // Fourth line: start of file time tag (32)
  line = (timeTag & 0xffffffff); // Avoid problems with 8Bytes time_t structure
  memcpy(fHead+12,&line,4);

  return PEVT_FHEAD_LEN*4;     // Return total size of file header in bytes

}

unsigned int create_file_tail(unsigned int nEvts, unsigned long int fileSize, time_t timeTag, void *fTail)
{

  uint32_t line;

  // First line: file tail tag (4) + number of events (28)
  line = (PEVT_FTAIL_TAG << 28) + (nEvts & 0x0FFFFFFF);
  memcpy(fTail,&line,4);

  // Second + third line: total file size (64)
  unsigned long int totalSize = fileSize + PEVT_FTAIL_LEN*4; // Add size of tail to total file size
  memcpy(fTail+4,&totalSize,8);

  // Fourth line: end of file time tag (32)
  line = (timeTag & 0xffffffff); // Avoid problems with 8Bytes time_t structure
  memcpy(fTail+12,&line,4);

  return PEVT_FTAIL_LEN*4;       // Return total size of file tail in bytes

}
