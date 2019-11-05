#include "TH1.h"
#include "TString.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* atof */
#include "TROOT.h"
#include "TStyle.h"
#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TGraphErrors.h"
#include "TFrame.h"
#include <TPaveStats.h>
#include "TAttText.h"
#include "TMultiGraph.h"
using namespace std;
void SetUp(){
  cout << "SetUp Plain " << endl;
  gROOT->SetStyle("Plain");
  //  gROOT->SetBatch(kTRUE);
  gROOT->ForceStyle();
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.3);
  gStyle->SetStatX(0.95);
  gStyle->SetStatY(0.95);
  gStyle->SetStatStyle(0);
  gStyle->SetStatFontSize(0.1);
  gStyle->SetStatTextColor(4);
  gStyle->SetOptTitle(0);
  //  gStyle->SetStatFontSize(0.08);
  //  gStyle->SetTitleH(0.1);
  //  gStyle->SetTitleW(0.60);
  //  gStyle->SetTitleX(0.25);
  //  gStyle->SetTitleY(0.90);
  //  gStyle->SetTitleStyle(1);
  //  gStyle->SetTitleFontSize(0.1);
  //  gStyle->SetLabelFont(64,"X");
  //  gStyle->SetLabelSize(20,"X");
  //  gStyle->SetTitleOffset(1,"X");
  //  gStyle->SetTitleFont(64,"X");
  //  gStyle->SetTitleSize(30,"X");
  //  gStyle->SetLabelFont(64,"Y");
  //  gStyle->SetLabelSize(20,"Y");
  gStyle->SetTitleOffset(1,"Y");
  //  gStyle->SetTitleFont(64,"Y");
  //  gStyle->SetTitleSize(30,"Y");
  //  gStyle->SetLabelFont(64,"Z");
  //  gStyle->SetLabelSize(20,"Z");
  gStyle->SetNdivisions(504,"X");
  gStyle->SetNdivisions(505,"Y");
  gStyle->SetOptFit();
  gStyle->SetOptStat(1110);
  gStyle->SetPadTopMargin(0.05);
  //  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetPadRightMargin(0.05);
  //
  if (!gROOT->GetClass("TMultiLayerPerceptron")) {
    gSystem->Load("libMLP");
    gSystem->Load("libTreePlayer");
  }
  return;
}
// define a function with 3 parameters
Double_t fitf(Double_t *x,Double_t *par) {
  Double_t arg = 0;
  if (par[2]!=0) arg = (x[0] - par[1])/par[2];
  Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg);
  return fitval;
}
int main(int argc, char *argv[]) {
  //void histogrambinned(){
  SetUp();
  
  for (int ialg=0; ialg<9; ++ialg) {
    
    string salg = " ";
    if (ialg==0)  salg="AthAlgSeq"; 
    if (ialg==1)  salg="SiPreProcessing";  
    if (ialg==2)  salg="InDetSiTrackerSpacePointFinder";  
    if (ialg==3)  salg="InDetSiSpTrackFinderSLHC";    
    if (ialg==4)  salg="InDetAmbiguitySolverSLHC";  
    if (ialg==5)  salg="InDetPriVxFinder";
    if (ialg==6)  salg="ITkgenonly";
    if (ialg==7)  salg="MuonCombinedInDetCandidateAlg";  
    if (ialg==8)  salg="AllAlgorithm"; 
    
    
    double fitted_mean[5];
    double fitted_merr[5];
    double fitted_nevents[5];
    double fitted_xerr[5];

    for (int m=0; m < 1; ++m) {
      int nevents = 1;
      if (m==0) nevents = 49 ;

      
      
      
      
      //const int ntrial = 30;
      const int ntrial = 20;
      cout << ntrial << endl;
      //                      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 20 21 22 23 24 25 26 27 28 29
      // double times[ntrial] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  };
      double times[ntrial] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0 };
      // ={336,295,310,297,292,308,296,291,303,292,306,310,310,317,284,324,330,352,296,310};
      
      
      
      string line;
      string ffile = Form("mydata_%s_%d.txt",salg.c_str(),nevents);
      //cout << "itag = " << itag << " ialg = " << ialg << " ffile = " << ffile << endl;
      cout << "m = " << m << " ialg = " << ialg << " ffile = " << ffile << endl;
      
      ifstream myfile(ffile.c_str());

      if (myfile.is_open()){
	int iline=0;
	while ( getline (myfile,line) ) {
	  cout << iline << " " << line << '\n';

	  times[iline] = atof(line.c_str())/nevents;

	  ++iline;
	}
	myfile.close();
      }
      else {
	cout << "Unable to open file";
      }
      
      double mintime=9999;
      double maxtime=0;
      for (int i=0;i<20;++i) {
	if (times[i] < mintime) mintime = times[i];
	if (times[i] > maxtime) maxtime = times[i];
      }
      cout << "mintime = " << mintime << " maxtime = " << maxtime << endl;

      double plotmintime = mintime*0.78;
      if(ialg == 2) plotmintime = 4.5;
      double plotmaxtime = maxtime*2.;
      if(ialg == 2) plotmaxtime = 20.0;
      
      /*
      if (ialg==1) plotmaxtime = 120.;
      if (ialg==5) plotmaxtime = 0.6;
      if (ialg==6) plotmaxtime = 16.;
      if (ialg==10) plotmaxtime = 0.004;
      */
      
      int nbin = 40;
      if(ialg == 0) nbin =30;
      if(ialg == 1) nbin =30;
      if(ialg == 2) nbin =20;
      if(ialg == 5) nbin =30;
      if(ialg == 7) nbin =30;
      
      /*
      if(ialg == 1) nbin =25;
      if(ialg == 3) nbin =30;
      if(ialg == 4) nbin =30;
      if(ialg == 5) nbin =20;
      if(ialg == 6) nbin =35;
      if(ialg == 7) nbin =30;
      if(ialg == 8) nbin =40;
      if(ialg == 9) nbin =30;
      if(ialg == 10) nbin =20;
      if(ialg == 11) nbin =20;
      if(ialg == 12) nbin =20;
      if(ialg == 13) nbin =30;
      */
      
      TString units = "sec";
      TH1F* htot = new TH1F("Total time",Form("Time distribution;time (%s)",units.Data()),nbin,plotmintime,plotmaxtime);
      for (int i=0; i<ntrial; ++i) htot->Fill(times[i]);
      //  gStyle->SetOptStat(1);
      gStyle->SetOptFit(1);
      htot->SetLineWidth(2);
      htot->SetLineColor(2);

      double fitmin = .8*mintime;
      double fitmax = 1.2*maxtime;
      
      /*if (ialg == 4){
	fitmin = 5.;
	fitmax = 8.;
	}*/
      

      double guessed_fitmean = htot->GetMean();
      double guessed_fitrms =  htot->GetRMS();
      /*
      if (ialg==3) {
       	if (m==0) {
	  fitmin =.8*mintime;
	  fitmax = 1.2*maxtime;
 	} else if (m==1) {
	  fitmin =.8*mintime;
	  fitmax = 1.2*maxtime;
	}
        else if (m==2) {
	  fitmin =.65*mintime;
	  fitmax = 1.2*maxtime;
	}
        else if (m==3) {
	  guessed_fitrms = 0.04;
	  fitmin = guessed_fitmean - 3*guessed_fitrms;
	  fitmax = guessed_fitmean + 3*guessed_fitrms;
	}
	else if (m==4){
	  guessed_fitrms = 0.04;
	  fitmin = 0.9*(guessed_fitmean - 3*guessed_fitrms);
	  fitmax = guessed_fitmean + 3*guessed_fitrms;
	}
	else {
	
	guessed_fitrms = 0.02;
	fitmin = guessed_fitmean - 3*guessed_fitrms;
	fitmax = guessed_fitmean + 3*guessed_fitrms;
	//}
    }
      cout << "ialg = " << ialg << " salg = " << salg << " fitmin = " << fitmin << " fitmax = " << fitmax << " mean = " << guessed_fitmean << " rms = " << guessed_fitrms << endl;
*/
      
      // Create a TF1 object using the function defined above.
      // The last three parameters specify the number of parameters
      // for the function.
      TF1 *func = new TF1("fit",fitf,plotmintime,plotmaxtime,3);
      // set the parameters to the mean and RMS of the histogram
      func->SetParameters(1,guessed_fitmean,guessed_fitrms);
      
      // give the parameters meaningful names
      func->SetParNames ("Constant","Mean","Sigma");

      htot->Fit("fit","LLMR","",fitmin,fitmax);

      
      
      htot->SetMaximum(1.25*htot->GetMaximum());

      if (ialg==0)    htot->SetMaximum(12);
      if (ialg==1)    htot->SetMaximum(15);
      if (ialg==2)    htot->SetMaximum(20);
      if (ialg==3)    htot->SetMaximum(20);
      if (ialg==4)    htot->SetMaximum(12);
      if (ialg==5)    htot->SetMaximum(12);
      if (ialg==6)    htot->SetMaximum(12);
      if (ialg==7)    htot->SetMaximum(12);
      if (ialg==8)    htot->SetMaximum(12);
      

      //    htot->Draw();
      cout << htot->GetMean(1) << endl;

      fitted_nevents[m]=nevents;
      fitted_xerr[m] = 0;
      cout << func->GetParameter("Mean") << " " << func->GetParameter(1) << " Width = " << func->GetParameter(2) << endl;;
      fitted_mean[m] = func->GetParameter(1);
      fitted_merr[m] = func->GetParameter(2)/sqrt(20);
      cout << "Error: " << func->GetParameter(2)/sqrt(20) << endl;
      double term1 = fitted_merr[m]/func->GetParameter(1);
      double term2 = 0.15/3.5 ;
      cout << "Time in HSO6: " << func->GetParameter(1)*3.5 << endl ;
      cout << "Error Table: " << func->GetParameter(1)*3.5*sqrt(term1*term1 + term2*term2) << endl;
      
      //      hlin->SetBinContent(nevents,(fitted_mean[m]));
      
      TCanvas* c1=new TCanvas("c1","c1",400,400);
      htot->Draw();
      c1->Update();
      c1->SaveAs(Form("pic/mydata_%s_%d.png",salg.c_str(),nevents));
      
      delete c1;
      c1=0;

      delete htot;
      htot=0;
      
    }
  }
  return 0;
}

