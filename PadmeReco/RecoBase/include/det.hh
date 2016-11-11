#ifndef _RECOBASE_DET_HH_
#define _RECOBASE_DET_HH_ 1
namespace det{
  enum class eSubSystem { eUnknown=0, eEVeto, ePVeto, eHEPVeto, eSAC, eECAL, eTarget };
  enum class eDigitizerTypes { eUnknown=0, eCAENv1751, eCAENv1742, eTEL62 } ;
  enum class eRODeviceTypes  { eUnknown=0, eHamamatsuH9500, ePhotonis, eChineesePMT, eSiPM };
  enum class eDetectorTypes  { eUnknown=0, eVetoScintillatorBar, eECALScintillator};

  class VDetector;
  class VDigitizerChannel;
  class VSubSystem;
  class VDetector;
  class VDigitizer;
  class VDigitizerChannel;
  class VRODevice;
}
#endif
