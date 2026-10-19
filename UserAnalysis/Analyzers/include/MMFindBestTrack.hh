#ifndef MMFindBestTrack_h
#define MMFindBestTrack_h 1

#include "utlConfigParser.hh"
#include "PadmeAnalysisEvent.hh"
#include "HistoSvc.hh"
#include "MMClustering.hh"
#include "GeneralInfo.hh"

struct MMTrack{
  int view  = -999;   //y=0, x=1
  int quad  = -999;   //0,1,2,3
  int level = -999;  //0 -> one segment per plane, 1-> two segments per plane

  int nhit  = -999;
  MMTracklet tracklet;

  void Initialize() {
    view = -999;
    quad = -999;
    level = -999;
  }
};

struct MMBestTrack{

  int view  = -999;   //y=0, x=1
  int quad  = -999;   //0,1,2,3
  int level = -999;  //0 -> one segment per plane, 1-> two segments per plane
  int nhit  = -999;
  Double_t slope; // dv/dz
  Double_t inter; // v at mesh plane
  Double_t errSlope = 0.010; //mrad
  Double_t errInter = 0.300; //mm
  Double_t slope_lvl0[2];
  Double_t inter_lvl0[2];
  Double_t chi2;  // if fit is done, otherwise it is a nominal value [-999]
  Double_t chi2IP;
  Double_t pchi2;
  Double_t pars[5];// x0,y0,x1,y1,dz: for mode = 0, fit x0,x1 or y0,y1 depending on the view and fix the other pair of parameters
  TVector3 lambda; // cosines of track directions
  vector<TVector3> vres; //vector of residuals
  Double_t purity=-999; //default, assigned only when calling track-clupos association
  TVector3 BestTrackExtrapolationAtZ(double z_in) {
    TVector3 ext_pos;

    ext_pos.SetX(pars[0]+lambda.X()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetY(pars[1]+lambda.Y()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
    ext_pos.SetZ(z_in);
    
    return ext_pos;
  }
};


struct MMBestTrack3D{
  MMBestTrack* tracks[2]={nullptr, nullptr}; //one per view y=0, x=1
  // Double_t slope; // dv/dz
  // Double_t inter; // v at mesh plane
  // Double_t chi2;  // if fit is done, otherwise it is a nominal value [-999]
  // Double_t chi2IP;
  // Double_t pchi2;
  // Double_t pars[5];// x0,y0,x1,y1,dz: for mode = 0, fit x0,x1 or y0,y1 depending on the view and fix the other pair of parameters
  // TVector3 lambda; // cosines of track directions
  // vector<TVector3> vres; //vector of residuals

  Double_t purity=0; //default, assigned only when calling track-clupos association

  // TVector3 BestTrackExtrapolationAtZ(double z_in) {
  //   TVector3 ext_pos;

  //   ext_pos.SetX(pars[0]+lambda.X()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
  //   ext_pos.SetY(pars[1]+lambda.Y()/lambda.Z()*(z_in-GeneralInfo::GetInstance()->GetMMPosPlaneZ(0)));
  //   ext_pos.SetZ(z_in);
    
  //   return ext_pos;
  // }
};

class MMFindBestTrack {

public:

  ~MMFindBestTrack();
  static MMFindBestTrack* GetInstance();
  Bool_t Init(PadmeAnalysisEvent* event);
  Bool_t Process();
  Bool_t Finalize();
  vector<MMBestTrack*> GetVectorTracks() {return fvTracks;};
  MMBestTrack* GetTrack(int i) {if(i>=0 || i<(int)fvTracks.size()) return fvTracks.at(i); return nullptr;};
  Int_t GetNTracks() {return (int) fvTracks.size();};
  MMBestTrack3D* GetBestTrackFromCluPos(TVector3 cluPos, Double_t extrZ); //
  Double_t PurityFunc(Double_t x, Double_t y, Double_t *p);
  Double_t PurityFunc_4HitL1(Double_t x, Double_t *p);
  Double_t PurityFunc_LowMultiplicity(Double_t x, Double_t *p);
  MMBestTrack3D* GetBestTrackFromCluID(Int_t cluId); //implemented but commented for now --> to use it remove comments in cc and in Process()
  void AssignPurity(MMBestTrack* track, TVector3 cluPos,Double_t extrZ);//
  
  Int_t QualityBin(Double_t purity, Int_t type) {
    //        purity_TAG < 0      --> quality_bin_TAG = 0 (no track considered)
    //    0 < purity_TAG < 0.02   --> quality_bin_TAG = 1 (bad purity)
    // 0.02 < purity_TAG < 0.2    --> quality_bin_TAG = 2
    //  0.2 < purity_TAG < 0.6    --> quality_bin_TAG = 3
    //  0.6 < purity_TAG < 1    --> quality_bin_TAG = 4
    //        purity_PROBE < 0      --> quality_bin_PROBE = 0 (no track considered)
    //    0 < purity_PROBE < 0.02   --> quality_bin_PROBE = 1 (bad purity)
    // 0.02 < purity_PROBE < 0.2    --> quality_bin_PROBE = 2
    //  0.2 < purity_PROBE < 0.6    --> quality_bin_PROBE = 3
    //  0.6 < purity_PROBE < 1    --> quality_bin_PROBE = 4
    
    if(type == 0) { //TAG
      if(purity < 0)     return 0;
      if(purity < 0.02)  return 1;
      if(purity < 0.2)   return 2;
      if(purity < 0.6)   return 3;
      return 4;
    }
    else { //PROBE
      if(purity < 0)     return 0;
      if(purity < 0.02)  return 1;
      if(purity < 0.2)   return 2;
      if(purity < 0.6)   return 3;
      return 4;
    }
  };

private:
  static MMFindBestTrack* fInstance;
  GeneralInfo* fGeneralInfo;
  MMClustering* fMMClusteringInstance;
  //Bool_t InitHistos(Int_t)

  Int_t fNRun;
  Bool_t fHistoMode;
  PadmeAnalysisEvent* fEvent;
  utl::ConfigParser* fCfgParser;
  HistoSvc* fHS;
  Int_t fVerbose;
  TString InputHistofile;
  TString InputHistofileName;
  int fEventCounter;
  bool fSaveEvent;
  vector<MMBestTrack*> fvTracks;
  std::map<Int_t, MMBestTrack3D*> fCluTrackAss;
  //purity fit parameters

  double pars_3h_lvl0[8] = {7.56008, 58.3746, -1.2901, 7.28396, 16.0513, 105.037, 3.37634, 0.263186};

  double pars_4h_lvl0[8] = {0.0265705, 92.0291, -1.24075, 6.90274, 19.2264, 104.307, 8.19723, 1.84315};

  double pars_4h_lvl1[6] = {1819.62, 0.294837, 9.89586, 1664.43, 10.6183, 83.4185};
  double err_pars_4h_lvl1[6] = {16.1599, 0.093497, 0.084371, 4.18318, 0.142544, 0.102222};
  
  double pars_5h_lvl1[8] = {18.5897, 34.5641, 0.906092, 11.324, 5.74614, 87.1964, 2.09614, 0.914284};

  double pars_6ph_lvl1[8] = {204.73, 306.454, 1.26521, 11.1982, 4.69843, 99.2033, -0.781994, 4.14681};


  Double_t pars_3h_lvl0_LM[10][7] = {{34.2824170022,-3.3375133199,8.63104355492,14.0794283237,19.596774537,89.9977514448,16.638837311},
				     {51.0092752192,-3.92812210013,7.47634319726,42.5223678457,25.8380101447,129.623608061,37.0447161353},
				     {12.3804352995,-8.3916704738,-13.8876078958,23.9979545167,20.0238962611,-82.1068927211,28.416312299},
				     {24.8490085527,-2.92653340769,12.4521629867,72.770673016,14.8683564272,-87.170061497,82.688671922},
				     {0.,10.108934761,-80.0440208314,158.992582069,1887.35802655,27.5476840622,166.178672138},
				     {136.316553168,-5.4355788947,6.32238462856,48.2412710089,8.39006792823,-76.0773009853,54.4369583137},
				     {155.170101159,-5.04557335242,6.36882443457,19.8324002593,2.71376723763,-77.9593412294,24.2464496748},
				     {229.748733428,-5.03815149309,6.35970956618,15.3777933428,-2.11723504476,-67.4114781273,17.0488957155},
				     {336.068586626,-5.34251313362,6.22798321462,13.9306783141,0.753851527807,71.4429968437,16.0799029608},
				     {439.96904651,-5.29733139428,6.40061460336,17.1942879986,4.12567538299,62.8585367102,20.4389514968}};
  Double_t pars_4h_lvl0_LM[10][7] = {{39.6853862761,-3.24810616386,10.8083739565,34.5546576064,28.7460299906,122.737348552,30.4951219153},
				     {15.5852584989,-3.29552110779,8.79785927925,15.8158961252,16.2551751731,74.8197153629,17.0908899372},
				     {0,-18.9972142086,0.00041616885639,45.644695544,11.6642826137,-74.0934519288,44.9469261339},
				     {0,1.86712424763e+13,8.4,40.0731655448,8.57656244168,63.3369012365,36.0955305612},
				     {9.68788953906,1.92098363117,-9.82028336339,18.1542242172,15.0589465417,-76.3586132379,20.610996518},
				     {0,-82.2869211658,91.8187828716,121.587626065,-73.5528942375,102.289141014,9.93183081216},
				     {16.852092807,-6.67116439504,6.67344186512,7.97365240827,3.27455089677,89.493822922,8.82292063462},
				     {22.6412620802,-5.16406762308,7.47215765066,9.83854739719,-20.3148698682,153.501681315,2.78961267662},
				     {41.2256264533,-5.29290655513,6.32727573695,5.73770903196,-7.01820188963,96.4021173542,5.20640839811},
				     {163.596880694,-5.16742379526,7.37633648535,5.78164143733,12.0218502994,-100.482336715,9.23973846058}};
  Double_t pars_4h_lvl1_LM[10][7] = {{82.9682363794,0.529571704858,8.28066942458,9.65373304218,16.4099471841,83.4504008914,5.79735943008},
				     {97.2726958135,-0.425336053372,7.15973788396,4.61705578978,13.6921491062,-61.829490641,2.94836090946},
				     {99.0842641367,-1.00350273927,8.33042544836,4.06565114723,8.02316806956,74.8963741554,2.79796395489},
				     {90.8591661102,-2.55076001142,8.16393368821,3.81580554287,1.7839905562,61.7753184806,2.3377222},
				     {85.4280126921,-2.29331072898,9.02808523884,2.10637396683,-59.0971282714,15.2119332126,2.51394950269},
				     {84.1487036569,-3.22793346124,8.89627130152,2.15741280196,24.6122123901,-71.0132114007,2.06008233879},
				     {89.9631406045,-3.11576333673,8.86478624854,2.32449828204,19.4228430606,-61.8671343809,2.09950639396},
				     {84.5087896121,-3.26395857548,9.18419882523,2.33315471699,-134.768525677,-2.03995221707,2.30704595778},
				     {61.0651757285,-1.91696463512,8.37600357909,0.834179274265,-14.4190295417,-67.3231623005,1.8660306517},
				     {18.6566900421,-1.70799786634,8.48533895884,0.546860918816,56.4324845175,-28.8884072747,1.24997710674}};
  Double_t pars_5h_lvl1_LM[10][7] = {{30.3152598326,-2.08226102036,8.83104205804,1.92076590478,-6.61564883819,-254.029468439,0.300814889754},
				     {29.3996217484,-0.512414896406,9.26165438509,3.11446113943,8.2824131822,73.7138966155,2.50744640388},
				     {19.2434761887,-1.78841761263,10.1236950614,2.14220095267,39.4288371411,52.1899905635,2.15677287478},
				     {34.3067087039,-1.6559985253,8.1429767104,7.04912639044,11.8546787739,72.6657676975,4.13710187839},
				     {94.2888746334,-1.44055688217,7.7274213115,11.386973315,0.984113841036,79.6243943286,6.78187362769},
				     {90.1439057035,-1.42523114464,8.29663621674,3.97346100717,6.62943446882,-31.6668276832,2.51674189238},
				     {93.5242280338,-1.13998900293,8.2647187574,2.20181996033,7.37007649076,41.9215114267,1.65183636467},
				     {135.994306366,-1.58922881872,8.51852356275,1.01909688043,1.09862070452,60.1398929189,1.67442607769},
				     {173.420526211,-2.03564848307,9.07716681,3.96991298941,7.36365768318,38.6092660321,1.87320739984},
				     {212.995575561,-2.07346191898,9.30323535737,2.69937207988,-27.2670016945,51.1870030711,2.41182871823}};
  Double_t pars_6ph_lvl1_LM[10][7] = {{100.066909796,-1.82688954473,9.43559759305,16.8570344,5.59636725775,86.8153341425,10.3424926347},
				      {32.7744211837,-0.501886283866,8.97207378558,7.31799919129,12.5935705621,66.9799926535,3.85325605324},
				      {55.1787826004,-0.783149279536,9.34282258915,12.478477282,-43.8834380943,155.216142416,3.89091110121},
				      {58.4630429131,-0.804306653593,9.06424977113,9.53372979694,3.84616149051,71.966380648,5.60064705565},
				      {82.7751157197,-1.10630674684,9.04359270158,7.11734569725,-3.13517593627,78.7987112,3.35140894638},
				      {68.8657391125,-1.62175530294,9.51686061279,2.85912716252,17.1473109851,52.7165844319,1.98320071573},
				      {79.8193150056,-1.69052237552,9.3616779026,1.58461919099,1.53192166436,55.1375858334,1.72102734221},
				      {120.254135543,-1.57222026107,9.15775257271,1.08852379649,-25.4346494332,69.4118383496,1.56854053837},
				      {160.070259446,-1.86945677441,10.2335543778,1.7199485477,-24.7968674601,55.8270415575,1.88278658001},
				      {185.462550575,-1.7656079578,11.0714678512,1.65746745473,-67.3530449174,-80.4176554738,2.73619468875}};
};
#endif
