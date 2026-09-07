#define Reco_cxx
#include "Reco.h"

using namespace std;

bool DOPLOT=false;

void Reco::CoordinateFinder(int iStrip,
                            int iLayer,
                            const std::vector<short>& camp,
                            double &t_strip,
                            double &x_strip,
                            double &z_strip,
                            double &q_strip)
{
   const int Nbins = camp.size();
   
   double qtot = 0.;
   double qmax = -1e9;
   
   for (int ibin = 0; ibin < Nbins; ibin++) {
     double q = static_cast<double>(camp[ibin]);
     qtot += q;
     if (q > qmax)
       qmax = q;
   }
   
   const double threshold = 0.2 * qtot / Nbins;
   
   double weightedTime = 0.;
   double qtotT = 0.;
   
   for (int ibin = 0; ibin < Nbins; ibin++) {
     
     double qbin = static_cast<double>(camp[ibin]);
     if (qbin < threshold) continue;
     
     double tbin = ibin*clock + clock/2.;
     
     weightedTime += qbin * tbin;
     qtotT += qbin;
   }
   
   if (qtotT > 0)
     t_strip = weightedTime / qtotT;
   else
     t_strip = 0.;
   
   if(iStrip <= 256)
     x_strip = (iStrip-256.) * pitch + pitch / 2. + shift_coord[iLayer] - geo_hole[iLayer]/2. ;
   else
     x_strip = (iStrip-256.) * pitch + pitch / 2. + shift_coord[iLayer] + geo_hole[iLayer]/2.;
   
   const double t0 = 0.;
   const double z_ion = 2.;
   
   if(iLayer > 3)
     z_strip = zm + z_ion - (t_strip - t0)*vd;
   else
     z_strip = (t_strip - t0)*vd - zm - z_ion;
   
   q_strip = qmax;
}

void Reco::FitBeamBlock(TH1F* h_block, int blockID, TH1F* h_mu_vs_time, TH1F* h_sigma_vs_time, TH1F* h_charge_vs_time)
{
  if(h_block->GetEntries() < 10) return;

  double xmin = h_block->GetXaxis()->GetXmin();
  double xmax = h_block->GetXaxis()->GetXmax();
   TF1* gausFit = new TF1("gausFit","gaus",xmin,xmax);
   
   h_block->Fit(gausFit,"RQ");
   
   double mu    = gausFit->GetParameter(1);
   double sigma = gausFit->GetParameter(2);
   double chi2  = gausFit->GetChisquare();
   int ndf      = gausFit->GetNDF();
   
   double integral = gausFit->Integral(xmin, xmax);

   cout<<"charge block: "<<integral<<endl;
   h_mu_vs_time->SetBinContent(blockID+1, mu);
   h_sigma_vs_time->SetBinContent(blockID+1, sigma);
   h_charge_vs_time->SetBinContent(blockID+1, integral);
   
   // ===== CANVAS =====
   if(DOPLOT==true){
     TCanvas* c = new TCanvas(Form("c_beam_block1D_%d",blockID),
			      Form("Beam fit block 1D %d",blockID),
			      400,400);
     
     c->cd(1);
     
     h_block->SetTitle(Form("Beam X 1D - block %d",blockID));
     h_block->GetXaxis()->SetTitle("X[mm]");
     h_block->Draw();
     gausFit->Draw("same");
     
     TPaveText* box = new TPaveText(0.35,0.15,0.65,0.30,"NDC");
     
     box->SetFillStyle(0);   // trasparente
     box->SetBorderSize(0);  // niente bordo
     box->SetShadowColor(0); // niente ombra
     box->SetTextAlign(22);  // testo centrato
     
     box->AddText(Form("#mu = %.2f",mu));
     box->AddText(Form("#sigma = %.2f",sigma));
     box->AddText(Form("#chi^{2}/ndf = %.1f / %d",chi2,ndf));
     box->SetFillColor(0);
     box->Draw();
     gSystem->mkdir("plots/Blocks1D",true);
     c->SaveAs(Form("plots/Blocks1D/BeamFit_block1D_%d.png",blockID));

   }
   delete gausFit;
}

void Reco::FitBeamBlock2D(TH2F* h_block, int blockID, TH1F* h_muX_vs_time, TH1F* h_sigmaX_vs_time, TH1F* h_muY_vs_time, TH1F* h_sigmaY_vs_time)
{
  if(h_block->GetEntries() < 10) return;
  
  // Proiezioni per X e Y
  TH1D* h_projX = h_block->ProjectionX("h_projX");
  TH1D* h_projY = h_block->ProjectionY("h_projY");
  
  // Fit gaussiano X
  TF1* gausX = new TF1("gausX","gaus", h_projX->GetXaxis()->GetXmin(), h_projX->GetXaxis()->GetXmax());
  h_projX->Fit(gausX,"RQ");
  double muX       = gausX->GetParameter(1);
  double sigmaX    = gausX->GetParameter(2);
  double errmuX    = gausX->GetParError(1);
  double errsigmaX = gausX->GetParError(2);
  double chi2X     = gausX->GetChisquare();
  int ndfX         = gausX->GetNDF();
  
  h_muX_vs_time->SetBinContent(blockID+1, muX);
  h_sigmaX_vs_time->SetBinContent(blockID+1, sigmaX);
  h_muX_vs_time->SetBinError(blockID+1, errmuX);
  h_sigmaX_vs_time->SetBinError(blockID+1, errsigmaX);
  
  // Fit gaussiano Y
  TF1* gausY = new TF1("gausY","gaus", h_projY->GetXaxis()->GetXmin(), h_projY->GetXaxis()->GetXmax());
  h_projY->Fit(gausY,"RQ");
  
  double muY       = gausY->GetParameter(1);
  double sigmaY    = gausY->GetParameter(2);
  double errmuY    = gausY->GetParError(1);
  double errsigmaY = gausY->GetParError(2);
  double chi2Y     = gausY->GetChisquare();
  int ndfY         = gausY->GetNDF();
  
  h_muY_vs_time->SetBinContent(blockID+1, muY);
  h_sigmaY_vs_time->SetBinContent(blockID+1, sigmaY);
  h_muY_vs_time->SetBinError(blockID+1, errmuY);
  h_sigmaY_vs_time->SetBinError(blockID+1, errsigmaY);
  
  // ===== CANVAS =====
  if(DOPLOT==true){
    TCanvas* c = new TCanvas(Form("c_beam_block_%d",blockID), Form("Beam fit block %d",blockID), 900,400);
    c->Divide(2,1);
    
    // ===== X PLOT =====
    c->cd(1);
    
    h_projX->SetTitle(Form("Beam X projection - block %d",blockID));
    h_projX->GetXaxis()->SetTitle("X [mm]");
    h_projX->Draw();
    gausX->Draw("same");
    
    TPaveText* boxX = new TPaveText(0.35,0.15,0.65,0.30,"NDC");
    
    boxX->SetFillStyle(0);   // trasparente
    boxX->SetBorderSize(0);  // niente bordo
    boxX->SetShadowColor(0); // niente ombra
    boxX->SetTextAlign(22);  // testo centrato
    
    boxX->AddText(Form("#mu = %.2f",muX));
    boxX->AddText(Form("#sigma = %.2f",sigmaX));
    boxX->AddText(Form("#chi^{2}/ndf = %.1f / %d",chi2X,ndfX));
    boxX->SetFillColor(0);
    boxX->Draw();
    
    // ===== Y PLOT =====
    c->cd(2);
    
    h_projY->SetTitle(Form("Beam Y projection - block %d",blockID));
    h_projY->GetXaxis()->SetTitle("Y [mm]");
    h_projY->Draw();
    gausY->Draw("same");
    
    
    TPaveText* boxY = new TPaveText(0.35,0.15,0.65,0.30,"NDC");
    
    boxY->SetFillStyle(0);   // trasparente
    boxY->SetBorderSize(0);  // niente bordo
    boxY->SetShadowColor(0); // niente ombra
    boxY->SetTextAlign(22);  // testo centrato
    
    boxY->AddText(Form("#mu = %.2f",muY));
    boxY->AddText(Form("#sigma = %.2f",sigmaY));
    boxY->AddText(Form("#chi^{2}/ndf = %.1f / %d",chi2Y,ndfY));
    boxY->SetFillColor(0);
    boxY->Draw();
    
    // ===== SALVATAGGIO =====
    gSystem->mkdir("plots/Blocks2D",true);
    c->SaveAs(Form("plots/Blocks2D/BeamFit_block2D_%d.png",blockID));
  
    delete c;
  }
  delete gausX;
  delete gausY;
  delete h_projX;
  delete h_projY;
}


TH1F* Reco::ExtractChargePeakPerStrip(TH2F* h_charge_pos_2D, int blockID, double xmin, double xmax)
{
  
  int nBinsX = h_charge_pos_2D->GetNbinsX();
  
  TH1F* h_peak = new TH1F(Form("h_charge_peak_block_%d",blockID), "Peak charge per strip", nBinsX, h_charge_pos_2D->GetXaxis()->GetXmin(), h_charge_pos_2D->GetXaxis()->GetXmax());
  
  // conversione coordinate → bin
  int firstBin = h_charge_pos_2D->GetXaxis()->FindBin(xmin);
  int lastBin  = h_charge_pos_2D->GetXaxis()->FindBin(xmax);
  
  int nSlices  = lastBin-firstBin+1;
  
  //int padCounter = 1;
  //cout<<"first: "<<firstBin<<" last: "<<lastBin<<endl;
  for(int ix=firstBin; ix<=lastBin; ix++){
    TH1D* slice = h_charge_pos_2D->ProjectionY(Form("slice_block%d_x%d",blockID,ix), ix, ix+1);
    //cout<<slice->GetEntries()<<endl;
    if(slice->GetEntries() < 1){
      delete slice;
      continue;
    }
      
    double mean = slice->GetMean();
    double rms  = slice->GetRMS();
      
    TF1* lor = new TF1(Form("lor_block%d_x%d",blockID,ix), "[0]/(1+((x-[1])/[2])^2)", mean-2*rms, mean+2*rms);
    lor->SetParameters(slice->GetMaximum(), mean, rms);
      
    //slice->Fit(lor,"RQ");
    
    double peak = lor->GetParameter(1);
    
    h_peak->SetBinContent(ix, peak);
    
    // ===== PLOT =====
    if(DOPLOT==true){   
      TCanvas* cSlices = new TCanvas(Form("cSlices_block_%d_x%d",blockID,ix), Form("Lorentz fits block %d slice %d",blockID,ix), 1400,800);
      cSlices->cd();
      slice->SetTitle(Form("Strip %.0f", h_charge_pos_2D->GetXaxis()->GetBinCenter(ix)));
      
      slice->GetXaxis()->SetTitle("Charge");
      slice->Draw();
      //lor->Draw("same");
      
      gSystem->mkdir("plots/ChargeFit",true);
      cSlices->SaveAs(Form("plots/ChargeFit/LorentzFits_block_%d_x%d.png",blockID,ix));
      delete lor;
      delete slice;
      delete cSlices;
    }
  }
  if(DOPLOT==true){
    TCanvas* c2D = new TCanvas("c2D","c2D",900,900);
    c2D->cd();
    h_charge_pos_2D->Draw("ZCOL");
    c2D->SaveAs("plots/ChargeFit/Charge_vs_Position.png");
    //delete cSlices;
  }
  return h_peak;
}

void Reco::Loop(int RunID, int DetRunID)
{
  if (!fChain) return;
  
  gStyle->SetOptStat(0);
  
  const int nLayers = 8;
  
  Long64_t totalentries = fChain->GetEntries();
  Long64_t nentries = 100000;

  fChain->ls();
  std::cout << "Total entries = " <<totalentries<< std::endl;
  cout << "Total events: " << nentries << endl;
  
  /// ================================
  ///  Create output ROOT file
  /// ================================
  gSystem->mkdir(Form("plots_%d", RunID));
  TFile *outfile = new TFile(Form("plots_%d/output_histos.root", RunID), "RECREATE");
  
  /// ================================
  ///  Histogram containers
  /// ================================
  std::vector<TH1F*> h_occupancy(nLayers);
  std::vector<TH1F*> hw_occupancy(nLayers);
  std::vector<TH1F*> hqmax(nLayers);
  std::vector<TH1F*> htmax(nLayers);
  
  for(int l=0; l<nLayers; l++) {
    
    h_occupancy[l] = new TH1F(Form("h_occ_L%d",l), Form("Occupancy Layer %d",l), maxStrip,0,maxStrip);
    hw_occupancy[l] = new TH1F(Form("h_occW_L%d",l), Form("Weighted Occupancy Layer %d",l), maxStrip,0,maxStrip);
    hqmax[l] = new TH1F(Form("h_Qmax_L%d",l), Form("Qmax Layer %d",l), 1000,10,2500);
    htmax[l] = new TH1F(Form("h_Tmax_L%d",l), Form("Time Layer %d",l), 100,0,700);
  
  }
  
  
  ////INPUTS FOR BEAM FIT////
  const double beamMin = -7.;//235
  const double beamMax = 12.;//265
  const double beamMinY = -25.;//220
  const double beamMaxY = 2.;//250
  const int eventsPerBlock = 10000;
   
  int nBlocks = nentries / eventsPerBlock;
  
  TH1F* h_beamX_block   = new TH1F("h_beamX_block", "Beam X block profile;Strip;Weighted occ", beamMax-beamMin, beamMin, beamMax);
  TH1F* h_beamY_block   = new TH1F("h_beamY_block", "Beam Y block profile;Strip;Weighted occ", beamMax-beamMin, beamMin, beamMax);
  TH2F* h_beam_block_2D = new TH2F("h_beam_block_2D", "Beam XY per block", beamMax-beamMin+1, beamMin, beamMax, beamMaxY-beamMinY+1, beamMinY, beamMaxY); // X = layer3 Y = layer0
  TH1F* h_muX1D_vs_time = new TH1F("h_muX1D_vs_time", "X Beam position 1D vs block;Block;#mu (mm)", nBlocks,0,nBlocks);
  TH1F* h_sigmaX1D_vs_time = new TH1F("h_sigmaX1D_vs_time", "X Beam sigma 1D vs block;Block;#sigma (mm)", nBlocks,0,nBlocks);
  TH1F* h_muY1D_vs_time = new TH1F("h_muY1D_vs_time", "Y Beam position 1D vs block;Block;#mu (mm)", nBlocks,0,nBlocks);
  TH1F* h_sigmaY1D_vs_time = new TH1F("h_sigmaY1D_vs_time", "Y Beam sigma 1D vs block;Block;#sigma (mm)", nBlocks,0,nBlocks);
  TH1F* h_muC_vs_time = new TH1F("h_muC_vs_time", "X Beam position Charge vs block;Block;#mu (mm)", nBlocks,0,nBlocks);
  TH1F* h_sigmaC_vs_time = new TH1F("h_sigmaC_vs_time", "X Beam sigma Charge vs block;Block;#sigma (mm)", nBlocks,0,nBlocks);
  TH1F* h_muX_vs_time = new TH1F("h_muX_vs_time", "X Beam position vs block;Block;#mu (mm)", nBlocks,0,nBlocks);
  TH1F* h_sigmaX_vs_time = new TH1F("h_sigmaX_vs_time", "X Beam sigma vs block;Block;#sigma (mm)", nBlocks,0,nBlocks);
  TH1F* h_muY_vs_time = new TH1F("h_muY_vs_time", "Y Beam position vs block;Block;#mu (mm)", nBlocks,0,nBlocks);
  TH1F* h_sigmaY_vs_time = new TH1F("h_sigmaY_vs_time", "Y Beam sigma vs block;Block;#sigma (mm)", nBlocks,0,nBlocks);
  TH1F* h_chargeX_vs_time = new TH1F("h_chargeX_vs_time", "X Beam charge vs block;Block;#sigma (mm)", nBlocks,0,nBlocks);
  TH1F* h_chargeY_vs_time = new TH1F("h_chargeY_vs_time", "Y Beam charge vs block;Block;#sigma (mm)", nBlocks,0,nBlocks);
    // TH2F* h_occupancy_2D = new TH2F("h_occupancy_2D",
    //				   "Occupancy 2D Beam X vs Y",
    //				   beamMax-beamMin+1, beamMin, beamMax,   // X = layer3
    //				   beamMaxY-beamMinY+1, beamMinY, beamMaxY); // Y = layer0  
  TH2F* h_occupancy_2D = new TH2F("h_occupancy_2D", "Occupancy 2D Beam X vs Y", maxStrip,-maxStrip/2.*pitch+shift_x,maxStrip/2.*pitch+shift_x, maxStrip,-maxStrip/2.*pitch+shift_y,maxStrip/2.*pitch+shift_y);  // X = layer3  Y = layer0
  TH2F* h_time_pos_2D_X = new TH2F("h_time_pos_2D_X", "Time vs Position X", maxStrip,-maxStrip/2.*pitch+shift_x,maxStrip/2.*pitch+shift_x, 700.,0.,700.);  // X = layer3
  TH2F* h_time_pos_2D_Y = new TH2F("h_time_pos_2D_Y", "Time vs Position Y", maxStrip,-maxStrip/2.*pitch+shift_y,maxStrip/2.*pitch+shift_y, 700.,0.,700.);  // Y = layer0
  TH2F* h_beam_block_PosCharge_X = new TH2F("h_beam_block_PosCharge_X", "Charge vs Position X", maxStrip,-maxStrip/2.*pitch+shift_x,maxStrip/2.*pitch+shift_x, 1000.,10.,2500.); // X = layer3
  TH2F* h_beam_block_PosCharge_Y = new TH2F("h_beam_block_PosCharge_Y", "Charge vs Position Y", maxStrip,-maxStrip/2.*pitch+shift_x,maxStrip/2.*pitch+shift_x, 1000.,10.,2500.); // X = layer3
  
  int currentBlock = 0;
  int eventCounter = 0;
  vector<double> beamX_event;
  vector<double> beamQX_event;
  vector<double> beamY_event;
  vector<double> beamQY_event;   
    
  /// ================================
  ///  EVENT LOOP
  /// ================================
  
  for (Long64_t jentry=0; jentry<nentries; jentry++) {
    
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    
    fChain->GetEntry(jentry);
    
    if (jentry % 5000 == 0) {
      cout << "Processed " << jentry << " / " << nentries
          << " (" << std::fixed << std::setprecision(1) << 100.*jentry/nentries << "%)"
          << endl;
      }
      
    const int nStrips = mmLayer->size();

    for(int j=0; j<nStrips; j++) {
	
      int layer = mmLayer->at(j);
      if(layer < 0 || layer >= nLayers) continue;

      const std::vector<short>& camp = raw_q->at(j);
      int channel = mmStrip->at(j);
      
      double t_strip=0., x_strip=0., z_strip=0., q_strip=0.;
      
      CoordinateFinder(channel, layer, camp, t_strip, x_strip, z_strip, q_strip);
    
      /// Fill cumulative histograms
      
      hqmax[layer]->Fill(q_strip);
      htmax[layer]->Fill(t_strip);
      
      h_occupancy[layer]->Fill(channel, 1./nentries);
      hw_occupancy[layer]->Fill(channel, q_strip);
    
      //if(layer == 0 && channel >= beamMin && channel <= beamMax) {
      //  h_beam_block->Fill(channel, q_strip);
      //}
	
	    if((layer == 0 || layer == 3)) {
        int x = -1, y = -1;
        
        if(layer == 0) y = x_strip;
        if(layer == 3) x = x_strip;
        
        // accumula X e Y per il blocco
        if(layer == 0) {
          beamY_event.push_back(x_strip);
          beamQY_event.push_back(q_strip);
          h_time_pos_2D_Y->Fill(x_strip,t_strip);
        }
        if(layer == 3) {
          beamX_event.push_back(x_strip);
          beamQX_event.push_back(q_strip);
          h_time_pos_2D_X->Fill(x_strip,t_strip);
        }
	    }
    }
      
    // Alla fine del loop sugli strips, prima di incrementare eventCounter, riempi il TH2
    if(!beamX_event.empty() && !beamY_event.empty()) {
      for(size_t i=0;i<beamX_event.size();i++){
        if(beamQX_event[i]>100.){
          h_beamX_block->Fill(beamX_event[i],beamQX_event[i]);
          h_beam_block_PosCharge_X->Fill(beamX_event[i],beamQX_event[i]);
        }
        for(size_t j=0;j<beamY_event.size();j++){
          if(beamQX_event[i]>100. && beamQY_event[j]>100.){
            //double weight = beamQX_event[i] * beamQY_event[j];
            double weight = beamQX_event[i] * beamQY_event[j];
            if(beamY_event[j]>-20. && beamY_event[j]<20.){
              double cal_const = -(1/45.)*beamY_event[j] +(5./9.);
              double qy_cal = beamQY_event[j]/cal_const;
              weight = beamQX_event[i]*qy_cal;
            }
            h_beam_block_2D->Fill(beamX_event[i], beamY_event[j],weight);  // peso uniformemente o moltiplica se vuoi ponderare q_strip
          }
        }
      }
      for(size_t j=0;j<beamY_event.size();j++){
        if(beamQY_event[j]>100.){
          h_beamY_block->Fill(beamY_event[j],beamQY_event[j]);
          h_beam_block_PosCharge_Y->Fill(beamY_event[j],beamQY_event[j]);
        }
      }
    } 
    // Solo se ho coordinate X e Y valide
    if(!beamX_event.empty() && !beamY_event.empty()) {
      for(size_t i=0; i<beamX_event.size(); i++) {
        for(size_t j=0; j<beamY_event.size(); j++) {
          double weight = beamQX_event[i] * beamQY_event[j];
          h_occupancy_2D->Fill(beamX_event[i], beamY_event[j],weight);  // peso uniformemente, o moltiplica per q_strip se vuoi ponderare
        }
      }
    }
    beamX_event.clear();
    beamY_event.clear();
    beamQX_event.clear();
    beamQY_event.clear();
    
    eventCounter++;
      
      
    if(eventCounter == eventsPerBlock) {
	
      FitBeamBlock(h_beamX_block, currentBlock, h_muX1D_vs_time, h_sigmaX1D_vs_time, h_chargeX_vs_time);
      FitBeamBlock(h_beamY_block, currentBlock, h_muY1D_vs_time, h_sigmaY1D_vs_time, h_chargeY_vs_time);    
      FitBeamBlock2D(h_beam_block_2D, currentBlock, h_muX_vs_time, h_sigmaX_vs_time, h_muY_vs_time, h_sigmaY_vs_time);

      TH1F* h_charge_peak_Y = ExtractChargePeakPerStrip(h_beam_block_PosCharge_Y, currentBlock, beamMinY, beamMaxY);
      TH1F* h_charge_peak_X = ExtractChargePeakPerStrip(h_beam_block_PosCharge_X, currentBlock, beamMin, beamMax);
    
      // opzionale: fit gaussiano del profilo del fascio
      //TF1* beamFit = new TF1("beamFit","gaus",
      //		       beamMin,beamMax);
      
      //h_charge_peak->Fit(beamFit,"RQ");
      
      //double muC    = beamFit->GetParameter(1);
      //double sigmaC = beamFit->GetParameter(2);

      //h_muC_vs_time->SetBinContent(currentBlock+1,muC);
      //h_sigmaC_vs_time->SetBinContent(currentBlock+1,sigmaC);
      
      //delete beamFit;

      h_beam_block_PosCharge_Y->Reset();
      h_beam_block_PosCharge_X->Reset();
      
      h_beamX_block->Reset();
      h_beamY_block->Reset();
      h_beam_block_2D->Reset();
      eventCounter = 0;
      currentBlock++;
    }
      
  }
    
  /// ================================
  ///  Draw & Save final plots_"+padmenumber+"
  /// ================================
  
  TCanvas *c1 = new TCanvas("c_QT","Charge & Time",1000,1500);
  c1->Divide(2,nLayers);
  
  for(int l=0; l<nLayers; l++){
    c1->cd(2*l+1);
    hqmax[l]->Draw("hist");
    c1->cd(2*l+2);
    htmax[l]->Draw("hist");
  }
    
  c1->SaveAs("plots_"+padmenumber+"/Qmax_time_stats.png");
  
  TCanvas *c2 = new TCanvas("c_occ","Occupancy",1000,1500);
  c2->Divide(2,nLayers);
  
  for(int l=0; l<nLayers; l++){
    c2->cd(2*l+1);
    h_occupancy[l]->Draw("hist");
    c2->cd(2*l+2);
    hw_occupancy[l]->Draw("hist");
  }
    
  c2->SaveAs("plots_"+padmenumber+"/Occupancy.png");
  
  
  // ===============================
  // Beam global profile (Layer 0)
  // ===============================
  
  TH1F* h_beam_global = (TH1F*)hw_occupancy[0]->Clone("h_beam_global");
  h_beam_global->GetXaxis()->SetRangeUser(beamMin, beamMax);
  
  TCanvas* c_beam_global = new TCanvas("c_beam_global", "Beam profile Layer 0", 800,600);
  
  h_beam_global->SetLineWidth(2);
  h_beam_global->SetTitle("Beam profile Layer 0;Strip;Weighted occupancy");
  h_beam_global->Draw("hist");
  
  c_beam_global->SaveAs("plots_"+padmenumber+"/BeamProfile_Layer0_Global.png");
    
    
  TCanvas* c2D = new TCanvas("c2D","Beam Occupancy 2D",800,600);
  h_occupancy_2D->GetXaxis()->SetTitle("X [mm] (layer3)");
  h_occupancy_2D->GetYaxis()->SetTitle("Y [mm] (layer0)");
  h_occupancy_2D->Draw("COLZ");
  c2D->SaveAs("plots_"+padmenumber+"/BeamOccupancy2D.png");
  delete c2D;
  
  TCanvas* c2DtpX = new TCanvas("c2DtpX","Beam Time_Pos 2D",800,600);
  h_time_pos_2D_X->GetXaxis()->SetTitle("X [mm]");
  h_time_pos_2D_X->GetYaxis()->SetTitle("time [ns]");
  h_time_pos_2D_X->Draw("COLZ");
  c2DtpX->SaveAs("plots_"+padmenumber+"/Time_Pos2D_X.png");
  delete c2DtpX;
  
  TCanvas* c2DtpY = new TCanvas("c2DtpY","Beam Time_Pos 2D",800,600);
  h_time_pos_2D_Y->GetXaxis()->SetTitle("Y [mm]");
  h_time_pos_2D_Y->GetYaxis()->SetTitle("time [ns]");
  h_time_pos_2D_Y->Draw("COLZ");
  c2DtpY->SaveAs("plots_"+padmenumber+"/Time_Pos2D_Y.png");
  delete c2DtpY;
    
    
  TCanvas* c_mu = new TCanvas("c_mu","Beam position vs time",800,600);
  c_mu->Divide(1,2);
  c_mu->cd(1);
  h_muX_vs_time->Draw("histE");
  c_mu->cd(2);
  h_muY_vs_time->Draw("histE");
  c_mu->SaveAs("plots_"+padmenumber+"/BeamPosition2D_vs_Block.png");

  TCanvas* c_sigma = new TCanvas("c_sigma","Beam width vs time",800,600);
  c_sigma->Divide(1,2);
  c_sigma->cd(1);
  h_sigmaX_vs_time->Draw("histE");
  c_sigma->cd(2);
  h_sigmaY_vs_time->Draw("histE");
  c_sigma->SaveAs("plots_"+padmenumber+"/BeamSigma2D_vs_Block.png");

  TCanvas* c_charge = new TCanvas("c_charge","Beam width vs time",800,600);
  c_charge->Divide(1,2);
  c_charge->cd(1);
  h_chargeX_vs_time->Draw("histE");
  c_charge->cd(2);
  h_chargeY_vs_time->Draw("histE");
  c_charge->SaveAs("plots_"+padmenumber+"/BeamCharge2D_vs_Block.png");


  //compute average beam position in time and rms//
    
  double beam_pos_meanX = 0.;
  double beam_pos_spreadX = 0.;
  double beam_sigma_meanX = 0.;
  double beam_sigma_spreadX = 0.;
  double nblocksX = h_muX_vs_time->GetNbinsX();

  for(int i=1; i<nblocksX+1; i++){
    beam_pos_meanX+=h_muX_vs_time->GetBinContent(i);
    beam_sigma_meanX+=h_sigmaX_vs_time->GetBinContent(i);

  }
  beam_pos_meanX = beam_pos_meanX/nblocksX;
  beam_sigma_meanX = beam_sigma_meanX/nblocksX;

  for(int i=1; i<nblocksX; i++){
    beam_pos_spreadX+=TMath::Power((h_muX_vs_time->GetBinContent(i)-beam_pos_meanX),2);
    beam_sigma_spreadX+=TMath::Power((h_sigmaX_vs_time->GetBinContent(i)-beam_sigma_meanX),2);
  }
  beam_pos_spreadX = sqrt(beam_pos_spreadX/nblocksX);
  beam_sigma_spreadX = sqrt(beam_sigma_spreadX/nblocksX);
  std::cout<<std::setprecision(5)<<"beam position X: "<<beam_pos_meanX<<endl;
  std::cout<<std::setprecision(5)<<"beam position spread X: "<<beam_pos_spreadX<<endl;
  std::cout<<std::setprecision(5)<<"beam sigma X: "<<beam_sigma_meanX<<endl;
  std::cout<<std::setprecision(5)<<"beam sigma spread X: "<<beam_sigma_spreadX<<endl;

  ///////////////////////////////////////////////////
  //compute average beam position in time and rms//
    
  double beam_pos_meanY = 0.;
  double beam_pos_spreadY = 0.;
  double beam_sigma_meanY = 0.;
  double beam_sigma_spreadY = 0.;
  double nblocksY = h_muY_vs_time->GetNbinsX();

  for(int i=1; i<nblocksY+1; i++){
    beam_pos_meanY+=h_muY_vs_time->GetBinContent(i);
    beam_sigma_meanY+=h_sigmaY_vs_time->GetBinContent(i);

  }
  beam_pos_meanY = beam_pos_meanY/nblocksY;
  beam_sigma_meanY = beam_sigma_meanY/nblocksY;

  for(int i=1; i<nblocksY; i++){
    beam_pos_spreadY+=TMath::Power((h_muY_vs_time->GetBinContent(i)-beam_pos_meanY),2);
    beam_sigma_spreadY+=TMath::Power((h_sigmaY_vs_time->GetBinContent(i)-beam_sigma_meanY),2);
  }
  beam_pos_spreadY = sqrt(beam_pos_spreadY/nblocksY);
  beam_sigma_spreadY = sqrt(beam_sigma_spreadY/nblocksY);
  std::cout<<std::setprecision(5)<<"beam position Y: "<<beam_pos_meanY<<endl;
  std::cout<<std::setprecision(5)<<"beam position spread Y: "<<beam_pos_spreadY<<endl;
  std::cout<<std::setprecision(5)<<"beam sigma Y: "<<beam_sigma_meanY<<endl;
  std::cout<<std::setprecision(5)<<"beam sigma spread Y: "<<beam_sigma_spreadY<<endl;

  ///////////////////////////////////////////////////
    
  TCanvas* c_muX1D = new TCanvas("c_muX1D","Beam X position/sigma 1D vs time",800,600);
  c_muX1D->Divide(1,2);
  c_muX1D->cd(1);
  h_muX1D_vs_time->Draw("hist");
  c_muX1D->cd(2);
  h_sigmaX1D_vs_time->Draw("hist");
  c_muX1D->SaveAs("plots_"+padmenumber+"/BeamXPositionSigma1D_vs_Block.png");

  TCanvas* c_muY1D = new TCanvas("c_muY1D","Beam Y position/sigma 1D vs time",800,600);
  c_muY1D->Divide(1,2);
  c_muY1D->cd(1);
  h_muY1D_vs_time->Draw("hist");
  c_muY1D->cd(2);
  h_sigmaY1D_vs_time->Draw("hist");
  c_muY1D->SaveAs("plots_"+padmenumber+"/BeamYPositionSigma1D_vs_Block.png");

  
  TCanvas* c_muC = new TCanvas("c_muC","Beam position/sigma Charge vs time",800,600);
  c_muC->Divide(1,2);
  c_muC->cd(1);
  h_muC_vs_time->Draw("hist");
  c_muC->cd(2);
  h_sigmaC_vs_time->Draw("hist");
  c_muC->SaveAs("plots_"+padmenumber+"/BeamPositionSigmaCharge_vs_Block.png");
  
  
  
  /// ================================
  ///  Write everything to ROOT file
  /// ================================
  outfile->Write();
  outfile->Close();
  
  cout << "Analysis completed successfully." << endl;
}
