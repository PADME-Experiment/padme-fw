#include "Constants.hh" 

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "MagneticFieldSetup.hh"

#include "TargetDetector.hh"
#include "ECalDetector.hh"
#include "SACDetector.hh"
#include "PVetoDetector.hh"
#include "EVetoDetector.hh"
#include "HEPVetoDetector.hh"
#include "TDumpDetector.hh"
#include "TPixDetector.hh"
#include "LAVDetector.hh"
#include "TungstenDetector.hh"

#include "MagnetStructure.hh"
#include "ChamberStructure.hh"
#include "BeamLineStructure.hh" //M. Raggi 07/03/2019
#include "HallStructure.hh"

#include "TargetGeometry.hh"
#include "ECalGeometry.hh"
#include "SACGeometry.hh"
#include "PVetoGeometry.hh"
#include "EVetoGeometry.hh"
#include "HEPVetoGeometry.hh"
#include "TDumpGeometry.hh"
#include "TPixGeometry.hh"

#include "MagnetGeometry.hh"
#include "ChamberGeometry.hh"
#include "BeamLineGeometry.hh" //M. Raggi 07/03/2019
#include "HallGeometry.hh"

#include "G4Material.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
//#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
//#include "G4PVParameterised.hh"
//#include "G4SDManager.hh"
//#include "G4GeometryTolerance.hh"
//#include "G4GeometryManager.hh"
#include "G4RotationMatrix.hh"
//#include "G4SubtractionSolid.hh"

//#include "G4UserLimits.hh"
#include "G4NistManager.hh"
//#include "G4VSDFilter.hh"
//#include "G4SDParticleFilter.hh"
//#include "G4MultiFunctionalDetector.hh"
//#include "G4VPrimitiveScorer.hh"
//#include "G4PSEnergyDeposit.hh"
//#include "G4PSNofSecondary.hh"
//#include "G4PSTrackLength.hh"
//#include "G4PSNofStep.hh"
//#include "G4PSMinKinEAtGeneration.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
DetectorConstruction::DetectorConstruction()
:fWorldLength(0.)
{

  fDetectorMessenger = new DetectorMessenger(this);

  fECalDetector      = new ECalDetector(0);
  fTargetDetector    = new TargetDetector(0);
  fSACDetector       = new SACDetector(0);
  fLAVDetector       = new LAVDetector(0);
  fPVetoDetector     = new PVetoDetector(0);
  fEVetoDetector     = new EVetoDetector(0);
  fHEPVetoDetector   = new HEPVetoDetector(0);
  fTDumpDetector     = new TDumpDetector(0);
  fTPixDetector      = new TPixDetector(0);
  fTungstenDetector  = new TungstenDetector(0); 
  fMagnetStructure   = new MagnetStructure(0);
  fChamberStructure  = new ChamberStructure(0);
  fBeamLineStructure = new BeamLineStructure(0); //M. Raggi 07/03/2019
  fHallStructure     = new HallStructure(0);

  fMagneticFieldManager = new MagneticFieldSetup();

  fEnableECal     = 1;
  fEnableTarget   = 1;
  fEnableSAC      = 1;
  fEnableLAV      = 1;
  fEnablePVeto    = 1;
  fEnableEVeto    = 1;
  fEnableHEPVeto  = 1;
  fEnableTDump    = 0;
  fEnableTPix     = 1;
  fEnableTungsten = 0;

  fEnableWall    = 0;
  fEnableMagnet  = 1;

  fEnableMagneticField = 1;
  fMagneticVolumeIsVisible = 0;

  fEnableChamber = 1;
  fEnableBeamLine = 1;  //M. Raggi 07/03/2019
  fChamberIsVisible = 1;
  fBeamLineIsVisible = 1; //M. Raggi 07/03/2019

  fWorldIsFilledWithAir = 0;

  fWorldLength = 12.*m;

  DefineMaterials();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
DetectorConstruction::~DetectorConstruction()
{

  //delete stepLimit;
  delete fDetectorMessenger;             

  delete fECalDetector;
  delete fTargetDetector;
  delete fSACDetector;
  delete fPVetoDetector;
  delete fEVetoDetector;
  delete fHEPVetoDetector;
  delete fTDumpDetector;
  delete fTPixDetector;
  delete fTungstenDetector;
  delete fMagnetStructure;
  delete fChamberStructure;
  delete fBeamLineStructure;    //M. Raggi 07/03/2019
  delete fHallStructure;
  delete fMagneticFieldManager;

}

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  // Access some useful geometries
  ChamberGeometry* geoChamber = ChamberGeometry::GetInstance();
  HEPVetoGeometry* geoHEPVeto = HEPVetoGeometry::GetInstance();
  TPixGeometry* geoTPix = TPixGeometry::GetInstance();

  //------------------------------
  // World Volume
  //------------------------------


  G4VPhysicalVolume* physicWorld = 0;
  G4LogicalVolume* logicWorld = 0;

  if (fEnableChamber) {

    // The world volume is defined inside the GDML file which also contains the geometry
    // of the Vacuum Chamber internal and external shells.
    fParser.Read("gdml/Chamber.gdml");
    physicWorld = fParser.GetWorldVolume(); // Recover physical world volume
    logicWorld = physicWorld->GetLogicalVolume(); // Recover logical world volume
    
    // Set world characteristics
    logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
    //logicWorld->SetVisAttributes(G4VisAttributes(G4Colour::White()));
    logicWorld->SetMaterial(G4Material::GetMaterial("Vacuum"));
    printf("World %s %s\n",logicWorld->GetName().data(),logicWorld->GetMaterial()->GetName().data());
    
    // Set color and material of the Vacuum Chamber shells
    G4int nD = logicWorld->GetNoDaughters();
    for(G4int iD = 0; iD<nD;iD++) {
      G4VPhysicalVolume* D = logicWorld->GetDaughter(iD);
      G4LogicalVolume* Dlog = D->GetLogicalVolume();
      if (fChamberIsVisible) {
	Dlog->SetVisAttributes(G4VisAttributes(G4Colour::Grey()));
      } else {
	Dlog->SetVisAttributes(G4VisAttributes::Invisible);
      }
      Dlog->SetMaterial(G4Material::GetMaterial("G4_STAINLESS-STEEL"));
      if (D->CheckOverlaps()) {
	printf("DetectorConstruction - WARNING - overlaps found in %s\n",Dlog->GetName().data());
      }
      printf("Vacuum Chamber %s %s\n",Dlog->GetName().data(),Dlog->GetMaterial()->GetName().data());
    }

  } else {

    // When the vacuum chamber is not required, just create the standard World volume
    G4Box* solidWorld = new G4Box("World",0.5*fWorldLength,0.5*fWorldLength,0.5*fWorldLength);
    logicWorld = new G4LogicalVolume(solidWorld,G4Material::GetMaterial("G4_AIR"),"World",0,0,0);
    if (! fWorldIsFilledWithAir) logicWorld->SetMaterial(G4Material::GetMaterial("Vacuum"));
    logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
    //logicWorld->SetVisAttributes(G4VisAttributes(G4Colour::White()));
    physicWorld = new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,0);

  }


  // Beam Line structure M. Raggi 07/03/2019
  if (fEnableBeamLine) {
    fBeamLineStructure->EnableBeamLine();
  } else {
    fBeamLineStructure->DisableBeamLine();
  }
  if (fBeamLineIsVisible) {
    fBeamLineStructure->SetBeamLineVisible();
  } else {
    fBeamLineStructure->SetBeamLineInvisible();
  }
  fBeamLineStructure->SetMotherVolume(logicWorld);
  fBeamLineStructure->CreateGeometry();

  // Vacuum chamber structure
  if (fEnableChamber) {
    fChamberStructure->EnableChamber();
  } else {
    fChamberStructure->DisableChamber();
  }
  if (fChamberIsVisible) {
    fChamberStructure->SetChamberVisible();
  } else {
    fChamberStructure->SetChamberInvisible();
  }
  fChamberStructure->SetMotherVolume(logicWorld);
  fChamberStructure->CreateGeometry();

  // Create magnetic volume inside vacuum chamber

  G4double magVolMinX = -0.5*geoChamber->GetVCInnerSizeX()+1.*um;
  G4double magVolMinY = -0.5*geoChamber->GetVCInnerSizeY()+1.*um;
  G4double magVolMinZ = geoChamber->GetVCInnerFacePosZ();

  G4double magVolMaxX = 0.5*geoChamber->GetVCInnerSizeX()-1.*um;
  G4double magVolMaxY = 0.5*geoChamber->GetVCInnerSizeY()-1.*um;
  G4double magVolMaxZ = 1000.0*mm; // Magnetic field map extends up to +/-1m from magnet center

  G4double magVolPosX = 0.5*(magVolMinX+magVolMaxX);
  G4double magVolPosY = 0.5*(magVolMinY+magVolMaxY);
  G4double magVolPosZ = 0.5*(magVolMinZ+magVolMaxZ);
  G4ThreeVector magVolPos = G4ThreeVector(magVolPosX,magVolPosY,magVolPosZ);

  G4double magVolHLX = 0.5*(magVolMaxX-magVolMinX);
  G4double magVolHLY = 0.5*(magVolMaxY-magVolMinY);
  G4double magVolHLZ = 0.5*(magVolMaxZ-magVolMinZ);

  printf ("--- Magnetic Volume ---\n");
  printf ("Min/max coords %f %f %f %f %f %f\n",magVolMinX,magVolMaxX,magVolMinY,magVolMaxY,magVolMinZ,magVolMaxZ);
  printf ("Half sides %f %f %f\n",magVolHLX,magVolHLY,magVolHLZ);
  printf ("Position %f %f %f\n",magVolPosX,magVolPosY,magVolPosZ);
  printf ("-----------------------\n");

  G4Box* solidMagneticVolume = new G4Box("MagneticVolume",magVolHLX,magVolHLY,magVolHLZ);
  G4LogicalVolume* logicMagneticVolumeVC =
    new G4LogicalVolume(solidMagneticVolume,G4Material::GetMaterial("Vacuum"),"MagneticVolumeVC",0,0,0);
  if (! fMagneticVolumeIsVisible) logicMagneticVolumeVC->SetVisAttributes(G4VisAttributes::Invisible);
  //new G4PVPlacement(0,magVolPos,logicMagneticVolumeVC,"MagneticVolumeVC",fChamberStructure->GetChamberLogicalVolume(),false,0);
  new G4PVPlacement(0,magVolPos,logicMagneticVolumeVC,"MagneticVolumeVC",logicWorld,false,0,true);

  // Create magnetic volume inside beam entrance pipe

  G4double cpzRIn = geoChamber->GetCPZRIn();
  G4double cpzLen = 46.*cm; // Length is set to not include the instrumented section of the target
  G4Tubs* cpzSolid = new G4Tubs("CPZ",0.,cpzRIn-1.*um,0.5*cpzLen,0.*deg,360.*deg);
  G4ThreeVector cpzPos(0.,0.,geoChamber->GetVCInnerFacePosZ()-0.5*cpzLen);
  G4LogicalVolume* logicMagneticVolumeCP =
    new G4LogicalVolume(cpzSolid,G4Material::GetMaterial("Vacuum"),"MagneticVolumeCP",0,0,0);
  if (! fMagneticVolumeIsVisible) logicMagneticVolumeCP->SetVisAttributes(G4VisAttributes::Invisible);
  new G4PVPlacement(0,cpzPos,logicMagneticVolumeCP,"MagneticVolumeCP",logicWorld,false,0,true);

  // Add magnetic field to volumes
  if (fEnableMagneticField) {
    //MagneticFieldSetup* magField = new MagneticFieldSetup();
    logicMagneticVolumeVC->SetFieldManager(fMagneticFieldManager->GetLocalFieldManager(),true);
    logicMagneticVolumeCP->SetFieldManager(fMagneticFieldManager->GetLocalFieldManager(),true);
  }

  // Tungsten target dump
  if (fEnableTungsten) {
    fTungstenDetector->SetMotherVolume(logicMagneticVolumeCP);
    fTungstenDetector->SetTungstenDisplacePosZ(cpzPos.z()); // Take into account magnetic volume displacement
    fTungstenDetector->CreateGeometry();
  }

  // Concrete wall at large Z
  if (fEnableWall) {
    fHallStructure->SetMotherVolume(logicWorld);
    fHallStructure->CreateGeometry();
  }

  // Magnet physical structure
  if (fEnableMagnet) { 
    //fMagnetStructure->SetMotherVolume(logicMagneticVolume);
    fMagnetStructure->SetMotherVolume(logicWorld);
    fMagnetStructure->CreateGeometry();
  }

  // Target
  if (fEnableTarget) {

    // Should target be included in the magnetic volume, do not forget to take into account its displacement
    //fTargetDetector->SetMotherVolume(logicMagneticVolumeCP);
    //fTargetDetector->SetTargetDisplacePosZ(cpzPos.z());

    fTargetDetector->SetMotherVolume(logicWorld);
    fTargetDetector->SetTargetDisplacePosZ(0.);

    fTargetDetector->CreateGeometry();
  }

  // SAC
  if (fEnableSAC) {
    fSACDetector->SetMotherVolume(logicWorld);
    fSACDetector->CreateGeometry();
  }

  //if (fEnableLAV) {
  //  fLAVDetector->SetMotherVolume(logicWorld);
  //  fLAVDetector->CreateGeometry();
  //}

  // TDump
  if (fEnableTDump) {
    fTDumpDetector->SetMotherVolume(logicWorld);
    fTDumpDetector->CreateGeometry();
  }

  // TPix
  if (fEnableTPix) {
    fTPixDetector->SetMotherVolume(logicWorld);
    // Position of TPix depends on shape of vacuum chamber
    geoTPix->SetTPixChamberWallAngle(geoChamber->GetVCBackFaceAngle());
    geoTPix->SetTPixChamberWallCorner(geoChamber->GetVCBackFaceCorner());
    fTPixDetector->CreateGeometry();
  }

  // ECal
  if (fEnableECal) {
    fECalDetector->SetMotherVolume(logicWorld);
    fECalDetector->CreateGeometry();
  }

  // PVeto
  if (fEnablePVeto) {
    fPVetoDetector->SetMotherVolume(logicMagneticVolumeVC);
    fPVetoDetector->SetPVetoDisplacePosZ(magVolPosZ);
    fPVetoDetector->CreateGeometry();
  }

  // EVeto
  if (fEnableEVeto) {
    fEVetoDetector->SetMotherVolume(logicMagneticVolumeVC);
    fEVetoDetector->SetEVetoDisplacePosZ(magVolPosZ);
    fEVetoDetector->CreateGeometry();
  }

  // HEPVeto
  if (fEnableHEPVeto) {
    fHEPVetoDetector->SetMotherVolume(logicWorld);
    // Position of HEPVeto depends on shape of vacuum chamber
    geoHEPVeto->SetHEPVetoChamberWallThickness(geoChamber->GetVCBackFaceThickness());
    geoHEPVeto->SetHEPVetoChamberWallAngle(geoChamber->GetVCBackFaceAngle());
    geoHEPVeto->SetHEPVetoChamberWallCorner(geoChamber->GetVCBackFaceCorner());
    fHEPVetoDetector->CreateGeometry();
  }

  // This code makes a full debug dump of the geometry, looking for overlaps
  // HUGE amount of output: enable at your own risk ;)
  //CheckDaughters(logicWorld,0);

  return physicWorld;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetupDetectors()
{}
 
void DetectorConstruction::DefineMaterials()
{

  // Use NIST database to create all needed materials
  G4NistManager* man = G4NistManager::Instance();
  man->SetVerbose(1);

  // Define materials already in the NIST database
  man->FindOrBuildMaterial("G4_C");                       // Carbon (Chamber)
  man->FindOrBuildMaterial("G4_Be");                      // Beam Line (Window) M. Raggi
  man->FindOrBuildMaterial("G4_W");                       // Tungsten (Chamber)
  man->FindOrBuildMaterial("G4_Al");                      // Aluminum (Chamber, Veto)
  man->FindOrBuildMaterial("G4_Fe");                      // Iron (Magnet)
  man->FindOrBuildMaterial("G4_Cu");                      // Copper (Magnet)
  man->FindOrBuildMaterial("G4_Si");                      // Silicon (TPix)
  man->FindOrBuildMaterial("G4_AIR");                     // Air (World)
  man->FindOrBuildMaterial("G4_BGO");                     // BGO (ECal)
  man->FindOrBuildMaterial("G4_CONCRETE");                // Concrete (Hall)
  man->FindOrBuildMaterial("G4_MYLAR");                   // Mylar (Chamber)
  man->FindOrBuildMaterial("G4_STAINLESS-STEEL");         // Stainless steel (Chamber)
  man->FindOrBuildMaterial("G4_POLYVINYLIDENE_FLUORIDE"); // Tedlar (ECal)
  man->FindOrBuildMaterial("G4_NEOPRENE");                // Neoprene (Magnet)
  man->FindOrBuildMaterial("G4_PLEXIGLASS");              // Plexiglass (ECal)
  //man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // Plastic scintillator (Veto)
  man->FindOrBuildMaterial("G4_POLYSTYRENE");             // Plastic scintillator (Veto)

  // Define all elements needed to define materials not in the NIST DB
  man->FindOrBuildElement("H");  // Hydrogen
  man->FindOrBuildElement("O");  // Oxygen
  man->FindOrBuildElement("N");  // Nitrogen
  man->FindOrBuildElement("C");  // Carbon
  man->FindOrBuildElement("F");  // Fluoride
  man->FindOrBuildElement("Pb"); // Lead
  man->FindOrBuildElement("Al"); // Aluminum
  man->FindOrBuildElement("Ti"); // Titanium

  // Vacuum: leave some residual air with low density (Chamber, World)
  G4Material* Vacuum = new G4Material("Vacuum",(1.290*0.000001)*mg/cm3,2); // 1mbar
  Vacuum->AddElement(G4Element::GetElement("N"),70.*perCent);
  Vacuum->AddElement(G4Element::GetElement("O"),30.*perCent);

  // Diamond (Target)
  G4Material* Diamond = new G4Material("Diamond",3.515*g/cm3,1);
  Diamond->AddElement(G4Element::GetElement("C"),1);

  // Lead fluoride PbF2 (SAC)
  G4Material* PbF2 = new G4Material("PbF2",7.77*g/cm3,2);
  PbF2->AddElement(G4Element::GetElement("Pb"),1);
  PbF2->AddElement(G4Element::GetElement("F"),2);

  // EJ510 reflective paint (ECal, SAC)
  G4Material* EJ510Paint = new G4Material("EJ510Paint",1.182*g/cm3,4);
  EJ510Paint->AddElement(G4Element::GetElement("Ti"),41.053*perCent);
  EJ510Paint->AddElement(G4Element::GetElement("C"),17.194*perCent);
  EJ510Paint->AddElement(G4Element::GetElement("H"),2.899*perCent);
  EJ510Paint->AddElement(G4Element::GetElement("O"),38.854*perCent);

  // Nomex honeycomb (ECal)
  G4Material* Nomex = new G4Material("Nomex",0.032*g/cm3,4);
  Nomex->AddElement(G4Element::GetElement("C"),13);
  Nomex->AddElement(G4Element::GetElement("H"),10);
  Nomex->AddElement(G4Element::GetElement("O"),2);
  Nomex->AddElement(G4Element::GetElement("N"),2);

  // Nomex foil(+glue) (ECal) => Should find better composition (now using that of Nomex)
  G4Material* NomexFoil = new G4Material("NomexFoil",1.07*g/cm3,4);
  NomexFoil->AddElement(G4Element::GetElement("C"),13);
  NomexFoil->AddElement(G4Element::GetElement("H"),10);
  NomexFoil->AddElement(G4Element::GetElement("O"),2);
  NomexFoil->AddElement(G4Element::GetElement("N"),2);

  // ASA (Acrylate Styrene Acrylonitrile) 3d printer plastic used for ECal support structures (ECal)
  G4Material* ASA = new G4Material("ASA",1.07*g/cm3,4);
  ASA->AddElement(G4Element::GetElement("C"),15);
  ASA->AddElement(G4Element::GetElement("H"),17);
  ASA->AddElement(G4Element::GetElement("O"),2);
  ASA->AddElement(G4Element::GetElement("N"),1);

  /*
  //--------- Materials definition ---------
  G4double a, z, density;
  G4int ncomponents, natoms;
  G4double fractionmass;  

  // A few useful elements
  G4Element* O  = new G4Element("Oxygen",   "O",  z= 8., a=16.00*g/mole);
  G4Element* N  = new G4Element("Nitrogen" ,"N" , z= 7., a=14.01*g/mole);
  G4Element* Lu = new G4Element("Lutetium", "Lu", z=71., a=174.97*g/mole);
  G4Element* Si = new G4Element("Silicon",  "Si", z=14., a=28.09*g/mole);

  // Vacuum inside vacuum chamber
  G4Material* Vacuum;
  if(IsResGasON==1) {
    Vacuum = new G4Material("Vacuum", density= (1.290*0.000001)*mg/cm3, ncomponents=2); //1mbar
    Vacuum->AddElement(N, fractionmass=70.*perCent);
    Vacuum->AddElement(O, fractionmass=30.*perCent);
  }else{
    Vacuum = new G4Material("Vacuum",z=1.,a=1.01*g/mole,universe_mean_density,kStateGas,2.73*kelvin,3.e-18*pascal);
  }

  //LSO crystals for the calorimeter L2S1O5
  G4Material* LSO = new G4Material("LSO", density=7.4*g/cm3,3);
  LSO->AddElement(Lu,2);
  LSO->AddElement(Si,1);
  LSO->AddElement(O,5);

  G4NistManager* man = G4NistManager::Instance();
  man->SetVerbose(1);
  G4Material* SiO2     = man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4Material* Air      = man->FindOrBuildMaterial("G4_AIR");
  G4Material* Pb       = man->FindOrBuildMaterial("G4_Pb");
  G4Material* PbWO4    = man->FindOrBuildMaterial("G4_PbWO4");
  G4Material* elC      = man->FindOrBuildMaterial("G4_C");
  G4Material* W        = man->FindOrBuildMaterial("G4_W");
  G4Material* Concrete = man->FindOrBuildMaterial("G4_CONCRETE");
  G4Material* Iron     = man->FindOrBuildMaterial("G4_Fe");
  G4Material* Scint    = man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  G4Material* Tedlar   = man->FindOrBuildMaterial("G4_POLYVINYLIDENE_FLUORIDE");
  G4Material* G4Vac    = man->FindOrBuildMaterial("G4_Galactic");
  G4Material* BGO      = man->FindOrBuildMaterial("G4_BGO");
  G4Material* BaF2     = man->FindOrBuildMaterial("G4_BARIUM_FLUORIDE");
  G4Material* Cu       = man->FindOrBuildMaterial("G4_Cu");
  G4Material* Al       = man->FindOrBuildMaterial("G4_Al");
  G4Material* Silicon  = man->FindOrBuildMaterial("G4_Si");
  G4Material* Neoprene = man->FindOrBuildMaterial("G4_NEOPRENE");
  G4Material* Mylar = man->FindOrBuildMaterial("G4_MYLAR");
  G4Material* Plexiglass = man->FindOrBuildMaterial("G4_PLEXIGLASS");
  G4Material* StainlessSteel = man->FindOrBuildMaterial("G4_STAINLESS-STEEL");
  G4Material* Oxygen = man->FindOrBuildMaterial("G4_O");
  G4Material* Hydrogen = man->FindOrBuildMaterial("G4_H");
  G4Material* Titanium = man->FindOrBuildMaterial("G4_Ti");

  //Materials for SF57

  //  G4Material* Na2O     = man->FindOrBuildMaterial("G4_CALCIUM_OXIDE");

  std::vector<G4String> SiO2El(2);
  std::vector<G4int> SiO2NA(2);
  SiO2El[0] = "Si"; SiO2NA[0] = 1;
  SiO2El[1] = "O";  SiO2NA[1] = 2;
  man->ConstructNewMaterial("SAC_SiO2",SiO2El,SiO2NA,2.32,true);

  // PbO
  std::vector<G4String> PbOEl(2);
  std::vector<G4int> PbONA(2);
  PbOEl[0] = "Pb"; PbONA[0] = 1;
  PbOEl[1] = "O";  PbONA[1] = 1;
  man->ConstructNewMaterial("PbO",PbOEl,PbONA,9.53,true);

  // Na2O
  std::vector<G4String> Na2OEl(2);
  std::vector<G4int> Na2ONA(2);
  Na2OEl[0] = "Na"; Na2ONA[0] = 2;
  Na2OEl[1] = "O";  Na2ONA[1] = 1;
  man->ConstructNewMaterial("Na2O",Na2OEl,Na2ONA,2.27,true);

  // K2O
  std::vector<G4String> K2OEl(2);
  std::vector<G4int> K2ONA(2);
  K2OEl[0] = "K"; K2ONA[0] = 2;
  K2OEl[1] = "O"; K2ONA[1] = 1;
  man->ConstructNewMaterial("K2O",K2OEl,K2ONA,2.32,true);

  // Lead fluoride PbF2
  man->FindOrBuildElement("Pb");
  man->FindOrBuildElement("F");
  G4Material* PbF2 = new G4Material("PbF2",7.77*g/cm3,2);
  PbF2->AddElement(G4Element::GetElement("Pb"),84.5*perCent);
  PbF2->AddElement(G4Element::GetElement("F"), 15.5*perCent);

  // Lead Glass (PbGl)
  G4Material* PbGl = new G4Material("PbGl_SF57",5.57*g/cm3,4);
  PbGl->AddMaterial(G4Material::GetMaterial("SAC_SiO2"), 24.0*perCent); // 22-26%
  PbGl->AddMaterial(G4Material::GetMaterial("PbO"),  74.0*perCent);     // 72-76%
  PbGl->AddMaterial(G4Material::GetMaterial("Na2O"),  1.0*perCent);     // 0.5-1.5%
  PbGl->AddMaterial(G4Material::GetMaterial("K2O"),   1.0*perCent);     // 0.5-1.5%

  //Lead Glass SF57 optical properties
  const G4int nEntries = 10;
  G4double PhotonEnergy[nEntries] =
    { 1.88914*eV, 1.92582*eV, 1.95929*eV, 2.10392*eV, 2.11001*eV,
      2.27035*eV, 2.55059*eV, 2.58300*eV, 2.84497*eV, 3.06360*eV};
  G4double RefractiveIndexPbGl[nEntries] =
    { 1.83650,    1.83808,    1.83957,    1.84636,    1.84666,
      1.85504,    1.87204,    1.87425,    1.89393,    1.91366 };
  G4double AbsorptionPbGl[nEntries] =
    {   420.*cm,    420.*cm,    420.*cm,    420.*cm,    420.*cm,
        420.*cm,    144.*cm,    130.*cm,     34.*cm,      8.*cm };
  // Define property table for Lead Glass
  G4MaterialPropertiesTable* myMPTPbGl = new G4MaterialPropertiesTable();
  myMPTPbGl->AddProperty("RINDEX",PhotonEnergy, RefractiveIndexPbGl, nEntries);
  myMPTPbGl->AddProperty("ABSLENGTH",PhotonEnergy, AbsorptionPbGl, nEntries);
  PbGl->SetMaterialPropertiesTable(myMPTPbGl);

  // Kevlar used for ECal window
  G4Material* Kevlar = new G4Material("Kevlar",1.44*g/cm3,4);
  Kevlar -> AddElement(G4Element::GetElement("H"),0.04);
  Kevlar -> AddElement(G4Element::GetElement("C"),0.71);
  Kevlar -> AddElement(G4Element::GetElement("O"),0.12);
  Kevlar -> AddElement(G4Element::GetElement("N"),0.13);

  // Diamond material to be used by target
  G4Material* Diamond = new G4Material("Diamond",3.515*g/cm3,1);
  Diamond->AddElement(G4Element::GetElement("C"),100.*perCent);

  // EJ510 reflective paint
  G4Material* EJ510Paint = new G4Material("EJ510Paint",1.182*g/cm3,4);
  EJ510Paint->AddElement(G4Element::GetElement("Ti"),41.08*perCent);
  EJ510Paint->AddElement(G4Element::GetElement("C"), 17.18*perCent);
  EJ510Paint->AddElement(G4Element::GetElement("H"),  2.88*perCent);
  EJ510Paint->AddElement(G4Element::GetElement("O"), 38.86*perCent);

  */

  //Print all the materials defined.
  G4cout << G4endl << "The elements defined are : " << G4endl << G4endl;
  G4cout << *(G4Element::GetElementTable()) << G4endl;
  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  
}

void DetectorConstruction::SetTargetMaterial(G4String materialName)
{
  // search the material by its name 
  G4Material* pttoMaterial = G4Material::GetMaterial(materialName);  
  if (pttoMaterial) {
    fTargetDetector->GetTargetLogicalVolume()->SetMaterial(pttoMaterial); 
    G4cout << "\n----> The Target material is now " << materialName << G4endl;
  } else {
    G4cout << "\n----> WARNING: attempt to use unknown material " << materialName << " for Target" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void DetectorConstruction::SetMaxStep(G4double maxStep)
//{
//  if ((stepLimit)&&(maxStep>0.)) stepLimit->SetMaxAllowedStep(maxStep);
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double DetectorConstruction::GetECalFrontFaceZ()
{
  if (fEnableECal) return fECalDetector->GetECalFrontFaceZ();

  // ECal is disabled (?): return a position 300cm after the nominal position of the target
  return 230.*cm;
}


G4double DetectorConstruction::GetTargetFrontFaceZ()
{
  if (fEnableTarget) return fTargetDetector->GetTargetFrontFaceZ();

  // Target is disabled (?): return a position 50cm before the front face of the magnet yoke
  return -100.*cm;
}

G4double DetectorConstruction::GetTargetThickness()
{
  if (fEnableTarget) return fTargetDetector->GetTargetThickness();

  // Target is disabled (?): return 100um
  return 0.1*mm;
}

void DetectorConstruction::EnableSubDetector(G4String det)
{
  printf("Enabling subdetector %s\n",det.data());
  if      (det=="ECal")    { fEnableECal    = 1; }
  else if (det=="Target")  { fEnableTarget  = 1; }
  else if (det=="SAC")     { fEnableSAC     = 1; }
  else if (det=="LAV")     { fEnableLAV     = 1; }
  else if (det=="PVeto")   { fEnablePVeto   = 1; }
  else if (det=="EVeto")   { fEnableEVeto   = 1; }
  else if (det=="HEPVeto") { fEnableHEPVeto = 1; }
  else if (det=="TDump")   { fEnableTDump   = 1; }
  else if (det=="TPix")    { fEnableTPix    = 1; }
  else if (det=="Tungsten"){ fEnableTungsten= 1; }
  else { printf("WARNING: request to enable unknown subdetector %s\n",det.data()); }
}

void DetectorConstruction::DisableSubDetector(G4String det)
{
  printf("Disabling subdetector %s\n",det.data());
  if      (det=="ECal")    { fEnableECal    = 0; }
  else if (det=="Target")  { fEnableTarget  = 0; }
  else if (det=="SAC")     { fEnableSAC     = 0; }
  else if (det=="LAV")     { fEnableLAV     = 0; }
  else if (det=="PVeto")   { fEnablePVeto   = 0; }
  else if (det=="EVeto")   { fEnableEVeto   = 0; }
  else if (det=="HEPVeto") { fEnableHEPVeto = 0; }
  else if (det=="TDump")   { fEnableTDump   = 0; }
  else if (det=="TPix")    { fEnableTPix    = 0; }
  else if (det=="Tungsten"){ fEnableTungsten= 0; }
  else { printf("WARNING: request to disable unknown subdetector %s\n",det.data()); }
}

void DetectorConstruction::EnableStructure(G4String str)
{
  printf("Enabling structure %s\n",str.data());
  if      (str=="Wall")     { fEnableWall    = 1; }
  else if (str=="Chamber")  { fEnableChamber = 1; }
  else if (str=="BemaLine") { fEnableBeamLine = 1; } 
  else if (str=="Magnet")   { fEnableMagnet  = 1; }
  else { printf("WARNING: request to enable unknown structure %s\n",str.data()); }
}

void DetectorConstruction::DisableStructure(G4String str)
{
  printf("Disabling structure %s\n",str.data());
  if      (str=="Wall")     { fEnableWall    = 0; }
  else if (str=="Chamber")  { fEnableChamber = 0; }
  else if (str=="BeamLine") { fEnableBeamLine = 0; }
  else if (str=="Magnet")   { fEnableMagnet  = 0; }
  else { printf("WARNING: request to disable unknown structure %s\n",str.data()); }
}

void DetectorConstruction::EnableMagneticField()
{
  printf("Enabling magnetic field\n");
  fEnableMagneticField = 1;
}

void DetectorConstruction::DisableMagneticField()
{
  printf("Disabling magnetic field\n");
  fEnableMagneticField = 0;
}

void DetectorConstruction::MagneticVolumeIsVisible()
{
  printf("Magnetic volume is visible\n");
  fMagneticVolumeIsVisible = 1;
}

void DetectorConstruction::MagneticVolumeIsInvisible()
{
  printf("Magnetic volume is invisible\n");
  fMagneticVolumeIsVisible = 0;
}

void DetectorConstruction::SetMagFieldValue(G4double v)
{
  printf("Setting constant value of magnetic field to %f\n",v);
  fMagneticFieldManager->SetMagneticFieldValue(v);
}

void DetectorConstruction::ChamberIsVisible()
{
  printf("Vacuum chamber is visible\n");
  fChamberIsVisible = 1;
}
//M. Raggi 07/03/2019
void DetectorConstruction::BeamLineIsVisible()
{
  printf("BeamLine is visible\n");
  fBeamLineIsVisible = 1;
}

void DetectorConstruction::ChamberIsInvisible()
{
  printf("Vacuum chamber is invisible\n");
  fChamberIsVisible = 0;
}

void DetectorConstruction::BeamLineIsInvisible()
{
  printf("Beam Line is invisible\n");
  fBeamLineIsVisible = 0;
}

void DetectorConstruction::WorldIsAir()
{
  printf("World and magnetic volume are filled with air\n");
  fWorldIsFilledWithAir = 1;
}

void DetectorConstruction::WorldIsVacuum()
{
  printf("World and magnetic volume are filled with vacuum (low pressure air)\n");
  fWorldIsFilledWithAir = 0;
}

void DetectorConstruction::CheckDaughters(G4LogicalVolume* mother,G4int lvl)
{

  // This code makes a full debug of the geometry, looking for overlaps

  G4String indent = "";
  for(G4int l=0;l<lvl;l++) indent += "\t";
  lvl++;

  G4int nD = mother->GetNoDaughters();
  for(G4int iD = 0; iD<nD;iD++) {
    G4VPhysicalVolume* daughter = mother->GetDaughter(iD);
    G4LogicalVolume* daughter_log = daughter->GetLogicalVolume();
    printf("%s%s %s\n",indent.data(),daughter_log->GetName().data(),daughter_log->GetMaterial()->GetName().data());
    if (daughter->CheckOverlaps(1000,0.,false)) {
      printf("%s - WARNING - overlaps found in %s\n",indent.data(),daughter_log->GetName().data());
    }
    CheckDaughters(daughter_log,lvl);
  }

}
