class bcolors:
    DEBUG     = '\033[94m'
    INFO      = '\033[92m'
    WARNING   = '\033[93m'
    ERROR     = '\033[91m'
    FATAL     = '\033[95m'
    ENDC      = '\033[0m'
    BOLD      = '\033[1m'
    UNDERLINE = '\033[4m'



class msg:
    """
    Class to handle the printout verbosity
    """
    def __init__(self):
        pass

    @classmethod
    def fatal(cls, where, what):
        print bcolors.FATAL + "Fatal!   " + bcolors.ENDC \
            + where + " : "+ what
    
    @classmethod
    def error(cls, where, what):
        print bcolors.ERROR + "Error!   " + bcolors.ENDC \
            + where + " : "+ what

    @classmethod
    def warning(cls, where, what):
        print bcolors.WARNING + "Warning! " + bcolors.ENDC \
            + where + " : "+ what

    @classmethod
    def info(cls, where, what):
        print bcolors.INFO + "Info!    " + bcolors.ENDC \
            + where + " : "+ what

    @classmethod
    def debug(cls, where, what):
        print bcolors.DEBUG + "Debug!   " + bcolors.ENDC \
            + where + " : "+ what
