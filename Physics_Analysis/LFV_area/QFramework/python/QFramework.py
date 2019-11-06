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
