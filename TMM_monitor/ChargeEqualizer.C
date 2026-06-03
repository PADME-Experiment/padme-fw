
float pitch=0.250; //mm
int maxStrip=358, maxStripApv=128;                                                                                      

// define a function with 3 parameters
Double_t fitf(Double_t *x,Double_t *par) {
  Double_t arg = 0;
  Double_t xnorm = par[0];
  if (par[2]!=0) arg = (x[0] - par[1])/par[2];
  if (x[0] < -14.25) xnorm = par[3];
  if (x[0] > 14.25) xnorm = par[4];
  Double_t fitval = xnorm*TMath::Exp(-0.5*arg*arg);
  return fitval;
}

// this function uses fitf to fit a histogram
void fitexample() {
  
  // open a file and get a histogram
  //TFile *f = new TFile("hsimple.root");
  //TH1F *hpx = (TH1F*)f->Get("hpx");
  
  // Create a TF1 object using the function defined above.
  // The last three parameters specify the number of parameters
  // for the function.
  TF1 *func = new TF1("fit",fitf,-50.,50.,5);
  // set the parameters to the mean and RMS of the histogram
  func->SetParameters(500,0.,10.,500,500);
  
  // give the parameters meaningful names
  func->SetParNames ("Constant_APV2","Mean_value","Sigma","Constant_APV1","Constant_APV3");
  
  // call TH1::Fit with the name of the TF1 object
  //hpx->Fit("fit");
} 
