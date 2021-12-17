#include<TADCTrigger.hh>
#include<TADCBoard.hh>
#include<TRawEvent.hh>
#include<TBranch.h>
#include<TTree.h>
#include<TFile.h>

#include<stdint.h>

typedef union{
  struct{
    uint32_t btf      :1; //0: BTF trigger
    uint32_t cosmic   :1; //1: cosmics trigger
    uint32_t unused1  :1; //2: unused
    uint32_t local    :1; //3: local trigger (from dual-timer module)
    uint32_t unused2  :1; //4: unused
    uint32_t reserved :1; //5: reserved
    uint32_t random   :1; //6: random trigger
    uint32_t delayed  :1; //7: delayed trigger
  };
  uint32_t all;
}trigger_mask_t;
typedef struct{
  int32_t         run_number        ;
  uint32_t        event_number      ;
  uint64_t        event_run_time    ;
  trigger_mask_t  event_trig_mask   ;
  uint32_t        event_status      ;
  uint32_t        missing_adc_boards;
  uint8_t         n_adc_boards      ;
}event_info_t;
typedef struct{
  uint8_t  board_id;
  uint32_t board_sn;
  uint16_t lvds_pattern;
  uint8_t  status;
  uint32_t board_status;
  uint8_t  group_mask;
  uint32_t event_counter;
  uint32_t event_timetag;
  uint8_t  supp_algrtm;
  uint32_t active_channel_mask;
  uint32_t accepted_channel_mask;
}board_info;

class PadmeRaw{
  public:
    PadmeRaw(char*fn){
      fRootFile = new TFile(fn);
      TTree* tRawEv = (TTree*)fRootFile->Get("RawEvents");
      fNumberOfEvents = tRawEv->GetEntries();
      fRootEvent = tRawEv->GetBranch("RawEvent");
      fEvent = new TRawEvent();
      fRootEvent->SetAddress(&fEvent);
      GotoEvent(0);
    }
    ~PadmeRaw(){
      delete fEvent;
      delete fRootEvent;
      delete fRootFile;
    }
    int GetNEvents(){return fNumberOfEvents;}
    void GotoEvent(int evtptr){
      fRootEvent->GetEntry(evtptr);
    }
    TRawEvent* fEvent;
  private:
    TFile* fRootFile;
    TBranch* fRootEvent;
    int fNumberOfEvents;
};


extern "C" {
  class PadmeRaw;
  PadmeRaw* PadmeRaw_new(char* fn){PadmeRaw* ptr = new PadmeRaw(fn); return ptr;}
  void       PadmeRaw_del             (PadmeRaw* ptr){delete ptr;}
  int        PadmeRaw_GetNEvents      (PadmeRaw* ptr){return ptr->GetNEvents();}
  void       PadmeRaw_GotoEvent       (PadmeRaw* ptr, uint32_t evtptr){ptr->GotoEvent(evtptr);}
  int        PadmeRaw_GetNADCBoards   (PadmeRaw* ptr){return ptr->fEvent->GetNADCBoards();}
  TADCBoard* PadmeRaw_GetADCBoard     (PadmeRaw* ptr, uint8_t board){return ptr->fEvent->ADCBoard(board);}
  int        ADCBoard_GetNADCChannels (TADCBoard* ptr){return ptr->GetNADCChannels();}
  int        ADCBoard_GetNADCTriggers (TADCBoard* ptr){return ptr->GetNADCTriggers();}
  uint16_t*  ADCBoard_GetADCChannel   (TADCBoard* ptr, uint8_t chan){return (uint16_t*)ptr->ADCChannel(chan)->GetSamplesArray();}
  uint16_t*  ADCBoard_GetADCTrigger   (TADCBoard* ptr, uint8_t trig){return (uint16_t*)ptr->ADCTrigger(trig)->GetSamplesArray();}

  event_info_t PadmeRaw_GetEventInfo(PadmeRaw* ptr){
    event_info_t evt_info;
    evt_info.run_number            = ptr->fEvent->GetRunNumber        ();
    evt_info.event_number          = ptr->fEvent->GetEventNumber      ();
    evt_info.event_run_time        = ptr->fEvent->GetEventRunTime     ();
    evt_info.event_trig_mask.all   = ptr->fEvent->GetEventTrigMask    ();
    evt_info.event_status          = ptr->fEvent->GetEventStatus      ();
    evt_info.missing_adc_boards    = ptr->fEvent->GetMissingADCBoards ();
    evt_info.n_adc_boards          = ptr->fEvent->GetNADCBoards       ();
    return evt_info;
  }

  board_info ADCBoard_GetBoardInfo(TADCBoard* ptr){
    board_info brd_info;
    brd_info.board_id              = ptr->GetBoardId            ();
    brd_info.board_sn              = ptr->GetBoardSN            ();
    brd_info.lvds_pattern          = ptr->GetLVDSPattern        ();
    brd_info.status                = ptr->GetStatus             ();
    brd_info.board_status          = ptr->GetBoardStatus        ();
    brd_info.group_mask            = ptr->GetGroupMask          ();
    brd_info.event_counter         = ptr->GetEventCounter       ();
    brd_info.event_timetag         = ptr->GetEventTimeTag       ();
    brd_info.supp_algrtm           = ptr->Get0SuppAlgrtm        ();
    brd_info.active_channel_mask   = ptr->GetActiveChannelMask  ();
    brd_info.accepted_channel_mask = ptr->GetAcceptedChannelMask();
    return brd_info;
  }
}
