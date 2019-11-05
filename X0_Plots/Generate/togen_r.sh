#!/bin/sh
THIS_R=$1
for r in 38 39 40 74 75 76 154 155 156 212 213 214 270 271 272
do
## double rmin=0; double bw=500./1000; for (int i=1;i<=1001;++i) {double r=rmin+(i-1)*bw;cout << " i = " << i << " r = " << r << endl;}
rmin=`echo $r | awk '{print $1-0.5}'`
rmax=`echo $r | awk '{print $1+0.5}'`
if [ ${THIS_R} = "-1" -o ${THIS_R} = "${r}" ] ; then
echo ${r}
rm -rf    Generate/Geantino_R${r}
mkdir -vp Generate/Geantino_R${r}
cd        Generate/Geantino_R${r}
cat > MC15.240000.ParticleGun_single_geantinos_E10.py <<EOF
evgenConfig.description = "Single geantino with flat eta-phi and E = 10 GeV"
evgenConfig.keywords = ["singleParticle"]

include("MC15JobOptions/ParticleGun_Common.py")

import ParticleGun as PG
genSeq.ParticleGun.sampler.pid = 999
genSeq.ParticleGun.sampler.mom = PG.EThetaMPhiSampler(energy=10000, theta=0)
genSeq.ParticleGun.sampler.pos = PG.PosSampler(x=[$rmin,$rmax],y=0,z=0,t=0.0)
EOF
cat > gen.todo <<EOF
Generate_tf.py \\
--jobConfig=MC15.240000.ParticleGun_single_geantinos_E10.py \\
--ecmEnergy=14000 \\
--outputEVNTFile=pgun_10GeV_geantinos_EVNT.root \\
--maxEvents=10000 \\
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
