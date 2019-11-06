import ROOT
import array
import itertools
import os

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
def rebin_input(input_file, name, binnings, output_dir='./'):

    output_name = os.path.join(
        output_dir, os.path.basename(input_file.GetName()))
    output_name = output_name.replace('.root', '_{}.root'.format(name))
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
                rebin=(k.GetName() in binnings.keys()), 
                binning=binnings[k.GetName()] if k.GetName() in binnings.keys() else None)
    target.Write()
    target.Close()



if __name__ == '__main__':

    ROOT.gROOT.Macro('$ROOTCOREDIR/scripts/load_packages.C')
    ROOT.SIG.DummyStruct()
    from argparse import ArgumentParser

    parser = ArgumentParser()
    parser.add_argument('rfile')
    parser.add_argument('--channel', default='hh', choices=['ll', 'lh', 'hh'])
    parser.add_argument('--outdir', default='./workspaces')
    args = parser.parse_args()

    rfile = args.rfile
    channel = args.channel
    outdir = args.outdir

    

    categories = [
        'hhAll_cba_vbf_highdr_loose_signal',
        'hhAll_cba_vbf_highdr_tight_signal',
        'hhAll_cba_vbf_lowdr_signal',
        ]

    binnings = {
        'hhAll_cba_vbf_highdr_loose_signal': array.array('d', [0, 80, 100, 120, 140, 160, 200]),
        'hhAll_cba_vbf_highdr_tight_signal': array.array('d', [0, 80, 100, 120, 140, 160, 200]),
        'hhAll_cba_vbf_lowdr_signal':        array.array('d', [0, 80, 100, 120, 140, 160, 200]),
        }
    

    
    input_file = ROOT.TFile.Open(rfile, 'read')
    print 'output directory: {}'.format(outdir)

    rebin_input(input_file, 'rebinned', binnings, output_dir=outdir)
    print 'huh?'
            
