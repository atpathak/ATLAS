from QFramework import *
from ROOT import *

def addObservables(config):

    filepath = "input/pdf_xlg.root"
    myMXLG = MXLG("MXLG",filepath)
    if not TQTreeObservable.addObservable(myMXLG):
        INFO("failed to add myMXLG observable")
        return False

    return True

