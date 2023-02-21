#ifndef HistoSvc_h
#define HistoSvc_h 1

#include <map>

class TFile;
class TTree;
class TGraph;
class TGraphErrors;
class TGraphAsymmErrors;
class TGraphBentErrors;
class TGraphPolar;
class TH1D;
class TH2D;
class TString;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoSvc
{

protected:
  HistoSvc();

public:

  ~HistoSvc();
  static HistoSvc* GetInstance();

private:

  static HistoSvc* fInstance;

public:

  Bool_t Initialize();
  Bool_t Initialize(TString);
  Bool_t Finalize();

  void SetVerbose(Int_t v){ fVerbose = v; }
  TString GetOutputFileName(){ return fOutputFileName; }

  Bool_t CreateDir(std::string);
  TH1D* BookHisto(std::string, std::string,std::string, Int_t, Double_t, Double_t);
  TH2D* BookHisto2(std::string, std::string,std::string, Int_t, Double_t, Double_t, Int_t, Double_t, Double_t);
  TTree* BookNtuple(std::string, std::string,std::string);
  void FillHisto(std::string, std::string, Double_t, Double_t weight = 1.0);
  void FillHisto2(std::string, std::string, Double_t, Double_t, Double_t weight = 1.0);
  void FillNtuple(std::string, std::string);

  // Allow saving user's TGraphs to output file
  void SaveTGraph(std::string,std::string,std::string,TGraph*);
  void SaveTGraph(std::string,std::string,std::string,TGraphErrors*);
  void SaveTGraph(std::string,std::string,std::string,TGraphAsymmErrors*);
  void SaveTGraph(std::string,std::string,std::string,TGraphBentErrors*);
  void SaveTGraph(std::string,std::string,std::string,TGraphPolar*);
  
private:

  Int_t fVerbose;
  TString fOutputFileName;
  TFile* fRootOutputFile;

  std::map<std::string, std::map<std::string,TObject*>> fListMap;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

