#!/user/bin/env python2

#creates submit jobs for BFG

samplePathFileName = "samplePathList.txt"
samplePathFile = open(samplePathFileName,'r')

submitJobsFileName = "submitJobs.sh"
submitJobsFile = open(submitJobsFileName,'w')

queue = 'express'
time = '20:00'

for samplePath in samplePathFile:
    samplePathClean = samplePath.replace('\n','')
    samplePathCleaner = samplePathClean.replace('/b','b').replace('/d','d').replace('/','_')

    jobFileName = samplePathCleaner + '.sh'

    submitJobsFile.write('sbatch -p ' + queue + ' -t ' + time + ' ' + jobFileName + '\n')

    jobFile = open(jobFileName,'w')
    
    jobFile.write('#!/bin/bash\n\n')
    jobFile.write('#SBATCH -o /storage/groups/atlsch/dirk/slurm_log/' + jobFileName + '.%j.%N.log\n')
    jobFile.write('#SBATCH -n 1\n')
    jobFile.write('#SBATCH --mem-per-cpu=3000\n\n')
    jobFile.write('echo BEGIN OF JOB\n')
    jobFile.write('date\n\n')
    jobFile.write('cd /storage/groups/atlsch/dirk/framework/\n')
    jobFile.write('rcSetup\n')
    jobFile.write('source pathstuff.sh\n')
    jobFile.write('cd Htautau2015/share/\n\n')
    jobFile.write('echo $GCC_DIR\n')
    jobFile.write('echo $GccVersion\n')
    jobFile.write('echo $ROOTSYS\n\n')
    jobFile.write('time ./runAnalysis.py config/1015.cfg --restrict ' + samplePathClean + ' --options outputFile:output/samples-' + samplePathCleaner + '.root:samples\n\n')
    jobFile.write('date\n')
    jobFile.write('echo END OF JOB')
    
    jobFile.close()

submitJobsFile.close()
samplePathFile.close()
