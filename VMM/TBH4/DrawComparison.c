#include <array>
#include <iostream>
#include <memory>
#include <string>

#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TSystem.h"


namespace {

constexpr int kNRuns = 6;

const std::array<int, kNRuns> runs = {
    123,
    161,
    171,
    185,
    186,
    188,
};

const std::array<const char*, kNRuns> fileNames = {
    "RecoFiles/V1_Reco_Events_run123_20260721174321.root",
    "RecoFiles/V1_Reco_Events_run161_20260726115127.root",
    "RecoFiles/V1_Reco_Events_run171_20260726160825.root",
    "RecoFiles/V1_Reco_Events_run185_20260727160744.root",
    "RecoFiles/V1_Reco_Events_run186_20260728111959.root",
    "RecoFiles/V1_Reco_Events_run188_20260728154653.root"
};

const std::array<int, kNRuns> colors = {
    kGreen,
    kBlack,
    kCyan,
    kRed,
    kMagenta,
    kBlue
};


TH2* GetInputHistogram(
    TFile* file,
    const std::string& view,
    const std::string& panel
)
{
    if (!file || file->IsZombie()) {
        return nullptr;
    }

    const std::string histogramPath =
        "Histograms/" + view + "/" + panel +
        "/hPosvsADC" + panel + view;

    TH2* histogram = nullptr;
    file->GetObject(histogramPath.c_str(), histogram);

    if (!histogram) {
        std::cerr
            << "Error: histogram "
            << histogramPath
            << " not found in file "
            << file->GetName()
            << '\n';
    }

    return histogram;
}


TH1* MakeNormalizedProjection(
    TH2* histogram,
    const std::string& name,
    int color
)
{
    if (!histogram) {
        return nullptr;
    }

    TH1* projection = histogram->ProjectionY(name.c_str());

    if (!projection) {
        return nullptr;
    }

    // Detach the projection from the input ROOT file.
    projection->SetDirectory(nullptr);

    const double integral = projection->Integral();

    if (integral > 0.) {
        projection->Scale(1. / integral);
    } else {
        std::cerr
            << "Warning: histogram "
            << name
            << " has zero integral and cannot be normalized.\n";
    }

    projection->SetLineColor(color);
    projection->SetMarkerColor(color);
    projection->SetLineWidth(2);
    projection->SetStats(false);

    projection->GetXaxis()->SetTitle("Q [ADC]");
    projection->GetYaxis()->SetTitle("# entries [a.u.]");

    return projection;
}


void DrawComparison(
    const std::array<TFile*, kNRuns>& files,
    const std::string& view,
    const std::string& panel
)
{
    const std::string canvasName =
        "c_" + panel + "_" + view;

    const std::string plotTitle =
        "HV=530 V - Thr=27 mV - G=3 mV/fC - " +
        panel + " panel, " + view + " view";

    TCanvas* canvas = new TCanvas(
        canvasName.c_str(),
        plotTitle.c_str(),
        1000,
        750
    );

    canvas->SetLeftMargin(0.13);
    canvas->SetRightMargin(0.05);
    canvas->SetBottomMargin(0.12);
    canvas->SetTopMargin(0.10);
    canvas->SetTicks(1, 1);

    TLegend* legend = new TLegend(0.70, 0.66, 0.89, 0.89);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->SetTextSize(0.035);

    bool firstHistogram = true;
    double maximum = 0.;

    std::array<TH1*, kNRuns> projections = {};

    // First create all projections and determine the common maximum.
    for (int i = 0; i < kNRuns; ++i) {
        TH2* inputHistogram =
            GetInputHistogram(files[i], view, panel);

        const std::string projectionName =
            "hADC_run" + std::to_string(runs[i]) +
            "_" + panel + "_" + view;

        projections[i] = MakeNormalizedProjection(
            inputHistogram,
            projectionName,
            colors[i]
        );

        if (projections[i]) {
            maximum = std::max(
                maximum,
                projections[i]->GetMaximum()
            );
        }
    }

    // Draw all runs on the same canvas.
    for (int i = 0; i < kNRuns; ++i) {
        TH1* histogram = projections[i];

        if (!histogram) {
            continue;
        }

        histogram->SetTitle(plotTitle.c_str());
        histogram->SetMaximum(1.15 * maximum);
        histogram->SetMinimum(0.);

        if (firstHistogram) {
            histogram->Draw("hist");
            firstHistogram = false;
        } else {
            histogram->Draw("hist same");
        }

        legend->AddEntry(
            histogram,
            Form("Run %d", runs[i]),
            "l"
        );
    }

    if (firstHistogram) {
        std::cerr
            << "Error: no histogram was drawn for "
            << panel << " panel, "
            << view << " view.\n";

        delete legend;
        return;
    }

    legend->Draw();

    canvas->Modified();
    canvas->Update();

    // Save each plot automatically.
   // canvas->SaveAs(Form("ADC_%s_%s.png", panel.c_str(), view.c_str()));
    canvas->SaveAs(Form("ADC_%s_%s.pdf", panel.c_str(), view.c_str()));
}

} // namespace


void CompareRuns()
{
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.04);

    // Uncomment this if you need your custom ROOT configuration.
    // gROOT->LoadMacro("/Users/marco/RootLogon.C");

    std::array<TFile*, kNRuns> files = {};

    for (int i = 0; i < kNRuns; ++i) {
        files[i] = TFile::Open(fileNames[i], "READ");

        if (!files[i] || files[i]->IsZombie()) {
            std::cerr
                << "Error: cannot open "
                << fileNames[i]
                << '\n';
        }
    }

    DrawComparison(files, "X", "front");
    DrawComparison(files, "X", "back");
    DrawComparison(files, "Y", "front");
    DrawComparison(files, "Y", "back");

    // The files are intentionally kept open because the canvases
    // remain available in the interactive ROOT session.
}