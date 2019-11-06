#channel = ["Combined (CBA)", "lh Combined (CBA)","lh vbf (CBA)", "lh inc3 (CBA)", "lh inc2 (CBA)", "lh inc1 (CBA)","ll Combined (CBA)","ll vbf (CBA)","ll inc (CBA)"]
for channel in comb_etau_cba lh_etau_cba ll_etau_cba
#for channel in comb_mtau_cba lh_mtau_cba ll_mtau_cba
do
#if  [ ${channel} == lh_etau_cba ]; then
#    declare -a arr=("limit" "config_vbf_limit" "config_inc3_limit" "config_inc2_limit" "config_inc1_limit")
#elif [ ${channel} == ll_etau_cba ]; then
 #   declare -a arr=("limit" "config_vbf_limit" "config_inc_limit" )
#elif [ ${channel} == comb_etau_cba ]; then
 #   declare -a arr=("limit")
#fi
#if  [ ${channel} == lh_etau_cba ]; then
#    declare -a arr=("signf" "config_vbf_signf" "config_inc3_signf" "config_inc2_signf" "config_inc1_signf")
#elif [ ${channel} == ll_etau_cba ]; then
#   declare -a arr=("signf" "config_vbf_signf" "config_inc_signf" )
#elif [ ${channel} == comb_etau_cba ]; then
#    declare -a arr=("signf")
#fi
if  [ ${channel} == lh_etau_cba ]; then
    declare -a arr=("muhat" "config_vbf_muhat" "config_inc3_muhat" "config_inc2_muhat" "config_inc1_muhat")
elif [ ${channel} == ll_etau_cba ]; then
    declare -a arr=("muhat" "config_vbf_muhat" "config_inc_muhat" )
elif [ ${channel} == comb_etau_cba ]; then
    declare -a arr=("muhat")
fi
for StatAnalysis in "${arr[@]}"
do
#grep "Expected limit" log/limit_new/${channel}_${StatAnalysis}.log | awk '{print $3}'
#grep "Observed limit" log/limit_new/${channel}_${StatAnalysis}.log | awk '{print $3}'
#grep "Expected limit" log/limit_new/${channel}_${StatAnalysis}.log | awk '{print $5}'
#grep "2sigma" log/limit_new/${channel}_${StatAnalysis}.log | awk 'FNR == 2 {print $2}'    
#grep "Observed significance" log/signf_new/${channel}_${StatAnalysis}.log | awk '{print $3}'
#grep "Median significance" log/signf_exp/${channel}_${StatAnalysis}_exp.log | awk '{print $3}'
#grep "muhat:" log/muhat_new/${channel}_${StatAnalysis}.log | awk '{print $2}'   
#grep "DataStat - no NPs  :" log/muhat_new/${channel}_${StatAnalysis}.log | sed -e 's/+/ /g' | awk '{print $6}'
grep "Total :" log/muhat_new/${channel}_${StatAnalysis}.log | awk '{print $7}' #|awk -F"." '{print $1}' #sed -e 's// /g' #| awk '{print $6}'
done
done
#cat limit_tee.txt | awk  '{ printf(", %s ", $1 ); } END { printf( "]\n" ); }'
