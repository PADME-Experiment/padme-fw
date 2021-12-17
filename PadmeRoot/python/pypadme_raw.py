#!/usr/bin/env python3

import os
import ctypes
from ctypes.util import find_library
import numpy
import matplotlib.pyplot as plt

class InfoBase(ctypes.Structure):
    def __repr__(self):
        ret_list = list()
        for field, *_ in self._fields_:
            val = getattr(self, field)
            ret_list.append(f'{field}={val}')
        return f'{type(self).__name__}({", ".join(ret_list)})'

class BoardInfo(InfoBase):
    _pack_ = 1
    _fields_ = [
            ('board_id'              , ctypes.c_uint8 ),
            ('board_sn'              , ctypes.c_uint32),
            ('lvds_pattern'          , ctypes.c_uint16),
            ('status'                , ctypes.c_uint8 ),
            ('board_status'          , ctypes.c_uint32),
            ('group_mask'            , ctypes.c_uint8 ),
            ('event_counter'         , ctypes.c_uint32),
            ('event_timetag'         , ctypes.c_uint32),
            ('supp_algrtm'           , ctypes.c_uint8 ),
            ('active_channel_mask'   , ctypes.c_uint32),
            ('accepted_channel_mask' , ctypes.c_uint32),
            ]

class EventInfo(InfoBase):
    _pack_ = 1
    _fields_ = [
            ('run_number'        , ctypes.c_int32    ),
            ('event_number'      , ctypes.c_uint32   ),
            ('event_run_time'    , ctypes.c_uint64   ),
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

_LIB.PadmeRaw_GetNEvents.argtypes = ctypes.c_void_p,
_LIB.PadmeRaw_GotoEvent.argtypes = ctypes.c_void_p, ctypes.c_uint32
_LIB.PadmeRaw_GetNADCBoards.argtypes = ctypes.c_void_p,

_LIB.PadmeRaw_GetADCBoard.argtypes = ctypes.c_void_p, ctypes.c_uint8
_LIB.PadmeRaw_GetADCBoard.restype = ctypes.c_void_p

_LIB.PadmeRaw_GetEventInfo.argtypes = ctypes.c_void_p,
_LIB.PadmeRaw_GetEventInfo.restype  = EventInfo

_LIB.ADCBoard_GetADCChannel.argtypes = ctypes.c_void_p, ctypes.c_uint8
_LIB.ADCBoard_GetADCChannel.restype = numpy.ctypeslib.ndpointer(dtype=numpy.uint16, shape=1024, ndim=1, flags='C_CONTIGUOUS')

_LIB.ADCBoard_GetADCTrigger.argtypes = ctypes.c_void_p, ctypes.c_uint8
_LIB.ADCBoard_GetADCTrigger.restype = numpy.ctypeslib.ndpointer(dtype=numpy.uint16, shape=1024, ndim=1, flags='C_CONTIGUOUS')

_LIB.ADCBoard_GetNADCChannels.argtypes = ctypes.c_void_p,
_LIB.ADCBoard_GetNADCTriggers.argtypes = ctypes.c_void_p,

_LIB.ADCBoard_GetBoardInfo.argtypes = ctypes.c_void_p,
_LIB.ADCBoard_GetBoardInfo.restype  = BoardInfo

class ADCBoard:
    def __init__(self, ptr: ctypes.c_void_p):
        self.ptr = ptr
        self.info = _LIB.ADCBoard_GetBoardInfo(self.ptr)
        self.nchannels = _LIB.ADCBoard_GetNADCChannels(self.ptr)
        self.ntriggers = _LIB.ADCBoard_GetNADCTriggers(self.ptr)
    def get_channel(self, chan: int):
        if 0 <= chan < self.nchannels:
            return _LIB.ADCBoard_GetADCChannel(self.ptr, chan)
        else:
            raise ValueError(f'chan {chan} is out of channel range [0; {self.nchannels})')
    def get_trigger(self, trig: int):
        if 0 <= trig < self.ntriggers:
            return _LIB.ADCBoard_GetADCTrigger(self.ptr, trig)
        else:
            raise ValueError(f'trig {trig} is out of trigger range [0; {self.ntriggers})')
    def get_board_info(self):
        return self.info
    def __getitem__(self, chan):
        return self.get_channel(chan)

class PadmeEvent:
    def __init__(self, boards, info):
        self.boards = boards
        self.info = info
    def __getitem__(self, brd):
        return self.boards[brd]
    def get_board(self, brd):
        return self.boards[brd]
    def get_board_by_id(self, brd_id):
        for b in self.boards:
            if b.get_board_info().board_id == brd_id:
                return b
        return None
    def get_board_by_sn(self, brd_sn):
        for b in self.boards:
            if b.get_board_info().board_sn == brd_sn:
                return b
        return None


class PadmeRawIterator:
    def __init__(self, parent):
        self.parent = parent
        self.eventptr = 0
    def __next__(self):
        if self.eventptr < self.parent.n_events:
            ret = self.parent.get_event(self.eventptr)
            self.eventptr += 1
            return ret
        else:
            raise StopIteration

class PadmeRaw:
    def __init__(self, root_file: str):
        self.ptr = _LIB.PadmeRaw_new(root_file.encode())
        self.n_events = _LIB.PadmeRaw_GetNEvents(self.ptr)
    def __del__(self):
        _LIB.PadmeRaw_del(self.ptr)
    def __len__(self):
        return self.n_events
    def __getitem__(self, key):
        if isinstance(key, slice):
            start, stop, step = key.start, key.stop, key.step
            if start == None:
                start = 0
            if stop == None:
                stop = self.parent.n_events
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
            _LIB.PadmeRaw_GotoEvent(self.ptr, event)
            evt_info = _LIB.PadmeRaw_GetEventInfo(self.ptr)
            nbrds = _LIB.PadmeRaw_GetNADCBoards(self.ptr)
            boards =  [ADCBoard(_LIB.PadmeRaw_GetADCBoard(self.ptr, brd)) for brd in range(nbrds)]
            return PadmeEvent(boards, evt_info)
        else:
            raise IndexError("event idex out of range")


if __name__ == '__main__':
    raw = PadmeRaw('/data/daq_0030355/run_0030355_20200917_113409_lvl1_00_000.root')
    for r in raw[:20]:
        trg = r[12].get_channel(10)
        plt.plot(trg)
    plt.show()
