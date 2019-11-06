# #
# @class Yields
#
import ROOT, os
from Verbose import *
import math
from GraphUtils import *

class Column(object):

    def __init__(self, 
                 name = "" , 
                 icolumn = {}
                 ):
        self.name = name
        self.column = icolumn

class Row(object):

    def __init__(self, 
                 name = "" , 
                 igraph = None
                 ):
        self.name = name
        self.graph = igraph

class Yields(object):
        
    def __init__(self, outdir = "./", 
                 outfile = "yields.txt",
                 integer_precision = False):
        self.columns = []
        self.rows = []
        self.column_names = []
        self.row_names = []
        self.gu = GraphUtils()
        #
        self.out_dir = outdir
        self.out_file = outfile
        self.integer_precision = integer_precision
        
    def __ncol(self):
        return len(self.column_names) + 1

    def __get_latex_table_top(self):
        
        column_names = self.column_names
        column_names.append("Ratio")
        ncolumns = "l" * (self.__ncol())
        titles = " & ".join(column_names)
        multicolumn = ""
        for i in xrange(1, self.__ncol()+1):
            multicolumn += "{#%i}"%i

        return """\\begin{table}
\\renewcommand{\\arraystretch}{1.3}
\\begin{center}
\\begin{tabular}{%s}\hline
Sample & %s \\\ \hline \hline \n"""%( ncolumns, titles)


    def __get_latex_table_bottom(self):
        caption = ", ".join(self.column_names)
        label = "_".join(self.column_names)
        return """\hline
\end{tabular}
\end{center}
\caption{%s yields.}
\label{tb:yields_%s}
\end{table}\n\n"""%(caption, label)

    def add_column(self, icolumn = None ):
        if icolumn:
            self.columns.append( icolumn )
            self.column_names.append( icolumn.name )

    def add_row(self, irow = None ):
        if irow:
            self.rows.append( irow )
            self.row_names.append( irow.name )

    def store(self):
        
        #upper table
        outbuffer =  self.__get_latex_table_top()
        
        #organize columns
        ref_column = self.columns[0]

        total_prefit = 0
        total_postfit = 0
        error_prefit2 = 0
        error_postfit2 = 0
        for sample, histo in ref_column.column.items():
            #prefit
            error = ROOT.Double()
            area = histo.IntegralAndError(-1, -1, error, "")
            if self.integer_precision:
                outbuffer += "%-*s & %-*i $\pm$ %-*i"%(15, sample, 15, int(area), 15, int(error))
            else:
                outbuffer += "%-*s & %-*.4f $\pm$ %-*.4f"%(15, sample, 15, area, 15, error)
            total_prefit += area
            error_prefit2 += error**2
            #postfit
            for icolumn in self.columns:
                if icolumn.name == ref_column.name:
                    continue
                for isample, ihisto in icolumn.column.items():
                    if isample == sample:
                        ierror = ROOT.Double()
                        iarea = ihisto.IntegralAndError(-1, -1, ierror, "")
                        if self.integer_precision:
                            outbuffer += " & %-*i $\pm$ %-*i"%(15, int(iarea), 15, int(ierror))
                        else:
                            outbuffer += " & %-*.4f $\pm$ %-*.4f"%(15, iarea, 15, ierror)
                        #ratio
                        ratio_val = iarea / area
                        ratio_err = ratio_val * math.sqrt( math.pow(ierror / iarea, 2) + math.pow(error / area, 2) )
                        outbuffer += " & %-*.2f $\pm$ %-*.2f"%(15, ratio_val, 15, ratio_err)
                        total_postfit += iarea
                        error_postfit2 += ierror**2
            outbuffer +=  "\\\\ \n"
        #
        error_prefit = math.sqrt(error_prefit2)
        error_postfit = math.sqrt(error_postfit2)
        total_ratio_val = total_postfit / total_prefit
        total_ratio_error = total_ratio_val * math.sqrt( math.pow(error_prefit / total_prefit, 2) + math.pow(error_postfit / total_postfit, 2) )
        outbuffer += "\hline \n"
        outbuffer += "%-*s & %-*.4f $\pm$ %-*.4f"%(15, "Total", 15, total_prefit, 15, error_prefit)
        outbuffer += " & %-*.4f $\pm$ %-*.4f"%(15, total_postfit, 15, error_postfit)
        outbuffer += " & %-*.2f $\pm$ %-*.2f"%(15, total_ratio_val, 15, total_ratio_error)
        outbuffer += "\\\\ \n"
        outbuffer += "\hline \n"
            
        #organize rows
        for irow in self.rows:
            content, error_low, error_high = self.gu.area_and_poissonian_errors(irow.graph)

            outbuffer += "%-*s & \multicolumn{%i}{c}{$%i^{+%i}_{-%i}$} & \\\\ \n"%(15, irow.name, self.__ncol()-2, content, error_high, error_low)

            total_ratio_prefit = content / total_prefit
            total_error_prefit_high = total_ratio_prefit * math.sqrt( math.pow(error_prefit / total_prefit, 2) + math.pow( error_high / content, 2) )
            total_error_prefit_low = total_ratio_prefit * math.sqrt( math.pow(error_prefit / total_prefit, 2) + math.pow( error_low / content, 2) )

            total_ratio_postfit = content / total_postfit
            total_error_postfit_high = total_ratio_postfit * math.sqrt( math.pow(error_postfit / total_postfit, 2) + math.pow( error_high / content, 2) )
            total_error_postfit_low = total_ratio_postfit * math.sqrt( math.pow(error_postfit / total_postfit, 2) + math.pow( error_low / content, 2) )

            outbuffer += "\hline \n"
            outbuffer += "%-*s"%(15, irow.name+"/Total")
            outbuffer += " & $%.4f^{+%.4f}_{-%.4f}$"%(total_ratio_prefit, total_error_prefit_high, total_error_prefit_low)
            outbuffer += " & $%.4f^{+%.4f}_{+%.4f}$"%(total_ratio_postfit, total_error_postfit_high, total_error_postfit_low)
            outbuffer += " & "

            outbuffer += "\\\\ \n"
            outbuffer += "\hline \n"


        #bottom table
        outbuffer += self.__get_latex_table_bottom() + "\n"
        
        fname = os.path.join(self.out_dir, self.out_file)
        f = open( fname, "w")
        f.write(outbuffer)
        f.close()
        msg.info("Yields::store", "Result is stored in '%s'"%(fname))
