// Written by E. Di Meco 10/11/2022
#include "ECalCalib22.hh"
#include "TF1.h"
#include "TFile.h"


//Added the Finstance to allow to use the information in all the enalyzers
ECalCalib22* ECalCalib22::fInstance = 0;
ECalCalib22* ECalCalib22::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new ECalCalib22(); }
  return fInstance;
}

ECalCalib22::ECalCalib22(TString cfgFile, Int_t verbose)
{
  fVerbose = verbose;
  if (fVerbose) {
    printf("---> Creating ECalCalib22\n");
    printf("     Configuration file %s\n",cfgFile.Data());
    if (fVerbose>1) printf("     Verbose level %d\n",fVerbose);
  }
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());
  fNRun = "";	     
  fCurrentRun=-1.;     
  fCurrentRunIndex=-1.;
}

ECalCalib22::~ECalCalib22(){
  delete fCfgParser;
  //  delete fRndm;
}

Bool_t ECalCalib22::Init(Bool_t fHistoModeVal, TString InputHistofileVal){
  if (fVerbose) printf("---> Initializing ECalCalib22\n");
  int iChNum=0;
  ifstream InFile("../PadmeReco/config/Calibration/ECalEnergyCalibration_7.dat");
  
  if(!InFile.is_open()) {
    cout<<"Cannot open ECal Channel list file!!!"<<endl;
  }else{
    while(!InFile.eof()){
      InFile>>FCol[iChNum]>>FRow[iChNum]>>FBdId[iChNum]>>FChId[iChNum]>>OldCalib[iChNum];
      ECalChNum[iChNum]=FCol[iChNum]*100+FRow[iChNum];
      
      iChNum++;
    }
  }

  InFile.close();
  if(iChNum!=613) cout<<"**** "<<(616-iChNum)<<" ECal Channels missing in ECalEnergyCalibration_6.dat **** "<<endl;


  InputHistofile = InputHistofileVal;

  fHistoMode = fHistoModeVal;
  if(fHistoMode){
  TObjArray *tx = InputHistofile.Tokenize("/");
  InputHistofileName = ((TObjString *)(tx->At(tx->GetLast())))->String(); 
  TObjArray *txRun = InputHistofileName.Tokenize(".");
  fNRun = ((TObjString *)(txRun->At(0)))->String()+"."+((TObjString *)(txRun->At(1)))->String(); 
  }

  InitHistos(); 
  // read calibration file data
  return true;
  

}

Bool_t ECalCalib22::InitHistos(){
  // ECalCalib22 directory will contain all histograms related to this analysis
  fHS->CreateList("ECalCalib22");
  Double_t hEMax=800;
  Double_t hEBins=800;
  fHS->BookHistoList("ECalCalib22","EECal_noCalib"  ,hEBins,0.,hEMax);
  fHS->BookHistoList("ECalCalib22","ETot2gCal_noCalib"  ,hEBins,0.,hEMax);
  fHS->BookHisto2List("ECalCalib22","EnMap_noCalib"  ,150,-300.,300.,150,-300.,300.);
  fHS->BookHisto2List("ECalCalib22","EnMap_ChCR",30,0,30,30,0,30);
  fHS->BookHisto2List("ECalCalib22","EnMap_EnCR",30,0,30,30,0,30);
  fHS->BookHisto2List("ECalCalib22","EnMap_EnHit",30,0,30,30,0,30);
  fHS->BookHisto2List("ECalCalib22","CRMap_MPV",30,0,30,30,0,30);
  fHS->BookHisto2List("ECalCalib22","CRMap_Eff",30,0,30,30,0,30);
  fHS->BookHistoList("ECalCalib22","CRMPV"  ,100,0,50);
  fHS->BookHisto2List("ECalCalib22","CalibRatio",30,0,30,30,0,30);

  for(int iCh=0; iCh<NTOTCh; iCh++){
    TString HistoName = Form("CREnCH%d",ECalChNum[iCh]);
    TString HistoNameVert = Form("CREnCH%d_Vert",ECalChNum[iCh]);
    ECalChHisto[iCh]=fHS->BookHistoList("ECalCalib22",HistoName.Data(),110,-10,100);
    ECalChHistoVert[iCh]=fHS->BookHistoList("ECalCalib22",HistoNameVert.Data(),110,-10,100);
  }
   TH1D *ECalChHisto_1=fHS->BookHistoList("ECalCalib22","CREnCH-1",110,-10,100);
   TH1D *ECalChHistoVert_1=fHS->BookHistoList("ECalCalib22", "CREnCH-1_Vert",110,-10,100);
 


  return true;
}

Bool_t ECalCalib22::Process(PadmeAnalysisEvent* event){
  fEvent = event;
  Double_t ETotEcal=0;
  // if (fEvent->RecoEvent->GetEventStatusBit(TRECOEVENT_STATUSBIT_SIMULATED)) fisMC=true;
  UInt_t trigMask = fEvent->RecoEvent->GetTriggerMask();
  Int_t NEvent = fEvent->RecoEvent->GetEventNumber(); 
  if(NEvent%10000==0) cout<<"NEvent "<<NEvent<<endl;

  Int_t fNClusters = fEvent->ECalRecoCl->GetNElements();
  //cout<<"trigMask: "<<trigMask<<" NEvent: "<<NEvent<<" NRun: "<<fNRun<<endl;  
  
  //Plot before calibration
  for(int iCal=0; iCal<fNClusters; iCal++){ 
   
    TVector3 pos    =  fEvent->ECalRecoCl->Element(iCal)->GetPosition();
    Double_t eECal = fEvent->ECalRecoCl->Element(iCal)->GetEnergy();
    ETotEcal+=eECal;
    
    fHS->FillHistoList("ECalCalib22","EECal_noCalib",eECal,1);
    fHS->FillHistoList("ECalCalib22","ETot2gCal_noCalib",ETotEcal,1);
    fHS->FillHisto2List("ECalCalib22","EnMap_noCalib",pos.X(),pos.Y(),eECal);

  }
  Int_t HitMap[30][40]={0};
  Double_t HitEnMap[30][30]={0.};
  Int_t ChId=0;
  Double_t eCR;
  Int_t Col,Row;
  Int_t NEventHits = fEvent->ECalRecoEvent->GetNHits();
  for(int iHit=0; iHit < NEventHits; iHit++){
    ChId=  fEvent->ECalRecoEvent->Hit(iHit)->GetChannelId();
    eCR =  fEvent->ECalRecoEvent->Hit(iHit)->GetEnergy();
    Row = ChId%100;
    Col = ChId/100;
     

    fHS->FillHisto2List("ECalCalib22","EnMap_EnHit",Col,Row, eCR);
  
  }
  //Cosmic rays energy distributions
  if(trigMask==2){ //selecting CR trigger 
    Int_t ChId=0;
    Double_t eCR;
    Int_t Col,Row;
    Int_t NEventHits = fEvent->ECalRecoEvent->GetNHits();
    for(int iHit=0; iHit < NEventHits; iHit++){
      ChId=  fEvent->ECalRecoEvent->Hit(iHit)->GetChannelId();
      eCR =  fEvent->ECalRecoEvent->Hit(iHit)->GetEnergy();
      Row = ChId%100;
      Col = ChId/100;
      HitMap[Col][Row]=1;
      HitEnMap[Col][Row]=eCR;
      fHS->FillHisto2List("ECalCalib22","EnMap_ChCR",Col,Row);
      fHS->FillHisto2List("ECalCalib22","EnMap_EnCR",Col,Row, eCR);
      TString HistoName = Form("CREnCH%d",ChId);
      fHS->FillHistoList("ECalCalib22", HistoName.Data(),eCR,1);
    }//chiude ciclo primario hit
    for(int iCol=0; iCol<30; iCol++){
      for(int iRow=0; iRow<30; iRow++){
    	   if((HitMap[iCol][iRow-1]==1 && HitMap[iCol][iRow+1]==1) || (HitMap[iCol][iRow-1]==1 && HitMap[iCol][iRow-2]==1 && HitMap[iCol][iRow-3]==1  && HitMap[iCol][iRow-4]==1 )|| (HitMap[iCol][iRow+1]==1  && HitMap[iCol][iRow+2]==1  && HitMap[iCol][iRow+3]==1  && HitMap[iCol][iRow+4]==1 ) || (HitMap[iCol][iRow-1]==1 && HitMap[iCol][iRow+2]==1 )||( HitMap[iCol][iRow-2]==1 && HitMap[iCol][iRow+1]==1)){
        
          Int_t ChIdCross=iCol*100+iRow;
    	    int iCh=0;
    	    for(iCh=0; iCh<NTOTCh; iCh++){
    	      if(ECalChNum[iCh]==ChIdCross) break;
      	  }
	      //if(iCh==NTOTCh) continue; //CHECK
    	  Cross[iCh]++;
	  	  if(HitMap[iCol][iRow]==1){
	          MiddleCross[iCh]++;
            TString HistoNameVert = Form("CREnCH%d_Vert",ECalChNum[iCh]);
            fHS->FillHistoList("ECalCalib22", HistoNameVert.Data(),HitEnMap[iCol][iRow],1);
	    	  
          }
	      //continue; CHECK
	      }//chiude if su hitmap 

      }//chiude righe
    }//chiude colonne
    //end alternative code

  }
  return true;
}

 //TString HistoNameVert = Form("CREnCH%d_Vert",ChId);
 //fHS->FillHistoList("ECalCalib22", HistoNameVert.Data(),eCR,1);



Bool_t ECalCalib22::Finalize()
{ if (fVerbose) printf("---> Finalizing ECalCalib22\n");
  if(fHistoMode && fGeneralInfo->isMC()==true){
  std::cout<<"This run is MC, DataQuality checks do not apply"<<std::endl;
  return false;
  } 
 ChannelLandauFit();
  
  return true;
}

Bool_t ECalCalib22::ChannelLandauFit(){
  if(fHistoMode==false) return false;
  
  TFile *InFile = new TFile(InputHistofile.Data());
  if(!InFile->IsOpen()){
    std::cout<<InputHistofile.Data()<<" not found--> exiting"<<std::endl;
    exit(1);
  }
  for(int iCh=0; iCh<NTOTCh; iCh++){
    TString HistoName = Form("CREnCH%d",ECalChNum[iCh]);
    TString HistoNameVert = Form("CREnCH%d_Vert",ECalChNum[iCh]);
    ECalChHisto[iCh]= (TH1D*) InFile->Get(Form("ECalCalib22/%s",HistoName.Data()))->Clone();
    ECalChHistoVert[iCh]=(TH1D*)InFile->Get(Form("ECalCalib22/%s",HistoNameVert.Data()))->Clone();
  }
  TFile *BadCalib = new TFile(Form("/data9Vd1/padme/dimeco/TagAndProbeOut/DATAout/BadCalib_%s.root", fNRun.Data()),"RECREATE");
  ofstream OutFile;
  if(NewCalib) OutFile.open(Form("/data9Vd1/padme/dimeco/TagAndProbeOut/DATAout/ECalEnergyCalibration_7.dat"));
  ofstream ErrFile(Form("/data9Vd1/padme/dimeco/TagAndProbeOut/DATAout/BadUnits_%s.txt",fNRun.Data()));
  cout<<"Performing Landau fits"<<endl<<endl;
  TF1 *LandauFun = new TF1("LandauFun", "landau",13., 100.);
  //LandauFun->SetParLimits(1,13.,23.);
  int iCh=0;
  if(ErrFile.is_open()){
    
    for(iCh=0; iCh<NTOTCh; iCh++){ //Fitting each histo with a Laundau function
      
      float NonEmpty = ECalChHistoVert[iCh]->Integral();
      if(NonEmpty>200) {
        ECalChHistoVert[iCh]->Fit("LandauFun","RQ"," ", 13., 60.); 
        Double_t LandauMPV= LandauFun->GetParameter(1);
        Double_t LandauSigma= LandauFun->GetParameter(2);
        LandauFun->ReleaseParameter(1);
        LandauFun->ReleaseParameter(2);
        LandauFun->SetParameter(1,17);
        LandauFun->SetParLimits(1,10.,30.);

        ECalChHistoVert[iCh]->Fit("LandauFun","RQ"," ", LandauMPV-2*LandauSigma, 60.); 
        ECalChHistoVert[iCh]->Fit("LandauFun","RQ"," ", LandauMPV-2*LandauSigma, 60.); 
        ECalChMVP[iCh]=LandauFun->GetParameter(1);
        //if(iCh==0) cout<<"First MPV:"<<ECalChMVP[iCh]<<endl;
        Double_t Chi2 = LandauFun->GetChisquare()/LandauFun->GetNDF();
        if(Cross[iCh]==0) ECalChEff[iCh]=-1;
        else ECalChEff[iCh]=(Double_t) MiddleCross[iCh]/(Double_t)Cross[iCh];
        
        if(Chi2 > 3.5 || ECalChMVP[iCh]<10 || ECalChMVP[iCh]>25) {
          ErrFile<<ECalChNum[iCh]<<"\t"<< ECalChMVP[iCh]<<" \t"<<Chi2<<endl; 
          BadCalib->cd();
          ECalChHistoVert[iCh]->Write();
          //ECalChMVP[iCh]=-1;
        }
        
        if(NewCalib && OutFile.is_open()) OutFile<<" "<<ECalChNum[iCh]/100<<" "<<ECalChNum[iCh]%100<<" "<<FBdId[iCh]<<" "<<FChId[iCh]<<" "<<ECalChMVP[iCh]*15.<<endl;
        //OutFile<<ECalChNum[iCh]<<"\t"<< ECalChMVP[iCh]<<"\t"<<ECalChEff[iCh]<<endl;
        
        
        fHS->FillHisto2List("ECalCalib22","CRMap_MPV",ECalChNum[iCh]/100, ECalChNum[iCh]%100,ECalChMVP[iCh]);
        fHS->FillHisto2List("ECalCalib22","CRMap_Eff",ECalChNum[iCh]/100, ECalChNum[iCh]%100,ECalChEff[iCh]);
        fHS->FillHistoList("ECalCalib22","CRMPV",ECalChMVP[iCh]);
        if(ECalChNum[iCh]!= 1727 && ECalChNum[iCh]!= 2407) fHS->FillHisto2List("ECalCalib22","CalibRatio",ECalChNum[iCh]/100, ECalChNum[iCh]%100, ECalChMVP[iCh]*15./OldCalib[iCh]);

        LandauFun->ReleaseParameter(1);
        }else {
          std::cout<<"Ch: "<<ECalChNum[iCh]<<" Vertical is empty "<<endl;
          BadCalib->cd();
          ECalChHistoVert[iCh]->Write();
          float NonEmpty = ECalChHisto[iCh]->Integral();
          if(NonEmpty>200){
            double _maxPeak= 0;
            double RealPeak=0; 
            for(int b= 0; b < (ECalChHisto[iCh]->GetNbinsX()); b++){ 
                double histoVal = ECalChHisto[iCh]->GetBinContent(b);
                if(histoVal >_maxPeak){
                    _maxPeak = histoVal;
                    RealPeak = ECalChHisto[iCh]->GetBinCenter(b);
                  }
                }

            ECalChHisto[iCh]->Fit("LandauFun","RQ"," ", 10., 60.); 
            Double_t LandauMPV= LandauFun->GetParameter(1);
            Double_t LandauSigma= LandauFun->GetParameter(2);
            LandauFun->ReleaseParameter(1);
            LandauFun->ReleaseParameter(2);
            LandauFun->SetParameter(1,17);
            LandauFun->SetParLimits(1,10,30);

            ECalChHisto[iCh]->Fit("LandauFun","RQ"," ", LandauMPV-0.5*LandauSigma, 60.); 
            ECalChHisto[iCh]->Fit("LandauFun","RQ"," ", LandauMPV-0.5*LandauSigma, 60.); 
            ECalChMVP[iCh]=LandauFun->GetParameter(1);
            if(NewCalib) OutFile<<" "<<ECalChNum[iCh]/100<<" "<<ECalChNum[iCh]%100<<" "<<FBdId[iCh]<<" "<<FChId[iCh]<<" "<<ECalChMVP[iCh]*15.<<endl;
            fHS->FillHisto2List("ECalCalib22","CRMap_MPV",ECalChNum[iCh]/100, ECalChNum[iCh]%100,ECalChMVP[iCh]);
            fHS->FillHisto2List("ECalCalib22","CRMap_Eff",ECalChNum[iCh]/100, ECalChNum[iCh]%100,ECalChEff[iCh]);
            fHS->FillHistoList("ECalCalib22","CRMPV",ECalChMVP[iCh]);
            LandauFun->ReleaseParameter(1);
            Double_t Chi2 = LandauFun->GetChisquare()/LandauFun->GetNDF();
            if(Chi2 > 3.5 || ECalChMVP[iCh]<10 || ECalChMVP[iCh]>25) {
            ErrFile<<ECalChNum[iCh]<<"\t"<< ECalChMVP[iCh]<<" \t"<<Chi2<<endl; 
            BadCalib->cd();
            ECalChHisto[iCh]->Write();
            //ECalChMVP[iCh]=-1;
            }
          
          }else{
           
            ECalChMVP[iCh]=-1;
            ErrFile<<ECalChNum[iCh]<<"\t"<< ECalChMVP[iCh]<<"\t"<<50<<endl; 

            //  ErrFile<<"Ch: "<<ECalChNum[iCh]<<" is DEAD "<<endl;
            if(NewCalib) OutFile<<" "<<ECalChNum[iCh]/100<<" "<<ECalChNum[iCh]%100<<" "<<FBdId[iCh]<<" "<<FChId[iCh]<<" "<<ECalChMVP[iCh]*15.<<endl;
          }
        }
    }
    cout<<endl;
    if(NewCalib) cout<<"\t\t---> Calibration constants can be found in ECalEnergyCalibration_7.dat"<<endl<<endl;
    cout<<"\t\t---> Calibration Errors log are in BadUnits_"<<fNRun.Data()<<".txt, plots can be found in BadCalib_"<<fNRun.Data()<<".root"<<endl<<endl;
    }else {
      cout<<"****** Cannot open ECalEnergyCalibration_7.dat ******"<<endl;
      cout<<"****** Cannot open BadUnits_"<<fNRun.Data()<<".txt file ******"<<endl;
    }

    return true;


}