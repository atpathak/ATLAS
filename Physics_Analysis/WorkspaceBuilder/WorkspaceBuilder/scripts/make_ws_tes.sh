#!/bin/bash
#
# a handy script to generate WS's for TES  
#
# zenon@cern.ch
#
# use example:
#
# source WorkspaceBuilder/scripts/make_ws_tes.sh /afs/cern.ch/user/l/lixia/workspace/public/TES_inputs/calo_tes_20170721.root true
#
FILE=$1
COM=""
VERSION=""

DO_ENVELOPES=$2

if [ -f "log.tes" ]; then
    rm -f log.tes
fi

if [ -f $FILE ]; then
   echo "File '$FILE' Exists -- Creating WS..."
   COM="python"
   VERSION=$(basename "$FILE" | cut -d. -f1)
   echo "Version:" $VERSION
else
   echo "The File '$FILE' Does Not Exist - No WS will be created..."
   COM="echo python"
fi

if [ "$DO_ENVELOPES" = true ]; then
    echo "Doing the Systematic Envelopes..."

    $COM  WorkspaceBuilder/python/runWSbuildTES.py\
 --tes calo\
 --channels muhad\
 --prongs 1p\
 --eta central\
 --pt pTslice1\
 --data unblind\
 --version $VERSION\
 --input $FILE\
 --parameters NoSmoothing AlphaPruningApply AlphaPruningKey=ALPHA KeepNuisApply KeepNuisWithKeys=ALPHA KeepNuisForSamples=Ztt
 --analyze\
 --envelopes\
 2>&1 | tee -a log.tes

else
    
    $COM  WorkspaceBuilder/python/runWSbuildTES.py\
 --tes calo\
 --channels muhad\
 --prongs 1p\
 --eta central forward\
 --pt pTincl pTslice1 pTslice2 pTslice3 pTslice4\
 --data unblind\
 --version $VERSION\
 --input $FILE\
 --parameters NoSmoothing AlphaPruningApply AlphaPruningKey=ALPHA KeepNuisApply KeepNuisWithKeys=ALPHA KeepNuisForSamples=Ztt KeepNuisForChannels=Signal\
 --analyze\
 2>&1 | tee -a log.tes

#    $COM  WorkspaceBuilder/python/runWSbuildTES.py\
# --tes calo\
# --channels muhad\
# --prongs 1p\
# --eta central\
# --pt pTslice1\
# --data unblind\
# --version $VERSION\
# --input $FILE\
# --parameters Smooth=systematics,W,Fake,Zll,Top ApplySpecialAlphaPruning=ALPHA\
# --analyze\
# 2>&1 | tee -a log.tes

fi

#  --parameters NoSmoothing ApplySpecialAlphaPruning=ALPHA\
# --parameters NoSmoothing ApplySpecialAlphaPruning=ALPHA\
#  --parameters NoSmoothing KeepNPcontaining=ALPHA\
# --parameters NoSmoothing NoPruning\
# --parameters Smooth=systematics,W,Fakes KeepNPcontaining=ALPHA\
# --pt pTincl pTslice1 pTslice2 pTslice3 pTslice4 pTcomb\
# --eta central forward\
# --prongs 1p 3p\