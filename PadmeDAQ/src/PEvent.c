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
  int pEvtStatus = 0;
  int pEvtGroupSize;

  uint32_t pEvtChMaskActive = 0;
  uint32_t pEvtChMaskAccepted = 0;

  int nSm;
  int freq,tr;
  int16_t myshort; // Used to store rounded samples (can be negative)
  uint32_t line;

  int Ch,iGr,iCh,iSm;
  uint32_t bCh; // bit mask for channel
  float s,sum,sum2,mean,rms,thrLo,thrHi;
  int overThr,nOverThr,nMaxOverThr;
  int acceptCh;

  // Pointer to move over the pEvt structure one byte at a time
  void *cursor = pEvt;
  void *cursor_old; // Used to save old position when applying zero suppression

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
      freq = Config->drs4_sampfreq; // 0=5GHz, 1=2.5GHz, 2=1GHz
      tr = 0; // trigger data - 0:no, 1:yes
      if ( (nSm = event->DataGroup[iGr].ChSize[8]) ) {
	tr = 1;
	pEvtGroupSize += (nSm/2 + nSm%2);
      }

      line = ((event->DataGroup[iGr].StartIndexCell & 0xFFFF)<<16)
	+ ((freq & 0x3)<<14) + ((tr & 0x3)<<12) + (pEvtGroupSize & 0xFFF);
      memcpy(cursor,&line,4);
      cursor += 4;

      // Copy trigger samples (if present)
      if (nSm) {
	for (iSm=0;iSm<nSm;iSm++) {
	  myshort = roundf(event->DataGroup[iGr].DataChannel[8][iSm]);
	  memcpy(cursor,&myshort,2);
	  cursor += 2;
	  //printf("Tr %d Sm %d %f %d\n",iGr,iSm,event->DataGroup[iGr].DataChannel[8][iSm],myshort);
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

      if ( Config->zero_suppression == 1 ) {

	// Copy the samples while applying zero suppression algorithm
	//printf("Applying zero suppression\n"); // Debug message

	cursor_old = cursor; // Save current position: will restart from here if channel is rejected

	sum = 0.;
	sum2 = 0.;
	mean = 0.;
	rms = 0.;
	thrHi = 8192.;
	thrLo = -8192;
	overThr = 0;
	nOverThr = 0;
	nMaxOverThr = 0;
	for (iSm=0;iSm<nSm;iSm++) {

	  // Get value of current sample
	  s = event->DataGroup[iGr].DataChannel[iCh][iSm];

	  // Use the first 80 samples to compute pedestal and sigma pedestal
	  if (iSm<Config->zs1_head) {

	    sum += s;
	    sum2 += s*s;
	    if (iSm==Config->zs1_head-1) {
	      mean = sum/Config->zs1_head;
	      rms = sqrt((sum2-sum*mean)/(Config->zs1_head-1));
	      thrHi = mean+Config->zs1_nsigma*rms;
	      thrLo = mean-Config->zs1_nsigma*rms;
	      //	      printf("Channel %d mean %f rms %f thrHi %f thrLo %f\n",Ch,mean,rms,thrHi,thrLo);
	    }

	  } else if (iSm<1024-Config->zs1_tail) { // Do not consider final set of samples

	    // Get longest set of consecutive samples above threshold
	    if (overThr) {
	      if (s<thrLo) {
		nOverThr++;
		if (nOverThr>nMaxOverThr) nMaxOverThr = nOverThr;
	      } else {
		overThr = 0;
		nOverThr = 0;
	      }
	    } else {
	      if (s<thrLo) {
		overThr = 1;
		nOverThr = 1;
		if (nOverThr>nMaxOverThr) nMaxOverThr = nOverThr;
	      }
	    }

	  }

	  // Copy sample to output structure
	  myshort = roundf(s);
	  memcpy(cursor,&myshort,2);
	  cursor += 2;
	  //printf("Ch %d Sm %d %f %d\n",Ch,iSm,event->DataGroup[iGr].DataChannel[iCh][iSm],myshort);

	}

	// If number of samples is odd, pad last sample to full word (probably never used)
	if (nSm%2) cursor += 2;

	// Decide if channel is accepted. 1:channel accepted, 0:channel rejected
	acceptCh = 0;
	// Channel is accepted if rms is bad or if at least zs1_nabovethr channels are above threshold
	if ( (rms>Config->zs1_badrmsthr) ||  (nMaxOverThr>=Config->zs1_nabovethr) ) acceptCh = 1;
	//	printf("Channel %d max consecutive samples over threshold: %d\n",Ch,nMaxOverThr);

	if (acceptCh) {
	  // Channel accepted: tag it in the accepted_channels mask and increase event size
	  pEvtChMaskAccepted |= bCh;
	  pEvtSize += (nSm/2 + nSm%2);
	  //	  printf("3 - pEvtSize %d\n",pEvtSize);
	  //	  printf("Channel %d accepted\n",Ch);
	} else {
	  // Channel rejected: overwrite it
	  cursor = cursor_old;
	  //	  printf("Channel %d rejected\n",Ch);
	}

      } else {

	// No zero suppression: just copy the samples and tag channel as accepted
	for (iSm=0;iSm<nSm;iSm++) {
	  myshort = roundf(event->DataGroup[iGr].DataChannel[iCh][iSm]);
	  memcpy(cursor,&myshort,2);
	  cursor += 2;
	}

	// If number of samples is odd, pad last sample to full word (probably never used)
	if (nSm%2) cursor += 2;

	pEvtChMaskAccepted |= bCh;
	pEvtSize += (nSm/2 + nSm%2);
	//	printf("4 - pEvtSize %d\n",pEvtSize);

      }

    }
  }
  //  printf("Final masks 0x%08X 0x%08X\n",pEvtChMaskActive,pEvtChMaskAccepted);

  // Create the event header

  // Line 0: event tag + event size (in 4bytes words)
  //  printf("Final - pEvtSize %d\n",pEvtSize);
  line = (PEVT_EVENT_TAG << 28) + (pEvtSize & 0x0FFFFFFF);
  memcpy(pEvt,&line,4);

  // Prepare events status mask for this event
  if (pEvtChMaskAccepted) { // Check if at least one channel was accepted
    pEvtStatus |=  (0x1 << PEVT_STATUS_HASDATA_BIT);
  } else {
    pEvtStatus &= !(0x1 << PEVT_STATUS_HASDATA_BIT);
  }
  if (Config->drs4corr_enable == 0) { // Check if DRS4 corrections were applied
    pEvtStatus &= !(0x1 << PEVT_STATUS_DRS4COR_BIT);
  } else {
    pEvtStatus |=  (0x1 << PEVT_STATUS_DRS4COR_BIT);
  }
  if (Config->zero_suppression == 0) { // Check if zero suppression algorithm was applied
    pEvtStatus &= !(0x1 << PEVT_STATUS_ZEROSUP_BIT);
  } else {
    pEvtStatus |=  (0x1 << PEVT_STATUS_ZEROSUP_BIT);
  }

  // Copy line 1 of V1742 event header to line 1 of pEvent header (board id,LVDS pattern)
  // adding event status and replacing original board id (5b) with our board id (8b)
  memcpy(&line,evtPtr+4,4);
  line = (line & 0x00FFFF0F) + ((Config->board_id & 0xFF) << 24) + ((pEvtStatus & 0xF) << 4);
  memcpy(pEvt+4,&line,4);

  // Copy line 2 of V1742 event header to line 2 of pEvent header (event counter)
  memcpy(pEvt+8,evtPtr+8,4);

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

unsigned int create_file_head(unsigned int fIndex, int runNr, time_t timeTag, void *fHead)
{
  uint32_t line;
  line = (PEVT_FHEAD_TAG << 28) + (PEVT_CURRENT_VERSION << 16) + (fIndex & 0xFFFF);
  memcpy(fHead,&line,4);      // First line: file head tag (4) + version (12) + file index (16)
  memcpy(fHead+4,&runNr,4);   // Second line: run number (32)
  memcpy(fHead+8,&timeTag,4); // Third line: start of file time tag (32)
  return PEVT_FHEAD_LEN*4;    // Return total size of file header in bytes
}

unsigned int create_file_tail(unsigned int nEvts, unsigned long int fSize, time_t timeTag, void *fTail)
{
  uint32_t line;
  unsigned long int fTotalSize = fSize + PEVT_FTAIL_LEN*4; // Add size of tail to total file size
  line = (PEVT_FTAIL_TAG << 28) + (nEvts & 0x0FFFFFFF);
  memcpy(fTail,&line,4);         // First line: file tail tag (4) + number of events (28)
  memcpy(fTail+4,&fTotalSize,8); // Second + third line: total file size (64)
  memcpy(fTail+12,&timeTag,4);   // Fourth line: end of file time tag (32)
  return PEVT_FTAIL_LEN*4;       // Return total size of file header in bytes
}
