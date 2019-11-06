#!/usr/bin/env python2

import argparse
import sys
import systematicsHandler

import os, os.path

# the alias is the 'appliation name' that will be dumped to the info tags of any
# sample folder created
alias = "makeSampleFile"

def includeDataDrivenBkgEstimate(origparser,fspath,treename,subpath):
  samples = origparser.getSampleFolder()
  if samples.getSampleFolder("bkg"):
    INFO("including data-driven estimate for sample '{:s}'".format(subpath))
  else:
    WARN("cannot include data-driven estimate for sample '{:s}' without '/bkg' folder!".format(subpath))
    return False

  # copy the parser
  parser = TQXSecParser(origparser)
  parser.setTagString("treeName",treename)
  if not samples:
    ERROR("cannot include data-driven estimate for sample {:s} - no sample folder set to parser!")
    return False

  # create a temporary sample folder to work with
  dd = TQSampleFolder.newSampleFolder("tmp")
  parser.setSampleFolder(dd)

  # parse the input arguments
  customtreename = TString()
  customfspath = TString()
  if TQFolder.parseLocation(fspath,customfspath,customtreename):
    if not customtreename.IsNull():
      treename = customtreename
    if not customfspath.IsNull():
      fspath = customfspath

  # read in the file system structure
  START("r.","reading files from '{:s}' (this may take a while)".format(fspath))
  folder = TQFolder.copyDirectoryStructure(fspath)
  if not folder or folder.isEmpty():
    END(TQMessageStream.FAIL)
    ERROR("no such file or directory: '{:s}'".format(fspath))
    return False
  END(TQMessageStream.OK)

  # use the parser to create the folder structure
  parser.disableSamplesWithColumnStringMatch("processinfo","*VBF*",True)
  parser.disableSamplesWithColumnStringMatch("path","sig/*",True)
  parser.setTagDouble("xSecScale",-1.)
  parser.setTagString("fileNamePattern",TQFolder.concatPaths(fspath,"$(name)_fake.root"))
  START("r","adding mc samples")
  parser.addAllSamples(True,True)
  END(TQMessageStream.OK)

  # add the data samples
  nData = 0
  sflist = samples.getListOfSampleFolders("bkg/?")
  if not sflist:
    BREAK("there are no sample folders matching 'bkg/?', but there should be - is your configuration valid?")
  START("r","adding data samples")
  for channel in sflist:
    nData = nData + parser.addAllSamplesFromFolder(folder,TQFolder.concatPaths("data",channel.GetName()),"Data.root","./*")
  if nData > 0:
    END(TQMessageStream.OK)
  else:
    END(TQMessageStream.FAIL)

  # remove the original mc folder
  samples.deleteObject(TQFolder.concatPaths("bkg/?",subpath).Append("!"))

  # copy the mc folders from the temporary folder to the 'real' one
  sflistbkg = dd.getListOfSampleFolders("bkg/?/?")
  if not sflistbkg:
    BREAK("there are no sample folders matching 'bkg/?/?', but there should be - is your configuration valid?")
  sflistdata = dd.getListOfSampleFolders("data/?/?")
  if not sflistdata:
    BREAK("there are no sample folders matching 'data/?/?', but there should be - is your configuration valid?")

  START("r","copying sample folders for mc")
  ok = sflistbkg.GetEntries() > 0
  for sf in sflistbkg:
    # obviously, don't copy the one we're using the DD estimate for
    if TQStringUtils.equal(subpath,sf.getName()) or samples.getFolder(sf.getPath()).getTagBoolDefault("isDataDriven",False):
      continue
    path = sf.getPath()
    TQFolder.getPathHead(path)
    ch = TQFolder.getPathHead(path)
    sf.detachFromBase()
    newpath = TQFolder.concatPaths("bkg",ch,subpath,"bkg")
    if not samples.addSampleFolder(sf,TQStringUtils.concat(newpath,"+")): 
      samples.setTagBool("doVBFFilterMerge",False,newpath)
      samples.setTagBool(".aj.pool.histograms",True,newpath)
      ok = False
  if ok:
    END(TQMessageStream.OK)
  else:
    END(TQMessageStream.FAIL)

  # copy the data folder
  ok = sflistdata.GetEntries() > 0
  START("r","copying sample folders for data")
  for sf in sflistdata:
    path = sf.getPath()
    TQFolder.getPathHead(path)
    ch = TQFolder.getPathHead(path)
    sf.detachFromBase()
    newpath = TQFolder.concatPaths("bkg",ch,subpath,"data")
    if not samples.addSampleFolder(sf,newpath + TString("+")):
      samples.setTagBool(".aj.pool.histograms",True,newpath)
      ok = False
  if ok:
    END(TQMessageStream.OK)
  else:
    END(TQMessageStream.FAIL)
    
  # set the "data-driven" tags
  samples.setTagBool("isDataDriven",True,TQFolder.concatPaths("/bkg/?/",subpath))
  samples.setTagBool(TQStringUtils.concat("isDD",subpath),True,TQFolder.concatPaths("/bkg/?/",subpath))

  return True
  
def parseXsecFiles(config,samples):
  # parse the listed Xsec-Files
  unit = TQXSecParser.unitName(TQXSecParser.unit(config.getTagStringDefault("XsecUnit","pb")))
  INFO("parsing cross-section files (assuming unit: {:s})".format(unit))

  # start by initializing a TQXSecParser  
  parser = TQXSecParser(samples);
  parser.setTagString("xSectionUnit",unit)
  
  # retrieve the filenames
  nfiles = 0
  XsecFiles = config.getTagVString("XsecFiles")
  # read them in sequentially
  for obj in XsecFiles:
    file = TString()
    treeName = TString()
    TQFolder.parseLocation(obj,file,treeName)
    if not treeName.IsNull():
        parser.setTagString("treeName",treeName)
    START("r",file)
    # this will actually parse the file
    # "," will be considered as separators
    # 0 leading lines will be ignored
    if parser.readCSVfile(file):
      END(TQMessageStream.OK)
      nfiles = nfiles+1
    else:
      END(TQMessageStream.FAIL)

  # read the "mapping", i.e. the paths to which the samples are mapped in the folder structure
  XsecMap = TString("");
  if not config.getTagString("XsecMap",XsecMap):
    # case 1: no map file given, taking from 'path' column of Xsec-Table
    START("l.","reading path mapping from X-section file column 'path'")
    if parser.readMappingFromColumn("path") > 0:
      END(TQMessageStream.OK)
    else:
      END(TQMessageStream.FAIL)
  else:
    # case 2: a map file was given - read it in
    START("l.","reading path mapping from map file '{:s}'".format(XsecMap))
    if parser.readMapping(XsecMap) > 0:
      # for bookkeeping reasons, we might want to save the paths in the parsed table
      parser.writeMappingToColumn("path");
      END(TQMessageStream.OK)
    else:
      END(TQMessageStream.FAIL)

  # set and apply the sample priorities
  priorityColumn = TString("")
  # first, we enable all samples
  parser.enableAllSamples();
  if config.getTagString("XsecPriorityColumn",priorityColumn):
    # if a priority column was set, use the contents as follows:
    p = ROOT.Long(1)
    if config.getTagInteger("XsecPriorityMax",p):
      #  disable all samples that have a priority greater than XsecPriorityMax
      parser.disableSamplesWithPriorityGreaterThan(priorityColumn,p)
    if config.getTagInteger("XsecPriorityMin",p):
      #  disable all samples that have a priority smaller than XsecPriorityMin
      parser.disableSamplesWithPriorityLessThan(priorityColumn,p)

  # read in the SumOfWeightsPerEvent information in the CSV map
  sumOfWeightsFile = TString()
  if config.getTagString("averageweights.filename",sumOfWeightsFile):
    START("l.","reading normalization information from file '{:s}'".format(sumOfWeightsFile))
    sumOfWeights = TQTable("WeightFileReader")
    if sumOfWeights.readCSVfile(sumOfWeightsFile,","):
      parser.readColumn(sumOfWeights,config.getTagStringDefault("averageweights.colname","Nominal"),"SampleID")
      parser.setEntry(0,parser.getNcols()-1,config.getTagStringDefault("averageWeightName","averageWeight"))
      END(TQMessageStream.OK)
    else:
      END(TQMessageStream.FAIL)

  # if requested, pring the XSection information
  if config.getTagBoolDefault("printXsec",False):
    parser.printPlain();

  # add path variants to the parser (announce the channels)
  # each path variant should reflect a set of path replacements, i.e.
  # a path variant "channel='electron'" will replace all occurences of 
  #  "$(channel)" with "electron" in the input paths
  # for this simple template analysis, we assume that each "channel"
  # consists of only one replacement of the type $(channel) => X
  # the placeholder string (here 'channel') can be configured
  channels = config.getTagVString("channels");
  channelPlaceHolder = config.getTagStringDefault("channelPlaceholder","channel")
  if len(channels) < 1:
    BREAK("please specify at least one channel via 'makeSampleFile.channels: ee,mm,...'")
  INFO("using channels: {:s}".format(TQStringUtils.concat(channels)))
  for c in channels:
    parser.addPathVariant(channelPlaceHolder,c)

  # let the TQXSectionParser do it's magic and create a folder structure
  if not parser.isGood() or nfiles < 1:
    BREAK("no valid cross section files listed. please use the '{:s}' option to schedule cross section files".format(alias+".XsecFiles"))
  else:
    # add only samples that have a mapping and a priority
    START("l.","adding mapped and prioritized samples")
    if parser.addAllSamples(True,True) > 0:
      END(TQMessageStream.OK)
    else:
      END(TQMessageStream.FAIL)

  # include the data-driven estimates  
  ddWjetsPath = TString()
  if config.getTagString("ddWjetsPath",ddWjetsPath):
    if includeDataDrivenBkgEstimate(parser,ddWjetsPath,"WjetsCR_$(channellong)","Wjets"):
      samples.setTagString("fakeWeightName","FakeWeightCorr","bkg/?/Wjets")
      samples.setTagString("sysFakeWeightName","SysFakeWeightCorr","bkg/?/Wjets")

  # include the data-driven estimates  
  ddWjetsPath = TString()
  if config.getTagString("ddQCDPath",ddWjetsPath):
    if includeDataDrivenBkgEstimate(parser,ddWjetsPath,"QCDCR_$(channellong)","QCD"):
      samples.setTagString("fakeWeightName","FakeWeightDijet*QCDCorr","bkg/?/QCD")
      samples.setTagString("sysFakeWeightName","SysQCDCorr","bkg/?/QCD")

  # here, we assume that data samples will not be listed in the Xsection file
  # but will be kept in a separate directory with a dedicated filename pattern
  dataFileNamePattern = config.getTagStringDefault("dataFilePattern","data*.root")
  # we will put all the data samples to a separate folder given by the data folder pattern
  # this will allow for different channels in much the same manner as before
  dataFolderPattern = config.getTagStringDefault("dataFolderPattern","data/$(channel)")
  dataPaths = config.getTagVString("dataPaths")
  if len(dataPaths) < 1:
    WARN("no data path given - adding MC only!")
  for directory in dataPaths:
    path = TString()
    treeName = TString()
    TQFolder.parseLocation(directory,path,treeName);
    INFO("adding data samples from path '{:s}' with file pattern '{:s}'".format(path,dataFileNamePattern))
    INFO("using data tree "+treeName)
    parser.setTagString("treeName",treeName);
    tags = TQTaggable()
    # add the data samples
    for c in channels:
      START("r",c)
      tags.setTagString(channelPlaceHolder,c)
      # add the samples indiscriminately
      if parser.addAllSamplesFromPath(path,tags.replaceInText(dataFolderPattern),dataFileNamePattern,"./") > 0:
        END(TQMessageStream.OK)
      else:
        END(TQMessageStream.FAIL)

  # save the cross-secion parser in the sample folder
  configfolder = samples.getFolder("info/objects+")
  if not configfolder.addObject(parser):
    ERROR("cannot add Xsecion parser to sample folder")


def initializeSamples(config,samples,systematics=None):
  # retrieve some basic information from the config
  mcPaths = config.getTagVString("mcPaths")
  if systematics:
      newmcPaths=vector(TString)()
      for p in mcPaths:
         mcPathsPaths=str(p).split(":")[0] 
         mcPathsSample=str(p).split(":")[1]
         tree=systematics.tree
         if not tree or tree=="*":
             tree=mcPathsSample
         path=mcPathsPaths+systematics.dir+":"+tree
         newmcPaths.push_back(TString(path))
      mcPaths=newmcPaths
      for p in mcPaths:
          INFO("systematic path "+str(p))
  luminosity = config.getTagDoubleDefault("luminosity",20000.)
  luminosityUnit = config.getTagStringDefault("luminosityUnit","pb")
  luminosityInPb = luminosity / TQXSecParser.unit(luminosityUnit)
  channels = config.getTagVString("channels");
  channelPlaceholder = config.getTagStringDefault("channelPlaceholder","channel");

  # setup a sample initializer to find the samples on the disk
  initializer = TQSampleInitializer();
  initializer.setTagString("averageWeightName",config.getTagStringDefault("averageWeightName","averageWeight"))
  
  if not config.getTagBoolDefault("ReadInputFileListFromTextFile",False) and not config.hasTagString("inputFileList"):
    # in order to be able to read MVA samples as well, we need to use the correct suffix
    initializer.setTagString("filenameSuffix",config.getTagStringDefault("mcFilenameSuffix","*.root"))
    initializer.setTagString("filenamePrefix",config.getTagStringDefault("mcFilenamePrefix",""))


    if len(mcPaths) == 0:
      BREAK("no valid nTuple paths listed. please use the '{:s}' option to add an nTuple path".format(alias+".mcPaths"))
  else:
    mcPaths=[""]

    initializer.setTagString("filenameSuffix",config.getTagStringDefault("mcFilenameSuffix","*.root"))
    initializer.setTagString("filenamePrefix",config.getTagStringDefault("mcFilenamePrefix",""))

  
  # by default, the initializer will use the GetEntries function of the tree
  # to learn about the number of entries. however, a different method is also
  # supported: usage of a count histogram, where the first bin will hold
  # the number of entries. this might be required depending on the sklimming.
  # we can safely enable it, since the tree entry count will be used as fallback.
  initializer.setTagBool   ("useCountHistogram"  ,config.getTagBoolDefault("initializer.useCountHistogram",True));
  initializer.setTagString ("countHistogram.name",config.getTagStringDefault("initializer.countHistogramName","Count"));
  initializer.setTagInteger("countHistogram.bin" ,config.getTagIntegerDefault("initializer.countHistogramBinNumber",1));

  # set luminosity information - this will be used for scaling samples
  # according to their respective cross sections and luminosity
  # as compared to data
  INFO("scaling to luminosity: {:g}/{:s} ({:g}/pb)".format(luminosity,luminosityUnit,luminosityInPb))
  initializer.setTagDouble("luminosity",luminosityInPb);

  # set the tree name pattern to be used in order do find the trees
  channelFolderPattern = config.getTagStringDefault("channelFolderPattern","?/$(channel)");

  # call upon the initializer to do its magic
  for directory in mcPaths:
    infilelist = TString("input.txt")

    if not config.getTagBoolDefault("ReadInputFileListFromTextFile",False) and not config.getTagString("inputFileList",infilelist):
      path = TString()
      treeName = TString()
      TQFolder.parseLocation(directory,path,treeName);
      INFO("initializing samples from path '{:s}' - this may take a while".format(path))
      # read the directory from disk to enable the initializer to find the files
      initializer.readDirectory(path)
      INFO("using tree "+treeName)
      initializer.setTagString("treeName",treeName);
    else:
      infilelist_orig = TString(infilelist)
      tokens = str(infilelist).rsplit(":", 1)
      if len(tokens) == 2:
        infilelist, treeName = [TString(x) for x in tokens]
      else:
        treeName = TString("NOMINAL")
      INFO("using the tree "+treeName)
      initializer.setTagString("treeName",treeName)
      #read list of files from given input, either as input.txt or the name given by inputFileList:
      if config.getTagBoolDefault("ReadInputFileListFromTextFile",False):
        WARN("reading all xAODs listed in input.txt")
        initializer.readInputFilesList("input.txt",False)
      elif config.getTagString("inputFileList",infilelist_orig):
        WARN("reading all xAODs listed in "+ infilelist)
        initializer.readInputFilesList(infilelist,True)
    
    for c in channels:
      # since the trees are named differently for the different channels,
      # we need to announce the channel to the initializer
      chlong = TString(c)
      chlong.ReplaceAll("m","mu")
      initializer.setTagString("channel",c)
      initializer.setTagString("channellong",chlong)
      channelFolder = initializer.replaceInText(channelFolderPattern);
      START("r",channelFolder)
      # actually initialize the samples
      if config.getTagBoolDefault("verbose",False):
        initializer.setVerbose(True)
      if samples.visitSampleFolders(initializer,channelFolder) >= config.getTagIntegerDefault("initializer.minSamples",1):
        END(TQMessageStream.OK)
      else:
        END(TQMessageStream.FAIL)
        if config.getTagBoolDefault("printSamplesFailed",False):
          initializer.getDirectory().printContents("rdt")

  # save the directory structure in the sample folder
  configfolder = samples.getFolder("info/objects+")
  if not configfolder.addObject(initializer.getDirectory()):
    ERROR("cannot add MC directory structure to sample folder")


def main(args):
  # print a welcome message
  print(TQStringUtils.makeBoldWhite("making Analysis ROOT File"))

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
    BREAK("unable to load configuration '{:s}' inside from  file '{:s}' - please check your arguments!".format(alias,cfgname))
  if config.getNTags() < 3:
    BREAK("very few configuration options were read -- are you sure you are reading the correct config file?")
  
  ## systematic for leplep

  systematics=systematicsHandler.checkForSystematics (config)

  # retrieve and parse some important variables
  sffilename = config.getTagStringDefault("sampleFile","input/samples.root:samples");
  filename = TString("samples.root")
  sfname = TString("samples")
  if systematics:
      WARN("running with systematics "+systematics.name)
      sffilename=systematics.changeFileName(sffilename,isSampleFile=True)
      filename = systematics.changeSampleName(filename)
      sfname =   systematics.changeSampleName(sfname)
  if not TQFolder.parseLocation(sffilename,filename,sfname):
    # bad string for the output file
    BREAK("cannot parse folder location/name from '{:s}'".format(config.getTagStringDefault("sampleFile","<undefined>")))
  # create the sample folder
  samples = TQSampleFolder.newSampleFolder(sfname)
  # set a couple of parameters
  luminosity = Double(0.)
  if config.getTagDouble("luminosity",luminosity):
    samples.setTagDouble("luminosity",luminosity)
    samples.setTagString("luminosityUnit",config.getTagStringDefault("luminosityUnit","pb"))
  energy = Double(0.)
  if config.getTagDouble("energy",energy):
    samples.setTagDouble("energy",energy)
    samples.setTagString("energyUnit",config.getTagStringDefault("energyUnit","TeV"))

  if not samples:
    BREAK("unable to allocate sample folder '{:s}' - invalid name?".format(sfname))

  # write the configuration to the info/config subfolder
  info = samples.getFolder("info/config/makeSampleFile+")
  info.setInfoTags()
  config.exportTags(info)

  # parse the cross section file and create the folder structure
  parseXsecFiles(config,samples);
  #if (samples.getSampleFolder("bkg")):
  #   samples.getSampleFolder("bkg").setTagBool("usemcweights",True)
  #if (samples.getSampleFolder("sig")):
  #   samples.getSampleFolder("sig").setTagBool("usemcweights",True)

  # check if we have found anything and produce some output
  if samples.isEmpty():
    BREAK("your sample folder is empty. please make sure your configuration is sensible and try again!")

  # set a bunch of tags
  errMsg = TString()
  for style in  config.getTagVString("style"):
    if not samples.importFromTextFile(style,errMsg):
      ERROR("error importing style tags from '{:s}': {:s}".format(style,errMsg))
    else:
      INFO("successfully imported style tags from '{:s}'".format(style))

  # run the sample initializer and find the samples
  if config.getTagBoolDefault("initialize",True):
    initializeSamples(config,samples,systematics);
  else:
    WARN("skipping initialization as requested!")

  # leplep tempalte fakes
  tfake=samples.getListOfSampleFolders("/bkg/?/tfakes*")
  if tfake:
      WARN("found template fakes for leplep, setting normalisation to 1")
      tfakeFolders=[f for f in tfake]
      def setNormForFolderAndSamples(f):
          if type(f)==TQSample:
              WARN("Normalisation in sample/folder "+f.getPath())
              f.setNormalisation(1)
          samples=f.getListOfSamples()
          if samples:
              for s in samples:
                  #WARN("Normalisation in sample "+s.getPath())
                  s.setNormalisation(1.0)
          folders=f.getListOfSampleFolders()
          if folders:
              for folder in folders:
                  setNormForFolderAndSamples(folder)
      for f in tfakeFolders:
          setNormForFolderAndSamples(f)
  
  # produce some output for the user to have a look at the result
  INFO("please investigate the top-level structure of your sample folder to make sure everything is in place!")
  samples.printContents(config.getTagStringDefault("printFolderTags","r1dt"))

  # write everything to disk
  START("l.","writing output sample folder '{:s}' to '{:s}'".format(sfname,filename))
  if TQUtils.ensureDirectoryForFile(sffilename) and (samples.writeToFile(filename,True) > 0):
    END(TQMessageStream.OK)
  else:
    END(TQMessageStream.FAIL)

if __name__ == "__main__":
  # parse the CLI arguments
  parser = argparse.ArgumentParser(description='Prepare a samples.root file for the HSG3 Run2 Analysis.')
  parser.add_argument('cfgname', metavar='CONFIG', type=str, 
            default="config/"+alias+".cfg",
            help='path to the analysis config file to be processed')
  parser.add_argument('--options', metavar='KEY:VALUE', type=str, nargs='*',
            default="", 
            help='set (overwrite) an option with respect to the config file')
  args = parser.parse_args()

  sys.path=[os.getcwd()]+sys.path
  
  from QFramework import *
  from ROOT import *
 
  TQLibrary.getQLibrary().setApplicationName(alias)

  main(args)


