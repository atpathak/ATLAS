from QFramework import *
from ROOT import *

def addObservables(config):

    ff_root_file = "fakefactors/FF_electron.root"

    mc = "" #TopMC, ZMC, WMC, has to be empty for standard Fakes!!!

    eFakeSystematicsPath = config.getTagStringDefault("eFakeSystematicsFile","False")
    eFakeSystematics = []
    if str(eFakeSystematicsPath).lower() != "false":
        with open(str(eFakeSystematicsPath)) as eFakeSystematicsFile:
            for line in eFakeSystematicsFile:
                line = line.strip()
                if not line.startswith("#") and len(line) > 0:
                    eFakeSystematics.append(line)
                    
    namesPreselection = ["eFakeFactorPreselection", "eFakeFactorPreselection_nominal"]
    namesPreselection += ["eFakeFactorPreselection_%s" % w for w in eFakeSystematics]
    for name in namesPreselection:
        myFakeFactorPreselection = eFakeFactor(name,ff_root_file)
        if not TQTreeObservable.addObservable(myFakeFactorPreselection):
            INFO("failed to add myFakeFactorPreselection observable named as " + name)
            return False

    return True

