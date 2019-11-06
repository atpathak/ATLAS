#!/bin/sh
mkdir -vp log/muhat_new
#for channel in ll_mtau_cba ll_etau_cba ll_mtau_mva ll_etau_mva \
 #              lh_mtau_cba lh_etau_cba lh_mtau_mva lh_etau_mva \
  #             comb_mtau_cba comb_etau_cba comb_etau_mva comb_mtau_mva
for channel in comb_etau_cba ll_etau_cba lh_etau_cba comb_mtau_cba ll_mtau_cba lh_mtau_cba \
	       comb_etau_mva ll_etau_mva llinc_etau_mva llvbf_etau_mva lh_etau_mva lhinc_etau_mva \
	       lhvbf_etau_mva  comb_mtau_mva ll_mtau_mva llinc_mtau_mva llvbf_mtau_mva lh_mtau_mva lhinc_mtau_mva lhvbf_mtau_mva
do
cat > todo_${channel}_muhat <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
rm -rf log/muhat_new/${channel}_muhat.log
python scripts/runMuHat.py data_new/${channel}/v20180326 2>&1 | tee log/muhat_new/${channel}_muhat.log
EOF
chmod +x todo_${channel}_muhat
logfile=log/muhat_new/${channel}.log
rm -f $logfile ; bsub_lxplus 2nw $logfile 0 ./todo_${channel}_muhat
done
