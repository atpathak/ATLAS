#!/usr/bin/env python

import argparse
import array
import logging
import math
import os
from copy import copy

from ROOT import gStyle, TCanvas, TH1F, TH2F, gPad, TGaxis, gROOT, TLine

#from get_bdt_blinding import get_blinding_threshold


class BinUncertainties(object):
    """Manage the uncertainties for several bins which will be eventually merged.

    Attributes:
        background_counts (list(float)): A list of the contents in each bin.
        abs_uncertainties (list(float)): A list of the absolute uncertainties in each bin.
    """

    def __init__(self):
        self.background_counts = list()
        self.abs_uncertainties = list()

    def add_bin(self, bin_content, bin_error):
        """Add a bin.

        Arguments:
            bin_content (float): content of the bin
            bin_error (float): uncertainty of the bin
        """
        self.background_counts.append(bin_content)
        self.abs_uncertainties.append(bin_error)

    def get_relative_uncertainty(self):
        """Calculate the absolute relative uncertainty if all bins are merged.

        Returns:
            float: The absolute relative uncertainty if the bins are merged. None if there are no events.
            """
        total_bkg = abs(sum(self.background_counts))
        total_uncert = math.sqrt(sum([uncert ** 2 for uncert in self.abs_uncertainties]))
        if total_bkg > 0:
            return total_uncert / total_bkg
        return None

#Helper class to setup channel-specific treatments
class ChannelSetup(object):

  def __init__(self,inFile='',channelName=''):
    logging.info('Setup channel with name %s'%channelName)
    self.name=channelName
    
    #provide finely binned ws input file
    self.inFileName=inFile
    from ROOT import TFile
    self.inFile=TFile(inFile,'r')
    
    #set up properties per channel
    if 'hadhad' in channelName:
      #cross check this list against input file directories
      self.validSamples=["VBFH","ggH","ZH","WH","ttH","Ztt","ZttEWK","Fake","Top","W","Zll","VV"]
      #define which processes in input file define a bkgd component
      #TOTAL bkgd necessary
      self.bkgdCombinations={
        "Ztt"       :["Ztt","ZttEWK"],
        "Fake"      :["Fake"],
        #"Others"    :["Top","W","Zll","VV"],
        "TOTAL"     :["Ztt","ZttEWK","Fake","Top","W","Zll","VV"],
        }
      #for now assuming one signal histogram only
      self.signalCombinations={
        "Signal"    :["VBFH","ggH","ZH","WH","ttH"],
        }
      #for hadhad, we don't want to run the BDT rebinning on the rest category
      self.ignoreCatgories=["hhAll_mva_rest_control","hhAll_mva_restmmc_control","hhAll_mva_restdphi_control"]
    elif 'lephad' in channelName:
      #cross check this list against input file directories
      self.validSamples=["VBFH","ggH","ZH","WH","ttH","Ztt","ZttEWK","Fake","Top","W","Zll","VV"]
      #define which processes in input file define a bkgd component
      self.bkgdCombinations={
        "Ztt"       :[],
        "TOTAL"     :["Ztt","ZttEWK","Fake","Top","W","Zll","VV"],
        }
      self.signalCombinations={
        "Signal"    :[],
        }
      #exclude categories containing
      self.ignoreCatgories=[""]

    elif 'leplep' in channelName:
      #cross check this list against input file directories
      self.validSamples=["VBFH","ggH","ZH","WH","ttH","Ztt","ZttEWK","Fake","Top","W","Zll","VV"]
      #define which processes in input file define a bkgd component
      self.bkgdCombinations={
        "Ztt"       :[],
        "TOTAL"     :["Ztt","ZttEWK","Fake","Top","W","Zll","VV"],
        }
      self.signalCombinations={
        "Signal"    :[],
        }
      #exclude categories
      self.ignoreCatgories=[""]
    else:
      raise ValueError("Cannot assign channelName %s."%channelName)
  
  def __str__(self):
    attrs=vars(self)
    outString="ChannelSetup Class for channel %s\n"
    outString+='\n'.join("- %s: %s" % item for item in attrs.items())
    return outString

class HistDiscovery(object):
  def __init__(self,channel,category):
    self.channel=channel
    self.category=category
    
    self.signalHist=None
    self.bkgdDict={}
  
    #get histograms from file
    self.discoverHists()
    pass
  
  def discoverHists(self):
    logging.debug("Hist Discovery imminent")
    from ROOT import TH1F
    allSamples=[]
    #loop to assign histos to each combination of samples
    for it_sample in self.category.GetListOfKeys():
      sample=it_sample.ReadObj()
      #skip lumi hists
      if isinstance(sample,TH1F): continue
      allSamples.append(sample)
    #find matching samples to channel sample list and construct histograms
    for sigComb in self.channel.signalCombinations.keys():
      combProcList=self.channel.signalCombinations[sigComb]
      filteredSampleList=[]
      for proc in combProcList:
        for sample in allSamples:
          if sample.GetName() in proc:
            filteredSampleList.append(sample)
      #assuming one signal histogram only
      self.signalHist=self.getCombinedHist(filteredSampleList,sigComb)
      break
    
    #find matching samples to channel sample list and construct histograms
    for bkgdComb in self.channel.bkgdCombinations.keys():
      combProcList=self.channel.bkgdCombinations[bkgdComb]
      filteredSampleList=[]
      for proc in combProcList:
        for sample in allSamples:
          if sample.GetName() in proc:
            filteredSampleList.append(sample)
      self.bkgdDict[bkgdComb]=self.getCombinedHist(filteredSampleList,bkgdComb)
    
  def getCombinedHist(self,filteredSampleList,title):
    combHist=None
    for sample in filteredSampleList:
      for it_hist in sample.GetListOfKeys():
        hist=it_hist.ReadObj()
        if not 'nominal' in hist.GetName(): continue
        if not combHist:
          combHist=copy(hist)
        else:
          combHist.Add(hist)
    combHist.SetName(title)
    return combHist
          


def get_run1_like_binning(histContainer):
    """Calculate the binning like in Run-1.

    Bins are merged from right to left (high to low BDT score),
    until the following criteria are met:

    - The total expected background in the new bin exceeds the total expected
      background in the previous bin.

    - The relative statistical uncertainty of each background component is smaller
      than the defined threshold. This is currently 100% for every background.
      The backgrounds are combined to the following backgrounds:
        - Zll (ZllS22+ZllS22f+ZllEW)
        - Ztt (ZttS22+ZttS22f+ZttEW)
        - Top (ttb+stop)
        - HWW (ggHWW+VBFHWW)
        - Diboson (diboson)
        - TFakes (tfakes+tfakesdata)

    - Either the signal-to-background ratio is at most 10% smaller than in the
      previous bin or the total expected background exceeds the one in the previous
      bin by more than 50%.

    Returns:
        list(float): A list with the new binning (bin borders).
    """
    bin_prec = 6  # round bins to this precision

    new_merge_bins = []
    new_bin_edges = [1]

    #assuming samples are registered under these keys 
    total_background_name = "TOTAL"
    total_signal_name = "Signal"
    
    #remove TOTAL from general bkgd hist dict
    bkg_hists = dict(histContainer.bkgdDict)
    del bkg_hists["TOTAL"]
    
    total_bkg_hist = histContainer.bkgdDict["TOTAL"]
    total_sig_hist = histContainer.signalHist
    
    #initialise uncertainty thresholds to 1
    thresholds = {}
    for bkgdKey in bkg_hists.keys():
      thresholds[bkgdKey] = 1.0
    
    bin_numbers = list(range(1, total_bkg_hist.GetNbinsX() + 1))
    bin_numbers.reverse()
    first_merge = True
    current_total_background_count = 0
    last_total_background_count = 0
    current_uncertainties = dict([(bkg, BinUncertainties()) for bkg in thresholds])
    current_total_signal_count = 0
    current_sbr = 0
    last_sbr = 0

    for i in bin_numbers:
        logging.debug("Bin number %d", i)
        # The total expected background in the new bin exceeds the total expected background in the previous bin
        current_total_background_count += total_bkg_hist.GetBinContent(i)
        if first_merge:
            cond_bkg_count = True
        else:
            cond_bkg_count = current_total_background_count > last_total_background_count

        # The relative statistical uncertainty of each background component is smaller
        # than the defined threshold
        cond_rel_uncert = True
        for bkg, bin_uncert in current_uncertainties.items():
            hist = bkg_hists[bkg]
            bin_uncert.add_bin(hist.GetBinContent(i), hist.GetBinError(i))
            rel_uncert = bin_uncert.get_relative_uncertainty()
            # check if rel_uncert is not none and smaller than threshold
            cond_rel_uncert_single = rel_uncert and rel_uncert < thresholds[bkg]
            
          #  if hist.GetBinCenter(i)>0.5:
          #    if hist.GetBinContent(i) > 0:
          #      cond_rel_uncert_single = True
            cond_rel_uncert = cond_rel_uncert and cond_rel_uncert_single


        # the signal-to-background ratio is at most 10% smaller than in the previous bin
        current_total_signal_count += total_sig_hist.GetBinContent(i)
        cond_sbr = True
        if first_merge:
            cond_sbr = True
        else:
            if current_total_background_count > 0:
                current_sbr = current_total_signal_count / current_total_background_count
            else:
                current_sbr = -999
            cond_sbr = current_sbr  > 0.9 * last_sbr

        # the total expected background exceeds the one in the previous bin by more than 50%.
        if first_merge:
            cond_bkg_count_50 = True
        else:
            cond_bkg_count_50 = current_total_background_count > 1.5 * last_total_background_count
        
        if cond_bkg_count and cond_rel_uncert and (cond_sbr or cond_bkg_count_50):
            bin_low_edge = round(total_bkg_hist.GetBinLowEdge(i), bin_prec)
            logging.debug("Merging at %g", bin_low_edge)
            new_merge_bins.append(i)
            new_bin_edges.append(bin_low_edge)

            logging.debug("Relative background uncertainties")
            for bkg, bin_uncert in current_uncertainties.items():
                logging.debug("    {}: {}".format(bkg, bin_uncert.get_relative_uncertainty()))

            first_merge = False
            last_total_background_count = current_total_background_count
            current_total_background_count = 0
            current_uncertainties = dict([(bkg, BinUncertainties()) for bkg in thresholds])
            current_total_signal_count = 0
            last_sbr = current_sbr

    if len(new_bin_edges) == 1:  # no rebinning was done
        new_bin_edges.append(-1)
    if new_bin_edges[-1] != -1:  # last bin has to few entries
        new_bin_edges.append(-1)  # add last bin edge
    new_bin_edges.reverse()
    return new_bin_edges


def main(args):
    """Entry point of this script."""
    gROOT.ProcessLine( "gErrorIgnoreLevel = 1001;" )  # disalbe INFO messages
    gROOT.SetBatch(True)  # enable batch mode
    gStyle.SetOptStat(0)  # disable statistics box

    
    logging.info("Getting optimal binning")
    logging.info("Using Run-1-like binning algorithm")
    
    channel=ChannelSetup(args.input_file,args.channel)
    logging.info(channel)
    
    #looping input file
    #assuming structure category/process/histos
    from ROOT import TH1F
    for it_category in channel.inFile.GetListOfKeys():
      catname=it_category.GetName()
      if catname in channel.ignoreCatgories:
        logging.warning("Skipping Rebinning for category %s"%catname)
        continue
      else:
        logging.info("Processing category %s"%catname)
      cat=it_category.ReadObj()
      histContainer=HistDiscovery(channel,cat)
      binning = get_run1_like_binning(histContainer)
 #       hist_sig_rebin = hist_sig.Rebin(len(binning) - 1, "{}/{}_rebin".format(cut_name, histogram_name), array.array('d', binning))
 #       #blinding = get_blinding_threshold(hist_sig_rebin, args.threshold)

      #logging.info("Binning for category %s"%catname)
      logging.info("Binning: {}".format(str(binning)))
 #       #logging.info("    Blinding: >= {}".format(blinding))


if __name__ == "__main__":
    PARSER = argparse.ArgumentParser("Get the optimized binning for the BDT distributions")
#    PARSER.add_argument("input_file", metavar="INPUT_FILE", type=str, help="Input file", default='/Users/Eric/runII/hadhad/wsinputs/171013_v18_comb_WSinput_MVA_wSys_Bertram_BoostSplit_newRebin/hhAll_merged_WSinput_WithTheoryEnv_WithModJERVar.root')
    PARSER.add_argument("--input_file",type=str, help="Input file", default='/nfs/dust/atlas/user/edrechsl/RunII/hadhad/wsinputs/171023_v18_comb_WSinput_MVA_BalthasarValarie/tmp_wsincollection/hhAll_merged_WSinput.root')
    PARSER.add_argument("--sample-folder", dest="sample_folder", type=str, help="Name of sample folder", default="samples")
    PARSER.add_argument("--threshold", type=float, help="Unblind up to this fraction of the total signal yield", default=0.3)
    PARSER.add_argument("--debug", dest="debug", action="store_true", help="Print debug messages", default=False)
    PARSER.add_argument("--channel", dest="channel", type=str, help="Choose channel", default='hadhad')
    ARGS = PARSER.parse_args()
    LOG_LEVEL = logging.DEBUG if ARGS.debug else logging.INFO
    logging.basicConfig(level=LOG_LEVEL, format='%(asctime)s - %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
    

    #from QFramework import TQSampleFolder, TQSampleDataReader
    main(ARGS)
