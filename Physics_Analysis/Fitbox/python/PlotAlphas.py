import ROOT
import sys, math

class Point (object):
    """
    Point class
    """
    def __init__(self,
                 name = "",
                 pdf_shift = 0,
                 value = 0,
                 error_up = 0,
                 error_down = 0
                 ):
        self.name = name
        self.pdf_shift = pdf_shift
        self.pdf_sigma = 1.0
        self.value = value
        self.error_up = error_up
        self.error_down = error_down

    def final_factor(self):
        return self.pdf_shift / self.pdf_sigma

    @property
    def final_value(self):
        return self.value * self.final_factor()

    @property
    def final_error_up(self):
        return self.error_up * self.final_factor()

    @property
    def final_error_down(self):
        return self.error_down * self.final_factor()

    @property
    def final_error_up_abs(self):
        return math.fabs(self.error_up) * self.final_factor()

    @property
    def final_error_down_abs(self):
        return math.fabs(self.error_down) * self.final_factor()

    @property
    def final_error_up_rel(self):
        return self.error_up / self.value * 100.

    @property
    def final_error_down_rel(self):
        return self.error_down / self.value * 100

    def print_me(self):
        
        print "$%s$ & %.4f & %.4f & %.4f \\\\"%(self.name.replace("#","\\").replace(",", ",\,"), 
                                                self.final_value, self.final_error_up, self.final_error_down)



class PointsSet (object):
    """
    A set of Point objects
    """
    def __init__(self,
                 active = True,
                 name = "",
                 points_list = [],
                 x_offset = 0,
                 ymin = 0,
                 color = ROOT.kBlack,
                 marker = 20,
                 ):
        self.active = active
        self.name = name
        self.points_list = points_list
        self.x_offset = x_offset
        self.y_axis_min = ymin
        self.color = color
        self.marker = marker
        #custom
        self.np = len(points_list)
        self.latex_list = []
        self.largest_label = 0
        self.max_y = 0.
        self.min_y = 1.
        self.graph = self.__get_graph()

    def is_active(self):
        return self.active

    def print_me(self):
        print """
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\\begin{table}
 %%
\\begin{center}
\\begin{tabular}{llll} \hline
Parameter & Central Value & Error Up & Error Down\\\\ \\hline
"""
        for p in self.points_list:
            p.print_me()

        print """
\\hline
\\end{tabular}
\\end{center}
%%
\\caption{%s}
\\end{table}
"""%(self.name.replace("#eta", "$\\eta$"))
        

    def GetGlobalMax(self):
        return self.max_y 

    def GetGlobalMin(self):
        return self.min_y 

    def __get_graph(self):

        g = ROOT.TGraphAsymmErrors(self.np)
        largest_name = 0
        for c, p in enumerate(self.points_list):

            if p.final_value + p.final_error_up_abs > self.max_y:
                self.max_y = p.final_value + p.final_error_up_abs

            if p.final_value - p.final_error_down_abs < self.min_y:
                self.min_y = p.final_value - p.final_error_down_abs

            x = c + self.x_offset

            g.SetPoint(c,
                       x,
                       p.final_value
                       )

            g.SetPointError(c, 
                            0, 
                            0,
                            p.final_error_down_abs,
                            p.final_error_up_abs
                            )

            ydown = p.value + p.error_down 

            if len(p.name) >  largest_name:
                largest_name =  len(p.name)
    
        self.largest_label = largest_name

        g.SetMarkerStyle(self.marker)
        g.SetMarkerSize(1.5)
        g.SetLineWidth(2)
        g.SetMarkerColor(self.color)
        g.SetLineColor(self.color)

        miny = self.min_y * 1.05 if self.min_y < 0 else self.min_y * 0.95
        for c, p in enumerate(self.points_list):

            lat = ROOT.TLatex(c, miny, p.name)
            lat.SetTextFont(42)
            lat.SetTextAlign(11)
            lat.SetTextSize( lat.GetTextSize() * 0.75 )
            lat.SetTextAngle( -65 )
            self.latex_list.append(lat)
        
        return g
#
def main(argv):

    ROOT.gROOT.SetBatch(True)

    #initial PDF variations
    alpha_TER_EM  = 0.10
    alpha_TER_HAD = 0.10
    alpha_TES_EM  = 0.10
    alpha_TES_HAD = 0.05


    #ranges
    minYaxis = -0.1
    maxYaxis = 0.1

    #list of point sets
    plist = []

    #calo_tes_20171010 -1p, central eta, fneu energy
    plist.append(
        PointsSet(True,
                  "p_{T} slice 1, central, f_neu(E)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.0155379, 0.273038, -0.273038),
                Point("#alpha(TES, EM)", alpha_TES_EM,  -0.103137, 0.139776, -0.139776) ],
                  -0.2,
                  minYaxis,
                  ROOT.kBlack,
                  25
                  )
        )
    plist.append(
        PointsSet(True,
                  "p_{T} slice 2, central, f_neu(E)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.589882, 0.199569, -0.199569),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.173838, 0.139152, -0.139152) ],
                  -0.1,
                  minYaxis,
                  ROOT.kMagenta,
                  25
                  )
        )
    plist.append(
        PointsSet(True,
                  "p_{T} slice 3, central, f_neu(E)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.434528, 0.0811426, -0.0811426),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.0386276, 0.145202, -0.145202) ],
                  0,
                  minYaxis,
                  ROOT.kRed,
                  25
                  )
        )
    plist.append(
        PointsSet(True,
                  "p_{T} slice 4, central, f_neu(E)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.630972, 0.071574, -0.071574),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.0241682, 0.222872, -0.222872) ],
                  0.1,
                  minYaxis,
                  ROOT.kAzure,
                  25
                  )
        )

    #calo_tes_20171010 -1p, central eta, fneu pT
    plist.append(
        PointsSet(True,
                  "p_{T} slice 1, central, f_neu(pT)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.0107467, 0.279784, -0.279784),
                Point("#alpha(TES, EM)", alpha_TES_EM,  -0.104087, 0.138643, -0.138643) ],
                  -0.2,
                  minYaxis,
                  ROOT.kBlack,
                  20
                  )
        )
    plist.append(
        PointsSet(True,
                  "p_{T} slice 2, central, f_neu(pT)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.599981, 0.202851, -0.202851),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.172377, 0.140202, -0.140202) ],
                  -0.1,
                  minYaxis,
                  ROOT.kMagenta,
                  20
                  )
        )
    plist.append(
        PointsSet(True,
                  "p_{T} slice 3, central, f_neu(pT)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.436222, 0.0810308, -0.0810308),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.0396039, 0.148767, -0.148767) ],
                  0,
                  minYaxis,
                  ROOT.kRed,
                  20
                  )
        )
    plist.append(
        PointsSet(True,
                  "p_{T} slice 4, central, f_neu(pT)", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.635554, 0.0714047, -0.0714047),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.0332609, 0.216214, -0.216214) ],
                  0.1,
                  minYaxis,
                  ROOT.kAzure,
                  20
                  )
        )

    #calo_tes_20170815 rQCD50 - 1P, central eta
    plist.append(
        PointsSet(False,
                  "p_{T} slice 1, central", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.129413, 0.279877, -0.279877),
                Point("#alpha(TES, EM)", alpha_TES_EM,  -0.0418127, 0.138001, -0.138001) ],
                  -0.2,
                  minYaxis,
                  ROOT.kBlack,
                  25
                  )
        )
    plist.append(
        PointsSet(False,
                  "p_{T} slice 2, central", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.606928, 0.208673, -0.208673),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.159761, 0.143587, -0.143587) ],
                  -0.1,
                  minYaxis,
                  ROOT.kMagenta,
                  25
                  )
        )
    plist.append(
        PointsSet(False,
                  "p_{T} slice 3, central", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.418424, 0.0829364, -0.0829364),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.0117784, 0.146469, -0.146469) ],
                  0,
                  minYaxis,
                  ROOT.kRed,
                  25
                  )
        )
    plist.append(
        PointsSet(False,
                  "p_{T} slice 4, central", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.631952, 0.0671674, -0.0671674),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.083524, 0.206412, -0.206412) ],
                  0.1,
                  minYaxis,
                  ROOT.kAzure,
                  25
                  )
        )

    #calo_tes_20170815 rQCD50 - 1P, forward eta
    plist.append(
        PointsSet(False,
                  "p_{T} slice 1, forward", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.731644, 0.620367, -0.620367),
                Point("#alpha(TES, EM)", alpha_TES_EM,  -0.425717, 0.257728, -0.257728) ],
                  -0.18,
                  minYaxis,
                  ROOT.kBlack,
                  20
                  )
        )
    plist.append(
        PointsSet(False,
                  "p_{T} slice 2, forward", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.352497, 0.283937, -0.283937),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.38781, 0.142371, -0.142371) ],
                  -0.08,
                  minYaxis,
                  ROOT.kMagenta,
                  20
                  )
        )
    plist.append(
        PointsSet(False,
                  "p_{T} slice 3, forward", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.397313, 0.0893721, -0.0893721),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.0589999, 0.240935, -0.240935) ],
                  0.02,
                  minYaxis,
                  ROOT.kRed,
                  20
                  )
        )
    plist.append(
        PointsSet(False,
                  "p_{T} slice 4, forward", [ 
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.468061, 0.105893, -0.105893),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.615834, 0.3357, -0.3357) ],
                  0.12,
                  minYaxis,
                  ROOT.kAzure,
                  20
                  )
        )
    

    # calo_tes_20170513
    plist.append(
        PointsSet(False,
                  "1P / #eta inclusive", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.551795, 0.145577, -0.146187),
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.430445, 0.434475, -0.394138),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.0262024, 0.0713267, -0.0819271),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.48187, 0.0706658, -0.0710398) ],
                  0.,
                  minYaxis,
                  ROOT.kBlack,
                  25
                  )
        )

    plist.append(
        PointsSet(False,
                  "1P / #eta central", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.147503, 0.204192, -0.24307),
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.156864, 0.284124, -0.299241),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.365463, 0.0665145, -0.0668713),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.303822, 0.0883542, -0.0840696) ],
                  -0.2,
                  minYaxis,
                  ROOT.kAzure-2,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "1P / #eta forward", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.435407, 0.704459, -0.652924),
                Point("#alpha(TER, EM)", alpha_TER_EM, 0.881699, 0.439077, -0.366102 ),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.365781, 0.0604457, -0.0676475),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.345477, 0.105397, -0.101393) ],
                  0.2,
                  minYaxis,
                  ROOT.kRed-4,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta inclusive", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.394328, 0.1668, -0.209967),
                Point("#alpha(TER, EM)", alpha_TER_EM, -1.10158, 0.595551, -0.635479),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.514166, 0.05331, -0.0594093),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.425926, 0.249022, -0.25477) ],
                  0.,
                  minYaxis,
                  ROOT.kBlack,
                  25
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta central", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.121508, 0.278001, -0.493541),
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.55494, 1.17522, -0.615891),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.674143, 0.0756077, -0.0932915),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.348485, 0.28344, -0.479956) ],
                  -0.2,
                  minYaxis,
                  ROOT.kAzure-2,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta forward", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, -0.141143, 0.485741, -0.495051),
                Point("#alpha(TER, EM)", alpha_TER_EM,  -0.529429, 0.914239, -0.656635),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.415497, 0.0805656, -0.0855575),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.5606, 0.320115, -0.311554) ],
                  0.2,
                  minYaxis,
                  ROOT.kRed-4,
                  21
                  )
        )

    # calo_tes_20170606 - normal alphas
    plist.append(
        PointsSet(False,
                  "1P / #eta central 06/06", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.327274, 0.135957, -0.137372),
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.211372, 0.242988, -0.240329),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.374786, 0.0684407, -0.0653457),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.326067, 0.0561187, -0.0560712) ],
                  0.2,
                  minYaxis,
                  ROOT.kRed-3,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "1P / #eta forward 06/06", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.0690084, 0.375098, -0.357149),
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.761439, 0.477665, -0.535989),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.619809, 0.0498495, -0.0499525),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.24361, 0.104399, -0.0997413) ],
                  0.2,
                  minYaxis,
                  ROOT.kAzure-2,
                  20
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta inclusive 06/06", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.394328, 0.1668, -0.209967),
                Point("#alpha(TER, EM)", alpha_TER_EM, -1.10158, 0.595551, -0.635479),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.514166, 0.05331, -0.0594093),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.425926, 0.249022, -0.25477) ],
                  0,
                  minYaxis,
                  ROOT.kBlack,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta central 06/06", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.121508, 0.278001, -0.493541),
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.55494, 1.17522, -0.615891),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.674143, 0.0756077, -0.0932915),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.348485, 0.28344, -0.479956) ],
                  -0.2,
                  minYaxis,
                  ROOT.kRed-3,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta forward 06/06", [ 
                Point("#alpha(TER, HAD)", alpha_TER_HAD,  -0.141143, 0.485741, -0.495051),
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.529429, 0.914239, -0.656635),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.415497, 0.0805656, -0.0855575),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.5606, 0.320115, -0.311554) ],
                  0.2,
                  minYaxis,
                  ROOT.kAzure-2,
                  20
                  )
        )

    # calo_tes_20170606 - TES trk pt alphas
    plist.append(
        PointsSet(False,
                  "1P / #eta central", [ 
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.154221, 0.239703, -0.240778),
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.327742, 0.137655, -0.140459),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.415642, 0.0687257, -0.0649771),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.374786, 0.0684407, -0.0653457),
                Point("#alpha(TES, HAD, 0 < p_{T}^{trk} < 12 GeV)", alpha_TES_HAD, -0.0101828, 0.21865, -0.231017),
                Point("#alpha(TES, HAD, 12 < p_{T}^{trk} < 20 GeV)",  alpha_TES_HAD, 1.99215e-05, 0.372271, -0.372291),
                Point("#alpha(TES, HAD, p_{T}^{trk} > 20 GeV)",  alpha_TES_HAD, -0.334946, 0.05969, -0.0605711)
                ],
                  -0.1,
                  minYaxis,
                  ROOT.kRed-3,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "1P / #eta forward", [ 
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.663492, 0.469658, -0.551048),
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.0613027, 0.392834, -0.375915),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.124079, 0.132747, -0.121971),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.619809, 0.0498495, -0.0499525),
                Point("#alpha(TES, HAD, 0 < p_{T}^{trk} < 12 GeV)", alpha_TES_HAD, -0.179121, 0.345449, -0.345231),
                Point("#alpha(TES, HAD, 12 < p_{T}^{trk} < 20 GeV)",  alpha_TES_HAD, -3.67732e-05, 0.367905, -0.367828),
                Point("#alpha(TES, HAD, p_{T}^{trk} > 20 GeV)",  alpha_TES_HAD, 0.646134, 0.0515145, -0.0515231)
                ],
                  0.1,
                  minYaxis,
                  ROOT.kAzure-2,
                  20
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta central", [ 
                Point("#alpha(TER, EM)", alpha_TER_EM, -0.142874, 0.270912, -0.277276),
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.15995, 0.12732, -0.138334),
                Point("#alpha(TES, EM)", alpha_TES_EM, -0.139543, 0.226992, -0.167162),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, -0.712448, 0.0490368, -0.0510701),
                Point("#alpha(TES, HAD, 0 < p_{T}^{trk} < 12 GeV)",alpha_TES_HAD, 0.0814163, 0.203837, -0.198274),
                Point("#alpha(TES, HAD, 12 < p_{T}^{trk} < 20 GeV)", alpha_TES_HAD, -9.08249e-06, 0.303032, -0.303014),
                Point("#alpha(TES, HAD, p_{T}^{trk} > 20 GeV)",  alpha_TES_HAD, -1.46766, 0.104305, -0.11261)
                ],
                  -0.1,
                  minYaxis,
                  ROOT.kRed-3,
                  21
                  )
        )

    plist.append(
        PointsSet(False,
                  "3P / #eta forward", [ 
                Point("#alpha(TER, EM)", alpha_TER_EM, 0.0202773, 0.187067, -0.183472),
                Point("#alpha(TER, HAD)", alpha_TER_HAD, 0.0181338, 0.168222, -0.167308),
                Point("#alpha(TES, EM)", alpha_TES_EM, 0.0412949, 0.155113, -0.156095),
                Point("#alpha(TES, HAD)", alpha_TES_HAD, 0.425576, 0.0556998, -0.0569048),
                Point("#alpha(TES, HAD, 0 < p_{T}^{trk} < 12 GeV)", alpha_TES_HAD, 0.246337, 0.163344, -0.151221),
                Point("#alpha(TES, HAD, 12 < p_{T}^{trk} < 20 GeV)", alpha_TES_HAD, -3.50638e-07, 0.16894, -0.168939),
                Point("#alpha(TES, HAD, p_{T}^{trk} > 20 GeV)",  alpha_TES_HAD, 0.684195, 0.147809, -0.146674)
                ],
                  0.1,
                  minYaxis,
                  ROOT.kAzure-2,
                  20
                  )
        )

    #file
    rfile = ROOT.TFile("alpha_tes.root", "RECREATE")

    #canvas
    canvas = ROOT.TCanvas("canvas", "", 10, 10, 800, 500)
    canvas.cd()

    #first active
    for p in plist:
        if  p.is_active():
            p_first_active = p
            break
    
    #null histo
    np = p_first_active.np
    hminX = -0.5
    hmaxX = np+1.5
    h = ROOT.TH1F("h", "", np+1, hminX, hmaxX)
    h.SetTitle("")
    h.GetYaxis().SetTitle("Absolute post-fit shift")
    h.GetYaxis().SetTitleSize( h.GetYaxis().GetTitleSize() *1.1)
    h.SetLineColor(0)
    h.SetMarkerColor(0)
    h.SetStats(0)
    h.SetLabelFont(42)
    h.SetTitleFont(42)
    h.GetXaxis().SetNdivisions(515)
    h.Draw()

    h.SetMaximum(maxYaxis)
    h.SetMinimum(minYaxis)
    h.SetLabelSize(0)
       
    #lines/boxes
    box2 = ROOT.TBox(hminX, -0.02, hmaxX, 0.02)
    box2.SetLineColor(ROOT.kGreen-7)
    box2.SetFillColor(ROOT.kGreen-7)
    box2.Draw()

    box1 = ROOT.TBox(hminX, -0.01, hmaxX, 0.01)
    box1.SetLineColor(ROOT.kYellow-6)
    box1.SetFillColor(ROOT.kYellow-6)
    box1.Draw()


    #line
    line = ROOT.TLine(hminX, 0, hmaxX, 0)
    line.SetLineColor(ROOT.kGray+2)
    line.SetLineWidth(1)
    line.SetLineStyle(2)
    line.Draw()
    
    #graph
    max_y = 0
    min_y = 1

    for p in plist:
        if not p.is_active():
            continue
        if p.GetGlobalMax() > max_y:
            max_y = p.GetGlobalMax()
        if p.GetGlobalMin() < min_y:
            min_y = p.GetGlobalMin()
        p.graph.Draw("e p z same")


    final_y_max = max_y * 1.05 
    final_y_min = min_y * 1.05 if min_y < 0 else min_y * 0.95
    h.SetMaximum(final_y_max)
    h.SetMinimum(final_y_min)

    #labels
    fbottom =  0.1 + p_first_active.largest_label/100.;
    canvas.SetBottomMargin( fbottom );
    canvas.SetRightMargin( canvas.GetRightMargin( ) * 0.5 );

    for lat in p_first_active.latex_list:
        lat.SetY(min_y * 0.9 if min_y > 0 else min_y * 1.1)
        lat.Draw( )

    #pad
    ROOT.gPad.SetTickx(1);
    ROOT.gPad.SetGridx(1);
    ROOT.gPad.SetTicky(1);
    ROOT.gPad.SetGridy(1);


    #legend
    leg_y_pos = 0.65 if math.fabs(final_y_max) > math.fabs(final_y_min)  else 0.25

    leg = ROOT.TLegend(0.65, leg_y_pos, 0.9,  leg_y_pos + 0.3, "", "brNDC")
    leg.SetBorderSize(0)
    leg.SetLineColor(0)
    leg.SetLineStyle(0)
    leg.SetLineWidth(0)
    leg.SetFillColor(0);
    leg.SetFillStyle(0);
    leg.SetTextSize( leg.GetTextSize() * 1.2 )

    
    #markers
    for p in plist:
        if not p.is_active():
            continue
        leg.AddEntry(p.graph, p.name, "lp")

    leg.Draw()

    #update 
    ROOT.gPad.Update()
    ROOT.gPad.RedrawAxis()


    rfile.cd()
    canvas.Write()
    rfile.Close()

    #print
    for p in plist:
        if p.is_active():
            p.print_me()

if __name__ == "__main__":

    main(sys.argv[1:])


    #second axis
'''
    maxXaxis2 = hmaxX
    minXaxis2 = maxXaxis2

    minYaxis2 =  minYaxis
    maxYaxis2 =  maxYaxis

    WminYaxis2 = get_abs_shift( minYaxis )
    WmaxYaxis2 = get_abs_shift( maxYaxis )

    print "Erf 1sigma = ", get_abs_shift(1.)
    print "Erf xsigma = ", get_abs_shift(-0.303822)
    print "Erf axis min = ", WminYaxis2
    print "Erf axis max = ", WmaxYaxis2

    axis2 = ROOT.TGaxis(minXaxis2, 
                        minYaxis2,
                        maxXaxis2,
                        maxYaxis2,
                        WminYaxis2,
                        WmaxYaxis2,
                        510,
                        "+L");
    
    axis2.SetTitle("Post-fit absolute shift");
    #axis2.SetLabelSize( axis2.GetLabelSize() *1.)
    #axis2.SetTitleSize( axis2.GetTitleSize() *1.2)
    axis2.SetTitleOffset(1.1)
    #axis2.SetLabelOffset(0.05)
    axis2.SetLabelFont(42)
    axis2.SetTitleFont(42)
    axis2.SetLineColor(ROOT.kAzure)
    axis2.SetLabelColor(ROOT.kAzure)
    axis2.Draw("")
'''
