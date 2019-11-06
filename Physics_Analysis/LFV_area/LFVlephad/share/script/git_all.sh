#/bin/sh
(
n=$(ls Htt2016* 2>/dev/null | wc -l)
while [ $n == 0 ]
do
    cd ..
    n=$(ls Htt2016* 2>/dev/null | wc -l)
    if [ $(pwd) == "/" ]; then echo "Cannot find Htt2016*"; exit 1 ;fi
done
for i in Htt2016*
do
    echo "--- In $i"
    ( cd $i && git $* )
    echo "---"
done
)
