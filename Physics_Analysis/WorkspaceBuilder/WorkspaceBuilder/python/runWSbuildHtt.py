import os, argparse, sys, subprocess
from argparse import ArgumentParser
import itertools

channels_options = ["all", "ll", "lh", "hh"]
regions_options = ["all", "boost", "vbf", "sr", "srboost", "srvbf", "cr", "crboost", "crvbf","vbfloose","vbftight","boostloose","boosttight","vbflowdr","vbfhighdrt","vbfhighdrl"]
special_options = ["split", "merged", "mvis", "withzttcr"]
data_options = ["asimov", "hybrid", "mublind", "rndmmu"]
poi_options = ["singleMu","ggHVBF", "muByProdMod","XStotalByProdMode"]
analysis_options = ["cba","mva"]

channels_optstr = map(str,channels_options)
regions_optstr = map(str,regions_options)
special_optstr = map(str,special_options)
data_optstr = map(str,data_options)
poi_optstr = map(str,poi_options)
analysis_optstr = map(str,analysis_options)

parser = ArgumentParser()
parser.add_argument( '--channels', nargs='+' , help='Valid names ['+', '.join(channels_optstr)+']', type=str           )
parser.add_argument( '--regions' , nargs='+' , help='Valid names ['+', '.join(regions_optstr)+']' , type=str           )
parser.add_argument( '--special' , nargs=1   , help='Valid names ['+', '.join(special_optstr)+']' , type=str           )
parser.add_argument( '--data',     nargs=1   , help='Valid names ['+', '.join(data_optstr)+']'    , type=str           )
parser.add_argument( '--poi',      nargs=1   , help='Valid names ['+', '.join(poi_optstr)+']'     , type=str           )
parser.add_argument( '--analysis',      nargs=1   , help='Valid names ['+', '.join(analysis_optstr)+']'     , type=str           )
parser.add_argument( '--mu',                   help='Value of mu to unblind'                      , type=int           )
parser.add_argument( '-p',       '--printsh' , help='Print sh'                                    , action='store_true')
# Please note: unblind is not implemented yet! 
parser.add_argument( '-u',       '--unblind' , help='Unblind data?'                               , action='store_true')
parser.add_argument( '-e',       '--envelope', help='Make envelope plots'                         , action='store_true')
parser.add_argument( '-v',       '--version' , help='Directory name appears in ./workspace'       , type=str           , required=True)
parser.add_argument( '-b',       '--batch'   , help='Do not require hitting ENTER at runtime'     , action='store_true')
parser.add_argument( '-l',       '--lowmass'   , help='Create a low-mass (m<100) UNBLINDED workspace'     , action='store_true')
result = parser.parse_args() 

if result.channels:
    for x in result.channels:
        if not x in channels_options:
            sys.exit(x+" not valid! Choose from "+", ".join(channels_optstr))
if result.regions:
    for x in result.regions: 
        if not x in regions_options:
            sys.exit(x+" not valid! Choose from "+", ".join(regions_optstr))
        if x in ["vbfloose","vbftight","boostloose","boosttight","vbflowdr","vbfhighdrt","vbfhighdrl"]:
            print "You have input one or more sub-regions. Please ensure you have selected a valid channel/region combination. This will not be checked!"
if result.special:
    for x in result.special: 
        if not x in special_options:
            sys.exit(x+" not valid! Choose from "+", ".join(special_optstr))
if result.poi:
    for x in result.poi:
        if not x in poi_options:
            sys.exit(x+" not valid! Choose from "+", ".join(poi_optstr)) 
if result.analysis:
    for x in result.analysis:
        if not x in analysis_options:
            sys.exit(x+" not valid! Choose from "+", ".join(analysis_optstr))
if result.data:
    for x in result.data: 
        if not x in data_options:
            sys.exit(x+" not valid! Choose from "+", ".join(data_optstr))
    #Check if necessary options are set
    if "mublind" in result.data:
        if not result.mu: sys.exit("Error: The option \"--mu x\" is mandatory when you specify \"--data mublind\".")


#Setting default values in case the options are not specified
channels = result.channels if result.channels else ['ll', 'lh', 'hh', 'all']
regions  = result.regions  if result.regions  else ['boost', 'vbf', 'all']
special  = result.special  if result.special  else ['merged']
data     = result.data     if result.data     else ['asimov']
poi      = result.poi      if result.poi      else ['singleMu']
analysis = result.analysis if result.analysis else ['cba']
mu       = result.mu
unblind  = True if (result.unblind or result.lowmass) else False
lowmass  = True if result.lowmass else False




print "Running over combinations of : "
print "Channels : ", channels
print "Regions : ",  regions
print "Special : ",  special
print "Data : ",     data
print "POI : ",      poi
print "Analysis: ",  analysis
print "Mu : ",       mu
if lowmass:
    print "Only using low-mass (m<100 GeV) region -- Make sure you use CBA inputs! If you don't use MMC as input, results will be unpredictable!"
print "Will be blinded" if not unblind else "WARNING! This will unblind the analysis!!"
if not result.batch:
    raw_input('ENTER to continue')

## here change path to your input root files - will be merged, processed by ZttTheorySyst script and then used as the inputs for WSbuilder
## Ztt theory unc post-processing
raw_llin="/afs/cern.ch/work/m/mmlynari/Public/leplep_18-03-01.mmc_mlm_rebin_WithTheoryEnv_WithModJERVar.root"
raw_lhin="/afs/cern.ch/user/a/ademaria/public/ForMichaela/rebinned_lephad_fs56_MMC_Fine_WithoutCrazyPDFBins_With_MadVSShe_input_AllReg_WithModJERVar_WithTheoryEnv_WithFinalPSUnc.root"
raw_hhin="/afs/cern.ch/user/a/ademaria/public/ForMichaela/hhAll_merged_WSinput_rebinned_WithTheoryEnv_WithModJERVar_WithFinalPSUnc.root"



if not os.path.isdir("WS_inputs"):
    os.makedirs("WS_inputs")

    
file_merged = "merged_inputs_"+result.version+".root"
file_overallSyst = "WorkspaceBuilder/data/Htautau/overallsystematics13TevZtt.txt"

if os.path.isfile(file_merged):
    print "Merged file already exists, going to delete it and merge new inputs."
    os.system("rm "+file_merged)
os.system("hadd "+file_merged+" "+raw_llin+" "+raw_lhin+" "+raw_hhin+" > WS_inputs/hadd_output_"+result.version+".txt")
print "hadd new inputs... Done! created:"+file_merged+" and WS_inputs/hadd_output_"+result.version+".txt"
print "delete old overallsystematics13TevZtt.txt..."
os.system("rm "+file_overallSyst)

print "starting script ZttTheorySyst.py"

os.system("python WorkspaceBuilder/scripts/ZttTheorySyst.py -ws "+file_merged)

file_merged_ZttTheory = file_merged.replace('.root','_zttTheoryConstrained.root')

# do not change the numbers
print "starting script ZttTheoryMGvsShShape.py"

os.system("python WorkspaceBuilder/scripts/ZttTheoryMGvsShShape.py -ws WS_inputs/"+file_merged_ZttTheory+" --cba_ll_weight 'hist' --ll_boost_tight_theory_ztt_MGvsSh_high 1.0257614422 --ll_boost_loose_theory_ztt_MGvsSh_high 0.9854057675 --ll_vbf_tight_theory_ztt_MGvsSh_high 0.9469958981 --ll_vbf_loose_theory_ztt_MGvsSh_high 1.0249683917 --cba_lh_weight 'hist' --lh_boost_tight_theory_ztt_MGvsSh_high 1.0257614422 --lh_boost_loose_theory_ztt_MGvsSh_high 0.9854057675 --lh_vbf_tight_theory_ztt_MGvsSh_high 1.0371028608 --lh_vbf_loose_theory_ztt_MGvsSh_high 0.9789237145 --cba_hh_weight 'hist' --hh_boost_tight_theory_ztt_MGvsSh_high 1.0103463132 --hh_boost_loose_theory_ztt_MGvsSh_high 0.9932258548 --hh_vbf_tight_theory_ztt_MGvsSh_high 0.944811071 --hh_vbf_loose_theory_ztt_MGvsSh_high 1.02754188 --hh_vbf_highPt_theory_ztt_MGvsSh_high 1.0710351246")

file_MGvsShShape  = file_merged_ZttTheory.replace('.root','_MGvsShShape.root')


## inputs for WSbuilder - do not change this part
## in for merged, 15 and 16 for split
llin="WS_inputs/"+file_MGvsShShape
lhin="WS_inputs/"+file_MGvsShShape
hhin="WS_inputs/"+file_MGvsShShape


### m_vis Inputs
if 'mvis' in special:
    llin="/afs/cern.ch/atlas/project/HSG4/HtautauRun2/ll/131016_InputsWS/ll_inputsWS.comb.all.SR_mvis_wlumi_wtheoryztt_rebin_for_merged.root"

nf  = "WorkspaceBuilder/data/Htautau/normfactors13TeV.txt"
of  = "WorkspaceBuilder/data/Htautau/overallsystematics13TeV.txt"
ofZtt = "WorkspaceBuilder/data/Htautau/overallsystematics13TevZtt.txt"

def makeSubmitStr( x ):
    # Please note if certain combinations are not available, please look in the relevant xml file and add them in! 
    # In particular the 15/16 split may have the wrong names
    xml = "WorkspaceBuilder/data/Htautau/HTauTau13TeVCombinationMVA.xml" if x[5]=="mva" else "WorkspaceBuilder/data/Htautau/HTauTau13TeVCombinationCBA.xml"
    if("withzttcr" in x[2]):
        xml = "WorkspaceBuilder/data/Htautau/HTauTau13TeVCombinationCBAwithZttCR.xml" #use the old xml that includes the Ztt CR.
    if x[4]=="singleMu": xml +=" WorkspaceBuilder/data/Htautau/HTauTau13TeVPOISingleMu.xml"
    if x[4]=="ggHVBF": xml +=" WorkspaceBuilder/data/Htautau/HTauTau13TeVPOIggHVBF.xml"
    if x[4]=="muByProdMod":  xml +=" WorkspaceBuilder/data/Htautau/HTauTau13TeVPOIMuByProdMode.xml"
    if x[4]=="XStotalByProdMode":  xml +=" WorkspaceBuilder/data/Htautau/HTauTau13TeVPOIXStotalByProdMode.xml"
    else: ""
    #Analysis regions
    StatAnalysisName = "HTauTau13TeV"
    StatAnalysisName += "LL" if x[0] == "ll" \
        else  "LH" if x[0] == "lh" \
        else  "HH" if x[0] == "hh" \
        else  "Combination"
    StatAnalysisName += "Boost" if "boost" in x[1] else "VBF" if "vbf" in x[1] else ""
    StatAnalysisName += "Loose" if "loose" in x[1] else "Tight" if "tight" in x[1] else "LowDR" if "lowdr" in x[1] else "HighDRTight" if "highdrt" in x[1] else "HighDRLoose" if "highdrl" in x[1] else ""
    StatAnalysisName += "SR" if "sr" in x[1] else "CR" if "cr" in x[1] else ""

    StatAnalysisName += "MVA" if x[5]=="mva" else "CBA"

    TARGET_DIR="workspaces/"+result.version+"/"+StatAnalysisName
    if not os.path.isdir(TARGET_DIR):
        os.makedirs(TARGET_DIR)
    outstr  = "BuildWorkspace -s "+StatAnalysisName
    outstr += " -v "+result.version
    outstr += " -x "+xml
   
    if x[0]=="all":
        outstr += " WorkspaceBuilder/data/Htautau/HTauTau13TeVSignalTotalTheoSystematicsZtt.xml "

    #Setting up input files
    b_nonSplitYearWS = (x[2] == "merged" or x[2] == "mvis" or x[2] == "withzttcr")
    outstr += " -r "+llin if b_nonSplitYearWS else " -r "+ll15+" -r "+ll16 #LL input is always needed basically
    if x[0] == "lh" or x[0] == "all": outstr += " -r "+lhin if b_nonSplitYearWS else " -r "+lh15+" -r "+lh16 #LH input
    if x[0] == "hh" or x[0] == "all": outstr += " -r "+hhin if b_nonSplitYearWS else " -r "+hh15+" -r "+hh16 #HH input
    #Other options
    outstr += " -n "+nf
    outstr += " -o "+of
    if x[0]=="all": outstr += " "+ofZtt
    outstr += " -a"
    if result.envelope: outstr += " --envelope"
    #Blinding options
    if unblind: outstr += " -u "
    else: outstr += " -p BlindFullWithFloat" if x[3] == "asimov" else " -p BlindWithMu="+str(mu) if x[3] == "mublind" else " -p RunRandomMu" if x[3] == "rndmmu" else ""
    if lowmass: outstr += " -p UseSideBandForFit "
    outstr += " -p AggressiveSymmLocal"
    outstr += " --smcoupling"
    outstr += " 2>&1 | tee workspaces/"+result.version+"/"+StatAnalysisName+"/wsbuild.log"

    return outstr

if __name__ == '__main__':
    combinations = itertools.product( channels, regions, special, data, poi, analysis )
    commands = map( makeSubmitStr, combinations )
    print "-----------------------"
    print commands
    print "-----------------------"
    if result.envelope:
        os.system("mkdir -p  workspaces/"+result.version+"/allEnvelopes")
    if result.printsh: 
        print "Writing to file", result.version+'.sh'
        outfile = open( result.version+".sh", "w" )
        for cmd in commands: outfile.write(cmd+'\n') 
        outfile.close()
        os.system("chmod +x "+result.version+".sh")
        os.system("./"+result.version+".sh")
    else: 
        for cmd in commands: 
            os.system(cmd)
            if result.envelope:
                os.system("mv workspaces/"+result.version+"/envelope/* workspaces/"+result.version+"/allEnvelopes")
    #Keep all the exact commands for the WS building 
    commandlistfile = open("workspaces/"+result.version+"/"+"commandlistfile.txt", 'w')
    pycommand = "python"
    for args in sys.argv:
        pycommand += " "+args
    pycommand+="\n"
    commandlistfile.write(pycommand)
    for command in commands:
        commandlistfile.write(command+"\n")
    commandlistfile.close()
