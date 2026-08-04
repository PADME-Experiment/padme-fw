#define VMMAnalysis_cxx
#include "VMMAnalysis.h"

#include <TFile.h>
#include <TString.h>
#include <TTree.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

using namespace std;

void VMMAnalysis::Loop()
{
   if (!fChain) {
      cerr << "VMMAnalysis::Loop: input tree is null" << endl;
      return;
   }

   const TString outputFileName = Form("Events_run%s.root", fRunName.Data());
   TFile outputFile(outputFileName, "RECREATE");

   if (outputFile.IsZombie()) {
      cerr << "Cannot create " << outputFileName << endl;
      return;
   }

   struct Hit {
      UInt_t id;
      Int_t det;
      Int_t plane;
      Int_t fec;
      Int_t vmm;

      Double_t readout_time;
      Double_t time;
      Double_t trigger_time;

      Int_t geo_id;
      Int_t ch;

      UShort_t pos;
      UShort_t bcid;

      Double_t tdc;
      Double_t adc;

      Bool_t over_threshold;
      Double_t chip_time;

      UShort_t event_counter16;
      Int_t event;
      UInt_t counter_epoch;
   };

   struct Event {
      Int_t event = -1;
      UShort_t event_counter16 = 0;
      UInt_t counter_epoch = 0;

      vector<UInt_t> id;
      vector<Int_t> det;
      vector<Int_t> plane;
      vector<Int_t> fec;
      vector<Int_t> vmm;

      vector<Double_t> readout_time;
      vector<Double_t> time;
      vector<Double_t> trigger_time;

      vector<Int_t> geo_id;
      vector<Int_t> ch;

      vector<UShort_t> pos;
      vector<UShort_t> bcid;

      vector<Double_t> tdc;
      vector<Double_t> adc;

      vector<Bool_t> over_threshold;
      vector<Double_t> chip_time;
   };

   // const Long64_t nentries = fChain->GetEntries();
   const Long64_t nentries = 5e6;
   vector<Hit> hits;
   hits.reserve(nentries);

   Long64_t nbytes = 0;

   // First loop: read all hits.
   for (Long64_t i = 0; i < nentries; ++i) {
      // if(time>90e9) break; // only in run135 - compromised
      if (LoadTree(i) < 0) break;

      const Long64_t nb = fChain->GetEntry(i);
      if (nb <= 0) continue;

      nbytes += nb;

      Hit hit;

      hit.id = id;
      hit.det = static_cast<Int_t>(det);
      hit.plane = static_cast<Int_t>(plane);
      hit.fec = static_cast<Int_t>(fec);
      hit.vmm = static_cast<Int_t>(vmm);

      hit.readout_time = readout_time;
      hit.time = time;
      hit.trigger_time = trigger_time;

      hit.geo_id = static_cast<Int_t>(geo_id);
      hit.ch = static_cast<Int_t>(ch);

      hit.pos = pos;
      hit.bcid = bcid;

      hit.tdc = static_cast<Double_t>(tdc);
      hit.adc = static_cast<Double_t>(adc);

      hit.over_threshold = over_threshold;
      hit.chip_time = chip_time;

      hit.event_counter16 = event_counter;
      hit.event = -1;
      hit.counter_epoch = 0;

      hits.push_back(hit);
   }

   // Chronological ordering is required before treating the rollover.
   stable_sort(
      hits.begin(),
      hits.end(),
      [](const Hit& a, const Hit& b) {
         if (a.time != b.time) return a.time < b.time;
         if (a.trigger_time != b.trigger_time)
            return a.trigger_time < b.trigger_time;
         return a.id < b.id;
      }
   );

   map<Int_t, Event> events;

   UInt_t epoch = 0;
   UShort_t previousCounter = 0;
   bool firstHit = true;

   // Second loop: unwrap event_counter and group the hits.
   for (Hit& hit : hits) {
      if (!firstHit) {
         const UInt_t backwardJump =
            hit.event_counter16 < previousCounter
               ? static_cast<UInt_t>(
                    previousCounter - hit.event_counter16
                 )
               : 0U;

         if (backwardJump > 32768U)
            ++epoch;
      }

      const uint64_t event64 =
         static_cast<uint64_t>(epoch) * 65536ULL +
         static_cast<uint64_t>(hit.event_counter16);

      if (event64 >
          static_cast<uint64_t>(numeric_limits<Int_t>::max())) {
         cerr << "Aligned event number exceeds Int_t range" << endl;
         return;
      }

      hit.event = static_cast<Int_t>(event64);
      hit.counter_epoch = epoch;

      Event& ev = events[hit.event];

      if (ev.id.empty()) {
         ev.event = hit.event;
         ev.event_counter16 = hit.event_counter16;
         ev.counter_epoch = hit.counter_epoch;
      }

      ev.id.push_back(hit.id);
      ev.det.push_back(hit.det);
      ev.plane.push_back(hit.plane);
      ev.fec.push_back(hit.fec);
      ev.vmm.push_back(hit.vmm);

      ev.readout_time.push_back(hit.readout_time);
      ev.time.push_back(hit.time);
      ev.trigger_time.push_back(hit.trigger_time);

      ev.geo_id.push_back(hit.geo_id);
      ev.ch.push_back(hit.ch);

      ev.pos.push_back(hit.pos);
      ev.bcid.push_back(hit.bcid);

      ev.tdc.push_back(hit.tdc);
      ev.adc.push_back(hit.adc);

      ev.over_threshold.push_back(hit.over_threshold);
      ev.chip_time.push_back(hit.chip_time);

      previousCounter = hit.event_counter16;
      firstHit = false;
   }

   TTree eventTree("events", "VMM hits grouped by aligned event counter");

   Int_t event = -1;
   UShort_t event_counter16 = 0;
   UInt_t counter_epoch = 0;
   Int_t n_hits = 0;

   Double_t event_time = 0.;
   Double_t event_time_min = 0.;
   Double_t event_time_max = 0.;
   Double_t event_readout_time_min = 0.;
   Double_t event_readout_time_max = 0.;

   vector<UInt_t> hit_id;
   vector<Int_t> hit_det;
   vector<Int_t> hit_plane;
   vector<Int_t> hit_fec;
   vector<Int_t> hit_vmm;

   vector<Double_t> hit_readout_time;
   vector<Double_t> hit_time;
   vector<Double_t> hit_trigger_time;

   vector<Int_t> hit_geo_id;
   vector<Int_t> hit_ch;

   vector<UShort_t> hit_pos;
   vector<UShort_t> hit_bcid;

   vector<Double_t> hit_tdc;
   vector<Double_t> hit_adc;

   vector<Bool_t> hit_over_threshold;
   vector<Double_t> hit_chip_time;

   eventTree.Branch("event", &event, "event/I");
   eventTree.Branch(
      "event_counter16",
      &event_counter16,
      "event_counter16/s"
   );
   eventTree.Branch(
      "counter_epoch",
      &counter_epoch,
      "counter_epoch/i"
   );
   eventTree.Branch("n_hits", &n_hits, "n_hits/I");

   eventTree.Branch("event_time", &event_time, "event_time/D");
   eventTree.Branch(
      "event_time_min",
      &event_time_min,
      "event_time_min/D"
   );
   eventTree.Branch(
      "event_time_max",
      &event_time_max,
      "event_time_max/D"
   );
   eventTree.Branch(
      "event_readout_time_min",
      &event_readout_time_min,
      "event_readout_time_min/D"
   );
   eventTree.Branch(
      "event_readout_time_max",
      &event_readout_time_max,
      "event_readout_time_max/D"
   );

   eventTree.Branch("id", &hit_id);
   eventTree.Branch("det", &hit_det);
   eventTree.Branch("plane", &hit_plane);
   eventTree.Branch("fec", &hit_fec);
   eventTree.Branch("vmm", &hit_vmm);

   eventTree.Branch("readout_time", &hit_readout_time);
   eventTree.Branch("time", &hit_time);
   eventTree.Branch("trigger_time", &hit_trigger_time);

   eventTree.Branch("geo_id", &hit_geo_id);
   eventTree.Branch("ch", &hit_ch);

   eventTree.Branch("pos", &hit_pos);
   eventTree.Branch("bcid", &hit_bcid);

   eventTree.Branch("tdc", &hit_tdc);
   eventTree.Branch("adc", &hit_adc);

   eventTree.Branch("over_threshold", &hit_over_threshold);
   eventTree.Branch("chip_time", &hit_chip_time);

   // Write one entry for each unique aligned event.
   for (const auto& item : events) {
      const Event& ev = item.second;

      event = ev.event;
      event_counter16 = ev.event_counter16;
      counter_epoch = ev.counter_epoch;
      n_hits = static_cast<Int_t>(ev.id.size());

      hit_id = ev.id;
      hit_det = ev.det;
      hit_plane = ev.plane;
      hit_fec = ev.fec;
      hit_vmm = ev.vmm;

      hit_readout_time = ev.readout_time;
      hit_time = ev.time;
      hit_trigger_time = ev.trigger_time;

      hit_geo_id = ev.geo_id;
      hit_ch = ev.ch;

      hit_pos = ev.pos;
      hit_bcid = ev.bcid;

      hit_tdc = ev.tdc;
      hit_adc = ev.adc;

      hit_over_threshold = ev.over_threshold;
      hit_chip_time = ev.chip_time;

      event_time_min = *min_element(hit_time.begin(), hit_time.end());
      event_time_max = *max_element(hit_time.begin(), hit_time.end());

      event_time = event_time_min;
      event_readout_time_min = *min_element(hit_readout_time.begin(), hit_readout_time.end());
      event_readout_time_max = *max_element(hit_readout_time.begin(), hit_readout_time.end());

      eventTree.Fill();
   }

   outputFile.cd();
   eventTree.Write();
   outputFile.Close();

   cout
      << "Created " << outputFileName
      << " with " << eventTree.GetEntries()
      << " events from " << hits.size()
      << " hits (" << nbytes << " bytes read)"
      << endl;
}