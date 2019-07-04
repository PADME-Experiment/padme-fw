#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>
//#include <CLHEP/Units/SystemOfUnits.h>

#include "HistoSvc.hh"
//#include "DatacardManager.hh"
//#include "MyEvent.hh"
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoSvc* HistoSvc::fInstance = 0;
HistoSvc* HistoSvc::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HistoSvc(); }
  return fInstance;
}

HistoSvc::HistoSvc()
 :fRootOutputFile(0),ntupl(0)
{
  // histograms
  fHisto1DMap.clear();
  fHisto2DMap.clear();
  // ntuple
  ntupl = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoSvc::~HistoSvc()
{
  if ( fRootOutputFile ) delete fRootOutputFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void HistoSvc::BookHisto (std::string name, Int_t nx, Double_t xlow, Double_t xup)
{
  
  TH1D* hT1y = new TH1D(name.c_str(), name.c_str(), nx, xlow, xup);
  fHisto1DMap[name]=hT1y;
  return;
}
void HistoSvc::BookHisto2(std::string name, Int_t nx, Double_t xlow, Double_t xup, Int_t ny, Double_t ylow, Double_t yup)
{
  TH2D* hT1y = new TH2D(name.c_str(), name.c_str(), nx, xlow, xup, ny, ylow, yup);
  fHisto2DMap[name]=hT1y;
  return;
}



//void HistoSvc::book(Int_t validation)
void HistoSvc::book(Int_t validation, Int_t ntuple) //flatNTP
{ 
 
  // Creating a tree container to handle histograms and ntuples.
  // This tree is associated to an output file.
  //
  //std::string fileName = fOutputFileName;
  //fRootOutputFile = new TFile(fileName.c_str(),"RECREATE");
  fRootOutputFile = new TFile(fOutputFileName,"RECREATE");
  if(!fRootOutputFile) {
    std::cout << " HistoSvc::book :"<<" problem creating the ROOT TFile "<< std::endl;
    return;
  }
  std::cout << " HistoSvc::book :"<<" output ROOT TFile "<<fOutputFileName<<" created"<< std::endl;

  if (validation) return;



  if (ntuple)
 { 
  // create PADME flay ntuple
  
  ntupl = new TTree("PADME_FlatNT", "PADME_Event");

  ntupl->Branch("NTEventStatus"     ,    &(myEvt.NTEventStatus)   ,                                  "NTEventStatus/I");
  ntupl->Branch("NTNTarget_Hits"     ,    &(myEvt.NTNTarget_Hits)   ,                               "NTNTarget_Hits/I");
  ntupl->Branch("NTTarget_Hits_Energy",   &(myEvt.NTTarget_Hits_Energy),      "NTTarget_Hits_Energy[NTNTarget_Hits]/D");
  ntupl->Branch("NTTarget_Hits_Time",     &(myEvt.NTTarget_Hits_Time),          "NTTarget_Hits_Time[NTNTarget_Hits]/D");
  ntupl->Branch("NTTarget_Hits_ChannelId",&(myEvt.NTTarget_Hits_ChannelId),"NTTarget_Hits_ChannelId[NTNTarget_Hits]/D");
  ntupl->Branch("NTTarget_Hits_Xpos",     &(myEvt.NTTarget_Hits_Xpos),          "NTTarget_Hits_Xpos[NTNTarget_Hits]/D");
  ntupl->Branch("NTTarget_Hits_Ypos",     &(myEvt.NTTarget_Hits_Ypos),          "NTTarget_Hits_Ypos[NTNTarget_Hits]/D");
  ntupl->Branch("NTTarget_Hits_Zpos",     &(myEvt.NTTarget_Hits_Zpos),          "NTTarget_Hits_Zpos[NTNTarget_Hits]/D");

  ntupl->Branch("NTTargetBeam_X"    ,        &(myEvt.NTTargetBeam_X),                  "NTTargetBeam_X/D");
  ntupl->Branch("NTTargetBeam_XErr" ,        &(myEvt.NTTargetBeam_XErr),            "NTTargetBeam_XErr/D"); 
  ntupl->Branch("NTTargetBeam_XW"   ,        &(myEvt.NTTargetBeam_XW),                "NTTargetBeam_XW/D");
  ntupl->Branch("NTTargetBeam_XWErr",        &(myEvt.NTTargetBeam_XWErr),          "NTTargetBeam_XWErr/D");
  ntupl->Branch("NTTargetBeam_Xfit" ,        &(myEvt.NTTargetBeam_Xfit),            "NTTargetBeam_Xfit/D");
  ntupl->Branch("NTTargetBeam_XfitErr",      &(myEvt.NTTargetBeam_XfitErr),      "NTTargetBeam_XfitErr/D");
  ntupl->Branch("NTTargetBeam_XWfit",        &(myEvt.NTTargetBeam_XWfit),          "NTTargetBeam_XWfit/D");
  ntupl->Branch("NTTargetBeam_XWfitErr",     &(myEvt.NTTargetBeam_XWfitErr),    "NTTargetBeam_XWfitErr/D");
  ntupl->Branch("NTTargetBeam_XChi2"   ,     &(myEvt.NTTargetBeam_XChi2),          "NTTargetBeam_XChi2/D");
  ntupl->Branch("NTTargetBeam_XNdof"   ,     &(myEvt.NTTargetBeam_XNdof),          "NTTargetBeam_XNdof/D");
  ntupl->Branch("NTTargetBeam_XCharge" ,     &(myEvt.NTTargetBeam_XCharge),      "NTTargetBeam_XCharge/D");
  ntupl->Branch("NTTargetBeam_XChargeErr",   &(myEvt.NTTargetBeam_XChargeErr),"NTTargetBeam_XChargeErr/D");
  ntupl->Branch("NTTargetBeam_XTime"   ,     &(myEvt.NTTargetBeam_XTime),          "NTTargetBeam_XTime/D");
  ntupl->Branch("NTTargetBeam_XTimeErr",     &(myEvt.NTTargetBeam_XTimeErr),    "NTTargetBeam_XTimeErr/D");
  					 	                          			   
  ntupl->Branch("NTTargetBeam_Y" ,           &(myEvt.NTTargetBeam_Y),                  "NTTargetBeam_Y/D");
  ntupl->Branch("NTTargetBeam_YErr" ,        &(myEvt.NTTargetBeam_YErr),            "NTTargetBeam_YErr/D"); 
  ntupl->Branch("NTTargetBeam_YW" ,          &(myEvt.NTTargetBeam_YW),                "NTTargetBeam_YW/D");
  ntupl->Branch("NTTargetBeam_YWErr" ,       &(myEvt.NTTargetBeam_YWErr),          "NTTargetBeam_YWErr/D");
  ntupl->Branch("NTTargetBeam_Yfit" ,        &(myEvt.NTTargetBeam_Yfit),            "NTTargetBeam_Yfit/D");
  ntupl->Branch("NTTargetBeam_YfitErr" ,     &(myEvt.NTTargetBeam_YfitErr),      "NTTargetBeam_YfitErr/D");
  ntupl->Branch("NTTargetBeam_YWfit" ,       &(myEvt.NTTargetBeam_YWfit),          "NTTargetBeam_YWfit/D");
  ntupl->Branch("NTTargetBeam_YWfitErr" ,    &(myEvt.NTTargetBeam_YWfitErr),    "NTTargetBeam_YWfitErr/D");
  ntupl->Branch("NTTargetBeam_YChi2" ,       &(myEvt.NTTargetBeam_YChi2),          "NTTargetBeam_YChi2/D");
  ntupl->Branch("NTTargetBeam_YNdof" ,       &(myEvt.NTTargetBeam_YNdof),          "NTTargetBeam_YNdof/D");
  ntupl->Branch("NTTargetBeam_YCharge" ,     &(myEvt.NTTargetBeam_YCharge),      "NTTargetBeam_YCharge/D");
  ntupl->Branch("NTTargetBeam_YChargeErr",   &(myEvt.NTTargetBeam_YChargeErr),"NTTargetBeam_YChargeErr/D");
  ntupl->Branch("NTTargetBeam_YTime" ,       &(myEvt.NTTargetBeam_YTime),          "NTTargetBeam_YTime/D");
  ntupl->Branch("NTTargetBeam_YTimeErr" ,    &(myEvt.NTTargetBeam_YTimeErr),    "NTTargetBeam_YTimeErr/D"); 					 
  ntupl->Branch("NTTargetBeam_NPOT" ,        &(myEvt.NTTargetBeam_NPOT),            "NTTargetBeam_NPOT/D");
  ntupl->Branch("NTTargetBeam_NPOTErr" ,     &(myEvt.NTTargetBeam_NPOTErr),      "NTTargetBeam_NPOTErr/D"); 

  ntupl->Branch("NTNPVeto_Hits"     ,    &(myEvt.NTNPVeto_Hits)   ,                               "NTNPVeto_Hits/I");
  ntupl->Branch("NTPVeto_Hits_Energy",   &(myEvt.NTPVeto_Hits_Energy),       "NTPVeto_Hits_Energy[NTNPVeto_Hits]/D");
  ntupl->Branch("NTPVeto_Hits_Time",     &(myEvt.NTPVeto_Hits_Time),           "NTPVeto_Hits_Time[NTNPVeto_Hits]/D");
  ntupl->Branch("NTPVeto_Hits_ChannelId",&(myEvt.NTPVeto_Hits_ChannelId), "NTPVeto_Hits_ChannelId[NTNPVeto_Hits]/D");
  ntupl->Branch("NTPVeto_Hits_Xpos",     &(myEvt.NTPVeto_Hits_Xpos),           "NTPVeto_Hits_Xpos[NTNPVeto_Hits]/D");
  ntupl->Branch("NTPVeto_Hits_Ypos",     &(myEvt.NTPVeto_Hits_Ypos),           "NTPVeto_Hits_Ypos[NTNPVeto_Hits]/D");
  ntupl->Branch("NTPVeto_Hits_Zpos",     &(myEvt.NTPVeto_Hits_Zpos),           "NTPVeto_Hits_Zpos[NTNPVeto_Hits]/D");

  ntupl->Branch("NTNPVeto_Clusters"  ,         &(myEvt.NTNPVeto_Clusters),                                      "NTNPVeto_Clusters/I");
  ntupl->Branch("NTPVeto_Clusters_Energy",     &(myEvt.NTPVeto_Clusters_Energy),       "NTPVeto_Clusters_Energy[NTNPVeto_Clusters]/D");
  ntupl->Branch("NTPVeto_Clusters_Time",       &(myEvt.NTPVeto_Clusters_Time),           "NTPVeto_Clusters_Time[NTNPVeto_Clusters]/D");
  ntupl->Branch("NTPVeto_Clusters_ChannelId",  &(myEvt.NTPVeto_Clusters_ChannelId), "NTPVeto_Clusters_ChannelId[NTNPVeto_Clusters]/D");
  ntupl->Branch("NTPVeto_Clusters_Xpos",       &(myEvt.NTPVeto_Clusters_Xpos),           "NTPVeto_Clusters_Xpos[NTNPVeto_Clusters]/D");
  ntupl->Branch("NTPVeto_Clusters_Ypos",       &(myEvt.NTPVeto_Clusters_Ypos),           "NTPVeto_Clusters_Ypos[NTNPVeto_Clusters]/D");
  ntupl->Branch("NTPVeto_Clusters_Zpos",       &(myEvt.NTPVeto_Clusters_Zpos),           "NTPVeto_Clusters_Zpos[NTNPVeto_Clusters]/D");

  ntupl->Branch("NTNEVeto_Hits" ,        &(myEvt.NTNEVeto_Hits)   ,                              "NTNEVeto_Hits/I");
  ntupl->Branch("NTEVeto_Hits_Energy",   &(myEvt.NTEVeto_Hits_Energy),      "NTEVeto_Hits_Energy[NTNEVeto_Hits]/D");
  ntupl->Branch("NTEVeto_Hits_Time",     &(myEvt.NTEVeto_Hits_Time),          "NTEVeto_Hits_Time[NTNEVeto_Hits]/D");
  ntupl->Branch("NTEVeto_Hits_ChannelId",&(myEvt.NTEVeto_Hits_ChannelId),"NTEVeto_Hits_ChannelId[NTNEVeto_Hits]/D");
  ntupl->Branch("NTEVeto_Hits_Xpos",     &(myEvt.NTEVeto_Hits_Xpos),          "NTEVeto_Hits_Xpos[NTNEVeto_Hits]/D");
  ntupl->Branch("NTEVeto_Hits_Ypos",     &(myEvt.NTEVeto_Hits_Ypos),          "NTEVeto_Hits_Ypos[NTNEVeto_Hits]/D");
  ntupl->Branch("NTEVeto_Hits_Zpos",     &(myEvt.NTEVeto_Hits_Zpos),          "NTEVeto_Hits_Zpos[NTNEVeto_Hits]/D");

  ntupl->Branch("NTNEVeto_Clusters"  ,       &(myEvt.NTNEVeto_Clusters),                                        "NTNEVeto_Clusters/I");
  ntupl->Branch("NTEVeto_Clusters_Energy",   &(myEvt.NTEVeto_Clusters_Energy),         "NTEVeto_Clusters_Energy[NTNEVeto_Clusters]/D");
  ntupl->Branch("NTEVeto_Clusters_Time",     &(myEvt.NTEVeto_Clusters_Time),             "NTEVeto_Clusters_Time[NTNEVeto_Clusters]/D");
  ntupl->Branch("NTEVeto_Clusters_ChannelId",&(myEvt.NTEVeto_Clusters_ChannelId),   "NTEVeto_Clusters_ChannelId[NTNEVeto_Clusters]/D");
  ntupl->Branch("NTEVeto_Clusters_Xpos",     &(myEvt.NTEVeto_Clusters_Xpos),             "NTEVeto_Clusters_Xpos[NTNEVeto_Clusters]/D");
  ntupl->Branch("NTEVeto_Clusters_Ypos",     &(myEvt.NTEVeto_Clusters_Ypos),             "NTEVeto_Clusters_Ypos[NTNEVeto_Clusters]/D");
  ntupl->Branch("NTEVeto_Clusters_Zpos",     &(myEvt.NTEVeto_Clusters_Zpos),             "NTEVeto_Clusters_Zpos[NTNEVeto_Clusters]/D");

  ntupl->Branch("NTNHEPVeto_Hits",         &(myEvt.NTNHEPVeto_Hits),                                       "NTNHEPVeto_Hits/I" );
  ntupl->Branch("NTHEPVeto_Hits_Energy",   &(myEvt.NTHEPVeto_Hits_Energy),           "NTHEPVeto_Hits_Energy[NTNHEPVeto_Hits]/D");
  ntupl->Branch("NTHEPVeto_Hits_Time",     &(myEvt.NTHEPVeto_Hits_Time),               "NTHEPVeto_Hits_Time[NTNHEPVeto_Hits]/D");
  ntupl->Branch("NTHEPVeto_Hits_ChannelId",&(myEvt.NTHEPVeto_Hits_ChannelId),     "NTHEPVeto_Hits_ChannelId[NTNHEPVeto_Hits]/D");
  ntupl->Branch("NTHEPVeto_Hits_Xpos",     &(myEvt.NTHEPVeto_Hits_Xpos),                 "NTHEPVeto_Hits_Xpos[NTNHEPVeto_Hits]/D");
  ntupl->Branch("NTHEPVeto_Hits_Ypos",     &(myEvt.NTHEPVeto_Hits_Ypos),                 "NTHEPVeto_Hits_Ypos[NTNHEPVeto_Hits]/D");
  ntupl->Branch("NTHEPVeto_Hits_Zpos",     &(myEvt.NTHEPVeto_Hits_Zpos),                 "NTHEPVeto_Hits_Zpos[NTNHEPVeto_Hits]/D");

  ntupl->Branch("NTNHEPVeto_Clusters",         &(myEvt.NTNHEPVeto_Clusters),                                               "NTNHEPVeto_Clusters/I");
  ntupl->Branch("NTHEPVeto_Clusters_Energy",   &(myEvt.NTHEPVeto_Clusters_Energy),              "NTHEPVeto_Clusters_Energy[NTNHEPVeto_Clusters]/D");
  ntupl->Branch("NTHEPVeto_Clusters_Time",     &(myEvt.NTHEPVeto_Clusters_Time),                  "NTHEPVeto_Clusters_Time[NTNHEPVeto_Clusters]/D");
  ntupl->Branch("NTHEPVeto_Clusters_ChannelId",&(myEvt.NTHEPVeto_Clusters_ChannelId),        "NTHEPVeto_Clusters_ChannelId[NTNHEPVeto_Clusters]/D");
  ntupl->Branch("NTHEPVeto_Clusters_Xpos",     &(myEvt.NTHEPVeto_Clusters_Xpos),                    "NTHEPVeto_Clusters_Xpos[NTNHEPVeto_Clusters]/D");
  ntupl->Branch("NTHEPVeto_Clusters_Ypos",     &(myEvt.NTHEPVeto_Clusters_Ypos),                    "NTHEPVeto_Clusters_Ypos[NTNHEPVeto_Clusters]/D");
  ntupl->Branch("NTHEPVeto_Clusters_Zpos",     &(myEvt.NTHEPVeto_Clusters_Zpos),                    "NTHEPVeto_Clusters_Zpos[NTNHEPVeto_Clusters]/D");
 
  ntupl->Branch("NTNECal_Hits",             &(myEvt.NTNECal_Hits),                                     "NTNECal_Hits/I");
  ntupl->Branch("NTECal_Hits_Energy",       &(myEvt.NTECal_Hits_Energy),           "NTECal_Hits_Energy[NTNECal_Hits]/D");
  ntupl->Branch("NTECal_Hits_Time",         &(myEvt.NTECal_Hits_Time),               "NTECal_Hits_Time[NTNECal_Hits]/D");
  ntupl->Branch("NTECal_Hits_ChannelId",    &(myEvt.NTECal_Hits_ChannelId),     "NTECal_Hits_ChannelId[NTNECal_Hits]/D");
  ntupl->Branch("NTECal_Hits_Xpos",         &(myEvt.NTECal_Hits_Xpos),               "NTECal_Hits_Xpos[NTNECal_Hits]/D");
  ntupl->Branch("NTECal_Hits_Ypos",         &(myEvt.NTECal_Hits_Ypos),               "NTECal_Hits_Ypos[NTNECal_Hits]/D");
  ntupl->Branch("NTECal_Hits_Zpos",         &(myEvt.NTECal_Hits_Zpos),               "NTECal_Hits_Zpos[NTNECal_Hits]/D");

  ntupl->Branch("NTNECal_Clusters"  ,       &(myEvt.NTNECal_Clusters),                                         "NTNECal_Clusters/I"); 
  ntupl->Branch("NTECal_Clusters_Energy",   &(myEvt.NTECal_Clusters_Energy),           "NTECal_Clusters_Energy[NTNECal_Clusters]/D");
  ntupl->Branch("NTECal_Clusters_Time",     &(myEvt.NTECal_Clusters_Time),               "NTECal_Clusters_Time[NTNECal_Clusters]/D");
  ntupl->Branch("NTECal_Clusters_ChannelId",&(myEvt.NTECal_Clusters_ChannelId),     "NTECal_Clusters_ChannelId[NTNECal_Clusters]/D");
  ntupl->Branch("NTECal_Clusters_Xpos",     &(myEvt.NTECal_Clusters_Xpos),               "NTECal_Clusters_Xpos[NTNECal_Clusters]/D");
  ntupl->Branch("NTECal_Clusters_Ypos",     &(myEvt.NTECal_Clusters_Ypos),               "NTECal_Clusters_Ypos[NTNECal_Clusters]/D");
  ntupl->Branch("NTECal_Clusters_Zpos",     &(myEvt.NTECal_Clusters_Zpos),               "NTECal_Clusters_Zpos[NTNECal_Clusters]/D");


  ntupl->Branch("NTNSAC_Hits",             &(myEvt.NTNSAC_Hits),                                           "NTNSAC_Hits/I");
  ntupl->Branch("NTSAC_Hits_Energy",       &(myEvt.NTSAC_Hits_Energy),                   "NTSAC_Hits_Energy[NTNSAC_Hits]/D");
  ntupl->Branch("NTSAC_Hits_Time",         &(myEvt.NTSAC_Hits_Time),                       "NTSAC_Hits_Time[NTNSAC_Hits]/D");
  ntupl->Branch("NTSAC_Hits_ChannelId",    &(myEvt.NTSAC_Hits_ChannelId),             "NTSAC_Hits_ChannelId[NTNSAC_Hits]/D");
  ntupl->Branch("NTSAC_Hits_Xpos",         &(myEvt.NTSAC_Hits_Xpos),                       "NTSAC_Hits_Xpos[NTNSAC_Hits]/D");
  ntupl->Branch("NTSAC_Hits_Ypos",         &(myEvt.NTSAC_Hits_Ypos),                       "NTSAC_Hits_Ypos[NTNSAC_Hits]/D");
  ntupl->Branch("NTSAC_Hits_Zpos",         &(myEvt.NTSAC_Hits_Zpos),                       "NTSAC_Hits_Zpos[NTNSAC_Hits]/D");

  ntupl->Branch("NTNSAC_Clusters",           &(myEvt.NTNSAC_Clusters),                                  "NTNSAC_Clusters/I" );
  ntupl->Branch("NTSAC_Clusters_Energy",     &(myEvt.NTSAC_Clusters_Energy),      "NTSAC_Clusters_Energy[NTNSAC_Clusters]/D");
  ntupl->Branch("NTSAC_Clusters_Time",       &(myEvt.NTSAC_Clusters_Time),          "NTSAC_Clusters_Time[NTNSAC_Clusters]/D");
  ntupl->Branch("NTSAC_Clusters_ChannelId",  &(myEvt.NTSAC_Clusters_ChannelId),"NTSAC_Clusters_ChannelId[NTNSAC_Clusters]/D");
  ntupl->Branch("NTSAC_Clusters_Xpos",       &(myEvt.NTSAC_Clusters_Xpos),        "NTSAC_Clusters_Xpos[NTNSAC_Clusters]/D");
  ntupl->Branch("NTSAC_Clusters_Ypos",       &(myEvt.NTSAC_Clusters_Ypos),        "NTSAC_Clusters_Ypos[NTNSAC_Clusters]/D");
  ntupl->Branch("NTSAC_Clusters_Zpos",       &(myEvt.NTSAC_Clusters_Zpos),        "NTSAC_Clusters_Zpos[NTNSAC_Clusters]/D");
 
  }									  

  std::cout << "\n----> Histogram file is opened in " << fOutputFileName << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::save()
{ 
  if (fRootOutputFile) {
    fRootOutputFile->Write();        // Writing the histograms to the file
    fRootOutputFile->Close();        // and closing the tree (and the file)
    std::cout << "\n----> Histogram Tree is saved \n" << std::endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto(std::string hname, Double_t xbin, Double_t weight)
{
  if (fHisto1DMap.find(hname)==fHisto1DMap.end()) {
    std::cout << "---> warning from HistoSvc::FillHisto() : histo " << hname
           << " does not exist. (xbin=" << xbin << " weight=" << weight << ")"
           << std::endl;
    return;
  }
 if  (fHisto1DMap[hname]) {fHisto1DMap[hname]->Fill(xbin, weight); }
 else
   {
      std::cout << "---> warning from HistoSvc::FillHisto() : histo " << hname
           << " found in the map with a NULL pointer "
           << std::endl;
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto2(std::string hname, Double_t xbin, Double_t ybin, Double_t weight)
{
   if (fHisto2DMap.find(hname)==fHisto2DMap.end()) {
    std::cout << "---> warning from HistoSvc::FillHisto() : histo2D " << hname
           << " does not exist. (xbin=" << xbin << " ybin=" << ybin <<" weight=" << weight << ")"
           << std::endl;
    return;
  }
   if  (fHisto2DMap[hname]) {fHisto2DMap[hname]->Fill(xbin, ybin, weight); }
 else
   {
      std::cout << "---> warning from HistoSvc::FillHisto() : histo " << hname
           << " found in the map with a NULL pointer "
           << std::endl;
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::Normalize(std::string hname, Double_t fac)
{
   if (fHisto1DMap.find(hname)!=fHisto1DMap.end()) {
     if  (fHisto1DMap[hname]) {
       fHisto1DMap[hname]->Scale(fac);
     }
     else{
       std::cout << "---> warning from HistoSvc::FillHisto() : histo " << hname
		 << " found in the map with a NULL pointer "
		 << std::endl;
     }
     //    std::cout << "---> warning from HistoSvc::FillHisto() : histo2D " << hname
     //           << ", to be normalized, does not exist. " << xbin << " ybin=" << ybin <<" weight=" << weight << ")"
     //           << std::endl;
    return;
   }
   else{
     if (fHisto2DMap.find(hname)!=fHisto2DMap.end()) {
       if  (fHisto2DMap[hname]) {
	 fHisto2DMap[hname]->Scale(fac);
       }
       else{
	 std::cout << "---> warning from HistoSvc::NormalizeHisto() : histo2D " << hname
		   << " found in the map with a NULL pointer "
		   << std::endl;
       }
       return;
     }
     else
       {
	 std::cout << "---> warning from HistoSvc::NormalizeHisto() : histo " << hname
		   << " NOT found in the map of 1D histos nor in the map of 2D histos"
		   << std::endl;
	 return;
       }
   }
}
   

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillNtuple()
{
  if(ntupl) ntupl->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


