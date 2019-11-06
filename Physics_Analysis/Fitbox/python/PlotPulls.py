from ROOT import TSystem
import sys
from subprocess import call, check_output
from Options import *
import os.path

def main(argv):

    root = Root()
    if not root.setup_all():
        sys.exit("Fatal: Exiting at SetRoot ...")

    options = Options("NPpulls").instance

    if True : #options.show_impact_only: 

        if not options.pulls_file:
            sys.exit("Fatal: an input file with NP pulls is not loaded correctly -- an NP ranking file is required...")


        odir = options.pulls_outputdir
        if not os.path.exists(odir):
            os.makedirs(odir)

        draw = ".x NLLProfilePlotter/PlotPulls.C(\"%s\", \"%s\", \"%s\", %f, %i, %i, %i, %i)"%(options.pulls_file,
                                                                                                odir,
                                                                                                options.pulls_highlight,
                                                                                                options.pulls_min_value,
                                                                                                options.pulls_only_nf,
                                                                                                options.pulls_sort_by_constraint,
                                                                                                options.pulls_no_sort,
                                                                                                options.pulls_multiple_pois)
        

        if ROOT.gROOT.ProcessLine(draw):
            sys.exit("Fatal: cannot execute %s"%(draw))            
            
# run main 
if __name__ == "__main__":

    main(sys.argv[1:])

