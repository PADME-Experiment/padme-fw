#include "PadmeAnaInMCHistoEvent.hh"


PadmeAnaInMCHistoEvent::PadmeAnaInMCHistoEvent( TTree *tree ){
  evType = MCHisto;
  fMCHistoTree = tree;
  fEntry =0;

     // BEGIN SET ADDRESS
  fMCHistoTree->SetBranchAddress("Nevent", &Nevent, &b_Nevent);
  fMCHistoTree->SetBranchAddress("ETot", &ETot, &b_ETot);
  fMCHistoTree->SetBranchAddress("IDProc", &IDProc, &b_IDProc);
  fMCHistoTree->SetBranchAddress("PBeam", &PBeam, &b_PBeam);
  fMCHistoTree->SetBranchAddress("PPrim", &PPrim, &b_PPrim);
  fMCHistoTree->SetBranchAddress("XBeam", &XBeam, &b_XBeam);
  fMCHistoTree->SetBranchAddress("YBeam", &YBeam, &b_YBeam);
  fMCHistoTree->SetBranchAddress("NClusters", &NClusters, &b_NClusters);
  fMCHistoTree->SetBranchAddress("NTracks", &NTracks, &b_NTracks);
  fMCHistoTree->SetBranchAddress("NHEPVetoTracks", &NHEPVetoTracks, &b_NHEPVetoTracks);
  fMCHistoTree->SetBranchAddress("NPVetoTracks", &NPVetoTracks, &b_NPVetoTracks);
  fMCHistoTree->SetBranchAddress("NEVetoTracks", &NEVetoTracks, &b_NEVetoTracks);
  fMCHistoTree->SetBranchAddress("NSAC", &NSAC, &b_NSAC);
  fMCHistoTree->SetBranchAddress("NCal", &NCal, &b_NCal);
  fMCHistoTree->SetBranchAddress("NLAV", &NLAV, &b_NLAV);
  fMCHistoTree->SetBranchAddress("NTarget", &NTarget, &b_NTarget);
  fMCHistoTree->SetBranchAddress("ESAC", ESAC, &b_ESAC);
  fMCHistoTree->SetBranchAddress("TSAC", TSAC, &b_TSAC);
  fMCHistoTree->SetBranchAddress("PTypeSAC", PTypeSAC, &b_PTypeSAC);
  fMCHistoTree->SetBranchAddress("XSAC", XSAC, &b_XSAC);
  fMCHistoTree->SetBranchAddress("YSAC", YSAC, &b_YSAC);
  fMCHistoTree->SetBranchAddress("SACCh", SACCh, &b_SACCh);
  fMCHistoTree->SetBranchAddress("EPartCal", EPartCal, &b_CalE);
  fMCHistoTree->SetBranchAddress("TPartCal", TPartCal, &b_CalT);
  fMCHistoTree->SetBranchAddress("PTypePartCal", PTypePartCal, &b_CalPType);
  fMCHistoTree->SetBranchAddress("XPartCal", XPartCal, &b_CalX);
  fMCHistoTree->SetBranchAddress("YPartCal", YPartCal, &b_CalY);
  fMCHistoTree->SetBranchAddress("ECluster", ECluster, &b_ECluster);
  fMCHistoTree->SetBranchAddress("QCluster", QCluster, &b_QCluster);
  fMCHistoTree->SetBranchAddress("XCluster", XCluster, &b_XCluster);
  fMCHistoTree->SetBranchAddress("YCluster", YCluster, &b_YCluster);
  fMCHistoTree->SetBranchAddress("ThCluster", ThCluster, &b_ThCluster);
  fMCHistoTree->SetBranchAddress("M2Cluster", M2Cluster, &b_M2Cluster);
  fMCHistoTree->SetBranchAddress("TCluster", TCluster, &b_TCluster);
  fMCHistoTree->SetBranchAddress("NClusCells", NClusCells, &b_NClusCells);
  fMCHistoTree->SetBranchAddress("ETarget", &ETarget, &b_ETarget);
  fMCHistoTree->SetBranchAddress("TTarget", &TTarget, &b_TTarget);
  fMCHistoTree->SetBranchAddress("XTarget", &XTarget, &b_XTarget);
  fMCHistoTree->SetBranchAddress("YTarget", &YTarget, &b_YTarget);
  fMCHistoTree->SetBranchAddress("HEPVetoTrEne", HEPVetoTrEne, &b_NTHEPVetoTrkEne);
  fMCHistoTree->SetBranchAddress("HEPVetoNFing", HEPVetoNFing, &b_NTHEPVetoTrkFinger);
  fMCHistoTree->SetBranchAddress("HEPVetoTrTime", HEPVetoTrTime, &b_NTHEPVetoTrkTime);
  fMCHistoTree->SetBranchAddress("HEPVetoFingE", HEPVetoFingE, &b_NTHEPVetoFingE);
  fMCHistoTree->SetBranchAddress("HEPVetoX", HEPVetoX, &b_NTHEPVetoX);
  fMCHistoTree->SetBranchAddress("HEPVetoY", HEPVetoY, &b_NTHEPVetoY);
  fMCHistoTree->SetBranchAddress("PVetoTrEne", PVetoTrEne, &b_NTPVetoTrkEne);
  fMCHistoTree->SetBranchAddress("PVetoNFing", PVetoNFing, &b_NTPVetoTrkFinger);
  fMCHistoTree->SetBranchAddress("PVetoTrTime", PVetoTrTime, &b_NTPVetoTrkTime);
  fMCHistoTree->SetBranchAddress("PVetoFingE", PVetoFingE, &b_NTPVetoFingE);
  fMCHistoTree->SetBranchAddress("PVetoX", PVetoX, &b_NTPVetoX);
  fMCHistoTree->SetBranchAddress("PVetoY", PVetoY, &b_NTPVetoY);
  fMCHistoTree->SetBranchAddress("EVetoTrEne", EVetoTrEne, &b_NTEVetoTrkEne);
  fMCHistoTree->SetBranchAddress("EVetoNFing", EVetoNFing, &b_NTEVetoTrkFinger);
  fMCHistoTree->SetBranchAddress("EVetoTrTime", EVetoTrTime, &b_NTEVetoTrkTime);
  fMCHistoTree->SetBranchAddress("EVetoFingE", EVetoFingE, &b_NTEVetoFingE);
  fMCHistoTree->SetBranchAddress("EVetoX", EVetoX, &b_NTEVetoX);
  fMCHistoTree->SetBranchAddress("EVetoY", EVetoY, &b_NTEVetoY);
      
}



int PadmeAnaInMCHistoEvent::NextEvent(){
  return fMCHistoTree->GetEntry(fEntry++);
}


PadmeAnaInMCHistoEvent::~PadmeAnaInMCHistoEvent(){
  ;
}
