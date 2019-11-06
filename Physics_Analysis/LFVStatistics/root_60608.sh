THISDIR=${PWD}
source /afs/cern.ch/user/a/atpathak/bin/slc6_gcc_4.8.1.sh
source /afs/cern.ch/user/a/atpathak/bin/slc6_python_2.7.3.sh
export ROOTSYS=/afs/cern.ch/sw/lcg/app/releases/ROOT/6.06.08/x86_64-slc6-gcc48-opt/root
cd ${ROOTSYS}
. bin/thisroot.sh
cd ${THISDIR}
export LD_LIBRARY_PATH=${PWD}/:${PWD}/tmp:${PWD}/lib:${PWD}/bin:${LD_LIBRARY_PATH}
export PATH=.:${PWD}/bin:${ROOTSYS}/include:${ROOTSYS}/lib:${ROOTSYS}/bin:${PATH}
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/project/eos/installation/pro/lib64/
export PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc6-gcc48-opt/bin/:$PATH
export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc6-gcc48-opt/lib64:${LD_LIBRARY_PATH}
