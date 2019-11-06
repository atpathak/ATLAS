#!/user/bin/env python2

from QFramework import *

#returns a list of the samplepaths

inputFile = '../input/samples-htt-lephad.root'
sampleFolder = 'samples'

outputFileName = 'samplePathList.txt'
outputFile = open(outputFileName,'w')

samples = TQSampleFolder.loadLazySampleFolder(inputFile+':'+sampleFolder)

sampleList = samples.getListOfSamples()

for sample in sampleList:
    if not sample.hasSubSamples():
        samplePath = sample.getPath()
        outputFile.write(str(samplePath)+'\n')

outputFile.close()



    
