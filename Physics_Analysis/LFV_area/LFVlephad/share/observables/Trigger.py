from QFramework import *
from ROOT import *

def addObservables():
    myTrigger = Trigger("Trigger")
    if not TQTreeObservable.addObservable(myTrigger):
        INFO("failed to add myTrigger observable")
        return False

    return True
