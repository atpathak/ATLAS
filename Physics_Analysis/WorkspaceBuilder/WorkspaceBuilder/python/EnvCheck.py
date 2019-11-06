##
# check environment/setup
# zenon@cern.ch
#
import sys, subprocess, os


def cmd_exists(cmd):
  return subprocess.call("type " + cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE) == 0

try: 
  if cmd_exists("BuildWorkspace"):
    print "BuildWorkspace found..."
except: 
  sys.exit("Error: Executable BuildWorkspace not found. Please compile.")


ROOTCOREBIN = os.getenv("ROOTCOREBIN")
if ROOTCOREBIN == None:
    sys.exit("Error: Please Setup ROOTCORE")
