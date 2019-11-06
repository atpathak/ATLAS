shopt -s expand_aliases

# Job info
echo "Running on $HOSTNAME: $0 $*"

# setup
echo "Running setup"
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source ${{ATLAS_LOCAL_ROOT_BASE}}/user/atlasLocalSetup.sh
pwd
cd {path}/../..
ls
pwd
#lsetup rcsetup
echo "sourcing rootcore setup script"
rcSetup
#source rcSetup.sh
echo ${{ROOTCOREBIN}}
echo ${{ROOTSYS}}
echo ${{ROOTCOREDIR}}
echo ${{ROOTCOREOBJ}}
# rcsetup 2.4.28,Base
# rc find_packages
# rc compile
cd {path}/..
ls {path}/..
echo "Testing ROOT"
root -b -q

echo "Now running..."

cd {path}/..

pwd
python python/binning_optimizer.py {inputfiles} {args}


#'/eos/user/q/qbuat/workspaces/htt_fine_binning/hhAll_merged_WSinput_WithTheoryEnv_WithModJERVar.root'