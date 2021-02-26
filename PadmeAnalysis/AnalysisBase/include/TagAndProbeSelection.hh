#ifndef TagAndProbeSelection_h
#define TagAndProbeSelection_h 1

#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TVector3;
class TRecoEvent;
class TRecoVObject;
class TRecoVClusCollection;
class TTargetRecoBeam;


class TagAndProbeSelection : public TObject
{

public:
  TagAndProbeSelection();
  TagAndProbeSelection(Int_t processingMode, Int_t verbosityFlag, Bool_t NoTargetBoolt );
  ~TagAndProbeSelection();
  virtual Bool_t Init(TRecoEvent* eventHeader, 
		      TRecoVObject* ECALev, TRecoVClusCollection* ECALcl,
		      TRecoVObject* SACev, TRecoVClusCollection* SACcl,
		      TRecoVObject* TARGETev, TTargetRecoBeam* TargetBeam);
  virtual Bool_t Finalize(){return true;}

  virtual Bool_t InitHistos();
  virtual Bool_t Process(Bool_t isMC);

  Bool_t phiSymmetricalInECal(TVector3 P1, TVector3 P2,  double& distPhi);
  Bool_t passPreselection(Bool_t isTargetOut, Bool_t isMC);

  void FillHistoTagAndProbe(Double_t DeltaEnergyFunction2, Double_t g2E, Bool_t dPhiIn25Degree,  Bool_t dPhiIn30Degree,  Bool_t dPhiIn35Degree,  Bool_t dPhiIn40Degree,  Bool_t dPhiIn45Degree,  Bool_t dPhiIn50Degree,  Bool_t dPhiIn70Degree,  Bool_t dPhiIn90Degree, std::string sufix);
  void FillHistoTagAndProbe_PhiRegionsTag(Double_t DeltaEnergyFunction,Double_t pTag, std::string sufix );
  void FillHistoTagAndProbe_PhiProbeForTagPhiRegions(Double_t pTag, Double_t pProbe, std::string sifix);
  void FillHistoTagAndProbe_PhiRegionsProbe(Double_t DeltaEnergyFunction2,Double_t pTag, std::string sifix);
  void FillHistoTagAndProbe_RadiusRegionsTag(Double_t DeltaEnergyFunction, Double_t rTag,  std::string sifix);
  void FillHistoTagAndProbe_RadiusRegionsProbe(Double_t DeltaEnergyFunction2,Double_t rTag , Double_t rProbe, std::string sufix);
  void FillHistoTagAndProbe_ProbeRadiusForTagRadiusRegions(Double_t  radius, Double_t radius2, std::string sufix);
  void FillHistoTagAndProbe_RadiusRegionsProbe(Double_t DeltaEnergyFunction2,Double_t rTag, std::string sufix);
  void FillHistoTagAndProbe_DeltaPhiForTagPhiRegions(Double_t phi1 ,Double_t deltaphi,std::string sufix );
  void FillHistoTagAndProbe_PhiRegionsRadiusRegionsTag(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix );
  void FillHistoTagAndProbe_PhiRegionsRadiusRegionsProbe(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix );
  void FillHistoTagAndProbe_PhiRadiusProbeDistribution(Double_t phi ,Double_t radius,Double_t phi2 ,Double_t radius2, std::string sufix );
  void FillHistoTagAndProbe_PhiRegionsNewRadiusRegionsTag(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix );
  void FillHistoTagAndProbe_PhiRegionsNewRadiusRegionsProbe(Double_t DeltaEnergyFunction, Double_t phi ,Double_t radius, std::string sufix );

protected:
  TRecoEvent*           fRecoEvent;
  TRecoVObject*         fECal_hitEvent   ;
  TRecoVObject*         fSAC_hitEvent    ;
  TRecoVObject*         fTarget_hitEvent ;

  TRecoVClusCollection* fECal_ClColl      ;
  TRecoVClusCollection* fSAC_ClColl       ;
  TTargetRecoBeam*      fTarget_RecoBeam  ;


  // fVerbose = 0 (minimal printout),  = 1 (info mode),  = 2 (debug quality)...
  TString  fAlgoName;
  Int_t    fVerbose;
  Int_t    fProcessingMode;
  Bool_t   fNoTargetBool;

  Bool_t fInitToComplete;  

  Double_t fdistanceTarget;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
