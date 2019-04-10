// --------------------------------------------------------------
// History:
//
// Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2016-03-23
//
// --------------------------------------------------------------
#include "Riostream.h"

#include "TargetReconstruction.hh"
#include "TTargetRecoBeam.hh"

#include "TTargetMCEvent.hh"
#include "TTargetMCHit.hh"
#include "TTargetMCDigi.hh"
#include "DigitizerChannelTarget.hh"
#include "TTargetSignalFitParams.hh"
#include "TH2F.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"

 
TargetReconstruction::TargetReconstruction(TFile* HistoFile, TString ConfigFileName)
  : PadmeVReconstruction(HistoFile, "Target", ConfigFileName)
{
  //fRecoEvent = new TRecoTargetEvent();
  //ParseConfFile(ConfigFileName);
  fChannelReco    = new DigitizerChannelTarget();
  fTargetRecoBeam = new TTargetRecoBeam();  
  fTriggerProcessor = new PadmeVTrigger();

  fWriteFitParams = false;
  fWriteFitParams = (Bool_t)fConfig->GetParOrDefault("Output", "SignalFitParams"      , 0 );
  fWriteTargetBeam = true;
  fWriteTargetBeam = (Bool_t)fConfig->GetParOrDefault("Output", "Beam"      , 1 );

}

void TargetReconstruction::HistoInit(){

  // a service histigram, no need to save it to root output 
  hprofile = new TH1F("hprofile","hprofile",16,-7.5,8.5);

  // histos to be saved 
  AddHisto("TargetBeamMultiplicity", new TH1F("TargetBeamMultiplicity" ,"Target Beam Multiplicity" ,  300,   0., 30000.          ));
  AddHisto("TargetProfileX"        , new TH1F("TargetProfileX"         ,"Target profile X"         ,   16, -7.5, 8.5             ));
  AddHisto("TargetProfileY"        , new TH1F("TargetProfileY"         ,"Target profile Y"         ,   16, -7.5, 8.5             ));
  AddHisto("TargetProfileLastX"    , new TH1F("TargetProfileLastX"     ,"Target last profile X"    ,   16, -7.5, 8.5             ));
  AddHisto("TargetProfileLastY"    , new TH1F("TargetProfileLastY"     ,"Target last profile Y"    ,   16, -7.5, 8.5             ));
  AddHisto("TargetProfileCNSX"     , new TH1F("TargetProfileCNSX"      ,"Target NOCNS profile X"   ,   16, -7.5, 8.5             ));
  AddHisto("TargetProfileCNSY"     , new TH1F("TargetProfileCNSY"      ,"Target NOCNS profile Y"   ,   16, -7.5, 8.5             ));
  AddHisto("TargetXYmap"           , new TH2F("TargetXYmap"            ,"Target XY map"            ,   16, -7.5, 8.5, 16,-7.5,8.5));
  AddHisto("TargetBeamSpot"        , new TH2F("TargetBeamSpot"         ,"Target beam spot"         ,  160, -7.5, 8.5,160,-7.5,8.5));
  
  
  (GetHisto("TargetBeamMultiplicity"))->GetXaxis()->SetTitle("Number of e^{+}");
  (GetHisto("TargetBeamMultiplicity"))->GetYaxis()->SetTitle("Counts/100e^{+}");
 
  (GetHisto("TargetProfileX"))->GetXaxis()->SetTitle("x[mm]");
  (GetHisto("TargetProfileX"))->GetYaxis()->SetTitle("MIPs Multiplicity");
 
  (GetHisto("TargetProfileLastX"))->GetXaxis()->SetTitle("x[mm]");
  (GetHisto("TargetProfileLastX"))->GetYaxis()->SetTitle("MIPs Multiplicity");
 
  (GetHisto("TargetProfileCNSX"))->GetXaxis()->SetTitle("x[mm]");
  (GetHisto("TargetProfileCNSX"))->GetYaxis()->SetTitle("MIPs Multiplicity");
   
  (GetHisto("TargetProfileY"))->GetXaxis()->SetTitle("y[mm]");
  (GetHisto("TargetProfileY"))->GetYaxis()->SetTitle("MIPs Multiplicity");
   
  (GetHisto("TargetProfileLastY"))->GetXaxis()->SetTitle("y[mm]");
  (GetHisto("TargetProfileLastY"))->GetYaxis()->SetTitle("MIPs Multiplicity");
   
  (GetHisto("TargetProfileCNSY"))->GetXaxis()->SetTitle("y[mm]");
  (GetHisto("TargetProfileCNSY"))->GetYaxis()->SetTitle("MIPs Multiplicity");

  ((TH2F *)GetHisto("TargetXYmap"))->GetXaxis()->SetTitle("x[mm]");
  ((TH2F *)GetHisto("TargetXYmap"))->GetYaxis()->SetTitle("y[mm]");

  ((TH2F *)GetHisto("TargetBeamSpot"))->GetXaxis()->SetTitle("x[mm]");
  ((TH2F *)GetHisto("TargetBeamSpot"))->GetYaxis()->SetTitle("y[mm]");

  //Waveform and Q histograms
  for(int iCh=0; iCh!=32 ; iCh++){
    char iName[100];

    sprintf(iName,"TargetCh%d",iCh);
    AddHisto(iName, new TH1F(iName, iName,  1024,  0, 1024 ));
    (GetHisto(iName))->GetXaxis()->SetTitle("Sampling #");
    (GetHisto(iName))->GetYaxis()->SetTitle("Amplitude[V]");

    sprintf(iName,"TargetLastCh%d",iCh);
    AddHisto(iName, new TH1F(iName, iName,  1024,  0, 1024 ));
    (GetHisto(iName))->GetXaxis()->SetTitle("Sampling #");
    (GetHisto(iName))->GetYaxis()->SetTitle("Amplitude[V]");

    sprintf(iName,"TargetNOCNSCh%d",iCh);
    AddHisto(iName, new TH1F(iName, iName,  1024,  0, 1024 ));
    (GetHisto(iName))->GetXaxis()->SetTitle("Sampling #");
    (GetHisto(iName))->GetYaxis()->SetTitle("Amplitude[V]");

    sprintf(iName,"TargetNOCNSLastCh%d",iCh);
    AddHisto(iName, new TH1F(iName, iName,  1024,  0, 1024 ));
    (GetHisto(iName))->GetXaxis()->SetTitle("Sampling #");
    (GetHisto(iName))->GetYaxis()->SetTitle("Amplitude[V]");

    sprintf(iName,"TargetQCh%d",iCh);
    AddHisto(iName, new TH1F(iName, iName,  200,  0, 2000 ));
    (GetHisto(iName))->GetXaxis()->SetTitle("Number of e^{+}");
    (GetHisto(iName))->GetYaxis()->SetTitle("Counts/10e^{+}");

  }


}


TargetReconstruction::~TargetReconstruction()
{
  //delete hprofile;
}

// void TargetReconstruction::Init(PadmeVReconstruction* MainReco)
// {

//   //common part for all the subdetectors
//   PadmeVReconstruction::Init(MainReco);

// }

// // Read Target reconstruction parameters from a configuration file
// void TargetReconstruction::ParseConfFile(TString ConfFileName) {

//   std::ifstream confFile(ConfFileName.Data());
//   if (!confFile.is_open()) {
//     perror(ConfFileName);
//     exit(1);
//   }

//   TString Line;
//   while (Line.ReadLine(confFile)) {
//     if (Line.BeginsWith("#")) continue;
//   }
//   confFile.close();
// }

void TargetReconstruction::ProcessEvent(TMCVEvent* tEvent, TMCEvent* tMCEvent)
{
  PadmeVReconstruction::ProcessEvent(tEvent,tMCEvent);
  TTargetMCEvent* tTargetEvent = (TTargetMCEvent*)tEvent;
  std::cout << "--- TargetReconstruction --- run/event/#hits/#digi " << tTargetEvent->GetRunNumber() << " " << tTargetEvent->GetEventNumber() << " " << tTargetEvent->GetNHits() << " " << tTargetEvent->GetNDigi() << std::endl;
  for (Int_t iH=0; iH<tTargetEvent->GetNHits(); iH++) {
    TTargetMCHit* hit = (TTargetMCHit*)tTargetEvent->Hit(iH);
    hit->Print();
  }
  for (Int_t iD=0; iD<tTargetEvent->GetNDigi(); iD++) {
    TTargetMCDigi* digi = (TTargetMCDigi*)tTargetEvent->Digi(iD);
    digi->Print();
  }
}


void TargetReconstruction::ProcessEvent(TRawEvent* rawEv){
  //std::cout<<this->GetName()<<"::ProcessEvent  ........... "<<std::endl;
  PadmeVReconstruction::BuildHits(rawEv);
  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetRecoHits());
  ReconstructBeam();
  RetrieveSignalFitParams();
  AnalyzeEvent(rawEv);
}
void TargetReconstruction::ProcessEvent(TRecoVObject* recoObj, TRecoEvent* tRecoEvent){

  //std::cout<<this->GetName()<<"::ProcessEvent  ........... "<<std::endl;

  PadmeVReconstruction::ReadHits(recoObj,tRecoEvent);
  //std::cout<<this->GetName()<<"::ProcessEvent  ........... HITS read"<<std::endl;

  // placeholder for hit calibration
  if(fChannelCalibration) fChannelCalibration->PerformCalibration(GetRecoHits());

  ReconstructBeam();
  //std::cout<<this->GetName()<<"::ProcessEvent  ........... Beam reconstructed "<<std::endl;

  RetrieveSignalFitParams();
  //std::cout<<this->GetName()<<"::ProcessEvent  ........... Fit Beam reconstructed "<<std::endl;
}

void TargetReconstruction::RetrieveSignalFitParams()
{
  //std::cout<<this->GetName()<<"::RetrieveSignalFitParams"<<std::endl;
  if (!fChannelReco) return;

  //Perform some cleaning before:
  vector<TTargetSignalFitParams *> &fitPars  = getSignalFitParams();
  for(unsigned int iHit = 0;iHit < fitPars.size();iHit++){
    delete fitPars[iHit];
  }

  fitPars.clear();
  for(Int_t i = 0; i < 32; ++i) {
    TTargetSignalFitParams* fitP = new TTargetSignalFitParams();
    fitP->setChi2(1.01);
    fitP->setNdf(3);
    fitPars.push_back(fitP);
    fitPars[i]->setIdWFfitFuncion(i);
    //std::cout<<i<< " TTargetSignalFitParams created in memory so far ... latest one has id="<<i<<std::endl;
  }
  
}

void TargetReconstruction::EndProcessing()
{  
  cout << "Entering End processing for " << this->GetName() << endl;

  //Correct for dead strips
  float  cont1  =  (GetHisto("TargetProfileX")) -> GetBinContent( 1);
  float  cont3  =  (GetHisto("TargetProfileX")) -> GetBinContent( 3);
  float  cont5  =  (GetHisto("TargetProfileX")) -> GetBinContent( 5);
  float cont15  =  (GetHisto("TargetProfileX")) -> GetBinContent(15);
  (GetHisto("TargetProfileX")) -> SetBinContent( 2, (cont1 + cont3)/2);
  (GetHisto("TargetProfileX")) -> SetBinContent( 4, (cont3 + cont5)/2);
  (GetHisto("TargetProfileX")) -> SetBinContent(16,          cont15/2);
 
  //Create 2D map assuming PDF(X,Y)=PDF(X)*PDF(Y)  
  float Qx= (GetHisto("TargetProfileX"))-> Integral();
  float Qy= (GetHisto("TargetProfileY"))-> Integral();
    //cout << "Qx "<< Qx << " Qy "<< Qy <<endl;

  if(Qx!=0&&Qy!=0)for(unsigned int iHitx = 0; iHitx < 16;++iHitx) for(unsigned int iHity = 0; iHity < 16;++iHity){{
    float qx = (GetHisto("TargetProfileX"))->GetBinContent(iHitx+1) /Qx; if(qx<0)qx= 0.01;
    float qy = (GetHisto("TargetProfileY"))->GetBinContent(iHity+1) /Qy; if(qy<0)qy= 0.01;
    ((TH2F *)GetHisto("TargetXYmap"))->SetBinContent(iHitx+1, iHity+1, qx*qy );
    //cout << "qx "<< qx << " qy "<< qy <<endl;
  }}

  //Normalize per event
  int Nevents = (GetHisto("TargetProfileX"))->GetEntries();  
  float Normalize = 1. ;
  if(Nevents) Normalize = 1./ float (Nevents) * 16  ; 
  (GetHisto("TargetProfileX"))->Scale(Normalize);
  (GetHisto("TargetProfileY"))->Scale(Normalize);
  
  Normalize = 1. ;
  if(((TH2F *)GetHisto("TargetXYmap"))->Integral()) Normalize = 1/((TH2F *)GetHisto("TargetXYmap"))->Integral();
  ((TH2F *)GetHisto("TargetXYmap"))->Scale(Normalize);

  //Fit profiles
  TF1 *fitFcnX = new TF1("fitFcnX",TargetReconstruction::fitProfile,-10,10,4);
  float baseline  =                                           0 ; fitFcnX->SetParameter(0, baseline);
  float amplitude =  (GetHisto("TargetProfileX"))-> GetMaximum(); fitFcnX->SetParameter(1, amplitude);
  float average   =  (GetHisto("TargetProfileX"))->    GetMean(); fitFcnX->SetParameter(2, average  );//fitFcnX->SetParLimits(2, -7.5, 8.5 );
  float width     =  (GetHisto("TargetProfileX"))->    GetRMS (); fitFcnX->SetParameter(3, width    );//fitFcnX->SetParLimits(3,    0,   8 );

  float NonEmpty= GetHisto("TargetProfileX")->Integral();
  if(NonEmpty)(GetHisto("TargetProfileX"))->Fit("fitFcnX","q");
  baseline  =  fitFcnX->GetParameter(0);
  amplitude =  fitFcnX->GetParameter(1);
  average   =  fitFcnX->GetParameter(2);
  width     =  fitFcnX->GetParameter(3);  
  int NBin= (GetHisto("TargetProfileX"))-> GetNbinsX();
  for(int iBin=0;iBin!=NBin;iBin++){
   float cont  =  (GetHisto("TargetProfileX")) -> GetBinContent(iBin+1);
   cont  = cont - baseline;
   (GetHisto("TargetProfileX")) -> SetBinContent(iBin+1,cont);
  }
  baseline=0;
  fitFcnX->FixParameter(0, baseline ); 
  float xlow  = average-width;if(xlow  <-7.5|| xlow >8.5)xlow =-7.5;
  float xhigh = average+width;if(xhigh <-7.5|| xhigh>8.5)xhigh= 8.5;
  if(NonEmpty)(GetHisto("TargetProfileX"))->Fit("fitFcnX","q","",xlow,xhigh); 
  fitFcnX->FixParameter(1, amplitude);
  fitFcnX->FixParameter(2, average  );
  fitFcnX->FixParameter(3, width    ); 
  if(NonEmpty)(GetHisto("TargetProfileX"))->Fit("fitFcnX","q");
 

  TF1 *fitFcnY = new TF1("fitFcnY",TargetReconstruction::fitProfile,-10,10,4);
  baseline  =                                           0 ; fitFcnY->SetParameter(0, baseline);
  amplitude =  (GetHisto("TargetProfileY"))-> GetMaximum(); fitFcnY->SetParameter(1, amplitude);
  average   =  (GetHisto("TargetProfileY"))->    GetMean(); fitFcnY->SetParameter(2, average  );//fitFcnY->SetParLimits(2, -7.5, 8.5 );
  width     =  (GetHisto("TargetProfileY"))->    GetRMS (); fitFcnY->SetParameter(3, width    );//fitFcnY->SetParLimits(3,    0,   8 );

  NonEmpty= GetHisto("TargetProfileY")->Integral();
  if(NonEmpty)GetHisto("TargetProfileY")->Fit("fitFcnY","q");
  baseline  =  fitFcnY->GetParameter(0);
  amplitude =  fitFcnY->GetParameter(1);
  average   =  fitFcnY->GetParameter(2);
  width     =  fitFcnY->GetParameter(3);  

  NBin= (GetHisto("TargetProfileY"))-> GetNbinsX();
  for(int iBin=0;iBin!=NBin;iBin++){
   float cont  =  (GetHisto("TargetProfileY")) -> GetBinContent(iBin+1);
   cont  = cont - baseline;
   (GetHisto("TargetProfileY")) -> SetBinContent(iBin+1,cont);
  }
  baseline=0;
  fitFcnY->FixParameter(0, baseline );
  if(NonEmpty)GetHisto("TargetProfileY")->Fit("fitFcnY","q","",average-width,average+width); 
  fitFcnY->FixParameter(1, amplitude);
  fitFcnY->FixParameter(2, average  );
  fitFcnY->FixParameter(3, width    ); 
  if(NonEmpty)GetHisto("TargetProfileY")->Fit("fitFcnY","q");
 

  //Waweform histograms
  char iName[100];
  for(UChar_t c=0;c<32;c++)// Loop over channels
  { 
   sprintf(iName,"TargetCh%d",c);
   Normalize = 1. ;
   if((GetHisto(iName))->GetEntries()) Normalize = 1/(GetHisto(iName))->GetEntries()*1024;
   (GetHisto(iName))->Scale(Normalize);
   for(UShort_t s=0;s<1024;s++)(GetHisto(iName))->SetBinError(s+1,1/4096);
  }// End loop over channels

  if(!fHistoFile) return;
  HistoExit();
}

void TargetReconstruction::AnalyzeEvent(TRawEvent* rawEv){
  
  //fTargetRecoBeam->setCentroid(4,1,2,1);
  

  vector<TRecoVHit *> &Hits  = GetRecoHits();
  if (Hits.size()!=32) 
    {
      std::cout<<this->GetName()<<"::AnalyzeEvent something wrong ... there are "<<Hits.size()<<" hits in this event; expected 32; skip event"<<std::endl;
      return;
    }

  char  iName                    [100]; 
  float charge_signX       = 1000/0.05;// 0.05 fC corresponds to about 10 um CCD in diamond
  float charge_signY       =-1000/0.05;// 0.05 fC corresponds to about 10 um CCD in diamond
  

  ((TH2F *)GetHisto("TargetBeamSpot"))->Fill(getRecoBeam()->getX(),getRecoBeam()->getY());

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    if(iHit1<16){
     (GetHisto("TargetProfileX"    ))->Fill         (Hits[iHit1]->GetChannelId()-8,Hits[iHit1]->GetEnergy()*charge_signX);
     (GetHisto("TargetProfileLastX"))->SetBinContent(Hits[iHit1]->GetChannelId()+1,Hits[iHit1]->GetEnergy()*charge_signX);       
     sprintf(iName,"TargetQCh%d",iHit1);
     (GetHisto(iName))->Fill(Hits[iHit1]->GetEnergy()*charge_signX);
    }
    else{
     (GetHisto("TargetProfileY"    ))->Fill         (Hits[iHit1]->GetChannelId()-16-8,Hits[iHit1]->GetEnergy()*charge_signY);
     (GetHisto("TargetProfileLastY"))->SetBinContent(Hits[iHit1]->GetChannelId()-16+1,Hits[iHit1]->GetEnergy()*charge_signY);   
     sprintf(iName,"TargetQCh%d",iHit1);
     (GetHisto(iName))->Fill(Hits[iHit1]->GetEnergy()*charge_signY); 
    }
  }

  //  std::cout<< "fTargetRecoBeam Centroid " << fTargetRecoBeam->getX()<< std::endl;


  //Waveform histograms
  UChar_t nBoards = rawEv->GetNADCBoards();
  Double_t adc_count[1024][32]        ; 
  Double_t adc_pedestal   [32]        ;
  Double_t charge         [32]        ;
  
  for(UChar_t b=0;b<nBoards;b++)// Loop over boards
  {
    TADCBoard* adcB = rawEv->ADCBoard(b);
    if(adcB->GetBoardId()!=28)continue; //should correspond to target board ;
    UChar_t nChn       = adcB ->GetNADCChannels(  );
    TADCChannel* Chn8  = adcB ->ADCChannel     ( 0);
    //    TADCChannel* Chn19 = adcB ->ADCChannel     ( 0); 
    TADCChannel* Chn21 = adcB ->ADCChannel     ( 0);
    TADCChannel* Chn27 = adcB ->ADCChannel     ( 0); 
   
    for(UChar_t c=0;c<nChn;c++){
     TADCChannel* chn = adcB->ADCChannel      (c);
     UChar_t ch       = chn ->GetChannelNumber( );
     if(ch ==         8) Chn8  = chn;
     //     if(ch == 19*1+28*0) Chn19 = chn;
     if(ch == 21*0+26*0) Chn21 = chn;
     if(ch == 27*0+20*0) Chn27 = chn;
    }

   for(UChar_t c=0;c<nChn;c++){// Loop over channels
	 TADCChannel* chn = adcB->ADCChannel(c);
         UChar_t ch = chn->GetChannelNumber();


         //commond noise subtraction
         for(UShort_t s=0;s<1024;s++){// 1-st loop over sampling to remove common noise subtracting strip X2 and Y2 from X and Y strips          
            if     (ch<16&&ch!=8)
             {adc_count[s][ch]= (Double_t) (chn->GetSample(s) -Chn8->GetSample(s)) ;}
            else if(ch>=17 && ch<=22 && ch!=21)
             {adc_count[s][ch]= (Double_t) (chn->GetSample(s) -Chn21->GetSample(s)) ;}
            else if(ch==16 || (ch>=23 && ch!=27))
             {adc_count[s][ch]= (Double_t) (chn->GetSample(s) -Chn27->GetSample(s)) ;}
            else
             {adc_count[s][ch]= (Double_t) (chn->GetSample(s)) ;}
	 }//End 1-st loop over sampling

         adc_pedestal[ch]=0;
         for(UShort_t s=0;s<200;s++){// 2-nd loop over sampling to calculate event by event pedestal                    
              adc_pedestal[ch] += adc_count[s][ch]/200 ;
	 }//End 2-nd loop over sampling
         charge[ch]=0;
         for(UShort_t s=200;s<700;s++){// 3-rd loop over sampling to remohttp://l0padme3:9090/monitorve event by event pedestal and fill waveform  
             adc_count[s][ch] = adc_count[s][ch] - adc_pedestal[ch];
             float adc = float(adc_count[s][ch])/4096;  //Volts      
             sprintf(iName,"TargetCh%d",ch);
             (GetHisto(iName))->Fill(s,adc);        
             sprintf(iName,"TargetLastCh%d",ch);
             (GetHisto(iName))->SetBinContent(s+1,     adc);
             (GetHisto(iName))->SetBinError  (s+1,  1/4096);
             charge[ch] += adc;
	 }//End 3-rd loop over sampling
         charge[ch] = charge[ch]/50*1E-9/1E-12/1000/1.75; //1pC/1000???
         
         if(ch<16){  
          sprintf(iName,"TargetProfileCNSX");
          (GetHisto(iName))->SetBinContent(Hits[ch]->GetChannelId()   +1,charge[ch]*charge_signX);   
         }
         else{
          sprintf(iName,"TargetProfileCNSY");
          (GetHisto(iName))->SetBinContent(Hits[ch]->GetChannelId()-16+1,charge[ch]*charge_signY);
          //std::cout<<"Histo name " << iName << std::endl;
          //std::cout << " channelID " << Hits[ch]->GetChannelId() -15<< std::endl;
          //std::cout << " charge " << charge[ch] << std::endl; 
         }
         

         //no commond noise subtraction
         for(UShort_t s=0;s<1024;s++){// 1-st loop over sampling          
             {adc_count[s][ch]= (Double_t) (chn->GetSample(s) ) ;}
	 }//End 1-st loop over sampling

         adc_pedestal[ch]=0;
         for(UShort_t s=0;s<200;s++){// 2-nd loop over sampling to calculate event by event pedestal                    
              adc_pedestal[ch] += adc_count[s][ch]/200 ;
	 }//End 2-nd loop over sampling
         
         //charge[ch]=0;
         for(UShort_t s=200;s<700;s++){// 3-rd loop over sampling to remove event by event pedestal and fill waveform  
             adc_count[s][ch] = adc_count[s][ch] - adc_pedestal[ch];
             float adc = float(adc_count[s][ch])/4096;        
             sprintf(iName,"TargetNOCNSCh%d",ch);
             (GetHisto(iName))->Fill(s,adc);        
             sprintf(iName,"TargetNOCNSLastCh%d",ch);
             (GetHisto(iName))->SetBinContent(s+1,     adc);
             (GetHisto(iName))->SetBinError  (s+1,  1/4096);
             charge[ch] += adc;
	 }//End 3-rd loop over sampling
         //charge[ch] = charge[ch]/50*1E-9/1E-12/1000/1.75; //1pC/1000???
        


    }// End loop over channels
  }// End loop over boards

}  

// Sum of baseline + gaussian peak
Double_t TargetReconstruction::fitProfile(Double_t *x, Double_t *par) {    
      return par[0] + par[1]/sqrt(2*TMath::Pi()*par[3]*par[3])*exp(-(x[0]-par[2])*(x[0]-par[2])/2/par[3]/par[3]);
}


void TargetReconstruction::ReconstructBeam(){
  vector<TRecoVHit *> &Hits  = GetRecoHits();

  //  char  iName                    [100]; 
  float charge_signX       = 1000/0.05;// 0.05 fC corresponds to about 10 um CCD in diamond
  float charge_signY       =-1000/0.05;// 0.05 fC corresponds to about 10 um CCD in diamond
  
  //estimate Xbeam and Ybeam of the event
  //TH1F * hprofile = new TH1F  ("hprofile","hprofile",16,-7.5,8.5);
  for(unsigned int iHit1 = 0; iHit1 < 16;++iHit1){ 
   hprofile->SetBinContent(Hits[iHit1]->GetChannelId()+1,Hits[iHit1]->GetEnergy()*charge_signX); 
  }
  TF1 *fitFcn = new TF1("fitFcn",TargetReconstruction::fitProfile,-100,100,4);
  float baselineX  =                                           0 ; fitFcn->SetParameter(0, baselineX);
  float amplitudeX =  hprofile-> GetMaximum(); fitFcn->SetParameter(1, amplitudeX);
  float averageX   =  hprofile->    GetMean(); fitFcn->SetParameter(2, averageX  ); //float meanX=averageX; 
  float widthX     =  hprofile->    GetRMS (); fitFcn->SetParameter(3, widthX    ); 
  float NonEmpty= hprofile->Integral();
  float MeanX=averageX;
  float MeanXErr=hprofile->GetMeanError();
  float RMS_X=widthX;
  float RMS_Xerr=hprofile->GetRMSError();
  if(NonEmpty)hprofile->Fit("fitFcn","q");
  baselineX  =        fitFcn->GetParameter(0) ;
  amplitudeX =        fitFcn->GetParameter(1) ;
  averageX   =        fitFcn->GetParameter(2) ;
  widthX     =  fabs( fitFcn->GetParameter(3));
  fitFcn->FixParameter(0,baselineX);
  float xlow  = averageX-widthX;if(xlow  <-7.5|| xlow >8.5)xlow =-7.5;
  float xhigh = averageX+widthX;if(xhigh <-7.5|| xhigh>8.5)xhigh= 8.5;
  if(NonEmpty)hprofile->Fit("fitFcn","q","",xlow,xhigh);
  baselineX  =        fitFcn->GetParameter(0) ;
  amplitudeX =        fitFcn->GetParameter(1) ;
  averageX   =        fitFcn->GetParameter(2) ;
  widthX     =  fabs( fitFcn->GetParameter(3));
  //float baselineXErr  =fitFcn->GetParError(0);
  //float amplitudeXErr =fitFcn->GetParError(1);
  float averageXErr   =fitFcn->GetParError(2);
  float widthXErr     =fitFcn->GetParError(3);
  float chi2X         =fitFcn->GetChisquare();
  float NdofX         =fitFcn->GetNDF();
 
  for(unsigned int iHit1 = 16; iHit1 < Hits.size();++iHit1){ 
   hprofile->SetBinContent(Hits[iHit1]->GetChannelId()-16+1,Hits[iHit1]->GetEnergy()*charge_signY); 
  }
  float baselineY  =                                           0 ; fitFcn->SetParameter(0, baselineY);
  float amplitudeY =  hprofile-> GetMaximum(); fitFcn->SetParameter(1, amplitudeY);
  float averageY   =  hprofile->    GetMean(); fitFcn->SetParameter(2, averageY  ); //float meanY=averageY; 
  float widthY     =  hprofile->    GetRMS (); fitFcn->SetParameter(3, widthY    ); 
  float MeanY=averageY;
  float MeanYErr=hprofile->GetMeanError();
  float RMS_Y=widthY;
  float RMS_Yerr=hprofile->GetRMSError();
  NonEmpty= hprofile->Integral();
  if(NonEmpty)hprofile->Fit("fitFcn","q");
  baselineY  =        fitFcn->GetParameter(0) ;
  amplitudeY =        fitFcn->GetParameter(1) ;
  averageY   =        fitFcn->GetParameter(2) ;
  widthY     =  fabs( fitFcn->GetParameter(3));
  fitFcn->FixParameter(0,baselineY);
  xlow  = averageY-widthY;if(xlow  <-7.5|| xlow >8.5)xlow =-7.5;
  xhigh = averageY+widthY;if(xhigh <-7.5|| xhigh>8.5)xhigh= 8.5;
  if(NonEmpty)hprofile->Fit("fitFcn","q","",xlow,xhigh);
  baselineY  =        fitFcn->GetParameter(0) ;
  amplitudeY =        fitFcn->GetParameter(1) ;
  averageY   =        fitFcn->GetParameter(2) ;
  widthY     =  fabs( fitFcn->GetParameter(3));
  //float baselineYErr  =fitFcn->GetParError(0);
  //float amplitudeYErr =fitFcn->GetParError(1);
  float averageYErr   =fitFcn->GetParError(2);
  float widthYErr     =fitFcn->GetParError(3);
  float chi2Y         =fitFcn->GetChisquare();
  float NdofY         =fitFcn->GetNDF();

  fTargetRecoBeam->setCentroid(MeanX,MeanXErr,MeanY,MeanYErr);
  fTargetRecoBeam->setWidth(RMS_X,RMS_Xerr,RMS_Y,RMS_Yerr);
  fTargetRecoBeam->setFitCentroid(averageX,averageXErr,averageY,averageYErr);
  fTargetRecoBeam->setFitWidth(widthX,widthXErr,widthY,widthYErr);
  fTargetRecoBeam->setFitParameter(chi2X,chi2Y, NdofX, NdofY);
  //std::cout<< "Centroid X: " << MeanX << " ErrX " << RMS_X << " Y " << MeanY << " ErrY "<< RMS_Y << std::endl;
  //std::cout<< "Centroid Fit X: " << averageX << " ErrX " << widthX << " Y " << averageY << " ErrY "<< widthY << std::endl;



  
  
  
  //Compute charge 
  float Qx=0;
  float Qy=0;
  float ErrQx=0;
  float ErrQy=0;
  int HitsX=0;
  int HitsY=0;

  for(unsigned int iHit1 = 0; iHit1 < Hits.size();++iHit1) {
    //std::cout<<" hit id, energy = "<<Hits[iHit1]->GetChannelId()<<" "<<Hits[iHit1]->GetEnergy()<<std::endl;   
    if(iHit1<16){
     Qx += Hits[iHit1]->GetEnergy();
     HitsX++;        
    }
    else{
     Qy += Hits[iHit1]->GetEnergy();
     HitsY++;       
    }
  }

  // ChargeToNPOT
  Double_t chargeToNPOT = 1000/0.05;
  
  //std::cout<< "Charge X: " << Qx << " ErrX " << ErrQx << " Y " << Qy << " ErrY "<< ErrQy << std::endl;
  
  float BeamMultiplicity = (fabs(Qx)+fabs(Qy))*chargeToNPOT/2.;
  (GetHisto("TargetBeamMultiplicity" ))->Fill(BeamMultiplicity);
  float ErrBeamMultiplicity=sqrt(BeamMultiplicity);
  fTargetRecoBeam->setPOT(BeamMultiplicity, ErrBeamMultiplicity);


  float xBeamMultiplicity = fabs(Qx)*chargeToNPOT;
  float yBeamMultiplicity = fabs(Qy)*chargeToNPOT;
  ErrQx=sqrt(xBeamMultiplicity)/chargeToNPOT;
  ErrQy=sqrt(yBeamMultiplicity)/chargeToNPOT;
  fTargetRecoBeam->setCharge(Qx,ErrQx,Qy,ErrQy);


}


