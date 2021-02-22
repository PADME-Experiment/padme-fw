#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>
#include <CLHEP/Units/SystemOfUnits.h>

#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include "Constants.hh"
#include "DatacardManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager* HistoManager::fInstance = 0;
HistoManager* HistoManager::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HistoManager(); }
  return fInstance;
}

HistoManager::HistoManager()
 :rootFile(0),ntupl(0)
{
  // histograms
  for (G4int k=0; k<MaxHisto; k++){ 
    histo[k] = 0;    
    histo2[k] = 0;    
  }
  // ntuple
  ntupl = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
  if ( rootFile ) delete rootFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::book()
{ 
 
  // Creating a tree container to handle histograms and ntuples.
  // This tree is associated to an output file.
  //
  G4String fileName = DatacardManager::GetInstance()->GetHistoFileName();
  rootFile = new TFile(fileName,"RECREATE");
  if(!rootFile) {
    G4cout << " HistoManager::book :"<<" problem creating the ROOT TFile "<< G4endl;
    return;
  }
  
  histo[1] = new TH1D("h1", "EtotCrys", 1100, 0., 11000*CLHEP::MeV);
  if (!histo[1]) G4cout << "\n can't create histo 1" << G4endl;
  histo[2] = new TH1D("h2", "EBeam", 900, 0., 900*CLHEP::MeV);
  if (!histo[2]) G4cout << "\n can't create histo 2" << G4endl;
  histo[3] = new TH1D("h3", "TBeam", 100, 0., 100.);
  if (!histo[3]) G4cout << "\n can't create histo 3" << G4endl;
  histo[4] = new TH1D("h4", "Microbunch", 500,0.,5.);
  if (!histo[4]) G4cout << "\n can't create histo 4" << G4endl;  
  histo[5] = new TH1D("h5", "beam angle",200,-0.01,0.01);
  if (!histo[5]) G4cout << "\n can't create histo 5" << G4endl;  
  histo[6] = new TH1D("h6", "NPrimary", 1000,0.,25000.);
  if (!histo[6]) G4cout << "\n can't create histo 6" << G4endl;  
  histo[7] = new TH1D("h7", "ECluster",1100,0.,1100.);
  if (!histo[7]) G4cout << "\n can't create histo 7" << G4endl;  
  histo[8] = new TH1D("h8", "Theta", 200,0.,10.);
  if (!histo[8]) G4cout << "\n can't create histo 8" << G4endl;  
  histo[9] = new TH1D("h9", "MMiss2", 475,-50.,900.);
  if (!histo[9]) G4cout << "\n can't create histo 9" << G4endl;  
  histo[10] = new TH1D("h10", "MMiss2 50MeV<ECl<400MeV Rad>3", 475,-50.,900.);
  if (!histo[10]) G4cout << "\n can't create histo 10" << G4endl;  
  histo[11] = new TH1D("h11", "NClusters", 30,-0.5,29.5);
  if (!histo[11]) G4cout << "\n can't create histo 9" << G4endl;  
  histo[12] = new TH1D("h12", "XCoord", 400,-10.,10.);
  if (!histo[12]) G4cout << "\n can't create histo 9" << G4endl;  
  histo[13] = new TH1D("h13", "YCoord", 400,-10.,10.);
  if (!histo[13]) G4cout << "\n can't create histo 13" << G4endl; 
  histo[14] = new TH1D("h14", "Primary interaction", 5,-0.5,4.5);
  if (!histo[14]) G4cout << "\n can't create histo 14" << G4endl; 
  histo[15] = new TH1D("h15", "Cluster Charge",1000.,0.,20000.);
  if (!histo[15]) G4cout << "\n can't create histo 15" << G4endl; 
  histo[16] = new TH1D("h16", "Massa 2g",1200.,0.,300.);
  if (!histo[16]) G4cout << "\n can't create histo 16" << G4endl; 
  // Additional histos for kinematics studies
  histo[17] = new TH1D("h17","Gamma emission angle in Uboson events",1000,0.,1.);
  if (!histo[17]) G4cout << "\n can't create histo 17" << G4endl;
   // SAC energy study
  histo[18] = new TH1D("h18","SAC energy",300,0.,600.);
  if (!histo[18]) G4cout << "\n can't create histo 18" << G4endl; 


  histo2[1] = new TH2D("h21", "XY cell weigh energy ",ECalNRow,-ECalSizeX*0.5,ECalSizeX*0.5,ECalNCol,-ECalSizeY*0.5,ECalSizeY*0.5);
  if (!histo2[1]) G4cout << "\n can't create histo 11" << G4endl;  
  histo2[2] = new TH2D("h22", "XY weigh energy ",600,-30., 30.,600,-30.,30.);
  if (!histo2[2]) G4cout << "\n can't create histo 11" << G4endl;  
  histo2[3] = new TH2D("h23", "nCell vs ETotCal",15,-0.5, 14.5,200,0.,100.);
  if (!histo2[3]) G4cout << "\n can't create histo 11" << G4endl;
  //histo2[4] = new TH2D("h24", "nCellCl vs ECluster",ECalNCells*0.5,-0.5, ECalNCells*0.5-0.5,BeamEnergy/2,0.,BeamEnergy);
  //if (!histo2[4]) G4cout << "\n can't create histo 11" << G4endl;
  histo2[5] = new TH2D("h25", "X Y cluster Pos ",ECalNRow*10,-ECalSizeX*0.5,ECalSizeX*0.5,ECalNRow*10,-ECalSizeY*0.5,ECalSizeY*0.5);
  if (!histo2[5]) G4cout << "\n can't create histo 15" << G4endl;
  histo2[6] = new TH2D("h26", "Energy vs Theta calo ",275,0.,550.,60,0.,12.);
  if (!histo2[6]) G4cout << "\n can't create histo 16" << G4endl;
  histo2[7] = new TH2D("h27", "Energy vs Theta calo >50 MeV ",275,0.,550.,60,0.,12.);
  if (!histo2[7]) G4cout << "\n can't create histo 16" << G4endl;
  histo2[8] = new TH2D("h28", "Energy vs Theta Brem",250,50.,550.,60,0.,12.);
  if (!histo2[8]) G4cout << "\n can't create histo 18" << G4endl;
  histo2[9] = new TH2D("h29", "Energy vs Theta Annhi ",200,50.,450.,50,0.,10.);
  if (!histo2[9]) G4cout << "\n can't create histo 19" << G4endl;
 
  // Additional histos for TimePix
  double TPixXmin= -14.1/2;
  double TPixXmax= TPixXmin+14.1*6;
  double TPiXBins = 14.1*6/0.055;

  double TPixYmin=-14.1/2;
  double TPixYmax=TPixYmin+14.1*2;
  double TPiYBins = 14.1*2/0.055;

  double MinE= 400.;
  double MaxE= 560.;
  int BinE= (int)(MaxE-MinE);

  // h50 Series TimePix study
  histo[50] = new TH1D("h50","Tpix energy",500,0.,1.);
  if (!histo[50]) G4cout << "\n can't create histo 18" << G4endl; 
  histo[51] = new TH1D("h51","TPix Time ",500,0.,500.);
  if (!histo[51]) G4cout << "\n can't create histo 51" << G4endl;
  histo[52] = new TH1D("h52","TPix X coord",TPiXBins,TPixXmin,TPixXmax);
  if (!histo[52]) G4cout << "\n can't create histo 52" << G4endl; 
  histo[53] = new TH1D("h53","TPix Y coord ",TPiYBins,TPixYmin,TPixYmax);
  if (!histo[53]) G4cout << "\n can't create histo 53" << G4endl; 
  histo[54] = new TH1D("h54","TPix Beam energy ",BinE,MinE,MaxE);
  if (!histo[54]) G4cout << "\n can't create histo 54" << G4endl;   

  histo2[55] = new TH2D("h55","TPix beam spot",TPiXBins,TPixXmin,TPixXmax,TPiYBins,TPixYmin,TPixYmax);
  if (!histo2[55]) G4cout << "\n can't create histo 55" << G4endl;
  histo2[56] = new TH2D("h56","TPix X vs E",TPiXBins,TPixXmin,TPixXmax,160,400.,560.);
  if (!histo2[56]) G4cout << "\n can't create histo 56" << G4endl;

  double TarXmin = -10.;
  double TarXmax =  10.;
  double TarXBins = 200.;

  double TarYmin = -10.;
  double TarYmax =  10.;
  double TarYBins = 200.;

  double BigFlagsXmin = -50.;
  double BigFlagsXmax =  50.;
  double BigFlagsXBins = 1000.;

  double BigFlagsYmin = -50.;
  double BigFlagsYmax =  50.;
  double BigFlagsYBins = 1000.;



  // h60 series Target study
  histo[60] = new TH1D("h60","Target energy",500,0.,0.1);
  if (!histo[60]) G4cout << "\n can't create histo 60" << G4endl; 
  histo[61] = new TH1D("h61","Target Angle ",500,0.,0.050);
  if (!histo[61]) G4cout << "\n can't create histo 61" << G4endl;
  histo[62] = new TH1D("h62","X coord ",TarXBins,TarXmin,TarXmax);
  if (!histo[62]) G4cout << "\n can't create histo 62" << G4endl; 
  histo[63] = new TH1D("h63","Y coord ",TarYBins,TarYmin,TarYmax);
  if (!histo[63]) G4cout << "\n can't create histo 63" << G4endl; 

  histo[64] = new TH1D("h64","Beam energy ",BinE,MinE,MaxE);
  if (!histo[64]) G4cout << "\n can't create histo 64" << G4endl;   
  histo2[65] = new TH2D("h65","Tar beam spot",TarXBins,TarXmin,TarXmax,TarYBins,TarYmin,TarYmax);
  if (!histo2[65]) G4cout << "\n can't create histo 65" << G4endl;
  histo2[66] = new TH2D("h66","Tar XvsE",TarXBins,TarXmin,TarXmax,320,500.,560.);
  if (!histo2[66]) G4cout << "\n can't create histo 66" << G4endl;

  histo2[67] = new TH2D("h67","Tar ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[67]) G4cout << "\n can't create histo 67" << G4endl;



  //****************************
  // Beam Flag1
  //****************************

  histo[110] = new TH1D("h110","Flag1 energy",500,0.,0.1);
  if (!histo[110]) G4cout << "\n can't create histo 110" << G4endl; 
  histo[111] = new TH1D("h111","Flag1 Angle ",500,0.,0.050);
  if (!histo[111]) G4cout << "\n can't create histo 111" << G4endl;
  histo[112] = new TH1D("h112","Flag1 X coord ",TarXBins,TarXmin,TarXmax);
  if (!histo[112]) G4cout << "\n can't create histo 112" << G4endl; 
  histo[113] = new TH1D("h113","Flag1 Y coord ",TarYBins,TarYmin,TarYmax);
  if (!histo[113]) G4cout << "\n can't create histo 113" << G4endl; 

  histo[114] = new TH1D("h114","Flag1 Beam energy ",BinE,MinE,MaxE);
  if (!histo[114]) G4cout << "\n can't create histo 114" << G4endl;   
  histo2[115] = new TH2D("h115","Flag1 beam spot",TarXBins,TarXmin,TarXmax,TarYBins,TarYmin,TarYmax);
  if (!histo2[115]) G4cout << "\n can't create histo 115" << G4endl;
  histo2[116] = new TH2D("h116","Flag1 XvsE",TarXBins,TarXmin,TarXmax,320,500.,560.);
  if (!histo2[116]) G4cout << "\n can't create histo 116" << G4endl;

  histo2[117] = new TH2D("h117","Flag1 ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[117]) G4cout << "\n can't create histo 117" << G4endl;

  //****************************
  // Beam Flag2
  //****************************

  histo[120] = new TH1D("h120","Flag2 energy",500,0.,0.1);
  if (!histo[120]) G4cout << "\n can't create histo 120" << G4endl; 
  histo[121] = new TH1D("h121","Flag2 Angle ",500,0.,0.050);
  if (!histo[121]) G4cout << "\n can't create histo 121" << G4endl;
  histo[122] = new TH1D("h122","Flag2 X coord ",TarXBins,TarXmin,TarXmax);
  if (!histo[122]) G4cout << "\n can't create histo 122" << G4endl; 
  histo[123] = new TH1D("h123","Flag2 Y coord ",TarYBins,TarYmin,TarYmax);
  if (!histo[123]) G4cout << "\n can't create histo 123" << G4endl; 

  histo[124] = new TH1D("h124","Flag2 Beam energy ",BinE,MinE,MaxE);
  if (!histo[124]) G4cout << "\n can't create histo 124" << G4endl;   
  histo2[125] = new TH2D("h125","Flag2 beam spot",TarXBins,TarXmin,TarXmax,TarYBins,TarYmin,TarYmax);
  if (!histo2[125]) G4cout << "\n can't create histo 125" << G4endl;
  histo2[126] = new TH2D("h126","Flag2 XvsE",TarXBins,TarXmin,TarXmax,320,500.,560.);
  if (!histo2[126]) G4cout << "\n can't create histo 126" << G4endl;

  histo2[127] = new TH2D("h127","Flag2 ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[127]) G4cout << "\n can't create histo 127" << G4endl;

  //****************************
  // Beam Flag3 before the target
  //****************************

  histo[130] = new TH1D("h130","Flag3 energy",500,0.,0.1);
  if (!histo[130]) G4cout << "\n can't create histo 130" << G4endl; 
  histo[131] = new TH1D("h131","Flag3 Angle ",500,0.,0.050);
  if (!histo[131]) G4cout << "\n can't create histo 131" << G4endl;
  histo[132] = new TH1D("h132","Flag3 X coord ",TarXBins,TarXmin,TarXmax);
  if (!histo[132]) G4cout << "\n can't create histo 132" << G4endl; 
  histo[133] = new TH1D("h133","Flag3 Y coord ",TarYBins,TarYmin,TarYmax);
  if (!histo[133]) G4cout << "\n can't create histo 133" << G4endl; 

  histo[134] = new TH1D("h134","Flag3 Beam energy ",BinE,MinE,MaxE);
  if (!histo[134]) G4cout << "\n can't create histo 134" << G4endl;   
  histo2[135] = new TH2D("h135","Flag3 beam spot",TarXBins,TarXmin,TarXmax,TarYBins,TarYmin,TarYmax);
  if (!histo2[135]) G4cout << "\n can't create histo 135" << G4endl;
  histo2[136] = new TH2D("h136","Flag3 XvsE",TarXBins,TarXmin,TarXmax,320,500.,560.);
  if (!histo2[136]) G4cout << "\n can't create histo 136" << G4endl;

  histo2[137] = new TH2D("h137","Flag3 ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[137]) G4cout << "\n can't create histo 137" << G4endl;

  //*****************************************
  // Beam Flag4 exit of DHSTB004 strait exit
  //*****************************************

  histo[140] = new TH1D("h140","Flag4 energy",500,0.,0.1);
  if (!histo[140]) G4cout << "\n can't create histo 140" << G4endl; 
  histo[141] = new TH1D("h141","Flag4 Angle ",500,0.,0.050);
  if (!histo[141]) G4cout << "\n can't create histo 141" << G4endl;
  histo[142] = new TH1D("h142","Flag4 X coord ",TarXBins,TarXmin,TarXmax);
  if (!histo[142]) G4cout << "\n can't create histo 142" << G4endl; 
  histo[143] = new TH1D("h143","Flag4 Y coord ",TarYBins,TarYmin,TarYmax);
  if (!histo[143]) G4cout << "\n can't create histo 143" << G4endl; 

  histo[144] = new TH1D("h144","Flag4 Beam energy ",BinE,MinE,MaxE);
  if (!histo[144]) G4cout << "\n can't create histo 144" << G4endl;   
  histo2[145] = new TH2D("h145","Flag4 beam spot",TarXBins,TarXmin,TarXmax,TarYBins,TarYmin,TarYmax);
  if (!histo2[145]) G4cout << "\n can't create histo 145" << G4endl;
  histo2[146] = new TH2D("h146","Flag4 XvsE",TarXBins,TarXmin,TarXmax,320,500.,560.);
  if (!histo2[146]) G4cout << "\n can't create histo 146" << G4endl;

  histo2[147] = new TH2D("h147","Flag4 ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[147]) G4cout << "\n can't create histo 147" << G4endl;

  //*****************************************
  // Beam Flag5 entrance of padme inner chamber
  //*****************************************

  histo[150] = new TH1D("h150","Flag5 energy",500,0.,0.1);
  if (!histo[150]) G4cout << "\n can't create histo 150" << G4endl; 
  histo[151] = new TH1D("h151","Flag5 Angle ",500,0.,0.050);
  if (!histo[151]) G4cout << "\n can't create histo 151" << G4endl;
  histo[152] = new TH1D("h152","Flag5 X coord ",BigFlagsXBins,BigFlagsXmin,BigFlagsXmax);
  if (!histo[152]) G4cout << "\n can't create histo 152" << G4endl; 
  histo[153] = new TH1D("h153","Flag5 Y coord ",BigFlagsYBins,BigFlagsYmin,BigFlagsYmax);
  if (!histo[153]) G4cout << "\n can't create histo 153" << G4endl; 

  histo[154] = new TH1D("h154","Flag5 Beam energy ",BinE,MinE,MaxE);
  if (!histo[154]) G4cout << "\n can't create histo 154" << G4endl;   
  histo2[155] = new TH2D("h155","Flag5 beam spot",200,BigFlagsXmin,BigFlagsXmax,200,BigFlagsYmin,BigFlagsYmax);
  if (!histo2[155]) G4cout << "\n can't create histo 155" << G4endl;
  histo2[156] = new TH2D("h156","Flag5 XvsE",TarXBins,BigFlagsXmin,BigFlagsXmax,320,500.,560.);
  if (!histo2[156]) G4cout << "\n can't create histo 156" << G4endl;

  histo2[157] = new TH2D("h157","Flag5 ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[157]) G4cout << "\n can't create histo 157" << G4endl;

  //*****************************************
  // Beam Flag6 entrance in front of TPix
  //*****************************************

  histo[160] = new TH1D("h160","Flag6 energy",500,0.,0.1);
  if (!histo[160]) G4cout << "\n can't create histo 160" << G4endl; 
  histo[161] = new TH1D("h161","Flag6 Angle ",500,0.,0.050);
  if (!histo[161]) G4cout << "\n can't create histo 161" << G4endl;
  histo[162] = new TH1D("h162","Flag6 X coord ",750,-75,75);
  if (!histo[162]) G4cout << "\n can't create histo 162" << G4endl; 
  histo[163] = new TH1D("h163","Flag6 Y coord ",500,-50,50);
  if (!histo[163]) G4cout << "\n can't create histo 163" << G4endl; 

  histo[164] = new TH1D("h164","Flag6 Beam energy ",BinE,MinE,MaxE);
  if (!histo[164]) G4cout << "\n can't create histo 164" << G4endl;   
  histo2[165] = new TH2D("h165","Flag6 beam spot",150,-75.,75.,100,-50.,50.);
  if (!histo2[165]) G4cout << "\n can't create histo 165" << G4endl;
  histo2[166] = new TH2D("h166","Flag6 XvsE",TarXBins,TarXmin,TarXmax,320,500.,560.);
  if (!histo2[166]) G4cout << "\n can't create histo 166" << G4endl;

  histo2[167] = new TH2D("h167","Flag6 ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[167]) G4cout << "\n can't create histo 167" << G4endl;

  // BeW study
  histo[70] = new TH1D("h70","BeW energy",500,0.,0.1);
  if (!histo[70]) G4cout << "\n can't create histo 70" << G4endl; 
  histo[71] = new TH1D("h71","BeW Angle ",500,0.,0.050);
  if (!histo[71]) G4cout << "\n can't create histo 71" << G4endl;
  histo[72] = new TH1D("h72","BeW X coord ",TarXBins,TarXmin,TarXmax);
  if (!histo[72]) G4cout << "\n can't create histo 72" << G4endl; 
  histo[73] = new TH1D("h73","BeW Y coord ",TarYBins,TarYmin,TarYmax);
  if (!histo[73]) G4cout << "\n can't create histo 73" << G4endl; 

  histo[74] = new TH1D("h74","BeW Beam energy ",BinE,MinE,MaxE);
  if (!histo[74]) G4cout << "\n can't create histo 74" << G4endl; 

  
  histo2[75] = new TH2D("h75","BeW beam spot",TarXBins,TarXmin,TarXmax,TarYBins,TarYmin,TarYmax);
  if (!histo2[75]) G4cout << "\n can't create histo 75" << G4endl;
  histo2[76] = new TH2D("h76","BeW XvsE",TarXBins,TarXmin,TarXmax,320,500.,560.);
  if (!histo2[76]) G4cout << "\n can't create histo 76" << G4endl;

  histo2[77] = new TH2D("h77","BeW ThetavsE",TarXBins,TarXmin,TarXmax,200,0,0.100);
  if (!histo2[77]) G4cout << "\n can't create histo 77" << G4endl;
  
  //Study gamma gamma txt
  histo[78] = new TH1D("h78","PxGamma1",1000,-100,700);
  if (!histo[78]) G4cout << "\n can't create histo 78" << G4endl;
  histo[79] = new TH1D("h79","PyGamma1",1000,-100,700);
  if (!histo[79]) G4cout << "\n can't create histo 79" << G4endl;
  histo[80] = new TH1D("h80","PzGamma1",1000,-100,700);
  if (!histo[80]) G4cout << "\n can't create histo 80" << G4endl;
  histo[81] = new TH1D("h81","PxGamma2",1000,-100,700);
  if (!histo[81]) G4cout << "\n can't create histo 81" << G4endl;
  histo[82] = new TH1D("h82","PyGamma2",1000,-100,700);
  if (!histo[82]) G4cout << "\n can't create histo 82" << G4endl;
  histo[83] = new TH1D("h83","PzGamma2",1000,-100,700);
  if (!histo[83]) G4cout << "\n can't create histo 83" << G4endl;
  histo[84] = new TH1D("h84","RadiusGamma1",200,-100,100);
  if (!histo[84]) G4cout << "\n can't create histo 84" << G4endl;
  histo[85] = new TH1D("h85","RadiusGamma2",200,-100,100);
  if (!histo[85]) G4cout << "\n can't create histo 85" << G4endl;

  //Hystogram of generated variables 30-39
  histo2[30] = new TH2D("h30", "Energy vs Theta Gen ",275,0.,550.,60,0.,12.);
  if (!histo2[30]) G4cout << "\n can't create histo 30" << G4endl;
  histo2[31] = new TH2D("h31", "Energy vs Theta Gen Brem ",275,0.,550.,60,0.,12.);
  if (!histo2[31]) G4cout << "\n can't create histo 31" << G4endl;
  histo2[32] = new TH2D("h32", "Energy vs Theta Gen Anni",275,0.,550.,60,0.,12.);
  if (!histo2[32]) G4cout << "\n can't create histo 32" << G4endl;
  histo2[33] = new TH2D("h33", "Energy vs Theta Gen in Acc",275,0.,550.,60,0.,12.);
  if (!histo2[33]) G4cout << "\n can't create histo 33" << G4endl;
  histo2[34] = new TH2D("h34", "Energy vs Theta Gen Brem in Acc ",275,0.,550.,60,0.,12.);
  if (!histo2[34]) G4cout << "\n can't create histo 31" << G4endl;
  histo2[35] = new TH2D("h35", "Energy vs Theta Gen Anni in Acc",275,0.,550.,60,0.,12.);
  if (!histo2[35]) G4cout << "\n can't create histo 32" << G4endl;
  histo2[36] = new TH2D("h36", "Beam X Y ",200,-5.,5.,200,-5.,5.);
  if (!histo2[36]) G4cout << "\n can't create histo 36" << G4endl;

  // Additional histos for kinematics studies
  histo2[37] = new TH2D("h37", "Gamma energy vs angle in Uboson events",100,0.,0.1,275,0.,550.);
  if (!histo2[37]) G4cout << "\n can't create histo 37" << G4endl;
  histo2[38] = new TH2D("h38", "Gamma energy vs angle Brem",100,0.,0.1,300,0.,600.);
  if (!histo2[38]) G4cout << "\n can't create histo 38" << G4endl;
  histo2[39] = new TH2D("h39", "Gamma energy vs angle Ann",315,0.,3.15,300,0.,600.);
  if (!histo2[39]) G4cout << "\n can't create histo 39" << G4endl;

  // create 1 ntuple in subdirectory "tuples"
  //
  ntupl = new TTree("U102", "Envent");
  ntupl->Branch("Nevent", &(myEvt.NTNevent), "Nevent/I");
  ntupl->Branch("ETot", &(myEvt.NTEtot), "ETot/D");

  ntupl->Branch("IDProc", &(myEvt.NTIDProc), "IDProc/D");
  ntupl->Branch("PBeam", &(myEvt.NTPBeam), "PBeam/D");
  ntupl->Branch("PPrim", &(myEvt.NTPriBeam), "PPrim/D");
  ntupl->Branch("XBeam", &(myEvt.NTXBeam), "XBeam/D");
  ntupl->Branch("YBeam", &(myEvt.NTYBeam), "YBeam/D");
  ntupl->Branch("PosMomX", &(myEvt.PMomX), "PosMomX/D");
  ntupl->Branch("PosMomY", &(myEvt.PMomY), "PosMomY/D");
  ntupl->Branch("PosMomZ", &(myEvt.PMomZ), "PosMomZ/D");

  ntupl->Branch("NClusters", &(myEvt.NTNCluster), "NClusters/I");
  ntupl->Branch("NTracks",   &(myEvt.NTNTracks), "NTracks/I");
  ntupl->Branch("NHEPVetoTracks", &(myEvt.NTNHEPVetoTracks), "NHEPVetoTracks/I");

  ntupl->Branch("NPVetoTracks", &(myEvt.NTNPVetoTracks), "NPVetoTracks/I");
  ntupl->Branch("NEVetoTracks", &(myEvt.NTNEVetoTracks), "NEVetoTracks/I");

  ntupl->Branch("NSAC", &(myEvt.NTSACNHit), "NSAC/I");
  ntupl->Branch("NCal", &(myEvt.NTNCal), "NCal/I");
  ntupl->Branch("NLAV", &(myEvt.NTLAVNHit), "NLAV/I");
  ntupl->Branch("NTarget", &(myEvt.NTNTarget), "NTarget/I");

  ntupl->Branch("ESAC", (myEvt.NTSACE), "ESAC[100]/D");
  ntupl->Branch("TSAC", (myEvt.NTSACT), "TSAC[100]/D");
  ntupl->Branch("PTypeSAC", (myEvt.NTSACPType), "PTypeSAC[100]/D");
  ntupl->Branch("XSAC", (myEvt.NTSACX), "XSAC[100]/D");
  ntupl->Branch("YSAC", (myEvt.NTSACY), "YSAC[100]/D");
  ntupl->Branch("SACCh", (myEvt.NTSACCh), "SACCh[100]/I");

  ntupl->Branch("ELAV", (myEvt.NTLAVE), "ELAV[100]/D");
  ntupl->Branch("TLAV", (myEvt.NTLAVT), "TLAV[100]/D");
  ntupl->Branch("PTypeLAV", (myEvt.NTLAVPType), "PTypeLAV[100]/D");
  ntupl->Branch("XLAV", (myEvt.NTLAVX), "XLAV[100]/D");
  ntupl->Branch("YLAV", (myEvt.NTLAVY), "YLAV[100]/D");
  
  ntupl->Branch("EPartCal",     (myEvt.NTCalPartE),     "CalE[20]/D");
  ntupl->Branch("TPartCal",     (myEvt.NTCalPartT),     "CalT[20]/D");
  ntupl->Branch("PTypePartCal", (myEvt.NTCalPartPType), "CalPType[20]/I");
  ntupl->Branch("XPartCal",     (myEvt.NTCalPartX),     "CalX[20]/D");
  ntupl->Branch("YPartCal",     (myEvt.NTCalPartY),     "CalY[20]/D");

  ntupl->Branch("ECluster", (myEvt.NTECluster), "ECluster[20]/D"); 
  ntupl->Branch("QCluster", (myEvt.NTQCluster), "QCluster[20]/D");
  ntupl->Branch("XCluster", (myEvt.NTXCluster), "XCluster[20]/D");
  ntupl->Branch("YCluster", (myEvt.NTYCluster), "YCluster[20]/D");
  ntupl->Branch("ThCluster",(myEvt.NTThCluster),"ThCluster[20]/D");
  ntupl->Branch("M2Cluster",(myEvt.NTM2Cluster),"M2Cluster[20]/D");
  ntupl->Branch("TCluster", (myEvt.NTTCluster), "TCluster[20]/D");  //last modified
  ntupl->Branch("NClusCells",(myEvt.NTNClusCells), "NClusCells[20]/D");  //last modified

//  ntupl->Branch("ECell",    (myEvt.NTECell), "ECell[1000]/D");
//  ntupl->Branch("QCell",    (myEvt.NTQCell), "QCell[1000]/D");
//  ntupl->Branch("TCell",    (myEvt.NTTCell), "TCell[1000]/D");

//  ntupl->Branch("ETracker", (myEvt.NTETracker), "ETracker[100]/D");
//  ntupl->Branch("TrackerLay",(myEvt.NTTrackerLay), "TrackerLay[100]/D");
//  ntupl->Branch("TTracker", (myEvt.NTTrackerTime), "TTracker[100]/D");
//  ntupl->Branch("ZTracker", (myEvt.NTTrackerZ), "ZTracker[100]/D");
//  ntupl->Branch("XTracker", (myEvt.NTTrackerX), "XTracker[100]/D");
//  ntupl->Branch("YTracker", (myEvt.NTTrackerY), "YTracker[100]/D");

  ntupl->Branch("ETarget", &(myEvt.NTETarget), "ETarget/D");
  ntupl->Branch("TTarget", &(myEvt.NTTTarget), "TTarget/D");
  ntupl->Branch("XTarget", &(myEvt.NTXTarget), "XTarget/D");
  ntupl->Branch("YTarget", &(myEvt.NTYTarget), "YTarget/D");

  ntupl->Branch("HEPVetoTrEne" ,(myEvt.NTHEPVetoTrkEne),   "NTHEPVetoTrkEne[100]/D");
  ntupl->Branch("HEPVetoNFing" ,(myEvt.NTHEPVetoTrkFinger),"NTHEPVetoTrkFinger[100]/I");
  ntupl->Branch("HEPVetoTrTime",(myEvt.NTHEPVetoTrkTime),  "NTHEPVetoTrkTime[100]/D");
  ntupl->Branch("HEPVetoFingE", (myEvt.NTHEPVetoFingerE),  "NTHEPVetoFingE[100]/D");
  ntupl->Branch("HEPVetoX", (myEvt.NTHEPVetoX),  "NTHEPVetoX[100]/D");
  ntupl->Branch("HEPVetoY", (myEvt.NTHEPVetoY),  "NTHEPVetoY[100]/D");
  ntupl->Branch("HEPVetoClIndex",   (myEvt.NTHEPVetoClIndex),  "NTHEPVetoClIndex[100]/I");
  ntupl->Branch("HEPVetoECl",       (myEvt.NTHEPVetoECl),      "NTHEPVetoECl[100][10]/D");
  ntupl->Branch("HEPVetoTimeCl",    (myEvt.NTHEPVetoTimeCl),   "NTHEPVetoTimeCl[100][10]/D");

  ntupl->Branch("PVetoTrEne" ,(myEvt.NTPVetoTrkEne),   "NTPVetoTrkEne[100]/D");
  ntupl->Branch("PVetoNFing" ,(myEvt.NTPVetoTrkFinger),"NTPVetoTrkFinger[100]/I");
  ntupl->Branch("PVetoTrTime",(myEvt.NTPVetoTrkTime),  "NTPVetoTrkTime[100]/D");
  ntupl->Branch("PVetoFingE", (myEvt.NTPVetoFingerE),  "NTPVetoFingE[100]/D");
  ntupl->Branch("PVetoX",     (myEvt.NTPVetoX),        "NTPVetoX[100]/D");
  ntupl->Branch("PVetoY",     (myEvt.NTPVetoY),        "NTPVetoY[100]/D");
  ntupl->Branch("PVetoBarE",     (myEvt.NTPVetoBarEnergy),        "NTPVetoBarEnergy[100]/D");
  ntupl->Branch("PVetoBarT",     (myEvt.NTPVetoBarTime),        "NTPVetoBarTime[100]/D");

  ntupl->Branch("PVetoClIndex",   (myEvt.NTPVetoClIndex),  "NTPVetoClIndex[100]/I");
  ntupl->Branch("PVetoECl",       (myEvt.NTPVetoECl),      "NTPVetoECl[100][10]/D");
  ntupl->Branch("PVetoTimeCl",    (myEvt.NTPVetoTimeCl),   "NTPVetoTimeCl[100][10]/D");


  ntupl->Branch("EVetoTrEne" ,(myEvt.NTEVetoTrkEne),   "NTEVetoTrkEne[100]/D");
  ntupl->Branch("EVetoNFing" ,(myEvt.NTEVetoTrkFinger),"NTEVetoTrkFinger[100]/I");
  ntupl->Branch("EVetoTrTime",(myEvt.NTEVetoTrkTime),  "NTEVetoTrkTime[100]/D");
  ntupl->Branch("EVetoFingE", (myEvt.NTEVetoFingerE),  "NTEVetoFingE[100]/D");
  ntupl->Branch("EVetoX",     (myEvt.NTEVetoX),        "NTEVetoX[100]/D");
  ntupl->Branch("EVetoY",     (myEvt.NTEVetoY),        "NTEVetoY[100]/D");
  ntupl->Branch("EVetoClIndex",   (myEvt.NTEVetoClIndex),  "NTEVetoClIndex[100]/I");
  ntupl->Branch("EVetoECl",       (myEvt.NTEVetoECl),      "NTEVetoECl[100][10]/D");
  ntupl->Branch("EVetoTimeCl",    (myEvt.NTEVetoTimeCl),   "NTEVetoTimeCl[100][10]/D");

  //if(IsTrackerRecoON==1){
  //  ntupl->Branch("NTNTrClus", &(myEvt.NTNTrClus),"NTNTrClus/I");
  //  ntupl->Branch("NTTrClusX", myEvt.NTTrClusX,"NTTrClusX[1000]/D");
  //  ntupl->Branch("NTTrClusY", myEvt.NTTrClusY,"NTTrClusY[1000]/D");
  //  ntupl->Branch("NTTrClusZ", myEvt.NTTrClusZ,"NTTrClusZ[1000]/D");
  //  ntupl->Branch("NTTrClusLayer", myEvt.NTTrClusLayer,"NTTrClusLayer[1000]/I");
  //} 

  G4cout << "\n----> Histogram file is opened in " << fileName << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::save()
{ 
  if (rootFile) {
    rootFile->Write();        // Writing the histograms to the file
    rootFile->Close();        // and closing the tree (and the file)
    G4cout << "\n----> Histogram Tree is saved \n" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillHisto(G4int ih, G4double xbin, G4double weight)
{
  if (ih >= MaxHisto) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << ih
           << " does not exist. (xbin=" << xbin << " weight=" << weight << ")"
           << G4endl;
    return;
  }
 if  (histo[ih]) { histo[ih]->Fill(xbin, weight); }
}

void HistoManager::FillHisto2(G4int ih, G4double xbin, G4double ybin, G4double weight)
{
  if (ih >= MaxHisto) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << ih
           << " does not exist. (xbin=" <<xbin<<" "<<ybin<< " weight=" << weight << ")"
           << G4endl;
    return;
  }
  if  (histo2[ih]) {histo2[ih]->Fill(xbin, ybin, weight); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Normalize(G4int ih, G4double fac)
{
  if (ih >= MaxHisto) {
    G4cout << "---> warning from HistoManager::Normalize() : histo " << ih
           << " does not exist. (fac=" << fac << ")" << G4endl;
    return;
  }
  if (histo[ih]) histo[ih]->Scale(fac);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillNtuple(NTEvent* Evt)
{
  if (ntupl) ntupl->Fill();
  if(ntSim) ntSim->Fill();
  if(ntGen) ntGen->Fill();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::PrintStatistic()
{
  if(histo[1]) {
    G4cout << "\n ----> print histograms statistic \n" << G4endl;
    
    G4cout<<" ECry : mean = " << G4BestUnit(histo[1]->GetMean(), "Energy") 
      << " rms = " << G4BestUnit(histo[1]->GetRMS(),  "Energy") << G4endl;
    G4cout<< " EBeam : mean = " << G4BestUnit(histo[2]->GetMean(), "Energy") 
      << " rms = " << G4BestUnit(histo[2]->GetRMS(),  "Energy") << G4endl;
    G4cout 
      << " ETracker : mean = " << G4BestUnit(histo[3]->GetMean(), "Energy") 
      << " rms = " << G4BestUnit(histo[3]->GetRMS(),  "Energy") << G4endl;
    //    G4cout 
    //    << " LGap : mean = " << G4BestUnit(histo[4]->GetMean(), "Length") 
    //            << " rms = " << G4BestUnit(histo[4]->GetRMS(),  "Length") << G4endl;
    
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


