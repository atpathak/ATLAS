#!/usr/bin/env python2

from QFramework import *
from ROOT import *

samplefile = TQSampleFolder.loadSampleFolder("output/fake_november/nominal.root:samples")

rd = TQSampleDataReader(samplefile)

channel_list = ["lephad"]
cut_list = ["Cutbin2"]
#var_list = ['sqrtSET_dMET']
var_list = ['dPhiTauLep_sumET']

var_name = [""]
# \#Delta\#phi(\#tau,l)", $(dMET) : "(E_{T}^{miss,reco}-E_{T}^{miss,truth})
color_list = [kBlue, kRed, kBlack]
outfile = TFile("HSG3_histo.root", "recreate")

include_over_under = 1
over_bin = [0,0,0,0,0,0]
under_bin = [0,0,0,0,0,0]

for channel in channel_list:
   for cutstage in cut_list:
      #sample = "bkg/"+channel+"/diboson/WW"
      sample = "sig/"+channel+"/ggH"
      #sample = "sig/"+channel
      gROOT.SetBatch(True)
      gStyle.SetOptStat(0)
      c = TCanvas("MET Resolution")
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
            histo_list[i].GetXaxis().SetTitle("#Delta#phi(#tau,l)")
            histo_list[i].GetYaxis().SetTitle("(E_{T}^{miss,reco}-E_{T}^{miss,truth})")
            pass
         pass

      # make plots
      histo_list[0].SetTitle("ggH+VBFH(#tau#mu)")
      histo_list[0].DrawCopy()
      for i in range(1,histo_list[0].GetNbinsX()):         
         mean = histo_list[0].ProjectionY("",i-1,i).GetMean()
         rms = histo_list[0].ProjectionY("",i-1,i).GetRMS()
         rmsErr = histo_list[0].ProjectionY("",i-1,i).GetRMSError()
         #sigma = histo_list[1].ProjectionY("",i-1,i).GetMean()
         value = " (%.2f, %.2f, %.4f)" % (mean, rms, rmsErr)
         print value
         pass
      # save plots
      c.SaveAs(channel+"_"+cutstage+"_MET_Res.pdf","pdf")
      
      pass
   pass

outfile.Write()
outfile.Close()
