from ROOT import TSystem
import sys
from subprocess import call, check_output
from Options import *
import os.path

def main(argv):

    root = Root()
    if not root.setup_all():
        sys.exit("Fatal: Exiting at SetRoot ...")

    options = Options("NPranking").instance

    if options.show_impact_only: #only ranking

        if options.np_merge :
            merged = ".x NLLProfilePlotter/MergeFiles.C(\"%s\")"%(options.np_merge)
            if ROOT.gROOT.ProcessLine(merged):
              sys.exit("Fatal: failed to merge NP files...")

        if not options.np_ranking:
            sys.exit("Fatal: input NP ranking file is not loaded correctly -- an NP ranking file is required...")

        outdir = options.np_output_dir
        if not os.path.exists(outdir):
                os.makedirs(outdir)

        draw_ranking = ".x NLLProfilePlotter/PlotNPRanking.C(\"%s\", \"%s\", %i, \"%s\", \"%s\")"%(options.np_ranking,
                                                                                                   options.np_name,
                                                                                                   options.np_number,
                                                                                                   outdir,
                                                                                                   options.output_file)
        if ROOT.gROOT.ProcessLine(draw_ranking):
            sys.exit("Fatal: cannot draw NP ranking only...")

    
    else: #ranking + pulls

        if options.np_merge :
            merged = ".x NLLProfilePlotter/MergeFiles.C(\"%s\")"%(options.np_merge)
            if ROOT.gROOT.ProcessLine(merged):
              sys.exit("Fatal: failed to merge NP files...") 
            
        if not options.np_pulls:
            sys.exit("Fatal: input NP pulls file is not loaded correctly -- an NP pulls file is required...")

        if not options.np_ranking:
            sys.exit("Fatal: input NP ranking file is not loaded correctly -- an NP ranking file is required...")

        inter_np_ranking_file = "inter_np_ranking_file.txt"
        matchpulls = ".x NLLProfilePlotter/MatchPullsAndRanking.C(\"%s\", \"%s\", \"%s\")"%(options.np_ranking, 
                                                                                            options.np_pulls, 
                                                                                            inter_np_ranking_file)

        drawpulls = ".x NLLProfilePlotter/PlotNPRankingAndPulls.C(\"%s\", %i, \"%s\", \"%s\")"%(inter_np_ranking_file, 
                                                                                                options.np_number,
                                                                                                options.output_file,
                                                                                                options.norm_factors_prefix)
        if ROOT.gROOT.ProcessLine(matchpulls):
            sys.exit("Fatal: cannot match NP pulls to NP rankings...")

        if ROOT.gROOT.ProcessLine(drawpulls):
            sys.exit("Fatal: cannot draw NP pulls to NP rankings...")

        if os.path.isfile(inter_np_ranking_file):
            call(['rm', '-f', inter_np_ranking_file], shell=False)

# run main 
if __name__ == "__main__":

    main(sys.argv[1:])

