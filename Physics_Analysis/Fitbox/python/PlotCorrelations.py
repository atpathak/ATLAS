from ROOT import TSystem
import sys
from subprocess import call, check_output
from Options import *
import os.path

def main(argv):

    root = Root()
    if not root.setup_all():
        sys.exit("Fatal: Exiting at SetRoot ...")

    options = Options("NPcorrelations").instance



    if not options.correlations_file:
        sys.exit("Fatal: an input file with NP pulls is not loaded correctly -- an NP ranking file is required...")


    odir = options.correlations_outputdir
    if not os.path.exists(odir):
            os.makedirs(odir)

    draw = ".x NLLProfilePlotter/CorrelationMatrix.C+(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %f, \"%s\")"%(
        options.correlations_file,
        odir,
        options.correlations_histo,
        options.correlations_title,
        options.correlations_subtitle,
        options.correlations_min_value,
        options.correlations_print_for_np
        )


    if ROOT.gROOT.ProcessLine(draw):
        sys.exit("Fatal: cannot execute %s"%(draw))            
            
# run main 
if __name__ == "__main__":

    main(sys.argv[1:])

