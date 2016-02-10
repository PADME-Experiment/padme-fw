
#include "MyEventGeneration.hh"

MyEventGenerator::MyEventGenerator()
 :virtualState(0){
}


MyEventGenerator::~MyEventGenerator(){
}


void MyEventGenerator::GenerateEvent(int type,double px,double py,double pz ){
  ClearEvent();
  double dir[3] = {px,py,pz};
  CreateInitialState(dir);
  CreateIntermediateState();
  CreateFinalState(type);
}

// void MyEventGenerator::GenerateEvent(int type){
//   ClearEvent();
//   CreateInitialState();
//   CreateIntermediateState();
//   CreateFinalState(type);
// }


void MyEventGenerator::ClearEvent(){
  primaries.clear();
  virtualState.clear();
  products.clear();
}


// void MyEventGenerator::CreateFinalState(int type){
//   if(type == 1) CreateFinalStateUboson();
//   if(type == 2)  CreateFinalState3Photon();
// }


void MyEventGenerator::CreateInitialState(double dir[3]){
  MyParticle positron(CLHEP::electron_mass_c2); //This sets the particles at rest
  MyParticle electron(CLHEP::electron_mass_c2);
    
  positron.setEnergy(positronE);
  //  positron.calcMomentum(0.,0.,1.); //this is the default by construction!
  positron.calcMomentum(dir[0],dir[1],dir[2]);
  //

  primaries.push_back(positron);
  primaries.push_back(electron); 
}


void MyEventGenerator::CreateFinalState(int type){
  if(type == 1) CreateFinalStateUboson();
  if(type == 2)  CreateFinalState3Photon();
}

void MyEventGenerator::CreateFinalState3Photon(){
  
  static int iline = 0;
  
  MyParticle gamma[3];
  for(int i=0;i<3;i++) {
    gamma[i].setMass(0.);
    gamma[i].setType("gamma");
  }
  
  std::ifstream infile;
  std::string Line = "";
  infile.open("/home/padmemc/Dropbox/btf-ubozon/btfmc/events_2_weight.txt");

  int il=0;
  while (!infile.eof() && il <= iline) {
     getline(infile, Line);
     il++;
  }
  infile.close();

  if (il == iline + 1) {
    // successfull read line ...... 
    // std::cout << "READ: " <<  Line << std::endl;
    iline ++;
    int itmp;
    double dtmp;
    double dtmp2;
    double p[3];
    
    std::istringstream iss(Line);
    
    iss >> itmp >> dtmp >> dtmp2 ;
    for(int j = 0;j<3;j++) { 
      iss >> p[0] >> p[1] >> p[2] ;
      for(int k = 0;k<3;k++ ) {
	p[k] *= GeV;
      }
      //std::cout << "Momentum " << j << ": " << p[0] << " " << p[1] << " " << p[2] <<std::endl; 
      gamma[j].setMomentum(p);
      gamma[j].calcE();
      products.push_back(gamma[j]);
    }

    
    // for(int i=0;i<3;i++) {
    //   products.push_back(gamma[i]);
    // }
    
    
  }


}

void MyEventGenerator::CreateFinalStateUboson(){
  //Really doing the decay of the particles
  
  //Define the process: X-> gU -> g e+e-
  
  MyParticle Uboson;
  MyParticle gamma;
  
  //Do the calculation in the Center of Mass system 
  Uboson.setMass(MU);
  Uboson.setType("Uboson");
  
  gamma.setMass(0.);
  gamma.setType("gamma");

  //Do the decay positronium -> gU
  //In the CM:
  
  double eu= (SQR(Uboson.m()) + SQR(virtualState.m()))/(2.*virtualState.m());
  double eg= (-SQR(Uboson.m()) + SQR(virtualState.m()))/(2.*virtualState.m());
  double pu = sqrt(SQR(eu) - SQR(Uboson.m()));
  
  Uboson.setEnergy(eu);
  gamma.setEnergy(eg);
  
  //  std::cout << "Energy of the U: " << eu << "\t Energy of g: "<< eg << std::endl;
  //  std::cout << "Momentum of the U: " << pu << "\t Momentum of g: "<< eg << std::endl;
  
  //Generate a random vector
  G4ThreeVector dir = G4RandomDirection();
  //  std::cout << "Random direction: " << dir.x() << "\t" << dir.y() << "\t" << dir.z() << std::endl;
  
  Uboson.calcMomentum(dir.x(),dir.y(),dir.z());
  gamma.calcMomentum(-dir.x(),-dir.y(),-dir.z());
  
  gamma.boostLAB(virtualState.get4Momentum());
  //  std::cout << "Gamma energy :"<< gamma.E() << std::endl;
  Uboson.boostLAB(virtualState.get4Momentum());


  //The primary decay is generated, set some parameters for U and get kinematics variables
  Uboson.calcGamma();
  Uboson.setPars(&eps,1);
  Uboson.calcWidthU();
  Uboson.calcTau();
  Uboson.calcBeta();

  //Generate a decay point
  Uboson.genDecayVtx();  

  products.push_back(gamma);
  //Now perform the decay of the U-boson, if possible:
  if(Uboson.m() > 2.*CLHEP::electron_mass_c2) {
    
    //In the CM of the U-boson:
    MyParticle ee(CLHEP::electron_mass_c2);
    ee.setType("e-");
    MyParticle ep(CLHEP::electron_mass_c2);
    ep.setType("e+");

    dir = G4RandomDirection();
    
    ee.setEnergy(Uboson.m()/2);
    ep.setEnergy(Uboson.m()/2);
    
    ee.calcMomentum(dir.x(),dir.y(),dir.z());
    ep.calcMomentum(-dir.x(),-dir.y(),-dir.z());
    
    ee.boostLAB(Uboson.get4Momentum());
    ep.boostLAB(Uboson.get4Momentum());
    
    ee.setPVtx(Uboson.getDVtx());
    ep.setPVtx(Uboson.getDVtx());

    products.push_back(ee);
    products.push_back(ep);
  }


  
}

void MyEventGenerator::CreateIntermediateState(){
  //loop over the primaries
  //virtualState.clear();
  std::vector<MyParticle>::iterator it;
  it = primaries.begin();
  while (it != primaries.end()) {
    virtualState += *it;
    it++;
  }
  virtualState.calcMass();
}

