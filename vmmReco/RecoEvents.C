#define RecoEvents_cxx
#include "RecoEvents.h"

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

// void RecoEvents::HitEfficiency(){
//    //count n-hit found in a track
//    // divided by n_hit attese --> to see holes
//    // 
// }

void RecoEvents::Loop()
{
   if (fChain == nullptr) {
      cerr << "RecoEvents::Loop: input tree is null" << endl;
      return;
   }

   const TString outputFileName = Form("V1_Reco_Events_run%s.root", fRunName.Data());
   TFile *outputFile = new TFile(outputFileName, "RECREATE");
   if (outputFile == nullptr || outputFile->IsZombie()) {
      cerr << "Cannot create output ROOT file" << outputFileName << endl;
      delete outputFile;
      return;
   }
   outputFile->cd();

   TDirectory* hDir = outputFile->mkdir("Histograms");
   TDirectory* hViewsDir[2] = {hDir->mkdir("X"), hDir->mkdir("Y")};
   TDirectory* hPanelDirX[2] = {hViewsDir[0]->mkdir("back"), hViewsDir[0]->mkdir("front")};
   TDirectory* hPanelDirY[2] = {hViewsDir[1]->mkdir("back"), hViewsDir[1]->mkdir("front")};

   TDirectory* evtdisplayDir = outputFile->mkdir("EventDisplays");
   TDirectory* evtdisplayViewsDir[2] = {evtdisplayDir->mkdir("X"), evtdisplayDir->mkdir("Y")};

   const Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nb = 0;

   // generate histograms - all faces all views
   TH1D* hTimeDiff = new TH1D("hTimeDiff", "Time wrt TriggerTime;#Delta t [ns];Entries", 200, 91499.5, 95999.5);
   TH2D* hTimeDiffvsPos = new TH2D("hTimeDiffvsPos", "Time - TriggerTime vs position;#Delta t [ns];Position [strip]", 200, 91499.5, 95999.5, 256, -0.5, 255.5);
   TH2D* hTimeDiffvsADC = new TH2D("hTimeDiffvsADC", "Time - TriggerTime vs ADC;#Delta t [ns];Q [adc]", 200, 91499.5, 95999.5, 1000, -0.5, 999.5);
   TH2D* hPosvsADC = new TH2D("hPosvsADC", "position vs ADC;V [strip];Q [adc]", 256, -0.5, 255.5, 1000, -0.5, 999.5);
   // TH2D* hVPosvsVPos = new TH2D("hVPosvsVPos", "X vs Y;X [strip];Y [strip]", 256, -0.5, 255.5, 256, -0.5, 255.5);
   TH2D* hPosvsADC_OutofTime = new TH2D("hPosvsADC_OutofTime", "position vs ADC;V [strip];Q [adc]",  256, -0.5, 255.5, 1000, -0.5, 999.5);
   
   TH1D* hTimeDiff_Sel = new TH1D("hTimeDiff_Sel", "Time wrt TriggerTime_Sel;#Delta t [ns];Entries", 200, 91499.5, 95999.5);
   TH2D* hTimeDiffvsPos_Sel = new TH2D("hTimeDiffvsPos_Sel", "Time wrt TriggerTime vs position_Sel;#Delta t [ns];Position [strip]", 200, 91499.5, 95999.5, 256, -0.5, 255.5);
   TH2D* hTimeDiffvsADC_Sel = new TH2D("hTimeDiffvsADC_Sel", "Time - TriggerTime vs ADC_Sel;#Delta t [ns];Q [adc]", 200, 91499.5, 95999.5, 1000, -0.5, 999.5);
   TH2D* hPosvsADC_Sel = new TH2D("hPosvsADC_Sel", "position - TriggerTime vs ADC_Sel;V [strip];Q [adc]", 256, -0.5, 255.5, 1000, -0.5, 999.5);
   // TH2D* hVPosvsVPos_Sel = new TH2D("hVPosvsVPos_Sel", "X vs Y_Sel;X [strip];Y [strip]", 256, -0.5, 255.5, 256, -0.5, 255.5);
   TH2D* hPosvsADC_OutofTime_Sel = new TH2D("hPosvsADC_OutofTime_Sel", "position vs ADC;V [strip];Q [adc]",  256, -0.5, 255.5, 1000, -0.5, 999.5);

   // array of histograms for each view and each panel
   // V == X or Y - Z == Z
   // C == constant term fit function - intercept
   // M == slope term fit function - slope
   // plane == view == X or Y
   // Detector == face == front or back face 
   
   // Combined VV distributions: one histogram for each view
   TH1D *VV_hTimeDiff[2]; // time distribution X-Y 
   TH2D *VV_hTimevsPos[2]; // time vs strip X-Y 
   TH2D *VV_hPosvsADC[2]; // strip vs ADC X-Y 
   TH2D *VV_hPosvsADC_OutofTime[2]; // strip vs ADC X-Y - out of time
   TH2D *VV_hZfitvsTimeDiff[2]; // Z fitted vs time diff --> plto to show the drift velocity from each view
   TH1D *VV_hNHit[2]; // hit per event distribution for each view
   TH2D *VV_hMvsC[2]; //track slope vs intercept for each view
   TH2D *VV_hVResvsC[2]; //X-Y residual vs intercept for each view
   TH2D *VV_hVResvsM[2]; //X-Y residual vs slope for each view
   TH2D *VV_hVResvsVhit[2]; //X-Y residual vs X-Y hit position for each view
   TH2D *VV_hZResvsZhit[2]; // Z residual vs Z for each view
   TH2D *VV_hZResvsADC[2]; // Z residual vs Z for each view
   TH2D *VV_hZResvsM[2]; // Z residual vs slope for each view

   // MVP distributions: one histogram for each panel and view
   TH1D *MVP_hTimeDiff[2][2]; // time distribution X-Y both det
   TH2D *MVP_hTimevsPos[2][2]; // time vs strip X-Y both det
   TH2D *MVP_hPosvsADC[2][2]; // strip vs ADC X-Y both det
   TH2D *MVP_hPosvsADC_OutofTime[2][2]; // strip vs ADC X-Y both det - out of time
   TH2D *MVP_hZfitvsTimeDiff[2][2]; // Z fitted vs time diff --> plto to show the drift velocity from each view and each det
   TH1D *MVP_hNHit[2][2]; // hit per event distribution for each view
   TH2D *MVP_hMvsC[2][2]; //track slope vs intercept for each view
   TH2D *MVP_hVResvsC[2][2]; //X-Y residual vs intercept for each view
   TH2D *MVP_hVResvsM[2][2]; //X-Y residual vs slope for each view
   TH2D *MVP_hVResvsVhit[2][2]; //X-Y residual vs X-Y hit position for each view
   TH2D *MVP_hZResvsZhit[2][2]; // Z residual vs Z for each view
   TH2D *MVP_hZResvsADC[2][2]; // Z residual vs Z for each view
   TH2D *MVP_hZResvsM[2][2]; // Z residual vs slope for each view

   // Combined VV distributions after selection
   TH1D *VV_hTimeDiff_Sel[2]; // time distribution X-Y bot plane - after Sel
   TH2D *VV_hTimevsPos_Sel[2]; // time vs strip X-Y both plane - after Sel
   TH2D *VV_hPosvsADC_Sel[2]; // strip vs ADC X-Y both det - after Sel
   TH2D *VV_hPosvsADC_OutofTime_Sel[2]; // strip vs ADC X-Y both det - out of time - after Sel
   TH1D *VV_hNHit_Sel[2]; // hit per event distribution for each view - after sel

   // MVP distributions after selection
   TH1D *MVP_hTimeDiff_Sel[2][2]; // time distribution X-Y bot plane - after Sel
   TH2D *MVP_hTimevsPos_Sel[2][2]; // time vs strip X-Y both plane - after Sel
   TH2D *MVP_hPosvsADC_Sel[2][2]; // strip vs ADC X-Y both det - after Sel
   TH2D *MVP_hPosvsADC_OutofTime_Sel[2][2]; // strip vs ADC X-Y both det - out of time - after Sel
   TH1D *MVP_hNHit_Sel[2][2]; // hit per event distribution for each view both det - after sel

   // Combined VV slope-intercept distributions
   TH2D *VV_hSlopeIntercept[2]; // slope-intercept after hits pairing - intercept on X axis and m on Y axis
   TH2D *VV_hSlopeIntercept_All[2]; // slope-intercept after hits pairing - intercept on X axis and m on Y axis
   TH2D *VV_hSlopeIntercept_event[2]; // slope-intercept after hits pairing - intercept on X axis and m on Y axis

   // MVP slope-intercept distributions
   TH2D *MVP_hSlopeIntercept[2][2]; // slope-intercept after hits pairing - intercept on X axis and m on Y axis
   TH2D *MVP_hSlopeIntercept_All[2][2]; // slope-intercept after hits pairing - intercept on X axis and m on Y axis
   TH2D *MVP_hSlopeIntercept_event[2][2]; // slope-intercept after hits pairing - intercept on X axis and m on Y axis


   for(int ip = 0; ip < 2; ++ip) {
      TString panel = (ip == 0) ? "back" : "front";

      for(int iv = 0; iv < 2; ++iv) {
         TString view = (iv == 0) ? "X" : "Y";

         // MVP histograms: panel + view
         MVP_hTimeDiff[ip][iv] = new TH1D(Form("hTimeDiff_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime, %s panel, %s view;#Delta t [ns];Entries", panel.Data(), view.Data()), 200, 91499.5, 95999.5);
         MVP_hTimevsPos[ip][iv] = new TH2D(Form("hTimeDiffvsPos_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime vs position, %s panel, %s view;#Delta t [ns];%s [strip]", panel.Data(), view.Data(), view.Data()), 200, 91499.5, 95999.5, 256, -0.5, 255.5);
         MVP_hPosvsADC[ip][iv] = new TH2D(Form("hPosvsADC%s%s", panel.Data(), view.Data()), Form("View vs ADC, %s panel, %s view;%s [strip];Q [adc]", panel.Data(), view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
         MVP_hPosvsADC_OutofTime[ip][iv] = new TH2D(Form("hPosvsADC_OutofTime%s%s", panel.Data(), view.Data()), Form("View vs ADC - out of time, %s panel, %s view;%s [strip];Q [adc]", panel.Data(), view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
         MVP_hZfitvsTimeDiff[ip][iv] = new TH2D(Form("hZfitvsTimeDiff_%s%s", panel.Data(), view.Data()), Form("Fitted Z vs time, %s panel, %s view;#Delta t [ns];Fitted Z [mm]", panel.Data(), view.Data()), 140, -200.5, 1199.5, 140, -70.5, 69.5);
         MVP_hNHit[ip][iv] = new TH1D(Form("hNHit_%s%s", panel.Data(), view.Data()), Form("Number of hits, %s panel, %s view;Hits per event;Entries", panel.Data(), view.Data()), 200, -0.5, 199.5);

         // MVP track and residual histograms
         MVP_hMvsC[ip][iv] = new TH2D(Form("hMvsC_%s%s", panel.Data(), view.Data()), Form("Track slope vs intercept, %s panel, %s view;Intercept [mm];Slope [mm/mm]", panel.Data(), view.Data()), 400, -200.5, 199.5, 200, -1.005, 0.995);
         MVP_hVResvsC[ip][iv] = new TH2D(Form("hVResvsC_%s%s", panel.Data(), view.Data()), Form("%s residual vs intercept, %s panel;Intercept [mm];%s residual [mm]", view.Data(), panel.Data(), view.Data()), 400, -200.5, 199.5, 200, -10.05, 9.95);
         MVP_hVResvsM[ip][iv] = new TH2D(Form("hVResvsM_%s%s", panel.Data(), view.Data()), Form("%s residual vs slope, %s panel;Slope [mm/mm];%s residual [mm]", view.Data(), panel.Data(), view.Data()), 200, -1.005, 0.995, 200, -10.05, 9.95);
         MVP_hVResvsVhit[ip][iv] = new TH2D(Form("hVResvsVhit_%s%s", panel.Data(), view.Data()), Form("%s residual vs hit position, %s panel;%s hit position [mm];%s residual [mm]", view.Data(), panel.Data(), view.Data(), view.Data()), 256, -0.5 * pitch, 255.5 * pitch, 200, -10.05, 9.95);
         MVP_hZResvsZhit[ip][iv] = new TH2D(Form("hZResvsZhit_%s%s", panel.Data(), view.Data()), Form("Z residual vs Z position, %s panel, %s view;Z [mm];Z residual [mm]", panel.Data(), view.Data()), 100, -50.5, 49.5, 1600, -20.5, 19.5);
         MVP_hZResvsADC[ip][iv] = new TH2D(Form("hZResvsADC_%s%s", panel.Data(), view.Data()), Form("Z residual vs charge, %s panel, %s view;Q_{%s} [ADC];Z residual [mm]", panel.Data(), view.Data(), view.Data()), 100, -0.5, 999.5, 1600, -20.5, 19.5);
         MVP_hZResvsM[ip][iv] = new TH2D(Form("hZResvsM_%s%s", panel.Data(), view.Data()), Form("Z residual vs slope, %s panel, %s view;Slope [mm/mm];Z residual [mm]", panel.Data(), view.Data()), 200, -1.005, 0.995, 1600, -20.5, 19.5);

         // MVP histograms after selection
         MVP_hTimeDiff_Sel[ip][iv] = new TH1D(Form("hTimeDiff_Sel_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime after Sel, %s panel, %s view;#Delta t [ns];Entries", panel.Data(), view.Data()), 200, 91499.5, 95999.5);
         MVP_hTimevsPos_Sel[ip][iv] = new TH2D(Form("hTimeDiffvsPos_Sel_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime vs position after Sel, %s panel, %s view;#Delta t [ns];%s [strip]", panel.Data(), view.Data(), view.Data()), 200, 91499.5, 95999.5, 256, -0.5, 255.5);
         MVP_hPosvsADC_Sel[ip][iv] = new TH2D(Form("hPosvsADC_Sel%s%s", panel.Data(), view.Data()), Form("View vs ADC_Sel, %s panel, %s view;%s [strip];Q [adc]", panel.Data(), view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
         MVP_hPosvsADC_OutofTime_Sel[ip][iv] = new TH2D(Form("hPosvsADC_OutofTime_Sel%s%s", panel.Data(), view.Data()), Form("View vs ADC - out of time_Sel, %s panel, %s view;%s [strip];Q [adc]", panel.Data(), view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
         MVP_hNHit_Sel[ip][iv] = new TH1D(Form("hNHit_Sel_%s%s", panel.Data(), view.Data()), Form("Number of hits after selection, %s panel, %s view;Hits per event;Entries", panel.Data(), view.Data()), 200, -0.5, 199.5);

         // MVP slope-intercept histograms
         MVP_hSlopeIntercept[ip][iv] = new TH2D(Form("hSlopeIntercept_%s%s", panel.Data(), view.Data()), Form("Slope vs intercept, %s panel, %s view;Intercept [mm];Slope [mm/mm]", panel.Data(), view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
         MVP_hSlopeIntercept_event[ip][iv] = new TH2D(Form("hSlopeIntercept_event_%s%s", panel.Data(), view.Data()), Form("Slope vs intercept per event, %s panel, %s view;Intercept [mm];Slope [mm/mm]", panel.Data(), view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
         MVP_hSlopeIntercept_All[ip][iv] = new TH2D(Form("hSlopeIntercept_All_%s%s", panel.Data(), view.Data()), Form("All slope-intercept combinations, %s panel, %s view;Intercept [mm];Slope [mm/mm]", panel.Data(), view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);

         // VV histograms: create only once for each view
         if (ip == 0) {
            VV_hTimeDiff[iv] = new TH1D(Form("hTimeDiff_%s", view.Data()), Form("Time wrt TriggerTime, %s view;#Delta t [ns];Entries", view.Data()), 200, 91499.5, 95999.5);
            VV_hTimevsPos[iv] = new TH2D(Form("hTimeDiffvsPos_%s", view.Data()), Form("Time wrt TriggerTime vs position, %s view;#Delta t [ns];%s [strip]", view.Data(), view.Data()), 200, 91499.5, 95999.5, 256, -0.5, 255.5);
            VV_hPosvsADC[iv] = new TH2D(Form("hPosvsADC_%s", view.Data()), Form("Position vs ADC, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
            VV_hPosvsADC_OutofTime[iv] = new TH2D(Form("hPosvsADC_OutofTime_%s", view.Data()), Form("Position vs ADC, out of time, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
            VV_hZfitvsTimeDiff[iv] = new TH2D(Form("hZfitvsTimeDiff_%s", view.Data()), Form("Fitted Z vs time, %s view;#Delta t [ns];Fitted Z [mm]", view.Data()), 140, -200.5, 1199.5, 140, -70.5, 69.5);
            VV_hNHit[iv] = new TH1D(Form("hNHit%s", view.Data()), Form("Nhit distribution, %s view;Hit ;Entries", view.Data()), 200, -0.5, 199.5);
            
            VV_hMvsC[iv] = new TH2D(Form("hMvsC_%s", view.Data()), Form("Track slope vs intercept, %s view;Intercept [mm];Slope [mm/mm]", view.Data()), 400, -200.5, 199.5, 200, -1.005, .995);
            VV_hVResvsC[iv] = new TH2D(Form("hVResvsC_%s", view.Data()), Form("%s residual vs intercept;Intercept [mm];%s residual [mm]", view.Data(), view.Data()), 400, -200.5, 199.5, 200, -10.05, 9.95);
            VV_hVResvsM[iv] = new TH2D(Form("hVResvsM_%s", view.Data()), Form("%s residual vs slope;Slope [mm/mm];%s residual [mm]", view.Data(), view.Data()), 200, -1.005, 0.995, 200, -10.05, 9.95);
            VV_hVResvsVhit[iv] = new TH2D(Form("hVResvsV_%s", view.Data()), Form("%s residual vs hit position;%s hit position [mm];%s residual [mm]", view.Data(), view.Data(), view.Data()), 256, -0.5*pitch, 255.5*pitch, 200, -10.05, 9.95);
            VV_hZResvsZhit[iv] = new TH2D(Form("hZResvsZ_%s", view.Data()), Form("Z residual vs Z position, view %s; Z [mm]; residual Z [mm]", view.Data()), 100, -50.5, 49.5, 1600, -20.5, 19.5);
            VV_hZResvsADC[iv] = new TH2D(Form("hZResvsADC_%s", view.Data()), Form("Z residual vs Charge; Q_{%s} [mm]; residual Z [mm]", view.Data()), 100, -0.5, 999.5, 1600, -20.5, 19.5);
            VV_hZResvsM[iv] = new TH2D(Form("hZResvsM%s", view.Data()), Form("Z residual vs slope, view %s;Slope [mm/mm];Z residual [mm]", view.Data()), 200, -1.005, 0.995, 1600, -20.5, 19.5);

            // VV histograms after selection
            VV_hTimeDiff_Sel[iv] = new TH1D(Form("hTimeDiff_Sel_%s", view.Data()), Form("Time wrt TriggerTime after selection, %s view;#Delta t [ns];Entries", view.Data()), 200, 91499.5, 95999.5);
            VV_hTimevsPos_Sel[iv] = new TH2D(Form("hTimeDiffvsPos_Sel_%s", view.Data()), Form("Time wrt TriggerTime vs position after selection, %s view;#Delta t [ns];%s [strip]", view.Data(), view.Data()), 200, 91499.5, 95999.5, 256, -0.5, 255.5);
            VV_hPosvsADC_Sel[iv] = new TH2D(Form("hPosvsADC_Sel_%s", view.Data()), Form("Position vs ADC after selection, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
            VV_hPosvsADC_OutofTime_Sel[iv] = new TH2D(Form("hPosvsADC_OutofTime_Sel_%s", view.Data()), Form("Position vs ADC, out of time, after selection, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 256, -0.5, 255.5, 1000, -0.5, 999.5);
            VV_hNHit_Sel[iv] = new TH1D(Form("hNHit_Sel%s", view.Data()), Form("Nhit distribution after Sel, %s view;Hit ;Entries", view.Data()), 200, -0.5, 199.5);

            // VV slope-intercept histograms
            VV_hSlopeIntercept[iv] = new TH2D(Form("hSlopeIntercept%s", view.Data()), Form("Slope vs Intercept, %s view ;Intercept [mm];Slope [mm/mm]", view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
            VV_hSlopeIntercept_event[iv] = new TH2D(Form("hSlopeIntercept_event%s", view.Data()), Form("Slope vs Intercept, %s view ;Intercept [mm];Slope [mm/mm]", view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
            VV_hSlopeIntercept_All[iv] = new TH2D(Form("hSlopeIntercept_All%s", view.Data()), Form("Slope vs Intercept, %s view ;Intercept [mm];Slope [mm/mm]", view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
         }
      }
   }  

   for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
   // for (Long64_t jentry = 0; jentry < 200000; ++jentry) {
      const Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);

      // cout << "jentry: "<< jentry << " - event: " << event << " - n_hits: " << n_hits << endl;
      // if(event!=44170){
      //     continue;
      // }
      // cout << "abbiamo trovato event: " << event << endl;
      // cout << "n_hits: " << n_hits << endl;
      
      size_t nhits = n_hits;

      int hitCounter[2] = {0, 0};
      int hitCounterMVP[2][2] = {{0, 0},{0,0}};
      
      for (size_t iH = 0; iH < nhits; ++iH) {
         hitCounter[plane->at(iH)]++;
         hitCounterMVP[det->at(iH)][plane->at(iH)]++;
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

         //filling general histograms
         hTimeDiff->Fill(time_diff);
         hTimeDiffvsPos->Fill(time_diff, pos->at(iH));
         hTimeDiffvsADC->Fill(time_diff, adc->at(iH));
         hPosvsADC->Fill(pos->at(iH), adc->at(iH));
         
         // hVPosvsVPos->Fill()

         

// Int_t ChamberMonitor::Compute2DOccupancy(){
//   for(int yl=0; yl<6; yl++) {
//     if(yl == 2 || yl == 3) continue;
//     for(int ihit=0; ihit<(int)x_mean[yl].size(); ihit++){ //Layer 0

//       double yy = x_mean[yl][ihit];
//       double qy = q_mean[yl][ihit];
//       int cy = c_mean[yl][ihit];
//       int xl=0, couple=0;
//       if(yl == 0 || yl == 1){           // P1 Y layers

//         if(cy <= fNStripsHalf) xl = 3; // X bottom layer

//         else xl = 2;                    // X top layer

//         couple = yl;
//       }
//       if(yl == 4 || yl == 5) {          // P2 Y layers

//         if(cy <= fNStripsHalf) xl = 7; // X bottom layer

//         else xl = 6;                    // X top layer

//         couple = yl-4;
//       }
//       for(int jhit=0; jhit<(int)x_mean[xl].size(); jhit++) {
//         int cx = c_mean[xl][jhit];
//         if(cx <= fNStripsHalf*(couple+1) && cx> fNStripsHalf*couple) {
//           double xx = x_mean[xl][jhit];
//           double qx = q_mean[xl][jhit];
//           if(yl < 2) {
//             hxyq_P1->Fill(xx,yy,qx*qy);
//           }
//           else {
//             hxyq_P2->Fill(xx,yy,qx*qy);
//           }
//         }
//       }
//     }
//   }

//   return 0;
// }




         MVP_hTimeDiff[det->at(iH)-1][plane->at(iH)]->Fill(time_diff);
         MVP_hTimevsPos[det->at(iH)-1][plane->at(iH)]->Fill(time_diff, pos->at(iH));
         MVP_hPosvsADC[det->at(iH)-1][plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));
         VV_hTimeDiff[plane->at(iH)]->Fill(time_diff);
         VV_hTimevsPos[plane->at(iH)]->Fill(time_diff, pos->at(iH));
         VV_hPosvsADC[plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));

         if(time_diff<92900 || time_diff>94400) {
            hPosvsADC_OutofTime->Fill(pos->at(iH), adc->at(iH));
            MVP_hPosvsADC_OutofTime[det->at(iH)-1][plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));
            VV_hPosvsADC_OutofTime[plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));
         }

         // hit pairing X/Y with Z 
         // hough transform X view
         // if(adc->at(iH)<130) continue;

         for(size_t iHH = iH+1; iHH < nhits; ++iHH){
            // if(adc->at(iHH)<130) continue;
            if(plane->at(iHH) == plane->at(iH)){
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

               VV_hSlopeIntercept_event[plane->at(iH)]->Fill(inter, slope);
               VV_hSlopeIntercept_All[plane->at(iH)]->Fill(inter, slope);
               if(det->at(iHH) == det->at(iH)) {
                  MVP_hSlopeIntercept_event[det->at(iH)-1][plane->at(iH)]->Fill(inter, slope);
                  MVP_hSlopeIntercept_All[det->at(iH)-1][plane->at(iH)]->Fill(inter, slope);
               }
            }
         }
      } //end of hit loop

      //hit distribution filling
      for(int i = 0; i<2; i++){
         VV_hNHit[i]->Fill(hitCounter[i]);
         for(int ip = 0; ip < 2; ++ip) MVP_hNHit[ip][i]->Fill(hitCounterMVP[ip][i]);

      }

      // maximum searching in the Hough tranaform space
      double SlpMax[2] = {-999, -999}; // slope max xy
      double InterMax[2] = {-999, -999}; // intercept max xy

      for (int iv = 0; iv < 2; ++iv) {
         const int nBinsX = VV_hSlopeIntercept_event[iv]->GetNbinsX();
         const int nBinsY = VV_hSlopeIntercept_event[iv]->GetNbinsY();

         double maximum = -999;
         for (int ibx = 1; ibx <= nBinsX; ++ibx) {
            for (int iby = 1; iby <= nBinsY; ++iby) {
               const double binContent = VV_hSlopeIntercept_event[iv]->GetBinContent(ibx, iby);
               if (binContent > maximum) {
                  maximum = binContent;
                  double IT = VV_hSlopeIntercept_event[iv]->GetXaxis()->GetBinCenter(ibx);
                  double SP = VV_hSlopeIntercept_event[iv]->GetYaxis()->GetBinCenter(iby);
                        
                  if(iv==0 && (fabs(SP)<0.005)) continue; // && (fabs(IT-140)<1 || fabs(IT-160)<1 || fabs(IT-129)<2)) continue;
                  // if(IT<5) continue; 
                  if(SP > -0.15) continue; // out of the beam region
                  InterMax[iv] = IT;
                  SlpMax[iv] = SP;
               }
            }
         }

         if(SlpMax[iv]<-500 || InterMax[iv]<-500) continue;
         VV_hSlopeIntercept[iv]->Fill(InterMax[iv], SlpMax[iv]);
         VV_hSlopeIntercept_event[iv]->Reset();
      }

      for(int ip = 0; ip < 2; ++ip) {
         for(int iv = 0; iv < 2; ++iv) {
            const int nBinsX = MVP_hSlopeIntercept_event[ip][iv]->GetNbinsX();
            const int nBinsY = MVP_hSlopeIntercept_event[ip][iv]->GetNbinsY();
            double maximum = -999;
            double panelInterMax = -999;
            double panelSlpMax = -999;
            for(int ibx = 1; ibx <= nBinsX; ++ibx) {
               for(int iby = 1; iby <= nBinsY; ++iby) {
                  const double binContent = MVP_hSlopeIntercept_event[ip][iv]->GetBinContent(ibx, iby);
                  if(binContent > maximum) {
                     const double IT = MVP_hSlopeIntercept_event[ip][iv]->GetXaxis()->GetBinCenter(ibx);
                     const double SP = MVP_hSlopeIntercept_event[ip][iv]->GetYaxis()->GetBinCenter(iby);
                     if(iv == 0 && fabs(SP) < 0.005) continue;
                     if(SP > -0.15) continue;
                     maximum = binContent;
                     panelInterMax = IT;
                     panelSlpMax = SP;
                  }
               }
            }
            if(SlpMax[iv]<-500 || InterMax[iv]<-500) continue;
            MVP_hSlopeIntercept[ip][iv]->Fill(panelInterMax, panelSlpMax);
            MVP_hSlopeIntercept_event[ip][iv]->Reset();
         }
      }

      //preparing graphs and variables for the event display
      bool saveEventDisplay = false;
      TGraph* gTrack[2] = {nullptr, nullptr};
      TF1* fTrack[2] = {nullptr, nullptr};

      if(saveEventDisplay) {
         gTrack[0] = new TGraph();
         gTrack[1] = new TGraph();
         TGraphAttribute(gTrack[0], Form("gTrack_event_%d_X", event), "z [mm]", "X [mm]", 20, kRed+1);
         TGraphAttribute(gTrack[1], Form("gTrack_event_%d_Y", event), "z [mm]", "Y [mm]", 20, kBlue+1);
      }

      vector<bool> acceptedHits(nhits, false); // to select only the hits belonging to the road
      
      //do the road:
      for (size_t iH = 0; iH < nhits; ++iH) {

         int currentPlane = plane->at(iH);

         double vpos = pos->at(iH)*pitch;
         double time_diff = time->at(iH) - trigger_time->at(iH) * 22.5;
         double z = (time_diff - T0) * DriftVelocity; // time shifted at 0 ns
         
         // cout << "pre if - det->at(iH): " << det->at(iH) << endl;
         if(det->at(iH)==1){ // back face
            z = z - driftgap;
         } else if(det->at(iH)==2){ // front face
            z = - z + driftgap;
         }
         
         for (size_t iHH = iH+1; iHH < nhits; ++iHH) {
            if (plane->at(iHH) != plane->at(iH)) continue;
            double vpos2 = pos->at(iHH)*pitch;
            double deltaV = vpos - vpos2;
            
            double time_diff2 = time->at(iHH) - trigger_time->at(iHH) * 22.5;
            double z2 = (time_diff2 - T0) * DriftVelocity; // time shifted at 0 ns
            
            // cout << "pre if - det->at(iH): " << det->at(iH) << endl;
            if(det->at(iHH)==1){ // back face
               z2 = z2 - driftgap;
            } else if(det->at(iHH)==2){ // front face
               z2 = - z2 + driftgap;
            }

            double deltaZ = z - z2;
            double slope = deltaV/deltaZ;
            double inter = vpos2 - slope * z2;
            // double road = (vpos - InterMax[plane->at(iH)] - SlpMax[plane->at(iH)]*z)/pitch;
            
            if(fabs(slope - SlpMax[plane->at(iH)])<.02 && fabs(inter - InterMax[plane->at(iH)])<3){
               acceptedHits[iH] = true;
               acceptedHits[iHH] = true;
            }
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

      int hitCounter_Sel[2] = {0,0};
      int hitCounterMVP_Sel[2][2] = {{0, 0}, {0, 0}};

      for (size_t iH = 0; iH < nhits; ++iH) {
         if(!acceptedHits.at(iH)) continue;
         double vpos = pos->at(iH)*pitch;
         double time_diff = time->at(iH) - trigger_time->at(iH) * 22.5;
         
         double z = (time_diff - T0) * DriftVelocity; // time shifted at 0 ns
         
         if(det->at(iH)==1){ // back face
            z = z - driftgap;
         } else if(det->at(iH)==2){ // front face
            z = - z + driftgap;
         }

         z_avg[plane->at(iH)] += z;
         zv_avg[plane->at(iH)] += z * vpos;
         v_avg[plane->at(iH)] += vpos;
         z2_avg[plane->at(iH)] += z * z;
         n_avg[plane->at(iH)]++;
         hitsroad[plane->at(iH)]++;
         hitCounter_Sel[plane->at(iH)]++;
         hitCounterMVP_Sel[det->at(iH)][plane->at(iH)]++;

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

         VV_hMvsC[i]->Fill(ct_avg[i], mt_avg[i]);
         for(int ip = 0; ip < 2; ++ip) if(hitCounterMVP_Sel[ip][i] > 0) MVP_hMvsC[ip][i]->Fill(ct_avg[i], mt_avg[i]);
      }

      // selected hit distribution filling
      for(int i = 0; i<2; i++){
         VV_hNHit_Sel[i]->Fill(hitCounter_Sel[i]);
         for(int ip = 0; ip < 2; ++ip) MVP_hNHit_Sel[ip][i]->Fill(hitCounterMVP_Sel[ip][i]);
      }

      bool saveEventDisplaySel = false;
      for (size_t iH = 0; iH < nhits; ++iH) {
         if(!acceptedHits.at(iH)) continue;
         // saveEventDisplaySel = true;
         double vpos = pos->at(iH)*pitch;
         double time_diff = time->at(iH) - trigger_time->at(iH) * 22.5;
         
         double z = (time_diff - T0) * DriftVelocity; // time shifted at 0 ns
         if(det->at(iH)==1){ // back face
            z = z - driftgap;
         } else if(det->at(iH)==2){ // front face
            z = - z + driftgap;
         }

         if(saveEventDisplay && saveEventDisplaySel) gTrack[plane->at(iH)]->SetPoint(gTrack[plane->at(iH)]->GetN(), z, vpos);

         if(plane->at(iH)==0 && mt_avg[0]>-0.15) continue; // slope cut on X only

         double tDrift = time_diff - T0;
         double vpos_fit = mt_avg[plane->at(iH)]*z + ct_avg[plane->at(iH)];
         double zpos_fit = (vpos - ct_avg[plane->at(iH)])/mt_avg[plane->at(iH)];
         if(hitCounter_Sel[plane->at(iH)]>5){
            // resolution computed only for tracks with a large number of hits
            // cout << "events: "<< event << " - hitCounter_Sel[plane->at(iH)]: " << hitCounter_Sel[plane->at(iH)] << endl;
            VV_hVResvsC[plane->at(iH)]->Fill(ct_avg[plane->at(iH)], vpos_fit - vpos);
            VV_hVResvsM[plane->at(iH)]->Fill(mt_avg[plane->at(iH)], vpos_fit - vpos);
            VV_hVResvsVhit[plane->at(iH)]->Fill(vpos, vpos_fit - vpos);
            VV_hZResvsZhit[plane->at(iH)]->Fill(z, zpos_fit-z);
            VV_hZResvsADC[plane->at(iH)]->Fill(adc->at(iH), zpos_fit-z);
            VV_hZResvsM[plane->at(iH)]->Fill(mt_avg[plane->at(iH)], zpos_fit-z);
            MVP_hZfitvsTimeDiff[det->at(iH)-1][plane->at(iH)]->Fill(tDrift, zpos_fit);
            VV_hZfitvsTimeDiff[plane->at(iH)]->Fill(tDrift, zpos_fit);
            MVP_hVResvsC[det->at(iH)-1][plane->at(iH)]->Fill(ct_avg[plane->at(iH)], vpos_fit - vpos);
            MVP_hVResvsM[det->at(iH)-1][plane->at(iH)]->Fill(mt_avg[plane->at(iH)], vpos_fit - vpos);
            MVP_hVResvsVhit[det->at(iH)-1][plane->at(iH)]->Fill(vpos, vpos_fit - vpos);
            MVP_hZResvsZhit[det->at(iH)-1][plane->at(iH)]->Fill(z, zpos_fit - z);
            MVP_hZResvsADC[det->at(iH)-1][plane->at(iH)]->Fill(adc->at(iH), zpos_fit - z);
            MVP_hZResvsM[det->at(iH)-1][plane->at(iH)]->Fill(mt_avg[plane->at(iH)], zpos_fit - z);
         
         }

         hTimeDiff_Sel->Fill(time_diff);
         MVP_hTimeDiff_Sel[det->at(iH)-1][plane->at(iH)]->Fill(time_diff);
         hTimeDiffvsADC_Sel->Fill(time_diff, adc->at(iH));
         hPosvsADC_Sel->Fill(pos->at(iH), adc->at(iH));
         hTimeDiffvsPos_Sel->Fill(time_diff, pos->at(iH));
         MVP_hTimevsPos_Sel[det->at(iH)-1][plane->at(iH)]->Fill(time_diff, pos->at(iH));
         MVP_hPosvsADC_Sel[det->at(iH)-1][plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));
         VV_hTimeDiff_Sel[plane->at(iH)]->Fill(time_diff);
         VV_hTimevsPos_Sel[plane->at(iH)]->Fill(time_diff, pos->at(iH));
         VV_hPosvsADC_Sel[plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));

         if(time_diff<92900 || time_diff>94400) {
            hPosvsADC_OutofTime_Sel->Fill(pos->at(iH), adc->at(iH));
            MVP_hPosvsADC_OutofTime_Sel[det->at(iH)-1][plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));
            VV_hPosvsADC_OutofTime_Sel[plane->at(iH)]->Fill(pos->at(iH), adc->at(iH));
         }

      }

      if(saveEventDisplay && saveEventDisplaySel) {
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
   } // end of loop over entries
   
   outputFile->cd();
   hDir->cd();

   hTimeDiff->Write();
   hTimeDiffvsPos->Write();
   hTimeDiffvsADC->Write();
   hPosvsADC->Write();
   hPosvsADC_OutofTime->Write();
   hTimeDiff_Sel->Write();
   hTimeDiffvsPos_Sel->Write();
   hTimeDiffvsADC_Sel->Write();
   hPosvsADC_Sel->Write();
   hPosvsADC_OutofTime_Sel->Write();

   for(int iv = 0; iv < 2; ++iv) {
      hViewsDir[iv]->cd();

      VV_hTimeDiff[iv]->Write();
      VV_hTimevsPos[iv]->Write();
      VV_hPosvsADC[iv]->Write();
      VV_hPosvsADC_OutofTime[iv]->Write();
      VV_hZfitvsTimeDiff[iv]->Write();
      VV_hNHit[iv]->Write();
      VV_hTimeDiff_Sel[iv]->Write();
      VV_hTimevsPos_Sel[iv]->Write();
      VV_hPosvsADC_Sel[iv]->Write();
      VV_hPosvsADC_OutofTime_Sel[iv]->Write();
      VV_hNHit_Sel[iv]->Write();
      VV_hMvsC[iv]->Write();
      VV_hVResvsC[iv]->Write();
      VV_hVResvsM[iv]->Write();
      VV_hVResvsVhit[iv]->Write();
      VV_hZResvsZhit[iv]->Write();
      VV_hZResvsADC[iv]->Write();
      VV_hZResvsM[iv]->Write();
      VV_hSlopeIntercept[iv]->Write();
      VV_hSlopeIntercept_All[iv]->Write();

      for(int ip = 0; ip < 2; ++ip) {
         if(iv==0) hPanelDirX[ip]->cd();
         if(iv==1) hPanelDirY[ip]->cd();

         MVP_hTimeDiff[ip][iv]->Write();
         MVP_hTimevsPos[ip][iv]->Write();
         MVP_hPosvsADC[ip][iv]->Write();
         MVP_hPosvsADC_OutofTime[ip][iv]->Write();
         MVP_hZfitvsTimeDiff[ip][iv]->Write();
         MVP_hNHit[ip][iv]->Write();
         MVP_hMvsC[ip][iv]->Write();
         MVP_hVResvsC[ip][iv]->Write();
         MVP_hVResvsM[ip][iv]->Write();
         MVP_hVResvsVhit[ip][iv]->Write();
         MVP_hZResvsZhit[ip][iv]->Write();
         MVP_hZResvsADC[ip][iv]->Write();
         MVP_hZResvsM[ip][iv]->Write();
         MVP_hTimeDiff_Sel[ip][iv]->Write();
         MVP_hTimevsPos_Sel[ip][iv]->Write();
         MVP_hPosvsADC_Sel[ip][iv]->Write();
         MVP_hPosvsADC_OutofTime_Sel[ip][iv]->Write();
         MVP_hNHit_Sel[ip][iv]->Write();
         MVP_hSlopeIntercept[ip][iv]->Write();
         MVP_hSlopeIntercept_All[ip][iv]->Write();
      }
   }
   
   cout << "Processed tree entries: " << nentries << endl;
   cout << "Selected hits: " << hTimeDiff_Sel->GetEntries() << endl;
   cout << "Output written to " << outputFileName << endl;
}