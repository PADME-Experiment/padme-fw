#include "PadmeVGeometry.hh"
#include "RecoVChannelID.hh"
#include "TRecoVHit.hh"

#include "Math/Translation3D.h"
#include "Math/RotationX.h"
#include "Math/RotationY.h"
#include "Math/RotationZ.h"


#include <map>
#include <vector>


PadmeVGeometry::PadmeVGeometry(){  

  fLocOxinPadmeFrame = 0.;
  fLocOyinPadmeFrame = 0.;
  fLocOzinPadmeFrame = 0.;
  fLocAngleXPadmeFrame = 0.;
  fLocAngleYPadmeFrame = 0.;
  fLocAngleZPadmeFrame = 0.;
  fChIdx0Offset = 0.;
  fChIdy0Offset = 0.;
  fChIdz0Offset = 0.;
  
  fUsePrimaryNumbers = true;

  //  fLocalPosMap  = NULL;
  fGlobalPosMap = NULL;
  fLocalToGlobal = NULL;  
}
PadmeVGeometry::~PadmeVGeometry()
{
  if (fGlobalPosMap) delete fGlobalPosMap;
  if (fLocalToGlobal) delete fLocalToGlobal;
}


void PadmeVGeometry::Init(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  //  fLocalPosMap  = new std::map<int, XYZPoint>;
  fGlobalPosMap = new std::map<int, XYZPoint>;
  fLocalToGlobal = new Transform3D();

  fUsePrimaryNumbers = (bool)cfg->GetParOrDefault("GEOMETRY","ComputeFromPrimaryNumbers",1);
  if (!fUsePrimaryNumbers) InitFromChIdPositionMap(cfg, chIdMgr); 
  else InitFromPrimaryNumbers(cfg);
}

void PadmeVGeometry::InitFromPrimaryNumbers(PadmeVRecoConfig *cfg)
{
  fLocOxinPadmeFrame = (double)cfg->GetParOrDefault("GEOMETRY","LocalOrigineX",0.);
  fLocOyinPadmeFrame = (double)cfg->GetParOrDefault("GEOMETRY","LocalOrigineY",0.);
  fLocOzinPadmeFrame = (double)cfg->GetParOrDefault("GEOMETRY","LocalOrigineZ",0.);
  fLocAngleXPadmeFrame = (double)cfg->GetParOrDefault("GEOMETRY","LocalAngleXPadmeFrame",0.);
  fLocAngleYPadmeFrame = (double)cfg->GetParOrDefault("GEOMETRY","LocalAngleYPadmeFrame",0.);
  fLocAngleZPadmeFrame = (double)cfg->GetParOrDefault("GEOMETRY","LocalAngleZPadmeFrame",0.);
  fStep1ChLocalX = (double)cfg->GetParOrDefault("GEOMETRY","Step1LocalX",0.);
  fStep1ChLocalY = (double)cfg->GetParOrDefault("GEOMETRY","Step1LocalY",0.);
  fStep1ChLocalZ = (double)cfg->GetParOrDefault("GEOMETRY","Step1LocalZ",0.);
  fChIdx0 = (int)cfg->GetParOrDefault("GEOMETRY","ChId@LocalX0",0.);
  fChIdy0 = (int)cfg->GetParOrDefault("GEOMETRY","ChId@LocalY0",0.);
  fChIdz0 = (int)cfg->GetParOrDefault("GEOMETRY","ChId@LocalZ0",0.);
  fChIdx0Offset = (double)cfg->GetParOrDefault("GEOMETRY","ChId@LocalX0Offset",0.);
  fChIdy0Offset = (double)cfg->GetParOrDefault("GEOMETRY","ChId@LocalY0Offset",0.);
  fChIdz0Offset = (double)cfg->GetParOrDefault("GEOMETRY","ChId@LocalZ0Offset",0.);
  Transform3D a = Translation3D(fLocOxinPadmeFrame, fLocOyinPadmeFrame, fLocOzinPadmeFrame)
    *RotationX(fLocAngleXPadmeFrame)
    *RotationY(fLocAngleYPadmeFrame)
    *RotationZ(fLocAngleZPadmeFrame);
  *fLocalToGlobal = a;
}
void PadmeVGeometry::InitFromChIdPositionMap(PadmeVRecoConfig *cfg, RecoVChannelID *chIdMgr)
{
  std::vector<int> chIds = chIdMgr->GetChannelIDVector();

  std::vector<float> vCoord;
  utl::ConfigParser *cfgParser= cfg->GetConfigParser();
  // look for explicit globalPositions for all channels
  std::string GPosId;
  for (unsigned int j=0; j<chIds.size();++j)
    {
      GPosId = "GPos_"+std::to_string(chIds[j]);
      if(cfgParser->HasConfig("GEOMETRY",GPosId)) 
	{
	  std::vector<std::string> xyzPosString = cfgParser->GetConfig("GEOMETRY",GPosId);
	  auto it = xyzPosString.begin();
	  double gposX = std::stof(*it);++it;
	  double gposY = std::stof(*it);++it;
	  double gposZ = std::stof(*it);++it;
	  if (  fGlobalPosMap->find(chIds[j])==fGlobalPosMap->end()  )
	    fGlobalPosMap->insert ( std::pair<int,XYZPoint>(chIds[j], XYZPoint(gposX, gposY, gposZ)) ); 
	}
    }
    
}

TVector3  PadmeVGeometry::LocalPosition(Int_t chId)
{
  double x = (chId-fChIdx0)*fStep1ChLocalX + fChIdx0Offset;
  double y = (chId-fChIdy0)*fStep1ChLocalY + fChIdy0Offset;
  double z = (chId-fChIdz0)*fStep1ChLocalZ + fChIdz0Offset;

  return TVector3(x,y,z);
}
TVector3  PadmeVGeometry::GlobalPosition(Int_t chId)
{

  if (!fUsePrimaryNumbers)
    {
      //using positions read and stored in the map
      if (  fGlobalPosMap->find(chId)!=fGlobalPosMap->end()  )
	{
	  XYZPoint p = fGlobalPosMap->find(chId)->second;
      return TVector3(p.X(),p.Y(),p.Z());
	}
      else
	{
	  std::cout<<"ERROR- Global Position for chID = "<<chId<<" not found in the map; set to 0,0,0"<<std::endl;
	  return TVector3(0.,0.,0.);
	}
    }
  
       TVector3 myPos = LocalPosition(chId);
       XYZPoint lPos = XYZPoint(myPos.X(), myPos.Y(), myPos.Z());
     
       //std::cout << "****x " <<myPos.X() << "***y  " << myPos.Y() << "   **z   "<<myPos.Z()<<std::endl;
 


  
  ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<double>, ROOT::Math::DefaultCoordinateSystemTag> gPos = 
    GetLocalToGlobalTransf()*lPos;

  //std::cout << "****************GetLocalToGlobalTransf()   "   <<*fLocalToGlobal<<std::endl;

  //std::cout << "channelId  "<<chId<<"   gpos  x " << gPos.X() << "  y  " << gPos.Y() << "   z   "<<gPos.Z()<<std::endl;
  
  return TVector3(gPos.X(), gPos.Y(), gPos.Z());
}


void PadmeVGeometry::ComputePositions(std::vector<TRecoVHit *> &Hits)
{
  for(unsigned int iHit = 0; iHit < Hits.size(); ++iHit){
    int ich = Hits[iHit]->GetChannelId();
    //std::cout << "Computing position of chId " << ich << "  " << GlobalPosition(ich) << std::endl;
    Hits[iHit]->SetPosition( GlobalPosition(ich) );
    
    
  }

}
void PadmeVGeometry::UpdateTransform()
{
 Transform3D a = Translation3D(fLocOxinPadmeFrame, fLocOyinPadmeFrame, fLocOzinPadmeFrame)
    *RotationX(fLocAngleXPadmeFrame)
    *RotationY(fLocAngleYPadmeFrame)
    *RotationZ(fLocAngleZPadmeFrame);
    *fLocalToGlobal = a;

}
