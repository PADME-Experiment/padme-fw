#include "NPoTAnalysis.hh"

NPoTAnalysis::NPoTAnalysis(TString cfgFile, Int_t verbose)
{
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating NPoTAnalysis\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fRndm = new TRandom3();
}

NPoTAnalysis::~NPoTAnalysis(){
  delete fCfgParser;
  delete fRndm;
}

Bool_t NPoTAnalysis::Init(PadmeAnalysisEvent* event){
  if (fVerbose) printf("---> Initializing NPoTAnalysis\n");
  fEvent = event;
  InitHistos();
  return true;
}

Bool_t NPoTAnalysis::InitHistos(){
  // NPoTAnalysis directory will contain all histograms related to this analysis
  fHS->CreateList("NPoTAnalysis");
  fHS->BookHistoList("NPoTAnalysis","NPoT",500,0.,50000.);
  return true;
}

Bool_t NPoTAnalysis::Process(){
  fNPoT = fRndm->Gaus(25000.,2000.);
  fHS->FillHistoList("NPoTAnalysis","NPoT",fNPoT);
  return true;
}

Bool_t NPoTAnalysis::Finalize()
{
  if (fVerbose) printf("---> Finalizing NPoTAnalysis\n");
  return true;
}
