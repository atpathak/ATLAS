#!/bin/sh
mkdir -vp log/signf_exp
#for channel in ll_mtau_cba ll_etau_cba ll_mtau_mva ll_etau_mva \
    #               lh_mtau_cba lh_etau_cba lh_mtau_mva lh_etau_mva
#for channel in ll_etau_mva
#do
for channel in ll_etau_cba lh_etau_cba ll_mtau_cba lh_mtau_cba 
do
if [ ${channel} == ll_mtau_cba ] || [ ${channel} == ll_etau_cba ]; then
    declare -a arr=("config_inc" "config_vbf")

 elif [ ${channel} == lh_mtau_cba ] || [ ${channel} == lh_etau_cba ]; then
    declare -a arr=("config_inc1" "config_inc2" "config_inc3" "config_vbf")
fi
for StatAnalysis in "${arr[@]}"
do
cat > todo_${channel}_${StatAnalysis}_signf_exp <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
python scripts/getSig.py data_new/${channel}/v20180326/${StatAnalysis} 1 125 2>&1 | tee log/signf_exp/${channel}_${StatAnalysis}_signf_exp.log
EOF
chmod +x todo_${channel}_${StatAnalysis}_signf_exp
logfile=log/signf_exp/${channel}_${StatAnalysis}.log
rm -f $logfile ; bsub_lxplus 2nw $logfile 0 ./todo_${channel}_${StatAnalysis}_signf_exp
done
done
