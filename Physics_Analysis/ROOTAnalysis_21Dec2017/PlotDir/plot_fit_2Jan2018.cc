#include "TH1.h"
#include "TFile.h"
#include "TString.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* atof */
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TF1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TGraphErrors.h"
#include "TFrame.h"
#include <TPaveStats.h>
#include "TAttText.h"
#include "TMultiGraph.h"
#include "TChain.h"
#include "TTree.h"
#include "TLatex.h"
#include <TCut.h>
#include <TLegend.h>
using namespace std;

double myDelThetaHadFunc(double *x, double *par){
  double fitval=1.0E-10;
  if(x[0]>TMath::Pi() || x[0]<0.0) return fitval;
  const double arg=x[0];
  const double arg_L=arg;
  const double mean=par[1];
  const double sigmaG=par[2];
  const double mpv=par[3];
  const double sigmaL=par[4];
  const double norm=sqrt(2.0*TMath::Pi());
  const double g1=TMath::Gaus(arg,mean,sigmaG)/norm;
  const double g2=TMath::Landau(arg_L,mpv,sigmaL)/norm;
  fitval=par[0]*g1/sigmaG+par[5]*g2/sigmaL;
  //
  if(fitval<0.0) return 0.0;
  return fitval;
}

//double dTheta3Dparam(const int & parInd, const int & tau_type, const double & P_tau, const double *par) {
double dTheta3Dparam(double *p, double *par) {
 //tau_type 0: l, 1:1-prong, 3:3-prong
  const double P_tau=p[0];
  if (P_tau<0.0) {
    return 0.0;
  } else {
    return par[0]*(exp(-par[1]*sqrt(P_tau))+par[2]/P_tau)+par[3]+par[4]*P_tau;
  }
}

double dTheta3Dparam0(double *p, double *par) {
 //tau_type 0: l, 1:1-prong, 3:3-prong
  const double P_tau=p[0];
  if (P_tau<0.0) {
    return 0.0;
  } else {
    return (par[0]+par[1]*P_tau+par[2]*pow(P_tau,2)+par[3]*pow(P_tau,3)+par[4]*pow(P_tau,4))*0.00125;
  }
}

int main(){


  //TFile *f1 = TFile::Open("AllRoot/all.root");

  //  TFile *f1 = TFile::Open("../AllRoot/ggH125_tautau_new.root");
  //  TTree *mytree1 = (TTree*)f1->Get("tree");

  TChain mychain1("tree");
  mychain1.Add("../AllRoot/ggH125_taue_new.root");
  mychain1.Add("../AllRoot/ggH125_taumu_new.root");
  mychain1.Add("../AllRoot/VBFH125_taue_new.root");
  mychain1.Add("../AllRoot/VBFH125_taumu_new.root");  
  TTree *mytree1 = &mychain1;
  //
  TChain mychain2("tree");
  mychain2.Add("../AllRoot/ggH125_tautau_new.root");
  TTree *mytree2 = &mychain2;
  //
  const int npbin=10;
  
  int pmin_0[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_0[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_0;
  for (int i =0; i<npbin; ++i) {
    cuts_0.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_0[i],pmax_0[i]));
  }

  int pmin_1[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_1[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_1;
  for (int i =0; i<npbin; ++i) {
    cuts_1.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_1[i],pmax_1[i]));
  }

  int pmin_2[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_2[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_2;
  for (int i =0; i<npbin; ++i) {
    cuts_2.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_2[i],pmax_2[i]));
  }

  int pmin_3[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_3[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_3;
  for (int i =0; i<npbin; ++i) {
    cuts_3.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_3[i],pmax_3[i]));
  }

  
  TString cut1a = "n_truth_tau==1 && IsHadronicTau == 1";   TString cut1b = "n_truth_tau==2 && IsHadronicTau == 1";
  TString cut2 = "angle_vis_neu < .35";
  TString cut3 = "numCharged == 1";
  TString cut4 = "numNeutral == 0";
  TString cut5 = "numNeutral > 0";
  TString cut6 = "numCharged == 3";

  int fit_status[4][npbin];
  double fit_param[4][6][npbin];
  double fit_error[4][6][npbin];

  gStyle->SetOptStat("e");
  gStyle->SetOptFit(01);

  for (int icut = 0; icut < 4 ; ++icut) {
    for (int ipbin = 0; ipbin <npbin; ++ipbin) {
      fit_status[icut][ipbin] = -1;
      //
      for (int i3 = 0; i3 < 6; ++i3) {
	fit_param[icut][i3][ipbin] = 0;
	fit_error[icut][i3][ipbin] = 0;
      }
      //
      if (icut == 0){
	if (ipbin == 0){
	  fit_param[icut][0][ipbin] = 3 ;
	  fit_param[icut][1][ipbin] = 0.3;
	  fit_param[icut][2][ipbin] = 0.3;
	  fit_param[icut][3][ipbin] = 0.04;
	  fit_param[icut][4][ipbin] = 0.04;
	  fit_param[icut][5][ipbin] = 15;
	} else if (ipbin == 2){
	  fit_param[icut][0][ipbin] = 2 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.01;
	  fit_param[icut][5][ipbin] = 10;
	} else if (ipbin == 5){
	  fit_param[icut][0][ipbin] = 2 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.01;
	  fit_param[icut][5][ipbin] = 8;
	} else if (ipbin == 7){
	  fit_param[icut][0][ipbin] = 2 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.01;
	  fit_param[icut][5][ipbin] = 8;
	}else if (ipbin == 8 || ipbin == 9){
	  fit_param[icut][0][ipbin] = 2 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 15;
	} else{
	  fit_param[icut][0][ipbin] = 1 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.01;
	  fit_param[icut][5][ipbin] = 10;
	}
    } else if (icut == 1){
	if (ipbin == 8 || ipbin == 9){
	  fit_param[icut][0][ipbin] = 2 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 15;
	} else{
	  fit_param[icut][0][ipbin] = 3 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 15;
	}
     } else if (icut == 2){
	if (ipbin == 1){
	  fit_param[icut][0][ipbin] = 0.5 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.9;
	} else if (ipbin == 2){
	  fit_param[icut][0][ipbin] = 0.1 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.01;
	  fit_param[icut][5][ipbin] = 0.1;
	} else if (ipbin == 3 ){
	  fit_param[icut][0][ipbin] = 0.1 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.01;
	  fit_param[icut][5][ipbin] = 0.1;
	} else if (ipbin == 4 ){
	  fit_param[icut][0][ipbin] = 0.5 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.8;
	} else if (ipbin == 5 ){
	  fit_param[icut][0][ipbin] = 0.1 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.1;
	} else if (ipbin == 6 ){
	  fit_param[icut][0][ipbin] = 0.65 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.9;
	} else if (ipbin == 7){
	  fit_param[icut][0][ipbin] = 0.6 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.9;
	} else if (ipbin == 8){
	  fit_param[icut][0][ipbin] = 0.6 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.85;
	} else if (ipbin == 9){
	  fit_param[icut][0][ipbin] = 0.65 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.9;
	}else{
	  fit_param[icut][0][ipbin] = 0.5 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.9;
	}
      } else if (icut == 3){
	if (ipbin == 1 ){
	  fit_param[icut][0][ipbin] = 0.5 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.8;
	}else if (ipbin == 2){
	  fit_param[icut][0][ipbin] = 0.8 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.8;
	}else if (ipbin == 3){
	  fit_param[icut][0][ipbin] = 0.1 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.95;
	}else if (ipbin == 4){
	  fit_param[icut][0][ipbin] = 0.2 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.04;
	  fit_param[icut][4][ipbin] = 0.06;
	  fit_param[icut][5][ipbin] = 0.95;
	} else if (ipbin == 5){
	  fit_param[icut][0][ipbin] = 0.65 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.95;
	} else if (ipbin == 6){
	  fit_param[icut][0][ipbin] = 0.7 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.9;
	} else if (ipbin == 7){
	  fit_param[icut][0][ipbin] = 0.7 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.9;
	}else if (ipbin == 8){
	  fit_param[icut][0][ipbin] = 0.55 ;
	  fit_param[icut][1][ipbin] = 0.22;
	  fit_param[icut][2][ipbin] = 0.23;
	  fit_param[icut][3][ipbin] = 0.024;
	  fit_param[icut][4][ipbin] = 0.024;
	  fit_param[icut][5][ipbin] = 0.85;
	} else {
	  fit_param[icut][0][ipbin] = 0.55 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.9;
	}
      }
      double thetamin = 0;
      double thetamax = 0.35;
      if (ipbin >= 1) thetamax = 0.3;
      if (ipbin >= 2) thetamax = 0.25;
      if (ipbin >= 3) thetamax = 0.2;
      if (ipbin >= 4) thetamax = 0.15;
      if (ipbin >= 5) thetamax = 0.125;
      if (ipbin >= 6) thetamax = 0.1;
      if (ipbin >= 7) thetamax = 0.075;
      if (ipbin >= 8) thetamax = 0.05;
      if (ipbin >= 9) thetamax = 0.025;
      //
      TH1F *hist1 = new TH1F("hist1", "hist1" ,50, thetamin, thetamax);
      //
      if(icut == 0) mytree1->Project("hist1","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1a.Data(), cut2.Data(), cut3.Data(), cut4.Data(), cuts_0[ipbin].Data()));
      if(icut == 1) mytree1->Project("hist1","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1a.Data(), cut2.Data(), cut3.Data(), cut5.Data(), cuts_1[ipbin].Data()));
      if(icut == 2) mytree1->Project("hist1","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1a.Data(), cut2.Data(), cut3.Data(),              cuts_2[ipbin].Data()));
      if(icut == 3) mytree1->Project("hist1","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1a.Data(), cut2.Data(), cut6.Data(),              cuts_3[ipbin].Data()));
      //
      TH1F *hist2 = new TH1F("hist2", "hist2" ,50, thetamin, thetamax);
      //
      if(icut == 0) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1b.Data(), cut2.Data(), cut3.Data(), cut4.Data(), cuts_0[ipbin].Data()));
      if(icut == 1) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1b.Data(), cut2.Data(), cut3.Data(), cut5.Data(), cuts_1[ipbin].Data()));
      if(icut == 2) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1b.Data(), cut2.Data(), cut3.Data(),              cuts_2[ipbin].Data()));
      if(icut == 3) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1b.Data(), cut2.Data(), cut6.Data(),              cuts_3[ipbin].Data()));
      //
      TH1F *hist=(TH1F*)hist1->Clone("hist");
      hist->Add(hist2,1);
      //
      TF1 *func = new TF1("fit",myDelThetaHadFunc,thetamin,thetamax,6);
      func->SetParameters(  fit_param[icut][0][ipbin],
			    fit_param[icut][1][ipbin],
			    fit_param[icut][2][ipbin],
			    fit_param[icut][3][ipbin],
			    fit_param[icut][4][ipbin],
			    fit_param[icut][5][ipbin]);
      func->SetParLimits(0,0,1); // norm > 0
      func->SetParLimits(5,0,1); // norm > 0
      func->SetParLimits(1,0,0.25);    // gaussian mean
      func->SetParLimits(2,0,0.25);    // gaussian width
      func->SetParLimits(3,0,0.25);    // 1andau mean
      func->SetParLimits(4,0,0.25);    // landau mean
      //
      //    
      //
      TCanvas* c0=new TCanvas("c0","c0",400,400);
      if (icut==0) {
	hist->SetTitle(Form("Angle(n_cha = 1 & n_neu = 0): %d < P(Gev) < %d", pmin_0[ipbin] , pmax_0[ipbin]));
      } else if (icut==1) {
	hist->SetTitle(Form("Angle(n_cha = 1 & n_neu > 0): %d < P(Gev) < %d", pmin_1[ipbin] , pmax_1[ipbin]));
      } else if (icut==2) {
	hist->SetTitle(Form("Angle(n_cha = 1): %d < P(Gev) < %d", pmin_2[ipbin] , pmax_2[ipbin]));
      } else if (icut==3) {
	hist->SetTitle(Form("Angle(n_cha = 3): %d < P(Gev) < %d", pmin_3[ipbin] , pmax_3[ipbin]));
      }
      hist->Scale(1./hist->Integral()); hist->SetLineColor(1);
      //      hist->Draw("HIST");
      //
      //hist2->Scale(1./hist2->Integral()); hist2->SetLineColor(2);
      //      hist2->Draw("HIST same");   
      //
      fit_status[icut][ipbin] = hist->Fit("fit","LLMR","",thetamin,thetamax);
      for (int i=0;i<6; ++i ) {
	fit_param[icut][i][ipbin] = func->GetParameter(i);
	fit_error[icut][i][ipbin] = func->GetParError(i);
      }
      c0->Modified();
      c0->Update();
      c0->SaveAs(Form("/afs/cern.ch/user/a/atpathak/afswork/public/Pixel/LFV_Plots/Plots_Fit_tautau/Angle_Vis_neutrino_decay%d_Fit_%d.pdf",icut,ipbin+1));
      delete c0;
      c0=0;
      delete func;
      func=0;
      delete hist;
      hist=0;
      delete hist1;
      hist1=0;
      delete hist2;
      hist2=0;
    }
  }
  //
  //  gStyle->SetOptStat(0020);
  //
  int fitp_status[4][6];
  double fitp_param[4][5][6];
  double fitp_error[4][5][6];
  Double_t p[npbin];
  Double_t perr[npbin];

  double old_fitp_param[4][5][6];
  double old_fitp_error[4][5][6];

  for (int icut = 0 ; icut < 4 ; ++icut) {
    //
    for (int ipbin = 0; ipbin < npbin; ++ipbin) {
      if (icut==0) p[ipbin] = pmin_0[ipbin] + (pmax_0[ipbin]-pmin_0[ipbin])/2;
      if (icut==1) p[ipbin] = pmin_1[ipbin] + (pmax_1[ipbin]-pmin_1[ipbin])/2;
      if (icut==2) p[ipbin] = pmin_2[ipbin] + (pmax_2[ipbin]-pmin_2[ipbin])/2;
      if (icut==3) p[ipbin] = pmin_3[ipbin] + (pmax_3[ipbin]-pmin_3[ipbin])/2;
      perr[ipbin] = 0;
      //
      cout << "icut = " << icut << " ipbin = " << ipbin << " " ;
      if (icut==0) cout << pmin_0[ipbin] << " < p < " << pmax_0[ipbin];
      if (icut==1) cout << pmin_1[ipbin] << " < p < " << pmax_1[ipbin];
      if (icut==2) cout << pmin_2[ipbin] << " < p < " << pmax_2[ipbin];
      if (icut==3) cout << pmin_3[ipbin] << " < p < " << pmax_3[ipbin];
      cout << " fit_status = " << fit_status[icut][ipbin] << " fit_param = " ;
      for (int i = 0; i < 6; ++i) {
	cout << fit_param[icut][i][ipbin] << " " ;
      }
      cout << endl;
    }
    //
    for (int i = 0; i < 6; ++i) {
      //
      fitp_status[icut][i] = -1;
      //
      for (int ip = 0; ip < 5; ++ip) {
	fitp_param[icut][ip][i] = 0;
	fitp_error[icut][ip][i] = 0;
      }
      //
      if (icut==0 || icut==1 || icut==2) {
	if (i==0) {
	  fitp_param[icut][0][i] =  0.7568;
	  fitp_param[icut][1][i] = -0.0001469;
	  fitp_param[icut][2][i] =  5.413E-7 ;
	  fitp_param[icut][3][i] = -6.754E-10 ;
	  fitp_param[icut][4][i] =  2.269E-13;
	} else if (i==1) {
	  fitp_param[icut][0][i] = -0.0288208;
	  fitp_param[icut][1][i] =  0.134174;
	  fitp_param[icut][2][i] = -142.588 ;
	  fitp_param[icut][3][i] = -0.00035606 ;
	  fitp_param[icut][4][i] = -6.94567E-20;
	} else if (i==2) {
	  fitp_param[icut][0][i] = -0.00468927;
	  fitp_param[icut][1][i] =  0.0378737;
	  fitp_param[icut][2][i] = -260.284 ;
	  fitp_param[icut][3][i] =  0.00241158 ;
	  fitp_param[icut][4][i] = -6.01766E-7;
	} else if (i==3) {
	  fitp_param[icut][0][i] = -0.170424;
	  fitp_param[icut][1][i] =  0.135764;
	  fitp_param[icut][2][i] = -50.2361 ;
	  fitp_param[icut][3][i] =  0.00735544 ;
	  fitp_param[icut][4][i] = -7.34073E-6;
	} else if (i==4) {
	  fitp_param[icut][0][i] = -0.0081364;
	  fitp_param[icut][1][i] =  0.0391428;
	  fitp_param[icut][2][i] = -141.936 ;
	  fitp_param[icut][3][i] =  0.0035034 ;
	  fitp_param[icut][4][i] = -1.21956E-6;
	} else if (i==5) {
	  fitp_param[icut][0][i] = 0.0;
	  fitp_param[icut][1][i] = 0.0;
	  fitp_param[icut][2][i] = 0.0 ;
	  fitp_param[icut][3][i] = 0.624*0.00125 ;
	  fitp_param[icut][4][i] = 0.0;
	}
      } else if (icut==3){
	if (i==0) {
	  fitp_param[icut][0][i] =  0.7562;
	  fitp_param[icut][1][i] = -1.168E-5;
	  fitp_param[icut][2][i] =  0.0 ;
	  fitp_param[icut][3][i] =  0.0 ;
	  fitp_param[icut][4][i] =  0.0;
	} else if (i==1) {
	  fitp_param[icut][0][i] = -0.0420458;
	  fitp_param[icut][1][i] =  0.15917;
	  fitp_param[icut][2][i] = -80.3259 ;
	  fitp_param[icut][3][i] =  0.000125729 ;
	  fitp_param[icut][4][i] = -2.43945E-18;
	} else if (i==2) {
	  fitp_param[icut][0][i] = -0.0216898;
	  fitp_param[icut][1][i] =  0.0243497;
	  fitp_param[icut][2][i] = -63.8273 ;
	  fitp_param[icut][3][i] =  0.0148339 ;
	  fitp_param[icut][4][i] = -4.45351E-6;
	} else if (i==3) {
	  fitp_param[icut][0][i] = -0.0879411;
	  fitp_param[icut][1][i] =  0.110092;
	  fitp_param[icut][2][i] = -75.4901 ;
	  fitp_param[icut][3][i] =  0.0116915 ;
	  fitp_param[icut][4][i] = -1E-5;
	} else if (i==4) {
	  fitp_param[icut][0][i] = -0.0118324;
	  fitp_param[icut][1][i] =  0.0280538;
	  fitp_param[icut][2][i] = -85.127 ;
	  fitp_param[icut][3][i] =  0.00724948 ;
	  fitp_param[icut][4][i] = -2.38792E-6;
	} else if (i==5) {
	  fitp_param[icut][0][i] =  0.0;
	  fitp_param[icut][1][i] =  0.0;
	  fitp_param[icut][2][i] =  0.0 ;
	  fitp_param[icut][3][i] =  0.6167*0.00125 ;
	  fitp_param[icut][4][i] =  0.0;
	}
      }
      //      
      double plotmax = 10.;
      if (i==0) plotmax = 50.;
      if (i==1) plotmax = 0.05;
      if (i==2) plotmax = 0.06;
      if (i==3) plotmax = 0.03;
      if (i==4) plotmax = 0.008;
      if (i==5) plotmax = 35;

      /*
      TH1D* hpfit = new TH1D(Form("Fit_p%d",i),Form("Fit_p%d",icut,i),2110,25.,2135.);
      for (int ipbin = 0; ipbin < 10; ++ipbin) {
	Double_t p = pmin[ipbin] + (pmax[ipbin]-pmin[ipbin])/2;
	int ibin = hpfit->FindBin(p);
	hpfit->SetBinContent(ibin,fit_param[icut][i][ipbin]);
	hpfit->SetBinError  (ibin,fit_error[icut][i][ipbin]);
	cout << hpfit->GetName() << " ipbin = " << ipbin << " p = " << p << " ibin = " << ibin << " cont = " <<  fit_param[icut][i][ipbin] << " err = " << fit_error[icut][i][ipbin] << endl;
      }
      */
     
      TGraphErrors* gr = new TGraphErrors(npbin, p, fit_param[icut][i], perr, fit_error[icut][i] );

      for (int ip = 0; ip < 5; ++ip) {
	old_fitp_param[icut][ip][i] = fitp_param[icut][ip][i];
      }
      
      TF1 *func_0 ;
      if (i==0) {
	func_0 = new TF1("fit_0",dTheta3Dparam0,0,750,5);
      } else {
	func_0 = new TF1("fit_0",dTheta3Dparam, 0,750,5);
      }
      func_0->SetParameters(old_fitp_param[icut][0][i],
			    old_fitp_param[icut][1][i],
			    old_fitp_param[icut][2][i],
			    old_fitp_param[icut][3][i],
			    old_fitp_param[icut][4][i]);
      TF1 *func_1 ;
      if (i==0) {
	func_1 = new TF1("fit_1",dTheta3Dparam0,0,750,5);
      } else {
	func_1 = new TF1("fit_1",dTheta3Dparam, 0,750,5);
      }
      func_1->SetParameters(fitp_param[icut][0][i],
			    fitp_param[icut][1][i],
			    fitp_param[icut][2][i],
			    fitp_param[icut][3][i],
			    fitp_param[icut][4][i]);
      

      
      TCanvas* cnew_0=new TCanvas("cnew_0","cnew_0",400,400);
      func_0->SetLineColor(4);
      func_0->Draw();
      gPad->Modified();
      gPad->Update();
      TLatex t0(.5,.7, Form("p0 = %g",old_fitp_param[icut][0][i]));     t0.SetNDC(kTRUE);   t0.Draw();
      TLatex t1(.5,.6, Form("p1 = %g",old_fitp_param[icut][1][i]));     t1.SetNDC(kTRUE);   t1.Draw();
      TLatex t2(.5,.5, Form("p2 = %g",old_fitp_param[icut][2][i]));     t2.SetNDC(kTRUE);   t2.Draw();
      TLatex t3(.5,.4, Form("p3 = %g",old_fitp_param[icut][3][i]));     t3.SetNDC(kTRUE);   t3.Draw();
      TLatex t4(.5,.3, Form("p4 = %g",old_fitp_param[icut][4][i]));     t4.SetNDC(kTRUE);   t4.Draw();
      gPad->Update();   
      cnew_0->SaveAs(Form("/afs/cern.ch/user/a/atpathak/afswork/public/Pixel/LFV_Plots/Plots_Fit_tautau/decay%d_FuncP%d.pdf",icut,i));
      delete cnew_0;
      cnew_0=0;
      cout << "cleaned cnew_0" << endl;

      TCanvas* cnew=new TCanvas("cnew","cnew",400,400);
      if (icut == 0) gr->SetTitle(Form("Angle(n_cha = 1 & n_neu = 0): Fit_p%d",i));
      if (icut == 1) gr->SetTitle(Form("Angle(n_cha = 1 & n_neu > 0): Fit_p%d",i));
      if (icut == 2) gr->SetTitle(Form("Angle(n_cha = 1): Fit_p%d",i));
      if (icut == 3) gr->SetTitle(Form("Angle(n_cha = 3): Fit_p%d",i));
      gr->Print();
      gr->Draw();
      gr->Fit("fit_1","MER");
      gPad->Modified();
      gPad->Update();
      cnew->SaveAs(Form("/afs/cern.ch/user/a/atpathak/afswork/public/Pixel/LFV_Plots/Plots_Fit_tautau/decay%d_FitP%d.pdf",icut,i));
      cout << "clean cnew" << endl;
      delete cnew;
      cnew=0;
      cout << "cleaned cnew" << endl;
      //
      cout << "cleaning up func_0 " << endl;
      delete func_0;
      func_0 =0;
      cout << "cleaned up func_0 " << endl;
      //
      cout << "cleaning up func_1 " << endl;
      delete func_1;
      func_1 =0;
      cout << "cleaned up func_1 " << endl;
      //
      cout << "cleaning graph " << endl;
      delete gr ;
      gr=0;
      cout << "cleaned graph " << endl;
      //
      cout << "i = " << i << " done " << endl; 
    }
    cout << "icut = " << icut << " done " << endl;
  }
  return 0;
}
      
