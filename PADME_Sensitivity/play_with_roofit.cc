#include "Roo1DTable.h"
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooAddPdf.h"
#include "RooAddition.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooBernstein.h"
#include "RooCFunction1Binding.h"
#include "RooCFunction3Binding.h"
#include "RooCategory.h"
#include "RooConstVar.h"
#include "RooDataSet.h"
#include "RooExtendPdf.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooGenericPdf.h"
#include "RooLandau.h"
#include "RooLinkedList.h"
#include "RooLognormal.h"
#include "RooMsgService.h"
#include "RooPlot.h"
#include "RooPoisson.h"
#include "RooPolynomial.h"
#include "RooProdPdf.h"
#include "RooProduct.h"
#include "RooProfileLL.h"
#include "RooRandom.h"
#include "RooRealConstant.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooUniform.h"
#include "RooWorkspace.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TBranch.h"

#include <cstdlib>


enum e_statisticType{
  kBackgroundOnly = 0,
  kSplusB
};

void play_with_roofit(int _bkgOrSB, int nObsTest) {
  // Move Minimization and Fitting logs from std::cout to file.
  RooMsgService::instance().setSilentMode(true);

  RooMsgService::instance().getStream(1).removeTopic(RooFit::Minimization);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Fitting);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::NumIntegration);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Generation);

  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::Minimization),
                                      RooFit::OutputFile("minimization.log"));
  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::Fitting),
                                      RooFit::OutputFile("minimization.log"));
  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::NumIntegration),
                                      RooFit::OutputFile("minimization.log"));
  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::Generation),
                                      RooFit::OutputFile("minimization.log"));

  RooRandom::randomGenerator()->SetSeed(42);

  e_statisticType bkgOrSB = static_cast<e_statisticType>(_bkgOrSB);
  TString output_branch_name = Form("q_%s_%dobs", bkgOrSB==kBackgroundOnly ? "bkg" : "sb", nObsTest);
  TFile *output_file = new TFile("hypo.root", "UPDATE");
  TTree *output_tree = static_cast<TTree*> (output_file->FindObjectAny("stats"));
  bool isFirstTime = kFALSE;
  if (output_tree == nullptr){
    output_tree = new TTree("stats", "stats");
    isFirstTime = kTRUE;
  }

  double q_stat = 0.0;
  TBranch *bq = output_tree->Branch(output_branch_name.Data(), &q_stat, Form("%s/D", output_branch_name.Data()));

  RooRealVar m_inv("m_inv", "m_inv", 100, 0., 200.);
  RooUniform b_shape("b_shape", "b_shape", m_inv);
  // RooGaussian s_shape("s_shape", "s_shape", m_inv, RooRealConstant::value(100.), RooRealConstant::value(10.));
  RooUniform s_shape("s_shape", "s_shape", m_inv);

  RooRealVar b_mc("b_mc", "b_mc", 1., 1E-6, 10.);
  b_mc.setConstant();
  RooRealVar b_true("b_true", "b_true", 1., 1E-6, 10.);
  RooRealVar b_sigma("b_sigma", "b_sigma", 1.13, 1.0 + 1E-6, 100.);
  b_sigma.setConstant();

  RooLognormal b_constraint("b_constraint", "b_constraint", b_mc, b_true,
                            b_sigma);

  RooRealVar s_yield("s_yield", "s_yield", 1.9, 0., 100.);
  s_yield.setConstant();
  
  // RooRealVar pot_true("pot_true", "pot_true", 1.0, 1E-6, 10.);
  // RooRealVar pot_sigma("pot_sigma", "pot_sigma", 1.2, 1.0+1.0E-6, 10.);
  // pot_sigma.setConstant();
  // RooRealVar pot_obs("pot_obs", "pot_obs", 1.0, 1E-6, 10.0);
  // pot_obs.setConstant();
  // RooLognormal pot_constraint("pot_constraint", "pot_constraint", pot_obs, pot_true, pot_sigma);

  // RooProduct s_expected("s_expected", "s_expected", {s_yield, pot_true});
  
  RooExtendPdf bkg_model("bkg_model", "bkg_model", b_shape, b_true);
  RooAddPdf sb_model("sb_model", "sb_model", {s_shape, b_shape},
                     {s_yield, b_true});

  RooProdPdf bkg_full("bkg_full", "bkg_full", {bkg_model, b_constraint});
  RooProdPdf sb_full("sb_full", "sb_full", {sb_model, b_constraint});

  RooArgSet global_observables_sb{b_mc};
  RooArgSet global_observables_bkg{b_mc};

  RooArgSet fit_params_sb{b_true};
  RooArgSet fit_params_bkg{b_true};

  RooArgSet *fit_params_init_sb = fit_params_sb.snapshot();
  RooArgSet *fit_params_init_bkg = fit_params_bkg.snapshot();

  RooDataSet *pseudo_aux{bkg_full.generate({global_observables_bkg}, 1)};
  pseudo_aux->Print("V");

  global_observables_bkg.assign(*pseudo_aux->get(0));

  RooDataSet *pseudo_data{
      bkg_full.generate({m_inv}, RooFit::Extended(), RooFit::Verbose(false))};
  int n_obs = pseudo_data->numEntries();
  while (nObsTest == 0 ? n_obs != 0 : n_obs == 0) {
    pseudo_data =
        bkg_full.generate({m_inv}, RooFit::Extended(), RooFit::Verbose(false));
    n_obs = pseudo_data->numEntries();
  }
  // pot_obs.setVal(1.0);
  pseudo_data->setGlobalObservables(global_observables_sb);
  
  pseudo_data->Print("V");

  std::unique_ptr<RooFitResult> fr_sb_obs{
      sb_full.fitTo(*pseudo_data, RooFit::PrintLevel(-1), RooFit::Extended(),
                    RooFit::GlobalObservables(global_observables_sb), RooFit::Save())};
  fr_sb_obs->Print("V");
  RooArgList params_sb = fr_sb_obs->floatParsFinal();

  b_true.setVal(1.);
  std::unique_ptr<RooFitResult> fr_bkg_obs{bkg_full.fitTo(
      *pseudo_data, RooFit::PrintLevel(-1), RooFit::GlobalObservables(global_observables_sb),
      RooFit::PrintEvalErrors(-1), RooFit::Warnings(false), RooFit::Extended(),
      RooFit::Verbose(false), RooFit::Save())};
  fr_bkg_obs->Print("V");
  RooArgList params_bkg = fr_bkg_obs->floatParsFinal();

  // toys
 
  int n_toys = 1000;
  int partition = n_toys / 10;
  RooAbsPdf *pdfGen = (bkgOrSB == kBackgroundOnly ? &bkg_full : &sb_full);
  // pdfGen->Print();
  for (int i = 0; i < n_toys; i++) {
    if (n_toys > 10) {
      if (i % partition == 0) {
        std::cout << "doing toy " << i << "/" << n_toys << std::endl;
      }
    }

    if (bkgOrSB == kBackgroundOnly){
      fit_params_bkg.assign(params_bkg);
    }
    else{
      fit_params_sb.assign(params_sb);
    }
    // sample global observations
    std::unique_ptr<RooDataSet> aux{pdfGen->generate(bkgOrSB == kBackgroundOnly ? global_observables_bkg : global_observables_sb, 1)};
    global_observables_sb.assign(*aux->get(0));
    // if (bkgOrSB == kBackgroundOnly){
    //   pot_obs.setVal(1.0);
    // }
    std::unique_ptr<RooDataSet> toy{
        pdfGen->generate({m_inv}, RooFit::Extended(), RooFit::Verbose(false))};
    toy->setGlobalObservables(global_observables_sb);
    if (n_toys == 1) {
      toy->Print("V");
    }

    fit_params_sb.assign(*fit_params_init_sb);
    std::unique_ptr<RooFitResult> fr_sb{sb_full.fitTo(
        *toy, RooFit::PrintLevel(-1), RooFit::GlobalObservables(global_observables_sb),
        RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
        RooFit::Extended(), RooFit::Verbose(false), RooFit::Save())};
    double nll_sb = fr_sb->minNll();
    if (n_toys == 1) {
      fr_sb->Print("V");
      std::cout << nll_sb << std::endl;
    }

    fit_params_bkg.assign(*fit_params_init_bkg);
    std::unique_ptr<RooFitResult> fr_bkg{bkg_full.fitTo(
        *toy, RooFit::PrintLevel(-1), RooFit::GlobalObservables(global_observables_sb),
        RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
        RooFit::Extended(), RooFit::Verbose(false), RooFit::Save())};
    double nll_bkg = fr_bkg->minNll();
    if (n_toys == 1) {
      fr_bkg->Print("V");
      std::cout << nll_bkg << std::endl;
    }

    q_stat =  2.0 * (nll_sb - nll_bkg);
    // std::cout << q_stat << std::endl;
    if (isFirstTime){
      output_tree->Fill();
    }
    else{
      bq->Fill();
    }
  }

  output_tree->Print();
  output_tree->Write("", TObject::kOverwrite);
  output_file->Close();
  delete output_file;
}