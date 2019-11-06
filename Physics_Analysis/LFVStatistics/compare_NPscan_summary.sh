#!/bin/sh
file=compare_NPscan_summary.txt
for np in `cat ${file}`
do
for ws in lhinc_etau_mva lhvbf_etau_mva lh_etau_mva lhvbf_etau_mva_decorr lh_etau_mva_decorr
do
if [ $ws = "lhvbf_etau_mva_decorr"  ] || [ $ws = "lh_etau_mva_decorr" ] ; then
    npscan="NPscan_alpha"
elif [ $ws = "lhinc_etau_mva" ] || [ $ws = "lhvbf_etau_mva" ] || [ $ws = "lh_etau_mva" ] ; then
     npscan="NPscan"
fi
file1=log/scans/${ws}/${npscan}_${np}_-1.5_1.5.log
var1=`grep FitVal ${file1} | awk -F"=" '{print $2}'| awk '{printf "%5.2f\n", $1}'`
errhi1=`grep FitVal ${file1} | awk -F"=" '{print $2}' | awk '{printf "%5.2f\n", $2}'`
errlo1=`grep FitVal ${file1} | awk -F"=" '{print $2}' | awk '{printf "%5.2f\n", $4}'`

#echo $np $ws $var1 $errhi1 $errlo1
echo $np $ws $var1 '\boldmath${^{'$errhi1'}_{'$errlo1'}}$'
done
#echo $np ${var1_lhinc_mtau_mva} '\boldmath${^{'${errhi1_lhinc_mtau_mva}'}_{'${errlo1_lhinc_mtau_mva}'}}$'#
#echo $np $var1_lhinc_mtau_mva $errhi1_lhinc_mtau_mva $errlo1_lhinc_mtau_mva
done

#$var1 \boldmath${^{$errhi1}_{$errlo1}}$
