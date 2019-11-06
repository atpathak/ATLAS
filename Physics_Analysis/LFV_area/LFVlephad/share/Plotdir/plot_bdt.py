#!/usr/bin/env python2

from ROOT import *
from ROOT import TMath
from ROOT import TTree, TFile, AddressOf, gROOT


#var_list = ["collMass","dAlpha","transverseMassLepMET","transverseMassTauMET","leptonPt","tauPt","CosDPhi","met","Mjj","DEtajj"]
var_list = ["collMass","dAlpha","transverseMassLepMET","transverseMassTauMET","leptonPt","tauPt","CosDPhi","met"]
#var_list = ["collMass","transverseMassLepMET","transverseMassTauMET","leptonPt","tauPt","met","Mjj","DEtajj"]
set_list = ["Signal","Background"]


#f = TFile.Open("/afs/cern.ch/work/a/atpathak/private/LFV_area_16Feb2018/LFVlephad/share/BDT_mtau_Cut2jet_0.root")
f = TFile.Open("/afs/cern.ch/work/a/atpathak/private/LFV_area_16Feb2018/LFVlephad/share/BDT_mtau_Cut01jet_0.root")
#f = TFile.Open("/afs/cern.ch/work/a/atpathak/private/LFV_area_16Feb2018/LFVlephad/share/BDT_etau_Cut2jet_0.root")
#dir = gFile.GetDirectory("InputVariables_Id")
#dir.cd()
#dir.ls()
for var in var_list:
    gROOT.SetBatch(True)
    gStyle.SetOptStat(0)
    c = TCanvas("Tau")
    histo_list = []
    for set in set_list:
            histo = f.Get("InputVariables_Id/"+var+"__"+set+"_Id")
            histo.SetName(set)
            histo_list.append(histo)
    print len(histo_list)
    for i in range(len(histo_list)):
        if i == 0:
            histo_list[i].SetLineColor(2)
            histo_list[i].SetFillColor(2)
            histo_list[i].SetFillStyle(3006)
        if i == 1:
            histo_list[i].SetLineColor(4)
            histo_list[i].SetFillColor(4)
            
    histo_list[0].Scale(1/histo_list[0].Integral())
    histo_list[1].Scale(1/histo_list[1].Integral())
    max = histo_list[0].GetMaximum()
    if histo_list[1].GetMaximum() > max:
        max = histo_list[1].GetMaximum()
    histo_list[0].SetMaximum(1.5*max)
    histo_list[0].SetTitle("")
    histo_list[0].GetYaxis().SetTitle("Normalised events")
    histo_list[0].DrawCopy("HIST")         
    histo_list[1].DrawCopy("same HIST")
    legend = TLegend(0.68,0.7,0.88,0.88,"","brNDC")
    legend.SetBorderSize(0)
    legend.SetTextFont(62)
    legend.SetTextSize(0.04)
    legend.AddEntry(histo_list[0],"signal","f")
    legend.AddEntry(histo_list[1],"Background","f")
    legend.Draw()
    l1 = TLatex()
    l1.SetNDC()
    l1.SetTextColor(1)
    l1.SetTextFont(62)
    l1.SetTextSize(0.036)
    l1.DrawLatex(0.15, 0.77,"H#rightarrow#mu#tau_{had}")
    #l1.DrawLatex(0.15, 0.77,"H#rightarrowe#tau_{had}")
    #c.SaveAs("mtau_Cut2jet_"+var+"_1.pdf","pdf")
    c.SaveAs("mtau_Cut01jet_"+var+"_1.pdf","pdf")
f.Close()
    
