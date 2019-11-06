#!/bin/sh
#for channel in mu_combined_sr mu_combined_sr_vbf e_combined_sr e_combined_sr_vbf \
#	       mu_combined_sr_mva mu_combined_sr_vbf_mva e_combined_sr_mva e_combined_sr_vbf_mva
#for channel in mu_combined_sr_vbf e_combined_sr_vbf mu_combined_sr_vbf_mva e_combined_sr_vbf_mva 
#for channel in mtau_comb_sr mtau_comb_sr_noAgg mtau_comb_sr_llbin mtau_comb_sr_llbin_noAgg \
#	       mtau_comb_sr_vbf_new mtau_comb_sr_vbf_noAgg mtau_comb_sr_vbf_llbin mtau_comb_sr_vbf_llbin_noAgg \
#	       etau_comb_sr etau_comb_sr_noAgg etau_comb_sr_llbin etau_comb_sr_llbin_noAgg \
#	       etau_comb_sr_vbf etau_comb_sr_vbf_noAgg etau_comb_sr_vbf_llbin etau_comb_sr_vbf_llbin_noAgg
#for channel in etau_comb_sr_vbf 
#for channel in mtau_comb_sr_vbf_ztt_constrained etau_comb_sr_vbf_ddZll_ztt_constrained
version=v20180326
for channel in comb_etau_mva comb_mtau_mva
do
if [ ${channel} == comb_mtau_cba ]; then
    StatAnalysis=LFV_mtau_cba
 elif [ ${channel} == comb_etau_cba ]; then
    StatAnalysis=LFV_etau_cba
 elif [ ${channel} == comb_mtau_mva ]; then
    StatAnalysis=LFV_mtau_mva
 elif [ ${channel} == comb_etau_mva ]; then
    StatAnalysis=LFV_etau_mva
fi  
for namesys in `seq 1 140`
do
cat > todo_${channel}_${namesys} <<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/atpathak/afswork/public/WSMaker
source setup.sh
#python scripts/runNPranking.py ${channel}/LFV13TeVCombinationCBA 125 ModelConfig SigXsecOverSM obsData ${namesys}
#python scripts/runNPranking.py ${channel}/v20180326/LFV_etau_mva 125 ModelConfig SigXsecOverSM obsData ${namesys}
python scripts/runNPranking.py data_new_check/${channel}/v20180326/${StatAnalysis} 125 ModelConfig SigXsecOverSM obsData ${namesys}
mkdir -vp Alllog_check
mv *.log Alllog_check 
EOF
chmod +x todo_${channel}_${namesys}
rm -f todo_${channel}_${namesys}.log ; bsub_lxplus 2nd todo_${channel}_${namesys}.log 0 ./todo_${channel}_${namesys}
done
done
