#!/usr/bin/env python

from QFramework import *

weightSysPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/systematics/weightsystematics'
kinematicSysPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/systematics/kinematicsystematics'
fakeSysPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/systematics/fakesystematics'
outputPath = '/storage/groups/atlsch/dirk/framework_test/Htautau2015/share/output/'
nominalFiles = ['fake__bkg_lephad_Fake_data','fake__bkg_lephad_Diboson','fake__bkg_lephad_Top','fake__bkg_lephad_WjetsInc','fake__bkg_lephad_ZllPoPy','fake__bkg_lephad_ZttPoPy']

systs = []
fakesysts = []

with open(weightSysPath) as weightSysFile:
    for line in weightSysFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
            systs.append(line)

with open(kinematicSysPath) as kinematicSysFile:
    for line in kinematicSysFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
            line = line.split(' ')[0]
            systs.append(line)

with open(fakeSysPath) as fakeSysFile:
    for line in fakeSysFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
            fakesysts.append(line)

for nominalFile in nominalFiles:
    if 'Fake' in nominalFile:
        for sys in systs:
            samples = TQSampleFolder.loadSampleFolder(outputPath+nominalFile+'.root:samples')
            subfolders = samples.getListOfSampleFolders("?")
            for subfolder in subfolders:
                if 'bkg' in str(subfolder):
                    original = subfolder.getSampleFolder("lephad")
                    newchannel = "lephad_"+sys
                    newfolder = original.copy(newchannel)
                    subfolder.addFolder(newfolder)
                    nameCopy = nominalFile
                    nameCopy = nameCopy.replace("lephad",newchannel)
                    #original.deleteAll()
                    samples.writeToFile(outputPath+nameCopy+'.root')
    else:
        for fakesys in fakesysts:
            samples = TQSampleFolder.loadSampleFolder(outputPath+nominalFile+'.root:samples')
            subfolders = samples.getListOfSampleFolders("?")
            for subfolder in subfolders:
                if 'bkg' in str(subfolder):
                    original = subfolder.getSampleFolder("lephad")
                    newchannel = "lephad_"+fakesys
                    newfolder = original.copy(newchannel)
                    subfolder.addFolder(newfolder)
                    nameCopy = nominalFile
                    nameCopy = nameCopy.replace("lephad",newchannel)
                    #original.deleteAll()
                    samples.writeToFile(outputPath+nameCopy+'.root')

