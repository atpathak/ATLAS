for rtag in 10808 10899 #10011 #9871 #10155 #9871 9875 9876 9877 
do
  #for m in 1 2 3 4 5
  #do	     
      for i in `seq 1 20`
      do
	  cd /data/apathak/VF_ITK_30Oct2018_r${rtag}/RunArea/Reconstr_${i}
         # cd /data/apathak/VF_ITK_30Oct2017_r${rtag}/RunArea_${m}/Reconstr_${i}
#    pwd
	  rm -rf Prepare_AllSecond.txt
	  grep ":Execute" log.RAWtoESD  | grep "INFO Time User" | sed -e 's/ INFO Time User   : Tot=/ /g' | sed -e 's/\[/ \[/g' | sed -e 's/:Execute/ /g' | sed -e 's/_//g' |\
	      awk '{print $2, $3, $4, $NF}' |  \
	      awk '{if ($3=="[ms]")  {printf "%-60s %12.6f %3d\n", $1,$2/1000,$4} else {if ($3=="[us]") {printf "%-60s %12.6f %3d\n", $1,$2/1000000,$4} \
              else {if ($3=="[min]") {printf "%-60s %12.6f %3d\n",$1,$2*60,$4} else {printf "%-60s %12.6f %3d\n",$1,$2,$4} }} }' > Prepare_AllSecond.txt
	  NUM=`head -1 Prepare_AllSecond.txt | awk '{print $NF}'`
	  #    echo $NUM
	  cat Prepare_AllSecond.txt | \
	      grep -e InDetCaloClusterROISelector \
		   -e InDetCopyAlg \
		   -e InDetTrackCollectionMerger \
		   -e InDetTrackParticles \
		   -e InDetVxLinkSetter \
		   -e InDetRecStatistics | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "ITkgenonly", s, NUM}' > my_tmp1
	  cat Prepare_AllSecond.txt | \
	      grep -e InDetSCTClusterization \
		   -e InDetPixelClusterization | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "SiPreProcessing", s, NUM}' > my_tmp2
	  cat Prepare_AllSecond.txt | \
	      grep -e InDetSCTClusterization \
		   -e InDetPixelClusterization \
		   -e InDetSiTrackerSpacePointFinder \
		   -e InDetSiSpTrackFinderSLHC \
		   -e InDetAmbiguitySolverSLHC \
		   -e InDetPriVxFinder \
		   -e InDetCaloClusterROISelector \
		   -e InDetCopyAlg \
		   -e InDetTrackCollectionMerger \
		   -e InDetTrackParticles \
		   -e InDetVxLinkSetter \
		   -e InDetRecStatistics \
		   -e MuonCombinedInDetCandidateAlg | awk -v NUM="$NUM" '{s+=$2}END{printf "%-60s %12.6f %3d\n", "AllAlgorithm", s, NUM}' > my_tmp3
	  cat Prepare_AllSecond.txt my_tmp1 my_tmp2 my_tmp3 > Prepare_AllSecond_Final.txt
	  cd -
      done
 # done
done
