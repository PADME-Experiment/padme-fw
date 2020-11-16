#include <stdio.h>

#include "TargetMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

TargetMonitor::TargetMonitor(TString cfgFile)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = new utl::ConfigParser((const std::string)cfgFile);
  if (fConfig->Verbose()>0) fConfigParser->Print();

  // Initialize all counters
  Initialize();

}

TargetMonitor::~TargetMonitor()
{;}

void TargetMonitor::Initialize()
{;}

void TargetMonitor::StartOfEvent()
{;}

void TargetMonitor::EndOfEvent()
{;}

void TargetMonitor::Finalize()
{;}

void TargetMonitor::Analyze(UChar_t board,UChar_t channel,Short_t* samples)
{;}
