#!/bin/sh
shopt -s expand_aliases
START_DIR=$PWD
##
cd /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/VF_ITK_26Oct2016_1
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh 20.20.6.2,AtlasProduction,here
##
cd ${START_DIR}
##
THIS_Z=$1
for z in 0 499 1004 1419 2023 2536 2950
do
## double zmin=-3512.; double bw=2*3512./1000; for (int i=1;i<=1001;++i) {double z=zmin+(i-1)*bw;cout << " i = " << i << " z = " << z << endl;}
if [ $z = "0" ] ; then
## i = 500 z = -7.024
## i = 501 z = 0
## i = 502 z = 7.024
  zmin=-7.024
  zmax=7.024
elif [ $z = "499" ] ; then
## i = 571 z = 491.68
## i = 572 z = 498.704
## i = 573 z = 505.728
  zmin=491.68
  zmax=505.728
elif [ $z = "1004" ] ; then
### 986
### 1027
## i = 642 z = 990.384
## i = 643 z = 997.408
## i = 644 z = 1004.43
## i = 645 z = 1011.46
## i = 646 z = 1018.48
## i = 647 z = 1025.5
  zmin=997.408
  zmax=1011.46
elif [ $z = "1419" ] ; then
### 1394
### 1448
## i = 702 z = 1411.82
## i = 703 z = 1418.85
## i = 704 z = 1425.87
  zmin=1411.82
  zmax=1425.87
elif [ $z = "2023" ] ; then
### 1997
### 2075
## i = 788 z = 2015.89
## i = 789 z = 2022.91
## i = 790 z = 2029.94
  zmin=2015.89
  zmax=2029.94
elif [ $z = "2536" ] ; then
### 2503
### 2589
## i = 861 z = 2528.64
## i = 862 z = 2535.66
## i = 863 z = 2542.69
  zmin=2528.64
  zmax=2542.69
elif [ $z = "2950" ] ; then
### 2867
### 3000
## i = 920 z = 2943.06
## i = 921 z = 2950.08
## i = 922 z = 2957.1
  zmin=2943.06
  zmax=2957.1
fi
if [ ${THIS_Z} = "-1" -o ${THIS_Z} = "${z}" ] ; then
echo ${z}
cd        Generate  
mkdir -vp Geantino_Z${z}
cd        Geantino_Z${z}
cat > MC15.240000.ParticleGun_single_geantinos_E10.py <<EOF
evgenConfig.description = "Single geantino with flat eta-phi and E = 10 GeV"
evgenConfig.keywords = ["singleParticle"]

include("MC15JobOptions/ParticleGun_Common.py")

import ParticleGun as PG
genSeq.ParticleGun.sampler.pid = 999
genSeq.ParticleGun.sampler.mom = PG.EEtaMPhiSampler(energy=10000, eta=0)
genSeq.ParticleGun.sampler.pos = PG.PosSampler(x=0.0,y=0.0,z=[$zmin,$zmax],t=0.0)
EOF
cat > gen.todo <<EOF
Generate_tf.py \\
--jobConfig=MC15.240000.ParticleGun_single_geantinos_E10.py \\
--ecmEnergy=14000 \\
--outputEVNTFile=pgun_10GeV_geantinos_EVNT.root \\
--maxEvents=50000 \\
--randomSeed=1234 \\
--runNumber=240000 \\
--firstEvent=1
EOF
chmod +x gen.todo
/bin/ls
cat MC15.240000.ParticleGun_single_geantinos_E10.py
cat gen.todo
echo "time ./gen.todo 2>&1 > gen.log"
      time ./gen.todo 2>&1 > gen.log
cd -
fi
done
