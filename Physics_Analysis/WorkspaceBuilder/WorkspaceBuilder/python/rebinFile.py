#!/usr/bin/python

from ROOT import TFile, TH1F, TCanvas, TObject, gROOT, gDirectory, TIter, TKey
from math import sqrt
from array import *
import argparse

maxstatlimits={"Ztt":0.3,"Fake":0.5,"Fakes":0.5} # the maximum allowed statistical uncertainty per bin

def getRegions(rootfile):
    rootfile.cd()
    nextkey = TIter(gDirectory.GetListOfKeys())
    key = nextkey.Next()
    regions={}
    while key and key.GetTitle():
        if key.IsFolder() == 1:
            label=getNewRegionName(key.GetName())
            if label in regions:
                regions[label].append(key.GetName())
            else:
                regions[label]=[key.GetName()]
        key=nextkey.Next()
    return regions

def getBinning(histograms):
    binborders=[histograms.values()[0].GetXaxis().GetXmax()]
    
    thisbin={}
    thisbinuncert2={}
    bin=histograms.values()[0].GetNbinsX()+1
    while bin>=1:            
        for histname in histograms.keys():
            if histname not in thisbin:
                thisbin[histname]=0.
                thisbinuncert2[histname]=0.
            thisbin[histname]=thisbin[histname]+histograms[histname].GetBinContent(bin)
            thisbinuncert2[histname]=thisbinuncert2[histname]+pow(histograms[histname].GetBinError(bin),2.)
        
        statuncertaintyok=True
        statuncertaintyreason="Reason:"
        for process in maxstatlimits.keys():
            if process in thisbin:
                if thisbin[process]<=0 or sqrt(thisbinuncert2[process])/thisbin[process]>maxstatlimits[process]:
                    statuncertaintyok=False
                    statuncertaintyreason=statuncertaintyreason+" "+process+": "+str(thisbin[process])+"+/-"+str(sqrt(thisbinuncert2[process]))
        if statuncertaintyok:
            binborder=histograms.values()[0].GetXaxis().GetBinLowEdge(bin)
            binborders.append(binborder)
            #print "Binning ", newregion, " bin ", len(binborders), " [x,",binborder, "]"
            thisbin.clear()
            thisbinuncert2.clear()
        if not statuncertaintyok and bin==1:
            binborder=histograms.values()[0].GetXaxis().GetBinLowEdge(bin)
            if len(binborders)>1:
                del binborders[-1]
            binborders.append(binborder)
            thisbin.clear()
            thisbinuncert2.clear()
        bin=bin-1
    binborders.reverse()
    
    return binborders

def rebin(histogram, binborders,directory):
    xarray = array('d',binborders)
    hist=TH1F(histogram.GetName(), histogram.GetTitle(),len(binborders)-1,xarray)
    hist.SetDirectory(directory)
    if histogram.GetTitle()=="nominal":
        print directory.GetName(), directory.GetMotherDir().GetName(), " -> ", histogram.GetEntries(), histogram.GetName(), histogram.GetTitle()
    sumw=0.
    sumw2=0.
    newbin=0
    for bin in range(0,histogram.GetNbinsX()+2):
        if abs(histogram.GetBinLowEdge(bin)-binborders[newbin])<0.0001:
            hist.SetBinContent(newbin,sumw)
            hist.SetBinError(newbin,sqrt(sumw2))
            newbin=newbin+1
            sumw=0.
            sumw2=0.
        sumw=sumw+histogram.GetBinContent(bin)
        sumw2=sumw2+pow(histogram.GetBinError(bin),2.)
    hist.SetName(histogram.GetName())
    hist.SetEntries(histogram.GetEntries())

    #if(directory.GetName()=="Data" and directory.GetMotherDir().GetName()=="mh15_vbf_tight_signal")
    #for bin in range(0,histogram.GetNbinsX()+2):
    return hist

def getNewRegionName(regionname):
    newregionname=regionname.replace("eh15","lh15")
    newregionname=newregionname.replace("mh15","lh15")
    newregionname=newregionname.replace("eh16","lh16")
    newregionname=newregionname.replace("mh16","lh16")
    return newregionname
    #return regionname

def writeNewFile(infile,outfile,regions):
    infile.cd()
    allregions=[]
    nextregionkey = TIter(gDirectory.GetListOfKeys())
    regionkey = nextregionkey.Next()
    histograms={}
    while regionkey and regionkey.GetTitle():
        if regionkey.IsFolder() == 1:
            regionname=regionkey.GetName()
            newregionname=getNewRegionName(regionname)
            histograms[newregionname]={}
            allregions.append(regionname)
            inregiondir=infile.Get(regionname)
            inregiondir.cd()
            nextsamplekey = TIter(gDirectory.GetListOfKeys())
            samplekey = nextsamplekey.Next()
            while samplekey:
                if samplekey.IsFolder() == 1:
                    samplename=samplekey.GetName()
                    insampledir=inregiondir.Get(samplename)                    
                    insampledir.cd()
                    hist=insampledir.Get("nominal")
                    if samplename in histograms[newregionname]:
                        histograms[newregionname][samplename].Add(hist)
                    else:
                        histograms[newregionname][samplename]=hist
                samplekey = nextsamplekey.Next()
        regionkey = nextregionkey.Next()

    #get the binning
    binning={}
    for newregion in  histograms.keys():
        binning[newregion]=getBinning(histograms[newregion])
        print "Binning for region ", newregion, " -> ", binning[newregion]

    #now write output file
    infile.cd()
    nextregionkey = TIter(gDirectory.GetListOfKeys())
    regionkey = nextregionkey.Next()
    while regionkey and regionkey.GetTitle():
        if regionkey.IsFolder() == 1:
            regionname=regionkey.GetName()
            newregionname=getNewRegionName(regionname)
            outfile.cd()
            outfile.mkdir(regionname)
            outregiondir=outfile.Get(regionname)
            infile.cd()
            inregiondir=infile.Get(regionname)
            inregiondir.cd()
            nextsamplekey = TIter(gDirectory.GetListOfKeys())
            samplekey = nextsamplekey.Next()
            while samplekey:
                if samplekey.IsFolder() == 1:
                    samplename=samplekey.GetName()
                    outregiondir.cd()
                    outregiondir.mkdir(samplename)
                    outsampledir=outregiondir.Get(samplename)
                    inregiondir.cd()
                    insampledir=inregiondir.Get(samplename)
                    insampledir.cd()
                    nextsystkey = TIter(gDirectory.GetListOfKeys())
                    systkey = nextsystkey.Next()
                    while systkey:
                        obj = systkey.ReadObj()
                        if obj.IsA().InheritsFrom("TH1"):
                            systname=systkey.GetName()
                            outsampledir.cd()
                            outhist=rebin(obj,binning[newregionname],outsampledir)
                            outhist.Write()
                        systkey = nextsystkey.Next()
                else: #take care of lumi histogram
                    obj = samplekey.ReadObj()
                    if obj.IsA().InheritsFrom("TH1"):
                        newobj=obj.Clone()
                        outregiondir.cd()
                        newobj.SetDirectory(outregiondir)
                        newobj.Write()
                samplekey = nextsamplekey.Next()
        regionkey = nextregionkey.Next()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Description of your program')
    parser.add_argument('-i','--infile', help='Input file', required=True)
    parser.add_argument('-o','--outfile', help='Output file', required=True)
    args = vars(parser.parse_args())
    
    infilename=args['infile'] #"test200bins.root"
    outfilename=args['outfile'] #"out.root"
    
    rootfile=TFile.Open(infilename,"READ")
    outfile=TFile.Open(outfilename,"RECREATE")
    regions=getRegions(rootfile)
    writeNewFile(rootfile,outfile,regions)
