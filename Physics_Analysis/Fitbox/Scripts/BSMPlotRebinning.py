#include <stdio.h>
#include "TFile.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TMacro.h"
#include "TString.h"
#include "TH1F.h"
#include "vector"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TClass.h"
#include <iostream>
#include "TKey.h"
#include "TROOT.h"
#include "TObject.h"
#include "TLegend.h"
#include "TGraph.h"

from ROOT import *
import array
import sys, os, math, operator

def main(plot_filename= "",output_filename = ""):
    plot_file = TFile(plot_filename,"READ")
    output_file = TFile(output_filename,"RECREATE")
    regions = {
        "Region_BMin0_J0_T0_isMVA0_L0_Y2015_distMTtot_DHadHad": [150, 170,180, 190,  200, 210,220, 240, 260, 300, 350, 400, 450, 500, 600, 700, 800,1000],
        "Region_BMin0_J0_T1_isMVA0_L0_Y2015_distMTtot_DHadHad": [150, 200, 250, 300, 350, 400, 450, 600,700],
        "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DMuHadTcr":[100, 150, 200, 250, 300, 350, 400, 450, 500, 600],
        "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DElHadTcr":[100, 150, 200, 250, 300, 350, 400, 500, 700],
        "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DMuHad":[50, 100, 150, 200, 250, 300, 400, 600],
        "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DElHad":[50, 100, 150, 200, 250, 300, 350, 400, 500, 700],
        "Region_BMin0_J0_T0_isMVA0_L1_Y2015_distMTtot_DMuHad":[50,70,80,90,100,120,140,160,180,200,220,240,260,280,300,500,700],
        "Region_BMin0_J0_T0_isMVA0_L1_Y2015_distMTtot_DElHad":[50, 70, 80, 90, 100,120,140,160,180,200,220,240,260,280,300,320,340,360,400,500,700],

    }

    for reg in regions:
        print reg
        for key in plot_file.GetListOfKeys():
            c1 = gROOT.GetClass(key.GetClassName())
            if not c1.InheritsFrom("TH1"):
                continue;
            histo_name = key.GetName()
            if reg not in histo_name:
                continue
            if reg == "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DElHad" and "Tcr" in histo_name:
                continue
            if reg == "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DMuHad" and "Tcr" in histo_name:
                continue

            print "working on %s" %histo_name
            original_histo = key.ReadObj()
            
            # access to the binning file  to get the original binning
            bins = len(regions[reg])
            
            original_histo.SetBins(bins-1,array.array('d',regions[reg]));
            original_histo.GetXaxis().SetRange(regions[reg][0],regions[reg][bins-1])
            if reg == "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DElHad":
                original_histo.SetName(histo_name+"Sig")
            if reg == "Region_BMin0_J0_T1_isMVA0_L1_Y2015_distMTtot_DMuHad":
                original_histo.SetName(histo_name+"Sig")

            output_file.cd();    
	    original_histo.Write();  

 
if __name__ == "__main__":

    main(str(sys.argv[1]),str(sys.argv[2]))

