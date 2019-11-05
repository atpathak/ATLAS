for rtag in 8905 #7725  
do
    #for m in 1 2 3 4 5
    #do
	for i in `seq 1 20`
	do
	   #cd /data/apathak/VF_ITK_6Oct2017_r${rtag}/RunArea/Reconstr_${i}
	    cd /data/apathak/VF_ITK_25Aug2018_r${rtag}/RunArea/Reconstr_${i}
	    #    pwd
	    rm -rf Prepare_AllSecond.txt
	    rm -rf Prepare_AllSecond_Final.txt
	    rm -rf my_tmp1 my_tmp2 my_tmp3 my_tmp4 my_tmp5 my_tmp6 my_tmp7
	    grep ":Execute" log.RAWtoESD  | grep "INFO Time User" | sed -e 's/ INFO Time User   : Tot=/ /g' | sed -e 's/\[/ \[/g' | sed -e 's/:Execute/ /g' | sed -e 's/_//g' |\
		awk '{print $2, $3, $4, $NF}' |  \
		awk '{if ($3=="[ms]")  {printf "%-60s %12.6f %3d\n", $1,$2/1000,$4} else {if ($3=="[us]") {printf "%-60s %12.6f %3d\n", $1,$2/1000000,$4} \
                else {if ($3=="[min]") {printf "%-60s %12.6f %3d\n",$1,$2*60,$4} else {printf "%-60s %12.6f %3d\n",$1,$2,$4} }} }' > Prepare_AllSecond.txt
	    NUM=`head -1 Prepare_AllSecond.txt | awk '{print $NF}'`
	    #    echo $NUM
	    cat Prepare_AllSecond.txt | \
		grep -e InDetSCTClusterization \
		     -e InDetPixelClusterization | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "SiPreProcessing", s, NUM}' > my_tmp1
	    cat Prepare_AllSecond.txt | \
		grep -e "InDetSiSpTrackFinder " \
		     -e InDetSiSpTrackFinderForwardTracks | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "Pattern", s, NUM}' > my_tmp2    
	    cat Prepare_AllSecond.txt | \
		grep -e "InDetAmbiguitySolver " \
		     -e InDetAmbiguitySolverForwardTracks | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "TotalAmbiguity", s, NUM}' > my_tmp3
	    cat Prepare_AllSecond.txt | \
		grep -e "InDetTRTRIOMaker " \
		     -e "InDetTRTExtension " \
		     -e "InDetExtensionProcessor " \
		     -e "InDetTRTTrackSegmentsFinder " \
		     -e "InDetTRTSeededTrackFinder " \
		     -e "InDetTRTSeededAmbiguitySolver " \
		     -e "InDetTRTStandaloneTrackFinder " | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "TRTBackTracking", s, NUM}' > my_tmp4
	    cat Prepare_AllSecond.txt | \
		grep -e "InDetCaloClusterROISelector " \
		     -e "InDetSegmentPRDAssociation " \
		     -e "InDetTRTonlyPRDAssociation " \
		     -e "InDetPRDAssociationForwardTracks " \
		     -e "InDetTrackCollectionMerger " \
		     -e "InDetTrackSlimmer " \
		     -e "InDetTrackParticles " \
		     -e "InDetForwardTrackParticles " \
		     -e "InDetVxLinkSetter " \
		     -e "InDetBCMZeroSuppression " \
		     -e "InDetCosmicsEventPhase " \
		     -e "InDetRecStatistics " | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "ITkgenonly", s, NUM}' > my_tmp5
	    cat Prepare_AllSecond.txt | \
		grep -e "InDetPRDAssociationPixelPrdAssociation " \
		     -e "InDetSiSpTrackFinderPixelPrdAssociation " \
		     -e "InDetAmbiguitySolverPixelPrdAssociation " \
		     -e "InDetTRTExtensionPhasePixelPrdAssociation " \
		     -e "InDetTrackCollectionMergerpix " \
		     -e "InDetPixelPrdAssociationTrackParticles " \
		     -e "InDetLowBetaTrkAlgorithm " | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "TotalLowBeta", s, NUM}' > my_tmp6
	    cat Prepare_AllSecond.txt | \
		grep -e "InDetSCTClusterization " \
		     -e "InDetPixelClusterization " \
		     -e "InDetSiTrackerSpacePointFinder " \
		     -e "InDetSiSpTrackFinder " \
		     -e "InDetSiSpTrackFinderForwardTracks " \
		     -e "InDetAmbiguitySolver " \
		     -e "InDetAmbiguitySolverForwardTracks " \
		     -e "InDetTRTRIOMaker " \
		     -e "InDetTRTExtension " \
		     -e "InDetExtensionProcessor " \
		     -e "InDetTRTTrackSegmentsFinder " \
		     -e "InDetTRTSeededTrackFinder " \
		     -e "InDetTRTSeededAmbiguitySolver " \
		     -e "InDetTRTStandaloneTrackFinder " \
		     -e "InDetPriVxFinder " \
		     -e "InDetCaloClusterROISelector " \
		     -e "InDetSegmentPRDAssociation " \
		     -e "InDetTRTonlyPRDAssociation " \
		     -e "InDetPRDAssociationForwardTracks " \
		     -e "InDetTrackCollectionMerger " \
		     -e "InDetTrackSlimmer " \
		     -e "InDetTrackParticles " \
		     -e "InDetForwardTrackParticles " \
		     -e "InDetVxLinkSetter " \
		     -e "InDetBCMZeroSuppression " \
		     -e "InDetCosmicsEventPhase " \
		     -e "InDetRecStatistics " \
		     -e "InDetPRDAssociationPixelPrdAssociation " \
		     -e "InDetSiSpTrackFinderPixelPrdAssociation " \
		     -e "InDetAmbiguitySolverPixelPrdAssociation " \
		     -e "InDetTRTExtensionPhasePixelPrdAssociation " \
		     -e "InDetTrackCollectionMergerpix " \
		     -e "InDetPixelPrdAssociationTrackParticles " \
		     -e "InDetLowBetaTrkAlgorithm " \
		     -e "MuonCombinedInDetCandidateAlg " | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "AllAlgorithm", s, NUM}' > my_tmp7
	    cat Prepare_AllSecond.txt my_tmp1 my_tmp2 my_tmp3 my_tmp4 my_tmp5 my_tmp6 my_tmp7 > Prepare_AllSecond_Final.txt
	    cd -
	done
    done
#done 
