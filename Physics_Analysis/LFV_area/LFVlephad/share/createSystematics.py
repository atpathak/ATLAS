#!/usr/bin/env python2

import argparse

alias = "createSystematics"

def main():
  
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

  
  handler = TQSystematicsHandler('systematics')
  
  inDir = config.getTagStringDefault("inputDir","output")
  outDir = config.getTagStringDefault("outputDir","systematics")
  outFile = config.getTagStringDefault("outputFile","systematics.root")
  sfname = config.getTagStringDefault("sampleFolder",":samples")
  channel = config.getTagStringDefault("channel")
  cuts = config.getTagVString("cuts")
  tableName = config.getTagStringDefault("tableName","systematics_table")

  print channel

  # nominal
  nominal_file_path = inDir + "/" + config.getTagStringDefault("nominalFile") + sfname  
  nominal_sf_path = 'bkg/%s/[Diboson+ZllSh+ZttSh+ZllEWSh+ZttEWSh+Top+Fake]' % channel
  #nominal_sf_path = '-1*bkg/%s/[Fake]' % channel #Fake only systematics
  #nominal_sf_path = 'bkg/%s' % channel
  handler.setNominalFilePath(nominal_file_path)
  handler.setNominalSampleFolderPath(nominal_sf_path)
  handler.setTagBool("useCache", False)

  # weight systematics
  weightSystematicsPath = config.getTagStringDefault("weightSystematicsFile", "false")
  weightSystematics = []
  oneSided = []
  if str(weightSystematicsPath).lower() != "false":
    INFO("get weight systematics")
    # parse file
    with open(str(weightSystematicsPath)) as weightSystematicsFile:
      for line in weightSystematicsFile:
        line = line.strip()
        print repr(line)
        if line.endswith('_high'):
          line = line[:-5]
          weightSystematics.append(line)
        elif not line.startswith("#") and len(line) > 0 and not line.endswith('_low'):
          #print line
          oneSided.append(line)

  # fake systematics
  fakeSystematicsPath = config.getTagStringDefault("fakeSystematicsFile", "false")
  fakeSystematics = []
  if str(fakeSystematicsPath).lower() != "false":
    INFO("get fake systematics")
    # parse file
    with open(str(fakeSystematicsPath)) as fakeSystematicsFile:
      for line in fakeSystematicsFile:
        line = line.strip()
        if line.endswith('_high'):
          line = line[:-5]
          fakeSystematics.append(line)
        elif not line.startswith("#") and len(line) > 0 and not line.endswith('_low'):
          #print line
          oneSided.append(line)

  # kinematic systematics
  kinematicSystematicsPath = config.getTagStringDefault("kinematicSystematicsFile", "true")
  kinematicSystematics = []
  if str(kinematicSystematicsPath).lower() != "false":
    INFO("get kinematic systematics")
    # parse file
    with open(str(kinematicSystematicsPath)) as kinematicSystematicsFile:
      for line in kinematicSystematicsFile:
        line = line.strip()
        if line.endswith('_high'):
          line = line[:-5]
          kinematicSystematics.append(line)
        elif not line.startswith("#") and len(line) > 0 and not line.endswith('_low'):
          line = line.split(' ')[0]
          #print line
          oneSided.append(line)

  print weightSystematics
  print kinematicSystematics
  print fakeSystematics
  print oneSided
  exit

  p = '[Diboson+ZllSh+ZttSh+ZllEWSh+ZttEWSh+Top+Fake]'
 # p = '[-1*Fake]' #Fake only systematics
  #p = ''
  for wSys in weightSystematics:
    handler.addSystematic(
      wSys,
      pair(inDir + '/systematics_Mar18/lfv_weight1.root' + sfname, 'bkg/'+channel+'_'+wSys+'_high/'+p),
      pair(inDir + '/systematics_Mar18/lfv_weight1.root' + sfname, 'bkg/'+channel+'_'+wSys+'_low/'+p)
      )

  for kSys in kinematicSystematics:
    handler.addSystematic(
      kSys,
      pair(inDir + '/lfv_lephad_kine0.root' + sfname, 'bkg/'+channel+'_'+kSys+'_high/'+p),
      pair(inDir + '/lfv_lephad_kine0.root' + sfname, 'bkg/'+channel+'_'+kSys+'_low/'+p)
      )

  for fSys in fakeSystematics:
    handler.addSystematic(
      fSys,
      pair(inDir + '/systematics_Mar18/lfv_fakesys.root' + sfname, 'bkg/'+channel+'_'+fSys+'_high/'+p),
      pair(inDir + '/systematics_Mar18/lfv_fakesys.root' + sfname, 'bkg/'+channel+'_'+fSys+'_low/'+p)
      )

  for oSys in oneSided:
    handler.addSystematic(
        oSys,
      pair(inDir + '/lfv_lephad_kine0.root' + sfname, 'bkg/'+channel+'_'+oSys+'/'+p)
      )

  for cut in cuts:
    print cut
    handler.addCut(cut)

  handler.printSystematics()
  handler.collectSystematics()

  systematics = handler.exportSystematics()
  systematics.writeToFile(outDir+'/'+outFile,True,0)

  for cut in cuts:
    table = handler.getTable(cut)
    table.writeHTML(outDir+'/'+cut+'_'+tableName+'.html')
    table.writeLaTeX(outDir+'/'+cut+'_'+tableName+'.tex')


if __name__ == "__main__":
  #parse the CLI arguments
  parser = argparse.ArgumentParser(description='Create systematics')
  parser.add_argument('cfgname', metavar='CONFIG', type=str, 
            default="config/readCutBased.cfg",
            help='path to the analysis config file to be processed')
  parser.add_argument('--options', metavar='KEY:VALUE', type=str, nargs='*',
            default="", 
            help='set (overwrite) an option with respect to the config file')
  args = parser.parse_args()

  from QFramework import *  
  from ROOT import *

  TQLibrary.getQLibrary().setApplicationName(alias);

  pair = ROOT.std.pair('TString','TString')

  gROOT.SetBatch(True)
  main(); 
