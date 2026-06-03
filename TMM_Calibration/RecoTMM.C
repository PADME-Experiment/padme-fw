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
    iStrip = maxStrip - iStrip;
  }
}

void RecoTMM::GlobalCoordinate(int iReadout, double &x_strip){ 
  // sistema di riferimento rispetto al centro della camera con padme reference frame
  // mancano traslazioni e rotazioni metrologia
  if(iReadout == 0) { //X
    x_strip = x_strip  + GLOBAL_X_TRANSLATION; 
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
  // da aggiungere qui correzione di strip calibration leggendo da txt con calibrazioni fatte
  x_strip = iStrip;
  float APV_q_corr = 1.;
  if(iReadout == 0) {
    if (iStrip < 122) APV_q_corr = APV1_fqx;
    if (iStrip > 122+113) APV_q_corr = APV3_fqx;
  }
  if(iReadout == 1) {
    if (iStrip < 122) APV_q_corr = APV1_fqy;
    if (iStrip > 122+113) APV_q_corr = APV3_fqy;
  }
  q_strip = qmax*APV_q_corr;
  
  GlobalCoordinate(iReadout, x_strip);
  // mappatura corretta delle strip con coordinate globali e carica corretta, dopo calibrazione apv
}

void RecoTMM::ComputeBeamSpotFromVectors(vector<double> &x_vec, vector<double> &q_vec, double &x_spot, double &x_rms, double &q_beam)
{

  if (x_vec.size() != q_vec.size()) {
    cerr << "ERROR: x_vec/q_vec size mismatch: " << x_vec.size() << " " << q_vec.size() << endl;
    q_beam = 0;
    x_spot = -999;
    x_rms = -999;
    return;
  }

  double sum_q = 0.;
  double sum_qx = 0.;
  double sum_qx2 = 0.;

  for(UInt_t h = 0; h < x_vec.size(); h++) {
    sum_q   += q_vec[h];
    sum_qx  += q_vec[h] * x_vec[h];
    sum_qx2 += q_vec[h] * x_vec[h] * x_vec[h];
  }

  if (sum_q <= 0) {
    q_beam = 0;
    x_spot = -999;
    x_rms = -999;
    return;
  }

  q_beam = sum_q;
  x_spot = sum_qx / sum_q;

  double variance = sum_qx2 / sum_q - x_spot * x_spot;

  if (variance < 0 && fabs(variance) < 1e-9) {
    variance = 0;
  }

  if (variance < 0) {
    x_rms = -999;
    return;
  }

  x_rms = sqrt(variance);

}

void RecoTMM::ComputeBeamStatsFromVectors(vector<double> &x_vec, vector<double> &q_vec, double &x_spot, double &err_x_spot, double &x_rms, double &err_x_rms, double &q_beam, double &err_q_beam)
{
  if (x_vec.size() != q_vec.size()) {
    cerr << "ERROR: x_vec/q_vec size mismatch: " << x_vec.size() << " " << q_vec.size() << endl;

    q_beam = 0;
    err_q_beam = 0;
    x_spot = -999;
    err_x_spot = 0;
    x_rms = -999;
    err_x_rms = 0;
    return;
  }

  const int n = x_vec.size();

  if (n == 0) {
    q_beam = 0;
    err_q_beam = 0;
    x_spot = -999;
    err_x_spot = 0;
    x_rms = -999;
    err_x_rms = 0;
    return;
  }

  double sum_q = 0.;
  double sum_q2 = 0.;
  double sum_qx = 0.;
  double sum_qx2 = 0.;

  for (int i = 0; i < n; i++) {
    const double q = q_vec[i];
    const double x = x_vec[i];

    sum_q   += q;
    sum_q2  += q*q;
    sum_qx  += q*x;
    sum_qx2 += q*x*x;
  }

  if (sum_q <= 0) {
    q_beam = 0;
    err_q_beam = 0;
    x_spot = -999;
    err_x_spot = 0;
    x_rms = -999;
    err_x_rms = 0;
    return;
  }

  q_beam = sum_q;

  // Approximate charge uncertainty.
  // This is not an electronics calibration uncertainty;
  // it is only a statistical proxy from the strip charges.
  err_q_beam = sqrt(sum_q2);

  x_spot = sum_qx / sum_q;

  double variance = sum_qx2 / sum_q - x_spot*x_spot;

  if (variance < 0 && fabs(variance) < 1e-9) {
    variance = 0;
  }

  if (variance < 0) {
    x_rms = -999;
    err_x_spot = 0;
    err_x_rms = 0;
    return;
  }

  x_rms = sqrt(variance);

  // Approximate errors.
  // Effective number of hits, charge-weighted.
  double n_eff = 0.;
  if (sum_q2 > 0) {
    n_eff = (sum_q * sum_q) / sum_q2;
  }

  if (n_eff > 1) {
    err_x_spot = x_rms / sqrt(n_eff);
    err_x_rms  = x_rms / sqrt(2. * (n_eff - 1.));
  } else {
    err_x_spot = 0;
    err_x_rms = 0;
  }
}

void RecoTMM::ComputeBeamSpot(int iReadout, double &x_spot, double &x_rms, double &q_beam)
{
  ComputeBeamSpotFromVectors(x_mean[iReadout], q_mean[iReadout], x_spot, x_rms, q_beam);
}

void RecoTMM::UpdateSummaryTxt(TString filename, int RunID, double qx, double err_qx, double qy, double err_qy, double x, double err_x, double y, double err_y, double sigmax, double err_sigmax, double sigmay, double err_sigmay)
{
  vector<TString> lines;
  bool foundRun = false;

  ifstream infile(filename.Data());

  if (infile.is_open()) {
    string line;

    while (getline(infile, line)) {
      TString tline(line);

      if (tline.BeginsWith("#")) {
        lines.push_back(tline);
        continue;
      }

      stringstream ss(line);
      int existingRun = -1;
      ss >> existingRun;

      if (existingRun == RunID) {
        TString newline = Form("%d %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g",
                              RunID,
                              qx, err_qx, qy, err_qy,
                              x, err_x, y, err_y,
                              sigmax, err_sigmax, sigmay, err_sigmay);

        lines.push_back(newline);
        foundRun = true;
      } else {
        lines.push_back(tline);
      }
    }

    infile.close();
  }

  if (!foundRun) {
    TString newline = Form("%d %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g %.10g",
                          RunID,
                          qx, err_qx, qy, err_qy,
                          x, err_x, y, err_y,
                          sigmax, err_sigmax, sigmay, err_sigmay);

    lines.push_back(newline);
  }

  ofstream summaryTxt(filename.Data());

  summaryTxt << "# Global TMM summary\n";
  summaryTxt << "# RunNumber qx err_qx qy err_qy x err_x y err_y sigmax err_sigmax sigmay err_sigmay\n";

  for (auto &line : lines) {
    if (line.BeginsWith("#")) continue;
    summaryTxt << line << endl;
  }

  summaryTxt.close();
}

void RecoTMM::LoopFileList(TObjArray &inputFileNameList) {

  // cout << "DEBUG LoopFileList: start" << endl;

  if (inputFileNameList.GetEntries() == 0) {
    cerr << "ERROR: empty input file list" << endl;
    return;
  }

  cout << "Number of input files: " << inputFileNameList.GetEntries() << endl;

  if (maxEvents > 0) {
    cout << "N entries requested: " << maxEvents << endl;
  } else {
    cout << "N entries requested: all available entries" << endl;
  }

  // ------------------------------------------------------------
  // Histograms and graphs: created once, before creating TChain and looping on events
  // ------------------------------------------------------------
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    //event by event th1
    hqmaxstrip[l] = new TH2F(Form("hqmaxstrip%d", l), TString("q_{max} vs x_{strip} [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstrip[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip[l]->SetYTitle("q_{max} [ADC counts]");

    hqmaxstripFull[l] = new TH2F(Form("hqmaxstripFull%d", l), TString("q_{max} vs x_{strip} full [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstripFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull[l]->SetYTitle("q_{max} [ADC counts]");

    hBeam[l] = new TH1F(Form("hBeam%d", l), TString("x_{strip} charge weighted [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hBeam[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hBeam[l]->SetYTitle("# entries");

    hBeamFull[l] = new TH1F(Form("hBeamFull%d", l), TString("x_{strip} charge weighted full [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hBeamFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hBeamFull[l]->SetYTitle("# entries");

    // FitSlicesY() histograms
    hMeanslice[l] = new TH1D(Form("hMeanslice%d", l), TString("mean value slice distribution (even strips only) d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hMeanslice[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hMeanslice[l]->SetYTitle("q_{max} [ADC counts]");
    
    hMeansliceFull[l] = new TH1D(Form("hMeansliceFull%d", l), TString("mean value slice distribution d [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hMeansliceFull[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hMeansliceFull[l]->SetYTitle("q_{max} [ADC counts]");

    //calibrated plots
    hqmaxstrip_cal[l] = new TH2F(Form("hqmaxstrip_cal%d", l), TString("q_{max-calib} vs x_{strip} [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstrip_cal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstrip_cal[l]->SetYTitle("q_{max} calib [ADC counts]");

    hqmaxstripFull_cal[l] = new TH2F(Form("hqmaxstripFull_cal%d", l), TString("q_{max-calib} vs x_{strip} full [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5, 1000, 0, 2500);
    hqmaxstripFull_cal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hqmaxstripFull_cal[l]->SetYTitle("q_{max} calib [ADC counts]");

    hBeam_cal[l] = new TH1F(Form("hBeam_cal%d", l), TString("x_{strip-calib} charge weighted [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hBeam_cal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hBeam_cal[l]->SetYTitle("# entries");

    hBeamFull_cal[l] = new TH1F(Form("hBeamFull_cal%d", l), TString("x_{strip-calib} charge weighted full [") + tmm_tag[l] + TString("]"), maxStrip, -0.5, maxStrip-0.5);
    hBeamFull_cal[l]->SetXTitle(TString(tmm_tag[l]+" [mm]").Data());
    hBeamFull_cal[l]->SetYTitle("# entries");

    // event by event tgrapherrors
    g_BeamSpot[l] = new TGraphErrors();
    TGraphAttribute(g_BeamSpot[l], Form("g_BeamSpot_%s", tmm_tag[l].Data()), "entry", "x_{Beam} [mm]", 20, kBlue + l);

    g_BeamSpread[l] = new TGraphErrors();
    TGraphAttribute(g_BeamSpread[l], Form("g_BeamSpread_%s", tmm_tag[l].Data()), "entry", "rms_{Beam} [mm]", 21, kRed + l);

    g_BeamCharge[l] = new TGraphErrors();
    TGraphAttribute(g_BeamCharge[l], Form("g_BeamCharge_%s", tmm_tag[l].Data()), "entry", "q_{Beam} [ADC counts]", 22, kGreen + 2 + l);
    
    //TGraph for strip calibration and equalization
    g_FitFullDiff[l] = new TGraphErrors();
    TGraphAttribute(g_FitFullDiff[l], Form("g_FitFullDiff%s", tmm_tag[l].Data()), "strip", "Q_{maxbin} - EvalFit [ADC counts]", 22, kBlack);

    g_FitFullRatio[l] = new TGraphErrors();
    TGraphAttribute(g_FitFullRatio[l], Form("g_FitFullRatio%s", tmm_tag[l].Data()), "strip", "Q_{maxbin} / EvalFit", 22, kBlack);
  }

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
  vector<double> x_global[TMMCH_N_Readout];
  vector<double> q_global[TMMCH_N_Readout];
  Long64_t globalEntry = 0;

  for (Long64_t iev = 0; iev < nToProcess; ++iev) {

    Long64_t nb = fTree->GetEntry(iev);    
    if (nb <= 0) {
      cerr << "WARNING: could not read event " << iev << endl;
      continue;
    }

    if (globalEntry % 1000 == 0) {
      float progress = static_cast<float>(globalEntry) / nToProcess;
      cout << "Processed " << globalEntry << " out of " << nToProcess << " entries (" << fixed << setprecision(2) << progress * 100 << "%)" << endl;
    }

    for (int l = 0; l < TMMCH_N_Readout; l++) {
      x_mean[l].clear();
      q_mean[l].clear();
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

      x_mean[iReadout_new].push_back(x_strip);
      q_mean[iReadout_new].push_back(q_strip);

      x_global[iReadout_new].push_back(x_strip);
      q_global[iReadout_new].push_back(q_strip);

      if (channel % 2 == 0) {
        hqmaxstrip[iReadout_new]->Fill(x_strip, q_strip);
        hBeam[iReadout_new]->Fill(x_strip, q_strip);
      }

      hqmaxstripFull[iReadout_new]->Fill(x_strip, q_strip);
      hBeamFull[iReadout_new]->Fill(x_strip, q_strip);
      
    }

    for (int r = 0; r < TMMCH_N_Readout; r++) {

      ComputeBeamSpot(r, x_spot[r], rms[r], q_beam[r]);

      g_BeamSpot[r]->SetPoint(globalEntry, globalEntry, x_spot[r]);
      g_BeamSpot[r]->SetPointError(globalEntry, 0, err_x_spot[r]);

      g_BeamSpread[r]->SetPoint(globalEntry, globalEntry, rms[r]);
      g_BeamSpread[r]->SetPointError(globalEntry, 0, err_x_rms[r]);

      g_BeamCharge[r]->SetPoint(globalEntry, globalEntry, q_beam[r]);
      g_BeamCharge[r]->SetPointError(globalEntry, 0, err_q_beam[r]);
    }

    globalEntry++;
  }

  cout << "#### Total reconstructed events: " << globalEntry << endl;
  cout << "################################ " << endl;
  cout << "#### Slice fit procedure to equalise channels + baseline removal" << endl;

  for(int iR = 0; iR<TMMCH_N_Readout;iR++){
    // TF1 *f2d = new TF1(Form("f2d_%d", iR), "gaus", 50, 250);
    // hqmaxstrip[iR]->FitSlicesY(f2d, 0, 2500, 0, "RQ", nullptr);
    hqmaxstrip[iR]->FitSlicesY();
    // cout << "DEBUG: fit slice DONE on hqmaxstrip" << endl;
    TH1D *h_muslice = (TH1D*)gDirectory->Get(Form("hqmaxstrip%i_1",iR));
    hMeanslice[iR] = (TH1D*)h_muslice->Clone();
    // if(iR==0) h_muslice->Rebin(2);
    if(iR==1) {
      double c;
      for(int i = 1; i< h_muslice->GetNbinsX(); i++){
        c = 1;
        if(i>99 && i<272){
          if(i%10==0 || (i-1)%10==0) c = 1.3;
          h_muslice->SetBinContent(i, h_muslice->GetBinContent(i)*c);
        }
      }
    }
    // TF1 *f2dFull = new TF1(Form("f2d_%d", iR), "gaus", 0, 2500);
    // hqmaxstripFull[iR]->FitSlicesY(f2dFull, 0, 2500 , 0, "RQ", nullptr);
    hqmaxstripFull[iR]->FitSlicesY();
    // cout << "DEBUG: fit slice DONE on hqmaxstripFull" << endl;
    TH1D *h_musliceFull = (TH1D*)gDirectory->Get(Form("hqmaxstripFull%i_1",iR));
    hMeansliceFull[iR] = (TH1D*)h_musliceFull->Clone();
    // cout << "DEBUG: hqmaxstripFull[iR]_1 accepted" << endl;

    // cout << "DEBUG: hqmaxstrip[iR]_1 accepted" << endl;
    double xmin = 40;
    double xmax = 320;
    if(iR == 1){
      xmin = 20;
      xmax = 340;
    }
    int bmax = h_muslice->GetMaximumBin();
    double A0 = h_muslice->GetBinContent(bmax);
    double mu0 = h_muslice->GetBinCenter(bmax);
    double rms0 = h_muslice->GetRMS();
    if (rms0 <= 0) rms0 = 10.0;

    f.push_back(new TF1(Form("f_%d", iR), "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[1])/[4])^2)", xmin, xmax));    
    // first gaussian: core - second gaussian: wider tail
    // if(iR == 0){
    f.at(iR)->SetParameters(A0/2, mu0, rms0, A0*2, rms0/2); 
    // } else {
    //   f.at(iR)->SetParameters(A0/2, mu0, rms0, A0*2, rms0/2); 
    // }
    f.at(iR)->SetParNames("A1", "mean", "sigma1", "A2", "sigma2");

    // constrain on parameter limits
    f.at(iR)->SetParLimits(0, 0.0, 10.0 * A0); // amplitudes
    f.at(iR)->SetParLimits(3, 0.0, 10.0 * A0);
    f.at(iR)->SetParLimits(1, xmin, xmax); // common mean value
    f.at(iR)->SetParLimits(2, rms0, 5.0 * (xmax - xmin)); // sigma core 
    f.at(iR)->SetParLimits(4, rms0, (xmax - xmin)); // sigma tails

    // fit
    h_muslice->Fit(f.at(iR), "R");
    // cout << "DEBUG: fit on each slice done" << endl;
    double d = 0;
    double r = 0;

    cout << "CHECK: dimension of each histogram -> hqmaxstrip[iR] = " << hqmaxstrip[iR]->GetNbinsX() << " - hqmaxstripFull[iR] = " << hqmaxstripFull[iR]->GetNbinsX() << endl;
    cout << "CHECK: dimension of each histogram -> h_muslice = " << h_muslice->GetNbinsX() << " - h_musliceFull = " << h_musliceFull->GetNbinsX() << endl;

    for(int iF = 1; iF <= hqmaxstripFull[iR]->GetNbinsX(); iF++){
      double yFit = f.at(iR)->Eval(h_musliceFull->GetBinCenter(iF));
      d = h_musliceFull->GetBinContent(iF) - yFit;
      // cout << "DEBUG: d=" << d << endl;
      r = (h_musliceFull->GetBinContent(iF) != 0 ?  yFit / h_musliceFull->GetBinContent(iF) : 0);
      // cout << "DEBUG: r=" << r << endl;
      FitFullDiff[iR].push_back(d);
      FitFullRatio[iR].push_back(r);

      g_FitFullDiff[iR]->SetPoint(iF, iF-1, d);
      g_FitFullDiff[iR]->SetPointError(iF, 0, 0); // -2 per corrispondenza con histo della carica vs strip

      g_FitFullRatio[iR]->SetPoint(iF, iF-1, r);
      g_FitFullRatio[iR]->SetPointError(iF, 0, 0);
      d = 0;
      r = 0;
    }
    // delete f2d;
  }

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
        hBeam_cal[iReadout_new]->Fill(x_strip, q_strip*chargecorrection);
      }
      hqmaxstripFull_cal[iReadout_new]->Fill(x_strip, q_strip*chargecorrection);
      hBeamFull_cal[iReadout_new]->Fill(x_strip, q_strip*chargecorrection);
    }

  }

  // ------------------------------------------------------------
  // Global summary
  // ------------------------------------------------------------
  double x_global_spot[TMMCH_N_Readout];
  double err_x_global_spot[TMMCH_N_Readout];

  double x_global_rms[TMMCH_N_Readout];
  double err_x_global_rms[TMMCH_N_Readout];

  double q_global_beam[TMMCH_N_Readout];
  double err_q_global_beam[TMMCH_N_Readout];

  for (int r = 0; r < TMMCH_N_Readout; r++) {
    ComputeBeamStatsFromVectors(
      x_global[r],
      q_global[r],
      x_global_spot[r],
      err_x_global_spot[r],
      x_global_rms[r],
      err_x_global_rms[r],
      q_global_beam[r],
      err_q_global_beam[r]
    );
  }

  // cout << "DEBUG: Finished computing global beam stats" << endl;
  UpdateSummaryTxt(
    "outputTMM/Calibration_TMM_summary.txt",
    RunID,
    q_global_beam[0], err_q_global_beam[0],
    q_global_beam[1], err_q_global_beam[1],
    x_global_spot[0], err_x_global_spot[0],
    x_global_spot[1], err_x_global_spot[1],
    x_global_rms[0], err_x_global_rms[0],
    x_global_rms[1], err_x_global_rms[1]
  );

  // cout << "DEBUG: Finished updating summary text file" << endl;

  // ------------------------------------------------------------
  // Write output ROOT file
  // ------------------------------------------------------------
  cout << "#### Creating output file " << outputFileName << endl;
  TFile *outfile = new TFile(outputFileName.Data(), "recreate");
  TDirectory *rawdir = outfile->mkdir("Raw");
  TDirectory *caldir = outfile->mkdir("Calib");

  if (!outfile || outfile->IsZombie()) {
    cerr << "ERROR: cannot create output file " << outputFileName << endl;
    if (outfile) delete outfile;
    return;
  }
  outfile->cd();
  rawdir->cd();
  // cout << "DEBUG: Created output file " << outputFileName << endl;
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip[l]->Write();
    hBeam[l]->Write();
    hqmaxstripFull[l]->Write();
    hBeamFull[l]->Write();
    g_BeamSpot[l]->Write();
    g_BeamSpread[l]->Write();
    g_BeamCharge[l]->Write();
    hMeanslice[l]->Write();
    hMeansliceFull[l]->Write();
    g_FitFullDiff[l]->Write();
    g_FitFullRatio[l]->Write();
    f[l]->Write();
  }
  outfile->cd();
  caldir->cd();
  // cout << "DEBUG: Created output file " << outputFileName << endl;
  for (int l = 0; l < TMMCH_N_Readout; l++) {
    hqmaxstrip_cal[l]->Write();
    hBeam_cal[l]->Write();
    hqmaxstripFull_cal[l]->Write();
    hBeamFull_cal[l]->Write();
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
