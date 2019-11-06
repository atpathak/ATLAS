#!/bin/sh
mkdir -vp log/fcc
#for channel in mu_combined_sr mu_combined_sr_vbf e_combined_sr e_combined_sr_vbf \
#	       mu_combined_sr_mva mu_combined_sr_vbf_mva e_combined_sr_mva e_combined_sr_vbf_mva
for channel in mu_combined_sr_vbf e_combined_sr_vbf mu_combined_sr_vbf_mva e_combined_sr_vbf_mva
do
cat > todo_${channel}_fcc <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
python scripts/runFitCrossCheck.py ${channel}/LFV13TeVCombinationCBA 2,5 125
python scripts/makeReducedDiagPlots.py -p 2,5 fccs/FitCrossChecks_${channel}/LFV13TeVCombinationCBA_combined
EOF
chmod +x todo_${channel}_fcc
logfile=log/fcc/${channel}.log
rm -f $logfile ; bsub_lxplus 1nw $logfile 0 ./todo_${channel}_fcc
done
