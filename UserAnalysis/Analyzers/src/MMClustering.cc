#include "MMClustering.hh"
#include "GeneralInfo.hh"

MMClustering* MMClustering::fInstance = 0;

MMClustering* MMClustering::GetInstance(){
  if ( fInstance == 0 ) { fInstance = new MMClustering(); }
  return fInstance;
}

MMClustering::MMClustering() {
  for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) {
    for(Int_t clumode=0; clumode<CLUSTERMODES; clumode++) {
      fMMClusters[ipmode][clumode].clear();
      fMergedMMClusters[ipmode][clumode].clear();
    }
  }
  fMMSoftHits.clear();
  fHS = HistoSvc::GetInstance();
  InitHistos();
}

MMClustering::~MMClustering() {
  for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) {
    for(Int_t clumode=0; clumode<CLUSTERMODES; clumode++) {
      fMMClusters[ipmode][clumode].clear();
      fMergedMMClusters[ipmode][clumode].clear();
    }
  }
  fMMSoftHits.clear();
}


void MMClustering::Init(TMMRecoEvent* mmevent){
  Int_t nhits = mmevent->GetNHits();

  for (int i=0; i<16; i++){
    for (int j=0; j<2; j++) fNHitsPerAPV[j][i] = 0;
  }

  for(Int_t h=0; h<nhits; h++) {
    MMSoftHit *softhit = new MMSoftHit();
    softhit->CopyHit(mmevent->Hit(h));
    MMchInfo mmi_hit = softhit->GetMMchInfo();
    int apvid = 0;
    if (mmi_hit.strip >= 128) apvid = 1;
    fNHitsPerAPV[apvid][mmi_hit.bdid]++;
      
    //isolation
    Int_t isollevel = 0;
    if(h>0) {
      TRecoVHit *hit_pre = mmevent->Hit(h-1);
      MMchInfo mmi_hit_pre = GeneralInfo::GetInstance()->DecodeMMChannel(hit_pre->GetChannelId());
      if(abs(mmi_hit.strip-mmi_hit_pre.strip)>1) isollevel += 1;
    }
    if(h<nhits-1) {
      TRecoVHit *hit_post = mmevent->Hit(h+1);
      MMchInfo mmi_hit_post = GeneralInfo::GetInstance()->DecodeMMChannel(hit_post->GetChannelId());
      if(abs(mmi_hit.strip-mmi_hit_post.strip)>1) isollevel += 2;
    }
    softhit->SetIsolLevel(isollevel);

    fMMSoftHits.push_back(softhit);
  }
}

Bool_t MMClustering::InitHistos(){
  // MMClustering directory will contain all histograms related to this analysis

  fHS->CreateList("MMClustering");
  cout<<" Creating MMClustering Hystograms"<<endl;    
  return true;
}

void MMClustering::Clear() {
  for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) {
    for(Int_t clumode=0; clumode<CLUSTERMODES; clumode++) {
		for(UInt_t i=0; i<fMMClusters[ipmode][clumode].size(); i++){
			delete fMMClusters[ipmode][clumode].at(i);
		}
    	fMMClusters[ipmode][clumode].clear();
    	fMergedMMClusters[ipmode][clumode].clear();
		for (auto it = begin (fIndicesOfMergedClusters[ipmode][clumode]); it != end (fIndicesOfMergedClusters[ipmode][clumode]); ++it) {
      		it->clear();
	    }
    }
  }
  
	for(UInt_t i=0; i<fMMSoftHits.size(); i++){
		delete fMMSoftHits.at(i);
	}
	fMMSoftHits.clear();
 
}

void MMClustering::Print() {
  for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) {
    for(Int_t clumode=0; clumode<CLUSTERMODES; clumode++) {
      for(Int_t i=0; i<(int) fMMClusters[ipmode][clumode].size(); i++) {
	fMMClusters[ipmode][clumode].at(i)->Print();
      }
    }
  }
}

void MMClustering::Clusterize() {

  Int_t mmSoftHits_size = fMMSoftHits.size();
  if(mmSoftHits_size<=1) return;

  /*for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) {
    bool openclu = kFALSE;
    for(Int_t h=1; h<mmSoftHits_size; h++) {
      caso openClu;
         vedo se posso inserire hit nrl cluster esistente  test di compatibilita agisce diversamente su doppietto o su multipletto
	   OK? abbiunbo hig
	   !OK chiudo clu (se ce 1 solo hit lo distruggo) apro prossimo clu e ci metto hit presente
      caso !openClu;
         inserisco hit come primo di un doppietto
	   aproclu: openclu = kTRUE
  */	   

  //CLUSTERING LEVEL 0  

  for(Int_t ipmode=0; ipmode<IPMODES; ipmode++) { // cluster done with and without ip constraint
    
    // open first temporary cluster    

    MMCluster* new_clu = new MMCluster(ipmode, 0);
    new_clu->AddHit(fMMSoftHits.at(0));

    // hit loop
    
    for(Int_t h=1; h<mmSoftHits_size; h++) {

      bool added = new_clu->AddHit(fMMSoftHits.at(h));

      if(!added) { // hit cannot be added
	if(new_clu->GetHitsVectorSize()>1) { // store the cluster if >1 hits are in it
	  bool hitrejected = new_clu->HitRejectionAlgorithm(); // hit rejection algorithm before storing the cluster 
	  for (int j=0; j<(int)new_clu->GetHitsVectorSize(); j++) new_clu->GetHit(j)->SetCluPtr((fMMClusters[ipmode][0].size()), ipmode, 0); // store the map hit --> clu
	  fMMClusters[ipmode][0].push_back(new_clu); // store the cluster
	  fMergedMMClusters[ipmode][0].push_back(kFALSE); // store the cluster
	}
	else delete new_clu; // only 1 hit -> cluster deleted
	new_clu = new MMCluster(ipmode, 0); // create a new temporary cluster with the present hit
	new_clu->AddHit(fMMSoftHits.at(h)); 
      }
    }

    // treat last temporary open cluster    

    if(new_clu->GetHitsVectorSize()>1) {
      for (int j=0; j<(int) new_clu->GetHitsVectorSize(); j++) new_clu->GetHit(j)->SetCluPtr((fMMClusters[ipmode][0].size()), ipmode, 0);
      bool hitrejected = new_clu->HitRejectionAlgorithm(); // hit rejection algorithm before storing the cluster 
      fMMClusters[ipmode][0].push_back(new_clu);
      fMergedMMClusters[ipmode][0].push_back(kFALSE); // store the cluster
    }
    else delete new_clu;
  } // loop over ipmode
  
//original code    for(Int_t h=1; h<fMMSoftHits_size; h++) {
//original code      MMchInfo mmi_pre = fMMSoftHits.at(h-1)->GetMMchInfo();
//original code      MMchInfo mmi_now = fMMSoftHits.at(h)->GetMMchInfo();
//original code      if((mmi_pre.bdid == mmi_now.bdid) && //geometric compatibility (same board)
//original code	 (abs(mmi_now.strip - mmi_pre.strip) < maxholes+1)) { //hole check WAS ">" 
//original code	bool added = new_clu->AddHit(fMMSoftHits.at(h));
//original code	if(!added) {
//original code	  if(new_clu->GetHitsVectorSize()>1) fMMClusters[ipmode][0].push_back(new_clu);
//original code	  else delete new_clu; //poi vediamo se mettere un clear :)
//original code	  new_clu = new MMCluster(ipmode, 0);
//original code	  new_clu->AddHit(fMMSoftHits.at(h)); 
//original code	}
//original code	else fMMSoftHits.at(h)->SetCluPtr((fMMCluster[ipmode][0].size()-1), ipmode, 0); //non sono sicuro che la prima hit abbia registrato il clu ptr [sono sicuro che non sia cosi] 
//original code      }
//original code      else { //out geometric compatibility
//original code	if(new_clu->GetHitsVectorSize()>1) fMMClusters[ipmode][0].push_back(new_clu);
//original code	else delete new_clu; //poi vediamo se mettere un clear :)
//original code	new_clu = new MMCluster(ipmode, 0);
//original code	new_clu->AddHit(fMMSoftHits.at(h));
//original code      }
//original code    }
//original code  }
  
  //CLUSTERING LEVEL 1, only done with ipmode = 0 --> using ip

  for (int i=0; i<(int) fMMClusters[0][0].size(); i++){ 
    if (fMergedMMClusters[0][0].at(i)) continue;
    MMCluster* new_clu = new MMCluster(0, 1); // temporary cluster of type 1
    new_clu->Import(fMMClusters[0][0].at(i)); // copy info from i-th cluster to new temporary cluster
    bool matched = kFALSE;
    for (int j=i+1; j<(int) fMMClusters[0][0].size(); j++){ 
      if (fMergedMMClusters[0][0].at(j)) continue;
      
      //if (new_clu->MergeAcrossPlanes(fMMClusters[0][0].at(j))) { //TO BE IMPLEMENTED //NON CHIAMARE MERGE ACROSS PLANES MA MergeAcrossPlanesWithDz almeno quando passa per FindBestTrack --> clustering alternativo?
      if (new_clu->MergeAcrossPlanesWithdZ(fMMClusters[0][0].at(j))) { //TO BE IMPLEMENTED //NON CHIAMARE MERGE ACROSS PLANES MA MergeAcrossPlanesWithDz almeno quando passa per FindBestTrack --> clustering alternativo?
	fMMClusters[0][1].push_back(new_clu);
	fMergedMMClusters[0][0].at(i) = kTRUE; // store the cluster-merged flag
	fMergedMMClusters[0][0].at(j) = kTRUE; // store the cluster-merged flag

	// store link between this level one cluster and all level zero clusters contributing to it
	vector<int> indicesLevelZero;
	indicesLevelZero.push_back(i);
	indicesLevelZero.push_back(j);
	fIndicesOfMergedClusters[0][1].push_back(indicesLevelZero);
	matched = kTRUE;
	break;
      }
    }
    if (!matched) delete new_clu;
  }
}


/*
double v_now=0,v_pre=0;
      double z_now=0,z_pre=0;
      
      if(mmi_now.view == XVIEW) {
	v_now = mmSoftHits.at(h)->GetPosition().X();
	v_pre = mmSoftHits.at(h-1)->GetPosition().X();
      }
      else if(mmi_now.view == YVIEW) {
	v_now = mmSoftHits.at(h)->GetPosition().Y();
	v_pre = mmSoftHits.at(h-1)->GetPosition().Y();
      }
      else {
	std::cerr<<"[Clustering] VIEW NOT ASSIGNED TO HIT!!! Hit index: "<<h<<std::endl;
	exit(1);
      }
      
      z_now = mmSoftHits.at(h)->GetPosition().Z();
      z_pre = mmSoftHits.at(h-1)->GetPosition().Z();
*/    


  //clustering algorithm
  /*
  if(vhits_indexes_size>1) {
    Track cluster; //first hit -> open first cluster
    cluster.nhit = 1;
    cluster.view = vhits_in[vhits_indexes[0]].view;
    cluster.quad = vhits_in[vhits_indexes[0]].quad;
    cluster.plane = vhits_in[vhits_indexes[0]].plane;
    cluster.ch = vhits_in[vhits_indexes[0]].ch;
    if(first_clustering) cluster.clustering = 0;
    else                 cluster.clustering = 1;
    
    double z_avg = 0, v_avg = 0, zv_avg = 0, z2_avg = 0; 
    double mt_avg=0;
    double ct_avg=0;
    int counter_pairs = 0; 
    
	
	double deltaV = fabs(v_now - v_pre);

	if((deltaV < (1+maxholes)*pitch+0.001)) {
	  counter_pairs++;
	  
	  if(counter_pairs > 1){
	    double v_reco = mt_avg*z_now + ct_avg;
	    double z_reco = (v_now-ct_avg)/mt_avg;

	    //HISTOs
	    if(counter_pairs == 2 && hclu_vz_3hit) hclu_vz_3hit->Fill(v_now-v_reco, z_now-z_reco); //compatibility cut control histogram for third hit
	    if(counter_pairs == 3 && hclu_vz_4hit) hclu_vz_4hit->Fill(v_now-v_reco, z_now-z_reco); //compatibility cut control histogram for third hit
	    if(counter_pairs == 4 && hclu_vz_5hit) hclu_vz_5hit->Fill(v_now-v_reco, z_now-z_reco); //compatibility cut control histogram for third hit
	    if(counter_pairs > 4 && hclu_vz_67hit) hclu_vz_67hit->Fill(v_now-v_reco, z_now-z_reco); //compatibility cut control histogram for third hit
	    if(hclu_vz)  hclu_vz->Fill(v_now-v_reco, z_now-z_reco); //compatibility cut control histogram
	    //END OF HISTOs

	    //cout<<endl;
	    //cout<<"V now: "<<v_now<<" V pre: "<<v_pre<<" Z now: "<<z_now<<" Z pre: "<<z_pre<<endl;
	    //cout<<"DeltaV: "<<v_now<<" - "<<v_reco<<" DeltaZ: "<<z_now<<" - "<<z_reco<<endl;
	    if((fabs(v_now-v_reco)<Dv_acc)&&(fabs(z_now-z_reco)<Dz_acc)) { //residues compatibility (hit read - hit reco)
	      //cout<<"passed"<<endl;
	      z_avg = (z_avg*(counter_pairs-1) + z_now)/counter_pairs;
	      v_avg = (v_avg*(counter_pairs-1) + v_now)/counter_pairs;
	      zv_avg = (zv_avg*(counter_pairs-1) + z_now*v_now)/counter_pairs;
	      z2_avg = (z2_avg*(counter_pairs-1) + z_now*z_now)/counter_pairs;
	      
	      mt_avg = (zv_avg - z_avg*v_avg)/(z2_avg-z_avg*z_avg);
	      ct_avg = v_avg - mt_avg*z_avg;
	      
	      double dv = v_avg-ct_avg;
	      double dz = z_avg-0;
	      double dz_ip = -(z_mesh_PADME-z_IP);
	      double dv_ip = -999;
	      
	      if(vhits_in[vhits_indexes[i]].view == Xview)      dv_ip = (x_IP-10) - ct_avg;
	      else if(vhits_in[vhits_indexes[i]].view == Yview) dv_ip = (y_IP+20) - ct_avg;
	      else cerr<<"[Clustering] VIEW NOT ASSIGNED!!! Hit index: "<<i<<endl;
	      
	      double ps_ip = (dv*dv_ip + dz*dz_ip)/(sqrt(dv*dv+dz*dz)*sqrt(dv_ip*dv_ip+dz_ip*dz_ip));
	      
	      if(fabs(ps_ip) > IPcostheta_CUT) { //IP compatibility cut on the 3+ hits
			
		vhits_in[vhits_indexes[i]].iclu = int(vtracks.size())-1;
		vtracks[int(vtracks.size())-1].nhit += 1;
		vtracks[int(vtracks.size())-1].m = mt_avg;
		vtracks[int(vtracks.size())-1].c = ct_avg;
	      } //close IP compatibility
	      else {
		cluster.nhit = 1;
		cluster.view = vhits_in[vhits_indexes[i]].view;
		cluster.quad = vhits_in[vhits_indexes[i]].quad;
		cluster.plane = vhits_in[vhits_indexes[i]].plane;
		cluster.ch = vhits_in[vhits_indexes[i]].ch;
		if(first_clustering) cluster.clustering = 0;
		else                 cluster.clustering = 1;
		
		z_avg = 0;
		v_avg = 0;
		zv_avg = 0;
		z2_avg = 0;
		mt_avg = 0;
		ct_avg = 0;
		counter_pairs = 0;
	      }
	    } //close residues compatibility (dv, dz)
	    else {    
	      cluster.nhit = 1;
	      cluster.view = vhits_in[vhits_indexes[i]].view;
	      cluster.quad = vhits_in[vhits_indexes[i]].quad;
	      cluster.plane = vhits_in[vhits_indexes[i]].plane;
	      cluster.ch = vhits_in[vhits_indexes[i]].ch;
	      if(first_clustering) cluster.clustering = 0;
	      else                 cluster.clustering = 1;
	      
	      z_avg = 0;
	      v_avg = 0;
	      zv_avg = 0;
	      z2_avg = 0;
	      mt_avg = 0;
	      ct_avg = 0;
	      counter_pairs = 0;
	    }
	  } //close more than 2 hits
	  else { //just two hits (first i=1 case only)
	    z_avg = 0.5*(z_now+z_pre);
	    v_avg = 0.5*(v_now+v_pre);
	    zv_avg = 0.5*(z_now*v_now+z_pre*v_pre);
	    z2_avg = 0.5*(z_now*z_now+z_pre*z_pre);
	    
	    mt_avg = (zv_avg - z_avg*v_avg)/(z2_avg-z_avg*z_avg);
	    ct_avg = v_avg - mt_avg*z_avg;

	    double dv = v_avg-ct_avg;
	    double dz = z_avg-0;
	    double dz_ip = -(z_mesh_PADME-z_IP);
	    double dv_ip = -999;
	    
	    if(vhits_in[vhits_indexes[i]].view == Xview)      dv_ip = (x_IP-10) - ct_avg;
	    else if(vhits_in[vhits_indexes[i]].view == Yview) dv_ip = (y_IP+20) - ct_avg;
	    else cerr<<"[Clustering] VIEW NOT ASSIGNED!!! Hit index: "<<i<<endl;

	    double ps_ip = (dv*dv_ip + dz*dz_ip)/(sqrt(dv*dv+dz*dz)*sqrt(dv_ip*dv_ip+dz_ip*dz_ip));
	    h_psIP_clu->Fill(ct_avg,ps_ip);

	    if(fabs(ps_ip) > IPcostheta_CUT) {
	      vtracks.push_back(cluster);
	      
	      vhits_in[vhits_indexes[i-1]].iclu = int(vtracks.size())-1;
	      vhits_in[vhits_indexes[i]].iclu = int(vtracks.size())-1;

	      vtracks[int(vtracks.size())-1].nhit += 1;
	      
	      vtracks[int(vtracks.size())-1].m = mt_avg;
	      vtracks[int(vtracks.size())-1].c = ct_avg;
	    } //closing compatibility with IP
	    else {
	      cluster.nhit = 1;
	      cluster.view = vhits_in[vhits_indexes[i]].view;
	      cluster.quad = vhits_in[vhits_indexes[i]].quad;
	      cluster.plane = vhits_in[vhits_indexes[i]].plane;
	      cluster.ch = vhits_in[vhits_indexes[i]].ch;
	      if(first_clustering) cluster.clustering = 0;
	      else                 cluster.clustering = 1;
	      
	      z_avg = 0;
	      v_avg = 0;
	      zv_avg = 0;
	      z2_avg = 0;
	      mt_avg = 0;
	      ct_avg = 0;
	      counter_pairs = 0;	
	    }
	    //cout<<"iclu: "<<int(vtracks.size())-1<<" (m,c): ("<<vtracks[int(vtracks.size())-1].m<<" , "<<vtracks[int(vtracks.size())-1].c<<endl;
	  }
	}//closing proximity compatibility (DeltaV)
	else {
	  cluster.nhit = 1;
	  cluster.view = vhits_in[vhits_indexes[i]].view;
	  cluster.quad = vhits_in[vhits_indexes[i]].quad;
	  cluster.plane = vhits_in[vhits_indexes[i]].plane;
	  cluster.ch = vhits_in[vhits_indexes[i]].ch;
	  if(first_clustering) cluster.clustering = 0;
	  else                 cluster.clustering = 1;
	  
	  z_avg = 0;
	  v_avg = 0;
	  zv_avg = 0;
	  z2_avg = 0;
	  mt_avg = 0;
	  ct_avg = 0;
	  counter_pairs = 0;	
	}
      } //close geometric compatibility (view, quad, plane) 
      else { //going to another quadrant, plane or view
	cluster.nhit = 1;
	cluster.view = vhits_in[vhits_indexes[i]].view;
	cluster.quad = vhits_in[vhits_indexes[i]].quad;
	cluster.plane = vhits_in[vhits_indexes[i]].plane;
	cluster.ch = vhits_in[vhits_indexes[i]].ch;
	if(first_clustering) cluster.clustering = 0;
	else                 cluster.clustering = 1;
	
	z_avg = 0;
	v_avg = 0;
	zv_avg = 0;
	z2_avg = 0;
	mt_avg = 0;
	ct_avg = 0;
	counter_pairs = 0;
      }
    }
  }

  vhits_indexes.clear();
}
  */
