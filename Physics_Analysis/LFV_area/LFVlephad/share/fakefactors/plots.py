#!/usr/bin/env python2

from ROOT import *
from ROOT import TMath
from ROOT import TTree, TFile, AddressOf, gROOT


rr_list = ["SR","WCR","QCDCR"]
#ff_root_file = "FF_fake.root"
#r_root_file = "R_fake.root"

#ff = TFile("FF_fake.root")
r  = TFile("R_fake.root")
hist = TH1D()
gROOT.SetBatch(True)
gStyle.SetOptStat(0)
#c1 = TCanvas("c1")
#hist = ff.Get("mtau_QCD_SR1_1Prong_SLT_2016")
#hist.SetTitle("FF_mtau_QCD_SR1_1Prong_SLT_2016")
#hist.Draw()
#c1.SaveAs("mtau_QCD_SR1_1Prong_SLT_2016.pdf")

for region in rr_list:
    c = TCanvas("c")
    h_QCD = r.Get("mtau_QCD_SR1_1Prong_SLT_"+region+"_2016_up")
    h_W =   r.Get("mtau_W_SR1_1Prong_SLT_"+region+"_2016_up")
    h1 = h_QCD.Clone()
    h1.Add(h_W)
    h1.SetTitle("R_mtau_add_SR1_1Prong_SLT_"+region+"_2016")
    h1.SetLineColor(kMagenta)
    h_QCD.SetLineColor(kBlue)
    h_W.SetLineColor(kRed)
    h_QCD.Draw()
    h_W.Draw("same")
    h1.Draw("same")
    c.SaveAs("mtau_add_SR1_1Prong_SLT_"+region+"_2016.pdf")




'''
 R_W_1Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_W_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_W_3Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_W_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  R_W_1Prong_SLT_2016 = (TH1D*)rFile->Get("etau_W_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_W_3Prong_SLT_2016 = (TH1D*)rFile->Get("etau_W_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  
  //R_Top_1Prong_SLT_2015 = (TH1D*)rFile->Get("Top_"+category+"_1Prong_SLT_"+region+"_2015"+Rvar);
  //R_Top_3Prong_SLT_2015 = (TH1D*)rFile->Get("Top_"+category+"_3Prong_SLT_"+region+"_2015"+Rvar);
  //R_Top_1Prong_SLT_2016 = (TH1D*)rFile->Get("Top_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  //R_Top_3Prong_SLT_2016 = (TH1D*)rFile->Get("Top_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  
  //R_Z_1Prong_SLT_2015 = (TH1D*)rFile->Get("Z_"+category+"_1Prong_SLT_"+region+"_2015"+Rvar);
  //R_Z_3Prong_SLT_2015 = (TH1D*)rFile->Get("Z_"+category+"_3Prong_SLT_"+region+"_2015"+Rvar);
  //R_Z_1Prong_SLT_2016 = (TH1D*)rFile->Get("Z_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  //R_Z_3Prong_SLT_2016 = (TH1D*)rFile->Get("Z_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  
  R_QCD_1Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_QCD_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_QCD_3Prong_SLT_2015 = (TH1D*)rFile->Get("mtau_QCD_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
  R_QCD_1Prong_SLT_2016 = (TH1D*)rFile->Get("etau_QCD_"+category+"_1Prong_SLT_"+region+"_2016"+Rvar);
  R_QCD_3Prong_SLT_2016 = (TH1D*)rFile->Get("etau_QCD_"+category+"_3Prong_SLT_"+region+"_2016"+Rvar);
'''
