#!/usr/bin/env python2

def main():
  samples = TQSampleFolder.newSampleFolder("samples")
  sample = TQSample("test")
  testfile = "/eos/atlas/user/a/ademaria/Slim_July_Production/data/data15/user.ademaria.00282631_slimming_SLT_Data_NewSlim_June_production_sgnl_tau_out.root/user.ademaria.12494599._000005.out.root"
  sample.setTagString(".xsp.filepath",testfile)
  sample.setTagString(".xsp.treename","NOMINAL")
  samples.addObject(sample)
  
  TQTreeObservable.allowErrorMessages(True)
  init = TQSampleInitializer()
  init.setTagString("makeCounter","initial");
  sample.visitMe(init)

  tok = sample.getTreeToken()
  if tok:
    INFO("successfully opened xAOD file and retrieved TTree!")
  else:
    BREAK("unable to obtain tree token from '{:s}'!".format(testfile))
  sample.returnTreeToken(tok)

  basecut = TQCut("base")
  basecut.setWeightExpression("1.")
  basecut.setCutExpression("1.")

  histograms = TQHistoMakerAnalysisJob()
  basecut.addAnalysisJob(histograms)

  eventlists = TQEventlistAnalysisJob()
  eventlists.addColumn("EventInfo.runNumber()","run")
  eventlists.addColumn("EventInfo.eventNumber()","event")
  basecut.addAnalysisJob(eventlists)

  vis = TQAnalysisSampleVisitor()
  vis.setVerbose(True)
  vis.setBaseCut(basecut)
  samples.visitMe(vis)

  INFO("writing output file")

  samples.writeToFile("samples.root")
  
  #clean up function that needs to be called to prevent a segfault due to interference between ASG libraries and the python garbage collector:
  ROOT.xAOD.ClearTransientTrees()
  
if __name__ == "__main__":
  from QFramework import *
  from ROOT import *
  TQLibrary.getQLibrary().setApplicationName("libQFramwork_xAOD_test")
  main()
