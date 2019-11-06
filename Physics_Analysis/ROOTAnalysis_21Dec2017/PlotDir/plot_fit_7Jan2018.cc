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
//
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
}//
double myDelThetaHadFuncNew(double *x, double *par){
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
  fitval*=par[6];
  //
  if(fitval<0.0) return 0.0;
  return fitval;
}
//
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
//
double dTheta3Dparam0(double *p, double *par) {
 //tau_type 0: l, 1:1-prong, 3:3-prong
  const double P_tau=p[0];
  if (P_tau<0.0) {
    return 0.0;
  } else {
    return (par[0]+par[1]*P_tau+par[2]*pow(P_tau,2)+par[3]*pow(P_tau,3)+par[4]*pow(P_tau,4))*0.00125;
  }
}
int main(int argc, char* argv[]){
  //
  // Argument variables
  //
  int imode    = (argc>1) ? atoi(argv[1]) : 0;
  //
  gStyle->SetOptStat("e");
  gStyle->SetOptFit(01);
  //
  //
  //  TFile *f1 = TFile::Open("AllRoot/all.root");  
  //  TFile *f1 = TFile::Open("../AllRoot/ggH125_tautau_new.root");
  //  TTree *mytree1 = (TTree*)f1->Get("tree");
  //
  TChain mychain1a("tree");
  mychain1a.Add("../AllRoot/ggH125_taue_new.root");
  mychain1a.Add("../AllRoot/VBFH125_taue_new.root");
  TTree *mytree1a = &mychain1a;
  //
  TChain mychain1b("tree");
  mychain1b.Add("../AllRoot/ggH125_taumu_new.root");
  mychain1b.Add("../AllRoot/VBFH125_taumu_new.root");  
  TTree *mytree1b = &mychain1b;
  //
  TChain mychain2("tree");
  mychain2.Add("../AllRoot/ggH125_tautau_new.root");
  TTree *mytree2 = &mychain2;
  //
  TChain mychain3("tree");
  mychain3.Add("../AllRoot/Ztautau_8TeV_0_100.root");
  TTree *mytree3 = &mychain3;
  //
  const int npbin=10;
  //
  int pmin_0[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_0[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_0;
  for (int i =0; i<npbin; ++i) {
    cuts_0.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_0[i],pmax_0[i]));
  }
  //
  int pmin_1[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_1[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_1;
  for (int i =0; i<npbin; ++i) {
    cuts_1.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_1[i],pmax_1[i]));
  }
  //
  int pmin_2[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_2[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_2;
  for (int i =0; i<npbin; ++i) {
    cuts_2.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_2[i],pmax_2[i]));
  }
  //
  int pmin_3[npbin] = {20, 40, 50, 60, 70, 80,  100, 150, 250,  450};
  int pmax_3[npbin] = {40, 50, 60, 70, 80, 100, 150, 250, 450, 1000};
  std::vector<TString> cuts_3;
  for (int i =0; i<npbin; ++i) {
    cuts_3.push_back(Form("tau_momentum/1000.  > %d  &&  tau_momentum/1000.  < %d",pmin_3[i],pmax_3[i]));
  }
  //  
  TString cut0 = "n_truth_tau==1 && IsHadronicTau == 1"; // H->etau,mutau
  TString cut1 = "n_truth_tau==2 && IsHadronicTau == 1"; // H->tautau
  TString cut2 = "angle_vis_neu < .3";
  TString cut3 = "numCharged == 1";
  TString cut4 = "numNeutral == 0";
  TString cut5 = "numNeutral > 0";
  TString cut6 = "numCharged == 3";
  //
  int fit_status[4][npbin];
  double old_fit_param[4][6][npbin];
  double fit_param[4][6][npbin];
  double fit_error[4][6][npbin];
  //
  Double_t p[4][npbin];
  Double_t perr[4][npbin];
  //
  int fitp_status[4][6];
  double fitp_param[4][5][6];
  double fitp_error[4][5][6];
  //
  // old_fitp_param[1p0n/1pnn/1p/3p][5 parameters of p-fit function dTheta3Dparam][6 parameters of theta-fit function myDelThetaHadFunc]
  //
  double old_fitp_param[4][5][6];
  //
  old_fitp_param[0][0][0] =  0.7568;
  old_fitp_param[0][1][0] = -0.0001469;
  old_fitp_param[0][2][0] =  5.413E-7 ;
  old_fitp_param[0][3][0] = -6.754E-10 ;
  old_fitp_param[0][4][0] =  2.269E-13;
  //
  old_fitp_param[0][0][1] = -0.0288208;
  old_fitp_param[0][1][1] =  0.134174;
  old_fitp_param[0][2][1] = -142.588 ;
  old_fitp_param[0][3][1] = -0.00035606 ;
  old_fitp_param[0][4][1] = -6.94567E-20;
  //
  old_fitp_param[0][0][2] = -0.00468927;
  old_fitp_param[0][1][2] =  0.0378737;
  old_fitp_param[0][2][2] = -260.284 ;
  old_fitp_param[0][3][2] =  0.00241158 ;
  old_fitp_param[0][4][2] = -6.01766E-7;
  //
  old_fitp_param[0][0][3] = -0.170424;
  old_fitp_param[0][1][3] =  0.135764;
  old_fitp_param[0][2][3] = -50.2361 ;
  old_fitp_param[0][3][3] =  0.00735544 ;
  old_fitp_param[0][4][3] = -7.34073E-6;
  //
  old_fitp_param[0][0][4] = -0.0081364;
  old_fitp_param[0][1][4] =  0.0391428;
  old_fitp_param[0][2][4] = -141.936 ;
  old_fitp_param[0][3][4] =  0.0035034 ;
  old_fitp_param[0][4][4] = -1.21956E-6;
  //
  old_fitp_param[0][0][5] = 0.0;
  old_fitp_param[0][1][5] = 0.0;
  old_fitp_param[0][2][5] = 0.0 ;
  old_fitp_param[0][3][5] = 0.624*0.00125 ;
  old_fitp_param[0][4][5] = 0.0;
  //
  //
  old_fitp_param[1][0][0] =  0.7568;
  old_fitp_param[1][1][0] = -0.0001469;
  old_fitp_param[1][2][0] =  5.413E-7 ;
  old_fitp_param[1][3][0] = -6.754E-10 ;
  old_fitp_param[1][4][0] =  2.269E-13;
  //
  old_fitp_param[1][0][1] = -0.0288208;
  old_fitp_param[1][1][1] =  0.134174;
  old_fitp_param[1][2][1] = -142.588 ;
  old_fitp_param[1][3][1] = -0.00035606 ;
  old_fitp_param[1][4][1] = -6.94567E-20;
  //
  old_fitp_param[1][0][2] = -0.00468927;
  old_fitp_param[1][1][2] =  0.0378737;
  old_fitp_param[1][2][2] = -260.284 ;
  old_fitp_param[1][3][2] =  0.00241158 ;
  old_fitp_param[1][4][2] = -6.01766E-7;
  //
  old_fitp_param[1][0][3] = -0.170424;
  old_fitp_param[1][1][3] =  0.135764;
  old_fitp_param[1][2][3] = -50.2361 ;
  old_fitp_param[1][3][3] =  0.00735544 ;
  old_fitp_param[1][4][3] = -7.34073E-6;
  //
  old_fitp_param[1][0][4] = -0.0081364;
  old_fitp_param[1][1][4] =  0.0391428;
  old_fitp_param[1][2][4] = -141.936 ;
  old_fitp_param[1][3][4] =  0.0035034 ;
  old_fitp_param[1][4][4] = -1.21956E-6;
  //
  old_fitp_param[1][0][5] = 0.0;
  old_fitp_param[1][1][5] = 0.0;
  old_fitp_param[1][2][5] = 0.0 ;
  old_fitp_param[1][3][5] = 0.624*0.00125 ;
  old_fitp_param[1][4][5] = 0.0;
  //
  //
  old_fitp_param[2][0][0] =  0.7568;
  old_fitp_param[2][1][0] = -0.0001469;
  old_fitp_param[2][2][0] =  5.413E-7 ;
  old_fitp_param[2][3][0] = -6.754E-10 ;
  old_fitp_param[2][4][0] =  2.269E-13;
  //
  old_fitp_param[2][0][1] = -0.0288208;
  old_fitp_param[2][1][1] =  0.134174;
  old_fitp_param[2][2][1] = -142.588 ;
  old_fitp_param[2][3][1] = -0.00035606 ;
  old_fitp_param[2][4][1] = -6.94567E-20;
  //
  old_fitp_param[2][0][2] = -0.00468927;
  old_fitp_param[2][1][2] =  0.0378737;
  old_fitp_param[2][2][2] = -260.284 ;
  old_fitp_param[2][3][2] =  0.00241158 ;
  old_fitp_param[2][4][2] = -6.01766E-7;
  //
  old_fitp_param[2][0][3] = -0.170424;
  old_fitp_param[2][1][3] =  0.135764;
  old_fitp_param[2][2][3] = -50.2361 ;
  old_fitp_param[2][3][3] =  0.00735544 ;
  old_fitp_param[2][4][3] = -7.34073E-6;
  //
  old_fitp_param[2][0][4] = -0.0081364;
  old_fitp_param[2][1][4] =  0.0391428;
  old_fitp_param[2][2][4] = -141.936 ;
  old_fitp_param[2][3][4] =  0.0035034 ;
  old_fitp_param[2][4][4] = -1.21956E-6;
  //
  old_fitp_param[2][0][5] = 0.0;
  old_fitp_param[2][1][5] = 0.0;
  old_fitp_param[2][2][5] = 0.0 ;
  old_fitp_param[2][3][5] = 0.624*0.00125 ;
  old_fitp_param[2][4][5] = 0.0;
  //
  //
  old_fitp_param[3][0][0] =  0.7562;
  old_fitp_param[3][1][0] = -1.168E-5;
  old_fitp_param[3][2][0] =  0.0 ;
  old_fitp_param[3][3][0] =  0.0 ;
  old_fitp_param[3][4][0] =  0.0;
  //
  old_fitp_param[3][0][1] = -0.0420458;
  old_fitp_param[3][1][1] =  0.15917;
  old_fitp_param[3][2][1] = -80.3259 ;
  old_fitp_param[3][3][1] =  0.000125729 ;
  old_fitp_param[3][4][1] = -2.43945E-18;
  //
  old_fitp_param[3][0][2] = -0.0216898;
  old_fitp_param[3][1][2] =  0.0243497;
  old_fitp_param[3][2][2] = -63.8273 ;
  old_fitp_param[3][3][2] =  0.0148339 ;
  old_fitp_param[3][4][2] = -4.45351E-6;
  //
  old_fitp_param[3][0][3] = -0.0879411;
  old_fitp_param[3][1][3] =  0.110092;
  old_fitp_param[3][2][3] = -75.4901 ;
  old_fitp_param[3][3][3] =  0.0116915 ;
  old_fitp_param[3][4][3] = -1E-5;
  //
  old_fitp_param[3][0][4] = -0.0118324;
  old_fitp_param[3][1][4] =  0.0280538;
  old_fitp_param[3][2][4] = -85.127 ;
  old_fitp_param[3][3][4] =  0.00724948 ;
  old_fitp_param[3][4][4] = -2.38792E-6;
  //
  old_fitp_param[3][0][5] =  0.0;
  old_fitp_param[3][1][5] =  0.0;
  old_fitp_param[3][2][5] =  0.0 ;
  old_fitp_param[3][3][5] =  0.6167*0.00125 ;
  old_fitp_param[3][4][5] =  0.0;
  //
  for (int icut=2;icut<4;++icut){ // for each topology
    //
    for (int ipbin = 0; ipbin < npbin; ++ipbin) { // for each momentum bin
      if (icut==0) p[icut][ipbin] = pmin_0[ipbin] + (pmax_0[ipbin]-pmin_0[ipbin])/2;
      if (icut==1) p[icut][ipbin] = pmin_1[ipbin] + (pmax_1[ipbin]-pmin_1[ipbin])/2;
      if (icut==2) p[icut][ipbin] = pmin_2[ipbin] + (pmax_2[ipbin]-pmin_2[ipbin])/2;
      if (icut==3) p[icut][ipbin] = pmin_3[ipbin] + (pmax_3[ipbin]-pmin_3[ipbin])/2;
      perr[icut][ipbin] = 0;
    }
    //
    for (int ipar=0;ipar<6;++ipar){ // for each fit parameter in the theta function
      TF1 *old_pfunction;
      if (ipar==0) {
	old_pfunction = new TF1("old_pfunction",dTheta3Dparam0,0,750,5);
      } else {
	old_pfunction = new TF1("old_pfunction",dTheta3Dparam,0,750,5);
      }
      old_pfunction->SetParameters(old_fitp_param[icut][0][ipar],
				   old_fitp_param[icut][1][ipar],
				   old_fitp_param[icut][2][ipar],
				   old_fitp_param[icut][3][ipar],
				   old_fitp_param[icut][4][ipar]);
      for (int ipbin=0;ipbin<npbin;++ipbin) { // for each momentum bin
	old_fit_param[icut][ipar][ipbin] = old_pfunction->Eval(p[icut][ipbin]); // evaluate at each momentum bin
	fit_param[icut][ipar][ipbin] = old_fit_param[icut][ipar][ipbin];
      }
      delete old_pfunction;
      old_pfunction=0;
    }
    //
    for (int ipbin = 0; ipbin <npbin; ++ipbin) { // for each momentum bin
      fit_status[icut][ipbin] = -1; 
      /*
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
	} else if (ipbin == 8 || ipbin == 9){
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
	} else{
	  fit_param[icut][0][ipbin] = 0.5 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.01;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.9;
	}
      } else if (icut == 3){
	if (ipbin == 0 ){
	  fit_param[icut][0][ipbin] = 0.5 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.02;
	  fit_param[icut][4][ipbin] = 0.02;
	  fit_param[icut][5][ipbin] = 0.8;
	} else if (ipbin == 1 ){
	  fit_param[icut][0][ipbin] = 0.5 ;
	  fit_param[icut][1][ipbin] = 0.2;
	  fit_param[icut][2][ipbin] = 0.2;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.8;
	} else if (ipbin == 2){
	  fit_param[icut][0][ipbin] = 0.8 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.8;
	} else if (ipbin == 3){
	  fit_param[icut][0][ipbin] = 0.1 ;
	  fit_param[icut][1][ipbin] = 0.1;
	  fit_param[icut][2][ipbin] = 0.1;
	  fit_param[icut][3][ipbin] = 0.03;
	  fit_param[icut][4][ipbin] = 0.03;
	  fit_param[icut][5][ipbin] = 0.95;
	} else if (ipbin == 4){
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
	} else if (ipbin == 8){
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
      */
      int nbin=300;
      double thetamin = 0;
      double thetamax = 0.30;
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
      TH1F *hist1a = new TH1F("hist1a", "hist1a" ,nbin, thetamin, thetamax);
      //
      if(icut == 0) mytree1a->Project("hist1a","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut0.Data(), cut2.Data(), cut3.Data(), cut4.Data(), cuts_0[ipbin].Data()));
      if(icut == 1) mytree1a->Project("hist1a","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut0.Data(), cut2.Data(), cut3.Data(), cut5.Data(), cuts_1[ipbin].Data()));
      if(icut == 2) mytree1a->Project("hist1a","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut0.Data(), cut2.Data(), cut3.Data(),              cuts_2[ipbin].Data()));
      if(icut == 3) mytree1a->Project("hist1a","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut0.Data(), cut2.Data(), cut6.Data(),              cuts_3[ipbin].Data()));
      //
      TH1F *hist1b = new TH1F("hist1b", "hist1b" ,nbin, thetamin, thetamax);
      //
      if(icut == 0) mytree1b->Project("hist1b","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut0.Data(), cut2.Data(), cut3.Data(), cut4.Data(), cuts_0[ipbin].Data()));
      if(icut == 1) mytree1b->Project("hist1b","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut0.Data(), cut2.Data(), cut3.Data(), cut5.Data(), cuts_1[ipbin].Data()));
      if(icut == 2) mytree1b->Project("hist1b","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut0.Data(), cut2.Data(), cut3.Data(),              cuts_2[ipbin].Data()));
      if(icut == 3) mytree1b->Project("hist1b","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut0.Data(), cut2.Data(), cut6.Data(),              cuts_3[ipbin].Data()));
      //
      TH1F *hist2 = new TH1F("hist2", "hist2" ,nbin, thetamin, thetamax);
      //
      if(icut == 0) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1.Data(), cut2.Data(), cut3.Data(), cut4.Data(), cuts_0[ipbin].Data()));
      if(icut == 1) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1.Data(), cut2.Data(), cut3.Data(), cut5.Data(), cuts_1[ipbin].Data()));
      if(icut == 2) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1.Data(), cut2.Data(), cut3.Data(),              cuts_2[ipbin].Data()));
      if(icut == 3) mytree2->Project("hist2","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1.Data(), cut2.Data(), cut6.Data(),              cuts_3[ipbin].Data()));
      //
      TH1F *hist3 = new TH1F("hist3", "hist3" ,nbin, thetamin, thetamax);
      //
      if(icut == 0) mytree3->Project("hist3","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1.Data(), cut2.Data(), cut3.Data(), cut4.Data(), cuts_0[ipbin].Data()));
      if(icut == 1) mytree3->Project("hist3","angle_vis_neu", Form( "%s && %s && %s && %s && %s",cut1.Data(), cut2.Data(), cut3.Data(), cut5.Data(), cuts_1[ipbin].Data()));
      if(icut == 2) mytree3->Project("hist3","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1.Data(), cut2.Data(), cut3.Data(),              cuts_2[ipbin].Data()));
      if(icut == 3) mytree3->Project("hist3","angle_vis_neu", Form( "%s && %s && %s && %s"      ,cut1.Data(), cut2.Data(), cut6.Data(),              cuts_3[ipbin].Data()));
      //
      TH1F *hist=(TH1F*)hist1a->Clone("hist");
      hist->Add(hist1b,1);
      hist->Add(hist2,1);
      //
      if (icut==0) {
	hist->SetTitle(Form("Angle(n_cha = 1 & n_neu = 0): %d < P(Gev) < %d", pmin_0[ipbin] , pmax_0[ipbin]));
      } else if (icut==1) {
	hist->SetTitle(Form("Angle(n_cha = 1 & n_neu > 0): %d < P(Gev) < %d", pmin_1[ipbin] , pmax_1[ipbin]));
      } else if (icut==2) {
	hist->SetTitle(Form("Angle(n_cha = 1): %d < P(Gev) < %d", pmin_2[ipbin] , pmax_2[ipbin]));
      } else if (icut==3) {
	hist->SetTitle(Form("Angle(n_cha = 3): %d < P(Gev) < %d", pmin_3[ipbin] , pmax_3[ipbin]));
      }
      double nentries = hist->Integral();
      cout << "icut = " << icut << " ipbin = " << ipbin << " hist->Integral() = " << hist->Integral() << endl;
      hist->Scale(1./hist->Integral());
      hist->SetLineColor(1);
      //
      //      TCanvas* old_c0=new TCanvas("old_c0","old_c0",400,400);
      TF1 *old_func = new TF1("oldthetafit",myDelThetaHadFunc,thetamin,thetamax,6);
      old_func->FixParameter(0,old_fit_param[icut][0][ipbin]);
      old_func->FixParameter(1,old_fit_param[icut][1][ipbin]);
      old_func->FixParameter(2,old_fit_param[icut][2][ipbin]);
      old_func->FixParameter(3,old_fit_param[icut][3][ipbin]);
      old_func->FixParameter(4,old_fit_param[icut][4][ipbin]);
      old_func->FixParameter(5,old_fit_param[icut][5][ipbin]);
      old_func->SetLineColor(8);
      /*
      old_c0->Modified();
      old_c0->Update();
      TLatex ot0(.5,.65, Form("t0 = %g",old_fit_param[icut][0][ipbin]));     ot0.SetNDC(kTRUE);   ot0.Draw();
      TLatex ot1(.5,.55, Form("t1 = %g",old_fit_param[icut][1][ipbin]));     ot1.SetNDC(kTRUE);   ot1.Draw();
      TLatex ot2(.5,.45, Form("t2 = %g",old_fit_param[icut][2][ipbin]));     ot2.SetNDC(kTRUE);   ot2.Draw();
      TLatex ot3(.5,.35, Form("t3 = %g",old_fit_param[icut][3][ipbin]));     ot3.SetNDC(kTRUE);   ot3.Draw();
      TLatex ot4(.5,.25, Form("t4 = %g",old_fit_param[icut][4][ipbin]));     ot4.SetNDC(kTRUE);   ot4.Draw();
      TLatex ot5(.5,.15, Form("t5 = %g",old_fit_param[icut][5][ipbin]));     ot5.SetNDC(kTRUE);   ot5.Draw();
      gPad->Update();   
      old_c0->SaveAs(Form("/afs/cern.ch/user/a/atpathak/afswork/public/Pixel/LFV_Plots/Plots_Fit_tautau_3Jan2018/Angle_Vis_neutrino_decay%d_OldFit_%d.pdf",icut,ipbin+1));
      delete old_c0;
      old_c0=0;
      */
      TH1F* oldhist = new TH1F("oldhist","Old Histogram",nbin,thetamin,thetamax);
      oldhist->FillRandom("oldthetafit",nentries);
      oldhist->Scale(1/oldhist->Integral());
      oldhist->SetLineColor(6);
      oldhist->Draw("same");
      //      
      TF1 *older_func = new TF1("olderthetafit",myDelThetaHadFunc,thetamin,thetamax,6);
      older_func->SetParameters(old_fit_param[icut][0][ipbin],
				old_fit_param[icut][1][ipbin],
				old_fit_param[icut][2][ipbin],
				old_fit_param[icut][3][ipbin],
				old_fit_param[icut][4][ipbin],
				old_fit_param[icut][5][ipbin]);
      oldhist->Fit("olderthetafit","LLMR","N",thetamin,thetamax);
      older_func->SetLineColor(4);
      //
      TF1 *func = new TF1("thetafit",myDelThetaHadFunc,thetamin,thetamax,6);
      func->SetParameters(  fit_param[icut][0][ipbin],
			    fit_param[icut][1][ipbin],
			    fit_param[icut][2][ipbin],
			    fit_param[icut][3][ipbin],
			    fit_param[icut][4][ipbin],
			    fit_param[icut][5][ipbin]);
      func->SetParLimits(2,0,1);    // gaussian sigma >0
      func->SetParLimits(4,0,1);    // landau sigma > 0
      /*
      func->SetParLimits(0,0,1); // norm > 0
      func->SetParLimits(5,0,1); // norm > 0
      func->SetParLimits(1,0,0.25);    // gaussian mean
      func->SetParLimits(3,0,0.25);    // 1andau mean
      */
      func->SetLineColor(2);
      //
      TCanvas* c0=new TCanvas("c0","c0",400,400);
      //      hist->Draw("HIST");
      //
      //hist2->Scale(1./hist2->Integral());
      //      hist2->SetLineColor(2);
      //      hist2->Draw("HIST same");   
      //
      fit_status[icut][ipbin] = hist->Fit("thetafit","LLMR","",thetamin,thetamax);
      for (int ipar=0;ipar<6;++ipar) {
	fit_param[icut][ipar][ipbin] = func->GetParameter(ipar);
	fit_error[icut][ipar][ipbin] = func->GetParError(ipar);
      }
      older_func->Draw("same");
      hist3->Scale(1./hist3->Integral());
      hist3->SetLineColor(9);
      hist3->Draw("same");
      //
      cout << "icut = " << icut << " ipbin = " << ipbin
	   << " old_func->freepar = " << old_func->GetParameter(6) << " "
	   << " old_func->Integral() = " << old_func->Integral(thetamin,thetamax)
	   << " func->Intrgral() = "     <<  func->Integral(thetamin,thetamax)
	   << " hist->Integral() = " << hist->Integral() << " "
	   << " oldhist->Integral() = " << oldhist->Integral() << " "
	   << " Hist/func = "     << hist->Integral() / func->Integral(thetamin,thetamax) << " "
	   << " Hist/old_func = " << hist->Integral() / old_func->Integral(thetamin,thetamax) << " "
	   << endl;
      //
      c0->Modified();
      c0->Update();
      c0->SaveAs(Form("/afs/cern.ch/user/a/atpathak/afswork/public/Pixel/LFV_Plots/Plots_Fit_tautau_3Jan2018/Angle_Vis_neutrino_decay%d_Fit_%d.pdf",icut,ipbin+1));
      delete c0;
      c0=0;
      //
      delete old_func;
      old_func=0;
      delete oldhist;
      oldhist=0;
      delete older_func;
      older_func=0;
      delete func;
      func=0;
      delete hist;
      hist=0;
      delete hist1a;
      hist1a=0;
      delete hist1b;
      hist1b=0;
      delete hist2;
      hist2=0;
      delete hist3;
      hist3=0;
    }
    //
    // Print Summary of fit over momentum bins
    //
    for (int ipbin = 0; ipbin < npbin; ++ipbin) {
      //
      cout << "icut = " << icut << " ipbin = " << ipbin << " " ;
      if (icut==0) cout << pmin_0[ipbin] << " < p < " << pmax_0[ipbin];
      if (icut==1) cout << pmin_1[ipbin] << " < p < " << pmax_1[ipbin];
      if (icut==2) cout << pmin_2[ipbin] << " < p < " << pmax_2[ipbin];
      if (icut==3) cout << pmin_3[ipbin] << " < p < " << pmax_3[ipbin];
      cout << " fit_status = " << fit_status[icut][ipbin] << " fit_param = " ;
      for (int ipar = 0; ipar < 6; ++ipar) {
	cout << fit_param[icut][ipar][ipbin] << " (" << old_fit_param[icut][ipar][ipbin] << ") " ;
      }
      cout << endl;
    }
    //
    for (int ipar = 0; ipar < 6; ++ipar) { // for each fit parameter in the theta function
      //
      fitp_status[icut][ipar] = -1;
      //
      for (int ip = 0; ip < 5; ++ip) {
	fitp_param[icut][ip][ipar] = old_fitp_param[icut][ip][ipar];
	fitp_error[icut][ip][ipar] = 0;
      }
      //      
      double plotmax = 10.;
      if (ipar==0) plotmax = 50.;
      if (ipar==1) plotmax = 0.05;
      if (ipar==2) plotmax = 0.06;
      if (ipar==3) plotmax = 0.03;
      if (ipar==4) plotmax = 0.008;
      if (ipar==5) plotmax = 35;
      //
      TGraphErrors* gr = new TGraphErrors(npbin, p[icut], fit_param[icut][ipar], perr[icut], fit_error[icut][ipar] );
      gr->SetMarkerColor(6);
      gr->SetMarkerStyle(20);
      gr->SetMarkerSize(.5);
      //      
      TF1 *func_0 ;
      if (ipar==0) {
	func_0 = new TF1("old_pfunction",dTheta3Dparam0,0,750,5);
      } else {
	func_0 = new TF1("old_pfunction",dTheta3Dparam, 0,750,5);
      }
      func_0->SetParameters(old_fitp_param[icut][0][ipar],
			    old_fitp_param[icut][1][ipar],
			    old_fitp_param[icut][2][ipar],
			    old_fitp_param[icut][3][ipar],
			    old_fitp_param[icut][4][ipar]);
      func_0->SetLineColor(4);
      //
      TF1 *func_1 ;
      if (ipar==0) {
	func_1 = new TF1("new_pfunction",dTheta3Dparam0,0,750,5);
      } else {
	func_1 = new TF1("new_pfunction",dTheta3Dparam, 0,750,5);
      }
      func_1->SetParameters(fitp_param[icut][0][ipar],
			    fitp_param[icut][1][ipar],
			    fitp_param[icut][2][ipar],
			    fitp_param[icut][3][ipar],
			    fitp_param[icut][4][ipar]);
      func_1->SetLineColor(2);
      //
      TCanvas* cnew_0=new TCanvas("cnew_0","cnew_0",400,400);
      func_0->Draw();
      gPad->Modified();
      gPad->Update();
      TLatex t0(.5,.7, Form("p0 = %g",old_fitp_param[icut][0][ipar]));     t0.SetNDC(kTRUE);   t0.Draw();
      TLatex t1(.5,.6, Form("p1 = %g",old_fitp_param[icut][1][ipar]));     t1.SetNDC(kTRUE);   t1.Draw();
      TLatex t2(.5,.5, Form("p2 = %g",old_fitp_param[icut][2][ipar]));     t2.SetNDC(kTRUE);   t2.Draw();
      TLatex t3(.5,.4, Form("p3 = %g",old_fitp_param[icut][3][ipar]));     t3.SetNDC(kTRUE);   t3.Draw();
      TLatex t4(.5,.3, Form("p4 = %g",old_fitp_param[icut][4][ipar]));     t4.SetNDC(kTRUE);   t4.Draw();
      gPad->Update();   
      cnew_0->SaveAs(Form("/afs/cern.ch/user/a/atpathak/afswork/public/Pixel/LFV_Plots/Plots_Fit_tautau_3Jan2018/decay%d_FuncP%d.pdf",icut,ipar));
      delete cnew_0;
      cnew_0=0;
      cout << "cleaned cnew_0" << endl;
      //
      TCanvas* cnew=new TCanvas("cnew","cnew",400,400);
      if (icut == 0) gr->SetTitle(Form("Angle(n_cha = 1 & n_neu = 0): Fit_p%d",ipar));
      if (icut == 1) gr->SetTitle(Form("Angle(n_cha = 1 & n_neu > 0): Fit_p%d",ipar));
      if (icut == 2) gr->SetTitle(Form("Angle(n_cha = 1): Fit_p%d",ipar));
      if (icut == 3) gr->SetTitle(Form("Angle(n_cha = 3): Fit_p%d",ipar));
      gr->Print();
      gr->Draw();
      gr->Fit("new_pfunction","MER");
      gPad->Modified();
      gPad->Update();
      cnew->SaveAs(Form("/afs/cern.ch/user/a/atpathak/afswork/public/Pixel/LFV_Plots/Plots_Fit_tautau_3Jan2018/decay%d_FitP%d.pdf",icut,ipar));
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
      cout << "ipar = " << ipar << " done " << endl; 
    }
    cout << "icut = " << icut << " done " << endl;
  }
  return 0;
}
