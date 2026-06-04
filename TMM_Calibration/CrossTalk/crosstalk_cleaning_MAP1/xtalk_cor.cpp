///////////////////////////////////////
//  Cross Talk Cleaning Tool        //
//  Created by Edoardo Farina       //
//  On 08/05/2014                   //
//                                  //
//  edoardo.maria.farina@cern.ch    //
//////////////////////////////////////


#include <vector>
#ifdef __MAKECINT__
#pragma link C++ class vector <vector <short> >+;

#pragma link C++ class vector <vector <short> >+;
#endif
#include <iostream>
#include <string>
#include <sstream>
#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TFile.h"
#include "TMath.h"
#include "TLatex.h"
#include "TBranch.h"
#include "TTree.h"
#include "TKey.h"
#include "TClass.h"
#include "TDirectory.h"
#include "TObjString.h"

//TFile *root_file;
#include "APV_RAW_PED.h"
#include "apv_raw.h"
apv_raw *apv_raw_obj;
APV_RAW_PED *apv_raw_ped;

using namespace std;

void CopyDir(TDirectory *source) {

    TDirectory *savdir = gDirectory;
    TDirectory *adir = savdir->mkdir("config");
    adir->cd();

    TKey *key;
    TIter nextkey(source->GetListOfKeys());
    while ((key = (TKey*)nextkey())) {
        const char *classname = key->GetClassName();
        TClass *cl = gROOT->GetClass(classname);
        if (!cl) continue;
        if (cl->InheritsFrom(TDirectory::Class())) {
            source->cd(key->GetName());
            TDirectory *subdir = gDirectory;
            adir->cd();
            CopyDir(subdir);
            adir->cd();
        } else {
            source->cd();
            TObject *obj = key->ReadObj();
            TObjString *takis = new TObjString(obj->GetName());
            savdir->cd();
            takis->Write(key->GetName());
        }
    }
    adir->SaveSelf(kTRUE);
    savdir->cd();
}

short max_charge(vector <short> charge){

    vector <short>::iterator it = max_element(charge.begin(), charge.end());
    return *it;

}

double get_time_bin_max(vector <short> charge){
    
    vector <short>::iterator it = max_element(charge.begin(), charge.end());
    double pos;
    pos= (it- charge.begin())*25;
    return pos;

}

double get_time_bin_bin_max(vector <short> charge){
    
    vector <short>::iterator it = max_element(charge.begin(), charge.end());
    double pos;
    pos= (it- charge.begin());
    return pos;
    
}

int main(int argc, char *argv[]){

    cout<<"**************************************************"<<endl;
    cout<<"*          Cross Talk Cleaner MAP 1              *"<<endl;
    cout<<"*          Date: February 19, 2016               *"<<endl;
    cout<<"*          Created by Edoardo Farina             *"<<endl;
    cout<<"*                                                *"<<endl;
    cout<<"*        This program cleanes the raw data       *"<<endl;
    cout<<"*            from the crosstalk effect           *"<<endl;
    cout<<"*      provide the filename as parameter         *"<<endl;
    cout<<"**************************************************"<<endl;

    string file = argv[1];
    
    bool cleaning =true;
    double factor=0; int mmReadout=50 ; int charge_cut =0;
     
	TFile *root_file = new TFile(file.c_str(),"READ");
	apv_raw_obj = new apv_raw((TTree*)root_file->Get("apv_raw"));
	
    double entries = (apv_raw_obj->fChain)->GetEntries();

    size_t found_run = file.find("run");
    size_t found_dotroot = file.find(".root");
    string run_name = file.substr(found_run,found_dotroot);
    string path = file.substr(0,found_run);
        
    vector <double> first, second;
    ifstream InputRead ("apv-crosstalk1.map");

    while (!InputRead.eof()) {
        double a;
        double b;
        double c;
        
        InputRead >> a >> b >> c;
        // cout << a << endl;
        first.push_back(a);
        second.push_back(b);
    }
    
	// int zeroed = 0;
	// int subtracted = 0;
    // entries = 10000;
    // CLEAning
    
    string new_file = run_name+"_xtalk.root";
    string action;
    action="RECREATE";
    
    TFile *ntuple = new TFile(new_file.c_str(), action.c_str());
    TTree *aux;
    aux = new TTree("apv_raw", "apv_raw");
   
    ULong64_t                write_evt;
    ULong64_t                write_evt_temp=0;
    UInt_t                   write_error;
    Int_t                    write_daqTimeSec;
    Int_t                    write_daqTimeMicroSec;
    Int_t                    write_srsTimeStamp;
    UInt_t                   write_srsTrigger;
    vector <unsigned int>    write_srsFec;
    vector <unsigned int>    write_srsChip;
    vector <unsigned int>    write_srsChan;
    vector <string>          write_mmChamber;
    vector <int>             write_mmLayer;
    vector <char>            write_mmReadout;
    vector <int>             write_mmStrip;
    vector <vector <short> > write_raw_q;

    if (cleaning){
    aux->Branch("evt"              , &write_evt            );
    aux->Branch("error"            , &write_error          );
    aux->Branch("daqTimeSec"       , &write_daqTimeSec     );
    aux->Branch("daqTimeMicroSec"  , &write_daqTimeMicroSec);
    aux->Branch("srsTimeStamp"     , &write_srsTimeStamp   );
    aux->Branch("srsTrigger"       , &write_srsTrigger     );
    aux->Branch("srsFec"           , &write_srsFec         );
    aux->Branch("srsChip"          , &write_srsChip        );
    aux->Branch("srsChan"          , &write_srsChan        );
    aux->Branch("mmChamber"        , &write_mmChamber      );
    aux->Branch("mmLayer"          , &write_mmLayer        );
    aux->Branch("mmReadout"        , &write_mmReadout      );
    aux->Branch("mmStrip"          , &write_mmStrip        );
    aux->Branch("raw_q"            , &write_raw_q    );
    }
    
    ULong64_t               evt_;

    UInt_t                  error_;
    Int_t                   daqTimeSec_;
    Int_t                   daqTimeMicroSec_;
    Int_t                   srsTimeStamp_;
    UInt_t                  srsTrigger_;
    vector <unsigned int>   srsFec_;
    vector <unsigned int>   srsChip_;

    vector <string>         mmChamber_;
    vector <int>            mmLayer_;
    vector <char>           mmReadout_;
    vector <int>            mmStrip_;
    
    //LIST OF VARIABLES:
    
    vector <double> channels_vec;
    vector < vector < short > > charge_vec_vec;
    int number_of_crosstalk;
    vector <int> cross_talk_vec;
    vector <double> vec_temp_cross_talk;
    vector <double> cross_effect_event;
    
    vector <double> charge_vec_distribution [2];
    vector <double> time_vec_distribution [2];
    vector <double > time_crosstalked_father[2];
    vector <double> strip_father;
    vector <double> strip_original;
    vector <double> strip_father_good;
    vector <double> charge_father_second_son;
    vector <double> charge_father_third_son;
    vector <double> charge_first_son_vec;
    vector <double> charge_father_for_sons;
    
	for(int iEntry=0; iEntry<entries; iEntry++) {
		if(iEntry%200==0)
        cout << "\r" << (double)iEntry/(double)entries*100 << "\% processed" << flush;

		(apv_raw_obj->fChain)->GetEntry(iEntry);

        write_srsFec.clear();
        write_srsChip.clear();
        write_srsChan.clear();
        write_mmChamber.clear();
        write_mmLayer.clear();
        write_mmReadout.clear();
        write_mmStrip.clear();
        write_raw_q.clear();
        
        //starting for on FEC and on chip.
        for (int isrsFec=1; isrsFec<=2; isrsFec++) {
            for (int isrsChip=0; isrsChip<=16; isrsChip++) {
                for (int j=0; j<(int)apv_raw_obj->srsFec->size(); j++) { // loop on all entries of the event
                    if(apv_raw_obj->srsFec->at(j)==isrsFec && apv_raw_obj->srsChip->at(j)==isrsChip && 
                        max_charge(apv_raw_obj->raw_q->at(j))> charge_cut && (apv_raw_obj->mmReadout->at(j)== mmReadout || cleaning)){
        
                        channels_vec.push_back(apv_raw_obj->srsChan->at(j));
                        charge_vec_vec.push_back(apv_raw_obj->raw_q->at(j));
                      
                        evt_=apv_raw_obj->evt;
                        error_= apv_raw_obj->error;
                        daqTimeSec_ = apv_raw_obj->daqTimeSec;
                        daqTimeMicroSec_= apv_raw_obj->daqTimeMicroSec;
                        srsTimeStamp_ = apv_raw_obj->srsTimeStamp;
                        srsTrigger_ = apv_raw_obj->srsTrigger;
                        srsFec_.push_back(apv_raw_obj->srsFec->at(j));
                        srsChip_.push_back(apv_raw_obj->srsChip->at(j));
                        mmChamber_.push_back(apv_raw_obj->mmChamber->at(j));
                        mmLayer_.push_back(apv_raw_obj->mmLayer->at(j));
                        mmStrip_.push_back(apv_raw_obj->mmStrip->at(j));
                        mmReadout_.push_back(apv_raw_obj->mmReadout->at(j));
                        
                        strip_original.push_back(apv_raw_obj->mmStrip->at(j));
                    }
                } //end of internal loop of entries
          
                for (int i=0; i < channels_vec.size(); i++){
                
                    if (channels_vec.size() >0){   
                        vector <double>::iterator it = channels_vec.begin()+i;
                    
                        //look into vector of affected channels to find the position
                        vector <double>::iterator pp = find(second.begin(), second.end(), *it);
                        
                        int pos = pp - second.begin();
                        // pos is the postion in the affected vector of the channel.
                        
                        vector <double>::iterator pp2 = find(channels_vec.begin(), channels_vec.end(), first[pos]);
                        // I see wheter the channel at the same position on the vector "first" exists in channels_vec
                        
                        int pos_in_channel_for_charge = pp2-channels_vec.begin();
                        // Position in channels vec
                        
                        // to put a limit in the charge add this:
                        if(pp2!=channels_vec.end()&& (max_charge(charge_vec_vec[i]))< max_charge(charge_vec_vec[pos_in_channel_for_charge]) && abs(get_time_bin_bin_max(charge_vec_vec[i]) - get_time_bin_bin_max(charge_vec_vec[pos_in_channel_for_charge])) < 3 ){
                            //DON't do anything   
                        } else {
                            bool cond = true;
                            number_of_crosstalk=0;
                            
                            write_evt =  evt_;
        
                            write_error = error_;
                            write_daqTimeSec = daqTimeSec_;
                            write_daqTimeMicroSec = daqTimeMicroSec_;
                            write_srsTimeStamp = srsTimeStamp_;
                            write_srsTrigger = srsTrigger_;
                            write_srsFec.push_back(srsFec_[i]);
                            write_srsChip.push_back(srsChip_[i]);
                            write_mmChamber.push_back(mmChamber_[i]);
                            write_mmLayer.push_back(mmLayer_[i]);
                            write_mmReadout.push_back(mmReadout_[i]);
                            write_mmStrip.push_back(mmStrip_[i]);
                            write_raw_q.push_back(charge_vec_vec[i]);
                            write_srsChan.push_back(channels_vec[i]);
                            
                            double charge_of_father = max_charge(charge_vec_vec[i]);
                            //this variable contain the max charge of the father
                            
                            double charge_of_first_son;
                            int time_father_max_bin;
                            // this variable contains the time of the father to be compared with the other.

                            while(cond){
                                //start the search of the son
                                vector <double>::iterator pp3 = find(first.begin(), first.end(), *it);
                                int pos_ = pp3 - first.begin();
                                //Look for my channel in the "first" vector and look for the position
                        
                                vector <double>::iterator pp5 = find(channels_vec.begin(), channels_vec.end(), *it);
                                int pos_father = pp5 - channels_vec.begin();
                                // position in the channel_vec of my channel

                                vector <double>::iterator pp4 = find(channels_vec.begin(), channels_vec.end(), second[pos_]);
                                int pos_son = pp4 - channels_vec.begin();
                                // look for the channel affected by my channel in channels_vec
                        
                                if (number_of_crosstalk==0){
                                    time_father_max_bin=get_time_bin_max(charge_vec_vec[pos_father]);
                                    // this vector contains all the father and the good events
                                    strip_father_good.push_back(mmStrip_[i]);
                                }
                        
                                if(pp4!=channels_vec.end() && (max_charge(charge_vec_vec[pos_father])- max_charge(charge_vec_vec[pos_son]))>0 && abs(get_time_bin_bin_max(charge_vec_vec[pos_father]) - get_time_bin_bin_max(charge_vec_vec[pos_son])) < 3){
                        
                                    //HAS SON!!!!!! so crosstalk guys!!!!
                                    number_of_crosstalk++;
                    
                                    vector <short> temp;
                                    for (int m=0; m<write_raw_q.back().size(); m++){
                                        if(charge_vec_vec[pos_son][m]>0)temp.push_back ((charge_vec_vec[pos_son][m]*factor/100)+write_raw_q.back()[m]);
                                        else temp.push_back (write_raw_q.back()[m]);
                                    }
                                    
                                    write_raw_q.pop_back();
                                    write_raw_q.push_back(temp);
                    
                                    ///// FILL THE CHARGE DISTRIBUTION

                                    if (number_of_crosstalk==1){
                                        charge_of_first_son=max_charge(charge_vec_vec[pos_son]);
                                        charge_vec_distribution[1].push_back(max_charge(charge_vec_vec[pos_son]));
                                        charge_vec_distribution[0].push_back(max_charge(charge_vec_vec[pos_father]));
                                        time_vec_distribution[0].push_back(get_time_bin_max(charge_vec_vec[pos_father]));
                                        time_crosstalked_father[0].push_back(time_father_max_bin-get_time_bin_max(charge_vec_vec[pos_father]));
                                        strip_father.push_back(mmStrip_[i]);
                                    }
                
                                    // FILL THE TIME DISTRIBUTION
                    
                                    if (number_of_crosstalk==1) time_vec_distribution[1].push_back(get_time_bin_max(charge_vec_vec[pos_son])); 
                                    //time difference between first father and crosstlaked channel
                                    if (number_of_crosstalk==1) time_crosstalked_father[1].push_back(time_father_max_bin-get_time_bin_max(charge_vec_vec[pos_son]));
                                    
                                    it = find(first.begin(), first.end(), second[pos_]);
                                    cond = false;

                                } else {
                                    cond = false;
                                }
                            } //end while
                    
                            cross_talk_vec.push_back(number_of_crosstalk);
                            vec_temp_cross_talk.push_back(number_of_crosstalk);
                            charge_father_for_sons.push_back(charge_of_father);
                    
                        } // finish else (if no father)
                    }
                }//for on all the channels in one event
            
                channels_vec.clear();
                charge_vec_vec.clear();
    
                srsFec_.clear();
                srsChip_.clear();
                mmChamber_.clear();
                mmLayer_.clear();
                mmStrip_.clear();
                mmReadout_.clear();
            
            } //END OF CHIP
        } // END OF FEC
        
        vector <double>::iterator gg =max_element (vec_temp_cross_talk.begin(), vec_temp_cross_talk.end());
        cross_effect_event.push_back(*gg);
        vec_temp_cross_talk.clear();
        
        if (cleaning)
        aux->Fill();
        
    } //end of for on the events
    
    //copy all the Pedestal information
    apv_raw_ped = new APV_RAW_PED((TTree*)root_file->Get("apv_raw_ped"));
    int entries_ped = (apv_raw_ped->fChain)->GetEntries();
    TTree *aux_ped;
    aux_ped = new TTree("apv_raw_ped", "apv_raw_ped");
    ULong64_t        write_pedevt;
    UInt_t           write_pederror;
    Int_t            write_peddaqTimeSec;
    Int_t            write_peddaqTimeMicroSec;
    Int_t            write_pedsrsTimeStamp;
    UInt_t           write_pedsrsTrigger;
    vector <unsigned int> write_pedsrsFec;
    vector <unsigned int> write_pedsrsChip;
    vector <unsigned int> write_pedsrsChan;
    vector <string>  write_pedmmChamber;
    vector <int>     write_pedmmLayer;
    vector <char>    write_pedmmReadout;
    vector <int>     write_pedmmStrip;
    vector <double>  write_pedped_mean;
    vector <double>  write_pedped_stdev;
    vector <double>  write_pedped_sigma;
    aux_ped->Branch("evt"              , &write_pedevt            );
    aux_ped->Branch("error"            , &write_pederror          );
    aux_ped->Branch("daqTimeSec"       , &write_peddaqTimeSec     );
    aux_ped->Branch("daqTimeMicroSec"  , &write_peddaqTimeMicroSec);
    aux_ped->Branch("srsTimeStamp"     , &write_pedsrsTimeStamp   );
    aux_ped->Branch("srsTrigger"       , &write_pedsrsTrigger     );
    aux_ped->Branch("srsFec"           , &write_pedsrsFec         );
    aux_ped->Branch("srsChip"          , &write_pedsrsChip        );
    aux_ped->Branch("srsChan"          , &write_pedsrsChan        );
    aux_ped->Branch("mmChamber"        , &write_pedmmChamber      );
    aux_ped->Branch("mmLayer"          , &write_pedmmLayer        );
    aux_ped->Branch("mmReadout"        , &write_pedmmReadout      );
    aux_ped->Branch("mmStrip"          , &write_pedmmStrip        );
    aux_ped->Branch("ped_mean"         , &write_pedped_mean       );
    aux_ped->Branch("ped_stdev"        , &write_pedped_stdev      );
    aux_ped->Branch("ped_sigma"        , &write_pedped_sigma      );
    
    cout << endl;
    for(int iEntry=0; iEntry<entries_ped; iEntry++) {
        
        if(iEntry%200==0) cout << setprecision(2) << "\r" << (double)iEntry/(double)entries_ped*100 << "\% processed" << flush;
        
        (apv_raw_ped->fChain)->GetEntry(iEntry);
        write_pedsrsFec.clear();
        write_pedsrsChip.clear();
        write_pedsrsChan.clear();
        write_pedmmChamber.clear();
        write_pedmmLayer.clear();
        write_pedmmReadout.clear();
        write_pedmmStrip.clear();
        
        write_pedevt = apv_raw_ped->evt;
        write_pederror = apv_raw_ped->error;
        write_peddaqTimeSec = apv_raw_ped->daqTimeSec;
        write_peddaqTimeMicroSec = apv_raw_ped->daqTimeMicroSec;
        write_pedsrsTimeStamp = apv_raw_ped->srsTimeStamp;
        write_pedsrsTrigger = apv_raw_ped->srsTrigger;
        for (int j=0; j<(int)apv_raw_ped->srsFec->size(); j++) {
            write_pedsrsFec.push_back(apv_raw_ped->srsFec->at(j));
            write_pedsrsChip.push_back(apv_raw_ped->srsChip->at(j));
            write_pedsrsChan.push_back(apv_raw_ped->srsChan->at(j));
            write_pedmmChamber.push_back(apv_raw_ped->mmChamber->at(j));
            write_pedmmLayer.push_back(apv_raw_ped->mmLayer->at(j));
            write_pedmmReadout.push_back(apv_raw_ped->mmReadout->at(j));
            write_pedmmStrip.push_back(apv_raw_ped->mmStrip->at(j));
            write_pedped_mean.push_back(apv_raw_ped->ped_mean->at(j));
            write_pedped_stdev.push_back(apv_raw_ped->ped_stdev->at(j));
            write_pedped_sigma.push_back(apv_raw_ped->ped_sigma->at(j));
        }
        aux_ped->Fill();
    }
    
    if(cleaning){
        ntuple->cd();
        TDirectory *target = gDirectory;
        target->cd();
        CopyDir(root_file);
        target->cd();
        ntuple->Write();
        ntuple->Close();
    }
} //end of run function
