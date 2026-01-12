#ifndef MMSoftHit_h
#define MMSoftHit_h 1

#define CLUSTERMODES 8
// 0 -> hit doublet in a plane in a board
// 1 -> after adding further hits in the same plane, board
// 2 -> after adding single hits / doublets from the corresponding board in the other plane [same view]
// 3 -> after matching the other view in the same quadrant [two-view cluster]

class MMSoftHit(){

 public:
  
  MMSoftHit();
  ~MMSoftHit(){};
  void Print(Option_t* option="") const;

 protected:

  Int_t    fChannelId;// from TRecoVHit: 4bits for bdid | 8 bits for chid [bdid = 0--15, chid = 0--255]
  TVector3 fPosition; // from TRecoVHit: x,y,z
  Double_t fEnergy;   // from TRecoVHit: peak of the charge samples, sigAmplitude [adc counts]
  Double_t fTime;     // from TRecoVHit: time of the maximum of the charge samples, sigTimePeak [ns]
  Int_t fIsolLevel;   // 0 = the strip before and after are both fired; 1 = the strip before (after) is not (is) fired; 2 = the strip before (after) is (is not) fired
  Int_t fCluPtr[2][CLUSTERMODES];  // link to the vector of clusters to which it belongs to: first index: IP connection forced/not forced; second index: cluster mode
  
  ClassDef(MMSoftHit,1);
};
#endif
