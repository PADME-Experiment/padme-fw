#include "Riostream.h"

#include "PadmeVRecoConfig.hh"

#include "ETagDigitizer.hh"

ETagDigitizer::ETagDigitizer(PadmeVRecoConfig* cfg)
{
  // Attach ETag configuration service
  fETagConfig = cfg;

  // Define verbose level
  fVerbose = fETagConfig->GetParOrDefault("Output", "Verbose", 0);

  if (fVerbose) printf("ETagDigitizer::ETagDigitizer - ETag digitization system created\n");
}

ETagDigitizer::~ETagDigitizer()
{
  if (fVerbose) printf("ETagDigitizer::~ETagDigitize - Deleting ETag digitization system\n");
}

void ETagDigitizer::Init()
{
  if (fVerbose) printf("ETagDigitizer::Init - Initilizing ETag digitizer\n");
}
