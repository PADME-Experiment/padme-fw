#define RecoVMM_cxx
#include "RecoVMM.h"

#include <TFile.h>

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

namespace {

constexpr int NPLANES = 3;
constexpr int NVIEWS = 2;
constexpr int XVIEW = 0;
constexpr int YVIEW = 1;

const char* VIEW_NAME[NVIEWS] = {"X", "Y"};
const char* PLANE_NAME[NPLANES] = {"Plane0_Y", "Plane1_X", "Plane2_Y"};
const int PLANE_TO_VIEW[NPLANES] = {YVIEW, XVIEW, YVIEW};

int ViewFromPlane(int plane)
{
   if (plane < 0 || plane >= NPLANES) return -1;
   return PLANE_TO_VIEW[plane];
}

int TGraphAttribute(TGraph *graph, TString title, TString xlabel, TString ylabel, int markerstyle, int color)
{
   if (!graph) return 1;
   graph->SetTitle(title);
   graph->SetName(title);
   graph->GetXaxis()->SetTitle(xlabel);
   graph->GetYaxis()->SetTitle(ylabel);
   graph->SetMarkerStyle(markerstyle);
   graph->SetMarkerSize(1.8);
   graph->SetMarkerColor(color);
   graph->UseCurrentStyle();
   return 0;
}

} // namespace

void RecoVMM::Loop()
{
   if (!fChain) {
      cerr << "RecoVMM::Loop: input tree is null" << endl;
      return;
   }

   const TString outputFileName = Form("Reco_Events_run%s.root", fRunName.Data());
   TFile *outputFile = new TFile(outputFileName, "RECREATE");
   if (!outputFile || outputFile->IsZombie()) {
      cerr << "Cannot create output ROOT file " << outputFileName << endl;
      delete outputFile;
      return;
   }

   outputFile->cd();

   TDirectory *hDir = outputFile->mkdir("Histograms");
   TDirectory *hViewsDir[NVIEWS] = {hDir->mkdir("X"), hDir->mkdir("Y")};
   TDirectory *hPlaneDir[NPLANES];
   hPlaneDir[0] = hViewsDir[YVIEW]->mkdir("Plane0");
   hPlaneDir[1] = hViewsDir[XVIEW]->mkdir("Plane1");
   hPlaneDir[2] = hViewsDir[YVIEW]->mkdir("Plane2");

   TDirectory *evtdisplayDir = outputFile->mkdir("EventDisplays");
   TDirectory *evtdisplayViewsDir[NVIEWS] = {
      evtdisplayDir->mkdir("X"),
      evtdisplayDir->mkdir("Y")
   };

   const Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nb = 0;

   // Overall monitoring.
   TH1D *hTimeDiff = new TH1D("hTimeDiff", "Time wrt TriggerTime;#Delta t [ns];Entries", 200, 91499.5, 95999.5);
   TH1D *hADC = new TH1D("hADC", "ADC distribution overall;ADC;Entries", 1200, -0.5, 1199.5);
   TH2D *hTimeDiffvsPos = new TH2D("hTimeDiffvsPos", "Time - TriggerTime vs position;#Delta t [ns];Position [strip]", 200, 91499.5, 95999.5, 1024, -0.5, 1023.5);
   TH2D *hTimeDiffvsADC = new TH2D("hTimeDiffvsADC", "Time - TriggerTime vs ADC;#Delta t [ns];Q [adc]", 200, 91499.5, 95999.5, 1200, -0.5, 1199.5);
   TH2D *hPosvsADC = new TH2D("hPosvsADC", "Position vs ADC;Position [strip];Q [adc]", 1024, -0.5, 1023.5, 1200, -0.5, 1199.5);
   TH2D *hPosvsADC_OutofTime = new TH2D("hPosvsADC_OutofTime", "Position vs ADC, out of time;Position [strip];Q [adc]", 1024, -0.5, 1023.5, 1200, -0.5, 1199.5);

   TH1D *hTimeDiff_Sel = new TH1D("hTimeDiff_Sel", "Time wrt TriggerTime after selection;#Delta t [ns];Entries", 200, 91499.5, 95999.5);
   TH1D *hADC_Sel = new TH1D("hADC_Sel", "ADC distribution overall;ADC;Entries", 1200, -0.5, 1199.5);
   TH2D *hTimeDiffvsPos_Sel = new TH2D("hTimeDiffvsPos_Sel", "Time wrt TriggerTime vs position after selection;#Delta t [ns];Position [strip]", 200, 91499.5, 95999.5, 1024, -0.5, 1023.5);
   TH2D *hTimeDiffvsADC_Sel = new TH2D("hTimeDiffvsADC_Sel", "Time - TriggerTime vs ADC after selection;#Delta t [ns];Q [adc]", 200, 91499.5, 95999.5, 1200, -0.5, 1199.5);
   TH2D *hPosvsADC_Sel = new TH2D("hPosvsADC_Sel", "Position vs ADC after selection;Position [strip];Q [adc]", 1024, -0.5, 1023.5, 1200, -0.5, 1199.5);
   TH2D *hPosvsADC_OutofTime_Sel = new TH2D("hPosvsADC_OutofTime_Sel", "Position vs ADC, out of time, after selection;Position [strip];Q [adc]", 1024, -0.5, 1023.5, 1200, -0.5, 1199.5);

   TH2D *hNHitP1vsP2 = new TH2D("hNHitP1vsP2", "Hits association between P1 and P2;P1 hits per event;P2 hits per event", 100, 0, 100, 100, 0, 100); //x>256

   // View-level histograms: X = Plane1, Y = Plane0 + Plane2.
   TH1D *VV_hTimeDiff[NVIEWS];
   TH1D *VV_hADC[NVIEWS];
   TH2D *VV_hTimevsPos[NVIEWS];
   TH2D *VV_hPosvsADC[NVIEWS];
   TH2D *VV_hPosvsADC_OutofTime[NVIEWS];
   TH2D *VV_hZfitvsTimeDiff[NVIEWS];
   TH1D *VV_hNHit[NVIEWS];
   TH2D *VV_hMvsC[NVIEWS];
   TH2D *VV_hVResvsC[NVIEWS];
   TH2D *VV_hVResvsM[NVIEWS];
   TH2D *VV_hVResvsVhit[NVIEWS];
   TH2D *VV_hZResvsZhit[NVIEWS];
   TH2D *VV_hZResvsADC[NVIEWS];
   TH2D *VV_hZResvsM[NVIEWS];
   TH1D *VV_hTimeDiff_Sel[NVIEWS];
   TH1D *VV_hADC_Sel[NVIEWS];
   TH2D *VV_hTimevsPos_Sel[NVIEWS];
   TH2D *VV_hPosvsADC_Sel[NVIEWS];
   TH2D *VV_hPosvsADC_OutofTime_Sel[NVIEWS];
   TH1D *VV_hNHit_Sel[NVIEWS];
   TH2D *VV_hSlopeIntercept[NVIEWS];
   TH2D *VV_hSlopeIntercept_All[NVIEWS];
   TH2D *VV_hSlopeIntercept_event[NVIEWS];

   // Physical-plane monitoring: Plane0(Y), Plane1(X), Plane2(Y).
   TH1D *P_hTimeDiff[NPLANES];
   TH1D *P_hADC[NPLANES];
   TH2D *P_hTimevsPos[NPLANES];
   TH2D *P_hPosvsADC[NPLANES];
   TH2D *P_hPosvsADC_OutofTime[NPLANES];
   TH2D *P_hZfitvsTimeDiff[NPLANES];
   TH1D *P_hNHit[NPLANES];
   TH2D *P_hMvsC[NPLANES];
   TH2D *P_hVResvsC[NPLANES];
   TH2D *P_hVResvsM[NPLANES];
   TH2D *P_hVResvsVhit[NPLANES];
   TH2D *P_hZResvsZhit[NPLANES];
   TH2D *P_hZResvsADC[NPLANES];
   TH2D *P_hZResvsM[NPLANES];
   TH1D *P_hTimeDiff_Sel[NPLANES];
   TH1D *P_hADC_Sel[NPLANES];
   TH2D *P_hTimevsPos_Sel[NPLANES];
   TH2D *P_hPosvsADC_Sel[NPLANES];
   TH2D *P_hPosvsADC_OutofTime_Sel[NPLANES];
   TH1D *P_hNHit_Sel[NPLANES];

   for (int iv = 0; iv < NVIEWS; ++iv) {
      TString view = VIEW_NAME[iv];

      VV_hTimeDiff[iv] = new TH1D(Form("hTimeDiff_%s", view.Data()), Form("Time wrt TriggerTime, %s view;#Delta t [ns];Entries", view.Data()), 200, 91499.5, 95999.5);
      VV_hADC[iv] = new TH1D(Form("hADC_%s", view.Data()), Form("ADC distribution, %s view;ADC;Entries", view.Data()), 1200, -0.5, 1199.5);
      VV_hTimevsPos[iv] = new TH2D(Form("hTimeDiffvsPos_%s", view.Data()), Form("Time wrt TriggerTime vs position, %s view;#Delta t [ns];%s [strip]", view.Data(), view.Data()), 200, 91499.5, 95999.5, 512, -0.5, 511.5);
      VV_hPosvsADC[iv] = new TH2D(Form("hPosvsADC_%s", view.Data()), Form("Position vs ADC, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      VV_hPosvsADC_OutofTime[iv] = new TH2D(Form("hPosvsADC_OutofTime_%s", view.Data()), Form("Position vs ADC, out of time, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      VV_hZfitvsTimeDiff[iv] = new TH2D(Form("hZfitvsTimeDiff_%s", view.Data()), Form("Fitted Z vs time, %s view;#Delta t [ns];Fitted Z [mm]", view.Data()), 200, -1000.5, 999.5, 50, -0.5, 99.5);
      VV_hNHit[iv] = new TH1D(Form("hNHit_%s", view.Data()), Form("Nhit distribution, %s view;Hits per event;Entries", view.Data()), 100, -0.5, 99.5);
      VV_hMvsC[iv] = new TH2D(Form("hMvsC_%s", view.Data()), Form("Track slope vs intercept, %s view;Intercept [mm];Slope [mm/mm]", view.Data()), 400, -200.5, 199.5, 200, -1.005, 0.995);
      VV_hVResvsC[iv] = new TH2D(Form("hVResvsC_%s", view.Data()), Form("%s residual vs intercept;Intercept [mm];%s residual [mm]", view.Data(), view.Data()), 400, -200.5, 199.5, 200, -10.05, 9.95);
      VV_hVResvsM[iv] = new TH2D(Form("hVResvsM_%s", view.Data()), Form("%s residual vs slope;Slope [mm/mm];%s residual [mm]", view.Data(), view.Data()), 200, -1.005, 0.995, 200, -10.05, 9.95);
      VV_hVResvsVhit[iv] = new TH2D(Form("hVResvsV_%s", view.Data()), Form("%s residual vs hit position;%s hit position [mm];%s residual [mm]", view.Data(), view.Data(), view.Data()), 256, -0.5*pitch, 255.5*pitch, 200, -10.05, 9.95);
      VV_hZResvsZhit[iv] = new TH2D(Form("hZResvsZ_%s", view.Data()), Form("Z residual vs Z position, %s view;Z [mm];Z residual [mm]", view.Data()), 100, -50.5, 49.5, 1600, -20.5, 19.5);
      VV_hZResvsADC[iv] = new TH2D(Form("hZResvsADC_%s", view.Data()), Form("Z residual vs charge, %s view;Q [ADC];Z residual [mm]", view.Data()), 100, -0.5, 999.5, 1600, -20.5, 19.5);
      VV_hZResvsM[iv] = new TH2D(Form("hZResvsM_%s", view.Data()), Form("Z residual vs slope, %s view;Slope [mm/mm];Z residual [mm]", view.Data()), 200, -1.005, 0.995, 1600, -20.5, 19.5);

      VV_hTimeDiff_Sel[iv] = new TH1D(Form("hTimeDiff_Sel_%s", view.Data()), Form("Time wrt TriggerTime after selection, %s view;#Delta t [ns];Entries", view.Data()), 200, 91499.5, 95999.5);
      VV_hADC_Sel[iv] = new TH1D(Form("hADC_Sel_%s", view.Data()), Form("ADC distribution after selection, %s view;ADC;Entries", view.Data()), 1200, -0.5, 1199.5);
      VV_hTimevsPos_Sel[iv] = new TH2D(Form("hTimeDiffvsPos_Sel_%s", view.Data()), Form("Time wrt TriggerTime vs position after selection, %s view;#Delta t [ns];%s [strip]", view.Data(), view.Data()), 200, 91499.5, 95999.5, 512, -0.5, 511.5);
      VV_hPosvsADC_Sel[iv] = new TH2D(Form("hPosvsADC_Sel_%s", view.Data()), Form("Position vs ADC after selection, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      VV_hPosvsADC_OutofTime_Sel[iv] = new TH2D(Form("hPosvsADC_OutofTime_Sel_%s", view.Data()), Form("Position vs ADC, out of time, after selection, %s view;%s [strip];Q [ADC]", view.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      VV_hNHit_Sel[iv] = new TH1D(Form("hNHit_Sel_%s", view.Data()), Form("Nhit distribution after selection, %s view;Hits per event;Entries", view.Data()), 100, -0.5, 99.5);

      VV_hSlopeIntercept[iv] = new TH2D(Form("hSlopeIntercept_%s", view.Data()), Form("Selected slope vs intercept, %s view;Intercept [mm];Slope [mm/mm]", view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
      VV_hSlopeIntercept_event[iv] = new TH2D(Form("hSlopeIntercept_event_%s", view.Data()), Form("Slope vs intercept per event, %s view;Intercept [mm];Slope [mm/mm]", view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
      VV_hSlopeIntercept_All[iv] = new TH2D(Form("hSlopeIntercept_All_%s", view.Data()), Form("All slope-intercept combinations, %s view;Intercept [mm];Slope [mm/mm]", view.Data()), 100, -50.5, 349.5, 120, -0.3005, 0.2995);
   }

   for (int ip = 0; ip < NPLANES; ++ip) {
      TString planeName = PLANE_NAME[ip];
      TString view = VIEW_NAME[ViewFromPlane(ip)];

      P_hTimeDiff[ip] = new TH1D(Form("hTimeDiff_%s", planeName.Data()), Form("Time wrt TriggerTime, %s;#Delta t [ns];Entries", planeName.Data()), 200, 91499.5, 95999.5);
      P_hADC[ip] = new TH1D(Form("hADC_%s", planeName.Data()), Form("ADC distribution, %s;ADC;Entries", planeName.Data()), 1200, -0.5, 1199.5);
      P_hTimevsPos[ip] = new TH2D(Form("hTimeDiffvsPos_%s", planeName.Data()), Form("Time wrt TriggerTime vs position, %s;#Delta t [ns];%s [strip]", planeName.Data(), view.Data()), 200, 91499.5, 95999.5, 512, -0.5, 511.5);
      P_hPosvsADC[ip] = new TH2D(Form("hPosvsADC_%s", planeName.Data()), Form("Position vs ADC, %s;%s [strip];Q [ADC]", planeName.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      P_hPosvsADC_OutofTime[ip] = new TH2D(Form("hPosvsADC_OutofTime_%s", planeName.Data()), Form("Position vs ADC, out of time, %s;%s [strip];Q [ADC]", planeName.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      P_hZfitvsTimeDiff[ip] = new TH2D(Form("hZfitvsTimeDiff_%s", planeName.Data()), Form("Fitted Z vs time, %s;#Delta t [ns];Fitted Z [mm]", planeName.Data()), 200, -1000.5, 999.5, 50, -0.5, 99.5);
      P_hNHit[ip] = new TH1D(Form("hNHit_%s", planeName.Data()), Form("Number of hits, %s;Hits per event;Entries", planeName.Data()), 100, -0.5, 99.5);
      P_hMvsC[ip] = new TH2D(Form("hMvsC_%s", planeName.Data()), Form("Track slope vs intercept for hits in %s;Intercept [mm];Slope [mm/mm]", planeName.Data()), 400, -200.5, 199.5, 200, -1.005, 0.995);
      P_hVResvsC[ip] = new TH2D(Form("hVResvsC_%s", planeName.Data()), Form("%s residual vs intercept, %s;Intercept [mm];Residual [mm]", view.Data(), planeName.Data()), 400, -200.5, 199.5, 200, -10.05, 9.95);
      P_hVResvsM[ip] = new TH2D(Form("hVResvsM_%s", planeName.Data()), Form("%s residual vs slope, %s;Slope [mm/mm];Residual [mm]", view.Data(), planeName.Data()), 200, -1.005, 0.995, 200, -10.05, 9.95);
      P_hVResvsVhit[ip] = new TH2D(Form("hVResvsVhit_%s", planeName.Data()), Form("Residual vs hit position, %s;Hit position [mm];Residual [mm]", planeName.Data()), 256, -0.5*pitch, 255.5*pitch, 200, -10.05, 9.95);
      P_hZResvsZhit[ip] = new TH2D(Form("hZResvsZhit_%s", planeName.Data()), Form("Z residual vs Z position, %s;Z [mm];Z residual [mm]", planeName.Data()), 100, -50.5, 49.5, 1600, -20.5, 19.5);
      P_hZResvsADC[ip] = new TH2D(Form("hZResvsADC_%s", planeName.Data()), Form("Z residual vs charge, %s;Q [ADC];Z residual [mm]", planeName.Data()), 100, -0.5, 999.5, 1600, -20.5, 19.5);
      P_hZResvsM[ip] = new TH2D(Form("hZResvsM_%s", planeName.Data()), Form("Z residual vs slope, %s;Slope [mm/mm];Z residual [mm]", planeName.Data()), 200, -1.005, 0.995, 1600, -20.5, 19.5);

      P_hTimeDiff_Sel[ip] = new TH1D(Form("hTimeDiff_Sel_%s", planeName.Data()), Form("Time wrt TriggerTime after selection, %s;#Delta t [ns];Entries", planeName.Data()), 200, 91499.5, 95999.5);
      P_hADC_Sel[ip] = new TH1D(Form("hADC_Sel_%s", planeName.Data()), Form("ADC distribution after selection, %s;ADC;Entries", planeName.Data()), 1200, -0.5, 1199.5);
      P_hTimevsPos_Sel[ip] = new TH2D(Form("hTimeDiffvsPos_Sel_%s", planeName.Data()), Form("Time wrt TriggerTime vs position after selection, %s;#Delta t [ns];%s [strip]", planeName.Data(), view.Data()), 200, 91499.5, 95999.5, 512, -0.5, 511.5);
      P_hPosvsADC_Sel[ip] = new TH2D(Form("hPosvsADC_Sel_%s", planeName.Data()), Form("Position vs ADC after selection, %s;%s [strip];Q [ADC]", planeName.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      P_hPosvsADC_OutofTime_Sel[ip] = new TH2D(Form("hPosvsADC_OutofTime_Sel_%s", planeName.Data()), Form("Position vs ADC, out of time, after selection, %s;%s [strip];Q [ADC]", planeName.Data(), view.Data()), 512, -0.5, 511.5, 1200, -0.5, 1199.5);
      P_hNHit_Sel[ip] = new TH1D(Form("hNHit_Sel_%s", planeName.Data()), Form("Number of hits after selection, %s;Hits per event;Entries", planeName.Data()), 100, -0.5, 99.5);
   }

   // All three planes are on the front side. det is not used for geometry anymore.
   auto GetZ = [&](double hitTime, double hitTriggerTime) {
      const double timeDiff = hitTime - hitTriggerTime * 22.5;
      const double zDrift = (timeDiff - T0) * DriftVelocity;
      return -zDrift + driftgap;
   };

   for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
      const Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);
      if (nb <= 0) continue;

      if (!plane || !pos || !time || !trigger_time || !adc) {
         cerr << "Null hit-vector branch at entry " << jentry << endl;
         continue;
      }

      size_t nhits = static_cast<size_t>(max(0, n_hits));
      nhits = min(nhits, plane->size());
      nhits = min(nhits, pos->size());
      nhits = min(nhits, time->size());
      nhits = min(nhits, trigger_time->size());
      nhits = min(nhits, adc->size());

      int hitCounterView[NVIEWS] = {0, 0};
      int hitCounterPlane[NPLANES] = {0, 0, 0};
      int hitCounterP1Dx = 0; //per hits association

      // Fill raw monitoring and build the Hough space per coordinate view.
      for (size_t iH = 0; iH < nhits; ++iH) {
         const int p = plane->at(iH);
         const int iv = ViewFromPlane(p);
         if (iv < 0) continue;

         ++hitCounterView[iv];
         ++hitCounterPlane[p];
         if(p == 1 && pos->at(iH)>255) ++hitCounterP1Dx;

         const double vpos = pos->at(iH) * pitch;
         const double time_diff = time->at(iH) - trigger_time->at(iH) * 22.5;
         const double z = GetZ(time->at(iH), trigger_time->at(iH));

         hTimeDiff->Fill(time_diff);
         hADC->Fill(adc->at(iH));
         hTimeDiffvsPos->Fill(time_diff, pos->at(iH));
         hTimeDiffvsADC->Fill(time_diff, adc->at(iH));
         hPosvsADC->Fill(pos->at(iH), adc->at(iH));

         VV_hTimeDiff[iv]->Fill(time_diff);
         VV_hADC[iv]->Fill(adc->at(iH));
         VV_hTimevsPos[iv]->Fill(time_diff, pos->at(iH));
         VV_hPosvsADC[iv]->Fill(pos->at(iH), adc->at(iH));

         P_hTimeDiff[p]->Fill(time_diff);
         P_hADC[p]->Fill(adc->at(iH));
         P_hTimevsPos[p]->Fill(time_diff, pos->at(iH));
         P_hPosvsADC[p]->Fill(pos->at(iH), adc->at(iH));

         if (time_diff < 92900 || time_diff > 94400) {
            hPosvsADC_OutofTime->Fill(pos->at(iH), adc->at(iH));
            VV_hPosvsADC_OutofTime[iv]->Fill(pos->at(iH), adc->at(iH));
            P_hPosvsADC_OutofTime[p]->Fill(pos->at(iH), adc->at(iH));
         }

         for (size_t iHH = iH + 1; iHH < nhits; ++iHH) {
            const int p2 = plane->at(iHH);
            const int iv2 = ViewFromPlane(p2);
            if (iv2 != iv) continue;

            const double vpos2 = pos->at(iHH) * pitch;
            const double z2 = GetZ(time->at(iHH), trigger_time->at(iHH));
            const double deltaZ = z - z2;
            if (fabs(deltaZ) < 1.e-9) continue;

            const double slope = (vpos - vpos2) / deltaZ;
            const double inter = vpos2 - slope * z2;

            VV_hSlopeIntercept_event[iv]->Fill(inter, slope);
            VV_hSlopeIntercept_All[iv]->Fill(inter, slope);
         }
      }
      hNHitP1vsP2->Fill(hitCounterP1Dx, hitCounterPlane[2]); //hit counter per efficienza

      for (int iv = 0; iv < NVIEWS; ++iv) VV_hNHit[iv]->Fill(hitCounterView[iv]);
      for (int p = 0; p < NPLANES; ++p) P_hNHit[p]->Fill(hitCounterPlane[p]);

      double SlpMax[NVIEWS] = {-999., -999.};
      double InterMax[NVIEWS] = {-999., -999.};

      for (int iv = 0; iv < NVIEWS; ++iv) {
         double maximum = -1.;
         TH2D *hHough = VV_hSlopeIntercept_event[iv];

         for (int ibx = 1; ibx <= hHough->GetNbinsX(); ++ibx) {
            for (int iby = 1; iby <= hHough->GetNbinsY(); ++iby) {
               const double content = hHough->GetBinContent(ibx, iby);
               if (content <= maximum) continue;

               const double inter = hHough->GetXaxis()->GetBinCenter(ibx);
               const double slope = hHough->GetYaxis()->GetBinCenter(iby);

               if (iv == XVIEW && fabs(slope) < 0.005) continue;
               if (slope > -0.15) continue;

               maximum = content;
               InterMax[iv] = inter;
               SlpMax[iv] = slope;
            }
         }

         if (SlpMax[iv] > -500. && InterMax[iv] > -500.)
            VV_hSlopeIntercept[iv]->Fill(InterMax[iv], SlpMax[iv]);

         // Must be reset for every event, also when no acceptable maximum was found.
         hHough->Reset();
      }

      bool saveEventDisplay = (event == 35355);
      TGraph *gTrack[NVIEWS] = {nullptr, nullptr};
      TF1 *fTrack[NVIEWS] = {nullptr, nullptr};

      if (saveEventDisplay) {
         for (int iv = 0; iv < NVIEWS; ++iv) {
            gTrack[iv] = new TGraph();
            TGraphAttribute(
               gTrack[iv],
               Form("gTrack_event_%d_%s", event, VIEW_NAME[iv]),
               "z [mm]",
               Form("%s [mm]", VIEW_NAME[iv]),
               20,
               (iv == XVIEW) ? kRed + 1 : kBlue + 1
            );
         }
      }

      vector<bool> acceptedHits(nhits, false);

      // Road selection is per VIEW, not per physical plane.
      // Therefore Plane0 and Plane2 contribute to the same Y road.
      for (size_t iH = 0; iH < nhits; ++iH) {
         const int p = plane->at(iH);
         const int iv = ViewFromPlane(p);
         if (iv < 0 || SlpMax[iv] < -500. || InterMax[iv] < -500.) continue;

         const double vpos = pos->at(iH) * pitch;
         const double z = GetZ(time->at(iH), trigger_time->at(iH));

         for (size_t iHH = iH + 1; iHH < nhits; ++iHH) {
            const int p2 = plane->at(iHH);
            if (ViewFromPlane(p2) != iv) continue;

            const double vpos2 = pos->at(iHH) * pitch;
            const double z2 = GetZ(time->at(iHH), trigger_time->at(iHH));
            const double deltaZ = z - z2;
            if (fabs(deltaZ) < 1.e-9) continue;

            const double slope = (vpos - vpos2) / deltaZ;
            const double inter = vpos2 - slope * z2;

            if (fabs(slope - SlpMax[iv]) < 0.02 && fabs(inter - InterMax[iv]) < 3.) {
               acceptedHits[iH] = true;
               acceptedHits[iHH] = true;
            }
         }
      }

      double mt_avg[NVIEWS] = {-999., -999.};
      double ct_avg[NVIEWS] = {-999., -999.};
      double z_avg[NVIEWS] = {0., 0.};
      double zv_avg[NVIEWS] = {0., 0.};
      double v_avg[NVIEWS] = {0., 0.};
      double z2_avg[NVIEWS] = {0., 0.};
      int n_avg[NVIEWS] = {0, 0};
      int hitCounter_Sel[NVIEWS] = {0, 0};
      int hitCounterPlane_Sel[NPLANES] = {0, 0, 0};

      for (size_t iH = 0; iH < nhits; ++iH) {
         if (!acceptedHits[iH]) continue;

         const int p = plane->at(iH);
         const int iv = ViewFromPlane(p);
         if (iv < 0) continue;

         const double vpos = pos->at(iH) * pitch;
         const double z = GetZ(time->at(iH), trigger_time->at(iH));

         z_avg[iv] += z;
         zv_avg[iv] += z * vpos;
         v_avg[iv] += vpos;
         z2_avg[iv] += z * z;
         ++n_avg[iv];
         ++hitCounter_Sel[iv];
         ++hitCounterPlane_Sel[p];
      }

      for (int iv = 0; iv < NVIEWS; ++iv) {
         if (n_avg[iv] < 2) continue;

         z_avg[iv] /= n_avg[iv];
         zv_avg[iv] /= n_avg[iv];
         v_avg[iv] /= n_avg[iv];
         z2_avg[iv] /= n_avg[iv];

         const double denominator = z2_avg[iv] - z_avg[iv] * z_avg[iv];
         if (fabs(denominator) < 1.e-12) continue;

         mt_avg[iv] = (zv_avg[iv] - z_avg[iv] * v_avg[iv]) / denominator;
         ct_avg[iv] = v_avg[iv] - mt_avg[iv] * z_avg[iv];

         VV_hMvsC[iv]->Fill(ct_avg[iv], mt_avg[iv]);

         for (int p = 0; p < NPLANES; ++p) {
            if (ViewFromPlane(p) == iv && hitCounterPlane_Sel[p] > 0)
               P_hMvsC[p]->Fill(ct_avg[iv], mt_avg[iv]);
         }

         if (saveEventDisplay) {
            fTrack[iv] = new TF1(Form("fTrack_event_%d_%s", event, VIEW_NAME[iv]), "[0]*x+[1]", -driftgap, driftgap);
            fTrack[iv]->SetParameter(0, mt_avg[iv]);
            fTrack[iv]->SetParameter(1, ct_avg[iv]);
            fTrack[iv]->SetLineColor((iv == XVIEW) ? kRed : kBlue);
            fTrack[iv]->SetLineWidth(2);
         }
      }

      for (int iv = 0; iv < NVIEWS; ++iv) VV_hNHit_Sel[iv]->Fill(hitCounter_Sel[iv]);
      for (int p = 0; p < NPLANES; ++p) P_hNHit_Sel[p]->Fill(hitCounterPlane_Sel[p]);

      bool saveEventDisplaySel = (event == 35355);

      for (size_t iH = 0; iH < nhits; ++iH) {
         if (!acceptedHits[iH]) continue;

         const int p = plane->at(iH);
         const int iv = ViewFromPlane(p);
         if (iv < 0 || mt_avg[iv] < -500. || ct_avg[iv] < -500.) continue;

         const double vpos = pos->at(iH) * pitch;
         const double time_diff = time->at(iH) - trigger_time->at(iH) * 22.5;
         const double z = GetZ(time->at(iH), trigger_time->at(iH));

         if (saveEventDisplay && saveEventDisplaySel && gTrack[iv])
            gTrack[iv]->SetPoint(gTrack[iv]->GetN(), z, vpos);

         // Old X-view slope, now correctly applied to Plane1 through the view map.
         if (iv == XVIEW && mt_avg[XVIEW] > -0.15) continue;

         const double tDrift = time_diff - T0;
         const double vpos_fit = mt_avg[iv] * z + ct_avg[iv];

         double zpos_fit = -999.;
         if (fabs(mt_avg[iv]) > 1.e-12)
            zpos_fit = (vpos - ct_avg[iv]) / mt_avg[iv];

         if (hitCounter_Sel[iv] > 5 && zpos_fit > -500.) {
            const double vres = vpos_fit - vpos;
            const double zres = zpos_fit - z;

            VV_hVResvsC[iv]->Fill(ct_avg[iv], vres);
            VV_hVResvsM[iv]->Fill(mt_avg[iv], vres);
            VV_hVResvsVhit[iv]->Fill(vpos, vres);
            VV_hZResvsZhit[iv]->Fill(z, zres);
            VV_hZResvsADC[iv]->Fill(adc->at(iH), zres);
            VV_hZResvsM[iv]->Fill(mt_avg[iv], zres);
            VV_hZfitvsTimeDiff[iv]->Fill(tDrift, zpos_fit);

            P_hVResvsC[p]->Fill(ct_avg[iv], vres);
            P_hVResvsM[p]->Fill(mt_avg[iv], vres);
            P_hVResvsVhit[p]->Fill(vpos, vres);
            P_hZResvsZhit[p]->Fill(z, zres);
            P_hZResvsADC[p]->Fill(adc->at(iH), zres);
            P_hZResvsM[p]->Fill(mt_avg[iv], zres);
            P_hZfitvsTimeDiff[p]->Fill(tDrift, zpos_fit);
         }

         hTimeDiff_Sel->Fill(time_diff);
         hADC_Sel->Fill(adc->at(iH));
         hTimeDiffvsADC_Sel->Fill(time_diff, adc->at(iH));
         hPosvsADC_Sel->Fill(pos->at(iH), adc->at(iH));
         hTimeDiffvsPos_Sel->Fill(time_diff, pos->at(iH));

         VV_hTimeDiff_Sel[iv]->Fill(time_diff);
         VV_hADC_Sel[iv]->Fill(adc->at(iH));
         VV_hTimevsPos_Sel[iv]->Fill(time_diff, pos->at(iH));
         VV_hPosvsADC_Sel[iv]->Fill(pos->at(iH), adc->at(iH));

         P_hTimeDiff_Sel[p]->Fill(time_diff);
         P_hADC_Sel[p]->Fill(adc->at(iH));
         P_hTimevsPos_Sel[p]->Fill(time_diff, pos->at(iH));
         P_hPosvsADC_Sel[p]->Fill(pos->at(iH), adc->at(iH));

         if (time_diff < 92900 || time_diff > 94400) {
            hPosvsADC_OutofTime_Sel->Fill(pos->at(iH), adc->at(iH));
            VV_hPosvsADC_OutofTime_Sel[iv]->Fill(pos->at(iH), adc->at(iH));
            P_hPosvsADC_OutofTime_Sel[p]->Fill(pos->at(iH), adc->at(iH));
         }
      }

      if (saveEventDisplay && saveEventDisplaySel) {
         for (int iv = 0; iv < NVIEWS; ++iv) {
            evtdisplayViewsDir[iv]->cd();
            if (gTrack[iv]) gTrack[iv]->Write();
            if (fTrack[iv]) fTrack[iv]->Write();
         }
      }

      for (int iv = 0; iv < NVIEWS; ++iv) {
         delete gTrack[iv];
         delete fTrack[iv];
      }
   }

   outputFile->cd();
   hDir->cd();

   hTimeDiff->Write();
   hADC->Write();
   hTimeDiffvsPos->Write();
   hTimeDiffvsADC->Write();
   hPosvsADC->Write();
   hPosvsADC_OutofTime->Write();
   hTimeDiff_Sel->Write();
   hADC_Sel->Write();
   hTimeDiffvsPos_Sel->Write();
   hTimeDiffvsADC_Sel->Write();
   hPosvsADC_Sel->Write();
   hPosvsADC_OutofTime_Sel->Write();

   hNHitP1vsP2->Write();

   for (int iv = 0; iv < NVIEWS; ++iv) {
      hViewsDir[iv]->cd();

      VV_hTimeDiff[iv]->Write();
      VV_hADC[iv]->Write();
      VV_hTimevsPos[iv]->Write();
      VV_hPosvsADC[iv]->Write();
      VV_hPosvsADC_OutofTime[iv]->Write();
      VV_hZfitvsTimeDiff[iv]->Write();
      VV_hNHit[iv]->Write();
      VV_hTimeDiff_Sel[iv]->Write();
      VV_hADC_Sel[iv]->Write();
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
   }

   for (int p = 0; p < NPLANES; ++p) {
      hPlaneDir[p]->cd();

      P_hTimeDiff[p]->Write();
      P_hADC[p]->Write();
      P_hTimevsPos[p]->Write();
      P_hPosvsADC[p]->Write();
      P_hPosvsADC_OutofTime[p]->Write();
      P_hZfitvsTimeDiff[p]->Write();
      P_hNHit[p]->Write();
      P_hMvsC[p]->Write();
      P_hVResvsC[p]->Write();
      P_hVResvsM[p]->Write();
      P_hVResvsVhit[p]->Write();
      P_hZResvsZhit[p]->Write();
      P_hZResvsADC[p]->Write();
      P_hZResvsM[p]->Write();
      P_hTimeDiff_Sel[p]->Write();
      P_hADC_Sel[p]->Write();
      P_hTimevsPos_Sel[p]->Write();
      P_hPosvsADC_Sel[p]->Write();
      P_hPosvsADC_OutofTime_Sel[p]->Write();
      P_hNHit_Sel[p]->Write();
   }

   outputFile->Close();
   delete outputFile;

   cout << "Processed tree entries: " << nentries << endl;
   // cout << "Selected hits: " << hTimeDiff_Sel->GetEntries() << endl;
   cout << "Output written to " << outputFileName << endl;
}
