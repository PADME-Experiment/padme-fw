#include "GlobalParams.h"
#include "StatisticsModel.h"

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

#include "MuMuBkgShape.h"
#include "MuMuSignalShape.h"

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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef vector<pair<double, vector<pair<double, double>>>> points_analyzed_t; //ho creato una nuova variabile con typedef

// To lazy to rewrite the class
// keep the yield and sigma histos here

void silence_roofit(const char* OutputFileName) {
  // Move Minimization and Fitting logs from cout to file.
  RooMsgService::instance().setSilentMode(true);

  RooMsgService::instance().getStream(1).removeTopic(RooFit::Minimization);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Fitting);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::NumIntegration);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Generation);

  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::Minimization),
                                      RooFit::OutputFile(OutputFileName));
  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::Fitting),
                                      RooFit::OutputFile(OutputFileName));
  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::NumIntegration),
                                      RooFit::OutputFile(OutputFileName));
  RooMsgService::instance().addStream(RooFit::INFO,
                                      RooFit::Topic(RooFit::Generation),
                                      RooFit::OutputFile(OutputFileName));
}

void usage() { 
   cout<<"./exe : runs with default coupling points and all masses from histogram\n"<<"./exe <filename> : runs with configuration from file. See example in ex_inp.txt\n"<<endl;
}

void debug_toy(const RooDataSet *toy_experiment) {
  toy_experiment->Print("V");

  const RooArgSet *test = toy_experiment->get();
  RooCategory *pid_test = static_cast<RooCategory *>(test->find("pid_category"));
  RooRealVar *x = static_cast<RooRealVar *>(test->find("mInv"));
  for (int iEv = 0; iEv < toy_experiment->numEntries(); iEv++) {
    toy_experiment->get(iEv);
    cout<<x->getVal()<<" "<<pid_test->getLabel()<<endl;
  }
}

void add_points_to_tree(map<TString, TTree *> &outputTrees, const points_analyzed_t &pts, TString treeName) {
  map<TString, TTree *>::iterator it = outputTrees.find(treeName);
  if (it != outputTrees.end()) {
    cout<<"Tree: "<< treeName.Data()<<" already exists. Won't write"<<endl;
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
  delete output_file;
}

void do_debug_run(global_config &config, StatisticsModel *full_model) {
  ////
  // NB: to avoid memory leaks in toys, datasets and fit results will be
  // unique_ptr.
  //     This is a bit uglier, but the thing has to run.
  ///
  cout<<"=============  Debug run starts now ==============="<<endl;
  TFile *output_debug = TFile::Open("output_debug.root", "RECREATE");
  cout<<output_debug<<" è il puntatore al file di output creato dalla classe do_debug_run"<<endl;
  TTree *tree_debug = new TTree("statistics", "statistics");
  cout<<tree_debug<<" è il puntatore al tree_debug creato dalla classe do_debug_run"<<endl;
  vector<double> q_toys_sb;
  vector<double> q_toys_bonly;
  vector<double> n_toys_gen;
  tree_debug->Branch("n_gen", &n_toys_gen);
  tree_debug->Branch("q_sb", &q_toys_sb);
  tree_debug->Branch("q_bkg", &q_toys_bonly);

  /////////////////////////////////////////////////////////////
  //////////// Reset computation details //////////////////////
  /////////////////////////////////////////////////////////////

  config.mass_points.clear();
  //config.mass_points.push_back(412.);
  config.log_coupling_points.clear();
  //config.log_coupling_points.push_back(-12.7);
  // config.mass_points.clear();
  // config.mass_points.push_back(545.);
  // config.log_coupling_points.clear();
  // config.log_coupling_points.push_back(-12.44);
  // config.n_toys = 1000;

  full_model->Update(config, config.mass_points[0], config.log_coupling_points[0]);
  cout<<"Mass = "<<config.mass_points[0]<<" logE = "<< config.log_coupling_points[0]<<" yield = "<< full_model->GetTotalYield()<<endl;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////// Define dummy observation/////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  RooDataSet observation("obs", "obs", {*full_model->invariantMass, *full_model->pid_categories});
  // full_model->SetDefaultAuxiliaryObservation(); 

  if (config.useMuMu) { //  questo crea qualche problema ->CAPIRE
    full_model->invariantMass->setVal(411.);
    full_model->pid_categories->setLabel("mumu");
    observation.add(RooArgSet(*full_model->invariantMass, *full_model->pid_categories));
    cout<<"STO PRENDENDO COME INPUT MODEL ee->MuMu"<<endl;
  }
  observation.Print("v");

  RooArgSet* params_init = static_cast<RooArgSet*>(full_model->fit_parameters->snapshot()); //copia i parametri iniziali e li tiene in memoria, così ho una versio che si aggiorna con full model una che rimane ai parametri iniziali
  double q_obs = 0.0;

  // decide which are the models
  RooAbsPdf *sb_model = nullptr;
  cout<<sb_model<<" è il puntatore a sb_model"<<endl;
  RooAbsPdf *bkg_model = nullptr;
  cout<<bkg_model<<" è il puntatore a bkg_model"<<endl;
  if (config.tossingType == kCousinsHighland) { //0 come input file
    sb_model = full_model->sbModel_simultaneous_noConstraints;
    bkg_model = full_model->bkgModel_simultaneous_noConstraints;
  } else if (config.tossingType == kFixToFit) { //1 come input file
    sb_model = full_model->sbModel_full;
    bkg_model = full_model->bkgModel_full;
  }

  cout<<sb_model<<" è il puntatore a sb_model dopo aver dichiarato il modello"<<endl;
  cout<<bkg_model<<" è il puntatore a bkg_model dopo aver dichiarato il modello"<<endl;  

  /////////////////////////////////////////////////////////////////////////
  ///////////////////// Fit under S+B and B only //////////////////////////
  /////////////////////////////////////////////////////////////////////////

  RooArgList fitted_values_sb;
  RooArgList fitted_values_bkg;
  if (config.tossingType == kFixToFit) { //1 come input file
    full_model->fit_parameters->assignFast(*params_init);
    full_model->mu_test->setVal(1.0);
    full_model->mu_test->setConstant(true);
    // unique_ptr<RooFitResult> fr_sb_obs{sb_model->fitTo(
    //     observation, /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
    //     RooFit::Constrain(*full_model->constrained_parameters),
    //     RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintEvalErrors(-1),
    //     RooFit::Warnings(false), RooFit::Verbose(false), RooFit::Save()
    //   )};

    RooFitResult *fr_sb_obs = sb_model->fitTo(
          observation, /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
          RooFit::Constrain(*full_model->constrained_parameters),
          RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintEvalErrors(-1),
          RooFit::Warnings(false), RooFit::Verbose(false), RooFit::Save()
        ); //puntatore modificato da vedere se funziona e cosa cambia.

    fr_sb_obs->Print("V");
    fitted_values_sb = fr_sb_obs->floatParsFinal();
    double nll_sb_obs = fr_sb_obs->minNll();

    full_model->fit_parameters->assignFast(*params_init);
    full_model->mu_test->setVal(0.0);
    full_model->mu_test->setConstant(true);
    RooFitResult *fr_bonly_obs = bkg_model->fitTo(
          observation, /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
          RooFit::Constrain(*full_model->constrained_parameters),
          RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintEvalErrors(-1),
          RooFit::Warnings(false), RooFit::Verbose(false), RooFit::Save()
        );
    // unique_ptr<RooFitResult> fr_bonly_obs{bkg_model->fitTo(
    //     observation, /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
    //     RooFit::Constrain(*full_model->constrained_parameters),
    //     RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintEvalErrors(-1),
    //     RooFit::Warnings(false), RooFit::Verbose(false), RooFit::Save()
    //   )};
    fr_bonly_obs->Print("V");
    fitted_values_bkg = fr_bonly_obs->floatParsFinal();

    if (config.statType == kTevatron) { //0 come input file
      q_obs = 2.0 * (nll_sb_obs - fr_bonly_obs->minNll());
    } else if (config.statType == kLHC) { //1 come input file
      ////////////////////////////////////////////////////////
      ///////////// Fit for maximum likelihood ///////////////
      ////////////////////////////////////////////////////////
      full_model->fit_parameters->assignFast(*params_init);
      full_model->mu_test->setVal(0.5);
      full_model->mu_test->setConstant(false);
      unique_ptr<RooFitResult> fr_unconditional{sb_model->fitTo(
            observation,
            /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
            RooFit::Constrain(*full_model->constrained_parameters),
            RooFit::PrintEvalErrors(-1), RooFit::Minimizer("Minuit2", "Migrad"),
            RooFit::Verbose(false), RooFit::Save()
          )};
      fr_unconditional->Print("V");

      double nll_uncond = fr_unconditional->minNll();
      cout<<"nll_sb = "<<nll_sb_obs<<" nll_uncond = "<<nll_uncond;
      q_obs = 2.0 * (nll_sb_obs - nll_uncond);
    } else if (config.statType == kCLSB) { //2 come input file
        cout<<"nll_sb = "<<nll_sb_obs<<" nll_bkg = "<<fr_bonly_obs->minNll();
        q_obs = 2.0 * nll_sb_obs;
    }
  } else if (config.tossingType == kCousinsHighland) { //0 come input file
      for (auto par : full_model->constraints_map) {
        par.first->setConstant(true);
      }
      full_model->mu_test->setVal(1.0);
      full_model->mu_test->setConstant(true);
      RooNLLVar nll_sb_var("nll_sb_var", "nll_sb_var", *sb_model, observation, RooFit::Extended()); //nll implements -log(likelihood) by itself
      cout<<"nll_sb = "<<nll_sb_var.getVal()<<endl;
      q_obs = nll_sb_var.getVal();

      RooNLLVar nll_bkg_var("nll_bkg_var", "nll_bkg_var", *bkg_model, observation, RooFit::Extended());
      cout<<" nll_bkg = "<<nll_bkg_var.getVal()<<endl;
      q_obs = q_obs - nll_bkg_var.getVal();
      q_obs = 2.0 * q_obs;
    }
  cout<<" q_obs = "<< q_obs<<endl;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////// Run S+B toys ////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  cout<<"Doing S+B toys: "<<config.n_toys<<endl;
  TStopwatch sw;
  int partition = config.n_toys > 10 ? config.n_toys / 10 : 1;
  double n_higher_sb = 0.0;
  RooRealVar n_generated_sb("n_generated_sb", "n_generated_sb", 0., 0., 100.);
  // RooArgSet *n_generated_sb_ARGSET = new RooArgSet("n_generated_sb_ARGSET");
  //RooDataSet ds_ngen("ds_ngen", "ds_ngen", {n_generated_sb_ARGSET});
  RooDataSet ds_ngen("ds_ngen", "ds_ngen", {n_generated_sb});

  for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
    if (partition > 0.) {
      if (iToy % partition == 0)
        cout<<"---starting: "<<iToy<<"/"<<config.n_toys<<endl;
    }

    unique_ptr<RooDataSet> toys_sb{full_model->Generate(config, 1.0, sb_model, fitted_values_sb)};
    // RooDataSet *toys_sb = full_model->Generate(config, 1.0, sb_model, fitted_values_sb); //puntatore modificato 

    n_generated_sb.setVal(toys_sb->numEntries());
    // n_generated_sb_ARGSET->add(n_generated_sb);
    // ds_ngen.add(n_generated_sb_ARGSET);
    ds_ngen.add({n_generated_sb});

    double q = full_model->ComputeQstatistic(config, *toys_sb, params_init);
    q_toys_sb.push_back(q);
    if (q_toys_sb.back() >= q_obs || fabs(q_toys_sb.back() - q_obs) < 1E-3){
      n_higher_sb += 1.0;
    }
  }

  const RooArgSet *variable = ds_ngen.get(); 
  for(int i = 0; i<ds_ngen.numEntries(); ++i){
    ds_ngen.get(i);
    n_toys_gen.push_back(variable->getRealValue("n_generated_sb"));
    tree_debug->Fill();
  }
  tree_debug->Write();
  double time_sb_toys = sw.RealTime();
  cout<<"Done "<<config.n_toys<<" toys in "<<time_sb_toys<<" s"<<endl;

  if (config.statType == kCLSB) { //2 come input file
    cout<<"n_higher_sb = "<<n_higher_sb<<endl;
    cout<<"CL_{SB} = "<<(n_higher_sb / config.n_toys)<<endl;
    return;
  }

  //////////////////////////////////////////////////////////////////////////
  /////////////////////// Run Bkg toys /////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  
  cout<<"Doing Bkg toys: "<<config.n_toys<<endl;
  sw.Continue();
  double n_higher_bkg = 0.0;
  // cout<<"config.n_toys = "<<config.n_toys<<endl;
  for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
    if (partition > 0.) {
      if (iToy % partition == 0){
        cout<<"---starting: "<<iToy<<"/"<<config.n_toys<<endl;
      }
    }

    // unique_ptr<RooDataSet> toys{full_model->Generate(config, 0.0, bkg_model, fitted_values_bkg)};
    RooDataSet *toys = full_model->Generate(config, 0.0, bkg_model, fitted_values_bkg); //puntatore modificato

    double q = full_model->ComputeQstatistic(config, *toys, params_init);
    q_toys_bonly.push_back(q);
    if (q_toys_bonly.back() >= q_obs || fabs(q_toys_bonly.back() - q_obs) < 1E-3){
      n_higher_bkg += 1.0;
    }
  }
  double time_bkg_toys = sw.RealTime() - time_sb_toys;
  cout<<"Done "<<config.n_toys<<" toys in "<<time_bkg_toys <<" s"<< endl;
  tree_debug->Fill();

  cout<<"N higher q (s+b): "<<n_higher_sb<<" N higher q (bkg): "<<n_higher_bkg<<endl;
  double cls = 1.0;
  if (n_higher_bkg > 1E-6){
    cls = n_higher_sb / n_higher_bkg;
  }
  cout<<"CLs = "<<cls<<endl;

  // save and cleanup
  tree_debug->Write();
  output_debug->Close();
  delete output_debug;
  delete params_init;
} //end of do debug

void do_exclusion_run(global_config &config, StatisticsModel *full_model) {
  ////
  // NB: to avoid memory leaks in toys, datasets and fit results will be
  // unique_ptr.
  //     This is a bit uglier, but the thing has to run.
  ///
  cout<<"============  Exclusion run starts now  ==============="<<endl;
  TFile *output_exclusion = TFile::Open("output_exclusion.root", "RECREATE");
  TTree *tree_exclusion = new TTree("CLS", "CLS");
  double cls = 1.0;
  double current_mass = 0.0;
  double current_loge = 0.0;
  tree_exclusion->Branch("Mass", &current_mass);
  tree_exclusion->Branch("LogCoupling", &current_loge);
  tree_exclusion->Branch("CLS", &cls);

  // decide which are the models
  RooAbsPdf *sb_model = nullptr; //creo il puntatore, ma per ora non lo faccio puntare a nulla
  RooAbsPdf *bkg_model = nullptr; // in base al modello faccio sì che punti alla corretta definizione della variabile (in questo caso sono pdf di bkg e sb model)
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

  RooDataSet observation("obs", "obs", {*full_model->invariantMass, *full_model->pid_categories});
  // full_model->SetDefaultAuxiliaryObservation();

  RooArgSet* globalObs = full_model->global_observarbles;
  TIterator* iter = globalObs->createIterator();
  RooAbsArg* globalObsArg;
  while ((globalObsArg = static_cast<RooAbsArg*>(iter->Next()))) {
    observation.addColumn(*globalObsArg);
  }
  delete iter;

  if (config.useMuMu) {
    full_model->invariantMass->setVal(411.);
    full_model->pid_categories->setLabel("mumu");
    observation.add(RooArgSet(*full_model->invariantMass, *full_model->pid_categories));
  }
  observation.Print("v");
  
  RooArgSet *params_init = static_cast<RooArgSet*>(full_model->fit_parameters->snapshot());

  TStopwatch sw;
  sw.RealTime();
  for (size_t iM = 0; iM < config.mass_points.size(); iM++) {
    current_mass = config.mass_points[iM];
    for (size_t iE = 0; iE < config.log_coupling_points.size(); iE++) {
      current_loge = config.log_coupling_points[iE];
      full_model->Update(config, current_mass, current_loge);

      cout<<"Current point: mass = "<<current_mass<<" log(e) = "<<current_loge<<" Total yield = "<< full_model->GetTotalYield()<<endl;

      // if (full_model->IsHighYield()) {
      //   cout << "High yield, setting CLS to 0" << endl;
      //   cls = 0.0;
      //   tree_exclusion->Fill();
      //   continue;
      // }
      // if (full_model->IsLowYield()) {
      //   cout << "Low yield, setting CLS to 1" << endl;
      //   cls = 1.0;
      //   tree_exclusion->Fill();
      //   continue;
      // }

      double q_obs = 0.0;
      RooArgList fitted_values_sb;
      RooArgList fitted_values_bkg;

      if (config.tossingType == kFixToFit) {
        // fit under s+b
        full_model->fit_parameters->assignFast(*params_init);
        full_model->mu_test->setVal(1.0);
        full_model->mu_test->setConstant(true);
        unique_ptr<RooFitResult> fr_sb_obs{sb_model->fitTo(
            observation,
            /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
            RooFit::Constrain(*full_model->constrained_parameters),
            RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
            RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
            RooFit::Verbose(false), RooFit::Save()
          )};
        fitted_values_sb = fr_sb_obs->floatParsFinal();
        double nll_sb_obs = fr_sb_obs->minNll();
        fr_sb_obs->Print("V");

        // fit under bonly
        full_model->fit_parameters->assignFast(*params_init);
        full_model->mu_test->setVal(0.0);
        full_model->mu_test->setConstant(true);
        unique_ptr<RooFitResult> fr_bkg_obs{bkg_model->fitTo(
            observation,
            /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
            RooFit::Constrain(*full_model->constrained_parameters),
            RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
            RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
            RooFit::Verbose(false), RooFit::Save()
          )};
        fitted_values_bkg = fr_bkg_obs->floatParsFinal();
        fr_bkg_obs->Print("V");

        if (config.statType == kLHC) {
          // fit for maximum likelihood
          full_model->fit_parameters->assignFast(*params_init);
          full_model->mu_test->setVal(1.0);
          full_model->mu_test->setConstant(false);
          unique_ptr<RooFitResult> fr_unconditional{sb_model->fitTo(
                  observation,
                  /*RooFit::GlobalObservables(*full_model->global_observarbles),*/
                  RooFit::Constrain(*full_model->constrained_parameters),
                  RooFit::Minimizer("Minuit2", "Migrad"),
                  RooFit::PrintLevel(-1), RooFit::PrintEvalErrors(-1),
                  RooFit::Warnings(false), RooFit::Verbose(false),
                  RooFit::Save()
              )};
          fr_unconditional->Print("V");

          double nll_uncond = fr_unconditional->minNll();
          cout<<"nll_sb = "<<nll_sb_obs<<" nll_uncond = "<<nll_uncond;
          q_obs = 2.0 * (nll_sb_obs - nll_uncond);
        } else {
          cout<<"nll_sb = "<<nll_sb_obs<<" nll_bkg_obs = "<< fr_bkg_obs->minNll();
          q_obs = 2.0 * (nll_sb_obs - fr_bkg_obs->minNll());
        }
      } else {
        for (auto par : full_model->constraints_map) {
          par.first->setConstant(true);
        }
        full_model->mu_test->setVal(1.0);
        full_model->mu_test->setConstant(true);
        RooNLLVar nll_sb_var("nll_sb_var", "nll_sb_var", *sb_model, observation, RooFit::Extended());
        cout<<"nll_sb = "<<nll_sb_var.getVal();
        q_obs = nll_sb_var.getVal();
        RooNLLVar nll_bkg_var("nll_bkg_var", "nll_bkg_var", *bkg_model, observation, RooFit::Extended());
        cout<<" nll_bkg = "<<nll_bkg_var.getVal();
        q_obs = q_obs - nll_bkg_var.getVal();
        q_obs = 2.0 * q_obs;
      }
      cout<<" q_obs = "<<q_obs<<endl;

      /////////////////////////////////////////////////////
      ///////////////// Run S+B toys //////////////////////
      /////////////////////////////////////////////////////

      cout<<"Doing S+B toys: "<<config.n_toys<<endl;
      double n_higher_sb = 0.0;
      double t_start_sb = sw.RealTime();
      sw.Continue();
      for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
        unique_ptr<RooDataSet> toys_sb{full_model->Generate(config, 1.0, sb_model, fitted_values_sb)};
        double q_toy = full_model->ComputeQstatistic(config, *toys_sb, params_init);
        if (q_toy >= q_obs || fabs(q_toy - q_obs) < 1E-3){
          n_higher_sb += 1.0;
        }
      }
      double time_sb_toys = sw.RealTime();
      cout<<"Done "<<config.n_toys<<" toys in "<<(time_sb_toys - t_start_sb)<<" s"<<endl;

      //////////////////////////////////////////////
      /////////////// Run Bkg toys /////////////////
      //////////////////////////////////////////////
      cout<<"Doing Bkg toys: "<<config.n_toys<<endl;
      sw.Continue();
      double n_higher_bkg = 0.0;
      for (Int_t iToy = 0; iToy < config.n_toys; ++iToy) {
        // generate the toy
        unique_ptr<RooDataSet> toys{full_model->Generate(config, 0.0, bkg_model, fitted_values_bkg)};
        double q_toy = full_model->ComputeQstatistic(config, *toys, params_init);

        if (q_toy >= q_obs || fabs(q_toy - q_obs) < 1E-3){
          n_higher_bkg += 1.0;
        }
      }
      double time_bkg_toys = sw.RealTime() - time_sb_toys;
      cout<<"Done "<<config.n_toys<<" toys in "<<time_bkg_toys<<" s"<<endl;

      cout<<"N higher q (s+b): "<<n_higher_sb<<" N higher q (nkg): "<<n_higher_bkg<<endl;
      cls = 1.0;
      if (n_higher_bkg > 1E-6){
        cls = n_higher_sb / n_higher_bkg;
      }
      cout<<"CLs = "<<cls<<endl;

      tree_exclusion->Fill();
    }
  }

  tree_exclusion->Write();
  output_exclusion->Close();
  delete output_exclusion;
  delete params_init;
}

void do_expectedband_run(global_config &config, StatisticsModel *full_model) {
  // NB: to avoid memory leaks in toys, datasets and fit results will be unique_ptr.
  //     This is a bit uglier, but the thing has to run.
  //
  cout<<"============  Expected band run starts now ==============="<<endl;
  TFile *output_exclusion = TFile::Open("output_exclusion_band.root", "RECREATE");
  TTree *tree_exclusion = new TTree("CLS", "CLS");
  double fraction_rejected = 1.0;
  double current_mass = 0.0;
  double current_loge = 0.0;
  tree_exclusion->Branch("Mass", &current_mass);
  tree_exclusion->Branch("LogCoupling", &current_loge);
  // called CLS for drawing later
  tree_exclusion->Branch("CLS", &fraction_rejected);

   /////////////////////////////////////////////////////////////////////////////
   //////////////// Store fit and global parameters ////////////////////////////
   /////////////////////////////////////////////////////////////////////////////

  full_model->SetDefaultAuxiliaryObservation();
  RooArgSet *params_init = static_cast<RooArgSet*>(full_model->fit_parameters->snapshot());

  // compare function - creazione di una funzione lambda che come argomenti ha i valori simulati e misurati di qualcosa e restituisce true/false nel caso in cui il misurato sia maggiore o uguale a quello simulato
  auto compare_q = [](double element, double value) {
    return (element < value) || (fabs(value - element) < 1E-6);
  };

  TStopwatch sw;
  sw.RealTime();
  // for (size_t iM = 0; iM <= 700; iM+=35) { //HARDCODED LOOP MASSA PER MASSA
  //   current_mass = iM;
  for (size_t iM = 0; iM < config.mass_points.size(); iM++) {
    current_mass = config.mass_points[iM];
    for (size_t iE = 0; iE < config.log_coupling_points.size(); iE++) {
      double t_start_point = sw.RealTime();
      sw.Continue();
      current_loge = config.log_coupling_points[iE];

      full_model->Update(config, current_mass, current_loge);

      cout<<"Current point: mass = "<<current_mass<<" log(e) = "<<current_loge<<" Total yield = "<<full_model->GetTotalYield()<<endl;

      if (full_model->IsHighYield()) {
        cout<<"High yield, setting fraction excluded to 1"<<endl;
        fraction_rejected = 1.0;
        tree_exclusion->Fill();
        continue;
      }
      if (full_model->IsLowYield()) {
        cout<<"Low yield, setting fraction excluded to 0"<<endl;
        fraction_rejected = 0.0;
        tree_exclusion->Fill();
        continue;
      }

      /////////////////////////////////////////////////////////////////////////
      ////////////////// Compute pdfs of q under S+B and B ////////////////////
      /////////////////////////////////////////////////////////////////////////
      cout<<"Generating first pseudo-observation"<<endl;
      // generate 1 toy, fit it and compute f(q|S+B) and f(q|B)
      full_model->fit_parameters->assignFast(*params_init);
      unique_ptr<RooDataSet> pseudo_data_aux{full_model->bkgModel_full->generate(*full_model->global_observarbles, 1)};
      full_model->global_observarbles->assignFast(*pseudo_data_aux->get(0));

      unique_ptr<RooDataSet> pseudo_observation{full_model->bkgModel_full->generate(
              {*full_model->invariantMass, *full_model->pid_categories},
              RooFit::Extended(), RooFit::Verbose(true)
          )};

      RooArgSet* pseudo_globalObs = full_model->global_observarbles;
      TIterator* iter = pseudo_globalObs->createIterator();
      RooAbsArg* pseudo_globalObsArg;
      while ((pseudo_globalObsArg = static_cast<RooAbsArg*>(iter->Next()))) {
	      pseudo_observation->addColumn(*pseudo_globalObsArg);
      }
      delete iter;

      pseudo_data_aux->Print("V");
      pseudo_observation->Print("V");

      cout<<"Fitting first pseudo-observation"<<endl;
      // fit under s+b
      full_model->fit_parameters->assignFast(*params_init);
      unique_ptr<RooFitResult> fr_sb_obs{full_model->sbModel_full->fitTo(
          *pseudo_observation,
          RooFit::GlobalObservables(*full_model->global_observarbles),
          RooFit::Constrain(*full_model->constrained_parameters),
          RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
          RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
          RooFit::Verbose(false), RooFit::Save()
        )};
      RooArgList fitted_values_sb = fr_sb_obs->floatParsFinal();
      fr_sb_obs->Print("V");

      // fit under b only
      full_model->fit_parameters->assignFast(*params_init);
      unique_ptr<RooFitResult> fr_bkg_obs{full_model->bkgModel_full->fitTo(
          *pseudo_observation,
          RooFit::GlobalObservables(*full_model->global_observarbles),
          RooFit::Constrain(*full_model->constrained_parameters),
          RooFit::Minimizer("Minuit2", "Migrad"), RooFit::PrintLevel(-1),
          RooFit::PrintEvalErrors(-1), RooFit::Warnings(false),
          RooFit::Verbose(false), RooFit::Save()
        )};
      RooArgList fitted_values_bkg = fr_bkg_obs->floatParsFinal();
      fr_bkg_obs->Print("V");

      cout<<"Computing q distribution"<<endl;

      vector<double> q_distribution_sb;
      full_model->ComputeQDistribution(config, full_model->sbModel_full, fitted_values_sb, params_init, q_distribution_sb);
      sort(q_distribution_sb.begin(), q_distribution_sb.end());
      cout<<"q_sb = [";
      for (size_t iq = 0; iq < q_distribution_sb.size(); ++iq) {
        cout<<" "<<q_distribution_sb[iq];
      }
      cout<<"]"<<endl;

      double t_q_sb = sw.RealTime();
      sw.Continue();
      cout<<"q-distribution for s+b done in "<<(t_q_sb - t_start_point)<<" s"<<endl;

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
      cout<<"q-distribution for bkg done in "<<(t_q_bkg - t_q_sb)<<" s"<<endl;

      /////////////////////////////////////////////////////////////////////////
      ///////////////// Generate rest of pseudo-observations //////////////////
      /////////////////////////////////////////////////////////////////////////

      double n_toys_rejected = 0.0;
      cout<<"Generate rest of pseudo-observations"<<endl;
      double t_start_toys = sw.RealTime();
      sw.Continue();
      int partition = config.n_toys / 10;
      for (int iToy = 0; iToy < config.n_toys; iToy++) {
        if (config.n_toys > 10) {
          if (iToy % partition == 0) {
            cout<<"Doing pseudo-observation "<<iToy<<"/"<<config.n_toys<<endl;
          }
        }

        full_model->fit_parameters->assignFast(*params_init);
        unique_ptr<RooDataSet> toy_aux{full_model->bkgModel_full->generate(*full_model->global_observarbles, 1)};
        full_model->global_observarbles->assignFast(*toy_aux->get(0));

        unique_ptr<RooDataSet> toy_observation{full_model->bkgModel_full->generate(
                {*full_model->invariantMass, *full_model->pid_categories},
                RooFit::Extended(), RooFit::Verbose(false)
            )};

	      RooArgSet* toy_globalObs = full_model->global_observarbles;
	      TIterator* iter = toy_globalObs->createIterator();
	      RooAbsArg* toy_globalObsArg;
	      while ((toy_globalObsArg = static_cast<RooAbsArg*>(iter->Next()))) {
	        toy_observation->addColumn(*toy_globalObsArg);
	      }
	      delete iter;

        double q = full_model->ComputeQstatistic(config, *toy_observation, params_init);
        // compute CLS for this pseudo-observation
        vector<double>::iterator it_sb = upper_bound(q_distribution_sb.begin(), q_distribution_sb.end(), q, compare_q);
        double n_higher_sb = 1.0 * config.n_toys - 1.0 * (it_sb - q_distribution_sb.begin());

        vector<double>::iterator it_bkg = upper_bound(q_distribution_bkg.begin(), q_distribution_bkg.end(), q, compare_q);
        double n_higher_bkg = 1.0 * config.n_toys - 1.0 * (it_bkg - q_distribution_bkg.begin());

        double cls = 1.0;
        if (n_higher_bkg > 1E-6){
          cls = n_higher_sb / n_higher_bkg;
        }
        // cout << "q = " << q <<   " n_higher_sb = " << n_higher_sb << "
        // n_higher_bkg = " << n_higher_bkg << " cls = " << cls << endl;
        if (cls <= 1.0 - config.cl){
          n_toys_rejected += 1.0;
        }
      }

      fraction_rejected = n_toys_rejected / config.n_toys;
      cout<<"Fraction rejected: "<<fraction_rejected<<endl;
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

 // /////////////////////////////////////////////////////////////
 // //////////// Set up a few convenience things ////////////////
 // /////////////////////////////////////////////////////////////
 config.retrieve_input();
 
 RooRandom::randomGenerator()->SetSeed(0);
 
 //////////////////////////////////////////////////////////////
 ///////////// Configure and build model //////////////////////
 //////////////////////////////////////////////////////////////
 config.configure_run(argc, argv);
 silence_roofit(config.OutputName); //se lo commento stampa sul terminale cosa sta facendo per favorire il debugging
 config.print_config();
 
 StatisticsModel *full_model = new StatisticsModel(config);
 if (config.useEE) full_model->AddChannel("ee", kGaussian, kLandau, 0.0094, 4.0);  
 if (config.useMuMu) full_model->AddChannel("mumu", kGaussian, kBernstein, 0.0087, 1.13);
 full_model->BuildModel();
 //////////////////////////////////////////////////////////////
 /////////// Select what we're doing //////////////////////////
 //////////////////////////////////////////////////////////////
 if (config.runType == kDebugRun) { //0 come input file
   do_debug_run(config, full_model);
 } 
 else if (config.runType == kExclusionRun) { //1 come input file
    do_exclusion_run(config, full_model);
 } else if (config.runType == kExpectedBandRun) {//2 come input file
    do_expectedband_run(config, full_model);
 }
 
 TF1 *f1 = new TF1("f1", "[0]/(TMath::Sqrt(2.0*TMath::Pi())*TMath::Log([1])*x) * "
		   "TMath::Exp(-0.5*(TMath::Log(x)-TMath::Log([2]))*(TMath::Log(x)-"
		   "TMath::Log([2]))/(TMath::Log([1])*TMath::Log([1])))");
 std::cout<<"Final DONE from Main!"<<std::endl;
 close_output(outputTrees);

 return 0;
}
