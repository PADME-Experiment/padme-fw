#ifndef _MY_PARTICLE_
#define _MY_PARTICLE_

#include "globals.hh"
//#include "G4String.hh"
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Units/PhysicalConstants.h"

#ifdef  G4MULTITHREADED
#include "G4MTHepRandom.hh"
#else
#endif

#define SQR(x) ((x)*(x))
//#include <string>

class MyParticle {
  
public:
  MyParticle();
  MyParticle(double mm);
  MyParticle(double mm,const double mom[3],const double prod[3]);
  ~MyParticle();
  
  MyParticle& operator= (const MyParticle& part);

  MyParticle operator+ (const MyParticle& part);
  MyParticle& operator+= (const MyParticle& part);
  
  double calcMass();
  
  double calcE();
  
  void boostCM(double pp[4]);
  void boostLAB(double pp[4]);

  void setEnergy(double val){p[0] = val;};
  double E(){return p[0];};

  void setMomentum(const double mom[3]);
  double* getMomentum(){return &(p[1]);};
  
  void set4Momentum(const double mom[4]);
  double* get4Momentum(){return p;};

  void setPVtx(const double prod[3]){setPVtx(prod[0],prod[1],prod[2]);};
  void setPVtx(double x,double y,double z) {pvtx[0]=x;pvtx[1]=y;pvtx[2]=z;};

  const double * getDVtx(){return dvtx;};
  const double * getPVtx(){return pvtx;};
  
  
  void calcMomentum(const double dir[3]);
  void calcMomentum(double x,double y,double z);
  void clear();
  
  std::string getType(){return type;};
  void setType(const char * name){type.append(name);};

  std::string getPeocess(){return process;};
  void setProcess(const char * name){process.append(name);};


  void setMass(const double val){mass = val;};
  double m(){return mass;};

  double calcTau();
  double calcTau(double width);
  

  double calcGamma(){return gamma=E()/mass;};
  double calcBeta();

  double getGamma(){return gamma;};
  double* getBeta(){return beta;};

  double calcWidthU(double eps, double mu);
  double calcWidthU();

  void setPars(double *arr,int n);
  
  void genDecayVtx();


  
private:
  double p[4];
  double pvtx[3];
  double dvtx[3];
  std::string process;

  double gamma;
  double beta[3];
  double b;
  double mass;
  double tau;            //lifetime in seconds
  double G;              //width is in MeV
  double par[4];
  std::string type;
  double mom;
};

#endif
