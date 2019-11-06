# source WorkspaceBuilder/CBAmakeWS.sh <version> <region> <data> <merge>
# version : name given to output folder
# region  : which fits to run (ll,lh,hh,boost,vbf,all)
# data    : either hybrid or asimov?
# merge   : either merge or split?

version=$1
ll=/afs/cern.ch/atlas/project/HSG4/HtautauRun2/ll/150716_InputWS/ll.27072016.withlumi_Shnorm_rebin_for_merged.root
lh=/afs/cern.ch/atlas/project/HSG4/HtautauRun2/lh/29July/rebin_comb_fs20.root
hh=/afs/cern.ch/atlas/project/HSG4/HtautauRun2/hh/160726_xTauAna_v12_CBA/merged/hh_WSinput_wFakeUnc.root

nf=WorkspaceBuilder/data/normfactors13TeV.txt

xml=WorkspaceBuilder/data/HTauTau13TeVCombination.xml
if ["$4" = "merge"]; then
    xml=WorkspaceBuilder/data/HTauTauAllCombination.xml
elif ["$4" = "split"]; then
    xml=WorkspaceBuilder/data/HTauTau13TeVCombination.xml
fi
of=WorkspaceBuilder/data/overallsystematics13TeV.txt

if ["$3" = "hybrid"]; then
    if [ "$2" = 'hh' ]; then
        BuildWorkspace -s HTauTau13TeVHHCBA -v ${version} -x ${xml} -r ${hh} -r ${ll} -n ${nf} -o ${of} -a 
    elif [ "$2" = 'lh' ]; then
        BuildWorkspace -s HTauTau13TeVLHCBA -v ${version} -x ${xml} -r ${lh} -r ${ll} -n ${nf} -o ${of} -a 
    elif [ "$2" = 'll' ]; then
        BuildWorkspace -s HTauTau13TeVLLCBA -v ${version} -x ${xml} -r ${ll} -n ${nf} -o ${of} -a 
    elif [ "$2" = 'all' ]; then
        BuildWorkspace -s HTauTau13TeVHHCBA -v ${version} -x ${xml} -r ${ll} -r ${hh} -n ${nf} -o ${of} -a
        BuildWorkspace -s HTauTau13TeVLHCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -n ${nf} -o ${of} -a
        BuildWorkspace -s HTauTau13TeVLLCBA -v ${version} -x ${xml} -r ${ll} -n ${nf} -o ${of} -a 
        BuildWorkspace -s HTauTau13TeVCombinationBoostCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a
        BuildWorkspace -s HTauTau13TeVCombinationVBFCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a
        BuildWorkspace -s HTauTau13TeVCombinationCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a
    elif [ "$2" = 'boost' ]; then
        BuildWorkspace -s HTauTau13TeVCombinationBoostCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a
    elif [ "$2" = 'vbf' ]; then
        BuildWorkspace -s HTauTau13TeVCombinationBoostCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a
    else
        BuildWorkspace -s HTauTau13TeVCombinationCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a --nomcstat 
    fi
elif ["$3" = "asimov"]; then
    if [ "$2" = 'hh' ]; then
        BuildWorkspace -s HTauTau13TeVHHCBA -v ${version} -x ${xml} -r ${hh} -r ${ll} -n ${nf} -o ${of} -a -p BlindFullWithFloat 
    elif [ "$2" = 'lh' ]; then
        BuildWorkspace -s HTauTau13TeVLHCBA -v ${version} -x ${xml} -r ${lh} -r ${ll} -n ${nf} -o ${of} -a -p BlindFullWithFloat
    elif [ "$2" = 'll' ]; then
        BuildWorkspace -s HTauTau13TeVLLCBA -v ${version} -x ${xml} -r ${ll} -n ${nf} -o ${of} -a -p BlindFullWithFloat
    elif [ "$2" = 'all' ]; then
        BuildWorkspace -s HTauTau13TeVHHCBA -v ${version} -x ${xml} -r ${ll} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
        BuildWorkspace -s HTauTau13TeVLHCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -n ${nf} -o ${of} -a -p BlindFullWithFloat 
        BuildWorkspace -s HTauTau13TeVLLCBA -v ${version} -x ${xml} -r ${ll} -n ${nf} -o ${of} -a   -p BlindFullWithFloat 
        BuildWorkspace -s HTauTau13TeVCombinationBoostCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
        BuildWorkspace -s HTauTau13TeVCombinationVBFCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
        BuildWorkspace -s HTauTau13TeVCombinationCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
    elif [ "$2" = 'boost' ]; then
        BuildWorkspace -s HTauTau13TeVCombinationBoostCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
    elif [ "$2" = 'vbf' ]; then
        BuildWorkspace -s HTauTau13TeVCombinationBoostCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
    else
        BuildWorkspace -s HTauTau13TeVCombinationCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a -p BlindFullWithFloat --nomcstat 
    fi
fi
