for rtag in r9871 r9875 r9876 r9877 r7725
do
GetTfCommand.py --AMI=${rtag} | tee reco_${rtag}.txt
cat reco_${rtag}.txt | grep Reco_tf | sed -e 's~--~\\\n--~g' | tee reco_${rtag}.todo
done
