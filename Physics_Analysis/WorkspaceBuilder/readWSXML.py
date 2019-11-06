#!/usr/bin/env python2

import argparse
import os
from xml.dom import minidom


def readItem(item, filepath):

    xmldoc = minidom.parse(filepath)
    itemlist = xmldoc.getElementsByTagName(item)
    for s in itemlist:
        print(s.childNodes[0].nodeValue)

def main(args):
    
    readItem(args.item, args.path)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Read WSBuilder xml files')
    parser.add_argument('--path', '-p', type=str, default="WorkspaceBuilder/WorkspaceBuilder/data/lfv_lephad/LFVlephad_Systematics.xml", help='path of the xml file')
    parser.add_argument('--item', '-i', type=str, default="SystematicNameInFile", help='name of the item')
    parser.add_argument('--attribute', '-a', type=str, default="InputChannel", help='name of the item attribute')
    args = parser.parse_args()
    main(args)
