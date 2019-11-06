#!/bin/sh
mkdir -vp log/signf
for channel in mue_mva_all emu_mva_all
do
cat > todo_${channel}_signf <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
python scripts/getSig.py ${channel}/LFV13TeVCombinationMVA 0 125 2>&1 | tee log/signf/${channel}_signf.log
EOF
chmod +x todo_${channel}_signf
logfile=log/signf/${channel}.log
rm -f $logfile ; bsub_lxplus 2nw $logfile 0 ./todo_${channel}_signf
done
