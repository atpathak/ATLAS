#!/usr/bin/env python

import sys
import os
import AnalysisMgr_Htautau as Mgr
import BatchMgr as Batch

# This script submits jobs for a combined workspace (runSig, FCC, NPrank) and for individual channels (runSig and optionally NPrank)

InputVersion = "Bigjob"

def add_config(mass, unc = "Systs", one_bin=False,mcstats=True, inclusive=True, debug=False):
    conf = Mgr.WorkspaceConfig_Htautau(unc, InputVersion=InputVersion, MassPoint=mass, OneBin=one_bin, UseStatSystematics=mcstats, MergeSamples=True, Debug=debug)
    #conf.set_channels(channel)
    conf.set_regions(inclusive=inclusive)
    fullversion = "LFV13TeV_" + outversion + "_" + unc + "_" + mass
    return conf.write_configs(fullversion)

if __name__ == "__main__":

    if len(sys.argv) is not 3:
        print "Usage: Bigjob_Htautau.py <outversion> <WSlocation>"
        exit(0)

    outversion = sys.argv[1]
    inputdir = sys.argv[2]

    # Give path of combined WS here:
    WSlink = "/afs/cern.ch/user/a/atpathak/afswork/public/WSMaker/"+inputdir+"/LFV13TeVCombinationCBA/combined/125.root"
    
    os.system("ls -lh " + WSlink)

    # Give paths of lh/ll/hh individual WSes here:
    WSind = {
       # "lh":inputdir+"/LFV13TeVCombinationCBA/combined/125.root",
        #"ll":inputdir+"/LFV13TeVCombinationCBA/combined/125.root",
        #"hh":inputdir+"/HTauTau13TeVHHCBA/combined/125.root",
        #"vbf":inputdir+"/HTauTau13TeVCombinationVBFCBA/combined/125.root",
        #"bst":inputdir+"/HTauTau13TeVCombinationBoostCBA/combined/125.root",
        #"llvbf":inputdir+"/HTauTau13TeVLLVBFCBA/combined/125.root",
        #"lhvbf":inputdir+"/HTauTau13TeVLHVBFCBA/combined/125.root",
        #"hhvbf":inputdir+"/HTauTau13TeVHHVBFCBA/combined/125.root",
        #"llbst":inputdir+"/HTauTau13TeVLLBoostCBA/combined/125.root",
        #"lhbst":inputdir+"/HTauTau13TeVLHBoostCBA/combined/125.root",
        #"hhbst":inputdir+"/HTauTau13TeVHHBoostCBA/combined/125.root",
        # HH + LH combination
        #        "hhlhbst":inputdir+"/HTauTau13TeVCombHHLHBoostCBA/combined/125.root",
        #        "hhlhvbf":inputdir+"/HTauTau13TeVCombHHLHVBFCBA/combined/125.root",
        # SR only fitting
        #        "srcomb":inputdir+"/HTauTau13TeVCombinationSRCBA/combined/125.root",
        #        "srcombbst":inputdir+"/HTauTau13TeVCombinationBoostSRCBA/combined/125.root",
        #        "srcombvbf":inputdir+"/HTauTau13TeVCombinationVBFSRCBA/combined/125.root",
        #        "srll":inputdir+"/HTauTau13TeVLLSRCBA/combined/125.root",
        #        "srllbst":inputdir+"/HTauTau13TeVLLBoostSRCBA/combined/125.root",
        #        "srllvbf":inputdir+"/HTauTau13TeVLLVBFSRCBA/combined/125.root",
        #        "srlh":inputdir+"/HTauTau13TeVLHSRCBA/combined/125.root",
        #        "srlhbst":inputdir+"/HTauTau13TeVLHBoostSRCBA/combined/125.root",
        #        "srlhvbf":inputdir+"/HTauTau13TeVLHVBFSRCBA/combined/125.root",
        #        "srhh":inputdir+"/HTauTau13TeVHHSRCBA/combined/125.root",
        #        "srhhbst":inputdir+"/HTauTau13TeVHHBoostSRCBA/combined/125.root",
        #        "srhhvbf":inputdir+"/HTauTau13TeVHHVBFSRCBA/combined/125.root",
        # CR only fitting
        #        "crcomb":inputdir+"/HTauTau13TeVCombinationCRCBA/combined/125.root",
        #        "crcombbst":inputdir+"/HTauTau13TeVCombinationBoostCRCBA/combined/125.root",
        #        "crcombvbf":inputdir+"/HTauTau13TeVCombinationVBFCRCBA/combined/125.root",
        #        "crll":inputdir+"/HTauTau13TeVLLCRCBA/combined/125.root",
        #        "crllbst":inputdir+"/HTauTau13TeVLLBoostCRCBA/combined/125.root",
        #        "crllvbf":inputdir+"/HTauTau13TeVLLVBFCRCBA/combined/125.root",
        #        "crlh":inputdir+"/HTauTau13TeVLHCRCBA/combined/125.root",
        #        "crlhbst":inputdir+"/HTauTau13TeVLHBoostCRCBA/combined/125.root",
        #        "crlhvbf":inputdir+"/HTauTau13TeVLHVBFCRCBA/combined/125.root",
        # Individual SR (i.e. no CRs)
        #        "srllbstlse":inputdir+"/HTauTau13TeVLLBoostLooseSRCBA/combined/125.root",
        #        "srllbsttgt":inputdir+"/HTauTau13TeVLLBoostTightSRCBA/combined/125.root",
        #        "srllvbflse":inputdir+"/HTauTau13TeVLLVBFLooseSRCBA/combined/125.root",
        #        "srllvbftgt":inputdir+"/HTauTau13TeVLLVBFTightSRCBA/combined/125.root",
        #        "srlhbstlse":inputdir+"/HTauTau13TeVLHBoostLooseSRCBA/combined/125.root",
        #        "srlhbsttgt":inputdir+"/HTauTau13TeVLHBoostTightSRCBA/combined/125.root",
        #        "srlhvbflse":inputdir+"/HTauTau13TeVLHVBFLooseSRCBA/combined/125.root",
        #        "srlhvbftgt":inputdir+"/HTauTau13TeVLHVBFTightSRCBA/combined/125.root",
        #        "srhhbstlse":inputdir+"/HTauTau13TeVHHBoostLooseSRCBA/combined/125.root",
        #        "srhhbsttgt":inputdir+"/HTauTau13TeVHHBoostTightSRCBA/combined/125.root",
        #        "srhhvbflowdr":inputdir+"/HTauTau13TeVHHVBFLowDRSRCBA/combined/125.root",
        #        "srhhvbfhighdrlse":inputdir+"/HTauTau13TeVHHVBFHighDRLooseSRCBA/combined/125.root",
        #        "srhhvbfhighdrtgt":inputdir+"/HTauTau13TeVHHVBFHighDRTightSRCBA/combined/125.root",
        # Individual SR with corresponding CR(s)
        #        "llbstlse":inputdir+"/HTauTau13TeVLLBoostLooseCBA/combined/125.root",
        #        "llbsttgt":inputdir+"/HTauTau13TeVLLBoostTightCBA/combined/125.root",
        #        "llvbflse":inputdir+"/HTauTau13TeVLLVBFLooseCBA/combined/125.root",
        #        "llvbftgt":inputdir+"/HTauTau13TeVLLVBFTightCBA/combined/125.root",
        #        "lhbstlse":inputdir+"/HTauTau13TeVLHBoostLooseCBA/combined/125.root",
        #        "lhbsttgt":inputdir+"/HTauTau13TeVLHBoostTightCBA/combined/125.root",
        #        "lhvbflse":inputdir+"/HTauTau13TeVLHVBFLooseCBA/combined/125.root",
        #        "lhvbftgt":inputdir+"/HTauTau13TeVLHVBFTightCBA/combined/125.root",
        #        "hhbstlse":inputdir+"/HTauTau13TeVHHBoostLooseCBA/combined/125.root",
        #        "hhbsttgt":inputdir+"/HTauTau13TeVHHBoostTightCBA/combined/125.root",
        #        "hhvbflowdr":inputdir+"/HTauTau13TeVHHVBFLowDRCBA/combined/125.root",
        #        "hhvbfhighdrlse":inputdir+"/HTauTau13TeVHHVBFHighDRLooseCBA/combined/125.root",
        #        "hhvbfhighdrtgt":inputdir+"/HTauTau13TeVHHVBFHighDRTightCBA/combined/125.root",
        # W/o Ztt CR
        #        "combnozttcr":inputdir+"/HTauTau13TeVCombinationNoZttCRCBA/combined/125.root",
        #        "bstnozttcr":inputdir+"/HTauTau13TeVCombinationBoostNoZttCRCBA/combined/125.root",
        #        "vbfnozttcr":inputdir+"/HTauTau13TeVCombinationVBFNoZttCRCBA/combined/125.root",
        #        "llnozttcr":inputdir+"/HTauTau13TeVLLNoZttCRCBA/combined/125.root",
        #        "llbstnozttcr":inputdir+"/HTauTau13TeVLLBoostNoZttCRCBA/combined/125.root",
        #        "llvbfnozttcr":inputdir+"/HTauTau13TeVLLVBFNoZttCRCBA/combined/125.root",
        #        "lhnozttcr":inputdir+"/HTauTau13TeVLHNoZttCRCBA/combined/125.root",
        #        "lhbstnozttcr":inputdir+"/HTauTau13TeVLHBoostNoZttCRCBA/combined/125.root",
        #        "lhvbfnozttcr":inputdir+"/HTauTau13TeVLHVBFNoZttCRCBA/combined/125.root",
        #        "hhnozttcr":inputdir+"/HTauTau13TeVHHNoZttCRCBA/combined/125.root",
        #        "hhbstnozttcr":inputdir+"/HTauTau13TeVHHBoostNoZttCRCBA/combined/125.root",
        #        "hhvbfnozttcr":inputdir+"/HTauTau13TeVHHVBFNoZttCRCBA/combined/125.root",
        }


    for k,w in WSind.iteritems():
        os.system("ls -lh "+w)
    
    # Should we run NPrank jobs for the individual WSes?
    runIndNPrank = False


    ########## create config files  

    print "Adding config files..."
    configfiles_stat = []
    configfiles = []
    masses = [125]

    for m in masses:
        configfiles_stat += add_config(str(m), "StatOnly")
        configfiles      += add_config(str(m), "Systs", one_bin=False, mcstats=False, inclusive=True, debug=True)

    for fi in configfiles_stat:
        print fi
    for fi in configfiles:
        print fi

    #sys.exit()

    ########## define tasks for combined WS
    # check in scripts/doActions.py for full definition of the options    

    tasks = ["-w"]                      # create workspace (do always)
    try:
        tasks += ["-k", WSlink]         # link workspace instead of creating one (requires -w)
    except NameError:
        pass
    #tasks += ["-l", "1,{MassPoint}"]   # limit
    tasks += ["-s", 0]                  # significance -- 0=unblind, 1=blind
    tasks += ["--fcc", "2@{MassPoint}"] # fitCrossChecks -- number before @ refers to options in scripts/runFitCrossCheck.py 2=unconditional global fit to data; 7=unconditional global fit to asimov(mu=1)
    tasks += ["-u", "0,{MassPoint},8"]  # breakdown of muhat errors stat/syst/theory -- 0=obs, 1=exp(mu=1.0); mode 8 corresponds to SMHtautau NPs
    # requires fitCrossChecks:
    tasks += ["-m", "2"]                # reduced diag plots (quick) -- numbers should be same as for fcc
    #tasks += ["-b", "2"]               # unfolded b-tag plots (quick)
    tasks += ["-p", "2@{MassPoint}"]    # post-fit plots (CPU intense)
    tasks += ["-t", "0,2@{MassPoint}"]  # yield tables (CPU intense) -- 0=prefit, 1=bkg-only postfit, 2=s+b postfit

    tasksNPR = ["-w"]
    tasksNPR += ["-k", WSlink]
    tasksNPR += ["-r","{MassPoint}"]    #Run NP ranking

    ########## submit jobs

    # in case you want to run something locally
    #Batch.run_local_batch(configfiles, outversion, tasks)
    
    #Run the main tasks on the batch system as a single job. 
    Batch.run_lxplus_batch(configfiles, outversion, tasks,'2nw')

    #Split the NPrank jobs into subjobs.
    #Batch.run_lxplus_batch(configfiles, outversion, tasksNPR,'1nw',jobs=20)


    #Run fit on individual WSes
    #If NP ranking plots are requested, then these will be submitted to the batch system split into subjobs
    for k,w in WSind.iteritems():
        outname = outversion + "_" + k
        print outname
        if runIndNPrank:
            Batch.run_lxplus_batch(configfiles, outname, ["-w","-k",w,"-r","{MassPoint}"],jobs=10)
        Batch.run_lxplus_batch(configfiles, outname, ["-w","-k",w,"-s", 0,"--fcc", "2@{MassPoint}","-m", "2"],'2nd')
        #Batch.run_lxplus_batch(configfiles, outname, ["-w","-k",w,"-s", 0],'2nd')

    sys.exit()
    #Batch.run_local_batch(configfiles_stat, outversion, ["-w", "-l", 1])

    # adjust to recover failed ranking subjobs
    redoSubJob = -1

    if redoSubJob < 0:
        print "Submitting stat only jobs..."
        Batch.run_lxplus_batch(configfiles_stat, outversion, ["-w", "-l", 1,"-s",1], '2nd')
        print "Submitting systs jobs..."
        Batch.run_lxplus_batch(configfiles, outversion, tasks, '2nd')
    print "Submitting rankings..."
    Batch.run_lxplus_batch(configfiles, outversion, ["-w", "-r", "125"], '2nd', jobs=20, subJob=redoSubJob)
    #Batch.run_lxplus_batch(configfiles, outversion, ["-w", "-k", WSlink, "-r", "125"], '2nd', jobs=20, subJob=redoSubJob)

    ########## non-default stuff. Warning: don't submit rankings / toys / NLL scans with the same outversion and nJobs!
    #print "Submitting toys..."
    #Batch.run_lxplus_batch(configfiles, outversion, ["-w", "-k", WSlink, "--fcc", "6"], '2nd', jobs=50, subJob=redoSubJob)
    #print "Submitting NLL scans..."
    #Batch.run_lxplus_batch(configfiles, outversion, ["-w", "-k", WSlink, "--fcc", "7,2,doNLLscan"], '2nd', jobs=50, subJob=redoSubJob)

#  LocalWords:  outversion
