#define RecoTMM_cxx
#include "RecoTMM.h"

using namespace std;

int TGraphAttribute(TGraphErrors *Graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
  Graph->SetTitle(title);
  Graph->SetName(title);
  Graph->GetXaxis()->SetTitle(xlabel);
  Graph->GetYaxis()->SetTitle(ylabel);
  Graph->SetMarkerStyle(markerstyle);
  Graph->SetMarkerColor(color);
  return 0;
}

void RecoTMM::StripFinder(int &iReadout, int &iStrip) { 
  // ireadout è la variabile del tree che ti dice se la strip sia x o y, istrip è la variabile del tree che ti dice il numero di strip 
  // il tree che sto analizzando è mappato da mmqad 
  // voglio ruotare le x e le y nel sistema di riferimento PADME
  // from the electronics maps to our local reference system, there are
  // old     new 
  // x  -->  y
  // y  --> -x
  if(iReadout == 0) {
    iReadout = 1; // x  -->  y
    iStrip = iStrip;
  }
  else if(iReadout == 1) {
    iReadout = 0; // y  -->  -x
    iStrip = maxStrip - iStrip - 1;
  }
}

void RecoTMM::GlobalCoordinate(int iReadout, double &x_strip){ 
  // sistema di riferimento rispetto al centro della camera con padme reference frame
  // mancano traslazioni e rotazioni metrologia
  if(iReadout == 0) { //X
    x_strip = x_strip + GLOBAL_X_TRANSLATION; 
  }
  else if(iReadout == 1) { //Y
    x_strip = x_strip + GLOBAL_Y_TRANSLATION;
  }
}

void RecoTMM::CoordinateFinder(int iStrip, const vector<short> &camp, int &iReadout, double &x_strip, double &q_strip){

  if (camp.empty()) {
    x_strip = -999;
    q_strip = 0;
    return;
  }
  int Nbins = camp.size();
  double qmax = -1000;
  
  for (int ibin = 0; ibin < Nbins; ibin++) {
    double qbin = camp.at(ibin); // Charge in the bin
    if (qbin > qmax) {
      qmax = qbin;
    }
  } //massimo assoluto in carica trovato

  StripFinder(iReadout, iStrip); //rimappa istrip nelle posizioni fisiche corrette con sistema di riferimento locale

  // correzione carica in base alla calibrazione APV, da fare dopo la strip finder per mappare correttamente i canali
  //x_strip = iStrip * pitch - pitch;
  x_strip = iStrip;
  float APV_q_corr = 1.;
  if(iReadout == 0) {
    if (iStrip < 122) APV_q_corr = APV1_fqx;
    if (iStrip >= 122+113) APV_q_corr = APV3_fqx;
  }
  if(iReadout == 1) {
    if (iStrip < 122) APV_q_corr = APV1_fqy;
    if (iStrip >= 122+113) APV_q_corr = APV3_fqy;
  }
  q_strip = qmax*APV_q_corr;
  
  GlobalCoordinate(iReadout, x_strip);
  // mappatura corretta delle strip con coordinate globali e carica corretta, dopo calibrazione apv
}

SliceFitResult RecoTMM::RunFitSlicesY(TH2F *H, TString tag){
  
  SliceFitResult out;

  if (!H) return out;

  H->FitSlicesY();

  TH1D *h0 = (TH1D*)gDirectory->Get(Form("%s_0", H->GetName()));
  TH1D *h1 = (TH1D*)gDirectory->Get(Form("%s_1", H->GetName()));
  TH1D *h2 = (TH1D*)gDirectory->Get(Form("%s_2", H->GetName()));
  TH1D *h3 = (TH1D*)gDirectory->Get(Form("%s_chi2", H->GetName()));

  if (!h0 || !h1 || !h2s) {
    cerr << "ERROR: FitSlicesY failed for " << H->GetName() << endl;
    return out;
  }

  out.amp   = (TH1D*)h0->Clone(Form("hAmpSlice_%s", tag.Data()));
  out.mean  = (TH1D*)h1->Clone(Form("hMeanSlice_%s", tag.Data()));
  out.sigma = (TH1D*)h2->Clone(Form("hSigmaSlice_%s", tag.Data()));
  out.chi2  = (TH1D*)h3->Clone(Form("hChi2Slice_%s", tag.Data()));

  out.amp->SetDirectory(0);
  out.mean->SetDirectory(0);
  out.sigma->SetDirectory(0);
  out.chi2->SetDirectory(0);

  return out;
}

TF1* RecoTMM::FitDoubleGaussian(TH1D *h, TString name, double xmin, double xmax){
  
  TF1 *prefit = new TF1(Form("prefit_%s", name.Data()), "gaus", xmin, xmax);
  h->Fit(prefit, "RQ0");

  double A0  = prefit->GetParameter(0);
  double mu0 = prefit->GetParameter(1);
  double s0  = fabs(prefit->GetParameter(2));
  if (s0 <= 0) s0 = 10.;

  TF1 *fit = new TF1( Form("f_%s", name.Data()), "[0]*exp(-0.5*((x-[1])/[2])^2)+[3]*exp(-0.5*((x-[1])/[4])^2)", xmin, xmax );

  fit->SetParNames("A_1", "mean", "sigma_1", "A_2", "sigma_2");
  fit->SetParameters(A0/2, mu0, s0, A0, s0/2);

  fit->SetParLimits(0, 0., 10.*A0);
  fit->SetParLimits(1, xmin, xmax);
  fit->SetParLimits(2, 1., xmax-xmin);
  fit->SetParLimits(3, 0., 10.*A0);
  fit->SetParLimits(4, 1., 3.*(xmax-xmin));

  h->Fit(fit, "RQ");

  delete prefit;
  return fit;
}

void RecoTMM::BuildFitRatio(TH1D *hMeanFull, TH1D *hSigmaFull, TF1 *fit, TGraphErrors *gRatio, TGraphErrors *gDiff, TGraphErrors *gRatio3s, TGraphErrors *gDiff3s, vector<bool> &pass3s){
  
  pass3s.assign(maxStrip, false);

  for (int bx = 1; bx <= hMeanFull->GetNbinsX(); bx++) {
    int strip = bx - 1;

    double x  = hMeanFull->GetBinCenter(bx);
    double q  = hMeanFull->GetBinContent(bx);
    double eq = hMeanFull->GetBinError(bx);
    double s  = hSigmaFull->GetBinContent(bx);
    double qfit = fit->Eval(x);

    if (q <= 0 || qfit <= 0 || s <= 0) continue;

    double ratio = qfit / q;
    double diff  = qfit - q;

    int ip = gRatio->GetN();
    gRatio->SetPoint(ip, x, ratio);
    gRatio->SetPointError(ip, 0., eq > 0 ? ratio * eq / q : 0.);

    int id = gDiff->GetN();
    gDiff->SetPoint(id, x, diff);
    gDiff->SetPointError(id, 0., eq);

    pass3s[strip] = fabs(diff) < 3. * s;
    if (pass3s[strip]) {
      int ip3 = gRatio3s->GetN();
      gRatio3s->SetPoint(ip3, x, ratio);
      gRatio3s->SetPointError(ip3, 0., eq > 0 ? ratio * eq / q : 0.);

      int id3 = gDiff3s->GetN();
      gDiff3s->SetPoint(id3, x, diff);
      gDiff3s->SetPointError(id3, 0., eq);
    }
  }
}

void RecoTMM::FillBlockGraphsFromSlices( int iR, vector<vector<bool>> &Blockpass3s){

  Blockpass3s.assign(hBlockqmaxstrip[iR].size(), vector<bool>(maxStrip, false));
  for (size_t ib = 0; ib < hBlockqmaxstrip[iR].size(); ib++) {

    SliceFitResult s = RunFitSlicesY(hBlockqmaxstrip[iR][ib], Form("%s_block%zu", tmm_tag[iR].Data(), ib) );

    hBlockAmpslice[iR][ib]   = s.amp;
    hBlockMeanslice[iR][ib]  = s.mean;
    hBlockSigmaslice[iR][ib] = s.sigma;
    hBlockChi2slice[iR][ib]  = s.chi2;

    SliceFitResult sFull = RunFitSlicesY(hBlockqmaxstripFull[iR][ib], Form("%s_block%zu_full", tmm_tag[iR].Data(), ib));

    hBlockAmpsliceFull[iR][ib]   = sFull.amp;
    hBlockMeansliceFull[iR][ib]  = sFull.mean;
    hBlockSigmasliceFull[iR][ib] = sFull.sigma;
    hBlockChi2sliceFull[iR][ib]  = sFull.chi2;

    if (!s.mean) continue;

    TF1 *fb = FitDoubleGaussian(s.mean, Form("%s_block%zu", tmm_tag[iR].Data(), ib), 20., 340.);

    double A1 = fb->GetParameter(0);
    double mu = fb->GetParameter(1);
    double s1 = fabs(fb->GetParameter(2));
    double A2 = fb->GetParameter(3);
    double s2 = fabs(fb->GetParameter(4));

    double e_mu = fb->GetParError(1);

    double norm = A1 + A2;
    if (norm <= 0) continue;

    double sigma_eff = sqrt((A1*s1*s1 + A2*s2*s2) / norm);
    double charge_proxy = A1 * s1 * sqrt(2. * TMath::Pi()) + A2 * s2 * sqrt(2. * TMath::Pi());
    double xblock = ib;

    int p0 = g_BlockBeamSpot[iR]->GetN();
    g_BlockBeamSpot[iR]->SetPoint(p0, xblock, mu);
    g_BlockBeamSpot[iR]->SetPointError(p0, 0., e_mu);

    int p1 = g_BlockBeamSpread[iR]->GetN();
    g_BlockBeamSpread[iR]->SetPoint(p1, xblock, sigma_eff);
    g_BlockBeamSpread[iR]->SetPointError(p1, 0., 0); //errors randomly selected

    int p2 = g_BlockBeamCharge[iR]->GetN();
    g_BlockBeamCharge[iR]->SetPoint(p2, xblock, charge_proxy);
    g_BlockBeamCharge[iR]->SetPointError(p2, 0., sqrt(charge_proxy)); //errors randomly selected

    BuildFitRatio(
      hBlockMeansliceFull[iR][ib],
      hBlockSigmasliceFull[iR][ib],
      fb,
      g_BlockFitFullRatio[iR][ib],
      g_BlockFitFullDiff[iR][ib],
      g_BlockFitFullRatio3s[iR][ib],
      g_BlockFitFullDiff3s[iR][ib],
      Blockpass3s[ib]
    );

  }
}

// void RecoTMM::UpdateSummaryTxt(TString filename, int RunID, double qx, double err_qx, double qy, double err_qy, double x, double err_x, double y, double err_y, double sigmax, double err_sigmax, double sigmay, double err_sigmay)
// {
//   vector<TString> lines;
//   bool foundRun = false;

//   ifstream infile(filename.Data());

//   if (infile.is_open()) {
//     string line;

//     while (getline(infile, line)) {
//       TString tline(line);

//       if (tline.BeginsWith("#")) {
//         lines.push_back(tline);
//         continue;
//       }

//       stringstream ss(line);
//       int existingRun = -1;
//       ss >> existingRun;

//       if (existingRun == RunID) {
//         TString newline = Form("%d %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g",
//                               RunID,
//                               qx, err_qx, qy, err_qy,
//                               x, err_x, y, err_y,
//                               sigmax, err_sigmax, sigmay, err_sigmay);

//         lines.push_back(newline);
//         foundRun = true;
//       } else {
//         lines.push_back(tline);
//       }
//     }

//     infile.close();
//   }

//   if (!foundRun) {
//     TString newline = Form("%d %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g",
//                           RunID,
//                           qx, err_qx, qy, err_qy,
//                           x, err_x, y, err_y,
//                           sigmax, err_sigmax, sigmay, err_sigmay);

//     lines.push_back(newline);
//   }

//   ofstream summaryTxt(filename.Data());

//   summaryTxt << "# Global TMM summary\n";
//   summaryTxt << "# RunNumber qx err_qx qy err_qy x err_x y err_y sigmax err_sigmax sigmay err_sigmay\n";

//   for (auto &line : lines) {
//     if (line.BeginsWith("#")) continue;
//     summaryTxt << line << endl;
//   }

//   summaryTxt.close();
// }

void RecoTMM::LoopFileList(TObjArray &inputFileNameList, int NevtBlock) {

  // cout << "DEBUG LoopFileList: start" << endl;

  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR: empty input file list" << endl;
    return;
  }

  if (NevtBlock <= 0) {
    cerr << "ERROR: NevtBlock must be > 0. Current value = " << NevtBlock << endl;
    return;
  }

  cout << "Number of input files: " << inputFileNameList.GetEntries() << endl;

  if (maxEvents > 0) {
    cout << "N entries requested: " << maxEvents << endl;
  } else {
    cout << "N entries requested: all available entries" << endl;
  }

  cout << "Block size: " << NevtBlock << " events" << endl;

  // ------------------------------------------------------------
  // Histograms and graphs: created once, before creating TChain and looping on events
  // ------------------------------------------------------------
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    // charge distribution histograms
    // hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%{s}", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), maxStrip, -xmax/2, +xmax/2, 1000, 0, 2500);
    hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstrip[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hqmaxstrip[l]->SetYTitle("q_{max} [ADC counts]");

    // hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), maxStrip, -xmax/2, +xmax/2, 1000, 0, 2500);
    hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%s", tmm_tag[l].Data()), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstripFull[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hqmaxstripFull[l]->SetYTitle("q_{max} [ADC counts]");

    // FitSlicesY() histograms
    hAmpslice[l] = new TH1D(Form("hAmpslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hAmpslice[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hAmpslice[l]->SetYTitle("AMP (q_{max} [ADC counts])");

    hMeanslice[l] = new TH1D(Form("hMeanslice%s", tmm_tag[l].Data()), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hMeanslice[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hMeanslice[l]->SetYTitle("q_{max} [ADC counts]");

    hSigmaslice[l] = new TH1D(Form("hSigmaslice%s", tmm_tag[l].Data()), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hSigmaslice[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hSigmaslice[l]->SetYTitle("#sigma_{q_{max}} [ADC counts]");
    
    hAmpsliceFull[l] = new TH1D(Form("hAmpsliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hAmpsliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hAmpsliceFull[l]->SetYTitle("AMP (q_{max} [ADC counts])");

    hMeansliceFull[l] = new TH1D(Form("hMeansliceFull%s", tmm_tag[l].Data()), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hMeansliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hMeansliceFull[l]->SetYTitle("q_{max} [ADC counts]");

    hSigmasliceFull[l] = new TH1D(Form("hSigmasliceFull%s", tmm_tag[l].Data()), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hSigmasliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hSigmasliceFull[l]->SetYTitle("#sigma_{q_{max}} [ADC counts]");

    //calibrated plots
    hqmaxstrip_cal[l] = new TH2F(Form("hqmaxstrip_cal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstrip_cal[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hqmaxstrip_cal[l]->SetYTitle("q_{max} calib [ADC counts]");

    hqmaxstripFull_cal[l] = new TH2F(Form("hqmaxstripFull_cal%s", tmm_tag[l].Data()), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstripFull_cal[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hqmaxstripFull_cal[l]->SetYTitle("q_{max} calib [ADC counts]");

    hqmaxstrip_cal3s[l] = new TH2F(Form("hqmaxstrip_cal3s%s", tmm_tag[l].Data()), TString("3Sigma threshold selection q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstrip_cal3s[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hqmaxstrip_cal3s[l]->SetYTitle("q_{max} calib [ADC counts]");

    hqmaxstripFull_cal3s[l] = new TH2F(Form("hqmaxstripFull_cal3s%s", tmm_tag[l].Data()), TString("3Sigma threshold selection q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstripFull_cal3s[l]->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
    hqmaxstripFull_cal3s[l]->SetYTitle("q_{max} calib [ADC counts]");

    // block by block tgrapherrors
    g_BlockBeamSpot[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpot[l], Form("g_BlockBeamSpot_%s", tmm_tag[l].Data()), "entry", Form("%s_{Beam} strip", tmm_tag[l].Data()), 20, kBlue + l);

    g_BlockBeamSpread[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamSpread[l], Form("g_BlockBeamSpread_%s", tmm_tag[l].Data()), "entry", Form("#sigma %s_{Beam} strip", tmm_tag[l].Data()), 21, kRed + l);

    g_BlockBeamCharge[l] = new TGraphErrors();
    TGraphAttribute(g_BlockBeamCharge[l], Form("g_BlockBeamCharge_%s", tmm_tag[l].Data()), "entry", "q_{Beam} [ADC counts]", 22, kGreen + 2 + l);
  
    //TGraph for strip calibration and equalization
    g_FitFullDiff[l] = new TGraphErrors();
    TGraphAttribute(g_FitFullDiff[l], Form("g_FitFullDiff%s", tmm_tag[l].Data()), Form("%s strip", tmm_tag[l].Data()), "Q_{maxbin} - EvalFit [ADC counts]", 22, kBlack);

    g_FitFullDiff3s[l] = new TGraphErrors();
    TGraphAttribute(g_FitFullDiff3s[l], Form("g_FitFullDiff3s%s", tmm_tag[l].Data()), Form("%s strip", tmm_tag[l].Data()), "Q_{maxbin} - EvalFit [ADC counts]", 22, kBlack);

    g_FitFullRatio[l] = new TGraphErrors();
    TGraphAttribute(g_FitFullRatio[l], Form("g_FitFullRatio%s", tmm_tag[l].Data()), Form("%s strip", tmm_tag[l].Data()), "EvalFit / Q_{maxbin}", 22, kBlack);
  
    g_FitFullRatio3s[l] = new TGraphErrors();
    TGraphAttribute(g_FitFullRatio3s[l], Form("g_FitFullRatio3s%s", tmm_tag[l].Data()), Form("%s strip", tmm_tag[l].Data()), "EvalFit / Q_{maxbin}", 22, kBlack);
  }

  // ------------------------------------------------------------
  // Block/global accumulators
  // ------------------------------------------------------------

  TString blockTxtName = Form("outputTMM/Monitor_TMM_run%d_blocks.txt", RunID);
  ofstream blockTxt(blockTxtName.Data());

  blockTxt << "#RunNumber = " << RunID << endl;
  blockTxt << "# period,qx,err_qx,qy,err_qy,x,err_x,y,err_y,sigmax,err_sigmax,sigmay,err_sigmay" << endl;

  Long64_t globalEntry = 0;
  int blockCounter = 0;

  auto EnsureBlockHistograms = [&](int iblock) {
    for (int l = 0; l < TMMCH_N_Readout; l++) {
      while ((int)hBlockqmaxstrip[l].size() <= iblock) {
        int b = hBlockqmaxstrip[l].size();

        // charge distribution histograms
        hBlockqmaxstrip[l].push_back(new TH2F( Form("hBlockqmaxstrip%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstrip[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstrip[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull[l].push_back(new TH2F( Form("hBlockqmaxstripFull%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstripFull[l].back()->SetYTitle("q_{max} [ADC counts]");

        // charge distribution histograms calibrated block by block
        hBlockqmaxstrip_cal[l].push_back(new TH2F( Form("hBlockqmaxstrip_cal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstrip_cal[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstrip_cal[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull_cal[l].push_back(new TH2F( Form("hBlockqmaxstripFull_cal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull_cal[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstripFull_cal[l].back()->SetYTitle("q_{max} [ADC counts]");

        // charge distribution histograms calibrated block by block - 3sigma criteria
        hBlockqmaxstrip_cal3s[l].push_back(new TH2F( Form("hBlockqmaxstrip_cal3s%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstrip_cal3s[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstrip_cal3s[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull_cal3s[l].push_back(new TH2F( Form("hBlockqmaxstripFull_cal3s%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull_cal3s[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstripFull_cal3s[l].back()->SetYTitle("q_{max} [ADC counts]");

        // charge distribution histograms overall calibrated
        hBlockqmaxstrip_Overallcal[l].push_back(new TH2F( Form("hBlockqmaxstrip_Overallcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstrip_Overallcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstrip_Overallcal[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull_Overallcal[l].push_back(new TH2F( Form("hBlockqmaxstripFull_Overallcal%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull_Overallcal[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstripFull_Overallcal[l].back()->SetYTitle("q_{max} [ADC counts]");

        // charge distribution histograms overall calibrated 3sigma criteria
        hBlockqmaxstrip_Overallcal3s[l].push_back(new TH2F( Form("hBlockqmaxstrip_Overallcal3s%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstrip_Overallcal3s[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstrip_Overallcal3s[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockqmaxstripFull_Overallcal3s[l].push_back(new TH2F( Form("hBlockqmaxstripFull_Overallcal3s%s_block%04d", tmm_tag[l].Data(), b), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + Form("] block %04d", b), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500));
        hBlockqmaxstripFull_Overallcal3s[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockqmaxstripFull_Overallcal3s[l].back()->SetYTitle("q_{max} [ADC counts]");

        // FitSlicesY() histograms
        hBlockAmpslice[l].push_back(new TH1D(Form("hBlockAmpslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockAmpslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockAmpslice[l].back()->SetYTitle("AMP (q_{max} [ADC counts])");

        hBlockMeanslice[l].push_back(new TH1D(Form("hBlockMeanslice%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockMeanslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockMeanslice[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockSigmaslice[l].push_back(new TH1D(Form("hBlockSigmaslice%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockSigmaslice[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockSigmaslice[l].back()->SetYTitle("#sigma_{q_{max}} [ADC counts]");

        hBlockChi2slice[l].push_back(new TH1D(Form("hBlockChi2slice%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockChi2slice[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockChi2slice[l].back()->SetYTitle("#Chi^{2}_{q_{max}} [ADC counts]");
        
        hBlockAmpsliceFull[l].push_back(new TH1D(Form("hBlockAmpsliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockAmpsliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockAmpsliceFull[l].back()->SetYTitle("AMP (q_{max} [ADC counts])");

        hBlockMeansliceFull[l].push_back(new TH1D(Form("hBlockMeansliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockMeansliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockMeansliceFull[l].back()->SetYTitle("q_{max} [ADC counts]");

        hBlockSigmasliceFull[l].push_back(new TH1D(Form("hBlockSigmasliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("sigma value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockSigmasliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockSigmasliceFull[l].back()->SetYTitle("#sigma_{q_{max}} [ADC counts]");

        hBlockChi2sliceFull[l].push_back(new TH1D(Form("hBlockChi2sliceFull%s_block%04d", tmm_tag[l].Data(), b), TString("Chi2 value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5));
        hBlockChi2sliceFull[l].back()->SetXTitle(TString(tmm_tag[l]+" [strip]").Data());
        hBlockChi2sliceFull[l].back()->SetYTitle("#Chi^{2}_{q_{max}} [ADC counts]");

        //TGraph for strip calibration and equalization
        g_BlockFitFullDiff[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockFitFullDiff[l].back(), Form("g_BlockFitFullDiff%s", tmm_tag[l].Data()), "strip", "EvalFit - Q_{maxbin} [ADC counts]", 22, kBlack);

        g_BlockFitFullDiff3s[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockFitFullDiff3s[l].back(), Form("g_BlockFitFullDiff3s%s", tmm_tag[l].Data()), "strip", "Q_{maxbin} - EvalFit [ADC counts]", 22, kBlack);

        g_BlockFitFullRatio[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockFitFullRatio[l].back(), Form("g_BlockFitFullRatio%s", tmm_tag[l].Data()), "strip", "EvalFit / Q_{maxbin}", 22, kBlack);
      
        g_BlockFitFullRatio3s[l].push_back(new TGraphErrors());
        TGraphAttribute(g_BlockFitFullRatio3s[l].back(), Form("g_BlockFitFullRatio3s%s", tmm_tag[l].Data()), "strip", "EvalFit / Q_{maxbin}", 22, kBlack);
  
      }
    }
  };

  // ------------------------------------------------------------
  // Create chain of input files
  // ------------------------------------------------------------
  // cout << "DEBUG LoopFileList: after creating TChain, before checking entries" << endl;

  if (!fTree) {
    cerr << "ERROR: fTree is null in LoopFileList" << endl;
    return;
  }
  // fTree->Print();
  Long64_t runNEntries = fTree->GetEntries();
  if (runNEntries <= 0) {
    cerr << "ERROR: input chain has zero entries" << endl;
    return;
  }
  cout << "Found Tree 'apv_raw' with " << runNEntries << " entries" << endl;
  Long64_t nToProcess = runNEntries;

  if (maxEvents > 0 && maxEvents < runNEntries) {
    nToProcess = maxEvents;
  }

  cout << "Will process " << nToProcess << " entries" << endl;

  for (Long64_t iev = 0; iev < nToProcess; ++iev) {

    Long64_t nb = fTree->GetEntry(iev);    
    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }
    EnsureBlockHistograms(blockCounter);

    if (globalEntry % 1000 == 0) {
      float progress = static_cast<float>(globalEntry) / nToProcess;

      cout << "Processed " << globalEntry << " out of " << nToProcess << " entries (" << fixed << setprecision(2) << progress * 100 << "%)" << endl;
    }

    if (!mmLayer || !mmReadout || !mmStrip || !raw_q) {
      cerr << "WARNING: null branch pointer at event " << iev << endl;
      continue;
    }

    int firedstrip_size = min(
      min((int)mmLayer->size(),   (int)mmReadout->size()),
      min((int)mmStrip->size(),  (int)raw_q->size())
    );
    // cout << "Event " << iev << ": firedstrip_size = " << firedstrip_size << endl;

    for (int j = 0; j < firedstrip_size; j++) {
      double x_strip = 0;
      double q_strip = 0;

      int channel = mmStrip->at(j);

      // mmReadout is vector<char>. Expected values appear to be 'X' = 88 and 'Y' = 89.
      int iReadout = (int)mmReadout->at(j);
      int iReadout_new = iReadout - 88; // Map 'X'->0, 'Y'->1 to be compatible with TMMCH_N_Readout

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

      CoordinateFinder(channel, raw_q->at(j), iReadout_new, x_strip, q_strip);

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

      if (channel % 2 == 0) {
        hqmaxstrip[iReadout_new]->Fill(x_strip, q_strip);
        hBlockqmaxstrip[iReadout_new][blockCounter]->Fill(x_strip, q_strip);
      }
      hqmaxstripFull[iReadout_new]->Fill(x_strip, q_strip);
      hBlockqmaxstripFull[iReadout_new][blockCounter]->Fill(x_strip, q_strip);
      
    }

    globalEntry++;

    if (globalEntry % NevtBlock == 0) {
        blockCounter++;
    }
  }

  cout << "#### Total reconstructed events: " << globalEntry << endl;
  cout << "################################ " << endl;
  cout << "#### Slice fit procedure to equalise channels + baseline removal" << endl;

  vector<vector<bool>> pass3s(TMMCH_N_Readout, vector<bool>(maxStrip, false));
  vector<vector<vector<bool>>> Blockpass3s(TMMCH_N_Readout);

  for (int iR = 0; iR < TMMCH_N_Readout; iR++) {

    SliceFitResult even = RunFitSlicesY(hqmaxstrip[iR], Form("%s_even", tmm_tag[iR].Data()));
    SliceFitResult full = RunFitSlicesY(hqmaxstripFull[iR], Form("%s_full", tmm_tag[iR].Data()));

    hAmpslice[iR]   = even.amp;
    hMeanslice[iR]  = even.mean;
    hSigmaslice[iR] = even.sigma;
    hChi2slice[iR]  = even.chi2;

    hAmpsliceFull[iR]   = full.amp;
    hMeansliceFull[iR]  = full.mean;
    hSigmasliceFull[iR] = full.sigma;
    hChi2sliceFull[iR]  = full.chi2;

    TF1 *fit = FitDoubleGaussian(hMeanslice[iR], tmm_tag[iR], 20., 340.);
    f.push_back(fit);

    BuildFitRatio(hMeansliceFull[iR], hSigmasliceFull[iR], fit, g_FitFullRatio[iR], g_FitFullDiff[iR], g_FitFullRatio3s[iR], g_FitFullDiff3s[iR], pass3s[iR]);  
  }

  for (int iR = 0; iR < TMMCH_N_Readout; iR++) {
    FillBlockGraphsFromSlices(iR, Blockpass3s[iR]);
  }

  for (int ib = 0; ib < g_BlockBeamSpot[0]->GetN(); ib++) {

    double bx, x, by, y;
    double err_bx, err_x, err_by, err_y;
    double dummy;

    g_BlockBeamCharge[0]->GetPoint(ib, dummy, bx);
    g_BlockBeamCharge[1]->GetPoint(ib, dummy, by);

    g_BlockBeamSpot[0]->GetPoint(ib, dummy, x);
    g_BlockBeamSpot[1]->GetPoint(ib, dummy, y);

    err_bx = g_BlockBeamCharge[0]->GetErrorY(ib);
    err_by = g_BlockBeamCharge[1]->GetErrorY(ib);

    err_x = g_BlockBeamSpot[0]->GetErrorY(ib);
    err_y = g_BlockBeamSpot[1]->GetErrorY(ib);

    double sigmax, sigmay, err_sigmax, err_sigmay;
    g_BlockBeamSpread[0]->GetPoint(ib, dummy, sigmax);
    g_BlockBeamSpread[1]->GetPoint(ib, dummy, sigmay);

    err_sigmax = g_BlockBeamSpread[0]->GetErrorY(ib);
    err_sigmay = g_BlockBeamSpread[1]->GetErrorY(ib);

    blockTxt << ib << ","
            << bx << "," << err_bx << ","
            << by << "," << err_by << ","
            << x  << "," << err_x << ","
            << y  << "," << err_y << ","
            << sigmax << "," << err_sigmax << ","
            << sigmay << "," << err_sigmay
            << endl;
  }

  // cout << "DEBUG: arriva alla fine del primo loop sugli eventi" << endl;

  //calibrated plots
  for (Long64_t iev = 0; iev < nToProcess; ++iev) {

    Long64_t nb = fTree->GetEntry(iev);    
    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }
    if (!mmLayer || !mmReadout || !mmStrip || !raw_q) {
      cerr << "WARNING: null branch pointer at event " << iev << endl;
      continue;
    }

    int firedstrip_size = min(
      min((int)mmLayer->size(),   (int)mmReadout->size()),
      min((int)mmStrip->size(),  (int)raw_q->size())
    );

    for (int j = 0; j < firedstrip_size; j++) {
      double x_strip = 0;
      double q_strip = 0;

      int channel = mmStrip->at(j);
      // cout << "DEBUG: channel = " << channel << endl;

      // mmReadout is vector<char>. Expected values appear to be 'X' = 88 and 'Y' = 89.
      int iReadout = (int)mmReadout->at(j);
      int iReadout_new = iReadout - 88; // Map 'X'->0, 'Y'->1 to be compatible with TMMCH_N_Readout

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

      CoordinateFinder(channel, raw_q->at(j), iReadout_new, x_strip, q_strip);
      // cout << "DEBUG: CoordinateFinder correctly applied and xstrip found = " << x_strip << endl;

      if (iReadout_new < 0 || iReadout_new >= TMMCH_N_Readout) {
        continue;
      }

      if (channel > 358) {
        cout << "ERROR channel bigger than FitFullRatio vector - channel = " << channel << endl;
      }

      double chargecorrection = 1;
      if(x_strip>20 && x_strip<340){
        chargecorrection = g_FitFullRatio[iReadout_new]->Eval(x_strip);
        // cout << "chargecorrection = " << chargecorrection << " - channel = " << channel << endl;
      }
      if (channel % 2 == 0) {
        hqmaxstrip_cal[iReadout_new]->Fill(x_strip, q_strip*chargecorrection);
      }
      hqmaxstripFull_cal[iReadout_new]->Fill(x_strip, q_strip*chargecorrection);
    
      int ic = iev / NevtBlock;

      // cout << "DEBUG: before loop inside block for calibrated plots" << endl;
      if (ic >= 0 && ic < (int)Blockpass3s[iReadout_new].size()) {

        double Blockchargecorrection = 1.;
        if (x_strip > 20 && x_strip < 340) {
          Blockchargecorrection = g_BlockFitFullRatio[iReadout_new][ic]->Eval(x_strip);
        }
        // cout << "DEBUG: Blockchargecorrection defined" << endl;
        if (channel % 2 == 0) {
          hBlockqmaxstrip_cal[iReadout_new][ic]->Fill(x_strip, q_strip * Blockchargecorrection);
          hBlockqmaxstrip_Overallcal[iReadout_new][ic]->Fill(x_strip, q_strip * chargecorrection);
        }
        // cout << "DEBUG: Blockchargecorrection defined" << endl;
        hBlockqmaxstripFull_cal[iReadout_new][ic]->Fill(x_strip, q_strip * Blockchargecorrection);
        hBlockqmaxstripFull_Overallcal[iReadout_new][ic]->Fill(x_strip, q_strip * chargecorrection);
      }
    }
  }
  // cout << "DEBUG: hBlockqmaxstripFull_cal are filled" << endl;

  for(int iR = 0; iR < TMMCH_N_Readout; iR++){
    for(int bx = 1; bx <= hqmaxstrip_cal[iR]->GetNbinsX(); bx++){
      int strip = bx - 1;

      if (strip < 0 || strip >= maxStrip) continue;
      if (!pass3s[iR][strip]) continue;
      // Copy accepted strips for even-only calibrated TH2
      for(int by = 1; by <= hqmaxstrip_cal[iR]->GetNbinsY(); by++) {
        double c = hqmaxstrip_cal[iR]->GetBinContent(bx, by);
        double e = hqmaxstrip_cal[iR]->GetBinError(bx, by);

        hqmaxstrip_cal3s[iR]->SetBinContent(bx, by, c);
        hqmaxstrip_cal3s[iR]->SetBinError(bx, by, e);
      }

      // Copy accepted strips for full calibrated TH2
      for(int by = 1; by <= hqmaxstripFull_cal[iR]->GetNbinsY(); by++) {
        double c = hqmaxstripFull_cal[iR]->GetBinContent(bx, by);
        double e = hqmaxstripFull_cal[iR]->GetBinError(bx, by);

        hqmaxstripFull_cal3s[iR]->SetBinContent(bx, by, c);
        hqmaxstripFull_cal3s[iR]->SetBinError(bx, by, e);
      }
    }
    for(int ic = 0; ic < (int)Blockpass3s[iR].size(); ic++){
      for(int bx = 1; bx <= hBlockqmaxstrip_cal[iR][ic]->GetNbinsX(); bx++){
        int strip = bx - 1;

        if (strip < 0 || strip >= maxStrip) continue;
        if (!Blockpass3s[iR][ic][strip]) continue;
        // Copy accepted strips for even-only calibrated TH2
        for(int by = 1; by <= hBlockqmaxstrip_cal[iR][ic]->GetNbinsY(); by++) {
          double c = hBlockqmaxstrip_cal[iR][ic]->GetBinContent(bx, by);
          double e = hBlockqmaxstrip_cal[iR][ic]->GetBinError(bx, by);
          hBlockqmaxstrip_cal3s[iR][ic]->SetBinContent(bx, by, c);
          hBlockqmaxstrip_cal3s[iR][ic]->SetBinError(bx, by, e);

          double cp = hBlockqmaxstrip_Overallcal[iR][ic]->GetBinContent(bx, by);
          double ep = hBlockqmaxstrip_Overallcal[iR][ic]->GetBinError(bx, by);
          hBlockqmaxstrip_Overallcal3s[iR][ic]->SetBinContent(bx, by, cp);
          hBlockqmaxstrip_Overallcal3s[iR][ic]->SetBinError(bx, by, ep);
        }

        // Copy accepted strips for full calibrated TH2
        for(int by = 1; by <= hBlockqmaxstripFull_cal[iR][ic]->GetNbinsY(); by++) {
          double c = hBlockqmaxstripFull_cal[iR][ic]->GetBinContent(bx, by);
          double e = hBlockqmaxstripFull_cal[iR][ic]->GetBinError(bx, by);
          hBlockqmaxstripFull_cal3s[iR][ic]->SetBinContent(bx, by, c);
          hBlockqmaxstripFull_cal3s[iR][ic]->SetBinError(bx, by, e);

          double cp = hBlockqmaxstripFull_Overallcal[iR][ic]->GetBinContent(bx, by);
          double ep = hBlockqmaxstripFull_Overallcal[iR][ic]->GetBinError(bx, by);
          hBlockqmaxstripFull_Overallcal3s[iR][ic]->SetBinContent(bx, by, cp);
          hBlockqmaxstripFull_Overallcal3s[iR][ic]->SetBinError(bx, by, ep);
        }
      }
    }
  }

  // ------------------------------------------------------------
  // Global summary
  // ------------------------------------------------------------
  
  // UpdateSummaryTxt(
  //   "outputTMM/Monitor_TMM_summary.txt",
  //   RunID,
  //   q_global_beam[0], err_q_global_beam[0],
  //   q_global_beam[1], err_q_global_beam[1],
  //   x_global_spot[0], err_x_global_spot[0],
  //   x_global_spot[1], err_x_global_spot[1],
  //   x_global_rms[0], err_x_global_rms[0],
  //   x_global_rms[1], err_x_global_rms[1]
  // );

  blockTxt.close();

  // ------------------------------------------------------------
  // Write output ROOT file
  // ------------------------------------------------------------
  cout << "#### Creating output file " << outputFileName << endl;
  TFile *outfile = new TFile(outputFileName.Data(), "recreate");
  TDirectory *overalldir = outfile->mkdir("Overall");
  TDirectory *rawoveralldir = overalldir->mkdir("Raw");
  TDirectory *caloveralldir = overalldir->mkdir("Calib");
  TDirectory *blockdir = outfile->mkdir("Blocks");

  if (!outfile || outfile->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outputFileName << endl;
    if (outfile) delete outfile;
    return;
  }
  outfile->cd();
  overalldir->cd();
  rawoveralldir->cd();
  // cout << "DEBUG: Created output file " << outputFileName << endl;
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip[l]->Write();
    hqmaxstripFull[l]->Write();
    hAmpslice[l]->Write();
    hMeanslice[l]->Write();
    hSigmaslice[l]->Write();
    hChi2slice[l]->Write();
    hAmpsliceFull[l]->Write();
    hMeansliceFull[l]->Write();
    hSigmasliceFull[l]->Write();
    hChi2sliceFull[l]->Write();
    g_FitFullDiff[l]->Write();
    g_FitFullRatio[l]->Write();
    g_FitFullDiff3s[l]->Write();
    g_FitFullRatio3s[l]->Write();
    // f[l]->Write();
  }
  outfile->cd();
  overalldir->cd();  
  caloveralldir->cd();
  // cout << "DEBUG: Created output file " << outputFileName << endl;
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip_cal[l]->Write();
    hqmaxstripFull_cal[l]->Write();
    hqmaxstrip_cal3s[l]->Write();
    hqmaxstripFull_cal3s[l]->Write();
  }

  // Write block histograms in subdirectories
  outfile->cd();
  blockdir->cd();
  for (int l = 0; l < TMMCH_N_Readout; l++) {  
    g_BlockBeamSpot[l]->Write();
    g_BlockBeamSpread[l]->Write();
    g_BlockBeamCharge[l]->Write();
  }
  for (int b = 0; b < (int)g_BlockBeamSpot[0]->GetN(); b++) {
    TDirectory *thisblockdir = blockdir->mkdir(Form("block_%04d", b));
    TDirectory *rawblockdir = thisblockdir->mkdir("Raw");
    TDirectory *calblockdir = thisblockdir->mkdir("Calib");
    TDirectory *caltotblockdir = thisblockdir->mkdir("Calib_overall");
    thisblockdir->cd();

    for (int l = 0; l < TMMCH_N_Readout; l++) {
      thisblockdir->cd();
      rawblockdir->cd();
      if (b < (int)hBlockqmaxstrip[l].size()) {
        hBlockqmaxstrip[l][b]->Write();
        hBlockAmpslice[l][b]->Write();
        hBlockMeanslice[l][b]->Write();
        hBlockSigmaslice[l][b]->Write();
        hBlockChi2slice[l][b]->Write();
      }

      if (b < (int)hBlockqmaxstripFull[l].size()) {
        hBlockqmaxstripFull[l][b]->Write();
        hBlockAmpsliceFull[l][b]->Write();
        hBlockMeansliceFull[l][b]->Write();
        hBlockSigmasliceFull[l][b]->Write();
        hBlockChi2sliceFull[l][b]->Write();
        g_BlockFitFullDiff[l][b]->Write();
        g_BlockFitFullRatio[l][b]->Write();
        g_BlockFitFullDiff3s[l][b]->Write();
        g_BlockFitFullRatio3s[l][b]->Write();
      }
      thisblockdir->cd();
      calblockdir->cd();
      if (b < (int)hBlockqmaxstrip_cal[l].size()) {
        hBlockqmaxstrip_cal[l][b]->Write();
        hBlockqmaxstrip_cal3s[l][b]->Write();

        // hBlockAmpslice[l][b]->Write();
        // hBlockMeanslice[l][b]->Write();
        // hBlockSigmaslice[l][b]->Write();
        // hBlockChi2slice[l][b]->Write();
      }
      if (b < (int)hBlockqmaxstripFull_cal[l].size()) {
        hBlockqmaxstripFull_cal[l][b]->Write();
        hBlockqmaxstripFull_cal3s[l][b]->Write();

        // hBlockAmpslice[l][b]->Write();
        // hBlockMeanslice[l][b]->Write();
        // hBlockSigmaslice[l][b]->Write();
        // hBlockChi2slice[l][b]->Write();
      }
      thisblockdir->cd();
      caltotblockdir->cd();
      if (b < (int)hBlockqmaxstrip_Overallcal[l].size()) {
        hBlockqmaxstrip_Overallcal[l][b]->Write();
        hBlockqmaxstrip_Overallcal3s[l][b]->Write();

        // hBlockAmpslice[l][b]->Write();
        // hBlockMeanslice[l][b]->Write();
        // hBlockSigmaslice[l][b]->Write();
        // hBlockChi2slice[l][b]->Write();
      }
      if (b < (int)hBlockqmaxstripFull_Overallcal[l].size()) {
        hBlockqmaxstripFull_Overallcal[l][b]->Write();
        hBlockqmaxstripFull_Overallcal3s[l][b]->Write();

        // hBlockAmpslice[l][b]->Write();
        // hBlockMeanslice[l][b]->Write();
        // hBlockSigmaslice[l][b]->Write();
        // hBlockChi2slice[l][b]->Write();
      }
    }
    blockdir->cd();
  }
  outfile->cd();

  outfile->Write();
  // cout << "DEBUG: 'outfile->Write(); DONE'" << endl;
  outfile->Close();
  // cout << "DEBUG: 'outfile->Close(); DONE'" << endl;
  delete outfile;
  outfile = nullptr;
  // cout << "DEBUG: 'delete outfile; DONE'" << endl;

  cout << "Output written to " << outputFileName << endl;
}
