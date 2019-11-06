#!/usr/bin/env python2

import argparse
import sys
import imp
import importlib
import inspect
import re

import os, os.path

import systematicsHandler

# the alias is the 'appliation name' that will be dumped to the info tags of any
# sample folder created
alias = "runAnalysis"

def loadLibMVA(config):
  mvalibpath = config.getTagStringDefault("libMVA","libTMVA.so")
  START("l.","attempting to load MVA library from '{:s}'".format(mvalibpath.Data()))
  try:
    ROOT.gSystem.Load(mvalibpath.Data())
    END(TQMessageStream.OK)
    return True
  except:
    END(TQMessageStream.FAIL)
    BREAK("unable to load MVA library - please specify the proper path via the 'libMVA' config option")
    return False

def recursive_change_tree(sampleFolder, newtreename):
  """
  This functions crawls recursivly all sub samples of the given sample
  folder. If a sample is a terminal leaf, i.e. a real data sample and no
  container object, and therefore has the tag ".init.treename", the tag is
  updated to the given new treename. In order to tell the framework to use
  the new tree, also the setTreeLocation is called with th new treename.

  The effect of this function is that, when a TQAnalysisSampleVisitor is
  used, the newtreename will be used. In case of
  TQMultiChannelAnalysisSampleVisitor the behaviour is different and will
  most likly use the NOMINAL tree!
  """
  subfolders = sampleFolder.getListOfSampleFolders("?")
  try:
    for subfolder in subfolders:
      if "data" in subfolder:
        break
      treename = TString()
      if subfolder.getTagString(".init.treename", treename):
        subfolder.setTagString(".init.treename", newtreename)
        treelocation = subfolder.getTreeLocation()
        subfolder.setTreeLocation(subfolder.extractFilename(treelocation) + ":" + newtreename)
      else:
        recursive_change_tree(subfolder, newtreename)
  except TypeError:
    pass

def is_prefix(prefix, pattern):
  """
  This method checks whether the first argument is a prefix of the second one,
  where the second one can include the character ? to denote 'matches
  everything'.
  Leading or trainling slashs are ignored.
  >>> is_prefix("/bkg/lephad", "?")
  True
  >>> is_prefix("bkg/lephad/", "/?/")
  True
  >>> is_prefix("/bkg/lephad", None)
  True
  >>> is_prefix("/bkg/lephad_mu_scale_low", "bkg/lephad/Diboson")
  False
  >>> is_prefix("/bkg/lephad_mu_scale_low", "bkg/lephad_mu_scale_high/Diboson")
  False
  >>> is_prefix("/bkg/lephad_mu_scale_low", "bkg/lephad_mu_scale_low/Diboson")
  True
  >>> is_prefix("/bkg/lephad_mu_scale_low", "bkg/lephad_?/Diboson")
  True
  >>> is_prefix("/bkg/lephad_mu_scale_low", "bkg/lephad/Diboson")
  False
  >>> is_prefix("/bkg/lephad_mu_scale_low", "bkg/?/Diboson")
  True
  >>> is_prefix("/bkg/lephad_mu_scale_low", "/bkg/?/Diboson/")
  True
  """
  if pattern is None:
    return True
  
  if pattern.startswith("/"): pattern = pattern[1:]
  if pattern.endswith("/"): pattern = pattern[:-1]
  if prefix.startswith("/"): prefix = prefix[1:]
  if prefix.endswith("/"): prefix = prefix[:-1]

  pattern = ["^%s$" % p.replace("?", ".*") for p in pattern.split("/")]
  prefix = prefix.split("/")
  for i in range(min(len(pattern), len(prefix))):
    if not re.match(pattern[i], prefix[i]):
      return False
  else:
    return True

def main(args):
  dummy=args.dummy

  # print a welcome message
  print(TQStringUtils.makeBoldWhite("running analysis"))

  # open the configfile
  cfgname=TString(args.cfgname)
  INFO("reading configuration for '{:s}' from file '{:s}'".format(alias,cfgname))
  configreader = TQConfigReader(alias,cfgname)
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
    BREAK("unable to retrieve configuration from file '{:s}' - please check the filename".format(cfgname))
  if config.getNTags() < 3:
    BREAK("very few configuration options were read -- are you sure you are reading the correct config file?")
  
  # leplep: systematics
  
  systematics = systematicsHandler.checkForSystematics(config)
  
  # load the aliases from the config file
  aliases = TQTaggable()
  aliases.importTagsWithoutPrefix(config,"cutParameters.")
  aliases.importTagsWithoutPrefix(config,"aliases.")

  # load MVA libraries if required
  try:
    libMVA = loadLibMVA(config)
  except Exception as ex:
    libMVA = False
    template = "An exception of type '{0}' occured: {1!s}"
    message = template.format(type(ex).__name__, ",".join(ex.args))
    ERROR(message)
  
  # load the sample folder
  sfinfilename = config.getTagStringDefault("inputFile","input/samples_cutbased.root:samples");
  # leplep: systematics
  if systematics:
      WARN("running with systematics "+systematics.name)
      sfinfilename=systematics.changeFileName(sfinfilename,isSampleFile=True)
  INFO("loading sample folder '{:s}'".format(sfinfilename))
  samples = TQSampleFolder.loadSampleFolder(sfinfilename)
  if not samples and not dummy:
    BREAK("unable to load sample folder '{:s}' - please check input path".format(sfinfilename))
  elif not samples:
    samples = TQSampleFolder("test")

  # write the configuration to the info/config subfolder
  info = samples.getFolder("info/config/runAnalysis+")
  info.setInfoTags()
  config.exportTags(info)

  # remove the data folder if not desired
  if not config.getTagBoolDefault("doData",True):
    INFO("removing data folder")
    samples.deleteObject("data!")
  # leplep: systematics
  if systematics:
      WARN("removing data folder because running systematics")
      samples.deleteObject("data!")
  
  # remove the channels that are not scheduled
  channels = config.getTagVString("channels")
  doNominal = config.getTagBoolDefault("doNominal",True)
  for sf in samples.getListOfSampleFolders("?/?"):
    found = False
    for c in channels:
      if TQStringUtils.equal(sf.getName(),c):
        found = True
        break
    if not found:
      INFO("removing folder '{:s}' - unscheduled channel!".format(sf.getPath()))
      sf.detachFromBase()
    
  # parse the output path
  if args.debug:
    sfoutfilename="debug.root:samples"
  else:
    sfoutfilename = config.getTagStringDefault("outputFile","output/samples_cutbased.root:samples");
  outfilename = TString("samples.root")
  # leplep: systematics
  if systematics:
      sfoutfilename=systematics.changeFileName(sfoutfilename)
      outfilename=  systematics.changeSampleName(outfilename)
  sfoutname = TString(samples.GetName())
  if not TQFolder.parseLocation(sfoutfilename,outfilename,sfoutname):
    # bad strings for the output file
    BREAK("cannot parse folder location/name from '{:s}'".format(sfoutfilename))
  if not sfoutname:
    sfoutname = samples.GetName()
  
  # set a bunch of tags
  errMsg = TString()
  for style in  config.getTagVString("style"):
    if not samples.importFromTextFile(style,errMsg):
      ERROR("error importing style tags from '{:s}': {:s}".format(style,errMsg))
    else:
      INFO("successfully imported style tags from '{:s}'".format(style))    
  
  # leplep: remove "fakes" if not explicitly used in --restricted
  if not args.pathselect:
    WARN("removing fakes folder")
    fakespath=[ str(s.getPath().Data()) for s in samples.getListOfSamples() ]
    fakespath=[ s for s in fakespath if "fakes" in s]
    for path in fakespath:
        samples.deleteObject(path+"!")
        WARN("removing paths "+path)
    WARN("you must run fakes with --restricted /bkg/fakes/")
  else:
    config.setTagString("restrictedPath",args.pathselect)
  
  # handle sysetmatics
  variation = config.getTagStringDefault("variation","nominal")
  if variation != "nominal":
    samples.setTagString("variation",variation)

  #############################
  # handle systematics (2016-04, FS)

  # make list of weight systematics, the list contains the names of the
  # systematics, that are assigned to the tag weightvariation
  weightSystematicsPath = config.getTagStringDefault("weightSystematicsFile","False")
  weightSystematics = []
  samples.setTagString("weightvariation", "nominal")
  #print repr(str(weightSystematicsPath).lower())
  if str(weightSystematicsPath).lower() != "false":
    INFO("running over weight systematics")
    # parse file
    with open(str(weightSystematicsPath)) as weightSystematicsFile:
      for line in weightSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          weightSystematics.append(line)

  # make list of tree systematics, the list contains the names of the
  # systematics and the corresponding trees. The name is assigned to the tag
  # kinematicvariation
  kinematicSystematicsPath = config.getTagStringDefault("kinematicSystematicsFile","False")
  kinematicSystematics = []
  samples.setTagString("kinematicvariation", "nominal")
  if str(kinematicSystematicsPath).lower() != "false":
    INFO("running over kinematic systematics")
    if config.getTagBoolDefault("useMultiChannelVisitor",False):
      # see comment in recursive_change_tree
      ERROR("MultiChannelAnalysisSampleVisitor cannot be used for kinematic systematics! MCASV is now deactivated.")
      config.setTagBool("useMultiChannelVisitor", False);
        
    # parse file
    with open(str(kinematicSystematicsPath)) as kinematicSystematicsFile:
      for line in kinematicSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          name, treename = re.split("\s+", line)[:2]
          kinematicSystematics.append([name.strip(), treename.strip()])

  fakeSystematicsPath = config.getTagStringDefault("fakeSystematicsFile","False")
  fakeSystematics = []
  samples.setTagString("fakevariation", "nominal")
  print repr(str(fakeSystematicsPath).lower())
  if str(fakeSystematicsPath).lower() != "false":
    INFO("running over fake systematics")
    # parse file
    with open(str(fakeSystematicsPath)) as fakeSystematicsFile:
      for line in fakeSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          fakeSystematics.append(line)

  efakeSystematicsPath = config.getTagStringDefault("eFakeSystematicsFile","False")
  efakeSystematics = []
  samples.setTagString("efakevariation", "nominal")
  print repr(str(efakeSystematicsPath).lower())
  if str(efakeSystematicsPath).lower() != "false":
    INFO("running over efake systematics")
    # parse file
    with open(str(efakeSystematicsPath)) as eFakeSystematicsFile:
      for line in eFakeSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          efakeSystematics.append(line)

  # copy all channel directories to a new channel for each systematic and
  # adjust tags and trees for these new channels
  subfolders = samples.getListOfSampleFolders("?")
  mcasvchannels = set([c for c in channels])
  #print channels
  for subfolder in subfolders:
    for channel in channels:
      original = subfolder.getSampleFolder(channel)
      if not original: continue
      original.setTagString(".mcasv.channel", channel)
      for weightSystematic in weightSystematics:
        newchannel = "%s_%s" % (channel, weightSystematic)
        fullname = "/%s/%s" % (subfolder.getName(), newchannel)
        if not is_prefix(fullname, args.pathselect):
          # this path is not sceduled, copied makes no sense
          continue
        newfolder = original.copy(newchannel)
        subfolder.addFolder(newfolder)
        newfolder.setTagString("weightvariation", weightSystematic)
        newfolder.setTagString(".mcasv.channel", newchannel)
        INFO("Added %s" % fullname)
        mcasvchannels.add(newchannel)

      for fakeSystematic in fakeSystematics:
        newchannel = "%s_%s" % (channel, fakeSystematic)
        fullname = "/%s/%s" % (subfolder.getName(), newchannel)
        if not is_prefix(fullname, args.pathselect):
          # this path is not sceduled, copied makes no sense
          continue
        newfolder = original.copy(newchannel)
        subfolder.addFolder(newfolder)
        newfolder.setTagString("fakevariation", fakeSystematic)
        newfolder.setTagString(".mcasv.channel", newchannel)
        INFO("Added %s" % fullname)
        mcasvchannels.add(newchannel)

      for efakeSystematic in efakeSystematics:
        newchannel = "%s_%s" % (channel, efakeSystematic)
        fullname = "/%s/%s" % (subfolder.getName(), newchannel)
        if not is_prefix(fullname, args.pathselect):
          # this path is not sceduled, copied makes no sense
          continue
        newfolder = original.copy(newchannel)
        subfolder.addFolder(newfolder)
        newfolder.setTagString("efakevariation", efakeSystematic)
        newfolder.setTagString(".mcasv.channel", newchannel)
        INFO("Added %s" % fullname)
        mcasvchannels.add(newchannel)

      for name, treename in kinematicSystematics:
        newchannel = "%s_%s" % (channel, name)
        fullname = "/%s/%s" % (subfolder.getName(), newchannel)
        if not is_prefix(fullname, args.pathselect):
          # this path is not sceduled, copied makes no sense
          continue
        newfolder = original.copy(newchannel)
        subfolder.addFolder(newfolder)
        newfolder.setTagString("kinematicvariation", name)
        #newfolder.setTagString(".mcasv.channel", newchannel)
        INFO("Added %s" % fullname)
        recursive_change_tree(newfolder, treename)
        #mcasvchannels.add(newchannel)
  # end of systematics handling
  #############################

  # remove the channels that are not scheduled
  if not doNominal:
    channels = config.getTagVString("channels")
    for sf in samples.getListOfSampleFolders("?/?"):
      if sf.getPath() == "/bkg/etau" or sf.getPath() == "/bkg/mtau":
        INFO("removing folder '{:s}' - unscheduled channel!".format(sf.getPath()))
        sf.detachFromBase()


  # if requested, we initialize the samples from the input lists
  initlists = config.getTagVString("initializerLists")
  init = TQSampleListInitializer(samples)
  init.setTagString("treeName","CollectionTree")
  for filelist in initlists:
    with open(filelist.Data(),'r') as f:
      for line in f:
        init.initializeSampleForFile(line)

  # the user also has an opportunity to define a bunch of custom observables
  customobservables = False
  for observablescript in config.getTagVString("addObservables"):
    TQStringUtils.removeLeadingText(observablescript,"definitions/")
    TQStringUtils.removeLeadingText(observablescript,"observables/")
    TQStringUtils.removeTrailingText(observablescript,".py")
    loader = str(observablescript.Data())
    START("l.","loading observables from '{:s}'".format(loader))
    try:
      addobservables = importlib.import_module("observables."+loader)
      argspec = inspect.getargspec(addobservables.addObservables)
      if 'config' in argspec.args and 'samples' in argspec.args:
        added = addobservables.addObservables(config=config,samples=samples)
      elif 'config' in argspec.args:
        added = addobservables.addObservables(config=config)
      elif 'samples' in argspec.args:
        added = addobservables.addObservables(samples=samples)
      elif len(argspec.args) == 1:
        added = addobservables.addObservables(config)
      elif len(argspec.args) == 0:
        added = addobservables.addObservables()
      else:
        BREAK("unable to add observable(s) from script '{:s}' - unknown arguments appeared: {:s}".format(loader,str(argspec.args)))
      if added: 
          END(TQMessageStream.OK)
          customobservables = True
      else:
          END(TQMessageStream.FAIL)
          BREAK("unable to properly setup custom observables")
    except IOError:
      END(TQMessageStream.FAIL)
      BREAK("unable to open file 'observables/{:s}.py' - please double-check!".format(loader))


  # load the cuts
  cutdeffiles = config.getTagVString("cuts")
  if len(cutdeffiles) < 1:
    BREAK("no cuts given!")
  cutfolder = TQFolder("cuts")
  for cutdeffile in cutdeffiles:
    INFO("loading cuts from '{:s}'".format(cutdeffile))
    # create a TQFolder from the cut definitions file
    ok = cutfolder.importFromTextFile(cutdeffile)
    if not ok:
      BREAK("unable to load cuts from '{:s}' ".format(cutdeffile))
  # retrieve the actual cut definitions from the folder
  cutdefs = cutfolder.getFolder("?")
  cutdefs.detachFromBase()
  # create compiled cuts
  cuts = TQCut.importFromFolder(cutdefs,aliases)
  if not cuts:
    BREAK("failed to load cuts from folder '{:s}' - please check input path".format(cutdefs.GetName()))

  # just some boolean variable to keep track of whether we are using cutbased or MVA
  mva = config.getTagVString("MVA")
  cutbased = (len(mva)==0)
  
  # initialize and configure the Analysis
  if config.getTagBoolDefault("cutbased.makeCutflow",cutbased):
    INFO("booking cutflow")
    cutflowjob = TQCutflowAnalysisJob("cutflowJob")
    cuts.addAnalysisJob(cutflowjob,"*")
    cutbased = True

  if ( config.hasTag("histograms.0") or config.hasTag("histograms") ) and config.getTagBoolDefault("makeHistograms",cutbased):
    INFO("booking histograms")
    histofiles = config.getTagVString("histograms");
    if TQHistoMakerAnalysisJob.importJobsFromTextFiles(histofiles,cuts,aliases,"*",config.getTagBoolDefault("printHistograms",False)) > 0:
      print(TQHistoMakerAnalysisJob.getErrorMessage());
      cutbased = True
    
  if ( config.hasTag("eventlists.0") or config.hasTag("eventlists") ) and config.getTagBoolDefault("makeEventLists",cutbased):
    INFO("booking eventlists")
    evtlistfiles = config.getTagVString("eventlists");
    if TQEventlistAnalysisJob.importJobsFromTextFiles(evtlistfiles,cuts,aliases,"*") > 0:
      print(TQEventlistAnalysisJob.getErrorMessage());
      cutbased = True

  if ( config.hasTag("ntuples.0") or config.hasTag("ntuples") ) and config.getTagBoolDefault("dumpNtuples",cutbased):
    INFO("preparing to dump ntuples")
    ntupfiles = config.getTagVString("ntuples");
    if TQNTupleDumperAnalysisJob.importJobsFromTextFiles(ntupfiles,cuts,aliases,"*",config.getTagBoolDefault("printNTuples",False)) > 0:
      print(TQNTupleDumperAnalysisJob.getErrorMessage());
      cutbased = True

  if config.getTagBoolDefault("printCuts",cutbased):
    cuts.printCut();

  if customobservables:
    INFO("custom observables were defined - this is the list of known observables:")
    TQTreeObservable.printObservables()
    
  # run the cutbased analysis
  if cutbased:
    if args.debug:
      maxEvents = 100
    else:
      maxEvents = config.getTagIntegerDefault("maxEvents",-1)
    if not config.getTagBoolDefault("useMultiChannelVisitor",False):
      # using regular analysis sample visitor (default)
      visitor = TQAnalysisSampleVisitor()
      visitor.setVerbose(True)
      visitor.setBaseCut(cuts)
    else:
      # using fast MultiChannel analysis sample visitor (experimental)
      visitor = TQMultiChannelAnalysisSampleVisitor()
      visitor.setVerbose(True)
      cutlist = []
      for channel in mcasvchannels:
        cut = cuts.getClone()
        cutlist.append(cut)
        visitor.addChannel(channel,cut)
    # book any algorithms
    for algorithm in config.getTagVString("algorithms"):
      TQStringUtils.removeTrailingText(algorithm,".py")
      loader = str(algorithm.Data())
      START("l.","adding algorithms from '{:s}'".format(loader))
      try:
        addalgorithms = importlib.import_module("algorithms."+loader)
        added = addalgorithms.addAlgorithms(visitor,config)
        if added: 
          END(TQMessageStream.OK)
        else:
          END(TQMessageStream.FAIL)
          BREAK("unable to properly setup custom algorithms")
      except IOError:
        END(TQMessageStream.FAIL)
        BREAK("unable to open file 'algorithms/{:s}.py' - please double-check!".format(loader))
    # proceed with analysis
    visitor.setVisitTraceID(alias)
    if maxEvents > 0:
      WARN("setting maximum number of events per sample to {:d}".format(maxEvents))
      visitor.setMaxEvents(maxEvents)
    TQLibrary.allowRedirection(False)

    timer = TStopwatch()
    if args.pathselect:
      paths = TString(args.pathselect)
      if not dummy:
        samples.visitSampleFolders(visitor,paths)
        if args.downmerge or args.downmergeTo:
          downmergeTargets = args.downmergeTo
          if not downmergeTargets:
            downmergeTargets = paths
          INFO("Downmerge to %s" % downmergeTargets)
          samples.setTag(".generalize.histograms",True,downmergeTargets)
          samples.setTag(".generalize.cutflows",True,downmergeTargets)
      else:
        WARN("dummy run, skipping execution of cutbased analysis on paths '{:s}'".format(args.pathselect))
    else:
      if not dummy:
        samples.visitMe(visitor)
      else:
        WARN("dummy run, skipping execution of cutbased analysis on root sample folder")
    if args.downmerge or args.downmergeTo:
      samples.generalizeObjects(".generalize")
    timer.Stop()

    if dummy:
      allevents = TQCounter("dummy",0,0,0)
    else:
      allevents = samples.getCounter(".",cuts.GetName())
    if allevents:
      raw = allevents.getRawCounter()
    else:
      raw = 0
    time = timer.RealTime()
    INFO("finished cutbased analysis run over {:d} events after {:.2f}s ({:.1f} Hz)".format(raw,time,float(raw)/time))

    # debugging printout
    if config.getTagBoolDefault("printCounterValues",False):
      samples.printListOfCounters()
    printhists = config.getTagVString("printHistogramsASCII")
    for hist in printhists:
      h = samples.getHistogram(".",hist)
      if not h:
          ERROR("unable to access histogram '{:s}'".format(hist))
      if h:
        TQHistogramUtils.printHistogramASCII(h)

        
  # attach the definitions to the info folder
  if not cuts.dumpToFolder(samples.getFolder("info/cuts+")):
    ERROR("unable to attach cuts to info folder")

  # run the multivariate analysis
  mvascriptnames = config.getTagVString("MVA")
  if len(mvascriptnames)>0:
    for mvaconfig in mvascriptnames:
      mvascriptname = TString()
      TQStringUtils.readUpTo(mvaconfig,mvascriptname,"(")
      TQStringUtils.removeLeadingText(mvaconfig,"(")
      TQStringUtils.removeTrailingText(mvaconfig,")")
      path = TQFolder.concatPaths("MVA",mvascriptname).Data() + ".py"
      if (mvaconfig):
        INFO("now running analysis '{:s}' with options '{:s}'".format(mvascriptname,mvaconfig))
      else:
        INFO("now running analysis '{:s}'".format(mvascriptname))
      allOK = True
      try:
        myMVA = imp.load_source("myMVA",path)
      except IOError:
        CRITICAL("unable to open file '{:s}' - please double-check!".format(path))
        allOK = False
  
      if allOK: 
        try:  
          TQUtils.ensureDirectory("weights")
          tqmva = TQMVA(samples)
          tqmva.setBaseCut(cuts)
          tqmva.setName(mvascriptname)
          tqmva.setTitle(mvascriptname)
          tqmva.setAliases(aliases)
          tqmva.importTags(mvaconfig,False)
          timer = TStopwatch()
          if not dummy:
            retval = myMVA.runMVA(tqmva)
          else:
            retval = True
            WARN("dummy run, skipping execution of MVA analysis '{:s}'".format(tqmva.GetName()))
          timer.Stop()
          if retval:
            INFO("analysis '{:s}' complete after {:.2f}s, output written to '{:s}'!".format(mvascriptname,timer.RealTime(),tqmva.getTagStringDefault("outputFileName","<unknown file>")))
          else:
            WARN("analysis '{:s}' returned '{:s}' - please double-check!".format(mvascript,str(retval)))
        except Exception as ex:
          template = "An exception of type '{0}' occured: {1!s}"
          message = template.format(type(ex).__name__, ",".join(map(str,ex.args)))
          ERROR(message)
          allOK = False

  elif not cutbased: 
    ERROR("no analysis script given, please use '{:s}.MVA: myAnalysis' to import and execute some python script 'MVA/myAnalysis.py'. it should contain a function 'runMVA(...)' that will receive a readily prepared sample folder at your disposal".format(alias))

  # write the results
  START("l.","writing output sample folder '{:s}' to '{:s}'".format(sfoutname,outfilename))
  # the folder gets written out by the TQFolder.writeToFile function
  # the second argument "True" specifies that any other ROOT file with the same name should be overwritten
  # the third argument specifies that the folder should be split upon writing (which avoids chrashes due to a buggy streamer)
  # the last argument "False" specifies that the folder should be deleted in the process
  if TQUtils.ensureDirectoryForFile(sfoutfilename) and (samples.writeToFile(outfilename,True,2,False) > 0):
    END(TQMessageStream.OK)
  else:
    END(TQMessageStream.FAIL)


if __name__ == "__main__":
  # parse the CLI arguments
  parser = argparse.ArgumentParser(description='Run the HSG3 Run2 Analysis.')
  parser.add_argument('cfgname', metavar='CONFIG', type=str, 
            default="config/runMVA.cfg", 
            help='path to the analysis config file to be processed')
  parser.add_argument('--options', metavar='KEY:VALUE', type=str, nargs='*',
            default="", 
            help='set (overwrite) an option with respect to the config file')
  parser.add_argument('--restrict', metavar="PATHS", dest="pathselect", 
            help='restrict the analysis to a path set')
  parser.add_argument('--downmergeTo', metavar="DOWNMERGETO", dest="downmergeTo", default=None,
            help='specify alternative downmerging targets to the paths listed at \'--restrict\'')
  parser.add_argument('--downmerge', action="store_const", const=True, default=False,
            help='if the analysis is restricted to a path set, objects will be merged up to the paths specified in \'--restrict\'')
  parser.add_argument('--dummy', dest="dummy", action="store_const", const=True, default=False,
            help='run a dummy analysis (do not read data)')
  parser.add_argument('--debug', dest="debug", action="store_const", const=True, default=False,
            help='run a debug analysis (same as "--options outputFile:debug.root:samples maxEvents:100")')
  args = parser.parse_args()
  
  sys.path=[os.getcwd()]+sys.path
  
  from QFramework import *
  from ROOT import *

  TQLibrary.getQLibrary().setApplicationName(alias);

  main(args);
