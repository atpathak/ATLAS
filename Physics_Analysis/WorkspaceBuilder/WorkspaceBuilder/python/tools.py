import ROOT

#-------------------------------------------------
def build_workspace(
    ws_input_path, 
    ws_output, 
    xmls,
    statanalysisname='HTauTau13TeVCombinationCBA',
    normfactors='./data/Htautau/normfactors13TeV.txt',
    overallsys='./data/Htautau/overallsystematics13TeV.txt',
    outputpath='./workspaces',
    mcstat=True,
    unblind=False,
    info=False):
    """
    parameters
    ----------
    ws_input_path: str or list(str) of input files
    ws_output: str name of output directory
    xmls: str or list(str) of input xml files (channels/samples/poi config)
    statanalysisname: str of the statanalysis name stored in the dedicated xml
    normfactors: str of file storing the norm factors
    overallsys: str of file storing the overall systematics
    outputpath: str of the directory where ws_output will be created
    mcstat: bool to turn on / off the mc stat gamma parameters
    unblind: bool
    info: bool
    """

    version                = ws_output
    xmlpaths               = ROOT.vector('string')()
    rootpaths              = ROOT.vector('string')()
    normfactorpaths        = ROOT.vector('string')()
    overallsystematicpaths = ROOT.vector('string')()
    parameters             = ROOT.map('string', 'string')()

    if isinstance(xmls, (list, tuple)):
        for xml in xmls:
            xmlpaths.push_back(xml)
    else:
        xmlpaths.push_back(xmls)

    if isinstance(ws_input_path, (list, tuple)):
        for wip in ws_input_path:
            rootpaths.push_back(wip)
    else:
        rootpaths.push_back(ws_input_path)

    normfactorpaths.push_back(normfactors)
    overallsystematicpaths.push_back(overallsys)

    parameters['BlindFullWithFloat'] = ''
    parameters['AggressiveSymmLocal'] =  ''

    lumi = -1

    user_tools = ROOT.WorkspaceBuilder.HTauTauSMCouplingUserTools(
        statanalysisname, outputpath, version, 
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
