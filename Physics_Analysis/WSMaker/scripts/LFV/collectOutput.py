#!/usr/bin/env python                                                                                                                                         

import sys
import os
import subprocess
from ROOT import TFile, TH1D

afshomedir=str(os.environ["HOME"])
afsworkdir = afshomedir.replace('user', 'work')
print afsworkdir

def getsig(filename):

      thefile=TFile.Open(filename)
      hist = thefile.Get("hypo")
      obsSig = hist.GetBinContent(1)
      expSig = hist.GetBinContent(2)
      obsP0 = hist.GetBinContent(4)
      expP0 = hist.GetBinContent(5)
      thefile.Close()
      print "obsSig: {0} expSig: {1} obsP0: {2} expP0: {3}".format(obsSig,expSig,obsP0,expP0)

      return obsSig,expSig,obsP0,expP0

def doNPrank(tag, subtag=None):

    # Collects the outputs from multiple jobs submitted to lxbatch for NPranking
    # and makes the plot. 
    # Output ends up in eps-files/ and pdf-files/ named tag_*


    nJobs = 20 # Modify this if you split into more jobs!
    here=os.environ["PWD"]
    commonpath =afsworkdir+"/analysis/statistics/batch/Bigjob."+tag
    if(subtag):
          commonpath+="/"+subtag+"_job"
    else:
          commonpath +="/job"
    # Loop over all jobs, untar and copy over the outputs
    warn = " "
    for i in range(0,nJobs):
          ipath = commonpath+str(i)+"of"+str(nJobs)+"_LFV13TeV_"+tag+"_Systs_125"
          os.chdir(ipath)
          os.system("tar -xzf output.tar.gz")
          # Check if there were any infinite loops (= problems...)
          logfile="logs/output_Bigjob."+tag
          if(subtag):
                logfile+="_"+subtag
          logfile+="_job"+str(i)+"of"+str(nJobs)+"_LFV13TeV_"+tag+"_Systs_125_runNPranking.log"
          grpcmd="grep Infinite "+logfile
#          infloop = subprocess.check_output(grpcmd, shell=True)
          infloop = subprocess.call(grpcmd, shell=True)
          #print infloop
          if infloop!=1:
                warning= "*** WARNING! Infinite Loop!! In subjob "+str(i)+" *** \n "
                print warning
                print logfile
                print 
                warn+=warning
          os.system("cp -r root-files/ "+here)
    
    #run the makeNPrankplots          
    name = "Bigjob."+tag
    if(subtag):
          name+="_"+subtag
    name+="_job"+str(nJobs)+"of"+str(nJobs)+"_LFV13TeV_"+tag+"_Systs_125"
    npcommand = "python $WORKDIR/scripts/makeNPrankPlots.py "+name+" 125"
    os.chdir(here)
    os.system(npcommand)
    print warn


def formatNPtable(pullstex,pullsfile):
    with open(pullstex) as f:
      content = f.readlines()
    linecounter =0

    with open(pullsfile,'w') as f:
       for line in content:
          if linecounter>30:
                linecounter=0
                f.write("\\hline\n \\end{tabular}\n\n")
                f.write("\\begin{tabular}{|l|c|}\hline \n Nuisance parameter \& postfit value (in $\\sigma$ unit) \\\\\\hline\n")
          f.write(line)
          linecounter+=1
          


def plotsToPdf(tag1,tag2,doNP,thedir):
   # Collect all the plots into a pdf. Assumes you are inside the corresponding directory, and have untarred the output already
   # doNP: 0 (no NP-collecting), 1 (only collect NP if tag1==tag2), 2 (collect all NPs) 


    extra = tag1+"_"+tag2
    name = tag2	
    if(tag1==tag2): 
          extra = tag1
          name= "combined"
          		
    # Get a "cover page" to identify the plots!
    os.system("cp $WORKDIR/scripts/SMHtautau/covertemplate.tex .")
    sedcommand="sed -e \"s=IDENTIFIER="+name+"=g\" covertemplate.tex > cover.tex"
    os.system(sedcommand)
    os.system("pdflatex -interaction=batchmode cover.tex")
			
    plotpath="plots/Bigjob."+extra+"_LFV13TeV_"+tag1+"_Systs_125"
    
    if(tag1==tag2):
          # Post-fit plots only produced for the combined...
          postfitplots=plotpath+"/postfit/*.eps"
          os.system("convert "+postfitplots+" "+name+"_plots.pdf")
          
    #Put all the pdfs together
    pdfcommand = "gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -sOutputFile="+name+"_figures.pdf "

    grpcmd= "grep FAILED fccs/FitCrossChecks_Bigjob."+extra+"_LFV13TeV_"+tag1+"_Systs_125_combined/output_0.log"
    os.system(grpcmd)


    fccplots = plotpath+"/fcc/GlobalFit_unconditionnal_mu1/" 
    pdfstomerge = [
        "cover.pdf",
        fccplots+"corr_origin.pdf",
        fccplots+"corr_noMCStat.pdf",
        fccplots+"corr_HighCorr.pdf",
        fccplots+"corr_TESJESTNorm.pdf",
        fccplots+"corr_HH.pdf",
        fccplots+"corr_LH.pdf",
        fccplots+"corr_LL.pdf",
        fccplots+"corr_noMCnoBTag.pdf",
        fccplots+"NP_all.pdf",
        fccplots+"NP_norm.pdf",
        fccplots+"NP_Jet.pdf",
        ]
    pdfmergecommand=pdfcommand
    for p in pdfstomerge:
        pdfmergecommand +=p
        pdfmergecommand +=" "
    
    if(tag1==tag2):
          pdfmergecommand+=name+"_plots.pdf"

    workdir = os.environ["PWD"] # Need to run doNPrank from the Hbb workdir
    if( doNP!=0 and tag1==tag2 ):
          os.chdir(workdir)
          doNPrank(tag1)
          os.system("cp pdf-files/Bigjob."+tag1+"_job20of20_LFV13TeV_"+tag1+"_Systs_125_pulls_125.pdf "+thedir+"/NPrank.pdf")
          pdfmergecommand+=" NPrank.pdf"
    if (doNP==2 and tag1!=tag2 ):
          os.chdir(workdir)
          doNPrank(tag1,tag2)
          os.system("cp pdf-files/Bigjob."+extra+"_job20of20_LFV13TeV_"+tag1+"_Systs_125_pulls_125.pdf "+thedir+"/NPrank.pdf")
          pdfmergecommand+=" NPrank.pdf"

    os.chdir(thedir)
    os.system(pdfmergecommand)
	
    return name+"_figures.pdf"




if __name__ == "__main__":

 
    NPrankModeDescription = ["No NP rankings to collect","Collect NP rankings for given tag","Collect NP rankings for given tag and sub-channels","ONLY collect NP rankings (given tag only)"]
    if (len(sys.argv) < 2) or (len(sys.argv) > 4):
        print "Usage: collectOutput.py <tag> <NPrankmode> <optional: alsoSubchannels>"
        print "- tag is the tag used when submitting the jobs"
        print "- NPrankmode: 0 = "+NPrankModeDescription[0]+"; 1 = "+NPrankModeDescription[1]+"; 2 = "+NPrankModeDescription[2]+"; 3 = "+NPrankModeDescription[3]
        print "- subChannelMode: No = only collect for given tag / Short = only include significance info for subchannels / Long = also include NP plots for subchannels / Comp = perform subchannel comparison plots"
        exit(0)

    tag = sys.argv[1]
    print "Collecting output for tag: "+tag
    NPrankmode = int(sys.argv[2])
    print NPrankModeDescription[NPrankmode]
    subMode = sys.argv[3]
    alsoSubs=True
    if(subMode=="No"):
          alsoSubs =  False	

    if(alsoSubs==False and NPrankmode == 2):
          print "Inconsistent settings: subchannelMode is None but NPrankmode is 2 (="+NPrankModeDescription[2]+")"
          print "Review the arguments you are passing and try again! Quiting."
          sys.exit()


    if(NPrankmode==3):
          doNPrank(tag)
          print "NP rankings have been collected -- exiting"
          sys.exit()

    # Do some extra stuff relating to formatting for the support note section on fit results?
    notestuff = True


    # I assume the subchannel tags are defined as in the bigjob script!
    subtags = ["ll","lh","hh","vbf","bst","llvbf","lhvbf","hhvbf","llbst","lhbst","hhbst"]

    # If you are running in Comp mode, need to define here which sets to perform comparisons for!
    # MAX nr of entries in a set is 5 - and that is going to be really hard to read!
    compSets=[["Combination","vbf","bst"], ["Combination","ll","lh","hh"],["ll","llvbf","llbst"],["lh","lhvbf","lhbst"],["hh","hhvbf","hhbst"]]

    here=os.environ["PWD"]
    
    # Start by collecting from the combined workspace

    thepath =afsworkdir+"/analysis/statistics/batch/Bigjob."+tag+"/LFV13TeV_"+tag+"_Systs_125"

    # Copy over the template and script for producing a tex document                                                                                        
    os.system("cp $WORKDIR/scripts/SMHtautau/tabletemplate.tex "+thepath)
    os.system("cp $WORKDIR/scripts/SMHtautau/sigtemplate.tex "+thepath)

    #Untar!
    os.chdir(thepath)
    os.system("tar -xzf output.tar.gz")

    # Check status of fits in runSig
    grpcmd="grep status logs/output_Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125_getSig.log"
    #os.system(grpcmd)
    warn = " "
    fitstatus = subprocess.check_output(grpcmd, shell=True)
    print fitstatus
    if fitstatus.find("status = 1")!=-1 or fitstatus.find("status = 2")!=-1 or fitstatus.find("status = 3")!=-1 or fitstatus.find("status = 4")!=-1:
          print "*** WARNING! Fit issues!! *** \n"
          warn+=(tag+" ")

    # RunSig outputs
    sigfile = "root-files/Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125_obs_p0/125.root"
    obsSig,expSig,obsP0,expP0 = getsig(sigfile)

    oSig = "{0:.4f}".format(obsSig)#str(obsSig)
    eSig = "{0:.4f}".format(expSig)#str(expSig)
    oP0 = "{0:.4f}".format(obsP0)#str(obsP0)
    eP0 = "{0:.4f}".format(expP0)#str(expP0)
    tabcols = "c"

    tabtitle = "Combination"
    pdfstomerge = [ plotsToPdf(tag,tag,NPrankmode,thepath) ]
    fccpath="Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125"
    fccpaths ={"Combination":fccpath}
    if "Comp" in subMode:
          os.system("cp -r fccs/FitCrossChecks_"+fccpath+"_combined $WORKDIR/fccs/")

    if (alsoSubs==True):
          for t in subtags:
                subpath = afsworkdir+"/analysis/statistics/batch/Bigjob."+tag+"/"+t+"_LFV13TeV_"+tag+"_Systs_125"
                os.chdir(subpath)
                os.system("tar -xzf output.tar.gz")
                subgrpcmd="grep status logs/output_Bigjob."+tag+"_"+t+"_LFV13TeV_"+tag+"_Systs_125_getSig.log"
                subfitstatus = subprocess.check_output(subgrpcmd, shell=True)
                print subfitstatus
                if subfitstatus.find("status = 1")!=-1 or subfitstatus.find("status = 2")!=-1 or subfitstatus.find("status = 3")!=-1 or subfitstatus.find("status = 4")!=-1:
                      print "WARNING! Fit issues!! \n"
                      warn+=(t+" ")
                subfile = "root-files/Bigjob."+tag+"_"+t+"_LFV13TeV_"+tag+"_Systs_125_obs_p0/125.root"
                sobsSig,sexpSig,sobsP0,sexpP0 = getsig(subfile)
                oSig += "\\&"+"{0:.4f}".format(sobsSig)
                eSig += "\\&"+"{0:.4f}".format(sexpSig)
                oP0 += "\\&"+"{0:.4f}".format(sobsP0)
                eP0 += "\\&"+"{0:.4f}".format(sexpP0)
                tabcols += "|c"
                tabtitle += "\\&"+t
                if "Long" in subMode:
                      pdf=plotsToPdf(tag,t,NPrankmode,subpath)
                      os.system("cp "+pdf+" "+thepath)
                      pdfstomerge.append( pdf )
                if "Comp" in subMode:
                      fccpath="Bigjob."+tag+"_"+t+"_LFV13TeV_"+tag+"_Systs_125"
                      os.system("cp -r fccs/FitCrossChecks_"+fccpath+"_combined $WORKDIR/fccs/")
                      fccpaths[t]=fccpath
				
          print 'Done looping over the subchannels for the significance.'
          os.chdir(thepath)

    #Copy the mu breakdown tables to a more palatable filename
    #NB!! This file name will be different if the fit is not made with SigXsecOverSM as POI
    brkdn = "plots/Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125/breakdown/muHatTable_Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125_mode8_Asimov0_SigXsecOverSM.tex"
    os.system("cp "+brkdn+" breakdown.tex")
    
    #NP pulls table
    pullstex ="fccs/FitCrossChecks_Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125_combined/LatexFileNPs/GlobalFit_nuisPar_unconditionnal_mu1.tex"
    pullsfile = "NPpull.tex"
    # split the table as it is too long to fit in one page...                                                                                                    
    formatNPtable(pullstex,pullsfile)

    # Make the tables
    tabcommand="sed -e \"s=IDENTIFIER="+tag+"=g\" -e \"s=COLUMNS="+tabcols+"=g\" -e \"s=TITLE="+tabtitle+"=g\"  -e \"s=OBSSIG="+oSig+"=g\" -e \"s=EXPSIG="+eSig+"=g\" -e \"s=OBSP0="+oP0+"=g\" -e \"s=EXPP0="+eP0+"=g\"  sigtemplate.tex > significances.tex"
    #print tabcommand
    os.system(tabcommand)
    tabcommand2="sed -e \"s=IDENTIFIER="+tag+"=g\" tabletemplate.tex > yieldtable.tex"
    #print tabcommand2
    os.system(tabcommand2)
    os.system("pdflatex -interaction=batchmode yieldtable.tex")

    if "Comp" in subMode:	
          # Run the comparison scripts
          absdir=thepath+"/comp"
          compcommand = "python $WORKDIR/scripts/comparePulls.py 0 0 "
          it=0
          compcolor=["black", "red", "blue","magenta","gray"]
          for s in compSets:
                os.chdir(here)
                it+=1
                absdir=thepath+"/comp"+str(it)
                thiscomp = compcommand+absdir+" 0 None "
                legend="In the next plots, NP pulls for: "
                ll=0
                for w in s:
                      thiscomp+=fccpaths[w]
                      thiscomp+=" "
                      legend+=w+" ("+compcolor[ll]+") "
                      ll+=1
                os.system(thiscomp)
                os.chdir(absdir)
                os.system("cp $WORKDIR/scripts/SMHtautau/comptemplate.tex "+absdir)
                csed="sed -e \"s=IDENTIFIER="+legend+"=g\" comptemplate.tex > compcover.tex"
                os.system(csed)
                os.system("pdflatex -interaction=batchmode compcover.tex")
                pdfmerge = "gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -sOutputFile=comp"+str(it)+".pdf compcover.pdf "
                pdfmerge+="NP_all.pdf "
                pdfmerge+="NP_BTag.pdf "
                pdfmerge+="NP_Jet.pdf "
                pdfmerge+="NP_Lepton.pdf "
                pdfmerge+="NP_Suspicious.pdf "
                os.system(pdfmerge)
                pdfstomerge.append(absdir+"/comp"+str(it)+".pdf")
          os.chdir(thepath)
          print 'Done preparing the comparison plots'
 
    pdfcommand = "gs -dBATCH -dNOPAUSE -q -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -sOutputFile="+tag+"_output.pdf "

    pdfmergecommand=pdfcommand
    pdfmergecommand+="yieldtable.pdf "
    for p in pdfstomerge:
        pdfmergecommand +=p
        pdfmergecommand +=" "
    
#    pdfmergecommand+=tag+"_plots.pdf"
    #print pdfmergecommand
    os.system(pdfmergecommand)

 
    
    # Gather it all in one place
    directory = "output_"+tag
    texdir = directory+"/fitresults"
    os.system("mkdir -vp "+directory)
    os.system("mkdir -vp "+texdir)

    plotpath="plots/Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125"
    fccplots = plotpath+"/fcc/GlobalFit_unconditionnal_mu1/"

    os.system("cp tables/Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125/prefit/Yields_Prefit.tex "+texdir)
    os.system("cp tables/Bigjob."+tag+"_LFV13TeV_"+tag+"_Systs_125/postfit/Yields_GlobalFit_unconditionnal_mu1.tex "+texdir+"/Yields_Postfit.tex")
    os.system("cp "+plotpath+"/postfit/*.pdf"+" "+texdir)
    os.system("cp significances.tex "+texdir)
    os.system("cp breakdown.tex "+texdir)
    os.system("cp "+fccplots+"corr_origin.pdf "+texdir)
    os.system("cp "+fccplots+"corr_noMCStat.pdf "+texdir)
    os.system("cp "+fccplots+"corr_HighCorr.pdf "+texdir)
    os.system("cp "+fccplots+"NP_all.pdf "+texdir)
    os.system("cp "+fccplots+"NP_norm.pdf "+texdir)
    os.system("cp "+pullsfile+" "+texdir)
    os.system("cp "+tag+"_output.pdf "+directory)

    os.system("cp -r "+directory+" "+here)
    os.chdir(here)
    
    # Now, collect the NP ranking outputs and produce the plot
    # This is now done earlier, but still want to copy over the combined NP-rank plot to the tex-dir for the note!
    if(NPrankmode!=0):
          print "All other items collected, now collecting NP rankings"
          #doNPrank(tag)
          #Also collect subchannels?
          #if(NPrankmode == 2):
          #      for t in subtags:
          #            doNPrank(tag,t)

          
          os.system("cp pdf-files/Bigjob."+tag+"_job20of20_LFV13TeV_"+tag+"_Systs_125_pulls_125.pdf "+texdir+"/NPrank.pdf")
          # Also include the ranking plots in the output PDF file
          #os.system("mv "+directory+"/"+tag+"_output.pdf temp.pdf")
          #mergecommand = pdfcommand +" temp.pdf pdf-files/Bigjob."+tag+"*.pdf"
          #os.system(mergecommand)
          #os.system("mv "+tag+"_output.pdf "+directory)
          #os.system("rm temp.pdf")


    #Create a test of the section in the support note
    #if(notestuff and NPrankmode!=0):
    #      os.chdir(directory)
    #      os.system("cp $WORKDIR/scripts/SMHtautau/textest.tex .")
    #      os.system("pdflatex -interaction=batchmode textest.tex")




    print "All done here!"
    print "Outputs have been gathered in: "+here+"/"+directory
    print "Summary pdf-file: "+here+"/"+directory+"/"+tag+"_output.pdf "
    if(notestuff): 
          print "Directory with material for the note here:  "+here+"/"+texdir
    if(warn!=" "):
          print warn+" WARNING!! Fit issues!! Please investigate."
    sys.exit()
