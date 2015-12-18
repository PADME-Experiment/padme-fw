#ifndef _MY_EVENT_GENERATOR_
#define _MY_EVENT_GENERATOR_


#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4Decay.hh"
#include "G4ThreeVector.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "MyParticle.hh"



//using namespace std;

//class  MyParticle;

class MyEventGenerator {
  
public:
  MyEventGenerator();
  ~MyEventGenerator();


  void SetUbosonMass(double val){MU = val;}
  double GetUbosonMass(){return MU;}

  void SetUbosonEps(double val){eps = val;}
  double GetUbosonEps(){return eps;}


  void SetBeamEnergy(double val){positronE = val;}
  double GetBeamEnergy() {return positronE;}

  void AddPrimary(MyParticle &part) {primaries.push_back(part);};
  void AddProduct(MyParticle &part) {products.push_back(part);};

  void GenerateEvent(int type,double px=0.,double py=0.,double pz=1.);
  
  void CreateInitialState(double dir[3]);
  void CreateIntermediateState();
  void CreateFinalState(int type);

  void CreateFinalStateUboson();
  void CreateFinalState3Photon();





  void ClearEvent();


  std::vector<MyParticle>* getParticles(){return &products;};


  
  
private:
  int nPart;
  double MU;
  double eps;
  double positronE;
  double vtx[3];

  std::vector<MyParticle> primaries;
  MyParticle virtualState;
  std::vector<MyParticle> products;
};

#endif
