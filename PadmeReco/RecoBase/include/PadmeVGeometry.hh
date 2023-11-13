#ifndef PADMEVGEOMETRY_H
#define PADMEVGEOMETRY_H

//#include "TRecoVHit.hh"
#include "PadmeVRecoConfig.hh"
//#include <map>
//#include <vector>
#include "TMath.h"
#include "Math/Transform3D.h"
//#include "Math/Translation3D.h"
//#include "Math/RotationX.h"
//#include "Math/RotationY.h"
//#include "Math/RotationZ.h"
#include "Math/PositionVector3D.h"
#include "Math/Point3Dfwd.h"
#include "TVector3.h"

//class TVector3;
class RecoVChannelID;
class TRecoVHit;
class TRecoVCluster;
//typedef ROOT::PositionVector3D<ROOT::Cartesian3D<double>,ROOT::DefaultCoordinateSystemTag> posVec3D;
//typedef ROOT::Transform3D trasf3D;

using namespace ROOT::Math;

class PadmeVGeometry {
public:
  PadmeVGeometry();
  virtual  ~PadmeVGeometry();
  
  virtual void Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr);
  virtual void InitFromPrimaryNumbers(PadmeVRecoConfig *cfg);  // initialization from a few numbers from CAD/Survey/etc - positions of ChId are computed
  virtual void InitFromChIdPositionMap(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr); // positions of detector elements (uniquely identified by ChId) are read / superseeded by values read in the map
  virtual bool usePrimaryNumbers() {return fUsePrimaryNumbers;}
  virtual bool useChIdPositionMap(){return !fUsePrimaryNumbers;}
  //virtual void overrideChIdPosWithMap(Int_t chId);
  //virtual void overrideChIdPosWithMap(std::vector<Int_t> chIdVec);


  virtual TVector3 LocalPosition(Int_t chId);
  virtual TVector3 LocalPosition(Double_t posXInChIdUnits, Double_t posYInChIdUnits);
  virtual TVector3 GlobalPosition(Int_t chId);
  virtual TVector3 GetPosition(Int_t chId){return this->GlobalPosition(chId);}
  virtual void SetLocalOriginInPadmeFrame(double x, double y, double z) {fLocOxinPadmeFrame=x; fLocOyinPadmeFrame=y; fLocOzinPadmeFrame=z;}
  virtual void SetLocalAngleAroundX(double angle_x){fLocAngleXPadmeFrame = angle_x;}
  virtual void SetLocalAngleAroundY(double angle_y){fLocAngleYPadmeFrame = angle_y;}
  virtual void SetLocalAngleAroundZ(double angle_z){fLocAngleZPadmeFrame = angle_z;}
  //  virtual void ResetLocalToGlobalTransf();
  virtual void ComputePositions(std::vector<TRecoVHit *> &hitArray);
  virtual double GetLocalAngleAroundX(){return fLocAngleXPadmeFrame;}
  virtual double GetLocalAngleAroundY(){return fLocAngleYPadmeFrame;}
  virtual double GetLocalAngleAroundZ(){return fLocAngleZPadmeFrame;}
  virtual TVector3 GetLocalOriginInPadmeFrame(){return TVector3(fLocOxinPadmeFrame,fLocOyinPadmeFrame,fLocOzinPadmeFrame);}
  virtual void UpdateTransform();
  XYZPoint globalFromLocal(TVector3& p);
  XYZPoint localFromGlobal(TVector3& p);
 
  
protected:
  int fNChannels;

  bool fUsePrimaryNumbers;  
  
  Transform3D& GetLocalToGlobalTransf(){return *fLocalToGlobal;}
  Transform3D GetGlobalToLocalTransf(){return (*fLocalToGlobal).Inverse();}
  
  //  std::map<int, XYZPoint> *fLocalPosMap;
  std::map<int, XYZPoint> *fGlobalPosMap;
  Transform3D* fLocalToGlobal;

  double fLocOxinPadmeFrame;
  double fLocOyinPadmeFrame;
  double fLocOzinPadmeFrame;
  double fLocAngleXPadmeFrame;
  double fLocAngleYPadmeFrame;
  double fLocAngleZPadmeFrame;
  double fStep1ChLocalX;
  double fStep1ChLocalY;
  double fStep1ChLocalZ;
  int fChIdx0;
  int fChIdy0;
  int fChIdz0;
  double fChIdx0Offset;
  double fChIdy0Offset;
  double fChIdz0Offset;
  
  };

#endif
