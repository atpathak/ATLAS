#!/bin/sh
THIS_RTAG=-1
THIS_N=-1
for rtag in 10845
do
if [ $rtag = "10901" ] ; then
    ami="r10899"
    AtProd="12"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root,SLHCtest_3.RDO.pool.root,SLHCtest_4.RDO.pool.root,SLHCtest_5.RDO.pool.root,SLHCtest_6.RDO.pool.root,SLHCtest_7.RDO.pool.root,SLHCtest_8.RDO.pool.root,SLHCtest_9.RDO.pool.root,SLHCtest_10.RDO.pool.root,SLHCtest_11.RDO.pool.root,SLHCtest_12.RDO.pool.root,SLHCtest_13.RDO.pool.root,SLHCtest_14.RDO.pool.root,SLHCtest_15.RDO.pool.root,SLHCtest_16.RDO.pool.root,SLHCtest_17.RDO.pool.root,SLHCtest_18.RDO.pool.root,SLHCtest_19.RDO.pool.root,SLHCtest_20.RDO.pool.root"
elif [ $rtag = "10845" ] ; then
    ami="r10845"
    AtProd="12"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root"
fi
for n in `seq 1 20`
do
if [ ${THIS_RTAG} = "-1" -o ${THIS_RTAG} = ${rtag} ] ; then
if [ ${THIS_N} = "-1" -o ${THIS_N} = "${n}" ] ; then
 skipevt=`echo $n | awk '{print 50*($1-1)}'`
 sleep=`echo $n | awk '{print 30*($1-1)}'`
 echo $rtag $n $skipevt
 mkdir -vp VF_ITK_18Nov2018_r${rtag}/RunArea/Reconstr_${n}
 cd        VF_ITK_18Nov2018_r${rtag}/RunArea/Reconstr_${n}
 if [ $rtag = "10901" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-04' \\
--numberOfHighPtMinBias '0.725172' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); userRunLumiOverride={"run":241720, "startmu":190.0, "endmu":210.0, "stepmu":1.0, "startlb":1, "timestamp":1411720000};from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedAlternative";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--numberOfLowPtMinBias '209.2692' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4_25x100.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu200.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2012Config1_DigitConfig.py,RunDependentSimData/configLumi_muRange.py' 'default:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '241720' \\
--postInclude 'all:PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4_25x100.py,InDetSLHC_Example/postInclude.SLHC_Setup.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.AnalogueClustering.py,InDetSLHC_Example/postInclude.NoJetPtFilter.py' \\
--postExec 'all:ToolSvc.LayoutTranslationHelper.translateIdentifiersForInclinedAlternative=True;' 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;ServiceMgr.PixelOfflineCalibSvc.ITkAnalogueClusteringConstantsFile="/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.20.12/AtlasProduction/20.20.12.1/InnerDetector/InDetConditions/PixelConditionsServices/share/step3_constants25by100.txt";' \\
--geometryVersion 'default:ATLAS-P2-ITK-17-00-01'
EOF
elif [ $rtag = "10845" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-04' \\
--postInclude 'all:PyJobTransforms/UseFrontier.py,InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4_25x100.py,InDetSLHC_Example/postInclude.SLHC_Setup.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.AnalogueClustering.py,InDetSLHC_Example/postInclude.NoJetPtFilter.py' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4");from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedAlternative";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4_25x100.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py' 'default:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '241700' \\
--postExec 'all:ToolSvc.LayoutTranslationHelper.translateIdentifiersForInclinedAlternative=True;' 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;ServiceMgr.PixelOfflineCalibSvc.ITkAnalogueClusteringConstantsFile="/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.20.12/AtlasProduction/20.20.12.1/InnerDetector/InDetConditions/PixelConditionsServices/share/step3_constants25by100.txt";' \\
--geometryVersion 'default:ATLAS-P2-ITK-17-00-01'
EOF
fi
chmod +x reco.todo
if [ $rtag = "10901" ] ; then
    /bin/ls -l /data/apathak/INPUT_r10901/mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s3348_s3347_r10901/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
elif [ $rtag = "10845" ] ; then
    /bin/ls -l /data/apathak/INPUT_r10845/mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s3348_s3347_r10845/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
fi
cat > todo <<EOF
#!/bin/bash
shopt -s expand_aliases
sleep ${sleep}
cd /data/apathak/VF_ITK_18Nov2018_r${rtag}
# directly go to directory to launch jobs so that multiple instances of asetup can be run simultaneously without interfereing with each other
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
asetup 20.20.12.3,AtlasProduction,here,slc6
cd /data/apathak/VF_ITK_18Nov2018_r${rtag}/RunArea/Reconstr_${n}
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
