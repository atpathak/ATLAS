import ROOT
import array
import itertools
import os

#--------------------------------------------------------
def define_edge(init_binning, pos='left'):
    mid = len(init_binning) / 2
    binnings = []
    for i_p in range(2, mid + 1):
        if pos == 'left':
            binnings.append({
                    'name': 'left_{}'.format(init_binning[i_p]),
                    'bins': array.array('d', [init_binning[0]] + init_binning[i_p:])})
        elif pos == 'right':
            binnings.append({
                    'name': 'right_{}'.format(init_binning[mid + i_p]),
                    'bins': array.array('d', init_binning[:mid + i_p] + [init_binning[-1]])})
        else:
            raise ValueError('wrong pos')
    return binnings


#--------------------------------------------------------
def copy_dir(input_dir, rebin=False, binning=None):
    sav_dir = ROOT.gDirectory.GetDirectory('')
    adir = sav_dir.mkdir(input_dir.GetName())
    adir.cd()
    for key in input_dir.GetListOfKeys():
        cl = ROOT.gROOT.GetClass(key.GetClassName())
        if cl.InheritsFrom(ROOT.TDirectory.Class()):
            input_dir.cd(key.GetName())
            subdir = ROOT.gDirectory.GetDirectory('')
            adir.cd()
            copy_dir(subdir, rebin=rebin, binning=binning)
            adir.cd()
        else:
            input_dir.cd()
            obj = key.ReadObj()
            adir.cd()
            if rebin and isinstance(obj, ROOT.TH1F) and (obj.GetName() != 'lumiininvpb'):
                rebinned_hist = obj.Rebin(len(binning) - 1, obj.GetName(), binning)
                rebinned_hist.Write()
            else:
                obj.Write()
            del obj
    adir.SaveSelf(ROOT.kTRUE)
    sav_dir.cd()

#--------------------------------------------------------
def rebin_input(input_file, cat, name, binning, output_dir='./'):

    output_name = os.path.join(
        output_dir, os.path.basename(input_file.GetName()))
    output_name = output_name.replace('.root', '_{}_{}.root'.format(cat, name))
    print 'output:', output_name
    rebinned_rfile = ROOT.TFile.Open(output_name, 'recreate')
    rebinned_rfile.cd()
    target = rebinned_rfile.GetDirectory('')
    print 'target: ', target
    print 'open source:', input_file.GetDirectory('')
    target.cd()
    for k in input_file.GetListOfKeys():
        cl = ROOT.gROOT.GetClass(k.GetClassName())
        if cl.InheritsFrom(ROOT.TDirectory.Class()):
            copy_dir(
                input_file.GetDirectory(k.GetName()), 
                rebin=(k.GetName() == cat), 
                binning=binning)
    target.Write()
    target.Close()
    return output_name


#----------------------------
def init_binnings(input_file, category):
    rfile = ROOT.TFile.Open(input_file, 'read')
    hist = rfile.Get('{}/Ztt/nominal'.format(category))
    binning = [hist.GetBinLowEdge(ibin +  1) for ibin in xrange(hist.GetNbinsX() + 1)]
    rfile.Close()
    return binning

#-------------------------------------------------
def build_workspace(name, ws_input_path, ws_path, statanalysisname='HTauTau13TeVHHVBFCBA'):
    """
    """
    statanalysisname = statanalysisname
    version = ws_path
    xmlpaths               = ROOT.vector('string')()
    rootpaths              = ROOT.vector('string')()
    normfactorpaths        = ROOT.vector('string')()
    overallsystematicpaths = ROOT.vector('string')()
    parameters             = ROOT.map('string', 'string')()

    xmlpaths.push_back('data/Htautau/HTauTau13TeVCombinationCBA.xml')
    xmlpaths.push_back('data/Htautau/HTauTau13TeVPOISingleMu.xml')
    # rootpaths.push_back('/eos/user/q/qbuat/workspaces/htt_fine_binning/hhAll_merged_WSinput_WithTheoryEnv_WithModJERVar.root')
    rootpaths.push_back(ws_input_path)
    normfactorpaths.push_back('./data/Htautau/normfactors13TeV.txt')
    overallsystematicpaths.push_back('./data/Htautau/overallsystematics13TeV.txt')

    parameters['BlindFullWithFloat'] = ''
    parameters['AggressiveSymmLocal'] =  ''

    lumi = -1
    unblind = False
    info = False
    mcstat = True
    outputpath = 'workspaces'

    user_tools = ROOT.WorkspaceBuilder.HTauTauSMCouplingUserTools(
        statanalysisname, './workspaces', version, 
        rootpaths, normfactorpaths, overallsystematicpaths, parameters, unblind, info)

    stat_ana = ROOT.WorkspaceBuilder.StatAnalysis(
        statanalysisname, version, True)

    stat_ana.setOutputPath(outputpath)
    stat_ana.readInformationFromXML(xmlpaths, user_tools, mcstat)
    stat_ana.readHistogramsFromRootFile(user_tools)
    stat_ana.deriveHistograms(user_tools, lumi)
    stat_ana.pruneSystematics(user_tools)
    stat_ana.deriveFinalHistograms(user_tools)
    stat_ana.writeHistogramsToRootFile()
    stat_ana.createModelConfig(user_tools)
    stat_ana.writeWorkspace(user_tools)
    stat_ana.writeXML()
    stat_ana.analyze(user_tools)
#--------------------------------------



if __name__ == '__main__':

    ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')
    ROOT.SIG.DummyStruct()
    from argparse import ArgumentParser

    parser = ArgumentParser()
    parser.add_argument('rfile')
    parser.add_argument('--category', default='hhAll_cba_vbf_lowdr_signal')
    parser.add_argument('--statanalysisname', default='HTauTau13TeVHHVBFCBA')
    parser.add_argument('--position', default='left')
    parser.add_argument('--channel', default='hh', choices=['ll', 'lh', 'hh'])
    parser.add_argument('--outdir', default='./workspaces')
    parser.add_argument('--binning', nargs='+', type=float)
    parser.add_argument('--suffix')
    args = parser.parse_args()

    rfile = args.rfile
    cat = args.category
    pos = args.position
    channel = args.channel
    outdir = args.outdir
    binning = args.binning
    binning = array.array('d', binning)
    binning_name = args.suffix
    statanalysisname = args.statanalysisname
    # categories = [
    #     'hhAll_cba_vbf_highdr_loose_signal',
    #     'hhAll_cba_vbf_highdr_tight_signal',
    #     'hhAll_cba_vbf_lowdr_signal',
    #     ]
    
    # positions = [
    #     'left', 
    #     'right'
    #     ]
    
    # rfile = '/eos/user/q/qbuat/workspaces/htt_fine_binning/hhAll_merged_WSinput_WithTheoryEnv_WithModJERVar.root'
    input_file = ROOT.TFile.Open(rfile, 'read')
    starting_bins = init_binnings(rfile, cat)
    print 'Starting binning: {}'.format(starting_bins)
    print 'category: {}'.format(cat)
    print 'Statistical analysis: {}'.format(statanalysisname)
    print 'optimize the {} bound'.format(pos)
    print 'channel: {}'.format(channel)
    print 'binning name: {}'.format(binning_name)
    print 'binning: {}'.format(binning)
    print 'output directory: {}'.format(outdir)

    ws_input = rebin_input(input_file, cat, binning_name, binning, output_dir='./workspaces/')
    print 'ws_input', ws_input
            
    ws_path = '{}_{}'.format(cat, binning_name)
    #ws_input = './workspaces/hhAll_merged_WSinput_WithTheoryEnv_WithModJERVar_{}.root'.format(ws_path)
    build_workspace(statanalysisname, ws_input, ws_path, statanalysisname)
    hist = ROOT.SIG.runSig("workspaces/{}/combined/125.root".format(ws_path), 
                           "combined", 
                           "ModelConfig", 
                           "obsData", 
                           "asimovData_1", 
                           "conditionalGlobs_1", 
                           "nominalGlobs", 
                           "125", 
                           "test", 0)

    hist.SaveAs('./workspaces/hist_sig_'+ws_input.split("/")[-1])
    for ibin in range(0, hist.GetNbinsX() + 1):
        print hist.GetXaxis().GetBinLabel(ibin), ': ', hist.GetBinContent(ibin)

