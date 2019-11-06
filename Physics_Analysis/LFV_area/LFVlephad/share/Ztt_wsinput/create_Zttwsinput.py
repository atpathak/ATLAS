#!/usr/bin/env python2

from QFramework import *
from ROOT import *

gROOT.SetBatch(True)
gStyle.SetOptStat(0)

samplefile1 = TQSampleFolder.loadSampleFolder("/afs/cern.ch/work/a/atpathak/public/LFV_area_26Apr2018/LFVlephad/share/output/lfv_lephad_Sherpa_test3_new.root:samples")
rd1 = TQSampleDataReader(samplefile1)
samplefile2 = TQSampleFolder.loadSampleFolder("/afs/cern.ch/work/a/atpathak/public/LFV_area_26Apr2018/LFVlephad/share/output/output_ZttMG/samples-lfv_lephad_nominal_Sherpa-syst_X_ZttMG.root:samples")
rd2 = TQSampleDataReader(samplefile2)

infile = TFile("lfv_lephad_ws_nominal_test.root", "read")
outfile = TFile("lfv_lephad_ws_nominal_test1.root", "recreate")
channel_list = ["mtau"]

#cut_list = ["CutTauMTSR1_VBF","CutTauMTSR2_VBF","CutTauPtSR3_VBF"]
cut_list = ["CutPreselection"] #, "CutTauMTSR1_1p","CutTauMTSR2_1p","CutTauPtSR3_1p","CutTauMTSR1_3p","CutTauMTSR2_3p","CutTauPtSR3_3p"]

#region_list = ["cba_sr1_vbf_signal", "cba_sr2_vbf_signal", "cba_sr3_vbf_signal"]
#region_list = ["cba_presel_signal"] #, "cba_sr1_1p_signal", "cba_sr2_1p_signal", "cba_sr3_1p_signal", "cba_sr1_3p_signal", "cba_sr2_3p_signal", "cba_sr3_3p_signal"]
region_list = ["cba_sr1_vbf_signal","cba_sr2_vbf_signal","cba_sr3_vbf_signal","cba_vbf_signal"]

#input_sample_list = ["ggH","VBFH","VH", "Diboson","[ZllSh+ZllEWSh]","[ZttSh+ZttEWSh]","Fake","[WjetsSh+WjetsEWSh]","Top","htt","data","eFake"]
input_sample_list = ["[ZttSh+ZttEWSh]"]

#sample_list = ["ggH","VBFH", "VH", "VV","Zll","Ztt","Fake","Wjets", "Top","Htt","Data","eFake"]
sample_list = ["Ztt"]

dist = "collMassBL"
n_region = 1
n_sample = 1



#file = open("/afs/cern.ch/work/n/ndang/LFV_area/LFVlephad/share/systematics/weight_list", "r")
#file = open("/afs/cern.ch/work/n/ndang/LFV_area/LFVlephad/share/systematics/test", "r")

for channel in channel_list:
    for j in range(0,n_region):
        cutstage = cut_list[j]
        region = region_list[j]
        outfile.cd()
        outfile.mkdir(channel+"_"+region)
        outfile.cd(channel+"_"+region)
        print channel+"_"+region
        for i in range(0,n_sample):
            outfile.mkdir(channel+"_"+region+"/"+sample_list[i])
            outfile.cd(channel+"_"+region+"/"+sample_list[i])
            file = open("/afs/cern.ch/work/a/atpathak/public/LFV_area_26Apr2018/LFVlephad/share/Ztt_wsinput/Ztt_list", "r")
            for variation in file:
                if variation and variation.strip():
                    variation = variation.strip()
                    if variation.startswith('#'):
                        continue
                    if variation == "nominal":
                        hnominal = infile.Get(channel+"_"+region+"/"+sample_list[i]+"/"+variation)
                        hnominal.Write(variation)
                    if variation == "theory_ztt_MGvsSh_constrained_high":
                        s = "bkg/"+channel+"/"+input_sample_list[i]
                        s2 = "syst/"+channel+"/ZttMG"
                        if sample_list[i] == "ggH" or sample_list[i] =="VBFH" or sample_list[i] =="VH":
                            s = "sig/"+channel+variation1+"/"+input_sample_list[i]
                        if sample_list[i] == "Data":
                            s = "data/"+channel+variation
                        histo = rd1.getHistogram(s,cutstage+"/"+dist)
                        histo2 = rd2.getHistogram(s2,cutstage+"/"+dist)
                        #if not histo:
                        #    histo = rd2.getHistogram(s,cutstage+"/"+dist)
                        if not histo:
                            #errMess = "no "+ input_sample_list[i] + " " + variation + " in " + channel+"_"+region
                            #print errMess
                            continue
                        #outfile.cd(channel+"_"+region+"/"+sample_list[i])
                        histo.SetBinContent( 1, histo.GetBinContent(0) + histo.GetBinContent(1) )
                        histo.SetBinContent( histo.GetNbinsX(), histo.GetBinContent(histo.GetNbinsX()) + histo.GetBinContent(histo.GetNbinsX()+1) )
                        histo2.SetBinContent( 1, histo2.GetBinContent(0) + histo2.GetBinContent(1) )
                        histo2.SetBinContent( histo2.GetNbinsX(), histo2.GetBinContent(histo2.GetNbinsX()) + histo2.GetBinContent(histo2.GetNbinsX()+1) )
                        histo.Scale(1./histo.Integral())
                        histo2.Scale(1./histo2.Integral())
                        histo2.Divide(histo)
                        histo2.Write(variation)
            file.close()
            outfile.cd(channel+"_"+region)
        outfile.cd("/")

outfile.Close()
