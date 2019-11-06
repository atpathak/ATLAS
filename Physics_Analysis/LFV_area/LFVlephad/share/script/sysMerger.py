#!/usr/bin/env python

import os

weightSysPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/systematics/weightsystematics'
kinematicSysPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/systematics/kinematicsystematics'
fakeSysPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/systematics/fakesystematics'
outputPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/output/'

systs = []

with open(weightSysPath) as weightSysFile:
    for line in weightSysFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0 and not '_low' in line:
            line = line.replace('_high','')
            #print line
            systs.append(line)

with open(kinematicSysPath) as kinematicSysFile:
    for line in kinematicSysFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0 and not '_low' in line:
            line = line.replace('_high','')
            line = line.split(' ')[0]
            #print line
            systs.append(line)

with open(fakeSysPath) as fakeSysFile:
    for line in fakeSysFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0 and not '_low' in line:
            line = line.replace('_high','')
            #print line
            systs.append(line)
            
for sys in systs:
    #print sys
    os.system('tqmerge -o '+ outputPath + sys +'.root ' + outputPath + '*bkg*_' + sys + '_* -t runAnalysis')
