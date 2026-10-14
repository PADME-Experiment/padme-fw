#define events_cxx
#include "events.h"

#include <TDirectory.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2D.h>

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

void events::Loop()
{
   if (fChain == nullptr) {
      cerr << "events::Loop: input tree is null" << endl;
      return;
   }

   TFile* outputFile = TFile::Open("PREL_RecoEvents_122.root", "RECREATE");

   if (outputFile == nullptr || outputFile->IsZombie()) {
      cerr << "Cannot create output ROOT file PREL_RecoEvents_122.root" << endl;
      if (outputFile != nullptr) delete outputFile;
      return;
   }

   outputFile->cd();

   TDirectory* histogramDirectory = outputFile->mkdir("histograms");
   TDirectory* eventDisplayDirectory = outputFile->mkdir("event_displays");

   if (histogramDirectory == nullptr || eventDisplayDirectory == nullptr) {
      cerr << "Cannot create output directories" << endl;
      outputFile->Close();
      delete outputFile;
      return;
   }

   histogramDirectory->cd();

   const Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0;
   Long64_t nb = 0;

   TH1D* hTimeDiff = new TH1D("hTimeDiff", "Time wrt TriggerTime;#Delta t [ns];Entries", 500, 91999.5, 94999.5);
   TH2D* hTimeDiffvsPos = new TH2D("hTimeDiffvsPos", "Time wrt TriggerTime vs position;#Delta t [ns];Position [strip]", 500, 91999.5, 94999.5, 256, -0.5, 255.5);

   TH1D* Mx_hTimeDiff[2][2] = {{nullptr, nullptr}, {nullptr, nullptr}};
   TH2D* Mx_hTimevsPos[2][2] = {{nullptr, nullptr}, {nullptr, nullptr}};
   TH2D* Vc_hMvsC[2] = {nullptr, nullptr};
   TH2D* Vc_hVResvsC[2] = {nullptr, nullptr};
   TH2D* Vc_hVResvsM[2] = {nullptr, nullptr};
   TH2D* Vc_hVResvsVhit[2] = {nullptr, nullptr};

   for (int ip = 0; ip < 2; ++ip) {
      TString panel = (ip == 0) ? "back" : "front";

      for (int iv = 0; iv < 2; ++iv) {
         TString view = (iv == 0) ? "X" : "Y";

         Mx_hTimeDiff[ip][iv] = new TH1D(Form("hTimeDiff_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime, %s panel, %s view;#Delta t [ns];Entries", panel.Data(), view.Data()), 1000, 91999.5, 95499.5);
         Mx_hTimevsPos[ip][iv] = new TH2D(Form("hTimeDiffvsPos_%s%s", panel.Data(), view.Data()), Form("Time wrt TriggerTime vs position, %s panel, %s view;#Delta t [ns];%s [strip]", panel.Data(), view.Data(), view.Data()), 1000, 91999.5, 95499.5, 256, -0.5, 255.5);

         if (ip == 0) {
            Vc_hMvsC[iv] = new TH2D(Form("hMvsC_%s", view.Data()), Form("Track slope vs intercept, %s view;Intercept [mm];Slope [mm/mm]", view.Data()), 400, -200, 200, 100, -1, 1);
            Vc_hVResvsC[iv] = new TH2D(Form("hVResvsC_%s", view.Data()), Form("%s residual vs intercept;Intercept [mm];%s residual [mm]", view.Data(), view.Data()), 400, -200, 200, 100, -10, 10);
            Vc_hVResvsM[iv] = new TH2D(Form("hVResvsM_%s", view.Data()), Form("%s residual vs slope;Slope [mm/mm];%s residual [mm]", view.Data(), view.Data()), 100, -1, 1, 100, -10, 10);
            Vc_hVResvsVhit[iv] = new TH2D(Form("hVResvsV_%s", view.Data()), Form("%s residual vs hit position;%s hit position [mm];%s residual [mm]", view.Data(), view.Data(), view.Data()), 256, -0.5*pitch, 255.5*pitch, 100, -10, 10);
         }
      }
   }

   for (Long64_t jentry = 0; jentry < nentries; ++jentry) {
      const Long64_t ientry = LoadTree(jentry);

      if (ientry < 0) break;

      nb = fChain->GetEntry(jentry);

      if (nb <= 0) {
         cerr << "Could not read entry " << jentry << endl;
         continue;
      }

      nbytes += nb;

      if (pos == nullptr || plane == nullptr || det == nullptr || time == nullptr || trigger_time == nullptr) {
         cerr << "Null vector branch at entry " << jentry << endl;
         continue;
      }

      const size_t nhits = min({pos->size(), plane->size(), det->size(), time->size(), trigger_time->size()});

      if (n_hits >= 0 && static_cast<size_t>(n_hits) != nhits) {
         cerr << "Warning: event " << event << " has n_hits = " << n_hits << ", while the usable vector size is " << nhits << endl;
      }

      Int_t x_seed = -1;
      Int_t y_seed = -1;

      for (size_t iH = 0; iH < nhits; ++iH) {
         const UChar_t hit_plane = (*plane)[iH];
         const Int_t hit_pos = static_cast<Int_t>((*pos)[iH]);

         if (hit_plane == 0 && x_seed < 0) x_seed = hit_pos;
         else if (hit_plane == 1 && y_seed < 0) y_seed = hit_pos;

         if (x_seed >= 0 && y_seed >= 0) break;
      }

      double mt_avg[2] = {0.0, 0.0};
      double ct_avg[2] = {0.0, 0.0};
      double z_avg[2] = {0.0, 0.0};
      double zv_avg[2] = {0.0, 0.0};
      double v_avg[2] = {0.0, 0.0};
      double z2_avg[2] = {0.0, 0.0};
      int n_avg[2] = {0, 0};
      int hitsroad[2] = {0, 0};

      for (size_t iH = 0; iH < nhits; ++iH) {
         const UChar_t hit_det = (*det)[iH];
         const UChar_t hit_plane = (*plane)[iH];
         const Int_t hit_pos = static_cast<Int_t>((*pos)[iH]);
         const Double_t vpos = hit_pos*pitch;
         const Double_t hit_time = (*time)[iH];
         const Double_t hit_trigger_time = (*trigger_time)[iH]*22.5;
         const Double_t time_diff = hit_time - hit_trigger_time;

         if (hit_det != 1 && hit_det != 2) continue;
         if (hit_plane != 0 && hit_plane != 1) continue;

         Double_t z = (time_diff - T0)*DriftVelocity;

         if (hit_det == 1) z = z - driftgap;
         else if (hit_det == 2) z = -z + driftgap;

         const bool insideRoad = (hit_plane == 0 && x_seed >= 0 && abs(hit_pos - x_seed) < 5) || (hit_plane == 1 && y_seed >= 0 && abs(hit_pos - y_seed) < 5);

         if (!insideRoad) continue;

         const int ip = hit_det - 1;
         const int iv = hit_plane;

         z_avg[iv] += z;
         zv_avg[iv] += z*vpos;
         v_avg[iv] += vpos;
         z2_avg[iv] += z*z;
         n_avg[iv]++;
         hitsroad[iv]++;

         hTimeDiff->Fill(time_diff);
         hTimeDiffvsPos->Fill(time_diff, hit_pos);
         Mx_hTimeDiff[ip][iv]->Fill(time_diff);
         Mx_hTimevsPos[ip][iv]->Fill(time_diff, hit_pos);
      }

      const int totalRoadHits = hitsroad[0] + hitsroad[1];
      const bool saveEventDisplay = (totalRoadHits >= 12);

      TGraph* gEventDisplay[2] = {nullptr, nullptr};

      if (saveEventDisplay) {
         for (int iv = 0; iv < 2; ++iv) {
            TString view = (iv == 0) ? "X" : "Y";

            gEventDisplay[iv] = new TGraph();
            gEventDisplay[iv]->SetName(Form("gEventDisplay_entry%lld_event%d_%s", jentry, event, view.Data()));
            gEventDisplay[iv]->SetTitle(Form("Entry %lld, event %d, %s view, %d selected hits;%s position [mm];z [mm]", jentry, event, view.Data(), totalRoadHits, view.Data()));
            gEventDisplay[iv]->SetMarkerStyle(20);
            gEventDisplay[iv]->SetMarkerSize(0.8);
         }
      }

      for (int iv = 0; iv < 2; ++iv) {
         if (n_avg[iv] < 2) continue;

         z_avg[iv] /= n_avg[iv];
         zv_avg[iv] /= n_avg[iv];
         v_avg[iv] /= n_avg[iv];
         z2_avg[iv] /= n_avg[iv];

         const double denominator = z2_avg[iv] - z_avg[iv]*z_avg[iv];

         if (std::abs(denominator) < 1.e-12) continue;

         mt_avg[iv] = (zv_avg[iv] - z_avg[iv]*v_avg[iv])/denominator;
         ct_avg[iv] = v_avg[iv] - mt_avg[iv]*z_avg[iv];

         Vc_hMvsC[iv]->Fill(ct_avg[iv], mt_avg[iv]);

         const Int_t seed = (iv == 0) ? x_seed : y_seed;

         for (size_t iH = 0; iH < nhits; ++iH) {
            const UChar_t hit_det = (*det)[iH];
            const UChar_t hit_plane = (*plane)[iH];
            const Int_t hit_pos = static_cast<Int_t>((*pos)[iH]);

            if (hit_det != 1 && hit_det != 2) continue;
            if (hit_plane != iv) continue;
            if (seed < 0 || abs(hit_pos - seed) >= 5) continue;

            const int ip = hit_det - 1;
            const Double_t vpos = hit_pos*pitch;
            const Double_t hit_time = (*time)[iH];
            const Double_t hit_trigger_time = (*trigger_time)[iH]*22.5;
            const Double_t time_diff = hit_time - hit_trigger_time;

            Double_t z = (time_diff - T0)*DriftVelocity;

            if (hit_det == 1) z = z - driftgap;
            else if (hit_det == 2) z = -z + driftgap;

            const Double_t vpos_fit = mt_avg[iv]*z + ct_avg[iv];
            const Double_t residual = vpos_fit - vpos;

            Vc_hVResvsC[iv]->Fill(ct_avg[iv], residual);
            Vc_hVResvsM[iv]->Fill(mt_avg[iv], residual);
            Vc_hVResvsVhit[iv]->Fill(vpos, residual);

            if (saveEventDisplay) gEventDisplay[iv]->SetPoint(gEventDisplay[iv]->GetN(), vpos, z);
         }
      }

      if (saveEventDisplay) {
         eventDisplayDirectory->cd();

         TDirectory* eventDirectory = eventDisplayDirectory->mkdir(Form("entry_%lld_event_%d_hits_%d", jentry, event, totalRoadHits));

         if (eventDirectory != nullptr) {
            eventDirectory->cd();
            for (int iv = 0; iv < 2; ++iv) {
               gEventDisplay[iv]->Write();
            }
         }

         for (int iv = 0; iv < 2; ++iv) {
            delete gEventDisplay[iv];
         }

         histogramDirectory->cd();
      }

      if (jentry % 1== 0) {
         cout << "Processed entry " << jentry << " / " << nentries << ", selected road hits = " << totalRoadHits << endl;
      }
   }

   histogramDirectory->cd();

   hTimeDiff->Write();
   hTimeDiffvsPos->Write();

   for (int ip = 0; ip < 2; ++ip) {
      for (int iv = 0; iv < 2; ++iv) {
         Mx_hTimeDiff[ip][iv]->Write();
         Mx_hTimevsPos[ip][iv]->Write();
      }
   }

   for (int iv = 0; iv < 2; ++iv) {
      Vc_hMvsC[iv]->Write();
      Vc_hVResvsC[iv]->Write();
      Vc_hVResvsM[iv]->Write();
      Vc_hVResvsVhit[iv]->Write();
   }

   const Double_t selectedHits = hTimeDiff->GetEntries();

   // outputFile->Write();
   // outputFile->Close();

   cout << "Processed tree entries: " << nentries << endl;
   cout << "Bytes read: " << nbytes << endl;
   cout << "Selected hits: " << selectedHits << endl;
   cout << "Output written to PREL_RecoEvents_122.root" << endl;

   // delete outputFile;
}