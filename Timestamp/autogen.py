#!/usr/bin/env python

"""
This script automates the build process of the timestamp program as well as
generates a one way RTT plot based on its output.
To avoid potential incompatibility with different shells (bash ksh tcsh, etc),
python is used rather than usual shell scripts.
"""

from __future__ import print_function

import matplotlib.pyplot as plt
import multiprocessing
import os
import subprocess
import sys


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
    autoGen()
