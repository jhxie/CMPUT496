#!/usr/bin/env python

"""
This script automates the build process of the timestamp program as well as
generates a one way RTT plot based on its output.
To avoid potential incompatibility with different shells (bash ksh tcsh, etc),
python is used rather than usual shell scripts.
"""

from __future__ import print_function

import argparse
import matplotlib.pyplot as plt
import multiprocessing
import os
import subprocess
import sys

from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel

from SingleSwitchTopo import SingleSwitchTopo


def autoGen():
    """
    Automatically generates the binary of the timestamp program.
    """

    buildDirectory = "./build"
    logicalCPUCount = str(multiprocessing.cpu_count() + 1)
    # Note the cmake commands used are not shown in official documentation:
    # '-B' specifies the build directory
    # '-H' specifies the source directory
    cmakeCommands = ["cmake", "-B" + buildDirectory, "-H."]
    makeCommands = ["make", "-j", logicalCPUCount,
                    "-C", buildDirectory, "install"]

    if "posix" != os.name:
        sys.exit("This script is only mean to be used on POSIX systems.")
    # Make a new build directory if it does not exist
    if (False == os.path.exists(buildDirectory)):
        os.mkdir(buildDirectory)
    # Do nothing if build directory exists:
    # cmake can speed up the build process by using existing valid compiled
    # object files
    elif (True == os.path.isdir(buildDirectory)):
        pass
    # There is a file named build otherwise
    elif (False == os.path.isdir(buildDirectory)):
        sys.exit("Name conflicts for the \"" + buildDirectory + "\"!")

    subprocess.call(cmakeCommands)
    subprocess.call(makeCommands)

if __name__ == "__main__":
    tsTestDescription = ""
    setLogLevel("info")
    parser = argparse.ArgumentParser(description=tsTestDescription)
    exclusiveFlagGroup = parser.add_mutually_exclusive_group()
    exclusiveFlagGroup.add_argument("-r",
                                    "--runs",
                                    help="Number of Runs for All 3 Tests",
                                    required=False,
                                    type=int)
    exclusiveFlagGroup.add_argument("-f",
                                    "--file",
                                    help="Exported Pickle Data To Be Plotted",
                                    required=False,
                                    type=str)
    parser.add_argument("-b",
                        "--build",
                        action="store_true",
                        help="Build the Timestamp Executable",
                        required=False)
    parser.add_argument("-o",
                        "--output",
                        help="File Name of Pickle Data To Be Exported" +
                        ", Must Be Used With Either -r or -f But Not Both",
                        required=False,
                        type=str)
    parser.add_argument("-p",
                        "--print",
                        action="store_true",
                        help="Print the Collected Data From All 3 Tests" +
                        ", Must Be Used With Either -r or -f But Not Both",
                        required=False)
    args = parser.parse_args()
    if args.runs:
        #resultList = perfGenResult(testRuns=args.runs)
        #perfPlotResult(resultList)
        pass
    elif args.file:
        #resultList = perfManageData(args.file, "r")
        #perfPlotResult(resultList)
        pass

    if args.build:
        autoGen()

    if (args.output) and (args.runs or args.file):
        #perfManageData(args.output, "w", resultList)
        pass

    if (args.print) and (args.runs or args.file):
        #perfPrintResult(resultList)
        pass
