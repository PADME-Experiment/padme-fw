#include "GlobalParams.h"
#include "StatisticsModel.h"
#include "ChannelModel.h"
#include "EEShapes.h"

#include "RooAbsPdf.h"
#include "RooAddPdf.h"
#include "RooAddition.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooBernstein.h"
#include "RooCategory.h"
#include "RooConstVar.h"
#include "RooDLLSignificanceMCSModule.h"
#include "RooDataSet.h"
#include "RooExtendPdf.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooGenericPdf.h"
#include "RooLandau.h"
#include "RooLinkedList.h"
#include "RooLognormal.h"
#include "RooMCStudy.h"
#include "RooMsgService.h"
#include "RooNLLVar.h"
#include "RooPlot.h"
#include "RooPoisson.h"
#include "RooProdPdf.h"
#include "RooProduct.h"
#include "RooProfileLL.h"
#include "RooRandom.h"
#include "RooRealConstant.h"
#include "RooRealVar.h"
#include "RooSimultaneous.h"
#include "RooUniform.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1D.h" 
#include "TH2D.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TTree.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef vector<pair<double, vector<pair<double, double>>>> points_analyzed_t;


void silence_roofit() {
  // Move Minimization and Fitting logs from cout to file.
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
}

void usage() {
  cout << "./exe : runs with default coupling points and all masses from "
               "histogram\n"
       << "./exe <filename> : runs with configuration from file. See "
               "example in ex_inp.txt\n";
}

void debug_toy(const RooDataSet *toy_experiment) {
  //MM: it seems to do nothing. Maybe I can delete it
  toy_experiment->Print("V");

  const RooArgSet *test = toy_experiment->get();
  RooCategory *pid_test = (RooCategory *)(test->find("ScanPoint_categories"));
  RooRealVar *x = (RooRealVar *)(test->find("SqrtS"));

  for (int iEv = 0; iEv < toy_experiment->numEntries(); iEv++) {
    toy_experiment->get(iEv);
    cout << x->getVal() << " " << pid_test->getCurrentLabel() << endl;
  }
}

void add_points_to_tree(map<TString, TTree *> &outputTrees, const points_analyzed_t &pts, TString treeName) {
  map<TString, TTree *>::iterator it = outputTrees.find(treeName);
  if (it != outputTrees.end()) {
    cout << "Tree: " << treeName.Data() << " already exists. Won't write" << endl;
    return;
  }
  outputTrees[treeName] = new TTree(treeName.Data(), treeName.Data());
  double mass;
  double logcoupling;
  double cls;
  outputTrees[treeName]->Branch("Mass", &mass);
  outputTrees[treeName]->Branch("LogCoupling", &logcoupling);
  outputTrees[treeName]->Branch("CLS", &cls);

  for (auto p : pts) {
    mass = p.first;
    for (auto l : p.second) {
      logcoupling = l.first;
      cls = l.second;
      outputTrees[treeName]->Fill();
    }
  }
}

void close_output(map<TString, TTree *> outputTrees) {
  TFile *output_file = TFile::Open("output.root", "RECREATE");
  output_file->cd();
  for (auto tree : outputTrees) {
    output_file->WriteObject(tree.second, tree.first.Data());
  }
  output_file->Close();
  // delete output_file;
}

void do_debug_run(global_config &config, StatisticsModel *full_model) {
  ////
  // NB: to avoid memory leaks in toys, datasets and fit results will be unique_ptr.
  ///
  cout << "============  Debug run starts now ===============" << endl;
  TFile *output_debug = new TFile("PADME_output_debug.root", "RECREATE");
  TTree *tree_debug = new TTree("statistics", "statistics");
  vector<double> q_toys_sb;
  vector<double> q_toys_bonly;
  tree_debug->Branch("q_sb", &q_toys_sb);
  tree_debug->Branch("q_bkg", &q_toys_bonly);

  /////////////////////////////////////////////////////////////
  //////////// Reset computation details //////////////////////
  /////////////////////////////////////////////////////////////

  config.mass_points.clear();
  // config.mass_points.push_back(16.5);
  config.mass_points.push_back(16.85);
  // config.mass_points.push_back(17.2);
  config.log_coupling_points.clear();
  // config.log_coupling_points.push_back(-4);
  config.log_coupling_points.push_back(-3);
  // config.log_coupling_points.push_back(-2);
  config.n_toys = 10;

  full_model->Update(config, config.mass_points[0], config.log_coupling_points[0]);
  cout << "Mass = " << config.mass_points[0] << " logE = " << config.log_coupling_points[0] << " yield = " << full_model->GetTotalYield() << endl;
  cout << "Mass = " << config.mass_points[0] << " coupling = " << pow(10,config.log_coupling_points[0]) << " yield = " << full_model->GetTotalYield() << endl;
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////// Define dummy observation/////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  RooDataSet observation("obs", "obs", {*full_model->SqrtS, *full_model->ScanPoint_categories});
  full_model->SetDefaultAuxiliaryObservation();
  observation.Print("V");

  RooArgSet *params_init = full_model->fitGlobal_parameters->snapshot();
  double q_obs = 0.0;

  // decide which are the models
  RooAbsPdf *sb_model = nullptr;
  RooAbsPdf *bkg_model = nullptr;
  if (config.tossingType == kCousinsHighland) {
    sb_model = full_model->sbModel_simultaneous_noConstraints;
    bkg_model = full_model->bkgModel_simultaneous_noConstraints;
  } else if (config.tossingType == kFixToFit) {
    sb_model = full_model->sbModel_full;
    bkg_model = full_model->bkgModel_full;
  }

  ///////////////////////////////////////////////////////////////
  //////////////////// Fit under S+B and Bonly //////////////////
  ///////////////////////////////////////////////////////////////
  RooArgList fitted_values_sb;
  RooArgList fitted_values_bkg;
  if (config.tossingType == kFixToFit) {
    full_model->fitGlobal_parameters->assignFast(*params_init);
    full_model->gve_test->setVal(1.0e-3);
    full_model->gve_test->setConstant(true);
    unique_ptr<RooFitResult> fr_sb_obs{sb_model->fitTo(observation, /*RooFit::GlobalObservables(*full_model->Global_observables),*/
                                                       RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                                       RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintEvalErrors(-1),
                                                       RooFit::Warnings(false), RooFit::Verbose(false), RooFit::Save())};
    fr_sb_obs->Print("V");
    fitted_values_sb = fr_sb_obs->floatParsFinal();
    double nll_sb_obs = fr_sb_obs->minNll();

    full_model->fitGlobal_parameters->assignFast(*params_init);

    full_model->gve_test->setVal(0.0);
    full_model->gve_test->setConstant(true);
    unique_ptr<RooFitResult> fr_bonly_obs{bkg_model->fitTo(observation, /*RooFit::GlobalObservables(*full_model->Global_observables),*/
                                          RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                          RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintEvalErrors(-1),
                                          RooFit::Warnings(false), RooFit::Verbose(false), RooFit::Save())
    };
    fr_bonly_obs->Print("V");
    fitted_values_bkg = fr_bonly_obs->floatParsFinal();

    if (config.statType == kTevatron) {
      q_obs = 2.0 * (nll_sb_obs - fr_bonly_obs->minNll());
    } else if (config.statType == kLHC) {
      //////////////////////////////////////////////////////////
      /////////////// Fit for maximum likelihood////////////////
      //////////////////////////////////////////////////////////
      full_model->fitGlobal_parameters->assignFast(*params_init);
      full_model->gve_test->setVal(1.0e-3);
      full_model->gve_test->setConstant(false);
      unique_ptr<RooFitResult> fr_unconditional{sb_model->fitTo(observation,
                                                /*RooFit::GlobalObservables(*full_model->Global_observables),*/
                                                RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                                RooFit::PrintEvalErrors(-1), RooFit::Minimizer("Minuit2", "Migrad"),
                                                RooFit::Verbose(false), RooFit::Save())
      };
      fr_unconditional->Print("V");

      double nll_uncond = fr_unconditional->minNll();
      cout << "nll_sb = " << nll_sb_obs << " nll_uncond = " << nll_uncond;
      q_obs = 2.0 * (nll_sb_obs - nll_uncond);
    } else if (config.statType == kCLSB) {
      cout << "nll_sb = " << nll_sb_obs << " nll_bkg = " << fr_bonly_obs->minNll();
      q_obs = 2.0 * nll_sb_obs;
    }
  } else if (config.tossingType == kCousinsHighland) {
    for (auto par : full_model->constraintsGlobal_map) {
      par.first->setConstant(true);
    }
    full_model->gve_test->setVal(1.0e-3);
    full_model->gve_test->setConstant(true);
    RooNLLVar nll_sb_var("nll_sb_var", "nll_sb_var", *sb_model, observation, RooFit::Extended());
    cout << "nll_sb = " << nll_sb_var.getVal();
    q_obs = nll_sb_var.getVal();

    RooNLLVar nll_bkg_var("nll_bkg_var", "nll_bkg_var", *bkg_model, observation, RooFit::Extended());
    cout << " nll_bkg = " << nll_bkg_var.getVal();
    q_obs = q_obs - nll_bkg_var.getVal();
    q_obs = 2.0 * q_obs;
  }
  cout << " q_obs = " << q_obs << endl;

  ///////////////////////////////////////////////////////////////////
  /////////////////////// Run S+B toys //////////////////////////////
  ///////////////////////////////////////////////////////////////////
  cout << "Doing S+B toys: " << config.n_toys << endl;
  TStopwatch sw;
  int partition = config.n_toys > 10 ? config.n_toys / 10 : 1;
  double n_higher_sb = 0.0;
  RooRealVar n_generated_sb("n_generated_sb", "n_generated_sb", 0., 0., 100.);
  RooDataSet ds_ngen("ds_ngen", "ds_ngen", {n_generated_sb});

  for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
    if (partition > 0.) {
      if (iToy % partition == 0)
        cout << "---starting: " << iToy << "/" << config.n_toys << endl;
    }

    unique_ptr<RooDataSet> toys_sb{full_model->Generate(config, 1.0e-3, sb_model, fitted_values_sb)};

    n_generated_sb.setVal(toys_sb->numEntries());
    ds_ngen.add({n_generated_sb});

    double q = full_model->ComputeQstatistic(config, *toys_sb, params_init);
    q_toys_sb.push_back(q);
    if (q_toys_sb.back() >= q_obs || fabs(q_toys_sb.back() - q_obs) < 1E-3)
      n_higher_sb += 1.0;
  }
  ds_ngen.GetClonedTree()->Write("n_gen");
  double time_sb_toys = sw.RealTime();
  cout << "Done " << config.n_toys << " toys in " << time_sb_toys << " s" << endl;

  if (config.statType == kCLSB) {
    cout << "n_higher_sb = " << n_higher_sb << endl;
    cout << "CL_{SB} = " << n_higher_sb / config.n_toys << endl;
    tree_debug->Write();
    output_debug->Close();
    delete output_debug;
    delete params_init;
    return;
  }

  /////////////////////////////////////////////////////////////
  //////////////////// Run Bkg toys ///////////////////////////
  /////////////////////////////////////////////////////////////
  cout << "Doing Bkg toys: " << config.n_toys << endl;
  sw.Continue();
  double n_higher_bkg = 0.0;
  for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
    if (partition > 0.) {
      if (iToy % partition == 0)
        cout << "---starting: " << iToy << "/" << config.n_toys << endl;
    }

    unique_ptr<RooDataSet> toys{full_model->Generate(config, 0.0, bkg_model, fitted_values_bkg)};
    double q = full_model->ComputeQstatistic(config, *toys, params_init);
    q_toys_bonly.push_back(q);
    if (q_toys_bonly.back() >= q_obs || fabs(q_toys_bonly.back() - q_obs) < 1E-3)
      n_higher_bkg += 1.0;
  }
  double time_bkg_toys = sw.RealTime() - time_sb_toys;
  cout << "Done " << config.n_toys << " toys in " << time_bkg_toys << " s" << endl;
  tree_debug->Fill();

  cout << "N higher q (s+b): " << n_higher_sb << " N higher q (nkg): " << n_higher_bkg << endl;
  double cls = 1.0;
  if (n_higher_bkg > 1E-6) cls = n_higher_sb / n_higher_bkg;
  cout << "CLs = " << cls << endl;

  // save and cleanup
  tree_debug->Write();
  output_debug->Close();
  delete output_debug;
  delete params_init;
}

void do_exclusion_run(global_config &config, StatisticsModel *full_model) {
  ////
  // NB: to avoid memory leaks in toys, datasets and fit results will be unique_ptr.
  ///
  cout << "============  Exclusion run starts now ===============" << endl;
  TFile *output_exclusion = new TFile("PADME_output_exclusion.root", "RECREATE");
  TTree *tree_exclusion = new TTree("CLS", "CLS");
  double cls = 1.0;
  double current_mass = 0.0;
  double current_loge = 0.0;
  tree_exclusion->Branch("Mass", &current_mass);
  tree_exclusion->Branch("LogCoupling", &current_loge);
  tree_exclusion->Branch("CLS", &cls);

  // decide which are the models
  RooAbsPdf *sb_model = nullptr;
  RooAbsPdf *bkg_model = nullptr;
  if (config.tossingType == kCousinsHighland) {
    sb_model = full_model->sbModel_simultaneous_noConstraints;
    bkg_model = full_model->bkgModel_simultaneous_noConstraints;
  } else if (config.tossingType == kFixToFit) {
    sb_model = full_model->sbModel_full;
    bkg_model = full_model->bkgModel_full;
  }

  /////////////////////////////////////////////////////////////////////////////
  //////////////// Define observation and extract parameters //////////////////
  /////////////////////////////////////////////////////////////////////////////

  RooDataSet observation("obs", "obs", {*full_model->SqrtS, *full_model->ScanPoint_categories});
  // full_model->SetDefaultAuxiliaryObservation();
  observation.setGlobalObservables(*full_model->Global_observables);
  
  // make_unique<RooArgSet> globObs(observation.get(0));
  // globObs.snapshot(*full_model->Global_observables);
  // for(auto * arg : *full_model->Global_observables) {
  //   arg->setAttribute("global",true);
  //   // Global observables are also always constant in fits
  //   if(auto lval = dynamic_cast<RooAbsRealLValue*>(arg)) lval->setConstant(true);
  //   if(auto lval = dynamic_cast<RooAbsCategoryLValue*>(arg)) lval->setConstant(true);
  // }

  RooArgSet *params_init = full_model->fitGlobal_parameters->snapshot();

  TStopwatch sw;
  sw.RealTime();
  for (size_t iM = 0; iM < config.mass_points.size(); iM++) {
    current_mass = config.mass_points[iM];
    for (size_t iE = 0; iE < config.log_coupling_points.size(); iE++) {
      current_loge = config.log_coupling_points[iE];
      full_model->Update(config, current_mass, current_loge);

      cout << "Current point: mass = " << current_mass << " log(e) = " << current_loge << " Total yield = " << full_model->GetTotalYield() << endl;

      if (full_model->IsHighYield()) {
        cout << "High yield, setting CLS to 0" << endl;
        cls = 0.0;
        tree_exclusion->Fill();
        continue;
      }
      if (full_model->IsLowYield()) {
        cout << "Low yield, setting CLS to 1" << endl;
        cls = 1.0;
        tree_exclusion->Fill();
        continue;
      }

      double q_obs = 0.0;
      RooArgList fitted_values_sb;
      RooArgList fitted_values_bkg;

      if (config.tossingType == kFixToFit) {
        // fit under s+b
        full_model->fitGlobal_parameters->assignFast(*params_init);
        full_model->gve_test->setVal(1.0e-3);
        full_model->gve_test->setConstant(true);
        unique_ptr<RooFitResult> fr_sb_obs{sb_model->fitTo(observation,
                                                           /*RooFit::GlobalObservables(*full_model->Global_observables),*/
                                                           RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                                           RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
                                                           RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
                                                           RooFit::Verbose(false), RooFit::Save())};
        fitted_values_sb = fr_sb_obs->floatParsFinal();
        double nll_sb_obs = fr_sb_obs->minNll();
        fr_sb_obs->Print("V");

        // fit under bonly
        full_model->fitGlobal_parameters->assignFast(*params_init);
        full_model->gve_test->setVal(0.0);
        full_model->gve_test->setConstant(true);
        unique_ptr<RooFitResult> fr_bkg_obs{bkg_model->fitTo(observation,
                                                             /*RooFit::GlobalObservables(*full_model->Global_observables),*/
                                                             RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                                             RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
                                                             RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
                                                             RooFit::Verbose(false), RooFit::Save())};
        fitted_values_bkg = fr_bkg_obs->floatParsFinal();
        fr_bkg_obs->Print("V");

        if (config.statType == kLHC) {
          // fit for maximum likelihood
          full_model->fitGlobal_parameters->assignFast(*params_init);
          full_model->gve_test->setVal(1.0e-3);
          full_model->gve_test->setConstant(false);
          unique_ptr<RooFitResult> fr_unconditional{sb_model->fitTo(observation,
                                                                    /*RooFit::GlobalObservables(*full_model->Global_observables),*/
                                                                    RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                                                    RooFit::Minimizer("Minuit2", "Migrad"),
                                                                    RooFit::PrintLevel(-1), RooFit::PrintEvalErrors(-1),
                                                                    RooFit::Warnings(false), RooFit::Verbose(false),
                                                                    RooFit::Save())};
          fr_unconditional->Print("V");

          double nll_uncond = fr_unconditional->minNll();
          cout << "nll_sb = " << nll_sb_obs << " nll_uncond = " << nll_uncond;
          q_obs = 2.0 * (nll_sb_obs - nll_uncond);
        } else {
          cout << "nll_sb = " << nll_sb_obs << " nll_bkg_obs = " << fr_bkg_obs->minNll();
          q_obs = 2.0 * (nll_sb_obs - fr_bkg_obs->minNll());
        }
      } else {
        for (auto par : full_model->constraintsGlobal_map) {
          par.first->setConstant(true);
        }
        full_model->gve_test->setVal(1.0e-3);
        full_model->gve_test->setConstant(true);
        RooNLLVar nll_sb_var("nll_sb_var", "nll_sb_var", *sb_model, observation, RooFit::Extended());
        cout << "nll_sb = " << nll_sb_var.getVal();
        q_obs = nll_sb_var.getVal();

        RooNLLVar nll_bkg_var("nll_bkg_var", "nll_bkg_var", *bkg_model, observation, RooFit::Extended());
        cout << " nll_bkg = " << nll_bkg_var.getVal();
        q_obs = q_obs - nll_bkg_var.getVal();
        q_obs = 2.0 * q_obs;
      }
      cout << " q_obs = " << q_obs << endl;

      /////////////////////////////////////////////////////
      /////////////////////// Run S+B toys ////////////////
      /////////////////////////////////////////////////////

      cout << "Doing S+B toys: " << config.n_toys << endl;
      double n_higher_sb = 0.0;
      double t_start_sb = sw.RealTime();
      sw.Continue();
      for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
        unique_ptr<RooDataSet> toys_sb{full_model->Generate(config, 1.0, sb_model, fitted_values_sb)};
        double q_toy = full_model->ComputeQstatistic(config, *toys_sb, params_init);
        if (q_toy >= q_obs || fabs(q_toy - q_obs) < 1E-3) n_higher_sb += 1.0;
      }
      double time_sb_toys = sw.RealTime();
      cout << "Done " << config.n_toys << " toys in " << time_sb_toys - t_start_sb << " s" << endl;

      ////////////////////////////////////////////////////
      /////////////////////// Run Bkg toys////////////////
      ////////////////////////////////////////////////////
      cout << "Doing Bkg toys: " << config.n_toys << endl;
      sw.Continue();
      double n_higher_bkg = 0.0;
      for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
        // generate the toy
        unique_ptr<RooDataSet> toys{full_model->Generate(config, 0.0, bkg_model, fitted_values_bkg)};
        double q_toy = full_model->ComputeQstatistic(config, *toys, params_init);

        if (q_toy >= q_obs || fabs(q_toy - q_obs) < 1E-3) n_higher_bkg += 1.0;
      }
      double time_bkg_toys = sw.RealTime() - time_sb_toys;
      cout << "Done " << config.n_toys << " toys in " << time_bkg_toys << " s" << endl;

      cout << "N higher q (s+b): " << n_higher_sb << " N higher q (nkg): " << n_higher_bkg << endl;
      cls = 1.0;
      if (n_higher_bkg > 1E-6) cls = n_higher_sb / n_higher_bkg;
      cout << "CLs = " << cls << endl;  

      tree_exclusion->Fill();
    }
  }

  tree_exclusion->Write();
  output_exclusion->Close();
  delete output_exclusion;
  delete params_init;
}

void do_expectedband_run(global_config &config, StatisticsModel *full_model) {

  ////
  // NB: to avoid memory leaks in toys, datasets and fit results will be unique_ptr.
  ////
  cout << "============  Expected band run starts now ===============" << endl;
  TFile *output_exclusion = new TFile("expectedband_TEST.root", "RECREATE");
  TTree *tree_exclusion = new TTree("CLS", "CLS");
  double fraction_rejected = 1.0;
  double current_mass = 0.0;
  double current_loge = 0.0;
  tree_exclusion->Branch("Mass", &current_mass);
  tree_exclusion->Branch("LogCoupling", &current_loge);
  // called CLS for drawing later
  tree_exclusion->Branch("CLS", &fraction_rejected);

  /////////////////////////////////////////////////////////////////////////////
  //////////////// Store fit and global parameters  ///////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // full_model->SetDefaultAuxiliaryObservation();
  RooArgSet *params_init = full_model->fitGlobal_parameters->snapshot();
  params_init->Print("V") ;

  // compare function
  auto compare_q = [](double element, double value) {
    return (element < value) || (fabs(value - element) < 1E-6);
  };

  TStopwatch sw;
  sw.RealTime();
  for (size_t iMass = 0; iMass < config.mass_points.size(); iMass++) {
    current_mass = config.mass_points[iMass];

    for (size_t iE = 0; iE < config.log_coupling_points.size(); iE++) {
      double t_start_point = sw.RealTime();
      sw.Continue();
      current_loge = config.log_coupling_points[iE];

      full_model->Update(config, current_mass, current_loge);

      cout << "Current point: mass = " << current_mass << " log(e) = " << current_loge << " Total yield = " << full_model->GetTotalYield() << endl;

      if (full_model->IsHighYield()) {
        // gve_test->getValue();
        cout << "High yield, setting fraction excluded to 1" << endl;
        fraction_rejected = 1.0;
        tree_exclusion->Fill();
        continue;
      }
      if (full_model->IsLowYield()) {
        // gve_test->getValue();
        cout << "Low yield, setting fraction excluded to 0" << endl;
        fraction_rejected = 0.0;
        tree_exclusion->Fill();
        continue;
      }

      /////////////////////////////////////////////////////////////////
      //////////// Compute pdfs of q under S+B and B //////////////////
      /////////////////////////////////////////////////////////////////
      cout << "Generating first pseudo-observation" << endl;
      // generate 1 toy, fit it and compute f(q|S+B) and f(q|B)
      
      full_model->fitGlobal_parameters->assignFast(*params_init);
      unique_ptr<RooDataSet> pseudo_data_aux{full_model->bkgModel_full->generate(*full_model->Global_observables, 1)};
      full_model->Global_observables->assignFast(*pseudo_data_aux->get(0));

      unique_ptr<RooDataSet> pseudo_observation{full_model->bkgModel_full->generate({*full_model->Theta, *full_model->ScanPoint_categories},
                                                                                     RooFit::Extended(), RooFit::Verbose(false))};
      pseudo_observation->setGlobalObservables(*full_model->Global_observables);
      pseudo_data_aux->Print("V");
      pseudo_observation->Print("V");

      full_model->ScanPoint_categories->Print("V");

      cout << "Fitting first pseudo-observation" << endl;
      cout << "Fitting first pseudo-observation signal+bkg" << endl;
      // fit under s+b
      full_model->fitGlobal_parameters->assignFast(*params_init);
      unique_ptr<RooFitResult> fr_sb_obs{full_model->sbModel_full->fitTo(*pseudo_observation,
                                                                         RooFit::GlobalObservables(*full_model->Global_observables),
                                                                         RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                                                         RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
                                                                         RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
                                                                         RooFit::Verbose(false), RooFit::Save())};
      RooArgList fitted_values_sb = fr_sb_obs->floatParsFinal();
      fr_sb_obs->Print("V");

      cout << "Fitting first pseudo-observation bkg only" << endl;
      // fit under b only
      full_model->fitGlobal_parameters->assignFast(*params_init);
      unique_ptr<RooFitResult> fr_bkg_obs{full_model->bkgModel_full->fitTo(*pseudo_observation,
                                                                         /*RooFit::GlobalObservables(*full_model->Global_observables),*/
                                                                           RooFit::Constrain(*full_model->constrainedGlobal_parameters),
                                                                           RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
                                                                           RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
                                                                           RooFit::Verbose(false), RooFit::Save())};
      RooArgList fitted_values_bkg = fr_bkg_obs->floatParsFinal();
      fr_bkg_obs->Print("V");

      cout << "Computing q distribution" << endl;

      vector<double> q_distribution_sb;
      full_model->ComputeQDistribution(config, full_model->sbModel_full, fitted_values_sb, params_init, q_distribution_sb);
      sort(q_distribution_sb.begin(), q_distribution_sb.end());
      cout << "q_sb = [";
      for (size_t iq = 0; iq < q_distribution_sb.size(); ++iq) {
        cout << " " << q_distribution_sb[iq];
      }
      cout << "]" << endl;

      double t_q_sb = sw.RealTime();
      sw.Continue();
      cout << "q-distribution for s+b done in " << t_q_sb - t_start_point << " s" << endl;

      vector<double> q_distribution_bkg;
      full_model->ComputeQDistribution(config, full_model->bkgModel_full, fitted_values_bkg, params_init, q_distribution_bkg);
      sort(q_distribution_bkg.begin(), q_distribution_bkg.end());

      // cout << "q_sb = [";
      // for (size_t iq = 0; iq < q_distribution_bkg.size(); ++iq){
      //   cout << " " << q_distribution_bkg[iq];
      // }
      // cout << "]" << endl;

      double t_q_bkg = sw.RealTime();
      sw.Continue();
      cout << "q-distribution for bkg done in " << t_q_bkg - t_q_sb << " s" << endl;

      /////////////////////////////////////////////////////////////////////////
      //////////// Generate rest of pseudo-observations ///////////////////////
      /////////////////////////////////////////////////////////////////////////

      double n_toys_rejected = 0.0;
      cout << "Generate rest of pseudo-observations" << endl;
      double t_start_toys = sw.RealTime();
      sw.Continue();
      int partition = config.n_toys / 10;
      for (int iToy = 0; iToy < config.n_toys; iToy++) {
        if (config.n_toys > 10) {
          if (iToy % partition == 0) {
            cout << "Doing pseudo-observation " << iToy << "/" << config.n_toys << endl;
          }
        }

        full_model->fitGlobal_parameters->assignFast(*params_init);
        unique_ptr<RooDataSet> toy_aux{full_model->bkgModel_full->generate(*full_model->Global_observables, 1)};
        full_model->Global_observables->assignFast(*toy_aux->get(0));

        unique_ptr<RooDataSet> toy_observation{full_model->bkgModel_full->generate({*full_model->SqrtS, *full_model->ScanPoint_categories},
                                                                                    RooFit::Extended(), RooFit::Verbose(false))};
        toy_observation->setGlobalObservables(*toy_aux->get(0));

        double q = full_model->ComputeQstatistic(config, *toy_observation, params_init);
        // compute CLS for this pseudo-observation
        vector<double>::iterator it_sb = upper_bound(q_distribution_sb.begin(), q_distribution_sb.end(), q, compare_q);
        double n_higher_sb = 1.0 * config.n_toys - 1.0 * (it_sb - q_distribution_sb.begin());

        vector<double>::iterator it_bkg = upper_bound(q_distribution_bkg.begin(), q_distribution_bkg.end(), q, compare_q);
        double n_higher_bkg = 1.0 * config.n_toys - 1.0 * (it_bkg - q_distribution_bkg.begin());

        double cls = 1.0;
        if (n_higher_bkg > 1E-6) cls = n_higher_sb / n_higher_bkg;

        // cout << "q = " << q <<   " n_higher_sb = " << n_higher_sb << "
        // n_higher_bkg = " << n_higher_bkg << " cls = " << cls << endl;
        if (cls <= 1.0 - config.cl) n_toys_rejected += 1.0;
      }

      fraction_rejected = n_toys_rejected / config.n_toys;
      cout << "Fraction rejected: " << fraction_rejected << endl;
      tree_exclusion->Fill();
    }
  }

  tree_exclusion->Write();
  output_exclusion->Close();
  delete output_exclusion;
  delete params_init;
}

int main(int argc, char **argv) {
  global_config config;
  map<TString, TTree *> outputTrees;

  /////////////////////////////////////////////////////////////
  //////////// Set up a few convenience things ////////////////
  /////////////////////////////////////////////////////////////
  silence_roofit();
  config.retrieve_input();

  RooRandom::randomGenerator()->SetSeed(0);

  //////////////////////////////////////////////////////////////
  ///////////// Configure and build model //////////////////////
  //////////////////////////////////////////////////////////////
  config.configure_run(argc, argv);
  config.print_config();

  cout << "number of input channel: " << config.nChannel <<endl;
  StatisticsModel *full_model = new StatisticsModel(config);
  for(int iC = 0; iC<config.nChannel; iC++){
    full_model->AddChannel((TString)(Form("SqrtS_%i", int(config.gPoT->GetPointX(iC)*1000))), kConst, kFlat, // 1,1,1,1,1,1,1,1);
                            config.gPoT->GetPointX(iC), config.gPoT->GetErrorX(iC), 
                            config.gBkg->GetPointY(iC), config.gBkg->GetErrorY(iC),
                            config.gPoT->GetPointY(iC), config.gPoT->GetErrorY(iC),
                            config.gEff->GetPointY(iC), config.gEff->GetErrorY(iC));
  }
  full_model->BuildModel();
  cout << "Model built " <<endl;
  //////////////////////////////////////////////////////////////
  /////////// Select what we're doing //////////////////////////
  //////////////////////////////////////////////////////////////
  if (config.runType == kDebugRun) {
    do_debug_run(config, full_model);
  } else if (config.runType == kExclusionRun) {
    do_exclusion_run(config, full_model);
  } else if (config.runType == kExpectedBandRun) {
    do_expectedband_run(config, full_model);
  }

  // close_output(outputTrees);

  return 0;
}