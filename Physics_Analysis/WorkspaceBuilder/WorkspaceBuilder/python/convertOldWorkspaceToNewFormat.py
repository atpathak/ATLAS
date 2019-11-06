#!/usr/bin/python
from xml.dom import minidom
from ROOT import TFile, TH1F, TCanvas, TObject

channelfilename=""
channelname=""
variablename="BDT"
channelconvertmap={ "ll11_boost":"ll11_boost_signal",
                    "ll12_boost":"ll12_boost_signal",
                    "ll11_btop" :"ll11_boost_top",
                    "ll12_btop" :"ll12_boost_top",
                    "ll11_bzll" :"ll11_boost_zll",
                    "ll12_bzll" :"ll12_boost_zll",
                    "ll11_vbf"  :"ll11_vbf_signal",
                    "ll12_vbf"  :"ll12_vbf_signal",
                    "ll11_vtop" :"ll11_vbf_top",
                    "ll12_vtop" :"ll12_vbf_top",
                    "ll11_vzll" :"ll11_vbf_zll",
                    "ll12_vzll" :"ll12_vbf_zll",

                    "lh11_boost":"lh11_boost_signal",
                    "lh12_boost":"lh12_boost_signal",
                    "lh11_btop" :"lh11_boost_top",
                    "lh12_btop" :"lh12_boost_top",
                    "lh11_vbf"  :"lh11_vbf_signal",
                    "lh12_vbf"  :"lh12_vbf_signal",
                    "lh11_vtop" :"lh11_vbf_top",
                    "lh12_vtop" :"lh12_vbf_top"
                   }
sampleconvertmap={"VBF" : "VBFH", "Zl2t" : "Zll"}
systematicconvertmap={"from" : "to"}
inputpath="../othercombinationcode/workspaces_Dec11_2014/" #with / at the end

def readChannel(xmldoc):
    # read the information from the channel XML tag and set the channelname, channelfilename and return a root file pointer
    channels = xmldoc.getElementsByTagName('Channel')
    if channels[0].hasAttribute('Name'):
        global channelname
        channelname=channels[0].attributes['Name'].value
        if channels[0].hasAttribute('InputFile'):
            global channelfilename
            channelfilename =channels[0].attributes['InputFile'].value
            channelrootfile=TFile.Open(inputpath+channelfilename)
            print "Found channel ", channelname, " with file ", channelfilename
            return channelrootfile

    return 0

def readData(xmldoc, inrootfile, outrootfile,newchannelname):
    # read the information from the data XML tag and write the histogram to the new output file
    data = xmldoc.getElementsByTagName('Data')
    if data[0].hasAttribute('HistoName'):
        histname=data[0].attributes['HistoName'].value
        if data[0].hasAttribute('HistoPath'):
            histname=data[0].attributes['HistoPath'].value+data[0].attributes['HistoName'].value
        hist=inrootfile.Get(histname)
    channeldir=outrootfile.Get(newchannelname)
    datadir=channeldir.mkdir("Data")
    datadir.cd()
    hist.SetDirectory(datadir)
    hist.SetName("nominal")
    hist.SetTitle("nominal")
    hist.Write()

def readSample(xmlsample, inrootfile, outrootfile,newchannelname):
    # reads the data from teh Sample tag, writes the histogram and initiates the systematics treatment
    samplename=xmlsample.attributes['Name'].value
    if xmlsample.hasAttribute("HistoName"):
        histname=xmlsample.attributes['HistoName'].value
        if xmlsample.hasAttribute('HistoPath'):
            histname=xmlsample.attributes['HistoPath'].value+xmlsample.attributes['HistoName'].value
        hist=inrootfile.Get(histname)
        channeldir=outrootfile.Get(newchannelname)
        newsamplename=getNewSampleName(samplename)
        sampledir=channeldir.mkdir(newsamplename)
        sampledir.cd()
        hist.SetDirectory(sampledir)
        hist.SetName("nominal")
        hist.SetTitle("nominal")
        hist.Write()
        normalisation=hist.GetSumOfWeights()
        readSystematicsForSample(xmlsample,normalisation,inrootfile,outrootfile,newchannelname,newsamplename)
    else:
        print "ERROR: No histogram found for sample ", samplename

def writeSystHisto(systname,inrootfile,outrootfile,histname,newchannelname,newsamplename):
    hist=inrootfile.Get(histname)
    channeldir=outrootfile.Get(newchannelname)
    #sampledir=channeldir.Get(newsamplename)
    systematicdir=channeldir.Get(systname)
    if systematicdir==None:
        systematicdir=channeldir.mkdir(systname)
    systematicdir.cd()
    hist.SetDirectory(systematicdir)
    hist.SetName(newsamplename)
    hist.SetTitle(newsamplename)
    hist.Write()

def getHistoSys(xmlsample,normalisation,inrootfile,systematicname,newchannelname):
    histosyslist = xmlsample.getElementsByTagName('HistoSys')
    for histosys in histosyslist:
        if histosys.attributes["Name"].value==systematicname:
            histnamehigh=histosys.attributes["HistoNameHigh"].value
            if histosys.hasAttribute('HistoPathHigh'):
                histnamehigh=histosys.attributes['HistoPathHigh'].value+histosys.attributes['HistoNameHigh'].value
            if histosys.hasAttribute("HistoFileHigh"):
                tmprootfile=TFile.Open(inputpath+histosys.attributes["HistoFileHigh"].value,"READ")
                highhist=tmprootfile.Get(histnamehigh)
                highhist.SetDirectory(0)
                tmprootfile.Close()
            else:
                highhist=inrootfile.Get(histnamehigh)
                highhist.SetDirectory(0)
            
            histnamelow=histosys.attributes["HistoNameLow"].value
            if histosys.hasAttribute('HistoPathLow'):
                histnamelow=histosys.attributes['HistoPathLow'].value+histosys.attributes['HistoNameLow'].value
            if histosys.hasAttribute("HistoFileLow"):
                tmprootfile=TFile.Open(inputpath+histosys.attributes["HistoFileLow"].value,"READ")
                lowhist=tmprootfile.Get(histnamelow)
                #if systematicname=="ATLAS_ANA_EMB_ISOL_2012":
                #    print "JULIAN1:", lowhist.GetNbinsX(),inputpath+histosys.attributes["HistoFileLow"].value, histnamelow
                lowhist.SetDirectory(0)
                #writeSystHisto(newsystematicname+"Low",tmprootfile,outrootfile,histnamelow,newchannelname,newsamplename)
                tmprootfile.Close()
            else:
                lowhist=inrootfile.Get(histnamelow)
                #if systematicname=="ATLAS_ANA_EMB_ISOL_2012":
                #    print "JULIAN2:", lowhist.GetNbinsX()
                lowhist.SetDirectory(0)
                #writeSystHisto(newsystematicname+"Low",inrootfile,outrootfile,histnamelow,newchannelname,newsamplename)
            #if systematicname=="ATLAS_ANA_EMB_ISOL_2012":
            #    print "JULIANHistosys", xmlsample.attributes['Name'].value, lowhist.GetNbinsX(), highhist.GetNbinsX()
                
            if abs(lowhist.GetSumOfWeights()-highhist.GetSumOfWeights()>0.001) or abs(lowhist.GetSumOfWeights()-normalisation)>0.001:
                print "ERROR: Histosys histogram with unequal contents: ", newchannelname, xmlsample.attributes['Name'].value, systematicname, lowhist.GetSumOfWeights(), highhist.GetSumOfWeights(), normalisation
            return [lowhist, highhist]

    lowhist=TH1F(xmlsample.attributes['Name'].value+systematicname+"_Low",xmlsample.attributes['Name'].value+systematicname+"_Low",1,-0.5,0.5)
    lowhist.SetDirectory(0)
    lowhist.SetBinContent(1,normalisation)
    highhist=TH1F(xmlsample.attributes['Name'].value+systematicname+"_High",xmlsample.attributes['Name'].value+systematicname+"_High",1,-0.5,0.5)
    highhist.SetDirectory(0)
    highhist.SetBinContent(1,normalisation)
    #if systematicname=="ATLAS_ANA_EMB_ISOL_2012":
    #    print "JULIANOverallsys", xmlsample.attributes['Name'].value, lowhist.GetNbinsX(), highhist.GetNbinsX()

    return [lowhist,highhist]
                    
def getOverallSys(xmlsample,systematicname):
    overallsyslist = xmlsample.getElementsByTagName('OverallSys')
    for overallsys in overallsyslist:
        if overallsys.attributes["Name"].value==systematicname:
            high=float(overallsys.attributes["High"].value)
            low=float(overallsys.attributes["Low"].value)
            return [low,high]
    return [1.,1.]

def readSystematicsForSample(xmlsample,normalisation,inrootfile,outrootfile,newchannelname,newsamplename):
    # reads the HistoSys and OverallSys tags and writes histograms to the output root file
    
    # get the list of systematics
    systematics=[]
    histosyslist = xmlsample.getElementsByTagName('HistoSys')
    for histosys in histosyslist:
        systematicname=histosys.attributes["Name"].value
        systematics.append(systematicname)
    overallsyslist = xmlsample.getElementsByTagName('OverallSys')
    for overallsys in overallsyslist:
        systematicname=overallsys.attributes["Name"].value
        systematics.append(systematicname)

    systematics=list(set(systematics)) #make unique

    print "Writing sample ", newsamplename, " with systematics ",  systematics

    for systematicname in systematics:
        newsystematicname=getNewSystematicName(systematicname)
        histosys=getHistoSys(xmlsample,normalisation,inrootfile,systematicname,newchannelname)
        overallsys=getOverallSys(xmlsample,systematicname)

        #if histosys[0].GetSumOfWeights()-

        inrootfile.cd()
        #scale the histograms
        histosys[0].Scale(overallsys[0])
        histosys[1].Scale(overallsys[1])

        #if newsamplename=="ggH" and systematicname=="ATLAS_EL_SCALE":
        #    print "JULIAN:", overallsys, histosys[0].GetSumOfWeights(), histosys[1].GetSumOfWeights()

        #write these histograms
        channeldir=outrootfile.Get(newchannelname)
        sampledir=channeldir.Get(newsamplename)
        sampledir.cd()
        #systematicdirhigh=channeldir.Get(newsystematicname+"_High")
        #if systematicdirhigh==None:
        #    systematicdirhigh=channeldir.mkdir(newsystematicname+"_High")
        #systematicdirhigh.cd()
        #histosys[1].SetDirectory(systematicdirhigh)
        #histosys[1].SetName(newsamplename)
        histosys[1].SetDirectory(sampledir)
        histosys[1].SetName(newsystematicname+"_high")
        histosys[1].SetTitle(newsystematicname+"_high")
        histosys[1].Write(histosys[1].GetName(),TObject.kOverwrite)

        #channeldir=outrootfile.Get(newchannelname)
        #systematicdirlow=channeldir.Get(newsystematicname+"_Low")
        #if systematicdirlow==None:
        #    systematicdirlow=channeldir.mkdir(newsystematicname+"_Low")
        #systematicdirlow.cd()
        #histosys[0].SetDirectory(systematicdirlow)
        #histosys[0].SetName(newsamplename)
        histosys[0].SetDirectory(sampledir)
        histosys[0].SetName(newsystematicname+"_low")
        histosys[0].SetTitle(newsystematicname+"_low")
        histosys[0].Write(histosys[0].GetName(),TObject.kOverwrite)

def getNewChannelName(oldchannelname):
    if oldchannelname in channelconvertmap:
        return channelconvertmap[oldchannelname]
    return oldchannelname

def getNewSampleName(oldsamplename):
    if oldsamplename in sampleconvertmap:
        return sampleconvertmap[oldsamplename]
    return oldsamplename

def getNewSystematicName(oldsystematicname):
    if oldsystematicname in systematicconvertmap:
        return systematicconvertmap[oldsystematicname]

    systematicname=oldsystematicname
    #return systematicname.replace("ATLAS_","")
    return systematicname

#return systematicname

def analyzeFile(filename):
    xmldoc = minidom.parse(filename)
    # read the information from the channel XML tag
    inrootfile=readChannel(xmldoc)
    print "ChannelName:", channelname
    newchannelname=getNewChannelName(channelname)
    #open the output root file
    outrootfile=TFile.Open(newchannelname+"_"+variablename+".root","RECREATE")
    #create the channel name directory
    outrootfile.mkdir(newchannelname)
    # read the information from the data XML tag
    readData(xmldoc, inrootfile,outrootfile,newchannelname)
    
    # loop over the samples
    samplelist = xmldoc.getElementsByTagName('Sample')
    for sample in samplelist:
        readSample(sample,inrootfile,outrootfile,newchannelname)

filenames=[ inputpath+"config_2/ll_boost_mH_2011.xml",
            inputpath+"config_2/ll_boost_mH_2012.xml",
            inputpath+"config_2/ll_btop_mH_2011.xml",
            inputpath+"config_2/ll_btop_mH_2012.xml",
            inputpath+"config_2/ll_bzll_mH_2011.xml",
            inputpath+"config_2/ll_bzll_mH_2012.xml",
            inputpath+"config_2/ll_vbf_mH_2011.xml",
            inputpath+"config_2/ll_vbf_mH_2012.xml",
            inputpath+"config_2/ll_vtop_mH_2011.xml",
            inputpath+"config_2/ll_vtop_mH_2012.xml",
            inputpath+"config_2/ll_vzll_mH_2011.xml",
            inputpath+"config_2/ll_vzll_mH_2012.xml",
            inputpath+"config_2/lh_boost_mH_2011.xml",
            inputpath+"config_2/lh_boost_mH_2012.xml",
            inputpath+"config_2/lh_btop_mH_2011.xml",
            inputpath+"config_2/lh_btop_mH_2012.xml",
            inputpath+"config_2/lh_vbf_mH_2011.xml",
            inputpath+"config_2/lh_vbf_mH_2012.xml",
            inputpath+"config_2/lh_vtop_mH_2011.xml",
            inputpath+"config_2/lh_vtop_mH_2012.xml",
            inputpath+"config_2/hh_boost_mH_2011.xml",
            inputpath+"config_2/hh_boost_mH_2012.xml",
            inputpath+"config_2/hh_rest_mH_2011.xml",
            inputpath+"config_2/hh_rest_mH_2012.xml",
            inputpath+"config_2/hh_vbf_mH_2011.xml",
            inputpath+"config_2/hh_vbf_mH_2012.xml"]

#filenames=[inputpath+"config_2/lh_vtop_mH_2012.xml"]

for filename in filenames:
    analyzeFile(filename)
