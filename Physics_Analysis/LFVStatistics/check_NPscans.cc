#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGraph.h"
#include "TLine.h"
#include "TH1.h"
#include "TLatex.h"
#include "TLegend.h"
#include <iostream>
#include <sys/stat.h>
#include <cstdlib>
#include <stdio.h>
#include <algorithm> 
using namespace std;
//
int main(int argc, char* argv[]){
  //
  // Argument variables
  //
  TString vname      = argv[1];
  
  int data = atoi(argv[2]);
  
  TString pname = argv[3]; 
  
  double xmass = atof(argv[4]);

  bool verbose = atoi(argv[5]);

  gROOT->SetStyle("Plain"); 
  gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadBottomMargin(.15);
  gStyle->SetPadRightMargin(.025);
  gStyle->SetPadLeftMargin(.15);
  gStyle->SetPadTopMargin(.05);
  gStyle->SetTextFont(42);
  gStyle->SetLabelFont(42,"X");
  gStyle->SetLabelFont(42,"Y");
  gStyle->SetLabelSize(0.044,"X");
  gStyle->SetLabelSize(0.044,"Y");
  gStyle->SetNdivisions(5,"X");
  gStyle->SetTickLength(.015,"X");
  gStyle->SetTickLength(.015,"Y");
  
  vector<double> xval;
  xval.push_back(-3.0);
  xval.push_back(-2.5);
  xval.push_back(-2.0);
  //  xval.push_back(-2.1);
  xval.push_back(-1.5);
  xval.push_back(-1.0);
  xval.push_back(-0.5);
  xval.push_back(0.0);
  xval.push_back(0.5);
  xval.push_back(1.0);
  xval.push_back(1.5);
  //  xval.push_back(2.1);
  xval.push_back(2.0);
  xval.push_back(2.5);
  xval.push_back(3.1);

  int nval=xval.size() -1;

  TString dname;
  if (data == 1) {
    dname="ObsData";
  } else if (data == 2) {
    dname="Asimov1";
  }
  //  cout << "data = " << data << " dname = " << dname << endl;
  
  for (int ival=0;ival<nval;++ival) {
    if (verbose) cout << endl;
    double xlow = xval[ival];

    double xhig = xval[ival+1];

    bool resubmit=false;
    TString fname=Form("root-files/%s_%3.0f_%d_scans/%s_%3.1f_%3.1f.root",pname.Data(),xmass,data,vname.Data(),xlow,xhig);
    
    if (gSystem->AccessPathName(fname)) {
      if (verbose) cout << "Filename: " << fname << " doesn't exist!" << endl;
      resubmit = true;
    } else { 
      TFile*file=TFile::Open(fname);
      if (!file) {
	if (verbose) cout << "Could not open : " << fname << endl;
	resubmit = true;
      } else {
	if (verbose) cout << "Opened " << fname << endl;
	TGraph* graph=(TGraph*)file->Get("Graph");
	if (!graph) {
	  if (verbose) cout << "No graph found in " << fname << endl;
	  resubmit = true;
	} else {
	  if (verbose) cout << "Read graph from " << fname << endl;
	}
	file->Close();
      }
    }
    if (verbose) cout << vname << " " << xlow << " " <<  xhig << " resubmit = " << resubmit << endl;
    if (resubmit) {
      TString logfilename=Form("log/scans/%s_%3.0d_%1d/NPscan_%s_%3.1f_%3.1f.log",pname.Data(),(int)xmass,data,vname.Data(),xlow,xhig);
      TString rootfilename=Form("/afs/cern.ch/user/s/swaban/xdata4/Summer2014/9Oct2014/tautau1/results/%s_%3.0d_combined_AllSys_model.root",pname.Data(),(int)xmass);
      TString workname="combined";
      TString modelname="ModelConfig";
      TString dataname="";
      if (data==1) dataname="obsData";
      if (data==2) dataname="asimovData_1_pamh";
      TString poiname="SigXsecOverSM";
      cout << Form("rm -f %s ; bsub_lxplus wisc %s 1 NPscan %s %s %s %s %s %s_%3.0d_%d %s %3.1f %3.1f \n",
		   logfilename.Data(),logfilename.Data(),rootfilename.Data(),workname.Data(),modelname.Data(),dataname.Data(), poiname.Data(),
		   pname.Data(),(int)xmass,data,vname.Data(),xlow,xhig) ;
    }
  }
  return 0;
  
}
