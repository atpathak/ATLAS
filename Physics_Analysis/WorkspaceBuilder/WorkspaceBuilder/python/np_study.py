import ROOT
import pickle


ROOT.Math.MinimizerOptions.SetDefaultPrintLevel(-1)



def get_np_constraint(np_name, workspace_file):

    if not isinstance(np_name, (list, tuple)):
        np_name = [np_name]

    rfile = ROOT.TFile(workspace_file)
    ws_name = 'combined'
    mc_name = 'ModelConfig'
    data_name = 'obsData'

    workspace = rfile.Get(ws_name)
    mc = workspace.obj(mc_name)
    data = workspace.data(data_name)

    nuisance_parameters = mc.GetNuisanceParameters()
    globs = mc.GetGlobalObservables()
    

    nll = mc.GetPdf().createNLL(
        data, 
        ROOT.RooFit.Constrain(nuisance_parameters), 
        ROOT.RooFit.GlobalObservables(globs), 
        ROOT.RooFit.Offset(1), 
        ROOT.RooFit.NumCPU(4, ROOT.RooFit.Hybrid), 
        ROOT.RooFit.Optimize(2))
    
    miminizer = ROOT.RooMinimizer(nll)
    status = miminizer.minimize(
        ROOT.Math.MinimizerOptions.DefaultMinimizerType(), 
        ROOT.Math.MinimizerOptions.DefaultMinimizerAlgo())

    # arg_set_for_minos = ROOT.RooArgSet()
    # for name in np_names:
    #     arg_set_for_minos.add(nuisance_parameters.find(name))
    # miminizer.minos(arg_set_for_minos)

    np_dict = {}
    for name in np_names:
        nuip = nuisance_parameters.find(name)
        if isinstance(nuip, ROOT.RooRealVar):
            print name
            print type(nuip)
            print nuip.getVal()
            print nuip.getErrorHi()
            print nuip.getErrorLo()
            # nom, low, high =  nuip.getVal(), nuip.getErrorLo(), nuip.getErrorHi()
            np_dict[name] = (nuip.getVal(), nuip.getErrorLo(), nuip.getErrorHi())
        else:
            np_dict[name] = (0, 0, 0)
    rfile.Close()
    return np_dict

# rfile = ROOT.TFile('workspaces/optimal/combined/125.root')
# rfile.ls()
# print get_np_constraint('jet_jer_np0', 'workspaces/optimal/combined/125.root')

if __name__ == '__main__':

    # directory = '/afs/cern.ch/atlas/project/HSG4/HtautauRun2/combination/Sept28SingleMuAsimov/{}/combined/125.root'
    directory = './workspaces/boost_rebin/{}/combined/125.root'
    directory_split = '/afs/cern.ch/work/c/coniavit/public/SplitForQuentin/{}/combined/125.root'

    fit_dict = {}
    fit_dict['all'] = {}
    fit_dict['vbf'] = {}
    fit_dict['boost'] = {}
    fit_dict['boost loose'] = {}
    fit_dict['boost tight'] = {}

    fit_dict['all']  ['all']    = directory.format('HTauTau13TeVCombinationCBA')
    fit_dict['vbf']  ['all']    = directory.format('HTauTau13TeVCombinationVBFCBA')
    fit_dict['boost']['all']    = directory.format('HTauTau13TeVCombinationBoostCBA')
    fit_dict['all']  ['hadhad'] = directory.format('HTauTau13TeVHHCBA')
    fit_dict['vbf']  ['hadhad'] = directory.format('HTauTau13TeVHHVBFCBA')
    fit_dict['boost']['hadhad'] = directory.format('HTauTau13TeVHHBoostCBA')
    fit_dict['all']  ['lephad'] = directory.format('HTauTau13TeVLHCBA')
    fit_dict['vbf']  ['lephad'] = directory.format('HTauTau13TeVLHVBFCBA')
    fit_dict['boost']['lephad'] = directory.format('HTauTau13TeVLHBoostCBA')
    fit_dict['all']  ['lephad'] = directory.format('HTauTau13TeVLHCBA')
    fit_dict['vbf']  ['lephad'] = directory.format('HTauTau13TeVLHVBFCBA')
    fit_dict['boost']['lephad'] = directory.format('HTauTau13TeVLHBoostCBA')
    fit_dict['all']  ['leplep'] = directory.format('HTauTau13TeVLLCBA')
    fit_dict['vbf']  ['leplep'] = directory.format('HTauTau13TeVLLVBFCBA')
    fit_dict['boost']['leplep'] = directory.format('HTauTau13TeVLLBoostCBA')
    
    # fit_dict['boost loose']['hadhad'] = directory_split.format('HTauTau13TeVHHBoostLooseCBA')
    # fit_dict['boost tight']['hadhad'] = directory_split.format('HTauTau13TeVHHBoostTightCBA')
    # fit_dict['boost loose']['lephad'] = directory_split.format('HTauTau13TeVLHBoostLooseCBA')
    # fit_dict['boost tight']['lephad'] = directory_split.format('HTauTau13TeVLHBoostTightCBA')

    np_names = [
        'ZttTheory_CKK',
        'ZttTheory_qsf',
        'ATLAS_TAU_EFF_ID_TOTAL',
        'ATLAS_TAU_TES_MODEL',
        'ATLAS_TAU_TES_INSITU',
        'ATLAS_JES_PU_Rho',
        'jet_jer_np0',
        'jet_jer_np1',
        'jet_jer_np2',
        'jet_jer_np3',
        'jet_jer_np4',
        'jet_jer_np5',
        'ATLAS_PRW_DATASF',
        ]


    result_dict = {}
    for k in fit_dict.keys():
        result_dict[k] = {}
        for t, o in fit_dict[k].items():
            print o
            result_dict[k][t] = get_np_constraint(np_names, o)

    print result_dict
    with open('pulls_dict.pkl', 'wb') as f:
        pickle.dump(result_dict, f, pickle.HIGHEST_PROTOCOL)
