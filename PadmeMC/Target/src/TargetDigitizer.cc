// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-07-22
//
// --------------------------------------------------------------

#include "TargetDigitizer.hh"
#include "TargetHit.hh"
#include "TargetDigi.hh"

#include "TargetDetector.hh"
#include "TargetGeometry.hh"

#include "G4DigiManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4DCofThisEvent.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"
#include <vector>

//#include <stdio>
#include <ctime>

// ROOT file target.root
//#include "TFile.h"

#include "TH1D.h"

TargetDigitizer::TargetDigitizer(G4String name)
 :G4VDigitizerModule(name),
  digiCollectionName("TargetDigiCollection"),
  nChannels(32),
  fThreshold(4e-10),
  nTrackDiv(100.)
{
  collectionName.push_back(digiCollectionName);
  //TargetGeometry* geo = TargetGeometry::GetInstance();

}

TargetDigitizer::~TargetDigitizer()
{
  //  delete fTargetMessenger;
}


//  FAST-LIGHT version
/*
void TargetDigitizer::Digitize()
{

  //// dEdX (MeV) to Charge conversion factor
  G4double MeV2Q=2.e-8;
  G4double fCCD=0.1;
  
  // Create digi vectors
  std::vector<G4int> dChannel;
  std::vector<G4double> dEnergy;
  std::vector<G4double> dTime;
  std::vector<G4double> dNumber;
  std::vector<G4double> dChargeT; // for true charge
  std::vector<G4double> dCharge; // for measured charge (ccd+fluct)

  for (G4int i=0; i < nChannels; i++) {
    dChannel.push_back(i);
    dEnergy.push_back(0.);
    dTime.push_back(100.*ns);
    dNumber.push_back(0);
  }


  TargetDigiCollection* targetDigiCollection = new TargetDigiCollection("TargetDigitizer",digiCollectionName);

  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for Target
  G4int targetHCID = theDM->GetHitsCollectionID("TargetCollection");
  if (targetHCID<0) return;
  TargetHitsCollection* targetHC = 0;
  targetHC = (TargetHitsCollection*)(theDM->GetHitsCollection(targetHCID));


  // If hits are present, digitize them
  if (targetHC) {


    // Loop over all hits
    G4int n_hit = targetHC->entries();
    for (G4int i=0;i<n_hit;i++) {

            // getting hit positions      
      G4double hPosPreX= (*targetHC)[i]->GetLocalPosX();
      G4double hPosPreY= (*targetHC)[i]->GetLocalPosY();
      //      G4double hPosPreZ= (*targetHC)[i]->GetPosZ(); 
      G4double hPosPostX=  (*targetHC)[i]->GetLocalPosPostX();
      G4double hPosPostY=  (*targetHC)[i]->GetLocalPosPostY();
      // G4double hPosPostZ=  (*targetHC)[i]->GetGlobalPosPostZ(); //local pos is in magnetic volume

      // mapping hit channel      
      G4int hVChannel = PosToChannel(hPosPreX); // X coordinate on front face (or hit beg)
      G4int hHChannel = PosToChannel(hPosPostY); // Y coordinate on back face (or hit end)
      // checking potential signal sharing 
      G4int hVChannelPost = PosToChannel(hPosPostX); // X coordinate on back face
      G4int hHChannelPre = PosToChannel(hPosPreY); // Y coordinate on front face
      
      G4double hTime    = (*targetHC)[i]->GetTime();
      G4double hEnergy  = (*targetHC)[i]->GetEnergy();
     
      //////       HIT DIAGNOSTICS
      // hitval = 2: hit in channel on both sides
      // hitval = 1: hit in channel on one side dead zone in the other
      // hitval = 0: hit in dead zones on both sides (typical eff 2%)
      // hitval < 0: hit with signal sharing (typical eff < 1/1000)
      // hitval < 90: hit out of diamond sensor (1/500000??)

      G4int hitval=0;

      if(hVChannel==-99||hHChannel==-99){
	hitval-=99; // hit out of Diamond Sensor
      }

      if(hVChannel!=hVChannelPost){
	hitval -=10;
      }	
      else{
	if(hVChannel!=-1)
	  hitval +=1;  // hit in X channel
      }
	

      if(hHChannel!=hHChannelPre){
	hitval -=10;
      }
      else {
	if(hHChannel!=-1)
	  hitval+=1;   // hit in Y channel
      }
      
      (*targetHC)[i]->SetHitVal(hitval);
      
      
      // signal only if hit contained in both channels (X Y faces) 
      if(hitval==2){

      dEnergy[hVChannel] += hEnergy;
      dEnergy[hHChannel+nChannels/2] += hEnergy;

      if (hTime < dTime[hVChannel]) dTime[hVChannel] = hTime;
      if (hTime < dTime[hHChannel+nChannels/2]) dTime[hHChannel+nChannels/2] = hTime;

      dNumber[hVChannel]+=1;
      dNumber[hHChannel+nChannels/2]+=1;
     
      } //if hitval==2      
    }// hits loop
    
    // fill digis for all channels above fThreshold
    for (G4int i=0; i<dChannel.size(); i++){

      TargetDigi* digi = new TargetDigi();
      digi->SetChannelId(dChannel[i]);
      digi->SetTime(dTime[i]);
      digi->SetEnergy(dEnergy[i]);
      digi->SetDNumber(dNumber[i]);
      dChargeT.push_back(dEnergy[i]/MeV*MeV2Q);// eff charge converted from dedx
      dCharge.push_back(dEnergy[i]/MeV*MeV2Q*fCCD);// eff charge converted from dedx
      // add energy fluctuation  
      // add ccd effect
      if (dCharge[i]<fThreshold){
	G4cout<<"DIGIDELETED: "<<dChannel[i]<<" time: "<<dTime[i]<<" energy: "<<dEnergy[i]<<" multi: "<<dNumber[i]<<" charge: "<<dCharge[i]<<G4endl;
	//	delete digi;
      }
      else{     
	digi->SetChargeT(dChargeT[i]);
	digi->SetCharge(dCharge[i]);
	digi->Print();
	targetDigiCollection->insert(digi);
      }     
    }// fill digis in targetdigicollection    
  }
  StoreDigiCollection(targetDigiCollection);
}
*/


void TargetDigitizer::Digitize()
{

  // root file for debugging waveforms (last event)
  //fTargetFile= new TFile("target.root","UPDATE");

  // constants needed for building waveforms  -> to move somewhere else
  G4int nbins =1024; // 
  G4int nmax =1024; // 
  G4int nsteps=10;
  G4double binw =0.1/ns; 
  G4double eion = 13.60*eV;
  G4int ele=1;
  G4int hole=0;
  
  // Create digi vectors
  std::vector<G4int> dChannel;
  std::vector<G4double> dEnergy;
  std::vector<G4double> dTime;
  std::vector<G4double> dNumber;
  std::vector<G4double> dChargeT;
  std::vector<G4double> dCharge;

  std::vector<G4double> dTTCCD;
  std::vector<G4double> dTTNCCD;
  std::vector<G4double> dTTVCCD;


  for (G4int i=0; i < nChannels; i++) {
    dChannel.push_back(i);
    dEnergy.push_back(0.);
    dTime.push_back(100.*ns);
    dNumber.push_back(0);
  }

  // histograms to store waveforms
  char hnameCCD [30];
  char hnameVCCD [30];

  // array to store waveforms
  G4double** dTimeTraceCCD = new G4double*[nChannels];

  for (G4int i=0;i<nChannels;i++){

    sprintf(hnameCCD, "TimeTraceCCD_ch_%d",i);
    sprintf(hnameVCCD, "TimeTraceVCCD_ch_%d",i);
    dTraceHCCD[i]= new TH1D(hnameCCD,hnameCCD,nbins,0.,nmax);
    dTraceVHCCD[i]= new TH1D(hnameVCCD,hnameVCCD,nbins,0.,nmax);

    dTimeTraceCCD[i]= new G4double[nbins];
    for (G4int j=0; j< nbins; j++)
      dTimeTraceCCD[i][j] = 0.;	
  }
  
  TargetDigiCollection* targetDigiCollection = new TargetDigiCollection("TargetDigitizer",digiCollectionName);
  G4DigiManager* theDM = G4DigiManager::GetDMpointer();

  // Get access to hit collection for Target
  G4int targetHCID = theDM->GetHitsCollectionID("TargetCollection");
  TargetHitsCollection* targetHC = 0;
  targetHC = (TargetHitsCollection*)(theDM->GetHitsCollection(targetHCID));

  // If hits are present, digitize them
  if (targetHC) {

    // Loop over all hits
    G4int n_hit = targetHC->entries();
    for (G4int i=0;i<n_hit;i++) {

      // getting hit positions      
      G4double hPosPreX= (*targetHC)[i]->GetLocalPosX();
      G4double hPosPreY= (*targetHC)[i]->GetLocalPosY();
      G4double hPosPreZ= (*targetHC)[i]->GetPosZ(); 
      G4double hPosPostX=  (*targetHC)[i]->GetLocalPosPostX();
      G4double hPosPostY=  (*targetHC)[i]->GetLocalPosPostY();
      G4double hPosPostZ=  (*targetHC)[i]->GetGlobalPosPostZ(); //local pos is in magnetic volume

      // mapping hit channel      
      G4int hVChannel = PosToChannel(hPosPreX); // X coordinate on front face (or hit beg)
      G4int hHChannel = PosToChannel(hPosPostY); // Y coordinate on back face (or hit end)
      // checking potential signal sharing 
      G4int hVChannelPost = PosToChannel(hPosPostX); // X coordinate on back face
      G4int hHChannelPre = PosToChannel(hPosPreY); // Y coordinate on front face
      
      G4double hTime    = (*targetHC)[i]->GetTime();
      G4double hEnergy  = (*targetHC)[i]->GetEnergy();
      G4double hTrack = hPosPostZ-hPosPreZ; // z dir

     
      //////       HIT DIAGNOSTICS
      // hitval = 2: hit in channel on both sides
      // hitval = 1: hit in channel on one side dead zone in the other
      // hitval = 0: hit in dead zones on both sides (typical eff 2%)
      // hitval < 0: hit with signal sharing (typical eff < 1/1000)
      // hitval < 90: hit out of diamond sensor (1/500000??)

      G4int hitval=0;

      if(hVChannel==-99||hHChannel==-99){
	hitval-=99; // hit out of Diamond Sensor
      }

      if(hVChannel!=hVChannelPost){
	hitval -=10;
      }	
      else{
	if(hVChannel!=-1)
	  hitval +=1;  // hit in X channel
      }
	

      if(hHChannel!=hHChannelPre){
	hitval -=10;
      }
      else {
	if(hHChannel!=-1)
	  hitval+=1;   // hit in Y channel
      }
      
      (*targetHC)[i]->SetHitVal(hitval);
      

      // signal only if hit contained in both channels (X Y faces) 
      if(hitval==2){
	
	/// parameters for signal calculation in stepfun -> to be moved somewhere else
	G4int startbin = (G4int)(hTime/binw); // in ns
	G4double dd = hTrack/m; // 
	//G4int nehold = (G4int)(hTrack*36*1000);
	G4int neh = (G4int)(hEnergy/eion/nTrackDiv);
	G4double os=(70*cm+hPosPreZ)/m; // hit track offset for signal calculation

	/*  	// random CCD
	G4double Vbias = 100.; //V
	G4double detThickness =0.000100; // in M
	G4double emfp = 2.e-5;
	G4double hmfp = 2.e-5;
	G4double emobility = 0.1714; //m2/V/s
	G4double hmobility = 0.2064;
	G4double vdrifte = emobility*Vbias/detThickness;	
	G4double vdrifth = hmobility*Vbias/detThickness;

	std::vector<G4double> ekilltimes; // array of killing times for electrons of this Hit
	std::vector<G4double> hkilltimes; // array of killing times for holes of this Hit	
	for(G4int nc=0; nc<neh;nc++){
	  G4double ekill= G4RandFlat::shoot();
	  G4double hkill= G4RandFlat::shoot();
	  ekilltimes.push_back((-emfp*log(1-ekill))/vdrifte);
	  hkilltimes.push_back((-hmfp*log(1-hkill))/vdrifth);
	  }
	*/

	dEnergy[hVChannel] += hEnergy;
	dEnergy[hHChannel+nChannels/2] += hEnergy;
	if (hTime < dTime[hVChannel]) dTime[hVChannel] = hTime;
	if (hTime < dTime[hHChannel+nChannels/2]) dTime[hHChannel+nChannels/2] = hTime;

	// filling channel waveform
	// X Channels   0--15,  Y Channels   16--31
	for (G4int j=startbin; j<startbin+nsteps; j++){

	  G4double econtr = 0.;
	  G4double hcontr = 0.;
	  G4double tbin= (j-startbin)*binw;

	  for(G4int nc=0; nc<neh;nc++){		
	    econtr=stepfunCCD(tbin,neh,nc,dd,os,ele,nTrackDiv);
	    hcontr=stepfunCCD(tbin,neh,nc,dd,os,hole,nTrackDiv);
	    dTimeTraceCCD[hVChannel][j] += econtr + hcontr;
	    dTimeTraceCCD[hHChannel+nChannels/2][j] += econtr + hcontr;
	    /*	   	// random CCD
	    // calculating using array of killing times/dist
	    if(ekilltimes[nc]*1e9>=tbin) 	 
	      econtrCCD1=stepfun(tbin,neh,nc,dd,os,ele);
	    else
	      econtrCCD1=0;
	    if(hkilltimes[nc]*1e9>=tbin) 	 
	      hcontrCCD1=stepfun(tbin,neh,nc,dd,os,hole);
	    else
	      hcontrCCD1=0;
	    dTimeTraceCCD1[hVChannel][j] += econtrCCD1 + hcontrCCD1;
	    dTimeTraceCCD1[hHChannel+nChannels/2][j] += econtrCCD1 + hcontrCCD1;
	    */
	  }
	} // waveform filling	  
	dNumber[hVChannel]+=1;
	dNumber[hHChannel+nChannels/2]+=1;
      } //if hitval==2
    }// hits loop
    
    // fill digis for all channels above fThreshold
    for (G4int i=0; i<dChannel.size(); i++){

      TargetDigi* digi = new TargetDigi();
      digi->SetChannelId(dChannel[i]);
      digi->SetTime(dTime[i]);
      digi->SetEnergy(dEnergy[i]);
      digi->SetDNumber(dNumber[i]);
      for(G4int j=0; j< nbins; j++){
	// current waveform histogram
	dTraceHCCD[dChannel[i]]->Fill(j,dTimeTraceCCD[dChannel[i]][j]);	    
	dTTCCD.push_back(dTimeTraceCCD[dChannel[i]][j]);
      } 

      digi->SetTimeTrace(dTTCCD);  // raw current waveform
      dTTNCCD=AddNoise(dTTCCD);
      dTTVCCD=RCFilter(dTTNCCD);  // integrate waveform ccd func 
      digi->SetTimeTraceV(dTTVCCD);  // voltage waveform

      for(G4int j=0; j< nbins; j++){
	// voltage waveform histogram
	dTraceVHCCD[dChannel[i]]->SetBinContent(j,dTTVCCD[j]);
      } 

      dChargeT.push_back(1e-10*dTraceHCCD[dChannel[i]]->Integral());// true charge
      dCharge.push_back(1e-10*dTraceVHCCD[dChannel[i]]->Integral());// ccd func charge
       
      if (dCharge[i]/50.<fThreshold){
	G4cout<<"DIGIDELETED: "<<dChannel[i]<<" time: "<<dTime[i]<<" energy: "<<dEnergy[i]<<" multi: "<<dNumber[i]<<" charge: "<<dCharge[i]/50.<<G4endl;
	//	delete digi;
      }
      else{     
	digi->SetChargeT(dChargeT[i]);
	digi->SetCharge(dCharge[i]/50.);
	digi->Print();
	targetDigiCollection->insert(digi);
      }

      //     dTraceHCCD[dChannel[i]]->Write();
      //    dTraceVHCCD[dChannel[i]]->Write();

      delete  dTraceHCCD[dChannel[i]];
      delete  dTraceVHCCD[dChannel[i]];

      dTTCCD.clear();
      dTTVCCD.clear();
      
    }// fill digis in targetdigicollection
    
    delete dTimeTraceCCD;	

  }
  //fTargetFile->Close();
  StoreDigiCollection(targetDigiCollection);
}


G4int TargetDigitizer::PosToChannel(G4double pos)
{


  /// target pitch 1.mm 
  G4double fp=1.;
  G4double stripSize = fp*850.*um;
  //G4double stripxSize = 18.*mm;
  G4double gapSize = fp*150.*um;
  //G4double porg = 7.850*mm; // fp 2 nchannels 16
  //G4double porg = 7.925*mm; // fp 1 nchannels 32
  //G4double porg = 7.9625*mm; // fp 0.5 nchannels 64
  G4double porg = (nChannels/4.*(stripSize+gapSize)-gapSize/2.)/mm;
 
  G4double posch= pos+porg;
  
  if(posch<0||pos>porg){
    return -1;
  }
  
  for(G4int ich=0; ich<nChannels/2; ich++){
    posch -= stripSize;   
    if (posch<0){
      return ich;         
    }
    posch -= gapSize;
    if (posch<0){
      return -1;
    }

  }
  return -99;
}


std::vector<G4double> TargetDigitizer::RCFilter (std::vector<G4double>& traceI){

  //G4double Rval=50.; //ohm
  G4double Rgain=50.; //ohm
  G4double RC=20.; // ns
  G4double tstep=0.1; // ns
  G4double RCalfa = tstep/(RC+tstep); //useful costant

  std::vector<G4double> traceV;
  G4int binno= traceI.size();
  traceV.reserve(binno);
  traceV.push_back(0.);

  for (G4int i=1; i<binno; i++)
    {
      G4double val=0.;
      val= (1. -RCalfa)*traceV[i-1]+ Rgain*RCalfa*traceI[i];
      traceV.push_back(val);
    }

  return traceV;
}


// gaussian noise for 1800 e-
std::vector<G4double> TargetDigitizer::AddNoise (std::vector<G4double>& traceI){
  
  std::vector<G4double> traceN;
  G4int binno= traceI.size();
  traceN.reserve(binno);

  G4double mean = 0.0;
  G4double stddev = 0.87; // uampere noise from 1800 e-
  for (G4int i=0; i<binno; i++)
    {
      G4double rnd = G4RandGauss::shoot(mean,stddev);
      traceN.push_back(traceI[i]+rnd);
    }
  return  traceN;  
}


// signal calculation with CCD
G4double TargetDigitizer::stepfunCCD(G4double& ct, G4int& Neh, G4int& cn,G4double& cd, G4double& os, G4int& cb, G4double& nt){
  G4double electronCharge =  0.16E-6; //pC 
  G4double Vbias = 100.;
  G4double detThickness =0.000100; // in M
  //G4int Neh = (G4int)(36*detThickness*1000000);
  G4double lamdbaIonization = cd/(G4double)Neh;
  G4double emobility = 0.1714; //m2/V/s
  G4double hmobility = 0.2064;
  G4double ecurrent =  electronCharge*nt*emobility*Vbias/detThickness/detThickness;
  G4double hcurrent =  electronCharge*nt*hmobility*Vbias/detThickness/detThickness;
  G4double mobility = 0.; 
  G4double current = 0.;
  G4double emfp = 0.00002; //150micron di libero cammino medio 
  G4double hmfp = 0.00002; //150micron di libero cammino medio 
  G4double mfp = 0.;
  G4double vdriftsate = 9600000.;//cm/s electrons 
  G4double vdriftsath =14100000.;//cm/s holes 



  G4double t_rac, xk, cts; 
  xk=lamdbaIonization*cn;
  cts=ct*1E-9;
  G4double distanceToElectrod = 0.;
  if (cb){
    mobility = emobility;
    current  = ecurrent;
    distanceToElectrod = xk+os; // if electron
    mfp = emfp;
  }
  else
    {
      mobility = hmobility;
      current  = hcurrent;
      distanceToElectrod = detThickness-(xk+os); // if hole
      mfp = hmfp;
    }
  //  if  (distanceToElectrod>= mfp ) distanceToElectrod = mfp;
  
  G4double vdrift = mobility*Vbias/detThickness;
  t_rac=distanceToElectrod*detThickness/(mobility*Vbias);

  if (cb)
    {
      if (vdrift > vdriftsate) vdrift= vdriftsate;
    }
  else 
    {
      if (vdrift > vdriftsath) vdrift= vdriftsath;      
    }
  
  if(cts<=t_rac && cts>0) return 1E-6*current*exp(-vdrift*cts/mfp);
  if(cts>t_rac)         return 0.;
  return 0;

}




