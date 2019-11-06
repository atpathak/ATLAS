#!/usr/bin/env python
# Frank Sauerburger, 2016-03

import os
import commands
import argparse
import subprocess
import re
import time
import glob
from random import shuffle
from abc import ABCMeta, abstractmethod

"""
Note: the unit tests can be run with
  python -m doctest submit.py
The unit tests depend on configuration files. If the unit tests fail, make sure
that the external files are present, or adjust the tests.
"""


class SlurmException(Exception):
  pass

class BoilerplateException(Exception):
  pass

class MVAOptionsNotInConfig(Exception):
  pass

class Job(object):
  __metaclass__ = ABCMeta
  boilerplate = { }
  boilerplate_path=None

  def get_boilerplate(self):
    cls = self.get_extention()
    if cls not in Job.boilerplate:
      path_alternatives = [Job.boilerplate_path]
      if cls != "default":
        path = "%s_%s" % (Job.boilerplate_path, cls)
        path_alternatives = [path] + path_alternatives

      for path in path_alternatives:
        path = os.path.expandvars(path)
        path = os.path.expanduser(path)

        try:
          with open(path) as b:
            Job.boilerplate[cls] = b.read()
            break
        except:
          pass
      else:
        raise BoilerplateException("No boilerplate file found. Tried " + ", ".join(path_alternatives))

    return Job.boilerplate[cls]

  def __init__(self, name=None, dependency=None):
    self.name = name
    self.dependency = dependency

  def __repr__(self):
    return repr(self.options)

  def get_extention(self):
    return "default"

  def submit(self, name=None, dependency=None):
    # merge dependencies
    if self.dependency is not None:
      if dependency is None: dependency = []
      dependency += self.dependency

    job_id = submit_job( self.get_script(), name=name or self.name,
      dependency=dependency)
    
    print "  Submitted job %s with id %d" % (name or self.name, job_id)
    return job_id

  @abstractmethod
  def get_script(self):
    pass

class MakeSampleFileJob(Job):
  def __init__(self, config, name=None, dependency=None, sample_file=None, mc_path=None):
    Job.__init__(self, name=name, dependency=dependency)
    self.config = config
    self.options = {}
    if sample_file is not None:
      self.options['sampleFile'] = sample_file
    if sample_file is not None:
      self.options['mcPaths'] = mc_path

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "python makeSampleFile.py " + self.config
    if len(self.options) > 0:
      script += " --options %s" % " ".join(["%s:%s" % kv for kv in self.options.items()])
    return script

  def get_extention(self):
    return "make"

class RunAnalysisJob(Job):
  def __init__(self, config, name=None, dependency=None, restrict=None,
      MVA=None, output_file=None, multi_channel_visitor=False, options=None):
    Job.__init__(self, name=name, dependency=dependency)
    self.config = config
    self.restrict = restrict
    if options is None:
        options = {"-q": "8nh"}
    self.options = options
    if MVA is not None:
      self.options['MVA'] = "'%s'" % MVA.replace("'", r"\'")
    if output_file is not None:
      self.options['outputFile'] = output_file
    if multi_channel_visitor:
      self.options['useMultiChannelVisitor'] = 'True'

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "python runAnalysis.py %s" % self.config
    script += " --downmerge"
    if self.restrict:
      script += " --restrict %s" % self.restrict
    if len(self.options) > 0:
      script += " --options %s" % " ".join(["%s:%s" % kv for kv in self.options.items()])
    return script
    
  def get_extention(self):
    return "run"

class DuplicateJob(Job):
  def __init__(self, src_channel, dest_channel, file_pattern, paths, dependency, name):
    Job.__init__(self, name=name, dependency=dependency)
    self.src_channel = src_channel
    self.dest_channel = dest_channel
    self.file_pattern = file_pattern
    self.paths = paths

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "scripts/duplicate.py --src %s --dest %s --pattern %s %s" % \
      (self.src_channel, self.dest_channel, self.file_pattern, " ".join(self.paths))
    return script
    
  def get_extention(self):
    return "duplicate"

class MergeJob(Job):
  def __init__(self, config, file_list, output_file, name=None, dependency=None):
    Job.__init__(self, name=name, dependency=dependency)
    self.config = config
    self.file_list = file_list
    self.output_file = output_file

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "tqmerge -o %s %s -t runAnalysis" % (self.output_file, " ".join(self.file_list))
    return script

  def get_extention(self):
    return "merge"
    

class WSInputMaking(Job):
  def __init__(self, config, macro, name=None, dependency=None):
    Job.__init__(self, name=name, dependency=dependency)
    self.config = config
    self.macro=macro

  def get_script(self):
    rootcorebin=commands.getoutput('echo ${ROOTCOREBIN}')
    if args.dryrun == False:
      os.system('echo QFWConfig: %s/../Htautau2015/share/%s > macros/mkWBInputs/QFWConfigLoc.env' % (rootcorebin, self.config))
    script = self.get_boilerplate() + "\n"
    script += "tqroot -b macros/mkWBInputs/forSbatchSubmission/%s" % self.macro
    return script

  def get_extention(self):
    return "wsinput_make"

class WSInputMerging(Job):
  def __init__(self, config, filenamePrefix, name=None, dependency=None):
    Job.__init__(self, name=name, dependency=dependency)
    self.config = config
    self.filenamePrefix=filenamePrefix

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "hadd -f %s.root %s_*.root" % (self.filenamePrefix, self.filenamePrefix)
    return script

  def get_extention(self):
    return "wsinput_merge"
    
class CreateSystematicsJob(Job):
  def __init__(self, config, name=None, dependency=None):
    Job.__init__(self, name="createSysmatics", dependency=dependency)
    self.config = config

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "./createSystematics.py %s" % self.config
    return script

  def get_extention(self):
    return "create"
    
class ReadAnalysisJob(Job):
  def __init__(self, config, name=None, dependency=None):
    Job.__init__(self, name=name, dependency=dependency)
    self.config = config

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "python readAnalysis.py " + self.config
    return script

  def get_extention(self):
    return "read"
    

class ErrorChecking(Job):
  def __init__(self, job_ids, name=None, dependency=None):
    Job.__init__(self, name=name, dependency=dependency)
    self.job_ids = job_ids

  def get_script(self):
    script = self.get_boilerplate() + "\n"
    script += "echo %s\n" % \
              " ".join(["*%s*" % id for id in self.job_ids])
    script += "grep --ignore-case -E \"ERROR|WARN|KILL|c++\" %s | grep -v numerical | grep -v fConstLD" \
              " | grep -v \"using config option\"" % \
              " ".join(["*%s*" % id for id in self.job_ids])
    return script

  def get_extention(self):
    return "error"

dry_run = False
job_dir = None
def submit_job(script, dependency=None, name=None):
  """
  Submits a new slurm job. The argument script should contain the contents of
  the script and not the path to the script. The argument will be passed
  directly as stdin to slurm. The return value is the job id.
  >>> id = submit_job("#!/bin/bash\\nls -l")
  >>> id > 0
  True

  If submission of a job fails, an exception is thrown.
  
  The optional parameter dependency specifies a dependency for the job. If it is a
  integer, the job will wait until the job with the given id succeeds. If the
  parameter is a list of integers, the job will until all jobs in the list
  succeeded.

  The optional parameter name specifies the value passed to sbatch --job-name.
  Whitspaces in the given value will be replaces by <backslash><space> before
  passed to slurm.
  """
  global dry_run

  if dependency is None:
    dependency = []
  elif isinstance(dependency, int):
    dependency = [str(dependency)]
  else:
    # dependency is supposed to be a list of integers
    dependency = [str(d) for d in dependency]

  # dependency is now a list of strings
  if len(dependency) > 0:
    dependency = ["--dependency=afterok:" + ":".join(dependency)]
  else:
    dependency = []

  if name is None:
    job_name = []
    script = script.replace("%M", "")
  else:
    script = script.replace("%M", sanitize(name))
    # escape name argument
    escaped_name = re.sub(r'\s+ ', r'\ ', name)
    job_name = ['--job-name=%s' % escaped_name]


  if dry_run:
    id = dry_run
    dry_run += 1
  else:
    while True:
      job_submission = subprocess.Popen(["bsub"] + dependency + job_name,
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      (stdoutdata, stderrdata) = job_submission.communicate(script)
      if job_submission.returncode == 0:
          break
      time.sleep(1)
      # retry
          

    id = int(time.time()) 
    # = int(stdoutdata.split(" ")[-1])


  if job_dir is not None:
    if name is not None:
      filename = "%s--slurm%d.sh" % (sanitize(name), id)
    else:
      filename = "%d.sh" % id

    with open(os.path.join(job_dir, filename), "w") as f:
      f.write(script)

  return id

def sanitize(name):
  """
  Replaces all characters which are not allowed in filenames by an underscore
  such that the return value can be used as a filename.
  >>> sanitize("hello_123-this_is_ok.txt")
  'hello_123-this_is_ok.txt'
  >>> sanitize("hello/bgk?/some%$other/data.x")
  'hello_bgk__some__other_data.x'
  """
  return re.sub("[^~a-zA-Z0-9_.-]", "_", name)

def parse_config(config_file):
  """
  This method parses the configuration file and returns all the options in a
  dictionary. If a line in the configuration file does not match the expected
  format it is simply skipped.
  >>> config = parse_config("config/htt_lephad_fake.cfg")
  >>> config['makeSampleFile.luminosityUnit']
  'fb'
  """

  config = { }
  with open(config_file) as f:
    for line in f:
      m = re.match(r'([\w.-]+):\s*([^#]*)\s*', line)
      if m:
        config[m.group(1).strip()] = m.group(2).strip()
  return config
  

if __name__ == "__main__":

  parser = argparse.ArgumentParser(
    description="Simple script to submit the jobs of the QFramwork. One job "
    "for each file in the paths file is submitted.\n\n"
    "For example to run the full analysis run "
    "  \"./scripts/submit.py <config_file> -p bdt.jobs -krse\". "
    "Once you have the samples (after makeSampleFile.py) you can leave out "
    "the 'k' in the option list.",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  
  parser.add_argument("-k", "--makeSampleFile", action="store_true", default=False,
    help="Submit the makeSampleFile job using the default config.")

  parser.add_argument("-q",  action="store_true", default=False,
    help="Submit a job BDT training job using the default config.")

  parser.add_argument("--train", type=str, default=None,
    help="Submit a job BDT training job using a custom config.")

  parser.add_argument("-r", "--runAnalysis", action="store_true", default=False,
    help="Submit the runAnalysis job(s) using the default config.")
  
  parser.add_argument("-s", "--readAnalysis", action="store_true", default=False,
    help="Submit the readAnalysis job using the default config.")
  
  parser.add_argument("-i", "--duplicate", action="store_true", default=False,
    help="Submit a duplicate nominal to systematics job.")

  parser.add_argument("-g", "--merge", action="store_true", default=False,
    help="Submit a merge job.")

  parser.add_argument("-c", "--createSysmatics", action="store_true", default=False,
    help="Submit a create systematics job with 16GB ram.")

  parser.add_argument("-p", "--paths", type=str, required=True,
    help="A filename which will be read line "
    "by line, each line is taken as a sample path for which one run analysis "
    "job is submitted.")

  parser.add_argument("-y", "--systematics", action="store_true", default=False,
    help="Add jobs to run the systematics. This option has no effect, when -r is "
    "not given.")

  parser.add_argument("-e", "--errors", action="store_true", default=False,
    help="Add jobs to check for errors in the log files.")

  parser.add_argument("-B", "--boilerplate", type=str,
    default="./.slurm_boilerplate",
    help="Path to a file with boilerplate code also containing the shebang "
    "and slurm options. This should be used to setup the environment.")

  parser.add_argument("-d", "--dryrun", action="store_true", default=False,
    help="Does not actually submit the jobs.")

  parser.add_argument("-l", "--sleep", default=1, type=int,
    help="Number of seconds to wait between job submissions. The option is "
    "ignored when --dryrun is given")

  parser.add_argument("-u", "--shuffle", action="store_true", default=False,
    help="Shuffle runAnalysis jobs.")

  parser.add_argument("-j", "--jobdir", 
    help="Directory where a bash script for each job will be created.")

  parser.add_argument("-w", "--wsinput", action="store_true", default=False,
    help="Making WorkspaceBuilder input file.")

  parser.add_argument("--restrict-systematics", 
    help="Regex to restrict the paths that are submitted (checked for "
    "run, duplicate and merge).")

  parser.add_argument("--restrict-path", 
    help="Regex to restrict the paths that are submitted (only checked for "
    "run).")

  parser.add_argument("-m", type=str, default="8nh")

  parser.add_argument("config", metavar="CONFIG", type=str, default=None,
    help="The path to the default configuration file.")


  args = parser.parse_args()

  if args.dryrun:
    dry_run = 1
    

  if args.jobdir:
    job_dir = args.jobdir

  Job.boilerplate_path = args.boilerplate

  dependencies = []
  all_job_ids = []

  if args.config is not None:
    config = parse_config(args.config)

  weightSystematics = []
  if "runAnalysis.weightSystematicsFile" in config and \
        config["runAnalysis.weightSystematicsFile"].lower() != "false":
    weightSystematicsPath = config["runAnalysis.weightSystematicsFile"]
    with open(weightSystematicsPath) as weightSystematicsFile:
      for line in weightSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          weightSystematics.append(line)

  kinematicSystematics = []
  path_replacements = {}
  if "runAnalysis.kinematicSystematicsFile" in config and \
        config["runAnalysis.kinematicSystematicsFile"].lower() != "false":
    kinematicSystematicsPath = config["runAnalysis.kinematicSystematicsFile"]
    with open(kinematicSystematicsPath) as kinematicSystematicsFile:
      for line in kinematicSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          tokens = re.split("\s+", line)
          name = tokens[0]
          kinematicSystematics.append(name)
          if len(tokens) == 3 and '->' in tokens[2] :
              path_replacements[name] = tuple(tokens[2].split('->'))


  fakeSystematics = []
  if "runAnalysis.fakeSystematicsFile" in config and \
        config["runAnalysis.fakeSystematicsFile"].lower() != "false":
    fakeSystematicsPath = config["runAnalysis.fakeSystematicsFile"]
    with open(fakeSystematicsPath) as fakeSystematicsFile:
      for line in fakeSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          fakeSystematics.append(line)

  job_ids = []
  # submit makeSampleFile
  if args.makeSampleFile:
    print "MakeSampleFile:"

    samples_file = config['makeSampleFile.sampleFile']
    make_jobs = {}
    make_jobs[None] = samples_file
    if args.systematics:
        unique_path_replacements = set(path_replacements.values())
        for search, replace in unique_path_replacements:
            make_jobs[(search, replace)] = samples_file.replace(".root", "_%s_%s.root" % (search, replace))

    for sr, output in make_jobs.items():
        mc_path = config['makeSampleFile.mcPaths']
        name = "makeSampleFile"
        if sr is not None:
            mc_path = mc_path.replace(*sr)
            name = "%s %s->%s" % (name, sr[0], sr[1])
        job = MakeSampleFileJob(config=args.config, sample_file=output, mc_path=mc_path)
        job_id = job.submit(name=name)
        job_ids.append(job_id)
        dependencies = job_ids
        all_job_ids += job_ids

        # for some reason this is necessary.
        if len(make_jobs) > 1:
            time.sleep(1)

  # prepare merge and batch
  if args.merge or args.runAnalysis or args.duplicate:
    output = config['runAnalysis.outputFile']
    output_pattern = output.replace(".root", "_%s.root")
    job_list = []
    merge_input_by_channel = {}
    items_to_duplicate = {}

    with open(args.paths) as f:
      for line in f:
        line = line.strip()
        if not line.startswith("#") and line != "":
          token = re.split("\\s*", line)
          line = token[0]
          if len(token) > 1:
            runOverSystematics = token[1:]
          else:
            runOverSystematics = []

          if args.systematics:
            for channel in re.split(",\\s*", config['runAnalysis.channels']):

              # nominal
              job_path = line.replace("?", channel)
              output_file = output_pattern % sanitize(job_path)
              nominal_job = RunAnalysisJob(config=args.config, 
                  restrict=job_path,
                  name=job_path,
                  output_file=output_file)
              job_list.append(nominal_job)

              if channel not in merge_input_by_channel: merge_input_by_channel[channel] = []
              merge_input_by_channel[channel].append(output_file)

              # systematics
              systematics_by_type = {
                'kinematic': kinematicSystematics,
                'weight': weightSystematics,
                'fake': fakeSystematics,
              }

              for type, systematics in systematics_by_type.items():
                for systematic in systematics:
                  if systematic.endswith("_high"):
                    raw_systematic_name = systematic[:-5]
                  elif systematic.endswith("_low"):
                    raw_systematic_name = systematic[:-4]
                  else:
                    raw_systematic_name = systematic

                  if type in runOverSystematics:
                    job_path = line.replace("?", "%s_%s" % (channel, systematic))
                    output_file = output_pattern % sanitize(job_path)

                    # use different samples input
                    if type == 'kinematic' and  systematic in path_replacements:
                        search, replace = path_replacements[systematic]
                        samples_file = config['makeSampleFile.sampleFile']
                        samples_file = samples_file.replace(".root", "_%s_%s.root" % (search, replace))
                        options = {'inputFile': samples_file}
                    else:
                        options = None

                    job_list.append(RunAnalysisJob(config=args.config,
                        restrict=job_path,
                        name=job_path,
                        options=options,
                        output_file=output_file))
  
                    label = "%s_%s" % (channel, raw_systematic_name)
                    if label not in merge_input_by_channel: merge_input_by_channel[label] = []
                    merge_input_by_channel[label].append(output_file)

                  elif 'DONT_DUPLICATE' not in runOverSystematics:
                    if (channel, systematic) not in items_to_duplicate:
                      items_to_duplicate[(channel, systematic)] = []

                    items_to_duplicate[(channel, systematic)].append(line)

                    label = "%s_%s" % (channel, raw_systematic_name)
                    job_path = line.replace("?", "%s_%s" % (channel, systematic))
                    output_file = output_pattern % sanitize(job_path)
                    if label not in merge_input_by_channel: merge_input_by_channel[label] = []
                    merge_input_by_channel[label].append(output_file)
  
                
          else:
            output_file = output_pattern % sanitize(line)
            job_list.append(RunAnalysisJob(config=args.config,
                restrict=line,
                #multi_channel_visitor=True,
                options={"kinematicSystematicsFile": "False", "weightSystematicsFile": "False", "fakeSystematicsFile": "False"},
                output_file=output_file,
                name=line))

            channel = "nominal"
            if channel not in merge_input_by_channel: merge_input_by_channel[channel] = []
            merge_input_by_channel[channel].append(output_file)

  # for key in items_to_duplicate:
  #   channel, systematic = key
  #   print "%s --> %s for" % key
  #   for path in items_to_duplicate[key]:
  #     print "  %s" % path
  #   print ""

  # submit BDT training
  if args.train or args.q:
    print "BDT Training:"
    training_config = parse_config(args.train or args.config)
    job_ids = []
    if "runAnalysis.MVA" not in training_config:
      raise MVAOptionsNotInConfig(args.train or args.config)
    for mva in re.split("\)\s*,", training_config["runAnalysis.MVA"]):
      mva = mva.strip()
      if (not mva.endswith(")")) and "(" in mva:
        mva = mva.strip() + ")"

      paths = []
      for channel in re.split(",\\s*", config['runAnalysis.channels']):
        paths.append("?/%s" % channel)
      paths = ",".join(paths)

      job = RunAnalysisJob(config=args.train or args.config, 
        name=mva, dependency=dependencies, restrict=paths,
        MVA=mva, options={"weightSystematicsFile": "False",
                          "kinematicSystematicsFile": "False",
                          "fakeSystematicsFile": "False"})

      job_id = job.submit()
      job_ids.append(job_id)
    dependencies = job_ids
    all_job_ids += job_ids

  # submit batched runAnalysis
  if args.runAnalysis:
    print "RunAnalysis:"
    job_ids = []
    if args.shuffle:
      shuffle(job_list)
    for i, job in enumerate(job_list):
      if args.restrict_path and not re.match(args.restrict_path, job.restrict):
        continue
      if args.restrict_systematics and not re.match(args.restrict_systematics, job.restrict):
        continue

      job_ids.append(job.submit(dependency=dependencies))
      if not args.dryrun and args.sleep > 0:
        time.sleep(args.sleep)
    dependencies = job_ids
    all_job_ids += job_ids

  # duplicate nominal to sys
  if args.duplicate:
    print "Duplicate nominal to sys:"
    job_ids = []

    for key in items_to_duplicate:
      src_channel, systematic = key
      dest_channel = "%s_%s" % key

      if args.restrict_systematics and not re.match(args.restrict_systematics, dest_channel):
        continue

      job = DuplicateJob(src_channel=channel,
          dest_channel=dest_channel,
          paths=items_to_duplicate[key],
          file_pattern=output_pattern,
          name="duplicate N -> %s" % systematic,
          dependency=dependencies)
      job_ids.append(job.submit())

    dependencies = job_ids
    all_job_ids += job_ids

  # submit merge
  if args.merge:
    print "Merge:"
    job_ids = []
    merge_output = config['runAnalysis.outputFile']
  
    # remove ':samples'
    if ':' in merge_output:
      merge_output = merge_output.split(':')[0]

    if "/" not in merge_output:
      path = "."
    else:
      path = merge_output[:merge_output.rindex("/")]

    for channel in merge_input_by_channel:
      if "_" in channel:
        name = channel[channel.index("_") + 1:]
      else:
        name = "nominal"

      if args.restrict_systematics and not re.match(args.restrict_systematics, channel):
        continue

      job = MergeJob(config=args.config,
          name="merge %s" % channel,
          file_list=merge_input_by_channel[channel],
          dependency=dependencies,
          output_file="%s/%s.root" % (path, name))
      job_ids.append(job.submit())

    dependencies = job_ids
    all_job_ids += job_ids

    #for channel in merge_input_by_channel:
    #  print channel, [x.options['outputFile'] for x in merge_input_by_channel[channel]]

  # submit WSInput making
  if args.wsinput:
    print "WBInputMaking:"
    job_ids = []
    stylefile = commands.getoutput("cat %s | grep -v -e '^\s*#' -e '^\s*$' | grep makeSampleFile.style | sed 's/[\t ]\+/\t/g' | cut -f2" % args.config)
    strUse2015 = commands.getoutput("cat %s | grep -v -e '^\s*#' -e '^\s*$' | grep use2015" % stylefile)
    strUse2016 = commands.getoutput("cat %s | grep -v -e '^\s*#' -e '^\s*$' | grep use2016" % stylefile)
    if strUse2015.find('true') > -1: use2015 = True
    else: use2015 = False
    if strUse2016.find('true') > -1: use2016 = True
    else: use2016 = False
    if use2015 & use2016:
      year = "_"
    elif use2015:
      year = "15"
    elif use2016:
      year = "16"
    else:
      print "Both use2015 and use2016 are false!!"
    macros = glob.glob('macros/mkWBInputs/forSbatchSubmission/mkAll*CBA%s*' % year)
    macros += glob.glob('macros/mkWBInputs/forSbatchSubmission/mkAll*CBAFINE%s*' % year)
    for i in macros:
      job = WSInputMaking(config=args.config,
          name="WS input making %s" % commands.getoutput("basename %s | cut -d'.' -f1" % i),
          dependency=dependencies,
          macro=commands.getoutput("basename %s" % i))
      job_ids.append(job.submit())
    print "WBInputMerging:"
    outfilename = commands.getoutput("cat %s | grep -v -e '^\s*#' -e '^\s*$' | grep runAnalysis.outputFile | sed 's/[\t ]\+/\t/g' | cut -f2 | cut -d':' -f1" % args.config)
    outputdir = commands.getoutput("dirname %s" % outfilename)
    tag=commands.getoutput("basename %s" % outputdir)
    if year == "_": year=''
    prefix = ( "%s/%s_cba_wbinput_coarseBins" % (outputdir, tag), "%s/%s_cba_wbinput_fineBins" % (outputdir, tag) )
    for i in prefix:
      job = WSInputMerging(config=args.config,
          name="WS input merging %s" % commands.getoutput("basename %s" % i),
          dependency=job_ids,
          filenamePrefix=i)
      job.submit()

  # submit createSysmatics
  if args.createSysmatics:
    print "Create Systematics:"
    job = CreateSystematicsJob(config=args.config, dependency=dependencies)
    job_ids.append(job.submit())

    all_job_ids += job_ids


  # submit readAnalysis
  if args.readAnalysis:
    print "ReadAnalysis:"
    job = ReadAnalysisJob(config=args.config,
        name="readAnalysis",
        dependency=dependencies)
    job_id = job.submit()
    dependencies = [job_id]
    all_job_ids.append(job_id)

  if args.errors:
    print "ErrorChecking:"
    job = ErrorChecking(name="error checking",
        dependency=dependencies,
        job_ids=all_job_ids)
    job.submit()
