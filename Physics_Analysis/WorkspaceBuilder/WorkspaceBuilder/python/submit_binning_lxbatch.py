#!/usr/bin/env python

__doc__ = "Send jobs to run binning_optimize on queue (LSF, PBS, Condor)"

# Takes the same arguments as TMVACalib.py + options for the batch jobs
# Modify the inputs to accept "lists" (comma separated values)
# Loop over the possible arguments and launch the jobs

import os
import itertools
import inspect
import stat
import datetime
import time
import logging
logging.basicConfig(level=logging.DEBUG)

from optparse import OptionGroup
path = os.path.dirname(os.path.abspath(__file__))


#--------------------------------------------------------
def define_edge(init_binning, pos='left'):
  binnings = []
  for i_p in range(1, len(init_binning)):
    if pos == 'left':
      binning = [init_binning[0]] + init_binning[i_p:]
    elif pos == 'right':
      binning =  init_binning[:i_p] + [init_binning[-1]]
    else:
      raise ValueError('wrong pos')
    if binning != init_binning:
      binnings.append(binning)
  return binnings

#--------------------------------------------------------
def define_spacing(init_binning, spacing=10):
    xmin, xmax = init_binning[1], init_binning[-2]
    dx = xmax - xmin
    nbins = (xmax - xmin) / float(spacing)
    binning = init_binning[0:1] + [xmin + i * spacing for i in xrange(int(nbins))] + init_binning[-2:]
    return binning


def make_list(x):
  "Return an iterable object, i.e. the object itself or a list with a single element"
  if hasattr(x, '__iter__'):
    return x
  else:
    return [x]


def pairwise(iterable):
  "s -> (s0,s1), (s1,s2), (s2, s3), ..."
  from itertools import tee, izip
  a, b = tee(iterable)
  next(b, None)
  return izip(a, b)


class Submitter(object):
  def __init__(self, job_arg=None, do_submit=True):
    self.do_submit = do_submit
    if job_arg is None:
      self.job_arg = []
    elif type(job_arg) is str:
      self.job_arg = [job_arg]
    else:
      self.job_arg = job_arg

  def copy_proxy(self):
    # copy the proxy and change env var
    logging.info("setting proxy to local folder")
    os.system("cp $X509_USER_PROXY proxy")
    os.environ['X509_USER_PROXY'] = os.getcwd() + "/proxy"

  def submit(self, job_name, args):
    raise NotImplementedError


class SubmitterSimple(Submitter):
  def __init__(self, job_arg, batch_command, job_name_key, do_submit=True):
    super(SubmitterSimple, self).__init__(job_arg, do_submit)
    self.batch_command = batch_command
    self.job_name_key = job_name_key  # -J or -N

  def prepare_script(self, args, inputfiles):
    time.sleep(0.05)
    now = str(datetime.datetime.now()).replace(' ', '_')
    script_name = 'run_binning_optimizer_%s.sh' % now

    full_script_name = os.path.join(path, '../scripts', script_name)
    runArgs = ' '.join('--{0} {1}'.format(i, j) if j is not None else "--{0}".format(i) for i, j in args.iteritems())

    with open(os.path.join(path, '../scripts/run.template.sh')) as f:
      text = f.read().format(path=path, args=runArgs, inputfiles=inputfiles)
    with open(full_script_name, 'w') as f:
      f.write(text)
    os.chmod(full_script_name, stat.S_IEXEC | stat.S_IREAD | stat.S_IWRITE)

    return full_script_name

  def submit(self, job_name, args, inputfiles):
    logging.info("submitting on lsf %s", job_name)
    full_script_name = self.prepare_script(args, inputfiles)
    path, script_name = os.path.split(full_script_name)
    os.chdir(path)

    command = '{command} {job_name_key} {job_name} {job_args} {script_name}'.format(
      command=self.batch_command, 
      job_name=job_name,
      job_name_key=self.job_name_key,
      job_args=' '.join(self.job_arg),
      script_name=script_name)

    # Send jobs or print the command
    if not self.do_submit:
      os.system('pwd')
      print os.getcwd()
      print command, '\n'
    else:
      os.system(command)


class SubmitterLSF(SubmitterSimple):
  def __init__(self, job_arg=None, do_submit=True):
    super(SubmitterLSF, self).__init__(job_arg, "bsub", "-J", do_submit)


class SubmitterPBS(SubmitterSimple):
  def __init__(self, job_arg=None, do_submit=True):
    super(SubmitterPBS, self).__init__(job_arg, "qsub", "-N", do_submit)


class SubmitterCondor(Submitter):
  def __init__(self, job_arg=None, do_submit=True):
    super(SubmitterCondor, self).__init__(job_arg, do_submit)
    self.copy_proxy()

  def prepare_script(self, job_name, args, inputfiles):
    time.sleep(0.05)
    now = str(datetime.datetime.now()).replace(' ', '_')
    script_name = 'run_TMVACalib_%s.vanilla' % now

    full_script_name = os.path.join('../scripts/', script_name)
    runArgs = ' '.join('--%s %s' % (i, j) if j is not None else "--%s" % i for i, j in args.iteritems())

    template = ['Executable= ../python/TMVACalib.py',
                'Universe = vanilla',
                'output = {job_name}.out',
                'error = {job_name}.err',
                'log = {job_name}.log',
                'arguments = {args} {inputfiles}',
                'use_x509userproxy = TRUE',
                'getenv = TRUE']
    for i in self.job_arg:
        template.append(i)
    template.append("queue")

    template = '\n'.join(template)
    template = template.format(job_name=job_name, args=runArgs, inputfiles=inputfiles)
    template = template.replace('"', '\\"')
    with open(full_script_name, 'w') as f:
      f.write(template)
    return full_script_name

  def submit(self, job_name, args, inputfiles):
    logging.info("submitting on condor %s", job_name)
    full_script_name = self.prepare_script(job_name, args, inputfiles=inputfiles)
    path, script_name = os.path.split(full_script_name)
    os.chdir(path)

    command = 'condor_submit %s' % full_script_name

    if self.do_submit:
      os.system(command)
    else:
      print command


def run_hh(submitter, inputfiles, pos='left', noSubmit=False):
  """
  Send jobs to batch queue, using all the combinations of etaRange, etRange,
  particleTypes and calibrationTypes (which should be iterables).
  <common_args> is a dictionary containing arguments for all jobs"""


  categories = [
    # 'hhAll_cba_vbf_lowdr_signal',
    # 'hhAll_cba_vbf_highdr_tight_signal',
    # 'hhAll_cba_vbf_highdr_loose_signal',
    'hhAll_cba_boost_tight_signal',
    'hhAll_cba_boost_loose_signal',
    ]
  inputfiles = '/eos/user/q/qbuat/workspaces/htt_fine_binning/hhAll_merged_WSinput_WithTheoryEnv_WithModJERVar.root'
  
  initial_binnings = {
    'left': {
      'hhAll_cba_vbf_lowdr_signal':        [5 * i for i in xrange(41)],
      'hhAll_cba_vbf_highdr_tight_signal': [5 * i for i in xrange(41)],
      'hhAll_cba_vbf_highdr_loose_signal': [5 * i for i in xrange(41)],
      'hhAll_cba_boost_tight_signal': [5 * i for i in xrange(41)],
      'hhAll_cba_boost_loose_signal': [5 * i for i in xrange(41)],
      },

    'right': {
      'hhAll_cba_vbf_lowdr_signal':        [0, 80] + [85 + 5 * i for i in xrange(24)],
      'hhAll_cba_vbf_highdr_tight_signal': [0, 80] + [85 + 5 * i for i in xrange(24)],
      'hhAll_cba_vbf_highdr_loose_signal': [0, 80] + [85 + 5 * i for i in xrange(24)],
      'hhAll_cba_boost_tight_signal':      [0, 60] + [65 + 5 * i for i in xrange(28)],
      'hhAll_cba_boost_loose_signal':      [0, 60] + [65 + 5 * i for i in xrange(28)],
      },

    'middle': {
      'hhAll_cba_vbf_lowdr_signal':        [0, 80] + [85 + i * 5 for i in xrange(19)] + [180, 200],
      'hhAll_cba_vbf_highdr_tight_signal': [0, 80] + [85 + i * 5 for i in xrange(15)] + [160, 200],
      'hhAll_cba_vbf_highdr_loose_signal': [0, 80] + [85 + i * 5 for i in xrange(15)] + [160, 200],
      'hhAll_cba_boost_tight_signal':      [0, 60] + [65 + 5 * i for i in xrange(20)] + [150, 200],
      'hhAll_cba_boost_loose_signal':      [0, 60] + [65 + 5 * i for i in xrange(20)] + [150, 200],
      }
    }
  
  pos = pos
  print
  for cat in categories:
    if 'vbf' in cat:
      statanalysisname = 'HTauTau13TeVHHVBFCBA'
    elif 'boost' in cat:
      statanalysisname = 'HTauTau13TeVHHBoostCBA'

    print 'category:', cat
    init_binning = initial_binnings[pos][cat]
    if pos in ('left', 'right'):
      binnings = define_edge(init_binning, pos=pos)
    elif pos == 'middle':
      binnings = [define_spacing(init_binning, spacing=spacing) for spacing in [5, 10, 15, 20, 25, 30, 35, 40]]
    else:
      raise RuntimeError

    for binning in binnings:
      
      print 'binning:', binning
      args = {'category': cat, 
              'statanalysisname': statanalysisname,
              'position': pos,
              'channel': 'hh',
              "binning": ' '.join([str(e) for e in binning])}

      if args['position'] == 'left':
        edge = binning[1]
      elif args['position'] == 'right':
        edge = binning[-2]
      elif args['position'] == 'middle':
        edge = binning[2] - binning[1]

      args['suffix'] = '{category}_{position}_{edge}'.format(
        category=args['category'], 
        position=args['position'], 
        edge=edge)

      job_name = 'job_' + args['suffix']
      print job_name
      if not isinstance(inputfiles, str):
        inputfiles = ' '.join(inputfiles)

      # Loop over all combinations 
      # and submit jobs or print the command
      submitter.submit(job_name, args, inputfiles)

def run_ll(submitter, inputfiles, pos='left', noSubmit=False):
  """
  Send jobs to batch queue, using all the combinations of etaRange, etRange,
  particleTypes and calibrationTypes (which should be iterables).
  <common_args> is a dictionary containing arguments for all jobs"""


  categories = [
    'llAll_cba_boost_tight_signal',
    'llAll_cba_boost_loose_signal',
    'llAll_cba_vbf_tight_signal',
    'llAll_cba_vbf_loose_signal',
    ]
  #inputfiles = '/eos/user/q/qbuat/workspaces/htt_fine_binning/hhAll_merged_WSinput_WithTheoryEnv_WithModJERVar.root'
  
  initial_binnings = {
    'left': {
      'llAll_cba_boost_loose_signal':        [60] + [60 + 5 * i for i in xrange(29)],
      'llAll_cba_boost_tight_signal':        [60] + [60 + 5 * i for i in xrange(29)],
      'llAll_cba_vbf_loose_signal':          [55] + [55 + 5 * i for i in xrange(31)],
      'llAll_cba_vbf_tight_signal':          [55] + [55 + 5 * i for i in xrange(31)],
      },

    'right': {
      'llAll_cba_boost_loose_signal':     [60, 70, 80, 90, 100, 110, 120, 130] + [135 + 5 * i for i in xrange(14)],
      'llAll_cba_boost_tight_signal':     [60, 70, 80, 90, 100, 110, 120, 130] + [135 + 5 * i for i in xrange(14)],
      'llAll_cba_vbf_loose_signal':       [55, 70, 80, 90, 100, 110, 120, 130] + [135 + 5 * i for i in xrange(15)],
      'llAll_cba_vbf_tight_signal':       [55, 70, 80, 90, 100, 110, 120, 130] + [135 + 5 * i for i in xrange(15)],
      },

    'middle': {
      'llAll_cba_boost_tight_signal':      [60] + [95 + 5 * i for i in xrange(14)] + [170, 200],
      'llAll_cba_boost_loose_signal':      [60] + [70 + 5 * i for i in xrange(14)] + [185, 200],
      'llAll_cba_vbf_tight_signal':        [55] + [60 + i * 5 for i in xrange(14)] + [195, 205],
      'llAll_cba_vbf_loose_signal':        [55] + [85 + i * 5 for i in xrange(14)] + [190, 205],
      }
    }
  
  pos = pos
  print
  for cat in categories:
    if 'vbf' in cat:
      statanalysisname = 'HTauTau13TeVLLVBFCBA'
    elif 'boost' in cat:
      statanalysisname = 'HTauTau13TeVLLBoostCBA'

    print 'category:', cat
    init_binning = initial_binnings[pos][cat]
    if pos in ('left', 'right'):
      binnings = define_edge(init_binning, pos=pos)
    elif pos == 'middle':
      binnings = [define_spacing(init_binning, spacing=spacing) for spacing in [5, 10, 15, 20, 25, 30, 35, 40]]
    else:
      raise RuntimeError

    for binning in binnings:
      
      print 'binning:', binning
      args = {'category': cat, 
              'statanalysisname': statanalysisname,
              'position': pos,
              'channel': 'll',
              "binning": ' '.join([str(e) for e in binning])}

      if args['position'] == 'left':
        edge = binning[1]
      elif args['position'] == 'right':
        edge = binning[-2]
      elif args['position'] == 'middle':
        edge = binning[2] - binning[1]

      args['suffix'] = '{category}_{position}_{edge}'.format(
        category=args['category'], 
        position=args['position'], 
        edge=edge)

      job_name = 'job_' + args['suffix']
      print job_name
      if not isinstance(inputfiles, str):
        inputfiles = ' '.join(inputfiles)

      # Loop over all combinations 
      # and submit jobs or print the command
      submitter.submit(job_name, args, inputfiles)


if __name__ == '__main__':
  from optparse import OptionParser, OptionGroup
  parser = OptionParser("%prog [options] <inputfiles>")

  group = OptionGroup(parser, "Batch job options")
  group.add_option('--position', default='left', choices=['left', 'right', 'middle'])
  group.add_option("--noSubmit", help="Do not submit the jobs, only print the commands", default=False, action="store_true")
  group.add_option("-q", "--queue", help="Batch queue (default: %default)", default='1nd')
  group.add_option("-u", "--urgent", help="Urgent priority", default=False, action="store_true")
  group.add_option("--driver", choices=["auto", "LSF", "PBS", "condor"], default="auto")
  group.add_option("--additional-conf", type=str)
  parser.add_option_group(group)
  (options, inputfiles) = parser.parse_args()

  import socket
  hostname = socket.gethostname()
  isCERN = 'cern.ch' in hostname
  isLyon = 'in2p3.fr' in hostname
  isMilan = 'mi.infn.it' in hostname

  job_args = []
  if options.driver == "auto":
    if isCERN:
       options.driver = "LSF"
       if options.urgent:
         job_args.append(' -m g_atlascaturgent')
    elif isLyon:
      options.driver = "pbs"
      job_args = ['-P P_atlas -l ct=03:00:00 -l vmem=3G,fsize=10G,sps=1 -o /sps/atlas/b/blenzi/job_outputs/ -j y']
      if options.urgent:
         job_args.append(' -m g_atlascaturgent')
    elif 'tier3.mi.infn.it' in hostname:
      options.driver = "pbs"
    elif isMilan:
      job_args = ["Requirements = (OpSysMajorVer == 6)"]
      options.driver = "condor"
    else:
      raise ValueError('Invalid host: %s' % os.environ['HOSTNAME'])

  if options.additional_conf:
    job_args.append(options.additional_conf)

  if options.driver == "LSF":
    job_args.append("-q %s" % options.queue)
    submitter = SubmitterLSF(job_args, do_submit=not options.noSubmit)
  elif options.driver == "pbs":
    job_args.append("-q %s" % options.queue)
    submitter = SubmitterPBS(job_args, do_submit=not options.noSubmit)
  elif options.driver == "condor":
    submitter = SubmitterCondor(job_args, do_submit=not options.noSubmit)
  else:
    assert False

  #run_hh(submitter, inputfiles, options.position, options.noSubmit)
  run_ll(submitter, inputfiles, options.position, options.noSubmit)
