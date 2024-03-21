#include "Riostream.h"

#include <TString.h>

#include "PadmeVRecoConfig.hh"

#include "TRawEvent.hh"

#include "HistoSvc.hh"
#include "TrigTimeSvc.hh"
#include "RunConditionSvc.hh"
#include "RunConfigurationSvc.hh"

#include "TargetDigitizer.hh"
#include "TargetHit.hh"

#include "TargetReconstruction.hh"

TargetReconstruction::TargetReconstruction(TString ConfigFileName)
{

  // Attach global services
  fHistoSvc = HistoSvc::GetInstance();
  fTrigTimeSvc = TrigTimeSvc::GetInstance();
  fRunConditionSvc = RunConditionSvc::GetInstance();
  fRunConfigurationSvc = RunConfigurationSvc::GetInstance();

  // Create Target histogram directory
  fHistoSvc->CreateDir("Target");

  // Enable Target specific configuration service
  fTargetConfig = new PadmeVRecoConfig(new utl::ConfigParser(ConfigFileName.Data()),"Target");

  // Define verbose level
  fVerbose = fTargetConfig->GetParOrDefault("Output", "Verbose", 0);

  // Create Target digitizer
  fTargetDigitizer = new TargetDigitizer(fTargetConfig);

  // Check if Hits and Clusters must be written to the output file
  //  fWriteHits     = (Bool_t)fTargetConfig->GetParOrDefault("Output", "Hits"     , 1);
  //fWriteClusters = (Bool_t)fTargetConfig->GetParOrDefault("Output", "Clusters" , 1);
  fWriteStripCharge     = (Bool_t)fTargetConfig->GetParOrDefault("Output", "StripCharge"     , 1);

  if (fVerbose) printf("TargetReconstruction::TargetReconstruction - Target reconstruction system created\n");

}

TargetReconstruction::~TargetReconstruction()
{
  if (fVerbose) printf("TargetReconstruction::~TargetReconstruction - Deleting Target reconstruction system\n");
  delete fTargetDigitizer;
  delete fTargetConfig;
}

void TargetReconstruction::Init()
{
  if (fVerbose>2) printf("TargetReconstruction::Init - Initializing Target reconstruction\n");

  fTargetDigitizer->Init();

}

void TargetReconstruction::ProcessEvent(TMCVEvent* tEvent,TMCEvent* tMCEvent)
{
  if (fVerbose>2) printf("TargetReconstruction::ProcessEvent - Reconstructing MC event\n");
}

void TargetReconstruction::ProcessEvent(TTargetRecoEvent* tEvent,TRecoEvent* tRecoEvent)
{
  if (fVerbose>2) printf("TargetReconstruction::ProcessEvent - Reprocessing reconstructed event\n");
}

void TargetReconstruction::ProcessEvent(TRawEvent* rawEvent)
{

  if (fVerbose>2) printf("TargetReconstruction::ProcessEvent - Reconstructing RawData event\n");

  // Show run energy (only when run changes)
  static Int_t run = 0;
  if (fRunConditionSvc->GetCurrentRun() != run) {
    run = fRunConditionSvc->GetCurrentRun();
    if (fVerbose) printf("TargetReconstruction::ProcessEvent - Run %d has energy %.3f\n",run,fRunConditionSvc->GetRunEnergy());
  }

  //commented by Beth 21/3/24: The concept of "hits" and "clusters" in the target doesn't really have any meaning. It would be better to compute the charge per strip in the digitizer and then convert it to NPoT and other physical quantities here
  /*  fHits.clear();

  //  fTargetDigitizer->BuildHits(rawEvent,fHits);
  //fTargetClusterizer->BuildClusters(fHits,fClusters);
  */
 
  fStripCharge.clear();
  fTargetDigitizer->ComputeChargePerStrip(rawEvent,fStripCharge);

}

void TargetReconstruction::EndProcessing()
{
  if (fVerbose) printf("TargetReconstruction::EndProcessing - Finalizing Target reconstruction\n");
}
