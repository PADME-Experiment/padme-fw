#ifndef ETAGSIMPLE_CLUSTERIZATION_H
#define ETAGSIMPLE_CLUSTERIZATION_H

#include "ChannelVReco.hh"
#include "utlConfigParser.hh"

#include "TH1.h"
#include "TFile.h"
#include "TH1.h"
#include "PadmeVClusterization.hh"

class TRecoVHit;
class TRecoVCluster;

class ETagSimpleClusterization : public PadmeVClusterization {
public:
  ETagSimpleClusterization(){;};
  virtual ~ETagSimpleClusterization(){;};

  virtual void Init(PadmeVRecoConfig *cfg);
  virtual void HistoInit(); 
  virtual void HistoExit();
  virtual void AddHisto(std::string name, TH1 *histo);
  virtual TH1* GetHisto(std::string);
  TFile* GetHistoFile() { return fHistoFile; };
  virtual void PrintConfig();
  virtual void Reconstruct(std::vector<TRecoVHit *> &hitArray, std::vector<TRecoVCluster *> &clArray);
  std::vector<std::vector<TRecoVHit*>> FindClustersByTimeSpread(std::map<int, std::vector<TRecoVHit*>> hitClusterMap);
  void putClustersIntoEvent(std::vector<TRecoVHit *> &Hits, std::vector<TRecoVCluster *> &myClusters, std::map<int, std::vector<int>> clusterHitsIdx, bool leftOrRight);
  std::vector<TRecoVHit *> removeOutOfTimeHits(std::vector<TRecoVHit *> &Hits);
  
protected:
  // configuration parameters
  Double_t fClusterDtMax;
  Int_t    fClusterDcellMax;
  Double_t fClusterEthrForHit;
  Double_t fClusterTLowForHit; 
  Double_t fClusterTHighForHit;

private:

  std::map<int, int> HitToClusterMapLeft = {
    {0, 0}, {1, 0}, {2, 0}, {3, 0},
    {100, 1}, {101, 1}, {102, 1}, {103, 1},
    {200, 2}, {201, 2}, {202, 2}, {203, 2},
    {300, 3}, {301, 3}, {302, 3}, {303, 3},
    {400, 4}, {401, 4}, {402, 4}, {403, 4},
    {500, 5}, {501, 5}, {502, 5}, {503, 5},
    {600, 60}, {601, 60}, {602, 60}, {603, 60},
    {700, 70}, {701, 70}, {702, 70}, {703, 70},
    {800, 80}, {801, 80}, {802, 80}, {803, 80},
    {900, 9}, {901, 9}, {902, 9}, {903, 9},
    {1000, 10}, {1001, 10}, {1002, 10}, {1003, 10},
    {1100, 11}, {1101, 11}, {1102, 11}, {1103, 11},
    {1200, 12}, {1201, 12}, {1202, 12}, {1203, 12},
    {1300, 13}, {1301, 13}, {1302, 13}, {1303, 13},
    {1400, 14}, {1401, 14}, {1402, 14}, {1403, 14},
  };

  std::map<int, int> HitToClusterMapRight = {
    {10, 0}, {11, 0}, {12, 0}, {13, 0},
    {110, 1}, {111, 1}, {112, 1}, {113, 1},
    {210, 2}, {211, 2}, {212, 2}, {213, 2},
    {310, 3}, {311, 3}, {312, 3}, {313, 3},
    {410, 4}, {411, 4}, {412, 4}, {413, 4},
    {510, 5}, {511, 5}, {512, 5}, {513, 5},
    {610, 61}, {611, 61}, {612, 61}, {613, 61},
    {710, 71}, {711, 71}, {712, 71}, {713, 71},
    {810, 81}, {811, 81}, {812, 81}, {813, 81},
    {910, 9}, {911, 9}, {912, 9}, {913, 9},
    {1010, 10}, {1011, 10}, {1012, 10}, {1013, 10},
    {1110, 11}, {1111, 11}, {1112, 11}, {1113, 11},
    {1210, 12}, {1211, 12}, {1212, 12}, {1213, 12},
    {1310, 13}, {1311, 13}, {1312, 13}, {1313, 13},
    {1410, 14}, {1411, 14}, {1412, 14}, {1413, 14},
  };

  TFile* fHistoFile;
  std::map<std::string,TH1 *> fHistoMap;
  
};
#endif
