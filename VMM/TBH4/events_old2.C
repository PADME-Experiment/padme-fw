#define events_cxx
#include "events.h"

#include <TFile.h>

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

int TGraphAttribute(TGraph *Graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color){
  Graph->SetTitle(title);
  Graph->SetName(title);
  Graph->GetXaxis()->SetTitle(xlabel);
  Graph->GetYaxis()->SetTitle(ylabel);
  Graph->SetMarkerStyle(markerstyle);
  Graph->SetMarkerSize(1.8);
  Graph->SetMarkerColor(color);

  Graph->UseCurrentStyle();  //
  return 0;
}

void events::Loop()
{
   if (fChain == nullptr) {
      cerr << "events::Loop: input tree is null" << endl;
      return;
   }

   const TString outputFileName = Form("Reco_New_Events_run%s.root", fRunName.Data());
   TFile *outputFile = new TFile(outputFileName, "RECREATE");
   if (outputFile == nullptr || outputFile->IsZombie()) {
      cerr << "Cannot create output ROOT file" << outputFileName << endl;
      delete outputFile;
      return;
   }
   outputFile->cd();

   TDirectory* hDir = outputFile->mkdir("Histograms");
   TDirectory* hViewsDir[2] = {hDir->mkdir("X"), hDir->mkdir("Y")};

   TDirectory* evtdisplayDir = outputFile->mkdir("EventDisplays");
   TDirectory* evtdisplayViewsDir[2] = {evtdisplayDir->mkdir("X"), evtdisplayDir->mkdir("Y")};

   const Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nb = 0;

   // generate histograms - all faces all views
   TH1D* hTimeDiff = new TH1D("hTimeDiff", "Time wrt TriggerTime;#Delta t [ns];Entries", 1000, 91999.5, 95499.5);
   TH2D* hTimeDiffvsPos = new TH2D("hTimeDiffvsPos", "Time wrt TriggerTime vs position;#Delta t [ns];Position [strip]", 1000, 91999.5, 95499.5, 256, -0.5, 255.5);
   
   TH1D* hTimeDiff_ResCut = new TH1D("hTimeDiff_ResCut", "Time wrt TriggerTime_ResCut;#Delta t [ns];Entries", 1000, 91999.5, 95499.5);
   TH2D* hTimeDiffvsPos_ResCut = new TH2D("hTimeDiffvsPos_ResCut", "Time wrt TriggerTime vs position_ResCut;#Delta t [ns];Position [strip]", 1000, 91999.5, 95499.5, 256, -0.5, 255.5);
   
   // array of histograms for each view and each panel
   // V == X or Y - Z == Z
   // C == constant term fit function - intercept
   // M == slope term fit function - slope
   // plane == view == X or Y
   // Detector == face == front or back face 

   TH1D *Mx_hTimeDiff[2][2]; // time distribution X-Y both det
   TH2D *Mx_hTimevsPos[2][2]; // time vs strip X-Y both det
   
   TH1D *Vc_hTimeDiff_2view[2]; // time distribution for each view
   TH2D *Vc_hMvsC[2]; //track slope vs intercept for each view
   TH2D *Vc_hVResvsC[2]; //X-Y residual vs intercept for each view
   TH2D *Vc_hVResvsM[2]; //X-Y residual vs slope for each view
   TH2D *Vc_hVResvsVhit[2]; //X-Y residual vs X-Y hit position for each view
   TH2D *Vc_hZResvsZhit[2]; // Z residual vs Z for each view

   TH1D *Vc_hTimeDiff_2view_ResCut[2]; // time distribution for each view - after res cut
   TH1D *Mx_hTimeDiff_ResCut[2][2]; // time distribution X-Y bot plane - after res cut
   TH2D *Mx_hTimevsPos_ResCut[2][2]; // time vs strip X-Y both plane - after res cut

   TH2D *Vc_hHitsZV[2]; // hit pairing Z with X-Y (Z on X axis and V on y)
   TH2D *Vc_hSlopeIntercept[2]; // slope-intercept after hits pairing - intercept on X axis and m on Y axis
   TH1D *Vc_hSlopeMean[2]; // slope mean after hit pairing 
   TH1D *Vc_hInterceptMean[2]; // intercept mean after hit pairing 
   

   for(int ip = 0; ip < 2; ++ip) {
      TString panel = (ip == 0) ? "back" : "front";
      for(int iv = 0; iv < 2; ++iv) {
         TString view = (iv == 0) ? "X" : "Y";

         Mx_hTimeDiff[ip][iv] = new TH1D(Form("hTimeDiff_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime, %s panel, %s view;#Delta t [ns];Entries", panel.Data(), view.Data()), 1000, 91999.5, 95499.5);
         Mx_hTimevsPos[ip][iv] = new TH2D(Form("hTimeDiffvsPos_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime vs position, %s panel, %s view;#Delta t [ns];%s [strip]", panel.Data(), view.Data(), view.Data()), 1000, 91999.5, 95499.5, 256, -0.5, 255.5);

         //after cut on residuals
         Mx_hTimeDiff_ResCut[ip][iv] = new TH1D(Form("hTimeDiff_ResCut_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime after Res Cut, %s panel, %s view;#Delta t [ns];Entries", panel.Data(), view.Data()), 1000, 91999.5, 95499.5);
         Mx_hTimevsPos_ResCut[ip][iv] = new TH2D(Form("hTimeDiffvsPos_ResCut_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime vs position after Res Cut, %s panel, %s view;#Delta t [ns];%s [strip]", panel.Data(), view.Data(), view.Data()), 1000, 91999.5, 95499.5, 256, -0.5, 255.5);

         if (ip == 0) {
            Vc_hTimeDiff_2view[iv] = new TH1D(Form("hTimeDiff_2view%s", view.Data()), Form("Time wrt TriggerTime, %s view;#Delta t [ns];Entries", view.Data()), 1000, 91999.5, 95499.5);
            Vc_hMvsC[iv] = new TH2D(Form("hMvsC_%s", view.Data()), Form("Track slope vs intercept, %s view;Intercept [mm];Slope [mm/mm]", view.Data()), 400, -200, 200, 201, -1.005, 1.005);
            Vc_hVResvsC[iv] = new TH2D(Form("hVResvsC_%s", view.Data()), Form("%s residual vs intercept;Intercept [mm];%s residual [mm]", view.Data(), view.Data()), 400, -200, 200, 201, -10.05, 10.05);
            Vc_hVResvsM[iv] = new TH2D(Form("hVResvsM_%s", view.Data()), Form("%s residual vs slope;Slope [mm/mm];%s residual [mm]", view.Data(), view.Data()), 201, -1.005, 1.005, 201, -10.05, 10.05);
            Vc_hVResvsVhit[iv] = new TH2D(Form("hVResvsV_%s", view.Data()), Form("%s residual vs hit position;%s hit position [mm];%s residual [mm]", view.Data(), view.Data(), view.Data()), 256, -0.5*pitch, 255.5*pitch, 201, -10.05, 10.05);
            Vc_hZResvsZhit[iv] = new TH2D(Form("hZResvsZ_%s", view.Data()), "Z residual vs Z position; Z [mm]; residual Z [mm]", 101, -50.5, 50.5, 1601, -80.5, 80.5);
         
            Vc_hTimeDiff_2view_ResCut[iv] = new TH1D(Form("hTimeDiff_2view_ResCut_%s", view.Data()), Form("Time wrt TriggerTime after Res Cut, %s view;#Delta t [ns];Entries", view.Data()), 1000, 91999.5, 95499.5);
        
            Vc_hHitsZV[iv] = new TH2D(Form("hHitsZ%s", view.Data()), Form("Hits pairing Z vs %s view ;z [mm];%s [mm]", view.Data(), view.Data()), 256, -0.5*pitch, 255.5*pitch, 101, -50.5, 50.5);
            Vc_hSlopeIntercept[iv] = new TH2D(Form("hSlopeIntercept%s", view.Data()), Form("Slope vs Intercept, %s view ;Intercept [mm];Slope [mm/mm]", view.Data()), 101, -100.5, 400.5, 201, -1.005, 1.005);
            Vc_hSlopeMean[iv] = new TH1D(Form("hSlopeMean%s", view.Data()), Form("Slope Mean vs, %s view ;Slope [mm/mm]", view.Data()), 201, -1.005, 1.005);
            Vc_hInterceptMean[iv] = new TH1D(Form("hInterceptMean%s", view.Data()), Form("Intercept Mean vs, %s view ;Intercept [mm]", view.Data()), 101, -100.5, 400.5);
         }
      }
   }  

   for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
      const Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);

      // cout << "jentry: "<< jentry << " - event: " << event << " - n_hits: " << n_hits << endl;
      // if(event!=44170){
      //    continue;
      // }
      // cout << "abbiamo trovato event: " << event << endl;
      // cout << "n_hits: " << n_hits << endl;

      size_t nhits = n_hits;

      Int_t x_seed = -1;
      Int_t y_seed = -1;

      // if (pos == nullptr || plane == nullptr || det == nullptr || time == nullptr || trigger_time == nullptr) {
      //    cerr << "Null vector branch at entry " << jentry << endl;
      //    continue;
      // }

      // const size_t nhits = min({pos->size(), plane->size(), det->size(), time->size(), trigger_time->size()});

      // if (n_hits >= 0 && static_cast<size_t>(n_hits) != nhits) {
      //    cerr << "Warning: event " << event << " has n_hits = " << n_hits << ", while the usable vector size is " << nhits << endl;
      // }

      for (size_t iH = 0; iH < nhits; ++iH) {
         if (plane->at(iH) == 0 && x_seed < 0) {
            // cout << "plane->at(iH): " << plane->at(iH) << endl;
            x_seed = pos->at(iH);
            // cout << "x_seed: " << x_seed << endl;
         } else if (plane->at(iH) == 1 && y_seed < 0) {
            // cout << "plane->at(iH): " << plane->at(iH) << endl;
            y_seed = pos->at(iH);
            // cout << "y_seed: " << y_seed << endl;
         }
         if (x_seed >= 0 && y_seed >= 0) {
            break;
         }
      }

      // variable for linear fit
      double mt_avg[2], ct_avg[2];
      double z_avg[2] = {0, 0};
      double zv_avg[2] = {0, 0};
      double v_avg[2] = {0, 0};
      double z2_avg[2] = {0, 0};
      int n_avg[2] = {0, 0};
      int hitsroad[2] = {0, 0};
 
      for (size_t iH = 0; iH < nhits; ++iH) {
         double vpos = pos->at(iH)*pitch;
         // cout << "vpos: " << vpos << endl;
         double time_diff = time->at(iH) - trigger_time->at(iH) * 22.5;
         // cout << "time_diff: " << time_diff << " - time->at(iH): " << time->at(iH) << " - trigger_time->at(iH) * 22.5: " << trigger_time->at(iH) * 22.5 << endl;
         
         double z = (time_diff - T0) * DriftVelocity; // time shifted at 0 ns
         
         // cout << "pre if - det->at(iH): " << det->at(iH) << endl;
         if(det->at(iH)==1){ // back face
            // cout << "det->at(iH): " << det->at(iH) << endl;
            z = z - driftgap;
         } else if(det->at(iH)==2){ // front face
            // cout << "det->at(iH): " << det->at(iH) << endl;
            z = - z + driftgap;
         }

         // hit pairing X/Y with Z 
         // hough transform X view
         if(plane->at(iH) == 0){
            for(size_t iHH = iH+1; iHH < nhits; ++iHH){
               if(plane->at(iHH) == 0){
                  double vpos2 = pos->at(iHH)*pitch;
                  double deltaV = vpos - vpos2;

                  double time_diff2 = time->at(iHH) - trigger_time->at(iHH) * 22.5;
                  double z2 = (time_diff2 - T0) * DriftVelocity;
                  if(det->at(iHH)==1){ // back face
                     // cout << "det->at(iH): " << det->at(iH) << endl;
                     z2 = z2 - driftgap;
                  } else if(det->at(iHH)==2){ // front face
                     // cout << "det->at(iH): " << det->at(iH) << endl;
                     z2 = - z2 + driftgap;
                  }
                  double deltaZ = z - z2;

                  double slope = deltaV/deltaZ;
                  double inter = vpos2 - slope * z2;

                  Vc_hSlopeIntercept[0]->Fill(inter, slope);
               }
            }
         } else if (plane->at(iH) == 1){
            for(size_t iHH = iH+1; iHH < nhits; ++iHH){
               if(plane->at(iHH) == 1){
                  double vpos2 = pos->at(iHH)*pitch;
                  double deltaV = vpos - vpos2;

                  double time_diff2 = time->at(iHH) - trigger_time->at(iHH) * 22.5;
                  double z2 = (time_diff2 - T0) * DriftVelocity;
                  if(det->at(iHH)==1){ // back face
                     // cout << "det->at(iH): " << det->at(iH) << endl;
                     z2 = z2 - driftgap;
                  } else if(det->at(iHH)==2){ // front face
                     // cout << "det->at(iH): " << det->at(iH) << endl;
                     z2 = - z2 + driftgap;
                  }
                  double deltaZ = z - z2;

                  double slope = deltaV/deltaZ;
                  double inter = vpos2 - slope * z2;

                  Vc_hSlopeIntercept[1]->Fill(inter, slope);
               }
            }
         }
         
         // hits average for track building (x and y separately)
         if(plane->at(iH) == 0){
            if(x_seed >= 0 && abs(pos->at(iH) - x_seed) < 5){
               z_avg[0] += z;
               zv_avg[0] += z * vpos;
               v_avg[0] += vpos;
               z2_avg[0] += z * z;
               n_avg[0]++;
               hitsroad[0]++;
            }
         } else if(plane->at(iH) == 1){
            if(y_seed >= 0 && abs(pos->at(iH) - y_seed) < 5){
               z_avg[1] += z;
               zv_avg[1] += z * vpos;
               v_avg[1] += vpos;
               z2_avg[1] += z * z;
               n_avg[1]++;
               hitsroad[1]++;
            }
         }

         if(det->at(iH)==1){
            if (plane->at(iH) == 0 && x_seed >= 0 && abs(pos->at(iH) - x_seed) < 5) {
               hTimeDiff->Fill(time_diff);
               Mx_hTimeDiff[0][0]->Fill(time_diff);
               Vc_hTimeDiff_2view[0]->Fill(time_diff);
               hTimeDiffvsPos->Fill(time_diff, pos->at(iH));
               Mx_hTimevsPos[0][0]->Fill(time_diff, pos->at(iH));
            }
            else if (plane->at(iH) == 1 && y_seed >= 0 && abs(pos->at(iH) - y_seed) < 5) {
               hTimeDiff->Fill(time_diff);
               Mx_hTimeDiff[0][1]->Fill(time_diff);
               Vc_hTimeDiff_2view[1]->Fill(time_diff);
               hTimeDiffvsPos->Fill(time_diff, pos->at(iH));
               Mx_hTimevsPos[0][1]->Fill(time_diff, pos->at(iH));
            }
         } else if(det->at(iH)==2){
            if (plane->at(iH) == 0 && x_seed >= 0 && abs(pos->at(iH) - x_seed) < 5) {
               hTimeDiff->Fill(time_diff);
               Mx_hTimeDiff[1][0]->Fill(time_diff);
               Vc_hTimeDiff_2view[0]->Fill(time_diff);
               hTimeDiffvsPos->Fill(time_diff, pos->at(iH));
               Mx_hTimevsPos[1][0]->Fill(time_diff, pos->at(iH));
            }
            else if (plane->at(iH) == 1 && y_seed >= 0 && abs(pos->at(iH) - y_seed) < 5) {
               hTimeDiff->Fill(time_diff);
               Mx_hTimeDiff[1][1]->Fill(time_diff);
               Vc_hTimeDiff_2view[1]->Fill(time_diff);
               hTimeDiffvsPos->Fill(time_diff, pos->at(iH));
               Mx_hTimevsPos[1][1]->Fill(time_diff, pos->at(iH));
            }
         }
         // cout << "n_avg[0]: " << n_avg[0] << " - n_avg[1]: " << n_avg[1] << endl;

      } //end of hit loop

      // road su hough transform
      double SlpInterMax[2] = {0.0, 0.0};  // Maximum bin content: 0 = X, 1 = Y
      int SlpInterMaxBinX[2] = {-1, -1};
      int SlpInterMaxBinY[2] = {-1, -1};

      double SlpMax[2] = {0.0, 0.0}; // slope max xy
      double InterMax[2] = {0.0, 0.0}; // intercept max xy

      for (int iv = 0; iv < 2; ++iv) {
         const int nBinsX = Vc_hSlopeIntercept[iv]->GetNbinsX();
         const int nBinsY = Vc_hSlopeIntercept[iv]->GetNbinsY();

         double maximum = -999;
         for (int ibx = 1; ibx <= nBinsX; ++ibx) {
            for (int iby = 1; iby <= nBinsY; ++iby) {
               const double binContent = Vc_hSlopeIntercept[iv]->GetBinContent(ibx, iby);
               if (binContent > maximum) {
                  maximum = binContent;
                  SlpInterMax[iv] = binContent;
                  SlpInterMaxBinX[iv] = ibx;
                  SlpInterMaxBinY[iv] = iby;

                  InterMax[iv] = Vc_hSlopeIntercept[iv]->GetXaxis()->GetBinCenter(ibx);
                  SlpMax[iv] = Vc_hSlopeIntercept[iv]->GetYaxis()->GetBinCenter(iby);
               }
            }
         }
         // cout << view << " view maximum:" << endl;
         // cout << "  content   = " << SlpInterMax[iv] << endl;
         // cout << "  bin       = (" << SlpInterMaxBinX[iv] << ", " << SlpInterMaxBinY[iv] << ")" << endl;
         // cout << "  intercept = " << SlpInterMaxX[iv] << endl;
         // cout << "  slope     = " << SlpInterMaxY[iv] << endl;
         Vc_hSlopeIntercept[iv]->Clear();
      }



      //preparing graphs and variables for the event display
      const int totalRoadHits = hitsroad[0] + hitsroad[1];
      const bool saveEventDisplay = 0 > 1;

      TGraph* gTrack[2] = {nullptr, nullptr};
      TF1* fTrack[2] = {nullptr, nullptr};

      if(saveEventDisplay) {
         gTrack[0] = new TGraph();
         gTrack[1] = new TGraph();
         TGraphAttribute(gTrack[0], Form("gTrack_event_%d_X", event), "z [mm]", "X [mm]", 20, kRed+1);
         TGraphAttribute(gTrack[1], Form("gTrack_event_%d_Y", event), "z [mm]", "Y [mm]", 20, kBlue+1);
      }

      // compute average for track building
      for(int i = 0; i < 2; ++i){
         if(n_avg[i] == 0) continue;
         z_avg[i] = z_avg[i]/n_avg[i];
         zv_avg[i] = zv_avg[i]/n_avg[i];
         v_avg[i] = v_avg[i]/n_avg[i];
         z2_avg[i] = z2_avg[i]/n_avg[i];

         mt_avg[i] = (zv_avg[i] - z_avg[i]*v_avg[i])/(z2_avg[i]-z_avg[i]*z_avg[i]);
         ct_avg[i] = v_avg[i] - mt_avg[i]*z_avg[i];

         // creating TF1 for the event display
         if(saveEventDisplay) {
            TString view = (i == 0) ? "X" : "Y";
            fTrack[i] = new TF1(Form("fTrack_event_%d_%s", event, view.Data()), "[0]*x+[1]", -driftgap, driftgap);
            fTrack[i]->SetParameter(0, mt_avg[i]);
            fTrack[i]->SetParameter(1, ct_avg[i]);
            fTrack[i]->SetLineColor((i == 0) ? kRed : kBlue);
            fTrack[i]->SetLineWidth(2);
         }

         Vc_hMvsC[i]->Fill(ct_avg[i], mt_avg[i]);
         
         for (size_t iH = 0; iH < nhits; ++iH) {
            double vpos = pos->at(iH)*pitch;
            double time_diff = time->at(iH) - trigger_time->at(iH) * 22.5;
            
            double z = (time_diff - T0) * DriftVelocity; // time shifted at 0 ns
            if(det->at(iH)==1){ // back face
               z = z - driftgap;
            } else if(det->at(iH)==2){ // front face
               z = - z + driftgap;
            }

            if(plane->at(iH) != i) continue;
            const Int_t seed = (i == 0) ? x_seed : y_seed;
            if(seed < 0 || abs(pos->at(iH) - seed) >= 5) continue;
            if(saveEventDisplay) gTrack[i]->SetPoint(gTrack[i]->GetN(), z, vpos);

            double vpos_fit = mt_avg[i]*z + ct_avg[i];
            double zpos_fit = (vpos - ct_avg[i])/mt_avg[i];
            Vc_hVResvsC[i]->Fill(ct_avg[i], vpos_fit - vpos);
            Vc_hVResvsM[i]->Fill(mt_avg[i], vpos_fit - vpos);
            Vc_hVResvsVhit[i]->Fill(vpos, vpos_fit - vpos);
            Vc_hZResvsZhit[i]->Fill(z, zpos_fit-z);

            if(fabs(zpos_fit-z)<40){
               if(det->at(iH)==1){
                  if (plane->at(iH) == 0 && x_seed >= 0 && abs(pos->at(iH) - x_seed) < 5) {
                     hTimeDiff_ResCut->Fill(time_diff);
                     Mx_hTimeDiff_ResCut[0][0]->Fill(time_diff);
                     Vc_hTimeDiff_2view_ResCut[0]->Fill(time_diff);   
                     hTimeDiffvsPos_ResCut->Fill(time_diff, pos->at(iH));
                     Mx_hTimevsPos_ResCut[0][0]->Fill(time_diff, pos->at(iH));
                  }
                  else if (plane->at(iH) == 1 && y_seed >= 0 && abs(pos->at(iH) - y_seed) < 5) {
                     hTimeDiff_ResCut->Fill(time_diff);
                     Mx_hTimeDiff_ResCut[0][1]->Fill(time_diff);
                     Vc_hTimeDiff_2view_ResCut[1]->Fill(time_diff);   
                     hTimeDiffvsPos_ResCut->Fill(time_diff, pos->at(iH));
                     Mx_hTimevsPos_ResCut[0][1]->Fill(time_diff, pos->at(iH));
                  }
               } else if(det->at(iH)==2){
                  if (plane->at(iH) == 0 && x_seed >= 0 && abs(pos->at(iH) - x_seed) < 5) {
                     hTimeDiff_ResCut->Fill(time_diff);
                     Mx_hTimeDiff_ResCut[1][0]->Fill(time_diff);
                     Vc_hTimeDiff_2view_ResCut[0]->Fill(time_diff);   
                     hTimeDiffvsPos_ResCut->Fill(time_diff, pos->at(iH));
                     Mx_hTimevsPos_ResCut[1][0]->Fill(time_diff, pos->at(iH));
                  }
                  else if (plane->at(iH) == 1 && y_seed >= 0 && abs(pos->at(iH) - y_seed) < 5) {
                     hTimeDiff_ResCut->Fill(time_diff);
                     Mx_hTimeDiff_ResCut[1][1]->Fill(time_diff);
                     Vc_hTimeDiff_2view_ResCut[1]->Fill(time_diff);   
                     hTimeDiffvsPos_ResCut->Fill(time_diff, pos->at(iH));
                     Mx_hTimevsPos_ResCut[1][1]->Fill(time_diff, pos->at(iH));
                  }
               }
            }

         }
      }
      if(saveEventDisplay) {
         outputFile->cd();
         evtdisplayDir->cd();
         for(int i = 0; i < 2; ++i) {
            evtdisplayViewsDir[i]->cd();

            if(gTrack[i] != nullptr) gTrack[i]->Write();
            if(fTrack[i] != nullptr) fTrack[i]->Write();

            if(gTrack[i] != nullptr) delete gTrack[i];
            if(fTrack[i] != nullptr) delete fTrack[i];

            gTrack[i] = nullptr;
            fTrack[i] = nullptr;
         }
      }
   }
   
   outputFile->cd();
   hDir->cd();

   hTimeDiff->Write();
   hTimeDiffvsPos->Write();
   hTimeDiff_ResCut->Write();
   hTimeDiffvsPos_ResCut->Write();

   for(int ip= 0; ip < 2; ++ip) {
      for(int iv = 0; iv < 2; ++iv) {
         hDir->cd();
         hViewsDir[iv]->cd();
         Mx_hTimeDiff[ip][iv]->Write();
         Mx_hTimevsPos[ip][iv]->Write();

         Mx_hTimeDiff_ResCut[ip][iv]->Write();
         Mx_hTimevsPos_ResCut[ip][iv]->Write();

         if(ip == 0) {
            Vc_hTimeDiff_2view[iv]->Write();   
            Vc_hMvsC[iv]->Write();
            Vc_hVResvsC[iv]->Write();
            Vc_hVResvsM[iv]->Write();
            Vc_hVResvsVhit[iv]->Write();
            Vc_hZResvsZhit[iv]->Write();
            Vc_hTimeDiff_2view_ResCut[iv]->Write();   
            Vc_hSlopeIntercept[iv]->Write();
         }
      }
   }
   
   cout << "Processed tree entries: " << nentries << endl;
   cout << "Selected hits: " << hTimeDiff->GetEntries() << endl;
   cout << "Output written to " << outputFileName << endl;
}
