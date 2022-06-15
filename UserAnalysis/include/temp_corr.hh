#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include "TFile.h"
#include "TH1F.h"
#include "TNtuple.h"
#include "TTree.h"

float GetEventTemp();

float GetEventTempCorr();

int InitTemps();

float GetTemp(long utc_time);
