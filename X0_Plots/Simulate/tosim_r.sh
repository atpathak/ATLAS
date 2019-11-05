#!/bin/sh
THIS_GEOM=$1
THIS_R=$2
for geom in ExtBrl4 InclBrl4 IExtBrl4
do
if [ $geom = "ExtBrl4" ] ; then
  geom_version="09"
  SLHC_Setup="ExtBrl_4"
elif [ $geom = "InclBrl4" ] ; then
  geom_version="10"
  SLHC_Setup="InclBrl_4"
elif [ $geom = "IExtBrl4" ] ; then
  geom_version="12"
  SLHC_Setup="IExtBrl_4"
fi
for r in 38 39 40 74 75 76 154 155 156 212 213 214 270 271 272
do
if [ ${THIS_GEOM} = "-1" -o ${THIS_GEOM} = ${geom} ] ; then
if [ ${THIS_R} = "-1" -o ${THIS_R} = "${r}" ] ; then
 echo $geom $r
 rm -rf    Simulate/${geom}/Geantino_R${r}
 mkdir -vp Simulate/${geom}/Geantino_R${r}
 cd        Simulate/${geom}/Geantino_R${r}
 cat > sim.todo <<EOF
Sim_tf.py \\
--inputEVNTFile /afs/cern.ch/user/s/swaban/afswork/public/Pixel/VF_ITK_26Oct2016_1/RunArea/Generate/Geantino_R${r}/pgun_10GeV_geantinos_EVNT.root \\
--outputHITSFile OUT.HITS_ExtBrl4.pool.root \\
--skipEvents 0 \\
--maxEvents 10000 \\
--geometryVersion 'default:ATLAS-P2-ITK-${geom_version}-00-01_VALIDATION' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-13' \\
--DataRunNumber '240000' \\
--randomSeed 873254 \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,InDetSLHC_Example/preInclude.SLHC_Setup_${SLHC_Setup}.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,G4UserActions/LengthIntegrator_options.py' \\
--postExec 'EVNTtoHITS:ServiceMgr.DetDescrCnvSvc.DoInitNeighbours = False' \\
           'EVNTtoHITS:from AthenaCommon import CfgGetter;CfgGetter.getPublicTool("ISF_StackFiller").GenParticleFilters=[]' \\
--postInclude 'all:PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_${SLHC_Setup}.py,InDetSLHC_Example/postInclude.SLHC_Setup.py' \\
--preExec 'all:from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True)'
EOF
chmod +x sim.todo
/bin/ls
cat sim.todo
echo "time ./sim.todo 2>&1 > sim.log"
      time ./sim.todo 2>&1 > sim.log
cd -
fi
fi
done
done
