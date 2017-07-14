// GENERAL FLAGS
const int   NPrint      = 1000;
const bool  fAutoSeed   = true; //random seed with clock
//const bool  fAutoSeed   = false; //fixed random seed
// DETECTORS FLAGS
const int    IsCalibRun  = 0;  //1 global Calo calibration 2 Calo Energy resolution
const double EMinSaveNT  = 5.;
// DETECTORS FLAGS
const double WorldLength  = 12.; //in meters

const int   IsBFieldON    = 1;  // 1 uniform B field 2 non uniform
const int   IsMagIronON   = 1;  // 1 CERN magnet 
const int   IsEcalON      = 1;
const int   IsEcalRoundON = 1;  

const int   IsTrackerON   = 0;   // cilindrical tracker obsolete set to 0
const int   IsPlanarGEMON = 0;   // 1 planar GEM spectrometer 
const int   IsPVetoON     = 1;   // 1  Veto inside the magnet Positron side
const int   IsEleVetoON   = 1;   // Veto inside the magnet Electron side
const int   IsEVetoON     = 1;   // Veto for High Momentum positron
const int   IsSACON       = 1;  //Small angle gamma veto.
const int   IsLAVON       = 0;  //LARGE angle gamma veto.

const int   IsMonitorON   = 0;  //first station for P measurement
const int   IsTargetON    = 0;  // is off if dump is On
const int   IsTDumpON     = 0;

const int   IsWallON        = 0;
const int   IsTrackerRecoON = 0;
const int   IsSpecON        = 0;    //not used
const int   IsPipeON        = 0;
const int   IsSpecInside    = -1;  //if 0 spectrometer is outside vacuum 1 is inside any other no vac chamb

// Beam parameter flags
const int  NPrimaries     = 5000;   //Maximum 10^5 on the macbook was 1000
const int  BeamSpot       = 1;   //Std 1
const int  BeamESpread    = 1;   //Std 1
const int  IsBeamTimeON   = 1;   //Std 1
const int  BeamEmittance  = 1;   //Std 1
const int  IsResGasON     = 1;   //Std 1, realistic vacuum
//const int  IsResGasON     = 0;   //Std 1, galactic vacuum

//Beam Parameters
const double BeamEnergy    = 550.; //in MeV 550 standard
const double SigmaBeamE    = BeamEnergy*0.01;  //in MeV
const double BunchLenght   = 40.; //At present from 3-40 ns
const double MicroBunchSpace = 0.350; //fixed to 350 ps
const double MicroBunchLength= 0.150; //fixed to 350 ps
const double SigmaBeamX    = 0.7;     // in mm 
const double SigmaBeamY    = 0.7;     // in mm
const double BeamDivX      = 1.e-3;   // in radiants Still to be tested
const double BeamDivY      = 1.e-3;   // in radiants Still to be tested
const double position      = -80.;    // in cm
const double VacuumP       = 1e-6;    // is in bar
const double VacuumDensity = 1.290*VacuumP;  //mg*cm3
const double MagneticField = 0.55; //0.6 

//Physics list flags
const int    UseNewPhysList = 1;   // was 0
const int    IsSincrotronON = 0;    //Std=1
const int    IsNuclearON    = 0;    //Std=1
const int    NuclearBertini = 1;  //Bertini cascade
const int    NuclearBinary  = 0;  //Binary cascade
const int    NuclearHP      = 1;  //Nuclear high performance tracking (not speed!)

// Turbo mode flags (kill e- e+ g with energy below Thr in MeV)
const int IsTurboOn       = 0;
const double CutG4        = 0.1;       // in cm default is 0.1
const double epKillEnergy = 10.;       // kill e+
const double emKillEnergy = 10.;       // kill e-
const double gKillEnergy  = 1.;        // kill gamma
const double nKillEnergy  = 10.;       // kill neutrons

//Other background events generation:
const int    NThreePhoton  = 0;
// U boson parameters    
const int    NUBosons      = 0;  //Works only with 0 and 1 for the moment....
const double UMass         = 22;  //in MeV 
const double epsilon       = 1E-3; //in 10-33
const int    UDecType      = 1;  //with 0 e+e-   and 1 for invisible DM
const int    UGenType      = 0;  //with 0 annihi and 1 for Bremsstrahlung not implemented

//  Monitor (second diamond station not used)
const double MonitorSizeX = 4.; //in cm
const double MonitorSizeY = 4.; //in cm
const double MonitorSizeZ = 4.; //in cm
				
const double MonitorPosiX = 0.;	  //in cm
const double MonitorPosiY = 0.;	  //in cm
const double MonitorPosiZ = -50.;  //in cm

//  Target Dimension
const double TargetSizeX =2.;	//in cm
const double TargetSizeY =2.;	//in cm
const double TargetSizeZ =0.01; //in cm 100 micron was 0.005

const double TargetPosiX =0.;	//in cm
const double TargetPosiY =0.;	//in cm
const double TargetPosiZ =-20.;	//in cm
//const double TargetPosiZ = 0.;	//in cm
//Dump region parameters only
const double TDumpFiltThick     = 10.;
const double TDumpFiltInnerRad1 = 3.5;
const double TDumpFiltOuterRad1 = TDumpFiltInnerRad1+TDumpFiltThick;
const double TDumpFiltInnerRad2 = 5.5;
const double TDumpFiltOuterRad2 = TDumpFiltInnerRad2+TDumpFiltThick;
const double TDumpFiltHz        = 5.; //questo per due

const double TDumpFiltPosiX =0.;	//in cm
const double TDumpFiltPosiY =0.;	//in cm
const double TDumpFiltPosiZ =-10.;	//in cm

const double TDumpInnerRad = 0.;
const double TDumpOuterRad = 3.5;
const double TDumpHz       = 3.; //questo per due

const double TDumpPosiX =0.;	//in cm
const double TDumpPosiY =0.;	//in cm
const double TDumpPosiZ =-65.;	//in cm

const double LeadBrickSizeX =  5.;	//in cm
const double LeadBrickSizeY = 10.;	//in cm
const double LeadBrickSizeZ = 20.;	//in cm

//  ECAL Dimension
const double ECalSizeX =58.;   //in cm diamter
const double ECalSizeY =58.;   //in cm
const double ECalSizeZ =22.;   //in cm full scale length

const double ECalInnHole = 4.; // this is radius
const double ECalCellSize= 2.;

const double ECalPosiX =0.;	//in cm
const double ECalPosiY =0.;	//in cm
const double ECalPosiZ =180.+ECalSizeZ*0.5;	//in cm//200.+ECalSizeZ*0.5;	//in cm

const int ECalNRow       = ECalSizeX/ECalCellSize;
const int ECalNCol       = ECalSizeY/ECalCellSize;
const int ECalNCells     = ECalNRow*ECalNCol;

//Cluster reconstruction variables
const double SeedE    = 5.;     //in MeV
const double CellsE   = 0.3;     //in MeV
const double RadiusCl = 4.5;     //in cm 25 crystal 2x2 involved
const double IsCaloCalibON= 0.;  //Calor calibration constants: 0 all 1; 1 gauss with sigma = 1%

//SAC geometry
const double SACX      = 12; // 6x6 crystals 2x2 cm each
const double SACY      = 12;
const double SACLength = 10;

//LAV geometry
const double LAVInnRad = 4.0; 
const double LAVOutRad = 10.;
const double LAVLength = 20; // in cm 

//  High Energy Positron Veto outside the magnet dimension
const double EVetoPosiX = 0.;	//in cm
const double EVetoPosiY =-75.;	//in cm was 85 cm
//const double EVetoPosiZ = -0.5*ECalSizeZ+0.5+ECalPosiZ+20.;	//in cm
const double EVetoPosiZ = 223.;	//in cm

const double EVetoSizeX =20.;   //in cm
const double EVetoSizeY =50.;   //in cm was 80
const double EVetoSizeZ =1.;    //in cm

const double EVetoFingerSize =1.;    //in cm

//  Positron Veto inside the magnet dimension
const double PVetoPosiX =  0.;	//in cm
const double PVetoPosiY =-20.;	//in cm was 85 cm
const double PVetoPosiZ =  0.;	//in cm
	     
const double PVetoSizeX = 19.8;    //in cm
const double PVetoSizeY =1.;    //in cm full detector size will decide finger number 
const double PVetoSizeZ =100.;     //in cm

const double PVetoFingerSize =1.;    //in cm

//  Electron Veto inside the magnet dimension
const double EleVetoPosiX = 0.;	//in cm
const double EleVetoPosiY =20;	//in cm was 85 cm
const double EleVetoPosiZ = 0.;	//in cm
	     
const double EleVetoSizeX =19.8;   //in cm
const double EleVetoSizeY =1.;   //in cm was 80
const double EleVetoSizeZ =100.;    //in cm
	     
const double EleVetoFingerSize =1.;    //in cm

// Tracker geometry same position of the magnet
const double TrackerPosiX = 0.;	        //in cm
const double TrackerPosiY = 0.;	        //in cm
const double TrackerPosiZ = 0.;	        //in cm is now in the magnet
//const double TrClsDist = 0.3*CLHEP::mm;

// **********************************
//  TRACKER GEOMETRY
//*******************************
// Cylindrical tracker
const double TrackerNLayers   = 5.;
const double TrackerLayerTick = 1.;
const double TrackerNRings    = 1.;
const double TrackerInnerRad  = 20.;
const double TrackerOuterRad  = TrackerInnerRad+TrackerLayerTick;
const double TrackerHz        = 50.; //questo per due

//Planar GEM traker
const int NChambers    = 6;
const double PGEMSizeX = 9.7;   //in cm
const double PGEMSizeY = 0.5;   //for each of the NChambers
const double PGEMSizeZ = 45.;   //in cm half size

const double PGEMPosiX =  0.;
const double PGEMPosiY = 20.;
const double PGEMPosiZ =  0.; //is inside the magnet volume

const double MagnetSizeX = 20.;  //magnetic field region
const double MagnetSizeY = 52.;
const double MagnetSizeZ = 100.;

const double MagnetPosiX = 0.;	//in cm
const double MagnetPosiY = 0.;	//in cm
const double MagnetPosiZ = 20+MagnetSizeZ*0.5;	//in cm

//top joke of magnet
const double MagUpSizeX =  77.; 
const double MagUpSizeY = 200.; //was 116
const double MagUpSizeZ = 100.;

const double MagUpX = 10.+MagUpSizeX/2;
const double MagUpY = 0.;
const double MagUpZ = MagnetPosiZ;	//in cm;

const double MagRgSizeX = 20.; 
const double MagRgSizeY = 42.;
const double MagRgSizeZ = 100.;

const double MagRgX = 0.;
const double MagRgY = 58.+MagRgSizeY/2; //was 26
const double MagRgZ = MagnetPosiZ;	//in cm;

const double startAngle = 0.;
const double spanningAngle = 180.;
const double MagCoilRMin=15.;  //numbers to be verified
const double MagCoilRMax=35.;  //numbers to be verified
const double MagCoilDz=10.;    //numbers to be verified

const double MagCoilPosX = 10.+MagCoilDz;
const double MagCoilPosY = 0.;
const double MagCoilPosZ = 20.;	//in cm;

const int MaxTracks = 100; //to be less than 50 check in EVENT ACTION histoManager.cc HistoManager.hh
const int MaxTrHits = 500;

//  Beam Dump wall Dimension	     
const double WallSizeX = 450.; // in cm
const double WallSizeY = 200.; // in cm
const double WallSizeZ =  50.; // in cm

const double WallPosiX = 0.; // in cm
const double WallPosiY = 0.; // in cm
const double WallPosiZ = 500.+WallSizeZ/2; // in cm, reference point: center of magnet
