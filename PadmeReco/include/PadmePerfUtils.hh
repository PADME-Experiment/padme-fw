#ifndef  _PADMEPERFUTILS_H_
#define _PADMEPERFUTILS_H_

// This is a collection of performance utilities used to
// benchmark the GeoModel (ATLAS/Athena).  Currently it includes a routine
// to fetch the current memory consumption in kilobytes and
// a routine to fetch the current CPU consumption in jiffies.
//
// Joe Boudreau March 2005. // Ported to PADME by S. Spagnolo

class PadmePerfUtils {

 public:

  // Get the current Memory Usage (kbytes);
  static int getMem();

  // Get the current CPU Usage (jiffies= 1/100th of a second):
  static int getCpu();

  // Get total process run time (jiffies= 1/100th of a second):
  static int getRunTime();

};

#endif
