#!/usr/bin/env python2

from ROOT import *
from ROOT import TMath
from ROOT import TTree, TFile, TCanvas, AddressOf, gROOT, gStyle


channel_list = ["SR1_1Prong","SR2_1Prong","SR3_1Prong","SR1_3Prong","SR2_3Prong","SR3_3Prong"]
color_list = [kBlue, kRed, kBlack, kGreen, kCyan, kMagenta]

ff = TFile("FF_fake.root","read")

gROOT.SetBatch(True)
gStyle.SetOptStat(0)  
c1 = TCanvas("c1")
histo_list = []

for channel in channel_list:
    name = "mtau_QCD_"+channel+"_SLT_2016"
    histo = ff.Get(name)
    histo.SetName(channel)
    histo_list.append(histo)
    print name
    #print channel
print len(histo_list)

for i in range(len(histo_list)):
    histo_list[i].SetLineColor(color_list[i])

histo_list[0].Scale(1/histo_list[0].Integral())
histo_list[0].Draw()
for i in range(1,len(histo_list)):
    #myhist = histo_list[i].Clone()
    #myhist.Scale(1/myhist.Integral())
    #myhist.DrawCopy("same")
    #histo_list[i].Scale(1/histo_list[i].Integral())
    histo_list[i].DrawCopy("same")
c1.SaveAs("All.pdf","pdf")


