from QFramework import *
from ROOT import *

def addObservables(config):

    ff_root_file = "fakefactors/FF_november.root"
    r_root_file = "fakefactors/R_november.root"

    mc = "" #TopMC, ZMC, WMC, has to be empty for standard Fakes!!!

    fakeSystematicsPath = config.getTagStringDefault("fakeSystematicsFile","False")
    fakeSystematics = []
    if str(fakeSystematicsPath).lower() != "false":
        with open(str(fakeSystematicsPath)) as fakeSystematicsFile:
            for line in fakeSystematicsFile:
                line = line.strip()
                if not line.startswith("#") and len(line) > 0:
                    fakeSystematics.append(line)
                    
    namesVBFCBSR = ["fakeFactorVBFCBSR", "fakeFactorVBFCBSR_nominal"]
    namesVBFCBSR += ["fakeFactorVBFCBSR_%s" % w for w in fakeSystematics]
    for name in namesVBFCBSR:
        myFakeFactorVBFCBSR = fakeFactor(name,ff_root_file,r_root_file,"VBFCB","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFCBSR):
            INFO("failed to add myFakeFactorVBFCBSR observable named as " + name)
            return False

    namesBOOSTCBSR = ["fakeFactorBOOSTCBSR", "fakeFactorBOOSTCBSR_nominal"]
    namesBOOSTCBSR += ["fakeFactorBOOSTCBSR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTCBSR:
        myFakeFactorBOOSTCBSR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTCB","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTCBSR):
            INFO("failed to add myFakeFactorBOOSTCBSR observable named as " + name)
            return False
 
    namesVBFCBTopCR = ["fakeFactorVBFCBTopCR", "fakeFactorVBFCBTopCR_nominal"]
    namesVBFCBTopCR += ["fakeFactorVBFCBTopCR_%s" % w for w in fakeSystematics]
    for name in namesVBFCBTopCR:
        myFakeFactorVBFCBTopCR = fakeFactor(name,ff_root_file,r_root_file,"VBFCB","TopCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFCBTopCR):
            INFO("failed to add myFakeFactorVBFCBTopCR observable named as " + name)
            return False

    namesBOOSTCBTopCR = ["fakeFactorBOOSTCBTopCR", "fakeFactorBOOSTCBTopCR_nominal"]
    namesBOOSTCBTopCR += ["fakeFactorBOOSTCBTopCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTCBTopCR:
        myFakeFactorBOOSTCBTopCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTCB","TopCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTCBTopCR):
            INFO("failed to add myFakeFactorBOOSTCBTopCR observable named as " + name)
            return False

    namesVBFCBWCR = ["fakeFactorVBFCBWCR", "fakeFactorVBFCBWCR_nominal"]
    namesVBFCBWCR += ["fakeFactorVBFCBWCR_%s" % w for w in fakeSystematics]
    for name in namesVBFCBWCR:
        myFakeFactorVBFCBWCR = fakeFactor(name,ff_root_file,r_root_file,"VBFCB","WCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFCBWCR):
            INFO("failed to add myFakeFactorVBFCBWCR observable named as " + name)
            return False

    namesBOOSTCBWCR = ["fakeFactorBOOSTCBWCR", "fakeFactorBOOSTCBWCR_nominal"]
    namesBOOSTCBWCR += ["fakeFactorBOOSTCBWCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTCBWCR:
        myFakeFactorBOOSTCBWCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTCB","WCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTCBWCR):
            INFO("failed to add myFakeFactorBOOSTCBWCR observable named as " + name)
            return False
 
    namesVBFCBZCR = ["fakeFactorVBFCBZCR", "fakeFactorVBFCBZCR_nominal"]
    namesVBFCBZCR += ["fakeFactorVBFCBZCR_%s" % w for w in fakeSystematics]
    for name in namesVBFCBZCR:
        myFakeFactorVBFCBZCR = fakeFactor(name,ff_root_file,r_root_file,"VBFCB","ZCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFCBZCR):
            INFO("failed to add myFakeFactorVBFCBZCR observable named as " + name)
            return False

    namesBOOSTCBZCR = ["fakeFactorBOOSTCBZCR", "fakeFactorBOOSTCBZCR_nominal"]
    namesBOOSTCBZCR += ["fakeFactorBOOSTCBZCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTCBZCR:
        myFakeFactorBOOSTCBZCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTCB","ZCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTCBZCR):
            INFO("failed to add myFakeFactorBOOSTCBZCR observable named as " + name)
            return False
 
    namesVBFCBQCDCR = ["fakeFactorVBFCBQCDCR", "fakeFactorVBFCBQCDCR_nominal"]
    namesVBFCBQCDCR += ["fakeFactorVBFCBQCDCR_%s" % w for w in fakeSystematics]
    for name in namesVBFCBQCDCR:
        myFakeFactorVBFCBQCDCR = fakeFactor(name,ff_root_file,r_root_file,"VBFCB","QCDCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFCBQCDCR):
            INFO("failed to add myFakeFactorVBFCBQCDCR observable named as " + name)
            return False

    namesBOOSTCBQCDCR = ["fakeFactorBOOSTCBQCDCR", "fakeFactorBOOSTCBQCDCR_nominal"]
    namesBOOSTCBQCDCR += ["fakeFactorBOOSTCBQCDCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTCBQCDCR:
        myFakeFactorBOOSTCBQCDCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTCB","QCDCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTCBQCDCR):
            INFO("failed to add myFakeFactorBOOSTCBQCDCR observable named as " + name)
            return False

    namesVBFMVASR = ["fakeFactorVBFMVASR", "fakeFactorVBFMVASR_nominal"]
    namesVBFMVASR += ["fakeFactorVBFMVASR_%s" % w for w in fakeSystematics]
    for name in namesVBFMVASR:
        myFakeFactorVBFMVASR = fakeFactor(name,ff_root_file,r_root_file,"VBFMVA","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFMVASR):
            INFO("failed to add myFakeFactorVBFMVASR observable named as " + name)
            return False

    namesBOOSTMVASR = ["fakeFactorBOOSTMVASR", "fakeFactorBOOSTMVASR_nominal"]
    namesBOOSTMVASR += ["fakeFactorBOOSTMVASR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTMVASR:
        myFakeFactorBOOSTMVASR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTMVA","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTMVASR):
            INFO("failed to add myFakeFactorBOOSTMVASR observable named as " + name)
            return False
 
    namesVBFMVATopCR = ["fakeFactorVBFMVATopCR", "fakeFactorVBFMVATopCR_nominal"]
    namesVBFMVATopCR += ["fakeFactorVBFMVATopCR_%s" % w for w in fakeSystematics]
    for name in namesVBFMVATopCR:
        myFakeFactorVBFMVATopCR = fakeFactor(name,ff_root_file,r_root_file,"VBFMVA","TopCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFMVATopCR):
            INFO("failed to add myFakeFactorVBFMVATopCR observable named as " + name)
            return False

    namesBOOSTMVATopCR = ["fakeFactorBOOSTMVATopCR", "fakeFactorBOOSTMVATopCR_nominal"]
    namesBOOSTMVATopCR += ["fakeFactorBOOSTMVATopCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTMVATopCR:
        myFakeFactorBOOSTMVATopCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTMVA","TopCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTMVATopCR):
            INFO("failed to add myFakeFactorBOOSTMVATopCR observable named as " + name)
            return False

    namesVBFMVAWCR = ["fakeFactorVBFMVAWCR", "fakeFactorVBFMVAWCR_nominal"]
    namesVBFMVAWCR += ["fakeFactorVBFMVAWCR_%s" % w for w in fakeSystematics]
    for name in namesVBFMVAWCR:
        myFakeFactorVBFMVAWCR = fakeFactor(name,ff_root_file,r_root_file,"VBFMVA","WCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFMVAWCR):
            INFO("failed to add myFakeFactorVBFMVAWCR observable named as " + name)
            return False

    namesBOOSTMVAWCR = ["fakeFactorBOOSTMVAWCR", "fakeFactorBOOSTMVAWCR_nominal"]
    namesBOOSTMVAWCR += ["fakeFactorBOOSTMVAWCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTMVAWCR:
        myFakeFactorBOOSTMVAWCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTMVA","WCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTMVAWCR):
            INFO("failed to add myFakeFactorBOOSTMVAWCR observable named as " + name)
            return False
 
    namesVBFMVAZCR = ["fakeFactorVBFMVAZCR", "fakeFactorVBFMVAZCR_nominal"]
    namesVBFMVAZCR += ["fakeFactorVBFMVAZCR_%s" % w for w in fakeSystematics]
    for name in namesVBFMVAZCR:
        myFakeFactorVBFMVAZCR = fakeFactor(name,ff_root_file,r_root_file,"VBFMVA","ZCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFMVAZCR):
            INFO("failed to add myFakeFactorVBFMVAZCR observable named as " + name)
            return False

    namesBOOSTMVAZCR = ["fakeFactorBOOSTMVAZCR", "fakeFactorBOOSTMVAZCR_nominal"]
    namesBOOSTMVAZCR += ["fakeFactorBOOSTMVAZCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTMVAZCR:
        myFakeFactorBOOSTMVAZCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTMVA","ZCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTMVAZCR):
            INFO("failed to add myFakeFactorBOOSTMVAZCR observable named as " + name)
            return False
 
    namesVBFMVAQCDCR = ["fakeFactorVBFMVAQCDCR", "fakeFactorVBFMVAQCDCR_nominal"]
    namesVBFMVAQCDCR += ["fakeFactorVBFMVAQCDCR_%s" % w for w in fakeSystematics]
    for name in namesVBFMVAQCDCR:
        myFakeFactorVBFMVAQCDCR = fakeFactor(name,ff_root_file,r_root_file,"VBFMVA","QCDCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorVBFMVAQCDCR):
            INFO("failed to add myFakeFactorVBFMVAQCDCR observable named as " + name)
            return False

    namesBOOSTMVAQCDCR = ["fakeFactorBOOSTMVAQCDCR", "fakeFactorBOOSTMVAQCDCR_nominal"]
    namesBOOSTMVAQCDCR += ["fakeFactorBOOSTMVAQCDCR_%s" % w for w in fakeSystematics]
    for name in namesBOOSTMVAQCDCR:
        myFakeFactorBOOSTMVAQCDCR = fakeFactor(name,ff_root_file,r_root_file,"BOOSTMVA","QCDCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorBOOSTMVAQCDCR):
            INFO("failed to add myFakeFactorBOOSTMVAQCDCR observable named as " + name)
            return False
 
    namesPreselectionSR = ["fakeFactorPreselectionSR", "fakeFactorPreselectionSR_nominal"]
    namesPreselectionSR += ["fakeFactorPreselectionSR_%s" % w for w in fakeSystematics]
    for name in namesPreselectionSR:
        myFakeFactorPreselectionSR = fakeFactor(name,ff_root_file,r_root_file,"Preselection","SR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionSR):
            INFO("failed to add myFakeFactorPreselectionSR observable named as " + name)
            return False

    namesPreselectionQCDCR = ["fakeFactorPreselectionQCDCR", "fakeFactorPreselectionQCDCR_nominal"]
    namesPreselectionQCDCR += ["fakeFactorPreselectionQCDCR_%s" % w for w in fakeSystematics]
    for name in namesPreselectionQCDCR:
        myFakeFactorPreselectionQCDCR = fakeFactor(name,ff_root_file,r_root_file,"Preselection","QCDCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionQCDCR):
            INFO("failed to add myFakeFactorPreselectionQCDCR observable named as " + name)
            return False

    namesPreselectionTopCR = ["fakeFactorPreselectionTopCR", "fakeFactorPreselectionTopCR_nominal"]
    namesPreselectionTopCR += ["fakeFactorPreselectionTopCR_%s" % w for w in fakeSystematics]
    for name in namesPreselectionTopCR:
        myFakeFactorPreselectionTopCR = fakeFactor(name,ff_root_file,r_root_file,"Preselection","TopCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionTopCR):
            INFO("failed to add myFakeFactorPreselectionTopCR observable named as " + name)
            return False

    namesPreselectionWCR = ["fakeFactorPreselectionWCR", "fakeFactorPreselectionWCR_nominal"]
    namesPreselectionWCR += ["fakeFactorPreselectionWCR_%s" % w for w in fakeSystematics]
    for name in namesPreselectionWCR:
        myFakeFactorPreselectionWCR = fakeFactor(name,ff_root_file,r_root_file,"Preselection","WCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionWCR):
            INFO("failed to add myFakeFactorPreselectionWCR observable named as " + name)
            return False
        
    namesPreselectionZCR = ["fakeFactorPreselectionZCR", "fakeFactorPreselectionZCR_nominal"]
    namesPreselectionZCR += ["fakeFactorPreselectionZCR_%s" % w for w in fakeSystematics]
    for name in namesPreselectionZCR:
        myFakeFactorPreselectionZCR = fakeFactor(name,ff_root_file,r_root_file,"Preselection","ZCR",mc)
        if not TQTreeObservable.addObservable(myFakeFactorPreselectionZCR):
            INFO("failed to add myFakeFactorPreselectionZCR observable named as " + name)
            return False

    return True
