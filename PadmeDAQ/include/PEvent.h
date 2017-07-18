#ifndef _PEVENT_H_
#define _PEVENT_H_

// Version 0: native CAEN V1742 raw event structure: data are unscrabled and uncorrected
//     Use NewDecode software to read, unscrable, correct, and write to root ntuple
// Version 1: used for all data taken after June 21, 2015. Use PadmeDigi to read them.
// Version 2: expanded board_id from 5 to 8 bits, in use after November 6, 2015.
#define PEVT_CURRENT_VERSION 2

#define PEVT_EVENT_TAG 0xE
#define PEVT_FHEAD_TAG 0x9
#define PEVT_FTAIL_TAG 0x5

#define PEVT_FHEAD_LEN 3
#define PEVT_FTAIL_LEN 4

#define PEVT_HEADER_LEN  6
#define PEVT_GRPHEAD_LEN 1
#define PEVT_GRPTTT_LEN  1

#define PEVT_CHMASK_ACTIVE_LINE   4
#define PEVT_CHMASK_ACCEPTED_LINE 5

#define PEVT_STATUS_HASDATA_BIT 0
#define PEVT_STATUS_DRS4COR_BIT 1
#define PEVT_STATUS_ZEROSUP_BIT 2

int create_pevent(void*,CAEN_DGTZ_X742_EVENT_t*,void*); // evtPtr, event, pEvt
unsigned int create_file_head(unsigned int,int,time_t,void*); // file_index,run_number,time_tag,fHead
unsigned int create_file_tail(unsigned int,unsigned long int,time_t,void*); // n_events,file_size,time_tag,fTail

#endif
