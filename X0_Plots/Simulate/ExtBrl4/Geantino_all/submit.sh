#!/bin/bash
shopt -s expand_aliases
cd /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_7Feb2017_1
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 20.20.8.1,AtlasProduction,here
cd /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_7Feb2017_1/RunArea/Simulate/ExtBrl4/Geantino_all
time ./sim.todo 2>&1 > sim.log 
