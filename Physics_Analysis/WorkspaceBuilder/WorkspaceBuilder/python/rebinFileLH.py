#!/usr/bin/python

from ROOT import TFile, TH1F, TCanvas, TObject, gROOT, gDirectory, TIter, TKey
from math import sqrt
from array import *
import argparse

maxstatlimits={"Ztt":0.3,"Fake":0.5,"Fakes":0.5} # the maximum allowed statistical uncertainty per bin
maxstatlimitsSig={"Ztt":0.5,"Fake":0.5}

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

def getGoodBinning(histograms):
    binborders = []
    print histograms.keys()
    histname = "ggH"
    old = 10000
    for bin in range(histograms.values()[0].GetNbinsX()+2):
        new = abs(histograms[histname].GetBinLowEdge(bin)- 125)
        middle = bin if new < old else middle
        old = new if new < old else old
        binborders.append(histograms[histname].GetBinLowEdge(bin))
    print middle, histograms[histname].GetBinLowEdge(middle)
    print "borders", binborders
    raw_input("")

    #for histname in histograms.keys():
    #    if histname not in thisbin:
    #        thisbin[histname]=0.
    #        thisbinuncert2[histname]=0.
    #    thisbin[histname]=thisbin[histname]+histograms[histname].GetBinContent(bin)
    #    thisbinuncert2[histname]=thisbinuncert2[histname]+pow(histograms[histname].GetBinError(bin),2.)
        
    # Range for acceptable
    minmid, maxmid = 70, 140
    value = {}
    uncsq = {}
    border = []
    for process in maxstatlimitsSig.keys():
        value[process] = 0#histograms[process].GetBinContent(middle)
        uncsq[process] = 0#pow(histograms[process].GetBinError(middle),2.)
    statuncertaintyreason="Reason:"
    print "Go up"
    for bin in range(max(abs(middle-histograms[histname].GetNbinsX()+2),middle)):
        for process in maxstatlimitsSig.keys():
            #statuncertaintyreason=statuncertaintyreason+" "+process+": "+str(value[process])+"+/-"+str(sqrt(uncsq[process]))
            #print bin, statuncertaintyreason
            value[process] += histograms[process].GetBinContent(middle+bin)
            uncsq[process] += pow(histograms[process].GetBinError(middle+bin),2.)
            print bin, process, value[process], ' +- ', sqrt(uncsq[process]), "ratio : ", sqrt(uncsq[process])/value[process]
    for process in maxstatlimitsSig.keys():
        value[process] = 0#histograms[process].GetBinContent(middle)
        uncsq[process] = 0#pow(histograms[process].GetBinError(middle),2.)
    print "Go down"
    for bin in range(max(abs(middle-histograms[histname].GetNbinsX()+2),middle)):
        for process in maxstatlimitsSig.keys():
            #statuncertaintyreason=statuncertaintyreason+" "+process+": "+str(value[process])+"+/-"+str(sqrt(uncsq[process]))
            #print bin, statuncertaintyreason
            value[process] += histograms[process].GetBinContent(middle-bin)
            uncsq[process] += pow(histograms[process].GetBinError(middle-bin),2.)
            print bin, process, value[process], ' +- ', sqrt(uncsq[process]), "ratio : ", sqrt(uncsq[process])/value[process]
    for process in maxstatlimitsSig.keys():
        value[process] = 0#histograms[process].GetBinContent(middle)
        uncsq[process] = 0#pow(histograms[process].GetBinError(middle),2.)
    print "Go both"
    for bin in range(max(abs(middle-histograms[histname].GetNbinsX()+2),middle)):
        for process in maxstatlimitsSig.keys():
            #statuncertaintyreason=statuncertaintyreason+" "+process+": "+str(value[process])+"+/-"+str(sqrt(uncsq[process]))
            #print bin, statuncertaintyreason
            value[process] += histograms[process].GetBinContent(middle-bin)
            uncsq[process] += pow(histograms[process].GetBinError(middle-bin),2.)
            value[process] += histograms[process].GetBinContent(middle-bin)
            uncsq[process] += pow(histograms[process].GetBinError(middle-bin),2.)
            print bin, process, value[process], ' +- ', sqrt(uncsq[process]), "ratio : ", sqrt(uncsq[process])/value[process]
    #for bin in range(max(abs(middle-histograms[histname].GetNbinsX()+2),middle)):
    #    # Start from 125 bin
    #    if middle - bin <
    #    statuncertaintyok=True
    #    statuncertaintyreason="Reason:"
    #    for process in maxstatlimitsSig.keys():
    #        if value[process]<=0 or sqrt(uncsq[process])/value[process] > maxstatlimitsSig[process]:
    #            statuncertaintyok = False
    #    if statuncertaintyok:
    #        border.append(histograms[histname].GetBinLowEdge(middle-bin))
    #        middle = middle - bin
    #    else:
    #        #statuncertaintyreason=statuncertaintyreason+" "+process+": "+str(value[process])+"+/-"+str(sqrt(uncsq[process]))
    #        #print statuncertaintyreason
    #        #print "ratio", sqrt(uncsq[process])/value[process] if value[process]>0 else "Div by 0"
    #        print "Adding +1 bin"
    #        for process in maxstatlimitsSig.keys():
    #            statuncertaintyreason=statuncertaintyreason+" "+process+": "+str(value[process])+"+/-"+str(sqrt(uncsq[process]))
    #            value[process] += histograms[process].GetBinContent(middle-bin)
    #            uncsq[process] += pow(histograms[process].GetBinError(middle-bin),2.)
    print border
    raw_input("")
    
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
    newregionname=newregionname.replace("ehAll","lhAll")
    newregionname=newregionname.replace("mhAll","lhAll")
    #newregionname=newregionname.replace("tight","incl")
    #newregionname=newregionname.replace("loose","incl")
    return newregionname
    #return regionname

def writeNewFile(infile,outfile,regions):
    infile.cd()
    allregions=[]
    nextregionkey = TIter(gDirectory.GetListOfKeys())
    regionkey = nextregionkey.Next()
    histograms={}
    while regionkey and regionkey.GetTitle():
        print regionkey
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
    for newregion in histograms.keys():
        #binning[newregion]=getBinning(histograms[newregion]) #based on the binning algorithm
        #        #ICHEP binning
        #        binning["lhAll_cba_boost_loose_signal"]  = [30.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 140.0, 230.0]
        #        binning["lhAll_cba_boost_tight_signal"] = [30.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 140.0, 230.0]
        #        binning["lhAll_cba_vbf_loose_signal"] = [30.0, 70.0, 80.0,  90.0, 100.0, 110.0, 130.0, 150.0, 230.0]
        #        binning["lhAll_cba_vbf_tight_signal"] = [30.0, 75.0, 95.0, 115.0, 125.0, 135.0, 150.0, 230.0]
        #        binning["lhAll_mva_boost_signal"]  = [-1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
        #        binning["lhAll_mva_vbf_signal"] =  [-1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 1.0]
        #Optimised binning
        binning["lhAll_cba_boost_loose_signal"] = [30.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 140.0, 150.0, 180.0, 230.0] #Optimisation MMC Ver7
        binning["lhAll_cba_boost_tight_signal"] = [30.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 140.0, 150.0, 180.0, 230.0] #Optimisation MMC Ver7
        binning["lhAll_cba_vbf_loose_signal"] = [30.0, 70.0, 80.0, 90.0, 100.0, 115.0, 135.0, 150.0, 230.0] #Optimisation MMC Ver7
        binning["lhAll_cba_vbf_tight_signal"] = [30.0, 70.0, 80.0, 90.0, 100.0, 115.0, 135.0, 150.0, 230.0] #Optimisation MMC Ver7
        binning["lhAll_mva_boost_signal"]  = [-1.0, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.775, 0.85, 0.925, 1.0] #Ver2
        binning["lhAll_mva_vbf_signal"] =  [-1.0, -0.85, -0.75, -0.65, -0.55, -0.45, -0.35, -0.25, -0.15, -0.05, 0.05, 0.15, 0.25, 0.35, 0.45, 0.55, 0.65, 0.75, 1.0] #Ver2
        #TopCRs for LH
        binning["lhAll_cba_boost_top"] = [0,1]
        binning["lhAll_cba_vbf_top"] = [0,1]
        binning["lhAll_mva_boost_top"] = [0,1]
        binning["lhAll_mva_vbf_top"] = [0,1]
        ##binning["lhAll_vbf_tight_signal"] = [30.0, 80.0, 95.0, 110.0, 130.0, 150.0, 230.0] #1 kyle fix
        print "Binning for region ", newregion, " -> ", binning[newregion]

    #now write output file
    infile.cd()
    nextregionkey = TIter(gDirectory.GetListOfKeys())
    regionkey = nextregionkey.Next()
    while regionkey and regionkey.GetTitle():
        print regionkey
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
                            #print binning[newregionname], newregionname
                            #if 'tight' or 'high' in newregionname:
                            #    goodbin = [float(x) for x in range(30,240,20)]
                            #    goodbin = [30.0, 90.0, 230.0]
                            #    print goodbin
                            #    outhist=rebin(obj,goodbin,outsampledir)
                            #elif 'loose' or 'low' in newregionname:
                            #    outhist=rebin(obj,goodbin,outsampledir)
                            #else:
                            #if "vbf" in newregionname and "tight" in newregionname:
                            outhist=rebin(obj,binning[newregionname],outsampledir)
                            #else: outhist = obj
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
