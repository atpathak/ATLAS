#channel = ["Combined (MVA)", "lh Combined (MVA)","lh vbf (MVA)", "lh inc (MVA)", "ll Combined (MVA)","ll vbf (MVA)","ll inc (MVA)"]
for channel in comb_etau_mva lh_etau_mva lhvbf_etau_mva lhinc_etau_mva ll_etau_mva llvbf_etau_mva llinc_etau_mva
do
#grep "Expected limit" log/limit_new/${channel}_limit.log | awk '{print $3}'
#grep "Observed limit" log/limit_new/${channel}_limit.log | awk '{print $3}'
#grep "Expected limit" log/limit_new/${channel}_limit.log | awk '{print $5}'
#grep "2sigma" log/limit_new/${channel}_limit.log | awk 'FNR == 2 {print $2}'    
#grep "Observed significance" log/signf_new/${channel}_signf.log | awk '{print $3}'
#grep "Median significance" log/signf_exp/${channel}_signf_exp.log | awk '{print $3}'
##grep "muhat:" log/muhat_new/${channel}_muhat.log | awk '{print $2}'   
#grep "DataStat - no NPs  :" log/muhat_new/${channel}_muhat.log | sed -e 's/+/ /g' | awk '{print $6}'
grep "Total :" log/muhat_new/${channel}_muhat.log | awk '{print $7}' #|awk -F"." '{print $1}' #sed -e 's// /g' #| awk '{print $6}'
done
#cat limit_tee.txt | awk  '{ printf(", %s ", $1 ); } END { printf( "]\n" ); }'
