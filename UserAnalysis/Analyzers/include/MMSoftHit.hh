#ifndef MMSoftHit_h
#define MMSoftHit_h 1

#include "TRecoVHit.hh"
#include "GeneralInfo.hh"

#define IPMODES 2
// 0 -> IP enforced
// 1 -> IP not enforced
#define CLUSTERMODES 8
// 0 -> hit doublet in a plane in a board
// 1 -> after adding further hits in the same plane, board
// 2 -> after adding single hits / doublets from the corresponding board in the other plane [same view]
// 3 -> after matching the other view in the same quadrant [two-view cluster]

class MMSoftHit {

 public:
  
  MMSoftHit();
  ~MMSoftHit();

  void Print() const;
  void CopyHit(TRecoVHit *Hit); //TODO forse getter bdid, view, quad, plane
    
  Int_t GetPlane() const {return fmmi.plane;}
  Int_t GetQuad() const {return fmmi.quad;}
  Int_t GetView() const {return fmmi.view;}
  Int_t GetStrip() const {return fmmi.strip;}
  Int_t GetBoardId() const {return fmmi.bdid;}

  Int_t GetChannelId() const {return fChannelId;}
  TVector3 GetPosition() const {return fPosition;}
  Double_t GetEnergy() const {return fEnergy;}
  Double_t GetTime() const {return fTime;}
  Int_t GetIsolLevel() const {return fIsolLevel;}; //bit 0(1) ON if preceding(following) strip is NOT fired 
  Int_t GetCluPtr(Int_t ipmode, Int_t clumode) const {return fCluPtr[ipmode][clumode];}
  MMchInfo GetMMchInfo() const {return fmmi;}
  
  void SetChannelId(Int_t ChannelId) {fChannelId = ChannelId;}
  void SetPosition(TVector3 Position) {fPosition = Position;}
  void SetEnergy(Double_t Energy) {fEnergy = Energy;}
  void SetTime(Double_t Time) {fTime = Time;}
  void SetIsolLevel(Int_t IsolLevel) {fIsolLevel = IsolLevel;}
  void SetCluPtr(Int_t CluPtr, Int_t ipmode, Int_t clumode) {fCluPtr[ipmode][clumode] = CluPtr;}
  
 protected:

  Int_t    fChannelId;// from TRecoVHit: 4bits for bdid | 8 bits for chid [bdid = 0--15, chid = 0--255]
  TVector3 fPosition; // from TRecoVHit: x,y,z
  Double_t fEnergy;   // from TRecoVHit: peak of the charge samples, sigAmplitude [adc counts]
  Double_t fTime;     // from TRecoVHit: time of the maximum of the charge samples, sigTimePeak [ns]
  Int_t fIsolLevel;   // 0 = the strip before and after are both fired; 1 = the strip before (after) is not (is) fired; 2 = the strip before (after) is (is not) fired
  Int_t fCluPtr[IPMODES][CLUSTERMODES];  // link to the vector of clusters to which it belongs to: first index: IP connection forced/not forced; second index: cluster mode

  MMchInfo fmmi;
};
#endif
