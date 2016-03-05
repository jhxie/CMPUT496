#!/usr/bin/env python

"""
This script automates the build process of the timestamp program as well as
generates three plots based on padding message size, loss rate, and RTT.
To avoid potential incompatibility with different shells (bash ksh tcsh, etc),
python is used rather than usual shell scripts.
Happy lazy sysadmins.
"""

from __future__ import print_function

import argparse
import getpass
import matplotlib.pyplot as plt
import multiprocessing
import os
import platform
import subprocess
import sys

from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel

from SingleSwitchTopo import SingleSwitchTopo


# ---------------------------- GLOBAL CONSTANTS -------------------------------
ENVNAME = 0
REPORTNAME = 1

TIMESTAMP_ATTRS = {0: "TIMESTAMP_OUTPUT",
                   1: "tsLog.csv"}

SSH_CMD = 0
SSHD_PATH = 1
SSHPASS_CMD = 2
# Strictly speaking, the password is not a 'constant', but it is only set once
# throughout the lifetime of this script.
SSH_PASSWD = 3

SSH_ATTRS = {0: "ssh -oStrictHostKeyChecking=no mininet@",
             1: "/usr/sbin/sshd",
             2: "sshpass -p ",
             3: str()}

# ---------------------------- GLOBAL CONSTANTS -------------------------------


def tsTestPadMsgSize(padMsgSize, numOfRuns, msgSent):
    """
    """
    for argument in (padMsgSize, numOfRuns, msgSent):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    os.system("cd /tmp/ && rm -f " + TIMESTAMP_ATTRS[REPORTNAME])
    # Environment variables set in the host will not be seen in the virtual
    # hosts set up by mininet, left as it is just in case
    os.system("export " + TIMESTAMP_ATTRS[ENVNAME] + "=" +
              TIMESTAMP_ATTRS[REPORTNAME])

    print("!! Performing TimeStamp Test Using Padding Message Size !!")
    topo = SingleSwitchTopo(n=2)
    net = Mininet(topo, link=TCLink)
    net.start()
    print("!! Dumping host connections !!")
    dumpNodeConnections(net.hosts)
    print("!! Testing network connectivity !!")
    net.pingAll()

    h1, h2 = net.getNodeByName("h1", "h2")
    h1.cmd(SSH_ATTRS[SSHD_PATH])
    h2.cmd(SSH_ATTRS[SSHD_PATH])
    print("!! Testing Normalized Arrival Time Between h1 ({0}) and h2 ({1}) !!"
          .format(h1.IP(), h2.IP()))

    # From section 7.1.3 'Format String Syntax' of the official python doc:
    # https://docs.python.org/2/library/string.html
    h1.cmd("ts -s -b {0} -c {1} | {2} '{3}' {4}{5} ts -r -b {0} -c {1}"
           .format(padMsgSize, msgSent,
                   SSH_ATTRS[SSHPASS_CMD], SSH_ATTRS[SSH_PASSWD],
                   SSH_ATTRS[SSH_CMD], h2.IP()))
    #for _ in range(numOfRuns):
    #    for _ in range(msgSent):
    #        pass
    net.stop()

def tsTestLoss(lossRate, numOfRuns, msgSent):
    """
    """
    for argument in (lossRate, numOfRuns, msgSent):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    os.system("cd /tmp/ && rm -f " + TIMESTAMP_ATTRS[REPORTNAME])
    os.system("export " + TIMESTAMP_ATTRS[ENVNAME] + "=" +\
              TIMESTAMP_ATTRS[REPORTNAME])

    print("!! Performing TimeStamp Test Using Loss Rate !!")


def tsTestRTT(RTT, numOfRuns, msgSent):
    """
    """
    for argument in (RTT, numOfRuns, msgSent):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    os.system("cd /tmp/ && rm -f " + TIMESTAMP_ATTRS[REPORTNAME])
    os.system("export " + TIMESTAMP_ATTRS[ENVNAME] + "=" +\
              TIMESTAMP_ATTRS[REPORTNAME])

    print("!! Performing TimeStamp Test Using RTT !!")


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

    # Define extra macros for cmake to find compilers that support c++11
    # if inside the mininet ubuntu 14.04 LTS VM
    if "mininet-vm" == platform.node():
        cmakeCommands.append("-DCMAKE_C_COMPILER=/usr/bin/gcc-5")
        cmakeCommands.append("-DCMAKE_CXX_COMPILER=/usr/bin/g++-5")

    # Make a new build directory if it does not exist
    if (False == os.path.exists(buildDirectory)):
        os.mkdir(buildDirectory)
    elif (True == os.path.isdir(buildDirectory)):
        os.system("rm -rf " + buildDirectory + "/*")
    # There is a file named build otherwise
    elif (False == os.path.isdir(buildDirectory)):
        sys.exit("Name conflicts for the \"" + buildDirectory + "\"!")

    subprocess.call(cmakeCommands)
    subprocess.call(makeCommands)

if __name__ == "__main__":
    tsTestDescription = ""
    passPrompt = "Password used for SSH among mininet virtual hosts: "
    setLogLevel("info")
    parser = argparse.ArgumentParser(description=tsTestDescription)
    parser.add_argument("-f",
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
    if args.file:
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

    SSH_ATTRS[SSH_PASSWD] = getpass.getpass(passPrompt)
    tsTestPadMsgSize(0, 0, 10)
