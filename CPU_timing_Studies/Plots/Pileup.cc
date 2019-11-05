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
#include "TLegend.h"
#include "TLatex.h"
using namespace std;
void ATLASLabelOld(Double_t x,Double_t y,bool Preliminary,Color_t color)
{
  TLatex l; //l.SetTextAlign(12); l.SetTextSize(tsize);
  l.SetNDC();
  l.SetTextFont(72);
  l.SetTextColor(color);
  l.SetTextSize(.04);
  l.DrawLatex(x,y,"ATLAS Simulation");
  if (Preliminary) {
    TLatex p;
    p.SetNDC();
    p.SetTextFont(42);
    p.SetTextColor(color);
    p.SetTextSize(.04);
    //p.DrawLatex(x+0.16,y,"Internal");
    p.DrawLatex(x+0.34,y,"Preliminary");
  }
}
void SetUp(){
  cout << "SetUp Plain " << endl;
  gROOT->SetStyle("Plain");
  //  gROOT->SetBatch(kTRUE);
  gROOT->ForceStyle();
  gStyle->SetStatW(0.3);
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
  gStyle->SetTitleOffset(1.4,"Y");
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
  gStyle->SetPadLeftMargin(0.125);
  gStyle->SetPadRightMargin(0.025);
  gStyle->SetTickLength(.02,"X");
  gStyle->SetTickLength(.02,"Y");
  //
  if (!gROOT->GetClass("TMultiLayerPerceptron")) {
    gSystem->Load("libMLP");
    gSystem->Load("libTreePlayer");
  }
  return;
}
int main() {
  SetUp();

  const Int_t n = 4;
  const Int_t m = 1;
  TMultiGraph* mg = new TMultiGraph();
  TLegend *leg  = new TLegend(0.5,0.121, 0.97, 0.46, "","brNDC");
  //  TLegend *leg2 = new TLegend(0.35,0.20, 0.65, 0.3, "","brNDC");
  //
  leg->SetTextSize(.025);
  leg->SetFillColor(0);
  //
  //  leg2->SetTextSize(.03);
  //  leg2->SetFillColor(0);
  //


  TGraphErrors*gr0;
  TGraphErrors*gr1;
  TGraphErrors*gr2;
  TGraphErrors*gr3;
  TGraphErrors*gr4;
  TGraphErrors*gr5;
  TGraphErrors*gr6;
  TGraphErrors*gr7;
  TGraphErrors*gr8;
  TGraphErrors*gre0;
  TGraphErrors*gre1;
  TGraphErrors*gre2;
  TGraphErrors*gre3;
  TGraphErrors*gre4;
  TGraphErrors*gre5;
  TGraphErrors*gre6;
  TGraphErrors*gre7;
  TGraphErrors*gre8;
  TGraphErrors*gre9;
  TGraphErrors*gre10;
  for (int irun = 0; irun < 2; ++irun){
    for (int ialg = 0; ialg < 11; ++ialg) {
      string salg = " ";
      if(irun==0){
	if (ialg==0) {
	  salg="AthAlgSeq";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] = { 8.68358*3.5,    35.4229*3.5,    108.486*3.5,  160.891*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n] = { 0.170727*3.5, 0.507139*3.5, 1.46496*3.5, 2.26446*3.5 };
 	  gr0 = new TGraphErrors(n,x,y,ex,ey);
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr0->SetMarkerColor(kBlack);
	  gr0->SetMarkerStyle(20);
	  gr0->SetMarkerSize(1);
	  gr0->SetLineColor(kBlack);
	  //mg->Add(gr0);
	}

	if (ialg==1) {
	  salg="SiPreProcessing";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] = { 0.131783*3.5,     1.94396*3.5,      4.74938*3.5,   7.30809*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n] = { 0.00180414*3.5, 0.0259664*3.5, 0.0766742*3.5, 0.100666*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr1 = new TGraphErrors(n,x,y,ex,ey);
	  gr1->SetMarkerColor(kBlue);
	  gr1->SetMarkerStyle(20);
	  gr1->SetMarkerSize(1);
	  gr1->SetLineColor(kBlue);
	  mg->Add(gr1);
	}

	if (ialg==2){
	  salg="InDetSiTrackerSpacePointFinder";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] = { 0.0450659*3.5,    1.01494*3.5,   3.55878*3.5,     6.90194*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n] = { 0.00092511*3.5, 0.023135*3.5, 0.111648*3.5, 0.159068*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr2 = new TGraphErrors(n,x,y,ex,ey);
	  gr2->SetMarkerColor(kRed);
	  gr2->SetMarkerStyle(20);
	  gr2->SetMarkerSize(1);
	  gr2->SetLineColor(kRed);
	  mg->Add(gr2);
	}
	
	if (ialg==3) {
	  salg="InDetSiSpTrackFinderSLHC";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] =  { 0.194764*3.5,   3.18541*3.5,   15.1203*3.5,   35.5557*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n]= { 0.00249833*3.5, 0.0363368*3.5, 0.156259*3.5, 0.4381*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr3 = new TGraphErrors(n,x,y,ex,ey);
	  gr3->SetMarkerColor(kMagenta);
	  gr3->SetMarkerStyle(20);
	  gr3->SetMarkerSize(1);
	  gr3->SetLineColor(kMagenta);
	  mg->Add(gr3);
	}

	if (ialg==4) {
	  salg="InDetAmbiguitySolverSLHC";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] =  { 0.674251*3.5,   7.54324*3.5,   18.0383*3.5,   27.2863*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n]= { 0.00670071*3.5, 0.0700017*3.5, 0.188901*3.5, 0.295827*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr4 = new TGraphErrors(n,x,y,ex,ey);
	  gr4->SetMarkerColor(kGreen+3);
	  gr4->SetMarkerStyle(20);
	  gr4->SetMarkerSize(1);
	  gr4->SetLineColor(kGreen+3);
	  mg->Add(gr4);
	}

	if (ialg==5) {
	  salg="InDetPriVxFinder";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] =  { 0.11901*3.5,   0.498734*3.5,   1.23415*3.5,   1.7386*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n]= {0.00347295*3.5, 0.00791181*3.5, 0.0237576*3.5, 0.0353506*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr5 = new TGraphErrors(n,x,y,ex,ey);
	  gr5->SetMarkerColor(kCyan);
	  gr5->SetMarkerStyle(20);
	  gr5->SetMarkerSize(1);
	  gr5->SetLineColor(kCyan);
	  mg->Add(gr5);
	}


	if (ialg==6) {
	  salg="ITkgenonly";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] =  { 0.0720683*3.5,   0.520818*3.5,   1.25303*3.5,   1.84965*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n]= { 0.00085698*3.5, 0.00521074*3.5, 0.01502*3.5, 0.0224507*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr6 = new TGraphErrors(n,x,y,ex,ey);
	  gr6->SetMarkerColor(kBlack );
	  gr6->SetMarkerStyle(20);
	  gr6->SetMarkerSize(1);
	  gr6->SetLineColor(kBlack );
	  mg->Add(gr6);
	}

	if (ialg==7) {
	  salg="MuonCombinedInDetCandidateAlg";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] =  { 0.00299197*3.5,   0.0259237*3.5,   0.0583009*3.5,   0.0838309*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n]= { 0.0000387428*3.5, 0.000253933*3.5, 0.000607752*3.5, 0.00108499*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr7 = new TGraphErrors(n,x,y,ex,ey);
	  gr7->SetMarkerColor(kRed+3 );
	  gr7->SetMarkerStyle(20);
	  gr7->SetMarkerSize(1);
	  gr7->SetLineColor(kRed+3 );
	  mg->Add(gr7);
	}

	if (ialg==8) {
	  salg="AllAlgorithm";
	  Double_t x[n] = { 0, 60, 140, 200 };
	  Double_t y[n] =  { 1.24127*3.5,   14.7013*3.5,   44.1023*3.5,   80.7785*3.5 };
	  Double_t ex[n] = {0,0,0,0};
	  Double_t ey[n]= { 0.0145777*3.5, 0.166914*3.5, 0.489753*3.5, 0.904573*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[1],ey[1]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[2],ey[2]) << " & "
	       << Form("%.2f $\\pm$ %.2f",y[3],ey[3]) << " \\\\ "
	       << endl;
	  gr8 = new TGraphErrors(n,x,y,ex,ey);
	  gr8->SetMarkerColor(kOrange+1  );
	  gr8->SetMarkerStyle(20);
	  gr8->SetMarkerSize(1);
	  gr8->SetLineColor(kOrange+1  );
	  mg->Add(gr8);
	}

      }else {
	if(ialg==0){
	  salg="AthAlgSeq";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 32.7469*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.499279*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre0 = new TGraphErrors(m,x,y,ex,ey);
	  gre0->SetMarkerColor(kBlack);
	  gre0->SetMarkerStyle(29);
	  gre0->SetMarkerSize(1.25);
	  gre0->SetLineColor(kBlack);
	  //mg->Add(gre0);
	}
	if(ialg==1){
	  salg="SiPreProcessing";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 0.417785*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.00565544*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre1 = new TGraphErrors(m,x,y,ex,ey);
	  gre1->SetMarkerColor(kBlue);
	  gre1->SetMarkerStyle(29);
	  gre1->SetMarkerSize(1.25);
	  gre1->SetLineColor(kBlue);
	  mg->Add(gre1);
	}
	if(ialg==2){
	  salg="InDetSiTrackerSpacePointFinder";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 0.209808*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.00315464*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre2 = new TGraphErrors(m,x,y,ex,ey);
	  gre2->SetMarkerColor(kRed);
	  gre2->SetMarkerStyle(29);
	  gre2->SetMarkerSize(1.25);
	  gre2->SetLineColor(kRed);
	  mg->Add(gre2);
	}
	if(ialg==3){
	  salg="Pattern";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 6.58081*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.187664*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre3 = new TGraphErrors(m,x,y,ex,ey);
	  gre3->SetMarkerColor(kMagenta);
	  gre3->SetMarkerStyle(29);
	  gre3->SetMarkerSize(1.25);
	  gre3->SetLineColor(kMagenta);
	  mg->Add(gre3);
	}
	if(ialg==4){
	  salg="TotalAmbiguity";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 4.40233*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.0951627*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre4 = new TGraphErrors(m,x,y,ex,ey);
	  gre4->SetMarkerColor(kGreen+3);
	  gre4->SetMarkerStyle(29);
	  gre4->SetMarkerSize(1.25);
	  gre4->SetLineColor(kGreen+3);
	  mg->Add(gre4);
	}
	if(ialg==9){
	  salg="TRTBackTracking";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 5.52289*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.0879532*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre9 = new TGraphErrors(m,x,y,ex,ey);
	  gre9->SetMarkerColor(kCyan-3);
	  gre9->SetMarkerStyle(29);
	  gre9->SetMarkerSize(1.25);
	  gre9->SetLineColor(kCyan-3);
	  mg->Add(gre9);
	}
	if(ialg==5){
	  salg="InDetPriVxFinder";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 0.146017*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.00232732*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre5 = new TGraphErrors(m,x,y,ex,ey);
	  gre5->SetMarkerColor(kCyan);
	  gre5->SetMarkerStyle(29);
	  gre5->SetMarkerSize(1.25);
	  gre5->SetLineColor(kCyan);
	  mg->Add(gre5);
	}
	if(ialg==6){
	  salg="ITkgenonly";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 0.613721*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.0103191*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre6 = new TGraphErrors(m,x,y,ex,ey);
	  gre6->SetMarkerColor(kBlack );
	  gre6->SetMarkerStyle(29);
	  gre6->SetMarkerSize(1.25);
	  gre6->SetLineColor(kBlack );
	  mg->Add(gre6);
	}

	if(ialg==10){
	  salg="TotalLowBet";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 0.3626*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.01032281*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre10 = new TGraphErrors(m,x,y,ex,ey);
	  gre10->SetMarkerColor(kMagenta-2 );
	  gre10->SetMarkerStyle(29);
	  gre10->SetMarkerSize(1.25);
	  gre10->SetLineColor(kMagenta-2 );
	  mg->Add(gre10);
	}

	if(ialg==7){
	  salg="MuonCombinedInDetCandidateAlg";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 0.01436*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.000233458*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre7 = new TGraphErrors(m,x,y,ex,ey);
	  gre7->SetMarkerColor(kRed+3 );
	  gre7->SetMarkerStyle(29);
	  gre7->SetMarkerSize(1.25);
	  gre7->SetLineColor(kRed+3 );
	  mg->Add(gre7);
	}
	if(ialg==8){
	  salg="AllAlgorithm";
	  Double_t x[m] = { 20 };
	  Double_t y[m] = { 18.2468*3.5 };
	  Double_t ex[m] = {0};
	  Double_t ey[m] = { 0.386892*3.5 };
	  cout << salg.c_str() << " & "
	       << Form("%.2f $\\pm$ %.2f",y[0],ey[0]) << " \\\\ "
	       << endl;
	  gre8 = new TGraphErrors(m,x,y,ex,ey);
	  gre8->SetMarkerColor(kOrange+1  );
	  gre8->SetMarkerStyle(29);
	  gre8->SetMarkerSize(1.25);
	  gre8->SetLineColor(kOrange+1  );
	  mg->Add(gre8);
	}
      }
    }
  }

  //leg->AddEntry(gre0, "AthAlgSeq (Run2)", "p");
  // leg->AddEntry(gr0, "AthAlgSeq (ITk)", "lp");
  
  leg->AddEntry(gre1, "Si Pre-Processing (Run2)", "p");
  leg->AddEntry(gr1,  "Si Pre-Processing (ITk)", "lp");
  
  leg->AddEntry(gre2, "SP formation (Run2)", "p");
  leg->AddEntry(gr2,  "SP formation (ITk)", "lp");

  leg->AddEntry(gre3, "Pattern recognition (Run2)", "p");
  leg->AddEntry(gr3,  "Pattern recognition (ITk)", "lp");
  
  leg->AddEntry(gre4, "Ambiguity (Run2)", "p");
  leg->AddEntry(gr4, "Ambiguity (ITk2)", "lp");

  leg->AddEntry(gre9, "Back-tracking (Run2)", "p");
  leg->AddEntry(gre10, "Low-Beta (Run2)", "p");
  
  leg->AddEntry(gre5, "Vertexing (Run2)", "p");
  leg->AddEntry(gr5,  "Vertexing (ITk)", "lp");

  leg->AddEntry(gre6, "ID general (Run2)", "p");
  leg->AddEntry(gr6,  "ID general (ITk)", "lp");

  leg->AddEntry(gre7, "Non-ID (Run2)", "p");
  leg->AddEntry(gr7,  "Non-ID (ITk)", "lp");

  leg->AddEntry(gre8, "Total ID (Run2)", "p");
  leg->AddEntry(gr8,  "Total ID (ITk)", "lp");
  
  //leg->AddEntry(gre7, "Particle creation (Run2)", "p");
  //leg->AddEntry(gr6,  "Particle creation (ITk)", "lp");



  
  
      TCanvas* c1=new TCanvas("c1","c1",600,800);
      c1->SetLogy();
      mg->Draw("ALP");
      
      //      mg->SetTitle(" Time for Different PileUp");
      mg->GetXaxis()->SetTitle("# of interactions / bunch-crossing (#mu) ");
      mg->GetYaxis()->SetTitle("HS06 seconds");
      leg->Draw();
      //      leg2->Draw();
      c1->Modified();
      mg->GetYaxis()->SetRangeUser(.00005,2000);
      ATLASLabelOld(0.2,.9,true,1);
      c1->Update();
      // c1->SaveAs("All_PileUp_linear.pdf");
      c1->SaveAs("All_PileUp_log_new.pdf");
}  
    