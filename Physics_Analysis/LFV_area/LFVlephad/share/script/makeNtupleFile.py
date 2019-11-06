#!/usr/bin/env python2

template_mc = "/storage/groups/atl/lephad_ntuples/june/mc/group.phys-higgs.ClhsH.mc15_13TeV.410000.PoPy_P2012_ttb_nonallh.D2.e3698_s2608_s2183_r7725_r7676_p2666.1606_hist/group.phys-higgs.8893128._000001.hist-output.root"
template_data = "/storage/groups/atl/lephad_ntuples/june/data/group.phys-higgs.ClhsH.data15_13TeV.00280853.physics_Main.D2.r7562_p2521_p2667.1606_hist/group.phys-higgs.8893318._000001.hist-output.root"

cut_mc = "CutPreselectionMC"
cut_data = "CutPreselectionData"

output = "definitions/htt_lephad_bdt.ntuple"
ntuple = "data/preselection.root"
tree_name = "NOMINAL"

#######################################################################

from ROOT import *

# type_map = {
# 	"B": "Char_t",
# 	"b": "UChar_t",
# 	"S": "Short_t",
# 	"s": "UShort_t",
# 	"I": "Int_t",
# 	"i": "UInt_t",
# 	"F": "Float_t",
# 	"D": "Double_t",
# 	"L": "Long64_t",
# 	"l": "ULong64_t",
# 	"O": "Bool_t"
# }
type_map = {
	"B": "char",
	"b": "char",
	"S": "int",
	"s": "int",
	"I": "int",
	"i": "int",
	"F": "float",
	"D": "double",
	"L": "int",
	"l": "int",
	"O": "bool"
}

def get_branches(template):
	template_file = TFile.Open(template)
	tree = template_file.Get(tree_name)
	branches = []
	for branch in tree.GetListOfBranches():
		name, type_code = branch.GetTitle().split("/")
		type = type_map[type_code]
		branches.append("%s %s << %s" % (type, name, name))
	template_file.Close()
	return branches

with open(output, "w") as f:
    print >>f, "mc: %s;" % ", ".join(get_branches(template_mc))
    print >>f, "data: %s;" % ", ".join(get_branches(template_data))
    print >>f, ""
    print >>f, "@%s: mc >> %s:mc" % (cut_mc, ntuple)
    print >>f, "@%s: data >> %s:Data" % (cut_data, ntuple)

