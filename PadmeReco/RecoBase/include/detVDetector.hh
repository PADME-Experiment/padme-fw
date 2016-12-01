#ifndef _RECOBASE_DETVDETECTOR_HH_
#define _RECOBASE_DETVDETECTOR_HH_ 1
#include<stdio.h>
#include<memory>
#include<iostream>
#include<vector>
#include<map>
#include<stdarg.h>
#include<typeinfo>   // operator typeid

#include"det.hh"
#include"fwk.hh"

#include"detVPadmeElement.hh"
#include"detVDetectorLayer.hh"


namespace det{
  class VDetectorLayer;
  class VDetector       :public VPadmeElement{
    public:
      ~VDetector       (){}
    protected:
      VDetector       (){ERROR("default constructor");}
      VDetector       (VDetectorLayer* lay):fParentLayer(lay){}
      VDetector       (const std::string&n,VDetectorLayer* lay):VPadmeElement(n),fParentLayer(lay){}
      VDetector       (const std::string&n):VPadmeElement(n){}
    protected:
      double fCoordinates[3];
      double fOrientation[2];
      VDetectorLayer* fParentLayer;
      //std::shared_ptr<VVeto> fParentLayer
      det::eDetectorTypes fDetectorType;
  };
  class EVetoScintillatorBar  :public VDetector{public:  EVetoScintillatorBar(const std::string&n):VDetector(n){fDetectorType=eDetectorTypes::eEVetoScintillatorBar  ;}  };
  class PVetoScintillatorBar  :public VDetector{public:  PVetoScintillatorBar(const std::string&n):VDetector(n){fDetectorType=eDetectorTypes::ePVetoScintillatorBar  ;}  };
  class HEPVetoScintillatorBar:public VDetector{public:HEPVetoScintillatorBar(const std::string&n):VDetector(n){fDetectorType=eDetectorTypes::eHEPVetoScintillatorBar;}  };
  class ECALScintillator      :public VDetector{public:      ECALScintillator(const std::string&n):VDetector(n){fDetectorType=eDetectorTypes::eECALScintillator   ;}  };





  //class RODeviceVPMT:public VRODevice{public: protected:double fHV;};
  //class RODeviceVSAPMT:public RODeviceVPMT{public: unsigned fM,fN;double fGain;};
  //class RODeviceVMAPMT:public RODeviceVPMT{public: unsigned fM,fN;double *pixelGains;};
  //class MAPMTHamamatsuH9500:public RODeviceVMAPMT{};
  //class PMTPhotonis:public RODeviceVSAPMT{};
  //class DetectorVScintillator:public VDetector{};
  //class DetBC540:public DetectorVScintillator{};

  //class RawVChannel{};
  //class RawChannelVSampling:public RawVChannel{};
}

#endif
