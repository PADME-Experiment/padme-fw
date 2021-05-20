#include <iostream>

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>

#include "PadmeAnalysisEvent.hh"

#include "HistoSvc.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoSvc* HistoSvc::fInstance = 0;
HistoSvc* HistoSvc::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HistoSvc(); }
  return fInstance;
}

HistoSvc::HistoSvc()
 :fRootOutputFile(0),fNtuple(0)
{
  // Clear histogram map
  fListMap.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoSvc::~HistoSvc()
{
  if ( fRootOutputFile ) delete fRootOutputFile;
}

Bool_t HistoSvc::Initialize(TString outputFileName)
{

  fOutputFileName = outputFileName;
  fRootOutputFile = new TFile(fOutputFileName,"RECREATE");
  if (fRootOutputFile) {
    printf("Output ROOT file '%s' successfully opened\n",outputFileName.Data());
  } else {
    printf(" HistoSvc::Initialize - ERROR - problem creating the ROOT TFile '%s'\n",fOutputFileName.Data());
    return false;
  }

  // Create default histogram lists
  CreateList("TOP");
  CreateList("EXTRA");

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t HistoSvc::Finalize()
{

  std::map<std::string, std::map<std::string,TObject*>>::iterator it;
  for (it = fListMap.begin(); it != fListMap.end(); it++) {
    printf("List %s has %lu histograms\n",it->first.c_str(),it->second.size());
    std::map<std::string,TObject*>::iterator jt;
    for (jt = it->second.begin(); jt != it->second.end(); jt++) {
      printf("- %s\n",jt->first.c_str());
    }
  }

  if (fRootOutputFile) {
    fRootOutputFile->Write();
    fRootOutputFile->Close();
    printf("\n---> Histogram file closed with size %lld\n",fRootOutputFile->GetSize());
  }

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TH1D* HistoSvc::BookHisto (std::string name, Int_t nx, Double_t xlow, Double_t xup)
{
  return BookHistoList("EXTRA",name,nx,xlow,xup);
}
TH2D* HistoSvc::BookHisto2(std::string name, Int_t nx, Double_t xlow, Double_t xup, Int_t ny, Double_t ylow, Double_t yup)
{
  return BookHisto2List("EXTRA",name,nx,xlow,xup,ny,ylow,yup);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto(std::string hname, Double_t xbin, Double_t weight)
{
  FillHistoList("EXTRA",hname,xbin,weight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto2(std::string hname, Double_t xbin, Double_t ybin, Double_t weight)
{
  FillHisto2List("EXTRA",hname,xbin,ybin,weight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void HistoSvc::Normalize(std::string hname, Double_t fac)
//{
//   if (fHisto1DMap.find(hname)!=fHisto1DMap.end()) {
//     if  (fHisto1DMap[hname]) {
//       fHisto1DMap[hname]->Scale(fac);
//     }
//     else{
//       std::cout << "---> warning from HistoSvc::FillHisto() : histo " << hname
//		 << " found in the map with a NULL pointer "
//		 << std::endl;
//     }
//     //    std::cout << "---> warning from HistoSvc::FillHisto() : histo2D " << hname
//     //           << ", to be normalized, does not exist. " << xbin << " ybin=" << ybin <<" weight=" << weight << ")"
//     //           << std::endl;
//    return;
//   }
//   else{
//     if (fHisto2DMap.find(hname)!=fHisto2DMap.end()) {
//       if  (fHisto2DMap[hname]) {
//	 fHisto2DMap[hname]->Scale(fac);
//       }
//       else{
//	 std::cout << "---> warning from HistoSvc::NormalizeHisto() : histo2D " << hname
//		   << " found in the map with a NULL pointer "
//		   << std::endl;
//       }
//       return;
//     }
//     else
//       {
//	 std::cout << "---> warning from HistoSvc::NormalizeHisto() : histo " << hname
//		   << " NOT found in the map of 1D histos nor in the map of 2D histos"
//		   << std::endl;
//	 return;
//       }
//   }
//}
   

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t HistoSvc::CreateList(std::string listname)
{

  if (fListMap.find(listname) != fListMap.end()) {
    printf("HistoSvc::CreateList - ERROR - List %s already defined\n",listname.c_str());
    return false;
  }

  printf("Creating list %s\n",listname.c_str());
  fListMap[listname].clear();

  // For each list create a corresponding directory in the output file
  if (listname.compare("TOP") != 0 && fRootOutputFile) {
    fRootOutputFile->cd("/");
    fRootOutputFile->mkdir(listname.c_str());
  }

  return true;

}

TH1D* HistoSvc::BookHistoList (std::string listname, std::string hname, Int_t nx, Double_t xlow, Double_t xup)
{
  TH1D* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      if (listname.compare("TOP")!=0) fRootOutputFile->cd(listname.c_str());
    }
    h = new TH1D(hname.c_str(), hname.c_str(), nx, xlow, xup);
    fListMap[listname][hname] = (TObject*)h;
  } else {
    printf("HistoSvc::BookHistoList - ERROR - list %s does not exist\n",listname.c_str());
  }
  return h;
}

TH2D* HistoSvc::BookHisto2List (std::string listname, std::string hname, Int_t nx, Double_t xlow, Double_t xup, Int_t ny, Double_t ylow, Double_t yup)
{
  TH2D* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      if (listname.compare("TOP")!=0) fRootOutputFile->cd(listname.c_str());
    }
    h = new TH2D(hname.c_str(), hname.c_str(), nx, xlow, xup, ny, ylow, yup);
    fListMap[listname][hname] = (TObject*)h;
  } else {
    printf("HistoSvc::BookHisto2List - ERROR - list %s does not exist\n",listname.c_str());
  }
  return h;
}

void HistoSvc::FillHistoList(std::string listname, std::string hname, Double_t xbin, Double_t weight=1.0)
{
  if (fListMap.find(listname) != fListMap.end()) {
    TH1D* h = (TH1D*)fListMap[listname][hname];
    if (h) {
      h->Fill(xbin,weight);
    } else {
      printf("HistoSvc::FillHistoList - ERROR - TH1D '%s' not found in list %s\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::FillHistoList - ERROR - list %s does not exist\n",listname.c_str());
  }
}

void HistoSvc::FillHisto2List(std::string listname, std::string hname, Double_t xbin, Double_t ybin, Double_t weight=1.0)
{
  if (fListMap.find(listname) != fListMap.end()) {
    TH2D* h = (TH2D*)fListMap[listname][hname];
    if (h) {
      h->Fill(xbin,ybin,weight);
    } else {
      printf("HistoSvc::FillHisto2List - ERROR - TH2D '%s' not found in list %s\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::FillHisto2List - ERROR - list %s does not exist\n",listname.c_str());
  }
}

void HistoSvc::BookNtuple()
{ 

  // Ntuple will go to the top directory of the output file
  if (fRootOutputFile) fRootOutputFile->cd("/");

  // Create flat ntuple
  fNtuple = new TTree("PADME_FlatNT", "PADME_Event");

  fNtuple->Branch("NTEvent_Run_Number"  , &(myEvt.NTEvent_Run_Number)  , "NTEvent_Run_Number/I"  );
  fNtuple->Branch("NTEvent_Event_Number", &(myEvt.NTEvent_Event_Number), "NTEvent_Event_Number/I");
  fNtuple->Branch("NTEvent_Reco_Time"   , &(myEvt.NTEvent_Reco_Time)   , "NTEvent_Reco_Time/D"   );
  fNtuple->Branch("NTEvent_Event_Time"  , &(myEvt.NTEvent_Event_Time)  , "NTEvent_Event_Time/D"  ); 
  fNtuple->Branch("NTEvent_Run_Clock"   , &(myEvt.NTEvent_Run_Clock)   , "NTEvent_Run_Clock/l"   );
  fNtuple->Branch("NTEvent_Event_Status", &(myEvt.NTEvent_Event_Status), "NTEvent_Event_Status/i");
  fNtuple->Branch("NTEvent_Trigger_Mask", &(myEvt.NTEvent_Trigger_Mask), "NTEvent_Trigger_Mask/i");

  fNtuple->Branch("NTNTarget_Hits"     ,    &(myEvt.NTNTarget_Hits)   ,                            "NTNTarget_Hits/I"   );
  fNtuple->Branch("NTTarget_Hits_ChannelId",&(myEvt.NTTarget_Hits_ChannelId),"NTTarget_Hits_ChannelId[NTNTarget_Hits]/I");
  fNtuple->Branch("NTTarget_Hits_Energy",   &(myEvt.NTTarget_Hits_Energy),      "NTTarget_Hits_Energy[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Time",     &(myEvt.NTTarget_Hits_Time),          "NTTarget_Hits_Time[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Xpos",     &(myEvt.NTTarget_Hits_Xpos),          "NTTarget_Hits_Xpos[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Ypos",     &(myEvt.NTTarget_Hits_Ypos),          "NTTarget_Hits_Ypos[NTNTarget_Hits]/D");
  fNtuple->Branch("NTTarget_Hits_Zpos",     &(myEvt.NTTarget_Hits_Zpos),          "NTTarget_Hits_Zpos[NTNTarget_Hits]/D");

  fNtuple->Branch("NTTargetBeam_X"    ,        &(myEvt.NTTargetBeam_X),                  "NTTargetBeam_X/D");
  fNtuple->Branch("NTTargetBeam_XErr" ,        &(myEvt.NTTargetBeam_XErr),            "NTTargetBeam_XErr/D"); 
  fNtuple->Branch("NTTargetBeam_XW"   ,        &(myEvt.NTTargetBeam_XW),                "NTTargetBeam_XW/D");
  fNtuple->Branch("NTTargetBeam_XWErr",        &(myEvt.NTTargetBeam_XWErr),          "NTTargetBeam_XWErr/D");
  fNtuple->Branch("NTTargetBeam_Xfit" ,        &(myEvt.NTTargetBeam_Xfit),            "NTTargetBeam_Xfit/D");
  fNtuple->Branch("NTTargetBeam_XfitErr",      &(myEvt.NTTargetBeam_XfitErr),      "NTTargetBeam_XfitErr/D");
  fNtuple->Branch("NTTargetBeam_XWfit",        &(myEvt.NTTargetBeam_XWfit),          "NTTargetBeam_XWfit/D");
  fNtuple->Branch("NTTargetBeam_XWfitErr",     &(myEvt.NTTargetBeam_XWfitErr),    "NTTargetBeam_XWfitErr/D");
  fNtuple->Branch("NTTargetBeam_XChi2"   ,     &(myEvt.NTTargetBeam_XChi2),          "NTTargetBeam_XChi2/D");
  fNtuple->Branch("NTTargetBeam_XNdof"   ,     &(myEvt.NTTargetBeam_XNdof),          "NTTargetBeam_XNdof/D");
  fNtuple->Branch("NTTargetBeam_XCharge" ,     &(myEvt.NTTargetBeam_XCharge),      "NTTargetBeam_XCharge/D");
  fNtuple->Branch("NTTargetBeam_XChargeErr",   &(myEvt.NTTargetBeam_XChargeErr),"NTTargetBeam_XChargeErr/D");
  fNtuple->Branch("NTTargetBeam_XTime"   ,     &(myEvt.NTTargetBeam_XTime),          "NTTargetBeam_XTime/D");
  fNtuple->Branch("NTTargetBeam_XTimeErr",     &(myEvt.NTTargetBeam_XTimeErr),    "NTTargetBeam_XTimeErr/D");
  					 	                          			   
  fNtuple->Branch("NTTargetBeam_Y" ,           &(myEvt.NTTargetBeam_Y),                  "NTTargetBeam_Y/D");
  fNtuple->Branch("NTTargetBeam_YErr" ,        &(myEvt.NTTargetBeam_YErr),            "NTTargetBeam_YErr/D"); 
  fNtuple->Branch("NTTargetBeam_YW" ,          &(myEvt.NTTargetBeam_YW),                "NTTargetBeam_YW/D");
  fNtuple->Branch("NTTargetBeam_YWErr" ,       &(myEvt.NTTargetBeam_YWErr),          "NTTargetBeam_YWErr/D");
  fNtuple->Branch("NTTargetBeam_Yfit" ,        &(myEvt.NTTargetBeam_Yfit),            "NTTargetBeam_Yfit/D");
  fNtuple->Branch("NTTargetBeam_YfitErr" ,     &(myEvt.NTTargetBeam_YfitErr),      "NTTargetBeam_YfitErr/D");
  fNtuple->Branch("NTTargetBeam_YWfit" ,       &(myEvt.NTTargetBeam_YWfit),          "NTTargetBeam_YWfit/D");
  fNtuple->Branch("NTTargetBeam_YWfitErr" ,    &(myEvt.NTTargetBeam_YWfitErr),    "NTTargetBeam_YWfitErr/D");
  fNtuple->Branch("NTTargetBeam_YChi2" ,       &(myEvt.NTTargetBeam_YChi2),          "NTTargetBeam_YChi2/D");
  fNtuple->Branch("NTTargetBeam_YNdof" ,       &(myEvt.NTTargetBeam_YNdof),          "NTTargetBeam_YNdof/D");
  fNtuple->Branch("NTTargetBeam_YCharge" ,     &(myEvt.NTTargetBeam_YCharge),      "NTTargetBeam_YCharge/D");
  fNtuple->Branch("NTTargetBeam_YChargeErr",   &(myEvt.NTTargetBeam_YChargeErr),"NTTargetBeam_YChargeErr/D");
  fNtuple->Branch("NTTargetBeam_YTime" ,       &(myEvt.NTTargetBeam_YTime),          "NTTargetBeam_YTime/D");
  fNtuple->Branch("NTTargetBeam_YTimeErr" ,    &(myEvt.NTTargetBeam_YTimeErr),    "NTTargetBeam_YTimeErr/D"); 					 
  fNtuple->Branch("NTTargetBeam_NPOT" ,        &(myEvt.NTTargetBeam_NPOT),            "NTTargetBeam_NPOT/D");
  fNtuple->Branch("NTTargetBeam_NPOTErr" ,     &(myEvt.NTTargetBeam_NPOTErr),      "NTTargetBeam_NPOTErr/D"); 

  fNtuple->Branch("NTNPVeto_Hits"     ,    &(myEvt.NTNPVeto_Hits)   ,                               "NTNPVeto_Hits/I");
  fNtuple->Branch("NTPVeto_Hits_ChannelId",&(myEvt.NTPVeto_Hits_ChannelId), "NTPVeto_Hits_ChannelId[NTNPVeto_Hits]/I");
  fNtuple->Branch("NTPVeto_Hits_Energy",   &(myEvt.NTPVeto_Hits_Energy),       "NTPVeto_Hits_Energy[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Time",     &(myEvt.NTPVeto_Hits_Time),           "NTPVeto_Hits_Time[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Xpos",     &(myEvt.NTPVeto_Hits_Xpos),           "NTPVeto_Hits_Xpos[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Ypos",     &(myEvt.NTPVeto_Hits_Ypos),           "NTPVeto_Hits_Ypos[NTNPVeto_Hits]/D");
  fNtuple->Branch("NTPVeto_Hits_Zpos",     &(myEvt.NTPVeto_Hits_Zpos),           "NTPVeto_Hits_Zpos[NTNPVeto_Hits]/D");

  fNtuple->Branch("NTNPVeto_Clusters"  ,         &(myEvt.NTNPVeto_Clusters),                                      "NTNPVeto_Clusters/I");
  fNtuple->Branch("NTPVeto_Clusters_ChannelId",  &(myEvt.NTPVeto_Clusters_ChannelId), "NTPVeto_Clusters_ChannelId[NTNPVeto_Clusters]/I");
  fNtuple->Branch("NTPVeto_Clusters_Energy",     &(myEvt.NTPVeto_Clusters_Energy),       "NTPVeto_Clusters_Energy[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Time",       &(myEvt.NTPVeto_Clusters_Time),           "NTPVeto_Clusters_Time[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Xpos",       &(myEvt.NTPVeto_Clusters_Xpos),           "NTPVeto_Clusters_Xpos[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Ypos",       &(myEvt.NTPVeto_Clusters_Ypos),           "NTPVeto_Clusters_Ypos[NTNPVeto_Clusters]/D");
  fNtuple->Branch("NTPVeto_Clusters_Zpos",       &(myEvt.NTPVeto_Clusters_Zpos),           "NTPVeto_Clusters_Zpos[NTNPVeto_Clusters]/D");

  fNtuple->Branch("NTNEVeto_Hits" ,        &(myEvt.NTNEVeto_Hits)   ,                              "NTNEVeto_Hits/I");
  fNtuple->Branch("NTEVeto_Hits_ChannelId",&(myEvt.NTEVeto_Hits_ChannelId),"NTEVeto_Hits_ChannelId[NTNEVeto_Hits]/I");
  fNtuple->Branch("NTEVeto_Hits_Energy",   &(myEvt.NTEVeto_Hits_Energy),      "NTEVeto_Hits_Energy[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Time",     &(myEvt.NTEVeto_Hits_Time),          "NTEVeto_Hits_Time[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Xpos",     &(myEvt.NTEVeto_Hits_Xpos),          "NTEVeto_Hits_Xpos[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Ypos",     &(myEvt.NTEVeto_Hits_Ypos),          "NTEVeto_Hits_Ypos[NTNEVeto_Hits]/D");
  fNtuple->Branch("NTEVeto_Hits_Zpos",     &(myEvt.NTEVeto_Hits_Zpos),          "NTEVeto_Hits_Zpos[NTNEVeto_Hits]/D");

  fNtuple->Branch("NTNEVeto_Clusters"  ,       &(myEvt.NTNEVeto_Clusters),                                        "NTNEVeto_Clusters/I");
  fNtuple->Branch("NTEVeto_Clusters_ChannelId",&(myEvt.NTEVeto_Clusters_ChannelId),   "NTEVeto_Clusters_ChannelId[NTNEVeto_Clusters]/I");
  fNtuple->Branch("NTEVeto_Clusters_Energy",   &(myEvt.NTEVeto_Clusters_Energy),         "NTEVeto_Clusters_Energy[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Time",     &(myEvt.NTEVeto_Clusters_Time),             "NTEVeto_Clusters_Time[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Xpos",     &(myEvt.NTEVeto_Clusters_Xpos),             "NTEVeto_Clusters_Xpos[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Ypos",     &(myEvt.NTEVeto_Clusters_Ypos),             "NTEVeto_Clusters_Ypos[NTNEVeto_Clusters]/D");
  fNtuple->Branch("NTEVeto_Clusters_Zpos",     &(myEvt.NTEVeto_Clusters_Zpos),             "NTEVeto_Clusters_Zpos[NTNEVeto_Clusters]/D");

  fNtuple->Branch("NTNHEPVeto_Hits",         &(myEvt.NTNHEPVeto_Hits),                                       "NTNHEPVeto_Hits/I" );
  fNtuple->Branch("NTHEPVeto_Hits_ChannelId",&(myEvt.NTHEPVeto_Hits_ChannelId),     "NTHEPVeto_Hits_ChannelId[NTNHEPVeto_Hits]/I");
  fNtuple->Branch("NTHEPVeto_Hits_Energy",   &(myEvt.NTHEPVeto_Hits_Energy),           "NTHEPVeto_Hits_Energy[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Time",     &(myEvt.NTHEPVeto_Hits_Time),               "NTHEPVeto_Hits_Time[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Xpos",     &(myEvt.NTHEPVeto_Hits_Xpos),                 "NTHEPVeto_Hits_Xpos[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Ypos",     &(myEvt.NTHEPVeto_Hits_Ypos),                 "NTHEPVeto_Hits_Ypos[NTNHEPVeto_Hits]/D");
  fNtuple->Branch("NTHEPVeto_Hits_Zpos",     &(myEvt.NTHEPVeto_Hits_Zpos),                 "NTHEPVeto_Hits_Zpos[NTNHEPVeto_Hits]/D");

  fNtuple->Branch("NTNHEPVeto_Clusters",         &(myEvt.NTNHEPVeto_Clusters),                                               "NTNHEPVeto_Clusters/I");
  fNtuple->Branch("NTHEPVeto_Clusters_ChannelId",&(myEvt.NTHEPVeto_Clusters_ChannelId),        "NTHEPVeto_Clusters_ChannelId[NTNHEPVeto_Clusters]/I");
  fNtuple->Branch("NTHEPVeto_Clusters_Energy",   &(myEvt.NTHEPVeto_Clusters_Energy),              "NTHEPVeto_Clusters_Energy[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Time",     &(myEvt.NTHEPVeto_Clusters_Time),                  "NTHEPVeto_Clusters_Time[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Xpos",     &(myEvt.NTHEPVeto_Clusters_Xpos),                    "NTHEPVeto_Clusters_Xpos[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Ypos",     &(myEvt.NTHEPVeto_Clusters_Ypos),                    "NTHEPVeto_Clusters_Ypos[NTNHEPVeto_Clusters]/D");
  fNtuple->Branch("NTHEPVeto_Clusters_Zpos",     &(myEvt.NTHEPVeto_Clusters_Zpos),                    "NTHEPVeto_Clusters_Zpos[NTNHEPVeto_Clusters]/D");
 
  fNtuple->Branch("NTNECal_Hits",             &(myEvt.NTNECal_Hits),                                     "NTNECal_Hits/I");
  fNtuple->Branch("NTECal_Hits_ChannelId",    &(myEvt.NTECal_Hits_ChannelId),     "NTECal_Hits_ChannelId[NTNECal_Hits]/I");
  fNtuple->Branch("NTECal_Hits_Energy",       &(myEvt.NTECal_Hits_Energy),           "NTECal_Hits_Energy[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Time",         &(myEvt.NTECal_Hits_Time),               "NTECal_Hits_Time[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Xpos",         &(myEvt.NTECal_Hits_Xpos),               "NTECal_Hits_Xpos[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Ypos",         &(myEvt.NTECal_Hits_Ypos),               "NTECal_Hits_Ypos[NTNECal_Hits]/D");
  fNtuple->Branch("NTECal_Hits_Zpos",         &(myEvt.NTECal_Hits_Zpos),               "NTECal_Hits_Zpos[NTNECal_Hits]/D");

  fNtuple->Branch("NTNECal_Clusters"  ,       &(myEvt.NTNECal_Clusters),                                         "NTNECal_Clusters/I"); 
  fNtuple->Branch("NTECal_Clusters_ChannelId",&(myEvt.NTECal_Clusters_ChannelId),     "NTECal_Clusters_ChannelId[NTNECal_Clusters]/I");
  fNtuple->Branch("NTECal_Clusters_Energy",   &(myEvt.NTECal_Clusters_Energy),           "NTECal_Clusters_Energy[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Time",     &(myEvt.NTECal_Clusters_Time),               "NTECal_Clusters_Time[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Xpos",     &(myEvt.NTECal_Clusters_Xpos),               "NTECal_Clusters_Xpos[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Ypos",     &(myEvt.NTECal_Clusters_Ypos),               "NTECal_Clusters_Ypos[NTNECal_Clusters]/D");
  fNtuple->Branch("NTECal_Clusters_Zpos",     &(myEvt.NTECal_Clusters_Zpos),               "NTECal_Clusters_Zpos[NTNECal_Clusters]/D");


  fNtuple->Branch("NTNSAC_Hits",             &(myEvt.NTNSAC_Hits),                                           "NTNSAC_Hits/I");
  fNtuple->Branch("NTSAC_Hits_ChannelId",    &(myEvt.NTSAC_Hits_ChannelId),             "NTSAC_Hits_ChannelId[NTNSAC_Hits]/I");
  fNtuple->Branch("NTSAC_Hits_Energy",       &(myEvt.NTSAC_Hits_Energy),                   "NTSAC_Hits_Energy[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Time",         &(myEvt.NTSAC_Hits_Time),                       "NTSAC_Hits_Time[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Xpos",         &(myEvt.NTSAC_Hits_Xpos),                       "NTSAC_Hits_Xpos[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Ypos",         &(myEvt.NTSAC_Hits_Ypos),                       "NTSAC_Hits_Ypos[NTNSAC_Hits]/D");
  fNtuple->Branch("NTSAC_Hits_Zpos",         &(myEvt.NTSAC_Hits_Zpos),                       "NTSAC_Hits_Zpos[NTNSAC_Hits]/D");

  fNtuple->Branch("NTNSAC_Clusters",           &(myEvt.NTNSAC_Clusters),                                  "NTNSAC_Clusters/I" );
  fNtuple->Branch("NTSAC_Clusters_ChannelId",  &(myEvt.NTSAC_Clusters_ChannelId),"NTSAC_Clusters_ChannelId[NTNSAC_Clusters]/I");
  fNtuple->Branch("NTSAC_Clusters_Energy",     &(myEvt.NTSAC_Clusters_Energy),      "NTSAC_Clusters_Energy[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Time",       &(myEvt.NTSAC_Clusters_Time),          "NTSAC_Clusters_Time[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Xpos",       &(myEvt.NTSAC_Clusters_Xpos),        "NTSAC_Clusters_Xpos[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Ypos",       &(myEvt.NTSAC_Clusters_Ypos),        "NTSAC_Clusters_Ypos[NTNSAC_Clusters]/D");
  fNtuple->Branch("NTSAC_Clusters_Zpos",       &(myEvt.NTSAC_Clusters_Zpos),        "NTSAC_Clusters_Zpos[NTNSAC_Clusters]/D");
 
  printf("\n---> Flat ntuple created\n");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillNtuple(PadmeAnalysisEvent* evt)
{

  TRecoEvent* rev;
  Int_t nHits;
  TRecoVHit* hit;
  Int_t nClus;
  TRecoVCluster* clu;
  TTargetRecoBeam* beam;

  // Event information
  rev = evt->RecoEvent;
  myEvt.NTEvent_Run_Number = rev->GetRunNumber();
  myEvt.NTEvent_Event_Number = rev->GetEventNumber();
  myEvt.NTEvent_Reco_Time = Double_t(rev->GetRecoTime().GetSec())+Double_t(rev->GetRecoTime().GetNanoSec())/1.E9;
  myEvt.NTEvent_Event_Time = Double_t(rev->GetEventTime().GetSec())+Double_t(rev->GetEventTime().GetNanoSec())/1.E9;
  myEvt.NTEvent_Run_Clock = rev->GetRunClock();
  myEvt.NTEvent_Event_Status = rev->GetEventStatus();
  myEvt.NTEvent_Trigger_Mask = rev->GetTriggerMask();

  // Target hits
  myEvt.NTNTarget_Hits=(evt->TargetRecoEvent)->GetNHits();
  nHits = min(myEvt.NTNTarget_Hits,NTargetMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->TargetRecoEvent)->Hit(i);
    myEvt.NTTarget_Hits_ChannelId[i]=hit->GetChannelId();
    myEvt.NTTarget_Hits_Energy[i]=hit->GetEnergy();
    myEvt.NTTarget_Hits_Time[i]=hit->GetTime();
    myEvt.NTTarget_Hits_Xpos[i]=hit->GetPosition().X();
    myEvt.NTTarget_Hits_Ypos[i]=hit->GetPosition().Y();
    myEvt.NTTarget_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // Target information
  beam = evt->TargetRecoBeam;
  myEvt.NTTargetBeam_X=beam->getX();
  myEvt.NTTargetBeam_Y=beam->getY();
  myEvt.NTTargetBeam_XErr=beam->getXError();
  myEvt.NTTargetBeam_YErr=beam->getYError();
  myEvt.NTTargetBeam_XW=beam->getXWidth();
  myEvt.NTTargetBeam_YW=beam->getYWidth();
  myEvt.NTTargetBeam_XWErr=beam->getXWidthError();
  myEvt.NTTargetBeam_YWErr=beam->getYWidthError();
  myEvt.NTTargetBeam_XChi2=beam->getXChi2();
  myEvt.NTTargetBeam_YChi2=beam->getYChi2();
  myEvt.NTTargetBeam_XNdof=beam->getXNdof();
  myEvt.NTTargetBeam_YNdof=beam->getYNdof();
  myEvt.NTTargetBeam_XCharge=beam->getXCharge();
  myEvt.NTTargetBeam_XChargeErr=beam->getXChargeErr();
  myEvt.NTTargetBeam_YCharge=beam->getYCharge();
  myEvt.NTTargetBeam_YChargeErr=beam->getYChargeErr();
  myEvt.NTTargetBeam_XTime=beam->getXTime();
  myEvt.NTTargetBeam_XTimeErr=beam->getXTimeErr();
  myEvt.NTTargetBeam_YTime=beam->getYTime();
  myEvt.NTTargetBeam_YTimeErr=beam->getYTimeErr();
  myEvt.NTTargetBeam_NPOT=beam->getnPOT();
  myEvt.NTTargetBeam_NPOTErr=beam->getnPOTError();

  // PVeto hits
  myEvt.NTNPVeto_Hits = (evt->PVetoRecoEvent)->GetNHits();
  nHits = min(myEvt.NTNPVeto_Hits,NPVetoMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->PVetoRecoEvent)->Hit(i);
    myEvt.NTPVeto_Hits_ChannelId[i]=hit->GetChannelId();
    myEvt.NTPVeto_Hits_Energy[i]=hit->GetEnergy();
    myEvt.NTPVeto_Hits_Time[i]=hit->GetTime();
    myEvt.NTPVeto_Hits_Xpos[i]=hit->GetPosition().X();
    myEvt.NTPVeto_Hits_Ypos[i]=hit->GetPosition().Y();
    myEvt.NTPVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // PVeto clusters
  myEvt.NTNPVeto_Clusters = (evt->PVetoRecoCl)->GetNElements();
  nClus = min(myEvt.NTNPVeto_Clusters,NPVetoMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->PVetoRecoCl)->Element(j);
    myEvt.NTPVeto_Clusters_ChannelId[j]=clu->GetChannelId();
    myEvt.NTPVeto_Clusters_Energy[j]=clu->GetEnergy();
    myEvt.NTPVeto_Clusters_Time[j]=clu->GetTime();
    myEvt.NTPVeto_Clusters_Xpos[j]=clu->GetPosition().X();
    myEvt.NTPVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
    myEvt.NTPVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // EVeto hits
  myEvt.NTNEVeto_Hits = (evt->EVetoRecoEvent)->GetNHits();
  nHits = min(myEvt.NTNEVeto_Hits,NEVetoMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->EVetoRecoEvent)->Hit(i);
    myEvt.NTEVeto_Hits_ChannelId[i]=hit->GetChannelId();
    myEvt.NTEVeto_Hits_Energy[i]=hit->GetEnergy();
    myEvt.NTEVeto_Hits_Time[i]=hit->GetTime();
    myEvt.NTEVeto_Hits_Xpos[i]=hit->GetPosition().X();
    myEvt.NTEVeto_Hits_Ypos[i]=hit->GetPosition().Y();
    myEvt.NTEVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // EVeto clusters
  myEvt.NTNEVeto_Clusters = (evt->EVetoRecoCl)->GetNElements();
  nClus = min(myEvt.NTNEVeto_Clusters,NEVetoMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->EVetoRecoCl)->Element(j);
    myEvt.NTEVeto_Clusters_ChannelId[j]=clu->GetChannelId();
    myEvt.NTEVeto_Clusters_Energy[j]=clu->GetEnergy();
    myEvt.NTEVeto_Clusters_Time[j]=clu->GetTime();
    myEvt.NTEVeto_Clusters_Xpos[j]=clu->GetPosition().X();
    myEvt.NTEVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
    myEvt.NTEVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // HEPVeto hits
  myEvt.NTNHEPVeto_Hits = (evt->HEPVetoRecoEvent)->GetNHits();
  nHits = min(myEvt.NTNHEPVeto_Hits,NHEPVetoMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->HEPVetoRecoEvent)->Hit(i);
    myEvt.NTHEPVeto_Hits_ChannelId[i]=hit->GetChannelId();
    myEvt.NTHEPVeto_Hits_Energy[i]=hit->GetEnergy();
    myEvt.NTHEPVeto_Hits_Time[i]=hit->GetTime();
    myEvt.NTHEPVeto_Hits_Xpos[i]=hit->GetPosition().X();
    myEvt.NTHEPVeto_Hits_Ypos[i]=hit->GetPosition().Y();
    myEvt.NTHEPVeto_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // HEPVeto clusters
  myEvt.NTNHEPVeto_Clusters = (evt->HEPVetoRecoCl)->GetNElements();
  nClus = min(myEvt.NTNHEPVeto_Clusters,NHEPVetoMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->HEPVetoRecoCl)->Element(j);
    myEvt.NTHEPVeto_Clusters_ChannelId[j]=clu->GetChannelId();
    myEvt.NTHEPVeto_Clusters_Energy[j]=clu->GetEnergy();
    myEvt.NTHEPVeto_Clusters_Time[j]=clu->GetTime();
    myEvt.NTHEPVeto_Clusters_Xpos[j]=clu->GetPosition().X();
    myEvt.NTHEPVeto_Clusters_Ypos[j]=clu->GetPosition().Y();
    myEvt.NTHEPVeto_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // ECal hits
  myEvt.NTNECal_Hits = (evt->ECalRecoEvent)->GetNHits();
  nHits = min(myEvt.NTNECal_Hits,NECalMaxHits);
  for (Int_t i=0; i<nHits; ++i) {
    hit = (evt->ECalRecoEvent)->Hit(i);
    myEvt.NTECal_Hits_ChannelId[i]=hit->GetChannelId();
    myEvt.NTECal_Hits_Energy[i]=hit->GetEnergy();
    myEvt.NTECal_Hits_Time[i]=hit->GetTime();
    myEvt.NTECal_Hits_Xpos[i]=hit->GetPosition().X();
    myEvt.NTECal_Hits_Ypos[i]=hit->GetPosition().Y();
    myEvt.NTECal_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // ECal clusters
  myEvt.NTNECal_Clusters = (evt->ECalRecoCl)->GetNElements();
  nClus = min(myEvt.NTNECal_Clusters,NECalMaxClusters);
  for (Int_t j=0; j<nClus; ++j) {
    clu = (evt->ECalRecoCl)->Element(j);
    myEvt.NTECal_Clusters_ChannelId[j]=clu->GetChannelId();
    myEvt.NTECal_Clusters_Energy[j]=clu->GetEnergy();
    myEvt.NTECal_Clusters_Time[j]=clu->GetTime();
    myEvt.NTECal_Clusters_Xpos[j]=clu->GetPosition().X();
    myEvt.NTECal_Clusters_Ypos[j]=clu->GetPosition().Y();
    myEvt.NTECal_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // SAC hits
  myEvt.NTNSAC_Hits = (evt->SACRecoEvent)->GetNHits();
  nHits = min(myEvt.NTNSAC_Hits,NSACMaxHits);
  for (Int_t i=0; i<nHits; ++i){
    hit = (evt->SACRecoEvent)->Hit(i);
    myEvt.NTSAC_Hits_ChannelId[i]=hit->GetChannelId();
    myEvt.NTSAC_Hits_Energy[i]=hit->GetEnergy();
    myEvt.NTSAC_Hits_Time[i]=hit->GetTime();
    myEvt.NTSAC_Hits_Xpos[i]=hit->GetPosition().X();
    myEvt.NTSAC_Hits_Ypos[i]=hit->GetPosition().Y();
    myEvt.NTSAC_Hits_Zpos[i]=hit->GetPosition().Z();
  }

  // SAC clusters
  myEvt.NTNSAC_Clusters = (evt->SACRecoCl)->GetNElements();
  nClus = min(myEvt.NTNSAC_Clusters,NSACMaxClusters);
  for (Int_t j=0; j<nClus; ++j){
    clu = (evt->SACRecoCl)->Element(j);
    myEvt.NTSAC_Clusters_ChannelId[j]=clu->GetChannelId();
    myEvt.NTSAC_Clusters_Energy[j]=clu->GetEnergy();
    myEvt.NTSAC_Clusters_Time[j]=clu->GetTime();
    myEvt.NTSAC_Clusters_Xpos[j]=clu->GetPosition().X();
    myEvt.NTSAC_Clusters_Ypos[j]=clu->GetPosition().Y();
    myEvt.NTSAC_Clusters_Zpos[j]=clu->GetPosition().Z();
  }

  // Copy event ntuple to output file
  fNtuple->Fill();

}
