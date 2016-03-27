#!/usr/bin/env python

"""
Module for various utility functions; for now it only has autoGen(), which
builds the executable for the Timestamp (ts) program.
"""

import multiprocessing
import os
import platform
import subprocess
import sys


def autoGen(buildDirectory="./build", sourceDirectory="."):
    """
    Automatically generates the binary of the timestamp program using a
    combination of cmake and make commands.
    Note when used in the mininet ubuntu 14.04 LTS VM, gcc has to be updated
    to the newest version according to the instructions given in the top level
    README.md file, otherwise this function would not work properly.
    """

    logicalCPUCount = str(multiprocessing.cpu_count() + 1)
    # Note the cmake commands used are not shown in official documentation:
    # '-B' specifies the build directory
    # '-H' specifies the source directory
    cmakeCommands = ["cmake", "-B" + buildDirectory, "-H" + sourceDirectory]
    makeCommands = ["make", "-j", logicalCPUCount, "-C", buildDirectory]

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
