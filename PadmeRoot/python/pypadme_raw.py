#!/usr/bin/env python3

import os
import ctypes
from ctypes.util import find_library
import numpy
import matplotlib.pyplot as plt

class InfoBase(ctypes.Structure):
    def __str__(self):
        ret_list = list()
        for fname, ftype, *_ in self._fields_:
            val = getattr(self, fname)
            if 'mask' in fname:
                sizeof = ctypes.sizeof(ftype)*8
                ret_list.append(f'{fname}=0b{val:0{sizeof}b}')
            else:
                ret_list.append(f'{fname}={val}')
        return f'{type(self).__name__}({", ".join(ret_list)})'

class TriggerInfo(InfoBase):
    _pack_ = 1
    _fields_ = [
            ('trigger_time_tag', ctypes.c_uint32),
            ('start_index_cell', ctypes.c_uint16),
            ('n_samples'       , ctypes.c_uint16),
            ('group_number'    , ctypes.c_uint8 ),
            ('frequency'       , ctypes.c_uint8 ),
            ('trigger_signal'  , ctypes.c_uint8 ),
            ]
class ChannelInfo(InfoBase):
    _pack_ = 1
    _fields_ = [
            ('n_samples'     , ctypes.c_uint16),
            ('channel_number', ctypes.c_uint8 ),
            ]
class BoardInfo(InfoBase):
    _pack_ = 1
    _fields_ = [
            ('event_counter'         , ctypes.c_uint32),
            ('event_timetag'         , ctypes.c_uint32),
            ('active_channel_mask'   , ctypes.c_uint32),
            ('accepted_channel_mask' , ctypes.c_uint32),
            ('board_status'          , ctypes.c_uint32),
            ('board_sn'              , ctypes.c_uint32),
            ('lvds_pattern'          , ctypes.c_uint16),
            ('board_id'              , ctypes.c_uint8 ),
            ('status'                , ctypes.c_uint8 ),
            ('group_mask'            , ctypes.c_uint8 ),
            ('supp_algrtm'           , ctypes.c_uint8 ),
            ('n_adc_triggers'        , ctypes.c_uint8 ),
            ('n_adc_channels'        , ctypes.c_uint8 ),
            ]

class EventInfo(InfoBase):
    _pack_ = 1
    _fields_ = [
            ('event_run_time'    , ctypes.c_uint64   ),
            ('run_number'        , ctypes.c_int32    ),
            ('event_number'      , ctypes.c_uint32   ),
            ('trigger_btf'       , ctypes.c_uint32, 1),
            ('trigger_cosmic'    , ctypes.c_uint32, 1),
            ('trigger_unused1'   , ctypes.c_uint32, 1),
            ('trigger_local'     , ctypes.c_uint32, 1),
            ('trigger_unused2'   , ctypes.c_uint32, 1),
            ('trigger_reserved'  , ctypes.c_uint32, 1),
            ('trigger_random'    , ctypes.c_uint32, 1),
            ('trigger_delayed'   , ctypes.c_uint32, 1),
            ('event_status'      , ctypes.c_uint32   ),
            ('missing_adc_boards', ctypes.c_uint32   ),
            ('n_adc_boards'      , ctypes.c_uint8    ),
            ]


_LIB = ctypes.cdll.LoadLibrary(os.environ['PADMEROOT']+'/lib/libPyPadmeRoot.so')
_LIB.PadmeRaw_new.restype = ctypes.c_void_p
_LIB.PadmeRaw_new.argtype = ctypes.c_char_p
_LIB.PadmeRaw_del.argtypes = ctypes.c_void_p,

_LIB.PadmeRaw_GetEventInfo.argtypes = ctypes.c_void_p, ctypes.POINTER(EventInfo)
#_LIB.PadmeRaw_GetEventInfo.restype  = 

_LIB.PadmeRaw_GetNEvents.argtypes = ctypes.c_void_p,
_LIB.PadmeRaw_GotoEvent.argtypes = ctypes.c_void_p, ctypes.c_uint32
_LIB.PadmeRaw_GetNADCBoards.argtypes = ctypes.c_void_p,

_LIB.PadmeRaw_GetADCBoard.argtypes = ctypes.c_void_p, ctypes.c_uint8
_LIB.PadmeRaw_GetADCBoard.restype = ctypes.c_void_p

_LIB.ADCBoard_GetBoardInfo.argtypes = ctypes.c_void_p, ctypes.POINTER(BoardInfo)
#_LIB.ADCBoard_GetBoardInfo.restype  = 

_LIB.ADCBoard_GetADCChannel.argtypes = ctypes.c_void_p, ctypes.c_uint8
_LIB.ADCBoard_GetADCChannel.restype = ctypes.c_void_p

_LIB.ADCBoard_GetADCTrigger.argtypes = ctypes.c_void_p, ctypes.c_uint8
_LIB.ADCBoard_GetADCTrigger.restype = ctypes.c_void_p

_LIB.ADCChannel_GetArray.argtypes = ctypes.c_void_p,
_LIB.ADCChannel_GetArray.restype = numpy.ctypeslib.ndpointer(dtype=numpy.uint16, shape=1024, ndim=1, flags='C_CONTIGUOUS')

_LIB.ADCTrigger_GetArray.argtypes = ctypes.c_void_p,
_LIB.ADCTrigger_GetArray.restype = numpy.ctypeslib.ndpointer(dtype=numpy.uint16, shape=1024, ndim=1, flags='C_CONTIGUOUS')

_LIB.ADCChannel_GetInfo.argtypes = ctypes.c_void_p, ctypes.POINTER(ChannelInfo)
#_LIB.ADCChannel_GetInfo.restype = 

_LIB.ADCTrigger_GetInfo.argtypes = ctypes.c_void_p, ctypes.POINTER(TriggerInfo)
#_LIB.ADCTrigger_GetInfo.restype = 



class ADCSamplingChannel:
    def __init__(self, *args, **kwargs):
        self._parent_brd = kwargs['parent']
    @property
    def parent_brd(self): return self._parent_brd
    def __getitem__(self, samp): return self.array[samp]
    def __array__(self): return self.array
    def __iter__(self): return iter(self.array)
    def __len__(self): return len(self.array)
    def __mul__(self, val): return self.array * val
    def __sub__(self, val): return self.array - val
    def __add__(self, val): return self.array + val
    def __truediv__(self, val): return self.array / val
    def __rmul__(self, val): return val * self.array
    def __rsub__(self, val): return val - self.array
    def __radd__(self, val): return val + self.array
    def __rtruediv__(self, val): return val / self.array
    def __str__(self):
        return f'{type(self).__name__}({self._info},{self.array})'
class ADCChannel(ADCSamplingChannel):
    def __init__(self, ptr: ctypes.c_void_p, trig, **kwargs):
        super().__init__(self, **kwargs)
        self.__ptr = ptr
        self._trig = trig
        self._info = None
        self._array = None
    @property
    def info(self):
        if self._info is None:
            self._info = ChannelInfo()
            _LIB.ADCChannel_GetInfo(self.__ptr, ctyeps.byref(self._info))
        return self._info
    @property
    def array(self):
        if self._array is None:
            self._array = _LIB.ADCChannel_GetArray(self.__ptr)
        return self._array
    @property
    def trigger(self):
        return self._trig

class ADCTrigger(ADCSamplingChannel):
    def __init__(self, ptr: ctypes.c_void_p, **kwargs):
        super().__init__(self, **kwargs)
        self.__ptr = ptr
        self._info = None
        self._array = None
    @property
    def info(self):
        if self._info is None:
            self._info = TriggerInfo()
            _LIB.ADCTrigger_GetInfo(self.__ptr, ctypes.byref(self._info))
        return self._info
    @property
    def array(self):
        if self._array is None:
            self._array = _LIB.ADCTrigger_GetArray(self.__ptr)
        return self._array

class ADCBoard:
    def __init__(self, ptr: ctypes.c_void_p):
        self.__ptr = ptr
        self._info = BoardInfo()
        _LIB.ADCBoard_GetBoardInfo(self.__ptr, ctypes.byref(self._info))
        self._channels = {}
        self._triggers = {}
        trig = 0
        for ofs in range(4):
            if self._info.group_mask & (1<<ofs):
                trig_ptr = _LIB.ADCBoard_GetADCTrigger(self.__ptr, trig)
                self._triggers.update({ofs: ADCTrigger(trig_ptr, parent=self)})
                trig += 1
        chan = 0
        for ofs in range(32):
            if self._info.active_channel_mask & (1<<ofs):
                chan_ptr = _LIB.ADCBoard_GetADCChannel(self.__ptr, chan)
                trig = self._triggers[ofs//8]
                self._channels.update({ofs: ADCChannel(chan_ptr, trig, parent=self)})
                chan += 1
    def __str__(self):
        return f'{type(self).__name__}({self._info})'
    @property
    def chan_map(self): return self._channels
    @property
    def trig_map(self): return self._triggers
    @property
    def chan_list(self): return list(self._channels.values())
    @property
    def trig_list(self): return list(self._triggers.values())
    @property
    def info(self): return self._info

class PadmeEvent:
    def __init__(self, boards, info):
        self._boards = boards
        self._info = info
    def __str__(self):
        return f'{type(self).__name__}({self._info})'
    def __getitem__(self, brd):
        return self._boards[brd]
    def get_board_by_id(self, brd_id):
        for b in self._boards:
            if b.get_board_info().board_id == brd_id:
                return b
        return None
    def get_board_by_sn(self, brd_sn):
        for b in self._boards:
            if b.get_board_info().board_sn == brd_sn:
                return b
        return None
    @property
    def info(self):
        return self._info
    @property
    def board_list(self):
        return self._boards

class PadmeRawIterator:
    def __init__(self, parent):
        self._parent = parent
        self._eventptr = 0
    def __next__(self):
        if self._eventptr < self._parent.n_events:
            ret = self._parent.get_event(self._eventptr)
            self._eventptr += 1
            return ret
        else:
            raise StopIteration

class PadmeRaw:
    def __init__(self, root_file: str):
        self._ptr = _LIB.PadmeRaw_new(root_file.encode())
        self.n_events = _LIB.PadmeRaw_GetNEvents(self._ptr)
    def __del__(self):
        _LIB.PadmeRaw_del(self._ptr)
    def __len__(self):
        return self.n_events
    def __enter__(self):
        return self
    def __exit__(self, exc_type, exc_val, exc_tb):
        pass
    def __getitem__(self, key):
        if isinstance(key, slice):
            start, stop, step = key.start, key.stop, key.step
            if start == None:
                start = 0
            if stop == None:
                stop = self.n_events
            if step == None:
                step = 1
            return (self.get_event(evt) for evt in range(start, stop, step))
        elif isinstance(key, int):
            return self.get_event(key)
        else:
            raise TypeError("Invalid argument type.")
    def __iter__(self):
        return PadmeRawIterator(self)
    def get_event(self, event):
        if 0 <= event < self.n_events:
            _LIB.PadmeRaw_GotoEvent(self._ptr, event)
            evt_info = EventInfo()
            _LIB.PadmeRaw_GetEventInfo(self._ptr, ctypes.byref(evt_info))
            nbrds = _LIB.PadmeRaw_GetNADCBoards(self._ptr)
            boards =  [ADCBoard(_LIB.PadmeRaw_GetADCBoard(self._ptr, brd)) for brd in range(nbrds)]
            return PadmeEvent(boards, evt_info)
        else:
            raise IndexError("event idex out of range")

if __name__ == '__main__':
    import matplotlib.pyplot as plt
    with PadmeRaw('/data/daq_0030355/run_0030355_20200917_113409_lvl1_00_000.root') as raw:
        for evt in raw[:20]:
            samp = evt.board_list[12].chan_map[20]
            samp_norm = (samp-samp[:200].mean())/4.096
            plt.plot(samp_norm)
        plt.show()
