#!/bin/sh
mkdir -vp log/muhat_new
for channel in ll_etau_cba lh_etau_cba ll_mtau_cba lh_mtau_cba 
do
if [ ${channel} == ll_mtau_cba ] || [ ${channel} == ll_etau_cba ]; then
    declare -a arr=("config_inc" "config_vbf")

 elif [ ${channel} == lh_mtau_cba ] || [ ${channel} == lh_etau_cba ]; then
    declare -a arr=("config_inc1" "config_inc2" "config_inc3" "config_vbf")
fi
for StatAnalysis in "${arr[@]}"
do
cat > todo_${channel}_${StatAnalysis}_muhat <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
rm -rf log/muhat_new/${channel}_${StatAnalysis}_muhat.log
python scripts/runMuHat.py data_new/${channel}/v20180326/${StatAnalysis} 2>&1 | tee log/muhat_new/${channel}_${StatAnalysis}_muhat.log
EOF
chmod +x todo_${channel}_${StatAnalysis}_muhat
logfile=log/muhat_new/${channel}_${StatAnalysis}_exp.log
rm -f $logfile ; bsub_lxplus 2nw $logfile 0 ./todo_${channel}_${StatAnalysis}_muhat
done
done
