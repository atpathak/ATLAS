#! /usr/bin/env python

import ROOT
import sys, math
from Verbose import *

##
# @class GraphUtils
#
class GraphUtils (object):
    """
    Neat graph utilities
    """
    def __init__(self):
        self.__dummy = 0

    def isclose(self, a, b, rel_tol=1e-06, abs_tol=0.0):
        return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

    def get_data_relative_error_graph(self, g):
        
        q = ROOT.TGraphAsymmErrors( g.GetN() )
        for ibin in xrange(g.GetN()):
            x      = ROOT.Double(0)
            y      = ROOT.Double(0)                
            point  = g.GetPoint(ibin, x, y)
            if self.isclose(y, 0.):
                dylow = 0
                dyhigh =0
            else:
                dylow  = g.GetErrorYlow(ibin)/y
                dyhigh = g.GetErrorYhigh(ibin)/y
            dxlow  = g.GetErrorXlow(ibin)
            dxhigh = g.GetErrorXhigh(ibin)
            q.SetPoint(ibin, x, 1)
            q.SetPointEYlow(ibin, dylow)
            q.SetPointEYhigh(ibin, dyhigh)
            q.SetPointEXlow(ibin, dxlow)
            q.SetPointEXhigh(ibin, dxhigh)

        return q

    def get_graph_info(self, A): 
        return A.GetX(), \
            A.GetY(), \
            A.GetEXlow(), \
            A.GetEXhigh(), \
            A.GetEYlow(), \
            A.GetEYhigh()

    def get_graph_ratio_combined_error(self, 
                                       A = ROOT.TGraphAsymmErrors(), 
                                       B = ROOT.TGraphAsymmErrors(), 
                                       E = ROOT.TGraphAsymmErrors()):
        """
        A = data 
        B = stack
        E = errors
        """

        n = A.GetN() 
        if n != B.GetN() or n != E.GetN() or B.GetN() != E.GetN():
            print "Error: cannot divide graphs of different sizes..."
            return None

        q =  ROOT.TGraphAsymmErrors(n)

        xA, yA, dxlowA, dxhighA, dylowA, dyhighA = self.get_graph_info(A)
        xB, yB, dxlowB, dxhighB, dylowB, dyhighB = self.get_graph_info(B)
        xE, yE, dxlowE, dxhighE, dylowE, dyhighE = self.get_graph_info(E)

        for i in xrange(n):          
            r = 0.
            if self.isclose(yB[i], 0.):
                print "Warning: cannot divide to zero value %f in bin %i"%(yB[i], i)
            else:
                r        = yA[i]/yB[i]

            if not self.isclose(xA[i], xB[i]) :
                sys.exit("Fatal: graphs A and B are not compatible... bye..")

            if not self.isclose(xA[i], xE[i]) :
                sys.exit("Fatal: graphs A and E are not compatible... bye..")

            if not self.isclose(xB[i], xE[i]) :
                sys.exit("Fatal: graphs B and E are not compatible (different range)... bye..")

            if not self.isclose(yB[i], yE[i], 1e-2) :
                msg.fatal("GraphUtils::get_graph_ratio_combined_error",
                          "Graphs B and E are not compatible (different y values): at bin %i B=%f E=%f"%(i, yB[i], yE[i]) )
            
            dyhigh = math.sqrt( dyhighB[i]**2 + dyhighE[i]**2) / yB[i]
            dylow = math.sqrt( dylowB[i]**2 + dylowE[i]**2) / yB[i]
            q.SetPoint(i, xA[i], r)
            q.SetPointError(i, dxlowA[i], dxhighA[i], dylow, dyhigh)

        return q

    def get_graph_ratio(self, 
                        A = ROOT.TGraphAsymmErrors(), 
                        B = ROOT.TGraphAsymmErrors(), 
                        Aerrors = False):

        n = A.GetN() 
        if n != B.GetN():
            print "Error: cannot divide graphs of different sizes..."
            return None

        q =  ROOT.TGraphAsymmErrors(n) 
        xA, yA, dxlowA, dxhighA, dylowA, dyhighA = self.get_graph_info(A)
        xB, yB, dxlowB, dxhighB, dylowB, dyhighB = self.get_graph_info(B)

        for i in xrange(n):          
            r = 0.
            if self.isclose(yB[i], 0.):
                print "Warning: cannot divide to zero value %f in bin %i"%(yB[i], i)
            else:
                r = yA[i]/yB[i]

            if not self.isclose(xA[i], xB[i]):
                sys.exit("Fatal: we are not dividing compatible graphs... bye..")
                
            A2       = yA[i]*yA[i];
            B2       = yB[i]*yB[i];
            sAhigh2  = dyhighA[i] * dyhighA[i] if Aerrors else 0.;
            sAlow2   = dylowA[i]  * dylowA[i]  if Aerrors else 0.;
            sBhigh2  = dyhighB[i] * dyhighB[i];
            sBlow2   = dylowB[i]  * dylowB[i];
            f2       = r*r;
            dA2_low  = sAlow2/A2  if A2 > 0. else 0.
            dB2_low  = sBlow2/B2  if B2 > 0. else 0.
            dA2_high = sAhigh2/A2 if A2 > 0. else 0.
            dB2_high = sBhigh2/B2 if B2 > 0. else 0.
            errYlow  = math.sqrt( f2*( dA2_low  + dB2_low ) )
            errYhigh = math.sqrt( f2*( dA2_high + dB2_high ) )
            q.SetPoint(i, xA[i], r)
            q.SetPointError(i, dxlowA[i], dxhighA[i], errYlow, errYhigh)

        return q
    
    def get_graph_with_poissonian_errors(self, histo = None):
        rhe =  ROOT.RooHistError.instance()
        g = ROOT.TGraphAsymmErrors( histo )
        for ibin in xrange(g.GetN()):
            x = ROOT.Double(0)
            y = ROOT.Double(0)
            point = g.GetPoint(ibin, x, y)
            mu1 = ROOT.Double(0)
            mu2 = ROOT.Double(0)
            n = int(y)
            seterror = rhe.getPoissonInterval(n, mu1, mu2, 1.)
            eylow = y-mu1
            eyhigh = mu2-y
            if not seterror:
                print "Warning: could not get the Poisson interval for point %i "%(ibin)
                eylow = 0
                eyhigh = 0
            g.SetPointEYlow(ibin, eylow)
            g.SetPointEYhigh(ibin, eyhigh)
        return g

    def area_and_poissonian_errors(self, g = None):
        rhe =  ROOT.RooHistError.instance()
        area = 0.0
        for i in xrange(g.GetN()):
            x = ROOT.Double(0)
            y = ROOT.Double(0)
            point = g.GetPoint(i, x, y)
            area += y
        #
        mu1 = ROOT.Double(0)
        mu2 = ROOT.Double(0)
        n = int(area)
        seterror = rhe.getPoissonInterval(n, mu1, mu2, 1.)
        errorlow = area-mu1
        errorhigh = mu2-area
        if not seterror:
            msg.warning("Column::area_and_poissonian_error", "Could not get the Poisson interval... " )
            errorlow = 0
            errorhigh = 0

        return area, errorlow, errorhigh
