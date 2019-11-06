#!/usr/bin/python
#
# Z. Zinonos - zenon@cernc.ch
# August 2016
#

import os, glob, subprocess, sys

from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument( '-i', '--inputdir',  help='Input directory with eps images',     default='./' )
parser.add_argument( '-o', '--outputdir', help='Where to store output', default='./' )
parser.add_argument( '-t', '--title',     help='Projects\'s title', default='Fit Results' )

options = parser.parse_args()



def get1(title):
  s=title.replace('_', ' ')
  return (
  "\documentclass[10pt]{report}\n"
  "\usepackage[utf8]{inputenc}\n"
  "\usepackage{color}\n"
  "\usepackage[paperwidth=700px, paperheight=700px, top=50px,bottom=50px,left=50px,right=50px,]{geometry}\n"
  "\usepackage{hyperref}\n"
  "\usepackage[all]{hypcap}\n" 
  "\hypersetup{colorlinks, breaklinks, urlcolor=red, linkcolor=magenta, citecolor=blue}\n"
  "\usepackage{graphicx}\n"
  "\DeclareGraphicsExtensions{.eps}\n"
  "% \graphicspath{ {./} }\n"
  "\usepackage{epsfig,bm,epsf,float}\n"
  "% Title Page\n"
  "\\title{ { \Large "+title+"}}\n"
  "% document\n"
  "\\begin{document}\n"
  "\maketitle\n"
  "\clearpage \n"
  "\\addcontentsline{toc}{chapter}{\listfigurename}\n"
  "\listoffigures\n"
  "\label{pg:first}\n"
  )
  
 
#============================================================================
def get2():
  return "\\end{document}\n" 
#============================================================================ 
def entry(s0, s1):
  return ("\\begin{figure}[htb]\n"
  "\centering\n"
  "\includegraphics {"+s0+"}\n"
  "\caption["+s1+" ]{"+s1+" - \hyperref[pg:first]{Go to Index} }\n" 
  "\label{fig:"+s0+"}\n"
  "\\end{figure}\n"
  "\clearpage\n"  )  

#============================================================================ 


wc = "*.eps"
images 	= sorted(glob.glob( os.path.join( options.inputdir, wc) ))

if not images:
  print "no images selected"
  sys.exit()

f = open('images_latex.tex','w')

f.write(get1( options.title ))

for i in images:
  s = i[i.rfind('/')+1:].replace('.eps', '').replace('_', ' ')
  f.write( entry(i,s) )
  
f.write( get2() )
  
  
f.close()


subprocess.call("PlotBookMaker/dolatex.sh " + options.outputdir, shell=True)
subprocess.call("rm -f images_latex.*", shell=True)
  
