THISDIR=${PWD}
source /afs/cern.ch/user/a/atpathak/bin/slc6_gcc_4.8.1.sh
source /afs/cern.ch/user/a/atpathak/bin/slc6_python_2.7.3.sh
# cd /afs/cern.ch/user/s/swaban/afswork/public/root
# git clone -n http://root.cern.ch/git/root.git root-v5-34
# cd root-v5-34
# git checkout -b v5-34-00-patches v5-34-32
# cat /afs/cern.ch/atlas/project/HSG7/root/root_v5-34-32/source_files/*.patch | patch -p1
# tar zxf /afs/cern.ch/atlas/project/HSG7/root/root_v5-34-32/source_files/hsg7_extra_150622.tar.gz
#./configure \
#   --fail-on-missing --enable-builtin-pcre --enable-cintex --enable-explicitlink --enable-gdml --enable-genvector \
#   --enable-krb5 --enable-mathmore --enable-minuit2 --enable-mysql --enable-oracle --enable-python --enable-reflex \
#   --enable-roofit --enable-table --enable-xml --enable-unuran --with-cint-maxstruct=36000 --with-cint-maxtypedef=36000 --with-cint-longline=4096 \
#   --with-castor-incdir=/afs/cern.ch/sw/lcg/external/castor/2.1.13-6/x86_64-slc6-gcc48-opt/usr/include/shift \
#   --with-castor-libdir=/afs/cern.ch/sw/lcg/external/castor/2.1.13-6/x86_64-slc6-gcc48-opt/usr/lib64 \
#   --with-cern-libdir=/afs/cern.ch/sw/lcg/external/cernlib/2006a/x86_64-slc6-gcc48-opt/lib \
#   --with-dcap-libdir=/afs/cern.ch/sw/lcg/external/dcache_client/2.47.6-1/x86_64-slc6-gcc48-opt/dcap/lib \
#   --with-dcap-incdir=/afs/cern.ch/sw/lcg/external/dcache_client/2.47.6-1/x86_64-slc6-gcc48-opt/dcap/include \
#   --with-fftw3-incdir=/afs/cern.ch/sw/lcg/external/fftw3/3.1.2/x86_64-slc6-gcc48-opt/include \
#   --with-fftw3-libdir=/afs/cern.ch/sw/lcg/external/fftw3/3.1.2/x86_64-slc6-gcc48-opt/lib \
#   --with-gccxml=/afs/cern.ch/sw/lcg/external/gccxml/0.9.0_20120309p2/x86_64-slc6-gcc48-opt/bin \
#   --with-gfal-libdir=/afs/cern.ch/sw/lcg/external/Grid/gfal/1.16.0-1/x86_64-slc6-gcc48-opt/lib64 \
#   --with-gfal-incdir=/afs/cern.ch/sw/lcg/external/Grid/gfal/1.16.0-1/x86_64-slc6-gcc48-opt/include \
#   --with-gsl-incdir=/afs/cern.ch/sw/lcg/external/GSL/1.10/x86_64-slc6-gcc48-opt/include \
#   --with-gsl-libdir=/afs/cern.ch/sw/lcg/external/GSL/1.10/x86_64-slc6-gcc48-opt/lib \
#   --with-mysql-incdir=/afs/cern.ch/sw/lcg/external/mysql/5.5.14/x86_64-slc6-gcc48-opt/include \
#   --with-mysql-libdir=/afs/cern.ch/sw/lcg/external/mysql/5.5.14/x86_64-slc6-gcc48-opt/lib \
#   --with-oracle-incdir=/afs/cern.ch/sw/lcg/external/oracle/11.2.0.3.0/x86_64-slc6-gcc48-opt/include \
#   --with-oracle-libdir=/afs/cern.ch/sw/lcg/external/oracle/11.2.0.3.0/x86_64-slc6-gcc48-opt/lib \
#   --with-rfio-incdir=/afs/cern.ch/sw/lcg/external/castor/2.1.13-6/x86_64-slc6-gcc48-opt/usr/include/shift \
#   --with-rfio-libdir=/afs/cern.ch/sw/lcg/external/castor/2.1.13-6/x86_64-slc6-gcc48-opt/usr/lib64 \
#   --with-pythia6-libdir=/afs/cern.ch/sw/lcg/external/MCGenerators/pythia6/428.2/x86_64-slc6-gcc46-opt/lib \
#   --with-pythia8-incdir=/afs/cern.ch/sw/lcg/external/MCGenerators/pythia8/170/x86_64-slc5-gcc43-opt/include \
#   --with-pythia8-libdir=/afs/cern.ch/sw/lcg/external/MCGenerators/pythia8/170/x86_64-slc5-gcc43-opt/lib \
#   --with-gviz-incdir=/afs/cern.ch/sw/lcg/external/graphviz/2.28.0/x86_64-slc6-gcc48-opt/include/graphviz \
#   --with-gviz-libdir=/afs/cern.ch/sw/lcg/external/graphviz/2.28.0/x86_64-slc6-gcc48-opt/lib \
#   --with-xrootd=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc6-gcc48-opt \
#   --with-srm-ifce-incdir=/afs/cern.ch/sw/lcg/external/Grid/srm-ifce/1.18.0-1/x86_64-slc6-gcc48-opt/include \
#   --with-python-incdir=/afs/cern.ch/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc48-opt/include/python2.7 \
#   --with-python-libdir=/afs/cern.ch/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc48-opt/lib
#gmake -j8
export ROOTSYS=/afs/cern.ch/user/s/swaban/afswork/public/root/root-v5-34
cd ${ROOTSYS}
. bin/thisroot.sh
cd ${THISDIR}
export LD_LIBRARY_PATH=${PWD}/:${PWD}/tmp:${PWD}/lib:${PWD}/bin:${LD_LIBRARY_PATH}
export PATH=.:${PWD}/bin:${ROOTSYS}/include:${ROOTSYS}/lib:${ROOTSYS}/bin:${PATH}
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/project/eos/installation/pro/lib64/
export PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc6-gcc48-opt/bin/:$PATH
export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc6-gcc48-opt/lib64:${LD_LIBRARY_PATH}
