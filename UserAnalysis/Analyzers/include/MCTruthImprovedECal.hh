#ifndef MCTruthImprovedECal_h
#define MCTruthImprovedECal_h 1

#include <TRandom3.h>
#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "GeneralInfo.hh"

#include "HistoSvc.hh"
#include <map>

struct MatchingStruct{ // to be used with originalvtxid as key 
  Int_t originalvtxid;
  Int_t cluid;
  Int_t originalpcleout;
  Int_t ecalvtxid;
  Int_t type;
};

class MCTruthImprovedECal 
{

protected:
  //  MCTruthImprovedECal();//??
  MCTruthImprovedECal(TString c = "config/UserAnalysis.conf", Int_t v = 0);
public:
  ~MCTruthImprovedECal();
  static MCTruthImprovedECal* GetInstance();


  Bool_t Init(PadmeAnalysisEvent*);
  Bool_t Process();
  Bool_t Finalize();
  Bool_t CorrelateVtxClu();
  Bool_t TagAndProbeFromVertex();

  void insertMatching(Int_t ecalvtxid, Int_t cluid, Int_t originalvtxid, Int_t originalpcleout, Int_t type){
    // fill matching struct
    MatchingStruct mstruct;
    mstruct.originalvtxid = originalvtxid;
    mstruct.originalpcleout = originalpcleout;
    mstruct.cluid = cluid;
    mstruct.ecalvtxid = ecalvtxid;
    mstruct.type = type;

    // find vector of matching structs given vtx, add to vector of matching structs
    auto itvtx = VtxKeyMap.find(originalvtxid);
    if (itvtx == VtxKeyMap.end()) {
      std::vector<MatchingStruct> VtxKeyMapValue;
      VtxKeyMapValue.push_back(mstruct);
      std::pair<Int_t,std::vector<MatchingStruct>> vtxmapval = {originalvtxid,VtxKeyMapValue};
      VtxKeyMap.insert(vtxmapval);
    } else {
      itvtx->second.push_back(mstruct);
    }    
    // find vector of matching structs given cluid, add to vector of matching structs
    auto itclu = CluKeyMap.find(cluid);
    if (itclu == CluKeyMap.end()) {
      std::vector<MatchingStruct> CluKeyMapValue;
      CluKeyMapValue = itclu->second;
      CluKeyMapValue.push_back(mstruct);
      std::pair<Int_t,std::vector<MatchingStruct>> clumapval = {cluid,CluKeyMapValue};
      CluKeyMap.insert(clumapval);
    } else{
      itclu->second.push_back(mstruct);
    }

    // find vector of matching structs given vtx, add to vector of matching structs
    std::pair<Int_t,Int_t> pairkey = {originalvtxid,originalpcleout};
    auto itvtxpclepair = VtxPclePairKeyMap.find(pairkey);
    if (itvtxpclepair == VtxPclePairKeyMap.end()) {
      std::vector<MatchingStruct> VtxPclePairKeyMapValue;
      VtxPclePairKeyMapValue = itvtxpclepair->second;
      VtxPclePairKeyMapValue.push_back(mstruct);
      std::pair<std::pair<Int_t,Int_t>,std::vector<MatchingStruct>> vtxpclepairmapval = {pairkey,VtxPclePairKeyMapValue};
      VtxPclePairKeyMap.insert(vtxpclepairmapval);
    } else {
      itvtxpclepair->second.push_back(mstruct);
    }
  }
  
  Int_t GetVtxFromCluID(Int_t cluid){ //returns a sigle vertex associated to a cluster
    auto itclu = CluKeyMap.find(cluid);
    if (itclu == CluKeyMap.end()) return -1;
    std::vector<MatchingStruct> CluKeyMapValue = itclu->second;
    for (int i=0; i < (int) CluKeyMapValue.size(); i++){
      MatchingStruct mstruct = CluKeyMapValue.at(i);
      return mstruct.originalvtxid;
    }
    return -1;
  }
  
  Int_t GetECalVtxFromCluID(Int_t cluid){ //returns a sigle ECal vertex associated to a cluster
    auto itclu = CluKeyMap.find(cluid);
    if (itclu == CluKeyMap.end()) return -1;
    std::vector<MatchingStruct> CluKeyMapValue = itclu->second;
    for (int i=0; i < (int) CluKeyMapValue.size(); i++){
      MatchingStruct mstruct = CluKeyMapValue.at(i);
      return mstruct.ecalvtxid;
    }
    return -1;
  }

  std::vector<Int_t> GetClusFromVtx(Int_t vtxid){ //returns the list of clusters associated to an original-type vertex
    std::vector<Int_t> clusarray;
    auto itvtx = VtxKeyMap.find(vtxid);
    if (itvtx == VtxKeyMap.end()) return {}; // empty vector

    std::vector<MatchingStruct> VtxKeyMapValue = itvtx->second;
    std::map <Int_t, Int_t> clustersfound;
    
    for (int i=0; i < (int) VtxKeyMapValue.size(); i++){
      MatchingStruct mstruct = VtxKeyMapValue.at(i);
      int cluid = mstruct.cluid;
      auto itclu = clustersfound.find(cluid);
      if (itclu == clustersfound.end()) { // first time this cluster appears
	clusarray.push_back(cluid);
      } 
    }
    return clusarray; // vector with clusters (no repetitions)
  }

  std::vector<Int_t> GetClusFromVtxPclePair(Int_t vtxid, Int_t pcleid){ //returns the list of clusters associated to a vertex and a particle out from that vertex
    std::vector<Int_t> clusarray;
    std::pair<Int_t,Int_t> pairin = {vtxid,pcleid};
    auto itpair = VtxPclePairKeyMap.find(pairin);
    if (itpair == VtxPclePairKeyMap.end()) return {}; // empty vector

    std::vector<MatchingStruct> VtxPclePairKeyMapValue = itpair->second;
    std::map <Int_t, Int_t> clustersfound;
    
    for (int i=0; i < (int) VtxPclePairKeyMapValue.size(); i++){
      MatchingStruct mstruct = VtxPclePairKeyMapValue.at(i);
      int cluid = mstruct.cluid;
      auto itclu = clustersfound.find(cluid);
      if (itclu == clustersfound.end()) { // first time this cluster appears
	clusarray.push_back(cluid);
	clustersfound.insert(std::pair{cluid,1});
      } 
    }
    return clusarray; // vector with clusters (no repetitions)
  }


  void DumpMapWithVtxKey(){
    std::cout <<"MCTruthImprovedECal: Dump map with vtxid as key: size " << VtxKeyMap.size() << std::endl;
    std::map<Int_t, std::vector<MatchingStruct>>::iterator it;
    for (it = VtxKeyMap.begin(); it != VtxKeyMap.end(); it++){
      std::cout << "Key = " << it->first << " has " << it->second.size() << " entries " << std::endl;
      for (int it2 = 0; it2 < (int) it->second.size(); it2++){
	std::cout << "entry " << it2 <<
	  " content " << it->second.at(it2).originalvtxid <<
	  " cluid " << it->second.at(it2).cluid <<
	  " originalpcluout " << it->second.at(it2).originalpcleout <<
	  " ecalvtxid " << it->second.at(it2).ecalvtxid << " type " << it->second.at(it2).type << std::endl;
      }
    }
  }

private:
  static MCTruthImprovedECal* fInstance; 
  Bool_t InitHistos();
  Int_t fVerbose;

  GeneralInfo* fGeneralInfo;
  Int_t fpdgcodes[3];//e+,e-,gamma
  Int_t fCellMap[4000]={-1};
  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;

  std::map<Int_t, std::vector<MatchingStruct>> VtxKeyMap; //vertex, MatchingStruct vector pair
  std::map<Int_t, std::vector<MatchingStruct>> CluKeyMap; //cluster, MatchingStruct vector pair
  std::map<std::pair<Int_t,Int_t>, std::vector<MatchingStruct> > VtxPclePairKeyMap; //{vtx,pcle}, MatchingStruct pair


  std::map<TString,int> fProcessIDsMap; // Map of the processName, internal index of the process
  Double_t fXMin;
  Double_t fXMax;
  Double_t fXW;
  Int_t fNXBins;
  Double_t fYMin;
  Double_t fYMax;
  Double_t fYW;
  Int_t fNYBins;
  TH2D *hBrem, *heIoni, *hAnnihil;
  const double cellSize = 21+0.12;//mm + crystal gap                                                                                                                                                                        
  const int ncells = 29; // per row or column                                                                                                                                                            
  double ecalEdge = (ncells/2+0.5)*cellSize; // 304.5mm                                                                                                                                                  
  const int nhole = 5; // 5x5 matrix is not instrumented                  

  TRecoVClusCollection* fECal_clEvent;
  TRecoEvent*           fRecoEvent;

  const int NPcles = 10;


};
#endif
