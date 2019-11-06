#!/usr/bin/env python2

from ROOT import *
from ROOT import TMath
from ROOT import TTree, TFile, AddressOf, gROOT


channel_list = ["SR1","SR2","SR3"]
prong_list = ["1Prong","3Prong"]
#ff_root_file = "FF_fake.root"
#r_root_file = "R_fake.root"

ff = TFile("FF_fake.root","read")

gROOT.SetBatch(True)
gStyle.SetOptStat(0)  
c1 = TCanvas("c1")
gROOT.SetBatch(True)
gStyle.SetOptStat(0)
c1 = TCanvas("c1")
h1 = ff.Get("mtau_QCD_SR1_1Prong_SLT_2016")
h2 = ff.Get("mtau_QCD_SR1_3Prong_SLT_2016")
h3 = ff.Get("mtau_QCD_SR2_1Prong_SLT_2016")
h4 = ff.Get("mtau_QCD_SR2_3Prong_SLT_2016")
h5 = ff.Get("mtau_QCD_SR3_1Prong_SLT_2016")
h6 = ff.Get("mtau_QCD_SR3_1Prong_SLT_2016")

h1.SetTitle("FF_mtau")
h1.SetLineColor(kBlue);
h2.SetLineColor(kGreen);
h3.SetLineColor(kRed);
h4.SetLineColor(kCyan);
h5.SetLineColor(kBlack);
h6.SetLineColor(kMagenta);

h1.Scale(1/h1.Integral())
h2.Scale(1/h1.Integral())
h3.Scale(1/h1.Integral())
h4.Scale(1/h1.Integral())
h5.Scale(1/h1.Integral())
h6.Scale(1/h1.Integral())

h1.Draw()
h2.Draw("same")
h3.Draw("same")
h4.Draw("same")
h5.Draw("same")
h6.Draw("same")
c1.SaveAs("ALL_FF_mtau.pdf")