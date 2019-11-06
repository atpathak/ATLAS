#!/usr/bin/env python

import argparse
import re
from QFramework import *

parser = argparse.ArgumentParser(
  description="Duplicate nominal samples to systematic variations.",
  formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument("--src", help="Source channel name.")
parser.add_argument("--dest", help="Destination channel name.")
parser.add_argument("--pattern", help="Input and output file pattern.")
parser.add_argument("paths", metavar="PATH", nargs="+",
  help="Sample path with ? channel placeholder to copy.")

args = parser.parse_args()

def sanitize(name):
  return re.sub("[^~a-zA-Z0-9_.-]", "_", name)

for path in args.paths:
  input_path = path.replace('?', args.src)
  input_file = args.pattern % sanitize(input_path)

  output_path = path.replace('?', args.dest)
  output_file = args.pattern % sanitize(output_path)
  output_file = output_file.split(":")[0]

  print "%s --> %s" % (input_path, output_path)

  samples = TQSampleFolder.loadSampleFolder(input_file)
  for folder in samples.getListOfSampleFolders():
    channel_folder = folder.getSampleFolder(args.src)
    channel_folder.setName(args.dest)

  samples.writeToFile(output_file)
