#include<TADCTrigger.hh>
#include<TADCChannel.hh>
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
  uint64_t        event_run_time    ;
  int32_t         run_number        ;
  uint32_t        event_number      ;
  trigger_mask_t  event_trig_mask   ;
  uint32_t        event_status      ;
  uint32_t        missing_adc_boards;
  uint8_t         n_adc_boards      ;
}event_info_t;
typedef struct{
  uint32_t event_counter;
  uint32_t event_timetag;
  uint32_t active_channel_mask;
  uint32_t accepted_channel_mask;
  uint32_t board_status;
  uint32_t board_sn;
  uint16_t lvds_pattern;
  uint8_t  board_id;
  uint8_t  status;
  uint8_t  group_mask;
  uint8_t  supp_algrtm;
  uint8_t  n_adc_triggers;
  uint8_t  n_adc_channels;
}board_info_t;
typedef struct{
  uint16_t  n_samples;
  uint8_t   channel_number;
}channel_info_t;
typedef struct{
  uint32_t   trigger_time_tag;
  uint16_t   start_index_cell;
  uint16_t   n_samples;
  uint8_t    group_number;
  uint8_t    frequency;
  uint8_t    trigger_signal;
}trigger_info_t;

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
  PadmeRaw*    PadmeRaw_new(char* fn){PadmeRaw* ptr = new PadmeRaw(fn); return ptr;}
  void         PadmeRaw_del             (PadmeRaw* ptr){delete ptr;}
  int          PadmeRaw_GetNEvents      (PadmeRaw* ptr){return ptr->GetNEvents();}
  void         PadmeRaw_GotoEvent       (PadmeRaw* ptr, uint32_t evtptr){ptr->GotoEvent(evtptr);}
  int          PadmeRaw_GetNADCBoards   (PadmeRaw* ptr){return ptr->fEvent->GetNADCBoards();}
  TADCBoard*   PadmeRaw_GetADCBoard     (PadmeRaw* ptr, uint8_t board){return ptr->fEvent->ADCBoard(board);}
  TADCChannel* ADCBoard_GetADCChannel   (TADCBoard* ptr, uint8_t chan){return ptr->ADCChannel(chan);}
  TADCTrigger* ADCBoard_GetADCTrigger   (TADCBoard* ptr, uint8_t trig){return ptr->ADCTrigger(trig);}

  void ADCChannel_GetArray(TADCChannel* ptr, float arr[]){
    auto samp = ptr->GetSamplesArray();
    for(int i=0;i<ptr->GetNSamples();i++){
      arr[i]=samp[i];
    }
  }
  void ADCTrigger_GetArray(TADCTrigger* ptr, float arr[]){
    auto samp = ptr->GetSamplesArray();
    for(int i=0;i<ptr->GetNSamples();i++){
      arr[i]=samp[i];
    }
  }

  void ADCChannel_GetInfo(TADCChannel* ptr, channel_info_t* chan_info){
    chan_info->channel_number = ptr->GetChannelNumber();
    chan_info->n_samples      = ptr->GetNSamples();
  }

  void ADCTrigger_GetInfo(TADCTrigger* ptr, trigger_info_t* trig_info){
    trig_info->group_number      = ptr->GetGroupNumber();
    trig_info->start_index_cell  = ptr->GetStartIndexCell();
    trig_info->frequency         = ptr->GetFrequency();
    trig_info->trigger_signal    = ptr->GetTriggerSignal();
    trig_info->trigger_time_tag  = ptr->GetTriggerTimeTag();
    trig_info->n_samples         = ptr->GetNSamples();
  }

  void PadmeRaw_GetEventInfo(PadmeRaw* ptr, event_info_t* evt_info){
    evt_info->run_number            = ptr->fEvent->GetRunNumber        ();
    evt_info->event_number          = ptr->fEvent->GetEventNumber      ();
    evt_info->event_run_time        = ptr->fEvent->GetEventRunTime     ();
    evt_info->event_trig_mask.all   = ptr->fEvent->GetEventTrigMask    ();
    evt_info->event_status          = ptr->fEvent->GetEventStatus      ();
    evt_info->missing_adc_boards    = ptr->fEvent->GetMissingADCBoards ();
    evt_info->n_adc_boards          = ptr->fEvent->GetNADCBoards       ();
  }

  void ADCBoard_GetBoardInfo(TADCBoard* ptr, board_info_t* brd_info){
    brd_info->board_id              = ptr->GetBoardId            ();
    brd_info->board_sn              = ptr->GetBoardSN            ();
    brd_info->lvds_pattern          = ptr->GetLVDSPattern        ();
    brd_info->status                = ptr->GetStatus             ();
    brd_info->board_status          = ptr->GetBoardStatus        ();
    brd_info->group_mask            = ptr->GetGroupMask          ();
    brd_info->event_counter         = ptr->GetEventCounter       ();
    brd_info->event_timetag         = ptr->GetEventTimeTag       ();
    brd_info->supp_algrtm           = ptr->Get0SuppAlgrtm        ();
    brd_info->active_channel_mask   = ptr->GetActiveChannelMask  ();
    brd_info->accepted_channel_mask = ptr->GetAcceptedChannelMask();
    brd_info->n_adc_channels        = ptr->GetNADCChannels       ();
    brd_info->n_adc_triggers        = ptr->GetNADCTriggers       ();
  }
}
