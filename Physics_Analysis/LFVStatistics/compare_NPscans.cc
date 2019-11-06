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
#include <cmath> // for "std::abs" and "std::fabs"
using namespace std;
//
struct point {
  int i;
  double x;
  double y;
  bool operator < (const point & other) const {
    return x < other.x; // for sorting in increasing order in x, so that first element has smallest x
  }
};
//
void ATLASLabelOld(Double_t x,Double_t y,bool Preliminary,Color_t color) 
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.SetTextSize(.045);
  l.DrawLatex(x,y,"ATLAS");
  if (Preliminary) {
    TLatex p; 
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(color);
    p.SetTextSize(.045);
    p.DrawLatex(x+0.16,y,"Internal");
    //p.DrawLatex(x+0.15,y,"Preliminary");
  }
}

//
int main(int argc, char* argv[]){
  //
  // Argument variables
  //
  TString vname      = argv[1];
  
  int data = atoi(argv[2]);
  
  TString pname = argv[3]; 
  
  double xmass = atof(argv[4]);
  
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
  
  //double xlow=-2.5;
  //double xhig=2.5;
  double xlow=-1.5;
  double xhig=1.5;
  if (vname.Contains("ATLAS_norm_")) {
    xlow=0.5;
    xhig=1.5;
  };
  if (vname.Contains("SigXsecOverSM") )   {
    if (pname == "mu" ) {
      xlow = -1.5;
      xhig =  4.5;
    }  
    if (pname == "tautau11_bv") {
      xlow = -4;
      xhig =  11;
    }    
    if (pname == "tautau11_b") {
      xlow = -5;
      xhig =  25;
    }    
    if (pname == "tautau11_v") {
      xlow = -4;
      xhig =  4.5;
    }    
    if (pname == "tautau12_bv") {
      xlow =  -0.5;
      xhig =  6;
    }    
    if (pname == "tautau12_b") {
      xlow =  -6;
      xhig =   5;
    }    
    if (pname == "tautau12_v") {
      xlow =  -0.5;
      xhig =  8.5;
    }    
    if (pname == "tautau_bv") {
      xlow =  0;
      xhig =  4.5;
    }    
    if (pname == "tautau_b") {
      xlow =  -1;
      xhig =   8;
    }    
    if (pname == "tautau_v") {
      xlow =  0;
      xhig =  4;
    }
    if (pname == "lfv") {
      xlow =  -1.5;
      xhig =  1.5;
    } 
  }
  
  
  TString dname;
  if (data == 1) {
    dname="ObsData";
  } else if (data == 2) {
    dname="Asimov1";
  }
  cout << "data = " << data << " dname = " << dname << endl;
  
  bool first=true;
  
  int docat;
if (pname.BeginsWith("mu")) {
    docat = 1;
  }
if (pname.BeginsWith("comb")) {
    docat = 1;
  }
  if (pname.BeginsWith("tautau")) {
    docat = 4;
  }
  if (pname.BeginsWith("tt12")) {
    docat = 4;
  }
  if (pname == "tt12") {
    docat = 6;
  }
  if (pname == "lh12") {
    docat = 3;
  }
  if (pname == "hh12") {
    docat = 6;
  }
  if (pname == "ll12") {
    docat = 3;
  }
 if (pname == "ll11") {
    docat = 3;
  }
 if (pname == "ll") {
    docat = 9;
  }
 if (pname == "lh") {
    docat = 9;
  }
 if (pname == "tt") {
   docat = 3;//36;
  }
 if (pname == "lfv") {
   docat = 5; // lh_mtau_mva and lhinc_mtau_mva and lhvbf_mtau_mva
  }
  
  const int ncat = (const int)docat;
  
  vector<int> lcol;

 if (pname == "mu" || pname == "comb") {
    lcol.push_back(1);
 }

  if (pname.BeginsWith("tautau")) {
    lcol.push_back(1);
    lcol.push_back(2);
    lcol.push_back(4);
    lcol.push_back(6);
//    lcol.push_back(kGreen-5);
//    lcol.push_back(38);
//    lcol.push_back(46);
    //    lcol.push_back(kOrange+1);
  }


  if (pname.BeginsWith("tt12")) {
    lcol.push_back(1);
    lcol.push_back(2);
    lcol.push_back(4);
    lcol.push_back(6);
  }

  
  if (pname == "lh12") {
    lcol.push_back(2);
    lcol.push_back(4);
//    lcol.push_back(6);
//    lcol.push_back(kGreen-5);
//    lcol.push_back(38);
//    lcol.push_back(46);
    lcol.push_back(1);
    //    lcol.push_back(kOrange+1);
  }
  
  if (pname == "hh12") {
    lcol.push_back(2);
    lcol.push_back(4);
    lcol.push_back(6);
    lcol.push_back(kGreen-5);
    lcol.push_back(38);
    //   lcol.push_back(46);
    lcol.push_back(1);
  }
  
  if (pname == "ll12") {
    lcol.push_back(2);
    lcol.push_back(4);
    //    lcol.push_back(6);
    //    lcol.push_back(kGreen-5);
    //    lcol.push_back(38);
    //    lcol.push_back(46);
    lcol.push_back(1);

  }
  
  if (pname == "ll11") {
    lcol.push_back(2);
    lcol.push_back(4);
    //    lcol.push_back(6);
    //    lcol.push_back(kGreen-5);
    //    lcol.push_back(38);
    //    lcol.push_back(46);
    lcol.push_back(1);

  }

 if (pname == "ll") {
    lcol.push_back(2);
    lcol.push_back(4);
    lcol.push_back(6);
    lcol.push_back(kGreen-5);
    lcol.push_back(38);
    lcol.push_back(46);
    lcol.push_back(26);
    lcol.push_back(16);
    lcol.push_back(1);
  }

 if (pname == "lh") {
    lcol.push_back(2);
    lcol.push_back(4);
    lcol.push_back(6);
    lcol.push_back(kGreen-5);
    lcol.push_back(38);
    lcol.push_back(46);
    lcol.push_back(26);
    lcol.push_back(16);
    lcol.push_back(1);
  }

 
 if (pname == "tt") {
   lcol.push_back(2);
   lcol.push_back(4);
   lcol.push_back(1);
//   lcol.push_back(36);
//   lcol.push_back(35);
//   lcol.push_back(34);
//   lcol.push_back(33);
//   lcol.push_back(32);
//   lcol.push_back(31);
//   lcol.push_back(30);
//   lcol.push_back(29);
//   lcol.push_back(28);
//   lcol.push_back(27);
//   lcol.push_back(26);
//   lcol.push_back(25);
//   lcol.push_back(24);
//   lcol.push_back(23);
//   lcol.push_back(22);
//   lcol.push_back(21);
//   lcol.push_back(20);
//   lcol.push_back(19);
//   lcol.push_back(18);
//   lcol.push_back(17);
//   lcol.push_back(16);
//   lcol.push_back(15);
//   lcol.push_back(14);
//   lcol.push_back(13);
//   lcol.push_back(12);
//   lcol.push_back(11);
//   lcol.push_back(10);
   //   lcol.push_back(3);
//   lcol.push_back(5);
//   lcol.push_back(6);
//   lcol.push_back(7);
//   lcol.push_back(8);
//   lcol.push_back(9);

 }
 if (pname == "lfv") {
   lcol.push_back(6);
   lcol.push_back(4);
   lcol.push_back(2);
   lcol.push_back(1);
   lcol.push_back(8);
 }
  
  //int lcol[ncat] = { 2, 4, 6, 38, 1};//, 2, 6};
  //  int lcol[ncat] = { 2, 4, 6, 7, kGreen-5, 30, 38, kOrange+1, 46, 1};//, 2, 6};
  
  
  
  TCanvas* c1 = new TCanvas("c1","c1",800,800);
  c1->cd();
  TGraph*graph[ncat];
  
  double val[ncat];
  double hig[ncat];
  double low[ncat];
  
  int ncat_do_plot=0;
  
  for (int icat=0; icat < ncat; ++icat) {
    graph[icat] = 0;
    TString scat;
    
    if (pname == "mu" || pname == "comb" ) {
      if (icat==0) {
	scat="tt12_bv";
      }
    }

    if (pname == "tautau12_bv" || pname == "tt12_bv") {
      if (icat==1) {
	scat="ll12_bv";
      } else if (icat==2) {
	scat="lh12_bv";
      } else if (icat==3) {
	scat="hh12_bv";
      } else if (icat==0) {
	scat="tt12_bv";
      }
    }
    if (pname == "tautau12_b" || pname == "tt12_b") {
      if (icat==1) {
	scat="ll12_b";
      } else if (icat==2) {
	scat="lh12_b";
      } else if (icat==3) {
	scat="hh12_b";
      } else if (icat==0) {
	scat="tt12_b";
      }
    }
    if (pname == "tautau12_v" || pname == "tt12_v") {
      if (icat==1) {
	scat="ll12_v";
      } else if (icat==2) {
	scat="lh12_v";
      } else if (icat==3) {
	scat="hh12_v";
      } else if (icat==0) {
	scat="tt12_v";
      }
    }

    if (pname == "tautau11_bv") {
      if (icat==1) {
	scat="ll11_bv";
      } else if (icat==2) {
	scat="lh11_bv";
      } else if (icat==3) {
	scat="hh11_bv_rCR";
      } else if (icat==0) {
	scat="tt11_bv_rCR";
      }
    }
    if (pname == "tautau11_b") {
      if (icat==1) {
	scat="ll11_b";
      } else if (icat==2) {
	scat="lh11_b";
      } else if (icat==3) {
	scat="hh11_b_rCR";
      } else if (icat==0) {
	scat="tt11_b_rCR";
      }
    }
    if (pname == "tautau11_v") {
      if (icat==1) {
	scat="ll11_v";
      } else if (icat==2) {
	scat="lh11_v";
      } else if (icat==3) {
	scat="hh11_v_rCR";
      } else if (icat==0) {
	scat="tt11_v_rCR";
      }
    }

    if (pname == "tautau_bv") {
      if (icat==1) {
	scat="ll_bv";
      } else if (icat==2) {
	scat="lh_bv";
      } else if (icat==3) {
	scat="hh_bv_rCR";
      } else if (icat==0) {
	scat="tt_bv_rCR";
      }
    }
    if (pname == "tautau_b") {
      if (icat==1) {
	scat="ll_b";
      } else if (icat==2) {
	scat="lh_b";
      } else if (icat==3) {
	scat="hh_b_rCR";
      } else if (icat==0) {
	scat="tt_b_rCR";
      }
    }
    if (pname == "tautau_v") {
      if (icat==1) {
	scat="ll_v";
      } else if (icat==2) {
	scat="lh_v";
      } else if (icat==3) {
	scat="hh_v_rCR";
      } else if (icat==0) {
	scat="tt_v_rCR";
      }
    }

    if (pname == "tt12") {
      if (icat==0) {
	scat="tt12_b";
      } else if (icat==1) {
	scat="tt12_v";
      } else if (icat==2) {
	scat="tt12_bv";
      } else if (icat==3) {
	scat="tt12_b_rCR";
      } else if (icat==4) {
	scat="tt12_v_rCR";
      } else if (icat==5) {
	scat="tt12_bv_rCR";
      }
    }
    
    if (pname == "lh12") {
      if (icat==0) {
	scat="lh12_b";
      } else if (icat==1) {
	scat="lh12_v";
      } else if (icat==2) {
	scat="lh12_bv";
      }
    }
    
    if (pname == "hh12") {
      if (icat==0) {
	scat="hh12_b";
      } else if (icat==1) {
	scat="hh12_v";
      } else if (icat==2) {
	scat="hh12_bv";
      } else if (icat==3) {
	scat="hh12_b_rCR";
      } else if (icat==4) {
	scat="hh12_v_rCR";
      } else if (icat==5) {
	scat="hh12_bv_rCR";
      }
    }
    
    if (pname == "ll12") {
      if (icat==0) {
	scat="ll12_b";
      } else if (icat==1) {
	scat="ll12_v";
      } else if (icat==2) {
	scat="ll12_bv";
      }
    }
    
    if (pname == "ll11") {
      if (icat==0) {
	scat="ll11_b";
      } else if (icat==1) {
	scat="ll11_v";
      } else if (icat==2) {
	scat="ll11_bv";
      }
    }

    if (pname == "ll") {
      if (icat==0) {
	scat="ll11_b";
      } else if (icat==1) {
	scat="ll11_v";
      } else if (icat==2) {
	scat="ll11_bv";
      } else if (icat==3) {
	scat="ll12_b";
      } else if (icat==4) {
	scat="ll12_v";
      } else if (icat==5) {
	scat="ll12_bv";
      } else if (icat==6) {
	scat="ll_b";
      } else if (icat==7) {
	scat="ll_v";
      } else if (icat==8) {
	scat="ll_bv";
      }
    }

    if (pname == "lh") {
      if (icat==0) {
	scat="lh11_b";
      } else if (icat==1) {
	scat="lh11_v";
      } else if (icat==2) {
	scat="lh11_bv";
      } else if (icat==3) {
	scat="lh12_b";
      } else if (icat==4) {
	scat="lh12_v";
      } else if (icat==5) {
	scat="lh12_bv";
      } else if (icat==6) {
	scat="lh_b";
      } else if (icat==7) {
	scat="lh_v";
      } else if (icat==8) {
	scat="lh_bv";
      }
    }
          
    if (pname=="tt") {
      if (icat==0) {
	scat="tt_b_rCR";
      } else if (icat==1) {
	scat="tt_v_rCR";
      } else if (icat==2) {
	scat="tt_bv_rCR";
      }
    }
    //
    if (pname=="tt_all") {
      if (icat==0) {
	scat="ll11_b";
      } else if (icat==1) {
	scat="ll11_v";
      } else if (icat==2) {
	scat="ll11_bv";
      } else if (icat==3) {
	scat="ll12_b";
      } else if (icat==4) {
	scat="ll12_v";
      } else if (icat==5) {
	scat="ll12_bv";
      } else if (icat==6) {
	scat="ll_b";
      } else if (icat==7) {
	scat="ll_v";
      } else if (icat==8) {
	scat="ll_bv";
      } else if (icat==9) {
	scat="lh11_b";
      } else if (icat==10) {
	scat="lh11_v";
      } else if (icat==11) {
	scat="lh11_bv";
      } else if (icat==12) {
	scat="lh12_b";
      } else if (icat==13) {
	scat="lh12_v";
      } else if (icat==14) {
	scat="lh12_bv";
      } else if (icat==15) {
	scat="lh_b";
      } else if (icat==16) {
	scat="lh_v";
      } else if (icat==17) {
	scat="lh_bv";
      } else if (icat==18) {
	scat="hh11_b_rCR";
      } else if (icat==19) {
	scat="hh11_v_rCR";
      } else if (icat==20) {
	scat="hh11_bv_rCR";
      } else if (icat==21) {
	scat="hh12_b_rCR";
      } else if (icat==22) {
	scat="hh12_v_rCR";
      } else if (icat==23) {
	scat="hh12_bv_rCR";
      } else if (icat==24) {
	scat="hh_b_rCR";
      } else if (icat==25) {
	scat="hh_v_rCR";
      } else if (icat==26) {
	scat="hh_bv_rCR";
      } else if (icat==27) {
	scat="tt11_b_rCR";
      } else if (icat==28) {
	scat="tt11_v_rCR";
      } else if (icat==29) {
	scat="tt11_bv_rCR";
      } else if (icat==30) {
	scat="tt12_b_rCR";
      } else if (icat==31) {
	scat="tt12_v_rCR";
      } else if (icat==32) {
	scat="tt12_bv_rCR";
      } else if (icat==33) {
	scat="tt_b_rCR";
      } else if (icat==34) {
	scat="tt_v_rCR";
      } else if (icat==35) {
	scat="tt_bv_rCR";
      }
    }
    if (pname == "lfv") {
      if (icat==0) {
	scat="lh_etau_mva";
      } else if (icat==1) {
	scat="lhinc_etau_mva";
      } else if (icat==2) {
	scat="lhvbf_etau_mva";
      } else if (icat==3) {
	scat="lhvbf_etau_mva_decorr";
      } else if (icat==4) {
	scat="lh_etau_mva_decorr";
      }
    }
    //
    // This file is opened in order to read in the values to be printed using TLegend
    // The same information exits in each of the root files, so just pick any one to read
    //
    // Changed to read from submit_pulls output
    //
    //    TString fname0=Form("root-files/%s_%3.0f_%d_pulls/%s.root",scat.Data(),xmass,data,vname.Data());
    TString fname0;
    //    if (pname == "mu" || pname == "comb" ) {
    fname0 = Form("root-files/%s_scans/%s_-1.5_1.5.root",scat.Data(),vname.Data()); 
      //    } else {
      //      fname0=Form("root-files/%s_%3.0f_%d_scans/%s_-2.1_2.1.root",scat.Data(),xmass,data,vname.Data());
      //    }

    if (gSystem->AccessPathName(fname0)) {
      cout << "Filename: " << fname0 << " doesn't exist!" << endl;
      continue;
    } else {
      cout << "Opening : " << fname0 << endl;
      ++ncat_do_plot;
    };
    
    TFile*file0=TFile::Open(fname0);
    if (!file0) {
      cout << "Could not open : " << fname0 << endl;
      continue;
    } 
    
    TString hname0=vname;
    
    TH1D*hist=(TH1D*)file0->Get(hname0);
    if (pname == "lfv" ) {
      val[icat] = hist->GetBinContent(1);
      hig[icat] = hist->GetBinContent(2);
      low[icat] = hist->GetBinContent(3);
    } else if (pname == "mu" || pname == "comb" ) {
      val[icat] = hist->GetBinContent(1)/12.;
      hig[icat] = hist->GetBinContent(2)/12.;
      low[icat] = hist->GetBinContent(3)/12.;
    } else {
      val[icat] = hist->GetBinContent(1)/8.;
      hig[icat] = hist->GetBinContent(2)/8.;
      low[icat] = hist->GetBinContent(3)/8.;
    }
    
    file0->Close();
    
    // 
    // This file must be the output of the hadd command, because it needs to full range for plotting
    //

    TString fname;
    //    if (pname == "mu" || pname == "comb") {
      fname = Form("root-files/%s_scans/%s_-1.5_1.5.root",scat.Data(),vname.Data());
      //    } else {
      //      fname=Form("root-files/%s_%3.0f_%d_scans/%s_-2.1_2.1.root",scat.Data(),xmass,data,vname.Data());
      //    }
    
    TFile*file=TFile::Open(fname);
    
    graph[icat]=(TGraph*)file->Get("Graph");
    graph[icat]->SetName(Form("%s",scat.Data()));
    
    file->Close();
    
    cout << "BEFORE " << endl;
     graph[icat]->Print();
     cout << "=================" << endl;
    Int_t	nval = graph[icat]->GetN();
    Double_t*	xval = graph[icat]->GetX();
    Double_t*	yval = graph[icat]->GetY();
    
    vector <point> vpoint;
    
       cout << "nval = " << nval << endl;
    for (int i = 0; i < nval; ++i) {
cout << "i = " << i << " x = " << xval[i] << " y = " << yval[i] << endl;
      
      point p;
      p.i = i;
      p.x = xval[i];
      p.y = yval[i];
      
      vpoint.push_back(p);
    }
    
    sort (vpoint.begin(), vpoint.end());
    
    graph[icat]->SetEditable( kTRUE);
    int newn = 0;
    double x_old = -999;
    for (int i = 0; i < nval; ++i) {
      
      double x = vpoint[i].x;
      
      double y = vpoint[i].y;
      
      if (fabs(x - x_old) > .001) {
	
	if (y<4) { 
	  ++newn;
	  graph[icat]->SetPoint(newn-1, x, y);
	}
      }
      
      x_old = x;
    }

    graph[icat]->Set(newn);
    cout << "newn = " << newn << endl;
    cout << "AFTER " << endl;
    graph[icat]->Print();
    cout << "=================" << endl;

    graph[icat]->SetLineStyle(1);
    
    graph[icat]->SetLineColor(lcol[icat]);
    
    graph[icat]->SetMarkerSize(0);
    
    graph[icat]->SetLineWidth(2);
    if (first) {
      first=false;
      TString xtitle=vname;
      xtitle.ReplaceAll("alpha_","");
      xtitle.ReplaceAll("ATLAS_","");
      if (vname == "SigXsecOverSM") xtitle = "#font[42]{Signal Strength (#mu)}";//#sigma/#sigma_{SM}";
      graph[icat]->GetXaxis()->SetTitle(xtitle);
      graph[icat]->GetXaxis()->SetTitleSize(.05);
      graph[icat]->GetYaxis()->SetTitleSize(.05);
      graph[icat]->GetXaxis()->SetTitleOffset(1.1);
      graph[icat]->GetYaxis()->SetTitleOffset(1.2);
      graph[icat]->GetXaxis()->SetLimits(xlow,xhig);
      graph[icat]->GetYaxis()->SetTitle("#font[42]{-2 #Delta ln(L)}");
      graph[icat]->SetMaximum(5);
      graph[icat]->SetMinimum(-1);
      if (vname == "SigXsecOverSM") graph[icat]->SetMinimum(0);
      if (pname == "comb") graph[icat]->SetMinimum(0);
      graph[icat]->Print("all");
      graph[icat]->Draw("al");
    } else {
      graph[icat]->Draw("l");
    }
  }
  
  TLine l;
  l.SetLineStyle(7);
  l.SetLineColor(36);
  l.SetLineWidth(2);
  l.DrawLine(xlow, 0, xhig, 0);
  l.DrawLine(xlow, 1, xhig, 1);
  l.DrawLine(xlow, 4, xhig, 4);

  TLine lone;
  lone.SetLineStyle(7);
  lone.SetLineColor(36);
  lone.SetLineWidth(2);
  if (vname.Contains("norm_"))  lone.DrawLine(1, 0, 1, 4);

  TLine lzero;
  lzero.SetLineStyle(7);
  lzero.SetLineColor(36);
  lzero.SetLineWidth(2);
  if (!(vname.Contains("norm_")))  lzero.DrawLine(0, 0, 0, 4);
  
  double x1leg,x2leg,y1leg,y2leg;
  
  x1leg=.16;
  x2leg=.93;
  y1leg=.85;
  y2leg=.95;
  y1leg=.15;
  y2leg=.30;
  
  if (pname == "mu" || pname == "comb" ) {
    y1leg = 0.78;
    y2leg = 0.88;
  }

  TString leg_tit;
  
//  if (pname == "embchi80sths1crand") leg_tit = "#scale[1.2]{embchi80sths1crand}";
//  if (pname == "embchi90sths1crand") leg_tit = "#scale[1.2]{embchi90sths1crand}";
//  if (pname == "embsmoothcrand")     leg_tit = "#scale[1.2]{HSG5 (3TES: FAKE, SS, TRUE)}";
//  if (pname == "embp02sofiacrand")   leg_tit = "#scale[1.2]{0.2 (3TES: FAKE, SS, TRUE)}";
//  if (pname == "embsofiacrand")      leg_tit = "#scale[1.2]{0.4 (3TES: FAKE, SS, TRUE)}";
//  if (pname == "embp06sofiacrand")   leg_tit = "#scale[1.2]{0.6 (3TES: FAKE, SS, TRUE)}";
//  if (pname == "embp08sofiacrand")   leg_tit = "#scale[1.2]{0.8 (3TES: FAKE, SS, TRUE)}";
//  if (pname == "embp10sofiacrand")   leg_tit = "#scale[1.2]{1.0 (3TES: FAKE, SS, TRUE)}";
//  if (pname == "embp12sofiacrand")   leg_tit = "#scale[1.2]{1.2 (3TES: FAKE, SS, TRUE)}";
//  if (pname == "embp15sofiacrand")   leg_tit = "#scale[1.2]{1.5 (3TES: FAKE, SS, TRUE)}";
  
  leg_tit = " ";
  
  
  TLegend *leg = new TLegend (x1leg,y1leg,x2leg,y2leg,leg_tit,"brNDC");
  
  //  if (ncat_do_plot <= 5) {
  //    leg->SetNColumns(ncat_do_plot);
  //  } else {
  if (pname.BeginsWith("tautau")) {
    leg->SetNColumns(4);
  } else if (pname == "tt") {
    leg->SetNColumns(3);
  } else if (pname == "lh") {
    leg->SetNColumns(3);
  } else  if (pname == "mu" || pname == "comb" ) {
    leg->SetNColumns(1);
  } else {
    leg->SetNColumns(3);
  }
  //  }
  
  for (int icat=0; icat < ncat; ++icat) {
    
    TString scat;

    if (pname == "mu" || pname == "comb" ) {
      scat=Form("H #rightarrow #tau#tau (%3.0f GeV)", xmass); 
    }

    if (pname.BeginsWith("tautau") || pname.BeginsWith("tt12")) {
      if (icat==1) {
	scat="H #rightarrow #tau_{lep} #tau_{lep}";// "ll (bv)";
      } else if (icat==2) {
	scat="H #rightarrow #tau_{lep} #tau_{had}"; // "lh (bv)";
      } else if (icat==3) {
	scat="H #rightarrow #tau_{had} #tau_{had}"; // "hh (bv_rCR)";
      } else if (icat==0) {
	scat="H #rightarrow #tau#tau" ; // "tt (bv_rCR)";
      }
    }
    if (pname == "lfv") {
      if (icat==0) {
	scat="H #rightarrow e#tau(comb)"; // H->mutau comb
      } else if (icat ==1) {
	scat="H #rightarrow e#tau(Non-vbf)"; // H->mutau inc
      } else if (icat ==2) {
	scat="H #rightarrow e#tau(vbf)"; // H->mutau  vbf
       } else if (icat ==3) {
	scat="H #rightarrow e#tau(vbf decorr)"; // H->mutau  vbf
       } else if (icat ==4) {
	scat="H #rightarrow e#tau(comb decorr)"; // H->mutau  vbf
       }
    }
    
    
    if (pname == "lh12") {
      if (icat==0) {
	scat="b";
      } else if (icat==1) {
	scat="v";
      } else if (icat==2) {
	scat="bv";
      }
    }

    if (pname == "hh12") {
      if (icat==0) {
	scat="b";
      } else if (icat==1) {
	scat="v";
      } else if (icat==2) {
	scat="bv";
      } else if (icat==3) {
	scat="b_rCR";
      } else if (icat==4) {
	scat="v_rCR";
      } else if (icat==5) {
	scat="bv_rCR";
      }
    }
    
    if (pname == "ll12") {
      if (icat==0) {
	scat="b";
      } else if (icat==1) {
	scat="v";
      } else if (icat==2) {
	scat="bv";
      }
    }

    if (pname == "ll11") {
      if (icat==0) {
	scat="b";
      } else if (icat==1) {
	scat="v";
      } else if (icat==2) {
	scat="bv";
      }
    }

    if (pname == "ll") {
      if (icat==0) {
	scat="b (7 TeV)";
      } else if (icat==1) {
	scat="v (7 TeV)"; 
      } else if (icat==2) {
	scat="bv (7 TeV)";
      } else if (icat==3) {
	scat="b (8 TeV)";
      } else if (icat==4) {
	scat="v (8 TeV)";
      } else if (icat==5) {
	scat="bv (8 TeV)";
      } else if (icat==6) {
	scat="b (7+8 TeV)";
      } else if (icat==7) {
	scat="v (7+8 TeV)";
      } else if (icat==8) {
	scat="bv (7+8 TeV)";
      }
    }

    if (pname == "lh") {
      if (icat==0) {
	scat="b (7 TeV)";
      } else if (icat==1) {
	scat="v (7 TeV)"; 
      } else if (icat==2) {
	scat="bv (7 TeV)";
      } else if (icat==3) {
	scat="b (8 TeV)";
      } else if (icat==4) {
	scat="v (8 TeV)";
      } else if (icat==5) {
	scat="bv (8 TeV)";
      } else if (icat==6) {
	scat="b (7+8 TeV)";
      } else if (icat==7) {
	scat="v (7+8 TeV)";
      } else if (icat==8) {
	scat="bv (7+8 TeV)";
      }
    }


    if (pname == "tt") {
      if (icat==0) {
	scat="tt_b";
      } else if (icat==1) {
	scat="tt_v";
      } else if (icat==2) {
	scat="tt_bv";
      }
    }

    if (graph[icat]) { 
      // cout << "Adding entry for " << scat << endl;
      if (pname == "comb") {
	TString stemp = vname;
	stemp.ReplaceAll("alpha_","");
	stemp.ReplaceAll("ATLAS_","");	
	leg->AddEntry((TObject*)0,Form("#color[%d]{%s #left(%s = %5.2f #splitline{+%5.2f}{-%5.2f}#right)}",lcol[icat],scat.Data(),stemp.Data(), val[icat],hig[icat],low[icat]),"");
      } else {
	leg->AddEntry((TObject*)0,Form("#color[%d]{%s #left(%5.2f #splitline{+%5.2f}{-%5.2f}#right)}",lcol[icat],scat.Data(),val[icat],hig[icat],low[icat]),"");
      }
    } else {
      cout << "Skipping " << scat << endl;
    }

     cout << scat.Data() << vname.Data() << Form("#left(%5.2f #splitline{+%5.2f}{-%5.2f}#right)",val[icat],hig[icat],low[icat]) << endl;
    
    if (leg) { 
      //          cout << "Legend has " << 	leg->GetNRows() << " rows  and " << leg->GetNColumns() << " columns " << endl;
      
      leg->SetLineColor(0);
      leg->SetFillColor(0);
      leg->SetFillStyle(0);
      leg->SetBorderSize(0);
      leg->SetTextFont(62);

      if (leg->GetNColumns() == 4) {
	if (pname.BeginsWith("tautau") || pname.BeginsWith("tt12")) {
	  leg->SetTextSize(0.0275); 
	}
      } else {
	if (pname == "tt") {
	  leg->SetTextSize(0.02);
	}
	if (pname == "lfv") {
	  leg->SetTextSize(0.02); // To write text inside " " 
	}
	if (pname == "lh") {
	  leg->SetTextSize(0.02);
	}
      }
      leg->SetMargin(.01);
      //if (leg->GetNColumns() == 5) {
      //	leg->SetColumnSeparation(0.125);
      //      } else {
      leg->SetColumnSeparation(0.1);
      //      }
      leg->SetEntrySeparation(1);
      //      cout << "Drawing legend " << endl;
      if (pname.BeginsWith("tautau")) {
      } else if (pname == "mu" || pname == "comb" ) {
	//	leg->Print("all");
	leg->SetTextSize(.025) ;
	leg->Draw("same");  
      } else { 
	leg->Draw("same"); } 
      //      cout << "Drawn legend " << endl;
      //      leg->Print();
    }
  }
  
  ATLASLabelOld(0.18,.91,true,1);

  TLatex l1;
  l1.SetNDC();
  l1.SetTextColor(1);
  l1.SetTextFont(62);
  l1.SetTextSize(0.036);
  if (pname.Contains("11")) {
    l1.DrawLatex(0.18, 0.85,"#scale[0.4]{#lower[-.2]{#int}} Ldt = 4.5 fb^{-1}, #sqrt{s} = 7 TeV");
  } else if (pname.Contains("12") || pname == "mu" || pname == "comb" ) {
    l1.DrawLatex(0.18, 0.85,"#scale[0.4]{#lower[-.2]{#int}} Ldt = 20.3 fb^{-1}, #sqrt{s} = 8 TeV");
  } else if (pname == "lfv") {
    l1.DrawLatex(0.18, 0.87,"#scale[0.4]{#lower[-.2]{#int}} Ldt = 36 fb^{-1}, #sqrt{s} = 13 TeV");
  } else if (pname != "mu") {
    l1.DrawLatex(0.18, 0.87,"#scale[0.4]{#lower[-.2]{#int}} Ldt = 4.5 fb^{-1}, #sqrt{s} = 7 TeV");
    l1.DrawLatex(0.18, 0.83,"#scale[0.4]{#lower[-.2]{#int}} Ldt = 20.3 fb^{-1}, #sqrt{s} = 8 TeV");
  }
  //  l1.DrawLatex(0.18, 0.80, "H #rightarrow #tau #tau");

  
  TLegend* leg2 ;
  if (pname == "mu") {
    leg2 = new TLegend(0.51,0.81,.95,0.95,"","brNDC");
  } else if (pname.BeginsWith("tautau") || pname.BeginsWith("tt12") ) {
    leg2 = new TLegend(0.61,0.81,.95,0.95,"","brNDC");
  } else {
    leg2 = new TLegend(0.6,0.81,.90,0.95,"","brNDC");
  }
  leg2->SetLineColor(0);
  leg2->SetFillColor(0);
  leg2->SetFillStyle(0);
  leg2->SetBorderSize(0);
  //  leg2->SetTextFont(42);

  leg2->SetLineColor(0);
  leg2->SetFillColor(0);
   leg2->SetTextSize(.03);
  //  leg2->SetMargin(.15);
  leg2->SetTextAlign(12);
  //
  //  if (pname == "mu") {
  //    if (graph[0]) leg2->AddEntry((TObject*)0, Form("H #rightarrow #tau#tau (%3.0f GeV)",xmass),"");
  //    leg2->Draw("same");
  //  }

  if (pname.BeginsWith("tautau") || pname.BeginsWith("tt12")) {
    if (pname.Contains("_b") && !pname.Contains("_bv")) {
      if (graph[0]) leg2->AddEntry(graph[0], "H #rightarrow #tau#tau (b)","l");
      if (graph[1]) leg2->AddEntry(graph[1], "H #rightarrow #tau_{lep}#tau_{lep} (b)","lf");
      if (graph[2]) leg2->AddEntry(graph[2], "H #rightarrow #tau_{lep}#tau_{had} (b)","lf");
      if (graph[3]) leg2->AddEntry(graph[3], "H #rightarrow #tau_{had}#tau_{had} (b)","lf");
    } else if (pname.Contains("_v"))  {
      if (graph[0]) leg2->AddEntry(graph[0], "H #rightarrow #tau#tau (v)","l");
      if (graph[1]) leg2->AddEntry(graph[1], "H #rightarrow #tau_{lep}#tau_{lep} (v)","lf");
      if (graph[2]) leg2->AddEntry(graph[2], "H #rightarrow #tau_{lep}#tau_{had} (v)","lf");
      if (graph[3]) leg2->AddEntry(graph[3], "H #rightarrow #tau_{had}#tau_{had} (v)","lf");
    } else {
      if (graph[0]) leg2->AddEntry(graph[0], "H #rightarrow #tau#tau","l");
      if (graph[1]) leg2->AddEntry(graph[1], "H #rightarrow #tau_{lep}#tau_{lep}","lf");
      if (graph[2]) leg2->AddEntry(graph[2], "H #rightarrow #tau_{lep}#tau_{had}","lf");
      if (graph[3]) leg2->AddEntry(graph[3], "H #rightarrow #tau_{had}#tau_{had}","lf");
    }
    leg2->Draw("same");
  }

  if (pname == "ll") {
    leg2->AddEntry(graph[0], "H #rightarrow #tau_{lep}#tau_{lep}","lf");
    leg2->Draw("same");
  }

  if (pname == "lh") {
    leg2->AddEntry(graph[8], "H #rightarrow #tau_{lep}#tau_{had}","lf");
    leg2->Draw("same");
  }

  if (pname == "tt") {
    if (graph[0]) leg2->AddEntry(graph[0], "H #rightarrow #tau#tau (Boost)","l");
    if (graph[1]) leg2->AddEntry(graph[1], "H #rightarrow #tau#tau (VBF)","l");
    if (graph[2]) leg2->AddEntry(graph[2], "H #rightarrow #tau#tau","l");
    //    leg2->AddEntry((TObject*)0, "",0);
    leg2->Draw("same");
  }
  if (pname == "lfv") {
    if (graph[0]) leg2->AddEntry(graph[0], "H #rightarrow e#tau(comb)","l");
    if (graph[1]) leg2->AddEntry(graph[1], "H #rightarrow e#tau(Non-vbf)","l");
    if (graph[2]) leg2->AddEntry(graph[2], "H #rightarrow e#tau(vbf)","l");
    if (graph[3]) leg2->AddEntry(graph[3], "H #rightarrow e#tau(vbf decorr)","l");
    if (graph[4]) leg2->AddEntry(graph[4], "H #rightarrow e#tau(comb decorr)","l");
    leg2->Draw("same");
  }

  gPad->Update();
  system("mkdir -vp NPscans_figures_etau");
  
  cout << "Making figures for vname = " << vname << " dname = " << dname << endl;
  gPad->SaveAs(Form("NPscans_figures_etau/%s_%s_%s_%3.0f.eps",vname.Data(),dname.Data(),pname.Data(),xmass));
  gPad->SaveAs(Form("NPscans_figures_etau/%s_%s_%s_%3.0f.png",vname.Data(),dname.Data(),pname.Data(),xmass));
  gPad->SaveAs(Form("NPscans_figures_etau/%s_%s_%s_%3.0f.pdf",vname.Data(),dname.Data(),pname.Data(),xmass));
  
  cout << "Made figures" << endl;
  
  return 0;
  
}
