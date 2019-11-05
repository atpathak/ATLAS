# Make working directory
mkdir -vp workdir
# asetup 
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase

source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh

asetup 20.20.10.7,AtlasProduction,here,slc6

# package checkout from svn if you need to do
 # in lxplus:

 pkgco.py InDetSLHC_Example-00-02-99

 pkgco.py InDetRecExample-02-06-99-45
 
 # in higgs

 svn co svn+ssh://atpathak@svn.cern.ch/reps/atlasoff/InnerDetector/InDetExample/InDetSLHC_Example/tags/InDetSLHC_Example-00-02-99 InnerDetector/InDetExample/InDetSLHC_Example

 echo InDetSLHC_Example-00-02-99 > InnerDetector/InDetExample/InDetSLHC_Example/cmt/version.cmt

 svn co svn+ssh://atpathak@svn.cern.ch/reps/atlasoff/InnerDetector/InDetExample/InDetRecExample/tags/InDetRecExample-02-06-99-45 InnerDetector/InDetExample/InDetRecExample

 echo InDetRecExample-02-06-99-45 > InnerDetector/InDetExample/InDetRecExample/cmt/version.cmt
 
 # if we have to copy a package given a tarball that is not in a release 
 
 cp -p -r /afs/cern.ch/user/a/atpathak/afswork/public/Pixel/PHOTONROI12/InnerDetector .

 cd InnerDetector/InDetRecTools/SiSpacePointsSeedTool_xk/cmt   ; rm -rf setup.sh setup.csh cleanup.sh cleanup.csh ; cmt config ; cd -

 cd InnerDetector/InDetDetDescr/SCT_Cabling/cmt                ; rm -rf setup.sh setup.csh cleanup.sh cleanup.csh ; cmt config ; cd -

 cd InnerDetector/InDetDetDescr/PixelCabling/cmt               ; rm -rf setup.sh setup.csh cleanup.sh cleanup.csh ; cmt config ; cd -

 cd InnerDetector/InDetDetDescr/InDetRegionSelector/cmt        ; rm -rf setup.sh setup.csh cleanup.sh cleanup.csh ; cmt config ; cd -

 cd InnerDetector/InDetRecAlgs/InDetCaloClusterROISelector/cmt ; rm -rf setup.sh setup.csh cleanup.sh cleanup.csh ; cmt config ; cd -

 cd InnerDetector/InDetRecAlgs/SiSPSeededTrackFinder/cmt       ; rm -rf setup.sh setup.csh cleanup.sh cleanup.csh ; cmt config ; cd -

 cd InnerDetector/InDetExample/InDetRecExample/cmt             ; rm -rf setup.sh setup.csh cleanup.sh cleanup.csh ; cmt config ; cd -

# Compile package 

rm -rf InstallArea WorkArea

setupWorkArea.py

cd WorkArea/cmt ; cmt config ; cmt bro gmake clean ; cmt bro gmake -j8 ; cmt bro gmake ; cd -

# Make RunArea 

mkdir -vp RunArea/Reconstr

cd RunArea/Reconstr

# Reco command to run over RDO's ( For example r9871) ( All my reco scripts are in  AllReco_Script directory )

cat > reco.todo <<EOF

Reco_tf.py \\

--inputRDOFile ${Your RDO input file will be here} \\

--outputESDFile SLHCtest.ESD.pool.root \\

--digiSeedOffset1 '50107' --digiSeedOffset2 '57122' \\

--maxEvents 50 \\

--skipEvents ${ if you want to skip some events } \\

--digiSteeringConf 'StandardInTimeOnlyTruth' \\

--conditionsTag 'default:OFLCOND-MC15c-SDR-14-03' \\

--numberOfHighPtMinBias '0.725172' \\

--preExec 'all:from AthenaCommon.GlobalFlags import globalflags; globalflags.DataSource.set_Value_and_Lock("geant4"); userRunLumiOverride={"run":242020, "startmu":190.0, "endmu":210.0, "stepmu":1.0, "startlb":1, "timestamp":1412020000};from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags;SLHC_Flags.doGMX.set_Value_and_Lock(True);SLHC_Flags.LayoutOption="InclinedDuals";' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags; digitizationFlags.doInDetNoise.set_Value_and_Lock(False); digitizationFlags.overrideMetadata+=["SimLayout","PhysicsList"];' 'RAWtoESD:rec.doTruth.set_Value_and_Lock(False);' 'ESDtoDPD:rec.DPDMakerScripts.set_Value_and_Lock(["InDetPrepRawDataToxAOD/InDetDxAOD.py","PrimaryDPDMaker/PrimaryDPDMaker.py"]);' \\

--numberOfLowPtMinBias '209.2692' \\

--preInclude 'all:InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py,InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py,InDetSLHC_Example/preInclude.SLHC_Calorimeter_mu200.py' 'HITtoRDO:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py,Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInclude.PileUpBunchTrains2012Config1_DigitConfig.py,RunDependentSimData/configLumi_muRange.py' 'default:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Reco.py,InDetSLHC_Example/SLHC_Setup_Reco_TrackingGeometry_GMX.py' 'RDOMergeAthenaMP:InDetSLHC_Example/preInclude.SLHC.py,InDetSLHC_Example/preInclude.NoTRT_NoBCM_NoDBM.py' 'POOLMergeAthenaMPAOD0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' 'POOLMergeAthenaMPDAODIDTRKVALID0:InDetSLHC_Example/preInclude.SLHC.NoTRT_NoBCM_NoDBM.Ana.py,InDetSLHC_Example/SLHC_Setup_Reco_Alpine.py' \\

--DataRunNumber '242020' \\

--postInclude 'all:InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py' 'HITtoRDO:InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py' 'RAWtoESD:InDetSLHC_Example/postInclude.DigitalClustering.py,InDetSLHC_Example/postInclude.NoJetPtFilter.py' \\

--postExec 'HITtoRDO:pixeldigi.EnableSpecialPixels=False; CfgMgr.MessageSvc().setError+=["HepMcParticleLink"];' 'RAWtoESD:ToolSvc.InDetSCT_ClusteringTool.useRowInformation=True;' \\

--geometryVersion 'default:ATLAS-P2-ITK-20-00-00'

EOF

   chmod +x reco.todo

cat > todo <<EOF

 cd { workdir }

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase

source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh

source $AtlasSetup/scripts/asetup.sh 20.20.10.9,AtlasProduction,here,slc6

cd { workdir }/RunArea/Reconstr

rm -f reco.log

time ./reco.todo 2>&1 > reco.log

EOF

chmod +x todo

# job submit in batch

 # in Lxplus

  rm -f todo.log ; bsub_lxplus 2nd todo.log 0 ./todo

 # in Higgs

 qsub -l nodes=1:ppn=1,walltime=5:00:00 -q batch -eo -o todo.log ./todo
 
 # In this example the job will use 1 node (we only have one), 1 cores, and a walltime of 5 hours
