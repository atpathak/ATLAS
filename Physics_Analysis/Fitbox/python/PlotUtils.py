#! /usr/bin/env python

##
# @class PlotUtils
#
import ROOT

class PlotUtils (object):
    """
    Plot utilities
    """
    def __init__(self):
        self.__vsplit   = 0.35
        self.__vspace   = 0.02
        self.__Canvas   = None
        self.__Pad1     = None
        self.__Pad2     = None
        self.__LegH1    = 0.055
        self.__LegH2    = 0.060
        self.__Leg1Up   = None
        self.__NLeg1Up  = 0
        self.__Leg2Up   = None
        self.__NLeg2Up  = 0
        self.__Leg1Down = None
        self.__Leg2Down = None
        self.__Axis1    = None
        self.__texts    = []

    def prepare_1pad(self, cname, width=800, height=800, isQuad = False):
        if isQuad:
            width = height
        if self.__Canvas: 
            if self.__Canvas.GetName() != cname:
                self.__Canvas = None
            else:
                "Info: Already found a canvas named:",cname
                return self.__Canvas
        self.__Canvas = ROOT.TCanvas(cname, cname, width, height)
        self.__Canvas.cd()     

    def get_canvas(self):
        if not self.__Canvas:
            print "WARNING: No Canvas has been created yet..."
        return self.__Canvas

    def prepare_2pads(self, cname, width=800, height=800, isQuad = False):
        self.prepare_1pad(cname, width, height, isQuad)
                
        self.__Pad1 = ROOT.TPad("p1_" + cname, cname, 0.0, self.__vsplit - 0.03, 1.0, 1.0)        
        self.__Pad1.SetBottomMargin(self.__vspace)  # set space between top and bottom pad
        self.__Pad1.SetTopMargin(0.05)
        self.__Pad1.SetRightMargin(0.05)
        self.__Pad1.SetGrid(1,1)
        self.__Pad1.SetTicks(1,1)
        ROOT.gStyle.SetGridColor(ROOT.kGray)
        ROOT.gStyle.SetOptStat(0)
        ROOT.gStyle.SetOptTitle(0)
        self.__Pad1.Draw()

        self.__Pad2 = ROOT.TPad("p2_" + cname, cname, 0.0, 0.0, 1.0, self.__vsplit - 0.03)
        self.__Pad2.SetTopMargin(self.__vspace)  # set space between top and bottom pad
        self.__Pad2.SetBottomMargin(0.25)
        self.__Pad2.SetRightMargin(0.05)
        self.__Pad2.SetGrid(1,1)
        self.__Pad2.SetTicks(1,1)
        ROOT.gStyle.SetGridColor(ROOT.kGray)
        ROOT.gStyle.SetOptStat(0)
        ROOT.gStyle.SetOptTitle(0)
        self.__Pad2.Draw()

    def GoToTopPad(self):
        self.__Canvas.cd()
        self.__Pad1.cd()

    def GoToBottomPad(self):
        self.__Canvas.cd()
        self.__Pad2.cd()
        
    def UpdateTopPad(self):
        self.__Pad1.Update()
        ROOT.gPad.RedrawAxis()

    def UpdateBottomPad(self):
        self.__Pad2.Update()
        ROOT.gPad.RedrawAxis()

    def GetTopPad(self):
        return self.__Pad1   

    def GetBottomPad(self):
        self.__Pad2.cd()
        return self.__Pad2

    def make_legends_up(self, legend_relative_font_size=1.2):
        self.__Pad1.cd()

        leg1 = ROOT.TLegend(0.67, 0.475, 0.93, 0.93, "", "brNDC")
        leg1.SetBorderSize(0)
        leg1.SetLineColor(0)
        leg1.SetLineStyle(0)
        leg1.SetLineWidth(0)
        leg1.SetFillColor(0);
        leg1.SetFillStyle(0);
        leg1.SetTextSize( leg1.GetTextSize() * legend_relative_font_size );

        leg2 = ROOT.TLegend(0.45, 0.475, 0.66, 0.93, "", "brNDC")
        leg2.SetBorderSize(0)
        leg2.SetLineColor(0)
        leg2.SetLineStyle(0)
        leg2.SetLineWidth(0)
        leg2.SetFillColor(0);
        leg2.SetFillStyle(0);
        leg2.SetTextSize( leg2.GetTextSize() * legend_relative_font_size );

        self.__Leg1Up = leg1
        self.__Leg2Up = leg2

    def add_entry_legend_up(self, histo = None, title = "", option = "", number = 1):
        if number == 1:
            self.__Leg1Up.AddEntry(histo, title, option)
            self.__NLeg1Up += 1
        elif number == 2:
            self.__Leg2Up.AddEntry(histo, title, option)
            self.__NLeg2Up += 1

    def draw_legends_up(self):
        self.__Leg1Up.SetY1( self.__Leg1Up.GetY2() - self.__NLeg1Up * self.__LegH1)
        self.__Leg2Up.SetY1( self.__Leg2Up.GetY2() - self.__NLeg2Up * self.__LegH2)

        self.__Leg1Up.Draw()
        self.__Leg2Up.Draw()

    def display_up(self, text=''):
        self.__Pad1.cd()
        lat = ROOT.TLatex(0.125, 0.875, text)
        lat.SetNDC(ROOT.kTRUE)
        lat.SetTextFont(42)
        lat.SetTextSize( lat.GetTextSize()*0.75 )
        lat.SetTextColor(ROOT.kGray+3)
        self.__texts.append(lat)
        lat.Draw()

    def make_legends_down(self, legend_relative_font_size=1.2):
        self.__Pad2.cd()

        leg1 = ROOT.TLegend(0.1, 0.6, 0.4, 0.9, "", "brNDC")
        leg1.SetBorderSize(0)
        leg1.SetLineColor(0)
        leg1.SetLineStyle(0)
        leg1.SetLineWidth(0)
        leg1.SetFillColor(0);
        leg1.SetFillStyle(0);
        leg1.SetTextSize( leg1.GetTextSize() * legend_relative_font_size );

        leg2 = ROOT.TLegend(0.5, 0.6, 0.8, 0.9, "", "brNDC")
        leg2.SetBorderSize(0)
        leg2.SetLineColor(0)
        leg2.SetLineStyle(0)
        leg2.SetLineWidth(0)
        leg2.SetFillColor(0);
        leg2.SetFillStyle(0);
        leg2.SetTextSize( leg2.GetTextSize() * legend_relative_font_size );

        self.__Leg1Down = leg1
        self.__Leg2Down = leg2

    def add_entry_legend_down(self, histo = None, title = "", option = "", number = 1):
        if number == 1:
            self.__Leg1Down.AddEntry(histo, title, option)
        elif number == 2:
            self.__Leg2Down.AddEntry(histo, title, option)

    def draw_legends_down(self):
        self.__Leg1Down.Draw()
        self.__Leg2Down.Draw()

    def display_second_axis_up(self, YaxisTitle = "",  href = None, ymin = 0., ymax = 1.):
        self.__Pad1.cd()
        n = href.GetNbinsX()
        x = href.GetBinLowEdge(n) + href.GetBinWidth(n)
        XaxisMin = x
        YaxisMin = href.GetMinimum()
        XaxisMax = x
        YaxisMax = href.GetMaximum()
        YaxisWmin = ymin
        YaxisWmax = ymax
        print XaxisMin, \
            YaxisMin, \
            XaxisMax, \
            YaxisMax, \
            YaxisWmin, \
            YaxisWmax \

        axis = ROOT.TGaxis(XaxisMin,
                           YaxisMin,
                           XaxisMax,
                           YaxisMax,
                           YaxisWmin,
                           YaxisWmax,
                           510,
                           "+L");
        
        axis.SetTitle(YaxisTitle);
        #    axis.SetLabelSize( )
        #    axis.SetTitleSize( )
        #axis.SetTitleOffset(1.5)
        #axis.SetLabelOffset()
        axis.SetLabelFont(42)
        axis.SetLineColor(ROOT.kAzure)
        axis.SetLabelColor(ROOT.kAzure)
        self.__Axis1 = axis
        axis.Draw()

    def display_second_axis_up(self, YaxisTitle = "",   
                               XaxisMin = 0.,
                               YaxisMin = 0.,
                               XaxisMax = 0.,
                               YaxisMax = 0.,
                               YaxisWmin = 0.,
                               YaxisWmax = 0.):
        self.__Pad1.cd()

        axis = ROOT.TGaxis(XaxisMin,
                           YaxisMin,
                           XaxisMax,
                           YaxisMax,
                           YaxisWmin,
                           YaxisWmax,
                           510,
                           "+L",
                           0.);
        
        axis.SetTitle(YaxisTitle);
        axis.SetTitleOffset( axis.GetTitleOffset() * 1.12 )
        axis.SetLabelSize( axis.GetLabelSize() * 0.8 )
        axis.SetTitleSize( axis.GetTitleSize() * 0.8 )
        axis.SetLabelFont(42)
        axis.SetLineColor(ROOT.kGray+2)
        axis.SetLabelColor(ROOT.kGray+2)
        axis.SetTitleColor(ROOT.kGray+2)
        self.__Axis1 = axis
        axis.Draw()

