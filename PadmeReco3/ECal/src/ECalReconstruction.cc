#include "Riostream.h"

#include <TString.h>

#include "PadmeVRecoConfig.hh"

#include "TRawEvent.hh"

#include "HistoSvc.hh"
#include "TrigTimeSvc.hh"
#include "RunConditionSvc.hh"
#include "RunConfigurationSvc.hh"

#include "ECalDigitizer.hh"
#include "ECalClusterizer.hh"
#include "ECalHit.hh"
#include "ECalCluster.hh"

#include "ECalReconstruction.hh"

ECalReconstruction::ECalReconstruction(TString ConfigFileName)
{

  // Attach global services
  fHistoSvc = HistoSvc::GetInstance();
  fTrigTimeSvc = TrigTimeSvc::GetInstance();
  fRunConditionSvc = RunConditionSvc::GetInstance();
  fRunConfigurationSvc = RunConfigurationSvc::GetInstance();

  // Create ECal histogram directory
  fHistoSvc->CreateDir("ECal");

  // Enable ECal specific configuration service
  fECalConfig = new PadmeVRecoConfig(new utl::ConfigParser(ConfigFileName.Data()),"ECal");

  // Define verbose level
  fVerbose = fECalConfig->GetParOrDefault("Output", "Verbose", 0);

  // Create ECal digitizer
  fECalDigitizer = new ECalDigitizer(fECalConfig);

  // Create ECal clusterizer
  fECalClusterizer = new ECalClusterizer(fECalConfig);

  // Check if Hits and Clusters must be written to the output file
  fWriteHits     = (Bool_t)fECalConfig->GetParOrDefault("Output", "Hits"     , 1);
  fWriteClusters = (Bool_t)fECalConfig->GetParOrDefault("Output", "Clusters" , 1);

  if (fVerbose) printf("ECalReconstruction::ECalReconstruction - ECal reconstruction system created\n");

}

ECalReconstruction::~ECalReconstruction()
{
  if (fVerbose) printf("ECalReconstruction::~ECalReconstruction - Deleting ECal reconstruction system\n");
  delete fECalDigitizer;
  delete fECalClusterizer;
  delete fECalConfig;
}

void ECalReconstruction::Init()
{
  if (fVerbose>2) printf("ECalReconstruction::Init - Initializing ECal reconstruction\n");

  fECalDigitizer->Init();
  fECalClusterizer->Init();

}

void ECalReconstruction::ProcessEvent(TMCVEvent* tEvent,TMCEvent* tMCEvent)
{
  if (fVerbose>2) printf("ECalReconstruction::ProcessEvent - Reconstructing MC event\n");
}

void ECalReconstruction::ProcessEvent(TECalRecoEvent* tEvent,TRecoEvent* tRecoEvent)
{
  if (fVerbose>2) printf("ECalReconstruction::ProcessEvent - Reprocessing reconstructed event\n");
}

void ECalReconstruction::ProcessEvent(TRawEvent* rawEvent)
{

  if (fVerbose>2) printf("ECalReconstruction::ProcessEvent - Reconstructing RawData event\n");

  // Show run energy (only when run changes)
  static Int_t run = 0;
  if (fRunConditionSvc->GetCurrentRun() != run) {
    run = fRunConditionSvc->GetCurrentRun();
    if (fVerbose) printf("ECalReconstruction::ProcessEvent - Run %d has energy %.3f\n",run,fRunConditionSvc->GetRunEnergy());
  }

  fHits.clear();
  fClusters.clear();

  fECalDigitizer->BuildHits(rawEvent,fHits);
 
  fECalClusterizer->BuildClusters(fHits,fClusters);

}

void ECalReconstruction::EndProcessing()
{
  if (fVerbose) printf("ECalReconstruction::EndProcessing - Finalizing ECal reconstruction\n");
}
