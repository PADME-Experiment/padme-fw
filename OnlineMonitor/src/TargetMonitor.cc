#include <stdio.h>

#include "TargetMonitor.hh"

#include "Configuration.hh"

#include "TMath.h"

TargetMonitor::TargetMonitor(utl::ConfigParser* cfgParser)
{

  // Connect to configuration class
  fConfig = Configuration::GetInstance();

  // Connect to analysis configuration parser
  fConfigParser = cfgParser;

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
