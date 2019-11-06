#! /usr/bin/env python

import ROOT
##
#@class Standards
#
class Standards(object):
    """
    Class to define the standard properties of a job
    """
    def __init__(self, 
                 lumi         = 0.,
                 analysis     = '',
                 signals      = [],
                 cosmetics    = [],
                 signal_label = '',
                 join_signals = False,
                 stats        = False,
                 purity       = False
                 ):
        self.__lumi         = lumi
        self.__analysis     = analysis
        self.__signals      = signals
        self.__cosmetics    = cosmetics
        self.__signal_label = signal_label
        self.__join_signals = join_signals
        self.__stats        = stats
        self.__purity       = purity

    @property
    def luminosity(self):
        return self.__lumi

    @property
    def analysis(self):
        return self.__analysis

    @property
    def signals(self):
        return self.__signals

    @property
    def cosmetics(self):
        return self.__cosmetics

    @property
    def signal_label(self):
        return self.__signal_label

    @property
    def join_signals(self):
        return self.__join_signals

    @property
    def statistics(self):
        return self.__stats

    @property
    def purity(self):
        return self.__purity


##
#@class AnalysisSetup
#
class AnalysisSetup(object):
    """
    Class to define the analysis' specifics
    """
    def __init__(self, 
                 standards = None,
                 observables = []
                 ):
        try:
            self.__standards   = standards
            self.__observables = observables
        except RuntimeError:
            print "Error: initalization failure of analysis setup..."

    @property
    def standards(self):
        return self.__standards

    @property
    def observables(self):
        return self.__observables

    @property
    def analysis(self):
        return self.__standards.analysis

    @property
    def signals(self):
        return self.__standards.signals

    @property
    def luminosity(self):
        return self.__standards.luminosity

    @property
    def cosmetics(self):
        return self.__standards.cosmetics

    @property
    def signal_label(self):
        return self.__standards.signal_label

    @property
    def join_signals(self):
        return self.__standards.join_signals

    @property
    def statistics(self):
        return self.__standards.statistics

    @property
    def purity(self):
        return self.__standards.purity

##
# @class Observable
#
class Observable(object):
    """
    Class to describe observables
    """
    def __init__(self,
                 observable = '',
                 filters = [],
                 xaxis_title = '',
                 yaxis_log = False
                 ):
        self.__observable  = observable
        self.__filters     = filters
        self.__xaxis_title = xaxis_title
        self.__yaxis_log   = yaxis_log
    
    def __format_xtitle(self):
        return self.__xaxis_title\
            .replace("#", "")\
            .replace(" ", "")\
            .replace("^", "")\
            .replace("(", "")\
            .replace(")", "")\
            .replace("{", "")\
            .replace("}", "")\
            .replace("[", "")\
            .replace("]", "")

    @property
    def observable(self):
        return self.__observable

    @property
    def filters(self):
        return self.__filters 

    @property
    def xaxis_title(self):
        return self.__xaxis_title

    @property
    def yaxis_log(self):
        return self.__yaxis_log 

    @property
    def observable_plain_string(self):
        return self.__observable \
            + "_" \
            + "_".join(str(x) for x in self.__filters) \
            + "_" \
            + self.__format_xtitle()

##
# @class Cosmetic
#
class Cosmetic(object):
    """
    Class to define the cosmetics for each sample in a plot
    """
    def __init__(self,
                 sample_key   = "",
                 sample_label = "",
                 fill_color   = 0,
                 line_color   = 0,
                 line_width   = 0,
                 marker_type  = 0,
                 ):
        self.__sample_key   = sample_key
        self.__sample_label = sample_label
        self.__fill_color   = fill_color
        self.__line_color   = line_color
        self.__line_width   = line_width
        self.__marker_type  = marker_type

    
    @property
    def sample_key(self):
        return self.__sample_key

    @property
    def sample_label(self):
        return self.__sample_label

    @property
    def fill_color(self):
        return self.__fill_color

    @property
    def line_color(self):
        return self.__line_color

    @property
    def line_width(self):
        return self.__line_width

    @property
    def marker_type(self):
        return self.__marker_type

##
# @class Cosmetic
#
class Purity(object):
    """
    Class to define the signal purity details
    """
    def __init__(self,
                 show   = False,
                 line_color = ROOT.kGray,
                 yaxis_x = 0,
                 yaxis_y_low = 0,
                 yaxis_y_up = 0,
                 yaxis_y_auto = True
                 ):
        self.__show              = show
        self.__line_color        = line_color
        self.__yaxis_x           = yaxis_x
        self.__yaxis_y_low       = yaxis_y_low
        self.__yaxis_y_up        = yaxis_y_up
        self.__set_auto_purity_y = yaxis_y_auto
    @property
    def show(self):
        return self.__show

    @property
    def line_color(self):
        return self.__line_color

    @property
    def yaxis_x(self):
        return self.__yaxis_x

    @property
    def yaxis_y_low(self):
        return self.__yaxis_y_low

    @property
    def yaxis_y_up(self):
        return self.__yaxis_y_up

    @property
    def set_auto_purity_y(self):
        return self.__set_auto_purity_y
