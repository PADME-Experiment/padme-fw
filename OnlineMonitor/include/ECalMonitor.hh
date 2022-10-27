#ifndef ECalMonitor_H
#define ECalMonitor_H

#include "TH1D.h"
#include "TFile.h"
#include "TString.h"

#include "utlConfigParser.hh"

class Configuration;

class ECalMonitor
{

public:

  ECalMonitor(TString);
  ~ECalMonitor();

  void Initialize();
  void Finalize();
  void StartOfEvent();
  void EndOfEvent();
  void AnalyzeBoard(UChar_t);
  void AnalyzeChannel(UChar_t,UChar_t,Short_t*);

  // Methods to access local results for combined analysis
  Double_t GetTotalEventEnergy() { return fECTotalEventEnergy; }

private:

  void ComputeChannelCharge(Short_t*);
  void ComputeChannelEnergy(UChar_t,UChar_t);

  Int_t OutputBeam();
  Int_t OutputOffBeam();
  Int_t OutputCosmics();
  Int_t OutputRandom();

  void ResetECalMap(Double_t map[29][29]);

  Configuration* fConfig;

  // Board/Channel map of calibration constants
  Double_t   fECal_Calibration[29][32];

  utl::ConfigParser* fConfigParser;

  Bool_t fIsCosmics;
  Bool_t fIsBeam;
  Bool_t fIsOffBeam;
  Bool_t fIsRandom;

  UInt_t fBeamOutputRate;
  UInt_t fOffBeamOutputRate;
  UInt_t fCosmicsOutputRate;
  UInt_t fRandomOutputRate;

  UInt_t fBeamEventCount;
  UInt_t fOffBeamEventCount;
  UInt_t fCosmicsEventCount;
  UInt_t fRandomEventCount;

  // Define parameters for pedestal and total charge evaluation
  UInt_t fPedestalSamples;    // Number of samples to use for pedestals
  UInt_t fSignalSamplesStart; // Index of first sample of signal (included)
  UInt_t fSignalSamplesEnd;   // Index of last sample of signal (excluded)

  // Results of pedestal, total charge and total energy evaluation
  Double_t fChannelPedestal; // Pedestal level from the first fPedestalSamples samples
  Double_t fChannelPedRMS;   // Pedestal RMS
  Double_t fChannelCharge;   // Total charge between fSignalSamplesStart and fSignalSamplesEnd
  Double_t fChannelEnergy;   // Total charge between fSignalSamplesStart and fSignalSamplesEnd

  Double_t   fECal_CosmSum[29][29]; // 100 events map
  Double_t   fECal_CosmEvt[29][29]; // Last event map
  Double_t   fECal_BeamESum[29][29]; // Map with sum of energies
  Double_t   fECal_BeamEEvt[29][29]; // Map with energy of last event

  Double_t fECTotalEventEnergy; // Total energy in a single event
  TH1D* fHECTotEnergyBM; // Distribution of total energy in ECal in each event

  // Trend vectors
  std::vector<Double_t> fVECTimeBM;
  std::vector<Double_t> fVECTotEnergyBM;
 
  // Trend support file
  TString fTFECTrendsBM;

  // Map from [board][channel] to position as yyxx
  Short_t fECal_map[29][32] = {
    {1500,1600,1401,1501,1601,1701,1801,1901,1502,1602,1702,1802,1902,2002,2102,1403,1503,1603,1703,1803,1903,2003,2103,2203,2303,1504,1604,1704,1804,1904,2004,  -1} , // Board 0
    {2104,2204,2304,2404,1405,1505,1605,1705,1805,1905,2005,2105,2205,2305,2405,2505,1506,1606,1706,1806,1906,2006,2106,2206,2306,2406,2506,1407,1507,1607,1707,1807} , // Board 1
    {1907,2007,2107,2207,2307,2407,2507,2607,1508,1608,1708,1808,1908,2008,2108,2208,2308,2408,2508,2608,1409,1509,1609,1709,1809,1909,2009,2109,2209,2309,2409,2509} , // Board 2
    {  -1,2609,2709,1510,1610,1710,1810,1910,2010,2110,2210,2310,2410,2510,2610,2710,1411,1511,1611,1711,1811,1911,2011,2111,2211,2311,2411,2511,2611,2711,1712,1812} , // Board 3
    {  -1,1912,2012,2112,2212,2312,2412,2512,2612,2712,2812,1713,1813,1913,2013,2113,2213,2313,2413,2513,2613,2713,2813,1714,1814,1914,2014,2114,2214,2314,2414,2514} , // Board 4
    {2614,2714,2814,1715,1815,1915,2015,2115,2215,2315,2415,2515,2615,2715,2815,1716,1816,1916,2016,2116,2216,2316,2416,2516,2616,2716,2816,1517,1617,1717,1817,1917} , // Board 5
    {  -1,2017,2117,2217,2317,2417,2517,2617,2717,1418,1518,1618,1718,1818,1918,2018,2118,2218,2318,2418,2518,2618,2718,1519,1619,1719,1819,1919,2019,2119,2219,2319} , // Board 6
    {  -1,2419,2519,2619,2719,1420,1520,1620,1720,1820,1920,2020,2120,2220,2320,2620,2520,2420,1521,1621,1721,1821,1921,2021,2121,2221,2321,2421,2521,2621,1422,  -1} , // Board 7
    {1522,1622,1722,1822,1922,2022,2122,2222,2322,2422,2522,1523,1623,1723,1823,1923,2023,2123,2223,2323,2423,2523,1424,1524,1624,1724,1824,1924,2024,2124,2224,2324} , // Board 8
    {2424,1525,1625,1725,1825,1925,2025,2125,2225,2325,1426,1526,1626,1726,1826,1926,2026,2126,1527,1627,1727,1827,1927,1428,1528,1628,  -1,  -1,  -1,  -1,  -1,  -1} , // Board 9
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 10
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 11
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 12
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 13
    {1200,1300,1400, 901,1001,1101,1201,1301, 702, 802, 902,1002,1102,1202,1302,1402, 503, 603, 703, 803, 903,1003,1103,1203,1303, 404, 504, 604, 704, 804, 904,  -1} , // Board 14
    {1004,1104,1204,1304,1404, 305, 405, 505, 605, 705, 805, 905,1005,1105,1205,1305, 306, 406, 506, 606, 706, 806, 906,1006,1106,1206,1306,1406, 207, 307, 407, 507} , // Board 15
    { 607, 707, 807, 907,1007,1107,1207,1307, 208, 308, 408, 508, 608, 708, 808, 908,1008,1108,1208,1308,1408, 109, 209, 309, 409, 509, 609, 709, 809, 909,1009,1109} , // Board 16
    {  -1,1209,1309, 110, 210, 310, 410, 510, 610, 710, 810, 910,1010,1110,1210,1310,1410, 111, 211, 311, 411, 511, 611, 711, 811, 911,1011,1111,1211,1311,  12, 112} , // Board 17
    {  -1, 212, 312, 412, 512, 612, 712, 812, 912,1012,1112,  13, 113, 213, 313, 413, 513, 613, 713, 813, 913,1013,1113,  14, 114, 214, 314, 414, 514, 614, 714, 814} , // Board 18
    { 914,1014,1114,  15, 115, 215, 315, 415, 515, 615, 715, 815, 915,1015,1115,  16, 116, 216, 316, 416, 516, 616, 716, 816, 916,1016,1116, 117, 217, 317, 417, 517} , // Board 19
    {  -1, 617, 717, 817, 917,1017,1117,1217,1317,1417, 118, 218, 318, 418, 518, 618, 718, 818, 918,1018,1118,1218,1318, 119, 219, 319, 419, 519, 619, 719, 819, 919} , // Board 20
    {  -1,1019,1119,1219,1319,1419, 220, 320, 420, 520, 620, 720, 820, 920,1020,1120,1220,1320, 221, 321, 421, 521, 621, 721, 821, 921,1021,1121,1221,1321,1421,  -1} , // Board 21
    { 322, 422, 522, 622, 722, 822, 922,1022,1122,1222,1322, 323, 423, 523, 623, 723, 823, 923,1023,1123,1223,1323,1423, 424, 524, 624, 724, 824, 924,1024,1124,1224} , // Board 22
    {1324, 525, 625, 725, 825, 925,1025,1125,1225,1325,1425, 726, 826, 926,1026,1126,1226,1326, 927,1027,1127,1227,1327,1427,1228,1328,  -1,  -1,  -1,  -1,  -1,  -1} , // Board 23
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 24
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 25
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 26
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 } , // Board 27
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }   // Board 28
  };

};
#endif
