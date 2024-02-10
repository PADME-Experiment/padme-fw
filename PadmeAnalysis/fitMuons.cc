void fitMuons(){
  
  TFile *f = TFile::Open("run50384_forCalib.root");
  TDirectory* dir = (TDirectory*) f->Get("ECalMLMuons");
  TFile *outRoot = new TFile("run50384_CalibMuons.root","RECREATE");
  outRoot->cd();
  TH1F *maxesHist = new TH1F("maxesHist","Position of the muon maximum in the ECalML Channels",100,80,130);
  TH1F *maxesHistECal = new TH1F("maxesHistECal","Position of the muon maximum in the ECal Channels",40,10,30);
  
  Float_t maxes[32][32];
  Float_t maxesECal[32][32];

  for(int i = 0;i<32;i++){
    for(int j=0;j<32;j++) {
      maxes[i][j]=0;
      maxesECal[i][j]=0;
    }
  }
  
  FILE *maxesFile;
  FILE *maxesECalFile;
 
  maxesFile = fopen("run50384_CalibMuons.dat","w");
  maxesECalFile = fopen("run50384_CalibMuonsECal.dat","w");
    
  TF1 *f1 = new TF1("f1","landau",80,300);
  TF1 *f2 = new TF1("f2","landau",12,100);
 
  for(int i=0;i<32;i++){
    for(int j=0;j<32;j++) {
      outRoot->cd();
      TH1D *ECalMLHisto = (TH1D*) ((TH1D*) ((TH1D*) dir->Get(Form("ECalMLMuons_ECalML_muons_energy%d",(100*i+j))))->Clone())->Rebin(5);
      TH1D *ECalHisto = (TH1D*) dir->Get(Form("ECalMLMuons_ECal_muons_energy%d",(100*i+j)));
      
      int numentries = ECalMLHisto->GetEntries();
      //int numentriesECal = ECalHisto->GetEntries();
       
      if(numentries>10){
	ECalMLHisto->Fit("f1","R");
	ECalHisto->Fit("f2","R");
      
	maxes[i][j] = f1->GetParameter(1);
	maxesECal[i][j] = f2->GetParameter(1);
	
	fprintf(maxesFile,"%f ",maxes[i][j]);
	fprintf(maxesECalFile,"%f ",maxesECal[i][j]);
	
	maxesHist->Fill(maxes[i][j]);
	maxesHistECal->Fill(maxesECal[i][j]);
	
      }
      
      else{
	fprintf(maxesFile,"%f ",maxes[i][j]);
	fprintf(maxesECalFile,"%f ",maxes[i][j]);
      }
      
    }
    fprintf(maxesFile,"\n");
    fprintf(maxesECalFile,"\n");
  }
  
  TCanvas *c1 = new TCanvas();
  c1->cd();
  maxesHist->Draw();

  fclose(maxesFile);
  fclose(maxesECalFile);
  
  std::cout<<"Writing ROOT file... "<<std::endl;
  
  outRoot->Write();

  std::cout<<"ROOT file written. Bye!"<<std::endl;
}
