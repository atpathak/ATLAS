version=$1
ll=/afs/cern.ch/atlas/project/HSG4/HtautauRun2/ll/150716_InputWS/ll.27072016.withlumi_Shnorm_rebin_for_merged.root
lh=/afs/cern.ch/atlas/project/HSG4/HtautauRun2/hh/29July/rebin_comb_fs20.root
hh=/afs/cern.ch/atlas/project/HSG4/HtautauRun2/hh/160726_xTauAna_v12_CBA/merged/hh_WSinput_wFakeUnc.root

nf=WorkspaceBuilder/data/normfactors13TeV.txt
xml=WorkspaceBuilder/data/HTauTauAllCombination.xml
of=WorkspaceBuilder/data/overallsystematics13TeV.txt

if [ "$2" = 'hh' ]; then
    BuildWorkspace -s HTauTau1516HHCBA -v ${version} -x ${xml} -r ${hh} -r ${ll} -n ${nf} -o ${of} -a -p BlindFullWithFloat 
elif [ "$2" = 'lh' ]; then
    BuildWorkspace -s HTauTau1516LHCBA -v ${version} -x ${xml} -r ${lh} -r ${ll} -n ${nf} -o ${of} -a -p BlindFullWithFloat
elif [ "$2" = 'll' ]; then
    BuildWorkspace -s HTauTau1516LLCBA -v ${version} -x ${xml} -r ${ll} -n ${nf} -o ${of} -a -p BlindFullWithFloat
elif [ "$2" = 'all' ]; then
    BuildWorkspace -s HTauTau1516HHCBA -v ${version} -x ${xml} -r ${ll} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
    BuildWorkspace -s HTauTau1516LHCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -n ${nf} -o ${of} -a -p BlindFullWithFloat 
    BuildWorkspace -s HTauTau1516LLCBA -v ${version} -x ${xml} -r ${ll} -n ${nf} -o ${of} -a   -p BlindFullWithFloat 
    BuildWorkspace -s HTauTau1516CombinationBoostCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
    BuildWorkspace -s HTauTau1516CombinationVBFCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
    BuildWorkspace -s HTauTau1516CombinationCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a  -p BlindFullWithFloat 
else
    BuildWorkspace -s HTauTau1516CombinationCBA -v ${version} -x ${xml} -r ${ll} -r ${lh} -r ${hh} -n ${nf} -o ${of} -a -p BlindFullWithFloat --nomcstat 
fi
