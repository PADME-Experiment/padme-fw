#include "temp_corr.hh"

const float t_ref=36.195;
const int nmax=40000;
long tstamp_r[nmax];
float ttemp_r[nmax];
int ivalr;
int iret=1;
bool verbose_t=false;
float event_temp;

// Define some simple structures
typedef struct {
  Long_t tstamp;
  Int_t nch;
  Float_t temp_r;
} ECALTR;
ECALTR ecaltr;

float GetEventTemp() { return event_temp;}

float GetEventTempCorr() {
  float corr=1.+(+.01*(event_temp-t_ref));
  return corr;
}

int InitTemps() {
   // Create a histogram for the values we read.
   //auto myHist_r = new TH1F("h1","ntuple",100,-4,4);
   // Open the file containing the tree.
   auto myFile_r = TFile::Open("ECAL_TR_tree.root");
   if (!myFile_r || myFile_r->IsZombie()) {
     iret=1;
     return 1;
   }
   TTree *f1 = (TTree*)myFile_r->Get("T_R");
   f1->SetBranchAddress("ecalT_r",&ecaltr);
 

   ivalr=0;
   Int_t nentries = (Int_t)f1->GetEntries();
   for (Int_t i=0;i<nentries;i++) {
      f1->GetEntry(i);
      tstamp_r[ivalr]=ecaltr.tstamp;
      ttemp_r[ivalr]=ecaltr.temp_r;
      if(ivalr<10) {
	printf(" i=%d  tstamp=%ld  temp=%f \n",ivalr,tstamp_r[ivalr],ttemp_r[ivalr]);
      }
      ivalr++;
   }
   printf(" number of right values read = %d \n",ivalr);
   myFile_r->Close();

   iret=0;
  /*
   // set correct values
    int ivalmax=ivalr;
    float timeinterv =tstamp_r[ivalmax-1]-tstamp_r[0];
    float timestart=tstamp_r[0];
    printf("ivalmax = %d , timeinterv=%f , timestart=%f \n",ivalmax,timeinterv,timestart);
   */

    return 0;
}

float GetTemp(long utc_time) {
  event_temp=0.;
  if(iret==0) {
    //int iret=InitTemps();
    int ivalmax=0;
    float timeinterv=0;
    float timestart=0;
    
    // set correct values
    ivalmax=ivalr;
    timeinterv =tstamp_r[ivalmax-1]-tstamp_r[0];
    timestart=tstamp_r[0];
    // printf("ivalmax = %d , timeinterv=%f , timestart=%f \n",ivalmax,timeinterv,timestart);
    
    int ival=0;
    int ivalfound=0;
    // check read values
    for (ival=0;ival<ivalmax;ival++) {
      if(utc_time<tstamp_r[ival]) {
	// safeguard for wrong points
	if(ttemp_r[ival-1]<32.) {
	  ivalfound=ival;
	} else {
	  ivalfound=ival-1;
	}
	break;
      }
    }
    if(verbose_t) {
      printf(" ivalfound=%d tvalue=%ld  tstamp_r=%ld  temp_r=%f  \n",ivalfound,utc_time,tstamp_r[ivalfound],ttemp_r[ivalfound]);
      printf("  tstamp_r+1=%ld   temp_r+1=%f \n",tstamp_r[ivalfound+1],ttemp_r[ivalfound+1]);
    }
   event_temp=ttemp_r[ivalfound];
   return ttemp_r[ivalfound];
  } else {
    printf(" -- init_temps not initialized ! -- \n");
    return 0.;
  }

}
