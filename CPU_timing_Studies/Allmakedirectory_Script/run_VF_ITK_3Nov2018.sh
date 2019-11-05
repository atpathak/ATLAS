#!/bin/sh
THIS_RTAG=-1
THIS_N=-1
for rtag in 60 #140
do
if [ $rtag = "140" ] ; then
    ami="r10899"
    AtProd="12"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root,SLHCtest_3.RDO.pool.root,SLHCtest_4.RDO.pool.root,SLHCtest_5.RDO.pool.root,SLHCtest_6.RDO.pool.root,SLHCtest_7.RDO.pool.root,SLHCtest_8.RDO.pool.root,SLHCtest_9.RDO.pool.root,SLHCtest_10.RDO.pool.root"
elif [ $rtag = "60" ] ; then
    ami="r10808"
    AtProd="12"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root,SLHCtest_3.RDO.pool.root,SLHCtest_4.RDO.pool.root"
fi
for n in `seq 1 20`
do
if [ ${THIS_RTAG} = "-1" -o ${THIS_RTAG} = ${rtag} ] ; then
if [ ${THIS_N} = "-1" -o ${THIS_N} = "${n}" ] ; then
 skipevt=`echo $n | awk '{print 50*($1-1)}'`
 sleep=`echo $n | awk '{print 30*($1-1)}'`
 echo $rtag $n $skipevt
 mkdir -vp VF_ITK_3Nov2018_r10899_mu${rtag}/RunArea/Reconstr_${n}
 cd        VF_ITK_3Nov2018_r10899_mu${rtag}/RunArea/Reconstr_${n}
 if [ $rtag = "140" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-04' \\
--numberOfHighPtMinBias '0.51798' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); userRunLumiOverride={"run":241714, "startmu":130.0, "endmu":150.0, "stepmu":1.0, "startlb":1, "timestamp":1411714000};from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedAlternative";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--numberOfLowPtMinBias '149.478' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu140.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2012Config1_DigitConfig.py,RunDependentSimData/configLumi_muRange.py' 'default:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '241714' \\
--postInclude 'all:PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/postInclude.SLHC_Setup.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.AnalogueClustering.py,InDetSLHC_Example/postInclude.NoJetPtFilter.py' \\
--postExec 'all:ToolSvc.LayoutTranslationHelper.translateIdentifiersForInclinedAlternative=True;' 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;ServiceMgr.PixelOfflineCalibSvc.ITkAnalogueClusteringConstantsFile="/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.20.12/AtlasProduction/20.20.12.1/InnerDetector/InDetConditions/PixelConditionsServices/share/step3_constants50by50.txt";' \\
--geometryVersion 'default:ATLAS-P2-ITK-17-00-01'
EOF
elif [ $rtag = "60" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-04' \\
--numberOfHighPtMinBias '0.241724' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); userRunLumiOverride={"run":241706, "startmu":50.0, "endmu":70.0, "stepmu":1.0, "startlb":1, "timestamp":1411706000};from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedAlternative";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--numberOfLowPtMinBias '69.7564' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu60.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2012Config1_DigitConfig.py,RunDependentSimData/configLumi_muRange.py' 'default:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '241706' \\
--postInclude 'all:PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/postInclude.SLHC_Setup.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.AnalogueClustering.py,InDetSLHC_Example/postInclude.NoJetPtFilter.py' \\
--postExec 'all:ToolSvc.LayoutTranslationHelper.translateIdentifiersForInclinedAlternative=True;' 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;ServiceMgr.PixelOfflineCalibSvc.ITkAnalogueClusteringConstantsFile="/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.20.12/AtlasProduction/20.20.12.1/InnerDetector/InDetConditions/PixelConditionsServices/share/step3_constants50by50.txt";' \\
--geometryVersion 'default:ATLAS-P2-ITK-17-00-01' \\
EOF
fi
chmod +x reco.todo
if [ $rtag = "140" ] ; then
    /bin/ls -l /data/apathak/INPUT_r10899_mu140/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
elif [ $rtag = "60" ] ; then
    /bin/ls -l /data/apathak/INPUT_r10899_mu60/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
fi
cat > todo <<EOF
#!/bin/bash
shopt -s expand_aliases
sleep ${sleep}
cd /data/apathak/VF_ITK_3Nov2018_r10899_mu${rtag}
# directly go to directory to launch jobs so that multiple instances of asetup can be run simultaneously without interfereing with each other
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
asetup 20.20.12.3,AtlasProduction,here,slc6
cd /data/apathak/VF_ITK_3Nov2018_r10899_mu${rtag}/RunArea/Reconstr_${n}
rm -f reco.log
time ./reco.todo 2>&1 > reco.log
EOF
chmod +x todo
rm -f todo.log
cat > submit.sh <<EOF
qsub -l nodes=1:ppn=1,walltime=7:00:00 -q batch -eo -o todo.log ./todo
EOF
chmod +x submit.sh
source submit.sh
cd -
fi
fi
done
done
