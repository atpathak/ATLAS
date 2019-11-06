#lxplus only

export ANALYSISTYPE=AZh

if [ -z "$1" ]
  then
    echo "No argument supplied"
    echo "Checking if you are a predefined user"
    if [[ $USER == "nmorange" ]] || [[ $USER == "cli" ]] || [[ $USER == "changqia" ]] || [[ $USER == "yuj" ]] || [[ $USER == "mdacunha" ]] || [[ $USER == "morange" ]] || [[ $USER == "abell" ]]
    then ANALYSISTYPE="VHbbRun2"
    elif [[ $USER == "hanar" ]]
    then ANALYSISTYPE="AZh"
    elif [[ $USER == "gwilliam" ]] || [[ $USER == "egraham" ]] || [[ $USER == "abetti" ]]
    then ANALYSISTYPE="HHbbtautau"
    elif [[ $USER == "rroehrig" ]]
    then ANALYSISTYPE="MonoH"
    elif [[ $USER == "pgadow" ]] || [[ $USER == "prieck" ]]  || [[ $USER == "xulou" ]]
    then ANALYSISTYPE="MonoV"
    elif [[ $USER == "jhetherl" ]]
    then ANALYSISTYPE="AZh"
    elif [[ $USER == "prose" ]]
    then ANALYSISTYPE="VBFGamma"
    elif [[ $USER == "xiaohu" ]]
    then ANALYSISTYPE="AZHeavyH"
    elif [[ $USER == "coniavit" ]]
    then ANALYSISTYPE="SMHtautau"
    elif [[ $USER == "jpasner" ]]
    then ANALYSISTYPE="VHF"
    fi
else
    echo ""
    echo "Setting ANALYSISTYPE to be: "$1
    export ANALYSISTYPE=$1
    echo ""
fi

if [ ! -d "scripts/"$ANALYSISTYPE ]; then
    echo ""
    echo "ERROR:"
    echo "This scripts directory does not exist: scripts/"$ANALYSISTYPE
    echo "I will not be proceeding unless you give me a proper ANALYSISTYPE"
    echo ""
    return
fi

here=$(pwd)

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

#localSetupROOT 5.34.32-HiggsComb-p1-x86_64-slc6-gcc48-opt
#localSetupROOT 6.06.02-x86_64-slc6-gcc49-opt
localSetupROOT 6.04.16-x86_64-slc6-gcc49-opt
lsetup "sft releases/doxygen/1.8.11-ae1d3"

# increase stack size - needed for large workspaces
ulimit -S -s 15360

export LD_LIBRARY_PATH=$ROOTSYS/lib:$LD_LIBRARY_PATH
export PATH=$ROOTSYS/bin:$PATH
# export PATH=/afs/cern.ch/sw/lcg/external/doxygen/1.8.2/x86_64-slc6-gcc48-opt/bin:$PATH
export PATH=$here:$here/scripts:$PATH
export PYTHONPATH=$ROOTSYS/lib:$PYTHONDIR/lib:$PYTHONPATH

# include local lib in library path
export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH

TEST="${0}"
if [[ ${TEST} == "-bash" ]]; then # sourcing this script in the bash shell
  export WORKDIR="$PWD"
else # zsh shell ( at least not bash )
  export WORKDIR="$( cd "$( dirname "$0" )" && pwd )"
fi
export PYTHONPATH=$here/scripts:$PYTHONPATH

if [[ $USER == "changqia" ]]; then
  if [ ! -f "launch_default_jobs.py" ]; then
    echo "Soft link of launch job doesn't exit! Generating it!"
    ln -s scripts/VHbbRun2/launch_default_jobs.py
  fi
  if [ ! -f "AdvGatherResults.sh" ]; then
    echo "Soft link of Merging script doesn't exit! Generating it!"
    ln -s scripts/VHbbRun2/AdvGatherResults.sh
  fi
fi


echo "Setting ANALYSISTYPE to : "$ANALYSISTYPE
export PYTHONPATH=$here/scripts/$ANALYSISTYPE:$PYTHONPATH
export PATH=$here/scripts/$ANALYSISTYPE:$PATH

export NCORE=4

echo ""
echo "What to do next:"
echo "Now you must compile the packages"
echo "$ gmake -j5"
