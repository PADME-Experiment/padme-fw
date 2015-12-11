#include "MyParticle.hh"


MyParticle::MyParticle(){
  clear();
}

MyParticle::MyParticle(double mm){
  mass = mm;
  p[0] = mass;
  p[1]=p[2]=p[3] = 0.;
  type.clear();
}

MyParticle::MyParticle(double mm,const double momm[3],const double prod[3]){
  mass = mm;
  setMomentum(momm);
  setPVtx(prod);
  type.clear();
}

MyParticle::~MyParticle(){
  
}

//===========================================================
void MyParticle::clear() {
  mass = 0.;
  p[0] = mass;
  p[1]=p[2]=p[3] = 0.;
  pvtx[0] = pvtx[1] = pvtx[2] = 0.;
  dvtx[0] = dvtx[1] = dvtx[2] = 0.;
  b = gamma = tau = G = 0.;
  par[0] = par[1] = par[2] = par[3] = 0.;
  type.clear();
  process.clear();
}


//===========================================================
MyParticle& MyParticle::operator= (const MyParticle& part){
  mass = part.mass;
  for(int i=0;i<4;i++) {
    p[i]   = part.p[i];
    par[i] = part.par[i];
  }
  type = part.type;
  tau  = part.tau;
  G    = part.G;
  b    = part.b;
  gamma = part.gamma;
  for(int i=0;i<3;i++) {
    pvtx[i] = part.pvtx[i];
    dvtx[i] = part.dvtx[i];
    beta[i] = part.beta[i];
  }

  return *this;
}


//===========================================================
double MyParticle::calcWidthU(double eps, double mu) {

  double MASS_ELEC=CLHEP::electron_mass_c2; //in MeV
  double PI=3.1415925;
  double alpha=1./137; //Electromagnetic constant

  G = (1./3.)*alpha* eps*eps*mu * sqrt(1. - (4.*MASS_ELEC*MASS_ELEC)/(mu*mu));
  
  
  return G;
}

//===========================================================
double MyParticle::calcWidthU() {
  double eps = par[0];
  double mu = mass;

  G = calcWidthU(eps, mu);
  
  // std::cout << "Particle mass: "<< mu << "  width: " << G << " MeV" << std::endl;
  return G;
}


//===========================================================
double MyParticle::calcTau(){
  const double hbar = 6.58211928e-22; //  6.58211928e−22; //MeV * s  
  tau = hbar / G;
  
  // std::cout << "Lifetime: " << tau << " s" << std::endl;
  return tau;
}


//===========================================================
double MyParticle::calcTau(double width){
  G=width;
  return calcTau();
}

//===========================================================
double MyParticle::calcE(){
  p[0] = sqrt( (mass*mass) + (p[1]*p[1]) + (p[2]*p[2]) + (p[3]*p[3])     ) ;
  return p[0];
}
//===========================================================
double MyParticle::calcBeta(){
  for(int i=0;i<3;i++) {
    beta[i] = p[i+1]/p[0];
  }
  b = sqrt(beta[0]*beta[0] + beta[1]*beta[1] +beta[2]*beta[2]);

  // std::cout << "Energy: " << p[0] << "  Beta: " << b << " " << std::endl;

  return b;
}

//===========================================================
void MyParticle::genDecayVtx(){
  const double c = 299792458.; //in m/s
  double mean = gamma*tau;
  
  // double path = CLHEP::RandExponential::shoot(mean);
  // double path = (G4MTHepRandom::getTheEngine())-> CLHEP::RandExponential::shoot(mean);

#ifdef G4MULTITHREADED
  double rnd =(G4MTHepRandom::getTheEngine())->flat();
#else
  double rnd = CLHEP::HepRandom::getTheEngine()->flat();
#endif

  //  std::cout << "Random variable: "<< rnd << std::endl;
  
  double path = - std::log(1. - rnd) *  mean;

  //  std::cout << "Path according to exponential decay: " << b*c*path << " m" <<  std::endl;

  for (int i=0;i<3;i++) {
    dvtx[i] = beta[i]*c*path  +   pvtx[i] ;
  }
  
}

//===========================================================
void MyParticle::setPars(double *arr,int n){
  for(int i=0; (i<4 && i<n) ; i++ ) {
    par[i] = arr[i];
    // std::cout << "Set a parameter" << i << " with value: " << par[i] << std::endl;
  }
}


//===========================================================
double MyParticle::calcMass(){
  double tmp;
  tmp = p[0]*p[0];

  for(int i=1;i<4;i++) {
    tmp-=p[i]*p[i];
  }
 
  mass = copysignl(1.,tmp) *   std::sqrt( std::fabs(tmp) );

  //// std::cout << "Particle mass " << mass/MeV << "MeV" << std::endl;
  return mass;
}


//===========================================================
void MyParticle::setMomentum(const double mom[3]){
  p[1] = mom[0];
  p[2] = mom[1];
  p[3] = mom[2];
}



//===========================================================
void MyParticle::calcMomentum(const double dir[3]){
  //assuming the particle flies along Z axis
  double tmp = sqrt(SQR(dir[0]) + SQR(dir[1]) + SQR(dir[2])); 
  if(p[0] > mass) {
    double pp = std::sqrt(   SQR(p[0]) - SQR(mass)   );
    p[1] = pp*dir[0]/tmp ;
    p[2] = pp*dir[1]/tmp ;
    p[3] = pp*dir[2]/tmp ;
  } else {
    // std::cout << "Particle is not initialized properly"<<std::endl;
  }
}

//===========================================================
void MyParticle::calcMomentum(double x,double y,double z){
  double dir[3] = {x,y,z};
  calcMomentum(dir);
}
//===========================================================
MyParticle MyParticle::operator+ (const MyParticle& part){

  MyParticle res;  
  for(int i=0;i<4;i++) {
    res.p[i] = p[i] + part.p[i];
  }  
  res.calcMass();
  
  return res;
}

//===========================================================
MyParticle& MyParticle::operator+= (const MyParticle& part){

  for(int i=0;i<4;i++) {
    p[i] += part.p[i];
  }  
  return *this;
}


void MyParticle::boostCM(double pp[4]){

  double m0      = sqrt(SQR(pp[0])-SQR(pp[1])-SQR(pp[2])-SQR(pp[3]));
  double ep      = (p[0]*pp[0]-p[3]*pp[3]-p[2]*pp[2]-p[1]*pp[1])/m0;
  double cons    = (p[0]+ep)/(m0+pp[0]);

  p[1] = p[1]-cons*pp[1];
  p[2] = p[2]-cons*pp[2];
  p[3] = p[3]-cons*pp[3];
  p[0] = ep;  
}

void MyParticle::boostLAB(double pp[4]){
  double m0      = sqrt(SQR(pp[0])-SQR(pp[1])-SQR(pp[2])-SQR(pp[3]));
  double ep      = (p[0]*pp[0]+p[3]*pp[3]+p[2]*pp[2]+p[1]*pp[1])/m0;
  double cons    = (p[0]+ep)/(m0+pp[0]);

  p[1] = p[1]+cons*pp[1];
  p[2] = p[2]+cons*pp[2];
  p[3] = p[3]+cons*pp[3];
  p[0] = ep;  
  
  
}
