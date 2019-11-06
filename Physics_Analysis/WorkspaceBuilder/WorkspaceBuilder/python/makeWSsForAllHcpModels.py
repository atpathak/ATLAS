import subprocess
from argparse import ArgumentParser
import sys
import os
import shutil
import logging

"""
What this script does has been replaced by the '-p Theta=xx' option and other
scripts. Will be deleted soon

"""


def run(templateXML, rootfile, normfactors, tag):

    logging.debug('Input XML: %s', templateXML)

    thetas = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170]
    for theta in thetas:

        outxml = templateXML.replace('.xml', '_theta_%d.xml' % theta)
        logging.debug('Output XML: %s', outxml)

        # Create the new xml file
        with open(templateXML, 'r') as template:
            with open(outxml, 'w') as target:
                target.write(template.read().replace('theta_X', 'theta_%d' % theta))

        name = "HTauTau13TeVHH"
        version = "HCPCombination_theta_%d" % theta
        if tag:
            version += '_' + tag

        # The BuildWorkspace command
        command = ['BuildWorkspace', '--cp', '-s', name, '-v', version, 
                   '-x', outxml, '-r', rootfile, '-n', normfactors,
                   '-p', 'FlatMultijet',
                   '-p', 'BlindFullWithFloat']

        logging.debug('Running command: %s', ' '.join(command))

        # Run, and print the output
        res = ''
        try:
            res = subprocess.check_output(command, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as e:
            print e.output
            logging.critical('BuildWorkspace returned %d, exiting', e.returncode)
            sys.exit()

        print res
        logging.debug('BuildWorkspace returned successfully.')
        print ''

        # Don't need the xml anymore (but keep if debugging)
        if not logging.getLogger().getEffectiveLevel() == logging.DEBUG:
            os.remove(outxml)



if __name__ == '__main__':

    parser = ArgumentParser()
    parser.add_argument('-t', help='Tag to append to output (e.g. \'vbf_only\')', default='')
    parser.add_argument('-x', help='XML template file', default=None)
    parser.add_argument('-r', help='Histogram input file', default=None)
    parser.add_argument('-n', help='Normfactors file', default='../data/normfactorsHCP.txt')
    parser.add_argument('-o', help='Output destination', default='../workspaces')
    parser.add_argument('-d', help='Debug output', action='store_true')
    args = parser.parse_args()
    
    print 'This script is deprecated and will be removed, try out the new option \'BuildWorkspace -p Theta=xx\' instead'

    if args.d:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)

    # Check that the files exists
    for f in [args.x, args.r, args.n]:
        if not os.path.isfile(f):
            print 'File doesn\'t exist: %s' % f
            sys.exit()
        else:
            logging.debug('Found file %s', f)

    # Run
    run(args.x, args.r, args.n, args.t)

    # Move results to desired destination (default: ../workspaces)
    if not os.path.exists(args.o):
        os.makedirs(args.o)
        logging.debug('Created output directory %s', args.o)
    else:
        logging.debug('Output directory %s already exist, will use this', args.o)
    
    for d in os.listdir('workspaces'):
        d = 'workspaces/'+d
        logging.debug('Moving %s to %s', d, args.o)
        shutil.move(d, args.o)

    shutil.rmtree('workspaces')

    logging.info('All workspaces saved in %s.', args.o)
