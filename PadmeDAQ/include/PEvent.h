#ifndef _PEVENT_H_
#define _PEVENT_H_

// Version 0: native CAEN V1742 raw event structure: data are unscrabled and uncorrected
//     Use NewDecode software to read, unscrable, correct, and write to root ntuple
// Version 1: used for all data taken after June 21, 2015. Use PadmeDigi to read them.
// Version 2: expanded board_id from 5 to 8 bits, in use after November 6, 2015.
// Version 3: added board s/n to file header, used 2 bits in event status for 0-suppression,
//            added id of 0-suppression algorithm to event header
#define PEVT_CURRENT_VERSION 3

#define PEVT_FHEAD_TAG 0x9
#define PEVT_EVENT_TAG 0xE
#define PEVT_FTAIL_TAG 0x5

#define PEVT_FHEAD_LEN 4
#define PEVT_FTAIL_LEN 4

#define PEVT_HEADER_LEN  6
#define PEVT_GRPHEAD_LEN 1
#define PEVT_GRPTTT_LEN  1

#define PEVT_CHMASK_ACTIVE_LINE   4
#define PEVT_CHMASK_ACCEPTED_LINE 5

#define PEVT_STATUS_HASDATA_BIT  0
#define PEVT_STATUS_DRS4CORR_BIT 1
#define PEVT_STATUS_ZEROSUPP_BIT 2
//#define PEVT_STATUS_BRDFAIL_BIT  3
#define PEVT_STATUS_MISSING_BIT  3
#define PEVT_STATUS_AUTOPASS_BIT 4

int create_pevent(void*,CAEN_DGTZ_X742_EVENT_t*,void*); // evtPtr, event, pEvt
//unsigned int create_file_head(unsigned int,int,uint32_t,time_t,void*); // file_index,run_number,board_sn,time_tag,fHead
unsigned int create_file_head(unsigned int,int,int,uint32_t,time_t,void*); // file_index,run_number,board_id,board_sn,time_tag,fHead
unsigned int create_file_tail(unsigned int,unsigned long int,time_t,void*); // n_events,file_size,time_tag,fTail

#endif
