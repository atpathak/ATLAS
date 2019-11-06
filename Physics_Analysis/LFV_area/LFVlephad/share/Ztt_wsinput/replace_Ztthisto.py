#!/usr/bin/env python2

from ROOT import *

gROOT.SetBatch(True)
gStyle.SetOptStat(0)

file = TFile("../lfv_lephad_WSinput_18_08_03_UnblindedData_fixedZtt.root", "update")
file1 = TFile("../lfv_lephad_WSinput_18_08_03_UnblindedData_fixedZtt_zttTheoryConstrained.root", "read")

channel_list = ["etau"]

region_list = ["cba_sr1_vbf_signal", "cba_sr2_vbf_signal", "cba_sr3_vbf_signal"] #, "cba_vbf_signal"] 

sample_list = ["Ztt"]

'''
for channel in channel_list:
    for region in region_list:
        for sample in sample_list:
            file.cd(channel+"_"+region+"/"+sample)
            nominal = file.Get(channel+"_"+region+"/"+sample+"/nominal")
            infile = open("/afs/cern.ch/work/n/ndang/LFV_area/LFVlephad/share/systematics/weightsystematics7", "r")
            for variation in infile:
                if variation and variation.strip():
                    print channel+"_"+region+"/"+sample+"/"+variation
                    variation = variation.strip()
                    var = file.Get(channel+"_"+region+"/"+sample+"/"+variation)
                    var1 = file1.Get(channel+"_"+region+"/"+sample+"/"+variation)
                    #print var.Integral()
                    #print var1.Integral()
                    var = var1.Clone()
                    #var.SetTitle(variation);
                    var.Write(variation,TObject.kOverwrite);

            file.cd("")
            infile.close()
'''
#["efake_FF_low","efake_FF_high","efake_ID_low","efake_ID_high","efake_MC_low","efake_MC_high"]
variation="theory_ztt_MGvsSh_constrained_high"
for channel in channel_list:
    for region in region_list:
        for sample in sample_list:
            file.cd(channel+"_"+region+"/"+sample)
            var = file.Get(channel+"_"+region+"/"+sample+"/"+variation)
            nominal = file.Get(channel+"_"+region+"/"+sample+"/"+variation)
            nominal1 = file1.Get(channel+"_"+region+"/"+sample+"/"+variation)
            print nominal.Integral()
            print nominal1.Integral()
            nominal = nominal1.Clone()
            #var.Multiply(nominal1)
            nominal.Write(variation,TObject.kOverwrite);
            file.cd("")

file.Close()




