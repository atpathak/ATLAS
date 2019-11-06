#!/usr/bin/env python2

import argparse
import sys
import imp
import systematicsHandler

import os, os.path

alias = "readAnalysis"

def main(args):
  # print a welcome message
  print(TQStringUtils.makeBoldWhite("reading analysis results"))

  # open the configfile
  INFO("reading configuration for '{:s}' from file '{:s}'".format(alias,args.cfgname))
  configreader = TQConfigReader(alias,args.cfgname)
  configreader.resolve()

  # parse config options from the command line
  if args.options:
    for s in args.options:
      opt = s.split(":",1)
      WARN("using config option '{:s} = {:s}' from command line".format(opt[0],opt[1]))
      configreader.set(alias+"."+opt[0],opt[1])  

  # export the config options to a taggable object    
  config = configreader.exportAsTags()
  if not config:
    BREAK("unable to retrieve configuration from file '{:s}' - please check the filename".format(args.cfgname))
  if config.getNTags() < 3:
    BREAK("very few configuration options were read -- are you sure you are reading the correct config file?")
  
  
  # load the sample folder
  sfinfilename = config.getTagStringDefault("inputFile","input/samples_cutbased.root:samples");
  systematics=systematicsHandler.checkForSystematics (config)
  if systematics:
      sfinfilename=systematics.changeFileName(sfinfilename)
  INFO("loading sample folder '{:s}'".format(sfinfilename))
  samples = TQSampleFolder.loadSampleFolder(sfinfilename)
  if not samples:
    BREAK("unable to read sample folder '{:s}' - please check input path".format(sfinfilename))

  # write the configuration to the info/config subfolder
  info = samples.getFolder("info/config/readAnalysis+")
  info.setInfoTags()
  config.exportTags(info)

  # get some information
  energy = samples.getTagStringDefault("energy","(?)") + TString(" ") + samples.getTagStringDefault("energyUnit","TeV")
  lumiValue = samples.getTagDoubleDefault("luminosity",-1.)
  lumiValue15 = config.getTagDoubleDefault("luminosity2015",-1.)
  lumiValue16 = config.getTagDoubleDefault("luminosity2016",-1.)
  luminosity = TString("")
  luminosity15 = TString("")
  luminosity16 = TString("")
  lumiUnit = samples.getTagStringDefault("luminosityUnit","pb")
  displayLumiUnit = config.getTagStringDefault("displayLuminosityUnit","fb")
  if lumiValue < 0:
    luminosity = "(?)"
  else:
    luminosity = TQStringUtils.formatSignificantDigits(TQXSecParser.convertUnit(lumiValue,lumiUnit,displayLumiUnit),4) + " "+displayLumiUnit+"^{-1}"

  if lumiValue15 < 0:
    luminosity15 = luminosity
    ERROR("Luminosity 2015 not found in config, using default luminosity.")
  else:
    luminosity15 = TQStringUtils.formatSignificantDigits(TQXSecParser.convertUnit(lumiValue15,lumiUnit,displayLumiUnit),4) + " "+displayLumiUnit+"^{-1}"

  if lumiValue16 < 0:
    luminosity16 = luminosity
    ERROR("Luminosity 2016 not found in config, using default luminosity.")
  else:
    luminosity16 = TQStringUtils.formatSignificantDigits(TQXSecParser.convertUnit(lumiValue16,lumiUnit,displayLumiUnit),4) + " "+displayLumiUnit+"^{-1}"

    
  # get the list of channels
  rawchannels = config.getTagVString("channels")
  channels = []
  prettychan = {"ee" : "e\\nu e\\nu","mm":"\\mu\\nu\\mu\\nu","em":"e\\nu\\mu\\nu","me":"\\mu\\nu e\\nu", "emme": "e\\nu\\mu\\nu+\\mu\\nu e\\nu", "eemm": "e\\nu e\\nu+\\mu\\nu\\mu\\nu", "ll":"l\\nu l\\nu","trilep":"trilep","quadlep":"quadlep","ehad":"ehad","muhad":"muhad", "lhVBF":"lephad VBF", "lhBoosted":"lephad Boosted", "lephad":"\\mu\\tau+e\\tau", "mtau":"\\mu\\tau", "etau":"e\\tau"}

  pathchan = {"ee":"ee","mm":"mm","em":"em","me":"me","emme":"[em+me]","eemm":"[ee+mm]","ll":"?","trilep":"trilep","quadlep":"quadlep","ehad":"ehad","muhad":"muhad", "lhVBF":"lhVBF", "lhBoosted":"lhBoosted", "lephad":"?", "mtau":"mtau", "etau":"etau"}
  for craw in rawchannels:
    c = TString(craw)
    c.ReplaceAll("+","")
    c.ReplaceAll("[","")
    c.ReplaceAll("]","")
    c.ReplaceAll("'","")
    channels.append(c.Data())

  # set a bunch of tags
  errMsg = TString()
  for style in  config.getTagVString("style"):
    if not samples.importFromTextFile(style,errMsg):
      ERROR("error importing style tags from '{:s}': {:s}".format(style,errMsg))
    else:
      INFO("successfully imported style tags from '{:s}'".format(style))    

  # parse the output path
  outputdir = config.getTagStringDefault("outputDir","results")
  sfoutfilename = config.getTagStringDefault("outputFile",TQFolder.concatPaths(outputdir,"samples_cutbased.root:samples"));
  outfilename = TString("samples.root")
  sfoutname = TString("samples")
  if systematics:
      outputdir=outputdir+"_"+systematics.alias
      sfoutfilename=systematics.changeFileName(sfoutfilename)
      outfilename=systematics.changeSampleName(outfilename)
      sfoutname=systematics.changeSampleName(sfoutname)
  if not TQFolder.parseLocation(sfoutfilename,outfilename,sfoutname):
    BREAK("cannot parse folder location/name from '{:s}'".format(sfoutfilename))

  # generate the normalization factors
  nfconfigs = config.getTagVString("nfConfig")
  cl = TQNFChainloader(nfconfigs,samples)
  cl.importTagsWithoutPrefix(config,"NF.")
  cl.initialize()
  cl.execute()
  cl.finalize()
    
  # initialize some classes
  reader = TQSampleDataReader(samples)
  
  # create a cut diagram
  INFO("creating cut diagram")
  cuts = TQCut.importFromFolder(samples.getFolder("info/cuts/?"))
  cuttextags = TQTaggable()
  cuttextags.importTagsWithoutPrefix(config,"cutprinter.")
  cuts.writeDiagramToFile(TQFolder.concatPaths(outputdir,"cuts.tex"),cuttextags)
            
  # make all cutflows
  cutflowprinter = TQCutflowPrinter(samples)
  if config.getTagBoolDefault("makeCutflows",False):
    processfiles = config.getTagVString("cutflowProcessFiles")
    cutfiles = config.getTagVString("cutflowCutFiles")
    cutflowtags = TQTaggable()
    cutflowtags.setTagBool("ensureDirectory",True)
    cutflowtags.importTagsWithoutPrefix(config,"cutflowprinter.")
    for c in channels:
      cutflowtags.setTagString("channel",pathchan[c])
      cutflowtags.setTagString("lepch",pathchan[c]);
      cutflowtags.setTagString("style.title.latex",config.getTagStringDefault("cutflowtitle","\\ensuremath{{\\sqrt{{s}}={:s}}}, \\ensuremath{{\\mathcal{{L}}={:s}}}, \\textbf{{{:s}}}".format(energy,luminosity,"\\ensuremath{"+prettychan[c]+"}" if ("\\" in prettychan[c]) else prettychan[c] )))
      cutflowtags.setTagString("style.title.html","&radic;<i>s</i>={:s}, <i>L</i>={:s} <b>{:s}</b>".format(energy,
                                                                                                                TQStringUtils.convertLaTeX2HTML(luminosity).Data(),
                                                                                                                TQStringUtils.convertLaTeX2HTML(prettychan[c]).Data()))
      for processfile in processfiles:
        processname = TString(processfile(processfile.Last('/')+1,processfile.Last('.')-processfile.Last('/')-1))
        cutflowprinter.resetProcesses()
        cutflowprinter.importProcessesFromFile(processfile)
        if not samples.getSampleFolder("data"):
          cutflowprinter.removeProcessesByName("*data*")
        for cutfile in cutfiles:
          cutname = TString(cutfile(cutfile.Last('/')+1,cutfile.Last('.')-cutfile.Last('/')-1))
          cutflowprinter.resetCuts()
          cutflowprinter.importCutsFromFile(cutfile)
          INFO("making cutflow '{:s}'/'{:s}' in {:s}-channel".format(processname,cutname,c))
          cutflow = cutflowprinter.createTable(cutflowtags)
          cutflow.importTags(cutflowtags)
          cutflow.writeCSV  ("{:s}/cutflows/{:s}-{:s}-{:s}.csv" .format(outputdir,c,processname,cutname),cutflowtags)
          cutflow.writeHTML ("{:s}/cutflows/{:s}-{:s}-{:s}.html".format(outputdir,c,processname,cutname),cutflowtags)
          cutflow.writeLaTeX("{:s}/cutflows/{:s}-{:s}-{:s}.tex" .format(outputdir,c,processname,cutname),cutflowtags)

  # print the event lists
  eventlistprinter = TQEventlistPrinter(samples)
  evtlists = config.getTagVString("makeEventlists")
  for evtlist in evtlists:
    processfiles = config.getTagVString("eventlistProcessFiles")
    cutfiles = config.getTagVString("eventlistCutFiles")
    eventlisttags = TQTaggable()
    eventlisttags.setTagBool("ensureDirectory",True)
    eventlisttags.setTagBool("adjustColWidth",True)
    eventlisttags.importTagsWithoutPrefix(config,"eventlistprinter.")
    for c in channels:
      eventlisttags.setTagString("channel",pathchan[c])
      eventlisttags.setTagString("lepch",pathchan[c]);
      for processfile in processfiles:
        processname = TString(processfile(processfile.Last('/')+1,processfile.Last('.')-processfile.Last('/')-1))
        eventlistprinter.resetProcesses()
        eventlistprinter.importProcessesFromFile(processfile)
        if not samples.getSampleFolder("data"):
          eventlistprinter.removeProcessesByName("*data*")
        for cutfile in cutfiles:
          cutname = TString(cutfile(cutfile.Last('/')+1,cutfile.Last('.')-cutfile.Last('/')-1))
          eventlistprinter.resetCuts()
          eventlistprinter.importCutsFromFile(cutfile)
          INFO("making eventlist '{:s}'/'{:s}' in {:s}-channel".format(processname,cutname,c))
          eventlistprinter.writeEventlists(evtlist,TQFolder.concatPaths(outputdir,"eventlists"),eventlisttags)
          
  # make all plots
  histograms = reader.getListOfHistogramNames()
  if args.select:
      WARN("select plots for plotting: "+args.select)
      list=[ h.GetName() for h in histograms ]
      INFO("have "+str(len(list))+" plots")
      start=None
      end=None
      split=None
      select=str(args.select)
      if (len(list)<100):
          select=select.replace("p","")
          INFO ("number of plots is <100, strip p: "+select)
      if "-" in select:
          split=[ s.strip() for s in  select.split("-") ]
          INFO(select+ " is a range "+str(split))
      else:
          INFO(select+ " is not a range ")
      if "p" in select:
          INFO(select+ " has p ")
          if split:
              try:
                  start=int(split[0].strip("p"))
                  end=int(split[1].strip("p"))
                  INFO(select+ " is p range from "+str(start)+ " " +str(end))
              except:
                  INFO(select+ " cannot recognise the numbers "+str(split))
                  pass
          else:
              try:
                  start=int(select.strip("p"))
                  end=start+1
                  INFO(select+ " is p "+str(start)+ "p to " +str(end)+"p")
              except:
                  INFO(select+ " cannot recognise number "+select)
                  pass
          start=int(len(list)/100.0*start)
          end=int(len(list)/100.0*end)
      else:
          INFO(select+ " is just a number ")
          if split:
              try:
                  start=int(split[0].strip("p"))
                  end=int(split[1].strip("p"))
                  INFO(select+ " is range from "+str(start)+ " to " +str(end))
              except:
                  INFO(select+ " cannot recognise the numbers "+str(split))
                  pass
          else:
              try:
                  start=int(select.strip("p"))
                  end=start+1
                  INFO(select+ " is p "+str(start)+ " " +str(end))
              except:
                  INFO(select+ " cannot recognise number "+select)
                  pass

      INFO(select+ " range from "+str(start)+ " " +str(end))
      newhistograms=[]
      for n in xrange(len(histograms)):
          if (n>=start and n< end):
              newhistograms.append(histograms[n])
      histograms=newhistograms
      INFO("will have only "+str(len(histograms))+" plots")

  plotfilter = config.getTagVString("makePlots")
  if histograms and not len(plotfilter) < 1:
    INFO("making plots")
    plotter = TQHWWPlotter(samples)
    plotter.loadSystematics(config.getTagStringDefault("systematics"))
    plotter.setNormalizationInfo("info/normalization")   
    plottags = TQTaggable()
    plottags.importTagsWithoutPrefix(config,"plotter.")
    if config.getTagBoolDefault("useEXIF",False):
      if TQLibrary.enableEXIFsupport():
        INFO("using EXIF")
      else:
        WARN("unable to configure EXIF")
    if config.getTagBoolDefault("embedPDFfonts",False):
      if TQLibrary.enablePDFfontEmbedding():
        INFO("using embedFonts")
        plottags.setTagBool("embedfonts",True)
      else:
        WARN("unable to configure font embedding")
    plotformats = config.getTagVString("plotformat")
    if len(plotformats) < 1:
      plotformats.push_back("pdf")
    plottags.setTagBool("ensureDirectory",True)
    plottags.setTagString("labels.0", TString("#sqrt{s} = ")+energy+TString(", #lower[-0.2]{#scale[0.6]{#int}} Ldt = ")+luminosity)
    histstyle = TString()
    if config.getTagString("histogramScheme",histstyle):
      if plotter.setScheme(histstyle):
        INFO("using histogram scheme '{:s}' for plotting".format(histstyle))
      else:
        ERROR("failed to use histogram scheme '{:s}' - unknown scheme".format(histstyle))
    if config.getTagString("histogramProcesses",histstyle):
      plotter.clearProcesses()
      if plotter.importProcessesFromFile(histstyle):
        INFO("reading histogram processes from  file '{:s}'".format(histstyle))
      else:
        ERROR("failed to read histogram processes from file '{:s}'".format(histstyle))
    if not samples.getSampleFolder("data") or not config.getTagBoolDefault("showData",True):
      INFO("removing data from plots")
      plotter.removeProcessesByName("*data*")
    plotchannels = []
    for ch in channels:
      c = pathchan[ch]
      plottesttags = TQTaggable()
      plottesttags.setTagString("lepch",c)
      plottesttags.setTagString("channel",c)
      ok = True
      for process in plotter.getListOfProcessNames():
        path = plottesttags.replaceInText(plotter.getProcessPath(process.GetName()))
        sflist = reader.getListOfSampleFolders(path)
        if not sflist or sflist.GetEntries() < 1:
          ok = False
          break
      if ok:
        plotchannels.append(ch)
      else:
        WARN("scheduled channel '{:s}', but didn't find sample folder '{:s}' in sample folder -- skipping channel!".format(c,path))
    for h in histograms:
      makeplot = False
      for pf in plotfilter:
        if TQStringUtils.matches(h.GetName(),TQStringUtils.trim(pf)):
          makeplot = True
      if not makeplot:
        INFO("skipping histogram '{:s}'".format(h.GetName()))
        continue
      sanename = TQFolder.makeValidIdentifier(h.GetName(),"-")
      if "2015-" in str(sanename):
        plottags.setTagString("labels.0", TString("#sqrt{s} = ")+energy+TString(", #lower[-0.2]{#scale[0.6]{#int}} Ldt = ")+luminosity15)
      elif "2016-" in str(sanename):
        plottags.setTagString("labels.0", TString("#sqrt{s} = ")+energy+TString(", #lower[-0.2]{#scale[0.6]{#int}} Ldt = ")+luminosity16)
      else:
        plottags.setTagString("labels.0", TString("#sqrt{s} = ")+energy+TString(", #lower[-0.2]{#scale[0.6]{#int}} Ldt = ")+luminosity)

      for c in plotchannels:
        plottags.setTagString("input.lepch",pathchan[c]);
        plottags.setTagString("input.channel",pathchan[c]);
        plottags.setTagString("labels.1", TString("H#rightarrow")+TQStringUtils.convertLaTeX2ROOTTeX(prettychan[c]));
        for plotformat in plotformats:
          plottags.setTagBool("style.showRatio",True)
          plottags.setTagBool("style.logScale",False)
          histname = "{:s}/plots/{:s}-{:s}-lin.{:s}".format(outputdir,c,sanename,plotformat)
          if not plotter.plotAndSaveAs(h.GetName(),histname,plottags):
            WARN("error plotting '{:s}'".format(histname))
          if config.getTagBoolDefault("logScale",True):
            plottags.setTagBool("style.logScale",True)
            histname = "{:s}/plots/{:s}-{:s}-log.{:s}".format(outputdir,c,sanename,plotformat)
            if not plotter.plotAndSaveAs(h.GetName(),histname,plottags):
              WARN("error plotting '{:s}'".format(histname))

  # write the results
  START("l.","writing post-read sample folder '{:s}' to '{:s}'".format(sfoutname,outfilename))
  if TQUtils.ensureDirectoryForFile(sfoutfilename) and (samples.writeToFile(outfilename,True) > 0):
    END(TQMessageStream.OK)
  else:
    END(TQMessageStream.FAIL)

if __name__ == "__main__":
  # parse the CLI arguments
  parser = argparse.ArgumentParser(description='Create plots and cutflows for the HSG3 Run2 Analysis.')
  parser.add_argument('cfgname', metavar='CONFIG', type=str, 
            default="config/readCutBased.cfg",
            help='path to the analysis config file to be processed')
  parser.add_argument('--options', metavar='KEY:VALUE', type=str, nargs='*',
            default="", 
            help='set (overwrite) an option with respect to the config file')
  parser.add_argument('--select', metavar="SELECT", dest="select", default=None, 
            help='restrict the analysis to a number, use % for percentage, use - for range')
  args = parser.parse_args()
  
  sys.path=[os.getcwd()]+sys.path

  from QFramework import *  
  from ROOT import *

  TQLibrary.getQLibrary().setApplicationName(alias);

  gROOT.SetBatch(True)
  main(args);
