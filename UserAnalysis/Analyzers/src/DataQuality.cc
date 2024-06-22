#include "DataQuality.hh"
#include "TGraph.h"
#include "TString.h"
#include "TMath.h"


DataQuality* DataQuality::fInstance = 0;

DataQuality* DataQuality::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new DataQuality(); }
  return fInstance;
}

DataQuality::~DataQuality(){
  delete fCfgParser;
}

Bool_t DataQuality::Init(PadmeAnalysisEvent* event,  Bool_t fHistoModeVal, TString InputHistofileVal){

  if (fVerbose) printf("---> Initializing DataQuality\n");
  TString cfgFile = "config/UserAnalysis.conf";
  fHS = HistoSvc::GetInstance();
  fGeneralInfo = GeneralInfo::GetInstance();
  fNPoTAnalysis = NPoTAnalysis::GetInstance();
  fCfgParser = new utl::ConfigParser((const std::string)cfgFile.Data());

  // deve poter leggere il config e sapere se e' in read mode o flag mode, se e' in flag mode deve leggere il file di testo coi periodi con problemi
  // e determina la flag per quell'evento in base al tempo  
  fEvent = event;
  fNRun = fGeneralInfo->GetRunNumberFromDB(); //30000 vale solo per il 2022
  cout<<" Creating DataQuality Hystograms for Run Init "<< fNRun<<endl;  
  fHistoMode = fHistoModeVal;
  InputHistofile = InputHistofileVal;

//   if(fHistoMode){
//   TObjArray *tx = InputHistofile.Tokenize("/");
//   InputHistofileName = ((TObjString *)(tx->At(tx->GetLast())))->String(); 

//   TObjArray *txRun = InputHistofileName.Tokenize(".");
//   fNRunString = ((TObjString *)(txRun->At(0)))->String()+"."+((TObjString *)(txRun->At(1)))->String(); 
//   //fNRun = ((TObjString *)(txRun->At(0)))->String(); //OCCHIO
  
//  }
  // se e' readmode fa quello che c'e' sotto
  fSafety = 1000;
  fTimeBin = 5.; // sec
  fTimeBinCoarse = 120.; // sec

  fNTimeBins = fSafety +((int)(fGeneralInfo->GetRunStopTime()-fGeneralInfo->GetRunStartTime()))/fTimeBin; //check carefully this ratio because there are two long long int subtracted
  fNTimeBinsCoarse =1+fSafety*fTimeBin/fTimeBinCoarse + ((int)(fGeneralInfo->GetRunStopTime()-fGeneralInfo->GetRunStartTime()))/fTimeBinCoarse; 

  const int nObservables = 8;
  TString obsNames[nObservables] = {"POT","ECalOverPOT","ECalHitOverPOT","POTBunchLength","LGPed","POTTarg", "TargXCharge", "TargYCharge"};
  for (int i=0; i<nObservables; i++){
    observable obsn;
    obsn.name = obsNames[i].Data();

    obsn.valueSum = new Double_t[fNTimeBins];
    obsn.valueSquareSum = new Double_t[fNTimeBins];
    obsn.valueSumCoarse = new Double_t[fNTimeBinsCoarse];
    obsn.valueSquareSumCoarse = new Double_t[fNTimeBinsCoarse];

    obsn.nCounts = new Int_t[fNTimeBins];
    obsn.nCountsCoarse = new Int_t[fNTimeBinsCoarse];

    for(int j =0; j<fNTimeBins; j++){
      obsn.valueSum[j] = 0;
      obsn.valueSquareSum[j]=0;
      obsn.nCounts[j]=0;
    }
     for(int j =0; j<fNTimeBinsCoarse; j++){
      obsn.valueSumCoarse[j] = 0;
      obsn.valueSquareSumCoarse[j]=0;
      obsn.nCountsCoarse[j]=0;
    }

    fObservables.push_back(obsn);
    
  }


  InitHistos(fNRun);
  return true;
}

Bool_t DataQuality::InitHistos(Int_t nRun){
  // DataQuality directory will contain all histograms related to this analysis

  fHS->CreateList("DataQuality");
  cout<<" Creating Hystograms for Run "<<nRun<<" "<<endl;  
   
  return true;
}

Bool_t DataQuality::Process(){

  // protection: should run on data only
  // riempire i valori delle 5 osservabili
  // prendere il tempo dell'evento e calcolare in che bin temporale cadi x 2 (normale e coarse)
  // incrementare gli array sum, sumsquare, nCounts
  // il codice sotto e' vecchio

  long long int fTimeStamp =(long long int) fEvent->RecoEvent->GetEventTime();
  int TimeBinVal =(int) (fTimeStamp - fGeneralInfo->GetRunStartTime())/ fTimeBin;
  int TimeBinValCoarse =(int) (fTimeStamp - fGeneralInfo->GetRunStartTime())/ fTimeBinCoarse;
  if(TimeBinVal>=fNTimeBins || TimeBinVal<0){
    std::cout<<"DataQuality WARNING * Exceeding the numer of bins available: TimeBinVal: "<<TimeBinVal<<" NTimeBins: "<<fNTimeBins<<std::endl;
    return false;
  }
  if(TimeBinValCoarse>=fNTimeBinsCoarse || TimeBinValCoarse<0){
    std::cout<<"DataQuality WARNING * Exceeding the numer of bins available: TimeBinValCoarse: "<<TimeBinValCoarse<<" NTimeBinsCoarse: "<<fNTimeBinsCoarse<<std::endl;
    return false;
  }

  for(std::vector<observable>::iterator iter = fObservables.begin(); iter != fObservables.end(); ++iter){
    Double_t Value;
    
    if((iter->name).CompareTo("POT")==0){
      Value = fNPoTAnalysis->GetNPoTLGCorr();
    }else if((iter->name).CompareTo("ECalOverPOT")==0){
      Double_t ETotECal=0;
      for(int ical = 0;ical < fEvent->ECalRecoCl->GetNElements(); ical++){
        double eECal = fEvent->ECalRecoCl->Element(ical)->GetEnergy();
        ETotECal+=eECal;
      }
        if(fNPoTAnalysis->GetNPoTLGCorr()!=0) Value = ETotECal/fNPoTAnalysis->GetNPoTLGCorr();
        else Value = 0;
    }else if((iter->name).CompareTo("ECalHitOverPOT")==0){
      Double_t TotHitE=0;
      
      for(int iHit = 0;iHit <fEvent->ECalRecoEvent->GetNHits() ; iHit++) {
        double HitE = fEvent->ECalRecoEvent->Hit(iHit)->GetEnergy();
        TotHitE+=HitE;
      }
      if(fNPoTAnalysis->GetNPoTLGCorr()!=0) Value = TotHitE/fNPoTAnalysis->GetNPoTLGCorr();
      else Value = 0;
      }else if((iter->name).CompareTo("POTBunchLength")==0){
        Value = fEvent->LeadGlassRecoEvent->GetBunchLength();

      }else if((iter->name).CompareTo("LGPed")==0){
        Value = fEvent->LeadGlassRecoEvent->GetPedestal();

      }else if((iter->name).CompareTo("POTTarg")==0){
        Value = fEvent->TargetRecoBeam->getnPOT();

      }else if((iter->name).CompareTo("TargXCharge")==0){
        Value = fEvent->TargetRecoBeam->getXCharge();

      }else if((iter->name).CompareTo("TargYCharge")==0){
        Value = fEvent->TargetRecoBeam->getYCharge();

      }else{
      Value = 1;
      std::cout<<" DataQuality ** WARNING ** The "<<(iter->name).Data() << " observable is not implemented"<<std::endl;
    }
    
    (iter->valueSum)[TimeBinVal] += Value;
    (iter->valueSquareSum)[TimeBinVal] += (Value*Value);
    (iter->valueSumCoarse)[TimeBinValCoarse] += (Value);
    (iter->valueSquareSumCoarse)[TimeBinValCoarse] += (Value*Value);
    (iter->nCounts)[TimeBinVal]++;
    (iter->nCountsCoarse)[TimeBinValCoarse]++;

  }

  return true;
}

Bool_t DataQuality::Finalize(){
// 
// 
// qui i Double_t* e gli Int_t* devono essere scritti in uscita in forma di TGraph* (histoService gestisce il salvare i TGraph*) 
// il nome del TGraph* deve dipendere dal run
// 
// macro esterne:
// quando tu fai hadd, lo devi fare per run
// devi ripassare sul graph e fare le medie avg=sum/counts, rms=sqrt((sumsquare/counts - avg^2) / (counts-1))
// queste medie e rms bisogna usarle per fare il data quality
// determini i criteri
// determini i periodi "meno buoni", "cattivi": start e stop time e runnumber
// dobbiamo passare i periodi a DataQuality da usare quando dataquality e' in flagmode, forse in un file di testo formattato 
if(fGeneralInfo->isMC()){
  std::cout<<"This run is MC, DataQuality checks do not apply"<<std::endl;
  return false;
} 

fNRun = fGeneralInfo->GetRunNumberFromDB();
if(!fHistoMode){
  for(std::vector<observable>::iterator iter = fObservables.begin(); iter != fObservables.end(); ++iter){
    TH1D *hvalueSum = fHS->BookHistoList("DataQuality", Form("hvalueSum_%s_%d", iter->name.Data(), fNRun), fNTimeBins, 0, fNTimeBins*fTimeBin); //in seconds
    TH1D *hvalueSquareSum = fHS->BookHistoList("DataQuality", Form("hvalueSquareSum_%s_%d", iter->name.Data(), fNRun), fNTimeBins, 0, fNTimeBins*fTimeBin); //in seconds
    TH1D *hvalueSumCoarse = fHS->BookHistoList("DataQuality", Form("hvalueSumCoarse_%s_%d", iter->name.Data(), fNRun), fNTimeBinsCoarse, 0, fNTimeBinsCoarse*fTimeBinCoarse); //in seconds
    TH1D *hvalueSquareSumCoarse = fHS->BookHistoList("DataQuality", Form("hvalueSquareSumCoarse_%s_%d", iter->name.Data(), fNRun), fNTimeBinsCoarse, 0, fNTimeBinsCoarse*fTimeBinCoarse); //in seconds
    TH1D *hnCounts = fHS->BookHistoList("DataQuality", Form("hnCounts_%s_%d", iter->name.Data(), fNRun), fNTimeBins, 0, fNTimeBins*fTimeBin); //in seconds
    TH1D *hnCountsCoarse = fHS->BookHistoList("DataQuality", Form("hnCountsCoarse_%s_%d", iter->name.Data(), fNRun), fNTimeBinsCoarse, 0, fNTimeBinsCoarse*fTimeBinCoarse); //in seconds
    
    for(int i=0;i< fNTimeBins; i++ ){

      hvalueSum->SetBinContent(i+1,(iter->valueSum)[i]);
      hvalueSquareSum->SetBinContent(i+1,(iter->valueSquareSum)[i]);
      hnCounts->SetBinContent(i+1,(iter->nCounts)[i]);
    }
    for(int i=0;i< fNTimeBinsCoarse; i++ ){
      hvalueSumCoarse->SetBinContent(i+1,(iter->valueSumCoarse)[i]);
      hvalueSquareSumCoarse->SetBinContent(i+1,(iter->valueSquareSumCoarse)[i]);
      hnCountsCoarse->SetBinContent(i+1,(iter->nCountsCoarse)[i]);
    }
    

}
std::cout<<"Data Quality TH1D plots filled "<<std::endl;
}

if(fHistoMode){
  TGraphErrors *gPoTratio = new TGraphErrors();
  gPoTratio->SetName("PoTRatio");
  TGraphErrors *gPoTLG = new TGraphErrors();
  gPoTLG->SetName("PoTLG");

  TGraphErrors *gPoTLGAll = new TGraphErrors();
  gPoTLGAll->SetName("PoTLGAll");

  TGraphErrors *gPoTTarget = new TGraphErrors();
  gPoTTarget->SetName("PoTTarget");
  TGraphErrors *gXCharge = new TGraphErrors();
  gPoTTarget->SetName("gTargXCharge");
  TGraphErrors *gXChargeAll = new TGraphErrors();
  gXChargeAll->SetName("gTargXChargeAll");

  TGraphErrors *gPoTratioCharge = new TGraphErrors();
  gPoTratioCharge->SetName("gPoTratioCharge");
  TGraphErrors *gPoTratioChargeAll = new TGraphErrors();
  gPoTratioChargeAll->SetName("gPoTratioChargeAll");
 

  TFile *fileIn = new TFile(InputHistofile.Data());
  if(!fileIn) std::cout<<"File not existing"<<std::endl;
  std::cout<<"File to analyze for DataQuality: "<< InputHistofile.Data()<<std::endl;
  //retrive info from th1d and assign it to the obs struct
  for(std::vector<observable>::iterator iter = fObservables.begin(); iter != fObservables.end(); ++iter){
    TH1D *hvalueSum = (TH1D*) fileIn->Get(Form("DataQuality/hvalueSum_%s_%d", iter->name.Data(), fNRun))->Clone(); //in seconds
    TH1D *hvalueSquareSum = (TH1D*) fileIn->Get(Form("DataQuality/hvalueSquareSum_%s_%d", iter->name.Data(), fNRun))->Clone(); //in seconds
    TH1D *hvalueSumCoarse = (TH1D*) fileIn->Get(Form("DataQuality/hvalueSumCoarse_%s_%d", iter->name.Data(), fNRun))->Clone();
    TH1D *hvalueSquareSumCoarse = (TH1D*) fileIn->Get(Form("DataQuality/hvalueSquareSumCoarse_%s_%d", iter->name.Data(), fNRun))->Clone(); //in seconds
    TH1D *hnCounts = (TH1D*) fileIn->Get(Form("DataQuality/hnCounts_%s_%d", iter->name.Data(), fNRun))->Clone(); //in seconds
    TH1D *hnCountsCoarse = (TH1D*) fileIn->Get(Form("DataQuality/hnCountsCoarse_%s_%d", iter->name.Data(), fNRun))->Clone();

    for(int i=0;i< fNTimeBins; i++ ){
      (iter->valueSum)[i] = hvalueSum->GetBinContent(i+1);
      (iter->valueSquareSum)[i] = hvalueSquareSum->GetBinContent(i+1);
      (iter->nCounts)[i] = hnCounts->GetBinContent(i+1);
    }
    for(int i=0;i< fNTimeBinsCoarse; i++ ){
      (iter->valueSumCoarse)[i] = hvalueSumCoarse->GetBinContent(i+1);
      (iter->valueSquareSumCoarse)[i] = hvalueSquareSumCoarse->GetBinContent(i+1);
      (iter->nCountsCoarse)[i] = hnCountsCoarse->GetBinContent(i+1);
    }

    TGraphErrors *obsplotMean = new TGraphErrors();
    TGraph *obsplotSigma = new TGraph();
    TGraphErrors *obsplotCoarseMean = new TGraphErrors();
    TGraph *obsplotCoarseSigma = new TGraph();

    //poi SaveTGraph con 


    for(int i =0; i< fNTimeBins; i++ ){
        if(iter->nCounts[i]<2) continue;
        Double_t meanVal = (iter->valueSum)[i]/(iter->nCounts)[i];
        int NpointMean = obsplotMean->GetN();
        int NpointSigma = obsplotSigma->GetN(); 
        obsplotMean->SetPoint(NpointMean, i*fTimeBin,meanVal); //remember when used to shift by timestamp     
        Double_t sigmaVal = TMath::Sqrt((((iter->valueSquareSum)[i]/(iter->nCounts)[i])-(meanVal*meanVal))/((iter->nCounts)[i]-1.));
        obsplotSigma->SetPoint(NpointSigma, i*fTimeBin,sigmaVal);
        obsplotMean->SetPointError(NpointMean, 0.5*fTimeBin ,sigmaVal);

    }


    for(int i =0; i< fNTimeBinsCoarse; i++ ){
          if(iter->nCountsCoarse[i]<2) continue;
          int NpointMean = obsplotCoarseMean->GetN();
          int NpointSigma = obsplotCoarseSigma->GetN(); 
          Double_t meanVal = (iter->valueSumCoarse)[i]/(iter->nCountsCoarse)[i];
          obsplotCoarseMean->SetPoint(NpointMean, i*fTimeBinCoarse,meanVal); //remember when used to shift by timestamp     
          
          Double_t sigmaVal = TMath::Sqrt((((iter->valueSquareSumCoarse)[i]/(iter->nCountsCoarse)[i])-(meanVal*meanVal))/((iter->nCountsCoarse)[i]-1.));
          obsplotCoarseSigma->SetPoint(NpointSigma, i*fTimeBinCoarse,sigmaVal);
          obsplotCoarseMean->SetPointError(NpointMean, 0.5* fTimeBin,sigmaVal);
          

      }
    obsplotMean->SetName(Form("gMean_%s", (iter->name).Data()));
    obsplotCoarseMean->SetName(Form("gMeanCoarse_%s", (iter->name).Data()));
    obsplotSigma->SetName(Form("gSigma_%s", (iter->name).Data()));
    obsplotCoarseSigma->SetName(Form("gSigmaCoarse_%s", (iter->name).Data()));

    fHS->SaveTGraphList("DataQuality", obsplotMean->GetName(),  obsplotMean);
    fHS->SaveTGraphList("DataQuality", obsplotCoarseMean->GetName(),obsplotCoarseMean );
    fHS->SaveTGraphList("DataQuality", obsplotSigma->GetName(), obsplotSigma);
    fHS->SaveTGraphList("DataQuality", obsplotCoarseSigma->GetName(),obsplotCoarseSigma);
    if((iter->name).CompareTo("POT")==0){ 
      gPoTLGAll = (TGraphErrors*) obsplotMean->Clone();
      gPoTLG = (TGraphErrors*) obsplotCoarseMean->Clone();
    }
    else if((iter->name).CompareTo("POTTarg")==0){
      gPoTTarget = (TGraphErrors*) obsplotCoarseMean->Clone();
    }else if((iter->name).CompareTo("TargXCharge")==0) {
      gXChargeAll = (TGraphErrors*) obsplotMean->Clone();

      gXCharge = (TGraphErrors*) obsplotCoarseMean->Clone();
      }
    }
    if(gPoTLG->GetN()== gPoTTarget->GetN()){
        for(int i =0; i<gPoTLG->GetN(); i++){
          Double_t XLG,YLG, XTa, YTa, XCha, YCha;
          Double_t  XChaAll, YChaAll, XLGAll,YLGAll;
          gPoTLG->GetPoint(i, XLG, YLG);
          gPoTLGAll->GetPoint(i, XLGAll, YLGAll);
          gPoTTarget->GetPoint(i, XTa, YTa);
          gPoTratio->SetPoint(i,XLG, YTa/YLG);
          gXCharge->GetPoint(i, XCha, YCha);
          gXChargeAll->GetPoint(i, XChaAll, YChaAll);

          gPoTratioCharge->SetPoint(i,XLG, YCha/YLG);
          gPoTratioChargeAll->SetPoint(i,XLGAll, YChaAll/YLGAll);


        }
    }
    fHS->SaveTGraphList("DataQuality", gPoTratio->GetName(),gPoTratio);
    fHS->SaveTGraphList("DataQuality", gPoTratioCharge->GetName(),gPoTratioCharge);
    fHS->SaveTGraphList("DataQuality", gPoTratioChargeAll->GetName(),gPoTratioChargeAll);
    
  }
  if (fVerbose) printf("---> Finalizing DataQuality\n");
  return true;
}
