#include "TFile.h"
#include "TVector3.h"
#include "TH2D.h"
#include "TCanvas.h"
#include <iostream>

void plotEvent(TFile* filo, TString canvaName, int iev){
  // quadrant: 0 x>0,y>0, etc.
  // | 0  1 |
  // |      |
  // | 3  2 |
  
  // boardSN plane Layer side  view otherview hole offset stripid_orig  quadrant   BDCompanion_other_view
  // 0       0     0     0     0    0         6    0      1-256         x<0  y<0   
  // 1       0     0     1     0    0         6    256    257-512       x<0  y>0   4
  // 2       0     1     0     0    1         6    0      0-255         x>0  y<0   7
  // 3       0     1     1     0    1         6    256    256-511       x>0  y>0   5
  
  // 4       0     2     0     1    1         1    0      0-255         x<0  y>0
  // 5       0     2     1     1    1         1    256    256-511       x>0  y>0
  // 6       0     3     0     1    0         1    0      0-255         x<0  y<0
  // 7       0     3     1     1    0         1    256    256-511       x>0  y<0
  
  // 8       1     4     0     0    0         1    0      0-255         
  // 9       1     4     1     0    0         1    256    256-511       
  // 10      1     5     0     0    1         1    0      0-255
  // 11      1     5     1     0    1         1    256    256-511       
  // 12      1     6     0     1    1         6    0      0-255
  // 13      1     6     1     1    1         6    256    256-511
  // 14      1     7     0     1    0         6    0      0-255         
  // 15      1     7     1     1    0         6    256    256-511       
  
  TH2D* clupos = (TH2D*) filo->Get(Form("MMStudy/ECalSelClusters_yvsx_ev%d",iev));
  int binmax = clupos->GetMaximumBin();
  int binx[2], biny[2], binz[2];
  double maxe[2];
  clupos->GetBinXYZ(binmax,binx[0],biny[0],binz[0]);
  maxe[0] = clupos->GetMaximum();
  clupos->SetBinContent(binmax,0);
  binmax = clupos->GetMaximumBin();
  clupos->GetBinXYZ(binmax,binx[1],biny[1],binz[1]);
  maxe[1] = clupos->GetMaximum();

  
  
  int boardid[2][2]; // cluster id, view
  TVector3 clupositions[2];
  for (int i=0; i<2; i++){
  //  clupos->GetBinXYZ(binmax,binx+1,biny+1,binz);
    
    if (clupos->GetXaxis()->GetBinCenter(binx[i]) > 0 && clupos->GetYaxis()->GetBinCenter(biny[i]) > 0) {
      boardid[i][0] = 3;
      boardid[i][1] = 5;
    }
    else if (clupos->GetXaxis()->GetBinCenter(binx[i]) > 0 && clupos->GetYaxis()->GetBinCenter(biny[i]) < 0) {
      boardid[i][0] = 2;
      boardid[i][1] = 7;
    }
    else if (clupos->GetXaxis()->GetBinCenter(binx[i]) < 0 && clupos->GetYaxis()->GetBinCenter(biny[i]) > 0) {
      boardid[i][0] = 1;
      boardid[i][1] = 4;
    }
    else {//(clupos->GetXaxis()->GetBinCenter(binx[i]) < 0 && clupos->GetYaxis()->GetBinCenter(biny[i]) < 0) {
      boardid[i][0] = 0;
      boardid[i][1] = 6;
    }
    clupositions[i].SetXYZ(clupos->GetXaxis()->GetBinCenter(binx[i]),clupos->GetYaxis()->GetBinCenter(biny[i]),2660);
    std::cout << "Cluster " << i << " energy = " << maxe[i] << " pos = " << clupos->GetXaxis()->GetBinCenter(binx[i]) << " , " << clupos->GetYaxis()->GetBinCenter(biny[i]) << " boards = " << boardid[i][0] << " " << boardid[i][1] << std::endl;
  }

  Double_t w = 1000;
  Double_t h = 800;
  TString canvanome = Form("%s_iev%d",canvaName.Data(),iev);
  TCanvas* cc = new TCanvas(canvanome.Data(), canvanome.Data(), w, h);
  cc->SetWindowSize(w + (w - cc->GetWw()), h + (h - cc->GetWh()));
  cc->Divide(2,2);
  TH2D* zvsv[4][2]; // clusterxview, plane
  TH2D* zvsvsel[4][2]; // clusterxview, plane, selected
  int colors[2] = {kBlue-4,kGreen+2};// plane colors
  int colorssel[2] = {kOrange-3,kMagenta+2};// plane colors
  const double vrange = 60;
  for (int i=0; i<2; i++){// cluster
    for (int j=0; j<2; j++){ // view
      int idx = i+2*j;
      cc->cd(1+idx);
      TH2D* clutracks = (TH2D*) filo->Get(Form("MMStudy/zvsv_board%d_ev%d_clus",boardid[i][j],iev));
      clutracks->SetLineColor(kPink);
      clutracks->Draw("box");
      clutracks->GetXaxis()->SetRangeUser(clupositions[i][1-j]-vrange,clupositions[i][1-j]+vrange);
      
      for (int ipl = 0; ipl<2; ipl++){
	zvsv[idx][ipl] = (TH2D*) filo->Get(Form("MMStudy/zvsv_board%d_ev%d",boardid[i][j]+8*ipl,iev));
	zvsv[idx][ipl]->SetLineColor(colors[ipl]);
	//	zvsv[idx][ipl]->Draw("boxsame");
      }
      for (int ipl = 0; ipl<2; ipl++){
	zvsvsel[idx][ipl] = (TH2D*) filo->Get(Form("MMStudy/zvsv_selecthit_board%d_ev%d",boardid[i][j]+8*ipl,iev));
	zvsvsel[idx][ipl]->SetLineColor(colorssel[ipl]);
	zvsvsel[idx][ipl]->Draw("boxsame");
      }
      for (int ipl = 0; ipl<2; ipl++){
	zvsvsel[idx][ipl] = (TH2D*) filo->Get(Form("MMStudy/zvsv_hitforclus_board%d_ev%d",boardid[i][j]+8*ipl,iev));
	zvsvsel[idx][ipl]->SetLineColor(colorssel[ipl]);
	zvsvsel[idx][ipl]->SetLineWidth(3.);
	zvsvsel[idx][ipl]->Draw("boxsame");
      }

    }
  }


  
}

  
