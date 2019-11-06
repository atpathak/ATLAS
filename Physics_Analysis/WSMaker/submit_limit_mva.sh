#!/bin/sh
mkdir -vp log/limit
for channel in mue_mva_all emu_mva_all
do
cat > todo_${channel} <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
rm -rf log/limit/${channel}_limit.log
python scripts/getLimit.py ${channel}/LFV13TeVCombinationMVA 0 125 2>&1 | tee log/limit/${channel}_limit.log
EOF
chmod +x todo_${channel}
logfile=log/limit/${channel}.log
rm -f $logfile ; bsub_lxplus 2nw $logfile 0 ./todo_${channel}
done
