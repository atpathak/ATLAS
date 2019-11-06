#!/bin/sh
mkdir -vp log/limit_new
#for channel in mtau_sr1 mtau_sr2 mtau_sr3 mtau_vbf etau_sr1 etau_sr2 etau_sr3 etau_vbf \
#	       mtau_comb_sr mtau_comb_sr_vbf etau_comb_sr etau_comb_sr_vbf mue_all emu_all \
#	       mu_combined_sr mu_combined_sr_vbf e_combined_sr e_combined_sr_vbf \
#	       mu_combined_sr_mva mu_combined_sr_vbf_mva e_combined_sr_mva e_combined_sr_vbf_mva
#for channel in mtau_sr1 mtau_sr2 mtau_sr3 etau_sr1 etau_sr2 etau_sr3 \
#	       mtau_sr1_vbf mtau_sr2_vbf mtau_sr3_vbf mtau_vbf etau_sr1_vbf etau_sr2_vbf etau_sr3_vbf etau_vbf \
 #              mtau_comb_sr etau_comb_sr mtau_comb_sr_vbf etau_comb_sr_vbf \
#for channel in etau_sr1_ddZll etau_sr2_ddZll etau_sr3_ddZll etau_sr1_vbf_ddZll etau_sr2_vbf_ddZll etau_sr3_vbf_ddZll etau_vbf_ddZll \
#	       etau_comb_sr_ddZll etau_comb_sr_vbf_ddZll
#for channel in mtau_sr1_vbf mtau_sr2_vbf mtau_sr3_vbf mtau_sr1_vbf_noAgg mtau_sr2_vbf_noAgg mtau_sr3_vbf_noAgg \
#               mtau_sr1_vbf_llbin mtau_sr2_vbf_llbin mtau_sr3_vbf_llbin mtau_sr1_vbf_llbin_noAgg mtau_sr2_vbf_llbin_noAgg mtau_sr3_vbf_llbin_noAgg \
#               mtau_vbf mtau_vbf_noAgg mtau_vbf_llbin mtau_vbf_llbin_noAgg \
#               mtau_comb_sr_vbf_new mtau_comb_sr_vbf_noAgg mtau_comb_sr_vbf_llbin_noAgg mtau_comb_sr_vbf_llbin mtau_comb_sr_vbf_llbin_wtag \
#               mtau_comb_sr_vbf_llbin_noAgg_wtag etau_comb_sr_vbf_llbin_noAgg_wtag etau_comb_sr_vbf_llbin_wtag etau_comb_sr_vbf_llbin_noAgg
#for channel in mtau_sr1 mtau_sr2 mtau_sr3 mtau_sr1_noAgg mtau_sr2_noAgg mtau_sr3_noAgg \
#               mtau_sr1_llbin mtau_sr2_llbin mtau_sr3_llbin mtau_sr1_llbin_noAgg mtau_sr2_llbin_noAgg mtau_sr3_llbin_noAgg \
#               mtau_comb_sr mtau_comb_sr_noAgg mtau_comb_sr_llbin mtau_comb_sr_llbin_noAgg
#for channel in etau_sr1 etau_sr2 etau_sr3 etau_sr1_noAgg etau_sr2_noAgg etau_sr3_noAgg etau_sr1_llbin etau_sr2_llbin etau_sr3_llbin \
#	       etau_sr1_llbin_noAgg etau_sr2_llbin_noAgg etau_sr3_llbin_noAgg etau_sr1_vbf etau_sr2_vbf etau_sr3_vbf \
#	       etau_sr1_vbf_noAgg etau_sr2_vbf_noAgg etau_sr3_vbf_noAgg etau_sr1_vbf_llbin etau_sr2_vbf_llbin etau_sr3_vbf_llbin \
#	       etau_sr1_vbf_llbin_noAgg etau_sr2_vbf_llbin_noAgg etau_sr3_vbf_llbin_noAgg etau_comb_sr etau_comb_sr_noAgg etau_comb_sr_llbin etau_comb_sr_llbin_noAgg \
#	       etau_comb_sr_vbf etau_comb_sr_vbf_noAgg etau_comb_sr_vbf_llbin etau_comb_sr_vbf_llbin_noAgg
#for channel in etau_vbf etau_vbf_noAgg etau_vbf_llbin etau_vbf_llbin_noAgg
#for channel in ll_mtau_cba ll_etau_cba ll_mtau_mva ll_etau_mva \
 #              lh_mtau_cba lh_etau_cba lh_mtau_mva lh_etau_mva \
#               comb_mtau_cba comb_etau_cba comb_etau_mva comb_mtau_mva
#for channel in ll_etau_mva
#for channel in ll_etau_cba_vbf_ap ll_etau_cba_inc_ap ll_etau_cba_ap lh_etau_cba_ap \
#	       lh_etau_cba_inc1_ap lh_etau_cba_inc2_ap lh_etau_cba_inc3_ap lh_etau_cba_vbf_ap comb_etau_cba_ap
#do
for channel in comb_etau_cba ll_etau_cba lh_etau_cba comb_mtau_cba ll_mtau_cba lh_mtau_cba \
	       comb_etau_mva ll_etau_mva llinc_etau_mva llvbf_etau_mva lh_etau_mva lhinc_etau_mva \
	       lhvbf_etau_mva  comb_mtau_mva ll_mtau_mva llinc_mtau_mva llvbf_mtau_mva lh_mtau_mva lhinc_mtau_mva lhvbf_mtau_mva
do
if [ ${channel} == ll_mtau_cba ] || [ ${channel} == lh_mtau_cba ] || [ ${channel} == comb_mtau_cba ]; then
    StatAnalysis=LFV_mtau_cba
 elif [ ${channel} == ll_etau_cba ] || [ ${channel} == lh_etau_cba ] || [ ${channel} == comb_etau_cba ]; then
    StatAnalysis=LFV_etau_cba
 elif [ ${channel} == ll_mtau_mva ] || [ ${channel} == llinc_mtau_mva ] || [ ${channel} == llvbf_mtau_mva ] || [ ${channel} == lh_mtau_mva ] || [ ${channel} == lhinc_mtau_mva ] || [ ${channel} == lhvbf_mtau_mva ]  || [ ${channel} == comb_mtau_mva ]; then
    StatAnalysis=LFV_mtau_mva
 elif [ ${channel} == ll_etau_mva ] || [ ${channel} == llinc_etau_mva ] || [ ${channel} == llvbf_etau_mva ] || [ ${channel} == lh_etau_mva ] || [ ${channel} == lhinc_etau_mva ] || [ ${channel} == lhvbf_etau_mva ]  || [ ${channel} == comb_etau_mva ]; then
    StatAnalysis=LFV_etau_mva
fi  
cat > todo_${channel} <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
rm -rf log/limit_new/${channel}_limit_exp.log
#python scripts/getLimit.py ${channel}/LFV13TeVCombinationCBA 0 125 2>&1 | tee log/limit/${channel}_limit.log
python scripts/getLimit.py data_new/${channel}/v20180326/${StatAnalysis} 0 125 2>&1 | tee log/limit_new/${channel}_limit.log
EOF
chmod +x todo_${channel}
logfile=log/limit_new/${channel}.log
rm -f $logfile ; bsub_lxplus 2nw $logfile 0 ./todo_${channel}
done
