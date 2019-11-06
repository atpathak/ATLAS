#!/user/bin/env python2

import sys
import os
from optparse import OptionParser

#creates submit jobs for BFG

parser = OptionParser()
parser.add_option('-n', '--name', dest='name', default=None)
parser.add_option('-q', '--queue', dest='queue', default=None)
parser.add_option('-t', '--time', dest='time', default=None)
parser.add_option('-c', '--config', dest='config', default=None)

(options, args) = parser.parse_args()

if options.name == None:
    print "ERROR: You need to define a name with -n <name>"
    sys.exit()
if options.queue == None:
    print "ERROR: You need to define a queue with -q <queue>"
    sys.exit()
if options.time == None:
    print "ERROR: You need to define a walltime with -t <time>"
    sys.exit()
if options.config == None:
    print "ERROR: You need to define a config file with -c <config>"
    sys.exit()

name = options.name
queue = options.queue
time = options.time
config = options.config

#samplePathList = ['/bkg/muhad/Diboson',
#                  '/bkg/muhad/WjetsInc',
#                  '/bkg/muhad/ZllPoPy',
#                  '/bkg/muhad/ZttPoPy',
#                  '/bkg/muhad/Top',
#                  '/sig/muhad/ggH',
#                  '/sig/muhad/VBFH',
#                  '/data/muhad/,/bkg/muhad/ddbkg'] #this is correct and no typo!
#                  #'/data/muhad'] 

samplePathList = ['/bkg/muhad/ZttPoPy',
                  '/bkg/muhad/Top',
                  '/sig/muhad/ggH',
                  '/sig/muhad/VBFH',
                  '/data/muhad/,/bkg/muhad/ddbkg'] 


if not os.path.exists(name):
    os.makedirs(name)
if not os.path.exists('/storage/groups/atlsch/dirk/slurm_log/' + name):
    os.makedirs( '/storage/groups/atlsch/dirk/slurm_log/' + name)


submitJobsFileName = 'submitJobs.sh'
submitJobsFile = open(name + '/' + submitJobsFileName,'w')

for samplePath in samplePathList:
    samplePathClean = samplePath.replace('\n','')
    samplePathCleaner = samplePathClean.replace('/,/','_').replace('/b','b').replace('/da','da').replace('/s','s').replace('/','_').replace(',','_')
    
    jobFileName = samplePathCleaner + '.sh'

    submitJobsFile.write('sbatch -p ' + queue + ' -t ' + time + ' ' + jobFileName + '\n')

    jobFile = open(name + '/' + jobFileName,'w')
    
    jobFile.write('#!/bin/bash\n\n')
    jobFile.write('#SBATCH -o /storage/groups/atlsch/dirk/slurm_log/' + name + '/' + jobFileName + '.%j.%N.log\n')
    jobFile.write('#SBATCH -n 1\n')
    jobFile.write('#SBATCH --mem-per-cpu=3000\n\n')
    jobFile.write('echo BEGIN OF JOB\n')
    jobFile.write('date\n\n')
    jobFile.write('cd /storage/groups/atlsch/dirk/framework_test/\n')
    jobFile.write('rcSetup\n')
    jobFile.write('source pathstuff.sh\n')
    jobFile.write('cd Htautau2015/share/\n\n')
    jobFile.write('echo $GCC_DIR\n')
    jobFile.write('echo $GccVersion\n')
    jobFile.write('echo $ROOTSYS\n\n')
    jobFile.write('time ./runAnalysis.py config/'+ config + '.cfg --restrict ' + samplePathClean + ' --options outputFile:output/' + name + '/samples-' + samplePathCleaner + '.root:samples\n\n')
    jobFile.write('date\n')
    jobFile.write('echo END OF JOB')
    
    jobFile.close()

submitJobsFile.close()

