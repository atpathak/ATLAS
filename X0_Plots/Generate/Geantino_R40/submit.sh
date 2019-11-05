#!/bin/bash
shopt -s expand_aliases
cd /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_5Dec2016_1
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 20.20.6.2,AtlasProduction,here
cd /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_5Dec2016_1/RunArea/Generate/Geantino_R40
time ./gen.todo 2>&1 > gen.log
