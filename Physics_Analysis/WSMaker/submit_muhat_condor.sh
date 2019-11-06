#!/bin/sh
for channel in lh_etau_mva lhvbf_etau_mva lhinc_etau_mva
do
cat > todo_${channel}_muhat <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
rm -rf ${channel}_muhat.log
python scripts/runMuHat.py workspaces_data/${channel}/v20180326 2>&1 | tee ${channel}_muhat_new.log
EOF
chmod +x todo_${channel}_muhat
logfile=${channel}.log
rm -f $logfile ; bsub_condor 1 $logfile ./todo_${channel}_muhat ${channel}
done
