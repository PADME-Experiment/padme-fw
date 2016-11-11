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
#include"detVDigitizerChannel.hh"
#include"detVDigitizer.hh"
#include"detVDetectorSystem.hh"
#include"detVRODevice.hh"

namespace det{
  class VDetector       :public VPadmeElement{
    protected:
      ~VDetector       (){}
    protected:
      double fCoordinates[3];
      double fOrientation[2];
      std::shared_ptr<VSubSystem> fParentDetector;
      //std::shared_ptr<VVeto> fParentLayer
      det::eDetectorTypes fDetectorType;
  };
  class VetoScintillatorBar:public VDetector{public:VetoScintillatorBar(){fDetectorType=eDetectorTypes::eVetoScintillatorBar;}  };
  class ECALScintillator   :public VDetector{public:ECALScintillator   (){fDetectorType=eDetectorTypes::eECALScintillator   ;}  };





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
