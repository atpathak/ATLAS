#!/usr/bin/env python2

from QFramework import *
from ROOT import *

samplefile = TQSampleFolder.loadSampleFolder("output/fake_november/nominal.root:samples")

rd = TQSampleDataReader(samplefile)

channel_list = ["lephad"]
cut_list = ["CutLeptons"]
#var_list = ['dPhiTauLep_dMET']
var_list = ['sqrtSET_dMET']
color_list = [kBlue, kRed, kBlack]
outfile = TFile("HSG3_histo.root", "recreate")

include_over_under = 1
over_bin = [0,0,0,0,0,0]
under_bin = [0,0,0,0,0,0]

for channel in channel_list:
   for cutstage in cut_list:
      #sample = "bkg/"+channel+"/diboson/WW"
      #sample = "sig/"+channel+"/mh125/ggf"
      sample = "sig/"+channel
      gROOT.SetBatch(True)
      gStyle.SetOptStat(0)
      c = TCanvas("Mass")
      c.cd()

      # read histograms from output
      histo_list = []
      for var in var_list:
         histo = rd.getHistogram(sample,cutstage+"/"+var)         
         histo_list.append(histo)
         #histo.Write()
         pass

      # set plotting styles
      for i in range(len(histo_list)):
         if i == 0:
            histo_list[i].GetXaxis().SetTitleSize(0.05)
            histo_list[i].GetYaxis().SetTitleSize(0.05)
            histo_list[i].SetTitleSize(0.05)
            #histo_list[i].GetXaxis().SetTitle("m_{l#tau}")
            #histo_list[i].GetYaxis().SetTitle("Arbitrary unit")
            pass
         pass

      # make plots
      histo_list[0].SetTitle("ggH(#tau#mu)")
      histo_list[0].GetXaxis().SetTitle("#sqrt{#Sigma E_{T}}")
      histo_list[0].DrawCopy("col")
      # save plots
      c.SaveAs(channel+"_"+cutstage+"_"+var_list[0]+".pdf","pdf")
      
      pass
   pass

outfile.Write()
outfile.Close()
