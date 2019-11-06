#!/bin/env python2
import argparse


def main(args):
    job = TQHistoMakerAnalysisJob()
    with open(args.input,"rt") as file:
        for line in file.readlines():
            if not "TH" in line:
                continue
            job.bookHistogram(line)
    job.printBooking("test")


if __name__ == "__main__":
    parser = argparse.ArgumentParser("test histogram parsing")
    parser.add_argument("--input","-i",required=True,help="input file to test parsing")
    args = parser.parse_args()
    from QFramework import *
    main(args)
