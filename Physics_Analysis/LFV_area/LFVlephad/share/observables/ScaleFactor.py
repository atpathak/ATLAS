from QFramework import *
from ROOT import *

def addObservables(config):

  weightSystematicsPath = config.getTagStringDefault("weightSystematicsFile","False")
  weightSystematics = []
  if str(weightSystematicsPath).lower() != "false":
    with open(str(weightSystematicsPath)) as weightSystematicsFile:
      for line in weightSystematicsFile:
        line = line.strip()
        if not line.startswith("#") and len(line) > 0:
          weightSystematics.append(line)

  names = ["ScaleFactor", "ScaleFactor_nominal"]
  names += ["ScaleFactor_%s" % w for w in weightSystematics]
  for name in names:
    myScaleFactor = ScaleFactor(name)
    if not TQTreeObservable.addObservable(myScaleFactor):
        INFO("failed to add myScaleFactor observable named as " + name)
        return False

  return True
