from QFramework import *
from ROOT import *

def addObservables(config):

    ff_root_file = "fakefactors/FF_fake.root"
    r_root_file = "fakefactors/R_fake.root"

    mc = "" #TopMC, ZMC, WMC, has to be empty for standard Fakes!!!

    fakeSystematicsPath = config.getTagStringDefault("fakeSystematicsFile","False")
    fakeSystematics = []
    if str(fakeSystematicsPath).lower() != "false":
        with open(str(fakeSystematicsPath)) as fakeSystematicsFile:
            for line in fakeSystematicsFile:
                line = line.strip()
                if not line.startswith("#") and len(line) > 0:
                    fakeSystematics.append(line)
                    
    namesPreselectionSR = ["fakeFactorPreselectionSR", "fakeFactorPreselectionSR_nominal"]
    namesPreselectionSR += ["fakeFactorPreselectionSR_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSR:
        myFakeFactorPreselectionSR = fakeFactor(name,ff_root_file,r_root_file,"Preselection","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSR):
            INFO("failed to add myFakeFactorPreselectionSR observable named as " + name)
            return False

    namesPreselectionSR0 = ["fakeFactorPreselectionSR0", "fakeFactorPreselectionSR0_nominal"]
    namesPreselectionSR0 += ["fakeFactorPreselectionSR0_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSR0:
        myFakeFactorPreselectionSR0 = fakeFactor(name,ff_root_file,r_root_file,"SR0","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSR0):
            INFO("failed to add myFakeFactorPreselectionSR observable named as " + name)
            return False

    namesPreselectionSR1 = ["fakeFactorPreselectionSR1", "fakeFactorPreselectionSR1_nominal"]
    namesPreselectionSR1 += ["fakeFactorPreselectionSR1_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSR1:
        myFakeFactorPreselectionSR1 = fakeFactor(name,ff_root_file,r_root_file,"SR1","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSR1):
            INFO("failed to add myFakeFactorPreselectionSR observable named as " + name)
            return False

    namesPreselectionSR2 = ["fakeFactorPreselectionSR2", "fakeFactorPreselectionSR2_nominal"]
    namesPreselectionSR2 += ["fakeFactorPreselectionSR2_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSR2:
        myFakeFactorPreselectionSR2 = fakeFactor(name,ff_root_file,r_root_file,"SR2","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSR2):
            INFO("failed to add myFakeFactorPreselectionSR observable named as " + name)
            return False

    namesPreselectionSR3 = ["fakeFactorPreselectionSR3", "fakeFactorPreselectionSR3_nominal"]
    namesPreselectionSR3 += ["fakeFactorPreselectionSR3_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSR3:
        myFakeFactorPreselectionSR3 = fakeFactor(name,ff_root_file,r_root_file,"SR3","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSR3):
            INFO("failed to add myFakeFactorPreselectionSR observable named as " + name)
            return False

    namesPreselectionSRVBF = ["fakeFactorPreselectionSRVBF", "fakeFactorPreselectionSRVBF_nominal"]
    namesPreselectionSRVBF += ["fakeFactorPreselectionSRVBF_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSRVBF:
        myFakeFactorPreselectionSRVBF = fakeFactor(name,ff_root_file,r_root_file,"SRVBF","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSRVBF):
            INFO("failed to add myFakeFactorPreselectionSR observable named as " + name)
            return False

    namesPreselectionSRQCD = ["fakeFactorPreselectionSRQCD", "fakeFactorPreselectionSRQCD_nominal"]
    namesPreselectionSRQCD += ["fakeFactorPreselectionSRQCD_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSRQCD:
        myFakeFactorPreselectionSRQCD = fakeFactor(name,ff_root_file,r_root_file,"SRQCD","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSRQCD):
            INFO("failed to add myFakeFactorPreselectionQCDCR observable named as " + name)
            return False

    namesPreselectionSRQCD2 = ["fakeFactorPreselectionSRQCD2", "fakeFactorPreselectionSRQCD2_nominal"]
    namesPreselectionSRQCD2 += ["fakeFactorPreselectionSRQCD2_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSRQCD2:
        myFakeFactorPreselectionSRQCD2 = fakeFactor(name,ff_root_file,r_root_file,"SRQCD2","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSRQCD2):
            INFO("failed to add myFakeFactorPreselectionQCDCR observable named as " + name)
            return False

    namesPreselectionSRTop = ["fakeFactorPreselectionSRTop", "fakeFactorPreselectionSRTop_nominal"]
    namesPreselectionSRTop += ["fakeFactorPreselectionSRTop_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSRTop:
        myFakeFactorPreselectionSRTop = fakeFactor(name,ff_root_file,r_root_file,"SRTop","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSRTop):
            INFO("failed to add myFakeFactorPreselectionTopCR observable named as " + name)
            return False

    namesPreselectionSRW = ["fakeFactorPreselectionSRW", "fakeFactorPreselectionSRW_nominal"]
    namesPreselectionSRW += ["fakeFactorPreselectionSRW_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSRW:
        myFakeFactorPreselectionSRW = fakeFactor(name,ff_root_file,r_root_file,"SRW","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSRW):
            INFO("failed to add myFakeFactorPreselectionWCR observable named as " + name)
            return False
        
    namesPreselectionSRZtt = ["fakeFactorPreselectionSRZtt", "fakeFactorPreselectionSRZtt_nominal"]
    namesPreselectionSRZtt += ["fakeFactorPreselectionSRZtt_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSRZtt:
        myFakeFactorPreselectionSRZtt = fakeFactor(name,ff_root_file,r_root_file,"SRZtt","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSRZtt):
            INFO("failed to add myFakeFactorPreselectionZCR observable named as " + name)
            return False

    return True
