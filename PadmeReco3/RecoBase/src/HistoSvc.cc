#include <iostream>

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TGraphBentErrors.h>
#include <TGraphPolar.h>

#include "HistoSvc.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoSvc* HistoSvc::fInstance = 0;
HistoSvc* HistoSvc::GetInstance()
{
  if ( fInstance == 0 ) { fInstance = new HistoSvc(); }
  return fInstance;
}

HistoSvc::HistoSvc()
  :fVerbose(0),fOutputFileName(""),fRootOutputFile(0)
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
    if (fVerbose) printf("HistoSvc::Initialize - Output ROOT file '%s' successfully opened\n",outputFileName.Data());
  } else {
    printf("HistoSvc::Initialize - ERROR - problem creating the ROOT TFile '%s'\n",fOutputFileName.Data());
    return false;
  }

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t HistoSvc::Finalize()
{

  if (fVerbose) {
    std::map<std::string, std::map<std::string,TObject*>>::iterator it;
    for (it = fListMap.begin(); it != fListMap.end(); it++) {
      printf("HistoSvc::Finalize - List '%s' has %lu histograms\n",it->first.c_str(),it->second.size());
      if (fVerbose>1) {
	std::map<std::string,TObject*>::iterator jt;
	for (jt = it->second.begin(); jt != it->second.end(); jt++) {
	  printf("     %-6s '%s'\n",jt->second->ClassName(),jt->first.c_str());
	}
      }
    }
  }

  if (fRootOutputFile) {
    fRootOutputFile->Write();
    fRootOutputFile->Close();
    if (fVerbose) printf("HistoSvc::Finalize - Histogram file closed with size %lld\n",fRootOutputFile->GetSize());
  }

  return true;

}

Bool_t HistoSvc::CreateDir(std::string listname)
{

  if (fListMap.find(listname) != fListMap.end()) {
    printf("HistoSvc::CreateDir - ERROR - Directory %s already defined\n",listname.c_str());
    return false;
  }

  if (fVerbose) printf("HistoSvc::CreateDir - Creating directory %s\n",listname.c_str());
  fListMap[listname].clear();

  // For each list create a corresponding directory in the output file
  if (fRootOutputFile) {
    fRootOutputFile->cd("/");
    fRootOutputFile->mkdir(listname.c_str());
  }

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TH1D* HistoSvc::BookHisto (std::string listname, std::string hname, std::string htitle, Int_t nx, Double_t xlow, Double_t xup)
{
  TH1D* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      fRootOutputFile->cd(listname.c_str());
    }
    if (fListMap[listname].find(hname) == fListMap[listname].end()) {
      h = new TH1D(hname.c_str(), htitle.c_str(), nx, xlow, xup);
      fListMap[listname][hname] = (TObject*)h;
    } else {
      printf("HistoSvc::BookHistoList - ERROR - an object named '%s' was already booked in list %s\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::BookHistoList - ERROR - list '%s' does not exist\n",listname.c_str());
  }
  return h;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TH2D* HistoSvc::BookHisto2 (std::string listname, std::string hname, std::string htitle, Int_t nx, Double_t xlow, Double_t xup, Int_t ny, Double_t ylow, Double_t yup)
{
  TH2D* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      fRootOutputFile->cd(listname.c_str());
    }
    if (fListMap[listname].find(hname) == fListMap[listname].end()) {
      h = new TH2D(hname.c_str(), htitle.c_str(), nx, xlow, xup, ny, ylow, yup);
      fListMap[listname][hname] = (TObject*)h;
    } else {
      printf("HistoSvc::BookHisto2List - ERROR - an object named '%s' was already booked in list %s\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::BookHisto2List - ERROR - list '%s' does not exist\n",listname.c_str());
  }
  return h;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TTree* HistoSvc::BookNtuple (std::string listname, std::string hname, std::string htitle)
{
  TTree* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      fRootOutputFile->cd(listname.c_str());
    }
    if (fListMap[listname].find(hname) == fListMap[listname].end()) {
      h = new TTree(hname.c_str(), htitle.c_str());
      fListMap[listname][hname] = (TObject*)h;
    } else {
      printf("HistoSvc::BookNtupleList - ERROR - an object named '%s' was already booked in list %s\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::BookNtupleList - ERROR - list '%s' does not exist\n",listname.c_str());
  }
  return h;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto(std::string listname, std::string hname, Double_t xbin, Double_t weight)
{
  if (fListMap.find(listname) != fListMap.end()) {
    TH1D* h = (TH1D*)fListMap[listname][hname];
    if (h) {
      h->Fill(xbin,weight);
    } else {
      printf("HistoSvc::FillHistoList - ERROR - TH1D '%s' not found in list '%s'\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::FillHistoList - ERROR - list '%s' does not exist\n",listname.c_str());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto2(std::string listname, std::string hname, Double_t xbin, Double_t ybin, Double_t weight)
{
  if (fListMap.find(listname) != fListMap.end()) {
    TH2D* h = (TH2D*)fListMap[listname][hname];
    if (h) {
      h->Fill(xbin,ybin,weight);
    } else {
      printf("HistoSvc::FillHisto2List - ERROR - TH2D '%s' not found in list '%s'\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::FillHisto2List - ERROR - list '%s' does not exist\n",listname.c_str());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillNtuple(std::string listname, std::string hname)
{
  if (fListMap.find(listname) != fListMap.end()) {
    TTree* h = (TTree*)fListMap[listname][hname];
    if (h) {
      h->Fill();
    } else {
      printf("HistoSvc::FillNtupleList - ERROR - TTree '%s' not found in list '%s'\n",hname.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::FillNtupleList - ERROR - list '%s' does not exist\n",listname.c_str());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::SaveTGraph(std::string listname, std::string name, std::string title, TGraphErrors* g)
{
  SaveTGraph(listname,name,title,(TGraph*)g);
}

void HistoSvc::SaveTGraph(std::string listname, std::string name, std::string title, TGraphAsymmErrors* g)
{
  SaveTGraph(listname,name,title,(TGraph*)g);
}

void HistoSvc::SaveTGraph(std::string listname, std::string name, std::string title, TGraphBentErrors* g)
{
  SaveTGraph(listname,name,title,(TGraph*)g);
}

void HistoSvc::SaveTGraph(std::string listname, std::string name, std::string title, TGraphPolar* g)
{
  SaveTGraph(listname,name,title,(TGraph*)g);
}

void HistoSvc::SaveTGraph(std::string listname, std::string name, std::string title, TGraph* g)
{
  if (fListMap.find(listname) != fListMap.end()) {
    if (fListMap[listname].find(name) == fListMap[listname].end()) {
      fListMap[listname][name] = (TObject*)g;
      if (fRootOutputFile) {
	fRootOutputFile->cd("/");
	fRootOutputFile->cd(listname.c_str());
	g->SetName(name.c_str());
	g->SetTitle(title.c_str());
	g->Write();
      }
    } else {
      printf("HistoSvc::SaveTGraphList - ERROR - an object named '%s' already exists in list %s\n",name.c_str(),listname.c_str());
    }
  } else {
    printf("HistoSvc::SaveTGraphList - ERROR - list '%s' does not exist\n",listname.c_str());
  }
}
