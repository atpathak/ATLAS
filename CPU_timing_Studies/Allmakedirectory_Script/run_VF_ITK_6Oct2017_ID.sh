#!/bin/sh
#
#THIS_RTAG=$1
#THIS_N=$2
THIS_RTAG=-1
THIS_N=-1
#for rtag in 9060 9133 9139
#for rtag in 7725 #9871 9875 9876 9877
for rtag in 7725
do
if [ $rtag = "9871" ] ; then
    ami="r9871"
    AtProd="9"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root,SLHCtest_3.RDO.pool.root,SLHCtest_4.RDO.pool.root,SLHCtest_5.RDO.pool.root,SLHCtest_6.RDO.pool.root,SLHCtest_7.RDO.pool.root,SLHCtest_8.RDO.pool.root,SLHCtest_9.RDO.pool.root,SLHCtest_10.RDO.pool.root,SLHCtest_11.RDO.pool.root,SLHCtest_12.RDO.pool.root,SLHCtest_13.RDO.pool.root,SLHCtest_14.RDO.pool.root,SLHCtest_15.RDO.pool.root,SLHCtest_16.RDO.pool.root,SLHCtest_17.RDO.pool.root,SLHCtest_18.RDO.pool.root,SLHCtest_19.RDO.pool.root,SLHCtest_20.RDO.pool.root"
elif [ $rtag = "9875" ] ; then
    ami="r9875"
    AtProd="9"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root,SLHCtest_3.RDO.pool.root,SLHCtest_4.RDO.pool.root,SLHCtest_5.RDO.pool.root,SLHCtest_6.RDO.pool.root,SLHCtest_7.RDO.pool.root,SLHCtest_8.RDO.pool.root,SLHCtest_9.RDO.pool.root,SLHCtest_10.RDO.pool.root"
elif [ $rtag = "9876" ] ; then
    ami="r9876"
    AtProd="9"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root,SLHCtest_3.RDO.pool.root,SLHCtest_4.RDO.pool.root,SLHCtest_5.RDO.pool.root,SLHCtest_6.RDO.pool.root,SLHCtest_7.RDO.pool.root,SLHCtest_8.RDO.pool.root,SLHCtest_9.RDO.pool.root,SLHCtest_10.RDO.pool.root"
elif [ $rtag = "9877" ] ; then
    ami="r9877"
    AtProd="9"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root,SLHCtest_3.RDO.pool.root,SLHCtest_4.RDO.pool.root"
elif [ $rtag = "7725" ] ; then
    ami="r7725"
    AtProd="1"
    input="SLHCtest_1.RDO.pool.root,SLHCtest_2.RDO.pool.root"
fi
#for n in 1 2 3 4 5 6 7 8 9 10
#for m in 1 2 5 10 20 50 100
#do
#for m in 1 2 3 4 5
#do
for n in `seq 1 20`
do
if [ ${THIS_RTAG} = "-1" -o ${THIS_RTAG} = ${rtag} ] ; then
if [ ${THIS_N} = "-1" -o ${THIS_N} = "${n}" ] ; then
 skipevt=`echo $n | awk '{print 50*($1-1)}'`
 sleep=`echo $n | awk '{print 30*($1-1)}'`
 echo $rtag $n $skipevt
 rm -rf    VF_ITK_6Oct2017_r${rtag}_ID/RunArea/Reconstr_${n}
 mkdir -vp VF_ITK_6Oct2017_r${rtag}_ID/RunArea/Reconstr_${n}
 cd        VF_ITK_6Oct2017_r${rtag}_ID/RunArea/Reconstr_${n}
 if [ $rtag = "9871" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-03' \\
--numberOfHighPtMinBias '0.725172' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); userRunLumiOverride={"run":242020, "startmu":190.0, "endmu":210.0, "stepmu":1.0, "startlb":1, "timestamp":1412020000};from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedDuals";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--numberOfLowPtMinBias '209.2692' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu200.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2012Config1_DigitConfig.py,RunDependentSimData/configLumi_muRange.py' 'default:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '242020' \\
--postInclude 'all:InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.DigitalClustering.py' \\
--postExec 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;' \\
--geometryVersion 'default:ATLAS-P2-ITK-20-00-00'
EOF
   chmod +x reco.todo
 elif [ $rtag = "9875" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-03' \\
--numberOfHighPtMinBias '0.51798' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); userRunLumiOverride={"run":242014, "startmu":130.0, "endmu":150.0, "stepmu":1.0, "startlb":1, "timestamp":1412014000};from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedDuals";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--numberOfLowPtMinBias '149.478' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu140.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2012Config1_DigitConfig.py,RunDependentSimData/configLumi_muRange.py' 'default:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '242014' \\
--postInclude 'all:InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.DigitalClustering.py' \\
--postExec 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;' \\
--geometryVersion 'default:ATLAS-P2-ITK-20-00-00'
EOF
  chmod +x reco.todo
 elif [ $rtag = "9876" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-03' \\
--numberOfHighPtMinBias '0.241724' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); userRunLumiOverride={"run":242006, "startmu":50.0, "endmu":70.0, "stepmu":1.0, "startlb":1, "timestamp":1412006000};from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedDuals";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--numberOfLowPtMinBias '69.7564' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu200.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2012Config1_DigitConfig.py,RunDependentSimData/configLumi_muRange.py' 'default:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '242006' \\
--postInclude 'all:InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.DigitalClustering.py' \\
--postExec 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;' \\
--geometryVersion 'default:ATLAS-P2-ITK-20-00-00'
EOF
  chmod +x reco.todo  
 elif [ $rtag = "9877" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--digiSteeringConf 'StandardInTimeOnlyTruth' \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-14-03' \\
--postInclude 'all:InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.DigitalClustering.py' \\
--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4");from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedDuals";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\
--preInclude 'all:InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu0.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py' 'default:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.SiliconOnly.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.SiliconOnly.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\
--DataRunNumber '242000' \\
--postExec 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;' \\
--geometryVersion 'default:ATLAS-P2-ITK-20-00-00'
EOF
  chmod +x reco.todo
 elif [ $rtag = "7725" ] ; then
   cat > reco.todo <<EOF
Reco_tf.py \\
--inputRDOFile ${input} \\
--outputESDFile SLHCtest.ESD.pool.root \\
--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\
--maxEvents 50 \\
--skipEvents ${skipevt} \\
--conditionsTag 'default:OFLCOND-MC15c-SDR-09' \\
--pileupFinalBunch '6' \\
--numberOfHighPtMinBias '0.12268057' \\
--autoConfiguration 'everything' \\
--numberOfLowPtMinBias '39.8773194' \\
--preInclude switch_off_non_id.py \\
--postInclude 'default:RecJobTransforms/UseFrontier.py' \\
--preExec 'all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)' 'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False' 'rec.doTruth.set_Value_and_Lock(False);' \\
--geometryVersion 'default:ATLAS-R2-2015-03-01-00' \\
--numberOfCavernBkg '0'
EOF
  chmod +x reco.todo
 fi
 if [ $rtag = "9871" ] ; then
  /bin/ls -l /data/apathak/INPUT_r9871/mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s3185_s3186_r9871/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
 elif [ $rtag = "9875" ] ; then
  /bin/ls -l /data/apathak/INPUT_r9875/mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s3185_s3186_r9875/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
 elif [ $rtag = "9876" ] ; then
  /bin/ls -l /data/apathak/INPUT_r9876/mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s3185_s3186_r9876/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
 elif [ $rtag = "9877" ] ; then
  /bin/ls -l /data/apathak/INPUT_r9877/mc15_14TeV.117050.PowhegPythia_P2011C_ttbar.recon.RDO.e2176_s3185_s3186_r9877/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
 elif [ $rtag = "7725" ] ; then
  /bin/ls -l /data/apathak/INPUT_r7725/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7725/*root* | awk '{print "ln -s "$NF" SLHCtest_"NR".RDO.pool.root"}' | sh
 fi
if [ $rtag = "9877" ] ; then
cat > todo <<EOF
#!/bin/bash
shopt -s expand_aliases
sleep ${sleep}
cd /data/apathak/VF_ITK_6Oct2017_r${rtag}_ID
# directly go to directory to launch jobs so that multiple instances of asetup can be run simultaneously without interfereing with each other
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
source \$AtlasSetup/scripts/asetup.sh 20.20.10.${AtProd},AtlasProduction,here
cd /data/apathak/VF_ITK_6Oct2017_r${rtag}_ID/RunArea/Reconstr_${n}
rm -f reco.log
time ./reco.todo 2>&1 > reco.log
EOF
elif [ $rtag = "7725" ] ; then
cat > switch_off_non_id.py <<EOF
# print rec
rec.doCalo = False
rec.doEgamma = False
rec.doForwardDet = False
rec.doInDet = True
rec.doJetMissingETTag = False
rec.doLArg = False
rec.doLucid = False
rec.doMuon = False
rec.doMuonCombined = False
# rec.doRecoTiming = True
rec.doSemiDetailedPerfMon = True
rec.doTau = False
rec.doTile = False
rec.doTrigger = False
EOF
cat > todo <<EOF
#!/bin/bash
shopt -s expand_aliases
# directly go to directory to launch jobs so that multiple instances of asetup can be run simultaneously without interfereing with each other
cd /data/apathak/VF_ITK_6Oct2017_r${rtag}_ID/RunArea/Reconstr_${n}
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
source \$AtlasSetup/scripts/asetup.sh 20.7.5.${AtProd},AtlasProduction,here
rm -f reco.log
time ./reco.todo 2>&1 > reco.log
EOF
fi
chmod +x todo
rm -f todo.log 
#     ./todo < /dev/null > todo.log 2>&1 
#nohup ./todo < /dev/null > todo.log 2>&1 &
cd -
fi
fi
done
done
#done
