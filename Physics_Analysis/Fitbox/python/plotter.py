#!/usr/bin/python
##
# Program to create pre/post-fit distributions
# @author Z.Zinonos - zenon@lxplus.cern.ch
# @date January 2017
#
#
import ROOT
import sys, os, math, operator
from PlotUtils import *
from GraphUtils import *
from AnalysisSetupParser import *
from Verbose import *
from Yields import *
from Options import *

##
# @class Defaults
#
class Defaults(object):
    """
    Class providing the defaults.
    
    Supported Templates 
    - TES : postfit_Fake__obs_x_TES_Calo_ChannelMuHad_3P_InclEta_RegWjets
    """
    def __init__(self, standards = None ):
        #from config file
        self.__signals           = standards.signals
        self.__cosmetics         = standards.cosmetics
        self.__statistics	 = standards.statistics
        self.__purity		 = standards.purity
        #fixed for the time being
        self.__prefitname        = "prefit"
        self.__postfitname       = "postfit"
        self.__fittype_position  = 0
        self.__sample_position   = 1
        self.__data              = "Data"
        self.__error             = "FitError"
        self.__prefit_line_color = ROOT.kRed
        self.__prefit_line_width = 2


    def get_postfit_name(self):
        return self.__postfitname

    def get_prefit_name(self):
        return self.__prefitname

    def get_fittype_position(self):
        return self.__fittype_position
        
    def get_sample_position(self):
        return self.__sample_position

    def get_sample_label(self, sample):
        for c in self.__cosmetics:
            if c.sample_key == sample:
                return c.sample_label

        return ROOT.kBlack
    
    def get_fill_color(self, sample):
        for c in self.__cosmetics:
            if c.sample_key == sample:
                return c.fill_color

        return ROOT.kBlack

    def get_line_color(self, sample):
        for c in self.__cosmetics:
            if c.sample_key == sample:
                return c.line_color

        return ROOT.kBlack

    def get_prefit_line_color(self):
        return self.__prefit_line_color

    def get_prefit_line_width(self):
        return self.__prefit_line_width

    def get_line_width(self, sample):
        for c in self.__cosmetics:
            if c.sample_key == sample:
                return c.line_width

        return 1

    def get_marker_type(self, sample):
        for c in self.__cosmetics:
            if c.sample_key == sample:
                return c.marker_type

        return 1

    def get_signal_names(self):
        return self.__signals

    def get_error_name(self):
        return self.__error

    def get_data_name(self):
        return self.__data

    def name_rule(self, raw_name):
        return raw_name.replace("Visible_Data", self.__data).replace("obs_x_", "").replace("__","_")

    def show_statisics(self):
        return self.__statistics

##
# @ class Format
#
class Format(Defaults, object):
    def __init__(self, standards = None):
        """
         Histo Formatter
        """
        Defaults.__init__(self, standards = standards)
    
    def format_histo(self, histo = None, name = "", fit_type = ""):
        if not histo:
            msg.warning("Format::format_histo", "Null histo %s, cannot be formatted"%(name))
            return
        if fit_type == self.get_postfit_name():
            histo.SetFillColor( self.get_fill_color(name) )
            histo.SetLineColor( self.get_line_color(name) )
            histo.SetLineWidth( self.get_line_width(name) )
        elif fit_type == self.get_prefit_name():
            histo.SetLineColor( self.get_prefit_line_color() )
            histo.SetLineWidth( self.get_prefit_line_width() )
        else:
            msg.warning("Format::format_histo", "Histo with fit type '%s' cannot be formatted"%(fit_type))
        
    def format_graph(self, g, name):

        if name == self.get_data_name():
            g.SetMarkerStyle( self.get_marker_type(name) )
            g.SetMarkerColor( self.get_line_color(name) )
            g.SetLineColor( self.get_line_color(name) )
            g.SetLineWidth( self.get_line_width(name) )

        if name == self.get_error_name():
            g.SetFillColor( ROOT.kYellow + 1 )
            g.SetFillStyle( 3013 )

        if name == "data_rel_error":
            g.SetLineColor( ROOT.kGray+1 )
            g.SetFillColor( ROOT.kGray )

        if name == "ratio":
            g.SetLineColor( ROOT.kBlack )
            g.SetMarkerColor( ROOT.kBlack )
            g.SetMarkerStyle(4)
            g.SetMarkerSize(1)
            g.SetLineWidth(1)

        if name == "ratio_combined_error":
            g.SetLineColor( ROOT.kYellow + 1 )
            g.SetFillColor( ROOT.kYellow + 1 )
            g.SetFillStyle( 3013 )

##
# @class Histo
#
class Histo(Defaults, object):
    def __init__(self, standards = None, histo = None):
        """
        Histogram class with extended features
        """
        try:
            Defaults.__init__(self, standards = standards)
        except RuntimeError:
            print "Error: initalization failure of default class in Histo..."
        self.__histo     = histo
        self.__name      = histo.GetName()
        self.__new_name  = self.name_rule(histo.GetName())
        self.__tokens    = self.__new_name.split("_")
        self.__type      = self.__tokens[ self.get_fittype_position() ]
        self.__sample    = self.__tokens[ self.get_sample_position() ]
        self.__mine      = self.ownership()
           
    def get_sample_name(self):
        return self.__sample

    def get_histo(self):
        return self.__histo

    def get_integral(self):
        return self.__histo.Integral()

    def get_histo_name(self):
        return self.__new_name

    def is_signal(self):
        return self.__sample in self.get_signal_names()

    def is_error(self):
        return self.get_error_name() == self.__sample

    def is_data(self):
        return self.get_data_name() == self.__sample

    def is_postfit(self):
        return self.__type == self.get_postfit_name()

    def is_prefit(self):
        return self.__type == self.get_prefit_name()

    def is_fit_type(self, fit):
        if fit == self.get_prefit_name():
            return self.is_prefit()
        elif fit == self.get_postfit_name():
            return self.is_postfit()
        else:
            print "Warning: unknown fit type..."
            
        return False
    
    def is_sample_stack(self):
        return not ( self.is_data() or self.is_error() or self.is_signal())

    def is_prefit_stack(self):
        return self.is_prefit() and self.is_sample_stack()

    def is_postfit_stack(self):
        return self.is_postfit() and self.is_sample_stack()

    def is_stack(self, fit):
        if fit == self.get_prefit_name():
            return self.is_prefit_stack()
        elif fit == self.get_postfit_name():
            return self.is_postfit_stack()
        else:
            msg.warning("Histo::is_stack()", "Unknown fit type for stack '%s'..."%(fit) )
            
        return "unknown"

    def ownership(self):
        self.__histo.SetDirectory(0)
        return self.__histo.GetDirectory() == None

    def get_template(self, name):
        hempty = self.__histo.Clone(name)
        hempty.Sumw2(ROOT.kTRUE)
        hempty.Reset()
        hempty.SetTitle("")

        return hempty

    def get_histo_limits(self):
        n = self.__histo.GetNbinsX()
        return self.__histo.GetBinLowEdge(1), self.__histo.GetBinLowEdge(n) + self.__histo.GetBinWidth(n)

    def set_sqrtN_errors(self):
        for i in xrange(1, self.__histo.GetNbinsX()+1):
            bc = self.__histo.GetBinContent(i)
            self.__histo.SetBinError( i, ROOT.TMath.Sqrt(bc) if bc > 0. else 0. )

    def new_histo(self):
        n = self.__histo.GetNbinsX()
        x0 = self.__histo.GetBinLowEdge(1)
        x1 = self.__histo.GetBinLowEdge(n) + self.__histo.GetBinWidth(n) 
        h = ROOT.TH1F("copy_"+self.__histo.GetName(), "", n, x0, x1)
        for i in xrange(1, n+1):
            c = self.__histo.GetBinContent(i)
            h.SetBinContent(i, c)
            h.SetBinError(i, ROOT.TMath.Sqrt(c) )
        return h
        
class Stack(object):
    """
    Class Stack
    """
    def __init__(self, 
                 samples = [], 
                 has_signal = False,
                 name = "", 
                 template_histo = None):
        self.__samples    = samples
        self.__has_signal = has_signal
        self.__name       = name
        self.__stack      = template_histo
        self.__dict       = dict()
        self.__form       = self.form()

    def name(self):
        return self.__name

    def sample_histo_dict(self):
        return self.__dict

    def stack(self):
        return self.__stack

    def has_signal(self):
        return self.__has_signal

    def has_signal_str(self):
        return "True" if self.has_signal() else "False"

    def integral(self):
        return self.__stack.Integral()

    def integral_str(self):
        return str(self.integral())

    def samples_str(self):
        return " ".join(self.__samples)

    def form(self):
        self.__stack.SetDirectory(0)
        if not self.__stack.GetSumw2N():
            self.__stack.Sumw2()
        
    def add_to_stack(self, h):
        self.__stack.Add(h, 1.) #TH1 Add 

    def add_to_dict(self, sample, histo):
        if sample not in self.__dict:
            self.__dict[sample] = histo

    def add_if_owns_sample(self, sample = "", histo = None):
        if sample in self.__samples:
            self.add_to_stack(histo)
            self.add_to_dict(sample, histo)
            

    def get_graph(self):
        return ROOT.TGraphAsymmErrors(self.__stack)

    
    def show_samples(self, note=""):
        summary = note
        for sample, histo in self.__dict.items():
            summary += " " + sample + ":" + str(histo.Integral())

        msg.info("Stack::show_samples", summary)

##
# @class File
#
class File(object):
    """
    Class to handle the output file
    """
    def __init__(self, 
                 analysis   = '',
                 outputpath = '',
                 outputfile = ''):
        self.__outputfilename = os.path.join(outputpath, "%s_%s"%(analysis,outputfile))
        self.__outputfile     = self.__get_output_file()

    def __get_output_file(self):
        return ROOT.TFile(self.__outputfilename, "RECREATE")

    @property
    def close(self):
        if self.__outputfile.TestBit(ROOT.TFile.kRecovered) or self.__outputfile.IsZombie():
            print "Error: outputfile looks problematic..."
            return False
        msg.info("File::close", "Result is stored in '%s'"%( self.__outputfile.GetName() ) )
        self.__outputfile.Close();
        return True

    @property
    def cd(self):
        return self.__outputfile.cd();
        
    @property
    def instance(self):
        return self.__outputfile
        
##
# @class Elements
#
class Elements(Defaults, object):
    """
    Class Elements
    """
    def __init__(self,
                 inputpath  = '', 
                 inputfile  = '', 
                 output     = None,
                 standards  = None, 
                 observable = None,
                 purity     = None
                 ):
        try:
            Defaults.__init__(self, standards = standards)
        except RuntimeError:
            print "Error: cannot initialize defaults class..."       
        self.__outputpath     = inputpath
        self.__filename       = os.path.join(inputpath, inputfile)
        self.__output         = output 
        self.__standards      = standards
        self.__observable     = observable
        self.__purity	      = purity
        #
        self.__analysis       = standards.analysis
        self.__luminosity     = standards.luminosity
        self.__gu             = GraphUtils()
        self.__fm             = Format(standards)
        self.__histo_list     = []
        self.__analysis_ready = self.analyze_file()
        self.__visual_ready   = self.visualize()
        self.__final_ready    = self.finalize()

    def ready(self):
        return self.__final_ready
        
    @property 
    def gu(self):
        return self.__gu

    @property 
    def fm(self):
        return self.__fm

    def get_standards(self):
        return self.__standards
        
    def get_analysis(self):
        return self.__analysis
      
    def get_luminosity(self):
        return self.__luminosity

    def get_luminosity_str(self):
        return "%.2f"%(self.get_luminosity())

    def get_luminosity_legend(self):
        return "#splitline{Data #sqrt{s}=13TeV}{#scale[0.6]{#int}dt#color[0]{.}#font[52]{L}#color[0]{.}=#color[0]{.}"\
            +self.get_luminosity_str()+"#color[0]{.}fb^{-1}}"

    def get_observable_quantity(self):
        return self.__observable.observable

    def show_purity(self):
        return self.__purity.show

    def get_purity_x(self):
        return self.__purity.yaxis_x

    def get_purity_y_low(self):
        return self.__purity.yaxis_y_low

    def get_purity_y_up(self):
        return self.__purity.yaxis_y_up


    def set_auto_purity_y(self):
        return self.__purity.set_auto_purity_y

    def get_purity_color(self):
        return self.__purity.line_color

    def get_filters(self):
        return self.__observable.filters

    def get_xaxis_title(self):
        return self.__observable.xaxis_title

    def get_yaxis_log(self):
        return self.__observable.yaxis_log

    def get_yaxis_offset(self):
        return 2.00 if self.__observable.yaxis_log else 1.75

    def get_canvas_name(self):
        return self.__observable.observable_plain_string

    def cd_file(self):
        return self.__output.cd;

    def check_input(self):
        return os.path.isfile(self.__filename) 

    def open_file(self):
        return ROOT.TFile(self.__filename, "READ")

    def obj_is_1d_histogram(self, obj):
        return obj and  \
            issubclass(type(obj), ROOT.TObject) and \
            obj.InheritsFrom("TH1") and \
            ("TH1D" in obj.IsA().GetName() or "TH1F" in obj.IsA().GetName())

    def key_is_1d_histogram(self, key):
        return "TH1" in key.GetClassName()

    def key_pass_filters(self, keyname):
        if not keyname:
            return False
        for ifilter in self.get_filters():
            if ifilter not in keyname:
                return False
        return True

    def add_element(self, histo):
        
        if self.obj_is_1d_histogram(histo):
            self.__histo_list.append( Histo(self.get_standards(), histo) )
        else:
            print "Error: expecting a root histo object but receiced instead: "+type(histo)
            return False
        return True

    def postfit_error_histo(self):
        for h in self.__histo_list:
            if h.is_error() and h.is_postfit():
                return h
        return None

    def get_error_graph(self):
        return ROOT.TGraphAsymmErrors( self.postfit_error_histo().get_histo() )

    def prefit_data_histo(self, sqrtNerrors=False, newHisto=False):
        for h in self.__histo_list:
            if h.is_data() and h.is_prefit():                   
                if sqrtNerrors:
                    h.set_sqrtNerrors()
                    return h.get_histo()
                elif newHisto:
                    return h.new_histo()
                else:
                    return h.get_histo()
        return None

    def postfit_data_histo(self, sqrtNerrors=False, newHisto=False):
        for h in self.__histo_list:
            if h.is_data() and h.is_postfit():
                if sqrtNerrors:
                    h.set_sqrtN_errors()
                    return h.get_histo()
                elif newHisto:
                    return h.new_histo()
                else:
                    return h.get_histo()
        return None

    def get_chi2_option(self):
        return "UW"

    def get_chi2_info_to_postfit_data(self, weighted_histo):
        chi2 = ROOT.Double(0)
        ndf = ROOT.Long(0)
        igood = ROOT.Long(0)
        p = self.postfit_data_histo(newHisto=True).Chi2TestX(weighted_histo, 
                                                             chi2,
                                                             ndf,
                                                             igood,
                                                             self.get_chi2_option())
        
        return p, chi2, ndf, igood

    def get_chi2_info_to_prefit_data(self, weighted_histo):
        chi2 = ROOT.Double(0)
        ndf = ROOT.Long(0)
        igood = ROOT.Long(0)
        p = self.prefit_data_histo(newHisto=True).Chi2TestX(weighted_histo, 
                                                            chi2,
                                                            ndf,
                                                            igood,
                                                            self.get_chi2_option())
                                               
        return p, chi2, ndf, igood



    def get_prefit_chi2_latex_info(self, h = None):
        p, chi2, ndf, igood = self.get_chi2_info_to_prefit_data(h)
        return "Pre-fit   p = %.2f (#chi^{2}/ndf = %.2f/%i)"%(p, chi2, ndf)

    def get_postfit_chi2_latex_info(self, h = None):
        p, chi2, ndf, igood = self.get_chi2_info_to_postfit_data(h)
        return "Post-fit p = %.2f (#chi^{2}/ndf = %.2f/%i)"%(p, chi2, ndf)

    def get_chi2_latex_info(self, prefit = None, postfit = None):
        prefit_chi2_info = self.get_prefit_chi2_latex_info(prefit)
        postfit_chi2_info = self.get_postfit_chi2_latex_info(postfit)
        
        return "#splitline{%s}{%s}"%(prefit_chi2_info, postfit_chi2_info)


    def get_data_graph(self):        
        return self.gu.get_graph_with_poissonian_errors(self.postfit_data_histo())

    def get_maximum_data_bin_value(self):
        h = self.postfit_data_histo()
        return h.GetBinContent( h.GetMaximumBin() ) if h.GetNbinsX() else 0.

    def analyze_file(self):

        if not self.check_input():
            msg.error("Elements::analyze_file", "Input file %s is not valid..."%(self.__filename))
            return False
        
        rfile  = self.open_file()
        
        if rfile.IsZombie():
            msg.error("Elements::analyze_file", "File %s is zombie ..."%(rfile.GetName()) )
            return False

        for key in rfile.GetListOfKeys():
            if self.key_is_1d_histogram(key) and self.key_pass_filters(key.GetName()):
                H = rfile.Get(key.GetName())
                
                error = ROOT.Double()
                area = H.IntegralAndError(-1,-1,error,"")
                #print "TMP get: ", key.GetName(), H.Integral(), area, error

                if not self.add_element( H ):
                    msg.error("Elements::analyze_file", "Element %s can not be pushed to the list of elements..."%(key.GetName()) )

        if not self.__histo_list:
            msg.error("Elements::analyze_file", "No histograms found to be plotted for observable %s ..."%(self.__observable.observable_plain_string))
            return False
        
        return True

    def get_stack_keys(self, fit_type):
        #get all key samples
        keys = []
        for entry in self.__histo_list:
            if entry.is_stack(fit_type):
                msg.debug( "Elements::get_stack_keys",  "appending '%s' for '%s' stack..."%( entry.get_sample_name(), fit_type ) )
                keys.append( (entry.get_sample_name(), entry.get_integral()) )

        #sort samples by area
        keys.sort(key=operator.itemgetter(1))

        #create a big list with elements of list samples
        stack_keys = []
        for c, (sample, area)  in enumerate(keys):
            key_list = []
            key_list.append(sample)
            for ikey in xrange( c+1, len(keys)):
                if keys[ikey][0] != sample:
                    key_list.append( keys[ikey][0] )
            stack_keys.append(key_list)

        #finally, add the signals on top
        for signal in self.get_signal_names():
            stack_keys.insert(0,  [signal] + stack_keys[0] )

        return stack_keys

    def get_histo_stack_list(self, fit_type = ""):

        hstack = []
        for skeys in self.get_stack_keys(fit_type):
            name = skeys[0]

            hstack.append( Stack(samples = skeys, 
                                 has_signal = not set(skeys).isdisjoint(self.get_standards().signals),
                                 name = name,
                                 template_histo = self.__histo_list[0].get_template(name)) )

        for hs in hstack:
            msg.info("Elements::get_histo_stack_list", 
                     "before adding, type=" \
                         + fit_type + " name=" \
                         + hs.name() + " samples=" \
                         + hs.samples_str() + " area=" \
                         + hs.integral_str() + " has signal=" \
                         + hs.has_signal_str())

        for c, entry in enumerate(self.__histo_list):

            if not entry.is_fit_type(fit_type):
                continue

            sample = entry.get_sample_name()

            for stack in hstack:
                stack.add_if_owns_sample( sample = sample, 
                                          histo = entry.get_histo())

        if not hstack:
            msg.info("Elements::get_histo_stack_list",  "Returning an empty histo stack '%s' ..."%(fit_type))

        return hstack

    def get_postfit_histo_stack_list(self):
        return self.get_histo_stack_list(self.get_postfit_name())

    def get_prefit_histo_stack_list(self):
        return self.get_histo_stack_list(self.get_prefit_name())

    def get_signal_purity_graph(self, stacklist, axis1):
        #entry 0 = s+b
        #entry 1 = b
        h_s_plus_b = stacklist[0].stack()
        h_b = stacklist[1].stack()
        h_purity =  h_s_plus_b.Clone()
        h_purity.Add(h_b, -1)
        h_purity.Divide(h_s_plus_b)
        minY2 = h_purity.GetBinContent( h_purity.GetMinimumBin() )
        maxY2 = h_purity.GetBinContent( h_purity.GetMaximumBin() )
        scale = axis1 /maxY2
        for i in xrange( 1, h_purity.GetNbinsX() + 1 ):
            bc = h_purity.GetBinContent( i )
            h_purity.SetBinContent( i, scale * bc )

        minY1 = h_purity.GetBinContent( h_purity.GetMinimumBin() )
        g = ROOT.TGraph(h_purity)
        g.SetLineColor(self.get_purity_color())
        g.SetMarkerColor(self.get_purity_color())
        return minY1, minY2, maxY2, g
        
    def visualize(self):

        if not self.__analysis_ready:
            msg.warning("Elements::visualize", "histogram analysis is not ready - no plotting will happen...")
            return False
        
        #make stacks
        hstack_postfit = self.get_postfit_histo_stack_list()
        hstack_prefit = self.get_prefit_histo_stack_list()
                
        #prepate pads
        pu = PlotUtils()
        pu.prepare_2pads( self.get_canvas_name(), 900, 800, False)
        pu.make_legends_up()

        ## top pad
        pu.GoToTopPad()
        hpanel = self.__histo_list[0].get_template("panel")
        hpanel.SetLabelSize(0, "X")
        hpanel.Draw()
        if self.get_yaxis_log():
            ROOT.gPad.SetLogy()

        #plot
        legend_one_signal_entry_added = False
        for c, hs in enumerate(hstack_postfit):
            msg.info("Elements::visualize", 
                     "after stack "\
                         + str(c) + " name=" \
                         + hs.name() + " samples="\
                         + hs.samples_str() + " area=" \
                         + hs.integral_str() + " has signal="\
                         + hs.has_signal_str())

            self.fm.format_histo(histo = hs.stack(), 
                                 name = hs.name(), 
                                 fit_type = self.get_postfit_name())
            hs.stack().Draw("hist same")
            label =  self.get_sample_label( hs.name() )
            if hs.has_signal():
                if self.get_standards().join_signals:
                    if not legend_one_signal_entry_added:
                        pu.add_entry_legend_up(hs.stack(), self.get_standards().signal_label, "fs")
                        legend_one_signal_entry_added = True
                else:
                    pu.add_entry_legend_up(hs.stack(), label, "fs")
            else:
                pu.add_entry_legend_up(hs.stack(), label, "fs")

        #postfit
        hpostfit = hstack_postfit[0].stack()
        if not self.obj_is_1d_histogram(hpostfit):
            msg.error("Elements::visualize", "Stack is not a TH1 object...")

        #plot error
        gerror = self.get_error_graph()
        self.fm.format_graph(gerror, self.get_error_name())
        gerror.Draw("z2 same")

        #plot prefit
        hprefit = hstack_prefit[0].stack()
        if not self.obj_is_1d_histogram(hprefit):
            msg.error("Elements::visualize", "Stack is not a TH1 object...")
        self.fm.format_histo(histo = hprefit, 
                             name = hstack_prefit[0].name(), 
                             fit_type = self.get_prefit_name())
        hprefit.Draw("hist same")

        #data
        gdata = self.get_data_graph()
        self.fm.format_graph(gdata, self.get_data_name())
        gdata.Draw("epz same")
        maxdata = self.get_maximum_data_bin_value()

        #store yields
        hstack_prefit[0].show_samples("prefit")
        hstack_postfit[0].show_samples("postfit")

 #      postfit yields are now produced directly by the NLLProfileCreator
 #       yields = Yields(outdir = self.__outputpath, 
 #                       outfile = "yields_"+pu.get_canvas().GetName()+".txt")               
 #       yields.add_column( Column("Prefit", hstack_prefit[0].sample_histo_dict()) )
 #       yields.add_column( Column("Postfit", hstack_postfit[0].sample_histo_dict()) )
 #       yields.add_row(  Row("Data", gdata ) )
 #       yields.store()

        #adjust axis
        hpanel.SetYTitle("Events/%.2f"%(hpanel.GetBinWidth(1)))#supporting fixed bin widths atm
        hpanel.SetTitleOffset( hpanel.GetTitleOffset("Y")*1.2, "Y")
        hpanel.SetTitleSize( hpanel.GetTitleSize("Y")*1.1, "Y")
        hpanel.SetMaximum(maxdata*self.get_yaxis_offset())

        #stats info
        if self.show_statisics():
            chi2_info = self.get_chi2_latex_info(prefit = hprefit, postfit = hpostfit)
            pu.display_up(chi2_info)

        #second axis
        if self.show_purity():
            #position y up
            purity_left_axis_high = self.get_purity_y_up()

            if self.set_auto_purity_y():
                purity_left_axis_high = maxdata * 0.95

            #position x
            purity_right_axis_x_position =self.get_purity_x()

            purity_minimum_left, purity_minimum_right, purity_maximum_right, gsignalPurity = self.get_signal_purity_graph(hstack_postfit, 
                                                                                                                          purity_left_axis_high)
            #max purity
            purity_right_axis_high = purity_maximum_right

            #position y down
            purity_left_axis_low = self.get_purity_y_low()
            if self.set_auto_purity_y():
                purity_left_axis_low = maxdata * 0.5

            #min purity
            purity_right_axis_low = purity_minimum_right

            gsignalPurity.Draw("C same")
            pu.display_second_axis_up("Signal Purity", 
                                      purity_right_axis_x_position, 
                                      purity_left_axis_low,
                                      purity_right_axis_x_position,
                                      purity_left_axis_high,
                                      purity_right_axis_low,
                                      purity_right_axis_high)


        #update
        pu.UpdateTopPad()

        ## bottom pad
        pu.GoToBottomPad()
        hratio = self.__histo_list[0].get_template("ratio")
        hratio.SetMinimum(0.78)
        hratio.SetMaximum(1.22)
        hratio.SetLabelSize( hratio.GetLabelSize("Y")*2.0 , "Y")
        hratio.SetTitleSize( hratio.GetTitleSize("Y")*2.0 , "Y")
        hratio.SetTitleOffset( hratio.GetTitleOffset("Y")*0.6 , "Y")
        hratio.SetYTitle("Observed/Predicted")
        hratio.SetLabelSize( hratio.GetLabelSize("X")*2.0 , "X")
        hratio.SetTitleSize( hratio.GetTitleSize("X")*2.5 , "X")
        hratio.SetTitleOffset( hratio.GetTitleOffset("X")*1.05 , "X")
        hratio.SetXTitle(self.get_xaxis_title())
        hratio.Draw()

        #data rel stat error
        gdata_err = self.gu.get_data_relative_error_graph(gdata)
        self.fm.format_graph(gdata_err, "data_rel_error")
        gdata_err.Draw("Z 2 same")

        x1, x2 = self.__histo_list[0].get_histo_limits()
        line = ROOT.TLine(x1, 1., x2, 1.)
        line.SetLineColor(ROOT.kWhite)
        line.SetLineStyle(7)
        line.Draw()

        gPostFitStack = hstack_postfit[0].get_graph() #first element of the stack: the final expected distribution
        gPostfitRatioWithPostfitError = self.gu.get_graph_ratio_combined_error( gdata,
                                                                                gPostFitStack,                                                       
                                                                                gerror)

        #ratio postfit and error
        self.fm.format_graph( gPostfitRatioWithPostfitError, "ratio_combined_error")
        gPostfitRatioWithPostfitError.Draw("Z 2 same")

        gPostfitRatio = self.gu.get_graph_ratio( gdata,  gPostFitStack)
        self.fm.format_graph(gPostfitRatio, "ratio")
        gPostfitRatio.Draw("epz same")

        pu.UpdateBottomPad()

        #still legend up
        pu.GoToTopPad()

        pu.add_entry_legend_up(gdata_err, 
                               "Data Stat. Error", 
                               "fs", 
                               2)

        pu.add_entry_legend_up(gerror, 
                               "Post-fit error", 
                               "fs",
                               2)

        pu.add_entry_legend_up(hprefit, 
                               "Pre-fit", 
                               "l",
                               2)

        pu.add_entry_legend_up(gdata,
                               self.get_luminosity_legend(), 
                               "lp",
                               2)

        pu.draw_legends_up()

        #update
        pu.UpdateTopPad()

        #store
        if self.cd_file():
            msg.info( "Elements::visualize", pu.get_canvas().GetName() )
            pu.get_canvas().Write()
            pu.get_canvas().SaveAs("plots/"+pu.get_canvas().GetName()+".png")
        else:
            print "Warning: cannot change directory to output file..."

        return True

    def finalize(self):

        if not self.__visual_ready:
            msg.warning( "Elements::finalize", "histogram visualization is not ready - output file might be not closed properly" )
            return False
        return True
        print "Done!"
        
# main
def main(argv):

    root = Root()
    if not root.setup_all():
        sys.exit("Fatal: Exiting at SetRoot ...")

    options = Options("plotter").instance

    setup = AnalysisSetupParser(options.config_file).instance

    output = File(analysis = setup.analysis, 
                  outputpath = options.input_path,
                  outputfile = options.output_file)

    purity = setup.purity
    count_all = 0
    count_ready = 0
    for observable in setup.observables:
        count_all +=1
        elements = Elements(
            inputpath  = options.input_path,
            inputfile  = options.input_file,
            output     = output,
            standards  = setup.standards,
            observable = observable,
            purity     = purity
            )
        if elements.ready():
            count_ready +=1

    msg.info("main", "Succesfully created and stored histograms = "+str(count_ready)+"/"+str(count_all))
    output.close

# run main 
if __name__ == "__main__":

    main(sys.argv[1:])

