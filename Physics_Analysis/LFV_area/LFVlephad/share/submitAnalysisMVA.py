#!/bin/env python2
import argparse
import os
import time
import errno
import ConfigParser
import sys

# this class is (sadly) needed so we can read the analysis config files
class FakeSecHead(object):
  def __init__(self, secname, fp):
    self.fp = fp
    self.sechead = '['+secname+']\n'
  def readline(self):
    if self.sechead:
      try: 
        return self.sechead
      finally: 
        self.sechead = None
    else: 
      return self.fp.readline()

# convenience function for getting substrings    
def upTo(instr, stopsign):
  pos = instr.find(stopsign)
  if pos == 0:
    return ""
  elif pos < 0:
    return instr
  else:
    return instr[:pos]

# autoselect the queue to which to submit
def choosequeue(qtype,mname):
  if "bsub" in qtype:
    return "2nw"
  elif "nafh" in mname:
    return "long.q"
  else:
    return "medium"

# build a submit command
def buildSubmitCommand(submitcmd,queue,jobid,memory):
  moreargs = []
  if "qsub" in submitcmd:
    jarg = " "
    #jarg = "-N"
    moreargs = ["-cwd","-l","h_vmem="+str(memory)+"G"]
  else:
    jarg = "-J"
  if "sbatch" in submitcmd:
    qarg = "-p"
    moreargs = ["-o outjob --mem",str(memory*1024)]
  else:
    qarg = "-q"
  return [submitcmd,qarg,queue,jarg,jobid]+moreargs

# find the absolute path of a binary
def findbin(name):
  from subprocess import Popen, PIPE, STDOUT
  p = Popen("type "+name, shell=True, stdout=PIPE, stdin=PIPE, stderr=PIPE)
  return p.communicate()[0].split(" ")[-1].strip()    

# intelligently choose a binary from a given list, depending on what is available on the system
def choosebin(bins):
  from subprocess import Popen, PIPE, STDOUT
  for name in bins:
    p = Popen("type "+name, shell=True, stdout=PIPE, stdin=PIPE, stderr=PIPE)
    bin =  p.communicate()[0].split(" ")[-1].strip()    
    if bin:
      return bin
  return ""

# implementation of "mkdir -p"
def mkdir_p(path):
  """create a directory, emulating the behavior of 'mkdir -p'"""
  try:
    os.makedirs(path)
  except OSError as exc: 
    if exc.errno == errno.EEXIST and os.path.isdir(path):
      pass
    else: raise

# concatenate a list of strings
def concat(pieces):
  if isinstance(pieces, basestring):
    return pieces
  else:
    return " ".join(pieces)

# execute a list of processes in parallel
def execute(pending,nprocs):
  running = []
  while len(pending) > 0 or len(running) > 0:
    if len(running) < nprocs and len(pending) > 0:
      command,logfile = pending.pop()
      from subprocess import Popen
      fp = open(logfile, "w")
      cmd = concat(command)
      print("starting execution of "+cmd)
      p = Popen(cmd, stdout=fp,stderr=fp,shell=True)
      running.append((p,fp))
    else:
      time.sleep(1)
      for tup in running:
        proc,fp = tup
        if proc.poll() != None : # process is done with its work
          print("collecting results")
          proc.communicate()
          fp.close()
          running.remove(tup)

# submit a given command to the batch system
def submit(command,inputs,debug,verbose):
  """submit a list of inputs with a command. debug and verbose control
     if submit will be performed, and what will be printed."""
  if verbose or debug:
    print(" ".join(command))
    for inputline in inputs:
      if len(inputline) > 0:
        print("> "+concat(inputline))
  if not debug:
    from subprocess import Popen, PIPE, STDOUT
    p = Popen(command, stdout=PIPE, stdin=PIPE, stderr=PIPE)
    stdinlines = [ concat(pieces) for pieces in inputs if len(pieces) > 0]
    stdout_data = p.communicate(input="\n".join(stdinlines))[0]
    if verbose:
      print(stdout_data)
    return stdout_data

def makeSmartJobList(joblist,inputFileName,maxSize=-1,maxCount=-1):
  import QFramework as qf
  import ROOT as root
  
  sf = qf.TQSampleFolder.loadSampleFolder(root.TString(inputFileName))
  if not sf:
    BREAK("unable to load sample folder '{:s}' to create smart job list - please check input path".format(inputFileName))
  
  retList = []
  allPathsWildcarded = [] #list to keep track of what contribution is already used somewhere to prevent double counting/running
  for (label,restrict,downmerge) in joblist:
    localPaths = [[]] #sublists will contain all paths for one subjob
    sampleFolders = sf.getListOfSampleFolders(restrict)
    if not sampleFolders:
      print "No matching sample folders found for expression '{:s}', exiting".format(restrict)
      sys.exit(1)
      
    allSamplesForSubjob = []
    for thisSF in sampleFolders:
      allSamplesForSubjob = allSamplesForSubjob + [x for x in thisSF.getListOfSamples() if not (x.hasSubSamples() or x in allSamplesForSubjob)] #only include samples which actually have a file attached
    localCount = 0
    localSize = 0.
    for sample in allSamplesForSubjob:
      if not sample: continue
      pathWildcarded = sample.getPathWildcarded().Data().strip()
      if pathWildcarded in allPathsWildcarded: continue #check if the path is already used somewhere and skipp it if needed to prevent double counting
      sampleSize = sample.getTagDoubleDefault(".init.filestamp.size",sample.getTagDoubleDefault(".xsp.fileSize",0.))
      if ( (maxSize>0 and localSize+sampleSize>maxSize) or (maxCount>0 and localCount+1>maxCount)  ): #check if we need to create a new batch of samples or we can add it to the current one
        if (len(localPaths[-1]) >0): #don't do anything if this is simply a single sample already exceeding the limit
          localPaths.append([])
          localSize = 0.
          localCount = 0.
      
      allPathsWildcarded.append(pathWildcarded) #we are using this path now so we add it to the used paths
      localPaths[-1].append(pathWildcarded)
      localSize += sampleSize
      localCount += 1
    
    #combine the sample paths into --restrict agruments
    nPart = 0
    for pList in localPaths:
      argument = ",".join(pList)
      retList.append( (label+(".part"+str(nPart) if len(localPaths)>1 else ""),argument,downmerge) )
      nPart += 1
    
  #print "Smart joblist was compiled:"
  #print retList
  return retList



# run the submit script
def main(args):
  """main submission loop"""

  # first, we try to determine the type of environment we live in
  cwd = os.getcwd()
  rcsetup = args.setup
  if rcsetup:
    rcsetup = os.path.abspath(args.setup)
  else:
    try:
      rcsetup = os.path.join(os.path.dirname(os.environ.get("ROOTCOREBIN")),"rcSetup.sh")
    except:
      print("error: cannot locate rootcore setup, please setup RootCore or provide setup script with '--setup' option")
      exit(0)
  atlaspath = os.environ.get("ATLAS_LOCAL_ROOT_BASE")
  if args.pythonpath:
    pythonpath = os.path.abspath(args.pythonpath)
  else:
    pythonpath = os.environ.get("PYTHONPATH")
  if not pythonpath:
    print("error: unable to read PYTHONPATH environment variable, please set with '--pythonpath' option")
    exit(0)
  if args.pythonbin:
    pythonbin = os.path.abspath(args.pythonbin)
  else:
    pythonbin = findbin("python2")
  if not pythonbin:
    print("error: unable to locate correct 'python' binary, please set path with '--pythonbin' option")
    exit(0)
  shebang = "#!"+findbin("bash")
  rcSetupPath = os.environ.get("ATLAS_LOCAL_RCSETUP_PATH")
  
  # having determined all of these paths, we can now proceed to build the code lines that we want to submit
  unsetupROOTCore = ["source",os.path.join(rcSetupPath,"rcSetup.sh"),"-u"] if rcSetupPath else []
  unsetupROOT = ["export","ROOTSYS=''"]
  setupROOTCore = ["source", rcsetup]
  setPYTHON = ["export", "PYTHONPATH="+pythonpath]
  setATLAS = ["export", "ATLAS_LOCAL_ROOT_BASE="+atlaspath]
  setupATLAS = ["source", os.path.join("$ATLAS_LOCAL_ROOT_BASE","user","atlasLocalSetup.sh")]
  
  #read come information from the regular config
  # this is the hacky part: we check the config file for the "nominal" input and output name
  cp = ConfigParser.SafeConfigParser()
  with open(args.cfgname) as cfg:
    cp.readfp(FakeSecHead("runAnalysis",cfg))
    mergeout = cp.get("runAnalysis","runAnalysis.outputFile")
    inputFileName = cp.get("runAnalysis","runAnalysis.inputFile")
  
  # now, we can start looping over the jobs that we would like to have
  results = []
  pending = []
  options = args.options if args.options else []
  runlocal = args.submit == "local" or "exe" in args.submit or not args.submit
  with open(args.jobs) as rawjoblist:
    #convert to a simple list
    joblist = []
    for line in rawjoblist:
      path = upTo(line.strip(),"#")
      if not path: continue
      commaPos = path.find(',')
      label=path.strip("/").replace("/","_").replace("?","X").replace(",","_")
      if commaPos>0:
        label = label[:commaPos-1]
      joblist.append((label,path,path)) #first element of tuple is used for labeling, second is actual path (passed to '--restrict'), third is downmerging target (passed to '--downmergeTo')
    #override list if smart splitting is requested (maxSampleSize or maxSampleCount set)
    if (args.maxSampleSize>0 or args.maxSampleCount>0):
      joblist = makeSmartJobList(joblist,inputFileName,maxSize=args.maxSampleSize,maxCount=args.maxSampleCount)
    for (label,restrict,downmergepath) in joblist:
      jobid = os.environ.get("USER")+"_"+args.identifier+"_"+label
      outname=os.path.join(args.outpath,"unmerged_"+args.identifier,"samples-"+os.path.basename(args.cfgname).split(".")[0]+"-"+label+".root")
      results.append(outname)
      if os.path.isfile(outname):
        print(outname + " is already present, skipping")
        continue
      cd = ["cd", cwd]
      runAnalysis = [pythonbin,args.command,args.cfgname,"--restrict",restrict] 
      if args.downmerge: runAnalysis = runAnalysis + ["--downmergeTo", downmergepath]
      runAnalysis = runAnalysis + ["--options","outputFile:"+outname+":samples"] + [o for o in options]
      # if we want to run locally, we just need to create the list of jobs for now
      if runlocal:
        print("scheduling "+args.command+" for "+restrict)
        pending.append((runAnalysis,"runAnalysis."+args.identifier+"."+label+".log"))
      # if we want to submit, we can to this now
      else:
        print("submitting "+args.command+" for "+restrict)
        commands = [shebang,unsetupROOTCore,unsetupROOT,setATLAS,setupATLAS,setupROOTCore,setPYTHON,cd]
        if args.voms: commands.append(["voms-proxy-init","--voms","atlas"])
        commands.append(runAnalysis)
        submitcommand = buildSubmitCommand(args.submit,args.queue,jobid,args.memory)
        submit(command=submitcommand,inputs=commands,debug=args.debug,verbose=args.verbose)
      time.sleep(0.1)
  print "Scheduled "+str(len(pending))+" jobs"
  #print pending
  # if we run locally, we can execute the jobs in parallel
  if runlocal:
    execute(pending,args.processes) 

  # we are done, so we can run the merging if requested
  tqpath = os.environ.get("TQPATH")
  if tqpath == None:
      print("TQPATH environment variable not set.  Please set before running")
      return
  
  command = " ".join([os.path.join(tqpath,"share","tqmerge"),"-o",mergeout,"-t","runAnalysis" if not args.downmerge else "generalize"] + ['-Sum' if args.downmerge else ""] ) + " " +" ".join(results)
  if args.merge:
    ready = []
    # proper batch system polling is complicated to do in a portable fashion
    # so we just check whether the output files exist and hope for the best
    while True:
      for f in results:
        if not f in ready and os.path.isfile(f):
          ready.append(f)
      running = len(results) - len(ready)
      if running > 0:
        print("{:d} output files are not ready yet.".format(running))
        time.sleep(60)
      else:
        break
    # actually run the merging
    from subprocess import Popen, PIPE, STDOUT
    print("merging to "+mergeout)
    p = Popen(command, stdout=PIPE, stdin=PIPE, stderr=PIPE,shell=True)
    p.communicate()
    print("all done!")
  else:
    print("wait for the jobs to finish. then merge with:")
    print(command)

if __name__ == "__main__":
  # parse the CLI arguments
  from multiprocessing import cpu_count
  import socket
  machinename = socket.gethostname()
  batchtype = choosebin(["bsub","sbatch","qsub"])
  parser = argparse.ArgumentParser(description='Submt Jobs to run the HSG3 Run2 Analysis.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('cfgname', metavar='CONFIG', type=str, default="config/runMVA.cfg", help='path to the analysis config file to be processed')
  parser.add_argument('--options', metavar='KEY:VALUE', type=str, nargs='*', help='set (overwrite) an option with respect to the config file')
  parser.add_argument('--debug', action="store_const", const=True, default=False, help='print actions instead of performing them')
  parser.add_argument('--jobs', default="jobs.txt", help='define how jobs should be split over the sample folder')
  parser.add_argument('--maxSampleSize', default=-1., type=float, help='maximum cumulated file size per job before splitting into sub-jobs')
  parser.add_argument('--maxSampleCount' , default=-1., type=float, help='maximum number of files per job before splitting into sub-jobs')
  parser.add_argument('--merge', action="store_const", const=True, default=False, help='wait for the output and merge it')
  parser.add_argument('--outpath', default="output", help='directory where the output should appear')
  parser.add_argument('--identifier', default=str(int(time.time())), help='identify this submission chunk')
  parser.add_argument('--processes', default=cpu_count()-1, type=int, help='how many cores to be used in local mode')
  parser.add_argument('--verbose', '-v', action="store_const", const=True, default=False, help='provide the user with verbose output')
  parser.add_argument('--queue', default=choosequeue(batchtype,machinename), help='name of the queue to submit to')
  parser.add_argument('--memory', default=4, help='memory to be requested per job (in GB)')
  parser.add_argument('--command', default="runAnalysis.py", help='path or name of the executable that runs the analysis')
  parser.add_argument('--submit', default=batchtype, help='submission command')
  parser.add_argument('--downmerge', action="store_const", const=True, default=False, help='merge objects down to the paths listed in the jobs file')
  parser.add_argument('--voms', action="store_const", const=True, default=False, help='call "voms-proxy-init --voms atlas" in every job - requires passwordless .globus!')
  parser.add_argument('--setup', help='path to setup script (required when submitting without environment setup)')
  parser.add_argument('--pythonpath', help='PYTHONPATH variable to be used by the job (required when submitting without environment setup)')
  parser.add_argument('--pythonbin', help='path to pytho binary to be used by the job (required when submitting without environment setup)')
  args = parser.parse_args()
  main(args)
