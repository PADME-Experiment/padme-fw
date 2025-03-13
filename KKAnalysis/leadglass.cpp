#include "TRandom.h"
#include "TMarker.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TSpline.h"
#include "TString.h"
#include "TRawEvent.hh"
#include "TF1.h"
#include "TButton.h"
#include <TSystem.h>

#include "Riostream.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

#define NTUPLE_N_BOARDS 1
#define NTUPLE_N_CHANNELS 2
#define VPP 1.

const int NPed = 100;
static const int NAvg = 1000;

const double DigThre = 1000; //mV digitizer saturation threshold
const double FEEThre = 1000; //mV, FEE saturation
const double fImpedance = 50.;
  
const double digiTime = 1.; // Sampled at 5 GS/s  ---> da prendere dalla rootupla
const double fTimeBin = digiTime*1e-9; // seconds

const int NChannels = 32; 

const double CFvalue = 0.11;
const double FixThre = 10.;

const double StopThre = 300.; //trigger
const double StartThre = 20.; //trigger

int go_ahead_flag = 0;
int write_to_file_flag = 1;

struct Eve{
  Int_t NTNevent;
  Double_t NTQCh[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTQCh1;
  Double_t NTPedCh[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTVMax[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTMax[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTThre[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTCFTime[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTTFit[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  Double_t NTChi2[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS];
  //Double_t Waves[NTUPLE_N_BOARDS][NTUPLE_N_CHANNELS][1024];
  //Double_t SampleTime[1024];
  
  Int_t NTTrigMask;
};

void go_ahead(){
  go_ahead_flag = 1;
}


int fillTGraph(TGraphErrors *graph, int i,  double x, double y, double dx, double dy) {
  graph->SetPoint(i, x, y);
  int n = graph->GetN();
  graph->SetPointError(n-1, dx, dy);
  return n;
}


// TH1S* GetHisto(TString name, TObjArray histos) {
//   for (Int_t iH = 0; iH < histos.GetEntries(); iH++) {
//     if ( ((TH1S*)histos.At(iH))->GetName() == name ) return (TH1S*)histos.At(iH);
//   }
//   return 0;
// }

// TProfile* GetProfile(TString name, TObjArray histos) {
//   for (Int_t iH = 0; iH < histos.GetEntries(); iH++) {
//     if ( ((TProfile*)histos.At(iH))->GetName() == name ) return (TProfile*)histos.At(iH);
//   }
//   return 0;
// }


int main(int argc, char* argv[]) {
  
  int c;
  int verbose = 0;
  int nevents = 0;

  TString inputFileName;
  TObjArray inputFileNameList;
  struct stat filestat;

  TString outputFileName = "RawHisto.root";
  Int_t event;
  std::vector<UInt_t> events;
  Int_t board;
  std::vector<UInt_t> boards;

  // Parse options
  while ((c = getopt (argc, argv, "i:l:o:n:e:b:vh")) != -1) {
    switch (c){
      case 'i':
        inputFileNameList.Add(new TObjString(optarg));
        fprintf(stdout,"Added input data file '%s'\n",optarg);
	      break;
      case 'l':
        if ( stat(Form(optarg),&filestat) == 0 ) {
          fprintf(stdout,"Reading list of input files from '%s'\n",optarg);
          std::ifstream inputList(optarg);
          while( inputFileName.ReadLine(inputList) ){
            if ( stat(Form(inputFileName.Data()),&filestat) == 0 ) {
              inputFileNameList.Add(new TObjString(inputFileName.Data()));
              fprintf(stdout,"Added input data file '%s'\n",inputFileName.Data());
            } else {
              fprintf(stdout,"WARNING: file '%s' is not accessible\n",inputFileName.Data());
            }
          }
        } else {
          fprintf(stdout,"WARNING: file list '%s' is not accessible\n",optarg);
        }
        break;
      case 'o':
        outputFileName = optarg;
        fprintf(stdout,"Output histogram file set to '%s'\n",optarg);
	      break;
      case 'n':
        if ( sscanf(optarg,"%d",&nevents) != 1 ) {
          fprintf (stderr, "Error while processing option '-n'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (nevents<0) {
          fprintf (stderr, "Error while processing option '-n'. Required %d events (must be >=0).\n", nevents);
          exit(1);
        }
        if (nevents) {
          fprintf(stdout,"Will read first %d events in file\n",nevents);
        } else {
          fprintf(stdout,"Will read all events in file\n");
        }
        break;
      case 'e':
        if ( sscanf(optarg,"%d",&event) != 1 ) {
          fprintf (stderr, "Error while processing option '-e'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (event<0) {
          fprintf (stderr, "Error while processing option '-e'. Required event %d (must be >=0).\n", event);
          exit(1);
        }
        events.push_back(event);
        fprintf(stdout,"Added event %d to list\n",event);
        break;
      case 'b':
        if ( sscanf(optarg,"%d",&board) != 1 ) {
          fprintf (stderr, "Error while processing option '-b'. Wrong parameter '%s'.\n", optarg);
          exit(1);
        }
        if (board<0 || board>31) {
          fprintf (stderr, "Error while processing option '-b'. Required board %d (must be 0<=board<=31).\n", board);
          exit(1);
        }
        boards.push_back(board);
        fprintf(stdout,"Added board %d to list\n",board);
        break;
      case 'v':
	      verbose++;
        break;
      case 'h':
        fprintf(stdout,"\nRawHisto [-i <file>] [-l <list>] [-o <file>] [-n <n>] [-e <event>] [-b <board>] [-v <level>] [-h]\n\n");
        fprintf(stdout,"  -i: add a file to list of input files (can be repeated)\n");
        fprintf(stdout,"  -l: specify text file with list of input files (one file per line)\n");
        fprintf(stdout,"  -o: define the name of the output file (default: %s)\n",outputFileName.Data());
        fprintf(stdout,"  -n: total number of events to save to output file (default: 0 i.e. no limit)\n");
        fprintf(stdout,"  -e: add <event> to list of events to save to output file (can be repeated)\n");
        fprintf(stdout,"  -b: add <board> to list of boards to save to output file (can be repeated)\n");
        fprintf(stdout,"  -v: enable verbose output (repeat for more output)\n");
        fprintf(stdout,"  -h: show this help message and exit\n\n");
        fprintf(stdout,"N.B. if no -e/-b options are specified, then all events/boards will be saved to the output file\n\n");
        exit(0);
      case '?':
        if (optopt == 'i' || optopt == 'l' || optopt == 'o' || optopt == 'n' || optopt == 'e' || optopt == 'b')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
              else if (isprint(optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
              else
                fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
              exit(1);
            default:
              abort();
    }
  }

  // Check if the input file list is empty
  if ( inputFileNameList.GetEntries() == 0 ) {
    fprintf(stderr,"ERROR - Input file list is empty\n");
    exit(1);
  }

  if (verbose) fprintf(stdout,"Set verbose level to %d\n",verbose);

  // Create chain of input files
  fprintf(stdout,"=== === === Chain of input files === === ===\n");
  TChain* inputChain = new TChain("RawEvents");
  for (Int_t iFile = 0; iFile < inputFileNameList.GetEntries(); iFile++) {
    fprintf(stdout,"%4d %s\n",iFile,((TObjString*)inputFileNameList.At(iFile))->GetString().Data());
    inputChain->AddFile(((TObjString*)inputFileNameList.At(iFile))->GetString());
  }
  if (inputChain->GetEntries() == 0) {
    perror(Form("ERROR No events found for tree 'RawEvents' in input chain"));
    exit(1);
  }

  // Get some info about the input chain
  Int_t runNEntries = inputChain->GetEntries();
  std::cout << "Found Tree 'RawEvents' with " << runNEntries << " entries" << std::endl;
  //for(Int_t i=0; i < inputChain->GetListOfBranches()->GetEntries(); i++) {
  //  std::cout << "Branch " << i << " is " << inputChain->GetListOfBranches()->At(i)->GetName() << std::endl;
  //}

  TRawEvent* rawEv = new TRawEvent();
  inputChain->SetBranchAddress("RawEvent",&rawEv);

  // Create ntuple -- maybe not needed in this case?
  Eve Event; 
  
  TTree* tree = new TTree("NTU","Event3");
  tree->Branch("Nevent",&(Event.NTNevent),"Nevent/I");
  tree->Branch("QCh",&(Event.NTQCh),Form("QCh[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("QCh1",&(Event.NTQCh1),"QCh1/D");
  tree->Branch("PedCh",&(Event.NTPedCh),Form("PedCh[%d][%d]/D,",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("VMax",&(Event.NTVMax),Form("VMax[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TMax",&(Event.NTTMax),Form("TMax[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TThre",&(Event.NTTThre),Form("TThre[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("CFTime",&(Event.NTCFTime),Form("CFTime[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("TFit",&(Event.NTTFit),Form("TFit[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  tree->Branch("Chi2",&(Event.NTChi2),Form("Chi2[%d][%d]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  // tree->Branch("Time",&(Event.SampleTime),Form("Time[%d]/D", 1024));

  tree->Branch("TrigMask",&(Event.NTTrigMask),Form("TrigMask/I"));
  //tree->Branch("Waves",&(Event.Waves), Form("Waves[%d][%d][1024]/D",NTUPLE_N_BOARDS,NTUPLE_N_CHANNELS));
  
  // Create output file for histograms
  TFile* histoFile = new TFile(outputFileName,"RECREATE");
  if(!histoFile) {
    fprintf(stderr,"ERROR - Cannot create output file %s\n",outputFileName.Data());
    exit(1);
  }

  // Create more histograms
  TH1F* hCh1Charge = new TH1F("hCh1Charge","hCh1Charge; Q [pC]; Number of events",625,-5.,120.);
  TGraph grCharge; grCharge.SetName("grCharge"); grCharge.SetTitle("grCharge; Event number; Q [pC]"); //Graph of the charge in the run -- for data quality check


  // Define parameters for signal analysis  
  Double_t Sample[1024];
  Double_t RecoSample[1024];
  Double_t AbsRecoSample[1024];
    
  Int_t readEvent = 0;
  //Double_t SpikeCheck = 1.; //mV
  //Double_t OVThre[NThresholds]= {1,2,3,5,7,10,12};

  // Loop over the events
  for(Int_t iev=0; iev<runNEntries; iev++){
    double toss = gRandom->Uniform(0, runNEntries/100);
    Bool_t Saturated = false;
    
    // Read event info
    inputChain->GetEntry(iev);
    Int_t runNumber = rawEv->GetRunNumber();
    Int_t evtNumber = rawEv->GetEventNumber();
    // Check if this event number was selected
    if ( (events.size() > 0) && (std::count(events.begin(),events.end(),evtNumber) == 0) ) {
      continue;
    }
    readEvent++;
    if ( nevents && (readEvent>nevents) ) {
      printf("- Read %d event(s): stopping here\n",nevents);
      break;
    }

    // Show some activity meter
    if ( readEvent % 1000 == 0 ) {
      printf("Run: %7d ------> Processing event: %8d/%d (%.0f%%)\n",runNumber,readEvent,runNEntries,(float)(100*readEvent/runNEntries));
    }

    // Get event trigger mask and select cosmics events
    //UInt_t trigMask = rawEv->GetEventTrigMask();
    //if ( !(trigMask & 0x01) ) continue; //da rimettere
    //Event.NTTrigMask= trigMask;
    
    // Loop over boards, ORA NON SERVE
    for(UChar_t brd = 0; brd<rawEv->GetNADCBoards(); brd++){

      // Check if this board was selected
      Int_t brdID = rawEv->ADCBoard(brd)->GetBoardId();
      if ( (boards.size() > 0) && (std::count(boards.begin(),boards.end(),brdID) == 0) ){
        continue;
      }

      TADCBoard* adcB = rawEv->ADCBoard(brd);
      UChar_t nTrg = adcB->GetNADCTriggers();
      UChar_t nChn = adcB->GetNADCChannels();

      // Analyse the signals
      if (brdID == 0 || brdID == 1) { //to be done not hardcoded

        // Loop over the channels
        for(UChar_t chan=0; chan<nChn; chan++){

          TGraphErrors WaveGraph(NAvg-1);

          int tStart= 250; //prima era 100
          int tStop = 25;        
        
          Double_t Charge = 0.;
          Double_t ChargeCh1;
          Double_t MaxHeight = 5000.;
          Double_t MaxSample = 0.;
          Double_t PedTemp = 0.;
          TADCChannel* ADCChn = adcB->ADCChannel(chan);
          UChar_t ChnNum = ADCChn->GetChannelNumber();

          // Loop over the samples
          for(UShort_t s=0; s<NAvg; s++){
            Sample[s] = (Double_t) ADCChn->GetSample(s);
            if(Sample[s] < MaxHeight){
              MaxHeight = Sample[s];
              MaxSample = (Double_t) s;
            }
          }

          // Check if channels has some signal (poor man zero suppression)

          Double_t rms1000 = TMath::RMS(NAvg,&Sample[0]); // get the rms of the samples

          //Double_t rms100 = TMath::RMS(NPed,&Sample[0]);

          //if(rms1000<10 && brdId!=29) continue; //2 mV Zsupp on BGOs brd



          // Compute the PEDESTAL
          PedTemp  = TMath::Mean(NPed,&Sample[0]);
          Double_t baseFrom{0},baseTo{0},baselineSum{0},baseN{0};

          // if( rms100>10 || trigMask>3)
          if(1) {
          // MaxSample>360 ? ( bFrom = MaxSample-350,  bTo = MaxSample-250) : ( bFrom = MaxSample+350,  bTo = MaxSample+450) ;
            baseFrom = 0,  baseTo = 150;
            for(Int_t s = baseFrom; s < baseTo; s++) {  
              baselineSum += Sample[s]; 
              baseN++;
            }
            PedTemp = (Double_t)baselineSum/(Double_t)baseN;
          }
  

          Double_t TatThre = -999;
          // Get total charge and position of maximum
          Double_t VMax = -999;
          Double_t TMax = -999.;
          Double_t CFTimeTmp = -999;
          Double_t TFromFit = -999;

          // Loop over the samples
          for(UShort_t s=0; s<NAvg; s++){
            Sample[s] = (Double_t)ADCChn->GetSample(s);          
            
            if(brdID == 0 && chan == 0){ //eventuali segnali negativi
              AbsRecoSample[s] = VPP*(PedTemp-Sample[s])/4096.*1000.;//Signal in mV
            } else{
              AbsRecoSample[s] = VPP*(Sample[s]-PedTemp)/4096.*1000.;//Signal in mV
            }

            //AbsRecoSample[s] = VPP*(TMath::Abs(Sample[s]-PedTemp))/4096.*1000.; //Signal in mV
            
            fillTGraph(&WaveGraph, s, s, AbsRecoSample[s], 0, 0);
            if (AbsRecoSample[s] > VMax) {  
              VMax = AbsRecoSample[s];
              TMax = (Double_t)s;
            }
            
          } // end of loop on samples

          //Check saturation HERE
          if(VMax>FEEThre) Saturated = true;        
        

          //Compute CHARGES
          Int_t qStart = TMax-20/1.;  //PROVA 2.5GS
          Int_t qStop = TMax+50/1.;   //it will be needed a different window for tagger and SiPMs

          for(UShort_t t=0; t<NAvg; t++){
            if(t>qStart && t<qStop){
              Charge += (AbsRecoSample[t]*1e-3)/fImpedance*fTimeBin/1E-12; //charge in pC
            }
          }

          //Get the charge in first channel
          if(brdID == 0 && chan == 0){
            ChargeCh1 = Charge;
            hCh1Charge->Fill(Charge);
            grCharge.AddPoint(grCharge.GetN(),Charge);
          }

          Int_t idx = brdID;

          //CF time evaluation through Spline Intertpolation
          if(1){ // before  5
            TSpline5 waveSp = TSpline5("wsp", &WaveGraph); 
            auto waveSpFun = [&waveSp](double *x, double *){ return waveSp.Eval(x[0]); };
            TF1 waveFitFun = TF1("fitf", waveSpFun ,(TMax-tStart) , (TMax+tStop), 0); 
            Double_t thr = VMax * CFvalue;
            // if(brdId == 29 && (c == 19 || c==20)){
            //   TF1 *FitFun = new TF1("FitFun","landau",(TMax-tStart),(TMax+tStop));

            //   waveGra.Fit(FitFun, "REMQ", "",(TMax-tStart),(TMax+tStop));
                
            //   //std::cout<<"!!!!!!!!!! Sbaglio con  il fit !!!!!!!! ev:"<<iev<<" b "<<(int)b<<" ch "<<(int)c<<std::endl;  
            //   CFTimeTmp = FitFun->GetX(thr);   
            //   TatThre = 0; // FitFun->GetX(FixThre);
            //   TFromFit =  FitFun->GetParameter(2);// WoodSaxonFun->GetParameter(0);

            //   Event.NTChi2[idx][ch] =  FitFun->GetChisquare()/FitFun->GetNDF();
            //   FitFun->ReleaseParameter(0);
            // }else{
              //std::cout<<"!!!!!!!!!! Sbaglio con la spline fit !!!!!!!!ev:"<<iev<<" b "<<(int)b<<" ch "<<(int)c<<std::endl;      c = new TCanvas("c", "c");
              CFTimeTmp = waveFitFun.GetX(thr);
              TatThre = 0; // waveFitFun.GetX(FixThre);
              TFromFit =  0;// WoodSaxonFun->GetParameter(0);
            //}
            // CFTimeTmp = waveFitFun.GetX(thr);
            // TatThre = waveFitFun.GetX(FixThre);
            

            // Save values into ntuple
            //TO DO --> save waveforms with a flag
            Event.NTQCh[idx][chan] = Charge;
            Event.NTQCh1 = ChargeCh1;
            Event.NTPedCh[idx][chan] = PedTemp;
            Event.NTVMax[idx][chan] = VMax;
            Event.NTTMax[idx][chan] = TMax * digiTime;
            Event.NTCFTime[idx][chan] = CFTimeTmp * digiTime;
            
            // for(UShort_t t=0;t<1024;t++){
            //   if(t<NAvg){
            //   Event.Waves[idx][ch][t] = AbsSamRec[t];
            //   Event.SampleTime[t]=t*digiTime;
            //   }else{
            //   Event.Waves[idx][ch][t] = 0.;
            //   }
            //   }

            // for(UShort_t s=0;s<NAvg;s++){
            //       Sam[s] = (Double_t) chn->GetSample(s);          
            //       if(brdId == 0 && (c ==18 || c== 19 || c==20)){ //eventuali segnali negativi
            //         AbsSamRec[s] = 2*(PedTemp-Sam[s])/4096.*1000.;//Signal in mV, 2 per Vpp
            //       }else{
            //         AbsSamRec[s] = 2*(Sam[s]-PedTemp)/4096.*1000.;//Signal in mV
            //       }
                  
            //       fillGraph(&waveGra,s, s, AbsSamRec[s], 0, 0);
            //       if (AbsSamRec[s] > VMax) {  
            //         VMax = AbsSamRec[s];
            //         TMax = (Double_t)s;
            //       }  
            // } // end of loop on samples

            //Saves some waveforms for diagnostics
            if (toss < 1) { //toss < 1 &&

              histoFile->cd();
              TCanvas *cc = new TCanvas(Form("e%d_b%d_c%d", iev,brd,chan)); cc->cd(); 

              WaveGraph.SetLineWidth(0); WaveGraph.SetMarkerStyle(20); WaveGraph.SetMarkerSize(.2); WaveGraph.SetMarkerColor(kBlue); WaveGraph.Draw(); 
              waveFitFun.SetLineColor(kTeal); waveFitFun.Draw("same");
              waveSp.SetLineColor(kBlack); waveSp.Draw("same");
              TMarker tp = TMarker(CFTimeTmp, waveSp.Eval(CFTimeTmp), 2); tp.SetMarkerSize(3); tp.SetMarkerColor(kRed); tp.Draw("same"); 
              TLine lqstart = TLine(qStart, 0, qStart, VMax );  lqstart.SetLineColor(kBlue); lqstart.Draw("same"); 
              TLine lqstop = TLine(qStop,0, qStop, VMax);  lqstop.SetLineColor(kBlue); lqstop.Draw("same");
              TLine ltstart = TLine((TMax-tStart), 0, (TMax-tStart), VMax );  ltstart.SetLineColor(kGreen+3); ltstart.Draw("same"); 
              TLine ltstop = TLine((TMax+tStop),0, (TMax+tStop), VMax);  ltstop.SetLineColor(kGreen+3); ltstop.Draw("same");

              TMarker tMax = TMarker(TMax, WaveGraph.Eval(TMax), 2); tMax.SetMarkerSize(3); tMax.SetMarkerColor(kGreen+3); tMax.Draw("same");
              TLine PedEnd=  TLine(NPed, 0, NPed, VMax);  PedEnd.SetLineColor(kGreen); PedEnd.Draw("same"); 
              TLine t0 = TLine(baseFrom,-5, baseFrom, VMax/5);  t0.SetLineColor(kPink); t0.Draw("same");
              TLine t1 = TLine(baseTo, -5, baseTo, VMax/5);  t1.SetLineColor(kPink); t1.Draw("same");
            
              //cc->Draw();
              //cc->Write(); 
              histoFile->cd();
            }
          } // if su rms1000  
        } // end loop on channels
      


      // for(UChar_t t=0;t<nTrg;t++){

      //   TGraphErrors waveGra(NAvg-1);
        
      //   Double_t Charge = 0.;

      //   TADCTrigger* trg = adcB->ADCTrigger(t);
      //   //UChar_t tr= trg->GetGroupNumber();
      //   for(UShort_t s=0;s<NAvg;s++){
      //     Sam[s] = (Double_t) trg->GetSample(s);
      //   }
        
      //   // Check if channels has some signal (poor man zero suppression)
      //   Double_t rms1000 = TMath::RMS(NAvg,&Sam[0]);
        
      //   if(rms1000<10 || trigMask>3) continue; //2 mV Zsupp on BGOs brd

      //   // Compute pedestal
      //   Double_t PedTemp = TMath::Mean(NPed,&Sam[0]);
      //   Double_t TatThre = 0;
      //   Double_t TFromFit =-999;
      //   //ch == 0 ? TFromFit=-999 : TFromFit=-555;
      //   // Get total charge and position of maximum
      //   Double_t VMax = 0.;
      //   Double_t TMax = -999.;
      //   Double_t CFTimeTmp = 0;
      //   for(UShort_t s=0;s<NAvg;s++){
      //     Sam[s] = (Double_t) trg->GetSample(s);          
      //     AbsSamRec[s] = (PedTemp-Sam[s])/4096.*1000.;//Signal in mV
      //     fillGraph(&waveGra,s, s*digiTime, AbsSamRec[s], 0, 0);
      //     if (AbsSamRec[s] > VMax) {
      //         VMax = AbsSamRec[s];
      //         TMax = (Double_t)s;
      //     }
          
      //   } // end of loop on samples

      
      // 
      //   
      //  

      //   
        
      //   
      //   }
      // }//end loop on triggers
      
    
      // for(UChar_t t=0;t<nTrg;t++){

      //   TGraphErrors waveGra(NAvg-1);
        
      //   Double_t Charge = 0.;

      //   TADCTrigger* trg = adcB->ADCTrigger(t);
      //   //UChar_t tr= trg->GetGroupNumber();
      //   for(UShort_t s=0;s<NAvg;s++){
      //     Sam[s] = (Double_t) trg->GetSample(s);
      //   }
        
      //   // Check if channels has some signal (poor man zero suppression)
      //   Double_t rms1000 = TMath::RMS(NAvg,&Sam[0]);
        
      //   if(rms1000<10 || trigMask>3) continue; //2 mV Zsupp on BGOs brd

      //   // Compute pedestal
      //   Double_t PedTemp = TMath::Mean(NPed,&Sam[0]);
      //   Double_t TatThre = 0;
      //   Double_t TFromFit =-999;
      //   //ch == 0 ? TFromFit=-999 : TFromFit=-555;
      //   // Get total charge and position of maximum
      //   Double_t VMax = 0.;
      //   Double_t TMax = -999.;
      //   Double_t CFTimeTmp = 0;
      //   for(UShort_t s=0;s<NAvg;s++){
      //     Sam[s] = (Double_t) trg->GetSample(s);          
      //     AbsSamRec[s] = (PedTemp-Sam[s])/4096.*1000.;//Signal in mV
      //     fillGraph(&waveGra,s, s*digiTime, AbsSamRec[s], 0, 0);
      //     if (AbsSamRec[s] > VMax) {
      //         VMax = AbsSamRec[s];
      //         TMax = (Double_t)s;
      //     }
          
      //   } // end of loop on samples

      
      //   // Int_t qStart = TMax-50; 
      //   // Int_t qStop = TMax+200;
      //   Double_t TatStart = -999;
      //   Double_t TatStop = -999;
      //   //NIM SIGNAL --> should be okay
      //   for(UShort_t g=0;g<NAvg;g++){
      //   if(AbsSamRec[g] >= StartThre){
      //       TatStart = g*digiTime;
      //       //std::cout<<"TatStart: "<<TatStart<<std::endl;
      //       break;
      //   }
      //   }
      //   for(UShort_t g=0;g<NAvg;g++){
      //     if(AbsSamRec[g] >= StopThre){
      //         TatStop = g*digiTime;
      //         //std::cout<<"TatStop: "<<TatStop<<std::endl;

      //         break;
      //     }
      //   }
        
      //   Int_t idx = brdId-29;
      //   //CF time evaluation through Spline Intertpolation only for SiPMs
      //   TMax*=digiTime; 

      //   TSpline5 waveSp = TSpline5("wsp", &waveGra); 
      //   auto waveSpFun = [&waveSp](double *x, double *){ return waveSp.Eval(x[0]); }; 
      //   TF1 waveFitFun = TF1("fitf", waveSpFun, 0, 1000*digiTime, 0);  //OCCHIO MOLTO HARDCODED
      //   //std::cout<<"TStart: "<<(TMax-tStart)<<" TStop: "<<TMax+tStop<<std::endl;
      //   Double_t thr = 200; //VMax * CFvalue;
      //   //std::cout<<"thr: "<<thr<<std::endl;
      //   // CFTimeTmp = waveFitFun.GetX(thr);   
      //   // TatThre = waveFitFun.GetX(FixThre);
        
      //   TMax = waveFitFun.GetMaximumX();
      //   //std::cout<<"This GetX is for TatStart and Stop"<<std::endl;

      //   // TatStop = waveFitFun.GetX(StopThre);
      //   // TatStart = waveFitFun.GetX(StartThre);

        
      //   //std::cout<<"StartThre: "<<StartThre<<" TatStart: "<<TatStart<<" StopThre:"<<StopThre<<" TatStop:" <<TatStop<<std::endl;
      //   //Fit with WoodSaxon
      //   Double_t WSstart= TatStart;//-5.8;
      //   Double_t WSstop= TatStop;//-3.6;
          

      //   TF1 *WoodSaxonFun = new TF1("WoodSaxonFun","pol2",WSstart,WSstop);
      //   // WoodSaxonFun->SetParameter(0,120);
      //   // WoodSaxonFun->SetParLimits(0,80,160);
      //   // WoodSaxonFun->SetParameter(1,1);
      //   // WoodSaxonFun->SetParLimits(1,5e-2,5e4);
      //   // WoodSaxonFun->FixParameter(2,420);
      //   // WoodSaxonFun->SetParLimits(2,300,500);
      //   //WoodSaxonFun->SetParameter(3,200);
      //   // WoodSaxonFun->SetParameter(4,10);
      //   // WoodSaxonFun->SetParameter(5,-1);
      //   // WoodSaxonFun->SetParameter(6,4.5);
      //   // WoodSaxonFun->SetParameter(7,-0.05);
      //   // WoodSaxonFun->SetParameter(8,-0.02);
      //   //std::cout<<"This GetX is for CFTime and co"<<std::endl;

      //     waveGra.Fit(WoodSaxonFun, "REMQ", "",WSstart,WSstop);
          

      //     TF1 *WoodSaxonFun2 = new TF1("WoodSaxonFun2","pol2",WSstart-50,WSstop);
      //     WoodSaxonFun2->SetParameters(WoodSaxonFun->GetParameters());

      //     // CFTimeTmp = WoodSaxonFun->GetX(thr);   
      //     // TatThre = WoodSaxonFun->GetX(FixThre);
      //     // std::cout  << (int) t <<std::endl;
      //     // std::cout << iev<<std::endl;
            
      //     TFromFit =  WoodSaxonFun2->GetX(0.);// WoodSaxonFun->GetParameter(0);

          
      //     WoodSaxonFun->ReleaseParameter(0);
      //     // Save values into ntuple
        
      //     Event.NTQCh[idx][NChannels+t] = Charge;
      //     Event.NTPedCh[idx][NChannels+t] = PedTemp;
      //     Event.NTVMax[idx][NChannels+t] = VMax;
            
      //     Event.NTTMax[idx][NChannels+t] = TMax; // * digiTime;
      //     Event.NTTThre[idx][NChannels+t] = TatThre; // * digiTime;
      //     Event.NTCFTime[idx][NChannels+t] = CFTimeTmp;// * digiTime;
      //     Event.NTTFit[idx][NChannels+t] = TFromFit;
      //     Event.NTChi2[idx][NChannels+t] =  WoodSaxonFun->GetChisquare()/WoodSaxonFun->GetNDF();
        
      //   if (trigMask<3  && toss<1) {
      //     histoFile->cd(specimens);
      //     TCanvas *cc = new TCanvas(Form("e%d_b%d_ch%d_trigger", iev,b,NChannels+t)); cc->cd(); 
      //     //if(b==1 && Charge < 10) continue;
          
      //     //waveGra.SetTitle(Form("bl=%.3f br=%.3f pk=%.2f pt=%.2f rt=%.2f qq=%.2f", blineTmp, brmsTmp, peakTmp, peakTimeTmp, timeTmp, chargeTmp));
      //     //waveGra.GetXaxis()->SetRangeUser(100,150);
      //     waveGra.SetLineWidth(0); waveGra.SetMarkerStyle(20); waveGra.SetMarkerSize(.2); waveGra.SetMarkerColor(kBlue); waveGra.Draw(""); 
      //     waveFitFun.SetLineColor(kTeal); waveFitFun.Draw("same");
          
      //     waveSp.SetLineColor(kBlack); waveSp.Draw("same");
      //     // TMarker tp = TMarker(CFTimeTmp, WoodSaxonFun-> Eval(CFTimeTmp), 2); tp.SetMarkerSize(3); tp.SetMarkerColor(kRed); tp.Draw("same"); 
      //     // TMarker tf = TMarker(TatThre, WoodSaxonFun->Eval(TatThre), 2); tf.SetMarkerSize(3); tf.SetMarkerColor(kBlue); tf.Draw("same"); 
      //     TMarker ts = TMarker(WSstart,StartThre ,2); ts.SetMarkerSize(3); ts.SetMarkerColor(kBlue+3); ts.Draw("same"); 
      //     TMarker tst = TMarker(WSstop,StopThre,2); tst.SetMarkerSize(3); tst.SetMarkerColor(kRed+3); tst.Draw("same"); 
      //     cc->Write(); 
      //     histoFile->cd();
        
      //   }
      // }//end loop on triggers
      
      } // end board selection

    } // end loop on boards
    
  Event.NTNevent = iev;
  
  if(Saturated == false){
    tree->Fill();
  }  //va fixato


  //memset(&Event, 0, sizeof(Event));

  // Clear event
  rawEv->Clear("C");
  
  } // end loop on events


  histoFile->cd();
  tree->Write();

  //TDirectory *histoDir = (TDirectory*)histoFile->mkdir("histos");
  //histoFile->mkdir("myHistos");
  
  TString baseName = outputFileName;
  //baseName = baseName(baseName.Last('/') + 1);
  baseName = baseName(baseName.Last('_') +1, 100);
  baseName = baseName(0, baseName.Last('.'));
  printf("%s\n",baseName.Data());

  //TCanvas* cQ = new TCanvas(); cQ->cd();
  //hCh1Charge->SetTitle("");

  hCh1Charge->Draw();
  //hCh1Charge->Write("hCh1Charge");
  //cQ->Draw();
  //cQ->Write();
  //cQ->SaveAs(Form("%s_hCh1Charge.png",baseName.Data()));

  TCanvas* cgrQ = new TCanvas("cgrQ"); cgrQ->cd();
  grCharge.SetTitle(baseName.Data());
  grCharge.SetMarkerStyle(6); grCharge.SetMarkerSize(.4); grCharge.SetMarkerColor(kBlue+2); grCharge.Draw("AP");
  grCharge.Write("grCharge");
  //cgrQ->Draw();
  //cgrQ->Write();
  //cgrQ->SaveAs(Form("%s_grCharge.png",baseName.Data()));
  //cgrQ->SaveAs(Form("%s_grCharge.eps",baseName.Data()));

  histoFile->Write();

  // Save and close output file
  printf("Closing output file\n");
  printf("\t\t\t----------> root -l %s \n",outputFileName.Data());
  histoFile->Close();

  delete rawEv;

  exit(0);
}