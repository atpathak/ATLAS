from ROOT import TSystem
import sys
from subprocess import call, check_output
from Options import *
import os.path

def main(argv):

    root = Root()
    if not root.setup_all():
        sys.exit("Fatal: Exiting at SetRoot ...")

    options = Options("contour").instance

    if True :  


        #if not os.path.exists(odir):
        #    os.makedirs(odir)

        draw = ".x NLLProfilePlotter/MakeContourPlot.C(\"%s\", %f, %f, \"%s\", \"%s\" )"%(options.input_path,
                                                                                          options.expxval,
                                                                                          options.expyval,
                                                                                          options.xtitle,
                                                                                          options.ytitle)
        
        if ROOT.gROOT.ProcessLine(draw):
            sys.exit("Fatal: cannot execute %s"%(draw))            
            
# run main 
if __name__ == "__main__":

    main(sys.argv[1:])

