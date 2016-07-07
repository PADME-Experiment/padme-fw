/**
 * \file TThrREVRaw.hh
 * \author Georgi Georgiev <ggeorge@cern.ch>
 * \date 2016-07-07
 */

#include "TREVRaw.hh"

#ifndef _TDiscretREVRaw_hh_
#define _TDiscretREVRaw_hh_ 0
class TDiscretREVRaw:TREVRaw{
  private:
    double fTimeStamp; ///< Timestamp of the amplitude change
    bool fIsRisingEdge; ///< True if rising edge, False if falling edge
    double fAmplitude; ///< On what amplitude triggers
};
#endif
