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
#include "TFile.h"

#include "TH1D.h"

TargetDigitizer::TargetDigitizer(G4String name)
 :G4VDigitizerModule(name),
  digiCollectionName("TargetDigiCollection")
{
  collectionName.push_back(digiCollectionName);
  fTargetGeometry = TargetGeometry::GetInstance();
  //std::cout<<"--------------------------------I'm in digitizerTarget" << std::endl;
}

TargetDigitizer::~TargetDigitizer()
{
  //  delete fTargetMessenger;
}


void TargetDigitizer::Digitize()
{

  fFastDigi= fTargetGeometry->FastDigitizationIsEnabled();
  fSaveWaveforms= fTargetGeometry->SaveWaveformToDigiIsEnabled();
  fReduceWaveforms= fTargetGeometry->ReduceWaveformIsEnabled();

  ////  root file for debugging waveforms
  // fTargetFile= new TFile("target.root","UPDATE");

  // Create digi vectors
  std::vector<G4int> dChannel;
  std::vector<G4double> dEnergy;
  std::vector<G4double> dTime;
  std::vector<G4double> dNumber;
  std::vector<G4double> dChargeT;
  std::vector<G4double> dCharge;
  std::vector<G4double> dNCharge;

  std::vector<G4double> dTTCCD;// 0.1 ns res
  std::vector<G4double> dTTCCD1; // 1ns res
  std::vector<G4double> dTTNCCD;
  std::vector<G4double> dTTVCCD;
  //  std::vector<G4double> dTTV1CCD; // for CSA

  G4int nChannels=fTargetGeometry->GetTargetDigiNChannels();
  G4double fThreshold=fTargetGeometry->GetTargetDigiThreshold();

  for (G4int i=0; i < nChannels; i++) {
    dChannel.push_back(i);
    dEnergy.push_back(0.);
    dTime.push_back(100.*ns);
    dNumber.push_back(0);
  }

  // used in full mode !fFastDigi
  
  // histograms to handle waveforms
  char hnameCCD [30];
  char hnameNCCD [30];
  char hnameVCCD [30];
  
  // array to handle waveforms
  G4double** dTimeTraceCCD = new G4double*[nChannels];
  
  // constants needed for building waveforms and integrate signal

  G4int nbins = fTargetGeometry->GetTargetDigiNBins();     // def 1024
  G4double intwindow = fTargetGeometry->GetTargetDigiWindow();     // def 100*ns
  G4double trigoff = fTargetGeometry->GetTargetDigiTrigOffset();     // def 0*ns

  G4double binw =0.1; // resolution 0.1 ns; 
  G4int nhires =int(nbins/binw); // 

  // constants needed for calculating signal (move?)
  G4int nsteps=10;
  G4double eion = 13.60*eV;
  G4int ele=1;
  G4int hole=0;
 

  if (!fFastDigi)
    {
      for (G4int i=0;i<nChannels;i++){
	
	sprintf(hnameCCD, "TimeTraceCCD_ch_%d",i);
	sprintf(hnameNCCD, "TimeTraceNCCD_ch_%d",i);
	sprintf(hnameVCCD, "TimeTraceVCCD_ch_%d",i);
	//sprintf(hnameV1CCD, "TimeTraceV1CCD_ch_%d",i);
	//    sprintf(hnameVCCD1, "TimeTraceVCCD1_ch_%d",i);
	dTraceHCCD[i]= new TH1D(hnameCCD,hnameCCD,nhires,0.,nhires); // 10240 
	//    dTraceNHCCD[i]= new TH1D(hnameNCCD,hnameNCCD,nhires,0.,nhires);
	//    dTraceVHCCD[i]= new TH1D(hnameVCCD,hnameVCCD,nhires,0.,nhires);
	dTraceNHCCD[i]= new TH1D(hnameNCCD,hnameNCCD,nbins,0.,nbins); // 1024
	dTraceVHCCD[i]= new TH1D(hnameVCCD,hnameVCCD,nbins,0.,nbins);
	//dTraceV1HCCD[i]= new TH1D(hnameV1CCD,hnameV1CCD,nbins,0.,nbins);
	//    dTraceVHCCD1[i]= new TH1D(hnameVCCD1,hnameVCCD1,nbins,0.,nbins); //1024
	
	dTimeTraceCCD[i]= new G4double[nhires];
	for (G4int j=0; j< nhires; j++)
	  dTimeTraceCCD[i][j] = 0.;	
      }
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

      //G4double hPosPostX=  (*targetHC)[i]->GetLocalPosPostX();
      //G4double hPosPostY=  (*targetHC)[i]->GetLocalPosPostY();
      // Take into account possible XY shifts of Target position
      G4double hPosPostX=  (*targetHC)[i]->GetLocalPosPostX()- fTargetGeometry->GetTargetPosX();
      G4double hPosPostY=  (*targetHC)[i]->GetLocalPosPostY()- fTargetGeometry->GetTargetPosY();
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

	dNumber[hVChannel]+=1;
	dNumber[hHChannel+nChannels/2]+=1;
	
	if(!fFastDigi){
	  
	  /// parameters for signal calculation in stepfun -> to be moved somewhere else
	  G4int startbin = (G4int)(hTime/binw) * ns;
	  G4double dd = hTrack/m; // 
	  //G4int nehold = (G4int)(hTrack*36*1000);
	  G4int neh = (G4int)(hEnergy/eion/fTargetGeometry->GetTargetDigiNTrackDiv());
	  G4double os=(70*cm+hPosPreZ)/m; // hit track offset for signal calculation
	  
	  // filling channel waveform
	  // X Channels   0--15,  Y Channels   16--31
	  for (G4int j=startbin; j<startbin+nsteps; j++){
	    
	    G4double econtr = 0.;
	    G4double hcontr = 0.;
	    G4double tbin= (j-startbin)*binw/ns;
	    
	    for(G4int nc=0; nc<neh;nc++){		
	      econtr=stepfunCCD(tbin,neh,nc,dd,os,ele);
	      hcontr=stepfunCCD(tbin,neh,nc,dd,os,hole);
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
	} // if !fFastDigi
	
      } //if hitval==2
    }// hits loop
    
    // fill digis for all channels above fThreshold

    G4int chn=(int)dChannel.size();

    for (G4int i=0; i<chn; i++){
      
      TargetDigi* digi = new TargetDigi();
      digi->SetChannelId(dChannel[i]);
      digi->SetTime(dTime[i]);
      digi->SetEnergy(dEnergy[i]);
      digi->SetDNumber(dNumber[i]);
      
      if(!fFastDigi){
	for(G4int j=0; j< nhires; j++){
	  // current waveform histogram
	  dTraceHCCD[dChannel[i]]->Fill(j,dTimeTraceCCD[dChannel[i]][j]);	    
	  dTTCCD.push_back(dTimeTraceCCD[dChannel[i]][j]);
	} 
	
	dTTCCD1=NsRebin(dTTCCD);  // rebin 1ns	
	dTTNCCD=AddNoiseAndBaseline(dTTCCD1);
	//dTTVCCD=RCFilter(dTTNCCD);  // integrate waveform ccd func 
	dTTVCCD=CSAFilter(dTTNCCD);  // integrate with CSA 
	
	for(G4int j=0; j< nbins; j++){
	  // current waveform histogram
	  dTraceNHCCD[dChannel[i]]->SetBinContent(j,dTTNCCD[j]);
	  // voltage waveform histogram
	  dTraceVHCCD[dChannel[i]]->SetBinContent(j,dTTVCCD[j]);
	}	  

	// Charge is calculated as: binsize in seconds * root method Integral(range) of trace histo 
	// (same also for lowres waveform as NsRebin sums the signal in 10 bins)

	dNCharge.push_back(1e-10*dTraceNHCCD[dChannel[i]]->Integral(nbins-intwindow,nbins));// noise charge 1ns res
	dChargeT.push_back(1e-10*dTraceHCCD[dChannel[i]]->Integral(trigoff/binw,intwindow/binw));// true charge 0.1 nsres
	dCharge.push_back(1e-10/50*(dTraceVHCCD[dChannel[i]]->Integral(trigoff,intwindow))); // ccd func integrated (CSAFilter) 1ns res (check factor 50 ohm)
		
      }// if !fFastDigi
      else{
	//// FAST DIGI
	
	//// dEdX (MeV) to Charge conversion factor obtained in FULL mode
	G4double fMeV2Q=fTargetGeometry->GetTargetDigiMeV2Q();	

	//// Charge Collection factor 
	// G4double fCCD=fTargetGeometry->GetTargetDigiCCD();
  
	//// MC noise charge RMS obtained from 0 multiplicity channels in FULL mode
	G4double fNoiseChargeRMS=fTargetGeometry->GetTargetDigiNoiseChargeRMS(); 

	G4double nran=G4RandGauss::shoot(0.,fNoiseChargeRMS);
	dNCharge.push_back(nran); 
	dChargeT.push_back(dEnergy[i]/MeV*fMeV2Q);// charge converted from G4 dEdX	
	dCharge.push_back(dEnergy[i]/MeV*fMeV2Q+nran);// eff charge  
      }
  
      
      if (dCharge[i]<fThreshold){	
	// G4cout<<"DIGIDELETED - channel: "<<dChannel[i]<<" time: "<<dTime[i]<<" energy: "<<dEnergy[i]<<" multi: "<<dNumber[i]<<" charge: "<<dCharge[i]<<" noisecharge: "<<dNCharge[i]<<G4endl;
	////	delete digi;

      }
      else{     

	if(!fFastDigi){
	  if (fReduceWaveforms){
	    dTTCCD1.erase(dTTCCD1.begin(),dTTCCD1.begin()+trigoff); // delete offset
	    dTTCCD1.resize(intwindow); // save only intwindow

	  }
	  digi->SetWaveformRaw(dTTCCD1);  // raw current waveform (no noise, 1ns res)
	  
	  if (fReduceWaveforms){	  
	    dTTVCCD.erase(dTTVCCD.begin(),dTTVCCD.begin()+trigoff); // delete offset
	    dTTVCCD.resize(intwindow); // save only intwindow
	  } 
	  digi->SetWaveform(dTTVCCD);  // voltage waveform (after noise and CSA filter, 1ns res)
	}
	digi->SetChargeT(dChargeT[i]);
	digi->SetCharge(dCharge[i]);
	digi->SetNoiseCharge(dNCharge[i]); 
	//	digi->Print();
	targetDigiCollection->insert(digi);
      }

      if (!fFastDigi) {
	if(dNumber[i]){
	  //// root file for debugging traces
	  dTraceHCCD[dChannel[i]]->Write();
	  dTraceNHCCD[dChannel[i]]->Write();
	  dTraceVHCCD[dChannel[i]]->Write();
	  // dTraceV1HCCD[dChannel[i]]->Write();
	  ////dTraceVHCCD1[dChannel[i]]->Write();
	  
	}
	delete  dTraceHCCD[dChannel[i]];
	delete  dTraceNHCCD[dChannel[i]];
	delete  dTraceVHCCD[dChannel[i]];
      }
      
       
      
      dTTCCD.clear();
      dTTCCD1.clear();
      dTTNCCD.clear();
      dTTVCCD.clear();

    }// fill digis in targetdigicollection
    
    delete dTimeTraceCCD;	    
  }
  //// root file for debuggind traces
  // fTargetFile->Close();
  StoreDigiCollection(targetDigiCollection);

  dChannel.clear();
  dEnergy.clear();
  dTime.clear();
  dNumber.clear();
  dChargeT.clear();
  dCharge.clear();
  dNCharge.clear();

}
  
  
  G4int TargetDigitizer::PosToChannel(G4double pos)
  {
    
    fTargetGeometry = TargetGeometry::GetInstance();
    G4int nChannels=fTargetGeometry->GetTargetDigiNChannels();
    G4double stripSize=fTargetGeometry->GetTargetStripSize();
    G4double gapSize=fTargetGeometry->GetTargetGapSize();
    
  
  /// target pitch 1.mm 
  //G4double fp=1.;
  // G4double stripSize = fp*850.*um;
  //G4double stripxSize = 18.*mm;
  //G4double gapSize = fp*150.*um;
  //G4double porg = 7.850*mm; // fp 2 nchannels 16
  //G4double porg = 7.925*mm; // fp 1 nchannels 32
  //G4double porg = 7.9625*mm; // fp 0.5 nchannels 64
  G4double porg = (nChannels/4.*(stripSize+gapSize)-gapSize/2.)/mm; 
  //  G4double porgmm = (nChannels/4.*(stripSize+gapSize)-gapSize/2.); 
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
  //G4double tstep=0.1; // ns resolution of waveform
  G4double tstep=1.; // ns resolution of waveform
  G4double RCalfa = tstep/(RC+tstep); // useful costant

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




std::vector<G4double> TargetDigitizer::CSAFilter (std::vector<G4double>& traceI){

  std::vector<G4double> traceV;
  G4int binno= traceI.size();
  traceV.reserve(binno);
  traceV.push_back(0.);

  G4double Tr=10.;
  G4double Tf=20.;
  G4double T0=0.;
  //  G4double G0=1.;
  G4double G0=4.;

  for(int i=1; i<binno; i++){
    T0    = double (i);
    G4double Vout = 0.;
    for(G4int k=0; k<binno; k++){   
      
      G4double V0=0;
      G4double t= double(k);
      if(t-T0<0){
	G4double exp_r = (t-T0)/Tr;
	G4double exp_f = (t-T0)/Tf;
	V0    = G0 * ( 1. - exp(exp_r))*exp(exp_f);
	Vout  += V0 * traceI[k];
      }    
    }
    traceV.push_back(Vout);
  }
  return traceV;
}





std::vector<G4double> TargetDigitizer::NsRebin (std::vector<G4double>& traceI){

  G4int tstep=10; // 0.1->1 ns rebin!
  std::vector<G4double> traceRI;
  //G4int binI= traceI.size();
  G4int binRI= int(traceI.size()/tstep);
  traceRI.reserve(binRI);
  for (G4int i=0; i<binRI; i++)
    {
      G4double val=0.;
      for(int j=0;j<tstep;j++){
	val+= traceI[i*tstep+j];
      }
      traceRI.push_back(val);
    }
  return traceRI;
}



// gaussian noise for 1800 e-
std::vector<G4double> TargetDigitizer::AddNoiseAndBaseline(std::vector<G4double>& traceI){

  fTargetGeometry = TargetGeometry::GetInstance();
  G4double noisestddev = sqrt(10.)*fTargetGeometry->GetTargetDigiNoiseRMS(); // factor sqrt(10.) as noise is put on 1ns trace 
  G4double baseline = fTargetGeometry->GetTargetDigiBaseline(); 
  // G4double noisemean = 0.0;
  //G4double stddev = 0.87; // uampere noise from 1800 e-
  //G4double stddev = 2.42; // uampere noise from 5000 e-

  std::vector<G4double> traceN;
  G4int binno= traceI.size();
  traceN.reserve(binno);
      
  for (G4int i=0; i<binno; i++)
    {
      G4double rnd = G4RandGauss::shoot(baseline,noisestddev);
      traceN.push_back(traceI[i]+rnd); // keep up- and downward fluctuations
    }
  return  traceN;  
}


// signal calculation with CCD
G4double TargetDigitizer::stepfunCCD(G4double& ct, G4int& Neh, G4int& cn,G4double& cd, G4double& os, G4int& cb){

  fTargetGeometry = TargetGeometry::GetInstance();
  G4int ntd=fTargetGeometry->GetTargetDigiNTrackDiv();

  G4double electronCharge =  0.16E-6; //pC 
  G4double Vbias = 100.;
  G4double detThickness =0.000100; // in M
  //G4int Neh = (G4int)(36*detThickness*1000000);
  G4double lamdbaIonization = cd/(G4double)Neh;
  G4double emobility = 0.1714; //m2/V/s
  G4double hmobility = 0.2064;
  G4double ecurrent =  electronCharge*ntd*emobility*Vbias/detThickness/detThickness;
  G4double hcurrent =  electronCharge*ntd*hmobility*Vbias/detThickness/detThickness;
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




