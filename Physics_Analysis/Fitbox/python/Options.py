import sys
import ROOT, argparse
from Verbose import *
#Root
class Root(object):
    """
    Root setup class
    """
    def __init__(self):
        pass

    def setup_all(self):
        ROOT.gROOT.SetBatch(True)    
    #ROOT.SetMemoryPolicy( ROOT.kMemoryHeuristics )
    #ROOT.gROOT.Macro("rootlogon.C")
    #ROOT.gROOT.SetStyle("ATLAS")

        return True

# options
class Options(object):
    """
    Options parser class
    """
    def __init__(self, prog=""):
        self.__parser = None
        self.__options = None
        self.__ready = self.parser(prog)

    def parser(self, prog = ""):

        self.__parser = argparse.ArgumentParser(description='Options for various programs"', 
                                                prog=prog)

        if prog == "plotter":

            self.__parser.add_argument('--input-path', 
                                       '-P' , 
                                       help='Input path containing the root file with raw RooFit plots', 
                                       required=True )
            self.__parser.add_argument('--input-file', 
                                       '-I' , 
                                       help='Input root file with raw RooFit plots', 
                                       default="plots_result.root")
            self.__parser.add_argument('--output-file', 
                                       '-O', 
                                       help='Output root file with formatted plots', 
                                       default="plots.root")
            self.__parser.add_argument('--config-file', 
                                       '-C',
                                       help='Analysis configuration file', 
                                       type=str, 
                                       required=True)
            
        elif prog == "NPranking":
            self.__parser.add_argument('--np-merge',
                                       '-Z',
                                       help='Input directory which containt text file for each NP',
                                      )

            self.__parser.add_argument('--np-ranking', 
                                       '-R' , 
                                       help='Input text file containing the NP ranking results.', 
                                       required=True )

            self.__parser.add_argument('--np-pulls', 
                                       '-P' , 
                                       help='Input text file containing the NP pull results.')

            self.__parser.add_argument('--np-number', 
                                       '-N' , 
                                       help='Number of nuisance parameter to be considered in the ranking.', 
                                       default = -1 )

            self.__parser.add_argument('--output-file',
                                       '-O',
                                       help='Name of output file containing the NP ranking plot.',
                                       default = 'np_ranking_plots' )

            self.__parser.add_argument('--np-output-dir', 
                                       '-D', 
                                       help='Output directory in which results will be stored.', 
                                       default="./results")

            self.__parser.add_argument( "--show-impact-only", 
                                        '-I',
                                        default=False, 
                                        action="store_true" , 
                                        help="Do not show pulls, only the post/pre-fit impact on the parameter")

            self.__parser.add_argument('--np-name',
                                       '-Q',
                                       help='Specify the name of the parameter for which the impact is calculated',
                                       default = '' )

            self.__parser.add_argument('--norm-factors-prefix',
                                       '-X',
                                       help='Specify the prefix of the NormFactor parameters in order to get highlighted',
                                       default = 'norm' )

            self.__parser.add_argument('--gamma-factors-prefix',
                                       '-G',
                                       help='Specify the prefix of the NormFactor parameters in order to get highlighted',
                                       default = 'gamma' ) 

        elif prog == "NPpulls":

            self.__parser.add_argument('--pulls-file', 
                                       '-F' ,
                                       required=True,
                                       help='Input text file containing the NP pull results.')

            self.__parser.add_argument('--pulls-outputdir', 
                                       '-D' , 
                                       default='./results/',
                                       help='Store plot pulls in this folder.')

            self.__parser.add_argument('--pulls-highlight', 
                                       '-H' , 
                                       default='',
                                       help='All NPs containing this key string will be highlighted.')

            self.__parser.add_argument('--pulls-min-value', 
                                       '-M' , 
                                       type=float,
                                       default=0.0,
                                       help='NPs will be plotted if their absolute pull value is higher than this minimum.')

            self.__parser.add_argument('--pulls-only-nf', 
                                       '-N' , 
                                       default=False,
                                       action="store_true",
                                       help='Only Normalization Factors (NF) will be plotted')

            self.__parser.add_argument('--pulls-sort-by-constraint', 
                                       '-T' , 
                                       default=False,
                                       action="store_true",
                                       help='NPs will be sorted by their constraint size.')

            self.__parser.add_argument('--pulls-no-sort', 
                                       '-S' , 
                                       default=False,
                                       action="store_true",
                                       help='NPs will be distributed randomly.')

            self.__parser.add_argument('--pulls-multiple-pois', 
                                       '-P' , 
                                       default=False,
                                       action="store_true",
                                       help='Plot NPs os a worskspace with multiple POIs.')


        elif prog == "NPcorrelations":

            self.__parser.add_argument('--correlations-file', 
                                       '-F' ,
                                       required=True,
                                       help='Input ROOT file containing the NP correlations matrix.')

            self.__parser.add_argument('--correlations-outputdir', 
                                       '-D' , 
                                       default='./results/',
                                       help='Store plot pulls in this folder.')

            self.__parser.add_argument('--correlations-histo', 
                                       '-H' , 
                                       default='',
                                       help='Name of a specific histogram to be considered.')


            self.__parser.add_argument('--correlations-min-value', 
                                       '-M' , 
                                       type=float,
                                       default=0.0,
                                       help='NPs will be plotted if their absolute correlation coefficient value is higher than this minimum.')

            self.__parser.add_argument('--correlations-title', 
                                       '-T' , 
                                       default='',
                                       help='A Title to be printed on the plot.')

            self.__parser.add_argument('--correlations-subtitle', 
                                       '-S' ,
                                       default='',
                                       help='A Subtitle to be printed on the plot.')

            self.__parser.add_argument('--correlations-print-for-np', 
                                       '-C' ,
                                       default='',
                                       help='Print correlation numbers for a given NP.')
        elif prog == "contour":

            self.__parser.add_argument('--input-path',
                                       '-P',
                                       help='Input path containing the files',
                                       required=True )

            self.__parser.add_argument('--expxval',
                                       '-X',
                                       type=float,
                                       default=0.0,
                                       help='expected value for the X parameter.')

            self.__parser.add_argument('--expyval',
                                       '-Y',
                                       type=float,
                                       default=0.0,
                                       help='expected value for the Y parameter.')
              
            self.__parser.add_argument('--xtitle',
                                       '-AX' ,
                                       default='',
                                       help='X axis title of the contour plot.') 

            self.__parser.add_argument('--ytitle',
                                       '-AY' ,
                                       default='',
                                       help='Y axis title of the contour plot.')


        else:
            msg.error("Options::parser", "Unknown parser options '%s'..."%(purpose))
            return False

        self.__options = self.__parser.parse_args()

        if not self.__options:
            self.__parser.print_help()    
            return False

        return True

    @property
    def instance(self):
        if self.__ready:
            return self.__options
        sys.exit("Error: Invalid options...")
