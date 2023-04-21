
void checkAnalysisOutput(){

  TFile *f = TFile::Open("opit.root");
  TDirectory* dir = (TDirectory*) f->Get("ECalMLCalibration");
  TFile *outRoot = new TFile("analysisOutput.root","RECREATE");

  TH2F *timesChannels = new TH2F("timesChannels","Time difference angular coefficient for each channel",1024,0,1024,100,0,30);
  TH1F *timesChannels1D = new TH1F("timesChannels1D","Time Differnece",1024,0.0,1024);
  TH1F *ampsChannelsCoeff = new TH1F("ampsChannelsCoeff","Amplitude difference angular coefficient for each channel",1024,0,1024);
  TH1F *ampsChannelsCnst = new TH1F("ampsChannelsCnst","Amplitude difference constant for each channel",1024,0,1024);
  TH1F *coefficients = new TH1F("Angular coefficients","Angular Coefficients",300,0.0,0.3);
  TF1 *f1 = new TF1("f1","gaus",-3,20);
  TF1 *f2 = new TF1("f2","pol1",0,100);
  f2->FixParameter(0,0);

  Float_t consts[32][32];
  Float_t coeffs[32][32];

  Float_t sumcoef=0;
  Float_t sumconsts=0;
  Int_t count=0;

  Float_t meancoef;
  Float_t meanconst;

  for(int i = 0;i<32;i++){
    for(int j=0;j<32;j++) {
      consts[i][j]=0;
      coeffs[i][j]=0; 
    }
  }

  FILE *coeffFile;
  FILE *constsFile;

  coeffFile = fopen("channelCalibCoefficients.dat","w");
  constsFile = fopen("channelCalibConstants.dat","w");

  for(int i = 0;i<32;i++){
    for(int j=0;j<32;j++) {
      TH1F *TimeHisto = (TH1F*) dir->Get(Form("ECalMLCalibration_ECal_ECalML_singleHits_timediff_%d",(100*i+j)));
      TH2D *AmpHisto = (TH2D*) dir->Get(Form("ECalMLCalibration_ECalML_calib_%d",(100*i+j)));
      
      int numentries = AmpHisto->GetEntries();

      if(numentries>10){
        
      	TProfile *prof = AmpHisto->ProfileX();
	//prof->Fit("f2");
      
	TimeHisto->Fit("f1","R");
	// AmpHisto->Fit("f2","Q");

	// float cnst = f2->GetParameter("p0");
	// float coeff = f2->GetParameter("p1");
	// consts[i][j] = (-cnst/coeff);
	// coeffs[i][j] = (1/coeff);
	// //std::cout<<"Constant: "<<consts[i][j]<<" Coefficient: "<<coeffs[i][j]<<std::endl;
	  
	// fprintf(coeffFile,"%f ",coeffs[i][j]);
        // fprintf(constsFile,"%f ",consts[i][j]);

	// sumcoef+=coeffs[i][j];
	// sumconsts+=consts[i][j];
	// count+=1;

	// coefficients->Fill(coeffs[i][j]);
	
	float mean = f1->GetParameter(1);
	float sigma = f1->GetParameter(2);
	
	//timesChannels->Fill((i*j),mean);

	
	if(sigma < 4.) {
	  
	prof->Fit("f2");
	//AmpHisto->Fit("f2","Q");

	float cnst = f2->GetParameter("p0");
	float coeff = f2->GetParameter("p1");
	consts[i][j] = (-cnst/coeff);
	coeffs[i][j] = (1/coeff);
	//std::cout<<"Constant: "<<consts[i][j]<<" Coefficient: "<<coeffs[i][j]<<std::endl;
	  
	fprintf(coeffFile,"%f ",coeffs[i][j]);
        fprintf(constsFile,"%f ",consts[i][j]);

	sumcoef+=coeffs[i][j];
	sumconsts+=consts[i][j];
	count+=1;

	coefficients->Fill(coeffs[i][j]);

	  timesChannels1D->SetBinContent(32*i+j+1,mean);
	  timesChannels1D->SetBinError(32*i+j+1,sigma);
	  ampsChannelsCoeff->SetBinContent(32*i+j+1,coeff);
	  ampsChannelsCnst->SetBinContent(32*i+j+1,cnst);
	}

	
      }
      else{
	fprintf(coeffFile,"%f ",coeffs[i][j]);
	fprintf(constsFile,"%f ",consts[i][j]);
      }
    }
    fprintf(coeffFile,"\n");
    fprintf(constsFile,"\n");
  }

  meancoef=sumcoef/count;
  meanconst=sumconsts/count;

  std::cout<<"Mean coefficient: "<<meancoef<<" Mean constant: "<<meanconst<<std::endl;

  
  TCanvas *c1 = new TCanvas();
  c1->cd();
  timesChannels1D->Draw();

  TCanvas *c2 = new TCanvas();
  c2->cd();
  ampsChannelsCoeff->Draw();

  TCanvas *c3 = new TCanvas();
  c3->cd();
  ampsChannelsCnst->Draw();

  TCanvas *c4 = new TCanvas();
  c4->cd();
  coefficients->Draw();
  
  fclose(coeffFile);
  fclose(constsFile);
  
  outRoot->Write();
}
