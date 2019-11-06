#!/usr/bin/env python2

from QFramework import *
from ROOT import *

samplefile = TQSampleFolder.loadSampleFolder("output/fake_november/nominal.root:samples")

rd = TQSampleDataReader(samplefile)

channel_list = ["lephad"]
cut_list = ["CutLeptons"]
#var_list = ['collMasslh','collMasslfv']
#var_name = ["m_{coll}(lephad)","m_{coll}(lfv)"]
#var_list = ['MMClh_maxw','MMClh_mlm','MMClh_mlnu3p']
#var_name = ["MMC^{maxw}(lephad)","MMC^{mlm}(lephad)","MMC^{mlnu3p}(lephad)"]
#var_list = ['MMClfv_maxw','visibleMass','collMasslfv']
var_list = ['MMClfv_maxw','visibleMass','collMasslfv']
var_name = ["MMC^{maxw}(lfv)","m_{vis}","m_{coll}(lfv)"]
color_list = [kBlue, kRed, kBlack]
outfile = TFile("HSG3_histo.root", "recreate")

include_over_under = 1
over_bin = [0,0,0,0,0,0]
under_bin = [0,0,0,0,0,0]

for channel in channel_list:
   for cutstage in cut_list:
      #sample = "bkg/"+channel+"/diboson/WW"
      #sample = "sig/"+channel+"/VBFH"
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
         histo_list[i].SetLineColor(color_list[i])
         if i == 0:
            histo_list[i].GetXaxis().SetTitleSize(0.05)
            histo_list[i].GetYaxis().SetTitleSize(0.05)
            histo_list[i].SetTitleSize(0.05)
            histo_list[i].GetXaxis().SetTitle("m_{l#tau}")
            histo_list[i].GetYaxis().SetTitle("Arbitrary unit")
            max =  histo_list[i].GetMaximum()
            pass
         if max < histo_list[i].GetMaximum():
            max = histo_list[i].GetMaximum()
            pass
         if include_over_under:
            under_bin[i] = histo_list[i].GetBinContent(0)
            over_bin[i] = histo_list[i].GetBinContent(histo_list[i].GetNbinsX()+1)
            histo_list[i].SetBinContent(1, histo_list[i].GetBinContent(1) + histo_list[i].GetBinContent(0) )
            histo_list[i].SetBinContent(histo_list[i].GetNbinsX(), histo_list[i].GetBinContent(histo_list[i].GetNbinsX()) + histo_list[i].GetBinContent(histo_list[i].GetNbinsX()+1) )
            pass
         pass

      # make plots
      histo_list[0].GetYaxis().SetRangeUser(0, max*1.2)
      histo_list[0].SetTitle("ggH+VBFH(#tau#mu)")
      histo_list[0].DrawCopy("hist")
      for i in range(len(histo_list)):
         histo_list[i].DrawCopy("hist same")        
         pass
      # draw legends
      legend = TLegend(0.55, 0.72, 0.85, 0.87)
      legend.SetBorderSize(0)
      legend.SetFillColor(0)
      legend.SetTextSize(0.03)
      for i in range(len(histo_list)):         
         histo_list[i].SetBinContent(1, histo_list[i].GetBinContent(1) - under_bin[i] )
         histo_list[i].SetBinContent(histo_list[i].GetNbinsX(), histo_list[i].GetBinContent(histo_list[i].GetNbinsX()) - over_bin[i] )
         mean = histo_list[i].GetMean()
         rms = histo_list[i].GetRMS()
         value = " (%.2f, %.2f)" % (mean, rms)
         if i ==1:
            value = " (%.2f, %.2f)" % (105.94, 12.46)
            pass
         if i ==2:
            value = " (%.2f, %.2f)" % (129.28, 10.59)
            pass
         legend.AddEntry(histo_list[i], var_name[i]+value, "f")  
         legend.Draw()
         print histo_list[i].Integral(1,histo_list[i].GetNbinsX())
         pass
      # save plots
      c.SaveAs(channel+"_"+cutstage+"_Mass.pdf","pdf")
      
      pass
   pass

outfile.Write()
outfile.Close()
