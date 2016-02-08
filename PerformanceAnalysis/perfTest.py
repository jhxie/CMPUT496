#!/usr/bin/env python

"""
This script comprises of 3 tests that use Mininet/Iperf to measure the
performance of a virtual network with 1 switch and 2 hosts attached.
"""

from __future__ import print_function  # use python3 print function instead

import argparse
import matplotlib.pyplot as plt
import math
import os
import pickle

from mininet.net import Mininet
from mininet.link import TCLink
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel

from SingleSwitchTopo import SingleSwitchTopo


# ---------------------------- GLOBAL CONSTANTS -------------------------------
# Imitate enum of python3
FILESIZE = 0
LATENCY = 1
LOSS = 2

REPORTNAMES = {0: "IperfClientFileSizeReport",
               1: "IperfClientLatencyReport",
               2: "IperfClientLossReport"}
# ---------------------------- GLOBAL CONSTANTS -------------------------------


def perfTestFileSize(rangeMin=1, rangeMax=11, useDD=True):
    """
    Create network and run performance test using file size as a parameter
    (i.e. no message loss or latency) with 1 switch and 2 hosts topology.
    The 3 optional parameters stand for the lower and upper bound exponent
    of file size with 2 as the base, along with the option to use the data
    description program to read random data from 'urandom' device, write to
    a file, then pass the file to 'iperf' client.
    CAVEAT
    Please NEVER SET THE OPTION useDD to False! Iperf sometimes "mysteriously"
    drop certain entries in the generated result file and the author has no
    idea what is the underlying cause for it.
    Even though the "dd" program is capable of generating file of specifed
    size, sometimes it would not do so when the file size is large
    (1024MB, for example), the actual file size could vary from 980MB up to
    the specified file size, the cause for it is unknown to the author.
    Example
    perfTestFileSize(1,11) would lead to file size range from 2MB up to 1024MB.
    """
    for argument in (rangeMin, rangeMax):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    os.system("cd /tmp/ && rm -f " + REPORTNAMES[FILESIZE])
    print("!! Performing Network Performance Test Using File Size !!")
    topo = SingleSwitchTopo(n=2)
    net = Mininet(topo, link=TCLink)
    net.start()
    print("!! Dumping host connections !!")
    dumpNodeConnections(net.hosts)
    print("!! Testing network connectivity !!")
    net.pingAll()

    # Host is an alias for Node
    h1, h2 = net.getNodeByName("h1", "h2")
    print("!! Testing bandwidth between h1 ({}) and h2 ({}) !!"
          .format(h1.IP(), h2.IP()))

    # The built-in Mininet.iperf is only suitable for very simple performance
    # tests since it only supports very limited amount of customization
    # We choose to run 'iperf' as a command instead in order to pass
    # custom command line flags to it
    # net.iperf(hosts=(h1, h2), l4Type="TCP")
    h1.cmd("iperf -f m -s > /dev/null 2>&1 &")

    # File to be transferred up to 2 ^ 10
    for mb in range(rangeMin, rangeMax):
        print("File Size -------- [{} MB]".format(str(2 ** mb)))
        # To avoid "accidental" compression by some of the application layer
        # protocols, 'urandom' is used here rather than the 'zero' device
        # to achieve better 'fidelity'.
        # Note that the 'fullblock' argument for iflag parameter is required
        # when dd is invoked using 'urandom' device; otherwise dd may not
        # read enough data to write the output file of proper size
        if False == useDD:
            h2.cmd("iperf -f m -c {} -n {}M  >> /tmp/"
                   .format(h1.IP(), str(2 ** mb)) + REPORTNAMES[FILESIZE])
        else:
            h2.cmd("dd if=/dev/urandom of={}M bs=1M count={} iflag=fullblock"
                   .format(str(2 ** mb), str(2 ** mb)))
            h2.cmd("iperf -f m -c {} -F {}M  >> /tmp/"
                   .format(h1.IP(), str(2 ** mb)) + REPORTNAMES[FILESIZE])
            h2.cmd("rm -f {}M".format(str(2 ** mb)))
            h2.cmd("sync")

    h1.cmd("kill %")
    h2.cmd("kill %")
    net.stop()

    print("!! Final iperf Report on File Size from Client Side !!")
    fileSizeBandwidthList = []
    with open("/tmp/" + REPORTNAMES[FILESIZE], "r") as report:
        for line in report:
            print(line)
            if all(keyword in line for keyword in ("MBytes", "Mbits/sec")):
                measuredBandwidth = float(line.split(" ")[-2])
                fileSizeBandwidthList.append(measuredBandwidth)

    return fileSizeBandwidthList


def perfTestLatency(rangeMin=1, rangeMax=182, rangeStep=20):
    """
    Create network and run performance test using latency as a parameter
    (i.e. no message loss or variation in file size, holding time constant,
    which is set to be 10 seconds by default for iperf version 2) with 1
    switch and 2 hosts topology.
    Here the parameters stands for the latency range in milisecond.
    """
    for argument in (rangeMin, rangeMax, rangeStep):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    os.system("cd /tmp/ && rm -f " + REPORTNAMES[LATENCY])
    print("!! Performing Network Performance Test Using Latency !!")
    # From mininet's documentation as well as in-source comment it is not clear
    # how to set the RTT directly, so delay is used here.
    # In other words, the actual RTT = 2 * delay in a single switch network
    # since ONLY THE FIRST LINK in a single switch network is set to 'delay'.
    # Also, how to change the delay of established link is unknown to the
    # author so network topology is set up and tear down repeatedly
    # which may not be an elegant or efficient way of implementing it
    # The actual sequence is like [1, 21, 41, ... 141, 161, 181] where n = 10
    for delay in range(rangeMin, rangeMax, rangeStep):
        SingleSwitchTopo.setBuildOption("delay", delay)
        topo = SingleSwitchTopo(n=2)
        net = Mininet(topo, link=TCLink)
        net.start()
        print("!! Dumping host connections !!")
        dumpNodeConnections(net.hosts)
        print("!! Testing network connectivity !!")
        net.pingAll()
        h1, h2 = net.getNodeByName("h1", "h2")
        print("!! Testing bandwidth between h1 ({}) and h2 ({}) !!"
              .format(h1.IP(), h2.IP()))
        # since both hosts would be tear down at each iteration
        # we have to be careful and remember to use 'append' shell redirection
        h1.cmd("iperf -f m -s > /dev/null 2>&1 &")
        print("Latency -------- [{} ms]".format(str(delay)))
        h2.cmd("iperf -f m -c {} >> /tmp/".format(h1.IP())
               + REPORTNAMES[LATENCY])
        h2.cmd("sync")
        h2.cmd("kill %")
        h1.cmd("kill %")
        net.stop()

    print("!! Final iperf Report on Latency from Client Side !!")
    latencyBandwidthList = []
    with open("/tmp/" + REPORTNAMES[LATENCY], "r") as report:
        for line in report:
            print(line)
            if all(keyword in line for keyword in ("MBytes", "Mbits/sec")):
                measuredBandwidth = float(line.split(" ")[-2])
                latencyBandwidthList.append(measuredBandwidth)

    return latencyBandwidthList


def perfTestLoss(rangeMin=0, rangeMax=5):
    """
    Create network and run performance test using message loss as a parameter
    (i.e. no variation in file size or latency, fix transfer time period as 10
    seconds, as mentioned previously) with 1 switch and 2 hosts topology.
    The argument stands for the PERCENTAGE for data loss on the FIRST LINK.
    """
    for argument in (rangeMin, rangeMax):
        if not isinstance(argument, int) or 0 > argument:
            raise ValueError("argument must be non-negative integers")

    os.system("cd /tmp/ && rm -f " + REPORTNAMES[LOSS])
    print("!! Performing Network Performance Test Using Message Loss Rate !!")
    # in mininet the loss rate can only be expressed in decimal,
    # no fractions are allowed; i.e. 0.1% data loss rate is not possible
    # here the loss rate range between [1, 10]
    for loss_rate in range(rangeMin, rangeMax):
        SingleSwitchTopo.setBuildOption("loss", loss_rate)
        topo = SingleSwitchTopo(n=2)
        net = Mininet(topo, link=TCLink)
        net.start()
        print("!! Dumping host connections !!")
        dumpNodeConnections(net.hosts)
        print("!! Testing network connectivity !!")
        net.pingAll()
        h1, h2 = net.getNodeByName("h1", "h2")
        print("!! Testing bandwidth between h1 ({}) and h2 ({}) !!"
              .format(h1.IP(), h2.IP()))
        print("Loss Rate -------- [{} %]".format(str(loss_rate)))
        # h2 is chosen as the client and h1 is server
        # based on assumption from the source code of 'mininet/net.py'
        # "
        # client, server = hosts
        # result = [ self._parseIperf( servout ), self._parseIperf( cliout ) ]
        # "
        tmpBandwidthPair = net.iperf(hosts=(h2, h1), fmt="m", seconds=10)
        retryedTimes = 1
        while (0 == len(tmpBandwidthPair)):
            print("!! Failed Iperf TCP Connection Retrying --------"
                  " [ {} time(s)] !!"
                  .format(str(retryedTimes)))
            tmpBandwidthPair = net.iperf(hosts=(h2, h1), fmt="m", seconds=10)
        # write the result measured from client side to the report file
        with open("/tmp/" + REPORTNAMES[LOSS], "a") as report:
            # write "dummy" number of MBytes transferred to work around the
            # parsing method used in 'perfParseResult'
            report.write("DUMMY MBytes " + str(tmpBandwidthPair[1]) + "\n")
        net.stop()

    print("!! Final iperf Report on Loss Rate from Client Side !!")
    lossBandwidthList = []
    with open("/tmp/" + REPORTNAMES[LOSS], "r") as report:
        for line in report:
            print(line)
            if all(keyword in line for keyword in ("MBytes", "Mbits/sec")):
                measuredBandwidth = float(line.split(" ")[-2])
                lossBandwidthList.append(measuredBandwidth)

    return lossBandwidthList


def perfGenResult(testFuncList=[perfTestFileSize,
                                perfTestLatency,
                                perfTestLoss],
                  testRuns=10):
    """
    Clean all the past generated iperf reports and run all tests.
    Remember to execute the script with proper permission to make the 'rm'
    command actually work.
    Then parse all 3 generated report file and return the result bandwidth in 3
    different lists; it assumes all the reports already exist.
    Structure of Returned List
    [
        [
            [sum of bandwidth for each point of testFuncList[0]],
            [average of bandwidth for each point of testFuncList[0]],
            [standard deviation of bandwidth for each point of testFuncList[0]]
        ],
        [
            [sum of bandwidth for each point of testFuncList[1]],
            [average of bandwidth for each point of testFuncList[1]],
            [standard deviation of bandwidth for each point of testFuncList[1]]
        ],
        ...
    ]
    """

    if not testFuncList:
        raise ValueError("testFuncList must not be empty or none")

    if not isinstance(testRuns, int) or 0 > testRuns:
            raise ValueError("testRuns must be non-negative integers")

    resultList = []
    for _ in range(len(testFuncList)):
        resultList.append([])

    # Iterate through all the functions
    for index, testFunc in enumerate(testFuncList):
        # sumList accumulates the sum of values in each record
        # a record is a list of measured bandwidth in one test run
        sumList = testFunc()
        # accuList is a list of records for test runs;
        # there would be n lists within accuList if n = number of test runs
        accuList = []
        accuList.append(sumList)
        # reduce the rest to a single sum for each value in every record
        for _ in range(testRuns - 1):
            tmpList = testFunc()
            accuList.append(tmpList)
            sumList = [x + y for x, y in zip(sumList, tmpList)]

        avgList = [x / testRuns for x in sumList]
        stdDevList = []

        for currentVal in range(len(accuList[0])):
            stdDevSum = 0
            for record in accuList:
                stdDevSum += (record[currentVal] - avgList[currentVal]) ** 2

            stdDevList.append(math.sqrt(
                stdDevSum / (testRuns if 1 == testRuns else testRuns - 1)))

        resultList[index].append(sumList)
        resultList[index].append(avgList)
        resultList[index].append(stdDevList)

    return resultList


def perfPlotResult(perfGenResultList=None):
    """
    Plot the performance graph using the report generated by
    'perfTestFileSize' 'perfTestLatency' 'perfTestLoss'.
    The argument "perfGenResultList" must be valid.
    """
    if not perfGenResultList:
        raise ValueError("perfGenResultList must not be empty or none")

    fig, axisArray = plt.subplots(3, sharey=True)
    fig.subplots_adjust(hspace=.9)

    _, fileSizeAvg, fileSizeStdDev = perfGenResultList[0]
    axisArray[0].errorbar([2 ** i for i in range(1, 11)],
                          fileSizeAvg,
                          yerr=fileSizeStdDev,
                          marker="o")
    axisArray[0].axis([0, 2 ** 10, 0, 2 ** 10])
    axisArray[0].set_title("Performance Analysis Using Iperf/Mininet")
    axisArray[0].set_xlabel("File Size (MB)")
    axisArray[0].set_ylabel("Bandwidth (Mbps)")
    axisArray[0].grid(True)

    _, latencyAvg, latencyStdDev = perfGenResultList[1]
    axisArray[1].errorbar([i for i in range(1, 182, 20)],
                          latencyAvg,
                          yerr=latencyStdDev,
                          marker="s")
    axisArray[1].axis([0, 162, 0, 2 ** 10])
    axisArray[1].set_xlabel("Latency (Miliseconds)")
    axisArray[1].set_ylabel("Bandwidth (Mbps)")
    axisArray[1].grid(True)

    _, lossAvg, lossStdDev = perfGenResultList[2]
    axisArray[2].errorbar([i for i in range(0, 5)],
                          lossAvg,
                          yerr=lossStdDev,
                          marker="^")
    axisArray[2].axis([0, 11, 0, 2 ** 10])
    axisArray[2].set_xlabel("Loss Rate (%)")
    axisArray[2].set_ylabel("Bandwidth (Mbps)")
    axisArray[2].grid(True)

    plt.show()
    fig.savefig("bandwidthSummary.png")

    # Reference for saving subplots
    # http://stackoverflow.com/questions/4325733/save-a-subplot-in-matplotlib
    for i in range(3):
        extent = axisArray[i].get_window_extent().transformed(
            fig.dpi_scale_trans.inverted())
        subPlotName = ""
        if FILESIZE == i:
            subPlotName = "fileSize"
            axisArray[i].set_title("")
        elif LATENCY == i:
            subPlotName = "latency"
        elif LOSS == i:
            subPlotName = "loss"
        fig.savefig(subPlotName + "Plot.png",
                    bbox_inches=extent.expanded(1.3, 1.8))


def perfManageData(dataFileName, mode, exportList=None):
    """
    Save or load the data generated by 'perfGenResult' function.
    Mode can be specified as either "r", which would let the function return
    the list loaded from the file named "dataFileName";
    or it can be "w", which would write the non-empty data specified by
    "exportList" to the file named "dataFileName".
    """
    if not isinstance(mode, str) or mode.lower() not in ("r", "w"):
        raise ValueError("mode must be string value of either r or w")
    if "w" == mode.lower() and not exportList:
        raise ValueError("exportList must not be empty when in write mode")

    importedList = []
    with open(dataFileName, mode + "b") as dataFile:
        if "r" == mode.lower():
            importedList = pickle.load(dataFile)
        elif "w" == mode.lower():
            pickle.dump(exportList, dataFile)

    if "r" == mode.lower():
        return importedList


def perfPrintResult(resultList=None):
    """
    Print the formatted result stored in the given parameter "resultList".
    """
    if not resultList:
        raise ValueError("resultList must not be empty or none")

    for testNumber in range(len(resultList)):
        print("!! Data Collected from the ", end="")
        if FILESIZE == testNumber:
            print("File Size Test !!")
        elif LATENCY == testNumber:
            print("Latency Test !!")
        elif LOSS == testNumber:
            print("Loss Rate Test !!")

        for recordIndex in range(len(resultList[0])):
            if 0 == recordIndex:
                print("!! Sum !!")
            elif 1 == recordIndex:
                print("!! Average !!")
            elif 2 == recordIndex:
                print("!! Standard Deviation !!")
            print(resultList[testNumber][recordIndex])


if __name__ == "__main__":
    perfTestDescription = "Perform Network Performance Analysis using " +\
                          "a Simple Topology with 1 Switch and 2 Hosts " +\
                          "under 3 metrics: File Size, Latency, and Loss Rate"
    # Tell mininet to print useful information
    setLogLevel("info")

    parser = argparse.ArgumentParser(description=perfTestDescription)
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
    resultList = []

    if args.runs:
        resultList = perfGenResult(testRuns=args.runs)
        perfPlotResult(resultList)
    elif args.file:
        resultList = perfManageData(args.file, "r")
        perfPlotResult(resultList)

    if (args.output) and (args.runs or args.file):
        perfManageData(args.output, "w", resultList)

    if (args.print) and (args.runs or args.file):
        perfPrintResult(resultList)
