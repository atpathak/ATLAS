#!/bin/bash
shopt -s expand_aliases
cd /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_20Feb2017_1
#export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
#export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 21.0.14,here
source /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_20Feb2017_1/build/x86_64-slc6-gcc49-opt/setup.sh
cd /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_20Feb2017_1/RunArea/Simulate/ID/Geantino_all
time ./sim.todo 2>&1 > sim.log
