# this is an automatically generated file
# sanitize shell output 
try:
  from os import environ
  if environ.get('TERM','') == 'xterm':
    environ['TERM'] = 'vt100'
except ImportError:
  pass
except KeyError:
  pass

# filter out annoying warnings regarding missing converters
import warnings
warnings.filterwarnings( action='ignore', category=RuntimeWarning, message='creating converter.*' )

# import basics
from os import getenv
from imp import load_source
from distutils.version import StrictVersion
from platform import python_version

try:
  import ROOT
  # check if the python version is compatible
  if StrictVersion(python_version()) < StrictVersion('2.7.0'):
    raise ImportError("\033[1;31mFATAL\033[0m: unsupported python version, please use at least version 2.7.0")

  # we will need this library later
  ROOT.gSystem.Load("libTreePlayer")

  # retrieve the root core dir environment variable
  RootCoreDir = getenv ("ROOTCOREDIR")
  try:
    t = ROOT.TQTaggable()
  except:
      if RootCoreDir:
        ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
      else:
        ROOT.gSystem.Load("$TQPATH/lib/libQFramework.so")
        ROOT.gSystem.Load("libRooFit")
        ROOT.gSystem.Load("$TQPATH/../SFramework/lib/libSFramework.so")

  # define some print commands that are available as preprocessor macros in the library
  def BREAK(arg):
    ROOT.TQLibrary.msgStream.sendMessage(ROOT.TQMessageStream.BREAK,arg)
      
  def ERROR(arg):
    ROOT.TQLibrary.msgStream.sendMessage(ROOT.TQMessageStream.ERROR,arg)

  def CRITICAL(arg):
    ROOT.TQLibrary.msgStream.sendMessage(ROOT.TQMessageStream.CRITICAL,arg)
    
  def INFO(arg):
    ROOT.TQLibrary.msgStream.sendMessage(ROOT.TQMessageStream.INFO,arg)
  
  def WARN(arg):
    ROOT.TQLibrary.msgStream.sendMessage(ROOT.TQMessageStream.WARNING,arg)
  
  def START(align,msg):
    ROOT.TQLibrary.msgStream.startProcessInfo(ROOT.TQMessageStream.INFO,min(ROOT.TQLibrary.getConsoleWidth(),120),align,msg)
  
  def END(result):
    ROOT.TQLibrary.msgStream.endProcessInfo(result)

  # provide sensible pretty-printing functionality for the basic classes
  def TQTaggable__repr__(self):
    return "{:s}(\"{:s}\")".format(self.Class().GetName(),self.exportTagsAsString().Data())
  ROOT.TQTaggable.__repr__ = TQTaggable__repr__

  def TQTaggable__str__(self):
    return self.exportTagsAsString().Data()
  ROOT.TQTaggable.__str__ = TQTaggable__str__

  def TQFolder__repr__(self):
    return "{:s}(\"{:s}\") @ {:s}:{:s} {:s}".format(self.Class().GetName(),self.GetName(),self.getRoot().GetName(),self.getPath().Data(),self.exportTagsAsString().Data())
  ROOT.TQFolder.__repr__ = TQFolder__repr__

  def TQFolder__str__(self):
    return "{:s}:{:s} {:s}".format(ROOT.TQStringUtils.makeBoldBlue(self.getRoot().GetName()).Data(),ROOT.TQStringUtils.makeBoldWhite(self.getPath()).Data(),self.exportTagsAsString().Data())
  ROOT.TQFolder.__str__ = TQFolder__str__

  # load libxml2, which is required for MVA stuff to work
  try:
    from ctypes.util import find_library
    libxmlpath = ROOT.TQLibrary.getlibXMLpath()
    ROOT.gSystem.Load(libxmlpath.Data())
  except Exception as ex:
    template = "unable to load libxml2 - an exception of type '{0}' occured: {1!s}"
    message = template.format(type(ex).__name__, ",".join(ex.args))
    WARN(message)

except ImportError as err:
  print(err)

try:
  ROOT.TString.__format__
except:
  def TString__format__(self,format_spec):
    s = str(self.Data())
    return s.__format__(format_spec)
  ROOT.TString.__format__ = TString__format__
  
def load_xAOD_packages():
  # this will load all RootCore packages needed to read the xAOD EDM
  RootCoreBin = getenv ("ROOTCOREBIN")
  RootCoreArch = getenv("ROOTCORECONFIG")
  if not RootCoreBin or not RootCoreArch:
    CRITICAL("unable to load xAOD packages: no RootCore available")
    return False
  pkgfiledef = RootCoreBin+"/load_packages_info_"+RootCoreArch
  ROOT.gSystem.Load("libReflex")
  ROOT.gSystem.Load("libCintex")
  ROOT.gSystem.Load("libCxxUtils")
  ROOT.gSystem.Load("libAthLinks")
  ROOT.gSystem.Load("libAthContainers")
  with open(pkgfiledef) as pkgfile:
    for line in pkgfile:
      if line.startswith("lib"):
        args = line.split("=")
        lib = args[1].strip()
        if "xAOD" in lib:
          try:
            ROOT.gSystem.Load(lib)
          except RuntimeError as err:
            WARN("unable to load library '"+lib+"'")
  if not ROOT.xAOD.Init().isSuccess():
    CRITICAL("failed to initialize xAOD reading!")
    return False
  return True


def TString__add__(self,other):
  return self.Data() + other
def TString__radd__(self,other):
  return other + self.Data()
def TString__bool__(self):
  return not self.IsNull()
ROOT.TString.__add__ = TString__add__
ROOT.TString.__radd__ = TString__radd__
ROOT.TString.__bool__ = TString__bool__
ROOT.TString.__nonzero__ = TString__bool__
TQABCDCalculator=ROOT.TQABCDCalculator
TQAlgorithm=ROOT.TQAlgorithm
TQAnalysisJob=ROOT.TQAnalysisJob
TQAnalysisSampleVisitorBase=ROOT.TQAnalysisSampleVisitorBase
TQAnalysisSampleVisitor=ROOT.TQAnalysisSampleVisitor
TQCompPlotter=ROOT.TQCompPlotter
TQConfigReader=ROOT.TQConfigReader
TQConstObservable=ROOT.TQConstObservable
TQCounterGrid=ROOT.TQCounterGrid
TQCounterGridNode=ROOT.TQCounterGridNode
TQCounterGridPoint=ROOT.TQCounterGridPoint
TQCounterGridVariable=ROOT.TQCounterGridVariable
TQCounter=ROOT.TQCounter
TQCutFactory=ROOT.TQCutFactory
TQCutflowAnalysisJob=ROOT.TQCutflowAnalysisJob
TQCutflowPlotter=ROOT.TQCutflowPlotter
TQCutflowPrinter=ROOT.TQCutflowPrinter
TQCut=ROOT.TQCut
TQEventIndexObservable=ROOT.TQEventIndexObservable
TQEventlistAnalysisJob=ROOT.TQEventlistAnalysisJob
TQEventlistPrinter=ROOT.TQEventlistPrinter
TQFilterObservable=ROOT.TQFilterObservable
TQFolder=ROOT.TQFolder
TQGraphMakerAnalysisJob=ROOT.TQGraphMakerAnalysisJob
TQGridScanAnalysisJob=ROOT.TQGridScanAnalysisJob
TQGridScanner=ROOT.TQGridScanner
TQGridScanPoint=ROOT.TQGridScanPoint
TQHistComparer=ROOT.TQHistComparer
TQHistoMakerAnalysisJob=ROOT.TQHistoMakerAnalysisJob
TQHWWPlotter=ROOT.TQHWWPlotter
TQImportLink=ROOT.TQImportLink
TQLibrary=ROOT.TQLibrary
TQLink=ROOT.TQLink
TQMessageStream=ROOT.TQMessageStream
TQMultiChannelAnalysisSampleVisitor=ROOT.TQMultiChannelAnalysisSampleVisitor
TQMultiObservable=ROOT.TQMultiObservable
TQMVA=ROOT.TQMVA
TQMVAObservable=ROOT.TQMVAObservable
TQNamedTaggable=ROOT.TQNamedTaggable
TQNFBase=ROOT.TQNFBase
TQNFCalculator=ROOT.TQNFCalculator
TQNFChainloader=ROOT.TQNFChainloader
TQNFCustomCalculator=ROOT.TQNFCustomCalculator
TQNFManualSetter=ROOT.TQNFManualSetter
TQNFTop0jetEstimator=ROOT.TQNFTop0jetEstimator
TQNFTop1jetEstimator=ROOT.TQNFTop1jetEstimator
TQNFUncertaintyScaler=ROOT.TQNFUncertaintyScaler
TQNTupleDumperAnalysisJob=ROOT.TQNTupleDumperAnalysisJob
TQObservable=ROOT.TQObservable
TQOptStringParser=ROOT.TQOptStringParser
TQPCAAnalysisJob=ROOT.TQPCAAnalysisJob
TQPCA=ROOT.TQPCA
TQPlotter=ROOT.TQPlotter
TQPresenter=ROOT.TQPresenter
TQSampleDataReader=ROOT.TQSampleDataReader
TQSampleFolder=ROOT.TQSampleFolder
TQSampleGroupingVisitor=ROOT.TQSampleGroupingVisitor
TQSample=ROOT.TQSample
TQSampleInitializerBase=ROOT.TQSampleInitializerBase
TQSampleInitializer=ROOT.TQSampleInitializer
TQSampleListInitializer=ROOT.TQSampleListInitializer
TQSampleNormalizationObservable=ROOT.TQSampleNormalizationObservable
TQSamplePurger=ROOT.TQSamplePurger
TQSampleRevisitor=ROOT.TQSampleRevisitor
TQSampleVisitor=ROOT.TQSampleVisitor
TQSignificanceEvaluator=ROOT.TQSignificanceEvaluator
TQSimpleSignificanceEvaluator=ROOT.TQSimpleSignificanceEvaluator
TQSimpleSignificanceEvaluator2=ROOT.TQSimpleSignificanceEvaluator2
TQSimpleSignificanceEvaluator3=ROOT.TQSimpleSignificanceEvaluator3
TQPoissonSignificanceEvaluator=ROOT.TQPoissonSignificanceEvaluator
TQSystematicsHandler=ROOT.TQSystematicsHandler
TQTable=ROOT.TQTable
TQTaggable=ROOT.TQTaggable
TQTHnBaseMakerAnalysisJob=ROOT.TQTHnBaseMakerAnalysisJob
TQToken=ROOT.TQToken
TQTreeFormulaObservable=ROOT.TQTreeFormulaObservable
TQTreeObservable=ROOT.TQTreeObservable
TQUniqueCut=ROOT.TQUniqueCut
TQValue=ROOT.TQValue
TQValueDouble=ROOT.TQValueDouble
TQValueInteger=ROOT.TQValueInteger
TQValueBool=ROOT.TQValueBool
TQValueString=ROOT.TQValueString
TQVectorAuxObservable=ROOT.TQVectorAuxObservable
TQXSecParser=ROOT.TQXSecParser
TQHistogramUtils=ROOT.TQHistogramUtils
TQListUtils=ROOT.TQListUtils
TQNTupleDumperAnalysisJobHelpers=ROOT.TQNTupleDumperAnalysisJobHelpers
TQObservableFactory=ROOT.TQObservableFactory
TQStringUtils=ROOT.TQStringUtils
TQTHnBaseUtils=ROOT.TQTHnBaseUtils
TQUtils=ROOT.TQUtils
