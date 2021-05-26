#include <iostream>

#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>

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
    if (fVerbose) printf("Output ROOT file '%s' successfully opened\n",outputFileName.Data());
  } else {
    printf(" HistoSvc::Initialize - ERROR - problem creating the ROOT TFile '%s'\n",fOutputFileName.Data());
    return false;
  }

  // Create default histogram lists
  CreateList("TOP");
  CreateList("STD");

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Bool_t HistoSvc::Finalize()
{

  if (fVerbose) {
    std::map<std::string, std::map<std::string,TObject*>>::iterator it;
    for (it = fListMap.begin(); it != fListMap.end(); it++) {
      printf("List '%s' has %lu histograms\n",it->first.c_str(),it->second.size());
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
    if (fVerbose) printf("\n---> Histogram file closed with size %lld\n",fRootOutputFile->GetSize());
  }

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TH1D* HistoSvc::BookHisto (std::string name, Int_t nx, Double_t xlow, Double_t xup)
{
  return BookHistoList("STD",name,nx,xlow,xup);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TH2D* HistoSvc::BookHisto2(std::string name, Int_t nx, Double_t xlow, Double_t xup, Int_t ny, Double_t ylow, Double_t yup)
{
  return BookHisto2List("STD",name,nx,xlow,xup,ny,ylow,yup);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TTree* HistoSvc::BookNtuple(std::string name)
{
  return BookNtupleList("STD",name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto(std::string name, Double_t xbin, Double_t weight)
{
  FillHistoList("STD",name,xbin,weight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillHisto2(std::string name, Double_t xbin, Double_t ybin, Double_t weight)
{
  FillHisto2List("STD",name,xbin,ybin,weight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoSvc::FillNtuple(std::string name)
{
  FillNtupleList("STD",name);
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

  if (fVerbose) printf("Creating list %s\n",listname.c_str());
  fListMap[listname].clear();

  // For each list create a corresponding directory in the output file
  if (listname.compare("TOP") != 0 && fRootOutputFile) {
    fRootOutputFile->cd("/");
    fRootOutputFile->mkdir(listname.c_str());
  }

  return true;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TH1D* HistoSvc::BookHistoList (std::string listname, std::string hname, Int_t nx, Double_t xlow, Double_t xup)
{
  TH1D* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      if (listname.compare("TOP")!=0) fRootOutputFile->cd(listname.c_str());
    }
    if (fListMap[listname].find(hname) == fListMap[listname].end()) {
      h = new TH1D(hname.c_str(), hname.c_str(), nx, xlow, xup);
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

TH2D* HistoSvc::BookHisto2List (std::string listname, std::string hname, Int_t nx, Double_t xlow, Double_t xup, Int_t ny, Double_t ylow, Double_t yup)
{
  TH2D* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      if (listname.compare("TOP")!=0) fRootOutputFile->cd(listname.c_str());
    }
    if (fListMap[listname].find(hname) == fListMap[listname].end()) {
      h = new TH2D(hname.c_str(), hname.c_str(), nx, xlow, xup, ny, ylow, yup);
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

TTree* HistoSvc::BookNtupleList (std::string listname, std::string hname)
{
  TTree* h = 0;
  if (fListMap.find(listname) != fListMap.end()) {
    if (fRootOutputFile) {
      fRootOutputFile->cd("/");
      if (listname.compare("TOP")!=0) fRootOutputFile->cd(listname.c_str());
    }
    if (fListMap[listname].find(hname) == fListMap[listname].end()) {
      h = new TTree(hname.c_str(), hname.c_str());
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

void HistoSvc::FillHistoList(std::string listname, std::string hname, Double_t xbin, Double_t weight)
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

void HistoSvc::FillHisto2List(std::string listname, std::string hname, Double_t xbin, Double_t ybin, Double_t weight)
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

void HistoSvc::FillNtupleList(std::string listname, std::string hname)
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
