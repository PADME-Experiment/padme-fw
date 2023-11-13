// testing the reliability of CalcHEP cuts. Analitical integration vs numerical
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h> 
#include <TString.h> 
#include <TObject.h>
#include <TCanvas.h> 
#include <TGraph.h> 
#include <TMultiGraph.h> 
#include <TAxis.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TVector3.h>
#include <regex>

using namespace std;

void TestCalcHEP(double TargetX, double TargetY, double AngBeamDirX, double AngBeamDirY, const char* OutputName){

double pi2 = TMath::Pi()*2;
static const int NRows = 200000;
double MassEl = 0.000511; //in GeV
double EBeam = 0.280; //in GeV
double CaloNBins = 30; 
double OccBins = 450;
double EnBins = EBeam*10*1000;
TVector3 TargetPosition (0.,0.,-1028.); //from MC full
TVector3 ECalPosition (0.,0.,2672.); //from MC full
double TargetCalo = abs(TargetPosition.Z() - ECalPosition.Z());
double ShadowCut = 102/abs(TargetPosition.Z()-500); // in Rad

//CUTS 
double RECal = 300.; //external radius of ECal
double HalfHole = 50.; 
double RMax = 270;
double RMin = 90;
double EMax = 0.180;
double EMin = 0.060;
double MagnetCut = ShadowCut*TargetCalo;
cout<<"Magnet Shadow on ECal = "<<MagnetCut<<" [mm]"<<endl;

//CUTS TOMMASO
TVector3 TargetImpact;
TVector3 BeamDirDisplacemente;
double SqrtS, BeamMomentum;
double BG, Beta, Gamma;
TVector3 BoostMom;
double Radius_min, Energy_max, Energy_min;

TargetImpact.SetXYZ(TargetX, TargetY, TargetPosition.Z()); //impact point of the beam on target 
BeamDirDisplacemente.SetXYZ(TargetX + TargetCalo*tan(AngBeamDirX), TargetY + TargetCalo*tan(AngBeamDirY), ECalPosition.Z() - TargetPosition.Z());
SqrtS = TMath::Sqrt(2.*MassEl*EBeam); // in GeV
BeamMomentum = TMath::Sqrt(EBeam*EBeam - 2*MassEl*MassEl);
BG = BeamMomentum/SqrtS; // beta gamma
Gamma = TMath::Sqrt(BG*BG + 1.);
Beta = BG/Gamma;
BoostMom.SetXYZ(BeamDirDisplacemente.X()-TargetImpact.X(), BeamDirDisplacemente.Y()-TargetImpact.Y(), BeamDirDisplacemente.Z()-TargetImpact.Z());
BoostMom *= (Beta/BoostMom.Mag());

double tanQMax = RMax/(BeamDirDisplacemente.Z()-TargetImpact.Z());
double tLim = TMath::ATan(Gamma*tanQMax);
if (tLim < TMath::Pi()*0.25) {
    cout << "No solution? " << tLim << " " << Gamma << endl;
    }

Radius_min = (BeamDirDisplacemente.Z()-TargetImpact.Z())*TMath::Tan(0.5*TMath::Pi()-tLim)/Gamma;// (fBeamDirDisplacemente.Z()-fRTarg.Z())*tanQMin; 
Energy_max = SqrtS*Gamma*0.5*(1.-TMath::Cos(2*tLim)); 
Energy_min = SqrtS*Gamma*0.5*(1.+TMath::Cos(2*tLim)); 

cout << "GeneralInfo: run-level info for run from DYNAMIC CUTS at " << EBeam <<" MeV" << " Pbeam = " << BeamMomentum<<endl;
cout << " target = { "<< TargetImpact.X()<< " , "<< TargetImpact.Y() << " , " << TargetImpact.Z() << " }; COG = { " << BeamDirDisplacemente.X() << " , " << BeamDirDisplacemente.Y() << " , "<< BeamDirDisplacemente.Z() << " }" <<endl; 
cout << " sqrt(s) = " << SqrtS << " bg = " << BG << " beta = " << Beta << endl;
cout << " energyRange = { " << Energy_min << " , " << Energy_max << " }; radiusRange = { " << Radius_min << " , " << RMax << " }" << endl;

const int numPoints = 100;
double* xmin = new double[numPoints];
double* ymin = new double[numPoints];
double* xmax = new double[numPoints];
double* ymax = new double[numPoints];
double* xECal = new double[numPoints];
double* yECal = new double[numPoints];
for (int i = 0; i < numPoints; ++i) {
    double angle = 2 * TMath::Pi() * i / numPoints;
    xmin[i] = Radius_min * TMath::Cos(angle) + TargetImpact.X() + BeamDirDisplacemente.X();
    ymin[i] = Radius_min * TMath::Sin(angle) + TargetImpact.Y() + BeamDirDisplacemente.Y();
    xmax[i] = RMax * TMath::Cos(angle) + TargetImpact.X() + BeamDirDisplacemente.X();
    ymax[i] = RMax * TMath::Sin(angle) + TargetImpact.Y() + BeamDirDisplacemente.Y();
    xECal[i] = RECal * TMath::Cos(angle);
    yECal[i] = RECal * TMath::Sin(angle);
}
TGraph* graphmin = new TGraph(numPoints, xmin, ymin);
graphmin->SetTitle(Form("Circle with mRadius_min %.2f", Radius_min));
graphmin->SetLineStyle(1);
graphmin->SetLineColor(kBlack);
TGraph* graphmax = new TGraph(numPoints, xmax, ymax);
graphmax->SetTitle(Form("Circle with Radius_max %.2f", RMax));
graphmax->SetLineStyle(1);
graphmax->SetLineColor(kBlack);
TGraph* graphECal = new TGraph(numPoints, xECal, yECal);
graphECal->SetTitle(Form("Circle with Radius_ECal %.2f", RECal));
graphECal->SetLineStyle(1);
graphECal->SetLineColor(kBlack);
TMultiGraph *MGDynCut = new TMultiGraph(Form("MGDynCut_%.2f",EBeam),Form("MGDynCut_%.2f",EBeam));
MGDynCut->Add(graphmin);
MGDynCut->Add(graphmax);
MGDynCut->Add(graphECal);

//target and beam multiplicity
double PoT = 1e10;
double Na  = 6.022e23;
double Z_C = 6;
double D_C = 3.5;
double A_C = 12;
double TThickness = 0.0097; //in cm
double CmtoPb = 1e-36;
double Luminosity_C = CmtoPb * Na * Z_C * D_C * TThickness / A_C;

// input file names 
static const int NFileIn = 8;
TString nameFile[NFileIn] = {"SCh280","SCh280_En","SCh280_An","SCh280_EnAn","Full280","Full280_En","Full280_An","Full280_EnAn"};

double Nev_NoCuts[NFileIn] = {0}; //must be deleted for a scan analysis
double Nev_Geom[NFileIn] = {0}; //must be deleted for a scan analysis
double Nev_AllCuts[NFileIn] = {0}; //must be deleted for a scan analysis

//creating output file 
TFile *FileOut = new TFile(OutputName, "RECREATE");
double counter[NFileIn] = {0}; //must be deleted for a scan analysis
for(int iF = 0; iF<NFileIn; iF++){
    counter[iF] = iF; //must be deleted for a scan analysis
    TFile *InputFile = TFile::Open("/home/marco/analysis/varie/" + nameFile[iF] + ".root", "read");
    TTree *TreeIn  = (TTree*)InputFile->Get("t280"); 

    ifstream txtIn("/home/marco/analysis/varie/" + nameFile[iF] + ".txt"); 
    string line;
    string searchString = "#Cross_";
    regex numberRegex("\\d+\\.?\\d*(?:[eE][-+]?\\d+)?");
    double CrossSection;
    double InstantLumi;

    while (getline(txtIn, line)) { // Reading every line of the input file.txt
        if (line.find(searchString) != string::npos) { // if it exists
            smatch match;
            if (regex_search(line, match, numberRegex)) {
                string numberString = match.str();
                CrossSection = stod(numberString);
                InstantLumi = stod(numberString) * Luminosity_C;
                break; 
            }
        }
    }
    txtIn.close(); 
    cout<<"Sigma (from input file)= "<<CrossSection<<" pb "<<endl;

    double P3pos_ini, P3ele_ini, FinalP1_ele, FinalP2_ele, FinalP3_ele, FinalP1_pos, FinalP2_pos, FinalP3_pos; 
    TreeIn->SetBranchAddress("InitialP3_positron", &P3pos_ini); // Initial state positron
    TreeIn->SetBranchAddress("InitialP3_electron", &P3ele_ini); // Initial state Electron (at rest)
    TreeIn->SetBranchAddress("FinalP1_electron", &FinalP1_ele); // Final state electron 
    TreeIn->SetBranchAddress("FinalP2_electron", &FinalP2_ele);
    TreeIn->SetBranchAddress("FinalP3_electron", &FinalP3_ele);
    TreeIn->SetBranchAddress("FinalP1_positron", &FinalP1_pos); // Final state positron
    TreeIn->SetBranchAddress("FinalP2_positron", &FinalP2_pos);
    TreeIn->SetBranchAddress("FinalP3_positron", &FinalP3_pos);

    //inizializing histos
    TDirectory *Dir = FileOut->mkdir(nameFile[iF]);
    TH2F* hOcc_pos = new TH2F(("hOcc_pos_" + nameFile[iF]), ("hOcc_pos_" + nameFile[iF]), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hOcc_ele = new TH2F(("hOcc_ele_" + nameFile[iF]), ("hOcc_ele_" + nameFile[iF]), OccBins, -450, 450, OccBins, -450, 450);
    TH2F* hOccGeom_pos = new TH2F(("hOccGeom_pos_" + nameFile[iF]), ("hOccGeom_pos_" + nameFile[iF]), CaloNBins, -315, 315, CaloNBins, -315, 315);
    TH2F* hOccGeom_ele = new TH2F(("hOccGeom_ele_" + nameFile[iF]), ("hOccGeom_ele_" + nameFile[iF]), CaloNBins, -315, 315, CaloNBins, -315, 315);
    TH2F* hOccAllCuts_pos = new TH2F(("hOccAllCuts_pos_" + nameFile[iF]), ("hOccAllCuts_pos_" + nameFile[iF]), CaloNBins, -315, 315, CaloNBins, -315, 315);
    TH2F* hOccAllCuts_ele = new TH2F(("hOccAllCuts_ele_" + nameFile[iF]), ("hOccAllCuts_ele_" + nameFile[iF]), CaloNBins, -315, 315, CaloNBins, -315, 315);

    TH1F* hEnDist_pos = new TH1F(("hEnDist_pos" + nameFile[iF]), ("hEnDist_pos" + nameFile[iF]), EnBins, -EBeam, EBeam);
    TH1F* hEnDist_ele = new TH1F(("hEnDist_ele" + nameFile[iF]), ("hEnDist_ele" + nameFile[iF]), EnBins, -EBeam, EBeam);
    TH1F* hEnDistGeom_pos = new TH1F(("hEnDistGeom_pos" + nameFile[iF]), ("hEnDistGeom_pos" + nameFile[iF]), EnBins, -EBeam, EBeam);
    TH1F* hEnDistGeom_ele = new TH1F(("hEnDistGeom_ele" + nameFile[iF]), ("hEnDistGeom_ele" + nameFile[iF]), EnBins, -EBeam, EBeam);
    TH1F* hEnDistAllCuts_pos = new TH1F(("hEnDistAllCuts_pos" + nameFile[iF]), ("hEnDistAllCuts_pos" + nameFile[iF]), EnBins, -EBeam, EBeam);
    TH1F* hEnDistAllCuts_ele = new TH1F(("hEnDistAllCuts_ele" + nameFile[iF]), ("hEnDistAllCuts_ele" + nameFile[iF]), EnBins, -EBeam, EBeam);

    TVector3 P1;
    TVector3 P2;
    TVector3 P3;
    TVector3 P4;

    for(int iR = 0; iR<NRows; iR++){
        TreeIn->GetEntry(iR);

        //defining 2->2 body process from Tree in input P1-P3 positrons, P2-P4 electrons
        P1.SetXYZ(0,0,P3pos_ini);
        P2.SetXYZ(0,0,P3ele_ini);
        P3.SetXYZ(FinalP1_pos,FinalP2_pos,FinalP3_pos);
        P4.SetXYZ(FinalP1_ele,FinalP2_ele,FinalP3_ele);
        // cout<<P1.Z()<<" "<<P2.Z()<<" "<<P3.X()<<" "<<P3.Y()<<" "<<P3.Y()<<" "<<P4.X()<<" "<<P4.Y()<<" "<<P4.Y()<<" "<<endl;

        // initial state
        double P1_Ptot = P1.Mag();
        double P2_Ptot = P2.Mag();
        double P1_En = TMath::Sqrt(MassEl*MassEl + P1_Ptot*P1_Ptot);
        double P2_En = MassEl;
        double E12_tot = P1_En + P2_En;

        // final state
        // total momentum
        double P3_Ptot = P3.Mag();
        double P4_Ptot = P4.Mag();
        // transverse momentum
        double P3_transP = P3.Perp();
        double P4_transP = P4.Perp();
        // energy 
        double P3_En = sqrt(MassEl*MassEl + P3_Ptot*P3_Ptot);
        double P4_En = sqrt(MassEl*MassEl + P4_Ptot*P4_Ptot);
        double E34_tot = P3_En + P4_En;
        if(E12_tot-E34_tot>1E-6){
            cout<<"No energy conservation!"<<" E12-E34 = "<<(E12_tot-E34_tot)<<endl;
            break;
        }
        // polar angle
        double P3_PolarTheta = P3.Theta();
        double P4_PolarTheta = P4.Theta();
        // cos(polar angle)
        double P3_CosPolar = P3.CosTheta();
        double P4_CosPolar = P4.CosTheta();
        // azimuthal angle
        double P3_Phi = P3.Phi();
        double P4_Phi = P4.Phi();
        // impact point @ECal -- 1term: impact point due to calchep P_i - 2term: target beam position - 3term: beam angle displacement
        double P3_x = (P3.X()/P3.Z())*TargetCalo + TargetImpact.X() + BeamDirDisplacemente.X();
        double P3_y = (P3.Y()/P3.Z())*TargetCalo + TargetImpact.Y() + BeamDirDisplacemente.Y();
        double P4_x = (P4.X()/P4.Z())*TargetCalo + TargetImpact.X() + BeamDirDisplacemente.X();
        double P4_y = (P4.Y()/P4.Z())*TargetCalo + TargetImpact.Y() + BeamDirDisplacemente.Y();
        
        double P3_R = tan(P3_PolarTheta)*TargetCalo;
        // cout<<P3_R<<" = "<<P3_R1<<" ?"<<endl;
        double P4_R = tan(P4_PolarTheta)*TargetCalo;
        // cout<<P4_R<<" = "<<P4_R1<<" ?"<<endl;

        //filling Histo
        hOcc_pos->Fill(P3_x, P3_y);
        hOcc_ele->Fill(P4_x, P4_y);
        hEnDist_pos->Fill(P3_En);
        hEnDist_ele->Fill(P4_En);
        //performing geometrical cuts
        if(P3_R<RMax && P4_R<RMax && P3_R>RMin && P4_R>RMin && abs(P3_y)<MagnetCut && abs(P4_y)<MagnetCut){
            hOccGeom_pos->Fill(P3_x, P3_y);
            hOccGeom_ele->Fill(P4_x, P4_y);
            hEnDistGeom_pos->Fill(P3_En);
            hEnDistGeom_ele->Fill(P4_En);   
            //performing energetical cuts
            if(P3_En<EMax && P4_En<EMax && P3_En>EMin && P4_En>EMin){
                hOccAllCuts_pos->Fill(P3_x, P3_y);
                hOccAllCuts_ele->Fill(P4_x, P4_y);
                hEnDistAllCuts_pos->Fill(P3_En);
                hEnDistAllCuts_ele->Fill(P4_En);
            }
        }   
    }

    //if(hOcc_pos->GetEntries() == hOcc_ele->GetEntries()) {
        Nev_NoCuts[iF] = CrossSection * (hOcc_pos->GetEntries() / NRows);
    //} 
    if(hOccGeom_pos->GetEntries() == hOccGeom_ele->GetEntries()){
        Nev_Geom[iF] = CrossSection * (hOccGeom_pos->GetEntries() / NRows);
    }
    if(hOccAllCuts_pos->GetEntries() == hOccAllCuts_ele->GetEntries()){
        Nev_AllCuts[iF] = CrossSection * (hOccAllCuts_pos->GetEntries() / NRows);
    } 
    else cout<<"CUTS ARE NOT WORKING!!"<<endl;

    FileOut->cd();
    Dir->cd();
    hOcc_pos->Write();
    hOcc_ele->Write();
    hEnDist_pos->Write();
    hEnDist_ele->Write();
    hOccGeom_pos->Write();
    hOccGeom_ele->Write();
    hEnDistGeom_pos->Write();
    hEnDistGeom_ele->Write();
    hOccAllCuts_pos->Write();
    hOccAllCuts_ele->Write();
    hEnDistAllCuts_pos->Write();
    hEnDistAllCuts_ele->Write();
}

//creating TGraph to see trend as a function of cuts
TGraph *gNoCuts = new TGraph(NFileIn, counter, Nev_NoCuts);
gNoCuts->SetName("gNoCuts");
gNoCuts->SetTitle("gNoCuts; simulation; N_{Ev}");
gNoCuts->SetMarkerStyle(20);
TAxis *aX_NoCuts = gNoCuts->GetXaxis();
int numBins = aX_NoCuts->GetNbins();
aX_NoCuts->Set(NFileIn, 0, NFileIn);
// Set custom labels for each bin on the X-axis
for (int i = 1; i <= NFileIn; ++i) {
    aX_NoCuts->SetBinLabel(i, nameFile[i-1]);
}
TGraph *gGeom = new TGraph(NFileIn, counter, Nev_Geom);
gGeom->SetTitle("gGeom; simulation; N_{Ev}");
gGeom->SetMarkerStyle(20);
TAxis *aX_Geom = gGeom->GetXaxis();
numBins = aX_Geom->GetNbins();
aX_Geom->Set(NFileIn, 0, NFileIn);
// Set custom labels for each bin on the X-axis
for (int i = 1; i <= NFileIn; ++i) {
    aX_Geom->SetBinLabel(i, nameFile[i-1]);
}
TGraph *gAllCuts = new TGraph(NFileIn, counter, Nev_AllCuts);
gAllCuts->SetTitle("gAllCuts; simulation; N_{Ev}");
gAllCuts->SetMarkerStyle(20);
TAxis *aX_AllCuts = gAllCuts->GetXaxis();
numBins = aX_AllCuts->GetNbins();
aX_AllCuts->Set(NFileIn, 0, NFileIn);
// Set custom labels for each bin on the X-axis
for (int i = 1; i <= NFileIn; ++i) {
    aX_AllCuts->SetBinLabel(i, nameFile[i-1]);
}

FileOut->cd();
gNoCuts->Write("AP");
gGeom->Write("AP");
gAllCuts->Write("AP");
MGDynCut->Write("AP");

FileOut->Close();
}
